/*
 * Copyright (C) 2018-2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/dom/interface.h"
#include "lexbor/dom/interfaces/cdata_section.h"
#include "lexbor/dom/interfaces/character_data.h"
#include "lexbor/dom/interfaces/comment.h"
#include "lexbor/dom/interfaces/document.h"
#include "lexbor/dom/interfaces/document_fragment.h"
#include "lexbor/dom/interfaces/document_type.h"
#include "lexbor/dom/interfaces/element.h"
#include "lexbor/dom/interfaces/event_target.h"
#include "lexbor/dom/interfaces/node.h"
#include "lexbor/dom/interfaces/processing_instruction.h"
#include "lexbor/dom/interfaces/shadow_root.h"
#include "lexbor/dom/interfaces/text.h"


lxb_dom_interface_t *
lxb_dom_interface_create(lxb_dom_document_t *document, lxb_tag_id_t tag_id,
                         lxb_ns_id_t ns)
{
    lxb_dom_element_t *domel;

    domel = lxb_dom_element_interface_create(document);
    if (domel == NULL) {
        return NULL;
    }

    domel->node.local_name = tag_id;
    domel->node.ns = ns;

    return domel;
}

lxb_dom_interface_t *
lxb_dom_interface_clone(lxb_dom_document_t *document,
                        const lxb_dom_interface_t *intrfc)
{
    const lxb_dom_node_t *node = intrfc;

    if (document == NULL) {
        document = node->owner_document;
    }

    switch (node->type) {
        case LXB_DOM_NODE_TYPE_ELEMENT:
            return lxb_dom_element_interface_clone(document, intrfc);

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

lxb_dom_interface_t *
lxb_dom_interface_destroy(lxb_dom_interface_t *intrfc)
{
    if (intrfc == NULL) {
        return NULL;
    }

    lxb_dom_node_t *node = intrfc;

    switch (node->type) {
        case LXB_DOM_NODE_TYPE_ELEMENT:
            return lxb_dom_element_interface_destroy(intrfc);

        case LXB_DOM_NODE_TYPE_TEXT:
            return lxb_dom_text_interface_destroy(intrfc);

        case LXB_DOM_NODE_TYPE_CDATA_SECTION:
            return lxb_dom_cdata_section_interface_destroy(intrfc);

        case LXB_DOM_NODE_TYPE_PROCESSING_INSTRUCTION:
            return lxb_dom_processing_instruction_interface_destroy(intrfc);

        case LXB_DOM_NODE_TYPE_COMMENT:
            return lxb_dom_comment_interface_destroy(intrfc);

        case LXB_DOM_NODE_TYPE_DOCUMENT:
            return lxb_dom_document_interface_destroy(intrfc);

        case LXB_DOM_NODE_TYPE_DOCUMENT_TYPE:
            return lxb_dom_document_type_interface_destroy(intrfc);

        case LXB_DOM_NODE_TYPE_DOCUMENT_FRAGMENT:
            return lxb_dom_document_fragment_interface_destroy(intrfc);

        default:
            return lexbor_mraw_free(node->owner_document->mraw, intrfc);
    }
}
