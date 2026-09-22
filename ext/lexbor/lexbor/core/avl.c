/*
 * Copyright (C) 2018-2022 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/core/avl.h"


lxb_inline short
lexbor_avl_node_height(lexbor_avl_node_t *node);

lxb_inline short
lexbor_avl_node_balance_factor(lexbor_avl_node_t *node);

lxb_inline void
lexbor_avl_node_set_height(lexbor_avl_node_t *node);

static lexbor_avl_node_t *
lexbor_avl_node_rotate_right(lexbor_avl_node_t *pos);

static lexbor_avl_node_t *
lexbor_avl_node_rotate_left(lexbor_avl_node_t *pos);

static lexbor_avl_node_t *
lexbor_avl_node_balance(lexbor_avl_node_t *node,
                             lexbor_avl_node_t **scope);

lxb_inline lexbor_avl_node_t *
lexbor_avl_find_min(lexbor_avl_node_t *node);

lxb_inline void
lexbor_avl_rotate_for_delete(lexbor_avl_node_t *delete_node,
                             lexbor_avl_node_t *node,
                             lexbor_avl_node_t **root);


lexbor_avl_t *
lexbor_avl_create(void)
{
    return lexbor_calloc(1, sizeof(lexbor_avl_t));
}

lxb_status_t
lexbor_avl_init(lexbor_avl_t *avl, size_t chunk_len, size_t struct_size)
{
    if (avl == NULL) {
        return LXB_STATUS_ERROR_OBJECT_IS_NULL;
    }

    if (chunk_len == 0
        || (struct_size != 0 && struct_size < sizeof(lexbor_avl_node_t)))
    {
        return LXB_STATUS_ERROR_WRONG_ARGS;
    }

    if (struct_size == 0) {
        struct_size = sizeof(lexbor_avl_node_t);
    }

    avl->last_right = NULL;

    avl->nodes = lexbor_dobject_create();
    return lexbor_dobject_init(avl->nodes, chunk_len, struct_size);
}

void
lexbor_avl_clean(lexbor_avl_t *avl)
{
    avl->last_right = NULL;

    lexbor_dobject_clean(avl->nodes);
}

lexbor_avl_t *
lexbor_avl_destroy(lexbor_avl_t *avl, bool self_destroy)
{
    if (avl == NULL)
        return NULL;

    avl->nodes = lexbor_dobject_destroy(avl->nodes, true);

    if (self_destroy) {
        return lexbor_free(avl);
    }

    return avl;
}

lexbor_avl_node_t *
lexbor_avl_node_make(lexbor_avl_t *avl, size_t type, void *value)
{
    lexbor_avl_node_t *node = lexbor_dobject_calloc(avl->nodes);
    if (node == NULL) {
        return NULL;
    }

    node->type = type;
    node->value = value;

    return node;
}

void
lexbor_avl_node_clean(lexbor_avl_node_t *node)
{
    memset(node, 0, sizeof(lexbor_avl_node_t));
}

lexbor_avl_node_t *
lexbor_avl_node_destroy(lexbor_avl_t *avl,
                        lexbor_avl_node_t *node, bool self_destroy)
{
    if (node == NULL) {
        return NULL;
    }

    if (self_destroy) {
        return lexbor_dobject_free(avl->nodes, node);
    }

    return node;
}

lxb_inline short
lexbor_avl_node_height(lexbor_avl_node_t *node)
{
    return (node) ? node->height : 0;
}

lxb_inline short
lexbor_avl_node_balance_factor(lexbor_avl_node_t *node)
{
    return (lexbor_avl_node_height(node->right)
            - lexbor_avl_node_height(node->left));
}

lxb_inline void
lexbor_avl_node_set_height(lexbor_avl_node_t *node)
{
    short left_height = lexbor_avl_node_height(node->left);
    short right_height = lexbor_avl_node_height(node->right);

    node->height = ((left_height > right_height)
                    ? left_height : right_height) + 1;
}

static lexbor_avl_node_t *
lexbor_avl_node_rotate_right(lexbor_avl_node_t *pos)
{
    lexbor_avl_node_t *node = pos->left;

    node->parent = pos->parent;

    if (node->right) {
        node->right->parent = pos;
    }

    pos->left   = node->right;
    pos->parent = node;

    node->right = pos;

    lexbor_avl_node_set_height(pos);
    lexbor_avl_node_set_height(node);

    return node;
}

static lexbor_avl_node_t *
lexbor_avl_node_rotate_left(lexbor_avl_node_t *pos)
{
    lexbor_avl_node_t *node = pos->right;

    node->parent = pos->parent;

    if (node->left) {
        node->left->parent = pos;
    }

    pos->right  = node->left;
    pos->parent = node;

    node->left = pos;

    lexbor_avl_node_set_height(pos);
    lexbor_avl_node_set_height(node);

    return node;
}

static lexbor_avl_node_t *
lexbor_avl_node_balance(lexbor_avl_node_t *node, lexbor_avl_node_t **scope)
{
    /* Set height */
    lexbor_avl_node_t *parent;

    short left_height = lexbor_avl_node_height(node->left);
    short right_height = lexbor_avl_node_height(node->right);

    node->height = ((left_height > right_height)
                    ? left_height : right_height) + 1;

    /* Check balance */
    switch ((right_height - left_height)) {
        case 2: {
            if (lexbor_avl_node_balance_factor(node->right) < 0) {
                node->right = lexbor_avl_node_rotate_right(node->right);
            }

            parent = node->parent;

            if (parent != NULL) {
                if (parent->right == node) {
                    parent->right = lexbor_avl_node_rotate_left(node);
                    return parent->right;
                }
                else {
                    parent->left = lexbor_avl_node_rotate_left(node);
                    return parent->left;
                }
            }

            return lexbor_avl_node_rotate_left(node);
        }
        case -2: {
            if (lexbor_avl_node_balance_factor(node->left) > 0) {
                node->left = lexbor_avl_node_rotate_left(node->left);
            }

            parent = node->parent;

            if (parent != NULL) {
                if (parent->right == node) {
                    parent->right = lexbor_avl_node_rotate_right(node);
                    return parent->right;
                }
                else {
                    parent->left = lexbor_avl_node_rotate_right(node);
                    return parent->left;
                }
            }

            return lexbor_avl_node_rotate_right(node);
        }
        default:
            break;
    }

    if (node->parent == NULL) {
        *scope = node;
    }

    return node->parent;
}

lexbor_avl_node_t *
lexbor_avl_insert(lexbor_avl_t *avl, lexbor_avl_node_t **scope,
                  size_t type, void *value)
{
    lexbor_avl_node_t *node, *new_node;

    if (*scope == NULL) {
        *scope = lexbor_avl_node_make(avl, type, value);
        return *scope;
    }

    node = *scope;
    new_node = lexbor_dobject_calloc(avl->nodes);

    for (;;) {
        if (type == node->type) {
            node->value = value;
            return node;
        }
        else if (type < node->type) {
            if (node->left == NULL) {
                node->left = new_node;

                new_node->parent = node;
                new_node->type   = type;
                new_node->value  = value;

                node = new_node;
                break;
            }

            node = node->left;
        }
        else {
            if (node->right == NULL) {
                node->right = new_node;

                new_node->parent = node;
                new_node->type   = type;
                new_node->value  = value;

                node = new_node;
                break;
            }

            node = node->right;
        }
    }

    while (node != NULL) {
        node = lexbor_avl_node_balance(node, scope);
    }

    return new_node;
}

lxb_inline lexbor_avl_node_t *
lexbor_avl_find_min(lexbor_avl_node_t *node)
{
    if (node == NULL) {
        return NULL;
    }

    while (node->right != NULL) {
        node = node->right;
    }

    return node;
}

lxb_inline void
lexbor_avl_rotate_for_delete(lexbor_avl_node_t *delete_node,
                             lexbor_avl_node_t *node, lexbor_avl_node_t **scope)
{
    lexbor_avl_node_t *balance_node;

    if (node) {
        if (delete_node->left == node) {
            balance_node = (node->left) ? node->left : node;

            node->parent = delete_node->parent;
            node->right  = delete_node->right;

            if (delete_node->right)
                delete_node->right->parent = node;
        }
        else {
            balance_node = node;

            node->parent->right = NULL;

            node->parent = delete_node->parent;
            node->right  = delete_node->right;
            node->left   = delete_node->left;

            if (delete_node->left != NULL) {
                delete_node->left->parent = node;
            }

            if (delete_node->right != NULL) {
                delete_node->right->parent = node;
            }
        }

        if (delete_node->parent != NULL) {
            if (delete_node->parent->left == delete_node) {
                delete_node->parent->left = node;
            }
            else {
                delete_node->parent->right = node;
            }
        }
        else {
            *scope = node;
        }
    }
    else {
        balance_node = delete_node->parent;

        if (balance_node != NULL) {
            if (balance_node->left == delete_node) {
                balance_node->left = delete_node->right;
            }
            else {
                balance_node->right = delete_node->right;
            }
        }
        else {
            *scope = delete_node->right;
        }

        if (delete_node->right != NULL) {
            delete_node->right->parent = balance_node;
        }
    }

    while (balance_node != NULL) {
        balance_node = lexbor_avl_node_balance(balance_node, scope);
    }
}

void *
lexbor_avl_remove(lexbor_avl_t *avl, lexbor_avl_node_t **scope, size_t type)
{
    void *value;
    lexbor_avl_node_t *node = *scope;

    while (node != NULL) {
        if (type == node->type) {
            avl->last_right = lexbor_avl_find_min(node->left);
            lexbor_avl_rotate_for_delete(node, avl->last_right, scope);

            value = node->value;
            lexbor_dobject_free(avl->nodes, node);

            return value;
        }
        else if (type < node->type) {
            node = node->left;
        }
        else {
            node = node->right;
        }
    }

    return NULL;
}

void
lexbor_avl_remove_by_node(lexbor_avl_t *avl, lexbor_avl_node_t **root,
                          lexbor_avl_node_t *node)
{
    avl->last_right = lexbor_avl_find_min(node->left);

    lexbor_avl_rotate_for_delete(node, avl->last_right, root);

    (void) lexbor_dobject_free(avl->nodes, node);
}

lexbor_avl_node_t *
lexbor_avl_search(lexbor_avl_t *avl, lexbor_avl_node_t *node, size_t type)
{
    while (node != NULL) {
        if (type == node->type) {
            return node;
        }
        else if (type < node->type) {
            node = node->left;
        }
        else {
            node = node->right;
        }
    }

    return NULL;
}

lxb_status_t
lexbor_avl_foreach(lexbor_avl_t *avl, lexbor_avl_node_t **scope,
                   lexbor_avl_node_f cb, void *ctx)
{
    lxb_status_t status;
    int state = 0;
    bool from_right = false;
    lexbor_avl_node_t *node, *parent, *root;

    if (scope == NULL || *scope == NULL) {
        return LXB_STATUS_ERROR_WRONG_ARGS;
    }

    node = *scope;
    root = node;

    while (node->left != NULL) {
        node = node->left;
    }

    do {
        parent = node->parent;

        if (!from_right) {
            if (node == root) {
                state = 2;
            }
            else {
                state = parent->left == node;
            }

            status = cb(avl, scope, node, ctx);
            if (status != LXB_STATUS_OK) {
                return status;
            }

            if (state == 2) {
                if (*scope != root) {
                    root = *scope;

                    if (root == NULL) {
                        return LXB_STATUS_OK;
                    }
                    else if (avl->last_right == root) {
                        node = root;
                    }
                    else {
                        node = root;
                        continue;
                    }
                }
            }
            else if (parent->left != node && parent->right != node) {
                if (state) {
                    if (parent->left != NULL && parent->left->right != NULL) {
                        node = parent->left;
                    }
                    else {
                        node = parent;
                        continue;
                    }
                }
                else {
                    if (parent->right != NULL) {
                        node = parent->right;

                        if (node != avl->last_right) {
                            continue;
                        }
                    }
                    else {
                        node = parent;
                    }
                }
            }
        }

        if (node->right != NULL && !from_right) {
            node = node->right;

            while (node->left != NULL) {
                node = node->left;
            }

            continue;
        }

        if (parent == root->parent) {
            return LXB_STATUS_OK;
        }
        else if (node == parent->left) {
            from_right = false;
        }
        else {
            from_right = true;
        }

        node = parent;
    }
    while (true);
}

void
lexbor_avl_foreach_recursion(lexbor_avl_t *avl, lexbor_avl_node_t *scope,
                             lexbor_avl_node_f callback, void *ctx)
{
    if (scope == NULL) {
        return;
    }

    callback(avl, NULL, scope, ctx);

    lexbor_avl_foreach_recursion(avl, scope->left, callback, ctx);
    lexbor_avl_foreach_recursion(avl, scope->right, callback, ctx);
}
