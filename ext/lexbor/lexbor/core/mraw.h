/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_MRAW_H
#define LEXBOR_MRAW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

#include "lexbor/core/base.h"
#include "lexbor/core/mem.h"
#include "lexbor/core/bst.h"


#define lexbor_mraw_meta_size()                                                \
    (((sizeof(size_t) % LEXBOR_MEM_ALIGN_STEP) != 0)                           \
    ? sizeof(size_t)                                                           \
        + (LEXBOR_MEM_ALIGN_STEP - (sizeof(size_t) % LEXBOR_MEM_ALIGN_STEP))   \
    : sizeof(size_t))


typedef struct {
    lexbor_mem_t *mem;
    lexbor_bst_t *cache;
    size_t       ref_count;
}
lexbor_mraw_t;


LXB_API lexbor_mraw_t *
lexbor_mraw_create(void);

LXB_API lxb_status_t
lexbor_mraw_init(lexbor_mraw_t *mraw, size_t chunk_size);

LXB_API void
lexbor_mraw_clean(lexbor_mraw_t *mraw);

LXB_API lexbor_mraw_t *
lexbor_mraw_destroy(lexbor_mraw_t *mraw, bool destroy_self);


LXB_API void *
lexbor_mraw_alloc(lexbor_mraw_t *mraw, size_t size);

LXB_API void *
lexbor_mraw_calloc(lexbor_mraw_t *mraw, size_t size);

LXB_API void *
lexbor_mraw_realloc(lexbor_mraw_t *mraw, void *data, size_t new_size);

LXB_API void *
lexbor_mraw_free(lexbor_mraw_t *mraw, void *data);


/*
 * Inline functions
 */
lxb_inline size_t
lexbor_mraw_data_size(void *data)
{
    return *((size_t *) (((uint8_t *) data) - lexbor_mraw_meta_size()));
}

lxb_inline void
lexbor_mraw_data_size_set(void *data, size_t size)
{
    data = (((uint8_t *) data) - lexbor_mraw_meta_size());
    memcpy(data, &size, sizeof(size_t));
}

lxb_inline void *
lexbor_mraw_dup(lexbor_mraw_t *mraw, const void *src, size_t size)
{
    void *data = lexbor_mraw_alloc(mraw, size);

    if (data != NULL) {
        memcpy(data, src, size);
    }

    return data;
}

lxb_inline size_t
lexbor_mraw_reference_count(lexbor_mraw_t *mraw)
{
    return mraw->ref_count;
}


/*
 * No inline functions for ABI.
 */
LXB_API size_t
lexbor_mraw_data_size_noi(void *data);

LXB_API void
lexbor_mraw_data_size_set_noi(void *data, size_t size);

LXB_API void *
lexbor_mraw_dup_noi(lexbor_mraw_t *mraw, const void *src, size_t size);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_MRAW_H */
