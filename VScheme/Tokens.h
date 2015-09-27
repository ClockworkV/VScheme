#pragma once

#include <string>
#include <queue>

#include "Expression.h"

typedef std::wstring Token;

const Token openParen = L"(";
const Token closeParen = L")";

bool IsNumber(const Token& token);

bool IsValidSymbol(const Token& token);

std::deque<Token> tokenize(std::wistream& program);

bool is_atom(Token token);

Expression make_atom(Token token, const Context& context);
