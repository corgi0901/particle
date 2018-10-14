#ifndef _MEM_HPP_
#define _MEM_HPP_

/// 変数
typedef struct variable
{
	/// 変数名
	char name[64];
	/// 値
	int value;
	/// 属するメモリ空間の階層
	int space;
} Variable;

/// 変数の種類
typedef enum
{
	/// ローカル変数
	VAR_LOCAL = 0,
	/// 関数の引数
	VAR_ARG,
} VAR_TYPE;

void initMemory(void);
void releaseMemory(void);

void pushMemorySpace(void);
void popMemorySpace(void);

void setVariable(char *, int, VAR_TYPE);
Variable *getVariable(char *);

#endif
