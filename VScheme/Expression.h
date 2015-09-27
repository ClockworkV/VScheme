#pragma once
#include <iostream>
#include <vector>
#include <memory>

struct Context;


struct Expression
{
	template<typename T>
	Expression(const T& src, const Context& context ):mpImpl(new Model<T>(src)), mpContext(context)
	{};

	struct Expression eval() const
	{
		return mpImpl->eval_(mpContext);
	}

	void print(std::wostream& os) const
	{
		mpImpl->print_(os);
	}

	Expression operator()(std::vector<Expression>& args) const
	{
		return mpImpl->do_(args);
	}

	template<typename T>
	const T& get() const
	{
		auto ptr = std::dynamic_pointer_cast<Model<T>>(mpImpl);
		if(ptr)
		{
			return ptr->data_;
		}
		throw "Invalid Type";
	}

	struct Concept
	{
		virtual Expression eval_(const Context& context) const = 0;
		virtual void print_(std::wostream& os) const = 0;
		virtual Expression do_(std::vector<Expression>& args) const = 0;
	};

	template<typename T>
	struct Model : public Concept
	{
		Model(const T& t): data_(t){}

		Expression eval_(const Context& context) const override
		{
			return Eval(data_, context);
		}

		void print_(std::wostream& os) const override
		{
			Print(data_, os);
		}

		Expression do_(std::vector<Expression>& args) const override
		{
			return Do(data_, args);
		}

		T data_;
	};

	std::shared_ptr<Concept> mpImpl;
	const Context& mpContext;
};

template<typename T>
Expression Eval(const T& list, const Context& context);

template<typename T>
Expression Do(const T& f, std::vector<Expression>& args)
{
	throw "Can't do!";
	//Context c();
	//return Expression(Number(0), c);
}