#include <stdio.h>
#include "debug.h"

/**
 * @brief トークン列を標準出力に表示する
 * @param tk トークン列
 */
void printTokens(Token *tk)
{
	for (Token *t = tk; t != NULL; t = t->next)
	{
		switch (t->type)
		{
		case TK_VARIABLE:
			printf("variable : %s\n", t->value.name);
			break;
		case TK_NUMBER:
			printf("constants : %d\n", t->value.value);
			break;
		case TK_UNARY_OP:
			printf("TK_UNARY_OP : %s\n", t->value.op);
			break;
		case TK_OPERATION:
			printf("TK_OPERATION : %s\n", t->value.op);
			break;
		case TK_LEFT_BK:
			printf("TK_LEFT_BK : %c\n", t->value.symbol);
			break;
		case TK_RIGHT_BK:
			printf("TK_RIGHT_BK : %c\n", t->value.symbol);
			break;
		case TK_SYMBOL:
			printf("TK_SYMBOL : %c\n", t->value.symbol);
			break;
		case TK_FUNCTION:
			printf("TK_FUNCTION : %s\n", t->value.func);
			break;
		case TK_KEYWORD:
			printf("TK_KEYWORD : %s\n", t->value.keyword);
			break;
		}
	}
};

/**
 * @brief 抽象構文木を標準出力に表示する
 * @param tree 抽象構文木
 * @param depth treeの階層
 */
void printAst(Ast *tree, int depth)
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
	case TK_VARIABLE:
		printf("%s\n", tree->root->value.name);
		break;
	case TK_NUMBER:
		printf("%d\n", tree->root->value.value);
		break;
	case TK_OPERATION:
		printf("%s\n", tree->root->value.op);
		break;
	case TK_UNARY_OP:
		printf("%s\n", tree->root->value.op);
		break;
	case TK_SYMBOL:
		printf("%c\n", tree->root->value.symbol);
		break;
	case TK_FUNCTION:
		printf("%s\n", tree->root->value.func);
		break;
	case TK_KEYWORD:
		printf("%s\n", tree->root->value.keyword);
		break;
	default:
		break;
	}

	printAst(tree->left, depth + 1);
	printAst(tree->right, depth + 1);
}