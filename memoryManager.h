#ifndef _MEMORY_MANAGER_H_
#define _MEMORY_MANAGER_H_

typedef struct variable
{
	char name[64];
	int value;
	int space;
} Variable;

typedef enum
{
	VAR_LOCAL = 0,
	VAR_ARG,
} VAR_TYPE;

void initMemory(void);
void releaseMemory(void);

void pushMemorySpace(void);
void popMemorySpace(void);

void setVariable(char *, int, VAR_TYPE);
Variable *getVariable(char *);

#endif
