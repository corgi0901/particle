#include <malloc.h>
#include <string.h>
#include "map.h"

/**
 * 変数マップ
 */
static var *var_map = NULL;

/**
 * サブルーチンマップ
 */
static Subroutine *sub_map = NULL;

/**
 * @brief 変数マップの初期化
 */
void map_init(void)
{
	var_map = NULL;
	sub_map = NULL;
};

/**
 * @brief 変数マップの開放
 */
void map_release(void)
{
	var *item = var_map;
	while (item)
	{
		var *temp = item;
		item = item->next;
		free(temp);
	}

	Subroutine *sub = sub_map;
	while (sub)
	{
		Subroutine *temp = sub;
		sub = sub->next;
		if (temp->buf)
		{
			free(temp->buf);
		}
		free(temp);
	}
};

/**
 * @brief 変数オブジェクトの生成
 * @param name 変数名
 * @param value 初期値
 * @retval NULL エラー
 * @retval Other 変数オブジェクトのポインタ
 */
var *createVar(char *name, int value)
{
	var *new_var = (var *)calloc(1, sizeof(var));
	if (!new_var)
	{
		return NULL;
	}
	strcpy(new_var->name, name);
	new_var->value = value;
	new_var->next = NULL;

	return new_var;
};

/**
 * @brief 変数マップへの変数オブジェクトの追加
 * @param item 変数オブジェクト
 */
void addVar(var *item)
{
	if (var_map == NULL)
	{
		var_map = item;
	}
	else
	{
		item->next = var_map;
		var_map = item;
	}
};

/**
 * @brief 変数マップから変数オブジェクトを取得する
 * @param name 変数名
 * @retval NULL 該当する変数オブジェクトがない
 * @retval Other 該当する変数オブジェクトのポインタ
 */
var *getVar(char *name)
{
	for (var *item = var_map; item != NULL; item = item->next)
	{
		if (strcmp(name, item->name) == 0)
		{
			return item;
		}
	}

	return NULL;
};

/**
 * @brief 名前を指定してサブルーチンを生成する
 * @param name サブルーチン名
 * @retval NULL エラー
 * @retval Other サブルーチンオブジェクトのポインタ
 */
Subroutine *createSubroutine(char *name)
{
	Subroutine *sub = (Subroutine *)calloc(1, sizeof(Subroutine));
	if (!sub)
	{
		return NULL;
	}
	sub->buf = NULL;
	strcpy(sub->name, name);
	sub->next = NULL;
	return sub;
};

/**
 * @brief サブルーチンマップにサブルーチンを追加する
 * @param sub サブルーチンオブジェクトのポインタ
 */
void addSubroutine(Subroutine *sub)
{
	if (sub_map == NULL)
	{
		sub_map = sub;
	}
	else
	{
		sub->next = sub_map;
		sub_map = sub;
	}
};

/**
 * @brief マップ先頭のサブルーチンに実行コードを追加する
 * @param inst 実行コード
 */
void addInstruction(char *inst)
{
	if (sub_map->buf == NULL)
	{
		sub_map->buf = (char *)calloc(strlen(inst) + 1, sizeof(char));
		if (sub_map->buf)
		{
			strcpy(sub_map->buf, inst);
		}
	}
	else
	{
		int size = strlen(sub_map->buf) + strlen(inst) + 2;
		sub_map->buf = (char *)realloc(sub_map->buf, size * sizeof(char));
		strcat(sub_map->buf, "\n");
		strcat(sub_map->buf, inst);
	}
};

/**
 * @brief 名前を指定してサブルーチンオブジェクトを取得する
 * @param name サブルーチン名
 * @retval NULL 該当するサブルーチンがない
 * @retval Other 該当するサブルーチン
 */
Subroutine *getSubroutine(char *name)
{
	for (Subroutine *item = sub_map; item != NULL; item = item->next)
	{
		if (strcmp(name, item->name) == 0)
		{
			return item;
		}
	}

	return NULL;
};
