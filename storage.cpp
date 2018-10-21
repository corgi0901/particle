#include <iostream>
#include <string.h>
#include "storage.hpp"
#include "debug.hpp"

/**
 * @brief コンストラクタ
 */
ProgramStorage::ProgramStorage(void)
{
	this->storage.clear();
	this->addr = -1;
	this->store("");
};

/**
 * @brief デストラクタ
 */
ProgramStorage::~ProgramStorage(void)
{
	for (char *code : this->storage)
	{
		delete code;
	}
};

/**
 * @brief 実行コードを登録する
 * @param code 実行コード
 */
void ProgramStorage::store(const char *code)
{
	DPRINTF("store : %s\n", code);
	char *copy = new char[strlen(code) + 1];
	strcpy(copy, code);
	this->storage.push_back(copy);
};

/**
 * @brief 次に実行されるコードを取得する
 * @return 実行コード
 */
char *ProgramStorage::fetch(void)
{
	char *code = NULL;
	int next_addr = this->addr + 1;

	if (next_addr < (int)this->storage.size())
	{
		code = this->storage[next_addr];
		this->addr = next_addr;
	}

	return code;
};

/**
 * @brief 指定した実行行に飛ぶ
 * @param address 番地
 */
void ProgramStorage::jump(int address)
{
	DPRINTF("jump : %d\n", pc);
	this->addr = address;
};

/**
 * @brief 現在の実行行の番地を取得する
 * @return 実行行の番地
 */
int ProgramStorage::address(void)
{
	return this->addr;
};
