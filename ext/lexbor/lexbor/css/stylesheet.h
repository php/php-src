/*
 * Copyright (C) 2020-2026 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LXB_CSS_STYLESHEET_H
#define LXB_CSS_STYLESHEET_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/css/rule.h"
#include "lexbor/core/mraw.h"
#include "lexbor/css/at_rule.h"
#include "lexbor/css/syntax/tokenizer.h"
#include "lexbor/css/selectors/base.h"


struct lxb_css_stylesheet {
    lxb_css_rule_t           *root;
    lxb_css_memory_t         *memory;

    void                     *element; /* lxb_html_style_element_t * */
};

/*
 * Create a new CSS stylesheet object.
 *
 * This function creates a stylesheet object which holds the parsed CSS rules.
 *
 * @param[in] memory  Optional. A memory pool to use for allocations.
 *                    If NULL, a new internal memory pool is created/managed
 *                    by the stylesheet.
 *
 * @return A new lxb_css_stylesheet_t * or NULL on failure.
 */
LXB_API lxb_css_stylesheet_t *
lxb_css_stylesheet_create(lxb_css_memory_t *memory);

/*
 * Destroy a CSS stylesheet object.
 *
 * @param[in] sst             Optional. The stylesheet object to destroy.
 *                            If NULL, the function returns NULL.
 * @param[in] destroy_memory  If true, the memory pool attached to
 *                            the stylesheet is also destroyed.
 *
 * @return Always NULL.
 */
LXB_API lxb_css_stylesheet_t *
lxb_css_stylesheet_destroy(lxb_css_stylesheet_t *sst, bool destroy_memory);

/*
 * Parse CSS content into the stylesheet.
 *
 * This function takes CSS text and builds the rule tree in the stylesheet.
 *
 * The function returns an error only in extremely unforeseen circumstances,
 * such as the inability to allocate memory for objects. Any broken CSS will
 * not cause an error.
 *
 * Selectors note:
 *   If the provided parser does not have an initialized selectors module, one
 *   will be created temporarily for this call. For better performance when
 *   parsing multiple stylesheets, initialize the selectors module in
 *   the parser once:
 *
 *     lxb_css_parser_t *parser = lxb_css_parser_create();
 *     lxb_css_parser_init(parser, NULL);
 *     lxb_css_parser_selectors_init(parser);
 *
 * @param[in] sst     Required. The target stylesheet.
 * @param[in] parser  Required. An initialized CSS parser.
 * @param[in] data    Optional. The CSS text data.
 * @param[in] length  Required. Length of the data in bytes.
 *
 * @return LXB_STATUS_OK on success, or an error code on failure.
 */
LXB_API lxb_status_t
lxb_css_stylesheet_parse(lxb_css_stylesheet_t *sst, lxb_css_parser_t *parser,
                         const lxb_char_t *data, size_t length);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LXB_CSS_STYLESHEET_H */
