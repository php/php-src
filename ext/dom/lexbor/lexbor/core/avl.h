/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_AVL_H
#define LEXBOR_AVL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/base.h"
#include "lexbor/core/dobject.h"


typedef struct lexbor_avl lexbor_avl_t;
typedef struct lexbor_avl_node lexbor_avl_node_t;

typedef lxb_status_t
(*lexbor_avl_node_f)(lexbor_avl_t *avl, lexbor_avl_node_t **root,
                     lexbor_avl_node_t *node, void *ctx);

struct lexbor_avl_node {
    size_t            type;
    short             height;
    void              *value;

    lexbor_avl_node_t *left;
    lexbor_avl_node_t *right;
    lexbor_avl_node_t *parent;
};

struct lexbor_avl {
    lexbor_dobject_t  *nodes;
    lexbor_avl_node_t *last_right;
};


LXB_API lexbor_avl_t *
lexbor_avl_create(void);

LXB_API lxb_status_t
lexbor_avl_init(lexbor_avl_t *avl, size_t chunk_len, size_t struct_size);

LXB_API void
lexbor_avl_clean(lexbor_avl_t *avl);

LXB_API lexbor_avl_t *
lexbor_avl_destroy(lexbor_avl_t *avl, bool self_destroy);


LXB_API lexbor_avl_node_t *
lexbor_avl_node_make(lexbor_avl_t *avl, size_t type, void *value);

LXB_API void
lexbor_avl_node_clean(lexbor_avl_node_t *node);

LXB_API lexbor_avl_node_t *
lexbor_avl_node_destroy(lexbor_avl_t *avl, lexbor_avl_node_t *node,
                        bool self_destroy);


LXB_API lexbor_avl_node_t *
lexbor_avl_insert(lexbor_avl_t *avl, lexbor_avl_node_t **scope,
                  size_t type, void *value);

LXB_API lexbor_avl_node_t *
lexbor_avl_search(lexbor_avl_t *avl, lexbor_avl_node_t *scope, size_t type);

LXB_API void *
lexbor_avl_remove(lexbor_avl_t *avl, lexbor_avl_node_t **scope, size_t type);

LXB_API void
lexbor_avl_remove_by_node(lexbor_avl_t *avl, lexbor_avl_node_t **root,
                          lexbor_avl_node_t *node);

LXB_API lxb_status_t
lexbor_avl_foreach(lexbor_avl_t *avl, lexbor_avl_node_t **scope,
                   lexbor_avl_node_f cb, void *ctx);

LXB_API void
lexbor_avl_foreach_recursion(lexbor_avl_t *avl, lexbor_avl_node_t *scope,
                             lexbor_avl_node_f callback, void *ctx);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_AVL_H */
