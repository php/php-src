/*
 * Copyright (C) 2018-2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/core/dobject.h"


#if defined(LEXBOR_HAVE_ADDRESS_SANITIZER)
    #include <sanitizer/asan_interface.h>
#endif


lexbor_dobject_t *
lexbor_dobject_create(void)
{
    return lexbor_calloc(1, sizeof(lexbor_dobject_t));
}

lxb_status_t
lexbor_dobject_init(lexbor_dobject_t *dobject,
                    size_t chunk_size, size_t struct_size)
{
    lxb_status_t status;

    if (dobject == NULL) {
        return LXB_STATUS_ERROR_OBJECT_IS_NULL;
    }

    if (chunk_size == 0 || struct_size == 0) {
        return LXB_STATUS_ERROR_WRONG_ARGS;
    }

    /* Set params */
    dobject->allocated = 0UL;
    dobject->struct_size = struct_size;

    /* Init memory */
    dobject->mem = lexbor_mem_create();

    status = lexbor_mem_init(dobject->mem,
                           lexbor_mem_align(chunk_size * dobject->struct_size));
    if (status) {
        return status;
    }

#if defined(LEXBOR_HAVE_ADDRESS_SANITIZER)
    ASAN_POISON_MEMORY_REGION(dobject->mem->chunk->data,
                              dobject->mem->chunk->size);
#endif

    /* Array */
    dobject->cache = lexbor_array_create();

    status = lexbor_array_init(dobject->cache, chunk_size);
    if (status)
        return status;

    return LXB_STATUS_OK;
}

void
lexbor_dobject_clean(lexbor_dobject_t *dobject)
{
    if (dobject != NULL) {
        dobject->allocated = 0UL;

        lexbor_mem_clean(dobject->mem);
        lexbor_array_clean(dobject->cache);
    }
}

lexbor_dobject_t *
lexbor_dobject_destroy(lexbor_dobject_t *dobject, bool destroy_self)
{
    if (dobject == NULL)
        return NULL;

    dobject->mem = lexbor_mem_destroy(dobject->mem, true);
    dobject->cache = lexbor_array_destroy(dobject->cache, true);

    if (destroy_self == true) {
        return lexbor_free(dobject);
    }

    return dobject;
}

void *
lexbor_dobject_alloc(lexbor_dobject_t *dobject)
{
    void *data;

    if (lexbor_array_length(dobject->cache) != 0) {
        dobject->allocated++;

#if defined(LEXBOR_HAVE_ADDRESS_SANITIZER)
        data = lexbor_array_pop(dobject->cache);
        ASAN_UNPOISON_MEMORY_REGION(data, dobject->struct_size);

        return data;
#else
        return lexbor_array_pop(dobject->cache);
#endif
    }

    data = lexbor_mem_alloc(dobject->mem, dobject->struct_size);
    if (data == NULL) {
        return NULL;
    }

#if defined(LEXBOR_HAVE_ADDRESS_SANITIZER)
    ASAN_UNPOISON_MEMORY_REGION(data, dobject->struct_size);
#endif

    dobject->allocated++;

    return data;
}

void *
lexbor_dobject_calloc(lexbor_dobject_t *dobject)
{
    void *data = lexbor_dobject_alloc(dobject);

    if (data != NULL) {
        memset(data, 0, dobject->struct_size);
    }

    return data;
}

void *
lexbor_dobject_free(lexbor_dobject_t *dobject, void *data)
{
    if (data == NULL) {
        return NULL;
    }

#if defined(LEXBOR_HAVE_ADDRESS_SANITIZER)
    ASAN_POISON_MEMORY_REGION(data, dobject->struct_size);
#endif

    if (lexbor_array_push(dobject->cache, data) == LXB_STATUS_OK) {
        dobject->allocated--;
        return NULL;
    }

    return data;
}

void *
lexbor_dobject_by_absolute_position(lexbor_dobject_t *dobject, size_t pos)
{
    size_t chunk_idx, chunk_pos, i;
    lexbor_mem_chunk_t *chunk;

    if (pos >= dobject->allocated) {
        return NULL;
    }

    chunk = dobject->mem->chunk_first;
    chunk_pos = pos * dobject->struct_size;
    chunk_idx = chunk_pos / dobject->mem->chunk_min_size;

    for (i = 0; i < chunk_idx; i++) {
        chunk = chunk->next;
    }

    return &chunk->data[chunk_pos % chunk->size];
}

/*
 * No inline functions for ABI.
 */
size_t
lexbor_dobject_allocated_noi(lexbor_dobject_t *dobject)
{
    return lexbor_dobject_allocated(dobject);
}

size_t
lexbor_dobject_cache_length_noi(lexbor_dobject_t *dobject)
{
    return lexbor_dobject_cache_length(dobject);
}
