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
    //symbol_list.newScope();
    //std::vector<const void *> blocks{block->toKoopaIR()};
    //symbol_list.deleteScope();
    koopa_raw_function_data_t *ir = new koopa_raw_function_data_t;
    ir->name = add_prefix("@", ident.c_str());
    ir->ty = make_func_ty(nullptr, (const koopa_raw_type_kind *)func_type->toKoopaIR());
    ir->params = make_slice(nullptr, KOOPA_RSIK_VALUE);

    std::vector<const void *> blocks;
    block_manager.init(&blocks);
    koopa_raw_basic_block_data_t *entry = new koopa_raw_basic_block_data_t();
    entry->name = add_prefix("%", "entry");
    entry->params = make_slice(nullptr, KOOPA_RSIK_VALUE);
    entry->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
    block_manager.newBlock(entry);
    block->toKoopaIR();
    if(block_manager.willBlockReturn())
    {
        koopa_raw_value_data_t *ret = new koopa_raw_value_data_t;
        ret->name = nullptr;
        ret->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        ret->ty = make_ty(KOOPA_RTT_UNIT);
        ret->kind.tag = KOOPA_RVT_RETURN;
        ret->kind.data.ret.value = (koopa_raw_value_data_t *)NumberAST(0).toKoopaIR();
        block_manager.addInst(ret);
    }
    block_manager.popBuffer();
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
    if(block_item_array) {
        for(auto &block_item : *block_item_array) {
            block_item->Dump();
        }
    }
    std::cout << " }";
}

void *BlockAST::toKoopaIR() const {
    //std::vector<const void *> stmts;
    //koopa_raw_basic_block_data_t *ir = new koopa_raw_basic_block_data_t;
    if(block_item_array) {
        symbol_list.newScope();
        for(auto &block_item : *block_item_array) {
            //block_item->toKoopaIR(stmts);
            block_item->toKoopaIR();
            /*
            if (stmts.size() > 0) {
                auto inst = (koopa_raw_value_t)stmts.back();
                if (inst->kind.tag == KOOPA_RVT_RETURN) {
                    break;
                }
            }
            */
        }
        symbol_list.deleteScope();
        //ir->insts = make_slice(&stmts, KOOPA_RSIK_VALUE);
    }
    //else {
    //    ir->insts = make_slice(nullptr, KOOPA_RSIK_VALUE);
    //}
    //ir->name = add_prefix("%", "entry");
    //ir->params = make_slice(nullptr, KOOPA_RSIK_VALUE);
    //ir->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
    return nullptr;
}

void *BlockAST::toKoopaIR(std::vector<const void *> &stmts) const {
    koopa_raw_basic_block_data_t *ir = new koopa_raw_basic_block_data_t;
    if(block_item_array) {
        for(auto &block_item : *block_item_array) {
            block_item->toKoopaIR(stmts);
            if (stmts.size() > 0) {
                auto inst = (koopa_raw_value_t)stmts.back();
                if (inst->kind.tag == KOOPA_RVT_RETURN) {
                    break;
                }
            }
        }
        ir->insts = make_slice(&stmts, KOOPA_RSIK_VALUE);
    }
    else {
        ir->insts = make_slice(nullptr, KOOPA_RSIK_VALUE);
    }
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
/*
void *BlockItemAST::toKoopaIR(std::vector<const void *> &stmts) const {
    if(block_item_type == "DECL") {
        return decl->toKoopaIR(stmts);
    }
    else if(block_item_type == "STMT") {
        return stmt->toKoopaIR(stmts);
    }
    return nullptr;
}
*/

void *BlockItemAST::toKoopaIR() const {
    if(block_item_type == "DECL") {
        return decl->toKoopaIR();
    }
    else if(block_item_type == "STMT") {
        return stmt->toKoopaIR();
    }
    return nullptr;
}

void DeclAST::Dump() const {
    std::cout << "DeclAST { ";
    if(decl_type == "CONST") {
        const_decl->Dump();
    }
    else if(decl_type == "VAR") {
        var_decl->Dump();
    }
    std::cout << " }";
}

/*
void *DeclAST::toKoopaIR(std::vector<const void *> &stmts) const {
    if(decl_type == "CONST") {
        return const_decl->toKoopaIR(stmts);
    }
    else if(decl_type == "VAR") {
        return var_decl->toKoopaIR(stmts);
    }
    return nullptr;
}
*/

void *DeclAST::toKoopaIR() const {
    if(decl_type == "CONST") {
        return const_decl->toKoopaIR();
    }
    else if(decl_type == "VAR") {
        return var_decl->toKoopaIR();
    }
    return nullptr;
}

void ConstDeclAST::Dump() const {
    std::cout << "ConstDeclAST { ";
    type->Dump();
    for(auto &const_def : *const_def_array) {
        const_def->Dump();
    }
    std::cout << " }";
}
/*
void *ConstDeclAST::toKoopaIR(std::vector<const void *> &stmts) const {
    for(auto &const_def : *const_def_array) {
        const_def->toKoopaIR(stmts);
    }
    return nullptr;
}
*/

void *ConstDeclAST::toKoopaIR() const {
    for(auto &const_def : *const_def_array) {
        const_def->toKoopaIR();
    }
    return nullptr;
}

void VarDeclAST::Dump() const {
    std::cout << "VarDeclAST { ";
    type->Dump();
    for(auto &var_def : *var_def_array) {
        var_def->Dump();
    }
    std::cout << " }";
}
/*
void *VarDeclAST::toKoopaIR(std::vector<const void *> &stmts) const {
    koopa_raw_type_t var_type = (const koopa_raw_type_t)type->toKoopaIR();
    for(auto &var_def : *var_def_array) {
        var_def->toKoopaIR(stmts, var_type);
    }
    return nullptr;
}
*/

void *VarDeclAST::toKoopaIR() const {
    koopa_raw_type_t var_type = (const koopa_raw_type_t)type->toKoopaIR();
    for(auto &var_def : *var_def_array) {
        var_def->toKoopaIR(var_type);
    }
    return nullptr;
}

void BTypeAST::Dump() const {
    std::cout << "BTypeAST { " << type << " }";
}

void *BTypeAST::toKoopaIR() const {
    if(type == "INT") {
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
/*
void *ConstDefAST::toKoopaIR(std::vector<const void *> &stmts) const {
    int val = const_init_val->calculate();
    Value value(ValueType::Const, val);
    symbol_list.addSymbol(ident, value);
    return nullptr;
}
*/

void *ConstDefAST::toKoopaIR() const {
    int val = const_init_val->calculate();
    Value value(ValueType::Const, val);
    symbol_list.addSymbol(ident, value);
    return nullptr;
}

void VarDefAST::Dump() const {
    std::cout << "VarDefAST { ";
    std::cout << ident << ", ";
    if(var_init_val) {
        var_init_val->Dump();
    }
    std::cout << " }";
}
/*
void *VarDefAST::toKoopaIR(std::vector<const void *> &stmts, koopa_raw_type_t type) const {
    koopa_raw_value_data_t *ir = new koopa_raw_value_data_t;
    ir->name = add_prefix("@", ident.c_str());
    ir->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
    ir->ty = make_pointer_ty(KOOPA_RTT_INT32);
    ir->kind.tag = KOOPA_RVT_ALLOC;
    stmts.push_back(ir);
    Value value(ValueType::Var, ir);
    symbol_list.addSymbol(ident, value);
    if(var_init_val!=nullptr) {
        koopa_raw_value_data_t *ir2 = new koopa_raw_value_data_t;
        ir2->name = nullptr;
        ir2->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        ir2->ty = make_ty(KOOPA_RTT_UNIT);//KOOPA_RTT_INT32?
        ir2->kind.tag = KOOPA_RVT_STORE;
        ir2->kind.data.store.dest = (koopa_raw_value_t)ir;
        ir2->kind.data.store.value = (koopa_raw_value_t)(var_init_val->toKoopaIR(stmts));
        stmts.push_back(ir2);
    }
    return nullptr;
}
*/

void *VarDefAST::toKoopaIR(koopa_raw_type_t type) const {
    koopa_raw_value_data_t *ir = new koopa_raw_value_data_t;
    ir->name = add_prefix("@", ident.c_str());
    ir->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
    ir->ty = make_pointer_ty(KOOPA_RTT_INT32);
    ir->kind.tag = KOOPA_RVT_ALLOC;
    //stmts.push_back(ir);
    block_manager.addInst(ir);
    Value value(ValueType::Var, ir);
    symbol_list.addSymbol(ident, value);
    if(var_init_val!=nullptr) {
        koopa_raw_value_data_t *ir2 = new koopa_raw_value_data_t;
        ir2->name = nullptr;
        ir2->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        ir2->ty = make_ty(KOOPA_RTT_INT32);//KOOPA_RTT_INT32?
        ir2->kind.tag = KOOPA_RVT_STORE;
        ir2->kind.data.store.dest = (koopa_raw_value_t)ir;
        ir2->kind.data.store.value = (koopa_raw_value_t)(var_init_val->toKoopaIR());
        block_manager.addInst(ir2);
    }
    return nullptr;
}

void ConstInitValAST::Dump() const {
    std::cout << "ConstInitValAST { ";
    const_exp->Dump();
    std::cout << " }";
}
/*
void *ConstInitValAST::toKoopaIR(std::vector<const void *> &stmts) const {
    return const_exp->toKoopaIR(stmts);
}
*/

void *ConstInitValAST::toKoopaIR() const {
    return const_exp->toKoopaIR();
}

int ConstInitValAST::calculate() const {
    return const_exp->calculate();
}

void InitValAST::Dump() const {
    std::cout << "InitValAST { ";
    exp->Dump();
    std::cout << " }";
}
/*
void *InitValAST::toKoopaIR(std::vector<const void *> &stmts) const {
    return exp->toKoopaIR(stmts);
}
*/

void *InitValAST::toKoopaIR() const {
    return exp->toKoopaIR();
}

int InitValAST::calculate() const {
    return exp->calculate();
}

// StmtAST 类的实现
void StmtAST::Dump() const {
    std::cout << "StmtAST { "<< stmt_type << ", ";
    if(stmt_type == "RETURN") {
        if(exp) {
            exp->Dump();
        }
    }
    else if(stmt_type == "ASSIGN") {
        lval->Dump();
        std::cout << " = ";
        exp->Dump();
    }
    else if(stmt_type == "EXP") {
        exp->Dump();
    }
    else if(stmt_type == "BLOCK") {
        block->Dump();
    }
    else if(stmt_type == "IFELSE") {
        if_stmt->Dump();
        else_stmt->Dump();
    }
    else if(stmt_type == "IF") {
        if_stmt->Dump();
    }
    else if(stmt_type == "LOOP") {
        exp->Dump();
        while_stmt->Dump();
    }
    std::cout << " }";
}
/*
void *StmtAST::toKoopaIR(std::vector<const void *> &stmts) const {
    koopa_raw_value_data_t *ir = new koopa_raw_value_data_t;
    ir->name = nullptr;
    ir->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
    ir->ty = make_ty(KOOPA_RTT_UNIT);
    if(stmt_type == "RETURN") {
        ir->kind.tag = KOOPA_RVT_RETURN;
        if(exp) {
            ir->kind.data.ret.value = (koopa_raw_value_data_t *)exp->toKoopaIR(stmts);
        }
        else {
            ir->kind.data.ret.value = nullptr;
        }
        stmts.push_back(ir);
    }
    else if(stmt_type == "ASSIGN") {
        ir->kind.tag = KOOPA_RVT_STORE;
        ir->kind.data.store.dest = (koopa_raw_value_t)lval->getKoopaIR();
        ir->kind.data.store.value = (koopa_raw_value_t)exp->toKoopaIR(stmts);
        stmts.push_back(ir);
    }
    else if(stmt_type == "EXP") {
        return exp->toKoopaIR(stmts);
    }
    else if(stmt_type == "BLOCK") {
        symbol_list.newScope();
        block->toKoopaIR(stmts);
        symbol_list.deleteScope();
    }
    else if(stmt_type == "EMPTY") {
        return nullptr;
    }
    return ir;
}
*/

void *StmtAST::toKoopaIR() const {
    koopa_raw_value_data_t *ir = new koopa_raw_value_data_t;
    ir->name = nullptr;
    ir->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
    ir->ty = make_ty(KOOPA_RTT_UNIT);
    if(stmt_type == "RETURN") {
        ir->kind.tag = KOOPA_RVT_RETURN;
        if(exp) {
            ir->kind.data.ret.value = (koopa_raw_value_data_t *)exp->toKoopaIR();
        }
        else {
            ir->kind.data.ret.value = nullptr;
        }
        block_manager.addInst(ir);
    }
    else if(stmt_type == "ASSIGN") {
        ir->kind.tag = KOOPA_RVT_STORE;
        ir->kind.data.store.dest = (koopa_raw_value_t)lval->getKoopaIR();
        ir->kind.data.store.value = (koopa_raw_value_t)exp->toKoopaIR();
        block_manager.addInst(ir);
    }
    else if(stmt_type == "EXP") {
        return exp->toKoopaIR();
    }
    else if(stmt_type == "BLOCK") {
        symbol_list.newScope();
        block->toKoopaIR();
        symbol_list.deleteScope();
    }
    else if(stmt_type == "BRANCH")
    {
        ir = (koopa_raw_value_data *)if_stmt->toKoopaIR();
        bool ifblock_might_comeback = block_manager.willBlockReturn();
        bool elseblock_might_comeback = true;

        koopa_raw_basic_block_data_t *else_block = new koopa_raw_basic_block_data_t;
        else_block->name = add_prefix("%", "else");
        else_block->params = make_slice(nullptr, KOOPA_RSIK_VALUE);
        else_block->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);

        koopa_raw_basic_block_data_t *end_block = new koopa_raw_basic_block_data_t;
        end_block->name = add_prefix("%", "end");
        end_block->params = make_slice(nullptr, KOOPA_RSIK_VALUE);
        end_block->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);

        ir->kind.data.branch.false_args = make_slice(nullptr, KOOPA_RSIK_VALUE);
        if(else_stmt != nullptr)
        {
            ir->kind.data.branch.false_bb = (koopa_raw_basic_block_t)else_block;
            ir->kind.data.branch.false_args = make_slice(nullptr, KOOPA_RSIK_VALUE);
            if(ifblock_might_comeback)
            {
                block_manager.addInst(jumpInst((koopa_raw_basic_block_t)end_block));
            }
            block_manager.newBlock(else_block);
            else_stmt->toKoopaIR();
            elseblock_might_comeback = block_manager.willBlockReturn();
        }
        else
        {
            ir->kind.data.branch.false_bb = (koopa_raw_basic_block_t)end_block;
        }
        if(elseblock_might_comeback)
        {
            block_manager.addInst(jumpInst((koopa_raw_basic_block_t)end_block));
        }
        if(ifblock_might_comeback || elseblock_might_comeback)
        {
            block_manager.newBlock(end_block);
        }
    }
    else if(stmt_type == "LOOP") {
        koopa_raw_basic_block_data_t *while_entry = new koopa_raw_basic_block_data_t;
        while_entry->name = add_prefix("%", "while_entry");
        while_entry->params = make_slice(nullptr, KOOPA_RSIK_VALUE);
        while_entry->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        block_manager.addInst(jumpInst((koopa_raw_basic_block_t)while_entry));
        block_manager.newBlock(while_entry);
        koopa_raw_value_data_t *ir = new koopa_raw_value_data_t;
        ir->name = nullptr;
        ir->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        ir->ty = make_ty(KOOPA_RTT_UNIT);
        ir->kind.tag = KOOPA_RVT_BRANCH;
        ir->kind.data.branch.cond = (koopa_raw_value_t)exp->toKoopaIR();
        koopa_raw_basic_block_data_t *while_body = new koopa_raw_basic_block_data_t;
        while_body->name = add_prefix("%", "while_body");
        while_body->params = make_slice(nullptr, KOOPA_RSIK_VALUE);
        while_body->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        ir->kind.data.branch.true_bb = (koopa_raw_basic_block_t)while_body;
        ir->kind.data.branch.true_args = make_slice(nullptr, KOOPA_RSIK_VALUE);
        koopa_raw_basic_block_data_t *while_end = new koopa_raw_basic_block_data_t;
        while_end->name = add_prefix("%", "while_end");
        while_end->params = make_slice(nullptr, KOOPA_RSIK_VALUE);
        while_end->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        ir->kind.data.branch.false_bb = (koopa_raw_basic_block_t)while_end;
        ir->kind.data.branch.false_args = make_slice(nullptr, KOOPA_RSIK_VALUE);
        loop_manager.pushLoop(while_entry, while_end);
        block_manager.addInst(ir);
        block_manager.newBlock(while_body);
        while_stmt->toKoopaIR();
        block_manager.addInst(jumpInst((koopa_raw_basic_block_t)while_entry));
        block_manager.newBlock(while_end);
        loop_manager.popLoop();
    }
    else if(stmt_type == "BREAK") {
        assert(loop_manager.getLoop().loop_end != nullptr);
        block_manager.addInst(jumpInst(loop_manager.getLoop().loop_end));
    }
    else if(stmt_type == "CONTINUE") {
        assert(loop_manager.getLoop().loop_entry != nullptr);
        block_manager.addInst(jumpInst(loop_manager.getLoop().loop_entry));
    }
    else if(stmt_type == "EMPTY") {
        return nullptr;
    }
    return ir;
}

void IfAST::Dump() const {
    std::cout << "IfAST { ";
    exp->Dump();
    std::cout << ", ";
    stmt->Dump();
    std::cout << " }";
}

void *IfAST::toKoopaIR() const {
    koopa_raw_value_data_t *ir = new koopa_raw_value_data_t;
    ir->name = nullptr;
    ir->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
    ir->ty = make_ty(KOOPA_RTT_UNIT);
    ir->kind.tag = KOOPA_RVT_BRANCH;
    ir->kind.data.branch.cond = (koopa_raw_value_t)exp->toKoopaIR();
    block_manager.addInst(ir);
    koopa_raw_basic_block_data_t *if_block = new koopa_raw_basic_block_data_t;
    if_block->name = add_prefix("%", "then");
    if_block->params = make_slice(nullptr, KOOPA_RSIK_VALUE);
    if_block->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
    ir->kind.data.branch.true_bb = (koopa_raw_basic_block_t)if_block;
    ir->kind.data.branch.true_args = make_slice(nullptr, KOOPA_RSIK_VALUE);
    block_manager.newBlock(if_block);
    stmt->toKoopaIR();
    return ir;
}

void ConstExpAST::Dump() const {
    std::cout << "ConstExpAST { ";
    exp->Dump();
    std::cout << " }";
}
/*
void *ConstExpAST::toKoopaIR(std::vector<const void *> &stmts) const {
    return exp->toKoopaIR(stmts);
}
*/

void *ConstExpAST::toKoopaIR() const {
    return exp->toKoopaIR();
}

int ConstExpAST::calculate() const {
    return exp->calculate();
}
/*
void *ExpAST::toKoopaIR(std::vector<const void *> &stmts) const {
    return lor_exp->toKoopaIR(stmts);
}
*/

void *ExpAST::toKoopaIR() const {
    return lor_exp->toKoopaIR();
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
/*
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
*/

void *LOrExpAST::toKoopaIR() const {
    //koopa_raw_value_data_t *ir = new koopa_raw_value_data_t;
    if(op == "||") {
        /*
        ir->name = nullptr;
        ir->ty = make_ty(KOOPA_RTT_INT32);
        ir->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        ir->kind.tag = KOOPA_RVT_BINARY;
        ir->kind.data.binary.op = KOOPA_RBO_OR;
        ir->kind.data.binary.lhs = (koopa_raw_value_t)toBoolKoopaIR(lor_exp);
        ir->kind.data.binary.rhs = (koopa_raw_value_t)toBoolKoopaIR(land_exp);
        block_manager.addInst(ir);
        return ir;
        */
        koopa_raw_value_data *temp_alloc = new koopa_raw_value_data;
        temp_alloc->name = add_prefix("@", "temp");
        temp_alloc->ty = make_pointer_ty(KOOPA_RTT_INT32);
        temp_alloc->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        temp_alloc->kind.tag = KOOPA_RVT_ALLOC;
        block_manager.addInst(temp_alloc);
        koopa_raw_value_data *temp_store = new koopa_raw_value_data;
        temp_store->name = nullptr;
        temp_store->ty = make_ty(KOOPA_RTT_INT32);
        temp_store->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        temp_store->kind.tag = KOOPA_RVT_STORE;
        temp_store->kind.data.store.dest = (koopa_raw_value_t)temp_alloc;
        temp_store->kind.data.store.value = (koopa_raw_value_t)NumberAST(1).toKoopaIR();
        block_manager.addInst(temp_store);
        koopa_raw_value_data *temp_branch = new koopa_raw_value_data;
        temp_branch->name = nullptr;
        temp_branch->ty = make_ty(KOOPA_RTT_UNIT);
        temp_branch->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        temp_branch->kind.tag = KOOPA_RVT_BRANCH;
        temp_branch->kind.data.branch.cond = (koopa_raw_value_t)toBoolKoopaIR(lor_exp);
        koopa_raw_basic_block_data_t *temp_then = new koopa_raw_basic_block_data_t;
        temp_then->name = add_prefix("%", "then");
        temp_then->params = make_slice(nullptr, KOOPA_RSIK_VALUE);
        temp_then->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        temp_branch->kind.data.branch.true_bb = (koopa_raw_basic_block_t)temp_then;
        temp_branch->kind.data.branch.true_args = make_slice(nullptr, KOOPA_RSIK_VALUE);
        koopa_raw_basic_block_data_t *temp_end = new koopa_raw_basic_block_data_t;
        temp_end->name = add_prefix("%", "end");
        temp_end->params = make_slice(nullptr, KOOPA_RSIK_VALUE);
        temp_end->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        temp_branch->kind.data.branch.false_bb = (koopa_raw_basic_block_t)temp_end;
        temp_branch->kind.data.branch.false_args = make_slice(nullptr, KOOPA_RSIK_VALUE);
        block_manager.addInst(temp_branch);
        block_manager.newBlock(temp_end);
        koopa_raw_value_data *temp_store2 = new koopa_raw_value_data;
        temp_store2->name = nullptr;
        temp_store2->ty = make_ty(KOOPA_RTT_INT32);
        temp_store2->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        temp_store2->kind.tag = KOOPA_RVT_STORE;
        temp_store2->kind.data.store.dest = (koopa_raw_value_t)temp_alloc;
        temp_store2->kind.data.store.value = (koopa_raw_value_t)toBoolKoopaIR(land_exp);
        block_manager.addInst(temp_store2);
        block_manager.addInst(jumpInst((koopa_raw_basic_block_t)temp_then));
        block_manager.newBlock(temp_then);
        koopa_raw_value_data *ret = new koopa_raw_value_data;
        ret->name = nullptr;
        ret->ty = make_ty(KOOPA_RTT_INT32);
        ret->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        ret->kind.tag = KOOPA_RVT_LOAD;
        ret->kind.data.load.src = (koopa_raw_value_t)temp_alloc;
        block_manager.addInst(ret);
        return ret;
    }
    else if(op == " ") {
        return land_exp->toKoopaIR();
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
/*
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
*/

void *LOrExpAST::toBoolKoopaIR(const std::unique_ptr<BaseAST> &exp) const {
    koopa_raw_value_data_t *ir = new koopa_raw_value_data_t;
    ir->name = nullptr;
    ir->ty = make_ty(KOOPA_RTT_INT32);
    ir->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
    ir->kind.tag = KOOPA_RVT_BINARY;
    ir->kind.data.binary.op = KOOPA_RBO_NOT_EQ;
    NumberAST zero(0);
    ir->kind.data.binary.lhs = (koopa_raw_value_t)zero.toKoopaIR();
    ir->kind.data.binary.rhs = (koopa_raw_value_t)exp->toKoopaIR();
    block_manager.addInst(ir);
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
/*
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
*/

void *LAndExpAST::toKoopaIR() const {
    //koopa_raw_value_data_t *ir = new koopa_raw_value_data_t;
    if(op == "&&") {
        /*
        ir->name = nullptr;
        ir->ty = make_ty(KOOPA_RTT_INT32);
        ir->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        ir->kind.tag = KOOPA_RVT_BINARY;
        ir->kind.data.binary.op = KOOPA_RBO_AND;
        ir->kind.data.binary.lhs = (koopa_raw_value_t)toBoolKoopaIR(land_exp);
        ir->kind.data.binary.rhs = (koopa_raw_value_t)toBoolKoopaIR(eq_exp);
        block_manager.addInst(ir);
        return ir;
        */
        koopa_raw_value_data *temp_alloc = new koopa_raw_value_data;
        temp_alloc->name = add_prefix("@", "temp");
        temp_alloc->ty = make_pointer_ty(KOOPA_RTT_INT32);
        temp_alloc->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        temp_alloc->kind.tag = KOOPA_RVT_ALLOC;
        block_manager.addInst(temp_alloc);
        koopa_raw_value_data *temp_store = new koopa_raw_value_data;
        temp_store->name = nullptr;
        temp_store->ty = make_ty(KOOPA_RTT_INT32);
        temp_store->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        temp_store->kind.tag = KOOPA_RVT_STORE;
        temp_store->kind.data.store.dest = (koopa_raw_value_t)temp_alloc;
        temp_store->kind.data.store.value = (koopa_raw_value_t)NumberAST(0).toKoopaIR();
        block_manager.addInst(temp_store);
        koopa_raw_value_data *temp_branch = new koopa_raw_value_data;
        temp_branch->name = nullptr;
        temp_branch->ty = make_ty(KOOPA_RTT_UNIT);
        temp_branch->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        temp_branch->kind.tag = KOOPA_RVT_BRANCH;
        temp_branch->kind.data.branch.cond = (koopa_raw_value_t)toBoolKoopaIR(land_exp);
        koopa_raw_basic_block_data_t *temp_then = new koopa_raw_basic_block_data_t;
        temp_then->name = add_prefix("%", "then");
        temp_then->params = make_slice(nullptr, KOOPA_RSIK_VALUE);
        temp_then->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        temp_branch->kind.data.branch.true_bb = (koopa_raw_basic_block_t)temp_then;
        temp_branch->kind.data.branch.true_args = make_slice(nullptr, KOOPA_RSIK_VALUE);
        koopa_raw_basic_block_data_t *temp_end = new koopa_raw_basic_block_data_t;
        temp_end->name = add_prefix("%", "end");
        temp_end->params = make_slice(nullptr, KOOPA_RSIK_VALUE);
        temp_end->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        temp_branch->kind.data.branch.false_bb = (koopa_raw_basic_block_t)temp_end;
        temp_branch->kind.data.branch.false_args = make_slice(nullptr, KOOPA_RSIK_VALUE);
        block_manager.addInst(temp_branch);
        block_manager.newBlock(temp_then);
        koopa_raw_value_data *temp_store2 = new koopa_raw_value_data;
        temp_store2->name = nullptr;
        temp_store2->ty = make_ty(KOOPA_RTT_INT32);
        temp_store2->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        temp_store2->kind.tag = KOOPA_RVT_STORE;
        temp_store2->kind.data.store.dest = (koopa_raw_value_t)temp_alloc;
        temp_store2->kind.data.store.value = (koopa_raw_value_t)toBoolKoopaIR(eq_exp);
        block_manager.addInst(temp_store2);
        block_manager.addInst(jumpInst((koopa_raw_basic_block_t)temp_end));
        block_manager.newBlock(temp_end);
        koopa_raw_value_data *ret = new koopa_raw_value_data;
        ret->name = nullptr;
        ret->ty = make_ty(KOOPA_RTT_INT32);
        ret->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
        ret->kind.tag = KOOPA_RVT_LOAD;
        ret->kind.data.load.src = (koopa_raw_value_t)temp_alloc;
        block_manager.addInst(ret);
        return ret;
    }
    else if(op == " ") {
        return eq_exp->toKoopaIR();
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
/*
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
*/

void *LAndExpAST::toBoolKoopaIR(const std::unique_ptr<BaseAST> &exp) const {
    koopa_raw_value_data_t *ir = new koopa_raw_value_data_t;
    ir->name = nullptr;
    ir->ty = make_ty(KOOPA_RTT_INT32);
    ir->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
    ir->kind.tag = KOOPA_RVT_BINARY;
    ir->kind.data.binary.op = KOOPA_RBO_NOT_EQ;
    NumberAST zero(0);
    ir->kind.data.binary.lhs = (koopa_raw_value_t)zero.toKoopaIR();
    ir->kind.data.binary.rhs = (koopa_raw_value_t)exp->toKoopaIR();
    block_manager.addInst(ir);
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
/*
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
*/

void *EqExpAST::toKoopaIR() const {
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
        ir->kind.data.binary.lhs = (koopa_raw_value_t)eq_exp->toKoopaIR();
        ir->kind.data.binary.rhs = (koopa_raw_value_t)rel_exp->toKoopaIR();
        block_manager.addInst(ir);
        return ir;
    }
    else if(op == " ") {
        return rel_exp->toKoopaIR();
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
/*
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
*/

void *RelExpAST::toKoopaIR() const {
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
        ir->kind.data.binary.lhs = (koopa_raw_value_t)rel_exp->toKoopaIR();
        ir->kind.data.binary.rhs = (koopa_raw_value_t)add_exp->toKoopaIR();
        block_manager.addInst(ir);
        return ir;
    }
    else if(op == " ") {
        return add_exp->toKoopaIR();
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
/*
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
*/

void *AddExpAST::toKoopaIR() const {
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
        ir->kind.data.binary.lhs = (koopa_raw_value_t)add_exp->toKoopaIR();
        ir->kind.data.binary.rhs = (koopa_raw_value_t)mul_exp->toKoopaIR();
        block_manager.addInst(ir);
        return ir;
    }
    else if(op == " ") {
        return mul_exp->toKoopaIR();
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
/*
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
*/

void *MulExpAST::toKoopaIR() const {
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
        ir->kind.data.binary.lhs = (koopa_raw_value_t)mul_exp->toKoopaIR();
        ir->kind.data.binary.rhs = (koopa_raw_value_t)unary_exp->toKoopaIR();
        block_manager.addInst(ir);
        return ir;
    }
    else if(op == " ") {
        return unary_exp->toKoopaIR();
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
/*
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
*/

void *UnaryExpAST::toKoopaIR() const {
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
            return unary_exp->toKoopaIR();
        }
        else if(unary_op == "!") {
            ir->kind.data.binary.op = KOOPA_RBO_EQ;
            NumberAST zero(0);
            ir->kind.data.binary.lhs = (koopa_raw_value_t)zero.toKoopaIR();
        }
        ir->kind.data.binary.rhs = (koopa_raw_value_t)unary_exp->toKoopaIR();
        block_manager.addInst(ir);
        return ir;
    }
    else if(tag == "PEXP") {
        return primary_exp->toKoopaIR();
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
/*
void *PrimaryExpAST::toKoopaIR(std::vector<const void *> &stmts) const {
    if(tag == "EXP") {
        return exp->toKoopaIR(stmts);
    }
    else if(tag == "NUM") {
        return number->toKoopaIR();
    }
    else if(tag == "LVAL") {
        return lval->toKoopaIR(stmts);
    }
    return nullptr;
}
*/

void *PrimaryExpAST::toKoopaIR() const {
    if(tag == "EXP") {
        return exp->toKoopaIR();
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

/*
void *LValAST::toKoopaIR(std::vector<const void *> &stmts) const {
    Value value = symbol_list.getSymbol(ident);
    koopa_raw_value_data_t *ir = new koopa_raw_value_data_t;
    ir->name = nullptr;
    ir->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
    ir->ty = make_ty(KOOPA_RTT_INT32);
    if(value.type == ValueType::Var) {
        ir->kind.tag = KOOPA_RVT_LOAD;
        //ir->kind.data.integer.value = value.data.const_value;
        ir->kind.data.load.src = (koopa_raw_value_t)value.data.var_value;
        stmts.push_back(ir);
    }
    else if(value.type == ValueType::Const) {
        ir->kind.tag = KOOPA_RVT_INTEGER;
        ir->kind.data.integer.value = value.data.const_value;
    }
    return ir;
}
*/

void *LValAST::toKoopaIR() const {
    Value value = symbol_list.getSymbol(ident);
    koopa_raw_value_data_t *ir = new koopa_raw_value_data_t;
    ir->name = nullptr;
    ir->used_by = make_slice(nullptr, KOOPA_RSIK_VALUE);
    ir->ty = make_ty(KOOPA_RTT_INT32);
    if(value.type == ValueType::Var) {
        ir->kind.tag = KOOPA_RVT_LOAD;
        //ir->kind.data.integer.value = value.data.const_value;
        ir->kind.data.load.src = (koopa_raw_value_t)value.data.var_value;
        block_manager.addInst(ir);
    }
    else if(value.type == ValueType::Const) {
        ir->kind.tag = KOOPA_RVT_INTEGER;
        ir->kind.data.integer.value = value.data.const_value;
    }
    return ir;
}

int LValAST::calculate() const {
    return symbol_list.getSymbol(ident).data.const_value;
}

void LValAST::Dump() const {
    std::cout << "LValAST { " << ident << " }";
}

void *LValAST::getKoopaIR() const {
    Value value = symbol_list.getSymbol(ident);
    if(value.type == ValueType::Var) {
        return (void *)value.data.var_value;
    }
    return nullptr;
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