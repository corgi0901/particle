#ifndef _ENGINE_H_
#define _ENGINE_H_

#include "ast.h"

typedef enum
{
	/// 実行継続
	RESULT_CONTINUE,
	/// 実行停止
	RESULT_EXIT,
} RESULT;

void engineInit(void);
void engineRelease(void);
RESULT engineRun(char *);

#endif