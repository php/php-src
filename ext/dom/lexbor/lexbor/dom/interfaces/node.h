/*
 * Copyright (C) 2018-2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_DOM_NODE_H
#define LEXBOR_DOM_NODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/dom/interface.h"
#include "lexbor/dom/collection.h"
#include "lexbor/dom/interfaces/event_target.h"


typedef lexbor_action_t
(*lxb_dom_node_simple_walker_f)(lxb_dom_node_t *node, void *ctx);


typedef enum {
    LXB_DOM_NODE_TYPE_UNDEF                  = 0x00,
    LXB_DOM_NODE_TYPE_ELEMENT                = 0x01,
    LXB_DOM_NODE_TYPE_ATTRIBUTE              = 0x02,
    LXB_DOM_NODE_TYPE_TEXT                   = 0x03,
    LXB_DOM_NODE_TYPE_CDATA_SECTION          = 0x04,
    LXB_DOM_NODE_TYPE_ENTITY_REFERENCE       = 0x05, // historical
    LXB_DOM_NODE_TYPE_ENTITY                 = 0x06, // historical
    LXB_DOM_NODE_TYPE_PROCESSING_INSTRUCTION = 0x07,
    LXB_DOM_NODE_TYPE_COMMENT                = 0x08,
    LXB_DOM_NODE_TYPE_DOCUMENT               = 0x09,
    LXB_DOM_NODE_TYPE_DOCUMENT_TYPE          = 0x0A,
    LXB_DOM_NODE_TYPE_DOCUMENT_FRAGMENT      = 0x0B,
    LXB_DOM_NODE_TYPE_NOTATION               = 0x0C, // historical
    LXB_DOM_NODE_TYPE_LAST_ENTRY             = 0x0D
}
lxb_dom_node_type_t;

struct lxb_dom_node {
    lxb_dom_event_target_t event_target;

    /* For example: <LalAla:DiV Fix:Me="value"> */

    uintptr_t              local_name; /* , lowercase, without prefix: div */
    uintptr_t              prefix;     /* lowercase: lalala */
    uintptr_t              ns;         /* namespace */

    lxb_dom_document_t     *owner_document;

    lxb_dom_node_t         *next;
    lxb_dom_node_t         *prev;
    lxb_dom_node_t         *parent;
    lxb_dom_node_t         *first_child;
    lxb_dom_node_t         *last_child;
    void                   *user;

    lxb_dom_node_type_t    type;

    size_t                 line;

#ifdef LXB_DOM_NODE_USER_VARIABLES
    LXB_DOM_NODE_USER_VARIABLES
#endif /* LXB_DOM_NODE_USER_VARIABLES */
};


LXB_API lxb_dom_node_t *
lxb_dom_node_interface_create(lxb_dom_document_t *document);

LXB_API lxb_dom_node_t *
lxb_dom_node_interface_clone(lxb_dom_document_t *document,
                             const lxb_dom_node_t *node, bool is_attr);

LXB_API lxb_dom_node_t *
lxb_dom_node_interface_destroy(lxb_dom_node_t *node);

LXB_API lxb_status_t
lxb_dom_node_interface_copy(lxb_dom_node_t *dst,
                            const lxb_dom_node_t *src, bool is_attr);

LXB_API lxb_dom_node_t *
lxb_dom_node_destroy(lxb_dom_node_t *node);

LXB_API lxb_dom_node_t *
lxb_dom_node_destroy_deep(lxb_dom_node_t *root);

LXB_API lxb_dom_node_t *
lxb_dom_node_clone(lxb_dom_node_t *node, bool deep);

LXB_API const lxb_char_t *
lxb_dom_node_name(lxb_dom_node_t *node, size_t *len);

LXB_API void
lxb_dom_node_insert_child_wo_events(lxb_dom_node_t *to, lxb_dom_node_t *node);

LXB_API void
lxb_dom_node_insert_child(lxb_dom_node_t *to, lxb_dom_node_t *node);

LXB_API void
lxb_dom_node_insert_before_wo_events(lxb_dom_node_t *to, lxb_dom_node_t *node);

LXB_API void
lxb_dom_node_insert_before(lxb_dom_node_t *to, lxb_dom_node_t *node);

LXB_API void
lxb_dom_node_insert_after_wo_events(lxb_dom_node_t *to, lxb_dom_node_t *node);

LXB_API void
lxb_dom_node_insert_after(lxb_dom_node_t *to, lxb_dom_node_t *node);

LXB_API void
lxb_dom_node_remove_wo_events(lxb_dom_node_t *node);

LXB_API void
lxb_dom_node_remove(lxb_dom_node_t *node);

LXB_API lxb_status_t
lxb_dom_node_replace_all(lxb_dom_node_t *parent, lxb_dom_node_t *node);

LXB_API void
lxb_dom_node_simple_walk(lxb_dom_node_t *root,
                         lxb_dom_node_simple_walker_f walker_cb, void *ctx);

LXB_API lxb_status_t
lxb_dom_node_by_tag_name(lxb_dom_node_t *root, lxb_dom_collection_t *collection,
                         const lxb_char_t *qualified_name, size_t len);
LXB_API lxb_status_t
lxb_dom_node_by_class_name(lxb_dom_node_t *root,
                           lxb_dom_collection_t *collection,
                           const lxb_char_t *class_name, size_t len);
LXB_API lxb_status_t
lxb_dom_node_by_attr(lxb_dom_node_t *root, lxb_dom_collection_t *collection,
                     const lxb_char_t *qualified_name, size_t qname_len,
                     const lxb_char_t *value, size_t value_len,
                     bool case_insensitive);
LXB_API lxb_status_t
lxb_dom_node_by_attr_begin(lxb_dom_node_t *root,
                           lxb_dom_collection_t *collection,
                           const lxb_char_t *qualified_name, size_t qname_len,
                           const lxb_char_t *value, size_t value_len,
                           bool case_insensitive);
LXB_API lxb_status_t
lxb_dom_node_by_attr_end(lxb_dom_node_t *root, lxb_dom_collection_t *collection,
                         const lxb_char_t *qualified_name, size_t qname_len,
                         const lxb_char_t *value, size_t value_len,
                         bool case_insensitive);
LXB_API lxb_status_t
lxb_dom_node_by_attr_contain(lxb_dom_node_t *root,
                             lxb_dom_collection_t *collection,
                             const lxb_char_t *qualified_name, size_t qname_len,
                             const lxb_char_t *value, size_t value_len,
                             bool case_insensitive);

/*
 * Memory of returns value will be freed in document destroy moment.
 * If you need to release returned resource after use, then call the
 * lxb_dom_document_destroy_text(node->owner_document, text) function.
 */
LXB_API lxb_char_t *
lxb_dom_node_text_content(lxb_dom_node_t *node, size_t *len);

LXB_API lxb_status_t
lxb_dom_node_text_content_set(lxb_dom_node_t *node,
                              const lxb_char_t *content, size_t len);

LXB_API bool
lxb_dom_node_is_empty(lxb_dom_node_t *root);


/*
 * Inline functions
 */
lxb_inline lxb_tag_id_t
lxb_dom_node_tag_id(lxb_dom_node_t *node)
{
    return node->local_name;
}

lxb_inline lxb_dom_node_t *
lxb_dom_node_next(lxb_dom_node_t *node)
{
    return node->next;
}

lxb_inline lxb_dom_node_t *
lxb_dom_node_prev(lxb_dom_node_t *node)
{
    return node->prev;
}

lxb_inline lxb_dom_node_t *
lxb_dom_node_parent(lxb_dom_node_t *node)
{
    return node->parent;
}

lxb_inline lxb_dom_node_t *
lxb_dom_node_first_child(lxb_dom_node_t *node)
{
    return node->first_child;
}

lxb_inline lxb_dom_node_t *
lxb_dom_node_last_child(lxb_dom_node_t *node)
{
    return node->last_child;
}

/*
 * No inline functions for ABI.
 */
LXB_API lxb_tag_id_t
lxb_dom_node_tag_id_noi(lxb_dom_node_t *node);

LXB_API lxb_dom_node_t *
lxb_dom_node_next_noi(lxb_dom_node_t *node);

LXB_API lxb_dom_node_t *
lxb_dom_node_prev_noi(lxb_dom_node_t *node);

LXB_API lxb_dom_node_t *
lxb_dom_node_parent_noi(lxb_dom_node_t *node);

LXB_API lxb_dom_node_t *
lxb_dom_node_first_child_noi(lxb_dom_node_t *node);

LXB_API lxb_dom_node_t *
lxb_dom_node_last_child_noi(lxb_dom_node_t *node);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_DOM_NODE_H */
