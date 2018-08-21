#ifndef _TOKEN_H_
#define _TOKEN_H_

/**
 * トークンの種類
 */
typedef enum token_type
{
	/// 変数トークン
	variable,
	/// 定数トークン
	constants,
	/// 算術演算子トークン
	operation,
	/// 単項演算子トークン
	unary_operation,
	/// 記号トークン
	symbol,
	/// 関数トークン
	function,
	/// キーワードトークン
	keyword,
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
} token;

token *createVariableToken(char *);
token *createConstantsToken(int);
token *createOperatorToken(char *);
token *createSymbolToken(char);
token *createFunctionToken(char *);
token *createKeywordToken(char *);
token *addToken(token *, token *);

#endif