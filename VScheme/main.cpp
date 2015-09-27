#include <iostream>
#include <string>
#include <vector>
#include <iterator>

#include <sstream>
#include <vector>
#include <memory>
#include <regex>
#include <map>
#include <algorithm>
#include <functional>
#include <deque>


#include "Tokens.h"
#include "Expression.h"
#include "Context.h"
#include "Number.h"
#include "Symbol.h"
#include "Operators.h"

using namespace std;

wstringstream program_test(L" 10 ( hello?) (hello -5) ( hi ( one two))  ");
wstringstream program_norwig(L"(begin (define r 10) (* pi (* r r)))");



Context gContext;


struct List
{
	List(){}

	static Expression read(deque<Token>& tokens, const Context& context)
	{
		List ret;

		while(tokens.front() != closeParen)
		{
			if(tokens.front() == openParen)
			{
				tokens.pop_front();

				ret.Add(List::read(tokens, context));
			}
			else
			{
				ret.Add(make_atom(tokens.front(), context));

				tokens.pop_front();
			}
		}

		tokens.pop_front();

		return Expression(ret, context);
	}

	friend Expression Eval(const List& list, const Context& context)
	{
		Expression result(Number(0), context);

		for(const auto& aExpr : list.list_)
		{
			wstringstream ss;

			aExpr.print(ss);

			auto val = ss.str();

			const auto& opExp = context.Find(val);

			auto operands = vector<Expression>(list.list_.rbegin(), --list.list_.rend());
			auto result = opExp(operands);
			return result;
		}

		return result;
	}

	friend void Print(const List& l, wostream& os)
	{
		os << "[";

		for(const auto& expr : l.list_)
		{
			expr.print(os);
			os << ' ';
		}

		os << "]";
	}

	void Add(Expression item)
	{
		list_.push_back(item);
	}

	Expression operator()(vector<Expression>&) const
	{
		throw "I'm a list, dammit";
	}
private:
	vector<Expression> list_;
};

vector<Expression> read(deque<Token>& tokens, const Context& context)
{
	vector<Expression> aProgram;

	while(!tokens.empty())
	{
		if(tokens.front() == openParen)
		{
			tokens.pop_front();

			aProgram.push_back(List::read(tokens, context));
		}
		else
		{
			aProgram.push_back(make_atom(tokens.front(), context));

			tokens.pop_front();
		}
	}

	return aProgram;
}

int main(int argc, char** argv)
{
	//cout << IsValidSymbol(L"number?") << '\n';
	//cout << IsValidSymbol(L"-number?") << '\n';
	//cout << IsValidSymbol(L"0") << '\n';
	//cout << IsValidSymbol(L"(0)") << '\n';

	//auto tokens = tokenize(program_norwig);

	//Expression e(List(tokens.begin(), tokens.end()));

	//e.print(wcout);

	//wcout << '\n';

	//gContext.map_.emplace(wstring(L"hello"), Expression(Number(10)));

	//auto h = Expression(Symbol(wstring(L"hello")));

	//auto expr = h.eval(gContext);

	//expr.print(wcout);
	//
	//wcout << '\n';

	gContext.map_.emplace(L"+", OpPlus());
	gContext.map_.emplace(L"-", OpMinus());
	gContext.map_.emplace(L"*", OpMult());

	try{
		while(1)
		{
			wcout << L">";

			wstring input;

			getline(wcin, input);

			auto sumTokens = tokenize(wstringstream(input));

			auto sumExpr = read(sumTokens, gContext);

			for(const auto& aExpression : sumExpr)
			{
				auto result = aExpression.eval();

				result.print(wcout);

				wcout << '\n';
			}
		}
	}
	catch(char* c)
	{
		cout << c << '\n';
	}
	return 0;
}