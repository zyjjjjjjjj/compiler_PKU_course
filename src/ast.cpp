// AST.cpp
#include "ast.h"

// CompUnitAST 类的实现
void CompUnitAST::Dump() const {
    std::cout << "CompUnitAST { ";
    func_def->Dump();
    std::cout << " }\n";
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
    ir->name = add_prefix("%", "entry");
    ir->params = make_slice(nullptr, KOOPA_RSIK_VALUE);
    ir->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
    std::vector<const void *> stmts;
    stmt->toKoopaIR(stmts);
    ir->insts = make_slice(&stmts, KOOPA_RSIK_VALUE);
    return ir;
}

// StmtAST 类的实现
void StmtAST::Dump() const {
    std::cout << "StmtAST { "<< Return << ", ";
    exp->Dump();
    std::cout << " }";
}

void *StmtAST::toKoopaIR(std::vector<const void *> &stmts) const {
    koopa_raw_value_data_t *ir = new koopa_raw_value_data_t;

    ir->name = nullptr;
    ir->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
    ir->ty = make_ty(KOOPA_RTT_UNIT);
    ir->kind.tag = KOOPA_RVT_RETURN;
/*
    koopa_raw_value_data_t *value = new koopa_raw_value_data_t;
    value->name = nullptr;
    value->ty = make_ty(KOOPA_RTT_INT32);
    value->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
    value->kind.tag = KOOPA_RVT_INTEGER;
    value->kind.data.integer.value = number; 
*/
    ir->kind.data.ret.value = (koopa_raw_value_data_t *)exp->toKoopaIR(stmts);
    stmts.push_back(ir);
    return ir;
}

void *ExpAST::toKoopaIR(std::vector<const void *> &stmts) const {
    return lor_exp->toKoopaIR(stmts);
}

void ExpAST::Dump() const {
    std::cout << "ExpAST { ";
    lor_exp->Dump();
    std::cout << " }";
}

// LOrExpAST 类的实现
void LOrExpAST::Dump() const {
    std::cout << "LOrExpAST { ";
    if(op == "||") {
        lor_exp->Dump();
        std::cout << " || ";
        land_exp->Dump();
    }
    else if(op == " ") {
        land_exp->Dump();
    }
    std::cout << " }";
}

void *LOrExpAST::toKoopaIR(std::vector<const void *> &stmts) const {
    koopa_raw_value_data_t *ir = new koopa_raw_value_data_t;
    if(op == "||") {
        ir->name = nullptr;
        ir->ty = make_ty(KOOPA_RTT_INT32);
        ir->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        ir->kind.tag = KOOPA_RVT_BINARY;
        ir->kind.data.binary.op = KOOPA_RBO_OR;
        ir->kind.data.binary.lhs = (koopa_raw_value_t)toBoolKoopaIR(lor_exp, stmts);
        ir->kind.data.binary.rhs = (koopa_raw_value_t)toBoolKoopaIR(land_exp, stmts);
        stmts.push_back(ir);
        return ir;
    }
    else if(op == " ") {
        return land_exp->toKoopaIR(stmts);
    }
    return nullptr;
}

void *LOrExpAST::toBoolKoopaIR(const std::unique_ptr<BaseAST> &exp, std::vector<const void *> &stmts) const {
    koopa_raw_value_data_t *ir = new koopa_raw_value_data_t;
    ir->name = nullptr;
    ir->ty = make_ty(KOOPA_RTT_INT32);
    ir->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
    ir->kind.tag = KOOPA_RVT_BINARY;
    ir->kind.data.binary.op = KOOPA_RBO_NOT_EQ;
    NumberAST zero(0);
    ir->kind.data.binary.lhs = (koopa_raw_value_t)zero.toKoopaIR();
    ir->kind.data.binary.rhs = (koopa_raw_value_t)exp->toKoopaIR(stmts);
    stmts.push_back(ir);
    return ir;
}

// LAndExpAST 类的实现
void LAndExpAST::Dump() const {
    std::cout << "LAndExpAST { ";
    if(op == "&&") {
        land_exp->Dump();
        std::cout << " && ";
        eq_exp->Dump();
    }
    else if(op == " ") {
        eq_exp->Dump();
    }
    std::cout << " }";
}

void *LAndExpAST::toKoopaIR(std::vector<const void *> &stmts) const {
    koopa_raw_value_data_t *ir = new koopa_raw_value_data_t;
    if(op == "&&") {
        ir->name = nullptr;
        ir->ty = make_ty(KOOPA_RTT_INT32);
        ir->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        ir->kind.tag = KOOPA_RVT_BINARY;
        ir->kind.data.binary.op = KOOPA_RBO_AND;
        ir->kind.data.binary.lhs = (koopa_raw_value_t)toBoolKoopaIR(land_exp, stmts);
        ir->kind.data.binary.rhs = (koopa_raw_value_t)toBoolKoopaIR(eq_exp, stmts);
        stmts.push_back(ir);
        return ir;
    }
    else if(op == " ") {
        return eq_exp->toKoopaIR(stmts);
    }
    return nullptr;
}

void *LAndExpAST::toBoolKoopaIR(const std::unique_ptr<BaseAST> &exp, std::vector<const void *> &stmts) const {
    koopa_raw_value_data_t *ir = new koopa_raw_value_data_t;
    ir->name = nullptr;
    ir->ty = make_ty(KOOPA_RTT_INT32);
    ir->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
    ir->kind.tag = KOOPA_RVT_BINARY;
    ir->kind.data.binary.op = KOOPA_RBO_NOT_EQ;
    NumberAST zero(0);
    ir->kind.data.binary.lhs = (koopa_raw_value_t)zero.toKoopaIR();
    ir->kind.data.binary.rhs = (koopa_raw_value_t)exp->toKoopaIR(stmts);
    stmts.push_back(ir);
    return ir;
}

// EqExpAST 类的实现
void EqExpAST::Dump() const {
    std::cout << "EqExpAST { ";
    if(op == "==" || op == "!=") {
        eq_exp->Dump();
        std::cout << " " << op << " ";
        rel_exp->Dump();
    }
    else if(op == " ") {
        rel_exp->Dump();
    }
    std::cout << " }";
}

void *EqExpAST::toKoopaIR(std::vector<const void *> &stmts) const {
    koopa_raw_value_data_t *ir = new koopa_raw_value_data_t;
    if(op != " ") {
        ir->name = nullptr;
        ir->ty = make_ty(KOOPA_RTT_INT32);
        ir->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        ir->kind.tag = KOOPA_RVT_BINARY;
        if(op == "==") {
            ir->kind.data.binary.op = KOOPA_RBO_EQ;
        }
        else if(op == "!=") {
            ir->kind.data.binary.op = KOOPA_RBO_NOT_EQ;
        }
        ir->kind.data.binary.lhs = (koopa_raw_value_t)eq_exp->toKoopaIR(stmts);
        ir->kind.data.binary.rhs = (koopa_raw_value_t)rel_exp->toKoopaIR(stmts);
        stmts.push_back(ir);
        return ir;
    }
    else if(op == " ") {
        return rel_exp->toKoopaIR(stmts);
    }
    return nullptr;
}

// RelExpAST 类的实现
void RelExpAST::Dump() const {
    std::cout << "RelExpAST { ";
    if(op == "<" || op == ">" || op == "<=" || op == ">=") {
        rel_exp->Dump();
        std::cout << " " << op << " ";
        add_exp->Dump();
    }
    else if(op == " ") {
        add_exp->Dump();
    }
    std::cout << " }";
}

void *RelExpAST::toKoopaIR(std::vector<const void *> &stmts) const {
    koopa_raw_value_data_t *ir = new koopa_raw_value_data_t;
    if(op != " ") {
        ir->name = nullptr;
        ir->ty = make_ty(KOOPA_RTT_INT32);
        ir->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        ir->kind.tag = KOOPA_RVT_BINARY;
        if(op == "<") {
            ir->kind.data.binary.op = KOOPA_RBO_LT;
        }
        else if(op == ">") {
            ir->kind.data.binary.op = KOOPA_RBO_GT;
        }
        else if(op == "<=") {
            ir->kind.data.binary.op = KOOPA_RBO_LE;
        }
        else if(op == ">=") {
            ir->kind.data.binary.op = KOOPA_RBO_GE;
        }
        ir->kind.data.binary.lhs = (koopa_raw_value_t)rel_exp->toKoopaIR(stmts);
        ir->kind.data.binary.rhs = (koopa_raw_value_t)add_exp->toKoopaIR(stmts);
        stmts.push_back(ir);
        return ir;
    }
    else if(op == " ") {
        return add_exp->toKoopaIR(stmts);
    }
    return nullptr;
}

// AddExpAST 类的实现
void AddExpAST::Dump() const {
    std::cout << "AddExpAST { ";
    if(op == "+" || op == "-") {
        add_exp->Dump();
        std::cout << " " << op << " ";
        mul_exp->Dump();
    }
    else if(op == " ") {
        mul_exp->Dump();
    }
    std::cout << " }";
}

void *AddExpAST::toKoopaIR(std::vector<const void *> &stmts) const {
    koopa_raw_value_data_t *ir = new koopa_raw_value_data_t;
    if(op != " ") {
        ir->name = nullptr;
        ir->ty = make_ty(KOOPA_RTT_INT32);
        ir->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        ir->kind.tag = KOOPA_RVT_BINARY;
        if(op == "+") {
            ir->kind.data.binary.op = KOOPA_RBO_ADD;
        }
        else if(op == "-") {
            ir->kind.data.binary.op = KOOPA_RBO_SUB;
        }
        ir->kind.data.binary.lhs = (koopa_raw_value_t)add_exp->toKoopaIR(stmts);
        ir->kind.data.binary.rhs = (koopa_raw_value_t)mul_exp->toKoopaIR(stmts);
        stmts.push_back(ir);
        return ir;
    }
    else if(op == " ") {
        return mul_exp->toKoopaIR(stmts);
    }
    return nullptr;
}

// MulExpAST 类的实现
void MulExpAST::Dump() const {
    std::cout << "MulExpAST { ";
    if(op == "*" || op == "/" || op == "%") {
        mul_exp->Dump();
        std::cout << " " << op << " ";
        unary_exp->Dump();
    }
    else if(op == " ") {
        unary_exp->Dump();
    }
    std::cout << " }";
}

void *MulExpAST::toKoopaIR(std::vector<const void *> &stmts) const {
    koopa_raw_value_data_t *ir = new koopa_raw_value_data_t;
    if(op != " ") {
        ir->name = nullptr;
        ir->ty = make_ty(KOOPA_RTT_INT32);
        ir->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        ir->kind.tag = KOOPA_RVT_BINARY;
        if(op == "*") {
            ir->kind.data.binary.op = KOOPA_RBO_MUL;
        }
        else if(op == "/") {
            ir->kind.data.binary.op = KOOPA_RBO_DIV;
        }
        else if(op == "%") {
            ir->kind.data.binary.op = KOOPA_RBO_MOD;
        }
        ir->kind.data.binary.lhs = (koopa_raw_value_t)mul_exp->toKoopaIR(stmts);
        ir->kind.data.binary.rhs = (koopa_raw_value_t)unary_exp->toKoopaIR(stmts);
        stmts.push_back(ir);
        return ir;
    }
    else if(op == " ") {
        return unary_exp->toKoopaIR(stmts);
    }
    return nullptr;
}

void *UnaryExpAST::toKoopaIR(std::vector<const void *> &stmts) const {
    koopa_raw_value_data_t *ir = new koopa_raw_value_data_t;
    if(tag == "OPEXP") {
        ir->name = nullptr;  
        ir->ty = make_ty(KOOPA_RTT_INT32);
        ir->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        ir->kind.tag = KOOPA_RVT_BINARY;
        if(unary_op == "-") {
            ir->kind.data.binary.op = KOOPA_RBO_SUB;
            NumberAST zero(0);
            ir->kind.data.binary.lhs = (koopa_raw_value_t)zero.toKoopaIR();
        }
        else if(unary_op == "+") {
            return unary_exp->toKoopaIR(stmts);
        }
        else if(unary_op == "!") {
            ir->kind.data.binary.op = KOOPA_RBO_EQ;
            NumberAST zero(0);
            ir->kind.data.binary.lhs = (koopa_raw_value_t)zero.toKoopaIR();
        }
        ir->kind.data.binary.rhs = (koopa_raw_value_t)unary_exp->toKoopaIR(stmts);
        stmts.push_back(ir);
        return ir;
    }
    else if(tag == "PEXP") {
        return primary_exp->toKoopaIR(stmts);
    }
    return nullptr;
}

void UnaryExpAST::Dump() const {
    std::cout << "UnaryExpAST { ";
    if(tag == "OPEXP") {
        std::cout << unary_op << " , ";
        unary_exp->Dump();
    }
    else if(tag == "PEXP") {
        primary_exp->Dump();
    }
    std::cout << " }";
}

void *PrimaryExpAST::toKoopaIR(std::vector<const void *> &stmts) const {
    if(tag == "EXP") {
        return exp->toKoopaIR(stmts);
    }
    else if(tag == "NUM") {
        return number->toKoopaIR();
    }
    return nullptr;
}

void PrimaryExpAST::Dump() const {
    std::cout << "PrimartExpAST { ";
    if(tag == "EXP") {
        exp->Dump();
    }
    else if(tag == "NUM") {
        number->Dump();
    }
    std::cout << " }";
}

// NumberAST 类的实现
NumberAST::NumberAST(int int_const) : int_const(int_const) {}

void NumberAST::Dump() const {
    std::cout << "NumberAST { " << int_const << " }";
}

void *NumberAST::toKoopaIR() const {
    koopa_raw_value_data_t *ir = new koopa_raw_value_data_t;
    ir->name = nullptr;
    ir->ty = make_ty(KOOPA_RTT_INT32);
    ir->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
    ir->kind.tag = KOOPA_RVT_INTEGER;
    ir->kind.data.integer.value = int_const;
    return ir;
}
