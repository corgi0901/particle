#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "debug.h"
#include "mem.h"
#include "util.h"

/// 変数マップ
typedef struct variable_list
{
	Variable *var;
	struct variable_list *next;
} VariableList;

static int space;
static VariableList *vlist;

/**
 * @brief 内部メモリを初期化する
 */
void initMemory(void)
{
	DPRINTF("%s\n", "initMemory");
	space = 0;
	vlist = NULL;
};

/**
 * @brief 内部メモリを破棄する
 */
void releaseMemory(void)
{
	DPRINTF("%s\n", "releaseMemory");
	VariableList *var = vlist;
	while (var)
	{
		VariableList *temp = var;
		var = var->next;
		free(temp->var);
		free(temp);
	}
};

/**
 * @brief 現在のメモリ空間の保存
 */
void pushMemorySpace(void)
{
	DPRINTF("%s\n", "pushMemorySpace");
	space++;
};

/**
 * @brief 現在のメモリ空間の破棄
 */
void popMemorySpace(void)
{
	DPRINTF("%s\n", "popMemorySpace");
	while (vlist && vlist->var->space == space)
	{
		VariableList *temp = vlist;
		vlist = vlist->next;
		free(temp->var);
		free(temp);
	}
	space--;
};

/**
 * @brief 変数を内部メモリに追加または更新する
 * @param name 変数名
 * @param value 値
 * @param type 変数タイプ
 */
void setVariable(char *name, int value, VAR_TYPE type)
{
	DPRINTF("setVariable : %s = %d\n", name, value);
	if (VAR_LOCAL == type)
	{
		// ローカル変数の場合、既存の変数であればそれを更新する
		Variable *var = getVariable(name);
		if (var)
		{
			var->value = value;
			return;
		}
	}

	// 変数の新規追加
	Variable *var = (Variable *)calloc(1, sizeof(Variable));
	strcpy(var->name, name);
	var->value = value;

	switch (type)
	{
	case VAR_LOCAL:
		var->space = space;
		break;
	case VAR_ARG:
		var->space = space + 1;
		break;
	default:
		var->space = space;
		break;
	}

	VariableList *new_vlist = (VariableList *)calloc(1, sizeof(VariableList));
	new_vlist->var = var;
	new_vlist->next = vlist;

	vlist = new_vlist;
};

/**
 * @brief 内部メモリ中の変数を取得する
 * @param name 変数名
 * @return 変数オブジェクト
 */
Variable *getVariable(char *name)
{
	DPRINTF("getVariable : %s\n", name);
	VariableList *vars;
	for (vars = vlist; vars; vars = vars->next)
	{
		Variable *var = vars->var;
		if (EQ(name, var->name) && var->space == space)
		{
			return var;
		}
	}
	return NULL;
};
