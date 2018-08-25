#ifndef _MAP_H_
#define _MAP_H_

/**
 * 変数オブジェクト
 */
typedef struct variable
{
	/// 変数名（最大64文字）
	char name[64];
	/// 値
	int value;
	/// 変数マップ中の次のオブジェクト
	struct variable *next;
} Var;

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
 * サブルーチンオブジェクト
 */
typedef struct subroutine
{
	/// サブルーチン名
	char name[64];
	/// 実行コードバッファ
	char *code;
	/// 引数リスト
	Arg *args;
	/// 次のサブルーチン
	struct subroutine *next;
} Subroutine;

void map_init(void);
void map_release(void);

Var *createVar(char *, int);
void addGlobalVar(Var *);
Var *getGlobalVar(char *);

void addLocalVar(Var *);
Var *getLocalVar(char *);
void releaseLocalVar(void);

Subroutine *createSubroutine(char *);
void addSubroutine(Subroutine *);
void addInstruction(char *);
void addArg(char *);
Subroutine *getSubroutine(char *);

#endif