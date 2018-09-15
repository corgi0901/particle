#include <stdio.h>
#include <malloc.h>
#include "stack.h"
#include "debug.h"

void push(Stack *stack, int value)
{
	DPRINTF("Push : %d\n", value);
	Elem *elem = (Elem *)calloc(1, sizeof(Elem));
	elem->value = value;
	elem->next = stack->head;
	stack->head = elem;
}

int pop(Stack *stack)
{
	Elem *elem = stack->head;
	int value = elem->value;
	stack->head = elem->next;
	free(elem);

	DPRINTF("Pop : %d\n", value);
	return value;
}

int peek(Stack *stack)
{
	return stack->head->value;
};

void printStack(Stack *stack)
{
	printf("=== Stack : %p ===\n", stack);

	int count = 0;
	for (Elem *elem = stack->head; elem != NULL; elem = elem->next)
	{
		printf("%04d : %d\n", count++, elem->value);
	}
}