/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_SBST_H
#define LEXBOR_SBST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

#include "lexbor/core/base.h"


typedef struct {
    lxb_char_t key;

    void       *value;
    size_t     value_len;

    size_t     left;
    size_t     right;
    size_t     next;
}
lexbor_sbst_entry_static_t;


/*
 * Inline functions
 */
lxb_inline const lexbor_sbst_entry_static_t *
lexbor_sbst_entry_static_find(const lexbor_sbst_entry_static_t *strt,
                              const lexbor_sbst_entry_static_t *root,
                              const lxb_char_t key)
{
    while (root != strt) {
        if (root->key == key) {
            return root;
        }
        else if (key > root->key) {
            root = &strt[root->right];
        }
        else {
            root = &strt[root->left];
        }
    }

    return NULL;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_SBST_H */
