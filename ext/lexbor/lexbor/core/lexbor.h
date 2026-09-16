/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_H
#define LEXBOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/def.h"

typedef void *(*lexbor_memory_malloc_f)(size_t size);
typedef void *(*lexbor_memory_realloc_f)(void *dst, size_t size);
typedef void *(*lexbor_memory_calloc_f)(size_t num, size_t size);
typedef void (*lexbor_memory_free_f)(void *dst);

LXB_API void *
lexbor_malloc(size_t size);

LXB_API void *
lexbor_realloc(void *dst, size_t size);

LXB_API void *
lexbor_calloc(size_t num, size_t size);

LXB_API void *
lexbor_free(void *dst);

LXB_API lxb_status_t
lexbor_memory_setup(lexbor_memory_malloc_f new_malloc, lexbor_memory_realloc_f new_realloc,
                    lexbor_memory_calloc_f new_calloc, lexbor_memory_free_f new_free);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_H */

