#include <stdio.h>
#include <string.h>
#include "engine.h"

typedef enum
{
	MODE_CONSOLE = 0, // 標準入力
	MODE_FILE		  // ファイル入力
} INPUT_MODE;

int main(int argc, char *argv[])
{
	INPUT_MODE mode;
	FILE *fp;

	char stream[256];
	memset(stream, 0, sizeof(stream));

	// 初期化
	initEngine();

	if (argc == 1)
	{
		mode = MODE_CONSOLE;
		fp = stdin;
		printf(">>> ");
	}
	else
	{
		mode = MODE_FILE;
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

		// コードの実行
		ENGINE_RESULT ret = runEngine(stream);
		if (ret == RESULT_EXIT)
		{
			break;
		}

		if (mode == MODE_CONSOLE)
		{
			if (isWaitEnd())
			{
				printf("... ");
			}
			else
			{
				printf(">>> ");
			}
		}
	}

	// リソース開放
	releaseEngine();

	if (mode == MODE_FILE)
	{
		fclose(fp);
	}

	return 0;
};
