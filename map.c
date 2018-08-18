#include <malloc.h>
#include <string.h>
#include "map.h"

/**
 * 変数マップ
 */
static var *var_map = NULL;

/**
 * @brief 変数マップの初期化
 */
void map_init(void)
{
	var_map = NULL;
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
void *addVar(var *item)
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