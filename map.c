#include <malloc.h>
#include <string.h>
#include "map.h"

/// サブルーチンマップ
static Subroutine *sub_map = NULL;

/**
 * @brief 変数マップに変数オブジェクトを追加する
 * @param map 変数マップ
 * @param var 変数オブジェクト
 */
void addVar(Var **map, Var *var)
{
	if (*map == NULL)
	{
		*map = var;
	}
	else
	{
		var->next = *map;
		*map = var;
	}
};

/**
 * @brief 変数マップから指定した名前の変数オブジェクトを取得する
 * @param map 変数マップ
 * @param name 変数名
 * @retval NULL 該当する変数がない
 * @retval Other 変数オブジェクトのポインタ
 */
Var *getVar(Var *map, char *name)
{
	for (Var *var = map; var != NULL; var = var->next)
	{
		if (strcmp(name, var->name) == 0)
		{
			return var;
		}
	}

	return NULL;
};

/**
 * @brief 変数マップを開放する
 * @param map 変数マップ
 */
void clearMap(Var **map)
{
	if (map == NULL)
	{
		return;
	}

	Var *var = *map;
	while (var)
	{
		Var *temp = var;
		var = var->next;
		free(temp);
	}
	*map = NULL;
};

/**
 * @brief 変数マップの初期化
 */
void map_init(void)
{
	sub_map = NULL;
};

/**
 * @brief 変数マップの開放
 */
void map_release(void)
{
	Subroutine *sub = sub_map;
	while (sub)
	{
		Subroutine *temp = sub;
		sub = sub->next;

		// 実行コードの開放
		if (temp->code)
		{
			free(temp->code);
		}

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
	strcpy(sub->name, name);
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
 * @brief マップ先頭のサブルーチンに引数情報を追加する
 * @param name 引数
 */
void addArg(char *name)
{
	Subroutine *sub = sub_map;
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
	for (Subroutine *var = sub_map; var != NULL; var = var->next)
	{
		if (strcmp(name, var->name) == 0)
		{
			return var;
		}
	}

	return NULL;
};
