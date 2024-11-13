/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_HTML_TREE_ERROR_H
#define LEXBOR_HTML_TREE_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/base.h"
#include "lexbor/core/array_obj.h"

#include "lexbor/html/token.h"


typedef enum {
    /* unexpected-token */
    LXB_HTML_RULES_ERROR_UNTO = 0x0000,
    /* unexpected-closed-token */
    LXB_HTML_RULES_ERROR_UNCLTO,
    /* null-character */
    LXB_HTML_RULES_ERROR_NUCH,
    /* unexpected-character-token */
    LXB_HTML_RULES_ERROR_UNCHTO,
    /* unexpected-token-in-initial-mode */
    LXB_HTML_RULES_ERROR_UNTOININMO,
    /* bad-doctype-token-in-initial-mode */
    LXB_HTML_RULES_ERROR_BADOTOININMO,
    /* doctype-token-in-before-html-mode */
    LXB_HTML_RULES_ERROR_DOTOINBEHTMO,
    /* unexpected-closed-token-in-before-html-mode */
    LXB_HTML_RULES_ERROR_UNCLTOINBEHTMO,
    /* doctype-token-in-before-head-mode */
    LXB_HTML_RULES_ERROR_DOTOINBEHEMO,
    /* unexpected-closed_token-in-before-head-mode */
    LXB_HTML_RULES_ERROR_UNCLTOINBEHEMO,
    /* doctype-token-in-head-mode */
    LXB_HTML_RULES_ERROR_DOTOINHEMO,
    /* non-void-html-element-start-tag-with-trailing-solidus */
    LXB_HTML_RULES_ERROR_NOVOHTELSTTAWITRSO,
    /* head-token-in-head-mode */
    LXB_HTML_RULES_ERROR_HETOINHEMO,
    /* unexpected-closed-token-in-head-mode */
    LXB_HTML_RULES_ERROR_UNCLTOINHEMO,
    /* template-closed-token-without-opening-in-head-mode */
    LXB_HTML_RULES_ERROR_TECLTOWIOPINHEMO,
    /* template-element-is-not-current-in-head-mode */
    LXB_HTML_RULES_ERROR_TEELISNOCUINHEMO,
    /* doctype-token-in-head-noscript-mode */
    LXB_HTML_RULES_ERROR_DOTOINHENOMO,
    /* doctype-token-after-head-mode */
    LXB_HTML_RULES_ERROR_DOTOAFHEMO,
    /* head-token-after-head-mode */
    LXB_HTML_RULES_ERROR_HETOAFHEMO,
    /* doctype-token-in-body-mode */
    LXB_HTML_RULES_ERROR_DOTOINBOMO,
    /* bad-ending-open-elements-is-wrong */
    LXB_HTML_RULES_ERROR_BAENOPELISWR,
    /* open-elements-is-wrong */
    LXB_HTML_RULES_ERROR_OPELISWR,
    /* unexpected-element-in-open-elements-stack */
    LXB_HTML_RULES_ERROR_UNELINOPELST,
    /* missing-element-in-open-elements-stack */
    LXB_HTML_RULES_ERROR_MIELINOPELST,
    /* no-body-element-in-scope */
    LXB_HTML_RULES_ERROR_NOBOELINSC,
    /* missing-element-in-scope */
    LXB_HTML_RULES_ERROR_MIELINSC,
    /* unexpected-element-in-scope */
    LXB_HTML_RULES_ERROR_UNELINSC,
    /* unexpected-element-in-active-formatting-stack */
    LXB_HTML_RULES_ERROR_UNELINACFOST,
    /* unexpected-end-of-file */
    LXB_HTML_RULES_ERROR_UNENOFFI,
    /* characters-in-table-text */
    LXB_HTML_RULES_ERROR_CHINTATE,
    /* doctype-token-in-table-mode */
    LXB_HTML_RULES_ERROR_DOTOINTAMO,
    /* doctype-token-in-select-mode */
    LXB_HTML_RULES_ERROR_DOTOINSEMO,
    /* doctype-token-after-body-mode */
    LXB_HTML_RULES_ERROR_DOTOAFBOMO,
    /* doctype-token-in-frameset-mode */
    LXB_HTML_RULES_ERROR_DOTOINFRMO,
    /* doctype-token-after-frameset-mode */
    LXB_HTML_RULES_ERROR_DOTOAFFRMO,
    /* doctype-token-foreign-content-mode */
    LXB_HTML_RULES_ERROR_DOTOFOCOMO,

    LXB_HTML_RULES_ERROR_LAST_ENTRY
}
lxb_html_tree_error_id_t;

typedef struct {
    lxb_html_tree_error_id_t id;
    size_t                   line;
    size_t                   column;
    size_t                   length;
}
lxb_html_tree_error_t;


LXB_API lxb_html_tree_error_t *
lxb_html_tree_error_add(lexbor_array_obj_t *parse_errors,
                        lxb_html_token_t *token, lxb_html_tree_error_id_t id);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_HTML_TREE_ERROR_H */

