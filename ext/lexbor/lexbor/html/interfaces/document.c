/*
 * Copyright (C) 2018-2024 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/core/str.h"

#include "lexbor/html/interfaces/document.h"
#include "lexbor/html/interfaces/title_element.h"
#include "lexbor/html/interfaces/style_element.h"
#include "lexbor/html/node.h"
#include "lexbor/html/parser.h"

#include "lexbor/tag/tag.h"

#include "lexbor/dom/interfaces/text.h"
#include "lexbor/dom/interfaces/element.h"

#define LXB_HTML_TAG_RES_DATA
#define LXB_HTML_TAG_RES_SHS_DATA
#include "lexbor/html/tag_res.h"


lxb_status_t
lxb_html_parse_chunk_prepare(lxb_html_parser_t *parser,
                             lxb_html_document_t *document);

lxb_inline lxb_status_t
lxb_html_document_parser_prepare(lxb_html_document_t *document);

static lexbor_action_t
lxb_html_document_title_walker(lxb_dom_node_t *node, void *ctx);


lxb_inline lxb_dom_interface_t *
lxb_html_document_interface_create_wrapper(lxb_dom_document_t *document,
                                           lxb_tag_id_t tag_id, lxb_ns_id_t ns)
{
    return lxb_html_interface_create(lxb_html_interface_document(document),
                                     tag_id, ns);
}

lxb_html_document_t *
lxb_html_document_interface_create(lxb_html_document_t *document)
{
    lxb_status_t status;
    lxb_dom_document_t *doc;

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

    status = lxb_dom_document_init(doc, lxb_dom_interface_document(document),
                                   lxb_html_document_interface_create_wrapper,
                                   lxb_html_interface_clone,
                                   lxb_html_interface_destroy,
                                   LXB_DOM_DOCUMENT_DTYPE_HTML, LXB_NS_HTML);
    if (status != LXB_STATUS_OK) {
        (void) lxb_dom_document_destroy(doc);
        return NULL;
    }

    return lxb_html_interface_document(doc);
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

    lxb_dom_document_clean(lxb_dom_interface_document(document));
}

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
    lxb_html_parser_t *parser;

    status = lxb_html_document_parser_prepare(document);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    parser = document->dom_document.parser;

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
