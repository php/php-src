/*
 * Copyright (C) 2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_SERIALIZE_H
#define LEXBOR_SERIALIZE_H


#include "lexbor/core/base.h"


#define lexbor_serialize_write(cb, data, length, ctx, status)                 \
    do {                                                                      \
        (status) = (cb)((lxb_char_t *) (data), (length), (ctx));              \
        if ((status) != LXB_STATUS_OK) {                                      \
            return (status);                                                  \
        }                                                                     \
    }                                                                         \
    while (false)


LXB_API lxb_status_t
lexbor_serialize_length_cb(const lxb_char_t *data, size_t length, void *ctx);


LXB_API lxb_status_t
lexbor_serialize_copy_cb(const lxb_char_t *data, size_t length, void *ctx);


#endif /* LEXBOR_SERIALIZE_H */
