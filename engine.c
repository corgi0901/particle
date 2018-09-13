#include <stdio.h>
#include <memory.h>
#include <string.h>
#include "engine.h"
#include "lexer.h"
#include "map.h"
#include "util.h"
#include "stack.h"
#include "programMemory.h"
#include "memoryMap.h"
#include "debug.h"

#define EQ(op, val) (strcmp(op, val) == 0)

/**
 * 実行エンジンの状態
 */
typedef enum
{
	/// 実行状態
	ESTATE_RUN = 0,
	/// 関数入力状態
	ESTATE_FUNC,
	/// 実行スキップ状態
	ESTATE_SKIP,
	/// 実行終了状態
	ESTATE_END
} ENGINE_STATE;

typedef enum
{
	BLOCK_FUNC,
	BLOCK_IF,
	BLOCK_WHILE,
} BLOCK_TYPE;

static ProgramMemory pmem;
static VarMapStack vms;
static Stack stack = {NULL};
static Stack return_stack = {NULL};
static Stack state_stack = {NULL};
static Stack block_stack = {NULL};
static int return_value = 0;
static int return_flag = 0;
static ENGINE_STATE state = ESTATE_RUN;

typedef int (*OPERATOR_FUNC)(Ast *);

typedef struct
{
	char *operator;
	OPERATOR_FUNC func;
} Operator_func_def;

static int eval(Ast *);
static int runFunction(Function *);

static int plus(Ast *);
static int minus(Ast *);
static int times(Ast *);
static int div(Ast *);
static int surplus(Ast *);
static int substitute(Ast *);
static int less(Ast *);
static int more(Ast *);
static int plusEq(Ast *);
static int minusEq(Ast *);
static int timesEq(Ast *);
static int divEq(Ast *);
static int surplusEQ(Ast *);
static int lessEq(Ast *);
static int moreEq(Ast *);
static int equal(Ast *);
static int notEq(Ast *);
static int comma(Ast *);

/**
 * 演算子とそれに対応した実処理関数のテーブル
 */
static Operator_func_def OPERATOR_FUNC_TBL[] = {
	{"=", substitute},

	{"+", plus},
	{"-", minus},
	{"*", times},
	{"/", div},
	{"%", surplus},

	{"<", less},
	{">", more},
	{"<=", lessEq},
	{">=", moreEq},
	{"==", equal},
	{"!=", notEq},

	{"+=", plusEq},
	{"-=", minusEq},
	{"*=", timesEq},
	{"/=", divEq},
	{"%=", surplusEQ},

	{",", comma},
};

static int plus(Ast *node)
{
	return eval(node->left) + eval(node->right);
};

static int minus(Ast *node)
{
	return eval(node->left) - eval(node->right);
};

static int times(Ast *node)
{
	return eval(node->left) * eval(node->right);
};

static int div(Ast *node)
{
	return eval(node->left) / eval(node->right);
};

static int surplus(Ast *node)
{
	return eval(node->left) % eval(node->right);
};

static int substitute(Ast *node)
{
	int value = eval(node->right);
	Variable *var = getOrCreateVariable(&vms, node->left->root->value.name);
	var->value = value;
	return value;
};

static int less(Ast *node)
{
	return eval(node->left) < eval(node->right);
};

static int more(Ast *node)
{
	return eval(node->left) > eval(node->right);
};

static int plusEq(Ast *node)
{
	int value = eval(node->right);
	Variable *var = getOrCreateVariable(&vms, node->left->root->value.name);
	var->value += value;
	return var->value;
};

static int minusEq(Ast *node)
{
	int value = eval(node->right);
	Variable *var = getOrCreateVariable(&vms, node->left->root->value.name);
	var->value -= value;
	return var->value;
};

static int timesEq(Ast *node)
{
	int value = eval(node->right);
	Variable *var = getOrCreateVariable(&vms, node->left->root->value.name);
	var->value *= value;
	return var->value;
};

static int divEq(Ast *node)
{
	int value = eval(node->right);
	Variable *var = getOrCreateVariable(&vms, node->left->root->value.name);
	var->value /= value;
	return var->value;
};

static int surplusEQ(Ast *node)
{
	int value = eval(node->right);
	Variable *var = getOrCreateVariable(&vms, node->left->root->value.name);
	var->value %= value;
	return var->value;
};

static int lessEq(Ast *node)
{
	return eval(node->left) <= eval(node->right);
};

static int moreEq(Ast *node)
{
	return eval(node->left) >= eval(node->right);
};

static int equal(Ast *node)
{
	return eval(node->left) == eval(node->right);
};

static int notEq(Ast *node)
{
	return eval(node->left) != eval(node->right);
};

static int comma(Ast *node)
{
	eval(node->left);
	eval(node->right);
	return 0;
};

/**
 * @brief 指定した演算子に対応する実処理関数を取得する
 * @param operator 演算子
 * @retval NULL 該当する演算子がない
 * @retval Other 実処理関数
 */
static OPERATOR_FUNC getEngineFunc(char *operator)
{
	OPERATOR_FUNC func = NULL;
	int num = sizeof(OPERATOR_FUNC_TBL) / sizeof(Operator_func_def);

	for (int i = 0; i < num; i++)
	{
		if (EQ(operator, OPERATOR_FUNC_TBL[i].operator))
		{
			func = OPERATOR_FUNC_TBL[i].func;
			break;
		}
	}
	return func;
};

/**
 * @brief 関数の引数をパースし、変数マップに格納する
 * @param func 関数オブジェクト
 * @param map 変数マップ
 * @param args 引数のトークン列
 */
static void parseArgs(Function *func, VariableMap *map, Ast *ast)
{
	Arg *arg = func->args;
	Ast *node = ast;

	while (arg)
	{
		int value;

		if (node->root->type == TK_OPERATION && EQ(node->root->value.op, ","))
		{
			value = eval(node->right);
			node = node->left;
		}
		else
		{
			value = eval(node);
		}

		addVariable(map, arg->name, value);

		arg = arg->next;
	}
};

/**
 * @brief 実行状態のときの評価処理
 * @param node 抽象構文木
 * @return 評価値
 */
static int evalRun(Ast *node)
{
	int value = 0;

	if (node == NULL)
	{
		return value;
	}

	switch (node->root->type)
	{
	case TK_VARIABLE:
	{
		Function *func = getFunction(node->root->value.name);
		if (func)
		{
			// メモリ空間の作成と引数の評価値の保存
			VariableMap *var_map = createVariableMap();
			parseArgs(func, var_map, node->left);
			pushVariableMap(&vms, var_map);

			// 関数の実行
			value = runFunction(func);

			// メモリ空間の破棄
			VariableMap *map = popVariableMap(&vms);
			releaseVariableMap(map);
		}
		else
		{
			// 変数の評価
			Variable *var = getOrCreateVariable(&vms, node->root->value.name);
			value = var->value;
		}
		break;
	}
	case TK_NUMBER:
	{
		value = node->root->value.value;
		break;
	}
	case TK_OPERATION:
	{
		OPERATOR_FUNC func = getEngineFunc(node->root->value.op);
		if (func)
		{
			value = func(node);
		}
		break;
	}
	case TK_UNARY_OP:
	{
		if (EQ("+", node->root->value.op))
		{
			value = eval(node->left);
		}
		else if (EQ("-", node->root->value.op))
		{
			value = -eval(node->left);
		}
		else if (EQ("!", node->root->value.op))
		{
			value = !eval(node->left);
		}
		break;
	}
	case TK_FUNCTION:
	{
		if (EQ(node->root->value.func, "print"))
		{
			printf("%d\n", eval(node->left));
		}
		else if (EQ(node->root->value.func, "exit"))
		{
			state = ESTATE_END;
		}
		break;
	}
	case TK_KEYWORD:
	{
		if (EQ(node->root->value.keyword, "func"))
		{
			state = ESTATE_FUNC;
			push(&block_stack, BLOCK_FUNC);

			// 関数定義の追加
			Function *func = createFunction(node->left->root->value.name, getpc(&pmem));
			addFunction(func);

			// 引数定義の評価
			Ast *arg = node->left->left;
			while (arg)
			{
				if (TK_OPERATION == arg->root->type && EQ(arg->root->value.op, ","))
				{
					addArg(arg->right->root->value.name);
					arg = arg->left;
				}
				else
				{
					addArg(arg->root->value.name);
					arg = NULL;
				}
			}
		}
		else if (EQ(node->root->value.keyword, "return"))
		{
			return_value = eval(node->left);
			return_flag = 1;
			jump(&pmem, pop(&return_stack));

			while (getpc(&pmem) != pop(&stack))
			{
				// Do nothing
			};

			while (BLOCK_FUNC != pop(&block_stack))
			{
				// Do nothing
			};
		}
		else if (EQ(node->root->value.keyword, "if"))
		{
			push(&state_stack, state);
			push(&block_stack, BLOCK_IF);

			if (0 == eval(node->left))
			{
				state = ESTATE_SKIP;
			}
		}
		else if (EQ(node->root->value.keyword, "else"))
		{
			state = ESTATE_SKIP;
		}
		else if (EQ(node->root->value.keyword, "while"))
		{
			push(&block_stack, BLOCK_WHILE);
			push(&state_stack, state);

			if (eval(node->left))
			{
				push(&stack, getpc(&pmem) - 1);
			}
			else
			{
				push(&stack, -1);
				state = ESTATE_SKIP;
			}
		}
		else if (EQ(node->root->value.keyword, "end"))
		{
			state = pop(&state_stack);
			BLOCK_TYPE block = pop(&block_stack);

			// 関数またはwhile節に対応するendならプログラムカウンタを飛ばす
			if (BLOCK_FUNC == block || BLOCK_WHILE == block)
			{
				int next_pc = pop(&stack);
				if (next_pc >= 0)
				{
					jump(&pmem, next_pc);
				}
			}
		}
		break;
	}
	default:
		break;
	}

	return value;
};

/**
 * @brief 関数定義状態のときの評価処理
 * @param node 抽象構文木
 * @return 評価値
 */
static int evalFunc(Ast *node)
{
	if (node == NULL || TK_KEYWORD != node->root->type)
	{
		return 0;
	}

	if (isStrMatch(node->root->value.keyword, "if"))
	{
		push(&block_stack, BLOCK_IF);
	}
	else if (isStrMatch(node->root->value.keyword, "while"))
	{
		push(&block_stack, BLOCK_WHILE);
	}
	else if (isStrMatch(node->root->value.keyword, "end"))
	{
		if (BLOCK_FUNC == pop(&block_stack))
		{
			state = ESTATE_RUN;
		}
	}

	return 0;
}

/**
 * @brief 実行スキップ状態のときの評価処理
 * @param node 抽象構文木
 * @return 評価値
 */
static int evalSkip(Ast *node)
{
	if (node == NULL || TK_KEYWORD != node->root->type)
	{
		return 0;
	}

	if (isStrMatch(node->root->value.keyword, "else"))
	{
		state = ESTATE_RUN;
	}
	else if (isStrMatch(node->root->value.keyword, "end"))
	{
		state = pop(&state_stack);

		// while節に対応するendならプログラムカウンタを飛ばす
		if (BLOCK_WHILE == pop(&block_stack))
		{
			int next_pc = pop(&stack);
			if (next_pc >= 0)
			{
				jump(&pmem, next_pc);
			}
		}
	}

	return 0;
};

/**
 * @brief 入力された抽象構文木を評価する
 * @param node 抽象構文木
 * @return 評価値
 */
static int eval(Ast *node)
{
	int value = 0;

	if (node == NULL)
	{
		return value;
	}

	switch (state)
	{
	case ESTATE_RUN:
		value = evalRun(node);
		break;
	case ESTATE_FUNC:
		value = evalFunc(node);
		break;
	case ESTATE_SKIP:
		value = evalSkip(node);
		break;
	default:
		break;
	}

	return value;
};

/**
 * @brief サブルーチンを実行する
 * @param func サブルーチンオブジェクト
 * @return 戻り値
 */
static int runFunction(Function *func)
{
	char *code;

	push(&return_stack, getpc(&pmem));
	push(&stack, getpc(&pmem));
	push(&block_stack, BLOCK_FUNC);
	push(&state_stack, state);

	return_flag = 0;

	// 関数にジャンプ
	jump(&pmem, func->start_pc);

	while (code = fetch(&pmem))
	{
		Token *tokens = tokenize(code);
		if (tokens)
		{
			Ast *ast = createAst(tokens);
			eval(ast);
			releaseAst(ast);
		}

		if (return_flag)
		{
			break;
		}
	}

	return_flag = 0;

	return return_value;
};

/**
 * @brief エンジン部の初期化
 */
void engineInit(void)
{
	mapInit();
	programMemoryInit(&pmem);
	initVarMapStack(&vms);
	state = ESTATE_RUN;
};

/**
 * @brief エンジン部の終了処理
 */
void engineRelease(void)
{
	mapRelease();
	programMemoryRelease(&pmem);
	releaseVarMapStack(&vms);
};

/**
 * @brief コードの実行
 * @param stream 実行コード
 * @return 結果コード
 */
RESULT engineRun(char *stream)
{
	char *code;
	int ret = RESULT_CONTINUE;

	// 空行またはコメント行ならスキップ
	if (0 == strlen(stream) || '#' == *stream)
	{
		return ret;
	}

	// コードをメモリに保存
	store(&pmem, stream);

	// コード実行
	while (code = fetch(&pmem))
	{
		Token *tokens = tokenize(code);
		if (tokens)
		{
			Ast *ast = createAst(tokens);
			eval(ast);
			releaseAst(ast);
		}

		if (ESTATE_END == state)
		{
			ret = RESULT_EXIT;
			break;
		}
	}

	return ret;
};
