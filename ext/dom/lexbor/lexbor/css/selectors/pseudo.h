/*
 * Copyright (C) 2020-2022 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_CSS_SELECTORS_PSEUDO_H
#define LEXBOR_CSS_SELECTORS_PSEUDO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/css/base.h"
#include "lexbor/css/syntax/parser.h"
#include "lexbor/css/selectors/base.h"


typedef struct {
    lxb_char_t                          *name;
    size_t                              length;
    unsigned                            id;
    bool                                empty;
    lxb_css_selector_combinator_t       combinator;
    const lxb_css_syntax_cb_function_t  cb;
    bool                                forgiving;
    bool                                comma;
}
lxb_css_selectors_pseudo_data_func_t;

typedef struct {
    lxb_char_t *name;
    size_t     length;
    unsigned   id;
}
lxb_css_selectors_pseudo_data_t;


LXB_API const lxb_css_selectors_pseudo_data_t *
lxb_css_selector_pseudo_class_by_name(const lxb_char_t *name, size_t length);

LXB_API const lxb_css_selectors_pseudo_data_func_t *
lxb_css_selector_pseudo_class_function_by_name(const lxb_char_t *name,
                                               size_t length);

LXB_API const lxb_css_selectors_pseudo_data_func_t *
lxb_css_selector_pseudo_class_function_by_id(unsigned id);

LXB_API const lxb_css_selectors_pseudo_data_t *
lxb_css_selector_pseudo_element_by_name(const lxb_char_t *name, size_t length);

LXB_API const lxb_css_selectors_pseudo_data_func_t *
lxb_css_selector_pseudo_element_function_by_name(const lxb_char_t *name,
                                                 size_t length);

LXB_API const lxb_css_selectors_pseudo_data_func_t *
lxb_css_selector_pseudo_element_function_by_id(unsigned id);

LXB_API const lxb_css_selectors_pseudo_data_func_t *
lxb_css_selector_pseudo_function_by_id(unsigned id, bool is_class);

LXB_API bool
lxb_css_selector_pseudo_function_can_empty(unsigned id, bool is_class);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_CSS_SELECTORS_PSEUDO_H */
