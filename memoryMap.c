#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "debug.h"
#include "memoryMap.h"

void initVarMapStack(VarMapStack *vms)
{
	DPRINTF("%s\n", "variableMapInit");
	VariableMap *map = createVariableMap();
	vms->map = map;
};

void releaseVarMapStack(VarMapStack *vms)
{
	DPRINTF("%s\n", "variableMapRelease");
	VariableMap *map = vms->map;
	while (map)
	{
		VariableMap *temp_map = map->next;
		releaseVariableMap(map);
		map = temp_map;
	}
}

void pushVariableMap(VarMapStack *vms, VariableMap *map)
{
	DPRINTF("%s\n", "pushVariableMap");
	if (NULL == vms->map)
	{
		vms->map = map;
	}
	else
	{
		map->next = vms->map;
		vms->map = map;
	}
};

VariableMap *popVariableMap(VarMapStack *vms)
{
	DPRINTF("%s\n", "popVariableMap");
	VariableMap *top = vms->map;
	vms->map = vms->map->next;
	return top;
};

VariableMap *createVariableMap(void)
{
	VariableMap *map = (VariableMap *)calloc(1, sizeof(VariableMap));
	map->vars = NULL;
	map->next = NULL;
	return map;
};

void releaseVariableMap(VariableMap *map)
{
	Variable *var = map->vars;
	while (var)
	{
		Variable *temp = var->next;
		free(var);
		var = temp;
	}
};

Variable *addVariable(VariableMap *map, char *name, int value)
{
	DPRINTF("addVariable : %s = %d\n", name, value);

	Variable *var = (Variable *)calloc(1, sizeof(Variable));
	strcpy(var->name, name);
	var->value = value;
	var->next = NULL;

	if (NULL == map->vars)
	{
		map->vars = var;
	}
	else
	{
		var->next = map->vars;
		map->vars = var;
	}

	return var;
};

Variable *getOrCreateVariable(VarMapStack *vms, char *name)
{
	DPRINTF("getOrCreateVariable : %s\n", name);

	Variable *var;
	for (var = vms->map->vars; var; var = var->next)
	{
		if (strcmp(name, var->name) == 0)
		{
			return var;
		}
	}

	var = addVariable(vms->map, name, 0);

	return var;
}