/*
 * Copyright (C) 2018-2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_DOM_ELEMENT_H
#define LEXBOR_DOM_ELEMENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/str.h"

#include "lexbor/dom/interfaces/document.h"
#include "lexbor/dom/interfaces/node.h"
#include "lexbor/dom/collection.h"
#include "lexbor/dom/interfaces/attr.h"

#include "lexbor/tag/tag.h"


typedef enum {
    LXB_DOM_ELEMENT_CUSTOM_STATE_UNDEFINED      = 0x00,
    LXB_DOM_ELEMENT_CUSTOM_STATE_FAILED         = 0x01,
    LXB_DOM_ELEMENT_CUSTOM_STATE_UNCUSTOMIZED   = 0x02,
    LXB_DOM_ELEMENT_CUSTOM_STATE_CUSTOM         = 0x03
}
lxb_dom_element_custom_state_t;

struct lxb_dom_element {
    lxb_dom_node_t                 node;

    /* For example: <LalAla:DiV Fix:Me="value"> */

    /* uppercase, with prefix: LALALA:DIV */
    lxb_dom_attr_id_t              upper_name;

    /* original, with prefix: LalAla:DiV */
    lxb_dom_attr_id_t              qualified_name;

    lexbor_str_t                   *is_value;

    lxb_dom_attr_t                 *first_attr;
    lxb_dom_attr_t                 *last_attr;

    lxb_dom_attr_t                 *attr_id;
    lxb_dom_attr_t                 *attr_class;

    lxb_dom_element_custom_state_t custom_state;
};


LXB_API lxb_dom_element_t *
lxb_dom_element_interface_create(lxb_dom_document_t *document);

LXB_API lxb_dom_element_t *
lxb_dom_element_interface_clone(lxb_dom_document_t *document,
                                const lxb_dom_element_t *element);

LXB_API lxb_dom_element_t *
lxb_dom_element_interface_destroy(lxb_dom_element_t *element);

LXB_API lxb_status_t
lxb_dom_element_interface_copy(lxb_dom_element_t *dst,
                               const lxb_dom_element_t *src);

LXB_API lxb_dom_element_t *
lxb_dom_element_create(lxb_dom_document_t *document,
                       const lxb_char_t *local_name, size_t lname_len,
                       const lxb_char_t *ns_name, size_t ns_len,
                       const lxb_char_t *prefix, size_t prefix_len,
                       const lxb_char_t *is, size_t is_len,
                       bool sync_custom);

LXB_API lxb_dom_element_t *
lxb_dom_element_destroy(lxb_dom_element_t *element);

LXB_API bool
lxb_dom_element_has_attributes(lxb_dom_element_t *element);

LXB_API lxb_dom_attr_t *
lxb_dom_element_set_attribute(lxb_dom_element_t *element,
                              const lxb_char_t *qualified_name, size_t qn_len,
                              const lxb_char_t *value, size_t value_len);

LXB_API const lxb_char_t *
lxb_dom_element_get_attribute(lxb_dom_element_t *element,
                              const lxb_char_t *qualified_name, size_t qn_len,
                              size_t *value_len);

LXB_API lxb_status_t
lxb_dom_element_remove_attribute(lxb_dom_element_t *element,
                                 const lxb_char_t *qualified_name, size_t qn_len);

LXB_API bool
lxb_dom_element_has_attribute(lxb_dom_element_t *element,
                              const lxb_char_t *qualified_name, size_t qn_len);

LXB_API lxb_status_t
lxb_dom_element_attr_append(lxb_dom_element_t *element, lxb_dom_attr_t *attr);

LXB_API lxb_status_t
lxb_dom_element_attr_remove(lxb_dom_element_t *element, lxb_dom_attr_t *attr);

LXB_API lxb_dom_attr_t *
lxb_dom_element_attr_by_name(lxb_dom_element_t *element,
                             const lxb_char_t *qualified_name, size_t length);

LXB_API lxb_dom_attr_t *
lxb_dom_element_attr_by_local_name_data(lxb_dom_element_t *element,
                                        const lxb_dom_attr_data_t *data);

LXB_API lxb_dom_attr_t *
lxb_dom_element_attr_by_id(lxb_dom_element_t *element,
                           lxb_dom_attr_id_t attr_id);

LXB_API lxb_dom_attr_t *
lxb_dom_element_attr_by_data(lxb_dom_element_t *element,
                             const lxb_dom_attr_data_t *data);

LXB_API bool
lxb_dom_element_compare(lxb_dom_element_t *first, lxb_dom_element_t *second);

LXB_API lxb_dom_attr_t *
lxb_dom_element_attr_is_exist(lxb_dom_element_t *element,
                              const lxb_char_t *qualified_name, size_t length);

LXB_API lxb_status_t
lxb_dom_element_is_set(lxb_dom_element_t *element,
                       const lxb_char_t *is, size_t is_len);

LXB_API lxb_status_t
lxb_dom_elements_by_tag_name(lxb_dom_element_t *root,
                             lxb_dom_collection_t *collection,
                             const lxb_char_t *qualified_name, size_t len);

LXB_API lxb_status_t
lxb_dom_elements_by_class_name(lxb_dom_element_t *root,
                               lxb_dom_collection_t *collection,
                               const lxb_char_t *class_name, size_t len);

LXB_API lxb_status_t
lxb_dom_elements_by_attr(lxb_dom_element_t *root,
                         lxb_dom_collection_t *collection,
                         const lxb_char_t *qualified_name, size_t qname_len,
                         const lxb_char_t *value, size_t value_len,
                         bool case_insensitive);

LXB_API lxb_status_t
lxb_dom_elements_by_attr_begin(lxb_dom_element_t *root,
                               lxb_dom_collection_t *collection,
                               const lxb_char_t *qualified_name, size_t qname_len,
                               const lxb_char_t *value, size_t value_len,
                               bool case_insensitive);

LXB_API lxb_status_t
lxb_dom_elements_by_attr_end(lxb_dom_element_t *root,
                             lxb_dom_collection_t *collection,
                             const lxb_char_t *qualified_name, size_t qname_len,
                             const lxb_char_t *value, size_t value_len,
                             bool case_insensitive);

LXB_API lxb_status_t
lxb_dom_elements_by_attr_contain(lxb_dom_element_t *root,
                                 lxb_dom_collection_t *collection,
                                 const lxb_char_t *qualified_name, size_t qname_len,
                                 const lxb_char_t *value, size_t value_len,
                                 bool case_insensitive);

LXB_API const lxb_char_t *
lxb_dom_element_qualified_name(lxb_dom_element_t *element, size_t *len);

LXB_API const lxb_char_t *
lxb_dom_element_qualified_name_upper(lxb_dom_element_t *element, size_t *len);

LXB_API const lxb_char_t *
lxb_dom_element_local_name(lxb_dom_element_t *element, size_t *len);

LXB_API const lxb_char_t *
lxb_dom_element_prefix(lxb_dom_element_t *element, size_t *len);

LXB_API const lxb_char_t *
lxb_dom_element_tag_name(lxb_dom_element_t *element, size_t *len);


/*
 * Inline functions
 */
lxb_inline const lxb_char_t *
lxb_dom_element_id(lxb_dom_element_t *element, size_t *len)
{
    if (element->attr_id == NULL) {
        if (len != NULL) {
            *len = 0;
        }

        return NULL;
    }

    return lxb_dom_attr_value(element->attr_id, len);
}

lxb_inline const lxb_char_t *
lxb_dom_element_class(lxb_dom_element_t *element, size_t *len)
{
    if (element->attr_class == NULL) {
        if (len != NULL) {
            *len = 0;
        }

        return NULL;
    }

    return lxb_dom_attr_value(element->attr_class, len);
}

lxb_inline bool
lxb_dom_element_is_custom(lxb_dom_element_t *element)
{
    return element->custom_state & LXB_DOM_ELEMENT_CUSTOM_STATE_CUSTOM;
}

lxb_inline bool
lxb_dom_element_custom_is_defined(lxb_dom_element_t *element)
{
    return element->custom_state & LXB_DOM_ELEMENT_CUSTOM_STATE_CUSTOM
        || element->custom_state & LXB_DOM_ELEMENT_CUSTOM_STATE_UNCUSTOMIZED;
}

lxb_inline lxb_dom_attr_t *
lxb_dom_element_first_attribute(lxb_dom_element_t *element)
{
    return element->first_attr;
}

lxb_inline lxb_dom_attr_t *
lxb_dom_element_next_attribute(lxb_dom_attr_t *attr)
{
    return attr->next;
}

lxb_inline lxb_dom_attr_t *
lxb_dom_element_prev_attribute(lxb_dom_attr_t *attr)
{
    return attr->prev;
}

lxb_inline lxb_dom_attr_t *
lxb_dom_element_last_attribute(lxb_dom_element_t *element)
{
    return element->last_attr;
}

lxb_inline lxb_dom_attr_t *
lxb_dom_element_id_attribute(lxb_dom_element_t *element)
{
    return element->attr_id;
}

lxb_inline lxb_dom_attr_t *
lxb_dom_element_class_attribute(lxb_dom_element_t *element)
{
    return element->attr_class;
}

lxb_inline lxb_tag_id_t
lxb_dom_element_tag_id(lxb_dom_element_t *element)
{
    return lxb_dom_interface_node(element)->local_name;
}

lxb_inline lxb_ns_id_t
lxb_dom_element_ns_id(lxb_dom_element_t *element)
{
    return lxb_dom_interface_node(element)->ns;
}


/*
 * No inline functions for ABI.
 */
LXB_API const lxb_char_t *
lxb_dom_element_id_noi(lxb_dom_element_t *element, size_t *len);

LXB_API const lxb_char_t *
lxb_dom_element_class_noi(lxb_dom_element_t *element, size_t *len);

LXB_API bool
lxb_dom_element_is_custom_noi(lxb_dom_element_t *element);

LXB_API bool
lxb_dom_element_custom_is_defined_noi(lxb_dom_element_t *element);

LXB_API lxb_dom_attr_t *
lxb_dom_element_first_attribute_noi(lxb_dom_element_t *element);

LXB_API lxb_dom_attr_t *
lxb_dom_element_next_attribute_noi(lxb_dom_attr_t *attr);

LXB_API lxb_dom_attr_t *
lxb_dom_element_prev_attribute_noi(lxb_dom_attr_t *attr);

LXB_API lxb_dom_attr_t *
lxb_dom_element_last_attribute_noi(lxb_dom_element_t *element);

LXB_API lxb_dom_attr_t *
lxb_dom_element_id_attribute_noi(lxb_dom_element_t *element);

LXB_API lxb_dom_attr_t *
lxb_dom_element_class_attribute_noi(lxb_dom_element_t *element);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_DOM_ELEMENT_H */
