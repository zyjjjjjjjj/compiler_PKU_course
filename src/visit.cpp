#include "visit.h"

void Visit(const koopa_raw_program_t &program, std::ostream &cout) {
    cout<<"\t.text\n";
    Visit(program.values, cout);
    Visit(program.funcs, cout);
}

void Visit(const koopa_raw_slice_t &slice, std::ostream &cout) {
  for (size_t i = 0; i < slice.len; ++i) {
    auto ptr = slice.buffer[i];
    switch (slice.kind) {
      case KOOPA_RSIK_FUNCTION:
        Visit(reinterpret_cast<koopa_raw_function_t>(ptr), cout);
        break;
      case KOOPA_RSIK_BASIC_BLOCK:
        Visit(reinterpret_cast<koopa_raw_basic_block_t>(ptr), cout);
        break;
      case KOOPA_RSIK_VALUE:
        Visit(reinterpret_cast<koopa_raw_value_t>(ptr), cout);
        break;
      default:
        assert(false);
    }
  }
}

//访问函数
void Visit(const koopa_raw_function_t &func, std::ostream &cout) {
    cout<<"\t.global "<<remove_prefix("@", func->name)<<"\n"<<remove_prefix("@", func->name)<<":\n";
    // 访问所有基本块
    Visit(func->bbs, cout);
}

// 访问基本块
void Visit(const koopa_raw_basic_block_t &bb, std::ostream &cout) {
  // 执行一些其他的必要操作
  // ...
  // 访问所有指令
  Visit(bb->insts, cout);
}

// 访问指令
void Visit(const koopa_raw_value_t &value, std::ostream &cout) {
  // 根据指令类型判断后续需要如何访问
  const auto &kind = value->kind;
  switch (kind.tag) {
    case KOOPA_RVT_RETURN:
      // 访问 return 指令
      Visit(kind.data.ret, cout);
      break;
    case KOOPA_RVT_INTEGER:
      // 访问 integer 指令
      Visit(kind.data.integer, cout);
      break;
    default:
      // 其他类型暂时遇不到
      assert(false);
  }
}

// 访问返回指令
void Visit(const koopa_raw_return_t &ret, std::ostream &cout) {
  cout<<"\tli a0, ";
  // 访问返回值
  Visit(ret.value, cout);
  cout<<"\tret\n";
}

// 访问整数常量
void Visit(const koopa_raw_integer_t &integer, std::ostream &cout) {
  cout<<integer.value<<"\n";
}