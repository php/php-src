/*
 * Copyright (C) 2018-2022 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/interfaces/element.h"
#include "lexbor/html/interfaces/document.h"


lxb_html_element_t *
lxb_html_element_interface_create(lxb_html_document_t *document)
{
    lxb_html_element_t *element;

    element = lexbor_mraw_calloc(document->dom_document.mraw,
                                 sizeof(lxb_html_element_t));
    if (element == NULL) {
        return NULL;
    }

    lxb_dom_node_t *node = lxb_dom_interface_node(element);

    node->owner_document = lxb_html_document_original_ref(document);
    node->type = LXB_DOM_NODE_TYPE_ELEMENT;

    return element;
}

lxb_html_element_t *
lxb_html_element_interface_destroy(lxb_html_element_t *element)
{
    (void) lxb_dom_node_interface_destroy(lxb_dom_interface_node(element));
    return NULL;
}

lxb_html_element_t *
lxb_html_element_inner_html_set(lxb_html_element_t *element,
                                const lxb_char_t *html, size_t size)
{
    lxb_dom_node_t *node, *child;
    lxb_dom_node_t *root = lxb_dom_interface_node(element);
    lxb_html_document_t *doc = lxb_html_interface_document(root->owner_document);

    node = lxb_html_document_parse_fragment(doc, &element->element, html, size);
    if (node == NULL) {
        return NULL;
    }

    while (root->first_child != NULL) {
        lxb_dom_node_destroy_deep(root->first_child);
    }

    while (node->first_child != NULL) {
        child = node->first_child;

        lxb_dom_node_remove(child);
        lxb_dom_node_insert_child(root, child);
    }

    lxb_dom_node_destroy(node);

    return lxb_html_interface_element(root);
}
