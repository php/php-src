/*
 * Copyright (C) 2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_PRINT_H
#define LEXBOR_PRINT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

#include "lexbor/core/base.h"


#define LXB_PRINT_ERROR ((size_t) - 1)


LXB_API size_t
lexbor_printf_size(const char *format, ...);

LXB_API size_t
lexbor_vprintf_size(const char *format, va_list va);

LXB_API size_t
lexbor_sprintf(lxb_char_t *dst, size_t size, const char *format, ...);

LXB_API size_t
lexbor_vsprintf(lxb_char_t *dst, size_t size, const char *format, va_list va);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_PRINT_H */
