#include <stdio.h>
#include "debug.h"

/**
 * @brief トークン列を標準出力に表示する
 * @param tk トークン列
 */
void printTokens(token *tk)
{
	for (token *t = tk; t != NULL; t = t->next)
	{
		switch (t->type)
		{
		case variable:
			printf("variable : %s\n", t->value.name);
			break;
		case constants:
			printf("constants : %d\n", t->value.value);
			break;
		case operation:
			printf("operation : %s\n", t->value.op);
			break;
		case symbol:
			printf("symbol : %c\n", t->value.symbol);
			break;
		case function:
			printf("function : %s\n", t->value.func);
			break;
		case keyword:
			printf("keyword : %s\n", t->value.keyword);
			break;
		}
	}
};

/**
 * @brief 抽象構文木を標準出力に表示する
 * @param tree 抽象構文木
 * @param depth treeの階層
 */
void printAst(ast_node *tree, int depth)
{
	if (tree == NULL)
	{
		return;
	}

	for (int i = 0; i < depth; i++)
	{
		printf("    ");
	}

	switch (tree->root->type)
	{
	case variable:
		printf("%s\n", tree->root->value.name);
		break;
	case constants:
		printf("%d\n", tree->root->value.value);
		break;
	case operation:
		printf("%s\n", tree->root->value.op);
		break;
	case unary_operation:
		printf("%s\n", tree->root->value.op);
		break;
	case function:
		printf("%s\n", tree->root->value.func);
		break;
	}

	printAst(tree->left, depth + 1);
	printAst(tree->right, depth + 1);
}