#ifndef _ENGINE_H_
#define _ENGINE_H_

typedef enum
{
	/// 実行成功
	RESULT_OK,
	/// 実行エラー
	RESULT_ERROR,
	/// 実行停止
	RESULT_EXIT,
} ENGINE_RESULT;

void initEngine(void);
void releaseEngine(void);
ENGINE_RESULT runEngine(char *);

#endif
