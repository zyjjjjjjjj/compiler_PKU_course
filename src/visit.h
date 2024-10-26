#pragma once
#include "koopa.h"
#include "utils.h"
#include <stdio.h>
#include <iostream>
#include <cassert>
#include <string>

void Visit(const koopa_raw_program_t &program, std::ostream &cout);
void Visit(const koopa_raw_slice_t &slice, std::ostream &cout);
void Visit(const koopa_raw_function_t &function, std::ostream &cout);
void Visit(const koopa_raw_basic_block_t &basic_block, std::ostream &cout);
void Visit(const koopa_raw_value_t &value, std::ostream &cout);
void Visit(const koopa_raw_integer_t &integer, std::ostream &cout);
void Visit(const koopa_raw_return_t &ret, std::ostream &cout);
void Visit(const koopa_raw_binary_t &binary_op, char value_cnt, std::ostream &cout);

void load(const koopa_raw_value_t &value, std::string r, std::ostream &cout);