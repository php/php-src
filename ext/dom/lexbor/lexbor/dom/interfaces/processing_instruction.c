/*
 * Copyright (C) 2018-2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/dom/interfaces/processing_instruction.h"
#include "lexbor/dom/interfaces/document.h"


lxb_dom_processing_instruction_t *
lxb_dom_processing_instruction_interface_create(lxb_dom_document_t *document)
{
    lxb_dom_processing_instruction_t *element;

    element = lexbor_mraw_calloc(document->mraw,
                                 sizeof(lxb_dom_processing_instruction_t));
    if (element == NULL) {
        return NULL;
    }

    lxb_dom_node_t *node = lxb_dom_interface_node(element);

    node->owner_document = lxb_dom_document_owner(document);
    node->type = LXB_DOM_NODE_TYPE_PROCESSING_INSTRUCTION;

    return element;
}

lxb_dom_processing_instruction_t *
lxb_dom_processing_instruction_interface_clone(lxb_dom_document_t *document,
                                               const lxb_dom_processing_instruction_t *pinstr)
{
    lxb_status_t status;
    lxb_dom_processing_instruction_t *new;

    new = lxb_dom_processing_instruction_interface_create(document);
    if (new == NULL) {
        return NULL;
    }

    status = lxb_dom_processing_instruction_copy(new, pinstr);
    if (status != LXB_STATUS_OK) {
        return lxb_dom_processing_instruction_interface_destroy(new);
    }

    return new;
}

lxb_dom_processing_instruction_t *
lxb_dom_processing_instruction_interface_destroy(lxb_dom_processing_instruction_t *processing_instruction)
{
    lexbor_mraw_t *text;
    lexbor_str_t target;

    text = lxb_dom_interface_node(processing_instruction)->owner_document->text;
    target = processing_instruction->target;

    (void) lxb_dom_character_data_interface_destroy(
                      lxb_dom_interface_character_data(processing_instruction));

    (void) lexbor_str_destroy(&target, text, false);

    return NULL;
}

lxb_status_t
lxb_dom_processing_instruction_copy(lxb_dom_processing_instruction_t *dst,
                                    const lxb_dom_processing_instruction_t *src)
{
    dst->target.length = 0;

    if (lexbor_str_copy(&dst->target, &src->target,
                        lxb_dom_interface_node(dst)->owner_document->text) == NULL)
    {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    return lxb_dom_character_data_interface_copy(&dst->char_data,
                                                 &src->char_data);
}


/*
 * No inline functions for ABI.
 */
const lxb_char_t *
lxb_dom_processing_instruction_target_noi(lxb_dom_processing_instruction_t *pi,
                                          size_t *len)
{
    return lxb_dom_processing_instruction_target(pi, len);
}
