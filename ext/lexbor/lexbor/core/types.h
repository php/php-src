/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_TYPES_H
#define LEXBOR_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* Inline */
#ifdef _MSC_VER
    #define lxb_inline static __forceinline
#else
    #define lxb_inline static inline
#endif


/* Simple types */
typedef uint32_t      lxb_codepoint_t;
typedef unsigned char lxb_char_t;
typedef unsigned int  lxb_status_t;

/* Callbacks */
typedef lxb_status_t (*lexbor_callback_f)(const lxb_char_t *buffer,
                                          size_t size, void *ctx);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_TYPES_H */
