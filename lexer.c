#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <memory.h>
#include "lexer.h"

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

typedef enum lexer_state
{
	lexer_init = 0,  // 初期状態
	lexer_variable,  // 変数
	lexer_constants, // 定数
	lexer_operation, // 演算子
	lexer_symbol,	// 記号
	lexer_space,	 // スペース
	lexer_error,	 // エラー
	lexer_state_num
} lexer_state;

typedef enum input_type
{
	input_char = 0, // 文字（a ~ z）
	input_num,		// 定数（0 ~ 9）
	input_op,		// 演算子（+, -, *, /, %）
	input_symbol,   // 記号（(,)）
	input_space,	// スペース
	input_other,	// その他
	input_type_num
} input_type;

typedef struct lexer
{
	char buf[64];
	int index;
	token *tokens;
	lexer_state state;
} lexer;

static lexer_state state_matrix[lexer_state_num][input_type_num] = {
	/*               char,           num,             op,              symbol,       space,       other */
	/* init      */ {lexer_variable, lexer_error, lexer_error, lexer_symbol, lexer_init, lexer_error},
	/* variable  */ {lexer_error, lexer_error, lexer_operation, lexer_error, lexer_space, lexer_error},
	/* constants */ {lexer_error, lexer_constants, lexer_operation, lexer_symbol, lexer_space, lexer_error},
	/* operation */ {lexer_variable, lexer_constants, lexer_error, lexer_symbol, lexer_space, lexer_error},
	/* symbol    */ {lexer_variable, lexer_constants, lexer_operation, lexer_symbol, lexer_space, lexer_error},
	/* space     */ {lexer_variable, lexer_constants, lexer_operation, lexer_symbol, lexer_space, lexer_error},
	/* error     */ {lexer_variable, lexer_constants, lexer_operation, lexer_symbol, lexer_space, lexer_error},
};

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