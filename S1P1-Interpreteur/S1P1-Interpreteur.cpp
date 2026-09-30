#include <iostream>
#include "Lexer.h"
#include <fstream>
#include <sstream>
#include <regex>

void identificateToken(std::string elt)
{
        if (elt.find("continue") != std::string::npos)
            std::cout << "CONTINUE found" << std::endl;

        else if (elt.find("Troop") != std::string::npos)
            std::cout << "TROOP found" << std::endl;

        else if (elt.find("elif") != std::string::npos)
            std::cout << "ELIF found" << std::endl;

        else if (elt.find("else") != std::string::npos)
            std::cout << "ELSE found" << std::endl;

        else if (elt.find("if") != std::string::npos)
            std::cout << "IF found" << std::endl;

        else if (elt.find("(") != std::string::npos)
            std::cout << "LPARENTHESIS found" << std::endl;

        else if (elt.find(")") != std::string::npos)
            std::cout << "RPARENTHESIS found" << std::endl;

        else if (elt.find(",") != std::string::npos)
            std::cout << "COMMA found" << std::endl;

        else if (elt.find(";") != std::string::npos)
            std::cout << "SEMICOLON found" << std::endl;

        else if (elt.find(":") != std::string::npos)
            std::cout << "COLON found" << std::endl;

        else if (elt.find("\"") != std::string::npos)
            std::cout << "QUOTE found" << std::endl;

}
int main()
{
    Lexer l;
    l.Tokenize("main.p2w");
   // l.Tokenize("main.p2w");
    std::string test = "if";   //[(] [,] [;] [:] [)] ["]
    identificateToken(test);
     test = "elif";  
    identificateToken(test);
     test = "else";   
    identificateToken(test);
     test = "Troop";   
    identificateToken(test);
    test = "continue";
    identificateToken(test);
    test = "(";
    identificateToken(test);
    test = ",";
    identificateToken(test);
    test = ";";
    identificateToken(test);
    test = ":";
    identificateToken(test);
    test = ":)";
    identificateToken(test);
    test = "\"";
    identificateToken(test);


}
