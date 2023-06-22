#include "IRGenerator.hpp"

#include <iostream>

//----------------------------------------------------------------
static Function *getFunction(std::string Name, LLVMWrapper &builder)
{
    if (auto *F = builder.getModule().getFunction(Name))
        return F;

    // If not, check whether we can codegen the declaration from some existing prototype.
    auto FI = builder.m_FunctionSignatures.find(Name);
    if (FI != builder.m_FunctionSignatures.end())
        return FI->second->codegen(builder);

    return nullptr;
}

static AllocaInst *createEntryBlockAlloca(LLVMWrapper &builder, Function *theFunction,
                                          const std::string &varName, std::shared_ptr<VariableTypeAST> type)
{
    IRBuilder<> TmpB(&theFunction->getEntryBlock(), theFunction->getEntryBlock().begin());
    auto ty = type->codegen(builder);
    int arrSize = ty->isArrayTy() ? ty->getArrayNumElements() : 0;
    return TmpB.CreateAlloca(ty, ConstantInt::get(IntegerTypeAST::getType(builder), arrSize), varName);
}

static Value *getDefaultRetValue(LLVMWrapper &builder)
{
    return NumberValueAST::getNumber(builder, 0);
}

std::string FunctionAST::returnLabel = "functionReturn";
std::string LoopAST::afterLoopLabel = "after_loop";

//----------------------------------------------------------------
//----------------------------------------------------------------

llvm::Function *FunctionSignatureAST::codegen(LLVMWrapper &builder)
{
    //inits param types
    std::vector<Type *> F_Params;
    for (auto &[name, varType] : m_Args)
        F_Params.push_back(varType->codegen(builder));

    //sets return type - if function then exists, else void type for procedure, and params
    FunctionType *FT = FunctionType::get(this->RetType(builder), F_Params, false);

    //finalizes int function with name
    Function *F = Function::Create(FT, Function::ExternalLinkage, getName(), builder.getModule());

    unsigned Idx = 0;
    for (auto &Arg : F->args())
        Arg.setName(m_Args[Idx++].first);

    return F;
}

llvm::Type *FunctionSignatureAST::RetType(LLVMWrapper &builder)
{
    //generate type or void type if is a procedure
    return hasRetType() ? m_RetType->codegen(builder) : Type::getVoidTy(builder.getContext());
}

Value *FunctionBodyAST::codegen(LLVMWrapper &builder)
{
    for (auto &expr : m_Body)
        if (!expr->codegen(builder))
            return nullptr;

    return getDefaultRetValue(builder);
}

llvm::Function *FunctionAST::codegen(LLVMWrapper &builder)
{
    // Transfer ownership of the prototype to the signatures map, but keep a
    // reference to it for use below.
    auto &P = *m_Signature;
    builder.m_FunctionSignatures[m_Signature->getName()] = std::move(m_Signature);

    Function *theFunction = getFunction(P.getName(), builder);

    if (!theFunction->empty())
        throw "Function cannot be redefined.";

    //----------------------------------------------------

    //ignore forward declaration
    if (!m_Body)
        return theFunction;

    //----------------------------------------------------

    BasicBlock *BB = BasicBlock::Create(builder.getContext(), "entry", theFunction);
    BasicBlock *funcEndBB = BasicBlock::Create(builder.getContext(), FunctionAST::returnLabel, theFunction);
    builder.getBuilder().SetInsertPoint(BB);

    //----------------------------------------------------

    //sets new {local variables,
    //          function arguments
    //          return value which has same name as function}
    int setsOfNewVars = 0;
    LLVMWrapper::valueTable localVars;

    int i = 0;
    //create function arguments
    for (auto &Arg : theFunction->args())
    {
        auto &type = P.Args()[i].second;
        auto alloca = createEntryBlockAlloca(builder, theFunction, Arg.getName(), type);
        builder.getBuilder().CreateStore(&Arg, alloca);

        localVars[Arg.getName()] = {alloca, type};
        i++;
    }

    //set var for return func:= val
    if (P.hasRetType())
    {
        auto alloca = createEntryBlockAlloca(builder, theFunction, P.getName(), P.RetTypeAST());
        localVars[P.getName()] = {alloca, P.RetTypeAST()};
    }

    builder.InsertNewValues(std::move(localVars));
    setsOfNewVars++;

    //local vars and consts memory creation
    for (auto &x : m_LocalVals)
        if (x->codegen(builder))
            setsOfNewVars++;

    //-------------------------------------------------------------

    Value *retVal = m_Body->codegen(builder);

    if (!retVal)
    {
        theFunction->eraseFromParent();
        //remove out of scope local variables
        builder.RemoveLastVals(setsOfNewVars);

        return nullptr;
    }

    //-------------------------------------------------------------

    //set dedicated ending place for function in case exit is used
    builder.getBuilder().CreateBr(funcEndBB);
    builder.getBuilder().SetInsertPoint(funcEndBB);

    if (P.hasRetType())
    {
        auto retVal = builder.getBuilder().CreateLoad(builder.SearchValAlloca(P.getName()));
        builder.getBuilder().CreateRet(retVal);
    }
    else
        builder.getBuilder().CreateRetVoid();

    verifyFunction(*theFunction);

    //remove out of scope local variables
    builder.RemoveLastVals(setsOfNewVars);
    return theFunction;
}

//-------------------------------------------------------------------------

llvm::Value *VarDeclarationExpression::codegen(LLVMWrapper &builder)
{
    LLVMWrapper::valueTable newVars;

    Function *theFunction = builder.getBuilder().GetInsertBlock()->getParent();

    for (auto &x : m_Declarations)
    {
        const std::string &varName = x->m_VariableName;
        auto init = x->m_InitValue.get();

        auto alloca = createEntryBlockAlloca(builder, theFunction, varName, x->m_Type);
        newVars[varName] = {alloca, x->m_Type};

        Value *initVal = nullptr;
        if (init)
        {
            //generate user defined value for var
            initVal = init->codegen(builder);
            if (!initVal)
                throw "couldnt init local var\n";

            builder.getBuilder().CreateStore(initVal, alloca);
        }
        /*
        else
        {
            //give default value for variable = 0
            initVal = x->m_Type->getDefaultValue(builder);
        }
        */
    }
    int size = newVars.size();
    builder.InsertNewValues(std::move(newVars));

    return NumberValueAST::getNumber(builder, size);
}

bool VarDeclarationExpression::GenerateGlobalVars(LLVMWrapper &builder)
{
    LLVMWrapper::valueTable table;
    for (auto &x : m_Declarations)
    {
        builder.getModule().getOrInsertGlobal(x->m_VariableName, x->m_Type->codegen(builder));
        auto globalVar = builder.getModule().getNamedGlobal(x->m_VariableName);

        ConstantInt *initVal = nullptr;
        if (x->m_InitValue)
        {
            initVal = static_cast<ConstantInt *>(x->m_InitValue->codegen(builder));
            if (!initVal)
                throw "couldnt init global var\n";
        }
        else
        {
            //default init value
            initVal = static_cast<ConstantInt *>(x->m_Type->getDefaultValue(builder));
        }
        globalVar->setInitializer(initVal);

        table[x->m_VariableName] = {globalVar, x->m_Type};
    }

    builder.InsertNewValues(std::move(table));
    return true;
}

//-------------------------------------------------------------------------

llvm::Value *ConstdeclarationExpression::codegen(LLVMWrapper &builder)
{
    LLVMWrapper::valueTable consts;
    auto constType = std::make_shared<IntegerTypeAST>(true); //bool param sets isconstant=true, cant access alloca afterwards

    for (auto &[name, val] : m_Constants)
    {
        auto initVal = static_cast<ConstantInt *>(val->codegen(builder));
        if (!initVal)
            throw "init value for constant isnt a constant";

        consts[name] = {initVal, constType};
    }

    int size = consts.size();
    builder.InsertNewValues(std::move(consts));

    return NumberValueAST::getNumber(builder, size);
}

//-------------------------------------------------------------------------

llvm::Value *UnaryExprAST::codegen(LLVMWrapper &builder)
{
    Value *operand = m_Operand->codegen(builder);
    if (!operand)
        return nullptr;

    switch (m_OP)
    {
    case tok_not:
        /**
         * note true value is always 1, false is 0, all other numbers create invalid bool value/undefined behaviour
         * */
        return builder.getBuilder().CreateNeg(operand, "neg");
    case tok_minus:
        return builder.getBuilder().CreateMul(
            NumberValueAST::getNumber(builder, -1),
            operand,
            "multiply -1");
    }

    return operand;
}
//-------------------------------------------------------------------------

llvm::Value *convertToInt(LLVMWrapper &builder, Value *input)
{
    return builder.getBuilder().CreateIntCast(input, IntegerTypeAST::getType(builder), false);
}

llvm::Value *BinaryExprAST::codegen(LLVMWrapper &builder)
{
    if (m_OP == tok_assign)
        return BuildAssign(builder);

    auto L = m_LHS->codegen(builder);
    auto R = m_RHS->codegen(builder);
    if (!L || !R)
        return nullptr;

    switch (m_OP)
    {
    case tok_or:
        return builder.getBuilder().CreateOr(L, R, "or");
    case tok_and:
        return builder.getBuilder().CreateAnd(L, R, "and");
    case tok_xor:
        return builder.getBuilder().CreateXor(L, R, "xor");

    //bool types have 1 for true, 0 for false
    case tok_less:
        return convertToInt(builder, builder.getBuilder().CreateICmpSLT(L, R, "less"));
    case tok_lessEqual:
        return convertToInt(builder, builder.getBuilder().CreateICmpSLE(L, R, "lessEq"));
    case tok_greater:
        return convertToInt(builder, builder.getBuilder().CreateICmpSGT(L, R, "greater"));
    case tok_greaterEqual:
        return convertToInt(builder, builder.getBuilder().CreateICmpSGE(L, R, "greaterEq"));
    case tok_equal:
        return convertToInt(builder, builder.getBuilder().CreateICmpEQ(L, R, "equal"));
    case tok_notEqual:
        return convertToInt(builder, builder.getBuilder().CreateICmpNE(L, R, "equal"));

    case tok_plus:
        return builder.getBuilder().CreateAdd(L, R, "add");
    case tok_minus:
        return builder.getBuilder().CreateSub(L, R, "sub");
    case tok_multiply:
        return builder.getBuilder().CreateMul(L, R, "multiply");
    case tok_div:
        return builder.getBuilder().CreateSDiv(L, R, "div");
    case tok_mod:
        return builder.getBuilder().CreateSRem(L, R, "remainder");
    }

    return nullptr;
}

llvm::Value *BinaryExprAST::BuildAssign(LLVMWrapper &builder)
{
    ReferenceAST *LHS = dynamic_cast<ReferenceAST *>(m_LHS.get());
    if (!LHS)
        throw "LHS of := operator must be a variable\n";

    auto var = LHS->getAlloca(builder);
    if (!var)
        throw "undefined variable\n";

    auto newVal = m_RHS->codegen(builder);
    if (!newVal)
        throw "RHS isnt a valid value\n";

    builder.getBuilder().CreateStore(newVal, var);

    return newVal;
}

//-------------------------------------------------------------------------

ConstantInt *NumberValueAST::getNumber(LLVMWrapper &builder, int number)
{
    return ConstantInt::get(builder.getContext(), APInt(NumberValueAST::bits, number, true));
}

llvm::Value *NumberValueAST::codegen(LLVMWrapper &builder)
{
    return getNumber(builder, m_Value);
}

//-------------------------------------------------------------------------

Value *VariableReferenceAST::getAlloca(LLVMWrapper &builder)
{
    auto alloca = builder.SearchValAlloca(getName());
    if (!alloca)
        throw "accessing undeclared variable\n";

    return alloca;
}

llvm::Value *VariableReferenceAST::codegen(LLVMWrapper &builder)
{
    auto var = builder.GetVal(getName());

    if (!var)
        throw "accessing undeclared variable\n";

    return var;
}

//-------------------------------------------------------------------------

Value *ArrayReferenceExprAST::getAlloca(LLVMWrapper &builder)
{
    LLVMWrapper::valTableItem info;

    if (!builder.GetItem(getName(), info))
        throw "referencing undeclared array";

    //typecast to array type, and generate Value* for lowest possible index
    auto lowIndexVal = dynamic_cast<ArrayTypeAST *>(info.second.get())->m_LowIndex->codegen(builder);
    auto offset = builder.getBuilder().CreateSub(m_Arg->codegen(builder), lowIndexVal);

    std::vector<Value *> idxList;
    idxList.push_back(ConstantInt::get(Type::getInt32Ty(builder.getContext()), 0));
    idxList.push_back(offset);

    return builder.getBuilder().CreateGEP(info.first, idxList);
}

llvm::Value *ArrayReferenceExprAST::codegen(LLVMWrapper &builder)
{
    return builder.getBuilder().CreateLoad(getAlloca(builder));
}

//-------------------------------------------------------------------------
const std::vector<std::string> FuncCallExprAST::specialFunctions{
    //"writeln", "write",
    "readln", "inc", "dec"};

llvm::Value *FuncCallExprAST::codegen(LLVMWrapper &builder)
{
    if (std::find(specialFunctions.begin(), specialFunctions.end(), m_FunctionName) != specialFunctions.end())
        return codegenSpecialFunction(builder);

    Function *CalleeF = getFunction(m_FunctionName, builder);
    if (!CalleeF)
        throw "unknown function reference\n";

    if (CalleeF->arg_size() != m_Args.size())
        throw "Incorrect # arguments passed";

    std::vector<Value *> ArgsV;
    for (auto &x : m_Args)
    {
        ArgsV.push_back(x->codegen(builder));
        if (!ArgsV.back())
            return nullptr;
    }

    return builder.getBuilder().CreateCall(CalleeF, ArgsV);
}

llvm::Value *FuncCallExprAST::codegenSpecialFunction(LLVMWrapper &builder)
{
    ReferenceAST *param = dynamic_cast<ReferenceAST *>(m_Args.front().get());
    if (!param)
        throw "param input isnt a variable\n";

    //get address of var to change its value
    auto alloca = param->getAlloca(builder);
    if (!alloca)
        throw "variable wasnt declared\n";

    if (m_FunctionName == "readln")
    {
        return builder.getBuilder().CreateCall(getFunction(m_FunctionName, builder), {alloca}, "readln into address");
    }
    else if (m_FunctionName == "dec" || m_FunctionName == "inc")
    {
        //+1 or -1
        Value *stepVal = NumberValueAST::getNumber(builder, m_FunctionName == "inc" ? 1 : -1);

        //loads value from memory
        Value *curVar = builder.getBuilder().CreateLoad(alloca, param->getName());

        //increments and stores value
        Value *nextVar = builder.getBuilder().CreateAdd(curVar, stepVal, "incremented/decremented");
        builder.getBuilder().CreateStore(nextVar, alloca);

        return nextVar;
    }

    return nullptr;
}
//-------------------------------------------------------------------------

llvm::Value *IfThenElseAST ::codegen(LLVMWrapper &builder)
{
    Function *theFunction = builder.getBuilder().GetInsertBlock()->getParent();
    BasicBlock *thenBB = BasicBlock::Create(builder.getContext(), "then", theFunction);
    BasicBlock *elseBB = BasicBlock::Create(builder.getContext(), "else", theFunction);
    BasicBlock *afterBB = BasicBlock::Create(builder.getContext(), "after_if", theFunction);

    //------------------------------------------------------------------

    auto condV = m_Cond->codegen(builder);
    if (!condV)
        return nullptr;

    condV = builder.getBuilder().CreateICmpEQ(condV,
                                              NumberValueAST::getNumber(builder, 1), "ifcond-if_EQ_1 - iftrue");

    builder.getBuilder().CreateCondBr(condV, thenBB, elseBB);

    //------------------------------------------------------------------

    //then body generation
    builder.getBuilder().SetInsertPoint(thenBB);
    if (!m_Then->codegen(builder))
        return nullptr;
    builder.getBuilder().CreateBr(afterBB);

    //------------------------------------------------------------------

    //else body genration
    builder.getBuilder().SetInsertPoint(elseBB);
    if (m_Else)
    {
        if (!m_Else->codegen(builder))
            return nullptr;
    }
    builder.getBuilder().CreateBr(afterBB);

    //------------------------------------------------------------------

    //set point for the rest of the code below
    builder.getBuilder().SetInsertPoint(afterBB);

    return getDefaultRetValue(builder);
}

llvm::Value *WhileLoopAST::codegen(LLVMWrapper &builder)
{

    Function *theFunction = builder.getBuilder().GetInsertBlock()->getParent();

    auto conditionBB = BasicBlock::Create(builder.getContext(), "condition", theFunction);
    auto loopBB = BasicBlock::Create(builder.getContext(), "loop", theFunction);
    auto afterLoopBB = BasicBlock::Create(builder.getContext(), LoopAST::afterLoopLabel, theFunction);

    //-----------------------------------------------------------

    //set label for condition checking place
    //generate checking
    builder.getBuilder().CreateBr(conditionBB);
    builder.getBuilder().SetInsertPoint(conditionBB);
    auto cond = m_Condition->codegen(builder);
    if (!cond)
        return nullptr;
    auto cmpCond = builder.getBuilder().CreateICmpEQ(cond, NumberValueAST::getNumber(builder, 1));

    //fork
    builder.getBuilder().CreateCondBr(cmpCond, loopBB, afterLoopBB);

    //-----------------------------------------------------------

    //generate while loop body
    builder.getBuilder().SetInsertPoint(loopBB);
    auto body = m_Body->codegen(builder);
    if (!body)
        return nullptr;
    //jump back to condition checking
    builder.getBuilder().CreateBr(conditionBB);

    //-----------------------------------------------------------

    //generate the rest below while loop
    builder.getBuilder().SetInsertPoint(afterLoopBB);
    return getDefaultRetValue(builder);
}

llvm::Value *ForLoopAST::codegen(LLVMWrapper &builder)
{
    Function *theFunction = builder.getBuilder().GetInsertBlock()->getParent();

    auto condBB = BasicBlock::Create(builder.getContext(), "condition", theFunction);
    auto loopBB = BasicBlock::Create(builder.getContext(), "loop", theFunction);
    auto afterBB = BasicBlock::Create(builder.getContext(), LoopAST::afterLoopLabel, theFunction);

    //----------------------------------------------------
    bool newLocal = false;

    {
        Value *initVal = m_InitVal ? m_InitVal->codegen(builder) : nullptr;

        //iterator doesnt exist yet, create a new one
        if (!builder.SearchValAlloca(m_IteratorVar))
        {
            if (!initVal)
                return nullptr;

            //initializes space for iterator variable
            auto alloca = createEntryBlockAlloca(builder, theFunction, m_IteratorVar, std::make_shared<IntegerTypeAST>());
            builder.getBuilder().CreateStore(initVal, alloca);

            auto itType = std::make_shared<IntegerTypeAST>();
            builder.InsertNewValues({{m_IteratorVar, {alloca, itType}}});

            newLocal = true;
        }
        //iterator exists - only overwrite value if defined right side
        else if (initVal)
        {
            builder.getBuilder().CreateStore(initVal, builder.SearchValAlloca(m_IteratorVar));
        }
    }
    //----------------------------------------------------
    {
        builder.getBuilder().CreateBr(condBB);
        builder.getBuilder().SetInsertPoint(condBB);
        Value *curVar = builder.getBuilder().CreateLoad(builder.SearchValAlloca(m_IteratorVar), m_IteratorVar);
        Value *endVal = m_EndVal->codegen(builder);

        Value *cond = nullptr;
        if (m_IterateUp)
            cond = builder.getBuilder().CreateICmpSLE(curVar, endVal, "loopcond_cur<=endval");
        else
            cond = builder.getBuilder().CreateICmpSGE(curVar, endVal, "loopcond_cur>=endval");

        // Insert the conditional branch for checking
        builder.getBuilder().CreateCondBr(cond, loopBB, afterBB);
    }

    //----------------------------------------------------

    {
        builder.getBuilder().SetInsertPoint(loopBB);

        if (!m_Body->codegen(builder))
        {
            //removes iterator
            builder.RemoveLastVals();
            return nullptr;
        }

        //loads value from memory to update in case iterator val was changed within loop body
        Value *curVar = builder.GetVal(m_IteratorVar);

        //increments and stores value
        Value *stepVal = NumberValueAST::getNumber(builder, m_IterateUp ? 1 : -1);
        Value *nextVar = builder.getBuilder().CreateAdd(curVar, stepVal, "next val in iteration");
        builder.getBuilder().CreateStore(nextVar, builder.SearchValAlloca(m_IteratorVar));

        //jump back to beginning to check condition
        builder.getBuilder().CreateBr(condBB);
    }

    //----------------------------------------------------

    // Any new code will be inserted in AfterBB.
    builder.getBuilder().SetInsertPoint(afterBB);

    if (newLocal)
    {
        //removes iterator which got out of scope
        builder.RemoveLastVals();
    }

    //return a value
    return getDefaultRetValue(builder);
}

llvm::Value *ExitExprAST::codegen(LLVMWrapper &builder)
{
    auto theFunction = builder.getBuilder().GetInsertBlock()->getParent();

    auto it = theFunction->getBasicBlockList().rbegin();
    auto end = theFunction->getBasicBlockList().rend();
    //find the last inserted block only
    for (; it != end; it++)
    {
        auto &block = *it;
        if (block.getName().startswith(FunctionAST::returnLabel))
        {
            //found the correct label to exit to
            builder.getBuilder().CreateBr(&block);

            //generate the rest after exit in its own block
            auto theRest = BasicBlock::Create(builder.getContext(), "after_exit", theFunction);
            builder.getBuilder().SetInsertPoint(theRest);
            break;
        }
    }

    return getDefaultRetValue(builder);
}

llvm::Value *BreakExprAST::codegen(LLVMWrapper &builder)
{
    auto theFunction = builder.getBuilder().GetInsertBlock()->getParent();

    auto it = theFunction->getBasicBlockList().rbegin();
    auto end = theFunction->getBasicBlockList().rend();
    //find the last inserted block only
    for (; it != end; it++)
    {
        auto &block = *it;
        if (block.getName().startswith(LoopAST::afterLoopLabel))
        {
            //found the correct label to exit to
            builder.getBuilder().CreateBr(&block);

            //generate the rest after break in its own block
            auto theRest = BasicBlock::Create(builder.getContext(), "after_break", theFunction);
            builder.getBuilder().SetInsertPoint(theRest);
            break;
        }
    }

    return getDefaultRetValue(builder);
}
//-------------------------------------------------------------------------
llvm::Type *IntegerTypeAST::getType(LLVMWrapper &builder)
{
    return Type::getInt32Ty(builder.getContext());
}

llvm::Type *IntegerTypeAST::codegen(LLVMWrapper &builder)
{
    return getType(builder);
}

Value *IntegerTypeAST::getDefaultValue(LLVMWrapper &builder)
{
    return getDefaultRetValue(builder);
}

//-------------------------------------------------------------------------

llvm::Type *ArrayTypeAST::codegen(LLVMWrapper &builder)
{
    return ArrayType::get(m_Type->codegen(builder), m_HighIndex->getVal() - m_LowIndex->getVal() + 1);
}

Value *ArrayTypeAST::getDefaultValue(LLVMWrapper &builder)
{
    //https://stackoverflow.com/questions/42255453/how-do-i-initialize-an-integer-array-in-llvm-using-a-list-of-integers
    return ConstantAggregateZero::get(m_Type->codegen(builder));
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

LLVMWrapper::LLVMWrapper()
    : m_MilaContext(), m_MilaBuilder(m_MilaContext), m_MilaModule("mila", m_MilaContext)

/*,m_FPM(std::make_unique<legacy::FunctionPassManager>(m_MilaModule)))*/ {}

//-------------------------------------------------------------------------

void LLVMWrapper::InsertNewValues(valueTable &&newVals)
{
    m_LookUpTable.emplace_back(std::move(newVals));
}

void LLVMWrapper::RemoveLastVals(int amount)
{
    for (int i = 0; i < amount; i++)
        m_LookUpTable.pop_back();
}

Value *LLVMWrapper::SearchValAlloca(const std::string &name)
{
    for (auto &table : m_LookUpTable)
    {
        auto ref = table.find(name);
        if (ref == table.end())
            continue;

        auto &item = ref->second;
        auto &type = item.second;
        if (type->isConstant())
            throw "accessing memory of a constant";

        return item.first;
    }

    return nullptr;
}

Value *LLVMWrapper::GetVal(const std::string &name)
{
    for (auto &table : m_LookUpTable)
    {
        auto ref = table.find(name);
        if (ref == table.end())
            continue;

        auto &item = ref->second;
        auto &type = item.second;
        if (type->isConstant()) //constants store their value directly
            return item.first;
        else //vars store their addresses, need to get value with load
            return getBuilder().CreateLoad(item.first, name);
    }

    return nullptr;
}

bool LLVMWrapper::GetItem(const std::string &name, valTableItem &ret)
{
    for (auto &table : m_LookUpTable)
    {
        auto ref = table.find(name);
        if (ref == table.end())
            continue;

        ret = ref->second;
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------

void LLVMWrapper::InitializeModuleAndPassManager()
{

    /*
  // Do simple "peephole" optimizations and bit-twiddling optzns.
  m_FPM->add(createInstructionCombiningPass());
  // Reassociate expressions.
  m_FPM->add(createReassociatePass());
  // Eliminate Common SubExpressions.
  m_FPM->add(createGVNPass());
  // Simplify the control flow graph (deleting unreachable blocks, etc).
  m_FPM->add(createCFGSimplificationPass());

  m_FPM->doInitialization();
*/
}
