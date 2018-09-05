#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "pool.h"
#include "debug.h"

void addCode(CodePool *pool, char *line)
{
	char *top = line;

	// 先頭の空白は削除
	while (*top == ' ' || *top == '\t')
	{
		top++;
	}

	Codelist *code = (Codelist *)calloc(1, sizeof(Codelist));

	code->code = (char *)calloc(strlen(top) + 1, sizeof(char));
	strcpy(code->code, top);

	if (pool->head == NULL)
	{
		pool->head = code;
		pool->tail = code;
	}
	else
	{
		pool->tail->next = code;
		pool->tail = code;
	}

	pool->size++;
};

char *getCode(CodePool *pool, int index)
{
	Codelist *code = pool->head;

	for (int i = 0; i < index; i++)
	{
		code = code->next;
	}

	DPRINTF("%03d : %s\n", index, code->code);

	return code->code;
}