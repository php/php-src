/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_ARRAY_H
#define LEXBOR_ARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/base.h"


typedef struct {
    void   **list;
    size_t size;
    size_t length;
}
lexbor_array_t;


LXB_API lexbor_array_t *
lexbor_array_create(void);

LXB_API lxb_status_t
lexbor_array_init(lexbor_array_t *array, size_t size);

LXB_API void
lexbor_array_clean(lexbor_array_t *array);

LXB_API lexbor_array_t *
lexbor_array_destroy(lexbor_array_t *array, bool self_destroy);


LXB_API void **
lexbor_array_expand(lexbor_array_t *array, size_t up_to);


LXB_API lxb_status_t
lexbor_array_push(lexbor_array_t *array, void *value);

LXB_API void *
lexbor_array_pop(lexbor_array_t *array);

LXB_API lxb_status_t
lexbor_array_insert(lexbor_array_t *array, size_t idx, void *value);

LXB_API lxb_status_t
lexbor_array_set(lexbor_array_t *array, size_t idx, void *value);

LXB_API void
lexbor_array_delete(lexbor_array_t *array, size_t begin, size_t length);


/*
 * Inline functions
 */
lxb_inline void *
lexbor_array_get(lexbor_array_t *array, size_t idx)
{
    if (idx >= array->length) {
        return NULL;
    }

    return array->list[idx];
}

lxb_inline size_t
lexbor_array_length(lexbor_array_t *array)
{
    return array->length;
}

lxb_inline size_t
lexbor_array_size(lexbor_array_t *array)
{
    return array->size;
}

/*
 * No inline functions for ABI.
 */
LXB_API void *
lexbor_array_get_noi(lexbor_array_t *array, size_t idx);

LXB_API size_t
lexbor_array_length_noi(lexbor_array_t *array);

LXB_API size_t
lexbor_array_size_noi(lexbor_array_t *array);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_ARRAY_H */
