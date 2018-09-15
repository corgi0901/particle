#ifndef _STACK_H_
#define _STACK_H_

typedef struct elem
{
	int value;
	struct elem *next;
} Elem;

typedef struct stack
{
	Elem *head;
} Stack;

void push(Stack *, int);
int pop(Stack *);
int peek(Stack *);
void printStack(Stack *);

#endif