#include "visit.h"

int cnt = 0;

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

char value_cnt = '0';

// 访问指令
void Visit(const koopa_raw_value_t &value, std::ostream &cout) {
  // 根据指令类型判断后续需要如何访问
  const auto &kind = value->kind;
  switch (kind.tag) {
    case KOOPA_RVT_RETURN:
      // 访问 return 指令
      Visit(kind.data.ret, cout);
      value_cnt += 1;
      if(value_cnt == '7') {
        value_cnt = '1';
      }
      break;
    case KOOPA_RVT_INTEGER:
      // 访问 integer 指令
      Visit(kind.data.integer, cout);
      break;
    case KOOPA_RVT_BINARY:
      // 访问 binary 指令
      Visit(kind.data.binary, value_cnt, cout);
      value_cnt += 1;
      if(value_cnt == '7') {
        value_cnt = '1';
      }
      break;
    default:
      // 其他类型暂时遇不到
      assert(false);
  }
  std::cout<<"value_cnt: "<<value_cnt<<"\n";
}

// 访问返回指令
void Visit(const koopa_raw_return_t &ret, std::ostream &cout) {
  if(ret.value->kind.tag == KOOPA_RVT_INTEGER) {
    cout<<"\tli    a0, "<<ret.value->kind.data.integer.value<<"\n";
  }
  else {
    cout<<"\tmv    a0, t"<<(char)(value_cnt - 1)<<"\n";
  }
  cout<<"\tret\n";
}

// 访问整数常量
void Visit(const koopa_raw_integer_t &integer, std::ostream &cout) {
  cout<<integer.value<<"\n";
}

void Visit(const koopa_raw_binary_t &binary_op, char value_cnt, std::ostream &cout) {
  // 访问操作符
  std::cout<<"visit binary\n";
  std::string lreg, rreg;
  std::string dest_reg = "t" + std::string(1, value_cnt);

  if(binary_op.lhs->kind.tag == KOOPA_RVT_INTEGER) {
    if(binary_op.lhs->kind.data.integer.value == 0) {
      lreg = "x0";
    }
    else {
      load(binary_op.lhs, dest_reg, cout);
      lreg = dest_reg;
    }
  }
  else {
    lreg = "t" + std::string(1, value_cnt-1);
  }

  if(binary_op.rhs->kind.tag == KOOPA_RVT_INTEGER) {
    if(binary_op.rhs->kind.data.integer.value == 0) {
      rreg = "x0";
    }
    else {
      if(binary_op.lhs->kind.tag == KOOPA_RVT_INTEGER && binary_op.lhs->kind.data.integer.value != 0) {
        load(binary_op.rhs, "t" + std::string(1, value_cnt+1), cout);
        rreg = "t" + std::string(1, value_cnt+1);
      }
      else {
      load(binary_op.rhs, dest_reg, cout);
      rreg = dest_reg;
      }
    }
  }
  else {
    if(binary_op.lhs->kind.tag != KOOPA_RVT_INTEGER) {
      rreg = "t" + std::string(1, value_cnt-2);
    }
    else {
      rreg = "t" + std::string(1, value_cnt-1);
    }
  }

  switch (binary_op.op) {
    case KOOPA_RBO_EQ:
      cout<<"\txor   "<<dest_reg<<", "<<lreg<<", "<<rreg<<"\n";
      cout<<"\tseqz  "<<dest_reg<<", "<<dest_reg<<"\n";
      break;
    case KOOPA_RBO_NOT_EQ:
      cout<<"\txor   "<<dest_reg<<", "<<lreg<<", "<<rreg<<"\n";
      cout<<"\tsnez  "<<dest_reg<<", "<<dest_reg<<"\n";
      break;
    case KOOPA_RBO_ADD:
      cout<<"\tadd   "<<dest_reg<<", "<<lreg<<", "<<rreg<<"\n";
      break;
    case KOOPA_RBO_SUB:
      cout<<"\tsub   "<<dest_reg<<", "<<lreg<<", "<<rreg<<"\n";
      break;
    case KOOPA_RBO_MUL:
      cout<<"\tmul   "<<dest_reg<<", "<<lreg<<", "<<rreg<<"\n";
      break;
    case KOOPA_RBO_DIV:
      cout<<"\tdiv   "<<dest_reg<<", "<<lreg<<", "<<rreg<<"\n";
      break;
    case KOOPA_RBO_MOD:
      cout<<"\trem   "<<dest_reg<<", "<<lreg<<", "<<rreg<<"\n";
      break;
    case KOOPA_RBO_LT:
      cout<<"\tslt   "<<dest_reg<<", "<<lreg<<", "<<rreg<<"\n";
      break;
    case KOOPA_RBO_LE:
      cout<<"\tsgt   "<<dest_reg<<", "<<lreg<<", "<<rreg<<"\n";
      cout<<"\tseqz  "<<dest_reg<<", "<<dest_reg<<"\n";
      break;
    case KOOPA_RBO_GT:
      cout<<"\tsgt   "<<dest_reg<<", "<<lreg<<", "<<rreg<<"\n";
      break;
    case KOOPA_RBO_GE:
      cout<<"\tslt   "<<dest_reg<<", "<<lreg<<", "<<rreg<<"\n";
      cout<<"\tseqz  "<<dest_reg<<", "<<dest_reg<<"\n";
      break;
    case KOOPA_RBO_AND:
      cout<<"\tand   "<<dest_reg<<", "<<lreg<<", "<<rreg<<"\n";
      break;
    case KOOPA_RBO_OR:
      cout<<"\tor    "<<dest_reg<<", "<<lreg<<", "<<rreg<<"\n";
      break;
    default:
      assert(false);
  }
}

void load(const koopa_raw_value_t &value, std::string r, std::ostream &cout) {
  cout<<"\tli    "<<r<<", ";
  Visit(value, cout);
}