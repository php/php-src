/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_MEM_H
#define LEXBOR_MEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

#include "lexbor/core/base.h"


typedef struct lexbor_mem_chunk lexbor_mem_chunk_t;
typedef struct lexbor_mem lexbor_mem_t;

struct lexbor_mem_chunk {
    uint8_t            *data;
    size_t             length;
    size_t             size;

    lexbor_mem_chunk_t *next;
    lexbor_mem_chunk_t *prev;
};

struct lexbor_mem {
    lexbor_mem_chunk_t *chunk;
    lexbor_mem_chunk_t *chunk_first;

    size_t             chunk_min_size;
    size_t             chunk_length;
};


LXB_API lexbor_mem_t *
lexbor_mem_create(void);

LXB_API lxb_status_t
lexbor_mem_init(lexbor_mem_t *mem, size_t min_chunk_size);

LXB_API void
lexbor_mem_clean(lexbor_mem_t *mem);

LXB_API lexbor_mem_t *
lexbor_mem_destroy(lexbor_mem_t *mem, bool destroy_self);


/*
 * The memory allocated in lexbor_mem_chunk_* functions needs to be freed
 * by lexbor_mem_chunk_destroy function.
 *
 * This memory will not be automatically freed by a function lexbor_mem_destroy.
 */
LXB_API uint8_t *
lexbor_mem_chunk_init(lexbor_mem_t *mem,
                      lexbor_mem_chunk_t *chunk, size_t length);

LXB_API lexbor_mem_chunk_t *
lexbor_mem_chunk_make(lexbor_mem_t *mem, size_t length);

LXB_API lexbor_mem_chunk_t *
lexbor_mem_chunk_destroy(lexbor_mem_t *mem,
                         lexbor_mem_chunk_t *chunk, bool self_destroy);

/*
 * The memory allocated in lexbor_mem_alloc and lexbor_mem_calloc function
 * will be freeds after calling lexbor_mem_destroy function.
 */
LXB_API void *
lexbor_mem_alloc(lexbor_mem_t *mem, size_t length);

LXB_API void *
lexbor_mem_calloc(lexbor_mem_t *mem, size_t length);


/*
 * Inline functions
 */
lxb_inline size_t
lexbor_mem_current_length(lexbor_mem_t *mem)
{
    return mem->chunk->length;
}

lxb_inline size_t
lexbor_mem_current_size(lexbor_mem_t *mem)
{
    return mem->chunk->size;
}

lxb_inline size_t
lexbor_mem_chunk_length(lexbor_mem_t *mem)
{
    return mem->chunk_length;
}

lxb_inline size_t
lexbor_mem_align(size_t size)
{
    return ((size % LEXBOR_MEM_ALIGN_STEP) != 0)
           ? size + (LEXBOR_MEM_ALIGN_STEP - (size % LEXBOR_MEM_ALIGN_STEP))
           : size;
}

lxb_inline size_t
lexbor_mem_align_floor(size_t size)
{
    return ((size % LEXBOR_MEM_ALIGN_STEP) != 0)
           ? size - (size % LEXBOR_MEM_ALIGN_STEP)
           : size;
}

/*
 * No inline functions for ABI.
 */
LXB_API size_t
lexbor_mem_current_length_noi(lexbor_mem_t *mem);

LXB_API size_t
lexbor_mem_current_size_noi(lexbor_mem_t *mem);

LXB_API size_t
lexbor_mem_chunk_length_noi(lexbor_mem_t *mem);

LXB_API size_t
lexbor_mem_align_noi(size_t size);

LXB_API size_t
lexbor_mem_align_floor_noi(size_t size);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_MEM_H */
