#ifndef _AST_H_
#define _AST_H_

#include "token.h"

typedef struct ast_node
{
	Token *root;
	struct ast_node *left;
	struct ast_node *right;
} Ast;

Ast *createAst(Token *);
void releaseAst(Ast *);

// デバッグ用
void printAst(Ast *, int);

#endif
