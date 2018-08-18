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
			printf("variable : %s\n", t->value.name);
			break;
		case constants:
			printf("constants : %d\n", t->value.value);
			break;
		case operation:
			printf("operation : %s\n", t->value.op);
			break;
		case symbol:
			printf("symbol : %c\n", t->value.symbol);
			break;
		case function:
			printf("function : %s\n", t->value.func);
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
		printf("%s\n", tree->root->value.name);
		break;
	case constants:
		printf("%d\n", tree->root->value.value);
		break;
	case operation:
		printf("%s\n", tree->root->value.op);
		break;
	case unary_operation:
		printf("%s\n", tree->root->value.op);
		break;
	case function:
		printf("%s\n", tree->root->value.func);
		break;
	}

	printAst(tree->left, depth + 1);
	printAst(tree->right, depth + 1);
}

typedef enum
{
	// 標準入力
	input_console = 0,
	// ファイル入力
	input_file
} input_type;

int main(int argc, char *argv[])
{
	input_type mode;
	FILE *fp;

	char stream[256];
	memset(stream, 0, sizeof(stream));

	// 初期化
	engine_init();

	if (argc == 1)
	{
		mode = input_console;
		fp = stdin;
		printf("> ");
	}
	else
	{
		mode = input_file;
		fp = fopen(argv[1], "r");
		if (!fp)
		{
			printf("Failed to open : %s\n", argv[1]);
			return 1;
		}
	}

	while (fgets(stream, sizeof(stream), fp))
	{
		stream[strlen(stream) - 1] = '\0'; // 末尾の改行コードを削除

		if (strcmp(stream, "exit") == 0)
		{
			break;
		}
		else
		{
			token *tokens = tokenize(stream);
			if (tokens)
			{
				ast_node *ast = createAst(tokens);
				engine_exec(ast);
				releaseAst(ast);
			}
		}

		if (mode == input_console)
		{
			printf("> ");
		}
	}

	// リソース開放
	engine_release();

	return 0;
}