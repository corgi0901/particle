#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "debug.h"
#include "mem.h"

/// 変数マップ
typedef struct variable_list
{
	Variable *var;
	struct variable_list *next;
} VariableList;

static int space;
static VariableList *vlist;

void initMemory(void)
{
	DPRINTF("%s\n", "initMemory");
	space = 0;
	vlist = NULL;
};

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

void pushMemorySpace(void)
{
	DPRINTF("%s\n", "pushMemorySpace");
	space++;
};

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

Variable *getVariable(char *name)
{
	DPRINTF("getVariable : %s\n", name);
	VariableList *vars;
	for (vars = vlist; vars; vars = vars->next)
	{
		Variable *var = vars->var;
		if (strcmp(name, var->name) == 0 && var->space == space)
		{
			return var;
		}
	}
	return NULL;
};
