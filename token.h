#ifndef _TOKEN_H_
#define _TOKEN_H_

// トークンの種類
typedef enum token_type
{
	variable,  // 変数トークン
	constants, // 定数トークン
	operation, // 演算子トークン
	symbol,	// 記号トークン
} token_type;

// トークンの値
typedef union token_value {
	char name;
	int value;
	char op;
	char symbol;
} token_value;

// トークン
typedef struct token
{
	token_type type;
	token_value value;
	struct token *prev;
	struct token *next;
} token;

#endif