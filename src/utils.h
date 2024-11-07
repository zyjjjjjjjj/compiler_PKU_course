#pragma once
#include "koopa.h"
#include "visit.h"
#include <map>
#include <vector>
#include <cstring>
#include <stdio.h>

enum ValueType { Const, Var};

struct Value {
  ValueType type;
  union SymbolListValue {
    int const_value;
    koopa_raw_value_t var_value;
  } data;
  Value() = default;
  Value(ValueType type, int value) : type(type) { data.const_value = value; }
  Value(ValueType type, koopa_raw_value_t value) : type(type) {
    if (type == Var)
      data.var_value = value;
  }
};

class SymbolList {
private:
  std::vector<std::map<std::string, Value>> symbol_list_array;

public:
  ~SymbolList() = default;
  void addSymbol(std::string symbol, Value value);
  Value getSymbol(std::string symbol);
  void Dump() const;
};

koopa_raw_slice_t make_slice(std::vector<const void *> *buf, koopa_raw_slice_item_kind_t kind);

koopa_raw_type_kind_t *make_func_ty(std::vector<const void *> *buf, const koopa_raw_type_kind *ret);

koopa_raw_type_kind_t *make_ty(koopa_raw_type_tag_t tag);

koopa_raw_type_kind_t *make_pointer_ty(koopa_raw_type_tag_t tag);

char *add_prefix(const char *prefix, const char *name);

char *remove_prefix(const char *prefix, const char *name);

void koopa_dump_riscv_to_file(koopa_raw_program_t &program, const char *path);
