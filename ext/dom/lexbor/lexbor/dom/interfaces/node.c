/*
 * Copyright (C) 2018-2022 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/dom/interfaces/node.h"
#include "lexbor/dom/interfaces/attr.h"
#include "lexbor/dom/interfaces/document.h"
#include "lexbor/dom/interfaces/document_type.h"
#include "lexbor/dom/interfaces/element.h"
#include "lexbor/dom/interfaces/processing_instruction.h"


typedef struct lxb_dom_node_cb_ctx lxb_dom_node_cb_ctx_t;

typedef bool
(*lxb_dom_node_attr_cmp_f)(lxb_dom_node_cb_ctx_t *ctx, lxb_dom_attr_t *attr);

struct lxb_dom_node_cb_ctx {
    lxb_dom_collection_t       *col;
    lxb_status_t               status;
    lxb_dom_node_attr_cmp_f    cmp_func;

    lxb_dom_attr_id_t          name_id;
    lxb_ns_prefix_id_t         prefix_id;

    const lxb_char_t           *value;
    size_t                     value_length;
};


LXB_API lxb_dom_attr_data_t *
lxb_dom_attr_local_name_append(lexbor_hash_t *hash,
                               const lxb_char_t *name, size_t length);

LXB_API const lxb_tag_data_t *
lxb_tag_append(lexbor_hash_t *hash, lxb_tag_id_t tag_id,
               const lxb_char_t *name, size_t length);

LXB_API const lxb_ns_data_t *
lxb_ns_append(lexbor_hash_t *hash, const lxb_char_t *link, size_t length);

static lexbor_action_t
lxb_dom_node_by_tag_name_cb(lxb_dom_node_t *node, void *ctx);

static lexbor_action_t
lxb_dom_node_by_tag_name_cb_all(lxb_dom_node_t *node, void *ctx);

static lexbor_action_t
lxb_dom_node_by_class_name_cb(lxb_dom_node_t *node, void *ctx);

static lexbor_action_t
lxb_dom_node_by_attr_cb(lxb_dom_node_t *node, void *ctx);

static bool
lxb_dom_node_by_attr_cmp_full(lxb_dom_node_cb_ctx_t *ctx,
                              lxb_dom_attr_t *attr);
static bool
lxb_dom_node_by_attr_cmp_full_case(lxb_dom_node_cb_ctx_t *ctx,
                                   lxb_dom_attr_t *attr);
static bool
lxb_dom_node_by_attr_cmp_begin(lxb_dom_node_cb_ctx_t *ctx,
                               lxb_dom_attr_t *attr);
static bool
lxb_dom_node_by_attr_cmp_begin_case(lxb_dom_node_cb_ctx_t *ctx,
                                    lxb_dom_attr_t *attr);
static bool
lxb_dom_node_by_attr_cmp_end(lxb_dom_node_cb_ctx_t *ctx,
                             lxb_dom_attr_t *attr);
static bool
lxb_dom_node_by_attr_cmp_end_case(lxb_dom_node_cb_ctx_t *ctx,
                                  lxb_dom_attr_t *attr);
static bool
lxb_dom_node_by_attr_cmp_contain(lxb_dom_node_cb_ctx_t *ctx,
                                 lxb_dom_attr_t *attr);
static bool
lxb_dom_node_by_attr_cmp_contain_case(lxb_dom_node_cb_ctx_t *ctx,
                                      lxb_dom_attr_t *attr);

static lexbor_action_t
lxb_dom_node_text_content_size(lxb_dom_node_t *node, void *ctx);

static lexbor_action_t
lxb_dom_node_text_content_concatenate(lxb_dom_node_t *node, void *ctx);


lxb_dom_node_t *
lxb_dom_node_interface_create(lxb_dom_document_t *document)
{
    lxb_dom_node_t *element;

    element = lexbor_mraw_calloc(document->mraw,
                                 sizeof(lxb_dom_node_t));
    if (element == NULL) {
        return NULL;
    }

    element->owner_document = lxb_dom_document_owner(document);
    element->type = LXB_DOM_NODE_TYPE_UNDEF;

    return element;
}

lxb_dom_node_t *
lxb_dom_node_interface_clone(lxb_dom_document_t *document,
                             const lxb_dom_node_t *node, bool is_attr)
{
    lxb_dom_node_t *new;

    new = lxb_dom_node_interface_create(document);
    if (new == NULL) {
        return NULL;
    }

    if (lxb_dom_node_interface_copy(new, node, is_attr) != LXB_STATUS_OK) {
        return lxb_dom_document_destroy_interface(new);
    }

    return new;
}

lxb_dom_node_t *
lxb_dom_node_interface_destroy(lxb_dom_node_t *node)
{
    lxb_dom_document_t *doc = node->owner_document;

    if (doc->ev_destroy != NULL) {
        doc->ev_destroy(node);
    }

    return lexbor_mraw_free(node->owner_document->mraw, node);
}

lxb_status_t
lxb_dom_node_interface_copy(lxb_dom_node_t *dst,
                            const lxb_dom_node_t *src, bool is_attr)
{
    lxb_dom_document_t *from, *to;
    const lxb_ns_data_t *ns;
    const lxb_tag_data_t *tag;
    const lxb_ns_prefix_data_t *prefix;
    const lexbor_hash_entry_t *entry;
    const lxb_dom_attr_data_t *data;

    dst->type = src->type;
    dst->user = src->user;

    if (dst->owner_document == src->owner_document) {
        dst->local_name = src->local_name;
        dst->ns = src->ns;
        dst->prefix = src->prefix;

        return LXB_STATUS_OK;
    }

    from = src->owner_document;
    to = dst->owner_document;

    if (is_attr) {
        if (src->local_name < LXB_DOM_ATTR__LAST_ENTRY) {
            dst->local_name = src->local_name;
        }
        else {
            data = lxb_dom_attr_data_by_id(from->attrs, src->local_name);
            if (data == NULL) {
                return LXB_STATUS_ERROR_NOT_EXISTS;
            }

            entry = &data->entry;

            data = lxb_dom_attr_local_name_append(to->attrs,
                                                  lexbor_hash_entry_str(entry),
                                                  entry->length);
            if (data == NULL) {
                return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
            }

            dst->local_name = (lxb_dom_attr_id_t) data;
        }
    }
    else {
        if (src->local_name < LXB_TAG__LAST_ENTRY) {
            dst->local_name = src->local_name;
        }
        else {
            tag = lxb_tag_data_by_id(from->tags, src->local_name);
            if (tag == NULL) {
                return LXB_STATUS_ERROR_NOT_EXISTS;
            }

            entry = &tag->entry;

            tag = lxb_tag_append(to->tags, LXB_TAG__UNDEF,
                                 lexbor_hash_entry_str(entry), entry->length);
            if (tag == NULL) {
                return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
            }

            dst->local_name = (lxb_dom_attr_id_t) tag;
        }
    }

    if (src->ns < LXB_NS__LAST_ENTRY) {
        dst->ns = src->ns;
    }
    else {
        ns = lxb_ns_data_by_id(from->ns, src->ns);
        if (ns == NULL) {
            return LXB_STATUS_ERROR_NOT_EXISTS;
        }

        entry = &ns->entry;

        ns = lxb_ns_append(to->ns, lexbor_hash_entry_str(entry),
                           entry->length);
        if (ns == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }

        dst->ns = (lxb_ns_id_t) ns;
    }

    if (src->prefix < LXB_NS__LAST_ENTRY) {
        dst->prefix = src->prefix;
    }
    else {
        prefix = lxb_ns_prefix_data_by_id(from->prefix, src->prefix);
        if (prefix == NULL) {
            return LXB_STATUS_ERROR_NOT_EXISTS;
        }

        entry = &prefix->entry;

        prefix = lxb_ns_prefix_append(to->prefix, lexbor_hash_entry_str(entry),
                                      entry->length);
        if (prefix == NULL) {
            return LXB_STATUS_ERROR;
        }

        dst->prefix = (lxb_ns_prefix_id_t) prefix;
    }

    return LXB_STATUS_OK;
}

lxb_dom_node_t *
lxb_dom_node_destroy(lxb_dom_node_t *node)
{
    lxb_dom_node_remove(node);

    if (node->owner_document->ev_destroy != NULL) {
        node->owner_document->ev_destroy(node);
    }

    return lxb_dom_document_destroy_interface(node);
}

lxb_dom_node_t *
lxb_dom_node_destroy_deep(lxb_dom_node_t *root)
{
    lxb_dom_node_t *tmp;
    lxb_dom_node_t *node = root;

    while (node != NULL) {
        if (node->first_child != NULL) {
            node = node->first_child;
        }
        else {
            while(node != root && node->next == NULL) {
                tmp = node->parent;

                lxb_dom_node_destroy(node);

                node = tmp;
            }

            if (node == root) {
                lxb_dom_node_destroy(node);

                break;
            }

            tmp = node->next;

            lxb_dom_node_destroy(node);

            node = tmp;
        }
    }

    return NULL;
}

lxb_dom_node_t *
lxb_dom_node_clone(lxb_dom_node_t *node, bool deep)
{
    return lxb_dom_document_import_node(node->owner_document, node, deep);
}

const lxb_char_t *
lxb_dom_node_name(lxb_dom_node_t *node, size_t *len)
{
    switch (node->type) {
        case LXB_DOM_NODE_TYPE_ELEMENT:
            return lxb_dom_element_tag_name(lxb_dom_interface_element(node),
                                            len);

        case LXB_DOM_NODE_TYPE_ATTRIBUTE:
            return lxb_dom_attr_qualified_name(lxb_dom_interface_attr(node),
                                               len);

        case LXB_DOM_NODE_TYPE_TEXT:
            if (len != NULL) {
                *len = sizeof("#text") - 1;
            }

            return (const lxb_char_t *) "#text";

        case LXB_DOM_NODE_TYPE_CDATA_SECTION:
            if (len != NULL) {
                *len = sizeof("#cdata-section") - 1;
            }

            return (const lxb_char_t *) "#cdata-section";

        case LXB_DOM_NODE_TYPE_PROCESSING_INSTRUCTION:
            return lxb_dom_processing_instruction_target(lxb_dom_interface_processing_instruction(node),
                                                         len);

        case LXB_DOM_NODE_TYPE_COMMENT:
            if (len != NULL) {
                *len = sizeof("#comment") - 1;
            }

            return (const lxb_char_t *) "#comment";

        case LXB_DOM_NODE_TYPE_DOCUMENT:
            if (len != NULL) {
                *len = sizeof("#document") - 1;
            }

            return (const lxb_char_t *) "#document";

        case LXB_DOM_NODE_TYPE_DOCUMENT_TYPE:
            return lxb_dom_document_type_name(lxb_dom_interface_document_type(node),
                                              len);

        case LXB_DOM_NODE_TYPE_DOCUMENT_FRAGMENT:
            if (len != NULL) {
                *len = sizeof("#document-fragment") - 1;
            }

            return (const lxb_char_t *) "#document-fragment";

        default:
            break;
    }

    if (len != NULL) {
        *len = 0;
    }

    return NULL;
}

void
lxb_dom_node_insert_child_wo_events(lxb_dom_node_t *to, lxb_dom_node_t *node)
{
    if (to->last_child != NULL) {
        to->last_child->next = node;
    }
    else {
        to->first_child = node;
    }

    node->parent = to;
    node->next = NULL;
    node->prev = to->last_child;

    to->last_child = node;
}

void
lxb_dom_node_insert_child(lxb_dom_node_t *to, lxb_dom_node_t *node)
{
    lxb_dom_node_insert_child_wo_events(to, node);

    if (node->owner_document->ev_insert != NULL) {
        node->owner_document->ev_insert(node);
    }
}

void
lxb_dom_node_insert_before_wo_events(lxb_dom_node_t *to, lxb_dom_node_t *node)
{
    if (to->prev != NULL) {
        to->prev->next = node;
    }
    else {
        if (to->parent != NULL) {
            to->parent->first_child = node;
        }
    }

    node->parent = to->parent;
    node->next = to;
    node->prev = to->prev;

    to->prev = node;
}

void
lxb_dom_node_insert_before(lxb_dom_node_t *to, lxb_dom_node_t *node)
{
    lxb_dom_node_insert_before_wo_events(to, node);

    if (node->owner_document->ev_insert != NULL) {
        node->owner_document->ev_insert(node);
    }
}

void
lxb_dom_node_insert_after_wo_events(lxb_dom_node_t *to, lxb_dom_node_t *node)
{
    if (to->next != NULL) {
        to->next->prev = node;
    }
    else {
        if (to->parent != NULL) {
            to->parent->last_child = node;
        }
    }

    node->parent = to->parent;
    node->next = to->next;
    node->prev = to;
    to->next = node;
}

void
lxb_dom_node_insert_after(lxb_dom_node_t *to, lxb_dom_node_t *node)
{
    lxb_dom_node_insert_after_wo_events(to, node);

    if (node->owner_document->ev_insert != NULL) {
        node->owner_document->ev_insert(node);
    }
}

void
lxb_dom_node_remove_wo_events(lxb_dom_node_t *node)
{
    if (node->parent != NULL) {
        if (node->parent->first_child == node) {
            node->parent->first_child = node->next;
        }

        if (node->parent->last_child == node) {
            node->parent->last_child = node->prev;
        }
    }

    if (node->next != NULL) {
        node->next->prev = node->prev;
    }

    if (node->prev != NULL) {
        node->prev->next = node->next;
    }

    node->parent = NULL;
    node->next = NULL;
    node->prev = NULL;
}

void
lxb_dom_node_remove(lxb_dom_node_t *node)
{
    if (node->owner_document->ev_remove != NULL) {
        node->owner_document->ev_remove(node);
    }

    lxb_dom_node_remove_wo_events(node);
}

lxb_status_t
lxb_dom_node_replace_all(lxb_dom_node_t *parent, lxb_dom_node_t *node)
{
    while (parent->first_child != NULL) {
        lxb_dom_node_destroy_deep(parent->first_child);
    }

    lxb_dom_node_insert_child(parent, node);

    return LXB_STATUS_OK;
}

void
lxb_dom_node_simple_walk(lxb_dom_node_t *root,
                         lxb_dom_node_simple_walker_f walker_cb, void *ctx)
{
    lexbor_action_t action;
    lxb_dom_node_t *node = root->first_child;

    while (node != NULL) {
        action = walker_cb(node, ctx);
        if (action == LEXBOR_ACTION_STOP) {
            return;
        }

        if (node->first_child != NULL && action != LEXBOR_ACTION_NEXT) {
            node = node->first_child;
        }
        else {
            while(node != root && node->next == NULL) {
                node = node->parent;
            }

            if (node == root) {
                break;
            }

            node = node->next;
        }
    }
}

lxb_inline lxb_status_t
lxb_dom_node_prepare_by_attr(lxb_dom_document_t *document,
                             lxb_dom_node_cb_ctx_t *cb_ctx,
                             const lxb_char_t *qname, size_t qlen)
{
    size_t length;
    const lxb_char_t *prefix_end;
    const lxb_dom_attr_data_t *attr_data;
    const lxb_ns_prefix_data_t *prefix_data;

    cb_ctx->prefix_id = LXB_NS__UNDEF;

    prefix_end = memchr(qname, ':', qlen);

    if (prefix_end != NULL) {
        length = prefix_end - qname;

        if (length == 0) {
            return LXB_STATUS_ERROR_WRONG_ARGS;
        }

        prefix_data = lxb_ns_prefix_data_by_name(document->prefix, qname, qlen);
        if (prefix_data == NULL) {
            return LXB_STATUS_STOP;
        }

        cb_ctx->prefix_id = prefix_data->prefix_id;

        length += 1;

        if (length >= qlen) {
            return LXB_STATUS_ERROR_WRONG_ARGS;
        }

        qname += length;
        qlen -= length;
    }

    attr_data = lxb_dom_attr_data_by_local_name(document->attrs, qname, qlen);
    if (attr_data == NULL) {
        return LXB_STATUS_STOP;
    }

    cb_ctx->name_id = attr_data->attr_id;

    return LXB_STATUS_OK;
}

lxb_inline lxb_status_t
lxb_dom_node_prepare_by(lxb_dom_document_t *document,
                        lxb_dom_node_cb_ctx_t *cb_ctx,
                        const lxb_char_t *qname, size_t qlen)
{
    size_t length;
    const lxb_char_t *prefix_end;
    const lxb_tag_data_t *tag_data;
    const lxb_ns_prefix_data_t *prefix_data;

    cb_ctx->prefix_id = LXB_NS__UNDEF;

    prefix_end = memchr(qname, ':', qlen);

    if (prefix_end != NULL) {
        length = prefix_end - qname;

        if (length == 0) {
            return LXB_STATUS_ERROR_WRONG_ARGS;
        }

        prefix_data = lxb_ns_prefix_data_by_name(document->prefix, qname, qlen);
        if (prefix_data == NULL) {
            return LXB_STATUS_STOP;
        }

        cb_ctx->prefix_id = prefix_data->prefix_id;

        length += 1;

        if (length >= qlen) {
            return LXB_STATUS_ERROR_WRONG_ARGS;
        }

        qname += length;
        qlen -= length;
    }

    tag_data = lxb_tag_data_by_name(document->tags, qname, qlen);
    if (tag_data == NULL) {
        return LXB_STATUS_STOP;
    }

    cb_ctx->name_id = tag_data->tag_id;

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_dom_node_by_tag_name(lxb_dom_node_t *root,
                         lxb_dom_collection_t *collection,
                         const lxb_char_t *qualified_name, size_t len)
{
    lxb_status_t status;
    lxb_dom_node_cb_ctx_t cb_ctx = {0};

    cb_ctx.col = collection;

    /* "*" (U+002A) */
    if (len == 1 && *qualified_name == 0x2A) {
        lxb_dom_node_simple_walk(root, lxb_dom_node_by_tag_name_cb_all,
                                 &cb_ctx);
        return cb_ctx.status;
    }

    status = lxb_dom_node_prepare_by(root->owner_document, &cb_ctx,
                                     qualified_name, len);
    if (status != LXB_STATUS_OK) {
        if (status == LXB_STATUS_STOP) {
            return LXB_STATUS_OK;
        }

        return status;
    }

    lxb_dom_node_simple_walk(lxb_dom_interface_node(root),
                             lxb_dom_node_by_tag_name_cb, &cb_ctx);

    return cb_ctx.status;
}

static lexbor_action_t
lxb_dom_node_by_tag_name_cb_all(lxb_dom_node_t *node, void *ctx)
{
    if (node->type != LXB_DOM_NODE_TYPE_ELEMENT) {
        return LEXBOR_ACTION_OK;
    }

    lxb_dom_node_cb_ctx_t *cb_ctx = ctx;

    cb_ctx->status = lxb_dom_collection_append(cb_ctx->col, node);
    if (cb_ctx->status != LXB_STATUS_OK) {
        return LEXBOR_ACTION_STOP;
    }

    return LEXBOR_ACTION_OK;
}

static lexbor_action_t
lxb_dom_node_by_tag_name_cb(lxb_dom_node_t *node, void *ctx)
{
    if (node->type != LXB_DOM_NODE_TYPE_ELEMENT) {
        return LEXBOR_ACTION_OK;
    }

    lxb_dom_node_cb_ctx_t *cb_ctx = ctx;

    if (node->local_name == cb_ctx->name_id
        && node->prefix == cb_ctx->prefix_id)
    {
        cb_ctx->status = lxb_dom_collection_append(cb_ctx->col, node);
        if (cb_ctx->status != LXB_STATUS_OK) {
            return LEXBOR_ACTION_STOP;
        }
    }

    return LEXBOR_ACTION_OK;
}

lxb_status_t
lxb_dom_node_by_class_name(lxb_dom_node_t *root,
                           lxb_dom_collection_t *collection,
                           const lxb_char_t *class_name, size_t len)
{
    if (class_name == NULL || len == 0) {
        return LXB_STATUS_OK;
    }

    lxb_dom_node_cb_ctx_t cb_ctx = {0};

    cb_ctx.col = collection;
    cb_ctx.value = class_name;
    cb_ctx.value_length = len;

    lxb_dom_node_simple_walk(lxb_dom_interface_node(root),
                             lxb_dom_node_by_class_name_cb, &cb_ctx);

    return cb_ctx.status;
}

static lexbor_action_t
lxb_dom_node_by_class_name_cb(lxb_dom_node_t *node, void *ctx)
{
    if (node->type != LXB_DOM_NODE_TYPE_ELEMENT) {
        return LEXBOR_ACTION_OK;
    }

    lxb_dom_node_cb_ctx_t *cb_ctx = ctx;
    lxb_dom_element_t *el = lxb_dom_interface_element(node);

    if (el->attr_class == NULL
        || el->attr_class->value == NULL
        || el->attr_class->value->length < cb_ctx->value_length)
    {
        return LEXBOR_ACTION_OK;
    }

    const lxb_char_t *data = el->attr_class->value->data;
    size_t length = el->attr_class->value->length;

    bool is_it = false;
    const lxb_char_t *pos = data;
    const lxb_char_t *end = data + length;

    lxb_dom_document_t *doc = el->node.owner_document;

    for (; data < end; data++) {
        if (lexbor_utils_whitespace(*data, ==, ||)) {

            if (pos != data && (size_t) (data - pos) == cb_ctx->value_length) {
                if (doc->compat_mode == LXB_DOM_DOCUMENT_CMODE_QUIRKS) {
                    is_it = lexbor_str_data_ncasecmp(pos, cb_ctx->value,
                                                     cb_ctx->value_length);
                }
                else {
                    is_it = lexbor_str_data_ncmp(pos, cb_ctx->value,
                                                 cb_ctx->value_length);
                }

                if (is_it) {
                    cb_ctx->status = lxb_dom_collection_append(cb_ctx->col,
                                                               node);
                    if (cb_ctx->status != LXB_STATUS_OK) {
                        return LEXBOR_ACTION_STOP;
                    }

                    return LEXBOR_ACTION_OK;
                }
            }

            if ((size_t) (end - data) < cb_ctx->value_length) {
                return LEXBOR_ACTION_OK;
            }

            pos = data + 1;
        }
    }

    if ((size_t) (end - pos) == cb_ctx->value_length) {
        if (doc->compat_mode == LXB_DOM_DOCUMENT_CMODE_QUIRKS) {
            is_it = lexbor_str_data_ncasecmp(pos, cb_ctx->value,
                                             cb_ctx->value_length);
        }
        else {
            is_it = lexbor_str_data_ncmp(pos, cb_ctx->value,
                                         cb_ctx->value_length);
        }

        if (is_it) {
            cb_ctx->status = lxb_dom_collection_append(cb_ctx->col, node);
            if (cb_ctx->status != LXB_STATUS_OK) {
                return LEXBOR_ACTION_STOP;
            }
        }
    }

    return LEXBOR_ACTION_OK;
}

lxb_status_t
lxb_dom_node_by_attr(lxb_dom_node_t *root,
                     lxb_dom_collection_t *collection,
                     const lxb_char_t *qualified_name, size_t qname_len,
                     const lxb_char_t *value, size_t value_len,
                     bool case_insensitive)
{
    lxb_status_t status;
    lxb_dom_node_cb_ctx_t cb_ctx = {0};

    cb_ctx.col = collection;
    cb_ctx.value = value;
    cb_ctx.value_length = value_len;

    status = lxb_dom_node_prepare_by_attr(root->owner_document, &cb_ctx,
                                          qualified_name, qname_len);
    if (status != LXB_STATUS_OK) {
        if (status == LXB_STATUS_STOP) {
            return LXB_STATUS_OK;
        }

        return status;
    }

    if (case_insensitive) {
        cb_ctx.cmp_func = lxb_dom_node_by_attr_cmp_full_case;
    }
    else {
        cb_ctx.cmp_func = lxb_dom_node_by_attr_cmp_full;
    }

    lxb_dom_node_simple_walk(root, lxb_dom_node_by_attr_cb, &cb_ctx);

    return cb_ctx.status;
}

lxb_status_t
lxb_dom_node_by_attr_begin(lxb_dom_node_t *root,
                           lxb_dom_collection_t *collection,
                           const lxb_char_t *qualified_name, size_t qname_len,
                           const lxb_char_t *value, size_t value_len,
                           bool case_insensitive)
{
    lxb_status_t status;
    lxb_dom_node_cb_ctx_t cb_ctx = {0};

    cb_ctx.col = collection;
    cb_ctx.value = value;
    cb_ctx.value_length = value_len;

    status = lxb_dom_node_prepare_by_attr(root->owner_document, &cb_ctx,
                                          qualified_name, qname_len);
    if (status != LXB_STATUS_OK) {
        if (status == LXB_STATUS_STOP) {
            return LXB_STATUS_OK;
        }

        return status;
    }

    if (case_insensitive) {
        cb_ctx.cmp_func = lxb_dom_node_by_attr_cmp_begin_case;
    }
    else {
        cb_ctx.cmp_func = lxb_dom_node_by_attr_cmp_begin;
    }

    lxb_dom_node_simple_walk(lxb_dom_interface_node(root),
                             lxb_dom_node_by_attr_cb, &cb_ctx);

    return cb_ctx.status;
}

lxb_status_t
lxb_dom_node_by_attr_end(lxb_dom_node_t *root, lxb_dom_collection_t *collection,
                         const lxb_char_t *qualified_name, size_t qname_len,
                         const lxb_char_t *value, size_t value_len,
                         bool case_insensitive)
{
    lxb_status_t status;
    lxb_dom_node_cb_ctx_t cb_ctx = {0};

    cb_ctx.col = collection;
    cb_ctx.value = value;
    cb_ctx.value_length = value_len;

    status = lxb_dom_node_prepare_by_attr(root->owner_document, &cb_ctx,
                                          qualified_name, qname_len);
    if (status != LXB_STATUS_OK) {
        if (status == LXB_STATUS_STOP) {
            return LXB_STATUS_OK;
        }

        return status;
    }

    if (case_insensitive) {
        cb_ctx.cmp_func = lxb_dom_node_by_attr_cmp_end_case;
    }
    else {
        cb_ctx.cmp_func = lxb_dom_node_by_attr_cmp_end;
    }

    lxb_dom_node_simple_walk(root, lxb_dom_node_by_attr_cb, &cb_ctx);

    return cb_ctx.status;
}

lxb_status_t
lxb_dom_node_by_attr_contain(lxb_dom_node_t *root,
                             lxb_dom_collection_t *collection,
                             const lxb_char_t *qualified_name, size_t qname_len,
                             const lxb_char_t *value, size_t value_len,
                             bool case_insensitive)
{
    lxb_status_t status;
    lxb_dom_node_cb_ctx_t cb_ctx = {0};

    cb_ctx.col = collection;
    cb_ctx.value = value;
    cb_ctx.value_length = value_len;

    status = lxb_dom_node_prepare_by_attr(root->owner_document, &cb_ctx,
                                          qualified_name, qname_len);
    if (status != LXB_STATUS_OK) {
        if (status == LXB_STATUS_STOP) {
            return LXB_STATUS_OK;
        }

        return status;
    }

    if (case_insensitive) {
        cb_ctx.cmp_func = lxb_dom_node_by_attr_cmp_contain_case;
    }
    else {
        cb_ctx.cmp_func = lxb_dom_node_by_attr_cmp_contain;
    }

    lxb_dom_node_simple_walk(root, lxb_dom_node_by_attr_cb, &cb_ctx);

    return cb_ctx.status;
}

static lexbor_action_t
lxb_dom_node_by_attr_cb(lxb_dom_node_t *node, void *ctx)
{
    if (node->type != LXB_DOM_NODE_TYPE_ELEMENT) {
        return LEXBOR_ACTION_OK;
    }

    lxb_dom_attr_t *attr;
    lxb_dom_node_cb_ctx_t *cb_ctx = ctx;
    lxb_dom_element_t *el = lxb_dom_interface_element(node);

    attr = lxb_dom_element_attr_by_id(el, cb_ctx->name_id);
    if (attr == NULL) {
        return LEXBOR_ACTION_OK;
    }

    if ((cb_ctx->value_length == 0 && (attr->value == NULL || attr->value->length == 0))
        || cb_ctx->cmp_func(cb_ctx, attr))
    {
        cb_ctx->status = lxb_dom_collection_append(cb_ctx->col, node);

        if (cb_ctx->status != LXB_STATUS_OK) {
            return LEXBOR_ACTION_STOP;
        }
    }

    return LEXBOR_ACTION_OK;
}

static bool
lxb_dom_node_by_attr_cmp_full(lxb_dom_node_cb_ctx_t *ctx, lxb_dom_attr_t *attr)
{
    if (attr->value != NULL && ctx->value_length == attr->value->length
        && lexbor_str_data_ncmp(attr->value->data, ctx->value,
                                ctx->value_length))
    {
        return true;
    }

    return attr->value == NULL && ctx->value_length == 0;
}

static bool
lxb_dom_node_by_attr_cmp_full_case(lxb_dom_node_cb_ctx_t *ctx,
                                   lxb_dom_attr_t *attr)
{
    if (attr->value != NULL && ctx->value_length == attr->value->length
        && lexbor_str_data_ncasecmp(attr->value->data, ctx->value,
                                    ctx->value_length))
    {
        return true;
    }

    return attr->value == NULL && ctx->value_length == 0;
}

static bool
lxb_dom_node_by_attr_cmp_begin(lxb_dom_node_cb_ctx_t *ctx, lxb_dom_attr_t *attr)
{
    if (attr->value != NULL && ctx->value_length <= attr->value->length
        && lexbor_str_data_ncmp(attr->value->data, ctx->value,
                                ctx->value_length))
    {
        return true;
    }

    return attr->value == NULL && ctx->value_length == 0;
}

static bool
lxb_dom_node_by_attr_cmp_begin_case(lxb_dom_node_cb_ctx_t *ctx,
                                    lxb_dom_attr_t *attr)
{
    if (attr->value != NULL && ctx->value_length <= attr->value->length
        && lexbor_str_data_ncasecmp(attr->value->data,
                                    ctx->value, ctx->value_length))
    {
        return true;
    }

    return attr->value == NULL && ctx->value_length == 0;
}

static bool
lxb_dom_node_by_attr_cmp_end(lxb_dom_node_cb_ctx_t *ctx, lxb_dom_attr_t *attr)
{
    if (attr->value != NULL && ctx->value_length <= attr->value->length) {
        size_t dif = attr->value->length - ctx->value_length;

        if (lexbor_str_data_ncmp_end(&attr->value->data[dif],
                                     ctx->value, ctx->value_length))
        {
            return true;
        }
    }

    return attr->value == NULL && ctx->value_length == 0;
}

static bool
lxb_dom_node_by_attr_cmp_end_case(lxb_dom_node_cb_ctx_t *ctx,
                                  lxb_dom_attr_t *attr)
{
    if (attr->value != NULL && ctx->value_length <= attr->value->length) {
        size_t dif = attr->value->length - ctx->value_length;

        if (lexbor_str_data_ncasecmp_end(&attr->value->data[dif],
                                         ctx->value, ctx->value_length))
        {
            return true;
        }
    }

    return attr->value == NULL && ctx->value_length == 0;
}

static bool
lxb_dom_node_by_attr_cmp_contain(lxb_dom_node_cb_ctx_t *ctx,
                                 lxb_dom_attr_t *attr)
{
    if (attr->value != NULL && ctx->value_length <= attr->value->length
        && lexbor_str_data_ncmp_contain(attr->value->data, attr->value->length,
                                        ctx->value, ctx->value_length))
    {
        return true;
    }

    return attr->value == NULL && ctx->value_length == 0;
}

static bool
lxb_dom_node_by_attr_cmp_contain_case(lxb_dom_node_cb_ctx_t *ctx,
                                      lxb_dom_attr_t *attr)
{
    if (attr->value != NULL && ctx->value_length <= attr->value->length
        && lexbor_str_data_ncasecmp_contain(attr->value->data, attr->value->length,
                                            ctx->value, ctx->value_length))
    {
        return true;
    }

    return attr->value == NULL && ctx->value_length == 0;
}

lxb_char_t *
lxb_dom_node_text_content(lxb_dom_node_t *node, size_t *len)
{
    lxb_char_t *text;
    size_t length = 0;

    switch (node->type) {
        case LXB_DOM_NODE_TYPE_DOCUMENT_FRAGMENT:
        case LXB_DOM_NODE_TYPE_ELEMENT:
            lxb_dom_node_simple_walk(node, lxb_dom_node_text_content_size,
                                     &length);

            text = lxb_dom_document_create_text(node->owner_document,
                                                (length + 1));
            if (text == NULL) {
                goto failed;
            }

            lxb_dom_node_simple_walk(node, lxb_dom_node_text_content_concatenate,
                                     &text);

            text -= length;

            break;

        case LXB_DOM_NODE_TYPE_ATTRIBUTE: {
            const lxb_char_t *attr_text;

            attr_text = lxb_dom_attr_value(lxb_dom_interface_attr(node), &length);
            if (attr_text == NULL) {
                goto failed;
            }

            text = lxb_dom_document_create_text(node->owner_document,
                                                (length + 1));
            if (text == NULL) {
                goto failed;
            }

            /* +1 == with null '\0' */
            memcpy(text, attr_text, sizeof(lxb_char_t) * (length + 1));

            break;
        }

        case LXB_DOM_NODE_TYPE_TEXT:
        case LXB_DOM_NODE_TYPE_PROCESSING_INSTRUCTION:
        case LXB_DOM_NODE_TYPE_COMMENT: {
            lxb_dom_character_data_t *ch_data;

            ch_data = lxb_dom_interface_character_data(node);
            length = ch_data->data.length;

            text = lxb_dom_document_create_text(node->owner_document,
                                                (length + 1));
            if (text == NULL) {
                goto failed;
            }

            /* +1 == with null '\0' */
            memcpy(text, ch_data->data.data, sizeof(lxb_char_t) * (length + 1));

            break;
        }

        default:
            goto failed;
    }

    if (len != NULL) {
        *len = length;
    }

    text[length] = 0x00;

    return text;

failed:

    if (len != NULL) {
        *len = 0;
    }

    return NULL;
}

static lexbor_action_t
lxb_dom_node_text_content_size(lxb_dom_node_t *node, void *ctx)
{
    if (node->type == LXB_DOM_NODE_TYPE_TEXT) {
        *((size_t *) ctx) += lxb_dom_interface_text(node)->char_data.data.length;
    }

    return LEXBOR_ACTION_OK;
}

static lexbor_action_t
lxb_dom_node_text_content_concatenate(lxb_dom_node_t *node, void *ctx)
{
    if (node->type != LXB_DOM_NODE_TYPE_TEXT) {
        return LEXBOR_ACTION_OK;
    }

    lxb_char_t **text = (lxb_char_t **) ctx;
    lxb_dom_character_data_t *ch_data = &lxb_dom_interface_text(node)->char_data;

    memcpy(*text, ch_data->data.data, sizeof(lxb_char_t) * ch_data->data.length);

    *text = *text + ch_data->data.length;

    return LEXBOR_ACTION_OK;
}

lxb_status_t
lxb_dom_node_text_content_set(lxb_dom_node_t *node,
                              const lxb_char_t *content, size_t len)
{
    lxb_status_t status;

    switch (node->type) {
        case LXB_DOM_NODE_TYPE_DOCUMENT_FRAGMENT:
        case LXB_DOM_NODE_TYPE_ELEMENT: {
            lxb_dom_text_t *text;

            text = lxb_dom_document_create_text_node(node->owner_document,
                                                     content, len);
            if (text == NULL) {
                return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
            }

            status = lxb_dom_node_replace_all(node, lxb_dom_interface_node(text));
            if (status != LXB_STATUS_OK) {
                lxb_dom_document_destroy_interface(text);

                return status;
            }

            break;
        }

        case LXB_DOM_NODE_TYPE_ATTRIBUTE:
            return lxb_dom_attr_set_existing_value(lxb_dom_interface_attr(node),
                                                   content, len);

        case LXB_DOM_NODE_TYPE_TEXT:
        case LXB_DOM_NODE_TYPE_PROCESSING_INSTRUCTION:
        case LXB_DOM_NODE_TYPE_COMMENT:
            return lxb_dom_character_data_replace(lxb_dom_interface_character_data(node),
                                                  content, len, 0, 0);

        default:
            return LXB_STATUS_OK;
    }

    return LXB_STATUS_OK;
}

bool
lxb_dom_node_is_empty(const lxb_dom_node_t *root)
{
    lxb_char_t chr;
    lexbor_str_t *str;
    const lxb_char_t *data, *end;
    lxb_dom_node_t *node = root->first_child;

    while (node != NULL) {
        if(node->local_name != LXB_TAG__EM_COMMENT) {
            if(node->local_name != LXB_TAG__TEXT)
                return false;

            str = &lxb_dom_interface_text(node)->char_data.data;
            data = str->data;
            end = data + str->length;

            while (data < end) {
                chr = *data++;

                if (lexbor_utils_whitespace(chr, !=, &&)) {
                    return false;
                }
            }
        }

        if(node->first_child != NULL) {
            node = node->first_child;
        }
        else {
            while(node != root && node->next == NULL) {
                node = node->parent;
            }

            if(node == root) {
                break;
            }

            node = node->next;
        }
    }

    return true;
}

lxb_tag_id_t
lxb_dom_node_tag_id_noi(lxb_dom_node_t *node)
{
    return lxb_dom_node_tag_id(node);
}

lxb_dom_node_t *
lxb_dom_node_next_noi(lxb_dom_node_t *node)
{
    return lxb_dom_node_next(node);
}

lxb_dom_node_t *
lxb_dom_node_prev_noi(lxb_dom_node_t *node)
{
    return lxb_dom_node_prev(node);
}

lxb_dom_node_t *
lxb_dom_node_parent_noi(lxb_dom_node_t *node)
{
    return lxb_dom_node_parent(node);
}

lxb_dom_node_t *
lxb_dom_node_first_child_noi(lxb_dom_node_t *node)
{
    return lxb_dom_node_first_child(node);
}

lxb_dom_node_t *
lxb_dom_node_last_child_noi(lxb_dom_node_t *node)
{
    return lxb_dom_node_last_child(node);
}
