#ifndef _MAP_H_
#define _MAP_H_

/**
 * 引数リスト
 */
typedef struct argument
{
	/// 引数名
	char name[64];
	/// 次の引数オブジェクト
	struct argument *next;
} Arg;

/**
 * 関数オブジェクト
 */
typedef struct function
{
	/// サブルーチン名
	char name[64];
	/// 引数リスト
	Arg *args;
	/// 次の関数オブジェクト
	struct function *next;
	/// プログラム開始位置
	int start_pc;
} Function;

void mapInit(void);
void mapRelease(void);

/* サブルーチン関連API */
Function *createFunction(char *, int);
void addFunction(Function *);
void addArg(char *);
Function *getFunction(char *);

#endif