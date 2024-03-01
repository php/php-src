/*
 * Copyright (C) 2018-2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/dom/interfaces/element.h"
#include "lexbor/dom/interfaces/attr.h"
#include "lexbor/tag/tag.h"
#include "lexbor/ns/ns.h"

#include "lexbor/core/str.h"
#include "lexbor/core/utils.h"
#include "lexbor/core/hash.h"


static const lxb_char_t *
lxb_dom_element_upper_update(lxb_dom_element_t *element, size_t *len);

const lxb_tag_data_t *
lxb_tag_append(lexbor_hash_t *hash, lxb_tag_id_t tag_id,
               const lxb_char_t *name, size_t length);

const lxb_tag_data_t *
lxb_tag_append_lower(lexbor_hash_t *hash,
                     const lxb_char_t *name, size_t length);

const lxb_ns_data_t *
lxb_ns_append(lexbor_hash_t *hash, const lxb_char_t *link, size_t length);


lxb_dom_element_t *
lxb_dom_element_interface_create(lxb_dom_document_t *document)
{
    lxb_dom_element_t *element;

    element = lexbor_mraw_calloc(document->mraw,
                                 sizeof(lxb_dom_element_t));
    if (element == NULL) {
        return NULL;
    }

    lxb_dom_node_t *node = lxb_dom_interface_node(element);

    node->owner_document = lxb_dom_document_owner(document);
    node->type = LXB_DOM_NODE_TYPE_ELEMENT;

    return element;
}

lxb_dom_element_t *
lxb_dom_element_interface_clone(lxb_dom_document_t *document,
                                const lxb_dom_element_t *element)
{
    lxb_dom_element_t *new;

    new = lxb_dom_element_interface_create(document);
    if (new == NULL) {
        return NULL;
    }

    if (lxb_dom_element_interface_copy(new, element) != LXB_STATUS_OK) {
        return lxb_dom_element_interface_destroy(new);
    }

    return new;
}

lxb_dom_element_t *
lxb_dom_element_interface_destroy(lxb_dom_element_t *element)
{
    lxb_dom_attr_t *attr_next;
    lxb_dom_attr_t *attr = element->first_attr;

    (void) lxb_dom_node_interface_destroy(lxb_dom_interface_node(element));

    while (attr != NULL) {
        attr_next = attr->next;

        lxb_dom_attr_interface_destroy(attr);

        attr = attr_next;
    }

    return NULL;
}

lxb_status_t
lxb_dom_element_interface_copy(lxb_dom_element_t *dst,
                               const lxb_dom_element_t *src)
{
    lxb_status_t status;
    lxb_dom_document_t *document;
    lxb_dom_attr_t *attr, *clone;

    status = lxb_dom_node_interface_copy(&dst->node, &src->node, false);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    document = lxb_dom_interface_node(dst)->owner_document;
    attr = src->first_attr;

    while (attr != NULL) {
        clone = lxb_dom_attr_interface_clone(document, attr);
        if (clone == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }

        (void) lxb_dom_element_attr_append(dst, clone);

        attr = attr->next;
    }

    return LXB_STATUS_OK;
}

LXB_API lxb_status_t
lxb_dom_element_qualified_name_set(lxb_dom_element_t *element,
                                   const lxb_char_t *prefix, size_t prefix_len,
                                   const lxb_char_t *lname, size_t lname_len)
{
    lxb_char_t *key = (lxb_char_t *) lname;
    const lxb_tag_data_t *tag_data;

    if (prefix != NULL && prefix_len != 0) {
        key = lexbor_malloc(prefix_len + lname_len + 2);
        if (key == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }

        memcpy(key, prefix, prefix_len);
        memcpy(&key[prefix_len + 1], lname, lname_len);

        lname_len = prefix_len + lname_len + 1;

        key[prefix_len] = ':';
        key[lname_len] = '\0';
    }

    tag_data = lxb_tag_append(element->node.owner_document->tags,
                              element->node.local_name, key, lname_len);

    if (key != lname) {
        lexbor_free(key);
    }

    if (tag_data == NULL) {
        return LXB_STATUS_ERROR;
    }

    element->qualified_name = (lxb_tag_id_t) tag_data;

    return LXB_STATUS_OK;
}

lxb_dom_element_t *
lxb_dom_element_create(lxb_dom_document_t *document,
                       const lxb_char_t *local_name, size_t lname_len,
                       const lxb_char_t *ns_link, size_t ns_len,
                       const lxb_char_t *prefix, size_t prefix_len,
                       const lxb_char_t *is, size_t is_len,
                       bool sync_custom)
{
    lxb_status_t status;
    const lxb_ns_data_t *ns_data;
    const lxb_tag_data_t *tag_data;
    const lxb_ns_prefix_data_t *ns_prefix;
    lxb_dom_element_t *element;

    /* TODO: Must implement custom elements */

    /* 7. Otherwise */

    ns_data = NULL;
    tag_data = NULL;
    ns_prefix = NULL;

    tag_data = lxb_tag_append_lower(document->tags, local_name, lname_len);
    if (tag_data == NULL) {
        return NULL;
    }

    if (ns_link != NULL) {
        ns_data = lxb_ns_append(document->ns, ns_link, ns_len);
    }
    else {
        ns_data = lxb_ns_data_by_id(document->ns, LXB_NS__UNDEF);
    }

    if (ns_data == NULL) {
        return NULL;
    }

    element = lxb_dom_document_create_interface(document, tag_data->tag_id,
                                                ns_data->ns_id);
    if (element == NULL) {
        return NULL;
    }

    if (prefix != NULL) {
        ns_prefix = lxb_ns_prefix_append(document->prefix, prefix, prefix_len);
        if (ns_prefix == NULL) {
            return lxb_dom_document_destroy_interface(element);
        }

        element->node.prefix = ns_prefix->prefix_id;

        status = lxb_dom_element_qualified_name_set(element, prefix, prefix_len,
                                                    local_name, lname_len);
        if (status != LXB_STATUS_OK) {
            return lxb_dom_document_destroy_interface(element);
        }
    }

    if (is_len != 0) {
        status = lxb_dom_element_is_set(element, is, is_len);
        if (status != LXB_STATUS_OK) {
            return lxb_dom_document_destroy_interface(element);
        }
    }

    element->node.local_name = tag_data->tag_id;
    element->node.ns = ns_data->ns_id;

    if (ns_data->ns_id == LXB_NS_HTML && is_len != 0) {
        element->custom_state = LXB_DOM_ELEMENT_CUSTOM_STATE_UNDEFINED;
    }
    else {
        element->custom_state = LXB_DOM_ELEMENT_CUSTOM_STATE_UNCUSTOMIZED;
    }

    return element;
}

lxb_dom_element_t *
lxb_dom_element_destroy(lxb_dom_element_t *element)
{
    return lxb_dom_document_destroy_interface(element);
}

bool
lxb_dom_element_has_attributes(lxb_dom_element_t *element)
{
    return element->first_attr != NULL;
}

lxb_dom_attr_t *
lxb_dom_element_set_attribute(lxb_dom_element_t *element,
                              const lxb_char_t *qualified_name, size_t qn_len,
                              const lxb_char_t *value, size_t value_len)
{
    lxb_status_t status;
    lxb_dom_attr_t *attr;

    attr = lxb_dom_element_attr_is_exist(element, qualified_name, qn_len);

    if (attr != NULL) {
        status = lxb_dom_attr_set_value(attr, value, value_len);
        if (status != LXB_STATUS_OK) {
            return lxb_dom_attr_interface_destroy(attr);
        }

        return attr;
    }

    attr = lxb_dom_attr_interface_create(element->node.owner_document);
    if (attr == NULL) {
        return NULL;
    }

    attr->node.ns = element->node.ns;

    if (element->node.ns == LXB_NS_HTML
        && element->node.owner_document->type == LXB_DOM_DOCUMENT_DTYPE_HTML)
    {
        status = lxb_dom_attr_set_name(attr, qualified_name, qn_len, true);
    }
    else {
        status = lxb_dom_attr_set_name(attr, qualified_name, qn_len, false);
    }

    if (status != LXB_STATUS_OK) {
        return lxb_dom_attr_interface_destroy(attr);
    }

    status = lxb_dom_attr_set_value(attr, value, value_len);
    if (status != LXB_STATUS_OK) {
        return lxb_dom_attr_interface_destroy(attr);
    }

    lxb_dom_element_attr_append(element, attr);

    return attr;
}

const lxb_char_t *
lxb_dom_element_get_attribute(lxb_dom_element_t *element,
                              const lxb_char_t *qualified_name, size_t qn_len,
                              size_t *value_len)
{
    lxb_dom_attr_t *attr;

    attr = lxb_dom_element_attr_by_name(element, qualified_name, qn_len);
    if (attr == NULL) {
        if (value_len != NULL) {
            *value_len = 0;
        }

        return NULL;
    }

    return lxb_dom_attr_value(attr, value_len);
}

lxb_status_t
lxb_dom_element_remove_attribute(lxb_dom_element_t *element,
                                 const lxb_char_t *qualified_name, size_t qn_len)
{
    lxb_status_t status;
    lxb_dom_attr_t *attr;

    attr = lxb_dom_element_attr_by_name(element, qualified_name, qn_len);
    if (attr == NULL) {
        return LXB_STATUS_OK;
    }

    status = lxb_dom_element_attr_remove(element, attr);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    lxb_dom_attr_interface_destroy(attr);

    return LXB_STATUS_OK;
}

bool
lxb_dom_element_has_attribute(lxb_dom_element_t *element,
                              const lxb_char_t *qualified_name, size_t qn_len)
{
    return lxb_dom_element_attr_by_name(element, qualified_name, qn_len) != NULL;
}

lxb_status_t
lxb_dom_element_attr_append(lxb_dom_element_t *element, lxb_dom_attr_t *attr)
{
    lxb_dom_attr_t *exist;
    lxb_dom_document_t *doc = lxb_dom_interface_node(element)->owner_document;

    if (attr->node.local_name == LXB_DOM_ATTR_ID) {
        exist = element->attr_id;

        if (exist != NULL) {
            lxb_dom_element_attr_remove(element, exist);
            lxb_dom_attr_interface_destroy(exist);
        }

        element->attr_id = attr;
    }
    else if (attr->node.local_name == LXB_DOM_ATTR_CLASS) {
        exist = element->attr_class;

        if (exist != NULL) {
            lxb_dom_element_attr_remove(element, exist);
            lxb_dom_attr_interface_destroy(exist);
        }

        element->attr_class = attr;
    }

    if (element->first_attr == NULL) {
        element->first_attr = attr;
        element->last_attr = attr;

        goto done;
    }

    attr->prev = element->last_attr;

    element->last_attr->next = attr;
    element->last_attr = attr;

done:

    attr->owner = element;

    if (doc->ev_insert != NULL) {
        doc->ev_insert(lxb_dom_interface_node(attr));
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_dom_element_attr_remove(lxb_dom_element_t *element, lxb_dom_attr_t *attr)
{
    (void) element;

    lxb_dom_attr_remove(attr);

    return LXB_STATUS_OK;
}

lxb_dom_attr_t *
lxb_dom_element_attr_by_name(lxb_dom_element_t *element,
                             const lxb_char_t *qualified_name, size_t length)
{
    const lxb_dom_attr_data_t *data;
    lexbor_hash_t *attrs = element->node.owner_document->attrs;
    lxb_dom_attr_t *attr = element->first_attr;

    if (element->node.ns == LXB_NS_HTML
        && element->node.owner_document->type == LXB_DOM_DOCUMENT_DTYPE_HTML)
    {
        data = lxb_dom_attr_data_by_local_name(attrs, qualified_name, length);
    }
    else {
        data = lxb_dom_attr_data_by_qualified_name(attrs, qualified_name,
                                                   length);
    }

    if (data == NULL) {
        return NULL;
    }

    while (attr != NULL) {
        if (attr->node.local_name == data->attr_id
            || attr->qualified_name == data->attr_id)
        {
            return attr;
        }

        attr = attr->next;
    }

    return NULL;
}

lxb_dom_attr_t *
lxb_dom_element_attr_by_local_name_data(lxb_dom_element_t *element,
                                        const lxb_dom_attr_data_t *data)
{
    lxb_dom_attr_t *attr = element->first_attr;

    while (attr != NULL) {
        if (attr->node.local_name == data->attr_id) {
            return attr;
        }

        attr = attr->next;
    }

    return NULL;
}

lxb_dom_attr_t *
lxb_dom_element_attr_by_id(lxb_dom_element_t *element,
                           lxb_dom_attr_id_t attr_id)
{
    lxb_dom_attr_t *attr = element->first_attr;

    while (attr != NULL) {
        if (attr->node.local_name == attr_id) {
            return attr;
        }

        attr = attr->next;
    }

    return NULL;
}

bool
lxb_dom_element_compare(lxb_dom_element_t *first, lxb_dom_element_t *second)
{
    lxb_dom_attr_t *f_attr = first->first_attr;
    lxb_dom_attr_t *s_attr = second->first_attr;

    if (first->node.local_name != second->node.local_name
        || first->node.ns != second->node.ns
        || first->qualified_name != second->qualified_name)
    {
        return false;
    }

    /* Compare attr counts */
    while (f_attr != NULL && s_attr != NULL) {
        f_attr = f_attr->next;
        s_attr = s_attr->next;
    }

    if (f_attr != NULL || s_attr != NULL) {
        return false;
    }

    /* Compare attr */
    f_attr = first->first_attr;

    while (f_attr != NULL) {
        s_attr = second->first_attr;

        while (s_attr != NULL) {
            if (lxb_dom_attr_compare(f_attr, s_attr)) {
                break;
            }

            s_attr = s_attr->next;
        }

        if (s_attr == NULL) {
            return false;
        }

        f_attr = f_attr->next;
    }

    return true;
}

lxb_dom_attr_t *
lxb_dom_element_attr_is_exist(lxb_dom_element_t *element,
                              const lxb_char_t *qualified_name, size_t length)
{
    const lxb_dom_attr_data_t *data;
    lxb_dom_attr_t *attr = element->first_attr;

    data = lxb_dom_attr_data_by_local_name(element->node.owner_document->attrs,
                                           qualified_name, length);
    if (data == NULL) {
        return NULL;
    }

    while (attr != NULL) {
        if (attr->node.local_name == data->attr_id
            || attr->qualified_name == data->attr_id)
        {
            return attr;
        }

        attr = attr->next;
    }

    return NULL;
}

lxb_status_t
lxb_dom_element_is_set(lxb_dom_element_t *element,
                       const lxb_char_t *is, size_t is_len)
{
    if (element->is_value == NULL) {
        element->is_value = lexbor_mraw_calloc(element->node.owner_document->mraw,
                                               sizeof(lexbor_str_t));
        if (element->is_value == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }

    if (element->is_value->data == NULL) {
        lexbor_str_init(element->is_value,
                        element->node.owner_document->text, is_len);

        if (element->is_value->data == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }

    if (element->is_value->length != 0) {
        element->is_value->length = 0;
    }

    lxb_char_t *data = lexbor_str_append(element->is_value,
                                         element->node.owner_document->text,
                                         is, is_len);
    if (data == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_dom_elements_by_tag_name(lxb_dom_element_t *root,
                             lxb_dom_collection_t *collection,
                             const lxb_char_t *qname, size_t len)
{
    return lxb_dom_node_by_tag_name(lxb_dom_interface_node(root),
                                    collection, qname, len);
}

lxb_status_t
lxb_dom_elements_by_class_name(lxb_dom_element_t *root,
                               lxb_dom_collection_t *collection,
                               const lxb_char_t *class_name, size_t len)
{
    return lxb_dom_node_by_class_name(lxb_dom_interface_node(root),
                                      collection, class_name, len);
}

lxb_status_t
lxb_dom_elements_by_attr(lxb_dom_element_t *root,
                         lxb_dom_collection_t *collection,
                         const lxb_char_t *qname, size_t qname_len,
                         const lxb_char_t *value, size_t value_len,
                         bool case_insensitive)
{
    return lxb_dom_node_by_attr(lxb_dom_interface_node(root),
                                collection, qname, qname_len,
                                value, value_len, case_insensitive);
}

lxb_status_t
lxb_dom_elements_by_attr_begin(lxb_dom_element_t *root,
                               lxb_dom_collection_t *collection,
                               const lxb_char_t *qname, size_t qname_len,
                               const lxb_char_t *value, size_t value_len,
                               bool case_insensitive)
{
    return lxb_dom_node_by_attr_begin(lxb_dom_interface_node(root),
                                      collection, qname, qname_len,
                                      value, value_len, case_insensitive);
}

lxb_status_t
lxb_dom_elements_by_attr_end(lxb_dom_element_t *root,
                             lxb_dom_collection_t *collection,
                             const lxb_char_t *qname, size_t qname_len,
                             const lxb_char_t *value, size_t value_len,
                             bool case_insensitive)
{
    return lxb_dom_node_by_attr_end(lxb_dom_interface_node(root),
                                    collection, qname, qname_len,
                                    value, value_len, case_insensitive);
}

lxb_status_t
lxb_dom_elements_by_attr_contain(lxb_dom_element_t *root,
                                 lxb_dom_collection_t *collection,
                                 const lxb_char_t *qname, size_t qname_len,
                                 const lxb_char_t *value, size_t value_len,
                                 bool case_insensitive)
{
    return lxb_dom_node_by_attr_contain(lxb_dom_interface_node(root),
                                        collection, qname, qname_len,
                                        value, value_len, case_insensitive);
}

const lxb_char_t *
lxb_dom_element_qualified_name(lxb_dom_element_t *element, size_t *len)
{
    const lxb_tag_data_t *data;

    if (element->qualified_name != 0) {
        data = lxb_tag_data_by_id(element->node.owner_document->tags,
                                  element->qualified_name);
    }
    else {
        data = lxb_tag_data_by_id(element->node.owner_document->tags,
                                  element->node.local_name);
    }

    if (len != NULL) {
        *len = data->entry.length;
    }

    return lexbor_hash_entry_str(&data->entry);
}

const lxb_char_t *
lxb_dom_element_qualified_name_upper(lxb_dom_element_t *element, size_t *len)
{
    lxb_tag_data_t *data;

    if (element->upper_name == LXB_TAG__UNDEF) {
        return lxb_dom_element_upper_update(element, len);
    }

    data = (lxb_tag_data_t *) element->upper_name;

    if (len != NULL) {
        *len = data->entry.length;
    }

    return lexbor_hash_entry_str(&data->entry);
}

static const lxb_char_t *
lxb_dom_element_upper_update(lxb_dom_element_t *element, size_t *len)
{
    size_t length;
    lxb_tag_data_t *data;
    const lxb_char_t *name;

    if (element->upper_name != LXB_TAG__UNDEF) {
        /* TODO: release current tag data if ref_count == 0. */
        /* data = (lxb_tag_data_t *) element->upper_name; */
    }

    name = lxb_dom_element_qualified_name(element, &length);
    if (name == NULL) {
        return NULL;
    }

    data = lexbor_hash_insert(element->node.owner_document->tags,
                              lexbor_hash_insert_upper, name, length);
    if (data == NULL) {
        return NULL;
    }

    data->tag_id = element->node.local_name;

    if (len != NULL) {
        *len = length;
    }

    element->upper_name = (lxb_tag_id_t) data;

    return lexbor_hash_entry_str(&data->entry);
}

const lxb_char_t *
lxb_dom_element_local_name(lxb_dom_element_t *element, size_t *len)
{
    const lxb_tag_data_t *data;

    data = lxb_tag_data_by_id(element->node.owner_document->tags,
                              element->node.local_name);
    if (data == NULL) {
        if (len != NULL) {
            *len = 0;
        }

        return NULL;
    }

    if (len != NULL) {
        *len = data->entry.length;
    }

    return lexbor_hash_entry_str(&data->entry);
}

const lxb_char_t *
lxb_dom_element_prefix(lxb_dom_element_t *element, size_t *len)
{
    const lxb_ns_prefix_data_t *data;

    if (element->node.prefix == LXB_NS__UNDEF) {
        goto empty;
    }

    data = lxb_ns_prefix_data_by_id(element->node.owner_document->tags,
                                    element->node.prefix);
    if (data == NULL) {
        goto empty;
    }

    return lexbor_hash_entry_str(&data->entry);

empty:

    if (len != NULL) {
        *len = 0;
    }

    return NULL;
}

const lxb_char_t *
lxb_dom_element_tag_name(lxb_dom_element_t *element, size_t *len)
{
    lxb_dom_document_t *doc = lxb_dom_interface_node(element)->owner_document;

    if (element->node.ns != LXB_NS_HTML
        || doc->type != LXB_DOM_DOCUMENT_DTYPE_HTML)
    {
        return lxb_dom_element_qualified_name(element, len);
    }

    return lxb_dom_element_qualified_name_upper(element, len);
}



/*
 * No inline functions for ABI.
 */
const lxb_char_t *
lxb_dom_element_id_noi(lxb_dom_element_t *element, size_t *len)
{
    return lxb_dom_element_id(element, len);
}

const lxb_char_t *
lxb_dom_element_class_noi(lxb_dom_element_t *element, size_t *len)
{
    return lxb_dom_element_class(element, len);
}

bool
lxb_dom_element_is_custom_noi(lxb_dom_element_t *element)
{
    return lxb_dom_element_is_custom(element);
}

bool
lxb_dom_element_custom_is_defined_noi(lxb_dom_element_t *element)
{
    return lxb_dom_element_custom_is_defined(element);
}

lxb_dom_attr_t *
lxb_dom_element_first_attribute_noi(lxb_dom_element_t *element)
{
    return lxb_dom_element_first_attribute(element);
}

lxb_dom_attr_t *
lxb_dom_element_next_attribute_noi(lxb_dom_attr_t *attr)
{
    return lxb_dom_element_next_attribute(attr);
}

lxb_dom_attr_t *
lxb_dom_element_prev_attribute_noi(lxb_dom_attr_t *attr)
{
    return lxb_dom_element_prev_attribute(attr);
}

lxb_dom_attr_t *
lxb_dom_element_last_attribute_noi(lxb_dom_element_t *element)
{
    return lxb_dom_element_last_attribute(element);
}

lxb_dom_attr_t *
lxb_dom_element_id_attribute_noi(lxb_dom_element_t *element)
{
    return lxb_dom_element_id_attribute(element);
}

lxb_dom_attr_t *
lxb_dom_element_class_attribute_noi(lxb_dom_element_t *element)
{
    return lxb_dom_element_class_attribute(element);
}
