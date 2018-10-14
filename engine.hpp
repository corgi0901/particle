#ifndef _ENGINE_HPP_
#define _ENGINE_HPP_

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
bool isWaitEnd(void);

#endif
