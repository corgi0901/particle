#ifndef _CONTEXT_HPP_
#define _CONTEXT_HPP_

#include <stack>

using namespace std;

/// コンテキスト
typedef struct context_data
{
	/// プログラムカウンタ
	stack<int> pc;
	/// 実行状態
	stack<int> state;
	/// コードブロック
	stack<int> block;
} ContextData;

class Context
{
  private:
	stack<ContextData *> cstack;

  public:
	Context();
	~Context();

	void pushContext(void);
	void popContext(void);

	void pushPC(int);
	int popPC(void);

	void pushState(int);
	int popState(void);
	int peekState(void);

	void pushBlock(int);
	int popBlock(void);
};

#endif
