#include <stdio.h>
#include <malloc.h>
#include <stack>
#include "context.h"

using namespace std;

/// コンテキスト
typedef struct context
{
	/// プログラムカウンタ
	stack<int> pc;
	/// 実行状態
	stack<int> state;
	/// コードブロック
	stack<int> block;
	/// 下位のコンテキスト
	struct context *next;
} Context;

static Context *context = NULL;

/**
 * @brief コンテキストの初期化
 */
void initContext(void)
{
	pushContext();
};

/**
 * @brief コンテキストの破棄
 */
void releaseContext(void)
{
	while (context)
	{
		popContext();
	}
};

/**
 * @brief 現在のコンテキストの保存
 */
void pushContext(void)
{
	Context *new_context = new Context();

	new_context->next = NULL;

	if (NULL == context)
	{
		context = new_context;
	}
	else
	{
		new_context->next = context;
		context = new_context;
	}
};

/**
 * @brief 現在のコンテキストの破棄
 */
void popContext(void)
{
	Context *peek = context;
	context = context->next;
	delete peek;
};

/**
 * @brief プログラムカウンタのpush
 * @param pc プログラムカウンタの値
 */
void pushPC(int pc)
{
	context->pc.push(pc);
};

/**
 * @brief プログラムカウンタのpop
 * @return プログラムカウンタの値
 */
int popPC(void)
{
	int ret = context->pc.top();
	context->pc.pop();
	return ret;
};

/**
 * @brief 実行状態のpush
 * @param state 実行状態
 */
void pushState(int state)
{
	context->state.push(state);
};

/**
 * @brief 実行状態のpop
 * @return 実行状態
 */
int popState(void)
{
	int ret = context->state.top();
	context->state.pop();
	return ret;
};

/**
 * @brief スタックトップの実行状態の取得
 * @return 実行状態
 */
int peekState(void)
{
	return context->state.top();
};

/**
 * @brief コードブロックのpush
 * @param block コードブロック
 */
void pushBlock(int block)
{
	context->block.push(block);
};

/**
 * @brief コードブロックのpop
 * @return コードブロック
 */
int popBlock(void)
{
	int ret = context->block.top();
	context->block.pop();
	return ret;
};
