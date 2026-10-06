#include "TreeWalker.h"
#include "Parser.h"
#include "Environment.h"

void TreeWalker::Execute(Environment* e)
{
	e->AddBlock();
	for (std::unique_ptr<Stmt>& stmt : statements)
	{
		stmt->Execute(e);
	}
	e->AddBlock(true);
}