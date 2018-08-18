#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdarg.h>

/**
 * @brief _isStrMatch関数のラッパー
 */
#define isStrMatch(x, ...)                                \
	_isStrMatch(                                          \
		x,                                                \
		sizeof((char *[]){__VA_ARGS__}) / sizeof(char *), \
		__VA_ARGS__)

/**
 * @brief _isCharMatch関数のラッパー
 */
#define isCharMatch(x, ...)                           \
	_isCharMatch(                                     \
		x,                                            \
		sizeof((char[]){__VA_ARGS__}) / sizeof(char), \
		__VA_ARGS__)

int _isStrMatch(char *, int, ...);
int _isCharMatch(char, int, ...);

#endif