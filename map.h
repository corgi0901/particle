#ifndef _MAP_H_
#define _MAP_H_

typedef struct var
{
	char name[64];
	int value;
	struct var *next;
} var;

void map_init(void);
void map_release(void);
var *createVar(char *, int);
void *addVar(var *);
var *getVar(char *);

#endif