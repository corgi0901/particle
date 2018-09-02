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
	strcpy(tk->value.name, name);
	return tk;
};

/**
 * @brief 定数トークンを生成する
 * @param value 値
 * @retval NULL トークン生成に失敗
 * @retval Other 定数トークン
 */
Token *createConstantsToken(char *value)
{
	Token *tk = (Token *)calloc(1, sizeof(Token));
	if (!tk)
	{
		return NULL;
	}
	tk->type = TK_NUMBER;
	tk->value.value = atoi(value);
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
	strcpy(tk->value.op, value);
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
	strcpy(tk->value.op, value);
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
	tk->value.symbol = symb;
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
	tk->value.symbol = symb;
	return tk;
};

/**
 * @brief 記号トークンを生成する
 * @param symb 記号
 * @retval NULL トークン生成に失敗
 * @retval Other 記号トークン
 */
Token *createSymbolToken(char symb)
{
	Token *tk = (Token *)calloc(1, sizeof(Token));
	if (!tk)
	{
		return NULL;
	}
	tk->type = TK_SYMBOL;
	tk->value.symbol = symb;
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
	strcpy(tk->value.func, name);
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
	strcpy(tk->value.keyword, word);
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
	Token *tk = tokens;

	while (tk->next != NULL)
	{
		tk = tk->next;
	};

	return tk;
};
