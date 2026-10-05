#include "SemanticAnalyser.h"

void SemanticAnalyser::AnalyseStmt()
{
	for (std::unique_ptr<Stmt>& stmt : stmtList)
	{
		int sementicCase;
		switch (stmt.get()->type)
		{
		case ASSIGN_STMT:
			sementicCase = SemanticAssign(stmt.get());
				break;
		case BLOCK_STMT:
			break;
		case IF_STMT:
			break;
		case WHILE_STMT:
			break;
		case FOR_STMT:
			break;
		case PARAM_STMT:
			break;
		case TROOP_STMT:
			break;
		case VAL_STMT:
			break;
		default:
			break;
			
		}
	}
}
int SemanticAnalyser::SemanticAssign(std::unique_ptr<Stmt> stmt)
{
	AssignStmt* asnStmt = static_cast<AssignStmt*>(stmt.get());
	ExprType type = asnStmt->expr.get()->type;

	if (type == ExprType::VARIABLE)
	{
		Variable* expr = static_cast<Variable*>(asnStmt->expr.get());
		if (expr->name == "")
			return 1;
		return 0;
	}
	else if (type == ExprType::GROUPING)
	{
		Grouping* grp = static_cast<Grouping*>(asnStmt->expr.get());

		if (grp->expr.get()->type != ExprType::VARIABLE)
			return 1;
		Variable* expr = static_cast<Variable*>(grp->expr.get());
		if (expr->name == "")
			return 1;
		return 0;
	}
	else
		return 2;
}

int SemanticAnalyser::SemanticBlock(std::unique_ptr<Stmt> stmt)
{
	return 0;
}

int SemanticAnalyser::SemanticIf(std::unique_ptr<Stmt> stmt)
{
	IfStmt* ifStmt = static_cast<IfStmt*>(stmt.get());
	ExprType type = ifStmt->condition.get()->type;

	if (type != ExprType::BINARY)
		return 2;
	Binary* expr = static_cast<Binary*>(ifStmt->condition.get());
	if (expr->left == nullptr)
		return 1;
	if (expr->right == nullptr)
		return 1;
	if (expr->op == "")
		return 1;

	return 0;
}

int SemanticAnalyser::SemanticWhile(std::unique_ptr<Stmt> stmt)
{
	WhileStmt* whileStmt = static_cast<WhileStmt*>(stmt.get());
	ExprType type = whileStmt->condition.get()->type;

	if (type != ExprType::BINARY)
		return 2;
	Binary* expr = static_cast<Binary*>(whileStmt->condition.get());
	if (expr->left == nullptr)
		return 1;
	if (expr->right == nullptr)
		return 1;
	if (expr->op == "")
		return 1;

	return 0;
}

int SemanticAnalyser::SemanticFor(std::unique_ptr<Stmt> stmt)
{
	return 0;
}

int SemanticAnalyser::SemanticTroop(std::unique_ptr<Stmt> stmt)
{
	return 0;
}

int SemanticAnalyser::SemanticVal(std::unique_ptr<Stmt> stmt)
{
	return 0;
}
