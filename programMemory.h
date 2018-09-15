#ifndef _PROGRAM_MEM_H_
#define _PROGRAM_MEM_H_

typedef struct codelist
{
	char *code;
	struct codelist *next;
} Codelist;

typedef struct programMemory
{
	Codelist *head;
	Codelist *tail;
	Codelist *current;
	int pc;
} ProgramMemory;

void initProgramMemory(ProgramMemory *);
void releaseProgramMemory(ProgramMemory *);

void store(ProgramMemory *, char *);
char *fetch(ProgramMemory *);
void jump(ProgramMemory *, int);
int getpc(ProgramMemory *);

#endif