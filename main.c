#include <stdio.h>
#include <string.h>
#include <memory.h>
#include "lexer.h"
#include "token.h"
#include "ast.h"
#include "engine.h"

static void printTokens(token *tk)
{
	for (token *t = tk; t != NULL; t = t->next)
	{
		switch (t->type)
		{
		case variable:
			printf("variable : %c\n", t->value.name);
			break;
		case constants:
			printf("constants : %d\n", t->value.value);
			break;
		case operation:
			printf("operation : %c\n", t->value.op);
			break;
		case symbol:
			printf("symbol : %c\n", t->value.symbol);
			break;
		}
	}
};

static void printAst(ast_node *tree, int depth)
{
	if (tree == NULL)
	{
		return;
	}

	for (int i = 0; i < depth; i++)
	{
		printf("    ");
	}

	switch (tree->root->type)
	{
	case variable:
		printf("%c\n", tree->root->value.name);
		break;
	case constants:
		printf("%d\n", tree->root->value.value);
		break;
	case operation:
		printf("%c\n", tree->root->value.op);
		break;
	}

	printAst(tree->left, depth + 1);
	printAst(tree->right, depth + 1);
}

int main(void)
{
	char stream[256];

	engine_init();

	while (1)
	{
		printf("> ");

		memset(stream, 0, sizeof(stream));
		if (!fgets(stream, sizeof(stream), stdin))
		{
			printf("Failed to read console\n");
			break;
		}
		stream[strlen(stream) - 1] = '\0';

		if (strcmp(stream, "exit") == 0)
		{
			break;
		}
		else if (strstr(stream, "print "))
		{
			char c = stream[strlen("print ")];
			printf("%d\n", memory[c - 'a']);
		}
		else
		{
			token *tokens = tokenize(stream);
			ast_node *ast = createAst(tokens);
			engine_exec(ast);
			releaseAst(ast);
		}
	}

	return 0;
}