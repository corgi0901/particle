#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "program.h"
#include "debug.h"

typedef struct codelist
{
	char *code;
	struct codelist *next;
} CodeList;

typedef struct programMemory
{
	CodeList *head;
	CodeList *tail;
	CodeList *current;
	int pc;
} ProgramMemory;

static ProgramMemory *pmem;

/**
 * @brief プログラムを初期化する
 */
void initProgram(void)
{
	pmem = (ProgramMemory *)calloc(1, sizeof(ProgramMemory));
	pmem->head = NULL;
	pmem->tail = NULL;
	pmem->current = NULL;
	pmem->pc = -1;

	// 空実行文を挿入
	store("");
};

/**
 * @brief プログラムを破棄する
 */
void releaseProgram(void)
{
	for (CodeList *code = pmem->head; code != NULL; code = code->next)
	{
		free(code->code);
		free(code);
	}
	free(pmem);
};

/**
 * @brief プログラムを保存する
 */
void store(char *code)
{
	DPRINTF("store : %s\n", code);

	CodeList *item = (CodeList *)calloc(1, sizeof(CodeList));

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

/**
 * @brief 次に実行されるコードを取得する
 * @return 実行コード
 */
char *fetch(void)
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

/**
 * @brief プログラムカウンタを設定する
 * @param プログラムカウンタ
 */
void jump(int pc)
{
	DPRINTF("jump : %d\n", pc);

	pmem->current = pmem->head;
	for (int i = 0; i < pc; i++)
	{
		pmem->current = pmem->current->next;
	}
	pmem->pc = pc;
};

/**
 * @brief 現在のプログラムカウンタを取得する
 * @return プログラムカウンタの値
 */
int getpc(void)
{
	return pmem->pc;
};
