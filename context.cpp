#include "context.hpp"

/**
 * @brief コンストラクタ
 */
Context::Context(void)
{
	ContextData *context = new ContextData();
	this->cstack.push(context);
};

/**
 * @brief デストラクタ
 */
Context::~Context(void)
{
	while (!this->cstack.empty())
	{
		ContextData *top = this->cstack.top();
		this->cstack.pop();
		delete top;
	}
};

/**
 * @brief 現在のコンテキストの保存
 */
void Context::pushContext(void)
{
	ContextData *new_context = new ContextData();
	this->cstack.push(new_context);
};

/**
 * @brief 現在のコンテキストの破棄
 */
void Context::popContext(void)
{
	ContextData *top = this->cstack.top();
	this->cstack.pop();
	delete top;
};

/**
 * @brief プログラムカウンタのpush
 * @param pc プログラムカウンタの値
 */
void Context::pushPC(int pc)
{
	ContextData *context = this->cstack.top();
	context->pc.push(pc);
};

/**
 * @brief プログラムカウンタのpop
 * @return プログラムカウンタの値
 */
int Context::popPC(void)
{
	ContextData *context = this->cstack.top();
	int ret = context->pc.top();
	context->pc.pop();
	return ret;
};

/**
 * @brief 実行状態のpush
 * @param state 実行状態
 */
void Context::pushState(int state)
{
	ContextData *context = this->cstack.top();
	context->state.push(state);
};

/**
 * @brief 実行状態のpop
 * @return 実行状態
 */
int Context::popState(void)
{
	ContextData *context = this->cstack.top();
	int ret = context->state.top();
	context->state.pop();
	return ret;
};

/**
 * @brief スタックトップの実行状態の取得
 * @return 実行状態
 */
int Context::peekState(void)
{
	ContextData *context = this->cstack.top();
	return context->state.top();
};

/**
 * @brief コードブロックのpush
 * @param block コードブロック
 */
void Context::pushBlock(int block)
{
	ContextData *context = this->cstack.top();
	context->block.push(block);
};

/**
 * @brief コードブロックのpop
 * @return コードブロック
 */
int Context::popBlock(void)
{
	ContextData *context = this->cstack.top();
	int ret = context->block.top();
	context->block.pop();
	return ret;
};
