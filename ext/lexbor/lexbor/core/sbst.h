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

#ifdef __has_attribute
# if __has_attribute(nonstring) && defined(__GNUC__) && !defined(__clang__) && __GNUC__ >= 15
#  define LXB_NONSTRING __attribute__((nonstring))
# else
#  define LXB_NONSTRING
# endif
#else
# define LXB_NONSTRING
#endif

typedef struct {
    lxb_char_t key;

    lxb_char_t         value[6] LXB_NONSTRING;
    unsigned char      value_len;

    unsigned short     left;
    unsigned short     right;
    unsigned short     next;
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
