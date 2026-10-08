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
		stmt->Print();
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
		break;
	case VAL_STMT:
		SemanticVal(stmt);
		break;
	case EXPR_STMT:
		SemanticFunction(stmt);
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

	if (!CheckValue(asnStmt->name))
	{
		ValInBlock(asnStmt->name, Type::TYPE_FLOAT);
	}
	else
		SemanticExpresion(asnStmt->expr.get());
}


void SemanticAnalyser::SemanticIf(Stmt* stmt)
{

	IfStmt* ifStmt = static_cast<IfStmt*>(stmt);

	SemanticExpresion(ifStmt->condition.get());

}

void SemanticAnalyser::SemanticWhile(Stmt* stmt)
{
	WhileStmt* whileStmt = static_cast<WhileStmt*>(stmt);
	SemanticExpresion(whileStmt->condition.get());
}

void SemanticAnalyser::SemanticFor(Stmt* stmt)
{
	ForStmt* forStmt = static_cast<ForStmt*>(stmt);

	actualStmt++;
	SemanticSwitch(forStmt->init.get());
	SemanticExpresion(forStmt->n.get());

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

	if (CheckValue(valStmt->name))
		Debug::FatalError("Var is allready initiate");
	else
		ValInBlock(valStmt->name, Type::TYPE_FLOAT);

}

void SemanticAnalyser::SemanticFunction(Stmt* stmt)
{
	ExpressionStmt* ExprStmt = static_cast<ExpressionStmt*>(stmt);
	SemanticExpresion(ExprStmt->expr.get());
}

void SemanticAnalyser::SemanticExpresion(Expr* expr)
{
	switch (expr->type)
	{
	case LITERAL:
		break;
	case VARIABLE:
		break;
	case GROUPING:
		SemanticGrouping(expr);
		break;
	case UNARY:
		break;
	case BINARY:
		SemanticBinary(expr);
		break;
	case CALL:
		break;
	default:
		Debug::FatalError("Expression not Instancied");
		break;
	}
}

void SemanticAnalyser::SemanticBinary(Expr* expr)
{
	Binary * binary = static_cast<Binary*>(expr);

	SemanticExpresion(binary->left.get());
	SemanticExpresion(binary->right.get());
}

void SemanticAnalyser::SemanticGrouping(Expr* expr)
{
	Grouping * group = static_cast<Grouping*>(expr);
	SemanticExpresion(group->expr.get());
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
			return true;
		}
		if (current_block == 0)
		{
			return false;
		}

		current_block--;
	}
	return false;
}
