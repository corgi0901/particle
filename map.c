#include <malloc.h>
#include <string.h>
#include "map.h"

/// サブルーチンマップ
static Function *func_map = NULL;

/**
 * @brief 変数マップに変数オブジェクトを追加する
 * @param map 変数マップ
 * @param var 変数オブジェクト
 */
void addVar(VarMap *map, Var *var)
{
	if (map->vars == NULL)
	{
		map->vars = var;
	}
	else
	{
		var->next = map->vars;
		map->vars = var;
	}
};

/**
 * @brief 変数マップから指定した名前の変数オブジェクトを取得する
 * @param map 変数マップ
 * @param name 変数名
 * @retval NULL 該当する変数がない
 * @retval Other 変数オブジェクトのポインタ
 */
Var *getVar(VarMap *map, char *name)
{
	for (Var *var = map->vars; var != NULL; var = var->next)
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
void clearMap(VarMap **map)
{
	if (map == NULL)
	{
		return;
	}

	Var *var = (*map)->vars;
	while (var)
	{
		Var *temp = var;
		var = var->next;
		free(temp);
	}
	free(*map);
};

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
 * @brief 変数マップを生成する
 */
VarMap *createVarMap(void)
{
	VarMap *map = (VarMap *)calloc(1, sizeof(VarMap));
	return map;
};

/**
 * @brief 名前を指定してサブルーチンを生成する
 * @param name サブルーチン名
 * @retval NULL エラー
 * @retval Other サブルーチンオブジェクトのポインタ
 */
Function *createFunction(char *name)
{
	Function *sub = (Function *)calloc(1, sizeof(Function));
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
void addFunction(Function *sub)
{
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
 * @brief マップ先頭のサブルーチンに実行コードを追加する
 * @param inst 実行コード
 */
void addInstruction(char *inst)
{
	if (func_map->code == NULL)
	{
		func_map->code = (char *)calloc(strlen(inst) + 1, sizeof(char));
		if (func_map->code)
		{
			strcpy(func_map->code, inst);
		}
	}
	else
	{
		int size = strlen(func_map->code) + strlen(inst) + 2;
		func_map->code = (char *)realloc(func_map->code, size * sizeof(char));
		strcat(func_map->code, "\n");
		strcat(func_map->code, inst);
	}
};

/**
 * @brief 名前を指定してサブルーチンオブジェクトを取得する
 * @param name サブルーチン名
 * @retval NULL 該当するサブルーチンがない
 * @retval Other 該当するサブルーチン
 */
Function *getFunction(char *name)
{
	for (Function *var = func_map; var != NULL; var = var->next)
	{
		if (strcmp(name, var->name) == 0)
		{
			return var;
		}
	}

	return NULL;
};
