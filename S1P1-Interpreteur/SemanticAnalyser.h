#pragma once
#include "Parser.h"

class SemanticAnalyser
{

	std::vector<std::unique_ptr<Stmt>> stmtList;
	SemanticAnalyser(std::vector<std::unique_ptr<Stmt>> _stmtList) { stmtList = _stmtList; };
	void AnalyseStmt();

public:
	int SemanticAssign();
	int SemanticBlock();
	int SemanticIf();
	int SemanticWhile();
	int SemanticFor();
	int SemanticTroop();
	int SemanticVal();
};
