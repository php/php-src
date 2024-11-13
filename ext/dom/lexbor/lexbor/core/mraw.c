/*
 * Copyright (C) 2018-2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/core/mraw.h"


#if defined(LEXBOR_HAVE_ADDRESS_SANITIZER)
    #include <sanitizer/asan_interface.h>
#endif


#define lexbor_mraw_meta_set(data, size)                                       \
    do {                                                                       \
        memcpy(data, size, sizeof(size_t));                                    \
    }                                                                          \
    while (0)

#define lexbor_mraw_data_begin(data)                                           \
    &((uint8_t *) (data))[ lexbor_mraw_meta_size() ]


lxb_inline void *
lexbor_mraw_realloc_tail(lexbor_mraw_t *mraw, void *data, void *begin,
                         size_t size, size_t begin_len, size_t new_size,
                         bool *is_valid);


lexbor_mraw_t *
lexbor_mraw_create(void)
{
    return lexbor_calloc(1, sizeof(lexbor_mraw_t));
}

lxb_status_t
lexbor_mraw_init(lexbor_mraw_t *mraw, size_t chunk_size)
{
    lxb_status_t status;

    if (mraw == NULL) {
        return LXB_STATUS_ERROR_OBJECT_IS_NULL;
    }

    if (chunk_size == 0) {
        return LXB_STATUS_ERROR_WRONG_ARGS;
    }

    /* Init memory */
    mraw->mem = lexbor_mem_create();

    status = lexbor_mem_init(mraw->mem, chunk_size + lexbor_mraw_meta_size());
    if (status) {
        return status;
    }

#if defined(LEXBOR_HAVE_ADDRESS_SANITIZER)
    ASAN_POISON_MEMORY_REGION(mraw->mem->chunk->data, mraw->mem->chunk->size);
#endif

    /* Cache */
    mraw->cache = lexbor_bst_create();

    status = lexbor_bst_init(mraw->cache, 512);
    if (status) {
        return status;
    }

    mraw->ref_count = 0;

    return LXB_STATUS_OK;
}

void
lexbor_mraw_clean(lexbor_mraw_t *mraw)
{
    if (mraw != NULL) {
        lexbor_mem_clean(mraw->mem);
        lexbor_bst_clean(mraw->cache);

        mraw->ref_count = 0;
    }
}

lexbor_mraw_t *
lexbor_mraw_destroy(lexbor_mraw_t *mraw, bool destroy_self)
{
    if (mraw == NULL) {
        return NULL;
    }

    mraw->mem = lexbor_mem_destroy(mraw->mem, true);
    mraw->cache = lexbor_bst_destroy(mraw->cache, true);

    if (destroy_self) {
        return lexbor_free(mraw);
    }

    return mraw;
}

lxb_inline void *
lexbor_mraw_mem_alloc(lexbor_mraw_t *mraw, size_t length)
{
    uint8_t *data;
    lexbor_mem_t *mem = mraw->mem;

    if (length == 0) {
        return NULL;
    }

    if ((mem->chunk->length + length) > mem->chunk->size) {
        lexbor_mem_chunk_t *chunk = mem->chunk;

        if ((SIZE_MAX - mem->chunk_length) == 0) {
            return NULL;
        }

        if (chunk->length == 0) {
            lexbor_mem_chunk_destroy(mem, chunk, false);
            lexbor_mem_chunk_init(mem, chunk, length);

            chunk->length = length;

#if defined(LEXBOR_HAVE_ADDRESS_SANITIZER)
            ASAN_POISON_MEMORY_REGION(chunk->data, chunk->size);
#endif

            return chunk->data;
        }

        size_t diff = lexbor_mem_align_floor(chunk->size - chunk->length);

        /* Save tail to cache */
        if (diff > lexbor_mraw_meta_size()) {
            diff -= lexbor_mraw_meta_size();

#if defined(LEXBOR_HAVE_ADDRESS_SANITIZER)
            ASAN_UNPOISON_MEMORY_REGION(&chunk->data[chunk->length],
                                        lexbor_mraw_meta_size());
#endif

            lexbor_mraw_meta_set(&chunk->data[chunk->length], &diff);

#if defined(LEXBOR_HAVE_ADDRESS_SANITIZER)
            ASAN_POISON_MEMORY_REGION(&chunk->data[chunk->length],
                                      diff + lexbor_mraw_meta_size());
#endif

            lexbor_bst_insert(mraw->cache,
                              lexbor_bst_root_ref(mraw->cache), diff,
                              lexbor_mraw_data_begin(&chunk->data[chunk->length]));

            chunk->length = chunk->size;
        }

        chunk->next = lexbor_mem_chunk_make(mem, length);
        if (chunk->next == NULL) {
            return NULL;
        }

        chunk->next->prev = chunk;
        mem->chunk = chunk->next;

        mem->chunk_length++;

#if defined(LEXBOR_HAVE_ADDRESS_SANITIZER)
        ASAN_POISON_MEMORY_REGION(mem->chunk->data, mem->chunk->size);
#endif
    }

    data = &mem->chunk->data[ mem->chunk->length ];
    mem->chunk->length += length;

    return data;
}

void *
lexbor_mraw_alloc(lexbor_mraw_t *mraw, size_t size)
{
    void *data;

    size = lexbor_mem_align(size);

    if (mraw->cache->tree_length != 0) {
        data = lexbor_bst_remove_close(mraw->cache,
                                       lexbor_bst_root_ref(mraw->cache),
                                       size, NULL);
        if (data != NULL) {

#if defined(LEXBOR_HAVE_ADDRESS_SANITIZER)
            uint8_t *real_data = ((uint8_t *) data) - lexbor_mraw_meta_size();

            /* Set unpoison for current data size */
            ASAN_UNPOISON_MEMORY_REGION(real_data, lexbor_mraw_meta_size());

            size_t cur_size = lexbor_mraw_data_size(data);

            ASAN_UNPOISON_MEMORY_REGION(real_data,
                                        (cur_size + lexbor_mraw_meta_size()));
#endif

            mraw->ref_count++;

            return data;
        }
    }

    data = lexbor_mraw_mem_alloc(mraw, (size + lexbor_mraw_meta_size()));

    if (data == NULL) {
        return NULL;
    }

#if defined(LEXBOR_HAVE_ADDRESS_SANITIZER)
    ASAN_UNPOISON_MEMORY_REGION(data, (size + lexbor_mraw_meta_size()));
#endif

    mraw->ref_count++;

    lexbor_mraw_meta_set(data, &size);
    return lexbor_mraw_data_begin(data);
}

void *
lexbor_mraw_calloc(lexbor_mraw_t *mraw, size_t size)
{
    void *data = lexbor_mraw_alloc(mraw, size);

    if (data != NULL) {
        memset(data, 0, lexbor_mraw_data_size(data));
    }

    return data;
}

/*
 * TODO: I don't really like this interface. Perhaps need to simplify.
 */
lxb_inline void *
lexbor_mraw_realloc_tail(lexbor_mraw_t *mraw, void *data, void *begin,
                         size_t size, size_t begin_len, size_t new_size,
                         bool *is_valid)
{
    lexbor_mem_chunk_t *chunk = mraw->mem->chunk;

    if (chunk->size > (begin_len + new_size)) {
        *is_valid = true;

        if (new_size == 0) {
            chunk->length = begin_len - lexbor_mraw_meta_size();
            return NULL;
        }

#if defined(LEXBOR_HAVE_ADDRESS_SANITIZER)
        ASAN_UNPOISON_MEMORY_REGION(begin, new_size + lexbor_mraw_meta_size());
#endif

        chunk->length = begin_len + new_size;
        memcpy(begin, &new_size, sizeof(size_t));

        return data;
    }

    /*
     * If the tail is short then we increase the current data.
     */
    if (begin_len == lexbor_mraw_meta_size()) {
        void *new_data;
        lexbor_mem_chunk_t new_chunk;

        *is_valid = true;

        lexbor_mem_chunk_init(mraw->mem, &new_chunk,
                              new_size + lexbor_mraw_meta_size());
        if(new_chunk.data == NULL) {
            return NULL;
        }

        lexbor_mraw_meta_set(new_chunk.data, &new_size);
        new_data = lexbor_mraw_data_begin(new_chunk.data);

        if (size != 0) {
            memcpy(new_data, data, sizeof(uint8_t) * size);
        }

#if defined(LEXBOR_HAVE_ADDRESS_SANITIZER)
        ASAN_UNPOISON_MEMORY_REGION(chunk->data, chunk->size);
#endif

        lexbor_mem_chunk_destroy(mraw->mem, chunk, false);

        chunk->data = new_chunk.data;
        chunk->size = new_chunk.size;
        chunk->length = new_size + lexbor_mraw_meta_size();

        return new_data;
    }

    *is_valid = false;

    /*
     * Next, this piece will go into the cache.
     */
    size = lexbor_mem_align_floor(size + (chunk->size - chunk->length));
    memcpy(begin, &size, sizeof(size_t));

    chunk->length = chunk->size;

    return NULL;
}

void *
lexbor_mraw_realloc(lexbor_mraw_t *mraw, void *data, size_t new_size)
{
    void *begin;
    size_t size, begin_len;
    lexbor_mem_chunk_t *chunk = mraw->mem->chunk;

    begin = ((uint8_t *) data) - lexbor_mraw_meta_size();
    memcpy(&size, begin, sizeof(size_t));

    new_size = lexbor_mem_align(new_size);

    /*
     * Look, whether there is an opportunity
     * to prolong the current data in chunk?
     */
    if (chunk->length >= size) {
        begin_len = chunk->length - size;

        if (&chunk->data[begin_len] == data) {
            bool is_valid;
            void *ptr = lexbor_mraw_realloc_tail(mraw, data, begin,
                                                 size, begin_len, new_size,
                                                 &is_valid);
            if (is_valid == true) {
                return ptr;
            }
        }
    }

    if (new_size < size) {
        if (new_size == 0) {

#if defined(LEXBOR_HAVE_ADDRESS_SANITIZER)
            ASAN_POISON_MEMORY_REGION(begin, size + lexbor_mraw_meta_size());
#endif
            mraw->ref_count--;

            lexbor_bst_insert(mraw->cache, lexbor_bst_root_ref(mraw->cache),
                              size, data);
            return NULL;
        }

        size_t diff = lexbor_mem_align_floor(size - new_size);

        if (diff > lexbor_mraw_meta_size()) {
            memcpy(begin, &new_size, sizeof(size_t));

            new_size = diff - lexbor_mraw_meta_size();
            begin = &((uint8_t *) data)[diff];

            lexbor_mraw_meta_set(begin, &new_size);

#if defined(LEXBOR_HAVE_ADDRESS_SANITIZER)
            ASAN_POISON_MEMORY_REGION(begin, new_size + lexbor_mraw_meta_size());
#endif
            lexbor_bst_insert(mraw->cache, lexbor_bst_root_ref(mraw->cache),
                              new_size, lexbor_mraw_data_begin(begin));
        }

        return data;
    }

    begin = lexbor_mraw_alloc(mraw, new_size);
    if (begin == NULL) {
        return NULL;
    }

    if (size != 0) {
        memcpy(begin, data, sizeof(uint8_t) * size);
    }

    lexbor_mraw_free(mraw, data);

    return begin;
}

void *
lexbor_mraw_free(lexbor_mraw_t *mraw, void *data)
{
    size_t size = lexbor_mraw_data_size(data);

#if defined(LEXBOR_HAVE_ADDRESS_SANITIZER)
    uint8_t *real_data = ((uint8_t *) data) - lexbor_mraw_meta_size();
    ASAN_POISON_MEMORY_REGION(real_data, size + lexbor_mraw_meta_size());
#endif

    lexbor_bst_insert(mraw->cache, lexbor_bst_root_ref(mraw->cache),
                      size, data);

    mraw->ref_count--;

    return NULL;
}

/*
 * No inline functions for ABI.
 */
size_t
lexbor_mraw_data_size_noi(void *data)
{
    return lexbor_mraw_data_size(data);
}

void
lexbor_mraw_data_size_set_noi(void *data, size_t size)
{
    lexbor_mraw_data_size_set(data, size);
}

void *
lexbor_mraw_dup_noi(lexbor_mraw_t *mraw, const void *src, size_t size)
{
    return lexbor_mraw_dup(mraw, src, size);
}
