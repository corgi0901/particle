#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "ast.h"
#include "util.h"
#include "particle.h"

static int priorLevel(char *);
static BOOL isLessPrior(Token *, Token *);
static Token *findRightBracket(Token *);

/**
 * @brief 演算子の優先度を返す
 * @param op 演算子
 * @retval 優先度（大きいほど優先度が高い）
 */
static int priorLevel(char *op)
{
	int level = 0;
	if (isStrMatch(op, ","))
	{
		level = 0;
	}
	else if (isStrMatch(op, "=", "+=", "-=", "*=", "/=", "%="))
	{
		level = 1;
	}
	else if (isStrMatch(op, "==", "!="))
	{
		level = 2;
	}
	else if (isStrMatch(op, "<", ">", "<=", ">="))
	{
		level = 3;
	}
	else if (isStrMatch(op, "+", "-"))
	{
		level = 4;
	}
	else if (isStrMatch(op, "*", "/", "%"))
	{
		level = 5;
	}
	return level;
};

/**
 * @brief トークン1に対し、トークン2の方が優先度が小さいかどうかを検証する
 * @param tk1 トークン1
 * @param tk2 トークン2
 * @retval 0 false
 * @retval 1 true
 */
static BOOL isLessPrior(Token *tk1, Token *tk2)
{
	return priorLevel(tk2->value.op) <= priorLevel(tk1->value.op) ? TRUE : FALSE;
};

/**
 * @brief 左括弧に対応する右括弧を検索する
 * @param start 左括弧のトークン
 * @retval NULL 見つからない
 * @retval Other 対応する右括弧のトークン
 */
static Token *findRightBracket(Token *start)
{
	Token *tk;
	int level = 1;
	for (tk = start->next; tk != NULL; tk = tk->next)
	{
		switch (tk->type)
		{
		case TK_LEFT_BK:
			level++;
			break;
		case TK_RIGHT_BK:
			level--;
			break;
		default:
			break;
		}

		if (level == 0)
		{
			return tk;
		}
	}
	return NULL;
};

/**
 * @brief 抽象構文木を生成する
 * @param tokens トークン群
 * @retval NULL エラー
 * @retval Other 抽象構文木
 */
Ast *createAst(Token *tokens)
{
	Ast *tree = (Ast *)calloc(1, sizeof(Ast));
	if (!tree)
	{
		return NULL;
	}

	// 括弧で囲まれたトークン群のときは先頭と末尾のそれを削除する
	while (tokens->type == TK_LEFT_BK)
	{
		// 括弧の中身がなければNULLを返す
		if (tokens->next->type == TK_RIGHT_BK)
		{
			free(tokens->next);
			free(tokens);
			free(tree);
			return NULL;
		}

		// 対応する右括弧を検索
		Token *tk = findRightBracket(tokens);

		// 対応する右括弧がトークン群の末尾ならそれらを削除
		if (tk->next == NULL)
		{
			Token *head = tokens;
			tokens = tokens->next;
			free(head);

			tk->prev->next = NULL;
			free(tk);
		}
		else
		{
			break;
		}
	}

	// トークンが１つしかないとき
	if (tokens->next == NULL)
	{
		tree->root = tokens;
		return tree;
	}

	// 先頭がキーワードのとき
	if (tokens->type == TK_KEYWORD)
	{
		tree->root = tokens;
		tree->left = createAst(tokens->next);
		tree->right = NULL;
		return tree;
	}

	// 最も優先度の低い演算子を探す
	Token *least_op = NULL;
	for (Token *tk = tokens; tk != NULL; tk = tk->next)
	{
		if (tk->type == TK_LEFT_BK)
		{
			tk = findRightBracket(tk);
		}
		else if (tk->type == TK_OPERATION)
		{
			if (least_op == NULL || isLessPrior(least_op, tk))
			{
				least_op = tk;
			}
		}
	}

	if (least_op)
	{
		tree->root = least_op;
		least_op->prev->next = NULL;
		tree->left = createAst(tokens);
		tree->right = createAst(least_op->next);
	}
	else
	{
		tree->root = tokens;
		tree->left = createAst(tokens->next);
		tree->right = NULL;
	}

	return tree;
};

/**
 * @brief 抽象構文木を破棄する
 * @param tree 破棄する抽象構文木
 */
void releaseAst(Ast *tree)
{
	if (tree->left)
	{
		releaseAst(tree->left);
	}
	if (tree->right)
	{
		releaseAst(tree->right);
	}
	free(tree->root);
	free(tree);
};

/**
 * @brief 抽象構文木を標準出力に表示する（デバッグ用）
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
};
