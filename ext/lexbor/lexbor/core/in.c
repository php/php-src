/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/core/in.h"
#include "lexbor/core/str_res.h"


lexbor_in_t *
lexbor_in_create(void)
{
    return lexbor_calloc(1, sizeof(lexbor_in_t));
}

lxb_status_t
lexbor_in_init(lexbor_in_t *incoming, size_t chunk_size)
{
    if (incoming == NULL) {
        return LXB_STATUS_ERROR_OBJECT_IS_NULL;
    }

    if (chunk_size == 0) {
        return LXB_STATUS_ERROR_WRONG_ARGS;
    }

    incoming->nodes = lexbor_dobject_create();

    return lexbor_dobject_init(incoming->nodes, chunk_size,
                               sizeof(lexbor_in_node_t));
}

void
lexbor_in_clean(lexbor_in_t *incoming)
{
    lexbor_dobject_clean(incoming->nodes);
}

lexbor_in_t *
lexbor_in_destroy(lexbor_in_t *incoming, bool self_destroy)
{
    if (incoming == NULL) {
        return NULL;
    }

    incoming->nodes = lexbor_dobject_destroy(incoming->nodes, true);

    if (self_destroy == true) {
        return lexbor_free(incoming);
    }

    return incoming;
}

lexbor_in_node_t *
lexbor_in_node_make(lexbor_in_t *incoming, lexbor_in_node_t *last_node,
                    const lxb_char_t *buf, size_t buf_size)
{
    lexbor_in_node_t *node = lexbor_dobject_alloc(incoming->nodes);

    if (node == NULL) {
        return NULL;
    }

    node->opt = LEXBOR_IN_OPT_UNDEF;
    node->begin = buf;
    node->end = buf + buf_size;
    node->use = buf;

    if (last_node != NULL) {
        last_node->next = node;
        node->offset = last_node->offset + (last_node->end - last_node->begin);
    }
    else {
        node->offset = 0;
    }

    node->prev = last_node;
    node->next = NULL;
    node->incoming = incoming;

    return node;
}

void
lexbor_in_node_clean(lexbor_in_node_t *node)
{
    lexbor_in_t *incoming = node->incoming;

    memset(node, 0, sizeof(lexbor_in_node_t));

    node->incoming = incoming;
}

lexbor_in_node_t *
lexbor_in_node_destroy(lexbor_in_t *incoming,
                       lexbor_in_node_t *node, bool self_destroy)
{
    if (node == NULL) {
        return NULL;
    }

    if (self_destroy) {
        return lexbor_dobject_free(incoming->nodes, node);
    }

    return node;
}

lexbor_in_node_t *
lexbor_in_node_split(lexbor_in_node_t *node, const lxb_char_t *pos)
{
    lexbor_in_node_t *new_node;

    new_node = lexbor_dobject_alloc(node->incoming->nodes);

    if (new_node == NULL) {
        return NULL;
    }

    new_node->offset   = node->offset + (pos - node->begin);
    new_node->opt      = LEXBOR_IN_OPT_UNDEF;
    new_node->begin    = pos;
    new_node->end      = node->end;
    new_node->next     = NULL;
    new_node->prev     = node;
    new_node->incoming = node->incoming;

    node->end  = pos;
    node->next = new_node;

    if (node->use > pos) {
        new_node->use = node->use;
        node->use = pos;
    }
    else {
        new_node->use = pos;
    }

    return new_node;
}

lexbor_in_node_t *
lexbor_in_node_find(lexbor_in_node_t *node, const lxb_char_t *pos)
{
    while (node->next) {
        node = node->next;
    }

    while (node && (node->begin > pos || node->end < pos)) {
        node = node->prev;
    }

    return node;
}

const lxb_char_t *
lexbor_in_node_pos_up(lexbor_in_node_t *node, lexbor_in_node_t **return_node,
                      const lxb_char_t *pos, size_t offset)
{
    do {
        pos = pos + offset;

        if (node->end >= pos) {
            if (return_node != NULL && *return_node != node) {
                *return_node = node;
            }

            return pos;
        }

        if (node->next == NULL) {
            if (return_node != NULL && *return_node != node) {
                *return_node = node;
            }

            return node->end;
        }

        offset = pos - node->end;
        node = node->next;
        pos = node->begin;

    }
    while (1);

    return NULL;
}

const lxb_char_t *
lexbor_in_node_pos_down(lexbor_in_node_t *node, lexbor_in_node_t **return_node,
                        const lxb_char_t *pos, size_t offset)
{
    do {
        pos = pos - offset;

        if (node->begin <= pos) {
            if (return_node != NULL && *return_node != node) {
                *return_node = node;
            }

            return pos;
        }

        if (node->prev == NULL) {
            if (return_node != NULL && *return_node != node) {
                *return_node = node;
            }

            return node->begin;
        }

        offset = node->begin - pos;
        node = node->prev;
        pos = node->end;

    }
    while (1);

    return NULL;
}

/*
* No inline functions for ABI.
*/
const lxb_char_t *
lexbor_in_node_begin_noi(const lexbor_in_node_t *node)
{
    return lexbor_in_node_begin(node);
}

const lxb_char_t *
lexbor_in_node_end_noi(const lexbor_in_node_t *node)
{
    return lexbor_in_node_end(node);
}

size_t
lexbor_in_node_offset_noi(const lexbor_in_node_t *node)
{
    return lexbor_in_node_offset(node);
}

lexbor_in_node_t *
lexbor_in_node_next_noi(const lexbor_in_node_t *node)
{
    return lexbor_in_node_next(node);
}

lexbor_in_node_t *
lexbor_in_node_prev_noi(const lexbor_in_node_t *node)
{
    return lexbor_in_node_prev(node);
}

lexbor_in_t *
lexbor_in_node_in_noi(const lexbor_in_node_t *node)
{
    return lexbor_in_node_in(node);
}

bool
lexbor_in_segment_noi(const lexbor_in_node_t *node, const lxb_char_t *data)
{
    return lexbor_in_segment(node, data);
}
