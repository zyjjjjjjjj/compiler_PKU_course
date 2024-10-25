#pragma once
#include "koopa.h"
#include <vector>
#include <cstring>


koopa_raw_slice_t make_slice(std::vector<const void *> *buf, koopa_raw_slice_item_kind_t kind);

koopa_raw_type_kind_t *make_func_ty(std::vector<const void *> *buf, const koopa_raw_type_kind *ret);

koopa_raw_type_kind_t *make_ty(koopa_raw_type_tag_t tag);

char *add_prefix(const char *prefix, const char *name);