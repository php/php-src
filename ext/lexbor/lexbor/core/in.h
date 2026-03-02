/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_IN_H
#define LEXBOR_IN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/base.h"
#include "lexbor/core/dobject.h"


typedef struct lexbor_in_node lexbor_in_node_t;
typedef int lexbor_in_opt_t;

enum lexbor_in_opt {
    LEXBOR_IN_OPT_UNDEF    = 0x00,
    LEXBOR_IN_OPT_READONLY = 0x01,
    LEXBOR_IN_OPT_DONE     = 0x02,
    LEXBOR_IN_OPT_FAKE     = 0x04,
    LEXBOR_IN_OPT_ALLOC    = 0x08
};

typedef struct {
    lexbor_dobject_t *nodes;
}
lexbor_in_t;

struct lexbor_in_node {
    size_t            offset;
    lexbor_in_opt_t   opt;

    const lxb_char_t  *begin;
    const lxb_char_t  *end;
    const lxb_char_t  *use;

    lexbor_in_node_t  *next;
    lexbor_in_node_t  *prev;

    lexbor_in_t       *incoming;
};


LXB_API lexbor_in_t *
lexbor_in_create(void);

LXB_API lxb_status_t
lexbor_in_init(lexbor_in_t *incoming, size_t chunk_size);

LXB_API void
lexbor_in_clean(lexbor_in_t *incoming);

LXB_API lexbor_in_t *
lexbor_in_destroy(lexbor_in_t *incoming, bool self_destroy);


LXB_API lexbor_in_node_t *
lexbor_in_node_make(lexbor_in_t *incoming, lexbor_in_node_t *last_node,
                    const lxb_char_t *buf, size_t buf_size);

LXB_API void
lexbor_in_node_clean(lexbor_in_node_t *node);

LXB_API lexbor_in_node_t *
lexbor_in_node_destroy(lexbor_in_t *incoming,
                       lexbor_in_node_t *node, bool self_destroy);


LXB_API lexbor_in_node_t *
lexbor_in_node_split(lexbor_in_node_t *node, const lxb_char_t *pos);

LXB_API lexbor_in_node_t *
lexbor_in_node_find(lexbor_in_node_t *node, const lxb_char_t *pos);

/**
 * Get position by `offset`.
 * If position outside of nodes return `begin` position of first node
 * in nodes chain.
 */
LXB_API const lxb_char_t *
lexbor_in_node_pos_up(lexbor_in_node_t *node, lexbor_in_node_t **return_node,
                      const lxb_char_t *pos, size_t offset);

/**
 * Get position by `offset`.
 * If position outside of nodes return `end`
 * position of last node in nodes chain.
 */
LXB_API const lxb_char_t *
lexbor_in_node_pos_down(lexbor_in_node_t *node, lexbor_in_node_t **return_node,
                        const lxb_char_t *pos, size_t offset);

/*
 * Inline functions
 */
lxb_inline const lxb_char_t *
lexbor_in_node_begin(const lexbor_in_node_t *node)
{
    return node->begin;
}

lxb_inline const lxb_char_t *
lexbor_in_node_end(const lexbor_in_node_t *node)
{
    return node->end;
}

lxb_inline size_t
lexbor_in_node_offset(const lexbor_in_node_t *node)
{
    return node->offset;
}

lxb_inline lexbor_in_node_t *
lexbor_in_node_next(const lexbor_in_node_t *node)
{
    return node->next;
}

lxb_inline lexbor_in_node_t *
lexbor_in_node_prev(const lexbor_in_node_t *node)
{
    return node->prev;
}

lxb_inline lexbor_in_t *
lexbor_in_node_in(const lexbor_in_node_t *node)
{
    return node->incoming;
}

lxb_inline bool
lexbor_in_segment(const lexbor_in_node_t *node, const lxb_char_t *data)
{
    return node->begin <= data && node->end >= data;
}

/*
 * No inline functions for ABI.
 */
LXB_API const lxb_char_t *
lexbor_in_node_begin_noi(const lexbor_in_node_t *node);

LXB_API const lxb_char_t *
lexbor_in_node_end_noi(const lexbor_in_node_t *node);

LXB_API size_t
lexbor_in_node_offset_noi(const lexbor_in_node_t *node);

LXB_API lexbor_in_node_t *
lexbor_in_node_next_noi(const lexbor_in_node_t *node);

LXB_API lexbor_in_node_t *
lexbor_in_node_prev_noi(const lexbor_in_node_t *node);

LXB_API lexbor_in_t *
lexbor_in_node_in_noi(const lexbor_in_node_t *node);

LXB_API bool
lexbor_in_segment_noi(const lexbor_in_node_t *node, const lxb_char_t *data);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_IN_H */
