#include <stdio.h>
#include <memory.h>
#include <string.h>
#include "engine.h"
#include "lexer.h"
#include "map.h"
#include "util.h"
#include "stack.h"
#include "pool.h"
#include "debug.h"

#define EQ(op, val) (strcmp(op, val) == 0)

static CodePool pool = {NULL, NULL, 0};
static int pc = 0;
static Stack stack = {NULL};
static Stack return_stack = {NULL};
static Stack state_stack = {NULL};
static Stack block_stack = {NULL};
static VarMapStack var_map_stack = {NULL};

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

typedef int (*OPERATOR_FUNC)(Ast *);

typedef struct
{
	char *operator;
	OPERATOR_FUNC func;
} Operator_func_def;

static int eval(Ast *);
static int runFunction(Function *);
static Var *getOrCreateVar(char *);

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

static VarMap *local_var_map = NULL;
static int return_value = 0;
static int return_flag = 0;
static ENGINE_STATE state = ESTATE_RUN;

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
	Var *var = getOrCreateVar(node->left->root->value.name);
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
	Var *var = getOrCreateVar(node->left->root->value.name);
	var->value += value;
	return var->value;
};

static int minusEq(Ast *node)
{
	int value = eval(node->right);
	Var *var = getOrCreateVar(node->left->root->value.name);
	var->value -= value;
	return var->value;
};

static int timesEq(Ast *node)
{
	int value = eval(node->right);
	Var *var = getOrCreateVar(node->left->root->value.name);
	var->value *= value;
	return var->value;
};

static int divEq(Ast *node)
{
	int value = eval(node->right);
	Var *var = getOrCreateVar(node->left->root->value.name);
	var->value /= value;
	return var->value;
};

static int surplusEQ(Ast *node)
{
	int value = eval(node->right);
	Var *var = getOrCreateVar(node->left->root->value.name);
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
static void parseArgs(Function *func, VarMap *map, Ast *args)
{
	Arg *arg = func->args;
	Ast *node = args;

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

		// ローカル変数として値を保持
		DPRINTF("arg : %s, value = %d\n", arg->name, value);
		Var *var = createVar(arg->name, value);
		addVar(map, var);

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
			// ローカル変数の退避(push)
			pushVarMap(&var_map_stack, local_var_map);

			// 引数の評価
			VarMap *var_map = createVarMap();
			parseArgs(func, var_map, node->left);
			local_var_map = var_map;

			// 関数本体の実行
			value = runFunction(func);

			// ローカル変数の復帰
			local_var_map = popVarMap(&var_map_stack);
			clearMap(&var_map);
		}
		else
		{
			// 変数の評価
			Var *var = getOrCreateVar(node->root->value.name);
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
			Function *func = createFunction(node->left->root->value.name, pc);
			addFunction(func);

			// 引数定義の評価
			Ast *arg = node->left->left;
			while (arg)
			{
				if (arg->root->type == TK_OPERATION && EQ(arg->root->value.op, ","))
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

			int ret_addr = pop(&return_stack);
			int next_pc = pop(&stack);
			while (ret_addr != next_pc)
			{
				next_pc = pop(&stack);
			}
			pc = ret_addr;
			return_flag = 1;

			while (pop(&block_stack) != BLOCK_FUNC)
			{
				// Do nothing
			};
		}
		else if (EQ(node->root->value.keyword, "if"))
		{
			push(&state_stack, state);
			push(&block_stack, BLOCK_IF);

			if (eval(node->left) == 0)
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

			if (eval(node->left) == 1)
			{
				push(&stack, pc - 1);
				push(&state_stack, state);
			}
			else
			{
				push(&stack, -1);
				push(&state_stack, state);
				state = ESTATE_SKIP;
			}
		}
		else if (EQ(node->root->value.keyword, "end"))
		{
			state = pop(&state_stack);
			BLOCK_TYPE block = pop(&block_stack);

			// 関数またはwhile節に対応するendならプログラムカウンタを飛ばす
			if (block == BLOCK_FUNC || block == BLOCK_WHILE)
			{
				int next_pc = pop(&stack);
				if (next_pc >= 0)
				{
					pc = next_pc;
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
	int value = 0;

	if (node == NULL)
	{
		return value;
	}

	switch (node->root->type)
	{
	case TK_KEYWORD:
	{
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
			if (pop(&block_stack) == BLOCK_FUNC)
			{
				state = ESTATE_RUN;
			}
		}
		break;
	}
	default:
		break;
	}

	return value;
}

/**
 * @brief 実行スキップ状態のときの評価処理
 * @param node 抽象構文木
 * @return 評価値
 */
static int evalSkip(Ast *node)
{
	int value = 0;

	if (node == NULL)
	{
		return value;
	}

	switch (node->root->type)
	{
	case TK_KEYWORD:
	{
		if (isStrMatch(node->root->value.keyword, "else"))
		{
			state = ESTATE_RUN;
		}
		else if (isStrMatch(node->root->value.keyword, "end"))
		{
			state = pop(&state_stack);
			BLOCK_TYPE block = pop(&block_stack);

			// 関数またはwhile節に対応するendならプログラムカウンタを飛ばす
			if (block == BLOCK_WHILE)
			{
				int next_pc = pop(&stack);
				if (next_pc >= 0)
				{
					pc = next_pc;
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
	push(&return_stack, pc);
	push(&stack, pc);
	push(&block_stack, BLOCK_FUNC);

	pc = func->start_pc + 1;

	return_flag = 0;

	while (pc < pool.size)
	{
		// プログラムカウンタの指すコード行を取得
		char *code = getCode(&pool, pc);

		Token *tokens = tokenize(code);
		if (tokens)
		{
			Ast *ast = createAst(tokens);
			eval(ast);
			releaseAst(ast);
		}

		if (return_flag == 1)
		{
			break;
		}

		pc++;
	}

	return_flag = 0;

	return return_value;
};

/**
 * @brief 変数オブジェクトを取得する（なければ生成して変数マップに追加）
 * @param name 変数名
 * @retval NULL エラー
 * @retval Other 変数オブジェクトのポインタ
 */
static Var *getOrCreateVar(char *name)
{
	Var *var = getVar(local_var_map, name);

	if (!var)
	{
		var = createVar(name, 0);
		if (var)
		{
			addVar(local_var_map, var);
		}
	}
	return var;
};

/**
 * @brief エンジン部の初期化
 */
void engineInit(void)
{
	mapInit();
	local_var_map = createVarMap();
	state = ESTATE_RUN;
};

/**
 * @brief エンジン部の終了処理
 */
void engineRelease(void)
{
	mapRelease();
	clearMap(&local_var_map);
	releasePool(&pool);
};

/**
 * @brief コードの実行
 * @param stream 実行コード
 * @return 結果コード
 */
RESULT engineRun(char *stream)
{
	int ret = RESULT_CONTINUE;

	// 空行またはコメント行ならスキップ
	if (strlen(stream) == 0 || *stream == '#')
	{
		return ret;
	}

	// コード行をプールに追加
	addCode(&pool, stream);

	while (pc < pool.size)
	{
		// プログラムカウンタの指すコード行を取得
		char *code = getCode(&pool, pc);

		Token *tokens = tokenize(code);
		if (tokens)
		{
			Ast *ast = createAst(tokens);
			eval(ast);
			releaseAst(ast);
		}

		if (state == ESTATE_END)
		{
			ret = RESULT_EXIT;
			break;
		}

		pc++;
	}

	return ret;
};
