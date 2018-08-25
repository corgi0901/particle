#include <stdio.h>
#include <memory.h>
#include <string.h>
#include "engine.h"
#include "lexer.h"
#include "map.h"
#include "util.h"
#include "debug.h"

#define EQ(op, val) (strcmp(op, val) == 0)

static int eval(ast_node *);
static void run_subroutine(Subroutine *sub);
static Var *getOrCreateVar(char *);

/**
 * 実行エンジンの状態
 */
typedef enum
{
	/// 実行状態
	RUN = 0,
	/// サブルーチン入力状態
	SUBROUTINE,
} engine_state;

static engine_state state;

typedef int (*ENGINE_FUNC)(ast_node *);
typedef struct
{
	char *operator;
	ENGINE_FUNC func;
} engine_func_def;

static int plus(ast_node *node)
{
	return eval(node->left) + eval(node->right);
};

static int minus(ast_node *node)
{
	return eval(node->left) - eval(node->right);
};

static int times(ast_node *node)
{
	return eval(node->left) * eval(node->right);
};

static int div(ast_node *node)
{
	return eval(node->left) / eval(node->right);
};

static int surplus(ast_node *node)
{
	return eval(node->left) % eval(node->right);
};

static int substitute(ast_node *node)
{
	int value = eval(node->right);
	Var *item = getOrCreateVar(node->left->root->value.name);
	item->value = value;
	return value;
};

static int less(ast_node *node)
{
	return eval(node->left) < eval(node->right);
};

static int more(ast_node *node)
{
	return eval(node->left) > eval(node->right);
};

static int plus_eq(ast_node *node)
{
	int value = eval(node->right);
	Var *item = getOrCreateVar(node->left->root->value.name);
	item->value += value;
	return item->value;
};

static int minus_eq(ast_node *node)
{
	int value = eval(node->right);
	Var *item = getOrCreateVar(node->left->root->value.name);
	item->value -= value;
	return item->value;
};

static int times_eq(ast_node *node)
{
	int value = eval(node->right);
	Var *item = getOrCreateVar(node->left->root->value.name);
	item->value *= value;
	return item->value;
};

static int div_eq(ast_node *node)
{
	int value = eval(node->right);
	Var *item = getOrCreateVar(node->left->root->value.name);
	item->value /= value;
	return item->value;
};

static int surplus_eq(ast_node *node)
{
	int value = eval(node->right);
	Var *item = getOrCreateVar(node->left->root->value.name);
	item->value %= value;
	return item->value;
};

static int less_eq(ast_node *node)
{
	return eval(node->left) <= eval(node->right);
};

static int more_eq(ast_node *node)
{
	return eval(node->left) >= eval(node->right);
};

static int equal(ast_node *node)
{
	return eval(node->left) == eval(node->right);
};

static int not_equal(ast_node *node)
{
	return eval(node->left) != eval(node->right);
};

static int comma(ast_node *node)
{
	eval(node->left);
	eval(node->right);
	return 0;
};

/**
 * 演算子とそれに対応した実処理関数のテーブル
 */
static engine_func_def engine_func_table[] = {
	{"=", substitute},

	{"+", plus},
	{"-", minus},
	{"*", times},
	{"/", div},
	{"%", surplus},

	{"<", less},
	{">", more},
	{"<=", less_eq},
	{">=", more_eq},
	{"==", equal},
	{"!=", not_equal},

	{"+=", plus_eq},
	{"-=", minus_eq},
	{"*=", times_eq},
	{"/=", div_eq},
	{"%=", surplus_eq},

	{",", comma},
};

/**
 * @brief 指定した演算子に対応する実処理関数を取得する
 * @param operator 演算子
 * @retval NULL 該当する演算子がない
 * @retval Other 実処理関数
 */
static ENGINE_FUNC getEngineFunc(char *operator)
{
	ENGINE_FUNC func = NULL;
	int num = sizeof(engine_func_table) / sizeof(engine_func_def);

	for (int i = 0; i < num; i++)
	{
		if (strcmp(operator, engine_func_table[i].operator) == 0)
		{
			func = engine_func_table[i].func;
			break;
		}
	}
	return func;
};

static void parseArgs(Subroutine *sub, ast_node *args)
{
	Arg *arg = sub->args;
	ast_node *node = args;

	while (arg)
	{
		int value;

		if (node->root->type == operation && strcmp(node->root->value.op, ",") == 0)
		{
			value = eval(node->left);
			node = node->right;
		}
		else
		{
			value = eval(node);
		}

		// ローカル変数として値を保持
		DPRINTF("arg : %s, value = %d\n", arg->name, value);
		Var *var = createVar(arg->name, value);
		addLocalVar(var);

		arg = arg->next;
	}
};

/**
 * @brief 入力された抽象構文木を評価する
 * @param node 抽象構文木
 * @return 評価値
 */
static int eval(ast_node *node)
{
	int value = 0;

	if (node == NULL)
	{
		return value;
	}

	if (state == RUN)
	{
		switch (node->root->type)
		{
		case variable:
		{
			Subroutine *sub = getSubroutine(node->root->value.name);
			if (sub)
			{
				// 引数の評価
				parseArgs(sub, node->left);
				// サブルーチン本体の実行
				run_subroutine(sub);
				// ローカル変数の削除
				releaseLocalVar();
			}
			else
			{
				Var *item = getOrCreateVar(node->root->value.name);
				value = item->value;
			}
			break;
		}
		case constants:
		{
			value = node->root->value.value;
			break;
		}
		case operation:
		{
			ENGINE_FUNC func = getEngineFunc(node->root->value.op);
			if (func)
			{
				value = func(node);
			}
			break;
		}
		case unary_operation:
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
		case function:
		{
			if (strcmp(node->root->value.func, "print") == 0)
			{
				printf("%d\n", eval(node->left));
			}
			break;
		}
		case keyword:
		{
			if (strcmp(node->root->value.keyword, "sub") == 0)
			{
				state = SUBROUTINE;
				Subroutine *sub = createSubroutine(node->left->root->value.name);
				addSubroutine(sub);
				eval(node->left->left);
			}
			break;
		}
		default:
			break;
		}
	}
	else if (state == SUBROUTINE)
	{
		switch (node->root->type)
		{
		case keyword:
		{
			if (isStrMatch(node->root->value.keyword, "end_sub"))
			{
				state = RUN;
			}
			break;
		}
		case operation:
		{
			if (strcmp(node->root->value.op, ",") == 0)
			{
				eval(node->left);
				eval(node->right);
			}
			break;
		}
		case variable:
		{
			addArg(node->root->value.name);
			break;
		}
		default:
			break;
		}
	}

	return value;
};

/**
 * @brief サブルーチンを実行する
 * @param sub サブルーチンオブジェクト
 */
static void run_subroutine(Subroutine *sub)
{
	char line[128];
	int i, start = 0;

	for (i = 0; i < strlen(sub->code); i++)
	{
		if (sub->code[i] == '\n')
		{
			memset(line, 0, sizeof(line));
			memcpy(line, sub->code + start, i - start);
			engine_run(line);
			start = i + 1;
		}
	}

	memset(line, 0, sizeof(line));
	memcpy(line, sub->code + start, i - start);
	engine_run(line);
};

/**
 * @brief 変数オブジェクトを取得する（なければ生成して変数マップに追加）
 * @param name 変数名
 * @retval NULL エラー
 * @retval Other 変数オブジェクトのポインタ
 */
static Var *getOrCreateVar(char *name)
{
	Var *item = getLocalVar(name);

	if (!item)
	{
		item = getVar(name);
	}

	if (!item)
	{
		item = createVar(name, 0);
		if (item)
		{
			addVar(item);
		}
	}
	return item;
};

/**
 * @brief エンジン部の初期化
 */
void engine_init(void)
{
	map_init();
	state = RUN;
};

/**
 * @brief エンジン部の終了処理
 */
void engine_release(void)
{
	map_release();
};

/**
 * @brief コードの実行
 * @param stream 実行コード
 */
void engine_run(char *stream)
{
	engine_state old_state = state;

	token *tokens = tokenize(stream);
	if (tokens)
	{
		ast_node *ast = createAst(tokens);
		eval(ast);
		releaseAst(ast);
	}

	// サブルーチン入力状態が継続していればコードを保存する
	if (old_state == state && state == SUBROUTINE)
	{
		addInstruction(stream);
	}
};
