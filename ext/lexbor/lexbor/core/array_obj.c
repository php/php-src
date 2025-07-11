/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/core/array_obj.h"


lexbor_array_obj_t *
lexbor_array_obj_create(void)
{
    return lexbor_calloc(1, sizeof(lexbor_array_obj_t));
}

lxb_status_t
lexbor_array_obj_init(lexbor_array_obj_t *array,
                      size_t size, size_t struct_size)
{
    if (array == NULL) {
        return LXB_STATUS_ERROR_OBJECT_IS_NULL;
    }

    if (size == 0 || struct_size == 0) {
        return LXB_STATUS_ERROR_TOO_SMALL_SIZE;
    }

    array->length = 0;
    array->size = size;
    array->struct_size = struct_size;

    array->list = lexbor_malloc(sizeof(uint8_t *)
                                * (array->size * struct_size));
    if (array->list == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    return LXB_STATUS_OK;
}

void
lexbor_array_obj_clean(lexbor_array_obj_t *array)
{
    if (array != NULL) {
        array->length = 0;
    }
}

lexbor_array_obj_t *
lexbor_array_obj_destroy(lexbor_array_obj_t *array, bool self_destroy)
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

uint8_t *
lexbor_array_obj_expand(lexbor_array_obj_t *array, size_t up_to)
{
    uint8_t *list;
    size_t new_size;

    if (array->length > (SIZE_MAX - up_to)) {
        return NULL;
    }

    new_size = array->length + up_to;

    list = lexbor_realloc(array->list, sizeof(uint8_t *)
                          * (new_size * array->struct_size));
    if (list == NULL) {
        return NULL;
    }

    array->list = list;
    array->size = new_size;

    return list;
}

void *
lexbor_array_obj_push(lexbor_array_obj_t *array)
{
    void *entry;

    if (array->length >= array->size)
    {
        if ((lexbor_array_obj_expand(array, 128) == NULL)) {
            return NULL;
        }
    }

    entry = array->list + (array->length * array->struct_size);
    array->length++;

    memset(entry, 0, array->struct_size);

    return entry;
}

void *
lexbor_array_obj_push_wo_cls(lexbor_array_obj_t *array)
{
    void *entry;

    if (array->length >= array->size) {
        if ((lexbor_array_obj_expand(array, 128) == NULL)) {
            return NULL;
        }
    }

    entry = array->list + (array->length * array->struct_size);
    array->length++;

    return entry;
}

void *
lexbor_array_obj_push_n(lexbor_array_obj_t *array, size_t count)
{
    void *entry;

    if ((array->length + count) > array->size) {
        if ((lexbor_array_obj_expand(array, count + 128) == NULL)) {
            return NULL;
        }
    }

    entry = array->list + (array->length * array->struct_size);
    array->length += count;

    return entry;
}

void *
lexbor_array_obj_pop(lexbor_array_obj_t *array)
{
    if (array->length == 0) {
        return NULL;
    }

    array->length--;
    return array->list + (array->length * array->struct_size);
}

void
lexbor_array_obj_delete(lexbor_array_obj_t *array, size_t begin, size_t length)
{
    if (begin >= array->length || length == 0) {
        return;
    }

    size_t end_len = begin + length;

    if (end_len >= array->length) {
        array->length = begin;
        return;
    }

    memmove(&array->list[ begin * array->struct_size ],
            &array->list[ end_len * array->struct_size ],
            sizeof(uint8_t *)
            * ((array->length - end_len) * array->struct_size));

    array->length -= length;
}

/*
 * No inline functions.
 */
void
lexbor_array_obj_erase_noi(lexbor_array_obj_t *array)
{
    lexbor_array_obj_erase(array);
}

void *
lexbor_array_obj_get_noi(lexbor_array_obj_t *array, size_t idx)
{
    return lexbor_array_obj_get(array, idx);
}

size_t
lexbor_array_obj_length_noi(lexbor_array_obj_t *array)
{
    return lexbor_array_obj_length(array);
}

size_t
lexbor_array_obj_size_noi(lexbor_array_obj_t *array)
{
    return lexbor_array_obj_size(array);
}

size_t
lexbor_array_obj_struct_size_noi(lexbor_array_obj_t *array)
{
    return lexbor_array_obj_struct_size(array);
}

void *
lexbor_array_obj_last_noi(lexbor_array_obj_t *array)
{
    return lexbor_array_obj_last(array);
}
