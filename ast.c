#include <stdio.h>
#include <malloc.h>
#include "ast.h"

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
 * @brief トークン1に対し、トークン2の方が優先度が大きいかどうかを検証する
 * @param tk1 トークン1
 * @param tk2 トークン2
 * @retval 0 false
 * @retval 1 true
 */
static int isPrior(token *tk1, token *tk2)
{
	if (priorLevel(tk2->value.op) <= priorLevel(tk1->value.op))
	{
		return 0;
	}
	else
	{
		return 1;
	}
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

	// トークンが１つしかないとき
	if (tokens->next == NULL)
	{
		tree->root = tokens;
		return tree;
	}

	// ()で囲まれたトークン群のとき
	if (tokens->type == symbol && tokens->value.symbol == '(')
	{
		token *tk = tokens;
		for (tk = tokens; tk != NULL; tk = tk->next)
		{
			if (tk->type == symbol && tk->value.symbol == ')')
			{
				break;
			}
		}

		if (tk->next == NULL)
		{
			token *head = tokens;
			tokens = tokens->next;
			free(head);

			tk->prev->next = NULL;
			free(tk);
		}
	}

	int inBrackets = 0;
	token *prior = NULL;
	for (token *tk = tokens; tk != NULL; tk = tk->next)
	{
		if (tk->type == operation && inBrackets == 0)
		{
			if (prior == NULL)
			{
				prior = tk;
			}
			else if (isPrior(prior, tk) == 0)
			{
				prior = tk;
			}
		}
		else if (tk->type == symbol && tk->value.symbol == '(')
		{
			inBrackets = 1;
		}
		else if (tk->type == symbol && tk->value.symbol == ')')
		{
			inBrackets = 0;
		}
	}

	tree->root = prior;
	prior->prev->next = NULL;
	tree->left = createAst(tokens);
	tree->right = createAst(prior->next);

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