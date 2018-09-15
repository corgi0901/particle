#include <malloc.h>
#include <string.h>
#include "debug.h"
#include "function.h"

/// 関数リスト
typedef struct func_list
{
	Function *functions;
} FuncList;

static FuncList *flist;

void initFuncList(void)
{
	DPRINTF("%s\n", "initFuncList");
	flist = (FuncList *)calloc(1, sizeof(FuncList));
	flist->functions = NULL;
}

void releaseFuncList(void)
{
	DPRINTF("%s\n", "releaseFuncList");

	Function *func = flist->functions;

	while (func)
	{
		Function *temp = func;
		func = func->next;

		// 引数オブジェクトの開放
		ArgList *arg = temp->args;
		while (arg)
		{
			ArgList *arg_temp = arg;
			arg = arg->next;
			free(arg_temp);
		}

		free(temp);
	}

	free(flist);
};

Function *createFunction(char *name, int pc)
{
	DPRINTF("createFunction : name = %s, pc = %d\n", name, pc);

	Function *func = (Function *)calloc(1, sizeof(Function));
	if (!func)
	{
		return NULL;
	}
	func->start_pc = pc;
	strcpy(func->name, name);
	func->args = NULL;
	func->next = NULL;

	return func;
};

void addArgument(Function *func, char *name)
{
	DPRINTF("addArgument : %s\n", name);

	ArgList *new_arg = (ArgList *)calloc(1, sizeof(ArgList));
	if (!new_arg)
	{
		return;
	}
	strcpy(new_arg->name, name);

	if (NULL == func->args)
	{
		func->args = new_arg;
		return;
	}
	else
	{
		ArgList *arg = func->args;
		while (arg->next)
		{
			arg = arg->next;
		}
		arg->next = new_arg;
	}
};

void addFunction(Function *func)
{
	DPRINTF("addFunction : %s\n", func->name);

	if (NULL == flist->functions)
	{
		flist->functions = func;
	}
	else
	{
		func->next = flist->functions;
		flist->functions = func;
	}
};

Function *getFunction(char *name)
{
	DPRINTF("getFunction : %s\n", name);

	for (Function *func = flist->functions; func != NULL; func = func->next)
	{
		if (strcmp(name, func->name) == 0)
		{
			return func;
		}
	}

	return NULL;
};
