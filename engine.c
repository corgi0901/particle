#include <stdio.h>
#include <memory.h>
#include "engine.h"

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
		int locate = node->root->value.name - 'a';
		value = memory[locate];
	}
	else if (node->root->type == constants)
	{
		value = node->root->value.value;
	}
	else if (node->root->type == operation)
	{
		switch (node->root->value.op)
		{
		case '+':
			value = eval(node->left) + eval(node->right);
			break;
		case '-':
			value = eval(node->left) - eval(node->right);
			break;
		case '*':
			value = eval(node->left) * eval(node->right);
			break;
		case '/':
			value = eval(node->left) / eval(node->right);
			break;
		case '%':
			value = eval(node->left) % eval(node->right);
			break;
		case '=':
			value = eval(node->right);
			memory[node->left->root->value.name - 'a'] = value;
			break;
		default:
			value = 0;
		}
	}

	return value;
};

/**
 * @brief エンジン部の初期化
 */
void engine_init(void)
{
	memset(memory, 0, sizeof(memory));
};

/**
 * @brief エンジンの実行
 * @param ast 抽象構文木
 */
void engine_exec(ast_node *ast)
{
	eval(ast);
};