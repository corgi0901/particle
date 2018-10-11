#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdarg.h>
#include <string.h>
#include "particle.h"

/**
 * @brief 文字列比較マクロ
 */
#define EQ(str1, str2) (strcmp(str1, str2) == 0)

/**
 * @brief _isStrMatch関数のラッパー
 */
#define isStrMatch(x, ...)                                      \
	_isStrMatch(                                                \
		x,                                                      \
		sizeof((const char *[]){__VA_ARGS__}) / sizeof(char *), \
		__VA_ARGS__)

/**
 * @brief _isCharMatch関数のラッパー
 */
#define isCharMatch(x, ...)                                 \
	_isCharMatch(                                           \
		x,                                                  \
		sizeof((const char[]){__VA_ARGS__}) / sizeof(char), \
		__VA_ARGS__)

BOOL _isStrMatch(const char *, int, ...);
BOOL _isCharMatch(char, int, ...);
void printError(const char *);

#endif
