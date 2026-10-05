#include "SemanticAnalyser.h"

void SemanticAnalyser::AnalyseStmt()
{
	for (std::unique_ptr<Stmt>& stmt : stmtList)
	{
		stmt->Print();

		switch (stmt->type)
		{
		case ASSIGN_STMT:
			SemanticAssign(stmt.get()); 
			break;
		case IF_STMT:
			SemanticIf(stmt.get());
			break;
		case WHILE_STMT:
			SemanticWhile(stmt.get());
			break;
		case FOR_STMT:
			SemanticFor(stmt.get());
			break;
		case TROOP_STMT:;
			SemanticTroop(stmt.get());
			break;
		case VAL_STMT:
			SemanticVal(stmt.get());
			break;
		default:
			break;
		}
	}
}
void SemanticAnalyser::SemanticAssign(Stmt* stmt)
{
	AssignStmt* asnStmt = static_cast<AssignStmt*>(stmt);
	ExprType type = asnStmt->expr.get()->type;

	if (type == ExprType::VARIABLE)
	{
		Variable* expr = static_cast<Variable*>(asnStmt->expr.get());
		if (expr->name == "")
			Debug::FatalError("value missing");
		else if (!idList.contains(expr->name))
			Debug::FatalError("Var allready create");
		return ;
	}
	else if (type == ExprType::GROUPING)
	{
		Grouping* grp = static_cast<Grouping*>(asnStmt->expr.get());

		if (grp->expr.get()->type != ExprType::VARIABLE)
			Debug::FatalError("wrong expression");
		Variable* expr = static_cast<Variable*>(grp->expr.get());
		if (expr->name == "")
			Debug::FatalError("value missing");
		else if (!idList.contains(expr->name))
			Debug::FatalError("Var allready create");
		return ;
	}
	else
		Debug::FatalError("wrong Statement");
}


void SemanticAnalyser::SemanticIf(Stmt* stmt)
{
	IfStmt* ifStmt = static_cast<IfStmt*>(stmt);
	ExprType type = ifStmt->condition.get()->type;

	if (type != ExprType::BINARY)
		Debug::FatalError("Wrong expression");
	Binary* expr = static_cast<Binary*>(ifStmt->condition.get());
	if (expr->left == nullptr)
		Debug::FatalError("value missing");
	if (expr->right == nullptr)
		Debug::FatalError("value missing");
	if (expr->op == "")
		Debug::FatalError("value missing");

	if (expr->left->type == ExprType::VARIABLE)
	{
		Variable* var = static_cast<Variable*>(expr->left.get());
		if (!idList.contains(var->name))
			Debug::FatalError("Var allready create");
	}
	if (expr->right->type == ExprType::VARIABLE )
	{
		Variable* var = static_cast<Variable*>(expr->left.get());
		if (!idList.contains(var->name))
			Debug::FatalError("Var allready create");
	}
	return ;
}

void SemanticAnalyser::SemanticWhile(Stmt* stmt)
{
	WhileStmt* whileStmt = static_cast<WhileStmt*>(stmt);
	ExprType type = whileStmt->condition.get()->type;

	if (type != ExprType::BINARY)
		Debug::FatalError("Wrong expression");
	Binary* expr = static_cast<Binary*>(whileStmt->condition.get());
	if (expr->left == nullptr)
		Debug::FatalError("value missing");
	if (expr->right == nullptr)
		Debug::FatalError("value missing");
	if (expr->op == "")
		Debug::FatalError("value missing");

	if (expr->left->type == ExprType::VARIABLE)
	{
		Variable* var = static_cast<Variable*>(expr->left.get());
		if (!idList.contains(var->name))
			Debug::FatalError("Var allready create");
	}
	if (expr->right->type == ExprType::VARIABLE)
	{
		Variable* var = static_cast<Variable*>(expr->left.get());
		if (!idList.contains(var->name))
			Debug::FatalError("Var allready create");
	}

	return ;
}

void SemanticAnalyser::SemanticFor(Stmt* stmt)
{
	ForStmt* whileStmt = static_cast<ForStmt*>(stmt);
	StmtType stmtType = whileStmt->init.get()->type;
	ExprType exprType;
	ValStmt* valStmt = static_cast<ValStmt*>(whileStmt->init.get());
	switch (stmtType)
	{
	case ASSIGN_STMT:

		if (valStmt->expr->type == ExprType::VARIABLE)
		{
			Variable* var = static_cast<Variable*>(valStmt->expr.get());
			if (!idList.contains(var->name))
				Debug::FatalError("Var allready create");
			else
				return;
		}
		else if (valStmt->expr->type == ExprType::LITERAL)
			return;
		else
			Debug::FatalError("Wrong expression ");
		break;
	case VAL_STMT:
		if (valStmt->expr->type == ExprType::VARIABLE)
		{
			Variable* var = static_cast<Variable*>(valStmt->expr.get());
			if (idList.contains(var->name))
				Debug::FatalError("Var allready create");
			else
			{
				idList[var->name] = actualID;
				actualID++;
				return ;
			}
		}
		else if (valStmt->expr->type == ExprType::LITERAL)
			return;
		else 
			Debug::FatalError("Wrong Expression");
		{

		}
		break;
	default:
		break;
	}

	return ;
}

void SemanticAnalyser::SemanticTroop(Stmt* stmt)
{
	TroopStmt* troopStmt = static_cast<TroopStmt*>(stmt);

	if (troopStmt->params.size() == 0)
		Debug::FatalError("not enough parameter in troop");
	std::unordered_map <std::string, bool > paramMap;
	for (std::unique_ptr<Stmt>& pstmt : troopStmt->params)
	{
		ParamStmt* paramstmt = static_cast<ParamStmt*>(pstmt.get());
		if (paramMap.contains(paramstmt->name))
			Debug::FatalError("parameter allready in troop");
		else
			paramMap[paramstmt->name] = true;
	}
	if (paramMap.size() < 8)
			Debug::FatalError("not enought parameter in troop");
}

void SemanticAnalyser::SemanticVal(Stmt* stmt)
{
	ValStmt* valStmt = static_cast<ValStmt*>(stmt);
	ExprType type = valStmt->expr.get()->type;


	if (type == ExprType::VARIABLE || type == ExprType::LITERAL)
	{
		if (valStmt->name == "")
			Debug::FatalError("Missing Value ");
		else if (type == ExprType::VARIABLE && idList.contains(valStmt->name))
			Debug::FatalError("Value Already initiate");
		idList[valStmt->name] = actualID;
		actualID++;
	}
	else
		Debug::FatalError("Wrong Expression");


}
