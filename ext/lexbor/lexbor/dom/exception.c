/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/dom/exception.h"


/*
 * No inline functions for ABI.
 */
void *
lxb_dom_exception_code_ref_set_noi(lxb_dom_exception_code_t *var,
                                   lxb_dom_exception_code_t code)
{
    return lxb_dom_exception_code_ref_set(var, code);
}
