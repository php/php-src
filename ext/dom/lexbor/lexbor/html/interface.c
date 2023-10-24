/*
 * Copyright (C) 2018-2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/core/mraw.h"

#include "lexbor/html/interface.h"
#include "lexbor/html/interfaces/document.h"

#include "lexbor/dom/interface.h"

#define LXB_HTML_INTERFACE_RES_CONSTRUCTORS
#define LXB_HTML_INTERFACE_RES_DESTRUCTOR
#include "lexbor/html/interface_res.h"


static lxb_dom_element_t *
lxb_html_interface_clone_element(lxb_dom_document_t *document,
                                 const lxb_dom_element_t *element);


lxb_dom_interface_t *
lxb_html_interface_create(lxb_html_document_t *document, lxb_tag_id_t tag_id,
                          lxb_ns_id_t ns)
{
    lxb_dom_node_t *node;

    if (tag_id >= LXB_TAG__LAST_ENTRY) {
        if (ns == LXB_NS_HTML) {
            lxb_html_unknown_element_t *unel;

            unel = lxb_html_unknown_element_interface_create(document);
            node = lxb_dom_interface_node(unel);
        }
        else if (ns == LXB_NS_SVG) {
            /* TODO: For this need implement SVGElement */
            lxb_dom_element_t *domel;

            domel = lxb_dom_element_interface_create(&document->dom_document);
            node = lxb_dom_interface_node(domel);
        }
        else {
            lxb_dom_element_t *domel;

            domel = lxb_dom_element_interface_create(&document->dom_document);
            node = lxb_dom_interface_node(domel);
        }
    }
    else {
        node = lxb_html_interface_res_constructors[tag_id][ns](document);
    }

    if (node == NULL) {
        return NULL;
    }

    node->local_name = tag_id;
    node->ns = ns;

    return node;
}

lxb_dom_interface_t *
lxb_html_interface_clone(lxb_dom_document_t *document,
                         const lxb_dom_interface_t *intrfc)
{
    const lxb_dom_node_t *node = intrfc;

    if (document == NULL) {
        document = node->owner_document;
    }

    switch (node->type) {
        case LXB_DOM_NODE_TYPE_ELEMENT:
            return lxb_html_interface_clone_element(document, intrfc);

        case LXB_DOM_NODE_TYPE_TEXT:
            return lxb_dom_text_interface_clone(document, intrfc);

        case LXB_DOM_NODE_TYPE_PROCESSING_INSTRUCTION:
            return lxb_dom_processing_instruction_interface_clone(document,
                                                                  intrfc);
        case LXB_DOM_NODE_TYPE_COMMENT:
            return lxb_dom_comment_interface_clone(document, intrfc);

        case LXB_DOM_NODE_TYPE_DOCUMENT:
            return lxb_dom_document_interface_clone(document, intrfc);

        case LXB_DOM_NODE_TYPE_DOCUMENT_TYPE:
            return lxb_dom_document_type_interface_clone(document, intrfc);

        default:
            return lxb_dom_node_interface_clone(document, node, false);
    }
}

static lxb_dom_element_t *
lxb_html_interface_clone_element(lxb_dom_document_t *document,
                                 const lxb_dom_element_t *element)
{
    lxb_dom_element_t *new;
    const lxb_dom_node_t *node = lxb_dom_interface_node(element);

    new = lxb_html_interface_create((lxb_html_document_t *) document,
                                    node->local_name, node->ns);
    if (new == NULL) {
        return NULL;
    }

    if (lxb_dom_element_interface_copy(new, element) != LXB_STATUS_OK) {
        return lxb_dom_element_interface_destroy(new);
    }

    return new;
}

lxb_dom_interface_t *
lxb_html_interface_destroy(lxb_dom_interface_t *intrfc)
{
    if (intrfc == NULL) {
        return NULL;
    }

    lxb_dom_node_t *node = intrfc;

    switch (node->type) {
        case LXB_DOM_NODE_TYPE_TEXT:
        case LXB_DOM_NODE_TYPE_COMMENT:
        case LXB_DOM_NODE_TYPE_ELEMENT:
        case LXB_DOM_NODE_TYPE_DOCUMENT:
        case LXB_DOM_NODE_TYPE_DOCUMENT_TYPE:
            if (node->local_name >= LXB_TAG__LAST_ENTRY) {
                if (node->ns == LXB_NS_HTML) {
                    return lxb_html_unknown_element_interface_destroy(intrfc);
                }
                else if (node->ns == LXB_NS_SVG) {
                    /* TODO: For this need implement SVGElement */
                    return lxb_dom_element_interface_destroy(intrfc);
                }
                else {
                    return lxb_dom_element_interface_destroy(intrfc);
                }
            }
            else {
                return lxb_html_interface_res_destructor[node->local_name][node->ns](intrfc);
            }

        case LXB_DOM_NODE_TYPE_ATTRIBUTE:
            return lxb_dom_attr_interface_destroy(intrfc);

        case LXB_DOM_NODE_TYPE_CDATA_SECTION:
            return lxb_dom_cdata_section_interface_destroy(intrfc);

        case LXB_DOM_NODE_TYPE_DOCUMENT_FRAGMENT:
            return lxb_dom_document_fragment_interface_destroy(intrfc);

        case LXB_DOM_NODE_TYPE_PROCESSING_INSTRUCTION:
            return lxb_dom_processing_instruction_interface_destroy(intrfc);

        default:
            return NULL;
    }
}
