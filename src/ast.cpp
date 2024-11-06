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
    for(auto &block_item : *block_item_array) {
        block_item->Dump();
    }
    std::cout << " }";
}

void *BlockAST::toKoopaIR() const {
    std::vector<const void *> stmts;
    for(auto &block_item : *block_item_array) {
        block_item->toKoopaIR(stmts);
    }
    koopa_raw_basic_block_data_t *ir = new koopa_raw_basic_block_data_t;
    ir->name = add_prefix("%", "entry");
    ir->params = make_slice(nullptr, KOOPA_RSIK_VALUE);
    ir->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
    ir->insts = make_slice(&stmts, KOOPA_RSIK_VALUE);
    return ir;
}

// BlockItemAST 类的实现
void BlockItemAST::Dump() const {
    std::cout << "BlockItemAST { ";
    if(block_item_type == "DECL") {
        decl->Dump();
    }
    else if(block_item_type == "STMT") {
        stmt->Dump();
    }
    std::cout << " }";
}

void *BlockItemAST::toKoopaIR(std::vector<const void *> &stmts) const {
    if(block_item_type == "DECL") {
        return decl->toKoopaIR(stmts);
    }
    else if(block_item_type == "STMT") {
        return stmt->toKoopaIR(stmts);
    }
    return nullptr;
}


void DeclAST::Dump() const {
    std::cout << "DeclAST { ";
    const_decl->Dump();
    std::cout << " }";
}

void *DeclAST::toKoopaIR(std::vector<const void *> &stmts) const {
    return const_decl->toKoopaIR(stmts);
}

void ConstDeclAST::Dump() const {
    std::cout << "ConstDeclAST { ";
    type->Dump();
    for(auto &const_def : *const_def_array) {
        const_def->Dump();
    }
    std::cout << " }";
}

void *ConstDeclAST::toKoopaIR(std::vector<const void *> &stmts) const {
    for(auto &const_def : *const_def_array) {
        const_def->toKoopaIR(stmts);
    }
    return nullptr;
}

void BTypeAST::Dump() const {
    std::cout << "BTypeAST { " << type << " }";
}

void *BTypeAST::toKoopaIR() const {
    if(type == "int") {
        koopa_raw_type_kind_t *ir = new koopa_raw_type_kind_t;
        ir->tag = KOOPA_RTT_INT32;
        return ir;
    }
    return nullptr;
}

void ConstDefAST::Dump() const {
    std::cout << "ConstDefAST { ";
    std::cout << ident << ", ";
    if(const_init_val) {
        const_init_val->Dump();
    }
    std::cout << " }";
}

void *ConstDefAST::toKoopaIR(std::vector<const void *> &stmts) const {
    int val = const_init_val->calculate();
    symbol_list.addSymbol(ident, val);
    return nullptr;
}

void ConstInitValAST::Dump() const {
    std::cout << "ConstInitValAST { ";
    const_exp->Dump();
    std::cout << " }";
}

void *ConstInitValAST::toKoopaIR(std::vector<const void *> &stmts) const {
    return const_exp->toKoopaIR(stmts);
}

int ConstInitValAST::calculate() const {
    return const_exp->calculate();
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

void ConstExpAST::Dump() const {
    std::cout << "ConstExpAST { ";
    exp->Dump();
    std::cout << " }";
}

void *ConstExpAST::toKoopaIR(std::vector<const void *> &stmts) const {
    return exp->toKoopaIR(stmts);
}

int ConstExpAST::calculate() const {
    return exp->calculate();
}

// ExpAST 类的实现
void *ExpAST::toKoopaIR(std::vector<const void *> &stmts) const {
    return lor_exp->toKoopaIR(stmts);
}

int ExpAST::calculate() const {
    return lor_exp->calculate();
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

int LOrExpAST::calculate() const {
    if (op == "||") {
        return lor_exp->calculate() || land_exp->calculate();
    }
    else if (op == " ") {
        return land_exp->calculate();
    }
    return 0;
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

int LAndExpAST::calculate() const {
    if (op == "&&") {
        return land_exp->calculate() && eq_exp->calculate();
    }
    else if (op == " ") {
        return eq_exp->calculate();
    }
    return 0;
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

int EqExpAST::calculate() const {
    if(op == "==") {
        return eq_exp->calculate() == rel_exp->calculate();
    }
    else if(op == "!=") {
        return eq_exp->calculate() != rel_exp->calculate();
    }
    else if(op == " ") {
        return rel_exp->calculate();
    }
    return 0;
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

int RelExpAST::calculate() const {
    if(op == "<") {
        return rel_exp->calculate() < add_exp->calculate();
    }
    else if(op == ">") {
        return rel_exp->calculate() > add_exp->calculate();
    }
    else if(op == "<=") {
        return rel_exp->calculate() <= add_exp->calculate();
    }
    else if(op == ">=") {
        return rel_exp->calculate() >= add_exp->calculate();
    }
    else if(op == " ") {
        return add_exp->calculate();
    }
    return 0;
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

int AddExpAST::calculate() const {
    if(op == "+") {
        return add_exp->calculate() + mul_exp->calculate();
    }
    else if(op == "-") {
        return add_exp->calculate() - mul_exp->calculate();
    }
    else if(op == " ") {
        return mul_exp->calculate();
    }
    return 0;
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

int MulExpAST::calculate() const {
    if(op == "*") {
        return mul_exp->calculate() * unary_exp->calculate();
    }
    else if(op == "/") {
        return mul_exp->calculate() / unary_exp->calculate();
    }
    else if(op == "%") {
        return mul_exp->calculate() % unary_exp->calculate();
    }
    else if(op == " ") {
        return unary_exp->calculate();
    }
    return 0;
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

int UnaryExpAST::calculate() const {
    if(tag == "OPEXP") {
        if(unary_op == "-") {
            return -unary_exp->calculate();
        }
        else if(unary_op == "+") {
            return unary_exp->calculate();
        }
        else if(unary_op == "!") {
            return !unary_exp->calculate();
        }
    }
    else if(tag == "PEXP") {
        return primary_exp->calculate();
    }
    return 0;
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
    else if(tag == "LVAL") {
        return lval->toKoopaIR();
    }
    return nullptr;
}

int PrimaryExpAST::calculate() const {
    if(tag == "EXP") {
        return exp->calculate();
    }
    else if(tag == "NUM") {
        return number->calculate();
    }
    else if(tag == "LVAL") {
        return lval->calculate();
    }
    return 0;
}

void PrimaryExpAST::Dump() const {
    std::cout << "PrimartExpAST { ";
    if(tag == "EXP") {
        exp->Dump();
    }
    else if(tag == "NUM") {
        number->Dump();
    }
    else if(tag == "LVAL") {
        lval->Dump();
    }
    std::cout << " }";
}

// LValAST 类的实现
void *LValAST::toKoopaIR() const {
    int value = symbol_list.getSymbol(ident);
    koopa_raw_value_data_t *ir = new koopa_raw_value_data_t;
    ir->name = nullptr;
    ir->ty = make_ty(KOOPA_RTT_INT32);
    ir->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
    ir->kind.tag = KOOPA_RVT_LOAD;
    ir->kind.tag = KOOPA_RVT_INTEGER;
    ir->kind.data.integer.value = value;
    return ir;
}
int LValAST::calculate() const {
    return symbol_list.getSymbol(ident);
}

void LValAST::Dump() const {
    std::cout << "LValAST { " << ident << " = " << symbol_list.getSymbol(ident) << " }";
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

int NumberAST::calculate() const {
    return int_const;
}