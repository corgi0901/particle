#include <stdio.h>
#include <memory.h>
#include <string.h>
#include "engine.h"
#include "lexer.h"
#include "map.h"
#include "debug.h"

#define EQ(op, val) (strcmp(op, val) == 0)

static int eval(ast_node *);
static void run_subroutine(Subroutine *sub);
static var *getOrCreateVar(char *);

/**
 * 実行エンジンの状態
 */
typedef enum
{
	/// 実行状態
	e_run = 0,
	/// サブルーチン検出
	e_start_sub,
	/// サブルーチン入力状態
	e_subroutine,
} engine_state;

static engine_state state;

/**
 * @brief 入力された抽象構文木を評価する
 * @param node 抽象構文木
 * @return 評価値
 */
static int eval(ast_node *node)
{
	int value = 0;

	if (state == e_run)
	{
		if (node->root->type == variable)
		{
			Subroutine *sub = getSubroutine(node->root->value.name);
			if (sub)
			{
				run_subroutine(sub);
			}
			else
			{
				var *item = getOrCreateVar(node->root->value.name);
				value = item->value;
			}
		}
		else if (node->root->type == constants)
		{
			value = node->root->value.value;
		}
		else if (node->root->type == operation)
		{
			if (EQ("+", node->root->value.op))
			{
				value = eval(node->left) + eval(node->right);
			}
			else if (EQ("-", node->root->value.op))
			{
				value = eval(node->left) - eval(node->right);
			}
			else if (EQ("*", node->root->value.op))
			{
				value = eval(node->left) * eval(node->right);
			}
			else if (EQ("/", node->root->value.op))
			{
				value = eval(node->left) / eval(node->right);
			}
			else if (EQ("%", node->root->value.op))
			{
				value = eval(node->left) % eval(node->right);
			}
			else if (EQ("=", node->root->value.op))
			{
				value = eval(node->right);
				var *item = getOrCreateVar(node->left->root->value.name);
				item->value = value;
			}
			else if (EQ("<", node->root->value.op))
			{
				value = eval(node->left) < eval(node->right);
			}
			else if (EQ(">", node->root->value.op))
			{
				value = eval(node->left) > eval(node->right);
			}
			else if (EQ("+=", node->root->value.op))
			{
				value = eval(node->right);
				var *item = getOrCreateVar(node->left->root->value.name);
				item->value += value;
			}
			else if (EQ("-=", node->root->value.op))
			{
				value = eval(node->right);
				var *item = getOrCreateVar(node->left->root->value.name);
				item->value -= value;
			}
			else if (EQ("*=", node->root->value.op))
			{
				value = eval(node->right);
				var *item = getOrCreateVar(node->left->root->value.name);
				item->value *= value;
			}
			else if (EQ("/=", node->root->value.op))
			{
				value = eval(node->right);
				var *item = getOrCreateVar(node->left->root->value.name);
				item->value /= value;
			}
			else if (EQ("%=", node->root->value.op))
			{
				value = eval(node->right);
				var *item = getOrCreateVar(node->left->root->value.name);
				item->value %= value;
			}
			else if (EQ("<=", node->root->value.op))
			{
				value = eval(node->left) <= eval(node->right);
			}
			else if (EQ(">=", node->root->value.op))
			{
				value = eval(node->left) >= eval(node->right);
			}
			else if (EQ("==", node->root->value.op))
			{
				value = eval(node->left) == eval(node->right);
			}
			else if (EQ("!=", node->root->value.op))
			{
				value = eval(node->left) != eval(node->right);
			}
		}
		else if (node->root->type == unary_operation)
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
		}
		else if (node->root->type == function)
		{
			if (strcmp(node->root->value.func, "print") == 0)
			{
				printf("%d\n", eval(node->left));
			}
		}
		else if (node->root->type == keyword)
		{
			if (strcmp(node->root->value.keyword, "sub") == 0)
			{
				state = e_start_sub;
				Subroutine *sub = createSubroutine(node->left->root->value.name);
				addSubroutine(sub);
			}
		}
	}
	else if (state == e_subroutine)
	{
		if (node->root->type == keyword)
		{
			if (strcmp(node->root->value.keyword, "end_sub") == 0)
			{
				state = e_run;
			}
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

	for (i = 0; i < strlen(sub->buf); i++)
	{
		if (sub->buf[i] == '\n')
		{
			memset(line, 0, sizeof(line));
			memcpy(line, sub->buf + start, i - start);
			engine_run(line);
			start = i + 1;
		}
	}

	memset(line, 0, sizeof(line));
	memcpy(line, sub->buf + start, i - start);
	engine_run(line);
};

/**
 * @brief 変数オブジェクトを取得する（なければ生成して変数マップに追加）
 * @param name 変数名
 * @retval NULL エラー
 * @retval Other 変数オブジェクトのポインタ
 */
static var *getOrCreateVar(char *name)
{
	var *item = getVar(name);
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
	state = e_run;
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
	token *tokens = tokenize(stream);
	if (tokens)
	{
		ast_node *ast = createAst(tokens);
		eval(ast);
		releaseAst(ast);
	}

	if (state == e_subroutine)
	{
		addInstruction(stream);
	}
	else if (state == e_start_sub)
	{
		state = e_subroutine;
	}
};
