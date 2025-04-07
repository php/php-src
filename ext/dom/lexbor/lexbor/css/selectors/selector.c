/*
 * Copyright (C) 2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/core/serialize.h"
#include "lexbor/css/css.h"
#include "lexbor/css/selectors/selectors.h"
#include "lexbor/css/selectors/selector.h"
#include "lexbor/css/selectors/pseudo.h"
#include "lexbor/css/selectors/pseudo_const.h"
#include "lexbor/css/selectors/pseudo_state.h"
#include "lexbor/css/selectors/state.h"
#include "lexbor/css/selectors/pseudo_res.h"


typedef void
(*lxb_css_selector_destroy_f)(lxb_css_selector_t *selector,
                              lxb_css_memory_t *mem);
typedef lxb_status_t
(*lxb_css_selector_serialize_f)(lxb_css_selector_t *selector,
                                lexbor_serialize_cb_f cb, void *ctx);


static void
lxb_css_selector_destroy_undef(lxb_css_selector_t *selector,
                               lxb_css_memory_t *mem);
static void
lxb_css_selector_destroy_any(lxb_css_selector_t *selector,
                             lxb_css_memory_t *mem);
static void
lxb_css_selector_destroy_id(lxb_css_selector_t *selector,
                            lxb_css_memory_t *mem);
static void
lxb_css_selector_destroy_attribute(lxb_css_selector_t *selector,
                                   lxb_css_memory_t *mem);
static void
lxb_css_selector_destroy_pseudo_class_function(lxb_css_selector_t *selector,
                                               lxb_css_memory_t *mem);
static void
lxb_css_selector_destroy_pseudo_element_function(lxb_css_selector_t *selector,
                                                 lxb_css_memory_t *mem);

static lxb_status_t
lxb_css_selector_serialize_undef(lxb_css_selector_t *selector,
                                 lexbor_serialize_cb_f cb, void *ctx);
static lxb_status_t
lxb_css_selector_serialize_any(lxb_css_selector_t *selector,
                               lexbor_serialize_cb_f cb, void *ctx);
static lxb_status_t
lxb_css_selector_serialize_id(lxb_css_selector_t *selector,
                              lexbor_serialize_cb_f cb, void *ctx);
static lxb_status_t
lxb_css_selector_serialize_class(lxb_css_selector_t *selector,
                                 lexbor_serialize_cb_f cb, void *ctx);
static lxb_status_t
lxb_css_selector_serialize_attribute(lxb_css_selector_t *selector,
                                     lexbor_serialize_cb_f cb, void *ctx);
static lxb_status_t
lxb_css_selector_serialize_pseudo_class(lxb_css_selector_t *selector,
                                        lexbor_serialize_cb_f cb, void *ctx);
static lxb_status_t
lxb_css_selector_serialize_pseudo_class_function(lxb_css_selector_t *selector,
                                                 lexbor_serialize_cb_f cb, void *ctx);
static lxb_status_t
lxb_css_selector_serialize_pseudo_element(lxb_css_selector_t *selector,
                                          lexbor_serialize_cb_f cb, void *ctx);
static lxb_status_t
lxb_css_selector_serialize_pseudo_element_function(lxb_css_selector_t *selector,
                                                   lexbor_serialize_cb_f cb, void *ctx);

static lxb_status_t
lxb_css_selector_serialize_pseudo_single(lxb_css_selector_t *selector,
                                         lexbor_serialize_cb_f cb, void *ctx,
                                         bool is_class);


static const lxb_css_selector_destroy_f
                  lxb_selector_destroy_map[LXB_CSS_SELECTOR_TYPE__LAST_ENTRY] =
{
    lxb_css_selector_destroy_undef,
    lxb_css_selector_destroy_any,
    lxb_css_selector_destroy_any,
    lxb_css_selector_destroy_id,
    lxb_css_selector_destroy_id,
    lxb_css_selector_destroy_attribute,
    lxb_css_selector_destroy_undef,
    lxb_css_selector_destroy_pseudo_class_function,
    lxb_css_selector_destroy_undef,
    lxb_css_selector_destroy_pseudo_element_function
};

static const lxb_css_selector_serialize_f
                lxb_selector_serialize_map[LXB_CSS_SELECTOR_TYPE__LAST_ENTRY] =
{
    lxb_css_selector_serialize_undef,
    lxb_css_selector_serialize_any,
    lxb_css_selector_serialize_any,
    lxb_css_selector_serialize_id,
    lxb_css_selector_serialize_class,
    lxb_css_selector_serialize_attribute,
    lxb_css_selector_serialize_pseudo_class,
    lxb_css_selector_serialize_pseudo_class_function,
    lxb_css_selector_serialize_pseudo_element,
    lxb_css_selector_serialize_pseudo_element_function
};


lxb_css_selector_t *
lxb_css_selector_create(lxb_css_selector_list_t *list)
{
    lxb_css_selector_t *selector = lexbor_dobject_calloc(list->memory->objs);
    if (selector == NULL) {
        return NULL;
    }

    selector->list = list;

    return selector;
}

void
lxb_css_selector_destroy(lxb_css_selector_t *selector)
{
    lxb_css_memory_t *memory;

    if (selector != NULL) {
        memory = selector->list->memory;

        lxb_selector_destroy_map[selector->type](selector, memory);
        lexbor_dobject_free(memory->objs, selector);
    }
}

void
lxb_css_selector_destroy_chain(lxb_css_selector_t *selector)
{
    lxb_css_selector_t *next;

    while (selector != NULL) {
        next = selector->next;
        lxb_css_selector_destroy(selector);
        selector = next;
    }
}

void
lxb_css_selector_remove(lxb_css_selector_t *selector)
{
    if (selector->next != NULL) {
        selector->next->prev = selector->prev;
    }

    if (selector->prev != NULL) {
        selector->prev->next = selector->next;
    }

    if (selector->list->first == selector) {
        selector->list->first = selector->next;
    }

    if (selector->list->last == selector) {
        selector->list->last = selector->prev;
    }
}

lxb_css_selector_list_t *
lxb_css_selector_list_create(lxb_css_memory_t *mem)
{
    lxb_css_selector_list_t *list;

    list = lexbor_dobject_calloc(mem->objs);
    if (list == NULL) {
        return NULL;
    }

    list->memory = mem;

    return list;
}

void
lxb_css_selector_list_remove(lxb_css_selector_list_t *list)
{
    if (list->next != NULL) {
        list->next->prev = list->prev;
    }

    if (list->prev != NULL) {
        list->prev->next = list->next;
    }
}

void
lxb_css_selector_list_selectors_remove(lxb_css_selectors_t *selectors,
                                       lxb_css_selector_list_t *list)
{
    lxb_css_selector_list_remove(list);

    if (selectors->list == list) {
        selectors->list = list->next;
    }

    if (selectors->list_last == list) {
        selectors->list_last = list->prev;
    }
}

void
lxb_css_selector_list_destroy(lxb_css_selector_list_t *list)
{
    if (list != NULL) {
        lxb_css_selector_destroy_chain(list->first);
        lexbor_dobject_free(list->memory->objs, list);
    }
}

void
lxb_css_selector_list_destroy_chain(lxb_css_selector_list_t *list)
{
    lxb_css_selector_list_t *next;

    while (list != NULL) {
        next = list->next;
        lxb_css_selector_list_destroy(list);
        list = next;
    }
}

void
lxb_css_selector_list_destroy_memory(lxb_css_selector_list_t *list)
{
    if (list != NULL) {
        (void) lxb_css_memory_destroy(list->memory, true);
    }
}

static void
lxb_css_selector_destroy_undef(lxb_css_selector_t *selector,
                               lxb_css_memory_t *mem)
{
    /* Do nothing. */
}

static void
lxb_css_selector_destroy_any(lxb_css_selector_t *selector,
                             lxb_css_memory_t *mem)
{
    if (selector->ns.data != NULL) {
        lexbor_mraw_free(mem->mraw, selector->ns.data);
    }

    if (selector->name.data != NULL) {
        lexbor_mraw_free(mem->mraw, selector->name.data);
    }
}

static void
lxb_css_selector_destroy_id(lxb_css_selector_t *selector,
                            lxb_css_memory_t *mem)
{
    if (selector->name.data != NULL) {
        (void) lexbor_mraw_free(mem->mraw, selector->name.data);
    }
}

static void
lxb_css_selector_destroy_attribute(lxb_css_selector_t *selector,
                                   lxb_css_memory_t *mem)
{
    if (selector->ns.data != NULL) {
        lexbor_mraw_free(mem->mraw, selector->ns.data);
    }

    if (selector->name.data != NULL) {
        lexbor_mraw_free(mem->mraw, selector->name.data);
    }

    if (selector->u.attribute.value.data != NULL) {
        lexbor_mraw_free(mem->mraw, selector->u.attribute.value.data);
    }
}

static void
lxb_css_selector_destroy_pseudo_class_function(lxb_css_selector_t *selector,
                                               lxb_css_memory_t *mem)
{
    lxb_css_selector_anb_of_t *anbof;
    lxb_css_selector_pseudo_t *pseudo;

    pseudo = &selector->u.pseudo;

    switch (pseudo->type) {
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_CURRENT:
            break;
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_DIR:
            break;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_HAS:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_IS:
            lxb_css_selector_list_destroy_chain(pseudo->data);
            break;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_LANG:
            break;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NOT:
            lxb_css_selector_list_destroy_chain(pseudo->data);
            break;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_CHILD:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_COL:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_LAST_CHILD:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_LAST_COL:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_LAST_OF_TYPE:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_OF_TYPE:
            anbof = pseudo->data;

            if (anbof != NULL) {
                lxb_css_selector_list_destroy_chain(anbof->of);
                lexbor_mraw_free(mem->mraw, anbof);
            }
            break;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_WHERE:
            lxb_css_selector_list_destroy_chain(pseudo->data);
            break;

        default:
            break;
    }
}

static void
lxb_css_selector_destroy_pseudo_element_function(lxb_css_selector_t *selector,
                                                 lxb_css_memory_t *mem)
{

}

lxb_status_t
lxb_css_selector_serialize(lxb_css_selector_t *selector,
                           lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_selector_serialize_map[selector->type](selector, cb, ctx);
}

lxb_status_t
lxb_css_selector_serialize_chain(lxb_css_selector_t *selector,
                                 lexbor_serialize_cb_f cb, void *ctx)
{
    size_t length;
    lxb_char_t *data;
    lxb_status_t status;

    if (selector == NULL) {
        return LXB_STATUS_OK;
    }

    if (selector->combinator > LXB_CSS_SELECTOR_COMBINATOR_CLOSE) {
        data = lxb_css_selector_combinator(selector, &length);
        if (data == NULL) {
            return LXB_STATUS_ERROR_UNEXPECTED_DATA;
        }

        lxb_css_selector_serialize_write(data, length);
        lxb_css_selector_serialize_write(" ", 1);
    }

    status = lxb_css_selector_serialize(selector, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    selector = selector->next;

    while (selector != NULL) {
        data = lxb_css_selector_combinator(selector, &length);
        if (data == NULL) {
            return LXB_STATUS_ERROR_UNEXPECTED_DATA;
        }

        if (length != 0) {
            lxb_css_selector_serialize_write(" ", 1);

            if (*data != ' ') {
                lxb_css_selector_serialize_write(data, length);
                lxb_css_selector_serialize_write(" ", 1);
            }
        }

        status = lxb_css_selector_serialize(selector, cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        selector = selector->next;
    }

    return LXB_STATUS_OK;
}

lxb_char_t *
lxb_css_selector_serialize_chain_char(lxb_css_selector_t *selector,
                                      size_t *out_length)
{
    size_t length = 0;
    lxb_status_t status;
    lexbor_str_t str;

    status = lxb_css_selector_serialize_chain(selector, lexbor_serialize_length_cb,
                                              &length);
    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    /* + 1 == '\0' */
    str.data = lexbor_malloc(length + 1);
    if (str.data == NULL) {
        goto failed;
    }

    str.length = 0;

    status = lxb_css_selector_serialize_chain(selector, lexbor_serialize_copy_cb,
                                              &str);
    if (status != LXB_STATUS_OK) {
        lexbor_free(str.data);
        goto failed;
    }

    str.data[str.length] = '\0';

    if (out_length != NULL) {
        *out_length = str.length;
    }

    return str.data;

failed:

    if (out_length != NULL) {
        *out_length = 0;
    }

    return NULL;
}

lxb_status_t
lxb_css_selector_serialize_list(lxb_css_selector_list_t *list,
                                 lexbor_serialize_cb_f cb, void *ctx)
{
    if (list != NULL) {
        return lxb_css_selector_serialize_chain(list->first, cb, ctx);
    }

    return LXB_STATUS_OK;
}

lxb_char_t *
lxb_css_selector_serialize_list_char(lxb_css_selector_list_t *list,
                                      size_t *out_length)
{
    size_t length = 0;
    lxb_status_t status;
    lexbor_str_t str;

    status = lxb_css_selector_serialize_list_chain(list, lexbor_serialize_length_cb,
                                                   &length);
    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    /* + 1 == '\0' */
    str.data = lexbor_malloc(length + 1);
    if (str.data == NULL) {
        goto failed;
    }

    str.length = 0;

    status = lxb_css_selector_serialize_list_chain(list, lexbor_serialize_copy_cb,
                                                   &str);
    if (status != LXB_STATUS_OK) {
        lexbor_free(str.data);
        goto failed;
    }

    str.data[str.length] = '\0';

    if (out_length != NULL) {
        *out_length = str.length;
    }

    return str.data;

failed:

    if (out_length != NULL) {
        *out_length = 0;
    }

    return NULL;
}

lxb_status_t
lxb_css_selector_serialize_list_chain(lxb_css_selector_list_t *list,
                                      lexbor_serialize_cb_f cb, void *ctx)
{
    lxb_status_t status;

    if (list == NULL) {
        return LXB_STATUS_OK;
    }

    status = lxb_css_selector_serialize_chain(list->first, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    list = list->next;

    while (list != NULL) {
        lxb_css_selector_serialize_write(", ", 2);

        status = lxb_css_selector_serialize_chain(list->first, cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        list = list->next;
    }

    return LXB_STATUS_OK;
}

lxb_char_t *
lxb_css_selector_serialize_list_chain_char(lxb_css_selector_list_t *list,
                                           size_t *out_length)
{
    size_t length = 0;
    lxb_status_t status;
    lexbor_str_t str;

    status = lxb_css_selector_serialize_list_chain(list, lexbor_serialize_length_cb,
                                                   &length);
    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    /* + 1 == '\0' */
    str.data = lexbor_malloc(length + 1);
    if (str.data == NULL) {
        goto failed;
    }

    str.length = 0;

    status = lxb_css_selector_serialize_list_chain(list, lexbor_serialize_copy_cb,
                                                   &str);
    if (status != LXB_STATUS_OK) {
        lexbor_free(str.data);
        goto failed;
    }

    str.data[str.length] = '\0';

    if (out_length != NULL) {
        *out_length = str.length;
    }

    return str.data;

failed:

    if (out_length != NULL) {
        *out_length = 0;
    }

    return NULL;
}


static lxb_status_t
lxb_css_selector_serialize_undef(lxb_css_selector_t *selector,
                                 lexbor_serialize_cb_f cb, void *ctx)
{
    return LXB_STATUS_ERROR_UNEXPECTED_DATA;
}

static lxb_status_t
lxb_css_selector_serialize_any(lxb_css_selector_t *selector,
                               lexbor_serialize_cb_f cb, void *ctx)
{
    lxb_status_t status;

    if (selector->ns.data != NULL) {
        lxb_css_selector_serialize_write(selector->ns.data,
                                         selector->ns.length);
        lxb_css_selector_serialize_write("|", 1);
    }

    if (selector->name.data != NULL) {
        return cb(selector->name.data, selector->name.length, ctx);
    }

    return LXB_STATUS_OK;
}

static lxb_status_t
lxb_css_selector_serialize_id(lxb_css_selector_t *selector,
                              lexbor_serialize_cb_f cb, void *ctx)
{
    lxb_status_t status;

    lxb_css_selector_serialize_write("#", 1);

    if (selector->name.data != NULL) {
        return cb(selector->name.data, selector->name.length, ctx);
    }

    return LXB_STATUS_OK;
}

static lxb_status_t
lxb_css_selector_serialize_class(lxb_css_selector_t *selector,
                                 lexbor_serialize_cb_f cb, void *ctx)
{
    lxb_status_t status;

    lxb_css_selector_serialize_write(".", 1);

    if (selector->name.data != NULL) {
        return cb(selector->name.data, selector->name.length, ctx);
    }

    return LXB_STATUS_OK;
}

static lxb_status_t
lxb_css_selector_serialize_attribute(lxb_css_selector_t *selector,
                                     lexbor_serialize_cb_f cb, void *ctx)
{
    lxb_char_t *p, *begin, *end;
    lxb_status_t status;
    lxb_css_selector_attribute_t *attr;

    lxb_css_selector_serialize_write("[", 1);

    status = lxb_css_selector_serialize_any(selector, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    attr = &selector->u.attribute;

    if (attr->value.data == NULL) {
        return cb((lxb_char_t *) "]", 1, ctx);
    }

    switch (attr->match) {
        case LXB_CSS_SELECTOR_MATCH_EQUAL:
            lxb_css_selector_serialize_write("=", 1);
            break;
        case LXB_CSS_SELECTOR_MATCH_INCLUDE:
            lxb_css_selector_serialize_write("~=", 2);
            break;
        case LXB_CSS_SELECTOR_MATCH_DASH:
            lxb_css_selector_serialize_write("|=", 2);
            break;
        case LXB_CSS_SELECTOR_MATCH_PREFIX:
            lxb_css_selector_serialize_write("^=", 2);
            break;
        case LXB_CSS_SELECTOR_MATCH_SUFFIX:
            lxb_css_selector_serialize_write("$=", 2);
            break;
        case LXB_CSS_SELECTOR_MATCH_SUBSTRING:
            lxb_css_selector_serialize_write("*=", 2);
            break;

        default:
            return LXB_STATUS_ERROR_UNEXPECTED_DATA;
    }

    p = attr->value.data;
    end = attr->value.data + attr->value.length;

    begin = p;

    lxb_css_selector_serialize_write("\"", 1);

    while (p < end) {
        if (*p == '"') {
            if (begin < p) {
                lxb_css_selector_serialize_write(begin, p - begin);
            }

            lxb_css_selector_serialize_write("\\000022", 7);

            begin = p + 1;
        }

        p++;
    }

    if (begin < p) {
        lxb_css_selector_serialize_write(begin, p - begin);
    }

    lxb_css_selector_serialize_write("\"", 1);

    if (attr->modifier != LXB_CSS_SELECTOR_MODIFIER_UNSET) {
        switch (attr->modifier) {
            case LXB_CSS_SELECTOR_MODIFIER_I:
                lxb_css_selector_serialize_write("i", 1);
                break;

            case LXB_CSS_SELECTOR_MODIFIER_S:
                lxb_css_selector_serialize_write("s", 1);
                break;

            default:
                return LXB_STATUS_ERROR_UNEXPECTED_DATA;
        }
    }

    return cb((lxb_char_t *) "]", 1, ctx);
}

static lxb_status_t
lxb_css_selector_serialize_pseudo_class(lxb_css_selector_t *selector,
                                        lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_selector_serialize_pseudo_single(selector, cb, ctx, true);
}

static lxb_status_t
lxb_css_selector_serialize_pseudo_class_function(lxb_css_selector_t *selector,
                                                 lexbor_serialize_cb_f cb, void *ctx)
{
    lxb_status_t status;
    lxb_css_selector_pseudo_t *pseudo;
    const lxb_css_selectors_pseudo_data_func_t *pfunc;

    pseudo = &selector->u.pseudo;

    pfunc = &lxb_css_selectors_pseudo_data_pseudo_class_function[pseudo->type];

    lxb_css_selector_serialize_write(":", 1);
    lxb_css_selector_serialize_write(pfunc->name, pfunc->length);
    lxb_css_selector_serialize_write("(", 1);

    switch (pseudo->type) {
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_CURRENT:
            break;
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_DIR:
            break;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_HAS:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_IS:
            status = lxb_css_selector_serialize_list_chain(pseudo->data,
                                                           cb, ctx);
            break;
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_LANG:
            break;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NOT:
            status = lxb_css_selector_serialize_list_chain(pseudo->data,
                                                           cb, ctx);
            break;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_CHILD:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_COL:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_LAST_CHILD:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_LAST_COL:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_LAST_OF_TYPE:
        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_NTH_OF_TYPE:
            status = LXB_STATUS_OK;

            if (pseudo->data != NULL) {
                status = lxb_css_selector_serialize_anb_of(pseudo->data,
                                                           cb, ctx);
            }
            break;

        case LXB_CSS_SELECTOR_PSEUDO_CLASS_FUNCTION_WHERE:
            status = lxb_css_selector_serialize_list_chain(pseudo->data,
                                                           cb, ctx);
            break;

        default:
            status = LXB_STATUS_OK;
            break;
    }

    if (status != LXB_STATUS_OK) {
        return status;
    }

    lxb_css_selector_serialize_write(")", 1);

    return LXB_STATUS_OK;
}

static lxb_status_t
lxb_css_selector_serialize_pseudo_element(lxb_css_selector_t *selector,
                                          lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_selector_serialize_pseudo_single(selector, cb, ctx, false);
}

static lxb_status_t
lxb_css_selector_serialize_pseudo_element_function(lxb_css_selector_t *selector,
                                                   lexbor_serialize_cb_f cb, void *ctx)
{
    return LXB_STATUS_OK;
}

static lxb_status_t
lxb_css_selector_serialize_pseudo_single(lxb_css_selector_t *selector,
                                         lexbor_serialize_cb_f cb, void *ctx,
                                         bool is_class)
{
    lxb_status_t status;
    lxb_css_selector_pseudo_t *pseudo;
    const lxb_css_selectors_pseudo_data_t *pclass;

    pseudo = &selector->u.pseudo;

    if (is_class) {
        pclass = &lxb_css_selectors_pseudo_data_pseudo_class[pseudo->type];
        lxb_css_selector_serialize_write(":", 1);
    }
    else {
        pclass = &lxb_css_selectors_pseudo_data_pseudo_element[pseudo->type];
        lxb_css_selector_serialize_write("::", 2);
    }

    lxb_css_selector_serialize_write(pclass->name, pclass->length);

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_css_selector_serialize_anb_of(lxb_css_selector_anb_of_t *anbof,
                                  lexbor_serialize_cb_f cb, void *ctx)
{
    lxb_status_t status;

    static const lxb_char_t of[] = " of ";

    status = lxb_css_syntax_anb_serialize(&anbof->anb, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    if (anbof->of != NULL) {
        lxb_css_selector_serialize_write(of, sizeof(of) - 1);

        return lxb_css_selector_serialize_list_chain(anbof->of, cb, ctx);
    }

    return LXB_STATUS_OK;
}

lxb_char_t *
lxb_css_selector_combinator(lxb_css_selector_t *selector, size_t *out_length)
{
    switch (selector->combinator) {
        case LXB_CSS_SELECTOR_COMBINATOR_DESCENDANT:
            if (out_length != NULL) {*out_length = 1;}
            return (lxb_char_t *) " ";

        case LXB_CSS_SELECTOR_COMBINATOR_CLOSE:
            if (out_length != NULL) {*out_length = 0;}
            return (lxb_char_t *) "";

        case LXB_CSS_SELECTOR_COMBINATOR_CHILD:
            if (out_length != NULL) {*out_length = 1;}
            return (lxb_char_t *) ">";

        case LXB_CSS_SELECTOR_COMBINATOR_SIBLING:
            if (out_length != NULL) {*out_length = 1;}
            return (lxb_char_t *) "+";

        case LXB_CSS_SELECTOR_COMBINATOR_FOLLOWING:
            if (out_length != NULL) {*out_length = 1;}
            return (lxb_char_t *) "~";

        case LXB_CSS_SELECTOR_COMBINATOR_CELL:
            if (out_length != NULL) {*out_length = 2;}
            return (lxb_char_t *) "||";

        default:
            if (out_length != NULL) {*out_length = 0;}
            return NULL;
    }
}

void
lxb_css_selector_list_append(lxb_css_selector_list_t *list,
                             lxb_css_selector_t *selector)
{
    selector->prev = list->last;

    if (list->last != NULL) {
        list->last->next = selector;
    }
    else {
        list->first = selector;
    }

    list->last = selector;
}

void
lxb_css_selector_append_next(lxb_css_selector_t *dist, lxb_css_selector_t *src)
{
    if (dist->next != NULL) {
        dist->next->prev = src;
    }

    src->prev = dist;
    src->next = dist->next;

    dist->next = src;
}

void
lxb_css_selector_list_append_next(lxb_css_selector_list_t *dist,
                                  lxb_css_selector_list_t *src)
{
    if (dist->next != NULL) {
        dist->next->prev = src;
    }

    src->prev = dist;
    src->next = dist->next;

    dist->next = src;
}
