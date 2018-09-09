#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include "lexer.h"
#include "util.h"

/**
 * Lexerの内部状態定義
 */
typedef enum
{
	/// 初期状態
	LSTATE_INIT = 0,
	/// シンボル
	LSTATE_SYMBOL,
	/// 数値
	LSTATE_NUMBER,
	/// 演算子
	LSTATE_OPERATION,
	/// 括弧
	LSTATE_BRACKET,
	/// 読み込み終了
	LSTATE_END,
	/// エラー
	LSTATE_ERROR,
	/// 状態数
	LEXER_STATE_NUM
} LEXER_STATE;

/**
 * 入力文字の種別
 */
typedef enum
{
	/// 文字（a ~ z, _）
	INPUT_CHAR = 0,
	/// 定数（0 ~ 9）
	INPUT_NUM,
	/// 演算子（+, -, *, /, %, =）
	INPUT_OP,
	/// 括弧
	INPUT_BRACKET,
	/// スペース
	INPUT_SPACE,
	/// EOF
	INPUT_EOF,
	/// その他
	INPUT_OTHER,
	/// 種別数
	INPUT_TYPE_NUM
} INPUT_TYPE;

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
	Token *tokens;
	/// 状態
	LEXER_STATE state;
} Lexer;

static int input(Lexer *, char);
static void createToken(Lexer *, TOKEN_TYPE);

/**
 * 現在のLexerの状態と、それに対する入力文字から遷移する先の状態の決定表
 */
#define _init LSTATE_INIT
#define _symb LSTATE_SYMBOL
#define _num_ LSTATE_NUMBER
#define __op_ LSTATE_OPERATION
#define _brac LSTATE_BRACKET
#define _EOF_ LSTATE_END
#define __x__ LSTATE_ERROR
static const LEXER_STATE state_matrix[LEXER_STATE_NUM][INPUT_TYPE_NUM] = {
	/*               char,   num,   op,  bracket, space, eof,  other */
	/* init      */ {_symb, _num_, __op_, _brac, _init, _EOF_, __x__},
	/* symbol    */ {_symb, _symb, __op_, _brac, _init, _EOF_, __x__},
	/* number    */ {__x__, _num_, __op_, _brac, _init, _EOF_, __x__},
	/* operation */ {_symb, _num_, __op_, _brac, _init, _EOF_, __x__},
	/* bracket   */ {_symb, _num_, __op_, _brac, _init, _EOF_, __x__},
	/* eof       */ {_EOF_, _EOF_, _EOF_, _EOF_, _EOF_, _EOF_, _EOF_},
	/* error     */ {__x__, __x__, __x__, __x__, __x__, __x__, __x__},
};
#undef _init
#undef _symb
#undef _num_
#undef __op_
#undef _brac
#undef _EOF_
#undef __x__

typedef void (*LEXER_FUNC)(Lexer *, char);
static void error(Lexer *, char);
static void skip(Lexer *, char);
static void add(Lexer *, char);
static void createSymbol(Lexer *, char);
static void createNumber(Lexer *, char);
static void createOperation(Lexer *, char);
static void createBracket(Lexer *, char);

/**
 * 現在のLexerの状態と、入力文字列に対する挙動の決定表
 */
#define __x__ error
#define _____ skip
#define _add_ add
#define _symb createSymbol
#define _num_ createNumber
#define _op__ createOperation
#define _brac createBracket
static const LEXER_FUNC func_matrix[LEXER_STATE_NUM][INPUT_TYPE_NUM] = {
	/*               char,   num,   op, bracket, space,  eof,  other */
	/* init      */ {_add_, _add_, _add_, _add_, _____, _____, __x__},
	/* symbol    */ {_add_, _add_, _symb, _symb, _symb, _symb, __x__},
	/* number    */ {__x__, _add_, _num_, _num_, _num_, _num_, __x__},
	/* operation */ {_op__, _op__, _op__, _op__, _op__, __x__, __x__},
	/* bracket   */ {_brac, _brac, _brac, _brac, _brac, _brac, __x__},
	/* eof       */ {_____, _____, _____, _____, _____, _____, _____},
	/* error     */ {_____, _____, _____, _____, _____, _____, _____},
};
#undef __x__
#undef _____
#undef _add_
#undef _symb
#undef _num_
#undef _op__
#undef _brac

/**
 * @brief Lexerへの入力エラー
 * @param lxr Lexer
 * @param c 入力文字
 */
static void error(Lexer *lxr, char c)
{
	printf("*** lexer error : %c***\n", c);
	return;
};

/**
 * @brief Lexerへの入力に対して何もしない
 * @param lxr Lexer
 * @param c 入力文字
 */
static void skip(Lexer *lxr, char c)
{
	return;
};

/**
 * @brief 入力文字をLexerに追加する
 * @param lxr Lexer
 * @param c 入力文字
 */
static void add(Lexer *lxr, char c)
{
	lxr->buf[lxr->index++] = c;
};

/**
 * @brief シンボルトークンの生成を行い、入力文字をLexerに追加する
 * @param lxr Lexer
 * @param c 入力文字
 */
static void createSymbol(Lexer *lxr, char c)
{
	if (isStrMatch(lxr->buf, "print", "exit"))
	{
		createToken(lxr, TK_FUNCTION);
	}
	else if (isStrMatch(lxr->buf, "func", "end", "return", "if", "else", "while"))
	{
		createToken(lxr, TK_KEYWORD);
	}
	else
	{
		createToken(lxr, TK_VARIABLE);
	}
	lxr->buf[lxr->index++] = c;
};

/**
 * @brief 数値トークンの生成を行い、入力文字をLexerに追加する
 * @param lxr Lexer
 * @param c 入力文字
 */
static void createNumber(Lexer *lxr, char c)
{
	createToken(lxr, TK_NUMBER);
	lxr->buf[lxr->index++] = c;
};

/**
 * @brief 演算子トークンの生成を行う
 * @param lxr Lexer
 * @param c 入力文字
 */
static void createOperation(Lexer *lxr, char c)
{
	if (c == '=' && isCharMatch(lxr->buf[0], '+', '-', '*', '/', '%', '=', '<', '>', '!'))
	{
		lxr->buf[lxr->index++] = c;
		return;
	}

	Token *last = getLastToken(lxr->tokens);

	if (last == NULL || (last->type != TK_VARIABLE && last->type != TK_NUMBER && last->type != TK_RIGHT_BK))
	{
		createToken(lxr, TK_UNARY_OP);
	}
	else
	{
		createToken(lxr, TK_OPERATION);
	}

	lxr->buf[lxr->index++] = c;
};

/**
 * @brief 括弧トークンの生成を行い、入力文字をLexerに追加する
 * @param lxr Lexer
 * @param c 入力文字
 */
static void createBracket(Lexer *lxr, char c)
{
	if (lxr->buf[0] == '(')
	{
		createToken(lxr, TK_LEFT_BK);
	}
	else if (lxr->buf[0] == ')')
	{
		createToken(lxr, TK_RIGHT_BK);
	}

	lxr->buf[lxr->index++] = c;
};

/**
 * @brief Lexerに対して指定した種類のトークンを生成する
 * @param lxr Lexerオブジェクト
 * @param type トークンの種類
 */
static void createToken(Lexer *lxr, TOKEN_TYPE type)
{
	Token *tk = NULL;
	int val;

	switch (type)
	{
	case TK_VARIABLE:
		tk = createVariableToken(lxr->buf);
		break;
	case TK_NUMBER:
		tk = createNumberToken(lxr->buf);
		break;
	case TK_LEFT_BK:
		tk = createLeftBracketToken(lxr->buf[0]);
		break;
	case TK_RIGHT_BK:
		tk = createRightBracketToken(lxr->buf[0]);
		break;
	case TK_UNARY_OP:
		tk = createUnaryOperatorToken(lxr->buf);
		break;
	case TK_OPERATION:
		tk = createOperatorToken(lxr->buf);
		break;
	case TK_FUNCTION:
		tk = createFunctionToken(lxr->buf);
		break;
	case TK_KEYWORD:
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
static int input(Lexer *lxr, char c)
{
	INPUT_TYPE type;

	if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_')
	{
		type = INPUT_CHAR;
	}
	else if ('0' <= c && c <= '9')
	{
		type = INPUT_NUM;
	}
	else if (isCharMatch(c, '+', '-', '*', '/', '%', '=', '<', '>', '!', ','))
	{
		type = INPUT_OP;
	}
	else if (isCharMatch(c, '(', ')'))
	{
		type = INPUT_BRACKET;
	}
	else if (isCharMatch(c, ' ', '\t'))
	{
		type = INPUT_SPACE;
	}
	else if (isCharMatch(c, '\0', '#'))
	{
		type = INPUT_EOF;
	}
	else
	{
		type = INPUT_OTHER;
	}

	LEXER_STATE new_state = state_matrix[lxr->state][type];
	LEXER_FUNC func = func_matrix[lxr->state][type];

	if (new_state == LSTATE_ERROR)
	{
		printf("*** input error : '%c' ***\n", c);
		return 1;
	}

	func(lxr, c);
	lxr->state = new_state;

	// 初期状態に戻ったらバッファもクリア
	if (lxr->state == LSTATE_INIT)
	{
		memset(lxr->buf, 0, sizeof(lxr->buf));
		lxr->index = 0;
	}

	return 0;
};

/**
 * @brief 入力文字列をトークン列に分解する
 * @param stream 入力文字列
 * @retval NULL エラー
 * @retval tokenのポインタ 分解されたトークン列
 */
Token *tokenize(char *stream)
{
	Lexer lxr;
	memset(lxr.buf, 0, sizeof(lxr.buf));
	lxr.index = 0;
	lxr.tokens = NULL;
	lxr.state = LSTATE_INIT;

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
