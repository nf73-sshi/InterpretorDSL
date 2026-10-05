#include "SemanticAnalyser.h"

void SemanticAnalyser::AnalyseStmt()
{
	for (std::unique_ptr<Stmt>& stmt : stmtList)
	{
		int sementicCase;
		switch (stmt->type)
		{
		case ASSIGN_STMT:
			sementicCase = SemanticAssign(stmt.get());
			break;
		case IF_STMT:
			sementicCase = SemanticIf(stmt.get());
			break;
		case WHILE_STMT:
			sementicCase = SemanticWhile(stmt.get());
			break;
		case FOR_STMT:
			sementicCase = SemanticFor(stmt.get());
			break;
		case PARAM_STMT:
			sementicCase = SemanticParam(stmt.get());
			break;
		case TROOP_STMT:;
			sementicCase = SemanticTroop(stmt.get());
			break;
		case VAL_STMT:
			sementicCase = SemanticTroop(stmt.get());
			break;
		default:
			break;
			
		}
	}
}
int SemanticAnalyser::SemanticAssign(Stmt* stmt)
{
	AssignStmt* asnStmt = static_cast<AssignStmt*>(stmt);
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


int SemanticAnalyser::SemanticIf(Stmt* stmt)
{
	IfStmt* ifStmt = static_cast<IfStmt*>(stmt);
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

int SemanticAnalyser::SemanticWhile(Stmt* stmt)
{
	WhileStmt* whileStmt = static_cast<WhileStmt*>(stmt);
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

int SemanticAnalyser::SemanticFor(Stmt* stmt)
{
	TroopStmt* valStmt = static_cast<TroopStmt*>(stmt);

	if (valStmt->params.size() == 0)
		return 2;

	else
		return 0;
}

int SemanticAnalyser::SemanticTroop(Stmt* stmt)
{
	return 0;
}

int SemanticAnalyser::SemanticParam(Stmt* stmt)
{
	return 0;
}

int SemanticAnalyser::SemanticVal(Stmt* stmt)
{
	ValStmt* valStmt = static_cast<ValStmt*>(stmt);
	ExprType type = valStmt->expr.get()->type;

	if (type != ExprType::VARIABLE)
		return 2;
	
	Variable* var = static_cast<Variable*>(valStmt->expr.get());
	if (var->name == "")
		return 1;
	else
		return 0;
	
}
