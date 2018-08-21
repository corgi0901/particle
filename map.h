#ifndef _MAP_H_
#define _MAP_H_

/**
 * 変数オブジェクト
 */
typedef struct var
{
	/// 変数名（最大64文字）
	char name[64];
	/// 値
	int value;
	/// 変数マップ中の次のオブジェクト
	struct var *next;
} var;

/**
 * サブルーチンオブジェクト
 */
typedef struct sub
{
	/// サブルーチン名
	char name[64];
	/// 実行コードバッファ
	char *buf;
	/// 次のサブルーチン
	struct sub *next;
} Subroutine;

void map_init(void);
void map_release(void);

var *createVar(char *, int);
void addVar(var *);
var *getVar(char *);

Subroutine *createSubroutine(char *);
void addSubroutine(Subroutine *);
void addInstruction(char *);
Subroutine *getSubroutine(char *);

#endif