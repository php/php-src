/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/core/array.h"


lexbor_array_t *
lexbor_array_create(void)
{
    return lexbor_calloc(1, sizeof(lexbor_array_t));
}

lxb_status_t
lexbor_array_init(lexbor_array_t *array, size_t size)
{
    if (array == NULL) {
        return LXB_STATUS_ERROR_OBJECT_IS_NULL;
    }

    if (size == 0) {
        return LXB_STATUS_ERROR_TOO_SMALL_SIZE;
    }

    array->length = 0;
    array->size = size;

    array->list = lexbor_malloc(sizeof(void *) * size);
    if (array->list == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    return LXB_STATUS_OK;
}

void
lexbor_array_clean(lexbor_array_t *array)
{
    if (array != NULL) {
        array->length = 0;
    }
}

lexbor_array_t *
lexbor_array_destroy(lexbor_array_t *array, bool self_destroy)
{
    if (array == NULL)
        return NULL;

    if (array->list) {
        array->length = 0;
        array->size = 0;
        array->list = lexbor_free(array->list);
    }

    if (self_destroy) {
        return lexbor_free(array);
    }

    return array;
}

void **
lexbor_array_expand(lexbor_array_t *array, size_t up_to)
{
    void **list;
    size_t new_size;

    if (array->length > (SIZE_MAX - up_to))
        return NULL;

    new_size = array->length + up_to;
    list = lexbor_realloc(array->list, sizeof(void *) * new_size);

    if (list == NULL)
        return NULL;

    array->list = list;
    array->size = new_size;

    return list;
}

lxb_status_t
lexbor_array_push(lexbor_array_t *array, void *value)
{
    if (array->length >= array->size) {
        if ((lexbor_array_expand(array, 128) == NULL)) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }

    array->list[ array->length ] = value;
    array->length++;

    return LXB_STATUS_OK;
}

void *
lexbor_array_pop(lexbor_array_t *array)
{
    if (array->length == 0) {
        return NULL;
    }

    array->length--;
    return array->list[ array->length ];
}

lxb_status_t
lexbor_array_insert(lexbor_array_t *array, size_t idx, void *value)
{
    if (idx >= array->length) {
        size_t up_to = (idx - array->length) + 1;

        if (idx >= array->size) {
            if ((lexbor_array_expand(array, up_to) == NULL)) {
                return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
            }
        }

        memset(&array->list[array->length], 0, sizeof(void *) * up_to);

        array->list[ idx ] = value;
        array->length += up_to;

        return LXB_STATUS_OK;
    }

    if (array->length >= array->size) {
        if ((lexbor_array_expand(array, 32) == NULL)) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }

    memmove(&array->list[idx + 1], &array->list[idx],
            sizeof(void *) * (array->length - idx));

    array->list[ idx ] = value;
    array->length++;

    return LXB_STATUS_OK;
}

lxb_status_t
lexbor_array_set(lexbor_array_t *array, size_t idx, void *value)
{
    if (idx >= array->length) {
        size_t up_to = (idx - array->length) + 1;

        if (idx >= array->size) {
            if ((lexbor_array_expand(array, up_to) == NULL)) {
                return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
            }
        }

        memset(&array->list[array->length], 0, sizeof(void *) * up_to);

        array->length += up_to;
    }

    array->list[idx] = value;

    return LXB_STATUS_OK;
}

void
lexbor_array_delete(lexbor_array_t *array, size_t begin, size_t length)
{
    if (begin >= array->length || length == 0) {
        return;
    }

    size_t end_len = begin + length;

    if (end_len >= array->length) {
        array->length = begin;
        return;
    }

    memmove(&array->list[begin], &array->list[end_len],
            sizeof(void *) * (array->length - end_len));

    array->length -= length;
}

/*
 * No inline functions.
 */
void *
lexbor_array_get_noi(lexbor_array_t *array, size_t idx)
{
    return lexbor_array_get(array, idx);
}

size_t
lexbor_array_length_noi(lexbor_array_t *array)
{
    return lexbor_array_length(array);
}

size_t
lexbor_array_size_noi(lexbor_array_t *array)
{
    return lexbor_array_size(array);
}
