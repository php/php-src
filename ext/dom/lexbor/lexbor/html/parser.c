/*
 * Copyright (C) 2018-2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/parser.h"
#include "lexbor/html/node.h"
#include "lexbor/html/tree/open_elements.h"
#include "lexbor/html/interfaces/element.h"
#include "lexbor/html/interfaces/html_element.h"
#include "lexbor/html/interfaces/form_element.h"
#include "lexbor/html/tree/template_insertion.h"
#include "lexbor/html/tree/insertion_mode.h"

#define LXB_HTML_TAG_RES_DATA
#define LXB_HTML_TAG_RES_SHS_DATA
#include "lexbor/html/tag_res.h"


static void
lxb_html_parse_fragment_chunk_destroy(lxb_html_parser_t *parser);


lxb_html_parser_t *
lxb_html_parser_create(void)
{
    return lexbor_calloc(1, sizeof(lxb_html_parser_t));
}

lxb_status_t
lxb_html_parser_init(lxb_html_parser_t *parser)
{
    if (parser == NULL) {
        return LXB_STATUS_ERROR_OBJECT_IS_NULL;
    }

    /* Tokenizer */
    parser->tkz = lxb_html_tokenizer_create();
    lxb_status_t status = lxb_html_tokenizer_init(parser->tkz);

    if (status != LXB_STATUS_OK) {
        return status;
    }

    /* Tree */
    parser->tree = lxb_html_tree_create();
    status = lxb_html_tree_init(parser->tree, parser->tkz);

    if (status != LXB_STATUS_OK) {
        return status;
    }

    parser->original_tree = NULL;
    parser->form = NULL;
    parser->root = NULL;

    parser->state = LXB_HTML_PARSER_STATE_BEGIN;

    parser->ref_count = 1;

    return LXB_STATUS_OK;
}

void
lxb_html_parser_clean(lxb_html_parser_t *parser)
{
    parser->original_tree = NULL;
    parser->form = NULL;
    parser->root = NULL;

    parser->state = LXB_HTML_PARSER_STATE_BEGIN;

    lxb_html_tokenizer_clean(parser->tkz);
    lxb_html_tree_clean(parser->tree);
}

lxb_html_parser_t *
lxb_html_parser_destroy(lxb_html_parser_t *parser)
{
    if (parser == NULL) {
        return NULL;
    }

    parser->tkz = lxb_html_tokenizer_unref(parser->tkz);
    parser->tree = lxb_html_tree_unref(parser->tree);

    return lexbor_free(parser);
}

lxb_html_parser_t *
lxb_html_parser_ref(lxb_html_parser_t *parser)
{
    if (parser == NULL) {
        return NULL;
    }

    parser->ref_count++;

    return parser;
}

lxb_html_parser_t *
lxb_html_parser_unref(lxb_html_parser_t *parser)
{
    if (parser == NULL || parser->ref_count == 0) {
        return NULL;
    }

    parser->ref_count--;

    if (parser->ref_count == 0) {
        lxb_html_parser_destroy(parser);
    }

    return NULL;
}


lxb_html_document_t *
lxb_html_parse(lxb_html_parser_t *parser, const lxb_char_t *html, size_t size)
{
    lxb_html_document_t *document = lxb_html_parse_chunk_begin(parser);
    if (document == NULL) {
        return NULL;
    }

    lxb_html_parse_chunk_process(parser, html, size);
    if (parser->status != LXB_STATUS_OK) {
        goto failed;
    }

    lxb_html_parse_chunk_end(parser);
    if (parser->status != LXB_STATUS_OK) {
        goto failed;
    }

    return document;

failed:

    lxb_html_document_interface_destroy(document);

    return NULL;
}

lxb_dom_node_t *
lxb_html_parse_fragment(lxb_html_parser_t *parser, lxb_html_element_t *element,
                        const lxb_char_t *html, size_t size)
{
    return lxb_html_parse_fragment_by_tag_id(parser,
                                             parser->tree->document,
                                             element->element.node.local_name,
                                             element->element.node.ns,
                                             html, size);
}

lxb_dom_node_t *
lxb_html_parse_fragment_by_tag_id(lxb_html_parser_t *parser,
                                  lxb_html_document_t *document,
                                  lxb_tag_id_t tag_id, lxb_ns_id_t ns,
                                  const lxb_char_t *html, size_t size)
{
    lxb_html_parse_fragment_chunk_begin(parser, document, tag_id, ns);
    if (parser->status != LXB_STATUS_OK) {
        return NULL;
    }

    lxb_html_parse_fragment_chunk_process(parser, html, size);
    if (parser->status != LXB_STATUS_OK) {
        return NULL;
    }

    return lxb_html_parse_fragment_chunk_end(parser);
}

lxb_status_t
lxb_html_parse_fragment_chunk_begin(lxb_html_parser_t *parser,
                                    lxb_html_document_t *document,
                                    lxb_tag_id_t tag_id, lxb_ns_id_t ns)
{
    lxb_dom_document_t *doc;
    lxb_html_document_t *new_doc;

    if (parser->state != LXB_HTML_PARSER_STATE_BEGIN) {
        lxb_html_parser_clean(parser);
    }

    parser->state = LXB_HTML_PARSER_STATE_FRAGMENT_PROCESS;

    new_doc = lxb_html_document_interface_create(document);
    if (new_doc == NULL) {
        parser->state = LXB_HTML_PARSER_STATE_ERROR;
        return parser->status;
    }

    doc = lxb_dom_interface_document(new_doc);

    if (document == NULL) {
        doc->scripting = parser->tree->scripting;
        doc->compat_mode = LXB_DOM_DOCUMENT_CMODE_NO_QUIRKS;
    }

    lxb_html_tokenizer_set_state_by_tag(parser->tkz, doc->scripting, tag_id, ns);

    parser->root = lxb_html_interface_create(new_doc, LXB_TAG_HTML, LXB_NS_HTML);
    if (parser->root == NULL) {
        parser->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        goto done;
    }

    lxb_dom_node_insert_child_wo_events(lxb_dom_interface_node(new_doc),
                                        parser->root);
    lxb_dom_document_attach_element(doc, lxb_dom_interface_element(parser->root));

    parser->tree->fragment = lxb_html_interface_create(new_doc, tag_id, ns);
    if (parser->tree->fragment == NULL) {
        parser->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        goto done;
    }

    /* Contains just the single element root */
    parser->status = lxb_html_tree_open_elements_push(parser->tree, parser->root);
    if (parser->status != LXB_STATUS_OK) {
        goto done;
    }

    if (tag_id == LXB_TAG_TEMPLATE && ns == LXB_NS_HTML) {
        parser->status = lxb_html_tree_template_insertion_push(parser->tree,
                                      lxb_html_tree_insertion_mode_in_template);
        if (parser->status != LXB_STATUS_OK) {
            goto done;
        }
    }

    lxb_html_tree_attach_document(parser->tree, new_doc);
    lxb_html_tree_reset_insertion_mode_appropriately(parser->tree);

    if (tag_id == LXB_TAG_FORM && ns == LXB_NS_HTML) {
        parser->form = lxb_html_interface_create(new_doc,
                                                 LXB_TAG_FORM, LXB_NS_HTML);
        if (parser->form == NULL) {
            parser->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

            goto done;
        }

        parser->tree->form = lxb_html_interface_form(parser->form);
    }

    parser->original_tree = lxb_html_tokenizer_tree(parser->tkz);
    lxb_html_tokenizer_tree_set(parser->tkz, parser->tree);

    lxb_html_tokenizer_tags_set(parser->tkz, doc->tags);
    lxb_html_tokenizer_attrs_set(parser->tkz, doc->attrs);
    lxb_html_tokenizer_attrs_mraw_set(parser->tkz, doc->text);

    parser->status = lxb_html_tree_begin(parser->tree, new_doc);

done:

    if (parser->status != LXB_STATUS_OK) {
        if (parser->root != NULL) {
            lxb_html_html_element_interface_destroy(lxb_html_interface_html(parser->root));
        }

        parser->state = LXB_HTML_PARSER_STATE_ERROR;
        parser->root = NULL;

        lxb_html_parse_fragment_chunk_destroy(parser);
    }

    return parser->status;
}

lxb_status_t
lxb_html_parse_fragment_chunk_process(lxb_html_parser_t *parser,
                                      const lxb_char_t *html, size_t size)
{
    if (parser->state != LXB_HTML_PARSER_STATE_FRAGMENT_PROCESS) {
        return LXB_STATUS_ERROR_WRONG_STAGE;
    }

    parser->status = lxb_html_tree_chunk(parser->tree, html, size);
    if (parser->status != LXB_STATUS_OK) {
        lxb_html_html_element_interface_destroy(lxb_html_interface_html(parser->root));

        parser->state = LXB_HTML_PARSER_STATE_ERROR;
        parser->root = NULL;

        lxb_html_parse_fragment_chunk_destroy(parser);
    }

    return parser->status;
}

lxb_dom_node_t *
lxb_html_parse_fragment_chunk_end(lxb_html_parser_t *parser)
{
    if (parser->state != LXB_HTML_PARSER_STATE_FRAGMENT_PROCESS) {
        parser->status = LXB_STATUS_ERROR_WRONG_STAGE;

        return NULL;
    }

    parser->status = lxb_html_tree_end(parser->tree);
    if (parser->status != LXB_STATUS_OK) {
        lxb_html_html_element_interface_destroy(lxb_html_interface_html(parser->root));

        parser->root = NULL;
    }

    lxb_html_parse_fragment_chunk_destroy(parser);

    lxb_html_tokenizer_tree_set(parser->tkz, parser->original_tree);

    parser->state = LXB_HTML_PARSER_STATE_END;

    return parser->root;
}

static void
lxb_html_parse_fragment_chunk_destroy(lxb_html_parser_t *parser)
{
    lxb_dom_document_t *doc;

    if (parser->form != NULL) {
        lxb_html_form_element_interface_destroy(lxb_html_interface_form(parser->form));

        parser->form = NULL;
    }

    if (parser->tree->fragment != NULL) {
        lxb_html_interface_destroy(parser->tree->fragment);

        parser->tree->fragment = NULL;
    }

    if (lxb_html_document_is_original(parser->tree->document) == false) {
        if (parser->root != NULL) {
            doc = lxb_dom_interface_node(parser->tree->document)->owner_document;
            parser->root->parent = &doc->node;
        }

        lxb_html_document_interface_destroy(parser->tree->document);

        parser->tree->document = NULL;
    }
}

LXB_API lxb_status_t
lxb_html_parse_chunk_prepare(lxb_html_parser_t *parser,
                             lxb_html_document_t *document)
{
    parser->state = LXB_HTML_PARSER_STATE_PROCESS;

    parser->original_tree = lxb_html_tokenizer_tree(parser->tkz);
    lxb_html_tokenizer_tree_set(parser->tkz, parser->tree);

    lxb_html_tokenizer_tags_set(parser->tkz, document->dom_document.tags);
    lxb_html_tokenizer_attrs_set(parser->tkz, document->dom_document.attrs);
    lxb_html_tokenizer_attrs_mraw_set(parser->tkz, document->dom_document.text);

    parser->status = lxb_html_tree_begin(parser->tree, document);
    if (parser->status != LXB_STATUS_OK) {
        parser->state = LXB_HTML_PARSER_STATE_ERROR;
    }

    return parser->status;
}

lxb_html_document_t *
lxb_html_parse_chunk_begin(lxb_html_parser_t *parser)
{
    lxb_html_document_t *document;

    if (parser->state != LXB_HTML_PARSER_STATE_BEGIN) {
        lxb_html_parser_clean(parser);
    }

    document = lxb_html_document_interface_create(NULL);
    if (document == NULL) {
        parser->state = LXB_HTML_PARSER_STATE_ERROR;
        parser->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return lxb_html_document_destroy(document);
    }

    document->dom_document.scripting = parser->tree->scripting;

    parser->status = lxb_html_parse_chunk_prepare(parser, document);
    if (parser->status != LXB_STATUS_OK) {
        return lxb_html_document_destroy(document);
    }

    return document;
}

lxb_status_t
lxb_html_parse_chunk_process(lxb_html_parser_t *parser,
                             const lxb_char_t *html, size_t size)
{
    if (parser->state != LXB_HTML_PARSER_STATE_PROCESS) {
        return LXB_STATUS_ERROR_WRONG_STAGE;
    }

    parser->status = lxb_html_tree_chunk(parser->tree, html, size);
    if (parser->status != LXB_STATUS_OK) {
        parser->state = LXB_HTML_PARSER_STATE_ERROR;
    }

    return parser->status;
}

lxb_status_t
lxb_html_parse_chunk_end(lxb_html_parser_t *parser)
{
    if (parser->state != LXB_HTML_PARSER_STATE_PROCESS) {
        return LXB_STATUS_ERROR_WRONG_STAGE;
    }

    parser->status = lxb_html_tree_end(parser->tree);

    lxb_html_tokenizer_tree_set(parser->tkz, parser->original_tree);

    parser->state = LXB_HTML_PARSER_STATE_END;

    return parser->status;
}

/*
 * No inline functions for ABI.
 */
lxb_html_tokenizer_t *
lxb_html_parser_tokenizer_noi(lxb_html_parser_t *parser)
{
    return lxb_html_parser_tokenizer(parser);
}

lxb_html_tree_t *
lxb_html_parser_tree_noi(lxb_html_parser_t *parser)
{
    return lxb_html_parser_tree(parser);
}

lxb_status_t
lxb_html_parser_status_noi(lxb_html_parser_t *parser)
{
    return lxb_html_parser_status(parser);
}

lxb_status_t
lxb_html_parser_state_noi(lxb_html_parser_t *parser)
{
    return lxb_html_parser_state(parser);
}

bool
lxb_html_parser_scripting_noi(lxb_html_parser_t *parser)
{
    return lxb_html_parser_scripting(parser);
}

void
lxb_html_parser_scripting_set_noi(lxb_html_parser_t *parser, bool scripting)
{
    lxb_html_parser_scripting_set(parser, scripting);
}
