/*
 * Copyright (C) 2018-2022 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/style.h"
#include "lexbor/html/interfaces/element.h"
#include "lexbor/html/interfaces/document.h"


typedef struct {
    lexbor_str_t  *str;
    lexbor_mraw_t *mraw;
}
lxb_html_element_style_ctx_t;

typedef struct {
    lxb_html_element_t          *element;
    lxb_html_element_style_cb_f cb;
    void                        *ctx;
    bool                        weak;
}
lxb_html_element_walk_ctx_t;


static lxb_status_t
lxb_html_element_style_walk_cb(lexbor_avl_t *avl, lexbor_avl_node_t **root,
                               lexbor_avl_node_t *node, void *ctx);

static lxb_status_t
lxb_html_element_style_weak_append(lxb_html_document_t *doc,
                                   lxb_html_style_node_t *node,
                                   lxb_css_rule_declaration_t *declr,
                                   lxb_css_selector_specificity_t spec);

static lxb_status_t
lxb_html_element_style_serialize_cb(lexbor_avl_t *avl, lexbor_avl_node_t **root,
                                    lexbor_avl_node_t *node, void *ctx);

#if 0
static lxb_status_t
lxb_html_element_style_serialize_str_cb(const lxb_char_t *data,
                                        size_t len, void *ctx);
#endif


lxb_html_element_t *
lxb_html_element_interface_create(lxb_html_document_t *document)
{
    lxb_html_element_t *element;

    element = lexbor_mraw_calloc(document->dom_document.mraw,
                                 sizeof(lxb_html_element_t));
    if (element == NULL) {
        return NULL;
    }

    lxb_dom_node_t *node = lxb_dom_interface_node(element);

    node->owner_document = lxb_html_document_original_ref(document);
    node->type = LXB_DOM_NODE_TYPE_ELEMENT;

    return element;
}

lxb_html_element_t *
lxb_html_element_interface_destroy(lxb_html_element_t *element)
{
    (void) lxb_dom_node_interface_destroy(lxb_dom_interface_node(element));
    return NULL;
}

lxb_html_element_t *
lxb_html_element_inner_html_set(lxb_html_element_t *element,
                                const lxb_char_t *html, size_t size)
{
    lxb_dom_node_t *node, *child;
    lxb_dom_node_t *root = lxb_dom_interface_node(element);
    lxb_html_document_t *doc = lxb_html_interface_document(root->owner_document);

    node = lxb_html_document_parse_fragment(doc, &element->element, html, size);
    if (node == NULL) {
        return NULL;
    }

    while (root->first_child != NULL) {
        lxb_dom_node_destroy_deep(root->first_child);
    }

    while (node->first_child != NULL) {
        child = node->first_child;

        lxb_dom_node_remove(child);
        lxb_dom_node_insert_child(root, child);
    }

    lxb_dom_node_destroy(node);

    return lxb_html_interface_element(root);
}

const lxb_css_rule_declaration_t *
lxb_html_element_style_by_name(lxb_html_element_t *element,
                               const lxb_char_t *name, size_t size)
{
#if 0
    uintptr_t id;
    lxb_html_style_node_t *node;
    lxb_dom_document_t *ddoc = lxb_dom_interface_node(element)->owner_document;
    lxb_html_document_t *doc = lxb_html_interface_document(ddoc);

    id = lxb_html_style_id_by_name(doc, name, size);
    if (id == LXB_CSS_PROPERTY__UNDEF) {
        return NULL;
    }

    node = (void *) lexbor_avl_search(doc->css.styles, element->style, id);

    return (node != NULL) ? node->entry.value : NULL;
#endif
    return NULL;
}

const lxb_css_rule_declaration_t *
lxb_html_element_style_by_id(lxb_html_element_t *element, uintptr_t id)
{
#if 0
    const lxb_html_style_node_t *node;

    node = lxb_html_element_style_node_by_id(element, id);
    if (node == NULL) {
        return NULL;
    }

    return node->entry.value;
#endif
    return NULL;
}

const lxb_html_style_node_t *
lxb_html_element_style_node_by_id(lxb_html_element_t *element, uintptr_t id)
{
    lxb_dom_document_t *ddoc = lxb_dom_interface_node(element)->owner_document;
    lxb_html_document_t *doc = lxb_html_interface_document(ddoc);

    return (lxb_html_style_node_t *) lexbor_avl_search(doc->css.styles,
                                                       element->style, id);
}

const lxb_html_style_node_t *
lxb_html_element_style_node_by_name(lxb_html_element_t *element,
                                    const lxb_char_t *name, size_t size)
{
#if 0
    uintptr_t id;
    lxb_dom_document_t *ddoc = lxb_dom_interface_node(element)->owner_document;
    lxb_html_document_t *doc = lxb_html_interface_document(ddoc);

    id = lxb_html_style_id_by_name(doc, name, size);
    if (id == LXB_CSS_PROPERTY__UNDEF) {
        return NULL;
    }

    return (lxb_html_style_node_t *) lexbor_avl_search(doc->css.styles,
                                                       element->style, id);
#endif
    return NULL;
}

const void *
lxb_html_element_css_property_by_id(lxb_html_element_t *element, uintptr_t id)
{
#if 0
    lxb_css_rule_declaration_t *declr;
    const lxb_html_style_node_t *node;

    node = lxb_html_element_style_node_by_id(element, id);
    if (node == NULL) {
        return lxb_css_property_initial_by_id(id);
    }

    declr = node->entry.value;

    return declr->u.user;
#endif
    return NULL;
}

lxb_status_t
lxb_html_element_style_walk(lxb_html_element_t *element,
                            lxb_html_element_style_cb_f cb, void *ctx,
                            bool with_weak)
{
    lxb_html_element_walk_ctx_t walk;

    walk.element = element;
    walk.cb = cb;
    walk.ctx = ctx;
    walk.weak = with_weak;

    return lexbor_avl_foreach(NULL, &element->style,
                              lxb_html_element_style_walk_cb, &walk);
}

static lxb_status_t
lxb_html_element_style_walk_cb(lexbor_avl_t *avl, lexbor_avl_node_t **root,
                               lexbor_avl_node_t *node, void *ctx)
{
    lxb_status_t status;
    lxb_html_style_weak_t *weak;
    lxb_html_style_node_t *style;
    lxb_html_element_walk_ctx_t *walk = ctx;

    style = (lxb_html_style_node_t *) node;

    status = walk->cb(walk->element, node->value, walk->ctx, style->sp, false);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    weak = style->weak;

    while (weak != NULL) {
        status = walk->cb(walk->element, weak->value, walk->ctx,
                          weak->sp, true);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        weak = weak->next;
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_html_element_style_parse(lxb_html_element_t *element,
                             const lxb_char_t *style, size_t size)
{
#if 0
    lxb_css_rule_declaration_list_t *list;

    lxb_dom_document_t *ddoc = lxb_dom_interface_node(element)->owner_document;
    lxb_html_document_t *doc = lxb_html_interface_document(ddoc);
    lxb_html_document_css_t *css = &doc->css;

    list = lxb_css_declaration_list_parse(css->parser, css->memory,
                                          style, size);
    if (list == NULL) {
        return css->parser->status;
    }

    element->list = list;

    return lxb_html_element_style_list_append(element, list,
                                              lxb_css_selector_sp_up_s(0));
#endif
    return LXB_STATUS_ERROR;
}

lxb_status_t
lxb_html_element_style_append(lxb_html_element_t *element,
                              lxb_css_rule_declaration_t *declr,
                              lxb_css_selector_specificity_t spec)
{
    uintptr_t id;
    lxb_status_t status;
    lexbor_str_t *name;
    lxb_html_style_node_t *node;

    lxb_dom_document_t *ddoc = lxb_dom_interface_node(element)->owner_document;
    lxb_html_document_t *doc = lxb_html_interface_document(ddoc);
    lxb_html_document_css_t *css = &doc->css;

    id = declr->type;

    lxb_css_selector_sp_set_i(spec, declr->important);

    if (id == LXB_CSS_PROPERTY__CUSTOM) {
        name = &declr->u.custom->name;

        id = lxb_html_document_css_customs_id(doc, name->data,
                                              name->length);
        if (id == 0) {
            /* FIXME: what to do with an error? */
            return LXB_STATUS_ERROR;
        }
    }

    node = (void *) lexbor_avl_search(css->styles, element->style, id);
    if (node != NULL) {
        if (spec < node->sp) {
            return lxb_html_element_style_weak_append(doc, node, declr, spec);
        }

        status = lxb_html_element_style_weak_append(doc, node,
                                                    node->entry.value, node->sp);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        lxb_css_rule_ref_dec(node->entry.value);

        node->entry.value = declr;
        node->sp = spec;

        return LXB_STATUS_OK;
    }

    node = (void *) lexbor_avl_insert(css->styles, &element->style,
                                      id, declr);
    if (node == NULL) {
        /* FIXME: what to do with an error? */
        return LXB_STATUS_ERROR;
    }

    node->sp = spec;

    return lxb_css_rule_ref_inc(lxb_css_rule(declr));
}

static lxb_status_t
lxb_html_element_style_weak_append(lxb_html_document_t *doc,
                                   lxb_html_style_node_t *node,
                                   lxb_css_rule_declaration_t *declr,
                                   lxb_css_selector_specificity_t spec)
{
    lxb_html_style_weak_t *weak, *prev, *new_weak;

    new_weak = lexbor_dobject_alloc(doc->css.weak);
    if (new_weak == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    new_weak->value = declr;
    new_weak->sp = spec;

    if (node->weak == NULL) {
        node->weak = new_weak;
        new_weak->next = NULL;

        goto done;
    }

    weak = node->weak;

    if (weak->sp <= spec) {
        node->weak = new_weak;
        new_weak->next = weak;

        goto done;
    }

    prev = weak;
    weak = weak->next;

    while (weak != NULL) {
        if (weak->sp <= spec) {
            prev->next = new_weak;
            new_weak->next = weak;

            goto done;
        }

        prev = weak;
        weak = weak->next;
    }

    prev->next = new_weak;
    new_weak->next = NULL;

done:

    return lxb_css_rule_ref_inc(lxb_css_rule(declr));
}

lxb_status_t
lxb_html_element_style_list_append(lxb_html_element_t *element,
                                   lxb_css_rule_declaration_list_t *list,
                                   lxb_css_selector_specificity_t spec)
{
    lxb_status_t status;
    lxb_css_rule_t *rule;
    lxb_css_rule_declaration_t *declr;

    rule = list->first;

    while (rule != NULL) {
        if (rule->type != LXB_CSS_RULE_DECLARATION) {
            goto next;
        }

        declr = lxb_css_rule_declaration(rule);

        status = lxb_html_element_style_append(element, declr, spec);
        if (status != LXB_STATUS_OK) {
            /* FIXME: what to do with an error? */
        }

    next:

        rule = rule->next;
    }

    return LXB_STATUS_OK;
}

void
lxb_html_element_style_remove_by_name(lxb_html_element_t *element,
                                      const lxb_char_t *name, size_t size)
{
#if 0
    uintptr_t id;
    lxb_dom_document_t *ddoc = lxb_dom_interface_node(element)->owner_document;
    lxb_html_document_t *doc = lxb_html_interface_document(ddoc);

    id = lxb_html_style_id_by_name(doc, name, size);
    if (id == LXB_CSS_PROPERTY__UNDEF) {
        return;
    }

    lxb_html_element_style_remove_by_id(element, id);
#endif
}

void
lxb_html_element_style_remove_by_id(lxb_html_element_t *element, uintptr_t id)
{
#if 0
    lxb_html_style_node_t *node;
    lxb_dom_document_t *ddoc = lxb_dom_interface_node(element)->owner_document;
    lxb_html_document_t *doc = lxb_html_interface_document(ddoc);

    node = (lxb_html_style_node_t *) lexbor_avl_search(doc->css.styles,
                                                       element->style, id);
    if (node != NULL) {
        lxb_html_element_style_remove_all(doc, &element->style, node);
    }
#endif
}

lxb_html_style_node_t *
lxb_html_element_style_remove_all_not(lxb_html_document_t *doc,
                                      lexbor_avl_node_t **root,
                                      lxb_html_style_node_t *style, bool bs)
{
    lxb_html_style_weak_t *weak, *prev, *next;

    weak = style->weak;
    prev = NULL;

    while (weak != NULL) {
        next = weak->next;

        if (lxb_css_selector_sp_s(weak->sp) == bs) {
            lxb_css_rule_ref_dec_destroy(weak->value);
            lexbor_dobject_free(doc->css.weak, weak);

            if (prev != NULL) {
                prev->next = next;
            }
            else {
                style->weak = next;
            }
        }
        else {
            prev = weak;
        }

        weak = next;
    }

    if (lxb_css_selector_sp_s(style->sp) != bs) {
        return style;
    }

    lxb_css_rule_ref_dec_destroy(style->entry.value);

    if (style->weak == NULL) {
        lexbor_avl_remove_by_node(doc->css.styles, root,
                                  (lexbor_avl_node_t *) style);
        return NULL;
    }

    weak = style->weak;

    style->entry.value = weak->value;
    style->sp = weak->sp;
    style->weak = weak->next;

    lexbor_dobject_free(doc->css.weak, weak);

    return style;
}

lxb_html_style_node_t *
lxb_html_element_style_remove_all(lxb_html_document_t *doc,
                                  lexbor_avl_node_t **root,
                                  lxb_html_style_node_t *style)
{
    lxb_html_style_weak_t *weak, *next;

    weak = style->weak;

    while (weak != NULL) {
        next = weak->next;

        lxb_css_rule_ref_dec_destroy(weak->value);
        lexbor_dobject_free(doc->css.weak, weak);

        weak = next;
    }

    lxb_css_rule_ref_dec_destroy(style->entry.value);
    lexbor_avl_remove_by_node(doc->css.styles, root,
                              (lexbor_avl_node_t *) style);
    return NULL;
}

lxb_html_style_node_t *
lxb_html_element_style_remove_by_list(lxb_html_document_t *doc,
                                      lexbor_avl_node_t **root,
                                      lxb_html_style_node_t *style,
                                      lxb_css_rule_declaration_list_t *list)
{
    lxb_html_style_weak_t *weak, *prev, *next;

    weak = style->weak;
    prev = NULL;

    while (weak != NULL) {
        next = weak->next;

        if (((lxb_css_rule_declaration_t *) weak->value)->rule.parent
            == (lxb_css_rule_t *) list)
        {
            lxb_css_rule_ref_dec_destroy(weak->value);
            lexbor_dobject_free(doc->css.weak, weak);

            if (prev != NULL) {
                prev->next = next;
            }
            else {
                style->weak = next;
            }
        }
        else {
            prev = weak;
        }

        weak = next;
    }

    if (((lxb_css_rule_declaration_t *) style->entry.value)->rule.parent
        != (lxb_css_rule_t *) list)
    {
        return style;
    }

    lxb_css_rule_ref_dec_destroy(style->entry.value);

    if (style->weak == NULL) {
        lexbor_avl_remove_by_node(doc->css.styles, root,
                                  (lexbor_avl_node_t *) style);
        return NULL;
    }

    weak = style->weak;

    style->entry.value = weak->value;
    style->sp = weak->sp;
    style->weak = weak->next;

    lexbor_dobject_free(doc->css.weak, weak);

    return style;
}

lxb_status_t
lxb_html_element_style_serialize(lxb_html_element_t *element,
                                 lxb_html_element_style_opt_t opt,
                                 lexbor_serialize_cb_f cb, void *ctx)
{
    lexbor_serialize_ctx_t context;

    context.cb = cb;
    context.ctx = ctx;
    context.opt = opt;
    context.count = 0;

    return lexbor_avl_foreach(NULL, &element->style,
                              lxb_html_element_style_serialize_cb, &context);
}

static lxb_status_t
lxb_html_element_style_serialize_cb(lexbor_avl_t *avl, lexbor_avl_node_t **root,
                                    lexbor_avl_node_t *node, void *ctx)
{
#if 0
    lxb_status_t status;
    lexbor_serialize_ctx_t *context = ctx;

    static const lexbor_str_t splt = lexbor_str("; ");

    if (context->count > 0) {
        lexbor_serialize_write(context->cb, splt.data, splt.length,
                               context->ctx, status);
    }

    context->count = 1;

    return lxb_css_rule_serialize(node->value, context->cb, context->ctx);
#endif
    return LXB_STATUS_ERROR;
}

lxb_status_t
lxb_html_element_style_serialize_str(lxb_html_element_t *element,
                                     lexbor_str_t *str,
                                     lxb_html_element_style_opt_t opt)
{
#if 0
    lxb_dom_document_t *doc;
    lxb_html_element_style_ctx_t context;

    doc = lxb_dom_interface_node(element)->owner_document;

    if (str->data == NULL) {
        lexbor_str_init(str, doc->text, 1024);

        if (str->data == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }

    context.str = str;
    context.mraw = doc->text;

    return lxb_html_element_style_serialize(element, opt,
                            lxb_html_element_style_serialize_str_cb, &context);
#endif
    return LXB_STATUS_ERROR;
}

#if 0
static lxb_status_t
lxb_html_element_style_serialize_str_cb(const lxb_char_t *data,
                                        size_t len, void *ctx)
{
    lxb_char_t *ret;
    lxb_html_element_style_ctx_t *context = ctx;

    ret = lexbor_str_append(context->str, context->mraw, data, len);
    if (ret == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    return LXB_STATUS_OK;
}
#endif
