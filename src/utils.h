#pragma once
#include "koopa.h"
#include "visit.h"
#include <vector>
#include <cstring>
#include <stdio.h>


koopa_raw_slice_t make_slice(std::vector<const void *> *buf, koopa_raw_slice_item_kind_t kind);

koopa_raw_type_kind_t *make_func_ty(std::vector<const void *> *buf, const koopa_raw_type_kind *ret);

koopa_raw_type_kind_t *make_ty(koopa_raw_type_tag_t tag);

char *add_prefix(const char *prefix, const char *name);

char *remove_prefix(const char *prefix, const char *name);

void koopa_dump_riscv_to_file(koopa_raw_program_t &program, const char *path);
