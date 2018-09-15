#ifndef _FUNCTION_H_
#define _FUNCTION_H_

/// 引数リスト
typedef struct argument_list
{
	/// 引数名
	char name[64];
	/// 次の引数
	struct argument_list *next;
} ArgList;

/// 関数オブジェクト
typedef struct function
{
	/// プログラム開始位置
	int start_pc;
	/// 関数名
	char name[64];
	/// 引数リスト
	ArgList *args;
	/// 次の関数
	struct function *next;
} Function;

/// 関数リスト
typedef struct func_list
{
	Function *functions;
} FuncList;

void initFuncList(FuncList *);
void releaseFuncList(FuncList *);

/* サブルーチン関連API */
Function *createFunction(char *, int);
void addArgument(Function *, char *);

void addFunction(FuncList *, Function *);
Function *getFunction(FuncList *, char *);

#endif