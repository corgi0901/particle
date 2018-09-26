#include <stdio.h>
#include <string.h>
#include "util.h"

/**
 * @brief 文字列がリスト内のいずれかと一致しているかどうか判定する
 * @param str 検索対象の文字列
 * @param count リストの要素数
 * @param ... 文字列リスト
 * @retval FALSE 一致なし
 * @retval TRUE 一致あり
 */
BOOL _isStrMatch(char *str, int count, ...)
{
	BOOL match = FALSE;
	va_list ap;
	va_start(ap, count);

	for (int i = 0; i < count; i++)
	{
		char *item = va_arg(ap, char *);
		if (strcmp(str, item) == 0)
		{
			match = TRUE;
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
 * @retval FALSE 一致なし
 * @retval TRUE 一致あり
 */
BOOL _isCharMatch(char c, int count, ...)
{
	BOOL match = FALSE;
	va_list ap;
	va_start(ap, count);

	for (int i = 0; i < count; i++)
	{
		char item = (char)va_arg(ap, int);
		if (c == item)
		{
			match = TRUE;
			break;
		}
	}

	return match;
};

/**
 * @brief エラーとして文字列を出力する
 * @param message 文字列
 */
void printError(char *message)
{
	printf("\x1b[1m\x1b[31m%s\x1b[39m\x1b[0m", message);
};
