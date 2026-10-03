#include "Parser.h"

void Parser::Parse()
{
    while (current < tokens.size())
    {
        statements.push_back(ParseStmt());
    }
}

Stmt* Parser::ParseStmt()
{
    Stmt* stmt = nullptr;
    Token& current = Peek();

    switch (current.type)
    {
    case TokenType::IDENTIFIER:
        if (Next().type == TokenType::ASSIGN)
        {
            stmt = ParseAssignement();
        }
        break;
    }

    return stmt;
}

Expr* Parser::ParseExpr(int min_binding_power)
{
    Advance();
    Expr* left = Prefix();                  // littéral, variable, appel, (expr), -x, !x
    
    while(true)
    {
        Token op = Peek();
        int bp = BindingPower(op);          // || 1, && 2, == != 3, < <= > >= 4, + - 5, * / % 6

        if (bp < min_binding_power || bp == 0)
            break;

        Advance();
        auto rhs = ParseExpr(bp + 1);       // bp + 1 : associativité à gauche
        left = new Binary(left, op.lexeme.data(), rhs);
    }

    return left;
}

Stmt* Parser::ParseAssignement()
{
    return nullptr;
}

int Parser::BindingPower(Token op)
{
    TokenType type = op.type;

    if (type == EQUAL || type == NOTEQUAL)
        return 3;

    else if (type == INFEQUAL || type == SUPEQUAL || type == INF || type == SUP)
        return 4;

    else if (type == ADD || type == SUB)
        return 5;

    else if (type == MUL || type == DIV)
        return 6;

    return -1;
}

Expr* Parser::Prefix()
{
    Token& current = Peek();

    switch (current.type)
    {
    case TokenType::NUMBER:
        return new Literal(std::stof(current.lexeme.data()));

    case TokenType::IDENTIFIER:
        return new Variable(current.lexeme.data());

    case TokenType::SUB:
    case TokenType::NOT:
        return new Unary(current.lexeme.data(), nullptr);


    }

    return nullptr;
}
