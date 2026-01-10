/*
 * Copyright (C) 2018-2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_DOM_PROCESSING_INSTRUCTION_H
#define LEXBOR_DOM_PROCESSING_INSTRUCTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/dom/interfaces/document.h"
#include "lexbor/dom/interfaces/text.h"


struct lxb_dom_processing_instruction {
    lxb_dom_character_data_t char_data;

    lexbor_str_t             target;
};


LXB_API lxb_dom_processing_instruction_t *
lxb_dom_processing_instruction_interface_create(lxb_dom_document_t *document);

LXB_API lxb_dom_processing_instruction_t *
lxb_dom_processing_instruction_interface_clone(lxb_dom_document_t *document,
                                               const lxb_dom_processing_instruction_t *processing_instruction);

LXB_API lxb_dom_processing_instruction_t *
lxb_dom_processing_instruction_interface_destroy(lxb_dom_processing_instruction_t *processing_instruction);


LXB_API lxb_status_t
lxb_dom_processing_instruction_copy(lxb_dom_processing_instruction_t *dst,
                                    const lxb_dom_processing_instruction_t *src);

/*
 * Inline functions
 */
lxb_inline const lxb_char_t *
lxb_dom_processing_instruction_target(lxb_dom_processing_instruction_t *pi,
                                      size_t *len)
{
    if (len != NULL) {
        *len = pi->target.length;
    }

    return pi->target.data;
}

/*
 * No inline functions for ABI.
 */
LXB_API const lxb_char_t *
lxb_dom_processing_instruction_target_noi(lxb_dom_processing_instruction_t *pi,
                                          size_t *len);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_DOM_PROCESSING_INSTRUCTION_H */
