#ifndef _AST_H_
#define _AST_H_

#include "token.h"

/// 抽象構文木の節
typedef struct ast_node
{
	/// 節
	Token *root;
	/// 左の葉
	struct ast_node *left;
	/// 右の葉
	struct ast_node *right;
} Ast;

Ast *createAst(Token *);
void releaseAst(Ast *);
void printAst(Ast *, int);

#endif
