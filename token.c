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
token *createVariableToken(char *name)
{
	token *tk = (token *)calloc(1, sizeof(token));
	if (!tk)
	{
		return NULL;
	}
	tk->type = variable;
	strcpy(tk->value.name, name);
	return tk;
};

/**
 * @brief 定数トークンを生成する
 * @param value 値
 * @retval NULL トークン生成に失敗
 * @retval Other 定数トークン
 */
token *createConstantsToken(char *value)
{
	token *tk = (token *)calloc(1, sizeof(token));
	if (!tk)
	{
		return NULL;
	}
	tk->type = constants;
	tk->value.value = atoi(value);
	return tk;
};

/**
 * @brief 演算子トークンを生成する
 * @param value 記号
 * @retval NULL トークン生成に失敗
 * @retval Other 演算子トークン
 */
token *createOperatorToken(char *value)
{
	token *tk = (token *)calloc(1, sizeof(token));
	if (!tk)
	{
		return NULL;
	}
	tk->type = operation;
	strcpy(tk->value.op, value);
	return tk;
};

/**
 * @brief 単項演算子トークンを生成する
 * @param value 記号
 * @retval NULL トークン生成に失敗
 * @retval Other 単項演算子トークン
 */
token *createUnaryOperatorToken(char *value)
{
	token *tk = (token *)calloc(1, sizeof(token));
	if (!tk)
	{
		return NULL;
	}
	tk->type = unary_operation;
	strcpy(tk->value.op, value);
	return tk;
};

/**
 * @brief 左括弧トークンを生成する
 * @param symb 記号
 * @retval NULL トークン生成に失敗
 * @retval Other 左括弧トークン
 */
token *createLeftBracketToken(char symb)
{
	token *tk = (token *)calloc(1, sizeof(token));
	if (!tk)
	{
		return NULL;
	}
	tk->type = left_bracket;
	tk->value.symbol = symb;
	return tk;
};

/**
 * @brief 右括弧トークンを生成する
 * @param symb 記号
 * @retval NULL トークン生成に失敗
 * @retval Other 右括弧トークン
 */
token *createRightBracketToken(char symb)
{
	token *tk = (token *)calloc(1, sizeof(token));
	if (!tk)
	{
		return NULL;
	}
	tk->type = right_bracket;
	tk->value.symbol = symb;
	return tk;
};

/**
 * @brief 記号トークンを生成する
 * @param symb 記号
 * @retval NULL トークン生成に失敗
 * @retval Other 記号トークン
 */
token *createSymbolToken(char symb)
{
	token *tk = (token *)calloc(1, sizeof(token));
	if (!tk)
	{
		return NULL;
	}
	tk->type = symbol;
	tk->value.symbol = symb;
	return tk;
};

/**
 * @brief 関数トークンを生成する
 * @param name 関数名
 * @retval NULL トークン生成に失敗
 * @retval Other 変数トークン
 */
token *createFunctionToken(char *name)
{
	token *tk = (token *)calloc(1, sizeof(token));
	if (!tk)
	{
		return NULL;
	}
	tk->type = function;
	strcpy(tk->value.func, name);
	return tk;
};

/**
 * @brief キーワードトークンを生成する
 * @param word キーワード
 * @retval NULL トークン生成に失敗
 * @retval Other キーワードトークン
 */
token *createKeywordToken(char *word)
{
	token *tk = (token *)calloc(1, sizeof(token));
	if (!tk)
	{
		return NULL;
	}
	tk->type = keyword;
	strcpy(tk->value.keyword, word);
	return tk;
};

/**
 * @brief トークン群にトークンを追加する
 * @param tokens トークンリストの先頭ポインタ
 * @param tk 追加するトークン
 * @return 追加後のトークン群の先頭ポインタ
 */
token *addToken(token *tokens, token *tk)
{
	if (!tokens)
	{
		return tk;
	}

	token *temp = tokens;
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
token *getLastToken(token *tokens)
{
	token *tk = tokens;

	while (tk->next != NULL)
	{
		tk = tk->next;
	};

	return tk;
};
