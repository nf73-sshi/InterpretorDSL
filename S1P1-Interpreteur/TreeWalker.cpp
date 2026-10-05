#include "TreeWalker.h"

void TreeWalker::Execute()
{
	for (std::unique_ptr<Stmt>& stmt : statements)
	{
		stmt->Evaluate();
	}

	for (std::unique_ptr<Stmt>& stmt : statements)
	{
		stmt->Execute();
	}
}