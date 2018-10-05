#ifndef _TOKEN_H_
#define _TOKEN_H_

/// トークンの種類
typedef enum
{
	/// 変数トークン
	TK_VARIABLE,
	/// 定数トークン
	TK_NUMBER,
	/// 算術演算子トークン
	TK_OPERATION,
	/// 単項演算子トークン
	TK_UNARY_OP,
	/// 左括弧
	TK_LEFT_BK,
	/// 右括弧
	TK_RIGHT_BK,
	/// 組み込み関数トークン
	TK_FUNCTION,
	/// キーワードトークン
	TK_KEYWORD,
} TOKEN_TYPE;

/// トークンの値
typedef union {
	char string[64];
	int number;
	char charactor;
} TOKEN_VALUE;

///トークン列
typedef struct token
{
	/// トークンの種類
	TOKEN_TYPE type;
	/// トークンの値
	TOKEN_VALUE value;
	/// 前方のトークン
	struct token *prev;
	/// 後方のトークン
	struct token *next;
} Token;

Token *createVariableToken(char *);
Token *createNumberToken(char *);
Token *createOperatorToken(char *);
Token *createUnaryOperatorToken(char *);
Token *createLeftBracketToken(char);
Token *createRightBracketToken(char);
Token *createFunctionToken(char *);
Token *createKeywordToken(char *);
Token *addToken(Token *, Token *);
Token *getLastToken(Token *);

// デバッグ用
void printTokens(Token *);

#endif
