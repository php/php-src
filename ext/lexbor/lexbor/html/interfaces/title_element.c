/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/interfaces/title_element.h"
#include "lexbor/html/interfaces/document.h"
#include "lexbor/dom/interfaces/text.h"


lxb_html_title_element_t *
lxb_html_title_element_interface_create(lxb_html_document_t *document)
{
    lxb_html_title_element_t *element;

    element = lexbor_mraw_calloc(document->dom_document.mraw,
                                 sizeof(lxb_html_title_element_t));
    if (element == NULL) {
        return NULL;
    }

    lxb_dom_node_t *node = lxb_dom_interface_node(element);

    node->owner_document = lxb_html_document_original_ref(document);
    node->type = LXB_DOM_NODE_TYPE_ELEMENT;

    return element;
}

lxb_html_title_element_t *
lxb_html_title_element_interface_destroy(lxb_html_title_element_t *title)
{
    lexbor_str_t *text;
    lxb_dom_document_t *doc = lxb_dom_interface_node(title)->owner_document;

    text = title->strict_text;

    (void) lxb_dom_node_interface_destroy(lxb_dom_interface_node(title));

    if (text != NULL) {
        lexbor_str_destroy(text, doc->text, false);
        lxb_dom_document_destroy_struct(doc, text);
    }
    
    return NULL;
}

const lxb_char_t *
lxb_html_title_element_text(lxb_html_title_element_t *title, size_t *len)
{
    if (lxb_dom_interface_node(title)->first_child == NULL) {
        goto failed;
    }

    if (lxb_dom_interface_node(title)->first_child->type != LXB_DOM_NODE_TYPE_TEXT) {
        goto failed;
    }

    lxb_dom_text_t *text;

    text = lxb_dom_interface_text(lxb_dom_interface_node(title)->first_child);

    if (len != NULL) {
        *len = text->char_data.data.length;
    }

    return text->char_data.data.data;

failed:

    if (len != NULL) {
        *len = 0;
    }

    return NULL;
}

const lxb_char_t *
lxb_html_title_element_strict_text(lxb_html_title_element_t *title, size_t *len)
{
    const lxb_char_t *text;
    size_t text_len;

    lxb_dom_document_t *doc = lxb_dom_interface_node(title)->owner_document;

    text = lxb_html_title_element_text(title, &text_len);
    if (text == NULL) {
        goto failed;
    }

    if (title->strict_text != NULL) {
        if (title->strict_text->length < text_len) {
            const lxb_char_t *data;

            data = lexbor_str_realloc(title->strict_text,
                                      doc->text, (text_len + 1));
            if (data == NULL) {
                goto failed;
            }
        }
    }
    else {
        title->strict_text = lxb_dom_document_create_struct(doc,
                                                            sizeof(lexbor_str_t));
        if (title->strict_text == NULL) {
            goto failed;
        }

        lexbor_str_init(title->strict_text, doc->text, text_len);
        if (title->strict_text->data == NULL) {
            title->strict_text = lxb_dom_document_destroy_struct(doc,
                                                                 title->strict_text);
            goto failed;
        }
    }

    memcpy(title->strict_text->data, text, sizeof(lxb_char_t) * text_len);

    title->strict_text->data[text_len] = 0x00;
    title->strict_text->length = text_len;

    lexbor_str_strip_collapse_whitespace(title->strict_text);

    if (len != NULL) {
        *len = title->strict_text->length;
    }

    return title->strict_text->data;

failed:

    if (len != NULL) {
        *len = 0;
    }

    return NULL;
}
