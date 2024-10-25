// AST.cpp
#include "ast.h"

// CompUnitAST 类的实现
void CompUnitAST::Dump() const {
    std::cout << "CompUnitAST { ";
    func_def->Dump();
    std::cout << " }";
}

void *CompUnitAST::toKoopaIR() const {
    std::vector<const void *> function{func_def->toKoopaIR()};
    koopa_raw_program_t *ir = new koopa_raw_program_t;
    ir->funcs = make_slice(&function, KOOPA_RSIK_FUNCTION);
    ir->values = make_slice(nullptr, KOOPA_RSIK_VALUE);
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

void *FuncDefAST::toKoopaIR() const {
    std::vector<const void *> blocks{block->toKoopaIR()};
    koopa_raw_function_data_t *ir = new koopa_raw_function_data_t;
    ir->name = add_prefix("@", ident.c_str());
    ir->ty = make_func_ty(nullptr, (const koopa_raw_type_kind *)func_type->toKoopaIR());
    ir->params = make_slice(nullptr, KOOPA_RSIK_VALUE);
    ir->bbs = make_slice(&blocks, KOOPA_RSIK_BASIC_BLOCK);
    return ir;
}

// FuncTypeAST 类的实现
void FuncTypeAST::Dump() const {
    std::cout << "FuncTypeAST { " << func_type << " }";
}

void *FuncTypeAST::toKoopaIR() const {
    if(func_type == "int") {
        koopa_raw_type_kind_t *ir = new koopa_raw_type_kind_t;
        ir->tag = KOOPA_RTT_INT32;
        return ir;
    }
    return nullptr;
}

// BlockAST 类的实现
void BlockAST::Dump() const {
    std::cout << "BlockAST { ";
    stmt->Dump();
    std::cout << " }";
}

void *BlockAST::toKoopaIR() const {
    koopa_raw_basic_block_data_t *ir = new koopa_raw_basic_block_data_t;
    ir->name = add_prefix("@", "entry");
    ir->params = make_slice(nullptr, KOOPA_RSIK_VALUE);
    ir->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
    std::vector<const void *> stmts;
    stmt->toKoopaIR(stmts);
    ir->insts = make_slice(&stmts, KOOPA_RSIK_VALUE);
    return ir;
}

// StmtAST 类的实现
void StmtAST::Dump() const {
    std::cout << "StmtAST { " << Return << ", " << number << " }";
}

void *StmtAST::toKoopaIR(std::vector<const void *> &stmts) const {
    koopa_raw_value_data_t *ir = new koopa_raw_value_data_t;

    ir->name = nullptr;
    ir->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
    ir->ty = make_ty(KOOPA_RTT_UNIT);
    ir->kind.tag = KOOPA_RVT_RETURN;

    koopa_raw_value_data_t *value = new koopa_raw_value_data_t;
    value->name = nullptr;
    value->ty = make_ty(KOOPA_RTT_INT32);
    value->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
    value->kind.tag = KOOPA_RVT_INTEGER;
    value->kind.data.integer.value = number;

    ir->kind.data.ret.value = value;
    stmts.push_back(ir);
    return ir;
}

// NumberAST 类的实现
void NumberAST::Dump() const {
    std::cout << "NumberAST { " << int_const << " }";
}
