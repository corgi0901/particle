#ifndef _PROGRAM_MEM_H_
#define _PROGRAM_MEM_H_

void initProgramMemory(void);
void releaseProgramMemory(void);
void store(char *);
char *fetch(void);
void jump(int);
int getpc(void);

#endif