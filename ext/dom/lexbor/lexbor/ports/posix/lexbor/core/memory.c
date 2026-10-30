/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/core/base.h"

static lexbor_memory_malloc_f lexbor_memory_malloc = malloc;
static lexbor_memory_realloc_f lexbor_memory_realloc = realloc;
static lexbor_memory_calloc_f lexbor_memory_calloc = calloc;
static lexbor_memory_free_f lexbor_memory_free = free;

void *
lexbor_malloc(size_t size)
{
    return lexbor_memory_malloc(size);
}

void *
lexbor_realloc(void *dst, size_t size)
{
    return lexbor_memory_realloc(dst, size);
}

void *
lexbor_calloc(size_t num, size_t size)
{
    return lexbor_memory_calloc(num, size);
}

void *
lexbor_free(void *dst)
{
    lexbor_memory_free(dst);
    return NULL;
}

lxb_status_t
lexbor_memory_setup(lexbor_memory_malloc_f new_malloc, lexbor_memory_realloc_f new_realloc,
                    lexbor_memory_calloc_f new_calloc, lexbor_memory_free_f new_free)
{
    if (new_malloc == NULL || new_realloc == NULL || new_calloc == NULL || new_free == NULL) {
        return LXB_STATUS_ERROR_OBJECT_IS_NULL;
    }

    lexbor_memory_malloc = new_malloc;
    lexbor_memory_realloc = new_realloc;
    lexbor_memory_calloc = new_calloc;
    lexbor_memory_free = new_free;

    return LXB_STATUS_OK;
}
