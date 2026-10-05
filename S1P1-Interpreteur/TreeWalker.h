#pragma once
#include "Parser.h"
#include <iostream>
#include <vector>

struct Environment;

struct TreeWalker
{
	std::vector<std::unique_ptr<Stmt>> statements;

	TreeWalker(std::vector<std::unique_ptr<Stmt>>& _s)
	{
		for (std::unique_ptr<Stmt>& stmt : _s)
		{
			statements.push_back(std::move(stmt));
		}
	}

	void Execute(Environment* e);
};

