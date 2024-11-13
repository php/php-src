/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_CONV_H
#define LEXBOR_CONV_H

#ifdef __cplusplus
extern "C" {
#endif


#include "lexbor/core/base.h"


LXB_API size_t
lexbor_conv_float_to_data(double num, lxb_char_t *buf, size_t len);

LXB_API size_t
lexbor_conv_long_to_data(long num, lxb_char_t *buf, size_t len);

LXB_API size_t
lexbor_conv_int64_to_data(int64_t num, lxb_char_t *buf, size_t len);

LXB_API double
lexbor_conv_data_to_double(const lxb_char_t **start, size_t len);

LXB_API unsigned long
lexbor_conv_data_to_ulong(const lxb_char_t **data, size_t length);

LXB_API long
lexbor_conv_data_to_long(const lxb_char_t **data, size_t length);

LXB_API unsigned
lexbor_conv_data_to_uint(const lxb_char_t **data, size_t length);

LXB_API size_t
lexbor_conv_dec_to_hex(uint32_t number, lxb_char_t *out, size_t length);

lxb_inline long
lexbor_conv_double_to_long(double number)
{
    if (number > (double) LONG_MAX) {
        return LONG_MAX;
    }

    if (number < (double) LONG_MIN) {
        return -LONG_MAX;
    }

    return (long) number;
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_CONV_H */
