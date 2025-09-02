/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_BST_MAP_H
#define LEXBOR_BST_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/bst.h"
#include "lexbor/core/str.h"
#include "lexbor/core/mraw.h"
#include "lexbor/core/dobject.h"


typedef struct {
    lexbor_str_t str;
    void         *value;
}
lexbor_bst_map_entry_t;

typedef struct {
    lexbor_bst_t     *bst;
    lexbor_mraw_t    *mraw;
    lexbor_dobject_t *entries;

}
lexbor_bst_map_t;


LXB_API lexbor_bst_map_t *
lexbor_bst_map_create(void);

LXB_API lxb_status_t
lexbor_bst_map_init(lexbor_bst_map_t *bst_map, size_t size);

LXB_API void
lexbor_bst_map_clean(lexbor_bst_map_t *bst_map);

LXB_API lexbor_bst_map_t *
lexbor_bst_map_destroy(lexbor_bst_map_t *bst_map, bool self_destroy);


LXB_API lexbor_bst_map_entry_t *
lexbor_bst_map_search(lexbor_bst_map_t *bst_map, lexbor_bst_entry_t *scope,
                      const lxb_char_t *key, size_t key_len);

LXB_API lexbor_bst_map_entry_t *
lexbor_bst_map_insert(lexbor_bst_map_t *bst_map, lexbor_bst_entry_t **scope,
                      const lxb_char_t *key, size_t key_len, void *value);

LXB_API lexbor_bst_map_entry_t *
lexbor_bst_map_insert_not_exists(lexbor_bst_map_t *bst_map,
                                 lexbor_bst_entry_t **scope,
                                 const lxb_char_t *key, size_t key_len);

LXB_API void *
lexbor_bst_map_remove(lexbor_bst_map_t *bst_map, lexbor_bst_entry_t **scope,
                      const lxb_char_t *key, size_t key_len);


/*
 * Inline functions
 */
lxb_inline lexbor_mraw_t *
lexbor_bst_map_mraw(lexbor_bst_map_t *bst_map)
{
    return bst_map->mraw;
}

/*
 * No inline functions for ABI.
 */
LXB_API lexbor_mraw_t *
lexbor_bst_map_mraw_noi(lexbor_bst_map_t *bst_map);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_BST_MAP_H */

