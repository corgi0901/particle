#include <malloc.h>
#include <string.h>
#include "debug.h"
#include "map.h"

/// サブルーチンマップ
static Function *func_map = NULL;

/**
 * @brief 変数マップの初期化
 */
void mapInit(void)
{
	func_map = NULL;
};

/**
 * @brief 変数マップの開放
 */
void mapRelease(void)
{
	Function *sub = func_map;
	while (sub)
	{
		Function *temp = sub;
		sub = sub->next;

		// 引数オブジェクトの開放
		Arg *arg = temp->args;
		while (arg)
		{
			Arg *arg_temp = arg;
			arg = arg->next;
			free(arg_temp);
		}

		free(temp);
	}
};

/**
 * @brief 名前を指定してサブルーチンを生成する
 * @param name サブルーチン名
 * @param pc プログラム開始位置
 * @retval NULL エラー
 * @retval Other サブルーチンオブジェクトのポインタ
 */
Function *createFunction(char *name, int pc)
{
	Function *sub = (Function *)calloc(1, sizeof(Function));
	if (!sub)
	{
		return NULL;
	}
	strcpy(sub->name, name);
	sub->start_pc = pc;
	return sub;
};

/**
 * @brief サブルーチンマップにサブルーチンを追加する
 * @param sub サブルーチンオブジェクトのポインタ
 */
void addFunction(Function *sub)
{
	DPRINTF("addFunction : %s\n", sub->name);
	if (func_map == NULL)
	{
		func_map = sub;
	}
	else
	{
		sub->next = func_map;
		func_map = sub;
	}
};

/**
 * @brief マップ先頭のサブルーチンに引数情報を追加する
 * @param name 引数
 */
void addArg(char *name)
{
	Function *sub = func_map;
	Arg *arg = (Arg *)calloc(1, sizeof(Arg));
	if (!arg)
	{
		return;
	}
	strcpy(arg->name, name);

	Arg *temp = sub->args;

	if (!temp)
	{
		sub->args = arg;
		return;
	}

	while (temp->next)
	{
		temp = temp->next;
	}

	temp->next = arg;
};

/**
 * @brief 名前を指定してサブルーチンオブジェクトを取得する
 * @param name サブルーチン名
 * @retval NULL 該当するサブルーチンがない
 * @retval Other 該当するサブルーチン
 */
Function *getFunction(char *name)
{
	DPRINTF("getFunction : %s\n", name);

	for (Function *var = func_map; var != NULL; var = var->next)
	{
		if (strcmp(name, var->name) == 0)
		{
			return var;
		}
	}

	return NULL;
};
