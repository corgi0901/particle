#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include "lexer.h"
#include "util.h"

/**
 * Lexerの内部状態定義
 */
typedef enum lexer_state
{
	/// 初期状態
	INIT = 0,
	/// 変数
	VARIABLE,
	/// 定数
	CONSTANT,
	/// 演算子
	OPERATION,
	/// 記号
	SYMBOL,
	/// 読み込み終了
	END,
	/// エラー
	ERROR,
	/// 状態数
	LEXER_STATE_NUM
} lexer_state;

/**
 * 入力文字の種別
 */
typedef enum input_type
{
	/// 文字（a ~ z, _）
	input_char = 0,
	/// 定数（0 ~ 9）
	input_num,
	/// 演算子（+, -, *, /, %, =）
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

static int input(lexer *, char);
static void createToken(lexer *, token_type);

/**
 * 現在のLexerの状態と、それに対する入力文字から遷移する先の状態の決定表
 */
#define _init INIT
#define _var_ VARIABLE
#define _cons CONSTANT
#define __op_ OPERATION
#define _symb SYMBOL
#define _EOF_ END
#define __x__ ERROR
static const lexer_state state_matrix[LEXER_STATE_NUM][input_type_num] = {
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
static void create_constant(lexer *, char);
static void create_operation(lexer *, char);
static void create_symbol(lexer *, char);

/**
 * 現在のLexerの状態と、入力文字列に対する挙動の決定表
 */
#define __x__ error
#define _____ skip
#define _add_ add
#define _var_ create_variable
#define _cons create_constant
#define _op__ create_operation
#define _symb create_symbol
static const LEXER_FUNC func_matrix[LEXER_STATE_NUM][input_type_num] = {
	/*               char,   num,   op,   symb,  space,  eof,  other */
	/* init      */ {_add_, _add_, _add_, _add_, _____, _____, __x__},
	/* variable  */ {_add_, _add_, _var_, _var_, _var_, _var_, __x__},
	/* constants */ {__x__, _add_, _cons, _cons, _cons, _cons, __x__},
	/* operation */ {_op__, _op__, _op__, _op__, _op__, __x__, __x__},
	/* symbol    */ {_symb, _symb, _symb, _symb, _symb, _symb, __x__},
	/* eof       */ {_____, _____, _____, _____, _____, _____, _____},
	/* error     */ {_____, _____, _____, _____, _____, _____, _____},
};
#undef __x__
#undef _____
#undef _add_
#undef _var_
#undef _cons
#undef _op__
#undef _symb

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
	if (isStrMatch(lxr->buf, "print"))
	{
		createToken(lxr, function);
	}
	else if (isStrMatch(lxr->buf, "func", "end", "return", "if", "else", "fi"))
	{
		createToken(lxr, keyword);
	}
	else
	{
		createToken(lxr, variable);
	}
	lxr->buf[lxr->index++] = c;
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
 * @brief 演算子トークンの生成を行う
 * @param lxr Lexer
 * @param c 入力文字
 */
static void create_operation(lexer *lxr, char c)
{
	if (c == '=' && isCharMatch(lxr->buf[0], '+', '-', '*', '/', '%', '=', '<', '>', '!'))
	{
		lxr->buf[lxr->index++] = c;
		return;
	}

	token *last = getLastToken(lxr->tokens);

	if (last == NULL || (last->type != variable && last->type != constants && last->type != right_bracket))
	{
		createToken(lxr, unary_operation);
	}
	else
	{
		createToken(lxr, operation);
	}

	lxr->buf[lxr->index++] = c;
};

/**
 * @brief 記号トークンの生成を行い、入力文字をLexerに追加する
 * @param lxr Lexer
 * @param c 入力文字
 */
static void create_symbol(lexer *lxr, char c)
{
	if (lxr->buf[0] == '(')
	{
		createToken(lxr, left_bracket);
	}
	else if (lxr->buf[0] == ')')
	{
		createToken(lxr, right_bracket);
	}
	else
	{
		createToken(lxr, symbol);
	}

	lxr->buf[lxr->index++] = c;
};

/**
 * @brief Lexerに対して指定した種類のトークンを生成する
 * @param lxr Lexerオブジェクト
 * @param type トークンの種類
 */
static void createToken(lexer *lxr, token_type type)
{
	token *tk = NULL;
	int val;

	switch (type)
	{
	case variable:
		tk = createVariableToken(lxr->buf);
		break;
	case constants:
		tk = createConstantsToken(lxr->buf);
		break;
	case left_bracket:
		tk = createLeftBracketToken(lxr->buf[0]);
		break;
	case right_bracket:
		tk = createRightBracketToken(lxr->buf[0]);
		break;
	case unary_operation:
		tk = createUnaryOperatorToken(lxr->buf);
		break;
	case operation:
		tk = createOperatorToken(lxr->buf);
		break;
	case symbol:
		tk = createSymbolToken(lxr->buf[0]);
		break;
	case function:
		tk = createFunctionToken(lxr->buf);
		break;
	case keyword:
		tk = createKeywordToken(lxr->buf);
		break;
	default:
		break;
	}

	if (tk)
	{
		lxr->tokens = addToken(lxr->tokens, tk);
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

	if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_')
	{
		type = input_char;
	}
	else if ('0' <= c && c <= '9')
	{
		type = input_num;
	}
	else if (isCharMatch(c, '+', '-', '*', '/', '%', '=', '<', '>', '!', ','))
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

	if (new_state == ERROR)
	{
		printf("*** input error : '%c' ***\n", c);
		return 1;
	}

	func(lxr, c);
	lxr->state = new_state;

	// 初期状態に戻ったらバッファもクリア
	if (lxr->state == INIT)
	{
		memset(lxr->buf, 0, sizeof(lxr->buf));
		lxr->index = 0;
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
	lxr.state = INIT;

	for (int i = 0; stream[i] != '\0'; i++)
	{
		if (input(&lxr, stream[i]) != 0)
		{
			return NULL;
		}
	}

	input(&lxr, '\0');

	return lxr.tokens;
};
