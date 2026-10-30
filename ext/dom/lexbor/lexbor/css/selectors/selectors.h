/*
 * Copyright (C) 2020-2022 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_CSS_SELECTORS_H
#define LEXBOR_CSS_SELECTORS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/dobject.h"
#include "lexbor/css/base.h"
#include "lexbor/css/syntax/parser.h"
#include "lexbor/css/syntax/syntax.h"
#include "lexbor/css/selectors/base.h"
#include "lexbor/css/selectors/selector.h"
#include "lexbor/css/selectors/pseudo_const.h"


struct lxb_css_selectors {
    lxb_css_selector_list_t       *list;
    lxb_css_selector_list_t       *list_last;

    lxb_css_selector_t            *parent;

    lxb_css_selector_combinator_t combinator;
    lxb_css_selector_combinator_t comb_default;

    uintptr_t                     error;
    bool                          status;
    bool                          err_in_function;
    bool                          failed;
};


LXB_API lxb_css_selectors_t *
lxb_css_selectors_create(void);

LXB_API lxb_status_t
lxb_css_selectors_init(lxb_css_selectors_t *selectors);

LXB_API void
lxb_css_selectors_clean(lxb_css_selectors_t *selectors);

LXB_API lxb_css_selectors_t *
lxb_css_selectors_destroy(lxb_css_selectors_t *selectors, bool self_destroy);

LXB_API lxb_css_selector_list_t *
lxb_css_selectors_parse(lxb_css_parser_t *parser,
                        const lxb_char_t *data, size_t length);

LXB_API lxb_css_selector_list_t *
lxb_css_selectors_parse_complex_list(lxb_css_parser_t *parser,
                                     const lxb_char_t *data, size_t length);

LXB_API lxb_css_selector_list_t *
lxb_css_selectors_parse_compound_list(lxb_css_parser_t *parser,
                                      const lxb_char_t *data, size_t length);

LXB_API lxb_css_selector_list_t *
lxb_css_selectors_parse_simple_list(lxb_css_parser_t *parser,
                                    const lxb_char_t *data, size_t length);

LXB_API lxb_css_selector_list_t *
lxb_css_selectors_parse_relative_list(lxb_css_parser_t *parser,
                                      const lxb_char_t *data, size_t length);

LXB_API lxb_css_selector_list_t *
lxb_css_selectors_parse_complex(lxb_css_parser_t *parser,
                                const lxb_char_t *data, size_t length);

LXB_API lxb_css_selector_list_t *
lxb_css_selectors_parse_compound(lxb_css_parser_t *parser,
                                 const lxb_char_t *data, size_t length);

LXB_API lxb_css_selector_list_t *
lxb_css_selectors_parse_simple(lxb_css_parser_t *parser,
                               const lxb_char_t *data, size_t length);

LXB_API lxb_css_selector_list_t *
lxb_css_selectors_parse_relative(lxb_css_parser_t *parser,
                                 const lxb_char_t *data, size_t length);

/*
 * Inline functions
 */
lxb_inline void
lxb_css_selectors_append_next(lxb_css_selectors_t *selectors,
                              lxb_css_selector_t *selector)
{
    if (selectors->list_last->last != NULL) {
        lxb_css_selector_append_next(selectors->list_last->last, selector);
    }
    else {
        selectors->list_last->first = selector;
    }

    selectors->list_last->last = selector;
}

lxb_inline void
lxb_css_selectors_list_append_next(lxb_css_selectors_t *selectors,
                                   lxb_css_selector_list_t *list)
{
    if (selectors->list_last != NULL) {
        lxb_css_selector_list_append_next(selectors->list_last, list);
    }
    else {
        selectors->list = list;
    }

    selectors->list_last = list;
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_CSS_SELECTORS_H */
