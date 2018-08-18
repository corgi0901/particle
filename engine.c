#include <stdio.h>
#include <memory.h>
#include <string.h>
#include "engine.h"
#include "map.h"

#define EQ(op, val) (strcmp(op, val) == 0)

static int eval(ast_node *);
static var *getOrCreateVar(char *);

/**
 * @brief 入力された抽象構文木を評価する
 * @param node 抽象構文木
 * @return 評価値
 */
static int eval(ast_node *node)
{
	int value = 0;

	if (node->root->type == variable)
	{
		var *item = getOrCreateVar(node->root->value.name);
		value = item->value;
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
	}

	return value;
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
};

/**
 * @brief エンジン部の終了処理
 */
void engine_release(void)
{
	map_release();
};

/**
 * @brief エンジンの実行
 * @param ast 抽象構文木
 */
void engine_exec(ast_node *ast)
{
	eval(ast);
};

/**
 * @brief 指定した変数の値を表示する
 * @param name 変数名
 */
void print(char *name)
{
	var *item = getOrCreateVar(name);
	printf("%d\n", item->value);
};