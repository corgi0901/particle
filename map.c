#include <malloc.h>
#include <string.h>
#include "map.h"

/**
 * 変数マップ
 */
static Var *var_map = NULL;

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
	Var *item = var_map;
	while (item)
	{
		Var *temp = item;
		item = item->next;
		free(temp);
	}

	Subroutine *sub = sub_map;
	while (sub)
	{
		Subroutine *temp = sub;
		sub = sub->next;
		if (temp->code)
		{
			free(temp->code);
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
Var *createVar(char *name, int value)
{
	Var *new_var = (Var *)calloc(1, sizeof(Var));
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
void addVar(Var *item)
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
Var *getVar(char *name)
{
	for (Var *item = var_map; item != NULL; item = item->next)
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
	sub->code = NULL;
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
	if (sub_map->code == NULL)
	{
		sub_map->code = (char *)calloc(strlen(inst) + 1, sizeof(char));
		if (sub_map->code)
		{
			strcpy(sub_map->code, inst);
		}
	}
	else
	{
		int size = strlen(sub_map->code) + strlen(inst) + 2;
		sub_map->code = (char *)realloc(sub_map->code, size * sizeof(char));
		strcat(sub_map->code, "\n");
		strcat(sub_map->code, inst);
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
