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
static int run_function(Function *);
static Var *getOrCreateVar(char *);

static VarMap *local_var_map = NULL;

static int return_value = 0;
static int return_flag = 0;

/**
 * 実行エンジンの状態
 */
typedef enum
{
	/// 実行状態
	RUN = 0,
	/// 関数入力状態
	FUNCTION,
	/// 実行スキップ状態
	SKIP
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
	Var *var = getOrCreateVar(node->left->root->value.name);
	var->value = value;
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
	Var *var = getOrCreateVar(node->left->root->value.name);
	var->value += value;
	return var->value;
};

static int minus_eq(ast_node *node)
{
	int value = eval(node->right);
	Var *var = getOrCreateVar(node->left->root->value.name);
	var->value -= value;
	return var->value;
};

static int times_eq(ast_node *node)
{
	int value = eval(node->right);
	Var *var = getOrCreateVar(node->left->root->value.name);
	var->value *= value;
	return var->value;
};

static int div_eq(ast_node *node)
{
	int value = eval(node->right);
	Var *var = getOrCreateVar(node->left->root->value.name);
	var->value /= value;
	return var->value;
};

static int surplus_eq(ast_node *node)
{
	int value = eval(node->right);
	Var *var = getOrCreateVar(node->left->root->value.name);
	var->value %= value;
	return var->value;
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

/**
 * @brief 関数の引数をパースし、変数マップに格納する
 * @param func 関数オブジェクト
 * @param map 変数マップ
 * @param args 引数のトークン列
 */
static void parseArgs(Function *func, VarMap *map, ast_node *args)
{
	Arg *arg = func->args;
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
		addVar(map, var);

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
			Function *func = getFunction(node->root->value.name);
			if (func)
			{
				// ローカル変数の退避
				VarMap *current_var_map = local_var_map;

				// 引数の評価
				VarMap *var_map = createVarMap();
				parseArgs(func, var_map, node->left);
				local_var_map = var_map;

				// サブルーチン本体の実行
				value = run_function(func);

				// ローカル変数の削除
				clearMap(&var_map);

				// ローカル変数の復帰
				local_var_map = current_var_map;
			}
			else
			{
				Var *var = getOrCreateVar(node->root->value.name);
				value = var->value;
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
			if (strcmp(node->root->value.keyword, "func") == 0)
			{
				state = FUNCTION;
				Function *func = createFunction(node->left->root->value.name);
				addFunction(func);
				eval(node->left->left);
			}
			else if (strcmp(node->root->value.keyword, "return") == 0)
			{
				return_value = eval(node->left);
				return_flag = 1;
			}
			else if (strcmp(node->root->value.keyword, "if") == 0)
			{
				if (eval(node->left) == 0)
				{
					state = SKIP;
				}
			}
			break;
		}
		default:
			break;
		}
	}
	else if (state == FUNCTION)
	{
		switch (node->root->type)
		{
		case keyword:
		{
			if (isStrMatch(node->root->value.keyword, "end"))
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
	else if (state == SKIP)
	{
		switch (node->root->type)
		{
		case keyword:
		{
			if (isStrMatch(node->root->value.keyword, "else"))
			{
				state = RUN;
			}
			else if (isStrMatch(node->root->value.keyword, "fi"))
			{
				state = RUN;
			}
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
 * @param func サブルーチンオブジェクト
 * @return 戻り値
 */
static int run_function(Function *func)
{
	char line[128];
	char c;
	int i = 0, start = 0;
	return_flag = 0;

	do
	{
		c = func->code[i];

		if (c == '\n' || c == '\0')
		{
			// 実行行をコピー
			memset(line, 0, sizeof(line));
			memcpy(line, func->code + start, i - start);

			// コード実行
			engine_run(line);

			// 戻り値チェック
			if (return_flag)
			{
				return_flag = 0;
				return return_value;
			}

			start = i + 1;
		}

		i++;

	} while (c != '\0');

	return 0;
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
void engine_init(void)
{
	map_init();
	local_var_map = createVarMap();
	state = RUN;
};

/**
 * @brief エンジン部の終了処理
 */
void engine_release(void)
{
	map_release();
	clearMap(&local_var_map);
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
	if (old_state == state && state == FUNCTION)
	{
		addInstruction(stream);
	}
};
