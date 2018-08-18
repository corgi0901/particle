#ifndef _ENGINE_H_
#define _ENGINE_H_

#include "ast.h"

void engine_init(void);
void engine_release(void);
void engine_exec(ast_node *ast);
void print(char *);

#endif