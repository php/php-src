/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/interfaces/option_element.h"
#include "lexbor/html/interfaces/select_element.h"
#include "lexbor/html/interfaces/selectedcontent_element.h"
#include "lexbor/html/interfaces/document.h"


lxb_html_option_element_t *
lxb_html_option_element_interface_create(lxb_html_document_t *document)
{
    lxb_html_option_element_t *element;

    element = lexbor_mraw_calloc(document->dom_document.mraw,
                                 sizeof(lxb_html_option_element_t));
    if (element == NULL) {
        return NULL;
    }

    lxb_dom_node_t *node = lxb_dom_interface_node(element);

    node->owner_document = lxb_html_document_original_ref(document);
    node->type = LXB_DOM_NODE_TYPE_ELEMENT;

    return element;
}

lxb_html_option_element_t *
lxb_html_option_element_interface_destroy(lxb_html_option_element_t *option_element)
{
    (void) lxb_dom_node_interface_destroy(lxb_dom_interface_node(option_element));
    return NULL;
}

lxb_dom_exception_code_t
lxb_html_option_maybe_clone_to_selectedcontent(lxb_html_option_element_t *option)
{
    bool is;
    lxb_dom_node_t *select;
    lxb_html_select_element_t *select_el;
    lxb_html_selectedcontent_element_t *sel_content;

    is = lxb_html_option_element_selectedness(lxb_html_interface_option(option));
    if (!is) {
        return LXB_DOM_EXCEPTION_OK;
    }

    select = lxb_dom_interface_node(option);

    do {
        select = select->parent;

        if (select == NULL) {
            return LXB_DOM_EXCEPTION_OK;
        }
    }
    while (select->local_name != LXB_TAG_SELECT || select->ns != LXB_NS_HTML);

    select_el = lxb_html_interface_select(select);

    sel_content = lxb_html_select_get_enabled_selectedcontent(select_el);
    if (sel_content == NULL) {
        return LXB_DOM_EXCEPTION_OK;
    }

    return lxb_html_selectedcontent_clone_option(sel_content, option);
}

bool
lxb_html_option_element_selectedness(lxb_html_option_element_t *option)
{
    lxb_dom_attr_t *selected;

    selected = lxb_dom_element_attr_by_id(lxb_dom_interface_element(option),
                                          LXB_DOM_ATTR_SELECTED);

    return selected != NULL;
}
