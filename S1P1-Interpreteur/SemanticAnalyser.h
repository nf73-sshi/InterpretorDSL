#pragma once
#include "Parser.h"

class SemanticAnalyser
{

	std::vector<std::unique_ptr<Stmt>> stmtList;
	SemanticAnalyser(std::vector<std::unique_ptr<Stmt>> _stmtList)
	{ 
		for (std::unique_ptr<Stmt>& stmt : _stmtList)
		{
			stmtList.push_back(std::move(stmt));
		}
	};

	void AnalyseStmt();

public:
	int SemanticAssign(Stmt* stmt);
	int SemanticIf(Stmt* stmt);
	int SemanticWhile(Stmt* stmt);
	int SemanticFor(Stmt* stmt);
	int SemanticTroop(Stmt* stmt);
	int SemanticParam(Stmt* stmt);
	int SemanticVal(Stmt* stmt);
};
