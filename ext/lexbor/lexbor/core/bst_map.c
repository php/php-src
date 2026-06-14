/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/core/bst_map.h"
#include "lexbor/core/utils.h"


lexbor_bst_map_t *
lexbor_bst_map_create(void)
{
    return lexbor_calloc(1, sizeof(lexbor_bst_map_t));
}

lxb_status_t
lexbor_bst_map_init(lexbor_bst_map_t *bst_map, size_t size)
{
    lxb_status_t status;

    if (bst_map == NULL) {
        return LXB_STATUS_ERROR_OBJECT_IS_NULL;
    }

    if (size == 0) {
        return LXB_STATUS_ERROR_WRONG_ARGS;
    }

    /* bst */
    bst_map->bst = lexbor_bst_create();
    status = lexbor_bst_init(bst_map->bst, size);
    if (status) {
        return status;
    }

    /* dobject */
    bst_map->entries = lexbor_dobject_create();
    status = lexbor_dobject_init(bst_map->entries, size,
                                 sizeof(lexbor_bst_map_entry_t));
    if (status) {
        return status;
    }

    /* mraw */
    bst_map->mraw = lexbor_mraw_create();
    status = lexbor_mraw_init(bst_map->mraw, (size * 6));
    if (status) {
        return status;
    }

    return LXB_STATUS_OK;
}

void
lexbor_bst_map_clean(lexbor_bst_map_t *bst_map)
{
    lexbor_bst_clean(bst_map->bst);
    lexbor_mraw_clean(bst_map->mraw);
    lexbor_dobject_clean(bst_map->entries);
}

lexbor_bst_map_t *
lexbor_bst_map_destroy(lexbor_bst_map_t *bst_map, bool self_destroy)
{
    if (bst_map == NULL) {
        return NULL;
    }

    bst_map->bst = lexbor_bst_destroy(bst_map->bst, true);
    bst_map->mraw = lexbor_mraw_destroy(bst_map->mraw, true);
    bst_map->entries = lexbor_dobject_destroy(bst_map->entries, true);

    if (self_destroy) {
        return lexbor_free(bst_map);
    }

    return bst_map;
}

lexbor_bst_map_entry_t *
lexbor_bst_map_search(lexbor_bst_map_t *bst_map, lexbor_bst_entry_t *scope,
                      const lxb_char_t *key, size_t key_len)
{
    lexbor_bst_map_entry_t *entry;
    lexbor_bst_entry_t *bst_entry;

    size_t hash_id = lexbor_utils_hash_hash(key, key_len);

    bst_entry = lexbor_bst_search(bst_map->bst, scope, hash_id);
    if (bst_entry == NULL) {
        return NULL;
    }

    do {
        entry = bst_entry->value;

        if (entry->str.length == key_len &&
            lexbor_str_data_cmp(entry->str.data, key))
        {
            return entry;
        }

        bst_entry = bst_entry->next;
    }
    while (bst_entry != NULL);

    return NULL;
}

lexbor_bst_map_entry_t *
lexbor_bst_map_insert(lexbor_bst_map_t *bst_map,
                      lexbor_bst_entry_t **scope,
                      const lxb_char_t *key, size_t key_len, void *value)
{
    lexbor_bst_map_entry_t *entry;

    entry = lexbor_bst_map_insert_not_exists(bst_map, scope, key, key_len);
    if (entry == NULL) {
        return NULL;
    }

    entry->value = value;

    return entry;
}

lexbor_bst_map_entry_t *
lexbor_bst_map_insert_not_exists(lexbor_bst_map_t *bst_map,
                                 lexbor_bst_entry_t **scope,
                                 const lxb_char_t *key, size_t key_len)
{
    lexbor_bst_map_entry_t *entry;
    lexbor_bst_entry_t *bst_entry;

    size_t hash_id = lexbor_utils_hash_hash(key, key_len);

    bst_entry = lexbor_bst_insert_not_exists(bst_map->bst, scope, hash_id);
    if (bst_entry == NULL) {
        return NULL;
    }

    if (bst_entry->value == NULL) {
        goto new_entry;
    }

    do {
        entry = bst_entry->value;

        if (entry->str.length == key_len &&
            lexbor_str_data_cmp(entry->str.data, key))
        {
            return entry;
        }

        if (bst_entry->next == NULL) {
            bst_entry->next = lexbor_bst_entry_make(bst_map->bst, hash_id);
            bst_entry = bst_entry->next;

            if (bst_entry == NULL) {
                return NULL;
            }

            goto new_entry;
        }

        bst_entry = bst_entry->next;
    }
    while (1);

    return NULL;

new_entry:

    entry = lexbor_dobject_calloc(bst_map->entries);
    if (entry == NULL) {
        return NULL;
    }

    lexbor_str_init(&entry->str, bst_map->mraw, key_len);
    if (entry->str.data == NULL) {
        lexbor_dobject_free(bst_map->entries, entry);

        return NULL;
    }

    lexbor_str_append(&entry->str, bst_map->mraw, key, key_len);

    bst_entry->value = entry;

    return entry;
}

void *
lexbor_bst_map_remove(lexbor_bst_map_t *bst_map, lexbor_bst_entry_t **scope,
                      const lxb_char_t *key, size_t key_len)
{
    lexbor_bst_map_entry_t *entry;
    lexbor_bst_entry_t *bst_entry;

    size_t hash_id = lexbor_utils_hash_hash(key, key_len);

    bst_entry = lexbor_bst_search(bst_map->bst, *scope, hash_id);
    if (bst_entry == NULL) {
        return NULL;
    }

    do {
        entry = bst_entry->value;

        if (entry->str.length == key_len &&
            lexbor_str_data_cmp(entry->str.data, key))
        {
            void *value = entry->value;

            lexbor_bst_remove_by_pointer(bst_map->bst, bst_entry, scope);

            lexbor_str_destroy(&entry->str, bst_map->mraw, false);
            lexbor_dobject_free(bst_map->entries, entry);

            return value;
        }

        bst_entry = bst_entry->next;
    }
    while (bst_entry != NULL);

    return NULL;
}

/*
 * No inline functions.
 */
lexbor_mraw_t *
lexbor_bst_map_mraw_noi(lexbor_bst_map_t *bst_map)
{
    return lexbor_bst_map_mraw(bst_map);
}
