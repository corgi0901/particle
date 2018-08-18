#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <memory.h>
#include "lexer.h"
#include "util.h"

/**
 * Lexerの内部状態定義
 */
typedef enum lexer_state
{
	/// 初期状態
	state_init = 0,
	/// 変数
	state_variable,
	/// 定数
	state_constant,
	/// 演算子
	state_operation,
	/// 記号
	state_symbol,
	/// 読み込み終了
	state_eof,
	/// エラー
	state_error,
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
	/// EOF
	input_eof,
	/// その他
	input_other,
	/// 種別数
	input_type_num
} input_type;

/**
 * Lexer構造体
 *
 */
typedef struct lexer
{
	/// 入力文字列バッファ
	char buf[64];
	/// 入力文字列インデックス
	int index;
	/// トークン群
	token *tokens;
	/// 状態
	lexer_state state;
} lexer;

/**
 * 現在のLexerの状態と、それに対する入力文字から遷移する先の状態の決定表
 */
#define _init state_init
#define _var_ state_variable
#define _cons state_constant
#define __op_ state_operation
#define _symb state_symbol
#define _EOF_ state_eof
#define __x__ state_error
static const lexer_state state_matrix[lexer_state_num][input_type_num] = {
	/*               char,   num,   op,   symb,  space,  eof,  other */
	/* init      */ {_var_, _cons, __op_, _symb, _init, _EOF_, __x__},
	/* variable  */ {_var_, _var_, __op_, _symb, _init, _EOF_, __x__},
	/* constants */ {__x__, _cons, __op_, _symb, _init, _EOF_, __x__},
	/* operation */ {_var_, _cons, __op_, _symb, _init, _EOF_, __x__},
	/* symbol    */ {_var_, _cons, __op_, _symb, _init, _EOF_, __x__},
	/* eof       */ {_EOF_, _EOF_, _EOF_, _EOF_, _EOF_, _EOF_, _EOF_},
	/* error     */ {__x__, __x__, __x__, __x__, __x__, __x__, __x__},
};
#undef _init
#undef _var_
#undef _cons
#undef __op_
#undef _symb
#undef _EOF_
#undef __x__

typedef void (*LEXER_FUNC)(lexer *, char);
static void error(lexer *, char);
static void skip(lexer *, char);
static void add(lexer *, char);
static void create_variable(lexer *, char);
static void create_variable2(lexer *, char);
static void create_constant(lexer *, char);
static void create_constant2(lexer *, char);
static void create_operation(lexer *, char);
static void create_operation2(lexer *, char);
static void create_symbol(lexer *, char);
static void create_symbol2(lexer *, char);

/**
 * 現在のLexerの状態と、入力文字列に対する挙動の決定表
 */
#define __x__ error
#define _____ skip
#define _add_ add
#define _var1 create_variable
#define _var2 create_variable2
#define _con1 create_constant
#define _con2 create_constant2
#define _op1_ create_operation
#define _op2_ create_operation2
#define symb1 create_symbol
#define symb2 create_symbol2
static const LEXER_FUNC func_matrix[lexer_state_num][input_type_num] = {
	/*               char,   num,   op,   symb,  space,  eof,  other */
	/* init      */ {_add_, _add_, _add_, _add_, _____, _____, __x__},
	/* variable  */ {_add_, _add_, _var1, _var1, _var2, _var2, __x__},
	/* constants */ {__x__, _add_, _con1, _con1, _con2, _con2, __x__},
	/* operation */ {_op1_, _op1_, _op1_, _op1_, _op2_, __x__, __x__},
	/* symbol    */ {symb1, symb1, symb1, symb1, symb2, symb1, __x__},
	/* eof       */ {_____, _____, _____, _____, _____, _____, _____},
	/* error     */ {_____, _____, _____, _____, _____, _____, _____},
};
#undef __x__
#undef _____
#undef _add_
#undef _var1
#undef _var2
#undef _con1
#undef _con2
#undef _op1_
#undef _op2_
#undef symb1
#undef symb2

static token *createVariableToken(char *);
static token *createConstantsToken(int);
static token *createOperatorToken(char *);
static token *createSymbolToken(char);
static token *addToken(token *, token *);
static void createToken(lexer *, token_type);

/**
 * @brief 変数トークンを生成する
 * @param name 変数名
 * @retval NULL トークン生成に失敗
 * @retval Other 変数トークン
 */
static token *createVariableToken(char *name)
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
static token *createOperatorToken(char *value)
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
 * @param tk 追加するトークン
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
 * @brief Lexerへの入力エラー
 * @param lxr Lexer
 * @param c 入力文字
 */
static void error(lexer *lxr, char c)
{
	printf("*** lexer error : %c***\n", c);
	return;
};

/**
 * @brief Lexerへの入力に対して何もしない
 * @param lxr Lexer
 * @param c 入力文字
 */
static void skip(lexer *lxr, char c)
{
	return;
};

/**
 * @brief 入力文字をLexerに追加する
 * @param lxr Lexer
 * @param c 入力文字
 */
static void add(lexer *lxr, char c)
{
	lxr->buf[lxr->index++] = c;
};

/**
 * @brief 変数トークンの生成を行い、入力文字をLexerに追加する
 * @param lxr Lexer
 * @param c 入力文字
 */
static void create_variable(lexer *lxr, char c)
{
	createToken(lxr, variable);
	lxr->buf[lxr->index++] = c;
};

/**
 * @brief 変数トークンの生成のみを行う。入力文字は無視。
 * @param lxr Lexer
 * @param c 入力文字
 */
static void create_variable2(lexer *lxr, char c)
{
	createToken(lxr, variable);
};

/**
 * @brief 定数トークンの生成を行い、入力文字をLexerに追加する
 * @param lxr Lexer
 * @param c 入力文字
 */
static void create_constant(lexer *lxr, char c)
{
	createToken(lxr, constants);
	lxr->buf[lxr->index++] = c;
};

/**
 * @brief 定数トークンの生成のみを行う。入力文字は無視。
 * @param lxr Lexer
 * @param c 入力文字
 */
static void create_constant2(lexer *lxr, char c)
{
	createToken(lxr, constants);
};

/**
 * @brief 演算子トークンの生成を行う
 * @param lxr Lexer
 * @param c 入力文字
 */
static void create_operation(lexer *lxr, char c)
{
	char top = lxr->buf[0];

	if (c == '=')
	{
		if (isCharMatch(top, '+', '-', '*', '/', '%'))
		{
			lxr->buf[lxr->index++] = c;
		}
	}
	else
	{
		createToken(lxr, operation);
		lxr->buf[lxr->index++] = c;
	}
};

/**
 * @brief 演算子トークンの生成のみを行う。入力文字は無視。
 * @param lxr Lexer
 * @param c 入力文字
 */
static void create_operation2(lexer *lxr, char c)
{
	createToken(lxr, operation);
};

/**
 * @brief 記号トークンの生成を行い、入力文字をLexerに追加する
 * @param lxr Lexer
 * @param c 入力文字
 */
static void create_symbol(lexer *lxr, char c)
{
	createToken(lxr, symbol);
	lxr->buf[lxr->index++] = c;
};

/**
 * @brief 記号トークンの生成のみを行う。入力文字は無視。
 * @param lxr Lexer
 * @param c 入力文字
 */
static void create_symbol2(lexer *lxr, char c)
{
	createToken(lxr, symbol);
};

/**
 * @brief Lexerに対して指定した種類のトークンを生成する
 * @param lxr Lexerオブジェクト
 * @param type トークンの種類
 */
static void createToken(lexer *lxr, token_type type)
{
	token *tk;
	int val;

	switch (type)
	{
	case variable:
		tk = createVariableToken(lxr->buf);
		lxr->tokens = addToken(lxr->tokens, tk);
		break;
	case constants:
		val = atoi(lxr->buf);
		tk = createConstantsToken(val);
		lxr->tokens = addToken(lxr->tokens, tk);
		break;
	case operation:
		tk = createOperatorToken(lxr->buf);
		lxr->tokens = addToken(lxr->tokens, tk);
		break;
	case symbol:
		tk = createSymbolToken(lxr->buf[0]);
		lxr->tokens = addToken(lxr->tokens, tk);
		break;
	default:
		break;
	}

	memset(lxr->buf, 0, sizeof(lxr->buf));
	lxr->index = 0;
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
	else if (isCharMatch(c, '+', '-', '*', '/', '%', '='))
	{
		type = input_op;
	}
	else if (isCharMatch(c, '(', ')'))
	{
		type = input_symbol;
	}
	else if (isCharMatch(c, ' ', '\t'))
	{
		type = input_space;
	}
	else if (isCharMatch(c, '\0', '#'))
	{
		type = input_eof;
	}
	else
	{
		type = input_other;
	}

	lexer_state new_state = state_matrix[lxr->state][type];
	LEXER_FUNC func = func_matrix[lxr->state][type];

	if (new_state == state_error)
	{
		printf("*** input error : '%c' ***\n", c);
		return 1;
	}

	func(lxr, c);
	lxr->state = new_state;

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
	lxr.state = state_init;

	int i = 0;

	while (1)
	{
		int ret = input(&lxr, stream[i]);
		if (ret)
		{
			return NULL;
		}

		if (stream[i] == '\0')
		{
			break;
		}

		i++;
	}

	return lxr.tokens;
};