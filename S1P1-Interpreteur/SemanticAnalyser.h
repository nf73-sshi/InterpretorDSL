#pragma once
#include "Parser.h"
#include <unordered_map>
#include "Debug.h"

struct SemanticAnalyser
{
	std::unordered_map<std::string, float> idList;

	std::vector<std::unique_ptr<Stmt>> stmtList;
	int actualID = 0;

	SemanticAnalyser(std::vector<std::unique_ptr<Stmt>>& _stmtList)
	{ 
		for (std::unique_ptr<Stmt>& stmt : _stmtList)
		{
			stmtList.push_back(std::move(stmt));
		}
	};

	void AnalyseStmt();
public:
	void SemanticAssign(Stmt* stmt);
	void SemanticIf(Stmt* stmt);
	void SemanticWhile(Stmt* stmt);
	void SemanticFor(Stmt* stmt);
	void SemanticTroop(Stmt* stmt);
	void SemanticParam(Stmt* stmt);
	void SemanticVal(Stmt* stmt);
};
