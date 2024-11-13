/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/interfaces/style_element.h"
#include "lexbor/html/interfaces/document.h"


lxb_html_style_element_t *
lxb_html_style_element_interface_create(lxb_html_document_t *document)
{
    lxb_html_style_element_t *element;

    element = lexbor_mraw_calloc(document->dom_document.mraw,
                                 sizeof(lxb_html_style_element_t));
    if (element == NULL) {
        return NULL;
    }

    lxb_dom_node_t *node = lxb_dom_interface_node(element);

    node->owner_document = lxb_html_document_original_ref(document);
    node->type = LXB_DOM_NODE_TYPE_ELEMENT;

    return element;
}

lxb_html_style_element_t *
lxb_html_style_element_interface_destroy(lxb_html_style_element_t *style_element)
{
    lxb_css_stylesheet_t *sst = style_element->stylesheet;

    (void) lxb_dom_node_interface_destroy(lxb_dom_interface_node(style_element));

    if (sst != NULL) {
#if 0
        (void) lxb_css_stylesheet_destroy(sst, true);
#endif
    }

    return NULL;
}

lxb_status_t
lxb_html_style_element_parse(lxb_html_style_element_t *element)
{
#if 0
    lexbor_str_t *str;
    lxb_dom_text_t *text;
#endif
    lxb_dom_node_t *node;

    lxb_dom_document_t *ddoc = lxb_dom_interface_node(element)->owner_document;
    lxb_html_document_t *doc = lxb_html_interface_document(ddoc);
    lxb_html_document_css_t *css = &doc->css;

    node = lxb_dom_interface_node(element);

    if (node->first_child == NULL
        || node->first_child->local_name != LXB_TAG__TEXT
        || node->first_child != node->last_child)
    {
        return LXB_STATUS_OK;
    }

#if 0
    text = lxb_dom_interface_text(lxb_dom_interface_node(element)->first_child);
    str = &text->char_data.data;

    element->stylesheet = lxb_css_stylesheet_parse(css->parser, str->data,
                                                   str->length);
#endif
    if (element->stylesheet == NULL) {
        return css->parser->status;
    }

    element->stylesheet->element = element;

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_html_element_style_remove(lxb_html_style_element_t *style)
{
    lxb_dom_document_t *ddoc = lxb_dom_interface_node(style)->owner_document;
    lxb_html_document_t *doc = lxb_html_interface_document(ddoc);

    lxb_html_document_stylesheet_remove(doc, style->stylesheet);

    return LXB_STATUS_OK;
}
