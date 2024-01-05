/*
 * Copyright (C) 2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 * Adapted for PHP libxml2 by: Niels Dossche <nielsdos@php.net>
 */


#ifndef LEXBOR_SELECTORS_H
#define LEXBOR_SELECTORS_H

#include "lexbor/selectors/base.h"
#include "lexbor/dom/dom.h"
#include "lexbor/css/selectors/selectors.h"
#include <libxml/tree.h>


typedef lxb_status_t
(*lxb_selectors_cb_f)(const xmlNode *node, lxb_css_selector_specificity_t spec,
                      void *ctx);

typedef struct lxb_selectors_entry lxb_selectors_entry_t;
typedef struct lxb_selectors_entry_child lxb_selectors_entry_child_t;

typedef struct {
    lexbor_dobject_t *objs;
    lexbor_dobject_t *chld;

    lxb_status_t     status;
}
lxb_selectors_t;

typedef struct {
    const xmlChar *name;
} lxb_selectors_adapted_id;

struct lxb_selectors_entry {
    lxb_selectors_adapted_id     id;
    lxb_css_selector_t          *selector;
    const xmlNode               *node;
    lxb_selectors_entry_t       *next;
    lxb_selectors_entry_t       *prev;
    lxb_selectors_entry_child_t *child;
};

struct lxb_selectors_entry_child {
    lxb_selectors_entry_t       *entry;
    lxb_selectors_entry_child_t *next;
};


LXB_API lxb_status_t
lxb_selectors_init(lxb_selectors_t *selectors);

LXB_API void
lxb_selectors_clean(lxb_selectors_t *selectors);

LXB_API void
lxb_selectors_destroy(lxb_selectors_t *selectors);

LXB_API lxb_status_t
lxb_selectors_find(lxb_selectors_t *selectors, const xmlNode *root,
                   lxb_css_selector_list_t *list, lxb_selectors_cb_f cb, void *ctx);

#endif /* LEXBOR_SELECTORS_H */
