#ifndef _PROGRAM_H_
#define _PROGRAM_H_

void initProgram(void);
void releaseProgram(void);
void store(const char *);
char *fetch(void);
void jump(int);
int getpc(void);

#endif
