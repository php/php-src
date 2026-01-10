/*
 * Copyright (C) 2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/core/serialize.h"
#include "lexbor/core/str.h"


lxb_status_t
lexbor_serialize_length_cb(const lxb_char_t *data, size_t length, void *ctx)
{
    *((size_t *) ctx) += length;
    return LXB_STATUS_OK;
}

lxb_status_t
lexbor_serialize_copy_cb(const lxb_char_t *data, size_t length, void *ctx)
{
    lexbor_str_t *str = ctx;

    memcpy(str->data + str->length, data, length);
    str->length += length;

    return LXB_STATUS_OK;
}
