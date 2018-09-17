#include <stdio.h>
#include <memory.h>
#include <string.h>
#include "engine.h"
#include "lexer.h"
#include "ast.h"
#include "function.h"
#include "util.h"
#include "stack.h"
#include "programMemory.h"
#include "memoryMap.h"

/**
 * 実行エンジンの状態
 */
typedef enum
{
	/// 実行状態
	ESTATE_RUN = 0,
	/// 関数入力状態
	ESTATE_FUNC_DEF,
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

static VarMapStack vms;
static Stack stack = {NULL};
static Stack return_stack = {NULL};
static Stack state_stack = {NULL};
static Stack block_stack = {NULL};
static int return_value = 0;
static int return_flag = 0;
static ENGINE_STATE state = ESTATE_RUN;

static int eval(Ast *);
static int runFunction(Function *);

typedef int (*OPERATOR_FUNC)(Ast *);
typedef struct
{
	char *operator;
	OPERATOR_FUNC func;
} OperatorFuncTable;

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
static OperatorFuncTable OPERATOR_FUNC_TBL[] = {
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
	int num = sizeof(OPERATOR_FUNC_TBL) / sizeof(OPERATOR_FUNC_TBL[0]);

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

static int unary_plus(Ast *);
static int unary_minus(Ast *);
static int unary_not(Ast *);

static OperatorFuncTable UNARY_OPERATOR_FUNC_TBL[] = {
	{"+", unary_plus},
	{"-", unary_minus},
	{"!", unary_not},
};

static int unary_plus(Ast *node)
{
	return eval(node->left);
};

static int unary_minus(Ast *node)
{
	return -eval(node->left);
};

static int unary_not(Ast *node)
{
	return !eval(node->left);
};

static OPERATOR_FUNC getEngineUnaryFunc(char *operator)
{
	OPERATOR_FUNC func = NULL;
	int num = sizeof(UNARY_OPERATOR_FUNC_TBL) / sizeof(UNARY_OPERATOR_FUNC_TBL[0]);

	for (int i = 0; i < num; i++)
	{
		if (EQ(operator, UNARY_OPERATOR_FUNC_TBL[i].operator))
		{
			func = UNARY_OPERATOR_FUNC_TBL[i].func;
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
	ArgList *arg = func->args;
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
		Variable *var = getOrCreateVariable(&vms, node->root->value.name);
		value = var->value;
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
		OPERATOR_FUNC func = getEngineUnaryFunc(node->root->value.op);
		if (func)
		{
			value = func(node);
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
		else
		{
			Function *func = getFunction(node->root->value.name);

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
		break;
	}
	case TK_KEYWORD:
	{
		if (EQ(node->root->value.keyword, "func"))
		{
			state = ESTATE_FUNC_DEF;
			push(&block_stack, BLOCK_FUNC);

			// 関数定義の追加
			Function *func = createFunction(node->left->root->value.name, getpc());
			addFunction(func);

			// 引数定義の評価
			Ast *arg = node->left->left;
			while (arg)
			{
				if (TK_OPERATION == arg->root->type && EQ(arg->root->value.op, ","))
				{
					addArgument(func, arg->right->root->value.name);
					arg = arg->left;
				}
				else
				{
					addArgument(func, arg->root->value.name);
					arg = NULL;
				}
			}
		}
		else if (EQ(node->root->value.keyword, "return"))
		{
			return_value = eval(node->left);
			return_flag = 1;
			jump(pop(&return_stack));

			while (getpc() != pop(&stack))
			{
				// Do nothing
			};

			while (BLOCK_FUNC != pop(&block_stack))
			{
				pop(&state_stack);
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
				push(&stack, getpc() - 1);
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
					jump(next_pc);
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

	if (isStrMatch(node->root->value.keyword, "if"))
	{
		push(&state_stack, state);
		push(&block_stack, BLOCK_IF);
	}
	else if (isStrMatch(node->root->value.keyword, "while"))
	{
		push(&state_stack, state);
		push(&block_stack, BLOCK_WHILE);
	}
	else if (isStrMatch(node->root->value.keyword, "else"))
	{
		if (ESTATE_RUN == peek(&state_stack))
		{
			state = ESTATE_RUN;
		}
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
				jump(next_pc);
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
	case ESTATE_FUNC_DEF:
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

	push(&return_stack, getpc());
	push(&stack, getpc());
	push(&block_stack, BLOCK_FUNC);

	return_flag = 0;

	// 関数にジャンプ
	jump(func->start_pc);

	while (code = fetch())
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
void initEngine(void)
{
	initProgramMemory();
	initVarMapStack(&vms);
	initFuncList();
	state = ESTATE_RUN;
};

/**
 * @brief エンジン部の終了処理
 */
void releaseEngine(void)
{
	releaseProgramMemory();
	releaseVarMapStack(&vms);
	releaseFuncList();
};

/**
 * @brief コードの実行
 * @param stream 実行コード
 * @return 結果コード
 */
ENGINE_RESULT runEngine(char *stream)
{
	char *code;
	int ret = RESULT_OK;

	// 空行またはコメント行ならスキップ
	if (0 == strlen(stream) || '#' == *stream)
	{
		return ret;
	}

	// コードをメモリに保存
	store(stream);

	// コード実行
	while (code = fetch())
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
