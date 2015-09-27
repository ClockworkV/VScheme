#pragma once

#include "Expression.h"

struct Number
{
	Number(int value): value_(value){}

	friend Expression Eval(const Number& n, const Context&  context)
	{
		return Expression(Number(n.value_), context);
	}
	operator int() const { return value_; }

	friend void Print(const Number& number, std::wostream& os) { os << number.value_; }

private:
	int value_;
};
