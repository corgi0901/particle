#include <stdio.h>
#include <malloc.h>
#include "context.h"
#include "stack.h"

typedef struct context
{
	Stack *pc;
	Stack *state;
	Stack *block;
	struct context *next;
} Context;

static Context *context = NULL;

void initContext(void)
{
	pushContext();
};

void releaseContext(void)
{
	while (context)
	{
		popContext();
	}
};

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

void pushPC(int pc)
{
	push(context->pc, pc);
};

int popPC(void)
{
	return pop(context->pc);
};

void pushState(int state)
{
	push(context->state, state);
};

int popState(void)
{
	return pop(context->state);
};

int peekState(void)
{
	return peek(context->state);
};

void pushBlock(int block)
{
	push(context->block, block);
};

int popBlock(void)
{
	return pop(context->block);
};
