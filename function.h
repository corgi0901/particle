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

void initFuncList(void);
void releaseFuncList(void);

/* サブルーチン関連API */
Function *createFunction(char *, int);
void addArgument(Function *, char *);

void addFunction(Function *);
Function *getFunction(char *);

#endif
