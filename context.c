#include <stdio.h>
#include <malloc.h>
#include "context.h"
#include "stack.h"

/// コンテキスト
typedef struct context
{
	/// プログラムカウンタ
	Stack *pc;
	/// 実行状態
	Stack *state;
	/// コードブロック
	Stack *block;
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
	Context *new_context;
	new_context = (Context *)calloc(1, sizeof(Context));
	new_context->pc = (Stack *)calloc(1, sizeof(Stack));
	new_context->state = (Stack *)calloc(1, sizeof(Stack));
	new_context->block = (Stack *)calloc(1, sizeof(Stack));
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

	Elem *elem = peek->pc->head;
	while (elem)
	{
		Elem *temp = elem;
		elem = elem->next;
		free(temp);
	}
	free(peek->pc);

	elem = peek->state->head;
	while (elem)
	{
		Elem *temp = elem;
		elem = elem->next;
		free(temp);
	}
	free(peek->state);

	elem = peek->block->head;
	while (elem)
	{
		Elem *temp = elem;
		elem = elem->next;
		free(temp);
	}
	free(peek->block);

	free(peek);
};

/**
 * @brief プログラムカウンタのpush
 * @param pc プログラムカウンタの値
 */
void pushPC(int pc)
{
	push(context->pc, pc);
};

/**
 * @brief プログラムカウンタのpop
 * @return プログラムカウンタの値
 */
int popPC(void)
{
	return pop(context->pc);
};

/**
 * @brief 実行状態のpush
 * @param state 実行状態
 */
void pushState(int state)
{
	push(context->state, state);
};

/**
 * @brief 実行状態のpop
 * @return 実行状態
 */
int popState(void)
{
	return pop(context->state);
};

/**
 * @brief スタックトップの実行状態の取得
 * @return 実行状態
 */
int peekState(void)
{
	return peek(context->state);
};

/**
 * @brief コードブロックのpush
 * @param block コードブロック
 */
void pushBlock(int block)
{
	push(context->block, block);
};

/**
 * @brief コードブロックのpop
 * @return コードブロック
 */
int popBlock(void)
{
	return pop(context->block);
};
