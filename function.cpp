#include <malloc.h>
#include <string.h>
#include "debug.hpp"
#include "function.hpp"

/// 関数リスト
typedef struct func_list
{
	Function *functions;
} FuncList;

static FuncList *flist;

/**
 * @brief 関数リストを初期化する
 */
void initFuncList(void)
{
	DPRINTF("%s\n", "initFuncList");
	flist = (FuncList *)calloc(1, sizeof(FuncList));
	flist->functions = NULL;
};

/**
 * @brief 関数リストを破棄する
 */
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

/**
 * @brief 関数オブジェクトを作成する
 * @param name 関数名
 * @param pc 関数の開始番地（プログラムカウンタ）
 * @return 関数オブジェクト
 */
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

/**
 * @brief 関数オブジェクトに引数の定義を追加する
 * @param func 関数オブジェクト
 * @param name 引数名
 */
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

/**
 * @brief 関数リストに関数を追加する
 * @param func 追加する関数
 */
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

/**
 * @brief 指定した関数を取得する
 * @param name 関数名
 * @return 関数オブジェクト
 */
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
