/*
 * Copyright (C) 2022-2026 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/css/css.h"
#include "lexbor/css/value/res.h"

#include "lexbor/core/serialize.h"
#include "lexbor/core/conv.h"


#ifndef LEXBOR_DISABLE_INTERNAL_EXTERN
    LXB_EXTERN const lxb_char_t lexbor_str_res_map_hex_to_char_lowercase[17];
    LXB_EXTERN const char *lexbor_str_res_char_to_two_hex_value_lowercase[257];
#endif

static const lexbor_str_t lxb_css_value_str_ws = lexbor_str(" ");
static const lexbor_str_t lxb_str_comma = lexbor_str(", ");
static const lexbor_str_t lxb_str_alpha = lexbor_str(" / ");
static const lexbor_str_t lxb_str_rp = lexbor_str(")");


const lxb_css_data_t *
lxb_css_value_by_id(uintptr_t id)
{
    if (id < LXB_CSS_VALUE__LAST_ENTRY) {
        return &lxb_css_value_data[id];
    }

    return NULL;
}

lxb_css_value_type_t
lxb_css_value_by_name(const lxb_char_t *name, size_t length)
{
    const lexbor_shs_entry_t *entry;

    entry = lexbor_shs_entry_get_lower_static(lxb_css_value_shs, name, length);
    if (entry == NULL) {
        return LXB_CSS_VALUE__UNDEF;
    }

    return (lxb_css_value_type_t) (uintptr_t) entry->value;
}

lxb_status_t
lxb_css_value_serialize(lxb_css_value_type_t type,
                        lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_css_data_t *data;

    if (type >= LXB_CSS_VALUE__LAST_ENTRY) {
        return LXB_STATUS_ERROR_WRONG_ARGS;
    }

    data = &lxb_css_value_data[type];

    return cb(data->name, data->length, ctx);
}

lxb_status_t
lxb_css_value_percentage_sr(const lxb_css_value_percentage_t *percent,
                               lexbor_serialize_cb_f cb, void *ctx)
{
    size_t length;
    lxb_char_t buf[128];
    lxb_status_t status;

    static const lexbor_str_t str_per = lexbor_str("%");

    /* FIXME: If length != sizeof(buf)? */
    length = lexbor_conv_float_to_data(percent->num, buf, sizeof(buf));

    lexbor_serialize_write(cb, buf, length, ctx, status);
    lexbor_serialize_write(cb, str_per.data, str_per.length, ctx, status);

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_css_value_length_sr(const lxb_css_value_length_t *len,
                        lexbor_serialize_cb_f cb, void *ctx)
{
    size_t length;
    lxb_char_t buf[128];
    lxb_status_t status;
    const lxb_css_data_t *unit;

    /* FIXME: If length != sizeof(buf)? */
    length = lexbor_conv_float_to_data(len->num, buf, sizeof(buf));

    lexbor_serialize_write(cb, buf, length, ctx, status);

    if (len->unit == LXB_CSS_UNIT__UNDEF) {
        return LXB_STATUS_OK;
    }

    unit = lxb_css_unit_by_id(len->unit);
    if (unit == NULL) {
        return LXB_STATUS_OK;
    }

    lexbor_serialize_write(cb, unit->name, unit->length, ctx, status);

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_css_value_number_sr(const lxb_css_value_number_t *number,
                        lexbor_serialize_cb_f cb, void *ctx)
{
    size_t length;
    lxb_char_t buf[128];
    lxb_status_t status;

    /* FIXME: If length != sizeof(buf)? */
    length = lexbor_conv_float_to_data(number->num, buf, sizeof(buf));

    lexbor_serialize_write(cb, buf, length, ctx, status);

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_css_value_integer_sr(const lxb_css_value_integer_t *integer,
                         lexbor_serialize_cb_f cb, void *ctx)
{
    size_t length;
    lxb_char_t buf[128];
    lxb_status_t status;

    /* FIXME: If length != sizeof(buf)? */
    length = lexbor_conv_long_to_data(integer->num, buf, sizeof(buf));

    lexbor_serialize_write(cb, buf, length, ctx, status);

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_css_value_length_percentage_sr(const lxb_css_value_length_percentage_t *lp,
                                   lexbor_serialize_cb_f cb, void *ctx)
{
    switch (lp->type) {
        case LXB_CSS_VALUE__LENGTH:
        case LXB_CSS_VALUE__NUMBER:
            return lxb_css_value_length_sr(&lp->u.length, cb, ctx);

        case LXB_CSS_VALUE__PERCENTAGE:
            return lxb_css_value_percentage_sr(&lp->u.percentage, cb, ctx);

        case LXB_CSS_VALUE__UNDEF:
            /* FIXME: ???? */
            break;

        default:
            return lxb_css_value_serialize(lp->type, cb, ctx);
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_css_value_number_length_sr(const lxb_css_value_number_length_t *nl,
                               lexbor_serialize_cb_f cb, void *ctx)
{
    switch (nl->type) {
        case LXB_CSS_VALUE__LENGTH:
        case LXB_CSS_VALUE__NUMBER:
            return lxb_css_value_length_sr(&nl->u.length, cb, ctx);

        default:
            return lxb_css_value_serialize(nl->type, cb, ctx);
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_css_value_number_percentage_sr(const lxb_css_value_number_percentage_t *np,
                                   lexbor_serialize_cb_f cb, void *ctx)
{
    switch (np->type) {
        case LXB_CSS_VALUE__NUMBER:
            return lxb_css_value_number_sr(&np->u.number, cb, ctx);

        case LXB_CSS_VALUE__PERCENTAGE:
            return lxb_css_value_percentage_sr(&np->u.percentage, cb, ctx);

        case LXB_CSS_VALUE__UNDEF:
            /* FIXME: ???? */
            break;

        default:
            return lxb_css_value_serialize(np->type, cb, ctx);
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_css_value_length_type_sr(const lxb_css_value_length_type_t *lt,
                             lexbor_serialize_cb_f cb, void *ctx)
{
    switch (lt->type) {
        case LXB_CSS_VALUE__LENGTH:
        case LXB_CSS_VALUE__NUMBER:
            return lxb_css_value_length_sr(&lt->length, cb, ctx);

        case LXB_CSS_VALUE__UNDEF:
            /* FIXME: ???? */
            break;

        default:
            return lxb_css_value_serialize(lt->type, cb, ctx);
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_css_value_percentage_type_sr(const lxb_css_value_percentage_type_t *pt,
                                 lexbor_serialize_cb_f cb, void *ctx)
{
    switch (pt->type) {
        case LXB_CSS_VALUE__PERCENTAGE:
            return lxb_css_value_percentage_sr(&pt->percentage, cb, ctx);

        case LXB_CSS_VALUE__UNDEF:
            /* FIXME: ???? */
            break;

        default:
            return lxb_css_value_serialize(pt->type, cb, ctx);
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_css_value_number_type_sr(const lxb_css_value_number_type_t *num,
                             lexbor_serialize_cb_f cb, void *ctx)
{
    switch (num->type) {
        case LXB_CSS_VALUE__NUMBER:
            return lxb_css_value_number_sr(&num->number, cb, ctx);

        default:
            return lxb_css_value_serialize(num->type, cb, ctx);
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_css_value_integer_type_sr(const lxb_css_value_integer_type_t *num,
                              lexbor_serialize_cb_f cb, void *ctx)
{
    switch (num->type) {
        case LXB_CSS_VALUE__INTEGER:
            return lxb_css_value_integer_sr(&num->integer, cb, ctx);

        default:
            return lxb_css_value_serialize(num->type, cb, ctx);
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_css_value_length_percentage_type_sr(const lxb_css_value_length_percentage_type_t *lpt,
                                        lexbor_serialize_cb_f cb, void *ctx)
{
    switch (lpt->type) {
        case LXB_CSS_VALUE__LENGTH:
            return lxb_css_value_length_percentage_sr(&lpt->length, cb, ctx);

        default:
            return lxb_css_value_serialize(lpt->type, cb, ctx);
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_css_value_number_length_percentage_type_sr(const lxb_css_value_number_length_percentage_t *nlp,
                                               lexbor_serialize_cb_f cb, void *ctx)
{
    switch (nlp->type) {
        case LXB_CSS_VALUE__NUMBER:
            return lxb_css_value_number_sr(&nlp->u.number, cb, ctx);

        case LXB_CSS_VALUE__LENGTH:
            return lxb_css_value_length_sr(&nlp->u.length, cb, ctx);

        case LXB_CSS_VALUE__PERCENTAGE:
            return lxb_css_value_percentage_sr(&nlp->u.percentage, cb, ctx);

        default:
            return lxb_css_value_serialize(nlp->type, cb, ctx);
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_css_value_angle_sr(const lxb_css_value_angle_t *angle,
                       lexbor_serialize_cb_f cb, void *ctx)
{
    size_t length;
    lxb_char_t buf[128];
    lxb_status_t status;
    const lxb_css_data_t *data;

    /* FIXME: If length != sizeof(buf)? */
    length = lexbor_conv_float_to_data(angle->num, buf, sizeof(buf));

    lexbor_serialize_write(cb, buf, length, ctx, status);

    data = lxb_css_unit_by_id(angle->unit);

    return cb(data->name, data->length, ctx);
}

lxb_status_t
lxb_css_value_hue_sr(const lxb_css_value_hue_t *hue,
                     lexbor_serialize_cb_f cb, void *ctx)
{
    switch (hue->type) {
        case LXB_CSS_VALUE__NUMBER:
            return lxb_css_value_number_sr(&hue->u.number, cb, ctx);

        case LXB_CSS_VALUE__ANGLE:
            return lxb_css_value_angle_sr(&hue->u.angle, cb, ctx);

        case LXB_CSS_VALUE__UNDEF:
            /* FIXME: ???? */
            break;

        default:
            return lxb_css_value_serialize(hue->type, cb, ctx);
    }

    return LXB_STATUS_OK;
}

static lxb_status_t
lxb_css_value_color_hex_sr(const lxb_css_value_color_hex_t *hex,
                           lexbor_serialize_cb_f cb, void *ctx)
{
    lxb_status_t status;
    static const lexbor_str_t str_hash = lexbor_str("#");
    const lxb_css_value_color_hex_rgba_t *rgba = &hex->rgba;

    const lxb_char_t *hmo = lexbor_str_res_map_hex_to_char_lowercase;
    const char **hmt = lexbor_str_res_char_to_two_hex_value_lowercase;

    lexbor_serialize_write(cb, str_hash.data, str_hash.length, ctx, status);

    switch (hex->type) {
        case LXB_CSS_PROPERTY_COLOR_HEX_TYPE_3:
        case LXB_CSS_PROPERTY_COLOR_HEX_TYPE_4:
            lexbor_serialize_write(cb, &hmo[rgba->r], 1, ctx, status);
            lexbor_serialize_write(cb, &hmo[rgba->g], 1, ctx, status);
            lexbor_serialize_write(cb, &hmo[rgba->b], 1, ctx, status);

            if (hex->type == LXB_CSS_PROPERTY_COLOR_HEX_TYPE_4) {
                lexbor_serialize_write(cb, &hmo[rgba->a], 1, ctx, status);
            }

            break;

        case LXB_CSS_PROPERTY_COLOR_HEX_TYPE_6:
        case LXB_CSS_PROPERTY_COLOR_HEX_TYPE_8:
            lexbor_serialize_write(cb, hmt[rgba->r], 2, ctx, status);
            lexbor_serialize_write(cb, hmt[rgba->g], 2, ctx, status);
            lexbor_serialize_write(cb, hmt[rgba->b], 2, ctx, status);

            if (hex->type == LXB_CSS_PROPERTY_COLOR_HEX_TYPE_8) {
                lexbor_serialize_write(cb, hmt[rgba->a], 2, ctx, status);
            }

            break;

        default:
            break;
    }

    return LXB_STATUS_OK;
}

static lxb_status_t
lxb_css_value_color_rgb_sr(const lxb_css_value_color_rgba_t *rgb,
                           lexbor_serialize_cb_f cb, void *ctx,
                           lxb_css_value_type_t type)
{
    lxb_status_t status;
    const lexbor_str_t *sep;
    static const lexbor_str_t str_rgb = lexbor_str("rgb(");
    static const lexbor_str_t str_rgba = lexbor_str("rgba(");

    if (type == LXB_CSS_COLOR_RGB) {
        lexbor_serialize_write(cb, str_rgb.data, str_rgb.length, ctx, status);
    }
    else {
        lexbor_serialize_write(cb, str_rgba.data, str_rgba.length, ctx, status);
    }

    sep = (rgb->old) ? &lxb_str_comma : &lxb_css_value_str_ws;

    status = lxb_css_value_number_percentage_sr(&rgb->r, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    lexbor_serialize_write(cb, sep->data, sep->length, ctx, status);

    status = lxb_css_value_number_percentage_sr(&rgb->g, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    lexbor_serialize_write(cb, sep->data, sep->length, ctx, status);

    status = lxb_css_value_number_percentage_sr(&rgb->b, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    if (rgb->a.type == LXB_CSS_VALUE__UNDEF) {
        return cb(lxb_str_rp.data, lxb_str_rp.length, ctx);
    }

    sep = (rgb->old) ? &lxb_str_comma : &lxb_str_alpha;

    lexbor_serialize_write(cb, sep->data, sep->length, ctx, status);

    status = lxb_css_value_number_percentage_sr(&rgb->a, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    return cb(lxb_str_rp.data, lxb_str_rp.length, ctx);
}

static lxb_status_t
lxb_css_value_color_hsl_sr(const lxb_css_value_color_hsla_t *hsl,
                           lexbor_serialize_cb_f cb, void *ctx,
                           lxb_css_value_type_t type)
{
    lxb_status_t status;
    const lexbor_str_t *sep;
    static const lexbor_str_t str_hsl = lexbor_str("hsl(");
    static const lexbor_str_t str_hsla = lexbor_str("hsla(");
    static const lexbor_str_t str_hwb = lexbor_str("hwb(");

    switch (type) {
        case LXB_CSS_COLOR_HSL:
            status = cb(str_hsl.data, str_hsl.length, ctx);
            break;

        case LXB_CSS_COLOR_HSLA:
            status = cb(str_hsla.data, str_hsla.length, ctx);
            break;

        case LXB_CSS_COLOR_HWB:
            status = cb(str_hwb.data, str_hwb.length, ctx);
            break;

        default:
            return LXB_STATUS_ERROR_WRONG_ARGS;
    }

    if (status != LXB_STATUS_OK) {
        return status;
    }

    sep = (hsl->old) ? &lxb_str_comma : &lxb_css_value_str_ws;

    status = lxb_css_value_hue_sr(&hsl->h, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    lexbor_serialize_write(cb, sep->data, sep->length, ctx, status);

    status = lxb_css_value_percentage_type_sr(&hsl->s, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    lexbor_serialize_write(cb, sep->data, sep->length, ctx, status);

    status = lxb_css_value_percentage_type_sr(&hsl->l, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    if (hsl->a.type == LXB_CSS_VALUE__UNDEF) {
        return cb(lxb_str_rp.data, lxb_str_rp.length, ctx);
    }

    sep = (hsl->old) ? &lxb_str_comma : &lxb_str_alpha;

    lexbor_serialize_write(cb, sep->data, sep->length, ctx, status);

    status = lxb_css_value_number_percentage_sr(&hsl->a, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    return cb(lxb_str_rp.data, lxb_str_rp.length, ctx);
}

static lxb_status_t
lxb_css_value_color_lab_sr(const lxb_css_value_color_lab_t *lab,
                           lexbor_serialize_cb_f cb, void *ctx,
                           lxb_css_value_type_t type)
{
    lxb_status_t status;
    static const lexbor_str_t str_lab = lexbor_str("lab(");
    static const lexbor_str_t str_oklab = lexbor_str("oklab(");

    switch (type) {
        case LXB_CSS_COLOR_LAB:
            status = cb(str_lab.data, str_lab.length, ctx);
            break;

        case LXB_CSS_COLOR_OKLAB:
            status = cb(str_oklab.data, str_oklab.length, ctx);
            break;

        default:
            return LXB_STATUS_ERROR_WRONG_ARGS;
    }

    if (status != LXB_STATUS_OK) {
        return status;
    }

    status = lxb_css_value_number_percentage_sr(&lab->l, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    lexbor_serialize_write(cb, lxb_css_value_str_ws.data,
                           lxb_css_value_str_ws.length, ctx, status);

    status = lxb_css_value_number_percentage_sr(&lab->a, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    lexbor_serialize_write(cb, lxb_css_value_str_ws.data,
                           lxb_css_value_str_ws.length, ctx, status);

    status = lxb_css_value_number_percentage_sr(&lab->b, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    if (lab->alpha.type == LXB_CSS_VALUE__UNDEF) {
        return cb(lxb_str_rp.data, lxb_str_rp.length, ctx);
    }

    lexbor_serialize_write(cb, lxb_str_alpha.data, lxb_str_alpha.length,
                           ctx, status);

    status = lxb_css_value_number_percentage_sr(&lab->alpha, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    return cb(lxb_str_rp.data, lxb_str_rp.length, ctx);
}

static lxb_status_t
lxb_css_value_color_lch_sr(const lxb_css_value_color_lch_t *lch,
                           lexbor_serialize_cb_f cb, void *ctx,
                           lxb_css_value_type_t type)
{
    lxb_status_t status;
    static const lexbor_str_t str_lch = lexbor_str("lch(");
    static const lexbor_str_t str_oklch = lexbor_str("oklch(");

    switch (type) {
        case LXB_CSS_COLOR_LCH:
            status = cb(str_lch.data, str_lch.length, ctx);
            break;

        case LXB_CSS_COLOR_OKLCH:
            status = cb(str_oklch.data, str_oklch.length, ctx);
            break;

        default:
            return LXB_STATUS_ERROR_WRONG_ARGS;
    }

    if (status != LXB_STATUS_OK) {
        return status;
    }

    status = lxb_css_value_number_percentage_sr(&lch->l, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    lexbor_serialize_write(cb, lxb_css_value_str_ws.data,
                           lxb_css_value_str_ws.length, ctx, status);

    status = lxb_css_value_number_percentage_sr(&lch->c, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    lexbor_serialize_write(cb, lxb_css_value_str_ws.data,
                           lxb_css_value_str_ws.length, ctx, status);

    status = lxb_css_value_hue_sr(&lch->h, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    if (lch->a.type == LXB_CSS_VALUE__UNDEF) {
        return cb(lxb_str_rp.data, lxb_str_rp.length, ctx);
    }

    lexbor_serialize_write(cb, lxb_str_alpha.data, lxb_str_alpha.length,
                           ctx, status);

    status = lxb_css_value_number_percentage_sr(&lch->a, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    return cb(lxb_str_rp.data, lxb_str_rp.length, ctx);
}

lxb_status_t
lxb_css_value_color_serialize(const lxb_css_value_color_t *color,
                              lexbor_serialize_cb_f cb, void *ctx)
{
    switch (color->type) {
        case LXB_CSS_COLOR_HEX:
            return lxb_css_value_color_hex_sr(&color->u.hex, cb, ctx);

        case LXB_CSS_COLOR_RGB:
        case LXB_CSS_COLOR_RGBA:
            return lxb_css_value_color_rgb_sr(&color->u.rgb, cb, ctx,
                                              color->type);

        case LXB_CSS_COLOR_HSL:
        case LXB_CSS_COLOR_HSLA:
        case LXB_CSS_COLOR_HWB:
            return lxb_css_value_color_hsl_sr(&color->u.hsl, cb, ctx,
                                              color->type);

        case LXB_CSS_COLOR_LAB:
        case LXB_CSS_COLOR_OKLAB:
            return lxb_css_value_color_lab_sr(&color->u.lab, cb, ctx,
                                              color->type);

        case LXB_CSS_COLOR_LCH:
        case LXB_CSS_COLOR_OKLCH:
            return lxb_css_value_color_lch_sr(&color->u.lch, cb, ctx,
                                              color->type);

        case LXB_CSS_VALUE__UNDEF:
            break;

        default:
            return lxb_css_value_serialize(color->type, cb, ctx);
    }

    return LXB_STATUS_OK;
}
