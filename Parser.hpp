#ifndef PJPPROJECT_PARSER_HPP
#define PJPPROJECT_PARSER_HPP

#include "Lexer.hpp"
#include "IRGenerator.hpp"

#include <vector>
#include <string>
#include <memory>
#include <map>
#include <utility>
#include <iostream>

using namespace llvm;

//-------------------------------------------------------------------------

class Parser
{
public:
    Parser(){getNextToken();}

    bool Parse();                   // parse
    const llvm::Module &Generate(); // generate
    void GenerateDemo();
private:
    /**
    * Simple token buffer.
    * m_CurTok is the current token the parser is looking at
    * getNextToken reads another token from the lexer and updates curTok with ts result
    * Every function in the parser will assume that m_CurTok is the cureent token that needs to be parsed
    * */
    int getNextToken();

    /**
     * checks for validity of current token against param
     * throws exception if not the same
     * 
     * also queues up next token into buffer
     * */
    void consumeToken(Token expected);

    int GetTokPrecedence();
    //----------------------------------------------------------------------------------

    bool ParseHeader();
    bool ParseGlobal();
    bool ParseMain();

    //----------------------------------------------------------------------------------
    std::unique_ptr<FunctionAST>ParseFunctionDeclaration(bool hasRetType = true);
    std::unique_ptr<FunctionSignatureAST>ParseSignature(bool hasRetType = true);

    std::unique_ptr<FunctionBodyAST> ParseFunctionBody();
    //----------------------------------------------------------------------------------
    std::unique_ptr<ExpressionAST> ParseUnary();
    std::unique_ptr<ExpressionAST> ParseBinaryOpRHS(int precedence, std::unique_ptr<ExpressionAST> LHS);

    std::unique_ptr<ExpressionAST> ParseParenthesis();
    std::unique_ptr<ExpressionAST> ParseExpression();
    std::unique_ptr<NumberValueAST> ParseNumberExpr();
    std::unique_ptr<ExitExprAST> ParseExitExpr();
    std::unique_ptr<BreakExprAST> ParseBreakExpr();


    /**
     * primary  -> Indent   
     *          -> Ident[]  // array element referencing
     *          -> Ident()  // function call
     * */
    std::unique_ptr<ExpressionAST> ParsePrimary();
    std::unique_ptr<ExpressionAST> ParseIndentifExpr();
    std::unique_ptr<ExpressionAST> ParseArrayReferenceExpr(const std::string &arrayName);
    std::unique_ptr<ExpressionAST> ParseFunctionCallExpr(const std::string &funcName);

    std::unique_ptr<VarDeclarationExpression> ParseVarExpr();
    std::vector<std::unique_ptr<VarDeclaration>> ParseVarDeclaration();
    std::shared_ptr<VariableTypeAST> ParseVarType();

    std::unique_ptr<ConstdeclarationExpression> ParseConstExpr();

    std::unique_ptr<IfThenElseAST> ParseIfElseExpr();
    std::unique_ptr<FunctionBodyAST> ParseThenBlockExpr();

    std::unique_ptr<WhileLoopAST> ParseWhileExpr();
    std::unique_ptr<ForLoopAST> ParseForExpr();
    //----------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------
    void GenerateSysFunc();
    void GenerateMain();
    
    //----------------------------------------------------------------------------------

    Lexer m_Lexer; // lexer is used to read tokens
    int m_CurTok;  // to keep the current token

    //----------------------------------------------------------------------------------
    std::string m_ProgramName;
    std::vector<std::shared_ptr<NodeAST>> m_GlobalDeclarations;
    std::shared_ptr<FunctionBodyAST> m_Main;
    //----------------------------------------------------------------------------------
    LLVMWrapper m_Generator;
};

//------------------------------------------------------

const std::map<Token, int> BinopPrecedence = {
    {tok_assign, 10},

    {tok_or, 20},

    {tok_and, 30},

    {tok_xor, 40},

    {tok_less, 50},
    {tok_lessEqual, 50},
    {tok_greater, 50},
    {tok_greaterEqual, 50},
    {tok_equal, 50},
    {tok_notEqual, 50},

    {tok_plus, 60},
    {tok_minus, 60},

    {tok_multiply, 70},
    {tok_div, 70},
    {tok_mod, 70}

};

#endif //PJPPROJECT_PARSER_HPP
