#include "Parser.h"

void Parser::Parse()
{
    while (m_actual < m_tokenList.size())
    {
        Token& current = peek();
        Token& next = Parser::next();

        switch (current.type)
        {
        case IDENTIFIER:
            switch (next.type)
            {
            case ASSIGN:
                parse_assign();
                break;
            }

            break;
        }
    }
}

Assign* Parser::parse_assign()
{
    Assign* result = new Assign(peek(), advance(), parse_expr());
    return result;
}

Expr* Parser::parse_expr(int min_binding_power)
{
    advance();
    Expr* left = prefix();                 // littéral, variable, appel, (expr), -x, !x
    
    while(true)
    {
        Token op = peek();
        int bp = binding_power(op);          // || 1, && 2, == != 3, < <= > >= 4, + - 5, * / % 6

        if (bp < min_binding_power || bp == 0)
            break;

        advance();
        auto rhs = parse_expr(bp + 1);       // bp + 1 : associativité à gauche
        left = new Binary(left, op, rhs);
    }

    return left;
}

int Parser::binding_power(Token op)
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

Expr* Parser::prefix()
{
    Token& current = peek();

    switch (current.type)
    {
    case TokenType::NUMBER:
        return new Literal(current);

    case TokenType::IDENTIFIER:
        return new Variable(current);

    }

    return nullptr;
}
