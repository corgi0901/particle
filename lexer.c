#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <memory.h>
#include "lexer.h"

/**
 * @brief 変数トークンを生成する
 * @param name 変数名
 * @retval NULL トークン生成に失敗
 * @retval Other 変数トークン
 */
static token *createVariableToken(char name)
{
	token *tk = (token *)calloc(1, sizeof(token));
	if (!tk)
	{
		return NULL;
	}
	tk->type = variable;
	tk->value.name = name;
	return tk;
};

/**
 * @brief 定数トークンを生成する
 * @param value 値
 * @retval NULL トークン生成に失敗
 * @retval Other 定数トークン
 */
static token *createConstantsToken(int value)
{
	token *tk = (token *)calloc(1, sizeof(token));
	if (!tk)
	{
		return NULL;
	}
	tk->type = constants;
	tk->value.value = value;
	return tk;
};

/**
 * @brief 演算子トークンを生成する
 * @param value 記号
 * @retval NULL トークン生成に失敗
 * @retval Other 演算子トークン
 */
static token *createOperatorToken(char value)
{
	token *tk = (token *)calloc(1, sizeof(token));
	if (!tk)
	{
		return NULL;
	}
	tk->type = operation;
	tk->value.op = value;
	return tk;
};

/**
 * @brief 記号トークンを生成する
 * @param symb 記号
 * @retval NULL トークン生成に失敗
 * @retval Other 記号トークン
 */
static token *createSymbolToken(char symb)
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
 * @brief トークン群にトークンを追加する
 * @param tokens トークンリストの先頭ポインタ
 * @param token 追加するトークン
 * @return 追加後のトークン群の先頭ポインタ
 */
static token *addToken(token *tokens, token *tk)
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
 * Lexerの内部状態定義
 */
typedef enum lexer_state
{
	/// 初期状態
	lexer_init = 0,
	/// 変数
	lexer_variable,
	/// 定数
	lexer_constants,
	/// 演算子
	lexer_operation,
	/// 記号
	lexer_symbol,
	/// スペース
	lexer_space,
	/// エラー
	lexer_error,
	/// 状態数
	lexer_state_num
} lexer_state;

/**
 * 入力文字の種別
 */
typedef enum input_type
{
	/// 文字（a ~ z）
	input_char = 0,
	/// 定数（0 ~ 9）
	input_num,
	/// 演算子（+, -, *, /, %）
	input_op,
	/// 記号（(,)）
	input_symbol,
	/// スペース
	input_space,
	/// その他
	input_other,
	/// 種別数
	input_type_num
} input_type;

typedef struct lexer
{
	char buf[64];
	int index;
	token *tokens;
	lexer_state state;
} lexer;

/**
 * 現在のLexerの状態と、それに対する入力文字から遷移する先の状態の決定表
 */
static const lexer_state state_matrix[lexer_state_num][input_type_num] = {
	/*               char,           num,             op,              symbol,       space,       other */
	/* init      */ {lexer_variable, lexer_error, lexer_error, lexer_symbol, lexer_init, lexer_error},
	/* variable  */ {lexer_error, lexer_error, lexer_operation, lexer_error, lexer_space, lexer_error},
	/* constants */ {lexer_error, lexer_constants, lexer_operation, lexer_symbol, lexer_space, lexer_error},
	/* operation */ {lexer_variable, lexer_constants, lexer_error, lexer_symbol, lexer_space, lexer_error},
	/* symbol    */ {lexer_variable, lexer_constants, lexer_operation, lexer_symbol, lexer_space, lexer_error},
	/* space     */ {lexer_variable, lexer_constants, lexer_operation, lexer_symbol, lexer_space, lexer_error},
	/* error     */ {lexer_variable, lexer_constants, lexer_operation, lexer_symbol, lexer_space, lexer_error},
};

/**
 * @brief Lexerの現在の状態に応じてトークンを生成する
 * @param lxr Lexerオブジェクト
 */
static void createToken(lexer *lxr)
{
	token *tk;
	int val;

	switch (lxr->state)
	{
	case lexer_variable:
		tk = createVariableToken(lxr->buf[0]);
		lxr->tokens = addToken(lxr->tokens, tk);
		break;
	case lexer_constants:
		val = atoi(lxr->buf);
		tk = createConstantsToken(val);
		lxr->tokens = addToken(lxr->tokens, tk);
		break;
	case lexer_operation:
		tk = createOperatorToken(lxr->buf[0]);
		lxr->tokens = addToken(lxr->tokens, tk);
		break;
	case lexer_symbol:
		tk = createSymbolToken(lxr->buf[0]);
		lxr->tokens = addToken(lxr->tokens, tk);
		break;
	default:
		break;
	}
};

/**
 * @brief Lexerに１文字入力する。１文字づつLexerに入力していくことで、順次内部の状態が変化する
 * @param lxr Lexerオブジェクト
 * @param c 入力文字
 * @retval 0 正常
 * @retval 0以外 エラー
 */
static int input(lexer *lxr, char c)
{
	input_type type;

	if ('a' <= c && c <= 'z')
	{
		type = input_char;
	}
	else if ('0' <= c && c <= '9')
	{
		type = input_num;
	}
	else if ('+' == c || '-' == c || '*' == c || '/' == c || '%' == c || '=' == c)
	{
		type = input_op;
	}
	else if ('(' == c || ')' == c)
	{
		type = input_symbol;
	}
	else if (' ' == c)
	{
		type = input_space;
	}
	else
	{
		type = input_other;
	}

	lexer_state new_state = state_matrix[lxr->state][type];

	if (new_state == lexer_error)
	{
		printf("*** input error : '%c' ***\n", c);
		return 1;
	}

	if (new_state == lxr->state)
	{
		lxr->buf[lxr->index++] = c;
	}
	else
	{
		createToken(lxr);
		lxr->state = new_state;
		memset(lxr->buf, 0, sizeof(lxr->buf));
		lxr->index = 0;
		lxr->buf[lxr->index++] = c;
	}

	return 0;
};

/**
 * @brief 入力文字列をトークン群に分解する
 * @param stream 入力文字列
 * @retval NULL エラー
 * @retval tokenのポインタ 分解されたトークン群
 */
token *tokenize(char *stream)
{
	lexer lxr;
	memset(lxr.buf, 0, sizeof(lxr.buf));
	lxr.index = 0;
	lxr.tokens = NULL;
	lxr.state = lexer_init;

	for (int i = 0; stream[i] != '\0'; i++)
	{
		int ret = input(&lxr, stream[i]);
		if (ret)
		{
			return NULL;
		}
	}

	createToken(&lxr);

	return lxr.tokens;
};