#include <stdio.h>
#include <string.h>
#include "engine.h"

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

		// コードの実行
		engine_run(stream);

		if (mode == input_console)
		{
			printf("> ");
		}
	}

	// リソース開放
	engine_release();

	return 0;
}