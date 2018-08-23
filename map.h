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
 * サブルーチンオブジェクト
 */
typedef struct subroutine
{
	/// サブルーチン名
	char name[64];
	/// 実行コードバッファ
	char *code;
	/// 次のサブルーチン
	struct subroutine *next;
} Subroutine;

void map_init(void);
void map_release(void);

Var *createVar(char *, int);
void addVar(Var *);
Var *getVar(char *);

Subroutine *createSubroutine(char *);
void addSubroutine(Subroutine *);
void addInstruction(char *);
Subroutine *getSubroutine(char *);

#endif