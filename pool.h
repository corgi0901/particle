#ifndef _POOL_H_
#define _POOL_H_

typedef struct codelist
{
	char *code;
	struct codelist *next;
} Codelist;

typedef struct codePool
{
	Codelist *head;
	Codelist *tail;
	int size;
} CodePool;

void addCode(CodePool *, char *);
char *getCode(CodePool *, int);
void releasePool(CodePool *);

#endif