#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include "stack.h"
#include "debug.h"

void push(Stack *stack, int value)
{
	DPRINTF("\033[32m----- Push : %3d -----\033[39m\n", value);
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

	DPRINTF("\033[33m----- Pop : %3d -----\033[39m\n", value);
	return value;
}
