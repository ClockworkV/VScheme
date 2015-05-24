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
	return regex_match(token, wregex(L"[0-9]+"));
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


struct Context;

//template<typename T>
//T Eval(const T& t, const Context& context) { return t; }

template<typename T>
void Print(const T& t,  wostream& os) {  }

struct Expression
{
	template<typename T>
	Expression(const T& src):mpImpl(new Model<T>(src))
	{}

	Expression eval(const Context& context) const
	{
		return mpImpl->eval_(context);
	}

	void print(wostream& os) const
	{
		mpImpl->print_(os);
	}

	template<typename T>
	T get() const
	{
		auto ptr = dynamic_pointer_cast<Model<T>>(mpImpl);
		if(ptr)
		{
			return ptr->data_;
		}
		throw "Invalid Type";
	}

	struct Concept
	{
		virtual Expression eval_(const Context& context) const = 0;
		virtual void print_(wostream& os) const = 0;
	};

	template<typename T>
	struct Model : public Concept
	{
		Model(const T& t): data_(t){}

		Expression eval_(const Context& context) const override
		{
			return Eval(data_, context);
		}

		void print_(wostream& os) const override
		{
			Print(data_, os);
		}

		T data_;
	};

	shared_ptr<Concept> mpImpl;
};

struct Context
{
	Expression Find(const Token& token) const
	{
		auto ret = map_.find(token);
		if(ret != map_.end())
		{
			return ret->second;
		}

		throw "Undefined symbol";
	}

	map<Token, Expression> map_;
};


Context gContext;

struct Number
{
	Number(int value): value_(value){}

	friend 
	Expression Eval(const Number& n, const Context& context) { return Number(n.value_); }

	operator int() const { return value_; }

	friend void Print(const Number& number, std::wostream& os) {os << number.value_;}

private:
	int value_;
};


struct Symbol
{
	Symbol(wstring value): value_(value){}

	friend Expression Eval(const Symbol& symbol, const Context& context) { return context.Find(symbol.value_); }

	operator wstring() const { return value_; }

	friend void Print(const Symbol& symbol, std::wostream& os)	{os << symbol.value_;}

private:
	wstring value_;
};


Expression make_atom(Token token)
{
	if(IsNumber(token))
	{
		wstringstream ss(token);
		
		int num = 0;
		
		ss >> num;

		return Number(num);
	}
	else
	{
		return Symbol(token);
	}
}

typedef unary_function<vector<Expression>&, Expression> Op;


struct OpPlus : public Op
{
	friend void Print(OpPlus, wostream& os)
	{
		os << L"+";
	}

	Expression operator()(vector<Expression>& operands) const
	{
		int sum = 0;

		for(const auto& operand : operands)
		{
			sum += operand.get<Number>();
		}

		return Number(sum);
	}

	Expression eval(const Context& context) const
	{
		return Symbol(L"+");
	}
};


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

	friend Expression Eval(const List& list, const Context& context)
	{
		Expression result(Number(0));

		for(const auto& aExpr : list.list_)
		{
			wstringstream ss;

			aExpr.print(ss);

			auto val = ss.str();

			if(val == L"+")
			{
				auto operands = vector<Expression>(++list.list_.begin(), list.list_.end());
				auto op = OpPlus();
				auto result = op(operands);
				return result;
			}
			else
			{
				result = aExpr.eval(context);
			}
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

private:
	vector<Expression> list_;
};








int main(int argc, char** argv)
{
	auto tokens = tokenize(program_norwig);

	Expression e(List(tokens.begin(), tokens.end()));

	e.print(wcout);

	wcout << '\n';



	gContext.map_.emplace(wstring(L"hello"), Expression(Number(10)));

	auto h = Expression(Symbol(wstring(L"hello")));

	auto expr = h.eval(gContext);

	expr.print(wcout);
	
	wcout << '\n';

	//wstring input;

	//getline(wcin, input);

	auto sumTokens_ = tokenize(wstringstream(L"(+ 1 2 3)"));
	//auto sumTokens = tokenize(wstringstream(input));
	
	auto sumExpr_ = Expression(List(sumTokens_.begin(), sumTokens_.end()));
	//auto sumExpr = Expression(List(sumTokens.begin(), sumTokens.end()));

	auto result = sumExpr_.eval(gContext);

	result.print(wcout);

	wcout << '\n';

	return 0;
}
