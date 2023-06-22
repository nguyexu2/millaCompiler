#include "Parser.hpp"


//---------------------------------------------------------------------

int Parser::getNextToken()
{
    return m_CurTok = m_Lexer.Gettok();
}

void Parser::consumeToken(Token expected)
{
    if (expected != m_CurTok)
    {
        std::string output = "parsing error: expected " + Lexer::tokToString(expected) + " got " + Lexer::tokToString((Token)m_CurTok) + "\n";
        std::cerr << output;
        throw output.c_str();
    }

    getNextToken();
}

int Parser::GetTokPrecedence()
{
    auto x = BinopPrecedence.find((Token)m_CurTok);
    if (x == BinopPrecedence.end())
        return -1;

    return x->second;
}

//---------------------------------------------------------------------

bool Parser::Parse()
{
    /*
    while(m_CurTok != tok_eof)
    {
        std::cerr << Lexer::tokToString((Token)m_CurTok) << " ";
        getNextToken();
    }
    return true;
    */

    try
    {
        if (!ParseHeader())
            return false;
        //std::cerr << "done header\n";

        if (!ParseGlobal())
            return false;
        //std::cerr << "done global\n";

        if (!ParseMain())
            return false;
        //std::cerr << "done main\n";
    }
    catch (const char *e)
    {
        std::cerr << e;
        return false;
    }

    return true;
}

bool Parser::ParseHeader()
{
    consumeToken(tok_program);

    m_ProgramName = m_Lexer.IdentifierStr();
    consumeToken(tok_identifier);

    m_Generator.getModule().setSourceFileName(m_ProgramName);
    m_Generator.getModule().setModuleIdentifier(m_ProgramName);

    consumeToken(tok_semicolon);

    return true;
}

bool Parser::ParseGlobal()
{
    while (m_CurTok != tok_begin)
    {
        switch (m_CurTok)
        {
        case tok_var:
            m_GlobalDeclarations.emplace_back(ParseVarExpr());
            break;
        case tok_const:
            m_GlobalDeclarations.emplace_back(ParseConstExpr());
            break;
        case tok_function:
        case tok_procedure:
            m_GlobalDeclarations.emplace_back(ParseFunctionDeclaration(m_CurTok == tok_function));
            break;
        case tok_semicolon:
            consumeToken(tok_semicolon);
            break;
        default:
            return false;
        }
    }

    return true;
}

bool Parser::ParseMain()
{
    m_Main = ParseFunctionBody();
    consumeToken(tok_dot);

    return m_CurTok == tok_eof;
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------

std::unique_ptr<FunctionAST> Parser::ParseFunctionDeclaration(bool hasRetType)
{
    if (m_CurTok == tok_function || m_CurTok == tok_procedure)
        getNextToken();

    auto signature = ParseSignature(hasRetType);
    consumeToken(tok_semicolon);

    if (m_CurTok == tok_forward)
    {
        consumeToken(tok_forward);
        return std::make_unique<FunctionAST>(std::move(signature));
    }

    std::vector<std::unique_ptr<ExpressionAST>> localVars;
    while (m_CurTok == tok_var || m_CurTok == tok_const)
    {
        if (m_CurTok == tok_var)
            localVars.emplace_back(ParseVarExpr());
        else if (m_CurTok == tok_const)
            localVars.emplace_back(ParseConstExpr());
    }

    auto body = ParseFunctionBody();

    return std::make_unique<FunctionAST>(std::move(signature), std::move(localVars), std::move(body));
}

std::unique_ptr<FunctionSignatureAST> Parser::ParseSignature(bool hasRetType)
{
    std::string funcName = m_Lexer.IdentifierStr();
    consumeToken(tok_identifier);

    std::vector<std::pair<std::string, std::shared_ptr<VariableTypeAST>>> args;

    consumeToken(tok_leftBracket);
    if (m_CurTok != tok_rightBracket)
    {
        while (true)
        {
            std::string paramName = m_Lexer.IdentifierStr();
            consumeToken(tok_identifier);
            consumeToken(tok_colon);

            args.push_back({paramName, ParseVarType()});

            if (m_CurTok == tok_rightBracket)
                break;

            consumeToken(tok_semicolon);
        }
    }
    consumeToken(tok_rightBracket);

    std::shared_ptr<VariableTypeAST> funcRetType = nullptr;
    if (hasRetType)
    {
        consumeToken(tok_colon);
        funcRetType = ParseVarType();
    }

    return std::make_unique<FunctionSignatureAST>(funcName, std::move(args), funcRetType);
}

//---------------------------------------------------------------------

std::unique_ptr<FunctionBodyAST> Parser::ParseFunctionBody()
{
    consumeToken(tok_begin);
    //vector of expressions in order of execution
    std::vector<std::unique_ptr<ExpressionAST>> functionBody;

    while (m_CurTok != tok_end)
    {
        auto ret = ParseExpression();

        //isnt an expression, not this method's job to parse
        if (!ret)
            break;

        functionBody.emplace_back(std::move(ret));

        if (m_CurTok == tok_semicolon)
            consumeToken(tok_semicolon);
    }

    consumeToken(tok_end);
    return std::make_unique<FunctionBodyAST>(std::move(functionBody));
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------

std::unique_ptr<VarDeclarationExpression> Parser::ParseVarExpr()
{
    consumeToken(tok_var);
    std::vector<std::unique_ptr<VarDeclaration>> declarations;
    while (m_CurTok == tok_identifier)
    {
        auto decl = ParseVarDeclaration();

        //moves content of dec at the end of declarations
        declarations.insert(declarations.end(), std::make_move_iterator(decl.begin()), std::make_move_iterator(decl.end()));
        consumeToken(tok_semicolon);
    }

    return std::make_unique<VarDeclarationExpression>(std::move(declarations));
}

std::vector<std::unique_ptr<VarDeclaration>> Parser::ParseVarDeclaration()
{
    //pair first - var name
    //pair second - var value attached
    std::vector<std::pair<std::string, std::unique_ptr<ExpressionAST>>> varNames;

    while (true)
    {
        std::string name = m_Lexer.IdentifierStr();
        consumeToken(tok_identifier);

        std::unique_ptr<ExpressionAST> assignVal = nullptr;
        if (m_CurTok == tok_assign)
        {
            consumeToken(tok_assign);
            assignVal = ParseExpression();
            if (!assignVal)
                throw "couldnt parse assign value expression for var";
        }

        varNames.push_back({name, std::move(assignVal)});

        if (m_CurTok != tok_comma)
            break;

        consumeToken(tok_comma);
    }

    consumeToken(tok_colon);
    auto type = ParseVarType();

    std::vector<std::unique_ptr<VarDeclaration>> ret;

    for (auto &x : varNames)
        ret.push_back(std::make_unique<VarDeclaration>(x.first, std::move(x.second), type));

    return ret;
}

std::shared_ptr<VariableTypeAST> Parser::ParseVarType()
{
    switch (m_CurTok)
    {
    case tok_integer:
        consumeToken(tok_integer);
        return std::make_shared<IntegerTypeAST>();
    case tok_array:
        consumeToken(tok_array);

        consumeToken(tok_leftArrBracket);
        auto low = ParseNumberExpr();

        consumeToken(tok_dot);
        consumeToken(tok_dot);

        auto high = ParseNumberExpr();
        consumeToken(tok_rightArrBracket);

        consumeToken(tok_of);

        auto type = ParseVarType();

        return std::make_shared<ArrayTypeAST>(std::move(type), std::move(low), std::move(high));
    }

    return nullptr;
}

//---------------------------------------------------------------------

std::unique_ptr<ConstdeclarationExpression> Parser::ParseConstExpr()
{
    consumeToken(tok_const);
    std::vector<std::pair<std::string, std::unique_ptr<ExpressionAST>>> constVars;

    while (m_CurTok == tok_identifier)
    {
        std::string namedConst = m_Lexer.IdentifierStr();
        consumeToken(tok_identifier);

        consumeToken(tok_equal);

        auto val = ParseExpression();
        constVars.push_back({namedConst, std::move(val)});

        consumeToken(tok_semicolon);
    }

    return std::make_unique<ConstdeclarationExpression>(std::move(constVars));
    
    
    /*
    consumeToken(tok_const);
    std::vector<std::pair<std::string, int>> constVars;

    while (m_CurTok == tok_identifier)
    {
        std::string namedConst = m_Lexer.IdentifierStr();
        consumeToken(tok_identifier);

        consumeToken(tok_equal);

        int val = m_Lexer.NumVal();
        consumeToken(tok_number);

        constVars.push_back({namedConst, val});

        consumeToken(tok_semicolon);
    }

    return std::make_unique<ConstdeclarationExpression>(std::move(constVars));
    */
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------

std::unique_ptr<ExpressionAST> Parser::ParseExpression()
{
    auto LHS = ParseUnary();
    if (!LHS)
        return nullptr;

    return ParseBinaryOpRHS(0, std::move(LHS));
}

std::unique_ptr<ExpressionAST> Parser::ParseUnary()
{
    if (m_CurTok != tok_not && m_CurTok != tok_minus)
        return ParsePrimary();

    //ascii if not defined operator, negative int (Token value) if defined
    int op = m_CurTok;
    getNextToken();

    auto operand = ParseUnary();
    if (!operand)
        return nullptr;

    return std::make_unique<UnaryExprAST>(op, std::move(operand));
}

std::unique_ptr<ExpressionAST> Parser::ParseBinaryOpRHS(int precedence, std::unique_ptr<ExpressionAST> LHS)
{
    while (true)
    {
        int tokPrec = GetTokPrecedence();
        if (tokPrec < precedence)
            return LHS;

        int binOP = m_CurTok;
        getNextToken();

        auto RHS = ParseUnary();
        if (!RHS)
            return nullptr;

        int nextPrec = GetTokPrecedence();
        if (tokPrec < nextPrec)
        {
            RHS = ParseBinaryOpRHS(tokPrec + 1, std::move(RHS));
            if (!RHS)
                return nullptr;
        }

        LHS = std::make_unique<BinaryExprAST>(binOP, std::move(LHS), std::move(RHS));
    }
    return nullptr;
}

//---------------------------------------------------------------------

std::unique_ptr<ExpressionAST> Parser::ParsePrimary()
{
    switch (m_CurTok)
    {
    case tok_leftBracket:
        return ParseParenthesis();
    case tok_number:
        return ParseNumberExpr();
    case tok_identifier:
        return ParseIndentifExpr();
    case tok_if:
        return ParseIfElseExpr();
    case tok_while:
        return ParseWhileExpr();
    case tok_for:
        return ParseForExpr();
    case tok_var:
        return ParseVarExpr();
    case tok_const:
        return ParseConstExpr();
    case tok_exit:
        return ParseExitExpr();
    case tok_break:
        return ParseBreakExpr();
        /*
    default:
        std::string th = "unidentified token when parsing primary: got " + Lexer::tokToString((Token)m_CurTok) + "\n";
        throw th;
    */
    }

    return nullptr;
}

//---------------------------------------------------------------------

std::unique_ptr<ExpressionAST> Parser::ParseParenthesis()
{
    consumeToken(tok_leftBracket);

    auto ret = ParseExpression();
    if (!ret)
        return nullptr;

    consumeToken(tok_rightBracket);
    return ret;
}

std::unique_ptr<NumberValueAST> Parser::ParseNumberExpr()
{
    int num = 1;
    if(m_CurTok == tok_minus)
    {
        num = -1;
        consumeToken(tok_minus);
    }
    
    num *= m_Lexer.NumVal();
    consumeToken(tok_number);
    return std::make_unique<NumberValueAST>(num);
}

std::unique_ptr<ExitExprAST> Parser::ParseExitExpr()
{
    consumeToken(tok_exit);
    return std::make_unique<ExitExprAST>();
}

std::unique_ptr<BreakExprAST> Parser::ParseBreakExpr()
{
    consumeToken(tok_break);
    return std::make_unique<BreakExprAST>();
}

//---------------------------------------------------------------------

std::unique_ptr<ExpressionAST> Parser::ParseIndentifExpr()
{
    std::string name = m_Lexer.IdentifierStr();
    consumeToken(tok_identifier);

    switch (m_CurTok)
    {
    case tok_leftBracket: //function call
        return ParseFunctionCallExpr(name);
    case tok_leftArrBracket: // array reference
        return ParseArrayReferenceExpr(name);
    default:
        return std::make_unique<VariableReferenceAST>(name);
    }
}

std::unique_ptr<ExpressionAST> Parser::ParseFunctionCallExpr(const std::string &funcName)
{
    consumeToken(tok_leftBracket);

    std::vector<std::unique_ptr<ExpressionAST>> args;
    //parse arguments of function call
    if (m_CurTok != tok_rightBracket)
    {
        while (true)
        {
            auto arg = ParseExpression();
            if (!arg)
                throw "unexpected token while parsing function call argument expression";

            args.push_back(std::move(arg));

            if (m_CurTok == tok_rightBracket)
                break;

            consumeToken(tok_comma);
        }
    }

    consumeToken(tok_rightBracket);

    return std::make_unique<FuncCallExprAST>(funcName, std::move(args));
}

std::unique_ptr<ExpressionAST> Parser::ParseArrayReferenceExpr(const std::string &arrayName)
{
    consumeToken(tok_leftArrBracket);

    auto arg = ParseExpression();
    if (!arg)
        throw "unexpected token while parsing argument for array referencing";
    consumeToken(tok_rightArrBracket);

    return std::make_unique<ArrayReferenceExprAST>(arrayName, std::move(arg));
}

//---------------------------------------------------------------------

std::unique_ptr<IfThenElseAST> Parser::ParseIfElseExpr()
{
    consumeToken(tok_if);
    auto cond = ParseExpression();
    if (!cond)
        throw "cant parse condition expression for if statement";

    consumeToken(tok_then);
    auto thenBody = ParseThenBlockExpr();
    if (!thenBody)
        throw "cant parse then body for of condition";

    //else-less if statement
    if (m_CurTok != tok_else)
        return std::make_unique<IfThenElseAST>(std::move(cond), std::move(thenBody), nullptr);

    consumeToken(tok_else);
    auto elseBody = ParseThenBlockExpr();
    if (!elseBody)
        throw "cant parse then body for else condition";

    return std::make_unique<IfThenElseAST>(std::move(cond), std::move(thenBody), std::move(elseBody));
}

std::unique_ptr<FunctionBodyAST> Parser::ParseThenBlockExpr()
{
    if (m_CurTok != tok_begin)
    {
        auto expr = ParseExpression();
        if (!expr)
            return nullptr;

        return std::make_unique<FunctionBodyAST>(std::move(expr));
    }

    return ParseFunctionBody();
}

//---------------------------------------------------------------------

std::unique_ptr<WhileLoopAST> Parser::ParseWhileExpr()
{
    consumeToken(tok_while);
    auto condition = ParseExpression();
    consumeToken(tok_do);

    auto body = ParseThenBlockExpr();

    return std::make_unique<WhileLoopAST>(std::move(condition), std::move(body));
}

//---------------------------------------------------------------------

std::unique_ptr<ForLoopAST> Parser::ParseForExpr()
{
    consumeToken(tok_for);

    std::string iteratorVar = m_Lexer.IdentifierStr();
    consumeToken(tok_identifier);

    std::unique_ptr<ExpressionAST> initVal = nullptr;
    if(m_CurTok == tok_assign)
    {
        consumeToken(tok_assign);

        initVal = ParseExpression();
    }

    bool iterateUp = m_CurTok == tok_to;
    if (m_CurTok == tok_to || m_CurTok == tok_downto)
        getNextToken();
    else
        throw "unexpected token at parsing for cycle";
    auto endVal = ParseExpression();

    consumeToken(tok_do);

    auto body = ParseThenBlockExpr();
    return std::make_unique<ForLoopAST>(iteratorVar, std::move(initVal), std::move(endVal), iterateUp, std::move(body));
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------

void Parser::GenerateDemo()
{
    GenerateSysFunc();

    // create main function
    {
        FunctionType *FT = FunctionType::get(Type::getInt32Ty(m_Generator.getContext()), false);
        Function *MainFunction = Function::Create(FT, Function::ExternalLinkage, "main", m_Generator.getModule());

        // block
        BasicBlock *BB = BasicBlock::Create(m_Generator.getContext(), "entry", MainFunction);
        m_Generator.getBuilder().SetInsertPoint(BB);

        m_Generator.getBuilder().CreateCall(m_Generator.getModule().getFunction("writeln"), {ConstantInt::get(m_Generator.getContext(), APInt(32, 4254045))});

        // return 0
        m_Generator.getBuilder().CreateRet(ConstantInt::get(Type::getInt32Ty(m_Generator.getContext()), 0));
    }
}

const Module &Parser::Generate()
{
    //GenerateDemo();

    try
    {

        GenerateSysFunc();
        for(auto & x : m_GlobalDeclarations)
        {
            if( auto tmp = dynamic_cast<VarDeclarationExpression*> (x.get()))
                tmp->GenerateGlobalVars(m_Generator);
            else
                x->codegen(m_Generator);
        }

        GenerateMain();
    }
    catch(const char * e)
    {
        std::cerr << e;
        throw e;
    }
    return m_Generator.getModule();
}

void Parser::GenerateSysFunc()
{
    // create writeln function
    {
        std::vector<Type *> Ints(1, Type::getInt32Ty(m_Generator.getContext()));
        FunctionType *FT = FunctionType::get(Type::getInt32Ty(m_Generator.getContext()), Ints, false);
        Function *F = Function::Create(FT, Function::ExternalLinkage, "writeln", m_Generator.getModule());
        for (auto &Arg : F->args())
            Arg.setName("x");
    }

    //create readln function
    {
        PointerType *ptr = PointerType::get(Type::getInt32Ty(m_Generator.getContext()), 0);
        FunctionType *FT = FunctionType::get(Type::getInt32Ty(m_Generator.getContext()), ptr, false);
        Function *F = Function::Create(FT, Function::ExternalLinkage, "readln", m_Generator.getModule());
        for (auto &Arg : F->args())
            Arg.setName("x");
    }

    //create

}

void Parser::GenerateMain()
{
    FunctionType *FT = FunctionType::get(Type::getInt32Ty(m_Generator.getContext()), false);
    Function *MainFunction = Function::Create(FT, Function::ExternalLinkage, "main", m_Generator.getModule());

    // block
    BasicBlock *BB = BasicBlock::Create(m_Generator.getContext(), "entry", MainFunction);
    BasicBlock *returnBB = BasicBlock::Create(m_Generator.getContext(), FunctionAST::returnLabel, MainFunction);
    m_Generator.getBuilder().SetInsertPoint(BB);

    m_Main->codegen(m_Generator);

    m_Generator.getBuilder().CreateBr(returnBB);
    m_Generator.getBuilder().SetInsertPoint(returnBB);

    //verifyFunction(*MainFunction);
    // return 0
    m_Generator.getBuilder().CreateRet(NumberValueAST::getNumber(m_Generator, 0));
}