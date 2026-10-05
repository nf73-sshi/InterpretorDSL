#include "Parser.h"
#include "Debug.h"

void Parser::Parse()
{
    while (current < tokens.size())
    {
        statements.push_back(std::move(ParseStmt()));
    }

    for (std::unique_ptr<Stmt>& stmt : statements)
    {
        stmt->Print();
    }
}



std::unique_ptr<Stmt> Parser::ParseStmt()
{
    std::unique_ptr<Stmt> stmt = nullptr;
    Token* current = Peek();

    switch (current->type)
    {
    case TokenType::IF:
        stmt = ParseIf();
        break;
    case TokenType::WHILE:
        stmt = ParseWhile();
        break;
    case TokenType::FOR:
        stmt = ParseFor();
        break;
    case TokenType::LBRACKET:
        stmt = ParseBlock();
        break;

    case TokenType::VAL:
        if (Next()->type == TokenType::IDENTIFIER)
        {
            stmt = ParseVal();
        }
        break;

    case TokenType::TROOP:
        if (Next()->type == TokenType::IDENTIFIER)
        {
            stmt = ParseTroop();
        }
        break;

    case TokenType::IDENTIFIER:
        if (Next()->type == TokenType::ASSIGN)
        {
            stmt = ParseAssignement();
        }
        break;
    }

    if (stmt == nullptr)
        Debug::FatalError("Invalid Statement !");

    return stmt;
}

std::unique_ptr<Stmt> Parser::ParseAssignement()
{
    Token* current = Advance(); //identifier
    Advance();

    return std::make_unique<AssignStmt>(current->GetLexeme(), ParseExpr());
}

std::unique_ptr<Stmt> Parser::ParseParam()
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

    return std::make_unique<ParamStmt>(val->GetLexeme(), ParseExpr());
}

std::unique_ptr<Stmt> Parser::ParseVal()
{
    Advance(); //Val
    Token* id = Advance(); //identifier
    Advance(); // =

    return std::make_unique<ValStmt>(id->GetLexeme(), ParseExpr());
}

std::unique_ptr<Stmt> Parser::ParseTroop()
{
    Advance(); //Troop
    Token* id = Advance(); //identifier
    Token* op = Advance(); //=

    if (op == nullptr || op->type != TokenType::ASSIGN)
    {
        Debug::SyntaxError(id, "EXPECTED ASSIGNEMENT ");
        return nullptr;
    }

    std::unique_ptr<TroopStmt> troop = std::make_unique<TroopStmt>(id->GetLexeme());

    for (int i = 0; i < 8; ++i) // There are 8 params in the troop 
    {
        troop->params.push_back(std::move(ParseParam()));
    }

    return troop;
}

std::unique_ptr<Stmt> Parser::ParseBlock()
{
    Advance();
    std::unique_ptr<BlockStmt> block = std::make_unique<BlockStmt>();

    while (true)
    {
        Token* current = Peek();

        if (current->type == TokenType::RBRACKET)
        {
            Advance();
            break;
        }

        block->statements.push_back(std::move(ParseStmt()));
    }

    return block;
}

std::unique_ptr<Stmt> Parser::ParseIf()
{
    Advance(); //IF
    std::unique_ptr<Expr> condition = ParseExpr();

    std::unique_ptr<Stmt> then = ParseStmt();

    Token* else_t = Advance();

    std::unique_ptr<Stmt> else_then = nullptr;

    if (else_t != nullptr && else_t->type == TokenType::ELSE)
    {
        else_then = ParseStmt();
    }

    return std::make_unique<IfStmt>(std::move(condition), std::move(then), std::move(else_then));
}

std::unique_ptr<Stmt> Parser::ParseWhile()
{
    Advance();
    std::unique_ptr<Expr> condition = ParseExpr();

    std::unique_ptr<Stmt> then = ParseStmt();

    return std::make_unique<WhileStmt>(std::move(condition), std::move(then));
}

std::unique_ptr<Stmt> Parser::ParseFor()
{
    Advance();
    std::unique_ptr<Expr> condition = ParseExpr();

    std::unique_ptr<Stmt> then = ParseStmt();

    return std::make_unique<ForStmt>(std::move(condition), std::move(then));
}

std::unique_ptr<Expr> Parser::ParseExpr(int min_binding_power)
{
    std::unique_ptr<Expr> left = Prefix();

    while (true)
    {
        Token* op = Peek();
        int bp = BindingPower(op);          // || 1, && 2, == != 3, < <= > >= 4, + - 5, * / % 6

        if (bp < min_binding_power || bp == 0)
            break;

        Advance();
        auto rhs = ParseExpr(bp + 1);       // bp + 1 : associativité à gauche
        left = std::make_unique<Binary>(std::move(left), op->GetLexeme(), std::move(rhs));
    }

    return left;
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

std::unique_ptr<Expr> Parser::Prefix()
{
    Token* current = Peek();

    if (current->type == TokenType::LPARENTHESIS)
    {
       Advance(); // (
       std::unique_ptr<Expr>  expr = ParseExpr(0);

       if (Peek()->type == TokenType::RPARENTHESIS)
       {
           Advance(); // )
           return std::make_unique<Grouping>(std::move(expr));
       }

       return nullptr;
    }

    switch (current->type)
    {
 

    case TokenType::NUMBER:
        Advance();
        return std::make_unique<Literal>(std::stof(current->GetLexeme()));

    case TokenType::TRUE:
        Advance();
        return std::make_unique<Literal>(1);

    case TokenType::FALSE:
        Advance();
        return std::make_unique<Literal>(0);

    case TokenType::IDENTIFIER:
        Advance();
        return std::make_unique<Variable>(current->GetLexeme());

    case TokenType::SUB:
    case TokenType::NOT:
        Advance();
        std::unique_ptr<Expr> right = ParseExpr(BindingPower(current));

        return std::make_unique<Unary>(current->GetLexeme(), std::move(right));
    }

    return nullptr;
}
