/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_DOM_EXCEPTION_H
#define LEXBOR_DOM_EXCEPTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/base.h"


typedef enum {
    LXB_DOM_INDEX_SIZE_ERR = 0x00,
    LXB_DOM_DOMSTRING_SIZE_ERR,
    LXB_DOM_HIERARCHY_REQUEST_ERR,
    LXB_DOM_WRONG_DOCUMENT_ERR,
    LXB_DOM_INVALID_CHARACTER_ERR,
    LXB_DOM_NO_DATA_ALLOWED_ERR,
    LXB_DOM_NO_MODIFICATION_ALLOWED_ERR,
    LXB_DOM_NOT_FOUND_ERR,
    LXB_DOM_NOT_SUPPORTED_ERR,
    LXB_DOM_INUSE_ATTRIBUTE_ERR,
    LXB_DOM_INVALID_STATE_ERR,
    LXB_DOM_SYNTAX_ERR,
    LXB_DOM_INVALID_MODIFICATION_ERR,
    LXB_DOM_NAMESPACE_ERR,
    LXB_DOM_INVALID_ACCESS_ERR,
    LXB_DOM_VALIDATION_ERR,
    LXB_DOM_TYPE_MISMATCH_ERR,
    LXB_DOM_SECURITY_ERR,
    LXB_DOM_NETWORK_ERR,
    LXB_DOM_ABORT_ERR,
    LXB_DOM_URL_MISMATCH_ERR,
    LXB_DOM_QUOTA_EXCEEDED_ERR,
    LXB_DOM_TIMEOUT_ERR,
    LXB_DOM_INVALID_NODE_TYPE_ERR,
    LXB_DOM_DATA_CLONE_ERR
}
lxb_dom_exception_code_t;


/*
 * Inline functions
 */
lxb_inline void *
lxb_dom_exception_code_ref_set(lxb_dom_exception_code_t *var,
                               lxb_dom_exception_code_t code)
{
    if (var != NULL) {
        *var = code;
    }

    return NULL;
}

/*
 * No inline functions for ABI.
 */
LXB_API void *
lxb_dom_exception_code_ref_set_noi(lxb_dom_exception_code_t *var,
                                   lxb_dom_exception_code_t code);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_DOM_EXCEPTION_H */
