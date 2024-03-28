/*
 * Copyright (C) 2018-2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/core/str.h"

#include "lexbor/html/interfaces/document.h"
#include "lexbor/html/interfaces/title_element.h"
#include "lexbor/html/interfaces/style_element.h"
#include "lexbor/html/node.h"
#include "lexbor/html/parser.h"
#include "lexbor/html/style.h"

#include "lexbor/tag/tag.h"

#include "lexbor/dom/interfaces/text.h"
#include "lexbor/dom/interfaces/element.h"

#define LXB_HTML_TAG_RES_DATA
#define LXB_HTML_TAG_RES_SHS_DATA
#include "lexbor/html/tag_res.h"


static const lexbor_hash_search_t  lxb_html_document_css_customs_se = {
    .cmp = lexbor_str_data_ncasecmp,
    .hash = lexbor_hash_make_id
};

static const lexbor_hash_insert_t  lxb_html_document_css_customs_in = {
    .copy = lexbor_hash_copy,
    .cmp = lexbor_str_data_ncasecmp,
    .hash = lexbor_hash_make_id
};


typedef struct {
    lexbor_hash_entry_t entry;
    uintptr_t           id;
}
lxb_html_document_css_custom_entry_t;

typedef struct {
    lxb_html_document_t *doc;
    bool                all;
}
lxb_html_document_event_ctx_t;

typedef struct {
    lxb_html_document_t             *doc;
    lxb_css_rule_declaration_list_t *list;
}
lxb_html_document_remove_ctx_t;


static lxb_html_document_css_custom_entry_t *
lxb_html_document_css_customs_insert(lxb_html_document_t *document,
                                     const lxb_char_t *key, size_t length);

#if 0
static lxb_status_t
lxb_html_document_style_remove_by_rule_cb(lxb_dom_node_t *node,
                                          lxb_css_selector_specificity_t spec,
                                          void *ctx);

static lxb_status_t
lxb_html_document_style_remove_avl_cb(lexbor_avl_t *avl,
                                      lexbor_avl_node_t **root,
                                      lexbor_avl_node_t *node, void *ctx);

static lxb_status_t
lxb_html_document_style_cb(lxb_dom_node_t *node,
                           lxb_css_selector_specificity_t spec, void *ctx);

static lxb_status_t
lxb_html_document_done(lxb_html_document_t *document);
#endif


lxb_status_t
lxb_html_parse_chunk_prepare(lxb_html_parser_t *parser,
                             lxb_html_document_t *document);

lxb_inline lxb_status_t
lxb_html_document_parser_prepare(lxb_html_document_t *document);

static lexbor_action_t
lxb_html_document_title_walker(lxb_dom_node_t *node, void *ctx);

#if 0
static lxb_status_t
lxb_html_document_event_insert(lxb_dom_node_t *node);

static lxb_status_t
lxb_html_document_event_insert_attribute(lxb_dom_node_t *node);

static lxb_status_t
lxb_html_document_event_remove(lxb_dom_node_t *node);

static lxb_status_t
lxb_html_document_style_remove_cb(lexbor_avl_t *avl, lexbor_avl_node_t **root,
                                  lexbor_avl_node_t *node, void *ctx);

static lxb_status_t
lxb_html_document_event_remove_attribute(lxb_dom_node_t *node);

static lxb_status_t
lxb_html_document_style_remove_my_cb(lexbor_avl_t *avl, lexbor_avl_node_t **root,
                                     lexbor_avl_node_t *node, void *ctx);

static lxb_status_t
lxb_html_document_event_destroy(lxb_dom_node_t *node);

static lxb_status_t
lxb_html_document_event_set_value(lxb_dom_node_t *node,
                                  const lxb_char_t *value, size_t length);
#endif


lxb_html_document_t *
lxb_html_document_interface_create(lxb_html_document_t *document)
{
    lxb_status_t status;
    lxb_dom_document_t *doc;
    lxb_html_document_t *hdoc;
    lxb_dom_interface_create_f icreator;

    if (document != NULL) {
        doc = lexbor_mraw_calloc(lxb_html_document_mraw(document),
                                 sizeof(lxb_html_document_t));
    }
    else {
        doc = lexbor_calloc(1, sizeof(lxb_html_document_t));
    }

    if (doc == NULL) {
        return NULL;
    }

    icreator = (lxb_dom_interface_create_f) lxb_html_interface_create;

    status = lxb_dom_document_init(doc, lxb_dom_interface_document(document),
                                   icreator, lxb_html_interface_clone,
                                   lxb_html_interface_destroy,
                                   LXB_DOM_DOCUMENT_DTYPE_HTML, LXB_NS_HTML);
    if (status != LXB_STATUS_OK) {
        (void) lxb_dom_document_destroy(doc);
        return NULL;
    }

    hdoc = lxb_html_interface_document(doc);

    if (document == NULL) {
        hdoc->css_init = false;
    }
    else {
        hdoc->css = document->css;
        hdoc->css_init = document->css_init;
    }

    return hdoc;
}

lxb_html_document_t *
lxb_html_document_interface_destroy(lxb_html_document_t *document)
{
    lxb_dom_document_t *doc;

    if (document == NULL) {
        return NULL;
    }

    doc = lxb_dom_interface_document(document);

    if (doc->node.owner_document == doc) {
        (void) lxb_html_parser_unref(doc->parser);
#if 0
        lxb_html_document_css_destroy(document);
#endif
    }

    (void) lxb_dom_document_destroy(doc);

    return NULL;
}

lxb_html_document_t *
lxb_html_document_create(void)
{
    return lxb_html_document_interface_create(NULL);
}

void
lxb_html_document_clean(lxb_html_document_t *document)
{
    document->body = NULL;
    document->head = NULL;
    document->iframe_srcdoc = NULL;
    document->ready_state = LXB_HTML_DOCUMENT_READY_STATE_UNDEF;

#if 0
    lxb_html_document_css_clean(document);
#endif

    lxb_dom_document_clean(lxb_dom_interface_document(document));
}

#if 0
lxb_status_t
lxb_html_document_css_init(lxb_html_document_t *document)
{
    lxb_status_t status;
    lxb_html_document_css_t *css = &document->css;

    if (document->css_init) {
        return LXB_HTML_STATUS_OK;
    }

    css->memory = lxb_css_memory_create();
    status = lxb_css_memory_init(css->memory, 1024);
    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    css->css_selectors = lxb_css_selectors_create();
    status = lxb_css_selectors_init(css->css_selectors);
    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    css->parser = lxb_css_parser_create();
    status = lxb_css_parser_init(css->parser, NULL);
    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    lxb_css_parser_memory_set(css->parser, css->memory);
    lxb_css_parser_selectors_set(css->parser, css->css_selectors);

    css->selectors = lxb_selectors_create();
    status = lxb_selectors_init(css->selectors);
    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    css->styles = lexbor_avl_create();
    status = lexbor_avl_init(css->styles, 2048, sizeof(lxb_html_style_node_t));
    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    css->stylesheets = lexbor_array_create();
    status = lexbor_array_init(css->stylesheets, 16);
    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    css->weak = lexbor_dobject_create();
    status = lexbor_dobject_init(css->weak, 2048,
                                 sizeof(lxb_html_style_weak_t));
    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    status = lxb_html_document_css_customs_init(document);
    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    document->css_init = true;

    document->dom_document.ev_insert = lxb_html_document_event_insert;
    document->dom_document.ev_remove = lxb_html_document_event_remove;
    document->dom_document.ev_destroy = lxb_html_document_event_destroy;
    document->dom_document.ev_set_value = lxb_html_document_event_set_value;

    document->done = lxb_html_document_done;

    return LXB_STATUS_OK;

failed:

    lxb_html_document_css_destroy(document);

    return status;
}

void
lxb_html_document_css_destroy(lxb_html_document_t *document)
{
    lxb_html_document_css_t *css = &document->css;

    if (!document->css_init
        || lxb_dom_interface_node(document)->owner_document
           != lxb_dom_interface_document(document))
    {
        return;
    }

    css->memory = lxb_css_memory_destroy(css->memory, true);
    css->css_selectors = lxb_css_selectors_destroy(css->css_selectors, true);
    css->parser = lxb_css_parser_destroy(css->parser, true);
    css->selectors = lxb_selectors_destroy(css->selectors, true);
    css->styles = lexbor_avl_destroy(css->styles, true);
    css->stylesheets = lexbor_array_destroy(css->stylesheets, true);
    css->weak = lexbor_dobject_destroy(css->weak, true);

    document->dom_document.ev_insert = NULL;
    document->dom_document.ev_remove = NULL;
    document->dom_document.ev_destroy = NULL;
    document->dom_document.ev_set_value = NULL;

    document->done = NULL;

    lxb_html_document_css_customs_destroy(document);
}

void
lxb_html_document_css_clean(lxb_html_document_t *document)
{
    lxb_html_document_css_t *css;

    if (lxb_dom_interface_node(document)->owner_document
        == lxb_dom_interface_document(document))
    {
        if (!document->css_init) {
            return;
        }

        css = &document->css;

        lxb_css_memory_clean(css->memory);
        lxb_css_selectors_clean(css->css_selectors);
        lxb_css_parser_clean(css->parser);
        lxb_selectors_clean(css->selectors);
        lexbor_avl_clean(css->styles);
        lexbor_array_clean(css->stylesheets);
        lexbor_dobject_clean(css->weak);
    }
}
#endif

void
lxb_html_document_css_parser_attach(lxb_html_document_t *document,
                                    lxb_css_parser_t *parser)
{
    document->css.parser = parser;
}

void
lxb_html_document_css_memory_attach(lxb_html_document_t *document,
                                    lxb_css_memory_t *memory)
{
    document->css.memory = memory;
}

lxb_status_t
lxb_html_document_css_customs_init(lxb_html_document_t *document)
{
    lxb_html_document_css_t *css = &document->css;

    css->customs_id = LXB_CSS_PROPERTY__LAST_ENTRY;

    css->customs = lexbor_hash_create();
    return lexbor_hash_init(css->customs, 512,
                            sizeof(lxb_html_document_css_custom_entry_t));
}

void
lxb_html_document_css_customs_destroy(lxb_html_document_t *document)
{
    document->css.customs = lexbor_hash_destroy(document->css.customs, true);
}

uintptr_t
lxb_html_document_css_customs_find_id(lxb_html_document_t *document,
                                      const lxb_char_t *key, size_t length)
{
    const lxb_html_document_css_custom_entry_t *entry;

    entry = lexbor_hash_search(document->css.customs,
                               &lxb_html_document_css_customs_se, key, length);

    return (entry != NULL) ? entry->id : 0;
}

static lxb_html_document_css_custom_entry_t *
lxb_html_document_css_customs_insert(lxb_html_document_t *document,
                                     const lxb_char_t *key, size_t length)
{
    lxb_html_document_css_custom_entry_t *entry;

    if (UINTPTR_MAX - document->css.customs_id == 0) {
        return NULL;
    }

    entry = lexbor_hash_insert(document->css.customs,
                               &lxb_html_document_css_customs_in, key, length);
    if (entry == NULL) {
        return NULL;
    }

    entry->id = document->css.customs_id++;

    return entry;
}

uintptr_t
lxb_html_document_css_customs_id(lxb_html_document_t *document,
                                 const lxb_char_t *key, size_t length)
{
    lxb_html_document_css_custom_entry_t *entry;

    entry = lexbor_hash_search(document->css.customs,
                               &lxb_html_document_css_customs_se, key, length);
    if (entry != NULL) {
        return entry->id;
    }

    entry = lxb_html_document_css_customs_insert(document, key, length);
    if (entry == NULL) {
        return 0;
    }

    return entry->id;
}

lxb_status_t
lxb_html_document_stylesheet_attach(lxb_html_document_t *document,
                                    lxb_css_stylesheet_t *sst)
{
    lxb_status_t status;

    status = lexbor_array_push(document->css.stylesheets, sst);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    return lxb_html_document_stylesheet_apply(document, sst);
}

lxb_status_t
lxb_html_document_stylesheet_apply(lxb_html_document_t *document,
                                   lxb_css_stylesheet_t *sst)
{
    lxb_status_t status = LXB_STATUS_OK;
    lxb_css_rule_t *rule;
    lxb_css_rule_list_t *list;

    rule = sst->root;

    if (rule->type != LXB_CSS_RULE_LIST) {
        return LXB_STATUS_ERROR_WRONG_ARGS;
    }

    list = lxb_css_rule_list(rule);
    rule = list->first;

    while (rule != NULL) {
        switch (rule->type) {
            case LXB_CSS_RULE_STYLE:
                status = lxb_html_document_style_attach(document,
                                                        lxb_css_rule_style(rule));
                break;

            default:
                break;
        }

        if (status != LXB_STATUS_OK) {
            /* FIXME: what to do with an error? */
        }

        rule = rule->next;
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_html_document_stylesheet_add(lxb_html_document_t *document,
                                 lxb_css_stylesheet_t *sst)
{
    if (sst == NULL) {
        return LXB_STATUS_OK;
    }

    return lexbor_array_push(document->css.stylesheets, sst);
}

lxb_status_t
lxb_html_document_stylesheet_remove(lxb_html_document_t *document,
                                    lxb_css_stylesheet_t *sst)
{
    size_t i, length;
    lxb_status_t status = LXB_STATUS_OK;
    lxb_css_rule_t *rule;
    lxb_css_rule_list_t *list;
    lxb_css_stylesheet_t *sst_in;

    rule = sst->root;

    if (rule->type != LXB_CSS_RULE_LIST) {
        return LXB_STATUS_ERROR_WRONG_ARGS;
    }

    list = lxb_css_rule_list(rule);
    rule = list->first;

    while (rule != NULL) {
        switch (rule->type) {
            case LXB_CSS_RULE_STYLE:
                status = lxb_html_document_style_remove(document,
                                                        lxb_css_rule_style(rule));
                break;

            default:
                break;
        }

        if (status != LXB_STATUS_OK) {
            /* FIXME: what to do with an error? */
        }

        rule = rule->next;
    }

    length = lexbor_array_length(document->css.stylesheets);

    for (i = 0; i < length; i++) {
        sst_in = lexbor_array_get(document->css.stylesheets, i);

        if (sst_in == sst) {
            lexbor_array_delete(document->css.stylesheets, i, 1);
            length = lexbor_array_length(document->css.stylesheets);
        }
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_html_document_element_styles_attach(lxb_html_element_t *element)
{
    lxb_status_t status = LXB_STATUS_OK;
    lxb_css_rule_t *rule;
    lexbor_array_t *ssts;
    lxb_css_rule_list_t *list;
    lxb_css_stylesheet_t *sst;
    lxb_html_document_t *document;

    document = lxb_html_element_document(element);
    ssts = document->css.stylesheets;

    for (size_t i = 0; i < lexbor_array_length(ssts); i++) {
        sst = lexbor_array_get(ssts, i);

        list = lxb_css_rule_list(sst->root);
        rule = list->first;

        while (rule != NULL) {
            switch (rule->type) {
                case LXB_CSS_RULE_STYLE:
                    status = lxb_html_document_style_attach_by_element(document,
                                             element, lxb_css_rule_style(rule));
                    break;

                default:
                    break;
            }

            if (status != LXB_STATUS_OK) {
                /* FIXME: what to do with an error? */
            }

            rule = rule->next;
        }
    }

    return LXB_STATUS_OK;
}

void
lxb_html_document_stylesheet_destroy_all(lxb_html_document_t *document,
                                         bool destroy_memory)
{
#if 0
    size_t length;
    lxb_css_stylesheet_t *sst;
    lxb_html_document_css_t *css = &document->css;

    length = lexbor_array_length(css->stylesheets);

    for (size_t i = 0; i < length; i++) {
        sst = lexbor_array_pop(css->stylesheets);

        (void) lxb_css_stylesheet_destroy(sst, destroy_memory);
    }
#endif
}

lxb_status_t
lxb_html_document_style_attach(lxb_html_document_t *document,
                               lxb_css_rule_style_t *style)
{
#if 0
    lxb_html_document_css_t *css = &document->css;

    return lxb_selectors_find(css->selectors, lxb_dom_interface_node(document),
                              style->selector, lxb_html_document_style_cb, style);
#endif
    return LXB_STATUS_OK;
}

lxb_status_t
lxb_html_document_style_remove(lxb_html_document_t *document,
                               lxb_css_rule_style_t *style)
{
#if 0
    lxb_html_document_css_t *css = &document->css;

    return lxb_selectors_find(css->selectors, lxb_dom_interface_node(document),
                              style->selector,
                              lxb_html_document_style_remove_by_rule_cb, style);
#endif
    return LXB_STATUS_OK;
}

#if 0
static lxb_status_t
lxb_html_document_style_remove_by_rule_cb(lxb_dom_node_t *node,
                                          lxb_css_selector_specificity_t spec,
                                          void *ctx)
{
    lxb_html_element_t *el;
    lxb_html_document_t *doc;
    lxb_css_rule_style_t *style = ctx;
    lxb_html_document_remove_ctx_t context;

    el = lxb_html_interface_element(node);

    if (el->style == NULL) {
        return LXB_STATUS_OK;
    }

    doc = lxb_html_interface_document(node->owner_document);

    context.doc = doc;
    context.list = style->declarations;

    return lexbor_avl_foreach(doc->css.styles, &el->style,
                              lxb_html_document_style_remove_avl_cb, &context);
}

static lxb_status_t
lxb_html_document_style_remove_avl_cb(lexbor_avl_t *avl,
                                      lexbor_avl_node_t **root,
                                      lexbor_avl_node_t *node, void *ctx)
{
    lxb_html_document_remove_ctx_t *context = ctx;
    lxb_html_style_node_t *style = (lxb_html_style_node_t *) node;

    lxb_html_element_style_remove_by_list(context->doc, root,
                                          style, context->list);
    return LXB_STATUS_OK;
}
#endif

lxb_status_t
lxb_html_document_style_attach_by_element(lxb_html_document_t *document,
                                          lxb_html_element_t *element,
                                          lxb_css_rule_style_t *style)
{
#if 0
    lxb_html_document_css_t *css = &document->css;

    return lxb_selectors_match_node(css->selectors,
                                    lxb_dom_interface_node(element),
                                    style->selector,
                                    lxb_html_document_style_cb, style);
#endif
    return LXB_STATUS_OK;
}

#if 0
static lxb_status_t
lxb_html_document_style_cb(lxb_dom_node_t *node,
                           lxb_css_selector_specificity_t spec, void *ctx)
{
    lxb_css_rule_style_t *style = ctx;

    // FIXME: we don't have support for anything other than HTML.

    if (node->ns != LXB_NS_HTML) {
        return LXB_STATUS_OK;
    }

    return lxb_html_element_style_list_append(lxb_html_interface_element(node),
                                              style->declarations, spec);
}
#endif

lxb_html_document_t *
lxb_html_document_destroy(lxb_html_document_t *document)
{
    return lxb_html_document_interface_destroy(document);
}

lxb_status_t
lxb_html_document_parse(lxb_html_document_t *document,
                        const lxb_char_t *html, size_t size)
{
    lxb_status_t status;
    lxb_dom_document_t *doc;
    lxb_html_document_opt_t opt;

    if (document->ready_state != LXB_HTML_DOCUMENT_READY_STATE_UNDEF
        && document->ready_state != LXB_HTML_DOCUMENT_READY_STATE_LOADING)
    {
        lxb_html_document_clean(document);
    }

    opt = document->opt;
    doc = lxb_dom_interface_document(document);

    status = lxb_html_document_parser_prepare(document);
    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    status = lxb_html_parse_chunk_prepare(doc->parser, document);
    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    status = lxb_html_parse_chunk_process(doc->parser, html, size);
    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    document->opt = opt;

    return lxb_html_parse_chunk_end(doc->parser);

failed:

    document->opt = opt;

    return status;
}

lxb_status_t
lxb_html_document_parse_chunk_begin(lxb_html_document_t *document)
{
    if (document->ready_state != LXB_HTML_DOCUMENT_READY_STATE_UNDEF
        && document->ready_state != LXB_HTML_DOCUMENT_READY_STATE_LOADING)
    {
        lxb_html_document_clean(document);
    }

    lxb_status_t status = lxb_html_document_parser_prepare(document);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    return lxb_html_parse_chunk_prepare(document->dom_document.parser,
                                        document);
}

lxb_status_t
lxb_html_document_parse_chunk(lxb_html_document_t *document,
                              const lxb_char_t *html, size_t size)
{
    return lxb_html_parse_chunk_process(document->dom_document.parser,
                                        html, size);
}

lxb_status_t
lxb_html_document_parse_chunk_end(lxb_html_document_t *document)
{
    return lxb_html_parse_chunk_end(document->dom_document.parser);
}

lxb_dom_node_t *
lxb_html_document_parse_fragment(lxb_html_document_t *document,
                                 lxb_dom_element_t *element,
                                 const lxb_char_t *html, size_t size)
{
    lxb_status_t status;
    lxb_html_parser_t *parser;
    lxb_html_document_opt_t opt = document->opt;

    status = lxb_html_document_parser_prepare(document);
    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    parser = document->dom_document.parser;

    status = lxb_html_parse_fragment_chunk_begin(parser, document,
                                                 element->node.local_name,
                                                 element->node.ns);
    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    status = lxb_html_parse_fragment_chunk_process(parser, html, size);
    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    document->opt = opt;

    return lxb_html_parse_fragment_chunk_end(parser);

failed:

    document->opt = opt;

    return NULL;
}

lxb_status_t
lxb_html_document_parse_fragment_chunk_begin(lxb_html_document_t *document,
                                             lxb_dom_element_t *element)
{
    lxb_status_t status;
    lxb_html_parser_t *parser = document->dom_document.parser;

    status = lxb_html_document_parser_prepare(document);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    return lxb_html_parse_fragment_chunk_begin(parser, document,
                                               element->node.local_name,
                                               element->node.ns);
}

lxb_status_t
lxb_html_document_parse_fragment_chunk(lxb_html_document_t *document,
                                       const lxb_char_t *html, size_t size)
{
    return lxb_html_parse_fragment_chunk_process(document->dom_document.parser,
                                                 html, size);
}

lxb_dom_node_t *
lxb_html_document_parse_fragment_chunk_end(lxb_html_document_t *document)
{
    return lxb_html_parse_fragment_chunk_end(document->dom_document.parser);
}

lxb_inline lxb_status_t
lxb_html_document_parser_prepare(lxb_html_document_t *document)
{
    lxb_status_t status;
    lxb_dom_document_t *doc;

    doc = lxb_dom_interface_document(document);

    if (doc->parser == NULL) {
        doc->parser = lxb_html_parser_create();
        status = lxb_html_parser_init(doc->parser);

        if (status != LXB_STATUS_OK) {
            lxb_html_parser_destroy(doc->parser);
            return status;
        }
    }
    else if (lxb_html_parser_state(doc->parser) != LXB_HTML_PARSER_STATE_BEGIN) {
        lxb_html_parser_clean(doc->parser);
    }

    return LXB_STATUS_OK;
}

#if 0
static lxb_status_t
lxb_html_document_done(lxb_html_document_t *document)
{
    size_t i, length;
    lxb_status_t status;
    lxb_css_stylesheet_t *sst;

    if (!document->css_init) {
        return LXB_STATUS_OK;
    }

    length = lexbor_array_length(document->css.stylesheets);

    for (i = 0; i < length; i++) {
        sst = lexbor_array_get(document->css.stylesheets, i);

        status = lxb_html_document_stylesheet_apply(document, sst);
        if (status != LXB_STATUS_OK) {
            return status;
        }
    }

    return LXB_STATUS_OK;
}
#endif

const lxb_char_t *
lxb_html_document_title(lxb_html_document_t *document, size_t *len)
{
    lxb_html_title_element_t *title = NULL;

    lxb_dom_node_simple_walk(lxb_dom_interface_node(document),
                             lxb_html_document_title_walker, &title);
    if (title == NULL) {
        return NULL;
    }

    return lxb_html_title_element_strict_text(title, len);
}

lxb_status_t
lxb_html_document_title_set(lxb_html_document_t *document,
                            const lxb_char_t *title, size_t len)
{
    lxb_status_t status;

    /* TODO: If the document element is an SVG svg element */

    /* If the document element is in the HTML namespace */
    if (document->head == NULL) {
        return LXB_STATUS_OK;
    }

    lxb_html_title_element_t *el_title = NULL;

    lxb_dom_node_simple_walk(lxb_dom_interface_node(document),
                             lxb_html_document_title_walker, &el_title);
    if (el_title == NULL) {
        el_title = (void *) lxb_html_document_create_element(document,
                                         (const lxb_char_t *) "title", 5, NULL);
        if (el_title == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }

        lxb_dom_node_insert_child(lxb_dom_interface_node(document->head),
                                  lxb_dom_interface_node(el_title));
    }

    status = lxb_dom_node_text_content_set(lxb_dom_interface_node(el_title),
                                           title, len);
    if (status != LXB_STATUS_OK) {
        lxb_html_document_destroy_element(&el_title->element.element);

        return status;
    }

    return LXB_STATUS_OK;
}

const lxb_char_t *
lxb_html_document_title_raw(lxb_html_document_t *document, size_t *len)
{
    lxb_html_title_element_t *title = NULL;

    lxb_dom_node_simple_walk(lxb_dom_interface_node(document),
                             lxb_html_document_title_walker, &title);
    if (title == NULL) {
        return NULL;
    }

    return lxb_html_title_element_text(title, len);
}

static lexbor_action_t
lxb_html_document_title_walker(lxb_dom_node_t *node, void *ctx)
{
    if (node->local_name == LXB_TAG_TITLE && node->ns == LXB_NS_HTML) {
        *((void **) ctx) = node;

        return LEXBOR_ACTION_STOP;
    }

    return LEXBOR_ACTION_OK;
}

lxb_dom_node_t *
lxb_html_document_import_node(lxb_html_document_t *doc, lxb_dom_node_t *node,
                              bool deep)
{
    return lxb_dom_document_import_node(&doc->dom_document, node, deep);
}

#if 0
static lxb_status_t
lxb_html_document_event_insert(lxb_dom_node_t *node)
{
    lxb_status_t status;
    lxb_html_document_t *doc;
    lxb_html_style_element_t *style;

    if (node->type == LXB_DOM_NODE_TYPE_ATTRIBUTE) {
        return lxb_html_document_event_insert_attribute(node);
    }
    else if (node->type != LXB_DOM_NODE_TYPE_ELEMENT) {
        return LXB_STATUS_OK;
    }

    // FIXME: we don't have support for anything other than HTML.

    if (node->ns != LXB_NS_HTML) {
        return LXB_STATUS_OK;
    }

    if (node->local_name == LXB_TAG_STYLE) {
        style = lxb_html_interface_style(node);

        status = lxb_html_style_element_parse(style);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        doc = lxb_html_interface_document(node->owner_document);

        status = lxb_html_document_stylesheet_attach(doc, style->stylesheet);
        if (status != LXB_STATUS_OK) {
            return status;
        }
    }

    return lxb_html_document_element_styles_attach(lxb_html_interface_element(node));
}

static lxb_status_t
lxb_html_document_event_insert_attribute(lxb_dom_node_t *node)
{
    lxb_status_t status;
    lxb_dom_attr_t *attr;
    lxb_html_element_t *el;

    if (node->type != LXB_DOM_NODE_TYPE_ATTRIBUTE
        || node->local_name != LXB_DOM_ATTR_STYLE)
    {
        return LXB_STATUS_OK;
    }

    // FIXME: we don't have support for anything other than HTML.

    if (node->ns != LXB_NS_HTML) {
        return LXB_STATUS_OK;
    }

    attr = lxb_dom_interface_attr(node);
    el = lxb_html_interface_element(attr->owner);

    if (el != NULL && el->list != NULL) {
        status = lxb_html_document_event_remove_attribute(node);
        if (status != LXB_STATUS_OK) {
            return status;
        }
    }

    if (attr->value == NULL || attr->value->data == NULL) {
        return LXB_STATUS_OK;
    }

    return lxb_html_element_style_parse(el, attr->value->data,
                                        attr->value->length);
}

static lxb_status_t
lxb_html_document_event_remove(lxb_dom_node_t *node)
{
    lxb_status_t status;
    lxb_html_element_t *el;
    lxb_html_document_t *doc;
    lxb_html_document_event_ctx_t context;

    if (node->type == LXB_DOM_NODE_TYPE_ATTRIBUTE) {
        return lxb_html_document_event_remove_attribute(node);
    }
    else if (node->type != LXB_DOM_NODE_TYPE_ELEMENT) {
        return LXB_STATUS_OK;
    }

    // FIXME: we don't have support for anything other than HTML.

    if (node->ns != LXB_NS_HTML) {
        return LXB_STATUS_OK;
    }

    if (node->local_name == LXB_TAG_STYLE) {
        status = lxb_html_element_style_remove((lxb_html_style_element_t *) node);
        if (status != LXB_STATUS_OK) {
            return status;
        }
    }

    el = lxb_html_interface_element(node);

    if (el->style == NULL) {
        return LXB_STATUS_OK;
    }

    doc = lxb_html_interface_document(node->owner_document);

    context.doc = doc;
    context.all = false;

    return lexbor_avl_foreach(doc->css.styles, &el->style,
                              lxb_html_document_style_remove_cb, &context);
}

static lxb_status_t
lxb_html_document_style_remove_cb(lexbor_avl_t *avl, lexbor_avl_node_t **root,
                                  lexbor_avl_node_t *node, void *ctx)
{
    lxb_html_document_event_ctx_t *context = ctx;
    lxb_html_style_node_t *style = (lxb_html_style_node_t *) node;

    if (context->all) {
        lxb_html_element_style_remove_all(context->doc, root, style);
    }

    lxb_html_element_style_remove_all_not(context->doc, root, style, false);

    return LXB_STATUS_OK;
}

static lxb_status_t
lxb_html_document_event_remove_attribute(lxb_dom_node_t *node)
{
    lxb_status_t status;
    lxb_dom_attr_t *attr;
    lxb_html_element_t *el;
    lxb_html_document_t *doc;
    lxb_html_document_event_ctx_t context;

    // FIXME: we don't have support for anything other than HTML.

    if (node->local_name != LXB_DOM_ATTR_STYLE || node->ns != LXB_NS_HTML) {
        return LXB_STATUS_OK;
    }

    attr = lxb_dom_interface_attr(node);
    el = lxb_html_interface_element(attr->owner);

    if (el == NULL || el->list == NULL) {
        return LXB_STATUS_OK;
    }

    doc = lxb_html_interface_document(node->owner_document);

    context.doc = doc;

    status = lexbor_avl_foreach(doc->css.styles, &el->style,
                                lxb_html_document_style_remove_my_cb, &context);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    el->list->first = NULL;
    el->list->last = NULL;

    el->list = lxb_css_rule_declaration_list_destroy(el->list, true);

    return LXB_STATUS_OK;
}

static lxb_status_t
lxb_html_document_style_remove_my_cb(lexbor_avl_t *avl, lexbor_avl_node_t **root,
                                     lexbor_avl_node_t *node, void *ctx)
{
    lxb_html_document_event_ctx_t *context = ctx;
    lxb_html_style_node_t *style = (lxb_html_style_node_t *) node;

    lxb_html_element_style_remove_all_not(context->doc, root, style, true);

    return LXB_STATUS_OK;
}

static lxb_status_t
lxb_html_document_event_destroy(lxb_dom_node_t *node)
{
    lxb_status_t status;
    lxb_html_element_t *el;
    lxb_html_document_t *doc;
    lxb_html_document_event_ctx_t context;

    if (node->type == LXB_DOM_NODE_TYPE_ATTRIBUTE) {
        return lxb_html_document_event_remove_attribute(node);
    }
    else if (node->type != LXB_DOM_NODE_TYPE_ELEMENT) {
        return LXB_STATUS_OK;
    }

    // FIXME: we don't have support for anything other than HTML.

    if (node->ns != LXB_NS_HTML) {
        return LXB_STATUS_OK;
    }

    el = lxb_html_interface_element(node);

    if (el->style == NULL) {
        if (el->list != NULL) {
            goto destroy;
        }

        return LXB_STATUS_OK;
    }

    doc = lxb_html_interface_document(node->owner_document);

    context.doc = doc;
    context.all = true;

    status = lexbor_avl_foreach(doc->css.styles, &el->style,
                                lxb_html_document_style_remove_cb, &context);

    if (status != LXB_STATUS_OK) {
        return status;
    }

destroy:

    el->list->first = NULL;
    el->list->last = NULL;

    el->list = lxb_css_rule_declaration_list_destroy(el->list, true);

    return LXB_STATUS_OK;
}

static lxb_status_t
lxb_html_document_event_set_value(lxb_dom_node_t *node,
                                  const lxb_char_t *value, size_t length)
{
    lxb_status_t status;
    lxb_dom_attr_t *attr = lxb_dom_interface_attr(node);

    if (node->type != LXB_DOM_NODE_TYPE_ATTRIBUTE
        || node->local_name != LXB_DOM_ATTR_STYLE)
    {
        return LXB_STATUS_OK;
    }

    // FIXME: we don't have support for anything other than HTML.

    if (node->ns != LXB_NS_HTML) {
        return LXB_STATUS_OK;
    }

    if (attr->owner == NULL) {
        return LXB_STATUS_OK;
    }

    status = lxb_html_document_event_remove_attribute(node);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    return lxb_html_element_style_parse(lxb_html_interface_element(node),
                                        value, length);
}
#endif

/*
 * No inline functions for ABI.
 */
lxb_html_head_element_t *
lxb_html_document_head_element_noi(lxb_html_document_t *document)
{
    return lxb_html_document_head_element(document);
}

lxb_html_body_element_t *
lxb_html_document_body_element_noi(lxb_html_document_t *document)
{
    return lxb_html_document_body_element(document);
}

lxb_dom_document_t *
lxb_html_document_original_ref_noi(lxb_html_document_t *document)
{
    return lxb_html_document_original_ref(document);
}

bool
lxb_html_document_is_original_noi(lxb_html_document_t *document)
{
    return lxb_html_document_is_original(document);
}

lexbor_mraw_t *
lxb_html_document_mraw_noi(lxb_html_document_t *document)
{
    return lxb_html_document_mraw(document);
}

lexbor_mraw_t *
lxb_html_document_mraw_text_noi(lxb_html_document_t *document)
{
    return lxb_html_document_mraw_text(document);
}

void
lxb_html_document_opt_set_noi(lxb_html_document_t *document,
                              lxb_html_document_opt_t opt)
{
    lxb_html_document_opt_set(document, opt);
}

lxb_html_document_opt_t
lxb_html_document_opt_noi(lxb_html_document_t *document)
{
    return lxb_html_document_opt(document);
}

void *
lxb_html_document_create_struct_noi(lxb_html_document_t *document,
                                    size_t struct_size)
{
    return lxb_html_document_create_struct(document, struct_size);
}

void *
lxb_html_document_destroy_struct_noi(lxb_html_document_t *document, void *data)
{
    return lxb_html_document_destroy_struct(document, data);
}

lxb_html_element_t *
lxb_html_document_create_element_noi(lxb_html_document_t *document,
                                     const lxb_char_t *local_name,
                                     size_t lname_len, void *reserved_for_opt)
{
    return lxb_html_document_create_element(document, local_name, lname_len,
                                            reserved_for_opt);
}

lxb_dom_element_t *
lxb_html_document_destroy_element_noi(lxb_dom_element_t *element)
{
    return lxb_html_document_destroy_element(element);
}
