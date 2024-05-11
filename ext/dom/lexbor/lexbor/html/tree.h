/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_HTML_TREE_H
#define LEXBOR_HTML_TREE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/dom/interfaces/node.h"
#include "lexbor/dom/interfaces/attr.h"

#include "lexbor/html/base.h"
#include "lexbor/html/node.h"
#include "lexbor/html/tokenizer.h"
#include "lexbor/html/interfaces/document.h"
#include "lexbor/html/tag.h"
#include "lexbor/html/tree/error.h"


typedef bool
(*lxb_html_tree_insertion_mode_f)(lxb_html_tree_t *tree,
                                  lxb_html_token_t *token);

typedef lxb_status_t
(*lxb_html_tree_append_attr_f)(lxb_html_tree_t *tree,
                               lxb_dom_attr_t *attr, void *ctx);

typedef struct {
    lexbor_array_obj_t *text_list;
    bool               have_non_ws;
}
lxb_html_tree_pending_table_t;

struct lxb_html_tree {
    lxb_html_tokenizer_t           *tkz_ref;

    lxb_html_document_t            *document;
    lxb_dom_node_t                 *fragment;

    lxb_html_form_element_t        *form;

    lexbor_array_t                 *open_elements;
    lexbor_array_t                 *active_formatting;
    lexbor_array_obj_t             *template_insertion_modes;

    lxb_html_tree_pending_table_t  pending_table;

    lexbor_array_obj_t             *parse_errors;

    bool                           foster_parenting;
    bool                           frameset_ok;
    bool                           scripting;
    bool                           has_explicit_html_tag;
    bool                           has_explicit_head_tag;
    bool                           has_explicit_body_tag;

    lxb_html_tree_insertion_mode_f mode;
    lxb_html_tree_insertion_mode_f original_mode;
    lxb_html_tree_append_attr_f    before_append_attr;

    lxb_status_t                   status;

    size_t                         ref_count;
};

typedef enum {
    LXB_HTML_TREE_INSERTION_POSITION_CHILD  = 0x00,
    LXB_HTML_TREE_INSERTION_POSITION_BEFORE = 0x01
}
lxb_html_tree_insertion_position_t;


LXB_API lxb_html_tree_t *
lxb_html_tree_create(void);

LXB_API lxb_status_t
lxb_html_tree_init(lxb_html_tree_t *tree, lxb_html_tokenizer_t *tkz);

LXB_API lxb_html_tree_t *
lxb_html_tree_ref(lxb_html_tree_t *tree);

LXB_API lxb_html_tree_t *
lxb_html_tree_unref(lxb_html_tree_t *tree);

LXB_API void
lxb_html_tree_clean(lxb_html_tree_t *tree);

LXB_API lxb_html_tree_t *
lxb_html_tree_destroy(lxb_html_tree_t *tree);

LXB_API lxb_status_t
lxb_html_tree_stop_parsing(lxb_html_tree_t *tree);

LXB_API bool
lxb_html_tree_process_abort(lxb_html_tree_t *tree);

LXB_API void
lxb_html_tree_parse_error(lxb_html_tree_t *tree, lxb_html_token_t *token,
                          lxb_html_tree_error_id_t id);

LXB_API bool
lxb_html_tree_construction_dispatcher(lxb_html_tree_t *tree,
                                      lxb_html_token_t *token);

LXB_API lxb_dom_node_t *
lxb_html_tree_appropriate_place_inserting_node(lxb_html_tree_t *tree,
                                      lxb_dom_node_t *override_target,
                                      lxb_html_tree_insertion_position_t *ipos);

LXB_API lxb_html_element_t *
lxb_html_tree_insert_foreign_element(lxb_html_tree_t *tree,
                                     lxb_html_token_t *token, lxb_ns_id_t ns);

LXB_API lxb_html_element_t *
lxb_html_tree_create_element_for_token(lxb_html_tree_t *tree,
                                       lxb_html_token_t *token, lxb_ns_id_t ns,
                                       lxb_dom_node_t *parent);

LXB_API lxb_status_t
lxb_html_tree_append_attributes(lxb_html_tree_t *tree,
                                lxb_dom_element_t *element,
                                lxb_html_token_t *token, lxb_ns_id_t ns);

LXB_API lxb_status_t
lxb_html_tree_append_attributes_from_element(lxb_html_tree_t *tree,
                                             lxb_dom_element_t *element,
                                             lxb_dom_element_t *from,
                                             lxb_ns_id_t ns);

LXB_API lxb_status_t
lxb_html_tree_adjust_mathml_attributes(lxb_html_tree_t *tree,
                                       lxb_dom_attr_t *attr, void *ctx);

LXB_API lxb_status_t
lxb_html_tree_adjust_svg_attributes(lxb_html_tree_t *tree,
                                    lxb_dom_attr_t *attr, void *ctx);

LXB_API lxb_status_t
lxb_html_tree_adjust_foreign_attributes(lxb_html_tree_t *tree,
                                        lxb_dom_attr_t *attr, void *ctx);

LXB_API lxb_status_t
lxb_html_tree_insert_character(lxb_html_tree_t *tree, lxb_html_token_t *token,
                               lxb_dom_node_t **ret_node);

LXB_API lxb_status_t
lxb_html_tree_insert_character_for_data(lxb_html_tree_t *tree,
                                        lexbor_str_t *str,
                                        lxb_dom_node_t **ret_node);

LXB_API lxb_dom_comment_t *
lxb_html_tree_insert_comment(lxb_html_tree_t *tree,
                             lxb_html_token_t *token, lxb_dom_node_t *pos);

LXB_API lxb_dom_document_type_t *
lxb_html_tree_create_document_type_from_token(lxb_html_tree_t *tree,
                                              lxb_html_token_t *token);

LXB_API void
lxb_html_tree_node_delete_deep(lxb_html_tree_t *tree, lxb_dom_node_t *node);

LXB_API lxb_html_element_t *
lxb_html_tree_generic_rawtext_parsing(lxb_html_tree_t *tree,
                                      lxb_html_token_t *token);

LXB_API lxb_html_element_t *
lxb_html_tree_generic_rcdata_parsing(lxb_html_tree_t *tree,
                                     lxb_html_token_t *token);

LXB_API void
lxb_html_tree_generate_implied_end_tags(lxb_html_tree_t *tree,
                                        lxb_tag_id_t ex_tag, lxb_ns_id_t ex_ns);

LXB_API void
lxb_html_tree_generate_all_implied_end_tags_thoroughly(lxb_html_tree_t *tree,
                                                       lxb_tag_id_t ex_tag,
                                                       lxb_ns_id_t ex_ns);

LXB_API void
lxb_html_tree_reset_insertion_mode_appropriately(lxb_html_tree_t *tree);

LXB_API lxb_dom_node_t *
lxb_html_tree_element_in_scope(lxb_html_tree_t *tree, lxb_tag_id_t tag_id,
                               lxb_ns_id_t ns, lxb_html_tag_category_t ct);

LXB_API lxb_dom_node_t *
lxb_html_tree_element_in_scope_by_node(lxb_html_tree_t *tree,
                                       lxb_dom_node_t *by_node,
                                       lxb_html_tag_category_t ct);

LXB_API lxb_dom_node_t *
lxb_html_tree_element_in_scope_h123456(lxb_html_tree_t *tree);

LXB_API lxb_dom_node_t *
lxb_html_tree_element_in_scope_tbody_thead_tfoot(lxb_html_tree_t *tree);

LXB_API lxb_dom_node_t *
lxb_html_tree_element_in_scope_td_th(lxb_html_tree_t *tree);

LXB_API bool
lxb_html_tree_check_scope_element(lxb_html_tree_t *tree);

LXB_API void
lxb_html_tree_close_p_element(lxb_html_tree_t *tree, lxb_html_token_t *token);

LXB_API bool
lxb_html_tree_adoption_agency_algorithm(lxb_html_tree_t *tree,
                                        lxb_html_token_t *token,
                                        lxb_status_t *status);

LXB_API bool
lxb_html_tree_html_integration_point(lxb_dom_node_t *node);

LXB_API lxb_status_t
lxb_html_tree_adjust_attributes_mathml(lxb_html_tree_t *tree,
                                       lxb_dom_attr_t *attr, void *ctx);

LXB_API lxb_status_t
lxb_html_tree_adjust_attributes_svg(lxb_html_tree_t *tree,
                                    lxb_dom_attr_t *attr, void *ctx);


/*
 * Inline functions
 */
lxb_inline lxb_status_t
lxb_html_tree_begin(lxb_html_tree_t *tree, lxb_html_document_t *document)
{
    tree->document = document;

    return lxb_html_tokenizer_begin(tree->tkz_ref);
}

lxb_inline lxb_status_t
lxb_html_tree_chunk(lxb_html_tree_t *tree, const lxb_char_t *html, size_t size)
{
    return lxb_html_tokenizer_chunk(tree->tkz_ref, html, size);
}

lxb_inline lxb_status_t
lxb_html_tree_end(lxb_html_tree_t *tree)
{
    if (tree->document->done != NULL) {
        tree->document->done(tree->document);
    }

    return lxb_html_tokenizer_end(tree->tkz_ref);
}

lxb_inline lxb_status_t
lxb_html_tree_build(lxb_html_tree_t *tree, lxb_html_document_t *document,
                    const lxb_char_t *html, size_t size)
{
    tree->status = lxb_html_tree_begin(tree, document);
    if (tree->status != LXB_STATUS_OK) {
        return tree->status;
    }

    tree->status = lxb_html_tree_chunk(tree, html, size);
    if (tree->status != LXB_STATUS_OK) {
        return tree->status;
    }

    return lxb_html_tree_end(tree);
}

lxb_inline lxb_dom_node_t *
lxb_html_tree_create_node(lxb_html_tree_t *tree,
                          lxb_tag_id_t tag_id, lxb_ns_id_t ns)
{
    return (lxb_dom_node_t *) lxb_html_interface_create(tree->document,
                                                        tag_id, ns);
}

lxb_inline bool
lxb_html_tree_node_is(lxb_dom_node_t *node, lxb_tag_id_t tag_id)
{
    return node->local_name == tag_id && node->ns == LXB_NS_HTML;
}

lxb_inline lxb_dom_node_t *
lxb_html_tree_current_node(lxb_html_tree_t *tree)
{
    if (tree->open_elements->length == 0) {
        return NULL;
    }

    return (lxb_dom_node_t *)
        tree->open_elements->list[ (tree->open_elements->length - 1) ];
}

lxb_inline lxb_dom_node_t *
lxb_html_tree_adjusted_current_node(lxb_html_tree_t *tree)
{
    if(tree->fragment != NULL && tree->open_elements->length == 1) {
        return lxb_dom_interface_node(tree->fragment);
    }

    return lxb_html_tree_current_node(tree);
}

lxb_inline lxb_html_element_t *
lxb_html_tree_insert_html_element(lxb_html_tree_t *tree,
                                  lxb_html_token_t *token)
{
    return lxb_html_tree_insert_foreign_element(tree, token, LXB_NS_HTML);
}

lxb_inline void
lxb_html_tree_insert_node(lxb_dom_node_t *to, lxb_dom_node_t *node,
                          lxb_html_tree_insertion_position_t ipos)
{
    if (ipos == LXB_HTML_TREE_INSERTION_POSITION_BEFORE) {
        lxb_dom_node_insert_before_wo_events(to, node);
        return;
    }

    lxb_dom_node_insert_child_wo_events(to, node);
}

/* TODO: if we not need to save parse errors?! */
lxb_inline void
lxb_html_tree_acknowledge_token_self_closing(lxb_html_tree_t *tree,
                                             lxb_html_token_t *token)
{
    if ((token->type & LXB_HTML_TOKEN_TYPE_CLOSE_SELF) == 0) {
        return;
    }

    bool is_void = lxb_html_tag_is_void(token->tag_id);

    if (is_void) {
        lxb_html_tree_parse_error(tree, token,
                                  LXB_HTML_RULES_ERROR_NOVOHTELSTTAWITRSO);
    }
}

lxb_inline bool
lxb_html_tree_mathml_text_integration_point(lxb_dom_node_t *node)
{
    if (node->ns == LXB_NS_MATH) {
        switch (node->local_name) {
            case LXB_TAG_MI:
            case LXB_TAG_MO:
            case LXB_TAG_MN:
            case LXB_TAG_MS:
            case LXB_TAG_MTEXT:
                return true;
        }
    }

    return false;
}

lxb_inline bool
lxb_html_tree_scripting(lxb_html_tree_t *tree)
{
    return tree->scripting;
}

lxb_inline void
lxb_html_tree_scripting_set(lxb_html_tree_t *tree, bool scripting)
{
    tree->scripting = scripting;
}

lxb_inline void
lxb_html_tree_attach_document(lxb_html_tree_t *tree, lxb_html_document_t *doc)
{
    tree->document = doc;
}

/*
 * No inline functions for ABI.
 */
LXB_API lxb_status_t
lxb_html_tree_begin_noi(lxb_html_tree_t *tree, lxb_html_document_t *document);

LXB_API lxb_status_t
lxb_html_tree_chunk_noi(lxb_html_tree_t *tree, const lxb_char_t *html,
                        size_t size);

LXB_API lxb_status_t
lxb_html_tree_end_noi(lxb_html_tree_t *tree);

LXB_API lxb_status_t
lxb_html_tree_build_noi(lxb_html_tree_t *tree, lxb_html_document_t *document,
                        const lxb_char_t *html, size_t size);

LXB_API lxb_dom_node_t *
lxb_html_tree_create_node_noi(lxb_html_tree_t *tree,
                              lxb_tag_id_t tag_id, lxb_ns_id_t ns);

LXB_API bool
lxb_html_tree_node_is_noi(lxb_dom_node_t *node, lxb_tag_id_t tag_id);

LXB_API lxb_dom_node_t *
lxb_html_tree_current_node_noi(lxb_html_tree_t *tree);

LXB_API lxb_dom_node_t *
lxb_html_tree_adjusted_current_node_noi(lxb_html_tree_t *tree);

LXB_API lxb_html_element_t *
lxb_html_tree_insert_html_element_noi(lxb_html_tree_t *tree,
                                      lxb_html_token_t *token);

LXB_API void
lxb_html_tree_insert_node_noi(lxb_dom_node_t *to, lxb_dom_node_t *node,
                              lxb_html_tree_insertion_position_t ipos);

LXB_API void
lxb_html_tree_acknowledge_token_self_closing_noi(lxb_html_tree_t *tree,
                                             lxb_html_token_t *token);

LXB_API bool
lxb_html_tree_mathml_text_integration_point_noi(lxb_dom_node_t *node);

LXB_API bool
lxb_html_tree_scripting_noi(lxb_html_tree_t *tree);

LXB_API void
lxb_html_tree_scripting_set_noi(lxb_html_tree_t *tree, bool scripting);

LXB_API void
lxb_html_tree_attach_document_noi(lxb_html_tree_t *tree,
                                  lxb_html_document_t *doc);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_HTML_TREE_H */
