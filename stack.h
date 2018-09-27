#ifndef _STACK_H_
#define _STACK_H_

/// スタックの要素
typedef struct elem
{
	/// 値
	int value;
	/// 次の要素
	struct elem *next;
} Elem;

/// スタック
typedef struct stack
{
	/// トップの要素
	Elem *head;
} Stack;

void push(Stack *, int);
int pop(Stack *);
int peek(Stack *);
void printStack(Stack *);

#endif
