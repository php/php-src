/*
 * Copyright (C) 2018-2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/dom/interfaces/attr.h"
#include "lexbor/dom/interfaces/attr_res.h"
#include "lexbor/dom/interfaces/document.h"
#include "lexbor/dom/interfaces/element.h"


LXB_API lxb_dom_attr_data_t *
lxb_dom_attr_local_name_append(lexbor_hash_t *hash,
                               const lxb_char_t *name, size_t length);

LXB_API lxb_dom_attr_data_t *
lxb_dom_attr_qualified_name_append(lexbor_hash_t *hash, const lxb_char_t *name,
                                   size_t length);

const lxb_ns_data_t *
lxb_ns_append(lexbor_hash_t *hash, const lxb_char_t *link, size_t length);


lxb_dom_attr_t *
lxb_dom_attr_interface_create(lxb_dom_document_t *document)
{
    lxb_dom_attr_t *attr;

    attr = lexbor_mraw_calloc(document->mraw, sizeof(lxb_dom_attr_t));
    if (attr == NULL) {
        return NULL;
    }

    lxb_dom_node_t *node = lxb_dom_interface_node(attr);

    node->owner_document = lxb_dom_document_owner(document);
    node->type = LXB_DOM_NODE_TYPE_ATTRIBUTE;

    return attr;
}

lxb_dom_attr_t *
lxb_dom_attr_interface_clone(lxb_dom_document_t *document,
                             const lxb_dom_attr_t *attr)
{
    lxb_dom_attr_t *new;
    const lxb_dom_attr_data_t *data;

    new = lxb_dom_attr_interface_create(document);
    if (new == NULL) {
        return NULL;
    }

    new->node.ns = attr->node.ns;

    if (document == attr->node.owner_document) {
        new->qualified_name = attr->qualified_name;
    }
    else {
        data = lxb_dom_attr_data_by_id(attr->node.owner_document->attrs,
                                       attr->qualified_name);
        if (data == NULL) {
            goto failed;
        }

        if (data->attr_id < LXB_DOM_ATTR__LAST_ENTRY) {
            new->qualified_name = attr->qualified_name;
        }
        else {
            data = lxb_dom_attr_qualified_name_append(document->attrs,
                                                      lexbor_hash_entry_str(&data->entry),
                                                      data->entry.length);
            if (data == NULL) {
                goto failed;
            }

            new->qualified_name = (lxb_dom_attr_id_t) data;
        }
    }

    if (lxb_dom_node_interface_copy(&new->node, &attr->node, true)
        != LXB_STATUS_OK)
    {
        goto failed;
    }

    if (attr->value == NULL) {
        return new;
    }

    new->value = lexbor_mraw_calloc(document->mraw, sizeof(lexbor_str_t));
    if (new->value == NULL) {
        goto failed;
    }

    if (lexbor_str_copy(new->value, attr->value, document->text) == NULL) {
        goto failed;
    }

    return new;

failed:

    return lxb_dom_attr_interface_destroy(new);
}

lxb_dom_attr_t *
lxb_dom_attr_interface_destroy(lxb_dom_attr_t *attr)
{
    lexbor_str_t *value;
    lxb_dom_document_t *doc = lxb_dom_interface_node(attr)->owner_document;

    value = attr->value;

    (void) lxb_dom_node_interface_destroy(lxb_dom_interface_node(attr));

    if (value != NULL) {
        if (value->data != NULL) {
            lexbor_mraw_free(doc->text, value->data);
        }

        lexbor_mraw_free(doc->mraw, value);
    }

    return NULL;
}

lxb_status_t
lxb_dom_attr_set_name(lxb_dom_attr_t *attr, const lxb_char_t *name,
                      size_t length, bool to_lowercase)
{
    lxb_dom_attr_data_t *data;
    lxb_dom_document_t *doc = lxb_dom_interface_node(attr)->owner_document;

    data = lxb_dom_attr_local_name_append(doc->attrs, name, length);
    if (data == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    attr->node.local_name = data->attr_id;

    if (to_lowercase == false) {
        data = lxb_dom_attr_qualified_name_append(doc->attrs, name, length);
        if (data == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }

        attr->qualified_name = (lxb_dom_attr_id_t) data;
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_dom_attr_set_name_ns(lxb_dom_attr_t *attr, const lxb_char_t *link,
                         size_t link_length, const lxb_char_t *name,
                         size_t name_length, bool to_lowercase)
{
    size_t length;
    lxb_char_t *p;
    const lxb_ns_data_t *ns_data;
    lxb_dom_attr_data_t *data;
    lxb_dom_document_t *doc = lxb_dom_interface_node(attr)->owner_document;

    ns_data = lxb_ns_append(doc->ns, link, link_length);
    if (ns_data == NULL || ns_data->ns_id == LXB_NS__UNDEF) {
        return LXB_STATUS_ERROR;
    }

    attr->node.ns = ns_data->ns_id;

    /* TODO: append check https://www.w3.org/TR/xml/#NT-Name */

    p = (lxb_char_t *) memchr(name, ':', name_length);
    if (p == NULL) {
        return lxb_dom_attr_set_name(attr, name, name_length, to_lowercase);
    }

    length = p - name;

    /* local name */
    data = lxb_dom_attr_local_name_append(doc->attrs, &name[(length + 1)],
                                          (name_length - (length + 1)));
    if (data == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    attr->node.local_name = (lxb_dom_attr_id_t) data;

    /* qualified name */
    data = lxb_dom_attr_qualified_name_append(doc->attrs, name, name_length);
    if (data == NULL) {
        return LXB_STATUS_ERROR;
    }

    attr->qualified_name = (lxb_dom_attr_id_t) data;

    /* prefix */
    attr->node.prefix = (lxb_ns_prefix_id_t) lxb_ns_prefix_append(doc->ns, name,
                                                                  length);
    if (attr->node.prefix == 0) {
        return LXB_STATUS_ERROR;
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_dom_attr_set_value(lxb_dom_attr_t *attr,
                       const lxb_char_t *value, size_t value_len)
{
    lxb_status_t status;
    lxb_dom_document_t *doc = lxb_dom_interface_node(attr)->owner_document;

    if (doc->ev_set_value != NULL) {
        status = doc->ev_set_value(lxb_dom_interface_node(attr),
                                   value, value_len);
        if (status != LXB_STATUS_OK) {
            return status;
        }
    }

    if (attr->value == NULL) {
        attr->value = lexbor_mraw_calloc(doc->mraw, sizeof(lexbor_str_t));
        if (attr->value == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }

    if (attr->value->data == NULL) {
        lexbor_str_init(attr->value, doc->text, value_len);
        if (attr->value->data == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }
    else {
        attr->value->length = 0;

        if (lexbor_str_size(attr->value) <= value_len) {
            const lxb_char_t *tmp;

            tmp = lexbor_str_realloc(attr->value, doc->text, (value_len + 1));
            if (tmp == NULL) {
                return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
            }
        }
    }

    memcpy(attr->value->data, value, sizeof(lxb_char_t) * value_len);

    attr->value->data[value_len] = 0x00;
    attr->value->length = value_len;

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_dom_attr_set_value_wo_copy(lxb_dom_attr_t *attr,
                               lxb_char_t *value, size_t value_len)
{
    if (attr->value == NULL) {
        lxb_dom_document_t *doc = lxb_dom_interface_node(attr)->owner_document;

        attr->value = lexbor_mraw_alloc(doc->mraw, sizeof(lexbor_str_t));
        if (attr->value == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }

    attr->value->data = value;
    attr->value->length = value_len;

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_dom_attr_set_existing_value(lxb_dom_attr_t *attr,
                                const lxb_char_t *value, size_t value_len)
{
    return lxb_dom_attr_set_value(attr, value, value_len);
}

lxb_status_t
lxb_dom_attr_clone_name_value(lxb_dom_attr_t *attr_from,
                              lxb_dom_attr_t *attr_to)
{
    attr_to->node.local_name = attr_from->node.local_name;
    attr_to->qualified_name = attr_from->qualified_name;

    return LXB_STATUS_OK;
}

bool
lxb_dom_attr_compare(lxb_dom_attr_t *first, lxb_dom_attr_t *second)
{
    if (first->node.local_name == second->node.local_name
        && first->node.ns == second->node.ns
        && first->qualified_name == second->qualified_name)
    {
        if (first->value == NULL) {
            if (second->value == NULL) {
                return true;
            }

            return false;
        }

        if (second->value != NULL
            && first->value->length == second->value->length
            && lexbor_str_data_ncmp(first->value->data, second->value->data,
                                    first->value->length))
        {
            return true;
        }
    }

    return false;
}

void
lxb_dom_attr_remove(lxb_dom_attr_t *attr)
{
    lxb_dom_element_t *element = attr->owner;
    lxb_dom_document_t *doc = lxb_dom_interface_node(attr)->owner_document;

    if (doc->ev_remove != NULL) {
        doc->ev_remove(lxb_dom_interface_node(attr));
    }

    if (element->attr_id == attr) {
        element->attr_id = NULL;
    }
    else if (element->attr_class == attr) {
        element->attr_class = NULL;
    }

    if (attr->prev != NULL) {
        attr->prev->next = attr->next;
    }
    else {
        element->first_attr = attr->next;
    }

    if (attr->next != NULL) {
        attr->next->prev = attr->prev;
    }
    else {
        element->last_attr = attr->prev;
    }

    attr->next = NULL;
    attr->prev = NULL;
    attr->owner = NULL;
}

lxb_dom_attr_data_t *
lxb_dom_attr_local_name_append(lexbor_hash_t *hash,
                               const lxb_char_t *name, size_t length)
{
    lxb_dom_attr_data_t *data;
    const lexbor_shs_entry_t *entry;

    if (name == NULL || length == 0) {
        return NULL;
    }

    entry = lexbor_shs_entry_get_lower_static(lxb_dom_attr_res_shs_data,
                                              name, length);
    if (entry != NULL) {
        return entry->value;
    }

    data = lexbor_hash_insert(hash, lexbor_hash_insert_lower, name, length);
    if (data == NULL) {
        return NULL;
    }

    data->attr_id = (uintptr_t) data;

    return data;
}

lxb_dom_attr_data_t *
lxb_dom_attr_qualified_name_append(lexbor_hash_t *hash, const lxb_char_t *name,
                                   size_t length)
{
    lxb_dom_attr_data_t *data;

    if (name == NULL || length == 0) {
        return NULL;
    }

    data = lexbor_hash_insert(hash, lexbor_hash_insert_raw, name, length);
    if (data == NULL) {
        return NULL;
    }

    data->attr_id = (uintptr_t) data;

    return data;
}

const lxb_dom_attr_data_t *
lxb_dom_attr_data_undef(void)
{
    return &lxb_dom_attr_res_data_default[LXB_DOM_ATTR__UNDEF];
}

const lxb_dom_attr_data_t *
lxb_dom_attr_data_by_id(lexbor_hash_t *hash, lxb_dom_attr_id_t attr_id)
{
    if (attr_id >= LXB_DOM_ATTR__LAST_ENTRY) {
        if (attr_id == LXB_DOM_ATTR__LAST_ENTRY) {
            return NULL;
        }

        return (const lxb_dom_attr_data_t *) attr_id;
    }

    return &lxb_dom_attr_res_data_default[attr_id];
}

const lxb_dom_attr_data_t *
lxb_dom_attr_data_by_local_name(lexbor_hash_t *hash,
                                const lxb_char_t *name, size_t length)
{
    const lexbor_shs_entry_t *entry;

    if (name == NULL || length == 0) {
        return NULL;
    }

    entry = lexbor_shs_entry_get_lower_static(lxb_dom_attr_res_shs_data,
                                              name, length);
    if (entry != NULL) {
        return entry->value;
    }

    return lexbor_hash_search(hash, lexbor_hash_search_lower, name, length);
}

const lxb_dom_attr_data_t *
lxb_dom_attr_data_by_qualified_name(lexbor_hash_t *hash,
                                    const lxb_char_t *name, size_t length)
{
    const lexbor_shs_entry_t *entry;

    if (name == NULL || length == 0) {
        return NULL;
    }

    entry = lexbor_shs_entry_get_static(lxb_dom_attr_res_shs_data,
                                        name, length);
    if (entry != NULL) {
        return entry->value;
    }

    return lexbor_hash_search(hash, lexbor_hash_search_raw, name, length);
}

const lxb_char_t *
lxb_dom_attr_qualified_name(lxb_dom_attr_t *attr, size_t *len)
{
    const lxb_dom_attr_data_t *data;

    if (attr->qualified_name != 0) {
        data = lxb_dom_attr_data_by_id(attr->node.owner_document->attrs,
                                       attr->qualified_name);
    }
    else {
        data = lxb_dom_attr_data_by_id(attr->node.owner_document->attrs,
                                       attr->node.local_name);
    }

    if (len != NULL) {
        *len = data->entry.length;
    }

    return lexbor_hash_entry_str(&data->entry);
}

/*
 * No inline functions for ABI.
 */
const lxb_char_t *
lxb_dom_attr_local_name_noi(lxb_dom_attr_t *attr, size_t *len)
{
    return lxb_dom_attr_local_name(attr, len);
}

const lxb_char_t *
lxb_dom_attr_value_noi(lxb_dom_attr_t *attr, size_t *len)
{
    return lxb_dom_attr_value(attr, len);
}
