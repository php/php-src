/*
 * Copyright (C) 2018-2025 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/interfaces/select_element.h"
#include "lexbor/html/interfaces/document.h"


static lexbor_action_t
lxb_html_select_find_selectedcontent_cb(lxb_dom_node_t *node, void *ctx);


lxb_html_select_element_t *
lxb_html_select_element_interface_create(lxb_html_document_t *document)
{
    lxb_html_select_element_t *element;

    element = lexbor_mraw_calloc(document->dom_document.mraw,
                                 sizeof(lxb_html_select_element_t));
    if (element == NULL) {
        return NULL;
    }

    lxb_dom_node_t *node = lxb_dom_interface_node(element);

    node->owner_document = lxb_html_document_original_ref(document);
    node->type = LXB_DOM_NODE_TYPE_ELEMENT;

    return element;
}

lxb_html_select_element_t *
lxb_html_select_element_interface_destroy(lxb_html_select_element_t *select_element)
{
    (void) lxb_dom_node_interface_destroy(lxb_dom_interface_node(select_element));
    return NULL;
}

lxb_html_selectedcontent_element_t *
lxb_html_select_get_enabled_selectedcontent(lxb_html_select_element_t *el)
{
    lxb_dom_attr_t *multiple;
    lxb_dom_node_t *selectedcontent, *node;

    multiple = lxb_dom_element_attr_by_id(lxb_dom_interface_element(el),
                                          LXB_DOM_ATTR_MULTIPLE);
    if (multiple != NULL) {
        return NULL;
    }

    selectedcontent = NULL;
    node = lxb_dom_interface_node(el);

    lxb_dom_node_simple_walk(node, lxb_html_select_find_selectedcontent_cb,
                             &selectedcontent);
    if (selectedcontent == NULL) {
        return NULL;
    }

    return lxb_html_interface_selectedcontent(selectedcontent);
}

static lexbor_action_t
lxb_html_select_find_selectedcontent_cb(lxb_dom_node_t *node, void *ctx)
{
    lxb_dom_node_t **selectedcontent;

    if (node->local_name == LXB_TAG_SELECTEDCONTENT
        && node->ns == LXB_NS_HTML)
    {
        selectedcontent = ctx;
        *selectedcontent = node;

        return LEXBOR_ACTION_STOP;
    }

    return LEXBOR_ACTION_OK;
}
