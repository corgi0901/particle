#ifndef _STORAGE_HPP_
#define _STORAGE_HPP_
#include <vector>

using std::vector;

/// 実行コードのリスト
class ProgramStorage
{
private:
	vector<char *> storage;
	int addr;

public:
	ProgramStorage(void);
	~ProgramStorage(void);

	void store(const char *);
	char *fetch(void);

	void jump(int);
	int address(void);
};

#endif
