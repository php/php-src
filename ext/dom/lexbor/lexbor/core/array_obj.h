/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_ARRAY_OBJ_H
#define LEXBOR_ARRAY_OBJ_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/base.h"


typedef struct {
    uint8_t *list;
    size_t  size;
    size_t  length;
    size_t  struct_size;
}
lexbor_array_obj_t;


LXB_API lexbor_array_obj_t *
lexbor_array_obj_create(void);

LXB_API lxb_status_t
lexbor_array_obj_init(lexbor_array_obj_t *array,
                      size_t size, size_t struct_size);

LXB_API void
lexbor_array_obj_clean(lexbor_array_obj_t *array);

LXB_API lexbor_array_obj_t *
lexbor_array_obj_destroy(lexbor_array_obj_t *array, bool self_destroy);


LXB_API uint8_t *
lexbor_array_obj_expand(lexbor_array_obj_t *array, size_t up_to);


LXB_API void *
lexbor_array_obj_push(lexbor_array_obj_t *array);

LXB_API void *
lexbor_array_obj_push_wo_cls(lexbor_array_obj_t *array);

LXB_API void *
lexbor_array_obj_push_n(lexbor_array_obj_t *array, size_t count);

LXB_API void *
lexbor_array_obj_pop(lexbor_array_obj_t *array);

LXB_API void
lexbor_array_obj_delete(lexbor_array_obj_t *array, size_t begin, size_t length);


/*
 * Inline functions
 */
lxb_inline void
lexbor_array_obj_erase(lexbor_array_obj_t *array)
{
    memset(array, 0, sizeof(lexbor_array_obj_t));
}

lxb_inline void *
lexbor_array_obj_get(const lexbor_array_obj_t *array, size_t idx)
{
    if (idx >= array->length) {
        return NULL;
    }

    return array->list + (idx * array->struct_size);
}

lxb_inline size_t
lexbor_array_obj_length(lexbor_array_obj_t *array)
{
    return array->length;
}

lxb_inline size_t
lexbor_array_obj_size(lexbor_array_obj_t *array)
{
    return array->size;
}

lxb_inline size_t
lexbor_array_obj_struct_size(lexbor_array_obj_t *array)
{
    return array->struct_size;
}

lxb_inline void *
lexbor_array_obj_last(lexbor_array_obj_t *array)
{
    if (array->length == 0) {
        return NULL;
    }

    return array->list + ((array->length - 1) * array->struct_size);
}


/*
 * No inline functions for ABI.
 */
LXB_API void
lexbor_array_obj_erase_noi(lexbor_array_obj_t *array);

LXB_API void *
lexbor_array_obj_get_noi(lexbor_array_obj_t *array, size_t idx);

LXB_API size_t
lexbor_array_obj_length_noi(lexbor_array_obj_t *array);

LXB_API size_t
lexbor_array_obj_size_noi(lexbor_array_obj_t *array);

LXB_API size_t
lexbor_array_obj_struct_size_noi(lexbor_array_obj_t *array);

LXB_API void *
lexbor_array_obj_last_noi(lexbor_array_obj_t *array);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_ARRAY_OBJ_H */
