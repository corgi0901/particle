#include <stdio.h>
#include <memory.h>
#include <string.h>
#include "engine.h"
#include "lexer.h"
#include "map.h"
#include "util.h"
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
	ESTATE_SKIP
} ENGINE_STATE;

typedef int (*OPERATOR_FUNC)(Ast *);

typedef struct
{
	char *operator;
	OPERATOR_FUNC func;
} Operator_func_def;

static int eval(Ast *);
static int run_function(Function *);
static Var *getOrCreateVar(char *);

static int plus(Ast *);
static int minus(Ast *);
static int times(Ast *);
static int div(Ast *);
static int surplus(Ast *);
static int substitute(Ast *);
static int less(Ast *);
static int more(Ast *);
static int plus_eq(Ast *);
static int minus_eq(Ast *);
static int times_eq(Ast *);
static int div_eq(Ast *);
static int surplus_eq(Ast *);
static int less_eq(Ast *);
static int more_eq(Ast *);
static int equal(Ast *);
static int not_equal(Ast *);
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

static int plus_eq(Ast *node)
{
	int value = eval(node->right);
	Var *var = getOrCreateVar(node->left->root->value.name);
	var->value += value;
	return var->value;
};

static int minus_eq(Ast *node)
{
	int value = eval(node->right);
	Var *var = getOrCreateVar(node->left->root->value.name);
	var->value -= value;
	return var->value;
};

static int times_eq(Ast *node)
{
	int value = eval(node->right);
	Var *var = getOrCreateVar(node->left->root->value.name);
	var->value *= value;
	return var->value;
};

static int div_eq(Ast *node)
{
	int value = eval(node->right);
	Var *var = getOrCreateVar(node->left->root->value.name);
	var->value /= value;
	return var->value;
};

static int surplus_eq(Ast *node)
{
	int value = eval(node->right);
	Var *var = getOrCreateVar(node->left->root->value.name);
	var->value %= value;
	return var->value;
};

static int less_eq(Ast *node)
{
	return eval(node->left) <= eval(node->right);
};

static int more_eq(Ast *node)
{
	return eval(node->left) >= eval(node->right);
};

static int equal(Ast *node)
{
	return eval(node->left) == eval(node->right);
};

static int not_equal(Ast *node)
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
static int eval(Ast *node)
{
	int value = 0;

	if (node == NULL)
	{
		return value;
	}

	if (state == ESTATE_RUN)
	{
		switch (node->root->type)
		{
		case TK_VARIABLE:
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
			break;
		}
		case TK_KEYWORD:
		{
			if (EQ(node->root->value.keyword, "func"))
			{
				state = ESTATE_FUNC;
				Function *func = createFunction(node->left->root->value.name);
				addFunction(func);
				eval(node->left->left);
			}
			else if (EQ(node->root->value.keyword, "return"))
			{
				return_value = eval(node->left);
				return_flag = 1;
			}
			else if (EQ(node->root->value.keyword, "if"))
			{
				if (eval(node->left) == 0)
				{
					state = ESTATE_SKIP;
				}
			}
			break;
		}
		default:
			break;
		}
	}
	else if (state == ESTATE_FUNC)
	{
		switch (node->root->type)
		{
		case TK_KEYWORD:
		{
			if (isStrMatch(node->root->value.keyword, "end"))
			{
				state = ESTATE_RUN;
			}
			break;
		}
		case TK_OPERATION:
		{
			if (EQ(node->root->value.op, ","))
			{
				eval(node->left);
				eval(node->right);
			}
			break;
		}
		case TK_VARIABLE:
		{
			addArg(node->root->value.name);
			break;
		}
		default:
			break;
		}
	}
	else if (state == ESTATE_SKIP)
	{
		switch (node->root->type)
		{
		case TK_KEYWORD:
		{
			if (isStrMatch(node->root->value.keyword, "else"))
			{
				state = ESTATE_RUN;
			}
			else if (isStrMatch(node->root->value.keyword, "fi"))
			{
				state = ESTATE_RUN;
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
	state = ESTATE_RUN;
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
	ENGINE_STATE old_state = state;

	Token *tokens = tokenize(stream);
	if (tokens)
	{
		Ast *ast = createAst(tokens);
		eval(ast);
		releaseAst(ast);
	}

	// サブルーチン入力状態が継続していればコードを保存する
	if (old_state == state && state == ESTATE_FUNC)
	{
		addInstruction(stream);
	}
};
