#include "Tokens.h"
#include <regex>
#include <queue>
#include <sstream>

#include "Expression.h"
#include "Number.h"
#include "Symbol.h"

using namespace std;

bool IsNumber(const Token & token)
{
	return regex_match(token, wregex(L"-?[[:d:]]+$"));
}

bool IsValidSymbol(const Token & token)
{
	return regex_match(token, wregex(L"[^[:d:]()]+$"));
}

deque<Token> tokenize(wistream & program)
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

bool is_atom(Token token)
{
	return IsNumber(token) || IsValidSymbol(token);
}

Expression make_atom(Token token, const Context& context)
{
	if(IsNumber(token))
	{
		wstringstream ss(token);

		int num = 0;

		ss >> num;

		return Expression(Number(num), context);
	}
	else
	{
		return Expression(Symbol(token), context);
	}
}
