#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "token.h"
#include "ast.h"

void printAst(ast_node *, int);
void printTokens(token *);

#endif