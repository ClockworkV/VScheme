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

using namespace std;

wstringstream program_test(L" 10 ( hello?) (hello -5) ( hi ( one two))  ");
wstringstream program_norwig(L"(begin (define r 10) (* pi (* r r)))");

typedef wstring Token;

const Token openParen = L"(";
const Token closeParen = L")";

bool IsNumber(const Token& token)
{
	return regex_match(token, wregex(L"-?[[:digit:]]+"));
}

bool IsValidSymbol(const Token& token)
{
	return regex_match(token, wregex(L"[^[:digit:]()]+"));
}

deque<Token> tokenize(wistream& program)
{
	deque<Token> tokens;

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

	Expression operator()(vector<Expression>& args) const
	{
		return mpImpl->do_(args);
	}


	template<typename T>
	const T& get() const
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
		virtual Expression do_(vector<Expression>& args) const = 0;
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

		Expression do_(vector<Expression>& args) const override
		{
			return Do(data_, args);
		}


		T data_;
	};

	shared_ptr<Concept> mpImpl;
};


template<typename T>
Expression Eval(const T& list, const Context& context);

template<typename T>
Expression Do(const T& f, vector<Expression>& args)
{
	throw "Can't do!";
	return Expression(Number(0));
}



struct Context
{
	const Expression& Find(const Token& token) const
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


bool is_atom(Token token)
{
	return IsNumber(token) || IsValidSymbol(token);
}


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

typedef function<Expression(vector<Expression>&)> Op;


struct OpPlus : public Op
{
	friend void Print(const Op&, wostream& os) 
	{
		os << L"+";
	}

	Expression operator()(vector<Expression>& operands) const
	{
		int sum = operands.back().eval(gContext).get<Number>();

		operands.pop_back();

		for(const auto& operand : operands)
		{
			sum += operand.eval(gContext).get<Number>();
		}

		return Number(sum);
	}

	friend Expression Do(const OpPlus& f, vector<Expression>& args)
	{
		return f(args);
	}

	friend Expression Eval(const OpPlus&, const Context& context) 
	{
		return Symbol(L"+");
	}
};

struct OpMinus : public Op
{
	friend void Print(const OpMinus&, wostream& os)
	{
		os << L"-";
	}

	Expression operator()(vector<Expression>& operands) const
	{
		int sum = operands.back().eval(gContext).get<Number>();

		operands.pop_back();

		for(const auto& operand : operands)
		{
			sum -= operand.eval(gContext).get<Number>();
		}

		return Number(sum);
	}

	friend Expression Do(const OpMinus& f, vector<Expression>& args)
	{
		return f(args);
	}

	friend Expression Eval(const OpMinus&, const Context& context)
	{
		return Symbol(L"-");
	}
};

struct OpMult : public Op
{
	friend void Print(const OpMult&, wostream& os)
	{
		os << L"*";
	}

	Expression operator()(vector<Expression>& operands) const
	{
		int prod = operands.back().eval(gContext).get<Number>();

		operands.pop_back();

		for(const auto& operand : operands)
		{
			prod *= operand.eval(gContext).get<Number>();
		}

		return Number(prod);
	}

	friend Expression Do(const OpMult& f, vector<Expression>& args)
	{
		return f(args);
	}

	friend Expression Eval(const OpMult&, const Context& context)
	{
		return Symbol(L"*");
	}
};

struct List
{
	List(){}

	static Expression read(deque<Token>& tokens)
	{
		List ret;

		while(tokens.front() != closeParen)
		{
			if(tokens.front() == openParen)
			{
				tokens.pop_front();

				ret.Add(List::read(tokens));
			}
			else
			{
				ret.Add(make_atom(tokens.front()));

				tokens.pop_front();
			}
		}

		tokens.pop_front();

		return ret;
	}

	friend Expression Eval(const List& list, const Context& context)
	{
		Expression result(Number(0));

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



vector<Expression> read(deque<Token>& tokens)
{

	vector<Expression> aProgram;

	while(!tokens.empty())
	{
		if(tokens.front() == openParen)
		{
			tokens.pop_front();

			aProgram.push_back(List::read(tokens));
		}
		else
		{
			aProgram.push_back(make_atom(tokens.front()));

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

			auto sumExpr = read(sumTokens); 

			for(const auto& aExpression : sumExpr)
			{
				auto result = aExpression.eval(gContext);

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
