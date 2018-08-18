#ifndef _AST_H_
#define _AST_H_

#include "token.h"

typedef struct ast_node
{
	token *root;
	struct ast_node *left;
	struct ast_node *right;
} ast_node;

ast_node *createAst(token *);
void releaseAst(ast_node *);

#endif