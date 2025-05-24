/*
 * Copyright (C) Alexander Borisov
 *
 * Based on nxt_strtod.h from NGINX NJS project
 *
 * Copyright (C) Dmitry Volyntsev
 * Copyright (C) Nginx, Inc.
 */

#ifndef LEXBOR_STRTOD_H
#define LEXBOR_STRTOD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/base.h"


LXB_API double
lexbor_strtod_internal(const lxb_char_t *start, size_t length, int exp);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_STRTOD_H */
