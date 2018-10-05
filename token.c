#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"

/**
 * @brief 変数トークンを生成する
 * @param name 変数名
 * @retval NULL トークン生成に失敗
 * @retval Other 変数トークン
 */
Token *createVariableToken(char *name)
{
	Token *tk = (Token *)calloc(1, sizeof(Token));
	if (!tk)
	{
		return NULL;
	}
	tk->type = TK_VARIABLE;
	strcpy(tk->value.string, name);
	return tk;
};

/**
 * @brief 定数トークンを生成する
 * @param value 値
 * @retval NULL トークン生成に失敗
 * @retval Other 定数トークン
 */
Token *createNumberToken(char *value)
{
	Token *tk = (Token *)calloc(1, sizeof(Token));
	if (!tk)
	{
		return NULL;
	}
	tk->type = TK_NUMBER;
	tk->value.number = atoi(value);
	return tk;
};

/**
 * @brief 演算子トークンを生成する
 * @param value 記号
 * @retval NULL トークン生成に失敗
 * @retval Other 演算子トークン
 */
Token *createOperatorToken(char *value)
{
	Token *tk = (Token *)calloc(1, sizeof(Token));
	if (!tk)
	{
		return NULL;
	}
	tk->type = TK_OPERATION;
	strcpy(tk->value.string, value);
	return tk;
};

/**
 * @brief 単項演算子トークンを生成する
 * @param value 記号
 * @retval NULL トークン生成に失敗
 * @retval Other 単項演算子トークン
 */
Token *createUnaryOperatorToken(char *value)
{
	Token *tk = (Token *)calloc(1, sizeof(Token));
	if (!tk)
	{
		return NULL;
	}
	tk->type = TK_UNARY_OP;
	strcpy(tk->value.string, value);
	return tk;
};

/**
 * @brief 左括弧トークンを生成する
 * @param symb 記号
 * @retval NULL トークン生成に失敗
 * @retval Other 左括弧トークン
 */
Token *createLeftBracketToken(char symb)
{
	Token *tk = (Token *)calloc(1, sizeof(Token));
	if (!tk)
	{
		return NULL;
	}
	tk->type = TK_LEFT_BK;
	tk->value.charactor = symb;
	return tk;
};

/**
 * @brief 右括弧トークンを生成する
 * @param symb 記号
 * @retval NULL トークン生成に失敗
 * @retval Other 右括弧トークン
 */
Token *createRightBracketToken(char symb)
{
	Token *tk = (Token *)calloc(1, sizeof(Token));
	if (!tk)
	{
		return NULL;
	}
	tk->type = TK_RIGHT_BK;
	tk->value.charactor = symb;
	return tk;
};

/**
 * @brief 関数トークンを生成する
 * @param name 関数名
 * @retval NULL トークン生成に失敗
 * @retval Other 変数トークン
 */
Token *createFunctionToken(char *name)
{
	Token *tk = (Token *)calloc(1, sizeof(Token));
	if (!tk)
	{
		return NULL;
	}
	tk->type = TK_FUNCTION;
	strcpy(tk->value.string, name);
	return tk;
};

/**
 * @brief キーワードトークンを生成する
 * @param word キーワード
 * @retval NULL トークン生成に失敗
 * @retval Other キーワードトークン
 */
Token *createKeywordToken(char *word)
{
	Token *tk = (Token *)calloc(1, sizeof(Token));
	if (!tk)
	{
		return NULL;
	}
	tk->type = TK_KEYWORD;
	strcpy(tk->value.string, word);
	return tk;
};

/**
 * @brief トークン群にトークンを追加する
 * @param tokens トークンリストの先頭ポインタ
 * @param tk 追加するトークン
 * @return 追加後のトークン群の先頭ポインタ
 */
Token *addToken(Token *tokens, Token *tk)
{
	if (!tokens)
	{
		return tk;
	}

	Token *temp = tokens;
	while (temp->next != NULL)
	{
		temp = temp->next;
	}

	tk->prev = temp;
	temp->next = tk;

	return tokens;
};

/**
 * @brief トークン列の末尾を取得する
 * @param tokens トークン列
 * @return 末尾のトークンのポインタ
 */
Token *getLastToken(Token *tokens)
{
	if (NULL == tokens)
	{
		return NULL;
	}

	Token *tk = tokens;
	while (tk->next != NULL)
	{
		tk = tk->next;
	};

	return tk;
};

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
			printf("TK_VARIABLE : %s\n", t->value.string);
			break;
		case TK_NUMBER:
			printf("TK_NUMBER : %d\n", t->value.number);
			break;
		case TK_OPERATION:
			printf("TK_OPERATION : %s\n", t->value.string);
			break;
		case TK_UNARY_OP:
			printf("TK_UNARY_OP : %s\n", t->value.string);
			break;
		case TK_LEFT_BK:
			printf("TK_LEFT_BK : %c\n", t->value.charactor);
			break;
		case TK_RIGHT_BK:
			printf("TK_RIGHT_BK : %c\n", t->value.charactor);
			break;
		case TK_FUNCTION:
			printf("TK_FUNCTION : %s\n", t->value.string);
			break;
		case TK_KEYWORD:
			printf("TK_KEYWORD : %s\n", t->value.string);
			break;
		}
	}
};
