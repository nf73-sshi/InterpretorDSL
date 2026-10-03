#include "Debug.h"

void Debug::SyntaxError(Token* t, const char* message)
{
	std::cout << "Syntax Error at line " << t->pos.line << " | word : " << t->GetLexeme();

	std::cout << "\n" << message << std::endl;
	abort();
}
