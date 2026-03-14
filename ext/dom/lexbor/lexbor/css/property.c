/*
 * Copyright (C) 2021-2023 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/css/css.h"
#include "lexbor/css/property.h"
#include "lexbor/css/parser.h"
#include "lexbor/css/stylesheet.h"
#include "lexbor/css/property/state.h"
#include "lexbor/css/property/res.h"
#include "lexbor/core/serialize.h"
#include "lexbor/core/conv.h"


const lxb_css_entry_data_t *
lxb_css_property_by_name(const lxb_char_t *name, size_t length)
{
    const lexbor_shs_entry_t *entry;

    entry = lexbor_shs_entry_get_lower_static(lxb_css_property_shs,
                                              name, length);
    if (entry == NULL) {
        return NULL;
    }

    return entry->value;
}

const lxb_css_entry_data_t *
lxb_css_property_by_id(uintptr_t id)
{
    return &lxb_css_property_data[id];
}

const void *
lxb_css_property_initial_by_id(uintptr_t id)
{
    if (id >= LXB_CSS_PROPERTY__LAST_ENTRY) {
        return NULL;
    }

    return lxb_css_property_data[id].initial;
}

void *
lxb_css_property_destroy(lxb_css_memory_t *memory, void *style,
                         lxb_css_property_type_t type, bool self_destroy)
{
    const lxb_css_entry_data_t *data;

    data = lxb_css_property_by_id(type);
    if (data == NULL) {
        return style;
    }

    return data->destroy(memory, style, self_destroy);
}

lxb_status_t
lxb_css_property_serialize(const void *style, lxb_css_property_type_t type,
                           lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_entry_data_t *data;

    data = lxb_css_property_by_id(type);
    if (data == NULL) {
        return LXB_STATUS_ERROR_NOT_EXISTS;
    }

    return data->serialize(style, cb, ctx);
}

lxb_status_t
lxb_css_property_serialize_str(const void *style, lxb_css_property_type_t type,
                               lexbor_mraw_t *mraw, lexbor_str_t *str)
{
    const lxb_css_entry_data_t *data;

    data = lxb_css_property_by_id(type);
    if (data == NULL) {
        return LXB_STATUS_ERROR_NOT_EXISTS;
    }

    return lxb_css_serialize_str_handler(style, str, mraw, data->serialize);
}

lxb_status_t
lxb_css_property_serialize_name(const void *style, lxb_css_property_type_t type,
                                lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_entry_data_t *data;

    switch (type) {
        case LXB_CSS_PROPERTY__UNDEF:
            return lxb_css_property__undef_serialize_name(style, cb, ctx);

        case LXB_CSS_PROPERTY__CUSTOM:
            return lxb_css_property__custom_serialize_name(style, cb, ctx);

        default:
            break;
    }

    data = lxb_css_property_by_id(type);
    if (data == NULL) {
        return LXB_STATUS_ERROR_NOT_EXISTS;
    }

    return cb(data->name, data->length, ctx);
}

lxb_status_t
lxb_css_property_serialize_name_str(const void *style, lxb_css_property_type_t type,
                                    lexbor_mraw_t *mraw, lexbor_str_t *str)
{
    const lxb_css_entry_data_t *data;

    switch (type) {
        case LXB_CSS_PROPERTY__UNDEF:
            return lxb_css_serialize_str_handler(style, str, mraw,
                                       lxb_css_property__undef_serialize_name);

        case LXB_CSS_PROPERTY__CUSTOM:
            return lxb_css_serialize_str_handler(style, str, mraw,
                                      lxb_css_property__custom_serialize_name);

        default:
            break;
    }

    data = lxb_css_property_by_id(type);
    if (data == NULL) {
        return LXB_STATUS_ERROR_NOT_EXISTS;
    }

    if (str->data == NULL) {
        lexbor_str_init(str, mraw, data->length);
        if (str->data == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }

    (void) lexbor_str_append(str, mraw, data->name, data->length);

    return LXB_STATUS_OK;
}

/* _undef. */

void *
lxb_css_property__undef_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property__undef_t));
}

void *
lxb_css_property__undef_destroy(lxb_css_memory_t *memory,
                                void *style, bool self_destroy)
{
    if (style == NULL) {
        return NULL;
    }

    if (self_destroy) {
        return lexbor_mraw_free(memory->mraw, style);
    }

    return style;
}

lxb_status_t
lxb_css_property__undef_serialize(const void *style,
                                  lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property__undef_t *undef = style;

    return cb(undef->value.data, undef->value.length, ctx);
}

lxb_status_t
lxb_css_property__undef_serialize_name(const void *style,
                                       lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property__undef_t *undef = style;
    const lxb_css_entry_data_t *data;

    if (undef->type == LXB_CSS_PROPERTY__UNDEF) {
        return LXB_STATUS_OK;
    }

    data = lxb_css_property_by_id(undef->type);
    if (data == NULL) {
        return LXB_STATUS_ERROR_NOT_EXISTS;
    }

    return cb(data->name, data->length, ctx);
}

lxb_status_t
lxb_css_property__undef_serialize_value(const void *style,
                                        lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property__undef_t *undef = style;

    if (undef->type == LXB_CSS_PROPERTY__UNDEF) {
        return cb(undef->value.data, undef->value.length, ctx);
    }

    return LXB_STATUS_OK;
}

/* _custom. */

void *
lxb_css_property__custom_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property__custom_t));
}

void *
lxb_css_property__custom_destroy(lxb_css_memory_t *memory,
                                 void *style, bool self_destroy)
{
    if (style == NULL) {
        return NULL;
    }

    if (self_destroy) {
        return lexbor_mraw_free(memory->mraw, style);
    }

    return style;
}

lxb_status_t
lxb_css_property__custom_serialize(const void *style,
                                   lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property__custom_t *custom = style;

    if (custom->value.data == NULL) {
        return LXB_STATUS_OK;
    }

    return cb(custom->value.data, custom->value.length, ctx);
}

lxb_status_t
lxb_css_property__custom_serialize_name(const void *style,
                                        lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property__custom_t *custom = style;

    return cb(custom->name.data, custom->name.length, ctx);
}

lxb_status_t
lxb_css_property__custom_serialize_value(const void *style,
                                         lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property__custom_t *custom = style;

    if (custom->value.data == NULL) {
        return LXB_STATUS_OK;
    }

    return cb(custom->value.data, custom->value.length, ctx);
}

/* Display. */

void *
lxb_css_property_display_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_display_t));
}

void *
lxb_css_property_display_destroy(lxb_css_memory_t *memory,
                                 void *property, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, property, self_destroy);
}

lxb_status_t
lxb_css_property_display_serialize(const void *property,
                                   lexbor_serialize_cb_f cb, void *ctx)
{
    lxb_status_t status;
    const lxb_css_data_t *data;
    const lxb_css_property_display_t *display = property;

    static const lexbor_str_t str_ws = lexbor_str(" ");

    data = lxb_css_value_by_id(display->a);
    if (data == NULL) {
        return LXB_STATUS_ERROR_NOT_EXISTS;
    }

    lexbor_serialize_write(cb, data->name, data->length, ctx, status);

    if (display->b == LXB_CSS_PROPERTY__UNDEF) {
        return LXB_STATUS_OK;
    }

    lexbor_serialize_write(cb, str_ws.data, str_ws.length, ctx, status);

    data = lxb_css_value_by_id(display->b);
    if (data == NULL) {
        return LXB_STATUS_ERROR_NOT_EXISTS;
    }

    lexbor_serialize_write(cb, data->name, data->length, ctx, status);

    if (display->c == LXB_CSS_PROPERTY__UNDEF) {
        return LXB_STATUS_OK;
    }

    lexbor_serialize_write(cb, str_ws.data, str_ws.length, ctx, status);

    data = lxb_css_value_by_id(display->c);
    if (data == NULL) {
        return LXB_STATUS_ERROR_NOT_EXISTS;
    }

    lexbor_serialize_write(cb, data->name, data->length, ctx, status);

    return LXB_STATUS_OK;
}

/* Order. */

void *
lxb_css_property_order_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_order_t));
}

void *
lxb_css_property_order_destroy(lxb_css_memory_t *memory,
                               void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

lxb_status_t
lxb_css_property_order_serialize(const void *style,
                                 lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_value_integer_type_sr(style, cb, ctx);
}

/* Visibility. */

void *
lxb_css_property_visibility_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_visibility_t));
}

void *
lxb_css_property_visibility_destroy(lxb_css_memory_t *memory,
                                    void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

lxb_status_t
lxb_css_property_visibility_serialize(const void *style,
                                      lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_visibility_t *vb = style;

    return lxb_css_value_serialize(vb->type, cb, ctx);
}

/* Width. */

void *
lxb_css_property_width_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_width_t));
}

void *
lxb_css_property_width_destroy(lxb_css_memory_t *memory,
                               void *property, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, property, self_destroy);
}

lxb_status_t
lxb_css_property_width_serialize(const void *property,
                                 lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_width_t *width = property;

    switch (width->type) {
        case LXB_CSS_VALUE__LENGTH:
        case LXB_CSS_VALUE__NUMBER:
            return lxb_css_value_length_sr(&width->u.length, cb, ctx);

        case LXB_CSS_VALUE__PERCENTAGE:
            return lxb_css_value_percentage_sr(&width->u.percentage, cb, ctx);

        case LXB_CSS_VALUE__UNDEF:
            /* FIXME: ???? */
            break;

        default:
            return lxb_css_value_serialize(width->type, cb, ctx);
    }

    return LXB_STATUS_OK;
}

/* Height. */

void *
lxb_css_property_height_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_height_t));
}

void *
lxb_css_property_height_destroy(lxb_css_memory_t *memory,
                                void *property, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, property, self_destroy);
}

lxb_status_t
lxb_css_property_height_serialize(const void *property,
                                  lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_property_width_serialize(property, cb, ctx);
}

/* Box-sizing. */

void *
lxb_css_property_box_sizing_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_box_sizing_t));
}

void *
lxb_css_property_box_sizing_destroy(lxb_css_memory_t *memory,
                                    void *property, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, property, self_destroy);
}

lxb_status_t
lxb_css_property_box_sizing_serialize(const void *property,
                                      lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_box_sizing_t *bsize = property;

    return lxb_css_value_serialize(bsize->type, cb, ctx);
}

/* Min-width. */

void *
lxb_css_property_min_width_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_min_width_t));
}

void *
lxb_css_property_min_width_destroy(lxb_css_memory_t *memory,
                                   void *property, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, property, self_destroy);
}

lxb_status_t
lxb_css_property_min_width_serialize(const void *property,
                                     lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_property_width_serialize(property, cb, ctx);
}

/* Min-height. */

void *
lxb_css_property_min_height_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_min_height_t));
}

void *
lxb_css_property_min_height_destroy(lxb_css_memory_t *memory,
                                    void *property, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, property, self_destroy);
}

lxb_status_t
lxb_css_property_min_height_serialize(const void *property,
                                      lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_property_width_serialize(property, cb, ctx);
}

/* Max-width. */

void *
lxb_css_property_max_width_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_max_width_t));
}

void *
lxb_css_property_max_width_destroy(lxb_css_memory_t *memory,
                                   void *property, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, property, self_destroy);
}

lxb_status_t
lxb_css_property_max_width_serialize(const void *property,
                                     lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_property_width_serialize(property, cb, ctx);
}

/* Max-height. */

void *
lxb_css_property_max_height_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_max_height_t));
}

void *
lxb_css_property_max_height_destroy(lxb_css_memory_t *memory,
                                    void *property, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, property, self_destroy);
}

lxb_status_t
lxb_css_property_max_height_serialize(const void *property,
                                      lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_property_width_serialize(property, cb, ctx);
}

/* Margin. */

void *
lxb_css_property_margin_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_margin_t));
}

void *
lxb_css_property_margin_destroy(lxb_css_memory_t *memory,
                                void *property, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, property, self_destroy);
}

lxb_status_t
lxb_css_property_margin_serialize(const void *property,
                                  lexbor_serialize_cb_f cb, void *ctx)
{
    lxb_status_t status;
    const lxb_css_property_margin_t *margin = property;

    static const lexbor_str_t str_ws = lexbor_str(" ");

    /* Top. */

    status = lxb_css_value_length_percentage_sr(&margin->top, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    if (margin->right.type == LXB_CSS_VALUE__UNDEF) {
        return LXB_STATUS_OK;
    }

    /* Right. */

    lexbor_serialize_write(cb, str_ws.data, str_ws.length, ctx, status);

    status = lxb_css_value_length_percentage_sr(&margin->right, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    if (margin->bottom.type == LXB_CSS_VALUE__UNDEF) {
        return LXB_STATUS_OK;
    }

    /* Bottom. */

    lexbor_serialize_write(cb, str_ws.data, str_ws.length, ctx, status);

    status = lxb_css_value_length_percentage_sr(&margin->bottom, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    if (margin->left.type == LXB_CSS_VALUE__UNDEF) {
        return LXB_STATUS_OK;
    }

    /* Left. */

    lexbor_serialize_write(cb, str_ws.data, str_ws.length, ctx, status);

    return lxb_css_value_length_percentage_sr(&margin->left, cb, ctx);
}

/* Margin-top. */

void *
lxb_css_property_margin_top_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_margin_top_t));
}

void *
lxb_css_property_margin_top_destroy(lxb_css_memory_t *memory,
                                    void *property, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, property, self_destroy);
}

lxb_status_t
lxb_css_property_margin_top_serialize(const void *property,
                                      lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_value_length_percentage_sr(property, cb, ctx);
}

/* Margin-right. */

void *
lxb_css_property_margin_right_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_margin_right_t));
}

void *
lxb_css_property_margin_right_destroy(lxb_css_memory_t *memory,
                                      void *property, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, property, self_destroy);
}

lxb_status_t
lxb_css_property_margin_right_serialize(const void *property,
                                        lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_value_length_percentage_sr(property, cb, ctx);
}

/* Margin-bottom. */

void *
lxb_css_property_margin_bottom_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_margin_bottom_t));
}

void *
lxb_css_property_margin_bottom_destroy(lxb_css_memory_t *memory,
                                       void *property, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, property, self_destroy);
}

lxb_status_t
lxb_css_property_margin_bottom_serialize(const void *property,
                                         lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_value_length_percentage_sr(property, cb, ctx);
}

/* Margin-left. */

void *
lxb_css_property_margin_left_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_margin_left_t));
}

void *
lxb_css_property_margin_left_destroy(lxb_css_memory_t *memory,
                                     void *property, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, property, self_destroy);
}

lxb_status_t
lxb_css_property_margin_left_serialize(const void *property,
                                       lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_value_length_percentage_sr(property, cb, ctx);
}

/* Padding. */

void *
lxb_css_property_padding_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_padding_t));
}

void *
lxb_css_property_padding_destroy(lxb_css_memory_t *memory,
                                 void *property, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, property, self_destroy);
}

lxb_status_t
lxb_css_property_padding_serialize(const void *property,
                                   lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_property_margin_serialize(property, cb, ctx);
}

/* Padding-top. */

void *
lxb_css_property_padding_top_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_padding_top_t));
}

void *
lxb_css_property_padding_top_destroy(lxb_css_memory_t *memory,
                                     void *property, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, property, self_destroy);
}

lxb_status_t
lxb_css_property_padding_top_serialize(const void *property,
                                       lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_value_length_percentage_sr(property, cb, ctx);
}

/* Padding-right. */

void *
lxb_css_property_padding_right_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_padding_right_t));
}

void *
lxb_css_property_padding_right_destroy(lxb_css_memory_t *memory,
                                       void *property, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, property, self_destroy);
}

lxb_status_t
lxb_css_property_padding_right_serialize(const void *property,
                                         lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_value_length_percentage_sr(property, cb, ctx);
}

/* Padding-bottom. */

void *
lxb_css_property_padding_bottom_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_padding_bottom_t));
}

void *
lxb_css_property_padding_bottom_destroy(lxb_css_memory_t *memory,
                                        void *property, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, property, self_destroy);
}

lxb_status_t
lxb_css_property_padding_bottom_serialize(const void *property,
                                          lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_value_length_percentage_sr(property, cb, ctx);
}

/* Padding-left. */

void *
lxb_css_property_padding_left_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_padding_left_t));
}

void *
lxb_css_property_padding_left_destroy(lxb_css_memory_t *memory,
                                      void *property, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, property, self_destroy);
}

lxb_status_t
lxb_css_property_padding_left_serialize(const void *property,
                                        lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_value_length_percentage_sr(property, cb, ctx);
}

/* Border. */

void *
lxb_css_property_border_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_border_t));
}

void *
lxb_css_property_border_destroy(lxb_css_memory_t *memory,
                                void *property, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, property, self_destroy);
}

lxb_status_t
lxb_css_property_border_serialize(const void *property,
                                  lexbor_serialize_cb_f cb, void *ctx)
{
    bool ws_print;
    lxb_status_t status;
    const lxb_css_property_border_t *border = property;

    static const lexbor_str_t str_ws = lexbor_str(" ");

    ws_print = false;

    if (border->width.type != LXB_CSS_VALUE__UNDEF) {
        status = lxb_css_value_length_type_sr(&border->width, cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        ws_print = true;
    }

    if (border->style != LXB_CSS_VALUE__UNDEF) {
        if (ws_print) {
            lexbor_serialize_write(cb, str_ws.data, str_ws.length, ctx, status);
        }

        status = lxb_css_value_serialize(border->style, cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        ws_print = true;
    }

    if (border->color.type != LXB_CSS_VALUE__UNDEF) {
        if (ws_print) {
            lexbor_serialize_write(cb, str_ws.data, str_ws.length, ctx, status);
        }

        return lxb_css_value_color_serialize(&border->color, cb, ctx);
    }

    return LXB_STATUS_OK;
}

/* Border-top. */

void *
lxb_css_property_border_top_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_border_top_t));
}

void *
lxb_css_property_border_top_destroy(lxb_css_memory_t *memory,
                                    void *property, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, property, self_destroy);
}

lxb_status_t
lxb_css_property_border_top_serialize(const void *property,
                                      lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_property_border_serialize(property, cb, ctx);
}

/* Border-right. */

void *
lxb_css_property_border_right_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_border_right_t));
}

void *
lxb_css_property_border_right_destroy(lxb_css_memory_t *memory,
                                      void *property, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, property, self_destroy);
}

lxb_status_t
lxb_css_property_border_right_serialize(const void *property,
                                        lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_property_border_serialize(property, cb, ctx);
}

/* Border-bottom. */

void *
lxb_css_property_border_bottom_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_border_bottom_t));
}

void *
lxb_css_property_border_bottom_destroy(lxb_css_memory_t *memory,
                                       void *property, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, property, self_destroy);
}

lxb_status_t
lxb_css_property_border_bottom_serialize(const void *property,
                                         lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_property_border_serialize(property, cb, ctx);
}

/* Border-left. */

void *
lxb_css_property_border_left_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_border_left_t));
}

void *
lxb_css_property_border_left_destroy(lxb_css_memory_t *memory,
                                     void *property, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, property, self_destroy);
}

lxb_status_t
lxb_css_property_border_left_serialize(const void *property,
                                       lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_property_border_serialize(property, cb, ctx);
}

void *
lxb_css_property_border_top_color_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_border_top_color_t));
}

void *
lxb_css_property_border_top_color_destroy(lxb_css_memory_t *memory,
                                          void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

lxb_status_t
lxb_css_property_border_top_color_serialize(const void *style,
                                            lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_value_color_serialize(style, cb, ctx);
}

void *
lxb_css_property_border_right_color_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_border_right_color_t));
}

void *
lxb_css_property_border_right_color_destroy(lxb_css_memory_t *memory,
                                            void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

lxb_status_t
lxb_css_property_border_right_color_serialize(const void *style,
                                              lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_value_color_serialize(style, cb, ctx);
}

void *
lxb_css_property_border_bottom_color_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_border_bottom_color_t));
}

void *
lxb_css_property_border_bottom_color_destroy(lxb_css_memory_t *memory,
                                             void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

lxb_status_t
lxb_css_property_border_bottom_color_serialize(const void *style,
                                               lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_value_color_serialize(style, cb, ctx);
}

void *
lxb_css_property_border_left_color_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_border_left_color_t));
}

void *
lxb_css_property_border_left_color_destroy(lxb_css_memory_t *memory,
                                           void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

lxb_status_t
lxb_css_property_border_left_color_serialize(const void *style,
                                             lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_value_color_serialize(style, cb, ctx);
}

void *
lxb_css_property_background_color_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_background_color_t));
}

void *
lxb_css_property_background_color_destroy(lxb_css_memory_t *memory,
                                          void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

lxb_status_t
lxb_css_property_background_color_serialize(const void *style,
                                            lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_value_color_serialize(style, cb, ctx);
}

/* Color. */

void *
lxb_css_property_color_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_color_t));
}

void *
lxb_css_property_color_destroy(lxb_css_memory_t *memory,
                               void *property, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, property, self_destroy);
}

lxb_status_t
lxb_css_property_color_serialize(const void *property,
                                 lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_value_color_serialize(property, cb, ctx);
}

void *
lxb_css_property_opacity_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_opacity_t));
}

void *
lxb_css_property_opacity_destroy(lxb_css_memory_t *memory,
                                 void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

lxb_status_t
lxb_css_property_opacity_serialize(const void *style,
                                   lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_value_number_percentage_sr(style, cb, ctx);
}

void *
lxb_css_property_position_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_position_t));
}

void *
lxb_css_property_position_destroy(lxb_css_memory_t *memory,
                                  void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

lxb_status_t
lxb_css_property_position_serialize(const void *style,
                                    lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_position_t *position = style;

    return lxb_css_value_serialize(position->type, cb, ctx);
}

void *
lxb_css_property_top_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_top_t));
}

void *
lxb_css_property_top_destroy(lxb_css_memory_t *memory,
                             void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

lxb_status_t
lxb_css_property_top_serialize(const void *style,
                               lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_value_length_percentage_sr(style, cb, ctx);
}

void *
lxb_css_property_right_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_right_t));
}

void *
lxb_css_property_right_destroy(lxb_css_memory_t *memory,
                               void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

lxb_status_t
lxb_css_property_right_serialize(const void *style,
                                 lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_property_top_serialize(style, cb, ctx);
}

void *
lxb_css_property_bottom_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_bottom_t));
}

void *
lxb_css_property_bottom_destroy(lxb_css_memory_t *memory,
                                void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

lxb_status_t
lxb_css_property_bottom_serialize(const void *style,
                                  lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_property_top_serialize(style, cb, ctx);
}

void *
lxb_css_property_left_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_left_t));
}

void *
lxb_css_property_left_destroy(lxb_css_memory_t *memory,
                              void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

lxb_status_t
lxb_css_property_left_serialize(const void *style,
                                lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_property_top_serialize(style, cb, ctx);
}

void *
lxb_css_property_inset_block_start_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_inset_block_start_t));
}

void *
lxb_css_property_inset_block_start_destroy(lxb_css_memory_t *memory,
                                           void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

lxb_status_t
lxb_css_property_inset_block_start_serialize(const void *style,
                                             lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_property_top_serialize(style, cb, ctx);
}

void *
lxb_css_property_inset_inline_start_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_inset_inline_start_t));
}

void *
lxb_css_property_inset_inline_start_destroy(lxb_css_memory_t *memory,
                                            void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

lxb_status_t
lxb_css_property_inset_inline_start_serialize(const void *style,
                                              lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_property_top_serialize(style, cb, ctx);
}

void *
lxb_css_property_inset_block_end_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_inset_block_end_t));
}

void *
lxb_css_property_inset_block_end_destroy(lxb_css_memory_t *memory,
                                         void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

lxb_status_t
lxb_css_property_inset_block_end_serialize(const void *style,
                                           lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_property_top_serialize(style, cb, ctx);
}

void *
lxb_css_property_inset_inline_end_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_inset_inline_end_t));
}

void *
lxb_css_property_inset_inline_end_destroy(lxb_css_memory_t *memory,
                                          void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

lxb_status_t
lxb_css_property_inset_inline_end_serialize(const void *style,
                                            lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_property_top_serialize(style, cb, ctx);
}

/* Text-transform. */

LXB_API void *
lxb_css_property_text_transform_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_text_transform_t));
}

LXB_API void *
lxb_css_property_text_transform_destroy(lxb_css_memory_t *memory,
                                        void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_text_transform_serialize(const void *style,
                                          lexbor_serialize_cb_f cb, void *ctx)
{
    bool ws_print;
    lxb_status_t status;
    const lxb_css_property_text_transform_t *tt = style;

    static const lexbor_str_t str_ws = lexbor_str(" ");

    ws_print = false;

    if (tt->type_case != LXB_CSS_VALUE__UNDEF) {
        status = lxb_css_value_serialize(tt->type_case, cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        ws_print = true;
    }

    if (tt->full_width != LXB_CSS_VALUE__UNDEF) {
        if (ws_print) {
            lexbor_serialize_write(cb, str_ws.data, str_ws.length, ctx, status);
        }

        status = lxb_css_value_serialize(tt->full_width, cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        ws_print = true;
    }

    if (tt->full_size_kana != LXB_CSS_VALUE__UNDEF) {
        if (ws_print) {
            lexbor_serialize_write(cb, str_ws.data, str_ws.length, ctx, status);
        }

        return lxb_css_value_serialize(tt->full_size_kana, cb, ctx);
    }

    return LXB_STATUS_OK;
}

/* Text-align. */

LXB_API void *
lxb_css_property_text_align_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_text_align_t));
}

LXB_API void *
lxb_css_property_text_align_destroy(lxb_css_memory_t *memory,
                                    void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_text_align_serialize(const void *style,
                                      lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_text_align_t *ta = style;

    return lxb_css_value_serialize(ta->type, cb, ctx);
}

/* Text-align-all. */

LXB_API void *
lxb_css_property_text_align_all_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_text_align_all_t));
}

LXB_API void *
lxb_css_property_text_align_all_destroy(lxb_css_memory_t *memory,
                                        void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_text_align_all_serialize(const void *style,
                                          lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_text_align_all_t *taa = style;

    return lxb_css_value_serialize(taa->type, cb, ctx);
}

/* Text-align-last. */

LXB_API void *
lxb_css_property_text_align_last_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_text_align_last_t));
}

LXB_API void *
lxb_css_property_text_align_last_destroy(lxb_css_memory_t *memory,
                                         void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_text_align_last_serialize(const void *style,
                                           lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_text_align_last_t *tal = style;

    return lxb_css_value_serialize(tal->type, cb, ctx);
}

/* Text-justify. */

LXB_API void *
lxb_css_property_text_justify_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_text_justify_t));
}

LXB_API void *
lxb_css_property_text_justify_destroy(lxb_css_memory_t *memory,
                                      void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_text_justify_serialize(const void *style,
                                        lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_text_justify_t *tj = style;

    return lxb_css_value_serialize(tj->type, cb, ctx);
}

/* Text-indent. */

LXB_API void *
lxb_css_property_text_indent_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_text_indent_t));
}

LXB_API void *
lxb_css_property_text_indent_destroy(lxb_css_memory_t *memory,
                                     void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_text_indent_serialize(const void *style,
                                       lexbor_serialize_cb_f cb, void *ctx)
{
    lxb_status_t status;
    const lxb_css_property_text_indent_t *ti = style;

    static const lexbor_str_t str_ws = lexbor_str(" ");

    if (ti->type == LXB_CSS_VALUE__LENGTH) {
        status = lxb_css_value_length_percentage_sr(&ti->length, cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }
    }
    else if (ti->type != LXB_CSS_VALUE__UNDEF) {
        status = lxb_css_value_serialize(ti->type, cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }
    }

    if (ti->hanging != LXB_CSS_VALUE__UNDEF) {
        lexbor_serialize_write(cb, str_ws.data, str_ws.length, ctx, status);

        status = lxb_css_value_serialize(ti->hanging, cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }
    }

    if (ti->each_line != LXB_CSS_VALUE__UNDEF) {
        lexbor_serialize_write(cb, str_ws.data, str_ws.length, ctx, status);

        return lxb_css_value_serialize(ti->each_line, cb, ctx);
    }

    return LXB_STATUS_OK;
}

/* White-space. */

LXB_API void *
lxb_css_property_white_space_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_white_space_t));
}

LXB_API void *
lxb_css_property_white_space_destroy(lxb_css_memory_t *memory,
                                     void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_white_space_serialize(const void *style,
                                       lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_white_space_t *ws = style;

    return lxb_css_value_serialize(ws->type, cb, ctx);
}

/* Tab-size. */

LXB_API void *
lxb_css_property_tab_size_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_tab_size_t));
}

LXB_API void *
lxb_css_property_tab_size_destroy(lxb_css_memory_t *memory,
                                  void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_tab_size_serialize(const void *style,
                                    lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_value_number_length_sr(style, cb, ctx);
}

/* Word-break. */

LXB_API void *
lxb_css_property_word_break_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_word_break_t));
}

LXB_API void *
lxb_css_property_word_break_destroy(lxb_css_memory_t *memory,
                                    void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_word_break_serialize(const void *style,
                                      lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_word_break_t *wb = style;

    return lxb_css_value_serialize(wb->type, cb, ctx);
}

/* Line-break. */

LXB_API void *
lxb_css_property_line_break_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_line_break_t));
}

LXB_API void *
lxb_css_property_line_break_destroy(lxb_css_memory_t *memory,
                                    void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_line_break_serialize(const void *style,
                                      lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_line_break_t *lb = style;

    return lxb_css_value_serialize(lb->type, cb, ctx);
}

/* Hyphens. */

LXB_API void *
lxb_css_property_hyphens_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_hyphens_t));
}

LXB_API void *
lxb_css_property_hyphens_destroy(lxb_css_memory_t *memory,
                                 void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_hyphens_serialize(const void *style,
                                   lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_hyphens_t *hpns = style;

    return lxb_css_value_serialize(hpns->type, cb, ctx);
}

/* Overflow-wrap. */

LXB_API void *
lxb_css_property_overflow_wrap_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_overflow_wrap_t));
}

LXB_API void *
lxb_css_property_overflow_wrap_destroy(lxb_css_memory_t *memory,
                                       void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_overflow_wrap_serialize(const void *style,
                                         lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_overflow_wrap_t *ow = style;

    return lxb_css_value_serialize(ow->type, cb, ctx);
}

/* Word-wrap. */

LXB_API void *
lxb_css_property_word_wrap_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_word_wrap_t));
}

LXB_API void *
lxb_css_property_word_wrap_destroy(lxb_css_memory_t *memory,
                                   void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_word_wrap_serialize(const void *style,
                                     lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_word_wrap_t *ww = style;

    return lxb_css_value_serialize(ww->type, cb, ctx);
}

/* Word-spacing. */

LXB_API void *
lxb_css_property_word_spacing_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_word_spacing_t));
}

LXB_API void *
lxb_css_property_word_spacing_destroy(lxb_css_memory_t *memory,
                                      void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_word_spacing_serialize(const void *style,
                                        lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_value_length_type_sr(style, cb, ctx);
}

/* Letter-spacing. */

LXB_API void *
lxb_css_property_letter_spacing_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_letter_spacing_t));
}

LXB_API void *
lxb_css_property_letter_spacing_destroy(lxb_css_memory_t *memory,
                                        void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_letter_spacing_serialize(const void *style,
                                          lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_value_length_type_sr(style, cb, ctx);
}

/* Hanging-punctuation. */

LXB_API void *
lxb_css_property_hanging_punctuation_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_hanging_punctuation_t));
}

LXB_API void *
lxb_css_property_hanging_punctuation_destroy(lxb_css_memory_t *memory,
                                             void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_hanging_punctuation_serialize(const void *style,
                                               lexbor_serialize_cb_f cb, void *ctx)
{
    bool ws_print;
    lxb_status_t status;
    const lxb_css_property_hanging_punctuation_t *hp = style;

    static const lexbor_str_t str_ws = lexbor_str(" ");

    ws_print = false;

    if (hp->type_first != LXB_CSS_VALUE__UNDEF) {
        status = lxb_css_value_serialize(hp->type_first, cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        ws_print = true;
    }

    if (hp->force_allow != LXB_CSS_VALUE__UNDEF) {
        if (ws_print) {
            lexbor_serialize_write(cb, str_ws.data, str_ws.length, ctx, status);
        }

        status = lxb_css_value_serialize(hp->force_allow, cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        ws_print = true;
    }

    if (hp->last != LXB_CSS_VALUE__UNDEF) {
        if (ws_print) {
            lexbor_serialize_write(cb, str_ws.data, str_ws.length, ctx, status);
        }

        return lxb_css_value_serialize(hp->last, cb, ctx);
    }

    return LXB_STATUS_OK;
}

/* Font-family. */

void *
lxb_css_property_font_family_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_font_family_t));
}

void *
lxb_css_property_font_family_destroy(lxb_css_memory_t *memory,
                                     void *style, bool self_destroy)
{
    lxb_css_property_font_family_t *ff = style;
    lxb_css_property_family_name_t *name, *next;

    name = ff->first;

    while (name != NULL) {
        next = name->next;

        if (!name->generic) {
            (void) lexbor_str_destroy(&name->u.str, memory->mraw, false);
        }

        lexbor_mraw_free(memory->mraw, name);

        name = next;
    }

    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

lxb_status_t
lxb_css_property_font_family_serialize(const void *style,
                                       lexbor_serialize_cb_f cb, void *ctx)
{
    lxb_status_t status;
    const lxb_css_property_font_family_t *ff = style;
    const lxb_css_property_family_name_t *name;

    static const lexbor_str_t str_comma = lexbor_str(", ");

    name = ff->first;

    while (name != NULL) {
        if (name->generic) {
            status = lxb_css_value_serialize(name->u.type, cb, ctx);
        }
        else {
            status = lxb_css_syntax_ident_or_string_serialize(name->u.str.data,
                                                              name->u.str.length,
                                                              cb, ctx);
        }

        if (status != LXB_STATUS_OK) {
            return status;
        }

        name = name->next;

        if (name != NULL) {
            lexbor_serialize_write(cb, str_comma.data, str_comma.length,
                                   ctx, status);
        }
    }

    return LXB_STATUS_OK;
}

/* Font-weight. */

void *
lxb_css_property_font_weight_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_font_weight_t));
}

void *
lxb_css_property_font_weight_destroy(lxb_css_memory_t *memory,
                                     void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

lxb_status_t
lxb_css_property_font_weight_serialize(const void *style,
                                       lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_value_number_type_sr(style, cb, ctx);
}

/* Font-stretch. */

void *
lxb_css_property_font_stretch_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_font_stretch_t));
}

void *
lxb_css_property_font_stretch_destroy(lxb_css_memory_t *memory,
                                      void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

lxb_status_t
lxb_css_property_font_stretch_serialize(const void *style,
                                        lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_value_percentage_type_sr(style, cb, ctx);
}

/* Font-style. */

void *
lxb_css_property_font_style_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_font_style_t));
}

void *
lxb_css_property_font_style_destroy(lxb_css_memory_t *memory,
                                    void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

lxb_status_t
lxb_css_property_font_style_serialize(const void *style,
                                      lexbor_serialize_cb_f cb, void *ctx)
{
    lxb_status_t status;
    const lxb_css_property_font_style_t *fs = style;

    static const lexbor_str_t str_ws = lexbor_str(" ");

    status = lxb_css_value_serialize(fs->type, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    if (fs->angle.unit != (lxb_css_unit_angle_t) LXB_CSS_UNIT__UNDEF) {
        lexbor_serialize_write(cb, str_ws.data, str_ws.length, ctx, status);

        status = lxb_css_value_angle_sr(&fs->angle, cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }
    }

    return LXB_STATUS_OK;
}

/* Font-size. */

LXB_API void *
lxb_css_property_font_size_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_font_size_t));
}

LXB_API void *
lxb_css_property_font_size_destroy(lxb_css_memory_t *memory,
                                   void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_font_size_serialize(const void *style,
                                     lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_value_length_percentage_type_sr(style, cb, ctx);
}

/* Float-reference. */

LXB_API void *
lxb_css_property_float_reference_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_float_reference_t));
}

LXB_API void *
lxb_css_property_float_reference_destroy(lxb_css_memory_t *memory,
                                         void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_float_reference_serialize(const void *style,
                                           lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_float_reference_t *fr = style;

    return lxb_css_value_serialize(fr->type, cb, ctx);
}

/* Float. */

LXB_API void *
lxb_css_property_float_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_float_t));
}

LXB_API void *
lxb_css_property_float_destroy(lxb_css_memory_t *memory,
                               void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_float_serialize(const void *style,
                                 lexbor_serialize_cb_f cb, void *ctx)
{
    lxb_status_t status;
    const lxb_css_property_float_t *flt = style;

    static const lexbor_str_t str_o = lexbor_str("(");
    static const lexbor_str_t str_cm = lexbor_str(", ");
    static const lexbor_str_t str_c = lexbor_str(")");

    status = lxb_css_value_serialize(flt->type, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    if (flt->length.type != LXB_CSS_VALUE__LENGTH) {
        return LXB_STATUS_OK;
    }

    lexbor_serialize_write(cb, str_o.data, str_o.length, ctx, status);

    status = lxb_css_value_length_sr(&flt->length.length, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    if (flt->snap_type == LXB_CSS_VALUE__UNDEF) {
        return cb(str_c.data, str_c.length, ctx);
    }

    lexbor_serialize_write(cb, str_cm.data, str_cm.length, ctx, status);

    status = lxb_css_value_serialize(flt->snap_type, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    return cb(str_c.data, str_c.length, ctx);
}

/* Clear. */

LXB_API void *
lxb_css_property_clear_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_clear_t));
}

LXB_API void *
lxb_css_property_clear_destroy(lxb_css_memory_t *memory,
                               void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_clear_serialize(const void *style,
                                 lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_clear_t *cls = style;

    return lxb_css_value_serialize(cls->type, cb, ctx);
}

/* Float-defer. */

LXB_API void *
lxb_css_property_float_defer_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_float_defer_t));
}

LXB_API void *
lxb_css_property_float_defer_destroy(lxb_css_memory_t *memory,
                                     void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_float_defer_serialize(const void *style,
                                       lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_float_defer_t *def = style;

    if (def->type == LXB_CSS_FLOAT_DEFER__INTEGER) {
        return lxb_css_value_integer_sr(&def->integer, cb, ctx);
    }

    return lxb_css_value_serialize(def->type, cb, ctx);
}

/* Float-offset. */

LXB_API void *
lxb_css_property_float_offset_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_float_offset_t));
}

LXB_API void *
lxb_css_property_float_offset_destroy(lxb_css_memory_t *memory,
                                      void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_float_offset_serialize(const void *style,
                                        lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_value_length_percentage_sr(style, cb, ctx);
}

/* Wrap-flow. */

LXB_API void *
lxb_css_property_wrap_flow_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_wrap_flow_t));
}

LXB_API void *
lxb_css_property_wrap_flow_destroy(lxb_css_memory_t *memory,
                                   void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_wrap_flow_serialize(const void *style,
                                     lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_wrap_flow_t *wf = style;

    return lxb_css_value_serialize(wf->type, cb, ctx);
}

/* Wrap-through. */

LXB_API void *
lxb_css_property_wrap_through_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_wrap_through_t));
}

LXB_API void *
lxb_css_property_wrap_through_destroy(lxb_css_memory_t *memory,
                                      void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_wrap_through_serialize(const void *style,
                                        lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_wrap_through_t *wt = style;

    return lxb_css_value_serialize(wt->type, cb, ctx);
}

/* Flex-direction. */

LXB_API void *
lxb_css_property_flex_direction_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_flex_direction_t));
}

LXB_API void *
lxb_css_property_flex_direction_destroy(lxb_css_memory_t *memory,
                                        void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_flex_direction_serialize(const void *style,
                                          lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_flex_direction_t *fd = style;

    return lxb_css_value_serialize(fd->type, cb, ctx);
}

/* Flex-wrap. */

LXB_API void *
lxb_css_property_flex_wrap_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_flex_wrap_t));
}

LXB_API void *
lxb_css_property_flex_wrap_destroy(lxb_css_memory_t *memory,
                                   void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_flex_wrap_serialize(const void *style,
                                     lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_flex_wrap_t *fw = style;

    return lxb_css_value_serialize(fw->type, cb, ctx);
}

/* Flex-flow. */

LXB_API void *
lxb_css_property_flex_flow_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_flex_flow_t));
}

LXB_API void *
lxb_css_property_flex_flow_destroy(lxb_css_memory_t *memory,
                                   void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_flex_flow_serialize(const void *style,
                                     lexbor_serialize_cb_f cb, void *ctx)
{
    lxb_status_t status;
    const lxb_css_property_flex_flow_t *ff = style;

    static const lexbor_str_t str_ws = lexbor_str(" ");

    if (ff->type_direction != LXB_CSS_VALUE__UNDEF) {
        status = lxb_css_value_serialize(ff->type_direction, cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }
    }

    if (ff->wrap != LXB_CSS_VALUE__UNDEF) {
        if (ff->type_direction != LXB_CSS_VALUE__UNDEF) {
            lexbor_serialize_write(cb, str_ws.data, str_ws.length, ctx, status);
        }

        return lxb_css_value_serialize(ff->wrap, cb, ctx);
    }

    return LXB_STATUS_OK;
}

/* Flex. */

LXB_API void *
lxb_css_property_flex_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_flex_t));
}

LXB_API void *
lxb_css_property_flex_destroy(lxb_css_memory_t *memory,
                              void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_flex_serialize(const void *style,
                                lexbor_serialize_cb_f cb, void *ctx)
{
    lxb_status_t status;
    const lxb_css_property_flex_t *flex = style;

    static const lexbor_str_t str_ws = lexbor_str(" ");

    if (flex->type != LXB_CSS_VALUE__UNDEF) {
        return lxb_css_value_serialize(flex->type, cb, ctx);
    }

    if (flex->grow.type != LXB_CSS_VALUE__UNDEF) {
        status = lxb_css_value_number_sr(&flex->grow.number, cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        if (flex->shrink.type != LXB_CSS_VALUE__UNDEF) {
            lexbor_serialize_write(cb, str_ws.data, str_ws.length, ctx, status);

            status = lxb_css_value_number_sr(&flex->shrink.number, cb, ctx);
            if (status != LXB_STATUS_OK) {
                return status;
            }
        }
    }

    if (flex->basis.type == LXB_CSS_VALUE__UNDEF) {
        return LXB_STATUS_OK;
    }

    if (flex->grow.type != LXB_CSS_VALUE__UNDEF) {
        lexbor_serialize_write(cb, str_ws.data, str_ws.length, ctx, status);
    }

    return lxb_css_property_flex_basis_serialize(&flex->basis, cb, ctx);
}

/* Flex-grow. */

LXB_API void *
lxb_css_property_flex_grow_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_flex_grow_t));
}

LXB_API void *
lxb_css_property_flex_grow_destroy(lxb_css_memory_t *memory,
                                   void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_flex_grow_serialize(const void *style,
                                     lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_value_number_type_sr(style, cb, ctx);
}

/* Flex-shrink. */

LXB_API void *
lxb_css_property_flex_shrink_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_flex_shrink_t));
}

LXB_API void *
lxb_css_property_flex_shrink_destroy(lxb_css_memory_t *memory,
                                     void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_flex_shrink_serialize(const void *style,
                                       lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_value_number_type_sr(style, cb, ctx);
}

/* Flex-basis. */

LXB_API void *
lxb_css_property_flex_basis_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_flex_basis_t));
}

LXB_API void *
lxb_css_property_flex_basis_destroy(lxb_css_memory_t *memory,
                                    void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_flex_basis_serialize(const void *style,
                                      lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_property_width_serialize(style, cb, ctx);
}

/* Justify-content. */

LXB_API void *
lxb_css_property_justify_content_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_justify_content_t));
}

LXB_API void *
lxb_css_property_justify_content_destroy(lxb_css_memory_t *memory,
                                         void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_justify_content_serialize(const void *style,
                                           lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_justify_content_t *jc = style;

    return lxb_css_value_serialize(jc->type, cb, ctx);
}

/* Align-items. */

LXB_API void *
lxb_css_property_align_items_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_align_items_t));
}

LXB_API void *
lxb_css_property_align_items_destroy(lxb_css_memory_t *memory,
                                     void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_align_items_serialize(const void *style,
                                       lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_align_items_t *ai = style;

    return lxb_css_value_serialize(ai->type, cb, ctx);
}

/* Align-self. */

LXB_API void *
lxb_css_property_align_self_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_align_self_t));
}

LXB_API void *
lxb_css_property_align_self_destroy(lxb_css_memory_t *memory,
                                    void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_align_self_serialize(const void *style,
                                      lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_align_self_t *as = style;

    return lxb_css_value_serialize(as->type, cb, ctx);
}

/* Align-content. */

LXB_API void *
lxb_css_property_align_content_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_align_content_t));
}

LXB_API void *
lxb_css_property_align_content_destroy(lxb_css_memory_t *memory,
                                       void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_align_content_serialize(const void *style,
                                         lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_align_content_t *ac = style;

    return lxb_css_value_serialize(ac->type, cb, ctx);
}

/* Dominant-baseline. */

LXB_API void *
lxb_css_property_dominant_baseline_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_dominant_baseline_t));
}

LXB_API void *
lxb_css_property_dominant_baseline_destroy(lxb_css_memory_t *memory,
                                           void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_dominant_baseline_serialize(const void *style,
                                             lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_dominant_baseline_t *db = style;

    return lxb_css_value_serialize(db->type, cb, ctx);
}

/* Vertical-align. */

LXB_API void *
lxb_css_property_vertical_align_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_vertical_align_t));
}

LXB_API void *
lxb_css_property_vertical_align_destroy(lxb_css_memory_t *memory,
                                        void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_vertical_align_serialize(const void *style,
                                          lexbor_serialize_cb_f cb, void *ctx)
{
    bool is;
    lxb_status_t status;
    const lxb_css_property_vertical_align_t *va = style;

    static const lexbor_str_t str_ws = lexbor_str(" ");

    is = false;

    if (va->type != LXB_CSS_VALUE__UNDEF) {
        status = lxb_css_value_serialize(va->type, cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        is = true;
    }

    if (va->alignment.type != LXB_CSS_VALUE__UNDEF) {
        if (is) {
            lexbor_serialize_write(cb, str_ws.data, str_ws.length, ctx, status);
        }

        status = lxb_css_value_serialize(va->alignment.type, cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        is = true;
    }

    if (va->shift.type != LXB_CSS_VALUE__UNDEF) {
        if (is) {
            lexbor_serialize_write(cb, str_ws.data, str_ws.length, ctx, status);
        }

        status = lxb_css_value_length_percentage_sr(&va->shift, cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }
    }

    return LXB_STATUS_OK;
}

/* Baseline-source. */

LXB_API void *
lxb_css_property_baseline_source_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_baseline_source_t));
}

LXB_API void *
lxb_css_property_baseline_source_destroy(lxb_css_memory_t *memory,
                                         void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_baseline_source_serialize(const void *style,
                                           lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_baseline_source_t *bs = style;

    return lxb_css_value_serialize(bs->type, cb, ctx);
}

/* Alignment-baseline. */

LXB_API void *
lxb_css_property_alignment_baseline_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_alignment_baseline_t));
}

LXB_API void *
lxb_css_property_alignment_baseline_destroy(lxb_css_memory_t *memory,
                                            void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_alignment_baseline_serialize(const void *style,
                                              lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_alignment_baseline_t *ab = style;

    return lxb_css_value_serialize(ab->type, cb, ctx);
}

/* Baseline-shift. */

LXB_API void *
lxb_css_property_baseline_shift_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_baseline_shift_t));
}

LXB_API void *
lxb_css_property_baseline_shift_destroy(lxb_css_memory_t *memory,
                                        void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_baseline_shift_serialize(const void *style,
                                          lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_value_length_percentage_sr(style, cb, ctx);
}

/* Line-height. */

LXB_API void *
lxb_css_property_line_height_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_line_height_t));
}

LXB_API void *
lxb_css_property_line_height_destroy(lxb_css_memory_t *memory,
                                     void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_line_height_serialize(const void *style,
                                       lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_value_number_length_percentage_type_sr(style, cb, ctx);
}

/* Z-index. */

LXB_API void *
lxb_css_property_z_index_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw, sizeof(lxb_css_property_z_index_t));
}

LXB_API void *
lxb_css_property_z_index_destroy(lxb_css_memory_t *memory,
                                 void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_z_index_serialize(const void *style,
                                   lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_value_integer_type_sr(style, cb, ctx);
}

/* Direction. */

LXB_API void *
lxb_css_property_direction_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_direction_t));
}

LXB_API void *
lxb_css_property_direction_destroy(lxb_css_memory_t *memory,
                                   void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_direction_serialize(const void *style,
                                     lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_direction_t *dr = style;

    return lxb_css_value_serialize(dr->type, cb, ctx);
}

/* Unicode-bidi. */

LXB_API void *
lxb_css_property_unicode_bidi_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_unicode_bidi_t));
}

LXB_API void *
lxb_css_property_unicode_bidi_destroy(lxb_css_memory_t *memory,
                                      void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_unicode_bidi_serialize(const void *style,
                                        lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_unicode_bidi_t *ub = style;

    return lxb_css_value_serialize(ub->type, cb, ctx);
}

/* Writing-mode. */

LXB_API void *
lxb_css_property_writing_mode_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_writing_mode_t));
}

LXB_API void *
lxb_css_property_writing_mode_destroy(lxb_css_memory_t *memory,
                                      void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_writing_mode_serialize(const void *style,
                                        lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_writing_mode_t *wm = style;

    return lxb_css_value_serialize(wm->type, cb, ctx);
}

/* Text-orientation. */

LXB_API void *
lxb_css_property_text_orientation_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_text_orientation_t));
}

LXB_API void *
lxb_css_property_text_orientation_destroy(lxb_css_memory_t *memory,
                                          void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_text_orientation_serialize(const void *style,
                                            lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_text_orientation_t *to = style;

    return lxb_css_value_serialize(to->type, cb, ctx);
}

/* Text-combine-upright. */

LXB_API void *
lxb_css_property_text_combine_upright_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_text_combine_upright_t));
}

LXB_API void *
lxb_css_property_text_combine_upright_destroy(lxb_css_memory_t *memory,
                                              void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_text_combine_upright_serialize(const void *style,
                                                lexbor_serialize_cb_f cb, void *ctx)
{
    lxb_status_t status;
    const lxb_css_property_text_combine_upright_t *tcu = style;

    static const lexbor_str_t str_ws = lexbor_str(" ");

    if (tcu->type == LXB_CSS_TEXT_COMBINE_UPRIGHT_DIGITS) {
        status = lxb_css_value_serialize(tcu->type, cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        if (tcu->digits.num != 0) {
            lexbor_serialize_write(cb, str_ws.data, str_ws.length, ctx, status);

            return lxb_css_value_integer_sr(&tcu->digits, cb, ctx);
        }

        return LXB_STATUS_OK;
    }

    return lxb_css_value_serialize(tcu->type, cb, ctx);
}

/* Overflow-x. */

LXB_API void *
lxb_css_property_overflow_x_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_overflow_x_t));
}

LXB_API void *
lxb_css_property_overflow_x_destroy(lxb_css_memory_t *memory,
                                    void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_overflow_x_serialize(const void *style,
                                      lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_overflow_x_t *ox = style;

    return lxb_css_value_serialize(ox->type, cb, ctx);
}

/* Overflow-y. */

LXB_API void *
lxb_css_property_overflow_y_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_overflow_y_t));
}

LXB_API void *
lxb_css_property_overflow_y_destroy(lxb_css_memory_t *memory,
                                    void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_overflow_y_serialize(const void *style,
                                      lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_overflow_y_t *oy = style;

    return lxb_css_value_serialize(oy->type, cb, ctx);
}

/* Overflow-block. */

LXB_API void *
lxb_css_property_overflow_block_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_overflow_block_t));
}

LXB_API void *
lxb_css_property_overflow_block_destroy(lxb_css_memory_t *memory,
                                        void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_overflow_block_serialize(const void *style,
                                          lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_overflow_block_t *ob = style;

    return lxb_css_value_serialize(ob->type, cb, ctx);
}

/* Overflow-inline. */

LXB_API void *
lxb_css_property_overflow_inline_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_overflow_inline_t));
}

LXB_API void *
lxb_css_property_overflow_inline_destroy(lxb_css_memory_t *memory,
                                         void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_overflow_inline_serialize(const void *style,
                                           lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_overflow_inline_t *oi = style;

    return lxb_css_value_serialize(oi->type, cb, ctx);
}

/* Text-overflow. */

LXB_API void *
lxb_css_property_text_overflow_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_text_overflow_t));
}

LXB_API void *
lxb_css_property_text_overflow_destroy(lxb_css_memory_t *memory,
                                       void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_text_overflow_serialize(const void *style,
                                         lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_text_overflow_t *to = style;

    return lxb_css_value_serialize(to->type, cb, ctx);
}

/* Text-decoration-line. */

LXB_API void *
lxb_css_property_text_decoration_line_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_text_decoration_line_t));
}

LXB_API void *
lxb_css_property_text_decoration_line_destroy(lxb_css_memory_t *memory,
                                              void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_text_decoration_line_serialize(const void *style,
                                                lexbor_serialize_cb_f cb, void *ctx)
{
    bool itis;
    lxb_status_t status;
    const lxb_css_property_text_decoration_line_t *tdl = style;

    static const lexbor_str_t str_ws = lexbor_str(" ");

    itis = false;

    if (tdl->underline != LXB_CSS_VALUE__UNDEF) {
        status = lxb_css_value_serialize(tdl->underline, cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        itis = true;
    }

    if (tdl->overline != LXB_CSS_VALUE__UNDEF) {
        if (itis) {
            lexbor_serialize_write(cb, str_ws.data, str_ws.length, ctx, status);
        }

        status = lxb_css_value_serialize(tdl->overline, cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        itis = true;
    }

    if (tdl->line_through != LXB_CSS_VALUE__UNDEF) {
        if (itis) {
            lexbor_serialize_write(cb, str_ws.data, str_ws.length, ctx, status);
        }

        status = lxb_css_value_serialize(tdl->line_through, cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        itis = true;
    }

    if (tdl->blink != LXB_CSS_VALUE__UNDEF) {
        if (itis) {
            lexbor_serialize_write(cb, str_ws.data, str_ws.length, ctx, status);
        }

        return lxb_css_value_serialize(tdl->blink, cb, ctx);
    }

    if (itis) {
        return LXB_STATUS_OK;
    }

    return lxb_css_value_serialize(tdl->type, cb, ctx);
}

/* Text-decoration-style. */

LXB_API void *
lxb_css_property_text_decoration_style_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_text_decoration_style_t));
}

LXB_API void *
lxb_css_property_text_decoration_style_destroy(lxb_css_memory_t *memory,
                                               void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_text_decoration_style_serialize(const void *style,
                                                 lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_property_text_decoration_style_t *tds = style;

    return lxb_css_value_serialize(tds->type, cb, ctx);
}

/* Text-decoration-color. */

LXB_API void *
lxb_css_property_text_decoration_color_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_text_decoration_color_t));
}

LXB_API void *
lxb_css_property_text_decoration_color_destroy(lxb_css_memory_t *memory,
                                               void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_text_decoration_color_serialize(const void *style,
                                                 lexbor_serialize_cb_f cb, void *ctx)
{
    return lxb_css_property_color_serialize(style, cb, ctx);
}

/* Text-decoration. */

LXB_API void *
lxb_css_property_text_decoration_create(lxb_css_memory_t *memory)
{
    return lexbor_mraw_calloc(memory->mraw,
                              sizeof(lxb_css_property_text_decoration_t));
}

LXB_API void *
lxb_css_property_text_decoration_destroy(lxb_css_memory_t *memory,
                                         void *style, bool self_destroy)
{
    return lxb_css_property__undef_destroy(memory, style, self_destroy);
}

LXB_API lxb_status_t
lxb_css_property_text_decoration_serialize(const void *style,
                                           lexbor_serialize_cb_f cb, void *ctx)
{
    bool itis;
    lxb_status_t status;
    const lxb_css_property_text_decoration_t *td = style;

    static const lexbor_str_t str_ws = lexbor_str(" ");

    itis = false;

    if (td->line.type != LXB_CSS_VALUE__UNDEF
        || td->line.underline != LXB_CSS_VALUE__UNDEF
        || td->line.overline != LXB_CSS_VALUE__UNDEF
        || td->line.line_through != LXB_CSS_VALUE__UNDEF
        || td->line.blink != LXB_CSS_VALUE__UNDEF)
    {
        status = lxb_css_property_text_decoration_line_serialize(&td->line,
                                                                 cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        itis = true;
    }

    if (td->style.type != LXB_CSS_VALUE__UNDEF) {
        if (itis) {
            lexbor_serialize_write(cb, str_ws.data, str_ws.length, ctx, status);
        }

        status = lxb_css_property_text_decoration_style_serialize(&td->style,
                                                                  cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        itis = true;
    }

    if (td->color.type != LXB_CSS_VALUE__UNDEF) {
        if (itis) {
            lexbor_serialize_write(cb, str_ws.data, str_ws.length, ctx, status);
        }

        return lxb_css_property_text_decoration_color_serialize(&td->color,
                                                                cb, ctx);
    }

    return LXB_STATUS_OK;
}
