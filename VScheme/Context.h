#pragma once

#include "Expression.h"
#include "Tokens.h"
#include <map>

struct Context
{

	explicit Context(Context* topLevelContext = nullptr)
		:topLevelContext_(topLevelContext)
	{}

	const Expression& Find(const Token& token) const
	{
		auto ret = map_.find(token);
		if(ret != map_.end())
		{
			return ret->second;
		}

		if(topLevelContext_)
		{
			return topLevelContext_->Find(token);
		}

		throw "Undefined symbol";
	}

	std::map<Token, Expression> map_;

private:
	Context* topLevelContext_;
};
