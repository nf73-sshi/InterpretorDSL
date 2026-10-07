#include "SemanticAnalyser.h"

void SemanticAnalyser::AnalyseStmt()
{
	AddDatasBlock();
	for ( actualStmt = 0; actualStmt < stmtList.size(); actualStmt++)
	{
		SemanticSwitch(stmtList[actualStmt]);

	}
	AddDatasBlock(true);
}
void SemanticAnalyser::SemanticSwitch(Stmt* stmt)
{
	switch (stmt->type)
	{
	case ASSIGN_STMT:
		SemanticAssign(stmt);
		break;
	case IF_STMT:
		SemanticIf(stmt);
		break;
	case WHILE_STMT:
		SemanticWhile(stmt);
		break;
	case FOR_STMT:
		SemanticFor(stmt);
		break;
	case TROOP_STMT:;
		SemanticTroop(stmt);
		break;
	case VAL_STMT:
		SemanticVal(stmt);
		break;
	case EXPR_STMT:
		SemanticExpr(stmt);
		break;
	case BLOCK_STMT:
		SemanticBlock(stmt);
		break;
	default:
		break;
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
			ValInBlock(asnStmt->name, Type::TYPE_FLOAT);
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
			ValInBlock(asnStmt->name, Type::TYPE_FLOAT);
		return ;
	}
	else
		Debug::FatalError("wrong Statement");
}


void SemanticAnalyser::SemanticIf(Stmt* stmt)
{
	std::unordered_map< std::string, Type> typeMap;

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
		if (CheckValue(var->name))
			Debug::FatalError("Var left missing");
	}
	if (expr->right->type == ExprType::VARIABLE )
	{
		Variable* var = static_cast<Variable*>(expr->left.get());
		if (CheckValue(var->name))
			Debug::FatalError("Var right missing");
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
		if (CheckValue(var->name))
			Debug::FatalError("Var left missing");
	}
	if (expr->right->type == ExprType::VARIABLE)
	{
		Variable* var = static_cast<Variable*>(expr->left.get());
		if (CheckValue(var->name))
			Debug::FatalError("Var right missing");
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
				ValInBlock(valStmt->name, Type::TYPE_FLOAT);
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
			ValInBlock(valStmt->name, Type::TYPE_FLOAT);
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

void SemanticAnalyser::SemanticBlock(Stmt* stmt)
{
	AddDatasBlock();
	BlockStmt* blockStmt = static_cast<BlockStmt*>(stmt);
	for (std::unique_ptr<Stmt>& stmtInBlock : blockStmt->statements)
	{
		actualStmt++;
		SemanticSwitch(stmtInBlock.get());
	}
	AddDatasBlock(true);
}

void SemanticAnalyser::SemanticVal(Stmt* stmt)
{
	ValStmt* valStmt = static_cast<ValStmt*>(stmt);
	ExprType type = valStmt->expr.get()->type;


	if (type == ExprType::VARIABLE || type == ExprType::LITERAL || type == ExprType::CALL)
	{
		if (valStmt->name == "")
			Debug::FatalError("Missing Value ");

			ValInBlock(valStmt->name, Type::TYPE_FLOAT);
	}
	else
		Debug::FatalError("Wrong Expression");


}

void SemanticAnalyser::SemanticExpr(Stmt* stmt)
{
	ExpressionStmt* ExprStmt = static_cast<ExpressionStmt*>(stmt);
	ExprType type = ExprStmt->expr->type;
	if (type == ExprType::CALL)
	{
		Call* call = static_cast<Call*>(ExprStmt->expr.get());
		for (std::unique_ptr<Expr>& expr : call->params)
		{
			if (expr->type != ExprType::LITERAL || expr->type != ExprType::VARIABLE)
			{

			}
		}
	}
}

void SemanticAnalyser::AddDatasBlock(bool remove)
{
	if (remove == false)
		datasType.push_back(std::unordered_map<std::string, Type>());
	else
		datasType.pop_back();
}

bool SemanticAnalyser::MapExists(size_t block, std::string name) const
{
	if (block >= datasType.size())
		return false; //out of scope

	return datasType[block].contains(name);
}

void SemanticAnalyser::ValInBlock(std::string name, Type value)
{
	size_t current_block = datasType.size() - 1;

	while (current_block >= 0)
	{
		if (!MapExists(current_block, name))
		{
			datasType[current_block][name] = value;
			break;
		}
		else
			Debug::FatalError("Var allready create");

		current_block--;
	}
}

bool SemanticAnalyser::CheckValue(std::string name)
{
	size_t current_block = datasType.size() - 1;

	while (current_block >= 0)
	{
		if (MapExists(current_block, name))
		{
			return false;
		}

		current_block--;
	}
	return true;
}
