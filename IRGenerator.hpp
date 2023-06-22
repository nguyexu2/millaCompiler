#pragma once

#include <string>
#include <vector>

#include "Token.hpp"

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"

#include <map>
#include <string>
#include <vector>
#include <memory>

using namespace llvm;
//-------------------------------------------------------------------------
class LLVMWrapper;
//-------------------------------------------------------------------------

class NodeAST
{
public:
    virtual llvm::Value *codegen(LLVMWrapper& builder) = 0;
    virtual ~NodeAST(){};
};

class ExpressionAST: public NodeAST
{
};

class VariableTypeAST
{
protected:
    bool m_IsConstant;
public:
    void setIsConst(bool newVal){m_IsConstant = newVal;}
    bool isConstant()const{return m_IsConstant;}
    VariableTypeAST(bool constant = false) : m_IsConstant(constant){}

    virtual llvm::Type*codegen(LLVMWrapper& builder) = 0;
    virtual Value* getDefaultValue(LLVMWrapper& builder) = 0;
    virtual ~VariableTypeAST(){};
};
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
struct VarDeclaration
{
    std::string m_VariableName;
    std::unique_ptr<ExpressionAST> m_InitValue;
    std::shared_ptr<VariableTypeAST> m_Type;
    
    VarDeclaration(
        const std::string & variableName,
        std::unique_ptr<ExpressionAST> &&initValue,
        const std::shared_ptr<VariableTypeAST> &type)
        : m_VariableName(variableName), m_InitValue(std::move(initValue)), m_Type(type){}

};

class VarDeclarationExpression : public ExpressionAST
{
    std::vector<std::unique_ptr<VarDeclaration>> m_Declarations;
public:
    VarDeclarationExpression(std::vector<std::unique_ptr<VarDeclaration>> && declarations)
        : m_Declarations(std::move(declarations)) {}

    llvm::Value *codegen(LLVMWrapper& builder) override;
    bool GenerateGlobalVars(LLVMWrapper& builder);
};

class ConstdeclarationExpression : public ExpressionAST
{
    //first -> name
    //second -> attached value
    std::vector<std::pair<std::string, std::unique_ptr<ExpressionAST>>> m_Constants;

public:
    ConstdeclarationExpression(
        std::vector<std::pair<std::string, std::unique_ptr<ExpressionAST>>> && constants
    )
    : m_Constants(std::move(constants)){}

    llvm::Value *codegen(LLVMWrapper& builder) override;
};

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

class FunctionSignatureAST
{
    std::string m_Name;
    std::vector<std::pair<std::string, std::shared_ptr<VariableTypeAST>>> m_Args;
    std::shared_ptr<VariableTypeAST> m_RetType;

public:
    FunctionSignatureAST(const std::string &name, std::vector<std::pair<std::string, std::shared_ptr<VariableTypeAST>>> &&args, const std::shared_ptr<VariableTypeAST> & retType)
        : m_Name(name), m_Args(std::move(args)), m_RetType(retType) {}

    llvm::Function *codegen(LLVMWrapper& builder);

    std::vector<std::pair<std::string, std::shared_ptr<VariableTypeAST>>>& Args(){return m_Args;}
    std::shared_ptr<VariableTypeAST> RetTypeAST(){return m_RetType;};
    llvm::Type*RetType(LLVMWrapper& builder);
    bool hasRetType() const {return m_RetType? true : false;}

    const std::string &getName() const { return m_Name; }
};

class FunctionBodyAST
{
    std::vector<std::unique_ptr<ExpressionAST>> m_Body;

public:
    FunctionBodyAST(std::vector<std::unique_ptr<ExpressionAST>> &&body)
        : m_Body(std::move(body)) {}

    FunctionBodyAST(std::unique_ptr<ExpressionAST> &&body)
        {m_Body.emplace_back(std::move(body));}

    Value * codegen(LLVMWrapper& builder);
};

class FunctionAST : public NodeAST
{
    std::unique_ptr<FunctionSignatureAST> m_Signature;
    std::unique_ptr<FunctionBodyAST> m_Body;
    std::vector<std::unique_ptr<ExpressionAST>> m_LocalVals;
    
public:
    static std::string returnLabel;

    FunctionAST( std::unique_ptr<FunctionSignatureAST>&& signature, std::vector<std::unique_ptr<ExpressionAST>> && localVals = {},
                std::unique_ptr<FunctionBodyAST> &&body = nullptr)
        : m_Signature(std::move(signature)), m_LocalVals(std::move(localVals)), m_Body(std::move(body)){}

    llvm::Function *codegen(LLVMWrapper& builder) override;
};

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
class UnaryExprAST : public ExpressionAST
{
    int m_OP;
    std::unique_ptr<ExpressionAST> m_Operand;

public:
    UnaryExprAST(int Opcode, std::unique_ptr<ExpressionAST>&& Operand)
        : m_OP(Opcode), m_Operand(std::move(Operand)) {}

    llvm::Value *codegen(LLVMWrapper& builder) override;
};

//-------------------------------------------------------------------------
class BinaryExprAST : public ExpressionAST
{
    int m_OP; //saved as a token, otherwise as ascii if not a valid token
    std::unique_ptr<ExpressionAST> m_LHS, m_RHS;

    llvm::Value *BuildAssign(LLVMWrapper& builder);

public:
    BinaryExprAST(int Op, std::unique_ptr<ExpressionAST> &&LHS, std::unique_ptr<ExpressionAST> &&RHS)
        : m_OP(Op), m_LHS(std::move(LHS)), m_RHS(std::move(RHS)) {}

    llvm::Value *codegen(LLVMWrapper& builder) override;
};

//-------------------------------------------------------------------------

class NumberValueAST : public ExpressionAST
{
    int m_Value;
public:
    int getVal()const{return m_Value;}
    static const unsigned int bits = 32;
    static ConstantInt * getNumber(LLVMWrapper& builder, int number);

    NumberValueAST(int numberValue)
        : m_Value(numberValue){};

    llvm::Value *codegen(LLVMWrapper& builder) override;
};

//-------------------------------------------------------------------------
class ReferenceAST : public ExpressionAST
{
protected:
    std::string m_Name;
public:
    ReferenceAST(const std::string & name) :m_Name(name) {}

    const std::string & getName() const {return m_Name;}
    virtual Value* getAlloca(LLVMWrapper& builder) = 0;    
};

class VariableReferenceAST : public ReferenceAST
{

public:
    VariableReferenceAST(const std::string &name)
        : ReferenceAST(name){};

    llvm::Value *codegen(LLVMWrapper& builder) override;
    Value* getAlloca(LLVMWrapper& builder) override;    

};


class ArrayReferenceExprAST: public ReferenceAST
{
    std::unique_ptr<ExpressionAST> m_Arg;
public:
    ArrayReferenceExprAST(const std::string &arrayName, std::unique_ptr<ExpressionAST> &&arg)
        : ReferenceAST(arrayName), m_Arg(std::move(arg)) {}

    llvm::Value *codegen(LLVMWrapper& builder) override;
    Value* getAlloca(LLVMWrapper& builder) override;    
};

//-------------------------------------------------------------------------

class FuncCallExprAST : public ExpressionAST
{
    std::string m_FunctionName;
    std::vector<std::unique_ptr<ExpressionAST>> m_Args;

    static const std::vector<std::string> specialFunctions;
    llvm::Value * codegenSpecialFunction(LLVMWrapper& builder);

public:
    FuncCallExprAST(const std::string &functionName, std::vector<std::unique_ptr<ExpressionAST>> &&args)
        : m_FunctionName(functionName), m_Args(std::move(args)) {}

    llvm::Value *codegen(LLVMWrapper& builder) override;
};

//-------------------------------------------------------------------------

class ExitExprAST: public ExpressionAST
{
public:
    llvm::Value *codegen(LLVMWrapper& builder) override;
};

class BreakExprAST : public ExpressionAST
{
public:
    llvm::Value *codegen(LLVMWrapper& builder) override;
};

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

class IfThenElseAST : public ExpressionAST
{
    std::unique_ptr<ExpressionAST> m_Cond;
    std::unique_ptr<FunctionBodyAST> m_Then, m_Else;

public:
    IfThenElseAST(std::unique_ptr<ExpressionAST>&& Cond, std::unique_ptr<FunctionBodyAST>&& Then, std::unique_ptr<FunctionBodyAST>&& Else)
        : m_Cond(std::move(Cond)), m_Then(std::move(Then)), m_Else(std::move(Else)) {}

    llvm::Value *codegen(LLVMWrapper& builder) override;
};

//-----------------------------------------------------------------------
class LoopAST : public ExpressionAST
{
    public:
    static std::string afterLoopLabel;

};

class WhileLoopAST : public LoopAST
{
    std::unique_ptr<ExpressionAST> m_Condition;
    std::unique_ptr<FunctionBodyAST> m_Body;
public:

    WhileLoopAST(std::unique_ptr<ExpressionAST> && condition,
    std::unique_ptr<FunctionBodyAST> && body)
    : m_Condition(std::move(condition)), m_Body(std::move(body)){}

    llvm::Value *codegen(LLVMWrapper& builder) override;
};

class ForLoopAST : public LoopAST
{
    std::string m_IteratorVar;
    std::unique_ptr<ExpressionAST> m_InitVal, m_EndVal;
    bool m_IterateUp;
    std::unique_ptr<FunctionBodyAST> m_Body;

public:

    ForLoopAST(const std::string & iteratorVar,
            std::unique_ptr<ExpressionAST> && initVal,
            std::unique_ptr<ExpressionAST> && endVal,
            bool iterateUp, std::unique_ptr<FunctionBodyAST> && body)
            : m_IteratorVar(iteratorVar),
                m_InitVal(std::move(initVal)),
                m_EndVal(std::move(endVal)),
                m_IterateUp(iterateUp),
                m_Body(std::move(body)){}


    llvm::Value *codegen(LLVMWrapper& builder) override;
};
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

class IntegerTypeAST: public VariableTypeAST
{
public:

    IntegerTypeAST(bool constant = false)
        : VariableTypeAST(constant)
        {}
    static Type * getType(LLVMWrapper& builder);
    llvm::Type *codegen(LLVMWrapper& builder) override;
    Value* getDefaultValue(LLVMWrapper& builder) override;

};


class ArrayTypeAST: public VariableTypeAST
{
    //shared because more variables can be of same type
    //can be too space consuming if more variables are of type array
    std::shared_ptr<VariableTypeAST> m_Type;
    std::unique_ptr<NumberValueAST> m_LowIndex, m_HighIndex;

    friend Value* ArrayReferenceExprAST::getAlloca(LLVMWrapper& builder);
    
public:
    int size;
    ArrayTypeAST(const std::shared_ptr<VariableTypeAST> & type, std::unique_ptr<NumberValueAST> &&low, std::unique_ptr<NumberValueAST> && high, bool constant = false)
        : VariableTypeAST(constant), m_Type(type), m_LowIndex(std::move(low)), m_HighIndex(std::move(high)){}

    Value* getDefaultValue(LLVMWrapper& builder) override;
    llvm::Type *codegen(LLVMWrapper& builder) override;
};


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
enum VarType
{
    globalVar, constant, variable
};

class LLVMWrapper
{
public:
    LLVMWrapper();
    LLVMContext & getContext(){return m_MilaContext;}
    IRBuilder<> & getBuilder(){return m_MilaBuilder;}
    Module & getModule(){ return m_MilaModule;} 

    //TODO add optimazation
    //std::unique_ptr<legacy::FunctionPassManager> m_FPM;

    //---------------------------------------------------------------

    typedef std::pair<Value*, std::shared_ptr<VariableTypeAST>> valTableItem;
    typedef std::map<std::string, valTableItem> valueTable;
    enum TypeVal
    {
        constant, variable
    };
    void InsertNewValues(valueTable && newVals);
    void RemoveLastVals(int amount = 1);

    /**
     * returns address of value only, for reading needs to be loaded
     * */
    Value* SearchValAlloca(const std::string & name);

    /**
     * returns value of the variable
     * */
    Value* GetVal(const std::string & name);

    bool GetItem(const std::string & name, valTableItem & ret);
    //---------------------------------------------------------------
    std::map<std::string, std::unique_ptr<FunctionSignatureAST>> m_FunctionSignatures;
    //---------------------------------------------------------------

    void InitializeModuleAndPassManager();

private:
    LLVMContext m_MilaContext; //owns a lot of core LLVM data structures, such as the type and constant value tables
    IRBuilder<> m_MilaBuilder; //helper object that makes it easy to generate LLVM instructions
    Module m_MilaModule;       //construct that contains functions and global variables.
    

    std::vector<valueTable> m_LookUpTable;
};