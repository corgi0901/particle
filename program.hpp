#ifndef _PROGRAM_HPP_
#define _PROGRAM_HPP_

void initProgram(void);
void releaseProgram(void);
void store(const char *);
char *fetch(void);
void jump(int);
int getpc(void);

#endif
