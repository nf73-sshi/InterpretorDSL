#include "Parser.h"
#include "Debug.h"

void Parser::Parse()
{
    while (current < tokens.size())
    {
        statements.push_back(ParseStmt());
    }

    for (Stmt* stmt : statements)
    {
        stmt->Print();
    }
}

Stmt* Parser::ParseStmt()
{
    Stmt* stmt = nullptr;
    Token* current = Peek();

    switch (current->type)
    {
    case TokenType::TROOP:
        if (Next()->type == TokenType::IDENTIFIER)
        {
            stmt = ParseTroop();
            break;
        }
    case TokenType::IDENTIFIER:
        if (Next()->type == TokenType::ASSIGN)
        {
            stmt = ParseAssignement();
            break;
        }
    }

    if (stmt == nullptr)
        Debug::FatalError("Invalid Statement !");

    return stmt;
}

Expr* Parser::ParseExpr(int min_binding_power)
{
    Expr* left = Prefix();
    
    while(true)
    {
        Token* op = Peek();
        int bp = BindingPower(op);          // || 1, && 2, == != 3, < <= > >= 4, + - 5, * / % 6

        if (bp < min_binding_power || bp == 0)
            break;

        Advance();
        auto rhs = ParseExpr(bp + 1);       // bp + 1 : associativité à gauche
        left = new Binary(left, op->GetLexeme(), rhs);
    }

    return left;
}

Stmt* Parser::ParseAssignement()
{
    Token* current = Advance(); //identifier
    Advance();

    AssignStmt* assign = new AssignStmt(current->GetLexeme(), ParseExpr());

    return assign;
}

Stmt* Parser::ParseParam()
{
    Token* val = Advance();

    if (val->type != TokenType::IDENTIFIER)
    {
        Debug::SyntaxError(val, "IDENTIFIER EXPECTED");
        return nullptr;
    }

    Token* colon = Advance();

    if (colon->type != TokenType::COLON)
    {
        Debug::SyntaxError(colon, "COLON EXPECTED");
        return nullptr;
    }

    ParamStmt* param = new ParamStmt(val->GetLexeme(), ParseExpr());
    return param;
}

Stmt* Parser::ParseTroop()
{
    Advance(); //Troop
    Token* id = Advance(); //identifier

    std::vector<ParamStmt*> params = {};

    Token* op = Advance(); //=

    if (op == nullptr || op->type != TokenType::ASSIGN)
    {
        Debug::SyntaxError(id, "EXPECTED ASSIGNEMENT ");
        return nullptr;
    }

    for (int i = 0; i < 8; ++i) // There are 8 params in the troop 
    {
        params.push_back(dynamic_cast<ParamStmt*>(ParseParam()));
    }
 
    TroopStmt* troop = new TroopStmt(id->GetLexeme(), params);

    return troop;
}

int Parser::BindingPower(Token* op)
{
    if (op == nullptr)
        return 0;

    TokenType type = op->type;

    if (type == EQUAL || type == NOTEQUAL)
        return 3;

    else if (type == INFEQUAL || type == SUPEQUAL || type == INF || type == SUP)
        return 4;

    else if (type == ADD || type == SUB)
        return 5;

    else if (type == MUL || type == DIV)
        return 6;

    return 0;
}

Expr* Parser::Prefix()
{
    Token* current = Peek();

    switch (current->type)
    {
    case TokenType::NUMBER:
        Advance();
        return new Literal(std::stof(current->GetLexeme()));

    case TokenType::TRUE:
        Advance();
        return new Literal(1);

    case TokenType::FALSE:
        Advance();
        return new Literal(0);

    case TokenType::IDENTIFIER:
        Advance();
        return new Variable(current->GetLexeme());

    case TokenType::SUB:
    case TokenType::NOT:
        Advance();
        Expr* right = ParseExpr(BindingPower(current));

        return new Unary(current->GetLexeme(), right);
    }

    return nullptr;
}
