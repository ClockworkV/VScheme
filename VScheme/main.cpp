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

using namespace std;

wstringstream program_test(L" 10 ( hello?) (hello -5) ( hi ( one two))  ");
wstringstream program_norwig(L"(begin (define r 10) (* pi (* r r)))");

typedef wstring Token;
typedef vector<Token>::iterator TokenItr;

const Token openParen = L"(";
const Token closeParen = L")";

bool IsNumber(const Token& token)
{
	return regex_match(token, wregex(L"[(-|+)|][0-9]+"));
}

vector<Token> tokenize(wistream& program)
{
	vector<Token> tokens;

	istreambuf_iterator<wchar_t> itr(program);
	istreambuf_iterator<wchar_t> eos;

	while(itr != eos)
	{
		while(itr != eos && iswspace(*itr))
			itr++;

		tokens.emplace_back();

		if(*itr == '(')
		{
			tokens.back() = openParen;
			itr++;
		}
		else if(*itr == ')')
		{
			tokens.back() = closeParen;
			itr++;
		}
		else
		{
			while(itr != eos && !iswspace(*itr) && !(*itr == ')') && !(*itr == '('))
			{
				tokens.back().push_back(*itr);
				itr++;
			}
		}

		while(itr != eos && iswspace(*itr))
			itr++;
	}

	return tokens;
}



template<typename T>
void print(const T& x, wostream& os)
{
	//os << x << '\n';
	os << x ;
}

class Context;

struct Expression
{
	template<typename T>
	Expression(const T& src):Expression_(new Impl<T>(src))
	{
	}

	Expression(const Expression& src):Expression_(src.Expression_){}

	friend void print(const Expression& x, wostream& os) 
	{
		x.Expression_->print_(os);
	}

	template<typename T>
	T get() const
	{
		auto impl = dynamic_cast<Impl<T>*>(Expression_.get());
		if(impl)
		{
			return impl->mData;
		}

		throw;
	}

private:
	struct Concept
	{
		virtual void print_(wostream&) const = 0;
		virtual Expression eval(const Context& context) const = 0;
	};
	
	template<typename T>
	struct Impl : public Concept
	{
		Impl(const T& data):mData(data){}

		void print_(wostream& os) const override
		{
			print(mData, os);
		}

		T mData;
	};


private:
	shared_ptr<Concept> Expression_;
};


struct Number
{
	Number(int value): value_(value){}

	Expression eval() const { return Expression(*this); }

	operator int() const { return value_; }

	void print_(wostream& os) const
	{
		os << value_;
	}

private:
	int value_;
};

Expression make_atom(Token token)
{
	if(IsNumber(token))
	{
		wstringstream ss(token);
		
		int num = 0;
		
		ss >> num;

		return num;
	}
	else
	{
		return token;
	}
}


struct List
{
	List(vector<Token>::iterator& itr, const vector<Token>::iterator& last)
	{
		while(itr != last)
		{
			if(*itr == openParen)
			{
				list_.emplace_back(List(++itr, last));
			}
			else if(*itr == closeParen)
			{
				itr++;
				return;
			}
			else
			{
				list_.push_back(make_atom(*itr));
				itr++;
			}
		}
	}

	Expression eval() const { return Expression(0); }

	void print_(wostream& os) const
	{
		os << "[";

		for(const auto& expr : list_)
		{
			print(expr, os);
			os << ' ';
		}

		os << "]";
	}

private:
	vector<Expression> list_;
};

wostream& operator<<(wostream& os, const List& listExpr)
{
	listExpr.print_(os);

	return os;
}

typedef map<wstring, Expression> Context;




struct OpPlus
{
	void print_(wostream& os) const
	{
		os << L"+";
	}

	Expression operator()(vector<Expression> operands)
	{
		int sum = 0;

		for(const auto& operand : operands)
		{
			sum += operand.get<int>();
		}

		return sum;
	}

};

wostream& operator<<(wostream& os, const OpPlus& op)
{
	op.print_(os);

	return os;
}






typedef vector<Expression> Program;



int main(int argc, char** argv)
{
	Context gContext;

	gContext.emplace(wstring(L"+"), Expression(OpPlus()));

	Program program;

	program.push_back(1);
	program.push_back(wstring(L"Hello"));
	program.push_back(Number(5));


	for(const auto& aExpr : program)
	{
		print(aExpr, wcout);
	}

	wcout << program.front().get<int>() << '\n';

	//Program toSum = { 1, 2, 3 , wstring(L"Hello")};
	Program toSum = { 1, 2, 3 };

	auto op = OpPlus();

	auto sumExpr = op(toSum);

	wcout << L"sum = ";
	
	print(sumExpr, wcout);
	
	wcout << '\n';

	auto aTokens = tokenize(program_norwig);

	auto myList = List(aTokens.begin(), aTokens.end());

	print(myList, wcout);

	wcout << '\n';

	aTokens = tokenize(program_test);

	auto myList2 = List(aTokens.begin(), aTokens.end());

	print(myList2, wcout);

	wcout << '\n';




	return 0;
}
