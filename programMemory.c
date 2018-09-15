#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "programMemory.h"
#include "debug.h"

void initProgramMemory(ProgramMemory *pmem)
{
	pmem->head = NULL;
	pmem->tail = NULL;
	pmem->current = NULL;
	pmem->pc = -1;
};

void releaseProgramMemory(ProgramMemory *pmem)
{
	for (Codelist *code = pmem->head; code != NULL; code = code->next)
	{
		free(code->code);
		free(code);
	}
};

void store(ProgramMemory *pmem, char *code)
{
	DPRINTF("store : %s\n", code);

	Codelist *item = (Codelist *)calloc(1, sizeof(Codelist));

	item->code = (char *)calloc(strlen(code) + 1, sizeof(char));
	strcpy(item->code, code);

	if (pmem->head == NULL)
	{
		pmem->head = item;
		pmem->tail = item;
	}
	else
	{
		pmem->tail->next = item;
		pmem->tail = item;
	}
};

char *fetch(ProgramMemory *pmem)
{
	if (pmem->current == pmem->tail)
	{
		return NULL;
	}

	if (NULL == pmem->current)
	{
		pmem->current = pmem->head;
		pmem->pc = 0;
	}
	else
	{
		pmem->current = pmem->current->next;
		pmem->pc += 1;
	}

	DPRINTF("fetch : %s\n", pmem->current->code);

	return pmem->current->code;
};

void jump(ProgramMemory *pmem, int pc)
{
	DPRINTF("jump : %d\n", pc);

	pmem->current = pmem->head;
	for (int i = 0; i < pc; i++)
	{
		pmem->current = pmem->current->next;
	}
	pmem->pc = pc;
};

int getpc(ProgramMemory *pmem)
{
	return pmem->pc;
};
