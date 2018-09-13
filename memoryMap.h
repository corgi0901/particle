#ifndef _MEMORY_MAP_H_
#define _MEMORY_MAP_H_

/// 変数オブジェクト
typedef struct variable
{
	char name[64];
	int value;
	struct variable *next;
} Variable;

/// 変数マップ
typedef struct variable_map
{
	Variable *vars;
	struct variable_map *next;
} VariableMap;

typedef struct var_map_stack
{
	VariableMap *map;
} VarMapStack;

void initVarMapStack(VarMapStack *);
void releaseVarMapStack(VarMapStack *);
void pushVariableMap(VarMapStack *, VariableMap *);
VariableMap *popVariableMap(VarMapStack *);

VariableMap *createVariableMap(void);
void releaseVariableMap(VariableMap *);
Variable *addVariable(VariableMap *, char *, int);
Variable *getOrCreateVariable(VarMapStack *, char *);

#endif
