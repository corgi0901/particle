#include <stdio.h>
#include "checker.hpp"
#include "util.hpp"

#define checkNextTokenType(x, ...)                  \
	_checkNextTokenType(                            \
		x,                                          \
		sizeof((int[]){__VA_ARGS__}) / sizeof(int), \
		__VA_ARGS__)

typedef bool (*CHECKER_FUNC)(Token *);

/**
 * @brief トークンの値を表示する
 * @param token トークン
 */
static void printTokenValue(Token *token)
{
	switch (token->type)
	{
	case TK_VARIABLE:
		printf("\"%s\"", token->value.string);
		break;
	case TK_NUMBER:
		printf("\"%d\"", token->value.number);
		break;
	case TK_OPERATION:
		printf("\"%s\"", token->value.string);
		break;
	case TK_UNARY_OP:
		printf("\"%s\"", token->value.string);
		break;
	case TK_LEFT_BK:
		printf("\"%c\"", token->value.charactor);
		break;
	case TK_RIGHT_BK:
		printf("\"%c\"", token->value.charactor);
		break;
	case TK_FUNCTION:
		printf("\"%s\"", token->value.string);
		break;
	case TK_KEYWORD:
		printf("\"%s\"", token->value.string);
		break;
	default:
		break;
	}
};

/**
 * @brief 次のトークンが受け入れ可能なトークンかどうかを判定する
 * @param tokens トークン
 * @param count 第3引数の個数
 * @param ... トークン種別の列挙
 * @return 判定結果
 */
static bool _checkNextTokenType(Token *tokens, int count, ...)
{
	Token *next = tokens->next;
	if (NULL == next)
	{
		return true;
	}

	va_list ap;
	va_start(ap, count);

	for (int i = 0; i < count; i++)
	{
		TOKEN_TYPE type = (TOKEN_TYPE)va_arg(ap, int);
		if (type == next->type)
		{
			return true;
		}
	}

	printError("error : ");
	printTokenValue(tokens->next);
	printf(" is unexpected token\n");

	return false;
};

/**
 * @brief 次のトークンが存在するかどうかを判定する
 * @param tokens トークン
 * @return 判定結果
 */
static bool hasNextToken(Token *tokens)
{
	if (tokens->next)
	{
		return true;
	}
	else
	{
		printError("error : ");
		printf("any token missing after ");
		printTokenValue(tokens);
		printf("\n");

		return false;
	}
}

/**
 * @brief 末尾のトークンかどうかを判定する
 * @param tokens トークン
 * @return 判定結果
 */
static bool isLastToken(Token *tokens)
{
	if (NULL == tokens->next)
	{
		return true;
	}
	else
	{
		printError("error : ");
		printf("any token can't exist after ");
		printTokenValue(tokens);
		printf("\n");
		return false;
	}
}

/**
 * @brief 変数トークンに対する構文チェック処理
 * @param tokens トークン
 * @return 判定結果
 */
static bool caseVariable(Token *tokens)
{
	return checkNextTokenType(tokens, TK_OPERATION, TK_RIGHT_BK);
};

/**
 * @brief 定数トークンに対する構文チェック処理
 * @param tokens トークン
 * @return 判定結果
 */
static bool caseNumber(Token *tokens)
{
	return checkNextTokenType(tokens, TK_OPERATION, TK_RIGHT_BK);
};

/**
 * @brief 算術演算子トークンに対する構文チェック処理
 * @param tokens トークン
 * @return 判定結果
 */
static bool caseOperation(Token *tokens)
{
	return hasNextToken(tokens) && checkNextTokenType(tokens, TK_VARIABLE, TK_NUMBER, TK_UNARY_OP, TK_LEFT_BK, TK_FUNCTION);
};

/**
 * @brief 単項演算子トークンに対する構文チェック処理
 * @param tokens トークン
 * @return 判定結果
 */
static bool caseUnaryOperation(Token *tokens)
{
	return checkNextTokenType(tokens, TK_VARIABLE, TK_NUMBER, TK_LEFT_BK, TK_FUNCTION);
};

/**
 * @brief 左括弧トークンに対する構文チェック処理
 * @param tokens トークン
 * @return 判定結果
 */
static bool caseLeftBracket(Token *tokens)
{
	if (false == hasNextToken(tokens))
	{
		return false;
	}

	if (false == checkNextTokenType(tokens, TK_VARIABLE, TK_NUMBER, TK_UNARY_OP, TK_LEFT_BK, TK_RIGHT_BK, TK_FUNCTION, TK_KEYWORD))
	{
		return false;
	}

	// 対応する右括弧のチェック
	bool ret = false;
	int depth = 1;
	for (Token *token = tokens->next; token; token = token->next)
	{
		if (TK_LEFT_BK == token->type)
		{
			depth++;
		}
		else if (TK_RIGHT_BK == token->type)
		{
			depth--;
		}

		if (depth == 0)
		{
			ret = true;
			break;
		}
	}

	if (false == ret)
	{
		printError("error : ");
		printf("missing \")\" corresponding to \"(\"\n");
	}

	return ret;
};

/**
 * @brief 右括弧トークンに対する構文チェック処理
 * @param tokens トークン
 * @return 判定結果
 */
static bool caseRightBracket(Token *tokens)
{
	return checkNextTokenType(tokens, TK_OPERATION, TK_RIGHT_BK);
};

/**
 * @brief 関数トークンに対する構文チェック処理
 * @param tokens トークン
 * @return 判定結果
 */
static bool caseFunction(Token *tokens)
{
	if (false == hasNextToken(tokens))
	{
		return false;
	}

	if (false == checkNextTokenType(tokens, TK_LEFT_BK))
	{
		return false;
	}

	// 直前に"func"がある場合
	Token *prev = tokens->prev;
	if (prev && TK_KEYWORD == prev->type && EQ(prev->value.string, "func"))
	{
		// 末尾のトークン
		Token *last = getLastToken(tokens);
		if (TK_RIGHT_BK != last->type)
		{
			printError("error : ");
			printf("In this line, any token can't exist after \")\"\n");
			return false;
		}

		for (Token *token = tokens->next->next; token != last; token = token->next)
		{
			if (TK_VARIABLE != token->type && !(TK_OPERATION == token->type && EQ(token->value.string, ",")))
			{
				printError("error : ");
				printTokenValue(token);
				printf(" is unexpected token\n");
				return false;
			}
		}
	}

	return true;
};

/**
 * @brief 予約語トークンに対する構文チェック処理
 * @param tokens トークン
 * @return 判定結果
 */
static bool caseKeyword(Token *tokens)
{
	char *keyword = tokens->value.string;

	if (EQ(keyword, "func"))
	{
		return hasNextToken(tokens) && checkNextTokenType(tokens, TK_FUNCTION);
	}
	else if (EQ(keyword, "end"))
	{
		return isLastToken(tokens);
	}
	else if (EQ(keyword, "return"))
	{
		return checkNextTokenType(tokens, TK_VARIABLE, TK_NUMBER, TK_UNARY_OP, TK_LEFT_BK, TK_FUNCTION);
	}
	else if (EQ(keyword, "if") || EQ(keyword, "while"))
	{
		Token *last = getLastToken(tokens);
		if (TK_RIGHT_BK != last->type)
		{
			printError("error : ");
			printf("In this line, any token can't exist after \")\"\n");
			return false;
		}

		return hasNextToken(tokens) && checkNextTokenType(tokens, TK_LEFT_BK);
	}
	else if (EQ(keyword, "else"))
	{
		return isLastToken(tokens);
	}

	return true;
};

/// 構文チェック関数テーブル
static CHECKER_FUNC checker_func_table[] = {
	caseVariable,
	caseNumber,
	caseOperation,
	caseUnaryOperation,
	caseLeftBracket,
	caseRightBracket,
	caseFunction,
	caseKeyword,
};

/**
 * @brief トークン列全体に対する構文チェック
 * @param tokens トークン列
 * @retval true OK
 * @retval false NG
 */
bool isCorrectTokens(Token *tokens)
{
	for (Token *tk = tokens; tk; tk = tk->next)
	{
		CHECKER_FUNC checker = checker_func_table[tk->type];
		if (false == checker(tk))
		{
			return false;
		}
	}
	return true;
};
