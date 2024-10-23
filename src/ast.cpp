// AST.cpp
#include "AST.h"

// CompUnitAST 类的实现
void CompUnitAST::Dump() const {
    std::cout << "CompUnitAST { ";
    func_def->Dump();
    std::cout << " }";
}

BaseIR *CompUnitAST::toKoopaIR() const {
    Program *ir = new Program();
    ir->function = std::unique_ptr<BaseIR>(func_def->toKoopaIR());
    return ir;
}

// FuncDefAST 类的实现
void FuncDefAST::Dump() const {
    std::cout << "FuncDefAST { ";
    func_type->Dump();
    std::cout << ", " << ident << ", ";
    block->Dump();
    std::cout << " }";
}

BaseIR *FuncDefAST::toKoopaIR() const {
    Function *ir = new Function();
    if(dynamic_cast<FuncTypeAST*>(func_type.get())->Int == "int") {
    ir->fun_type = "i32";
    }
    ir->fun_name = ident;
    ir->basic_block = std::unique_ptr<BaseIR>(block->toKoopaIR());
    return ir;
}

// FuncTypeAST 类的实现
void FuncTypeAST::Dump() const {
    std::cout << "FuncTypeAST { " << Int << " }";
}

// BlockAST 类的实现
void BlockAST::Dump() const {
    std::cout << "BlockAST { ";
    stmt->Dump();
    std::cout << " }";
}

BaseIR *BlockAST::toKoopaIR() const {
    BasicBlock *ir = new BasicBlock();
    ir->value = std::unique_ptr<BaseIR>(stmt->toKoopaIR());
    return ir;
}

// StmtAST 类的实现
void StmtAST::Dump() const {
    std::cout << "StmtAST { " << Return << ", " << number << " }";
}

BaseIR *StmtAST::toKoopaIR() const {
    Value *ir = new Value();
    ir->instruction = "ret";
    ir->number = number;
    return ir;
}

// NumberAST 类的实现
void NumberAST::Dump() const {
    std::cout << "NumberAST { " << int_const << " }";
}
