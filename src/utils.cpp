#include "utils.h"

void BlockManager::init(std::vector<const void *> *block_list) {
    this->block_list = block_list;
    return;
}

void BlockManager::addInst(const void *inst) {
    std::cout << "addInst" << std::endl;
    inst_buf.push_back(inst);
    return;
}

void BlockManager::popBuffer() {
    if(block_list->size() > 0)
    {
        if(inst_buf.size() > 0) 
        {
            koopa_raw_basic_block_data_t *last_block = (koopa_raw_basic_block_data_t *)block_list->back();
            for (size_t i = 0; i < inst_buf.size(); i++) {
                koopa_raw_value_t inst = (koopa_raw_value_t)inst_buf[i];
                if (inst->kind.tag == KOOPA_RVT_RETURN ||
                    inst->kind.tag == KOOPA_RVT_JUMP ||
                    inst->kind.tag == KOOPA_RVT_BRANCH) 
                {
                    inst_buf.resize(i + 1);
                    break;
                }
            }
            if(!last_block->insts.buffer) {
                last_block->insts = make_slice(&inst_buf, KOOPA_RSIK_VALUE);
            }
            inst_buf.clear();
        }
        else {
            block_list->pop_back();
        }
    }
    return;
}

void BlockManager::newBlock(koopa_raw_basic_block_data_t *basic_block) {
    popBuffer();
    basic_block->insts.buffer = nullptr;
    basic_block->insts.len = 0;
    block_list->push_back(basic_block);
    return;
}

void SymbolList::addSymbol(std::string symbol, Value value) {
    if(symbol_list_array.empty()) {
        symbol_list_array.push_back(std::map<std::string, Value>());
    }
    symbol_list_array.back()[symbol] = value;
}

Value SymbolList::getSymbol(std::string symbol) {
    for(auto it = symbol_list_array.rbegin(); it != symbol_list_array.rend(); it++) {
        if(it->find(symbol) != it->end()) {
            return it->at(symbol);
        }
    }
    assert(false);
}

void SymbolList::newScope() {
    symbol_list_array.push_back(std::map<std::string, Value>());
}

void SymbolList::deleteScope() {
    symbol_list_array.pop_back();
}

void SymbolList::Dump() const {
    for(auto &symbol_list : symbol_list_array) {
        for(auto &symbol : symbol_list) {
            std::cout << symbol.first << " : ";
            if(symbol.second.type == Const) {
                std::cout << symbol.second.data.const_value << std::endl;
            }
            else if(symbol.second.type == Var) {
                std::cout << symbol.second.data.var_value << std::endl;
            }
        }
    }
}

koopa_raw_slice_t make_slice(std::vector<const void *> *buf, koopa_raw_slice_item_kind_t kind) {
    koopa_raw_slice_t slice;
    slice.kind = kind;
    if(buf!=nullptr){
        slice.buffer = new const void *[buf->size()];
        std::copy(buf->begin(), buf->end(), slice.buffer);
        slice.len = buf->size();
        return slice;
    }
    slice.buffer = nullptr;
    slice.len = 0;
    return slice;
}

koopa_raw_type_kind_t *make_func_ty(std::vector<const void *> *buf, const koopa_raw_type_kind *ret) {
    koopa_raw_type_kind_t *ty = new koopa_raw_type_kind_t;
    ty->tag = KOOPA_RTT_FUNCTION;
    ty->data.function.ret = ret;
    ty->data.function.params = make_slice(buf, KOOPA_RSIK_TYPE);
    return ty;
}

koopa_raw_type_kind_t *make_ty(koopa_raw_type_tag_t tag) {
    koopa_raw_type_kind_t *ty = new koopa_raw_type_kind_t;
    ty->tag = tag;
    return ty;
}

koopa_raw_type_kind_t *make_pointer_ty(koopa_raw_type_tag_t tag) {
    koopa_raw_type_kind_t *ty = new koopa_raw_type_kind_t;
    ty->tag = KOOPA_RTT_POINTER;
    ty->data.pointer.base = make_ty(tag);
    return ty;
}

char *add_prefix(const char *prefix, const char *name) {
    char *new_name = new char[strlen(prefix) + strlen(name) + 1];
    strcpy(new_name, prefix);
    strcat(new_name, name);
    return new_name;
}

char *remove_prefix(const char *prefix, const char *name) {
    if(strncmp(prefix, name, strlen(prefix)) == 0) {
        return strdup(name + strlen(prefix));
    }
    return strdup(name);
}

void koopa_dump_riscv_to_file(koopa_raw_program_t &raw_program, const char *path) {
    FILE* original_stdout = stdout;
    freopen(path, "w", stdout);
    stdout = original_stdout;
}