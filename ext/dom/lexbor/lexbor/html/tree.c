/*
 * Copyright (C) 2018-2022 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/dom/interfaces/document_fragment.h"
#include "lexbor/dom/interfaces/document_type.h"
#include "lexbor/dom/interfaces/comment.h"
#include "lexbor/dom/interfaces/text.h"

#include "lexbor/html/tree.h"
#include "lexbor/html/tree_res.h"
#include "lexbor/html/tree/insertion_mode.h"
#include "lexbor/html/tree/open_elements.h"
#include "lexbor/html/tree/active_formatting.h"
#include "lexbor/html/tree/template_insertion.h"
#include "lexbor/html/interface.h"
#include "lexbor/html/interface.h"
#include "lexbor/html/interfaces/template_element.h"
#include "lexbor/html/interfaces/unknown_element.h"
#include "lexbor/html/tokenizer/state_rawtext.h"
#include "lexbor/html/tokenizer/state_rcdata.h"


lxb_dom_attr_data_t *
lxb_dom_attr_local_name_append(lexbor_hash_t *hash,
                               const lxb_char_t *name, size_t length);

lxb_dom_attr_data_t *
lxb_dom_attr_qualified_name_append(lexbor_hash_t *hash, const lxb_char_t *name,
                                   size_t length);

const lxb_tag_data_t *
lxb_tag_append_lower(lexbor_hash_t *hash,
                     const lxb_char_t *name, size_t length);

static lxb_html_token_t *
lxb_html_tree_token_callback(lxb_html_tokenizer_t *tkz,
                             lxb_html_token_t *token, void *ctx);

static lxb_status_t
lxb_html_tree_insertion_mode(lxb_html_tree_t *tree, lxb_html_token_t *token);


lxb_html_tree_t *
lxb_html_tree_create(void)
{
    return lexbor_calloc(1, sizeof(lxb_html_tree_t));
}

lxb_status_t
lxb_html_tree_init(lxb_html_tree_t *tree, lxb_html_tokenizer_t *tkz)
{
    if (tree == NULL) {
        return LXB_STATUS_ERROR_OBJECT_IS_NULL;
    }

    if (tkz == NULL) {
        return LXB_STATUS_ERROR_WRONG_ARGS;
    }

    lxb_status_t status;

    /* Stack of open elements */
    tree->open_elements = lexbor_array_create();
    status = lexbor_array_init(tree->open_elements, 128);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    /* Stack of active formatting */
    tree->active_formatting = lexbor_array_create();
    status = lexbor_array_init(tree->active_formatting, 128);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    /* Stack of template insertion modes */
    tree->template_insertion_modes = lexbor_array_obj_create();
    status = lexbor_array_obj_init(tree->template_insertion_modes, 64,
                                   sizeof(lxb_html_tree_template_insertion_t));
    if (status != LXB_STATUS_OK) {
        return status;
    }

    /* Stack of pending table character tokens */
    tree->pending_table.text_list = lexbor_array_obj_create();
    status = lexbor_array_obj_init(tree->pending_table.text_list, 16,
                                   sizeof(lexbor_str_t));
    if (status != LXB_STATUS_OK) {
        return status;
    }

    /* Parse errors */
    tree->parse_errors = lexbor_array_obj_create();
    status = lexbor_array_obj_init(tree->parse_errors, 16,
                                                sizeof(lxb_html_tree_error_t));
    if (status != LXB_STATUS_OK) {
        return status;
    }

    tree->tkz_ref = lxb_html_tokenizer_ref(tkz);

    tree->document = NULL;
    tree->fragment = NULL;

    tree->form = NULL;

    tree->foster_parenting = false;
    tree->frameset_ok = true;

    tree->mode = lxb_html_tree_insertion_mode_initial;
    tree->before_append_attr = NULL;

    tree->status = LXB_STATUS_OK;

    tree->ref_count = 1;

    lxb_html_tokenizer_callback_token_done_set(tkz,
                                               lxb_html_tree_token_callback,
                                               tree);

    return LXB_STATUS_OK;
}

lxb_html_tree_t *
lxb_html_tree_ref(lxb_html_tree_t *tree)
{
    if (tree == NULL) {
        return NULL;
    }

    tree->ref_count++;

    return tree;
}

lxb_html_tree_t *
lxb_html_tree_unref(lxb_html_tree_t *tree)
{
    if (tree == NULL || tree->ref_count == 0) {
        return NULL;
    }

    tree->ref_count--;

    if (tree->ref_count == 0) {
        lxb_html_tree_destroy(tree);
    }

    return NULL;
}

void
lxb_html_tree_clean(lxb_html_tree_t *tree)
{
    lexbor_array_clean(tree->open_elements);
    lexbor_array_clean(tree->active_formatting);
    lexbor_array_obj_clean(tree->template_insertion_modes);
    lexbor_array_obj_clean(tree->pending_table.text_list);
    lexbor_array_obj_clean(tree->parse_errors);

    tree->document = NULL;
    tree->fragment = NULL;

    tree->form = NULL;

    tree->foster_parenting = false;
    tree->frameset_ok = true;

    tree->mode = lxb_html_tree_insertion_mode_initial;
    tree->before_append_attr = NULL;

    tree->status = LXB_STATUS_OK;
}

lxb_html_tree_t *
lxb_html_tree_destroy(lxb_html_tree_t *tree)
{
    if (tree == NULL) {
        return NULL;
    }

    tree->open_elements = lexbor_array_destroy(tree->open_elements, true);
    tree->active_formatting = lexbor_array_destroy(tree->active_formatting,
                                                   true);
    tree->template_insertion_modes = lexbor_array_obj_destroy(tree->template_insertion_modes,
                                                              true);
    tree->pending_table.text_list = lexbor_array_obj_destroy(tree->pending_table.text_list,
                                                             true);

    tree->parse_errors = lexbor_array_obj_destroy(tree->parse_errors, true);
    tree->tkz_ref = lxb_html_tokenizer_unref(tree->tkz_ref);

    return lexbor_free(tree);
}

static lxb_html_token_t *
lxb_html_tree_token_callback(lxb_html_tokenizer_t *tkz,
                             lxb_html_token_t *token, void *ctx)
{
    lxb_status_t status;

    status = lxb_html_tree_insertion_mode(ctx, token);
    if (status != LXB_STATUS_OK) {
        tkz->status = status;
        return NULL;
    }

    return token;
}

/* TODO: not complete!!! */
lxb_status_t
lxb_html_tree_stop_parsing(lxb_html_tree_t *tree)
{
    tree->document->ready_state = LXB_HTML_DOCUMENT_READY_STATE_COMPLETE;

    return LXB_STATUS_OK;
}

bool
lxb_html_tree_process_abort(lxb_html_tree_t *tree)
{
    if (tree->status == LXB_STATUS_OK) {
        tree->status = LXB_STATUS_ABORTED;
    }

    tree->open_elements->length = 0;
    tree->document->ready_state = LXB_HTML_DOCUMENT_READY_STATE_COMPLETE;

    return true;
}

void
lxb_html_tree_parse_error(lxb_html_tree_t *tree, lxb_html_token_t *token,
                          lxb_html_tree_error_id_t id)
{
    lxb_html_tree_error_add(tree->parse_errors, token, id);
}

bool
lxb_html_tree_construction_dispatcher(lxb_html_tree_t *tree,
                                      lxb_html_token_t *token)
{
    lxb_dom_node_t *adjusted;

    adjusted = lxb_html_tree_adjusted_current_node(tree);

    if (adjusted == NULL || adjusted->ns == LXB_NS_HTML) {
        return tree->mode(tree, token);
    }

    if (lxb_html_tree_mathml_text_integration_point(adjusted))
    {
        if ((token->type & LXB_HTML_TOKEN_TYPE_CLOSE) == 0
            && token->tag_id != LXB_TAG_MGLYPH
            && token->tag_id != LXB_TAG_MALIGNMARK)
        {
            return tree->mode(tree, token);
        }

        if (token->tag_id == LXB_TAG__TEXT) {
            return tree->mode(tree, token);
        }
    }

    if (adjusted->local_name == LXB_TAG_ANNOTATION_XML
        && adjusted->ns == LXB_NS_MATH
        && (token->type & LXB_HTML_TOKEN_TYPE_CLOSE) == 0
        && token->tag_id == LXB_TAG_SVG)
    {
        return tree->mode(tree, token);
    }

    if (lxb_html_tree_html_integration_point(adjusted)) {
        if ((token->type & LXB_HTML_TOKEN_TYPE_CLOSE) == 0
            || token->tag_id == LXB_TAG__TEXT)
        {
            return tree->mode(tree, token);
        }
    }

    if (token->tag_id == LXB_TAG__END_OF_FILE) {
        return tree->mode(tree, token);
    }

    return lxb_html_tree_insertion_mode_foreign_content(tree, token);
}

static lxb_status_t
lxb_html_tree_insertion_mode(lxb_html_tree_t *tree, lxb_html_token_t *token)
{
    while (lxb_html_tree_construction_dispatcher(tree, token) == false) {}

    return tree->status;
}

/*
 * Action
 */
lxb_dom_node_t *
lxb_html_tree_appropriate_place_inserting_node(lxb_html_tree_t *tree,
                                       lxb_dom_node_t *override_target,
                                       lxb_html_tree_insertion_position_t *ipos)
{
    lxb_dom_node_t *target, *adjusted_location = NULL;

    *ipos = LXB_HTML_TREE_INSERTION_POSITION_CHILD;

    if (override_target != NULL) {
        target = override_target;
    }
    else {
        target = lxb_html_tree_current_node(tree);
    }

    if (tree->foster_parenting && target->ns == LXB_NS_HTML
           && (target->local_name == LXB_TAG_TABLE
            || target->local_name == LXB_TAG_TBODY
            || target->local_name == LXB_TAG_TFOOT
            || target->local_name == LXB_TAG_THEAD
            || target->local_name == LXB_TAG_TR))
    {
        lxb_dom_node_t *last_temp, *last_table;
        size_t last_temp_idx, last_table_idx;

        last_temp = lxb_html_tree_open_elements_find_reverse(tree,
                                                          LXB_TAG_TEMPLATE,
                                                          LXB_NS_HTML,
                                                          &last_temp_idx);

        last_table = lxb_html_tree_open_elements_find_reverse(tree,
                                                             LXB_TAG_TABLE,
                                                             LXB_NS_HTML,
                                                             &last_table_idx);

        if(last_temp != NULL && (last_table == NULL
                         || last_temp_idx > last_table_idx))
        {
            lxb_dom_document_fragment_t *doc_fragment;

            doc_fragment = lxb_html_interface_template(last_temp)->content;

            return lxb_dom_interface_node(doc_fragment);
        }
        else if (last_table == NULL) {
            adjusted_location = lxb_html_tree_open_elements_first(tree);

            lexbor_assert(adjusted_location != NULL);
            lexbor_assert(adjusted_location->local_name == LXB_TAG_HTML);
        }
        else if (last_table->parent != NULL) {
            adjusted_location = last_table;

            *ipos = LXB_HTML_TREE_INSERTION_POSITION_BEFORE;
        }
        else {
            lexbor_assert(last_table_idx != 0);

            adjusted_location = lxb_html_tree_open_elements_get(tree,
                                                            last_table_idx - 1);
        }
    }
    else {
        adjusted_location = target;
    }

    if (adjusted_location == NULL) {
        return NULL;
    }

    /*
     * In Spec it is not entirely clear what is meant:
     *
     * If the adjusted insertion location is inside a template element,
     * let it instead be inside the template element's template contents,
     * after its last child (if any).
     */
    if (lxb_html_tree_node_is(adjusted_location, LXB_TAG_TEMPLATE)) {
        lxb_dom_document_fragment_t *df;

        df = lxb_html_interface_template(adjusted_location)->content;
        adjusted_location = lxb_dom_interface_node(df);
    }

    return adjusted_location;
}

lxb_html_element_t *
lxb_html_tree_insert_foreign_element(lxb_html_tree_t *tree,
                                     lxb_html_token_t *token, lxb_ns_id_t ns)
{
    lxb_status_t status;
    lxb_dom_node_t *pos;
    lxb_html_element_t *element;
    lxb_html_tree_insertion_position_t ipos;

    pos = lxb_html_tree_appropriate_place_inserting_node(tree, NULL, &ipos);

    if (ipos == LXB_HTML_TREE_INSERTION_POSITION_CHILD) {
        element = lxb_html_tree_create_element_for_token(tree, token, ns, pos);
    }
    else {
        element = lxb_html_tree_create_element_for_token(tree, token, ns,
                                                         pos->parent);
    }

    if (element == NULL) {
        return NULL;
    }

    if (pos != NULL) {
        lxb_html_tree_insert_node(pos, lxb_dom_interface_node(element), ipos);
    }

    status = lxb_html_tree_open_elements_push(tree,
                                              lxb_dom_interface_node(element));
    if (status != LXB_HTML_STATUS_OK) {
        return lxb_html_interface_destroy(element);
    }

    return element;
}

lxb_html_element_t *
lxb_html_tree_create_element_for_token(lxb_html_tree_t *tree,
                                       lxb_html_token_t *token, lxb_ns_id_t ns,
                                       lxb_dom_node_t *parent)
{
    lxb_dom_node_t *node = lxb_html_tree_create_node(tree, token->tag_id, ns);
    if (node == NULL) {
        return NULL;
    }

    node->line = token->line;
    /* We only expose line number in PHP DOM */

    lxb_status_t status;
    lxb_dom_element_t *element = lxb_dom_interface_element(node);

    if (token->base_element == NULL) {
        status = lxb_html_tree_append_attributes(tree, element, token, ns);
    }
    else {
        status = lxb_html_tree_append_attributes_from_element(tree, element,
                                                       token->base_element, ns);
    }

    if (status != LXB_HTML_STATUS_OK) {
        return lxb_html_interface_destroy(element);
    }

    return lxb_html_interface_element(node);
}

lxb_status_t
lxb_html_tree_append_attributes(lxb_html_tree_t *tree,
                                lxb_dom_element_t *element,
                                lxb_html_token_t *token, lxb_ns_id_t ns)
{
    lxb_status_t status;
    lxb_dom_attr_t *attr;
    lxb_html_document_t *doc;
    lxb_html_token_attr_t *token_attr = token->attr_first;

    doc = lxb_html_interface_document(element->node.owner_document);

    while (token_attr != NULL) {
        attr = lxb_dom_element_attr_by_local_name_data(element,
                                                       token_attr->name);
        if (attr != NULL) {
            token_attr = token_attr->next;
            continue;
        }

        attr = lxb_dom_attr_interface_create(lxb_dom_interface_document(doc));
        if (attr == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }

        if (token_attr->value_begin != NULL) {
            status = lxb_dom_attr_set_value_wo_copy(attr, token_attr->value,
                                                    token_attr->value_size);
            if (status != LXB_HTML_STATUS_OK) {
                return status;
            }
        }

        attr->node.local_name = token_attr->name->attr_id;
        attr->node.ns = ns;

        /* Fix for adjust MathML/SVG attributes */
        if (tree->before_append_attr != NULL) {
            status = tree->before_append_attr(tree, attr, NULL);
            if (status != LXB_STATUS_OK) {
                return status;
            }
        }

        lxb_dom_element_attr_append(element, attr);

        token_attr = token_attr->next;
    }

    return LXB_HTML_STATUS_OK;
}

lxb_status_t
lxb_html_tree_append_attributes_from_element(lxb_html_tree_t *tree,
                                             lxb_dom_element_t *element,
                                             lxb_dom_element_t *from,
                                             lxb_ns_id_t ns)
{
    lxb_status_t status;
    lxb_dom_attr_t *attr = from->first_attr;
    lxb_dom_attr_t *new_attr;

    while (attr != NULL) {
        new_attr = lxb_dom_attr_interface_create(element->node.owner_document);
        if (new_attr == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }

        status = lxb_dom_attr_clone_name_value(attr, new_attr);
        if (status != LXB_HTML_STATUS_OK) {
            return status;
        }

        new_attr->node.ns = attr->node.ns;

        /* Fix for  adjust MathML/SVG attributes */
        if (tree->before_append_attr != NULL) {
            status = tree->before_append_attr(tree, new_attr, NULL);
            if (status != LXB_STATUS_OK) {
                return status;
            }
        }

        lxb_dom_element_attr_append(element, attr);

        attr = attr->next;
    }

    return LXB_HTML_STATUS_OK;
}

lxb_status_t
lxb_html_tree_adjust_mathml_attributes(lxb_html_tree_t *tree,
                                       lxb_dom_attr_t *attr, void *ctx)
{
    lexbor_hash_t *attrs;
    const lxb_dom_attr_data_t *data;

    attrs = attr->node.owner_document->attrs;
    data = lxb_dom_attr_data_by_id(attrs, attr->node.local_name);

    if (data->entry.length == 13
        && lexbor_str_data_cmp(lexbor_hash_entry_str(&data->entry),
                               (const lxb_char_t *) "definitionurl"))
    {
        data = lxb_dom_attr_qualified_name_append(attrs,
                                      (const lxb_char_t *) "definitionURL", 13);
        if (data == NULL) {
            return LXB_STATUS_ERROR;
        }

        attr->qualified_name = data->attr_id;
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_html_tree_adjust_svg_attributes(lxb_html_tree_t *tree,
                                    lxb_dom_attr_t *attr, void *ctx)
{
    lexbor_hash_t *attrs;
    const lxb_dom_attr_data_t *data;
    const lxb_html_tree_res_attr_adjust_t *adjust;

    size_t len = sizeof(lxb_html_tree_res_attr_adjust_svg_map)
        / sizeof(lxb_html_tree_res_attr_adjust_t);

    attrs = attr->node.owner_document->attrs;

    data = lxb_dom_attr_data_by_id(attrs, attr->node.local_name);

    for (size_t i = 0; i < len; i++) {
        adjust = &lxb_html_tree_res_attr_adjust_svg_map[i];

        if (data->entry.length == adjust->len
            && lexbor_str_data_cmp(lexbor_hash_entry_str(&data->entry),
                                   (const lxb_char_t *) adjust->from))
        {
            data = lxb_dom_attr_qualified_name_append(attrs,
                                (const lxb_char_t *) adjust->to, adjust->len);
            if (data == NULL) {
                return LXB_STATUS_ERROR;
            }

            attr->qualified_name = data->attr_id;

            return LXB_STATUS_OK;
        }
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_html_tree_adjust_foreign_attributes(lxb_html_tree_t *tree,
                                        lxb_dom_attr_t *attr, void *ctx)
{
    size_t lname_length;
    lexbor_hash_t *attrs, *prefix;
    const lxb_dom_attr_data_t *attr_data;
    const lxb_ns_prefix_data_t *prefix_data;
    const lxb_dom_attr_data_t *data;
    const lxb_html_tree_res_attr_adjust_foreign_t *adjust;

    size_t len = sizeof(lxb_html_tree_res_attr_adjust_foreign_map)
        / sizeof(lxb_html_tree_res_attr_adjust_foreign_t);

    attrs = attr->node.owner_document->attrs;
    prefix = attr->node.owner_document->prefix;

    data = lxb_dom_attr_data_by_id(attrs, attr->node.local_name);

    for (size_t i = 0; i < len; i++) {
        adjust = &lxb_html_tree_res_attr_adjust_foreign_map[i];

        if (data->entry.length == adjust->name_len
            && lexbor_str_data_cmp(lexbor_hash_entry_str(&data->entry),
                                   (const lxb_char_t *) adjust->name))
        {
            if (adjust->prefix_len != 0) {
                data = lxb_dom_attr_qualified_name_append(attrs,
                           (const lxb_char_t *) adjust->name, adjust->name_len);
                if (data == NULL) {
                    return LXB_STATUS_ERROR;
                }

                attr->qualified_name = data->attr_id;

                lname_length = adjust->name_len - adjust->prefix_len - 1;

                attr_data = lxb_dom_attr_local_name_append(attrs,
                         (const lxb_char_t *) adjust->local_name, lname_length);
                if (attr_data == NULL) {
                    return LXB_STATUS_ERROR;
                }

                attr->node.local_name = attr_data->attr_id;

                prefix_data = lxb_ns_prefix_append(prefix,
                       (const lxb_char_t *) adjust->prefix, adjust->prefix_len);
                if (prefix_data == NULL) {
                    return LXB_STATUS_ERROR;
                }

                attr->node.prefix = prefix_data->prefix_id;
            }

            attr->node.ns = adjust->ns;

            return LXB_STATUS_OK;
        }
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_html_tree_insert_character(lxb_html_tree_t *tree, lxb_html_token_t *token,
                               lxb_dom_node_t **ret_node)
{
    size_t size;
    lxb_status_t status;
    lexbor_str_t str = {0};

    size = token->text_end - token->text_start;

    lexbor_str_init(&str, tree->document->dom_document.text, size + 1);
    if (str.data == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    memcpy(str.data, token->text_start, size);

    str.data[size] = 0x00;
    str.length = size;

    status = lxb_html_tree_insert_character_for_data(tree, &str, ret_node);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_html_tree_insert_character_for_data(lxb_html_tree_t *tree,
                                        lexbor_str_t *str,
                                        lxb_dom_node_t **ret_node)
{
    const lxb_char_t *data;
    lxb_dom_node_t *pos;
    lxb_dom_character_data_t *chrs = NULL;
    lxb_html_tree_insertion_position_t ipos;

    if (ret_node != NULL) {
        *ret_node = NULL;
    }

    pos = lxb_html_tree_appropriate_place_inserting_node(tree, NULL, &ipos);
    if (pos == NULL) {
        return LXB_STATUS_ERROR;
    }

    if (lxb_html_tree_node_is(pos, LXB_TAG__DOCUMENT)) {
        goto destroy_str;
    }

    if (ipos == LXB_HTML_TREE_INSERTION_POSITION_BEFORE) {
        /* No need check namespace */
        if (pos->prev != NULL && pos->prev->local_name == LXB_TAG__TEXT) {
            chrs = lxb_dom_interface_character_data(pos->prev);

            if (ret_node != NULL) {
                *ret_node = pos->prev;
            }
        }
    }
    else {
        /* No need check namespace */
        if (pos->last_child != NULL
            && pos->last_child->local_name == LXB_TAG__TEXT)
        {
            chrs = lxb_dom_interface_character_data(pos->last_child);

            if (ret_node != NULL) {
                *ret_node = pos->last_child;
            }
        }
    }

    if (chrs != NULL) {
        /* This is error. This can not happen, but... */
        if (chrs->data.data == NULL) {
            data = lexbor_str_init(&chrs->data, tree->document->dom_document.text,
                                   str->length);
            if (data == NULL) {
                return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
            }
        }

        data = lexbor_str_append(&chrs->data, tree->document->dom_document.text,
                                 str->data, str->length);
        if (data == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }

        goto destroy_str;
    }

    lxb_dom_node_t *text = lxb_html_tree_create_node(tree, LXB_TAG__TEXT,
                                                     LXB_NS_HTML);
    if (text == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    lxb_dom_interface_text(text)->char_data.data = *str;

    if (tree->tkz_ref) {
        text->line = tree->tkz_ref->token->line;
        /* We only expose line number in PHP DOM */
    }

    if (ret_node != NULL) {
        *ret_node = text;
    }

    lxb_html_tree_insert_node(pos, text, ipos);

    return LXB_STATUS_OK;

destroy_str:

    lexbor_str_destroy(str, tree->document->dom_document.text, false);

    return LXB_STATUS_OK;
}

lxb_dom_comment_t *
lxb_html_tree_insert_comment(lxb_html_tree_t *tree,
                             lxb_html_token_t *token, lxb_dom_node_t *pos)
{
    lxb_dom_node_t *node;
    lxb_dom_comment_t *comment;
    lxb_html_tree_insertion_position_t ipos;

    if (pos == NULL) {
        pos = lxb_html_tree_appropriate_place_inserting_node(tree, NULL, &ipos);
    }
    else {
        ipos = LXB_HTML_TREE_INSERTION_POSITION_CHILD;
    }

    lexbor_assert(pos != NULL);

    node = lxb_html_tree_create_node(tree, token->tag_id, pos->ns);
    comment = lxb_dom_interface_comment(node);

    if (comment == NULL) {
        return NULL;
    }

    node->line = token->line;
    /* We only expose line number in PHP DOM */

    tree->status = lxb_html_token_make_text(token, &comment->char_data.data,
                                            tree->document->dom_document.text);
    if (tree->status != LXB_STATUS_OK) {
        return NULL;
    }

    lxb_html_tree_insert_node(pos, node, ipos);

    return comment;
}

lxb_dom_document_type_t *
lxb_html_tree_create_document_type_from_token(lxb_html_tree_t *tree,
                                              lxb_html_token_t *token)
{
    lxb_status_t status;
    lxb_dom_node_t *doctype_node;
    lxb_dom_document_type_t *doc_type;

    /* Create */
    doctype_node = lxb_html_tree_create_node(tree, token->tag_id, LXB_NS_HTML);
    if (doctype_node == NULL) {
        return NULL;
    }

    doc_type = lxb_dom_interface_document_type(doctype_node);

    /* Parse */
    status = lxb_html_token_doctype_parse(token, doc_type);
    if (status != LXB_STATUS_OK) {
        return lxb_dom_document_type_interface_destroy(doc_type);
    }

    return doc_type;
}

/*
 * TODO: need use ref and unref for nodes (ref counter)
 * Not implemented until the end. It is necessary to finish it.
 */
void
lxb_html_tree_node_delete_deep(lxb_html_tree_t *tree, lxb_dom_node_t *node)
{
    lxb_dom_node_remove(node);
}

lxb_html_element_t *
lxb_html_tree_generic_rawtext_parsing(lxb_html_tree_t *tree,
                                      lxb_html_token_t *token)
{
    lxb_html_element_t *element;

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        return NULL;
    }

    /*
     * Need for tokenizer state RAWTEXT
     * See description for 'lxb_html_tokenizer_state_rawtext_before' function
     */
    lxb_html_tokenizer_tmp_tag_id_set(tree->tkz_ref, token->tag_id);
    lxb_html_tokenizer_state_set(tree->tkz_ref,
                                 lxb_html_tokenizer_state_rawtext_before);

    tree->original_mode = tree->mode;
    tree->mode = lxb_html_tree_insertion_mode_text;

    return element;
}

/* Magic of CopyPast power! */
lxb_html_element_t *
lxb_html_tree_generic_rcdata_parsing(lxb_html_tree_t *tree,
                                     lxb_html_token_t *token)
{
    lxb_html_element_t *element;

    element = lxb_html_tree_insert_html_element(tree, token);
    if (element == NULL) {
        return NULL;
    }

    /*
     * Need for tokenizer state RCDATA
     * See description for 'lxb_html_tokenizer_state_rcdata_before' function
     */
    lxb_html_tokenizer_tmp_tag_id_set(tree->tkz_ref, token->tag_id);
    lxb_html_tokenizer_state_set(tree->tkz_ref,
                                 lxb_html_tokenizer_state_rcdata_before);

    tree->original_mode = tree->mode;
    tree->mode = lxb_html_tree_insertion_mode_text;

    return element;
}

void
lxb_html_tree_generate_implied_end_tags(lxb_html_tree_t *tree,
                                        lxb_tag_id_t ex_tag, lxb_ns_id_t ex_ns)
{
    lxb_dom_node_t *node;

    lexbor_assert(tree->open_elements != 0);

    while (lexbor_array_length(tree->open_elements) != 0) {
        node = lxb_html_tree_current_node(tree);

        lexbor_assert(node != NULL);

        switch (node->local_name) {
            case LXB_TAG_DD:
            case LXB_TAG_DT:
            case LXB_TAG_LI:
            case LXB_TAG_OPTGROUP:
            case LXB_TAG_OPTION:
            case LXB_TAG_P:
            case LXB_TAG_RB:
            case LXB_TAG_RP:
            case LXB_TAG_RT:
            case LXB_TAG_RTC:
                if(node->local_name == ex_tag && node->ns == ex_ns) {
                    return;
                }

                lxb_html_tree_open_elements_pop(tree);

                break;

            default:
                return;
        }
    }
}

void
lxb_html_tree_generate_all_implied_end_tags_thoroughly(lxb_html_tree_t *tree,
                                                       lxb_tag_id_t ex_tag,
                                                       lxb_ns_id_t ex_ns)
{
    lxb_dom_node_t *node;

    lexbor_assert(tree->open_elements != 0);

    while (lexbor_array_length(tree->open_elements) != 0) {
        node = lxb_html_tree_current_node(tree);

        lexbor_assert(node != NULL);

        switch (node->local_name) {
            case LXB_TAG_CAPTION:
            case LXB_TAG_COLGROUP:
            case LXB_TAG_DD:
            case LXB_TAG_DT:
            case LXB_TAG_LI:
            case LXB_TAG_OPTGROUP:
            case LXB_TAG_OPTION:
            case LXB_TAG_P:
            case LXB_TAG_RB:
            case LXB_TAG_RP:
            case LXB_TAG_RT:
            case LXB_TAG_RTC:
            case LXB_TAG_TBODY:
            case LXB_TAG_TD:
            case LXB_TAG_TFOOT:
            case LXB_TAG_TH:
            case LXB_TAG_THEAD:
            case LXB_TAG_TR:
                if(node->local_name == ex_tag && node->ns == ex_ns) {
                    return;
                }

                lxb_html_tree_open_elements_pop(tree);

                break;

            default:
                return;
        }
    }
}

void
lxb_html_tree_reset_insertion_mode_appropriately(lxb_html_tree_t *tree)
{
    lxb_dom_node_t *node;
    size_t idx = tree->open_elements->length;

    /* Step 1 */
    bool last = false;
    void **list = tree->open_elements->list;

    /* Step 3 */
    while (idx != 0) {
        idx--;

        /* Step 2 */
        node = list[idx];

        /* Step 3 */
        if (idx == 0) {
            last = true;

            if (tree->fragment != NULL) {
                node = tree->fragment;
            }
        }

        lexbor_assert(node != NULL);

        /* Step 16 */
        if (node->ns != LXB_NS_HTML) {
            if (last) {
                tree->mode = lxb_html_tree_insertion_mode_in_body;
                return;
            }

            continue;
        }

        /* Step 4 */
        if (node->local_name == LXB_TAG_SELECT) {
            /* Step 4.1 */
            if (last) {
                tree->mode = lxb_html_tree_insertion_mode_in_select;
                return;
            }

            /* Step 4.2 */
            size_t ancestor = idx;

            for (;;) {
                /* Step 4.3 */
                if (ancestor == 0) {
                    tree->mode = lxb_html_tree_insertion_mode_in_select;
                    return;
                }

                /* Step 4.4 */
                ancestor--;

                /* Step 4.5 */
                lxb_dom_node_t *ancestor_node = list[ancestor];

                if(lxb_html_tree_node_is(ancestor_node, LXB_TAG_TEMPLATE)) {
                    tree->mode = lxb_html_tree_insertion_mode_in_select;
                    return;
                }

                /* Step 4.6 */
                else if(lxb_html_tree_node_is(ancestor_node, LXB_TAG_TABLE)) {
                    tree->mode = lxb_html_tree_insertion_mode_in_select_in_table;
                    return;
                }
            }
        }

        /* Step 5-15 */
        switch (node->local_name) {
            case LXB_TAG_TD:
            case LXB_TAG_TH:
                if (last == false) {
                    tree->mode = lxb_html_tree_insertion_mode_in_cell;
                    return;
                }

                break;

            case LXB_TAG_TR:
                tree->mode = lxb_html_tree_insertion_mode_in_row;
                return;

            case LXB_TAG_TBODY:
            case LXB_TAG_TFOOT:
            case LXB_TAG_THEAD:
                tree->mode = lxb_html_tree_insertion_mode_in_table_body;
                return;

            case LXB_TAG_CAPTION:
                tree->mode = lxb_html_tree_insertion_mode_in_caption;
                return;

            case LXB_TAG_COLGROUP:
                tree->mode = lxb_html_tree_insertion_mode_in_column_group;
                return;

            case LXB_TAG_TABLE:
                tree->mode = lxb_html_tree_insertion_mode_in_table;
                return;

            case LXB_TAG_TEMPLATE:
                tree->mode = lxb_html_tree_template_insertion_current(tree);

                lexbor_assert(tree->mode != NULL);

                return;

            case LXB_TAG_HEAD:
                if (last == false) {
                    tree->mode = lxb_html_tree_insertion_mode_in_head;
                    return;
                }

                break;

            case LXB_TAG_BODY:
                tree->mode = lxb_html_tree_insertion_mode_in_body;
                return;

            case LXB_TAG_FRAMESET:
                tree->mode = lxb_html_tree_insertion_mode_in_frameset;
                return;

            case LXB_TAG_HTML: {
                if (tree->document->head == NULL) {
                    tree->mode = lxb_html_tree_insertion_mode_before_head;
                    return;
                }

                tree->mode = lxb_html_tree_insertion_mode_after_head;
                return;
            }

            default:
                break;
        }

        /* Step 16 */
        if (last) {
            tree->mode = lxb_html_tree_insertion_mode_in_body;
            return;
        }
    }
}

lxb_dom_node_t *
lxb_html_tree_element_in_scope(lxb_html_tree_t *tree, lxb_tag_id_t tag_id,
                               lxb_ns_id_t ns, lxb_html_tag_category_t ct)
{
    lxb_dom_node_t *node;

    size_t idx = tree->open_elements->length;
    void **list = tree->open_elements->list;

    while (idx != 0) {
        idx--;
        node = list[idx];

        if (node->local_name == tag_id && node->ns == ns) {
            return node;
        }

        if (lxb_html_tag_is_category(node->local_name, node->ns, ct)) {
            return NULL;
        }
    }

    return NULL;
}

lxb_dom_node_t *
lxb_html_tree_element_in_scope_by_node(lxb_html_tree_t *tree,
                                       lxb_dom_node_t *by_node,
                                       lxb_html_tag_category_t ct)
{
    lxb_dom_node_t *node;

    size_t idx = tree->open_elements->length;
    void **list = tree->open_elements->list;

    while (idx != 0) {
        idx--;
        node = list[idx];

        if (node == by_node) {
            return node;
        }

        if (lxb_html_tag_is_category(node->local_name, node->ns, ct)) {
            return NULL;
        }
    }

    return NULL;
}

lxb_dom_node_t *
lxb_html_tree_element_in_scope_h123456(lxb_html_tree_t *tree)
{
    lxb_dom_node_t *node;

    size_t idx = tree->open_elements->length;
    void **list = tree->open_elements->list;

    while (idx != 0) {
        idx--;
        node = list[idx];

        switch (node->local_name) {
            case LXB_TAG_H1:
            case LXB_TAG_H2:
            case LXB_TAG_H3:
            case LXB_TAG_H4:
            case LXB_TAG_H5:
            case LXB_TAG_H6:
                if (node->ns == LXB_NS_HTML) {
                    return node;
                }

                break;

            default:
                break;
        }

        if (lxb_html_tag_is_category(node->local_name, LXB_NS_HTML,
                                     LXB_HTML_TAG_CATEGORY_SCOPE))
        {
            return NULL;
        }
    }

    return NULL;
}

lxb_dom_node_t *
lxb_html_tree_element_in_scope_tbody_thead_tfoot(lxb_html_tree_t *tree)
{
    lxb_dom_node_t *node;

    size_t idx = tree->open_elements->length;
    void **list = tree->open_elements->list;

    while (idx != 0) {
        idx--;
        node = list[idx];

        switch (node->local_name) {
            case LXB_TAG_TBODY:
            case LXB_TAG_THEAD:
            case LXB_TAG_TFOOT:
                if (node->ns == LXB_NS_HTML) {
                    return node;
                }

                break;

            default:
                break;
        }

        if (lxb_html_tag_is_category(node->local_name, LXB_NS_HTML,
                                     LXB_HTML_TAG_CATEGORY_SCOPE_TABLE))
        {
            return NULL;
        }
    }

    return NULL;
}

lxb_dom_node_t *
lxb_html_tree_element_in_scope_td_th(lxb_html_tree_t *tree)
{
    lxb_dom_node_t *node;

    size_t idx = tree->open_elements->length;
    void **list = tree->open_elements->list;

    while (idx != 0) {
        idx--;
        node = list[idx];

        switch (node->local_name) {
            case LXB_TAG_TD:
            case LXB_TAG_TH:
                if (node->ns == LXB_NS_HTML) {
                    return node;
                }

                break;

            default:
                break;
        }

        if (lxb_html_tag_is_category(node->local_name, LXB_NS_HTML,
                                     LXB_HTML_TAG_CATEGORY_SCOPE_TABLE))
        {
            return NULL;
        }
    }

    return NULL;
}

bool
lxb_html_tree_check_scope_element(lxb_html_tree_t *tree)
{
    lxb_dom_node_t *node;

    for (size_t i = 0; i < tree->open_elements->length; i++) {
        node = tree->open_elements->list[i];

        switch (node->local_name) {
            case LXB_TAG_DD:
            case LXB_TAG_DT:
            case LXB_TAG_LI:
            case LXB_TAG_OPTGROUP:
            case LXB_TAG_OPTION:
            case LXB_TAG_P:
            case LXB_TAG_RB:
            case LXB_TAG_RP:
            case LXB_TAG_RT:
            case LXB_TAG_RTC:
            case LXB_TAG_TBODY:
            case LXB_TAG_TD:
            case LXB_TAG_TFOOT:
            case LXB_TAG_TH:
            case LXB_TAG_THEAD:
            case LXB_TAG_TR:
            case LXB_TAG_BODY:
            case LXB_TAG_HTML:
                return true;

            default:
                break;
        }
    }

    return false;
}

void
lxb_html_tree_close_p_element(lxb_html_tree_t *tree, lxb_html_token_t *token)
{
    lxb_html_tree_generate_implied_end_tags(tree, LXB_TAG_P, LXB_NS_HTML);

    lxb_dom_node_t *node = lxb_html_tree_current_node(tree);

    if (lxb_html_tree_node_is(node, LXB_TAG_P) == false) {
        lxb_html_tree_parse_error(tree, token,
                                  LXB_HTML_RULES_ERROR_UNELINOPELST);
    }

    lxb_html_tree_open_elements_pop_until_tag_id(tree, LXB_TAG_P, LXB_NS_HTML,
                                                 true);
}

#include "lexbor/html/serialize.h"

bool
lxb_html_tree_adoption_agency_algorithm(lxb_html_tree_t *tree,
                                        lxb_html_token_t *token,
                                        lxb_status_t *status)
{
    lexbor_assert(tree->open_elements->length != 0);

    /* State 1 */
    bool is;
    short outer_loop;
    lxb_html_element_t *element;
    lxb_dom_node_t *node, *marker, **oel_list, **afe_list;

    lxb_tag_id_t subject = token->tag_id;

    oel_list = (lxb_dom_node_t **) tree->open_elements->list;
    afe_list = (lxb_dom_node_t **) tree->active_formatting->list;
    marker = (lxb_dom_node_t *) lxb_html_tree_active_formatting_marker();

    *status = LXB_STATUS_OK;

    /* State 2 */
    node = lxb_html_tree_current_node(tree);
    lexbor_assert(node != NULL);

    if (lxb_html_tree_node_is(node, subject)) {
        is = lxb_html_tree_active_formatting_find_by_node_reverse(tree, node,
                                                                  NULL);
        if (is == false) {
            lxb_html_tree_open_elements_pop(tree);

            return false;
        }
    }

    /* State 3 */
    outer_loop = 0;

    /* State 4 */
    while (outer_loop < 8) {
        /* State 5 */
        outer_loop++;

        /* State 6 */
        size_t formatting_index = 0;
        size_t idx = tree->active_formatting->length;
        lxb_dom_node_t *formatting_element = NULL;

        while (idx) {
            idx--;

            if (afe_list[idx] == marker) {
                    return true;
            }
            else if (afe_list[idx]->local_name == subject) {
                formatting_index = idx;
                formatting_element = afe_list[idx];

                break;
            }
        }

        if (formatting_element == NULL) {
            return true;
        }

        /* State 7 */
        size_t oel_formatting_idx;
        is = lxb_html_tree_open_elements_find_by_node_reverse(tree,
                                                              formatting_element,
                                                              &oel_formatting_idx);
        if (is == false) {
            lxb_html_tree_parse_error(tree, token,
                                      LXB_HTML_RULES_ERROR_MIELINOPELST);

            lxb_html_tree_active_formatting_remove_by_node(tree,
                                                           formatting_element);

            return false;
        }

        /* State 8 */
        node = lxb_html_tree_element_in_scope_by_node(tree, formatting_element,
                                                      LXB_HTML_TAG_CATEGORY_SCOPE);
        if (node == NULL) {
            lxb_html_tree_parse_error(tree, token,
                                      LXB_HTML_RULES_ERROR_MIELINSC);
            return false;
        }

        /* State 9 */
        node = lxb_html_tree_current_node(tree);

        if (formatting_element != node) {
            lxb_html_tree_parse_error(tree, token,
                                      LXB_HTML_RULES_ERROR_UNELINOPELST);
        }

        /* State 10 */
        lxb_dom_node_t *furthest_block = NULL;
        size_t furthest_block_idx = 0;
        size_t oel_idx = tree->open_elements->length;

        for (furthest_block_idx = oel_formatting_idx;
             furthest_block_idx < oel_idx;
             furthest_block_idx++)
        {
            is = lxb_html_tag_is_category(oel_list[furthest_block_idx]->local_name,
                                          oel_list[furthest_block_idx]->ns,
                                          LXB_HTML_TAG_CATEGORY_SPECIAL);
            if (is) {
                furthest_block = oel_list[furthest_block_idx];

                break;
            }
        }

        /* State 11 */
        if (furthest_block == NULL) {
            lxb_html_tree_open_elements_pop_until_node(tree, formatting_element,
                                                       true);

            lxb_html_tree_active_formatting_remove_by_node(tree,
                                                           formatting_element);

            return false;
        }

        lexbor_assert(oel_formatting_idx != 0);

        /* State 12 */
        lxb_dom_node_t *common_ancestor = oel_list[oel_formatting_idx - 1];

        /* State 13 */
        size_t bookmark = formatting_index;

        /* State 14 */
        lxb_dom_node_t *node;
        lxb_dom_node_t *last = furthest_block;
        size_t node_idx = furthest_block_idx;

        /* State 14.1 */
        size_t inner_loop_counter = 0;

        /* State 14.2 */
        while (1) {
            inner_loop_counter++;

            /* State 14.3 */
            lexbor_assert(node_idx != 0);

            if (node_idx == 0) {
                return false;
            }

            node_idx--;
            node = oel_list[node_idx];

            /* State 14.4 */
            if (node == formatting_element) {
                break;
            }

            /* State 14.5 */
            size_t afe_node_idx;
            is = lxb_html_tree_active_formatting_find_by_node_reverse(tree,
                                                                      node,
                                                                      &afe_node_idx);
            /* State 14.5 */
            if (inner_loop_counter > 3 && is) {
                lxb_html_tree_active_formatting_remove_by_node(tree, node);

                continue;
            }

            /* State 14.6 */
            if (is == false) {
                lxb_html_tree_open_elements_remove_by_node(tree, node);

                continue;
            }

            /* State 14.7 */
            lxb_html_token_t fake_token = {0};

            fake_token.tag_id = node->local_name;
            fake_token.base_element = node;

            element = lxb_html_tree_create_element_for_token(tree, &fake_token,
                                                             LXB_NS_HTML,
                                                             common_ancestor);
            if (element == NULL) {
                *status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

                return false;
            }

            node = lxb_dom_interface_node(element);

            afe_list[afe_node_idx] = node;
            oel_list[node_idx] = node;

            /* State 14.8 */
            if (last == furthest_block) {
                bookmark = afe_node_idx + 1;

                lexbor_assert(bookmark < tree->active_formatting->length);
            }

            /* State 14.9 */
            if (last->parent != NULL) {
                lxb_dom_node_remove(last);
            }

            lxb_dom_node_insert_child_wo_events(node, last);

            /* State 14.10 */
            last = node;
        }

        if (last->parent != NULL) {
            lxb_dom_node_remove(last);
        }

        /* State 15 */
        lxb_dom_node_t *pos;
        lxb_html_tree_insertion_position_t ipos;

        pos = lxb_html_tree_appropriate_place_inserting_node(tree,
                                                             common_ancestor,
                                                             &ipos);
        if (pos == NULL) {
            return false;
        }

        lxb_html_tree_insert_node(pos, last, ipos);

        /* State 16 */
        lxb_html_token_t fake_token = {0};

        fake_token.tag_id = formatting_element->local_name;
        fake_token.base_element = formatting_element;

        element = lxb_html_tree_create_element_for_token(tree, &fake_token,
                                                         LXB_NS_HTML,
                                                         furthest_block);
        if (element == NULL) {
            *status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

            return false;
        }

        /* State 17 */
        lxb_dom_node_t *next;
        node = furthest_block->first_child;

        while (node != NULL) {
            next = node->next;

            lxb_dom_node_remove(node);
            lxb_dom_node_insert_child_wo_events(lxb_dom_interface_node(element),
                                                node);
            node = next;
        }

        node = lxb_dom_interface_node(element);

        /* State 18 */
        lxb_dom_node_insert_child_wo_events(furthest_block, node);

        /* State 19 */
        lxb_html_tree_active_formatting_remove(tree, formatting_index);

        if (bookmark > tree->active_formatting->length) {
            bookmark = tree->active_formatting->length;
        }

        *status = lxb_html_tree_active_formatting_insert(tree, node, bookmark);
        if (*status != LXB_STATUS_OK) {
            return false;
        }

        /* State 20 */
        lxb_html_tree_open_elements_remove_by_node(tree, formatting_element);

        lxb_html_tree_open_elements_find_by_node(tree, furthest_block,
                                                 &furthest_block_idx);

        *status = lxb_html_tree_open_elements_insert_after(tree, node,
                                                           furthest_block_idx);
        if (*status != LXB_STATUS_OK) {
            return false;
        }
    }

    return false;
}

bool
lxb_html_tree_html_integration_point(lxb_dom_node_t *node)
{
    if (node->ns == LXB_NS_MATH
        && node->local_name == LXB_TAG_ANNOTATION_XML)
    {
        lxb_dom_attr_t *attr;
        attr = lxb_dom_element_attr_is_exist(lxb_dom_interface_element(node),
                                             (const lxb_char_t *) "encoding",
                                             8);
        if (attr == NULL || attr->value == NULL) {
            return false;
        }

        if (attr->value->length == 9
            && lexbor_str_data_casecmp(attr->value->data,
                                       (const lxb_char_t *) "text/html"))
        {
            return true;
        }

        if (attr->value->length == 21
            && lexbor_str_data_casecmp(attr->value->data,
                                       (const lxb_char_t *) "application/xhtml+xml"))
        {
            return true;
        }

        return false;
    }

    if (node->ns == LXB_NS_SVG
        && (node->local_name == LXB_TAG_FOREIGNOBJECT
            || node->local_name == LXB_TAG_DESC
            || node->local_name == LXB_TAG_TITLE))
    {
        return true;
    }

    return false;
}

lxb_status_t
lxb_html_tree_adjust_attributes_mathml(lxb_html_tree_t *tree,
                                       lxb_dom_attr_t *attr, void *ctx)
{
    lxb_status_t status;

    status = lxb_html_tree_adjust_mathml_attributes(tree, attr, ctx);
    if (status !=LXB_STATUS_OK) {
        return status;
    }

    return lxb_html_tree_adjust_foreign_attributes(tree, attr, ctx);
}

lxb_status_t
lxb_html_tree_adjust_attributes_svg(lxb_html_tree_t *tree,
                                    lxb_dom_attr_t *attr, void *ctx)
{
    lxb_status_t status;

    status = lxb_html_tree_adjust_svg_attributes(tree, attr, ctx);
    if (status !=LXB_STATUS_OK) {
        return status;
    }

    return lxb_html_tree_adjust_foreign_attributes(tree, attr, ctx);
}
