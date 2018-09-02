#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "ast.h"
#include "util.h"

static int priorLevel(char *);
static int isLessPrior(Token *, Token *);
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
static int isLessPrior(Token *tk1, Token *tk2)
{
	return priorLevel(tk2->value.op) <= priorLevel(tk1->value.op);
};

/**
 * @brief 開き括弧に対応する閉じ括弧を検索する
 * @param start 開き括弧のトークン
 * @retval NULL 見つからない
 * @retval Other 対応する閉じ括弧のトークン
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
	Ast *tree = (Ast*)calloc(1, sizeof(Ast));
	if (!tree)
	{
		return NULL;
	}

	// 括弧で囲まれたトークン群のときは先頭と末尾のそれを削除する
	while (tokens->type == TK_LEFT_BK)
	{
		// 対応する閉じ括弧を検索
		Token *tk = findRightBracket(tokens);

		// 対応する閉じ括弧がトークン群の末尾ならそれらを削除
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

	// 最も優先度の低い演算子を探す
	Token *least_op = NULL;
	for (Token *tk = tokens; tk != NULL; tk = tk->next)
	{
		// 開き括弧があった場合
		if (tk->type == TK_LEFT_BK)
		{
			// 対応する閉じ括弧まで飛ばす
			tk = findRightBracket(tk);
		}
		// 演算子の場合
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