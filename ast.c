#include <stdio.h>
#include <malloc.h>
#include "ast.h"

static int priorLevel(char);
static int isLessPrior(token *, token *);
static token *findCloseBracket(token *);

/**
 * @brief 演算子の優先度を返す
 * @param op 演算子
 * @retval 優先度（大きいほど優先度が高い）
 */
static int priorLevel(char op)
{
	int level = 0;
	switch (op)
	{
	case '=':
		level = 0;
		break;
	case '+':
	case '-':
		level = 1;
		break;
	case '*':
	case '/':
	case '%':
		level = 2;
		break;
	default:
		break;
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
static int isLessPrior(token *tk1, token *tk2)
{
	if (priorLevel(tk2->value.op) <= priorLevel(tk1->value.op))
	{
		return 1;
	}
	else
	{
		return 0;
	}
};

/**
 * @brief 開き括弧に対応する閉じ括弧を検索する
 * @param start 開き括弧のトークン
 * @retval NULL 見つからない
 * @retval Other 対応する閉じ括弧のトークン
 */
static token *findCloseBracket(token *start)
{
	token *tk;
	int level = 1;
	for (tk = start->next; tk != NULL; tk = tk->next)
	{
		if (tk->type == symbol)
		{
			switch (tk->value.op)
			{
			case '(':
				level++;
				break;
			case ')':
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
	}
	return NULL;
};

/**
 * @brief 抽象構文木を生成する
 * @param tokens トークン群
 * @retval NULL エラー
 * @retval Other 抽象構文木
 */
ast_node *createAst(token *tokens)
{
	ast_node *tree = (ast_node *)calloc(1, sizeof(ast_node));
	if (!tree)
	{
		return NULL;
	}

	// '('ではじまるトークン群のとき
	if (tokens->type == symbol && tokens->value.symbol == '(')
	{
		// 対応する閉じ括弧を検索
		token *tk = findCloseBracket(tokens);

		// 対応する閉じ括弧がトークン群の末尾ならそれらを削除
		if (tk->next == NULL)
		{
			token *head = tokens;
			tokens = tokens->next;
			free(head);

			tk->prev->next = NULL;
			free(tk);
		}
	}

	// 先頭の算術演算子は単項演算子として扱う
	if (tokens->type == operation && (tokens->value.op == '-' || tokens->value.op == '+'))
	{
		tokens->type = unary_operation;
	}

	// トークンが１つしかないとき
	if (tokens->next == NULL)
	{
		tree->root = tokens;
		return tree;
	}

	// 最も優先度の低い演算子を探す
	token *least_op = NULL;
	for (token *tk = tokens; tk != NULL; tk = tk->next)
	{
		// 開き括弧があった場合
		if (tk->type == symbol && tk->value.op == '(')
		{
			// 対応する閉じ括弧まで飛ばす
			tk = findCloseBracket(tk);
		}
		// 算術演算子の場合（ただし、演算子が2つ連続した場合は2つ目を単項演算子として扱う）
		else if (tk->type == operation && tk->prev->type != operation)
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
void releaseAst(ast_node *tree)
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