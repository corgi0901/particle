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
	/// 次の変数オブジェクト
	struct variable *next;
} Var;

/**
 * 変数マップ
 */
typedef struct var_map
{
	/// 変数リスト
	Var *vars;
	struct var_map *next;
} VarMap;

typedef struct varMapStack
{
	VarMap *head;
} VarMapStack;

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
	/// 実行コードバッファ
	char *code;
	/// 引数リスト
	Arg *args;
	/// 次の関数オブジェクト
	struct function *next;
	/// プログラム開始位置
	int start_pc;
} Function;

void mapInit(void);
void mapRelease(void);

/* 変数マップ関連API */
VarMap *createVarMap(void);
void clearMap(VarMap **);

/* 変数関連API */
Var *createVar(char *, int);
void addVar(VarMap *, Var *);
Var *getVar(VarMap *, char *);

void pushVarMap(VarMapStack *, VarMap *);
VarMap *popVarMap(VarMapStack *);

/* サブルーチン関連API */
Function *createFunction(char *, int);
void addFunction(Function *);
void addInstruction(char *);
void addArg(char *);
Function *getFunction(char *);

#endif