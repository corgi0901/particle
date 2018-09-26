#include <stdio.h>
#include <malloc.h>
#include "stack.h"
#include "debug.h"

/**
 * @brief スタックのpush
 * @param stack スタック
 * @param value pushする値
 */
void push(Stack *stack, int value)
{
	DPRINTF("Push : %d\n", value);
	Elem *elem = (Elem *)calloc(1, sizeof(Elem));
	elem->value = value;
	elem->next = stack->head;
	stack->head = elem;
};

/**
 * @brief スタックのpop
 * @param stack スタック
 * @return popした値
 */
int pop(Stack *stack)
{
	Elem *elem = stack->head;
	int value = elem->value;
	stack->head = elem->next;
	free(elem);

	DPRINTF("Pop : %d\n", value);
	return value;
};

/**
 * @brief スタックトップの値を取得する（popはしない）
 * @param stack スタック
 * @param value スタックトップの値
 */
int peek(Stack *stack)
{
	return stack->head->value;
};

/**
 * @brief スタックの中身を表示する（デバッグ用）
 * @param stack スタック
 */
void printStack(Stack *stack)
{
	printf("=== Stack : %p ===\n", stack);

	int count = 0;
	for (Elem *elem = stack->head; elem != NULL; elem = elem->next)
	{
		printf("%04d : %d\n", count++, elem->value);
	}
};
