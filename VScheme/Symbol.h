#pragma once

#include <string>

#include "Expression.h"

struct Symbol
{
	Symbol(std::wstring value): value_(value){}

	friend Expression Eval(const Symbol& symbol, const Context& context);

	operator std::wstring() const;

	friend void Print(const Symbol& symbol, std::wostream& os);

private:
	std::wstring value_;
};