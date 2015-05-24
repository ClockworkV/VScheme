#include <iostream>
#include <string>
#include <vector>
#include <iterator>

#include <sstream>
#include <vector>
#include <memory>
#include <regex>

using namespace std;

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

enum class Type
{
	NONE, NUMBER, SYMBOL, LIST, PROC, LAMBDA
};


class Context;


struct ObjectImplBase : public enable_shared_from_this<ObjectImplBase>
{
	virtual Type getType() const = 0;

	virtual	shared_ptr<ObjectImplBase> evaluate(Context& context) = 0;

	virtual void print(wostream& os) const = 0;
};


struct Object
{
	Object(const TokenItr valueToken);

	Object(const Object& src):mpImpl(src.mpImpl)
	{}

	Type getType() const
	{
		return mpImpl->getType();
	}

	Object evaluate(Context& context) 
	{
		return Object(mpImpl->evaluate(context));
	}

	void print(wostream& os) const
	{
		mpImpl->print(os);
	}

	static const Object gEmptyList; 

private:

	Object(shared_ptr<ObjectImplBase> pValue)
	{
		mpImpl = pValue;
	}

	shared_ptr<ObjectImplBase> mpImpl;
};

const Object Object::gEmptyList = Object(nullptr);




template<Type T>
struct ObjectImpl : public ObjectImplBase 
{
	Type getType() const override { return T; }
};

struct Int : public ObjectImpl<Type::NUMBER>
{
	Int(int value):mValue(value)
	{}

	shared_ptr<ObjectImplBase> evaluate(Context& context) override
	{
		return shared_from_this();
	}

	void print(wostream& os) const override
	{
		os << mValue;
	}



private:
	int mValue;
};


struct Symbol : public ObjectImpl<Type::SYMBOL>
{
	Symbol(wstring symbol) : mValue(symbol)
	{}

	shared_ptr<ObjectImplBase> evaluate(Context& context) override
	{
		return shared_from_this();
	}

	void print(wostream& os) const override
	{
		os << mValue;
	}

private:
	const wstring mValue;
};


struct List : public ObjectImpl<Type::LIST>
{

	List(){}

	shared_ptr<ObjectImplBase> evaluate(Context& context) override
	{
		return nullptr;
	}

	void print(wostream& os) const override
	{
	}

	void Add(Object object)
	{
		mList.push_back(object);
	}

	Object Car() const
	{
		if(mList.size() >= 1)
		{
			return mList.front();
		}
		else
		{
			return Object(make_shared<List>());
		}
	}

private:
	vector<Object> mList;
};



Object read(vector<Token> tokens)
{

	auto currentTokenItr = tokens.begin();

	if(*currentTokenItr == openParen)
	{
		auto listOfObjects = Object();

		currentTokenItr++;

		return Object(listOfObjects);

	}
	else
	{
		return Object(Type::NUMBER, tokens.front());
	}
}


void print(Object)
{}

wstringstream program(L" 10 ( hello?) (hello -5) ( hi ( one two))  ");
wstringstream program_norwig(L"(begin (define r 10) (* pi (* r r)))");

int main(int argc, char** argv)
{

	while(1)
	{
		wstring prog;

		getline(wcin, prog);

		wstringstream ss(prog);
	
		auto o = read(tokenize(ss));

		o.print(wcout);

		cout << '\n';
	}
	
	return 0;
}

inline Object::Object(const TokenItr valueToken)
{
	if(IsNumber(*valueToken))
	{
		wstringstream ss(*valueToken);

		int value;

		ss >> value;

		if(ss.good())
		{
			mpImpl = make_shared<Int>(value);
		}
	}
	else if(*valueToken == openParen)
	{
		mpImpl = make_shared<List>();
	}
	else
	{
		mpImpl = make_shared<Symbol>(valueToken);
	}
}
