#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

#include "Lexer.h"
#include "Parser.h"
#include "SemanticAnalyser.h"
//

int main()
{
    Lexer lexer;
    lexer.Tokenize("main.p2w");

    Parser parser(lexer.tokens);
    parser.Parse();

    SemanticAnalyser sem(parser.statements);
    sem.AnalyseStmt();
}
