/*
 * Copyright (C) Alexander Borisov
 *
 * Based on nxt_dtoa.h from NGINX NJS project
 *
 * Copyright (C) Dmitry Volyntsev
 * Copyright (C) Nginx, Inc.
 */

#ifndef LEXBOR_DTOA_H
#define LEXBOR_DTOA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/base.h"


LXB_API size_t
lexbor_dtoa(double value, lxb_char_t *begin, size_t len);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_DTOA_H */
