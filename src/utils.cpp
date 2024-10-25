#include "utils.h"

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

char *add_prefix(const char *prefix, const char *name) {
    char *new_name = new char[strlen(prefix) + strlen(name) + 1];
    strcpy(new_name, prefix);
    strcat(new_name, name);
    return new_name;
}