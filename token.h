#ifndef _TOKEN_H_
#define _TOKEN_H_

/**
 * トークンの種類
 */
typedef enum token_type
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
	/// 記号トークン
	TK_SYMBOL,
	/// 関数トークン
	TK_FUNCTION,
	/// キーワードトークン
	TK_KEYWORD,
} token_type;

/**
 * トークンの値
 */
typedef union token_value {
	/// 変数名
	char name[64]; // 最大64文字
	/// 値
	int value;
	/// 演算子
	char op[3]; // 最大2文字
	/// 記号
	char symbol;
	/// 関数名
	char func[64]; // 最大64文字
	/// キーワード
	char keyword[16];
} token_value;

/**
 * トークン列構造体
 */
typedef struct token
{
	/// トークンの種類
	token_type type;
	/// トークンの値
	token_value value;
	/// 前方トークン
	struct token *prev;
	/// 後方トークン
	struct token *next;
} Token;

Token *createVariableToken(char *);
Token *createConstantsToken(char *);
Token *createOperatorToken(char *);
Token *createUnaryOperatorToken(char *);
Token *createLeftBracketToken(char);
Token *createRightBracketToken(char);
Token *createSymbolToken(char);
Token *createFunctionToken(char *);
Token *createKeywordToken(char *);
Token *addToken(Token *, Token *);
Token *getLastToken(Token *);

#endif