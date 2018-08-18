#include <string.h>
#include "util.h"

/**
 * @brief 文字列がリスト内のいずれかと一致しているかどうか判定する
 * @param str 検索対象の文字列
 * @param count リストの要素数
 * @param ... 文字列リスト
 * @retval 0 一致なし
 * @retval 1 一致あり
 */
int _isStrMatch(char *str, int count, ...)
{
	int match = 0;
	va_list ap;
	va_start(ap, count);

	for (int i = 0; i < count; i++)
	{
		char *item = va_arg(ap, char *);
		if (strcmp(str, item) == 0)
		{
			match = 1;
			break;
		}
	}

	return match;
}

/**
 * @brief 文字がリスト内のいずれかと一致しているかどうか判定する
 * @param c 検索対象の文字
 * @param count リストの要素数
 * @param ... 文字リスト
 * @retval 0 一致なし
 * @retval 1 一致あり
 */
int _isCharMatch(char c, int count, ...)
{
	int match = 0;
	va_list ap;
	va_start(ap, count);

	for (int i = 0; i < count; i++)
	{
		char item = (char)va_arg(ap, int);
		if (c == item)
		{
			match = 1;
			break;
		}
	}

	return match;
}