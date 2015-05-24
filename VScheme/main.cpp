#include <iostream>
#include <string>
#include <vector>
#include <iterator>

#include <sstream>
#include <vector>
#include <memory>

using namespace std;

typedef wstring Token;
const Token openParen = L"(";
const Token closeParen = L")";


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
	NONE, FIXNUM, SYMBOL, LIST,
};


class Context;

struct ObjectImpl : public enable_shared_from_this<ObjectImpl>
{
	ObjectImpl(Type type):mType(type){};

	Type getType() const { return mType; }

	virtual	shared_ptr<ObjectImpl> evaluate(Context& context) = 0;

	virtual void print(wostream& os) const = 0;

private:
	const Type mType;
};

struct Int : public ObjectImpl
{
	Int(int value):ObjectImpl(Type::FIXNUM), mValue(value)
	{}

	shared_ptr<ObjectImpl> evaluate(Context& context) override
	{
		return shared_from_this();
	}

	virtual void print(wostream& os) const
	{
		os << mValue;
	}

private:
	int mValue;
};


struct Symbol : public ObjectImpl
{
	Symbol(wstring symbol) : ObjectImpl(Type::SYMBOL), mValue(symbol)
	{}

	shared_ptr<ObjectImpl> evaluate(Context& context) override
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


struct List : public ObjectImpl
{

	List(): ObjectImpl(Type::LIST){}

	shared_ptr<ObjectImpl> evaluate(Context& context) override
	{
		return nullptr;
	}

	void print(wostream& os) const override
	{
	}


private:
	vector<ObjectImpl> mList;
};



struct Object
{
	Object(Type type, Token valueToken)
	{
		switch(type)
		{
		case Type::FIXNUM:
			{
				wstringstream ss(valueToken);

				int value;

				ss >> value;

				if(ss.good())
				{
					mpImpl = make_shared<Int>(value);
				}
			}
			break;

		case Type::SYMBOL:
			mpImpl = make_shared<Symbol>(valueToken);
			break;

		case Type::LIST:
		case Type::NONE:
		default:
			break;
		}
	}

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

private:

	Object(shared_ptr<ObjectImpl> pValue)
	{
		mpImpl = pValue;
	}


	shared_ptr<ObjectImpl> mpImpl;
};


Object read(vector<Token> tokens)
{

	auto currentTokenItr = tokens.begin();

	if(*currentTokenItr == openParen)
	{
		auto listOfObjects = Object(Type::LIST, L"");

		currentTokenItr++;

		return Object(listOfObjects);

	}
	else
	{
		return Object(Type::FIXNUM, tokens.front());
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