#pragma once
#include "Parser.h"

class SemanticAnalyser
{

	std::vector<std::unique_ptr<Stmt>> stmtList;
	SemanticAnalyser(std::vector<std::unique_ptr<Stmt>> _stmtList) { stmtList = _stmtList; };
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
