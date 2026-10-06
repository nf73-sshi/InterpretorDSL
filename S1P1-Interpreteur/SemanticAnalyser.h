#pragma once
#include "Parser.h"
#include <unordered_map>
#include "Debug.h"

struct SemanticAnalyser
{
	std::vector<std::unordered_map<std::string, Type>> datasType = {};

	std::vector<std::unique_ptr<Stmt>> stmtList;

	int actualStmt;
	SemanticAnalyser(std::vector<std::unique_ptr<Stmt>>& _stmtList)
	{ 
		for (std::unique_ptr<Stmt>& stmt : _stmtList)
		{
			stmtList.push_back(std::move(stmt));
		}
	};

	void AnalyseStmt();
public:
	void SemanticSwitch(Stmt* stmt);
	void SemanticAssign(Stmt* stmt);
	void SemanticIf(Stmt* stmt);
	void SemanticWhile(Stmt* stmt);
	void SemanticFor(Stmt* stmt);
	void SemanticTroop(Stmt* stmt);
	void SemanticBlock(Stmt* stmt);
	void SemanticVal(Stmt* stmt);
	void SemanticExpr(Stmt* stmt);
	void AddDatasBlock(bool remove = false);
	bool MapExists(size_t block, std::string name) const;
	void ValInBlock(std::string name, Type value);
	bool CheckValue(std::string name);
};
