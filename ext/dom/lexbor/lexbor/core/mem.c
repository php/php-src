/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/core/mem.h"


lexbor_mem_t *
lexbor_mem_create(void)
{
    return lexbor_calloc(1, sizeof(lexbor_mem_t));
}

lxb_status_t
lexbor_mem_init(lexbor_mem_t *mem, size_t min_chunk_size)
{
    if (mem == NULL) {
        return LXB_STATUS_ERROR_OBJECT_IS_NULL;
    }

    if (min_chunk_size == 0) {
        return LXB_STATUS_ERROR_WRONG_ARGS;
    }

    mem->chunk_min_size = lexbor_mem_align(min_chunk_size);

    /* Create first chunk */
    mem->chunk = lexbor_mem_chunk_make(mem, mem->chunk_min_size);
    if (mem->chunk == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    mem->chunk_length = 1;
    mem->chunk_first = mem->chunk;

    return LXB_STATUS_OK;
}

void
lexbor_mem_clean(lexbor_mem_t *mem)
{
    lexbor_mem_chunk_t *prev, *chunk;

    if (mem == NULL) {
        return;
    }

    chunk = mem->chunk;

    while (chunk->prev) {
        prev = chunk->prev;

        chunk->data = lexbor_free(chunk->data);
        lexbor_free(chunk);

        chunk = prev;
    }

    chunk->next = NULL;
    chunk->length = 0;

    mem->chunk = mem->chunk_first;
    mem->chunk_length = 1;
}

lexbor_mem_t *
lexbor_mem_destroy(lexbor_mem_t *mem, bool destroy_self)
{
    lexbor_mem_chunk_t *chunk, *prev;

    if (mem == NULL) {
        return NULL;
    }

    /* Destroy all chunk */
    if (mem->chunk) {
        chunk = mem->chunk;

        while (chunk) {
            prev = chunk->prev;
            lexbor_mem_chunk_destroy(mem, chunk, true);
            chunk = prev;
        }

        mem->chunk = NULL;
    }

    if (destroy_self) {
        return lexbor_free(mem);
    }

    return mem;
}

uint8_t *
lexbor_mem_chunk_init(lexbor_mem_t *mem,
                      lexbor_mem_chunk_t *chunk, size_t length)
{
    length = lexbor_mem_align(length);

    if (length > mem->chunk_min_size) {
        if (mem->chunk_min_size > (SIZE_MAX - length)) {
            chunk->size = length;
        }
        else {
            chunk->size = length + mem->chunk_min_size;
        }
    }
    else {
        chunk->size = mem->chunk_min_size;
    }

    chunk->length = 0;
    chunk->data = lexbor_malloc(chunk->size * sizeof(uint8_t));

    return chunk->data;
}

lexbor_mem_chunk_t *
lexbor_mem_chunk_make(lexbor_mem_t *mem, size_t length)
{
    lexbor_mem_chunk_t *chunk = lexbor_calloc(1, sizeof(lexbor_mem_chunk_t));

    if (chunk == NULL) {
        return NULL;
    }

    if (lexbor_mem_chunk_init(mem, chunk, length) == NULL) {
        return lexbor_free(chunk);
    }

    return chunk;
}

lexbor_mem_chunk_t *
lexbor_mem_chunk_destroy(lexbor_mem_t *mem,
                         lexbor_mem_chunk_t *chunk, bool self_destroy)
{
    if (chunk == NULL || mem == NULL) {
        return NULL;
    }

    if (chunk->data) {
        chunk->data = lexbor_free(chunk->data);
    }

    if (self_destroy) {
        return lexbor_free(chunk);
    }

    return chunk;
}

void *
lexbor_mem_alloc(lexbor_mem_t *mem, size_t length)
{
    if (length == 0) {
        return NULL;
    }

    length = lexbor_mem_align(length);

    if ((mem->chunk->length + length) > mem->chunk->size) {
        if ((SIZE_MAX - mem->chunk_length) == 0) {
            return NULL;
        }

        mem->chunk->next = lexbor_mem_chunk_make(mem, length);
        if (mem->chunk->next == NULL) {
            return NULL;
        }

        mem->chunk->next->prev = mem->chunk;
        mem->chunk = mem->chunk->next;

        mem->chunk_length++;
    }

    mem->chunk->length += length;

    return &mem->chunk->data[(mem->chunk->length - length)];
}

void *
lexbor_mem_calloc(lexbor_mem_t *mem, size_t length)
{
    void *data = lexbor_mem_alloc(mem, length);

    if (data != NULL) {
        memset(data, 0, length);
    }

    return data;
}

/*
 * No inline functions for ABI.
 */
size_t
lexbor_mem_current_length_noi(lexbor_mem_t *mem)
{
    return lexbor_mem_current_length(mem);
}

size_t
lexbor_mem_current_size_noi(lexbor_mem_t *mem)
{
    return lexbor_mem_current_size(mem);
}

size_t
lexbor_mem_chunk_length_noi(lexbor_mem_t *mem)
{
    return lexbor_mem_chunk_length(mem);
}
size_t
lexbor_mem_align_noi(size_t size)
{
    return lexbor_mem_align(size);
}

size_t
lexbor_mem_align_floor_noi(size_t size)
{
    return lexbor_mem_align_floor(size);
}
