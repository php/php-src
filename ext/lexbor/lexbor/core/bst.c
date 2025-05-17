/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/core/bst.h"
#include "lexbor/core/conv.h"


lexbor_bst_t *
lexbor_bst_create(void)
{
    return lexbor_calloc(1, sizeof(lexbor_bst_t));
}

lxb_status_t
lexbor_bst_init(lexbor_bst_t *bst, size_t size)
{
    lxb_status_t status;

    if (bst == NULL) {
        return LXB_STATUS_ERROR_OBJECT_IS_NULL;
    }

    if (size == 0) {
        return LXB_STATUS_ERROR_WRONG_ARGS;
    }

    bst->dobject = lexbor_dobject_create();
    status = lexbor_dobject_init(bst->dobject, size,
                                 sizeof(lexbor_bst_entry_t));
    if (status != LXB_STATUS_OK) {
        return status;
    }

    bst->root = 0;
    bst->tree_length = 0;

    return LXB_STATUS_OK;
}

void
lexbor_bst_clean(lexbor_bst_t *bst)
{
    if (bst != NULL) {
        lexbor_dobject_clean(bst->dobject);

        bst->root = 0;
        bst->tree_length = 0;
    }
}

lexbor_bst_t *
lexbor_bst_destroy(lexbor_bst_t *bst, bool self_destroy)
{
    if (bst == NULL) {
        return NULL;
    }

    bst->dobject = lexbor_dobject_destroy(bst->dobject, true);

    if (self_destroy) {
        return lexbor_free(bst);
    }

    return bst;
}

lexbor_bst_entry_t *
lexbor_bst_entry_make(lexbor_bst_t *bst, size_t size)
{
    lexbor_bst_entry_t *new_entry = lexbor_dobject_calloc(bst->dobject);
    if (new_entry == NULL) {
        return NULL;
    }

    new_entry->size = size;

    bst->tree_length++;

    return new_entry;
}

lexbor_bst_entry_t *
lexbor_bst_insert(lexbor_bst_t *bst, lexbor_bst_entry_t **scope,
                  size_t size, void *value)
{
    lexbor_bst_entry_t *new_entry, *entry;

    new_entry = lexbor_dobject_calloc(bst->dobject);
    if (new_entry == NULL) {
        return NULL;
    }

    new_entry->size  = size;
    new_entry->value = value;

    bst->tree_length++;

    if (*scope == NULL) {
        *scope = new_entry;
        return new_entry;
    }

    entry = *scope;

    while (entry != NULL) {
        if (size == entry->size) {
            if (entry->next) {
                new_entry->next = entry->next;
            }

            entry->next = new_entry;
            new_entry->parent = entry->parent;

            return new_entry;
        }
        else if (size > entry->size) {
            if (entry->right == NULL) {
                entry->right = new_entry;
                new_entry->parent = entry;

                return new_entry;
            }

            entry = entry->right;
        }
        else {
            if (entry->left == NULL) {
                entry->left = new_entry;
                new_entry->parent = entry;

                return new_entry;
            }

            entry = entry->left;
        }
    }

    return NULL;
}

lexbor_bst_entry_t *
lexbor_bst_insert_not_exists(lexbor_bst_t *bst, lexbor_bst_entry_t **scope,
                             size_t size)
{
    lexbor_bst_entry_t *entry;

    if (*scope == NULL) {
        *scope = lexbor_bst_entry_make(bst, size);

        return *scope;
    }

    entry = *scope;

    while (entry != NULL) {
        if (size == entry->size) {
            return entry;
        }
        else if (size > entry->size) {
            if (entry->right == NULL) {
                entry->right = lexbor_bst_entry_make(bst, size);
                entry->right->parent = entry;

                return entry->right;
            }

            entry = entry->right;
        }
        else {
            if (entry->left == NULL) {
                entry->left = lexbor_bst_entry_make(bst, size);
                entry->left->parent = entry;

                return entry->left;
            }

            entry = entry->left;
        }
    }

    return NULL;
}

lexbor_bst_entry_t *
lexbor_bst_search(lexbor_bst_t *bst, lexbor_bst_entry_t *scope, size_t size)
{
    while (scope != NULL) {
        if (scope->size == size) {
            return scope;
        }
        else if (size > scope->size) {
            scope = scope->right;
        }
        else {
            scope = scope->left;
        }
    }

    return NULL;
}

lexbor_bst_entry_t *
lexbor_bst_search_close(lexbor_bst_t *bst, lexbor_bst_entry_t *scope,
                        size_t size)
{
    lexbor_bst_entry_t *max = NULL;

    while (scope != NULL) {
        if (scope->size == size) {
            return scope;
        }
        else if (size > scope->size) {
            scope = scope->right;
        }
        else {
            max = scope;
            scope = scope->left;
        }
    }

    return max;
}

void *
lexbor_bst_remove(lexbor_bst_t *bst, lexbor_bst_entry_t **scope, size_t size)
{
    lexbor_bst_entry_t *entry = *scope;

    while (entry != NULL) {
        if (entry->size == size) {
            return lexbor_bst_remove_by_pointer(bst, entry, scope);
        }
        else if (size > entry->size) {
            entry = entry->right;
        }
        else {
            entry = entry->left;
        }
    }

    return NULL;
}

void *
lexbor_bst_remove_close(lexbor_bst_t *bst, lexbor_bst_entry_t **scope,
                        size_t size, size_t *found_size)
{
    lexbor_bst_entry_t *entry = *scope;
    lexbor_bst_entry_t *max = NULL;

    while (entry != NULL) {
        if (entry->size == size) {
            if (found_size) {
                *found_size = entry->size;
            }

            return lexbor_bst_remove_by_pointer(bst, entry, scope);
        }
        else if (size > entry->size) {
            entry = entry->right;
        }
        else {
            max = entry;
            entry = entry->left;
        }
    }

    if (max != NULL) {
        if (found_size != NULL) {
            *found_size = max->size;
        }

        return lexbor_bst_remove_by_pointer(bst, max, scope);
    }

    if (found_size != NULL) {
        *found_size = 0;
    }

    return NULL;
}

void *
lexbor_bst_remove_by_pointer(lexbor_bst_t *bst, lexbor_bst_entry_t *entry,
                             lexbor_bst_entry_t **root)
{
    void *value;
    lexbor_bst_entry_t *next, *right, *left;

    bst->tree_length--;

    if (entry->next != NULL) {
        next = entry->next;
        entry->next = entry->next->next;

        value = next->value;

        lexbor_dobject_free(bst->dobject, next);

        return value;
    }

    value = entry->value;

    if (entry->left == NULL && entry->right == NULL) {
        if (entry->parent != NULL) {
            if (entry->parent->left == entry)  entry->parent->left  = NULL;
            if (entry->parent->right == entry) entry->parent->right = NULL;
        }
        else {
            *root = NULL;
        }

        lexbor_dobject_free(bst->dobject, entry);
    }
    else if (entry->left == NULL) {
        if (entry->parent == NULL) {
            entry->right->parent = NULL;

            *root = entry->right;

            lexbor_dobject_free(bst->dobject, entry);

            entry = *root;
        }
        else {
            right = entry->right;
            right->parent = entry->parent;

            memcpy(entry, right, sizeof(lexbor_bst_entry_t));

            lexbor_dobject_free(bst->dobject, right);
        }

        if (entry->right != NULL) {
            entry->right->parent = entry;
        }

        if (entry->left != NULL) {
            entry->left->parent = entry;
        }
    }
    else if (entry->right == NULL) {
        if (entry->parent == NULL) {
            entry->left->parent = NULL;

            *root = entry->left;

            lexbor_dobject_free(bst->dobject, entry);

            entry = *root;
        }
        else {
            left = entry->left;
            left->parent = entry->parent;

            memcpy(entry, left, sizeof(lexbor_bst_entry_t));

            lexbor_dobject_free(bst->dobject, left);
        }

        if (entry->right != NULL) {
            entry->right->parent = entry;
        }

        if (entry->left != NULL) {
            entry->left->parent = entry;
        }
    }
    else {
        left = entry->right;

        while (left->left != NULL) {
            left = left->left;
        }

        /* Swap */
        entry->size  = left->size;
        entry->next  = left->next;
        entry->value = left->value;

        /* Change parrent */
        if (entry->right == left) {
            entry->right = left->right;

            if (entry->right != NULL) {
                left->right->parent = entry;
            }
        }
        else {
            left->parent->left = left->right;

            if (left->right != NULL) {
                left->right->parent = left->parent;
            }
        }

        lexbor_dobject_free(bst->dobject, left);
    }

    return value;
}

void
lexbor_bst_serialize(lexbor_bst_t *bst, lexbor_callback_f callback, void *ctx)
{
    lexbor_bst_serialize_entry(bst->root, callback, ctx, 0);
}

void
lexbor_bst_serialize_entry(lexbor_bst_entry_t *entry,
                           lexbor_callback_f callback, void *ctx, size_t tabs)
{
    size_t len;
    lxb_char_t buff[1024];

    if (entry == NULL) {
        return;
    }

    /* Left */
    for (size_t i = 0; i < tabs; i++) {
        callback((lxb_char_t *) "\t", 1, ctx);
    }
    callback((lxb_char_t *) "<left ", 6, ctx);

    if (entry->left) {
        len = lexbor_conv_int64_to_data((int64_t) entry->left->size,
                                        buff, sizeof(buff));
        callback(buff, len, ctx);

        callback((lxb_char_t *) ">\n", 2, ctx);
        lexbor_bst_serialize_entry(entry->left, callback, ctx, (tabs + 1));

        for (size_t i = 0; i < tabs; i++) {
            callback((lxb_char_t *) "\t", 1, ctx);
        }
    }
    else {
        callback((lxb_char_t *) "NULL>", 5, ctx);
    }

    callback((lxb_char_t *) "</left>\n", 8, ctx);

    /* Right */
    for (size_t i = 0; i < tabs; i++) {
        callback((lxb_char_t *) "\t", 1, ctx);
    }
    callback((lxb_char_t *) "<right ", 7, ctx);

    if (entry->right) {
        len = lexbor_conv_int64_to_data((int64_t) entry->right->size,
                                        buff, sizeof(buff));
        callback(buff, len, ctx);

        callback((lxb_char_t *) ">\n", 2, ctx);
        lexbor_bst_serialize_entry(entry->right, callback, ctx, (tabs + 1));

        for (size_t i = 0; i < tabs; i++) {
            callback((lxb_char_t *) "\t", 1, ctx);
        }
    }
    else {
        callback((lxb_char_t *) "NULL>", 5, ctx);
    }

    callback((lxb_char_t *) "</right>\n", 9, ctx);
}
