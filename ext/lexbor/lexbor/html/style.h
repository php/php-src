/*
 * Copyright (C) 2022-2023 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_HTML_STYLE_H
#define LEXBOR_HTML_STYLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/avl.h"
#include "lexbor/css/selectors/selector.h"
#include "lexbor/html/interfaces/document.h"


typedef struct lxb_html_style_weak lxb_html_style_weak_t;

struct lxb_html_style_weak {
    void                           *value;
    lxb_css_selector_specificity_t sp;

    lxb_html_style_weak_t          *next;
};

typedef struct {
    lexbor_avl_node_t              entry;
    lxb_html_style_weak_t          *weak;

    lxb_css_selector_specificity_t sp;
}
lxb_html_style_node_t;


LXB_API uintptr_t
lxb_html_style_id_by_name(lxb_html_document_t *doc,
                          const lxb_char_t *name, size_t size);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_HTML_STYLE_H */
