#ifndef _CONTEXT_H_
#define _CONTEXT_H_

void initContext(void);
void releaseContext(void);

void pushContext(void);
void popContext(void);

void pushPC(int);
int popPC(void);

void pushState(int);
int popState(void);
int peekState(void);

void pushBlock(int);
int popBlock(void);

#endif
