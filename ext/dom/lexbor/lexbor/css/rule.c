/*
 * Copyright (C) 2021-2026 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/css/rule.h"
#include "lexbor/css/parser.h"
#include "lexbor/core/serialize.h"
#include "lexbor/css/stylesheet.h"
#include "lexbor/css/selectors/selectors.h"


void *
lxb_css_rule_destroy(lxb_css_rule_t *rule, bool self_destroy)
{
    switch (rule->type) {
        case LXB_CSS_RULE_LIST:
            return lxb_css_rule_list_destroy(lxb_css_rule_list(rule),
                                             self_destroy);
        case LXB_CSS_RULE_AT_RULE:
            return lxb_css_rule_at_destroy(lxb_css_rule_at(rule),
                                           self_destroy);
        case LXB_CSS_RULE_STYLE:
            return lxb_css_rule_style_destroy(lxb_css_rule_style(rule),
                                              self_destroy);
        case LXB_CSS_RULE_BAD_STYLE:
            return lxb_css_rule_bad_style_destroy(lxb_css_rule_bad_style(rule),
                                                  self_destroy);
        case LXB_CSS_RULE_DECLARATION:
            return lxb_css_rule_declaration_destroy(lxb_css_rule_declaration(rule),
                                                    self_destroy);
        case LXB_CSS_RULE_DECLARATION_LIST:
            return lxb_css_rule_declaration_list_destroy(lxb_css_rule_declaration_list(rule),
                                                         self_destroy);
        case LXB_CSS_RULE_STYLESHEET:
        case LXB_CSS_RULE_UNDEF:
            return NULL;
    }

    return NULL;
}

lxb_status_t
lxb_css_rule_serialize(const lxb_css_rule_t *rule,
                       lexbor_serialize_cb_f cb, void *ctx)
{
    switch (rule->type) {
        case LXB_CSS_RULE_LIST:
            return lxb_css_rule_list_serialize(lxb_css_rule_list(rule), cb, ctx);

        case LXB_CSS_RULE_AT_RULE:
            return lxb_css_rule_at_serialize(lxb_css_rule_at(rule), cb, ctx);

        case LXB_CSS_RULE_STYLE:
            return lxb_css_rule_style_serialize(lxb_css_rule_style(rule),
                                                cb, ctx);
        case LXB_CSS_RULE_BAD_STYLE:
            return lxb_css_rule_bad_style_serialize(lxb_css_rule_bad_style(rule),
                                                    cb, ctx);
        case LXB_CSS_RULE_DECLARATION:
            return lxb_css_rule_declaration_serialize(lxb_css_rule_declaration(rule),
                                                      cb, ctx);
        case LXB_CSS_RULE_DECLARATION_LIST:
            return lxb_css_rule_declaration_list_serialize(lxb_css_rule_declaration_list(rule),
                                                           cb, ctx);
        case LXB_CSS_RULE_STYLESHEET:
        case LXB_CSS_RULE_UNDEF:
            break;
    }

    return LXB_STATUS_ERROR_WRONG_ARGS;
}

lxb_status_t
lxb_css_rule_serialize_chain(const lxb_css_rule_t *rule,
                             lexbor_serialize_cb_f cb, void *ctx)
{
    lxb_status_t status;

    static const lxb_char_t ws_str[] = "\n";

    status = lxb_css_rule_serialize(rule, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    rule = rule->next;

    while (rule != NULL) {
        lexbor_serialize_write(cb, ws_str, (sizeof(ws_str) - 1), ctx, status);

        status = lxb_css_rule_serialize(rule, cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        rule = rule->next;
    }

    return LXB_STATUS_OK;
}

lxb_css_rule_list_t *
lxb_css_rule_list_destroy(lxb_css_rule_list_t *list, bool self_destroy)
{
    lxb_css_rule_t *rule, *next, *parent;
    lxb_css_rule_t *child_rule;
    lxb_css_memory_t *memory;
    lxb_css_rule_at_t *at;
    lxb_css_rule_style_t *style;
    lxb_css_rule_bad_style_t *bad;
    lxb_css_rule_list_t *sub_list;
    lxb_css_rule_declaration_list_t *declr_list;

    if (list == NULL) {
        return NULL;
    }

    rule = list->first;
    memory = lxb_css_rule(list)->memory;

    while (rule != NULL) {
        child_rule = NULL;

        switch (rule->type) {
            case LXB_CSS_RULE_LIST:
                sub_list = lxb_css_rule_list(rule);

                if (sub_list->first != NULL) {
                    child_rule = sub_list->first;

                    sub_list->first = NULL;
                    sub_list->last = NULL;
                }
                break;

            case LXB_CSS_RULE_STYLE:
                style = lxb_css_rule_style(rule);

                if (style->child != NULL) {
                    child_rule = lxb_css_rule(style->child);
                    style->child = NULL;
                }
                break;

            case LXB_CSS_RULE_BAD_STYLE:
                bad = lxb_css_rule_bad_style(rule);

                if (bad->child != NULL) {
                    child_rule = lxb_css_rule(bad->child);
                    bad->child = NULL;
                }
                break;

            case LXB_CSS_RULE_AT_RULE:
                at = lxb_css_rule_at(rule);

                if (at->type == LXB_CSS_AT_RULE_MEDIA) {
                    if (at->u.media != NULL && at->u.media->block != NULL) {
                        child_rule = lxb_css_rule(at->u.media->block);
                        at->u.media->block = NULL;
                    }
                }
                else if (at->type == LXB_CSS_AT_RULE__UNDEF) {
                    if (at->u.undef != NULL && at->u.undef->block != NULL) {
                        child_rule = lxb_css_rule(at->u.undef->block);
                        at->u.undef->block = NULL;
                    }
                }
                else if (at->type == LXB_CSS_AT_RULE__CUSTOM) {
                    if (at->u.custom != NULL && at->u.custom->block != NULL) {
                        child_rule = lxb_css_rule(at->u.custom->block);
                        at->u.custom->block = NULL;
                    }
                }
                else if (at->type == LXB_CSS_AT_RULE_FONT_FACE) {
                    if (at->u.font_face != NULL && at->u.font_face->block != NULL) {
                        child_rule = lxb_css_rule(at->u.font_face->block);
                        at->u.font_face->block = NULL;
                    }
                }
                break;

            case LXB_CSS_RULE_DECLARATION_LIST:
                declr_list = lxb_css_rule_declaration_list(rule);

                if (declr_list->first != NULL) {
                    child_rule = declr_list->first;

                    declr_list->first = NULL;
                    declr_list->last = NULL;
                }
                break;

            default:
                break;
        }

        if (child_rule != NULL) {
            rule = child_rule;
            continue;
        }

        next = rule->next;
        parent = rule->parent;

        (void) lxb_css_rule_destroy(rule, true);

        if (next != NULL) {
            rule = next;
        }
        else {
            if (parent == lxb_css_rule(list)) {
                break;
            }

            rule = parent;
        }
    }

    if (self_destroy) {
        return lexbor_mraw_free(memory->tree, list);
    }

    list->first = NULL;
    list->last = NULL;

    return list;
}

lxb_status_t
lxb_css_rule_list_serialize(const lxb_css_rule_list_t *list,
                            lexbor_serialize_cb_f cb, void *ctx)
{
    lxb_status_t status;
    lxb_css_rule_t *rule;

    static const lxb_char_t nl_str[] = "\n";

    rule = list->first;

    if (rule == NULL) {
        return LXB_STATUS_OK;
    }

    status = lxb_css_rule_serialize(rule, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    rule = rule->next;

    while (rule != NULL) {
        lexbor_serialize_write(cb, nl_str, (sizeof(nl_str) - 1), ctx, status);

        status = lxb_css_rule_serialize(rule, cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        rule = rule->next;
    }

    return LXB_STATUS_OK;
}

lxb_css_rule_at_t *
lxb_css_rule_at_destroy(lxb_css_rule_at_t *at, bool self_destroy)
{
    lxb_css_memory_t *memory = lxb_css_rule(at)->memory;

    switch (at->type) {
        case LXB_CSS_AT_RULE__UNDEF:
            (void) lxb_css_at_rule__undef_destroy(memory, at->u.undef, true);
            break;

        case LXB_CSS_AT_RULE_MEDIA:
            (void) lxb_css_at_rule_media_destroy(memory, at->u.media, true);
            break;

        case LXB_CSS_AT_RULE_NAMESPACE:
            (void) lxb_css_at_rule_namespace_destroy(memory, at->u.ns, true);
            break;

        case LXB_CSS_AT_RULE__LAST_ENTRY:
            break;
    }

    if (self_destroy) {
        return lexbor_mraw_free(memory->tree, at);
    }

    return at;
}

lxb_status_t
lxb_css_rule_at_serialize(const lxb_css_rule_at_t *at, lexbor_serialize_cb_f cb,
                          void *ctx)
{
    lxb_status_t status;
    const lxb_css_at_rule__undef_t *undef;
    const lxb_css_at_rule__custom_t *custom;
    const lxb_css_entry_at_rule_data_t *data, *undata;

    static const lxb_char_t at_str[] = "@";

    data = lxb_css_at_rule_by_id(at->type);
    if (data == NULL) {
        return LXB_STATUS_ERROR_NOT_EXISTS;
    }

    lexbor_serialize_write(cb, at_str, (sizeof(at_str) - 1), ctx, status);

    if (at->type == LXB_CSS_AT_RULE__UNDEF) {
        undef = at->u.undef;

        undata = lxb_css_at_rule_by_id(undef->type);
        if (undata == NULL) {
            return LXB_STATUS_ERROR_NOT_EXISTS;
        }

        lexbor_serialize_write(cb, undata->name, undata->length, ctx, status);
    }
    else if (at->type == LXB_CSS_AT_RULE__CUSTOM) {
        custom = at->u.custom;

        lexbor_serialize_write(cb, custom->name.data,
                               custom->name.length, ctx, status);
    }
    else {
        lexbor_serialize_write(cb, data->name, data->length, ctx, status);
    }

    return data->serialize(at->u.user, cb, ctx);
}

lxb_status_t
lxb_css_rule_at_serialize_name(const lxb_css_rule_at_t *at,
                               lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_at_rule_serialize_name(at->u.user, at->type, cb, ctx);
}

lxb_css_rule_style_t *
lxb_css_rule_style_destroy(lxb_css_rule_style_t *style, bool self_destroy)
{
    lxb_css_memory_t *memory = lxb_css_rule(style)->memory;

    lxb_css_selector_list_destroy(style->selector);
    (void) lxb_css_rule_declaration_list_destroy(style->declarations, true);
    (void) lxb_css_rule_list_destroy(style->child, true);

    style->selector = NULL;
    style->declarations = NULL;
    style->child = NULL;

    if (self_destroy) {
        return lexbor_mraw_free(memory->tree, style);
    }

    return style;
}

lxb_status_t
lxb_css_rule_style_serialize(const lxb_css_rule_style_t *style,
                             lexbor_serialize_cb_f cb, void *ctx)
{
    lxb_status_t status;

    static const lxb_char_t lc_str[] = " {";
    static const lxb_char_t rc_str[] = "}";
    static const lxb_char_t cm_str[] = "; ";

    status = lxb_css_selector_serialize_list_chain(style->selector, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    if (style->declarations != NULL) {
        lexbor_serialize_write(cb, lc_str, (sizeof(lc_str) - 1), ctx, status);

        status = lxb_css_rule_declaration_list_serialize(style->declarations,
                                                         cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        if (style->child == NULL) {
            lexbor_serialize_write(cb, rc_str, (sizeof(rc_str) - 1),
                                   ctx, status);
        }
    }

    if (style->child != NULL && style->child->first != NULL) {
        lexbor_serialize_write(cb, cm_str, (sizeof(cm_str) - 1), ctx, status);

        status = lxb_css_rule_list_serialize(style->child, cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }
    }

    if (style->declarations != NULL) {
        lexbor_serialize_write(cb, rc_str, (sizeof(rc_str) - 1), ctx, status);
    }

    return LXB_STATUS_OK;
}

lxb_css_rule_bad_style_t *
lxb_css_rule_bad_style_destroy(lxb_css_rule_bad_style_t *bad, bool self_destroy)
{
    lxb_css_memory_t *memory = lxb_css_rule(bad)->memory;

    (void) lexbor_str_destroy(&bad->selectors, memory->mraw, false);
    bad->declarations = lxb_css_rule_declaration_list_destroy(bad->declarations,
                                                              true);
    bad->child = lxb_css_rule_list_destroy(bad->child, true);

    if (self_destroy) {
        return lexbor_mraw_free(memory->tree, bad);
    }

    return bad;
}

lxb_status_t
lxb_css_rule_bad_style_serialize(const lxb_css_rule_bad_style_t *bad,
                                 lexbor_serialize_cb_f cb, void *ctx)
{
    lxb_status_t status;

    static const lxb_char_t lc_str[] = "{";
    static const lxb_char_t rc_str[] = "}";

    if (bad->selectors.data != NULL) {
        lexbor_serialize_write(cb, bad->selectors.data, bad->selectors.length,
                               ctx, status);
    }

    if (bad->declarations != NULL) {
        lexbor_serialize_write(cb, lc_str, (sizeof(lc_str) - 1), ctx, status);

        status = lxb_css_rule_declaration_list_serialize(bad->declarations,
                                                         cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        return cb(rc_str, (sizeof(rc_str) - 1), ctx);
    }

    return LXB_STATUS_OK;
}

lxb_css_rule_declaration_list_t *
lxb_css_rule_declaration_list_destroy(lxb_css_rule_declaration_list_t *list,
                                      bool self_destroy)
{
    lxb_css_rule_t *declr, *next;

    if (list == NULL) {
        return NULL;
    }

    declr = list->first;

    while (declr != NULL) {
        next = declr->next;
        (void) lxb_css_rule_destroy(declr, true);
        declr = next;
    }

    if (self_destroy) {
        return lexbor_mraw_free(lxb_css_rule(list)->memory->tree, list);
    }

    list->first = NULL;
    list->last = NULL;

    return list;
}

lxb_status_t
lxb_css_rule_declaration_list_serialize(const lxb_css_rule_declaration_list_t *list,
                                        lexbor_serialize_cb_f cb, void *ctx)
{
    lxb_status_t status;
    const lxb_css_rule_t *declr;

    static const lxb_char_t sm_str[] = "; ";

    declr = list->first;

    if (declr == NULL) {
        return LXB_STATUS_OK;
    }

    status = lxb_css_rule_serialize(declr, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    declr = declr->next;

    while (declr != NULL) {
        lexbor_serialize_write(cb, sm_str, (sizeof(sm_str) - 1), ctx, status);

        status = lxb_css_rule_serialize(declr, cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        declr = declr->next;
    }

    return LXB_STATUS_OK;
}

lxb_css_rule_declaration_t *
lxb_css_rule_declaration_destroy(lxb_css_rule_declaration_t *declr,
                                 bool self_destroy)
{
    lxb_css_memory_t *memory = lxb_css_rule(declr)->memory;

    if (declr->u.user != NULL) {
        declr->u.user = lxb_css_property_destroy(memory, declr->u.user,
                                                 declr->type, true);
    }

    if (self_destroy) {
        return lexbor_mraw_free(memory->tree, declr);
    }

    return declr;
}

lxb_status_t
lxb_css_rule_declaration_serialize(const lxb_css_rule_declaration_t *declaration,
                                   lexbor_serialize_cb_f cb, void *ctx)
{
    lxb_status_t status;
    const lxb_css_entry_data_t *data, *undata;
    const lxb_css_property__undef_t *undef;
    const lxb_css_property__custom_t *custom;

    static const lxb_char_t cl_str[] = ": ";
    static const lxb_char_t imp_str[] = " !important";

    data = lxb_css_property_by_id(declaration->type);
    if (data == NULL) {
        return LXB_STATUS_ERROR_NOT_EXISTS;
    }

    if (declaration->type == LXB_CSS_PROPERTY__UNDEF) {
        undef = declaration->u.undef;

        if (undef->type > LXB_CSS_PROPERTY__CUSTOM) {
            undata = lxb_css_property_by_id(undef->type);
            if (undata == NULL) {
                return LXB_STATUS_ERROR_NOT_EXISTS;
            }

            lexbor_serialize_write(cb, undata->name, undata->length, ctx, status);
            lexbor_serialize_write(cb, cl_str, (sizeof(cl_str) - 1), ctx, status);
        }
    }
    else if (declaration->type == LXB_CSS_PROPERTY__CUSTOM) {
        custom = declaration->u.custom;

        lexbor_serialize_write(cb, custom->name.data, custom->name.length,
                               ctx, status);
        lexbor_serialize_write(cb, cl_str, (sizeof(cl_str) - 1), ctx, status);
    }
    else {
        lexbor_serialize_write(cb, data->name, data->length, ctx, status);
        lexbor_serialize_write(cb, cl_str, (sizeof(cl_str) - 1), ctx, status);
    }

    status = data->serialize(declaration->u.user, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    if (declaration->important && declaration->type != LXB_CSS_PROPERTY__UNDEF) {
        lexbor_serialize_write(cb, imp_str, (sizeof(imp_str) - 1), ctx, status);
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_css_rule_declaration_serialize_name(const lxb_css_rule_declaration_t *declr,
                                        lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_property_serialize_name(declr->u.user, declr->type, cb, ctx);
}
