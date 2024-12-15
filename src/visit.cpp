#include "visit.h"

int cnt = 0;
int cur_size = 0;
std::map<koopa_raw_value_t, int> addr_map;

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
    cout<<"\t.global "<<remove_prefix("@", func->name)<<"\n";
    cout<<remove_prefix("@", func->name)<<":\n";
    cout<<"\tli    t0, -"<<func_size(func)<<"\n";
    cout<<"\tadd   sp, sp, t0\n";
    // 访问所有基本块
    Visit(func->bbs, cout);
}

// 访问基本块
void Visit(const koopa_raw_basic_block_t &bb, std::ostream &cout) {
  if(bb->name != nullptr && std::string(bb->name) != "@entry") {
    cout<<remove_prefix("%", bb->name)<<":\n";
  }
  // 访问所有指令
  Visit(bb->insts, cout);
}

int value_cnt = 0;

// 访问指令
void Visit(const koopa_raw_value_t &value, std::ostream &cout) {
  // 根据指令类型判断后续需要如何访问
  int dest_addr;
  if(value->ty->tag != KOOPA_RTT_UNIT)
    dest_addr = get_addr(value);
  const auto &kind = value->kind;
  switch (kind.tag) {
    case KOOPA_RVT_RETURN:
      // 访问 return 指令
      Visit(kind.data.ret, cout);
      value_cnt += 1;
      if(value_cnt == 7) {
        value_cnt = 1;
      }
      break;
    case KOOPA_RVT_INTEGER:
      // 访问 integer 指令
      Visit(kind.data.integer, cout);
      break;
    case KOOPA_RVT_BINARY:
      // 访问 binary 指令
      Visit(kind.data.binary, dest_addr, cout);
      value_cnt += 1;
      if(value_cnt == 7) {
        value_cnt = 1;
      }
      break;
    case KOOPA_RVT_ALLOC:
      break;
    case KOOPA_RVT_STORE:
      // 访问 store 指令
      Visit(kind.data.store, cout);
      value_cnt += 1;
      break;
    case KOOPA_RVT_LOAD:
      // 访问 load 指令
      Visit(kind.data.load, dest_addr, cout);
      break;
    case KOOPA_RVT_BRANCH:
      // 访问 branch 指令
      Visit(kind.data.branch, cout);
      break;
    case KOOPA_RVT_JUMP:
      // 访问 jump 指令
      Visit(kind.data.jump, cout);
      break;
    default:
      // 其他类型暂时遇不到
      std::cout<<kind.tag<<"\n";
      assert(false);
  }
}

// 访问跳转指令
void Visit(const koopa_raw_jump_t &jump, std::ostream &cout) {
  // 访问跳转指令
  cout<<"\tj     "<<remove_prefix("%", jump.target->name)<<"\n";
}

// 访问分支指令
void Visit(const koopa_raw_branch_t &branch, std::ostream &cout) {
  // 访问分支指令
  if(branch.cond->kind.tag == KOOPA_RVT_INTEGER) {
    cout<<"\tli    t0, "<<branch.cond->kind.data.integer.value<<"\n";
  }
  else {
    cout<<"\tli    t0, "<<get_addr(branch.cond)<<"\n";
    cout<<"\tadd   t0, t0, sp\n";
    cout<<"\tlw    t0, (t0)\n";
  }
  cout<<"\tbnez  t0, "<<remove_prefix("%", branch.true_bb->name)<<"\n";
  cout<<"\tj     "<<remove_prefix("%", branch.false_bb->name)<<"\n";
}

// 访问返回指令
void Visit(const koopa_raw_return_t &ret, std::ostream &cout) {
  if(ret.value->kind.tag == KOOPA_RVT_INTEGER) {
    cout<<"\tli    a0, "<<ret.value->kind.data.integer.value<<"\n";
  }
  else {
    cout<<"\tli    t0, "<<get_addr(ret.value)<<"\n";
    cout<<"\tadd   t0, t0, sp\n";
    cout<<"\tlw    a0, (t0)\n";
  }
  cout<<"\taddi sp, sp, "<<cur_size<<"\n";
  cout<<"\tret\n";
}

//Store指令
void Visit(const koopa_raw_store_t &store, std::ostream &cout) {
  // 访问存储指令
  if(store.value->kind.tag == KOOPA_RVT_INTEGER) {
    cout<<"\tli    t0"<<", "<<store.value->kind.data.integer.value<<"\n";
  }
  else {
    cout<<"\tli    t0, "<<get_addr(store.value)<<"\n";
    cout<<"\tadd   t0, t0, sp\n";
    cout<<"\tlw    t0, (t0)\n";
  }
  cout<<"\tli    t1, "<<get_addr(store.dest)<<"\n";
  cout<<"\tadd   t1, t1, sp\n";
  cout<<"\tsw    t0, (t1)\n";
}

//Load指令
void Visit(const koopa_raw_load_t &load, int dest_addr, std::ostream &cout) {
  // 访问加载指令
  cout<<"\tli    t0, "<<get_addr(load.src)<<"\n";
  cout<<"\tadd   t0, t0, sp\n";
  cout<<"\tlw    t0, (t0)\n";
  cout<<"\tli    t1, "<<dest_addr<<"\n";
  cout<<"\tadd   t1, t1, sp\n";
  cout<<"\tsw    t0, (t1)\n";
}

// 访问整数常量
void Visit(const koopa_raw_integer_t &integer, std::ostream &cout) {
  cout<<integer.value<<"\n";
}

void Visit(const koopa_raw_binary_t &binary_op, int dest_addr, std::ostream &cout) {
  // 访问操作符
  std::string lreg, rreg;
  std::string dest_reg = "t" + std::string(1, value_cnt+'0');

  if(binary_op.lhs->kind.tag == KOOPA_RVT_INTEGER) {
    cout<<"\tli    t0, "<<binary_op.lhs->kind.data.integer.value<<"\n";
  }
  else {
    cout<<"\tli    t0, "<<get_addr(binary_op.lhs)<<"\n";
    cout<<"\tadd   t0, t0, sp\n";
    cout<<"\tlw    t0, (t0)\n";
  }

  if(binary_op.rhs->kind.tag == KOOPA_RVT_INTEGER) {
    cout<<"\tli    t1, "<<binary_op.rhs->kind.data.integer.value<<"\n";
  }
  else {
    cout<<"\tli    t1, "<<get_addr(binary_op.rhs)<<"\n";
    cout<<"\tadd   t1, t1, sp\n";
    cout<<"\tlw    t1, (t1)\n";
  }

  switch (binary_op.op) 
  {
    case KOOPA_RBO_EQ:
      cout<<"\txor   t0, t0, t1\n";
      cout<<"\tseqz  t0, t0\n";
      break;
    case KOOPA_RBO_NOT_EQ:
      cout<<"\txor   t0, t0, t1\n";
      cout<<"\tsnez  t0, t0\n";
      break;
    case KOOPA_RBO_ADD:
      cout<<"\tadd   t0, t0, t1\n";
      break;
    case KOOPA_RBO_SUB:
      cout<<"\tsub   t0, t0, t1\n";
      break;
    case KOOPA_RBO_MUL:
      cout<<"\tmul   t0, t0, t1\n";
      break;
    case KOOPA_RBO_DIV:
      cout<<"\tdiv   t0, t0, t1\n";
      break;
    case KOOPA_RBO_MOD:
      cout<<"\trem   t0, t0, t1\n";
      break;
    case KOOPA_RBO_LT:
      cout<<"\tslt   t0, t0, t1\n";
      break;
    case KOOPA_RBO_LE:
      cout<<"\tsgt   t0, t0, t1\n";
      cout<<"\tseqz  t0, t0\n";
      break;
    case KOOPA_RBO_GT:
      cout<<"\tsgt   t0, t0, t1\n";
      break;
    case KOOPA_RBO_GE:
      cout<<"\tslt   t0, t0, t1\n";
      cout<<"\tseqz  t0, t0\n";
      break;
    case KOOPA_RBO_AND:
      cout<<"\tand   t0, t0, t1\n";
      break;
    case KOOPA_RBO_OR:
      cout<<"\tor    t0, t0, t1\n";
      break;
    default:
      assert(false);
  }
  cout<<"\tli    t1, "<<dest_addr<<"\n";
  cout<<"\tadd   t1, t1, sp\n";
  cout<<"\tsw    t0, (t1)\n";
}

void load(const koopa_raw_value_t &value, std::string r, std::ostream &cout) {
  cout<<"\tli    "<<r<<", ";
  Visit(value, cout);
}

int func_size(const koopa_raw_function_t &func) {
  int size = 0;
  for (size_t i = 0; i < func->bbs.len; ++i) {
    auto ptr = func->bbs.buffer[i];
    size += bb_size(reinterpret_cast<koopa_raw_basic_block_t>(ptr));
  }
  return size;
}

int bb_size(const koopa_raw_basic_block_t &bb) {
  int size = 0;
  for (size_t i = 0; i < bb->insts.len; ++i) {
    auto ptr = bb->insts.buffer[i];
    size += inst_size(reinterpret_cast<koopa_raw_value_t>(ptr));
  }
  return size;
}

int inst_size(const koopa_raw_value_t &inst) {
  if (inst->ty->tag != KOOPA_RTT_UNIT)
    return 4;
  return 0;
}

int get_addr(const koopa_raw_value_t &value) {
  if (addr_map.find(value) == addr_map.end()) {
    addr_map[value] = cur_size;
    cur_size += 4;
  }
  return addr_map[value];
}