#include <stdio.h>
#include <string.h>
#include "util.hpp"

/**
 * @brief 文字列がリスト内のいずれかと一致しているかどうか判定する
 * @param str 検索対象の文字列
 * @param count リストの要素数
 * @param ... 文字列リスト
 * @retval false 一致なし
 * @retval true 一致あり
 */
bool _isStrMatch(const char *str, int count, ...)
{
	bool match = false;
	va_list ap;
	va_start(ap, count);

	for (int i = 0; i < count; i++)
	{
		char *item = va_arg(ap, char *);
		if (strcmp(str, item) == 0)
		{
			match = true;
			break;
		}
	}

	return match;
};

/**
 * @brief 文字がリスト内のいずれかと一致しているかどうか判定する
 * @param c 検索対象の文字
 * @param count リストの要素数
 * @param ... 文字リスト
 * @retval false 一致なし
 * @retval true 一致あり
 */
bool _isCharMatch(char c, int count, ...)
{
	bool match = false;
	va_list ap;
	va_start(ap, count);

	for (int i = 0; i < count; i++)
	{
		char item = (char)va_arg(ap, int);
		if (c == item)
		{
			match = true;
			break;
		}
	}

	return match;
};

/**
 * @brief エラーとして文字列を出力する
 * @param message 文字列
 */
void printError(const char *message)
{
	printf("\x1b[1m\x1b[31m%s\x1b[39m\x1b[0m", message);
};
