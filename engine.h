#ifndef _ENGINE_H_
#define _ENGINE_H_

#include "ast.h"

int memory[26];

void engine_init(void);
void engine_exec(ast_node *ast);
void printMemory(void);

#endif