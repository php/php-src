/*
 * Copyright (C) 2018-2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_HTML_DOCUMENT_H
#define LEXBOR_HTML_DOCUMENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/mraw.h"
#include "lexbor/core/avl.h"

#include "lexbor/tag/tag.h"
#include "lexbor/ns/ns.h"
#include "lexbor/html/interface.h"
#include "lexbor/dom/interfaces/attr.h"
#include "lexbor/dom/interfaces/document.h"
#include "lexbor/css/css.h"
#include "lexbor/selectors/selectors.h"


typedef lxb_status_t
(*lxb_html_document_done_cb_f)(lxb_html_document_t *document);

typedef unsigned int lxb_html_document_opt_t;

typedef enum {
    LXB_HTML_DOCUMENT_READY_STATE_UNDEF       = 0x00,
    LXB_HTML_DOCUMENT_READY_STATE_LOADING     = 0x01,
    LXB_HTML_DOCUMENT_READY_STATE_INTERACTIVE = 0x02,
    LXB_HTML_DOCUMENT_READY_STATE_COMPLETE    = 0x03,
}
lxb_html_document_ready_state_t;

enum lxb_html_document_opt {
    LXB_HTML_DOCUMENT_OPT_UNDEF     = 0x00,
    LXB_HTML_DOCUMENT_PARSE_WO_COPY = 0x01
};

typedef struct {
    lxb_css_memory_t    *memory;
    lxb_css_selectors_t *css_selectors;
    lxb_css_parser_t    *parser;
    lxb_selectors_t     *selectors;

    lexbor_avl_t        *styles;
    lexbor_array_t      *stylesheets;
    lexbor_dobject_t    *weak;

    lexbor_hash_t       *customs;
    uintptr_t           customs_id;
}
lxb_html_document_css_t;

struct lxb_html_document {
    lxb_dom_document_t              dom_document;

    void                            *iframe_srcdoc;

    lxb_html_head_element_t         *head;
    lxb_html_body_element_t         *body;

    lxb_html_document_css_t         css;
    bool                            css_init;

    lxb_html_document_done_cb_f     done;
    lxb_html_document_ready_state_t ready_state;

    lxb_html_document_opt_t         opt;
};


LXB_API lxb_html_document_t *
lxb_html_document_interface_create(lxb_html_document_t *document);

LXB_API lxb_html_document_t *
lxb_html_document_interface_destroy(lxb_html_document_t *document);


LXB_API lxb_html_document_t *
lxb_html_document_create(void);

LXB_API void
lxb_html_document_clean(lxb_html_document_t *document);

LXB_API lxb_html_document_t *
lxb_html_document_destroy(lxb_html_document_t *document);

LXB_API lxb_status_t
lxb_html_document_css_init(lxb_html_document_t *document);

LXB_API void
lxb_html_document_css_destroy(lxb_html_document_t *document);

LXB_API void
lxb_html_document_css_clean(lxb_html_document_t *document);

LXB_API void
lxb_html_document_css_parser_attach(lxb_html_document_t *document,
                                    lxb_css_parser_t *parser);

LXB_API void
lxb_html_document_css_memory_attach(lxb_html_document_t *document,
                                    lxb_css_memory_t *memory);

LXB_API uintptr_t
lxb_html_document_css_customs_find_id(lxb_html_document_t *document,
                                      const lxb_char_t *key, size_t length);

LXB_API uintptr_t
lxb_html_document_css_customs_id(lxb_html_document_t *document,
                                 const lxb_char_t *key, size_t length);

LXB_API lxb_status_t
lxb_html_document_stylesheet_attach(lxb_html_document_t *document,
                                    lxb_css_stylesheet_t *sst);

LXB_API lxb_status_t
lxb_html_document_stylesheet_apply(lxb_html_document_t *document,
                                   lxb_css_stylesheet_t *sst);

LXB_API lxb_status_t
lxb_html_document_stylesheet_add(lxb_html_document_t *document,
                                 lxb_css_stylesheet_t *sst);

LXB_API lxb_status_t
lxb_html_document_stylesheet_remove(lxb_html_document_t *document,
                                    lxb_css_stylesheet_t *sst);

LXB_API lxb_status_t
lxb_html_document_element_styles_attach(lxb_html_element_t *element);

LXB_API void
lxb_html_document_stylesheet_destroy_all(lxb_html_document_t *document,
                                         bool destroy_memory);

LXB_API lxb_status_t
lxb_html_document_style_attach(lxb_html_document_t *document,
                               lxb_css_rule_style_t *style);

LXB_API lxb_status_t
lxb_html_document_style_remove(lxb_html_document_t *document,
                               lxb_css_rule_style_t *style);

LXB_API lxb_status_t
lxb_html_document_style_attach_by_element(lxb_html_document_t *document,
                                          lxb_html_element_t *element,
                                          lxb_css_rule_style_t *style);

LXB_API lxb_status_t
lxb_html_document_css_customs_init(lxb_html_document_t *document);

LXB_API void
lxb_html_document_css_customs_destroy(lxb_html_document_t *document);

LXB_API lxb_status_t
lxb_html_document_parse(lxb_html_document_t *document,
                        const lxb_char_t *html, size_t size);

LXB_API lxb_status_t
lxb_html_document_parse_chunk_begin(lxb_html_document_t *document);

LXB_API lxb_status_t
lxb_html_document_parse_chunk(lxb_html_document_t *document,
                              const lxb_char_t *html, size_t size);

LXB_API lxb_status_t
lxb_html_document_parse_chunk_end(lxb_html_document_t *document);

LXB_API lxb_dom_node_t *
lxb_html_document_parse_fragment(lxb_html_document_t *document,
                                 lxb_dom_element_t *element,
                                 const lxb_char_t *html, size_t size);

LXB_API lxb_status_t
lxb_html_document_parse_fragment_chunk_begin(lxb_html_document_t *document,
                                             lxb_dom_element_t *element);

LXB_API lxb_status_t
lxb_html_document_parse_fragment_chunk(lxb_html_document_t *document,
                                       const lxb_char_t *html, size_t size);

LXB_API lxb_dom_node_t *
lxb_html_document_parse_fragment_chunk_end(lxb_html_document_t *document);

LXB_API const lxb_char_t *
lxb_html_document_title(lxb_html_document_t *document, size_t *len);

LXB_API lxb_status_t
lxb_html_document_title_set(lxb_html_document_t *document,
                            const lxb_char_t *title, size_t len);

LXB_API const lxb_char_t *
lxb_html_document_title_raw(lxb_html_document_t *document, size_t *len);

LXB_API lxb_dom_node_t *
lxb_html_document_import_node(lxb_html_document_t *doc, lxb_dom_node_t *node,
                              bool deep);

/*
 * Inline functions
 */
lxb_inline lxb_html_head_element_t *
lxb_html_document_head_element(lxb_html_document_t *document)
{
    return document->head;
}

lxb_inline lxb_html_body_element_t *
lxb_html_document_body_element(lxb_html_document_t *document)
{
    return document->body;
}

lxb_inline lxb_dom_document_t *
lxb_html_document_original_ref(lxb_html_document_t *document)
{
    if (lxb_dom_interface_node(document)->owner_document
        != &document->dom_document)
    {
        return lxb_dom_interface_node(document)->owner_document;
    }

    return lxb_dom_interface_document(document);
}

lxb_inline bool
lxb_html_document_is_original(lxb_html_document_t *document)
{
    return lxb_dom_interface_node(document)->owner_document
        == &document->dom_document;
}

lxb_inline lexbor_mraw_t *
lxb_html_document_mraw(lxb_html_document_t *document)
{
    return (lexbor_mraw_t *) lxb_dom_interface_document(document)->mraw;
}

lxb_inline lexbor_mraw_t *
lxb_html_document_mraw_text(lxb_html_document_t *document)
{
    return (lexbor_mraw_t *) lxb_dom_interface_document(document)->text;
}

lxb_inline void
lxb_html_document_opt_set(lxb_html_document_t *document,
                          lxb_html_document_opt_t opt)
{
    document->opt = opt;
}

lxb_inline lxb_html_document_opt_t
lxb_html_document_opt(lxb_html_document_t *document)
{
    return document->opt;
}

lxb_inline lexbor_hash_t *
lxb_html_document_tags(lxb_html_document_t *document)
{
    return document->dom_document.tags;
}

lxb_inline void *
lxb_html_document_create_struct(lxb_html_document_t *document,
                                size_t struct_size)
{
    return lexbor_mraw_calloc(lxb_dom_interface_document(document)->mraw,
                              struct_size);
}

lxb_inline void *
lxb_html_document_destroy_struct(lxb_html_document_t *document, void *data)
{
    return lexbor_mraw_free(lxb_dom_interface_document(document)->mraw, data);
}

lxb_inline lxb_html_element_t *
lxb_html_document_create_element(lxb_html_document_t *document,
                                 const lxb_char_t *local_name, size_t lname_len,
                                 void *reserved_for_opt)
{
    return (lxb_html_element_t *) lxb_dom_document_create_element(&document->dom_document,
                                                                  local_name, lname_len,
                                                                  reserved_for_opt);
}

lxb_inline lxb_dom_element_t *
lxb_html_document_destroy_element(lxb_dom_element_t *element)
{
    return lxb_dom_document_destroy_element(element);
}

/*
 * No inline functions for ABI.
 */
LXB_API lxb_html_head_element_t *
lxb_html_document_head_element_noi(lxb_html_document_t *document);

LXB_API lxb_html_body_element_t *
lxb_html_document_body_element_noi(lxb_html_document_t *document);

LXB_API lxb_dom_document_t *
lxb_html_document_original_ref_noi(lxb_html_document_t *document);

LXB_API bool
lxb_html_document_is_original_noi(lxb_html_document_t *document);

LXB_API lexbor_mraw_t *
lxb_html_document_mraw_noi(lxb_html_document_t *document);

LXB_API lexbor_mraw_t *
lxb_html_document_mraw_text_noi(lxb_html_document_t *document);

LXB_API void
lxb_html_document_opt_set_noi(lxb_html_document_t *document,
                              lxb_html_document_opt_t opt);

LXB_API lxb_html_document_opt_t
lxb_html_document_opt_noi(lxb_html_document_t *document);

LXB_API void *
lxb_html_document_create_struct_noi(lxb_html_document_t *document,
                                    size_t struct_size);

LXB_API void *
lxb_html_document_destroy_struct_noi(lxb_html_document_t *document, void *data);

LXB_API lxb_html_element_t *
lxb_html_document_create_element_noi(lxb_html_document_t *document,
                                     const lxb_char_t *local_name,
                                     size_t lname_len, void *reserved_for_opt);

LXB_API lxb_dom_element_t *
lxb_html_document_destroy_element_noi(lxb_dom_element_t *element);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_HTML_DOCUMENT_H */
