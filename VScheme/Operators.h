#pragma once

#include <vector>
#include <iostream>
#include <functional>

#include "Expression.h"
#include "Number.h"
#include "Symbol.h"

typedef std::function<Expression ( std::vector<Expression>& ) > Op;

struct Define : public Op
{

};

struct OpPlus : public Op
{
	friend void Print(const Op&, std::wostream& os)
	{
		os << L"+";
	}

	Number operator()(std::vector<Expression>& operands) const
	{
		int sum = operands.back().eval().get<Number>();

		operands.pop_back();

		for(const auto& operand : operands)
		{
			sum += operand.eval().get<Number>();
		}

		return Number(sum);
	}

	friend Number Do(const OpPlus& f, std::vector<Expression>& args)
	{
		return f(args);
	}

	friend Symbol Eval(const OpPlus&, const Context& context)
	{
		return Symbol(L"+");
	}
};

struct OpMinus : public Op
{
	friend void Print(const OpMinus&, std::wostream& os)
	{
		os << L"-";
	}

	Number operator()(std::vector<Expression>& operands) const
	{
		int sum = operands.back().eval().get<Number>();

		operands.pop_back();

		for(const auto& operand : operands)
		{
			sum -= operand.eval().get<Number>();
		}

		return Number(sum);
	}

	friend Number Do(const OpMinus& f, std::vector<Expression>& args)
	{
		return f(args);
	}

	friend Symbol Eval(const OpMinus&, const Context& context)
	{
		return Symbol(L"-");
	}
};

struct OpMult : public Op
{
	friend void Print(const OpMult&, std::wostream& os)
	{
		os << L"*";
	}

	Number operator()(std::vector<Expression>& operands) const
	{
		int prod = operands.back().eval().get<Number>();

		operands.pop_back();

		for(const auto& operand : operands)
		{
			prod *= operand.eval().get<Number>();
		}

		return Number(prod);
	}

	friend Number Do(const OpMult& f, std::vector<Expression>& args)
	{
		return f(args);
	}

	friend Symbol Eval(const OpMult&, const Context& context)
	{
		return Symbol(L"*");
	}
};
