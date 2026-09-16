/*
 * Copyright (C) 2020-2022 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/css/css.h"
#include "lexbor/css/selectors/state.h"
#include "lexbor/css/selectors/pseudo.h"
#include "lexbor/css/selectors/pseudo_state.h"
#include "lexbor/css/selectors/pseudo_res.h"


const lxb_css_selectors_pseudo_data_t *
lxb_css_selector_pseudo_class_by_name(const lxb_char_t *name, size_t length)
{
    const lexbor_shs_entry_t *entry;

    entry = lexbor_shs_entry_get_lower_static(lxb_css_selectors_pseudo_class_shs,
                                              name, length);
    if (entry == NULL) {
        return NULL;
    }

    return entry->value;
}

const lxb_css_selectors_pseudo_data_func_t *
lxb_css_selector_pseudo_class_function_by_name(const lxb_char_t *name,
                                               size_t length)
{
    const lexbor_shs_entry_t *entry;

    entry = lexbor_shs_entry_get_lower_static(lxb_css_selectors_pseudo_class_function_shs,
                                              name, length);
    if (entry == NULL) {
        return NULL;
    }

    return entry->value;
}

const lxb_css_selectors_pseudo_data_func_t *
lxb_css_selector_pseudo_class_function_by_id(unsigned id)
{
    return &lxb_css_selectors_pseudo_data_pseudo_class_function[id];
}

const lxb_css_selectors_pseudo_data_t *
lxb_css_selector_pseudo_element_by_name(const lxb_char_t *name, size_t length)
{
    const lexbor_shs_entry_t *entry;

    entry = lexbor_shs_entry_get_lower_static(lxb_css_selectors_pseudo_element_shs,
                                              name, length);
    if (entry == NULL) {
        return NULL;
    }

    return entry->value;
}

const lxb_css_selectors_pseudo_data_func_t *
lxb_css_selector_pseudo_element_function_by_name(const lxb_char_t *name,
                                                 size_t length)
{
    const lexbor_shs_entry_t *entry;

    entry = lexbor_shs_entry_get_lower_static(lxb_css_selectors_pseudo_element_function_shs,
                                              name, length);
    if (entry == NULL) {
        return NULL;
    }

    return entry->value;
}

const lxb_css_selectors_pseudo_data_func_t *
lxb_css_selector_pseudo_element_function_by_id(unsigned id)
{
    return &lxb_css_selectors_pseudo_data_pseudo_element_function[id];
}

const lxb_css_selectors_pseudo_data_func_t *
lxb_css_selector_pseudo_function_by_id(unsigned id, bool is_class)
{
    if (is_class) {
        return &lxb_css_selectors_pseudo_data_pseudo_class_function[id];
    }

    return &lxb_css_selectors_pseudo_data_pseudo_element_function[id];
}

bool
lxb_css_selector_pseudo_function_can_empty(unsigned id, bool is_class)
{
    if (is_class) {
        return lxb_css_selectors_pseudo_data_pseudo_class_function[id].empty;
    }

    return lxb_css_selectors_pseudo_data_pseudo_element_function[id].empty;
}
