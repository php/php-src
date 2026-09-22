/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_BST_H
#define LEXBOR_BST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

#include "lexbor/core/base.h"
#include "lexbor/core/dobject.h"


#define lexbor_bst_root(bst) (bst)->root
#define lexbor_bst_root_ref(bst) &((bst)->root)


typedef struct lexbor_bst_entry lexbor_bst_entry_t;
typedef struct lexbor_bst lexbor_bst_t;

typedef bool (*lexbor_bst_entry_f)(lexbor_bst_t *bst,
                                   lexbor_bst_entry_t *entry, void *ctx);

struct lexbor_bst_entry {
    void               *value;

    lexbor_bst_entry_t *right;
    lexbor_bst_entry_t *left;
    lexbor_bst_entry_t *next;
    lexbor_bst_entry_t *parent;

    size_t             size;
};

struct lexbor_bst {
    lexbor_dobject_t   *dobject;
    lexbor_bst_entry_t *root;

    size_t             tree_length;
};


LXB_API lexbor_bst_t *
lexbor_bst_create(void);

LXB_API lxb_status_t
lexbor_bst_init(lexbor_bst_t *bst, size_t size);

LXB_API void
lexbor_bst_clean(lexbor_bst_t *bst);

LXB_API lexbor_bst_t *
lexbor_bst_destroy(lexbor_bst_t *bst, bool self_destroy);

LXB_API lexbor_bst_entry_t *
lexbor_bst_entry_make(lexbor_bst_t *bst, size_t size);

LXB_API lexbor_bst_entry_t *
lexbor_bst_insert(lexbor_bst_t *bst, lexbor_bst_entry_t **scope,
                  size_t size, void *value);

LXB_API lexbor_bst_entry_t *
lexbor_bst_insert_not_exists(lexbor_bst_t *bst, lexbor_bst_entry_t **scope,
                             size_t size);


LXB_API lexbor_bst_entry_t *
lexbor_bst_search(lexbor_bst_t *bst, lexbor_bst_entry_t *scope, size_t size);

LXB_API lexbor_bst_entry_t *
lexbor_bst_search_close(lexbor_bst_t *bst, lexbor_bst_entry_t *scope,
                        size_t size);


LXB_API void *
lexbor_bst_remove(lexbor_bst_t *bst, lexbor_bst_entry_t **root, size_t size);

LXB_API void *
lexbor_bst_remove_close(lexbor_bst_t *bst, lexbor_bst_entry_t **root,
                        size_t size, size_t *found_size);

LXB_API void *
lexbor_bst_remove_by_pointer(lexbor_bst_t *bst, lexbor_bst_entry_t *entry,
                             lexbor_bst_entry_t **root);


LXB_API void
lexbor_bst_serialize(lexbor_bst_t *bst, lexbor_callback_f callback, void *ctx);

LXB_API void
lexbor_bst_serialize_entry(lexbor_bst_entry_t *entry,
                           lexbor_callback_f callback, void *ctx, size_t tabs);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_BST_H */



