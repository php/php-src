/*
 * Copyright (C) 2021-2026 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/css/property.h"
#include "lexbor/css/parser.h"
#include "lexbor/css/rule.h"
#include "lexbor/css/value.h"
#include "lexbor/css/unit.h"
#include "lexbor/css/property/state.h"
#include "lexbor/css/property/res.h"


#ifndef LEXBOR_DISABLE_INTERNAL_EXTERN
    LXB_EXTERN const lxb_char_t lexbor_str_res_map_hex[256];
    LXB_EXTERN const lxb_char_t lexbor_str_res_map_lowercase[256];
#endif

#include "lexbor/core/conv.h"


#define lxb_css_property_state_check_token(parser, token)                     \
    if ((token) == NULL) {                                                    \
        return lxb_css_parser_memory_fail(parser);                            \
    }

#define lxb_css_property_state_get_type(parser, token, type)                  \
    do {                                                                      \
        lxb_css_syntax_parser_consume(parser);                                \
                                                                              \
        token = lxb_css_syntax_parser_token_wo_ws(parser);                    \
        lxb_css_property_state_check_token(parser, token);                    \
                                                                              \
        if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {                      \
            return lxb_css_parser_success(parser);                            \
        }                                                                     \
                                                                              \
        type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data, \
                                  lxb_css_syntax_token_ident(token)->length); \
    }                                                                         \
    while (false)

#define LXB_CSS_PROPERTY_STATE_HEX_MASK(n)                                    \
    ((((uint32_t) 1 << (32 - (n))) - 1) << (n))


static bool
lxb_css_property_state_color_rgba_old(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token,
                                      lxb_css_value_color_t *color);
static bool
lxb_css_property_state_color_hsla_old(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token,
                                      lxb_css_value_color_hsla_t *hsl);

static bool
lxb_css_property_state_length(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token,
                              lxb_css_value_length_t *length)
{
    const lxb_css_data_t *unit;

    switch (token->type) {
        case LXB_CSS_SYNTAX_TOKEN_DIMENSION:
            unit = lxb_css_unit_absolute_relative_by_name(lxb_css_syntax_token_dimension(token)->str.data,
                                                          lxb_css_syntax_token_dimension(token)->str.length);
            if (unit == NULL) {
                return false;
            }

            length->num = lxb_css_syntax_token_dimension(token)->num.num;
            length->is_float = lxb_css_syntax_token_dimension(token)->num.is_float;
            length->unit = (lxb_css_unit_t) unit->unique;
            break;

        case LXB_CSS_SYNTAX_TOKEN_NUMBER:
            if (lxb_css_syntax_token_number(token)->num != 0) {
                return false;
            }

            length->num = lxb_css_syntax_token_number(token)->num;
            length->is_float = lxb_css_syntax_token_number(token)->is_float;
            length->unit = LXB_CSS_UNIT__UNDEF;
            break;

        default:
            return false;
    }

    lxb_css_syntax_parser_consume(parser);

    return true;
}

static bool
lxb_css_property_state_length_percentage(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         lxb_css_value_length_percentage_t *lp)
{
    const lxb_css_data_t *unit;

    switch (token->type) {
        case LXB_CSS_SYNTAX_TOKEN_DIMENSION:
            unit = lxb_css_unit_absolute_relative_by_name(lxb_css_syntax_token_dimension(token)->str.data,
                                                          lxb_css_syntax_token_dimension(token)->str.length);
            if (unit == NULL) {
                return false;
            }

            lp->type = LXB_CSS_VALUE__LENGTH;
            lp->u.length.num = lxb_css_syntax_token_dimension(token)->num.num;
            lp->u.length.is_float = lxb_css_syntax_token_dimension(token)->num.is_float;
            lp->u.length.unit = (lxb_css_unit_t) unit->unique;
            break;

        case LXB_CSS_SYNTAX_TOKEN_NUMBER:
            if (lxb_css_syntax_token_number(token)->num != 0) {
                return false;
            }

            lp->type = LXB_CSS_VALUE__NUMBER;
            lp->u.length.num = lxb_css_syntax_token_number(token)->num;
            lp->u.length.is_float = lxb_css_syntax_token_number(token)->is_float;
            lp->u.length.unit = LXB_CSS_UNIT__UNDEF;
            break;

        case LXB_CSS_SYNTAX_TOKEN_PERCENTAGE:
            lp->type = LXB_CSS_VALUE__PERCENTAGE;
            lp->u.percentage.num = lxb_css_syntax_token_percentage(token)->num;
            lp->u.percentage.is_float = lxb_css_syntax_token_percentage(token)->is_float;
            break;

        default:
            return false;
    }

    lxb_css_syntax_parser_consume(parser);

    return true;
}

static bool
lxb_css_property_state_number_length_percentage(lxb_css_parser_t *parser,
                                                const lxb_css_syntax_token_t *token,
                                                lxb_css_value_number_length_percentage_t *nlp)
{
    const lxb_css_data_t *unit;

    switch (token->type) {
        case LXB_CSS_SYNTAX_TOKEN_DIMENSION:
            unit = lxb_css_unit_absolute_relative_by_name(lxb_css_syntax_token_dimension(token)->str.data,
                                                          lxb_css_syntax_token_dimension(token)->str.length);
            if (unit == NULL) {
                return false;
            }

            nlp->type = LXB_CSS_VALUE__LENGTH;
            nlp->u.length.num = lxb_css_syntax_token_dimension(token)->num.num;
            nlp->u.length.is_float = lxb_css_syntax_token_dimension(token)->num.is_float;
            nlp->u.length.unit = (lxb_css_unit_t) unit->unique;
            break;

        case LXB_CSS_SYNTAX_TOKEN_NUMBER:
            nlp->type = LXB_CSS_VALUE__NUMBER;
            nlp->u.number.num = lxb_css_syntax_token_number(token)->num;
            nlp->u.number.is_float = lxb_css_syntax_token_number(token)->is_float;
            break;

        case LXB_CSS_SYNTAX_TOKEN_PERCENTAGE:
            nlp->type = LXB_CSS_VALUE__PERCENTAGE;
            nlp->u.percentage.num = lxb_css_syntax_token_percentage(token)->num;
            nlp->u.percentage.is_float = lxb_css_syntax_token_percentage(token)->is_float;
            break;

        default:
            return false;
    }

    lxb_css_syntax_parser_consume(parser);

    return true;
}

static bool
lxb_css_property_state_number_length(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token,
                                     lxb_css_value_number_length_t *nl)
{
    const lxb_css_data_t *unit;

    switch (token->type) {
        case LXB_CSS_SYNTAX_TOKEN_DIMENSION:
            unit = lxb_css_unit_absolute_relative_by_name(lxb_css_syntax_token_dimension(token)->str.data,
                                                          lxb_css_syntax_token_dimension(token)->str.length);
            if (unit == NULL) {
                return false;
            }

            nl->type = LXB_CSS_VALUE__LENGTH;
            nl->u.length.num = lxb_css_syntax_token_dimension(token)->num.num;
            nl->u.length.is_float = lxb_css_syntax_token_dimension(token)->num.is_float;
            nl->u.length.unit = (lxb_css_unit_t) unit->unique;
            break;

        case LXB_CSS_SYNTAX_TOKEN_NUMBER:
            nl->type = LXB_CSS_VALUE__NUMBER;
            nl->u.number.num = lxb_css_syntax_token_number(token)->num;
            nl->u.number.is_float = lxb_css_syntax_token_number(token)->is_float;
            break;

        default:
            return false;
    }

    lxb_css_syntax_parser_consume(parser);

    return true;
}

static bool
lxb_css_property_state_number(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token,
                              lxb_css_value_number_t *number)
{
    if (token->type != LXB_CSS_SYNTAX_TOKEN_NUMBER) {
        return false;
    }

    number->num = lxb_css_syntax_token_number(token)->num;
    number->is_float = lxb_css_syntax_token_number(token)->is_float;

    lxb_css_syntax_parser_consume(parser);

    return true;
}

static bool
lxb_css_property_state_integer(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token,
                               lxb_css_value_integer_t *intg)
{
    long ln;
    double num;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_NUMBER) {
        return false;
    }

    num = lxb_css_syntax_token_number(token)->num;
    ln = lexbor_conv_double_to_long(num);

    num = num - (double) ln;

    if (num < 0.0 || num > 0.0) {
        return false;
    }

    intg->num = ln;

    lxb_css_syntax_parser_consume(parser);

    return true;
}

static bool
lxb_css_property_state_percentage(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token,
                                  lxb_css_value_percentage_t *perc)
{
    if (token->type != LXB_CSS_SYNTAX_TOKEN_PERCENTAGE) {
        return false;
    }

    perc->num = lxb_css_syntax_token_percentage(token)->num;
    perc->is_float = lxb_css_syntax_token_percentage(token)->is_float;

    lxb_css_syntax_parser_consume(parser);

    return true;
}

static bool
lxb_css_property_state_number_percentage_none(lxb_css_parser_t *parser,
                                              const lxb_css_syntax_token_t *token,
                                              lxb_css_value_number_percentage_t *np)
{
    double num;
    lxb_css_value_type_t type;

    if (token->type == LXB_CSS_SYNTAX_TOKEN_NUMBER) {
        np->type = LXB_CSS_VALUE__NUMBER;
    }
    else if (token->type == LXB_CSS_SYNTAX_TOKEN_PERCENTAGE) {
        np->type = LXB_CSS_VALUE__PERCENTAGE;
    }
    else if (token->type == LXB_CSS_SYNTAX_TOKEN_IDENT) {
        type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                     lxb_css_syntax_token_ident(token)->length);
        if (type != LXB_CSS_VALUE_NONE) {
            return false;
        }

        np->type = LXB_CSS_VALUE_NONE;

        lxb_css_syntax_parser_consume(parser);

        return true;
    }
    else {
        return false;
    }

    num = lxb_css_syntax_token_number(token)->num;

    np->u.number.num = num;
    np->u.number.is_float = lxb_css_syntax_token_number(token)->is_float;

    lxb_css_syntax_parser_consume(parser);

    return true;
}

static bool
lxb_css_property_state_percentage_none(lxb_css_parser_t *parser,
                                       const lxb_css_syntax_token_t *token,
                                       lxb_css_value_percentage_type_t *np)
{
    double num;
    lxb_css_value_type_t type;

    if (token->type == LXB_CSS_SYNTAX_TOKEN_PERCENTAGE) {
        np->type = LXB_CSS_VALUE__PERCENTAGE;
    }
    else if (token->type == LXB_CSS_SYNTAX_TOKEN_IDENT) {
        type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                     lxb_css_syntax_token_ident(token)->length);
        if (type != LXB_CSS_VALUE_NONE) {
            return false;
        }

        np->type = LXB_CSS_VALUE_NONE;

        lxb_css_syntax_parser_consume(parser);

        return true;
    }
    else {
        return false;
    }

    num = lxb_css_syntax_token_number(token)->num;

    np->percentage.num = num;
    np->percentage.is_float = lxb_css_syntax_token_number(token)->is_float;

    lxb_css_syntax_parser_consume(parser);

    return true;
}

static bool
lxb_css_property_state_number_percentage(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         lxb_css_value_number_percentage_t *np)
{
    double num;

    if (token->type == LXB_CSS_SYNTAX_TOKEN_NUMBER) {
        np->type = LXB_CSS_VALUE__NUMBER;
    }
    else if (token->type == LXB_CSS_SYNTAX_TOKEN_PERCENTAGE) {
        np->type = LXB_CSS_VALUE__PERCENTAGE;
    }
    else {
        return false;
    }

    num = lxb_css_syntax_token_number(token)->num;

    np->u.number.num = num;
    np->u.number.is_float = lxb_css_syntax_token_number(token)->is_float;

    lxb_css_syntax_parser_consume(parser);

    return true;
}

static bool
lxb_css_property_state_angle(lxb_css_parser_t *parser,
                             const lxb_css_syntax_token_t *token,
                             lxb_css_value_angle_t *angle)
{
    const lxb_css_data_t *unit;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_DIMENSION) {
        return false;
    }

    unit = lxb_css_unit_angle_by_name(lxb_css_syntax_token_dimension(token)->str.data,
                                      lxb_css_syntax_token_dimension(token)->str.length);
    if (unit == NULL) {
        return false;
    }

    angle->num = lxb_css_syntax_token_dimension(token)->num.num;
    angle->is_float = lxb_css_syntax_token_dimension(token)->num.is_float;
    angle->unit = (lxb_css_unit_angle_t) unit->unique;

    lxb_css_syntax_parser_consume(parser);

    return true;
}

bool
lxb_css_property_state_width_handler(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token,
                                     lxb_css_property_width_t *width)
{
    lxb_css_value_type_t type;

    if (token->type == LXB_CSS_SYNTAX_TOKEN_IDENT) {
        type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                     lxb_css_syntax_token_ident(token)->length);
        switch (type) {
            case LXB_CSS_VALUE_AUTO:
            case LXB_CSS_VALUE_MIN_CONTENT:
            case LXB_CSS_VALUE_MAX_CONTENT:
                width->type = type;
                break;

            default:
                return false;
        }

        lxb_css_syntax_parser_consume(parser);

        return true;
    }

    return lxb_css_property_state_length_percentage(parser, token,
                                   (lxb_css_value_length_percentage_t *)width);
}

static bool
lxb_css_property_state_hue(lxb_css_parser_t *parser,
                           const lxb_css_syntax_token_t *token,
                           lxb_css_value_hue_t *hue)
{
    const lxb_css_data_t *unit;

    switch (token->type) {
        case LXB_CSS_SYNTAX_TOKEN_DIMENSION:
            unit = lxb_css_unit_angle_by_name(lxb_css_syntax_token_dimension(token)->str.data,
                                              lxb_css_syntax_token_dimension(token)->str.length);
            if (unit == NULL) {
                return false;
            }

            hue->type = LXB_CSS_VALUE__ANGLE;
            hue->u.angle.num = lxb_css_syntax_token_dimension(token)->num.num;
            hue->u.angle.is_float = lxb_css_syntax_token_dimension(token)->num.is_float;
            hue->u.angle.unit = (lxb_css_unit_angle_t) unit->unique;
            break;

        case LXB_CSS_SYNTAX_TOKEN_NUMBER:
            hue->type = LXB_CSS_VALUE__NUMBER;
            hue->u.number.num = lxb_css_syntax_token_number(token)->num;
            hue->u.number.is_float = lxb_css_syntax_token_number(token)->is_float;
            break;

        default:
            return false;
    }

    lxb_css_syntax_parser_consume(parser);

    return true;
}

lxb_inline bool
lxb_css_property_state_hue_none(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token,
                                lxb_css_value_hue_t *hue)
{
    lxb_css_value_type_t type;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_property_state_hue(parser, token, hue);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    if (type != LXB_CSS_VALUE_NONE) {
        return false;
    }

    hue->type = LXB_CSS_VALUE_NONE;

    lxb_css_syntax_parser_consume(parser);

    return true;
}

static bool
lxb_css_property_state_color_hex(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token,
                                 lxb_css_value_color_t *color)
{
    size_t length;
    uint32_t chex;
    lxb_char_t ch;
    const lxb_char_t *end, *p;
    lxb_css_value_color_hex_rgba_t *rgba;

    length = token->types.hash.length;

    if (length > 8) {
        return false;
    }

    p = token->types.hash.data;
    end = p + length;

    chex = 0;

    while (p < end) {
        ch = lexbor_str_res_map_lowercase[lexbor_str_res_map_hex[*p]];

        if (ch == 0xff) {
            return false;
        }

        chex = chex << 4 | ch;

        p++;
    }

    rgba = &color->u.hex.rgba;

    switch (length) {
        case 3:
            rgba->r = chex >> 8;
            rgba->g = chex >> 4 & ~LXB_CSS_PROPERTY_STATE_HEX_MASK(4);
            rgba->b = chex & ~LXB_CSS_PROPERTY_STATE_HEX_MASK(4);
            rgba->a = 0xff;

            color->u.hex.type = LXB_CSS_PROPERTY_COLOR_HEX_TYPE_3;
            break;

        case 4:
            rgba->r = chex >> 12;
            rgba->g = chex >> 8 & ~LXB_CSS_PROPERTY_STATE_HEX_MASK(4);
            rgba->b = chex >> 4 & ~LXB_CSS_PROPERTY_STATE_HEX_MASK(4);
            rgba->a = chex & ~LXB_CSS_PROPERTY_STATE_HEX_MASK(4);

            color->u.hex.type = LXB_CSS_PROPERTY_COLOR_HEX_TYPE_4;
            break;

        case 6:
            rgba->r = chex >> 16;
            rgba->g = chex >> 8 & ~LXB_CSS_PROPERTY_STATE_HEX_MASK(8);
            rgba->b = chex & ~LXB_CSS_PROPERTY_STATE_HEX_MASK(8);
            rgba->a = 0xff;

            color->u.hex.type = LXB_CSS_PROPERTY_COLOR_HEX_TYPE_6;
            break;

        case 8:
            rgba->r = chex >> 24;
            rgba->g = chex >> 16 & ~LXB_CSS_PROPERTY_STATE_HEX_MASK(8);
            rgba->b = chex >> 8 & ~LXB_CSS_PROPERTY_STATE_HEX_MASK(8);
            rgba->a = chex & ~LXB_CSS_PROPERTY_STATE_HEX_MASK(8);

            color->u.hex.type = LXB_CSS_PROPERTY_COLOR_HEX_TYPE_8;
            break;

        default:
            return false;
    }

    color->type = LXB_CSS_COLOR_HEX;

    lxb_css_syntax_parser_consume(parser);

    return true;
}

static bool
lxb_css_property_state_color_rgba(lxb_css_parser_t *parser,
                                  lxb_css_value_color_t *color)
{
    bool res;
    lxb_css_color_type_t type;
    lxb_css_value_color_rgba_t *rgb;
    const lxb_css_syntax_token_t *token;

    rgb = &color->u.rgb;

    lxb_css_syntax_parser_consume(parser);
    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    res = lxb_css_property_state_number_percentage_none(parser, token, &rgb->r);
    if (res == false) {
        return false;
    }

    type = rgb->r.type;

    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    if (token->type == LXB_CSS_SYNTAX_TOKEN_COMMA) {
        /* Deprecated format. */

        if (type == LXB_CSS_VALUE_NONE) {
            return false;
        }

        lxb_css_syntax_parser_consume(parser);
        token = lxb_css_syntax_parser_token_wo_ws(parser);
        lxb_css_property_state_check_token(parser, token);

        rgb->old = true;

        return lxb_css_property_state_color_rgba_old(parser, token, color);
    }

    res = lxb_css_property_state_number_percentage_none(parser, token, &rgb->g);
    if (res == false) {
        return false;
    }

    if (type != rgb->g.type) {
        if (type == LXB_CSS_VALUE_NONE) {
            type = rgb->g.type;
        }
        else if (rgb->g.type != LXB_CSS_VALUE_NONE) {
            return false;
        }
    }

    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    res = lxb_css_property_state_number_percentage_none(parser, token, &rgb->b);
    if (res == false) {
        return false;
    }

    if (type != rgb->b.type && type != LXB_CSS_VALUE_NONE
        && rgb->b.type != LXB_CSS_VALUE_NONE)
    {
            return false;
    }

    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    if (token->type == LXB_CSS_SYNTAX_TOKEN_DELIM) {
        if (lxb_css_syntax_token_delim(token)->character != '/') {
            return false;
        }

        lxb_css_syntax_parser_consume(parser);
        token = lxb_css_syntax_parser_token_wo_ws(parser);
        lxb_css_property_state_check_token(parser, token);
    }
    else if (token->type == LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS) {
        lxb_css_syntax_parser_consume(parser);
        return true;
    }
    else {
        return false;
    }

    res = lxb_css_property_state_number_percentage_none(parser, token, &rgb->a);
    if (res == false) {
        return false;
    }

    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    if (token->type != LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS) {
        return false;
    }

    lxb_css_syntax_parser_consume(parser);

    return true;
}

static bool
lxb_css_property_state_color_rgba_old(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token,
                                      lxb_css_value_color_t *color)
{
    bool res;
    lxb_css_value_color_rgba_t *rgb;

    rgb = &color->u.rgb;

    res = lxb_css_property_state_number_percentage(parser, token, &rgb->g);
    if (res == false) {
        return false;
    }

    if (rgb->r.type != rgb->g.type) {
        return false;
    }

    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    if (token->type != LXB_CSS_SYNTAX_TOKEN_COMMA) {
        return false;
    }

    lxb_css_syntax_parser_consume(parser);
    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    res = lxb_css_property_state_number_percentage(parser, token, &rgb->b);
    if (res == false) {
        return false;
    }

    if (rgb->r.type != rgb->b.type) {
        return false;
    }

    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    if (token->type == LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS) {
        lxb_css_syntax_parser_consume(parser);
        return true;
    }
    else if (token->type != LXB_CSS_SYNTAX_TOKEN_COMMA) {
        return false;
    }

    lxb_css_syntax_parser_consume(parser);
    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    res = lxb_css_property_state_number_percentage(parser, token, &rgb->a);
    if (res == false) {
        return false;
    }

    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    if (token->type != LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS) {
        return false;
    }

    lxb_css_syntax_parser_consume(parser);

    return true;
}

static bool
lxb_css_property_state_color_hsla(lxb_css_parser_t *parser,
                                  lxb_css_value_color_t *color)
{
    bool res;
    lxb_css_value_color_hsla_t *hsl;
    const lxb_css_syntax_token_t *token;

    lxb_css_syntax_parser_consume(parser);
    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    hsl = &color->u.hsl;

    res = lxb_css_property_state_hue_none(parser, token, &hsl->h);
    if (res == false) {
        return false;
    }

    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    if (token->type == LXB_CSS_SYNTAX_TOKEN_COMMA) {
        /* Deprecated format. */

        if (hsl->h.type == LXB_CSS_VALUE_NONE) {
            return false;
        }

        lxb_css_syntax_parser_consume(parser);
        token = lxb_css_syntax_parser_token_wo_ws(parser);
        lxb_css_property_state_check_token(parser, token);

        hsl->old = true;

        return lxb_css_property_state_color_hsla_old(parser, token, hsl);
    }

    res = lxb_css_property_state_percentage_none(parser, token, &hsl->s);
    if (res == false) {
        return false;
    }

    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    res = lxb_css_property_state_percentage_none(parser, token, &hsl->l);
    if (res == false) {
        return false;
    }

    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    if (token->type == LXB_CSS_SYNTAX_TOKEN_DELIM) {
        if (lxb_css_syntax_token_delim(token)->character != '/') {
            return false;
        }

        lxb_css_syntax_parser_consume(parser);
        token = lxb_css_syntax_parser_token_wo_ws(parser);
        lxb_css_property_state_check_token(parser, token);
    }
    else if (token->type == LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS) {
        lxb_css_syntax_parser_consume(parser);
        return true;
    }

    res = lxb_css_property_state_number_percentage_none(parser, token, &hsl->a);
    if (res == false) {
        return false;
    }

    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    if (token->type != LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS) {
        return false;
    }

    lxb_css_syntax_parser_consume(parser);

    return true;
}

static bool
lxb_css_property_state_color_hsla_old(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token,
                                      lxb_css_value_color_hsla_t *hsl)
{
    bool res;

    res = lxb_css_property_state_percentage(parser, token, &hsl->s.percentage);
    if (res == false) {
        return false;
    }

    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    if (token->type != LXB_CSS_SYNTAX_TOKEN_COMMA) {
        return false;
    }

    lxb_css_syntax_parser_consume(parser);
    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    res = lxb_css_property_state_percentage(parser, token, &hsl->l.percentage);
    if (res == false) {
        return false;
    }

    lxb_css_syntax_parser_consume(parser);
    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    if (token->type == LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS) {
        goto done;
    }
    else if (token->type != LXB_CSS_SYNTAX_TOKEN_COMMA) {
        return false;
    }

    lxb_css_syntax_parser_consume(parser);
    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    res = lxb_css_property_state_number_percentage(parser, token, &hsl->a);
    if (res == false) {
        return false;
    }

    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    if (token->type != LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS) {
        return false;
    }

done:

    lxb_css_syntax_parser_consume(parser);

    hsl->s.type = LXB_CSS_VALUE__PERCENTAGE;
    hsl->l.type = LXB_CSS_VALUE__PERCENTAGE;

    return true;
}

static bool
lxb_css_property_state_color_lab(lxb_css_parser_t *parser,
                                 lxb_css_value_color_t *color)
{
    bool res;
    lxb_css_value_color_lab_t *lab;
    const lxb_css_syntax_token_t *token;

    lab = &color->u.lab;

    lxb_css_syntax_parser_consume(parser);
    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    res = lxb_css_property_state_number_percentage_none(parser, token, &lab->l);
    if (res == false) {
        return false;
    }

    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    res = lxb_css_property_state_number_percentage_none(parser, token, &lab->a);
    if (res == false) {
        return false;
    }

    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    res = lxb_css_property_state_number_percentage_none(parser, token, &lab->b);
    if (res == false) {
        return false;
    }

    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    if (token->type == LXB_CSS_SYNTAX_TOKEN_DELIM) {
        if (lxb_css_syntax_token_delim(token)->character != '/') {
            return false;
        }

        lxb_css_syntax_parser_consume(parser);
        token = lxb_css_syntax_parser_token_wo_ws(parser);
        lxb_css_property_state_check_token(parser, token);
    }
    else if (token->type == LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS) {
        lxb_css_syntax_parser_consume(parser);
        return true;
    }
    else {
        return false;
    }

    res = lxb_css_property_state_number_percentage_none(parser, token,
                                                        &lab->alpha);
    if (res == false) {
        return false;
    }

    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    if (token->type != LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS) {
        return false;
    }

    lxb_css_syntax_parser_consume(parser);

    return true;
}

static bool
lxb_css_property_state_color_lch(lxb_css_parser_t *parser,
                                 lxb_css_value_color_t *color)
{
    bool res;
    lxb_css_value_color_lch_t *lch;
    const lxb_css_syntax_token_t *token;

    lch = &color->u.lch;

    lxb_css_syntax_parser_consume(parser);
    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    res = lxb_css_property_state_number_percentage_none(parser, token, &lch->l);
    if (res == false) {
        return false;
    }

    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    res = lxb_css_property_state_number_percentage_none(parser, token, &lch->c);
    if (res == false) {
        return false;
    }

    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    res = lxb_css_property_state_hue_none(parser, token, &lch->h);
    if (res == false) {
        return false;
    }

    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    if (token->type == LXB_CSS_SYNTAX_TOKEN_DELIM) {
        if (lxb_css_syntax_token_delim(token)->character != '/') {
            return false;
        }

        lxb_css_syntax_parser_consume(parser);
        token = lxb_css_syntax_parser_token_wo_ws(parser);
        lxb_css_property_state_check_token(parser, token);
    }
    else if (token->type == LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS) {
        lxb_css_syntax_parser_consume(parser);
        return true;
    }
    else {
        return false;
    }

    res = lxb_css_property_state_number_percentage_none(parser, token, &lch->a);
    if (res == false) {
        return false;
    }

    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    if (token->type != LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS) {
        return false;
    }

    lxb_css_syntax_parser_consume(parser);

    return true;
}

/*
 * Return:
 *     true  and status always LXB_STATUS_OK — token consumed, ok.
 *     false and status != LXB_STATUS_OK     — token consumed, not ok.
 *     false and status == LXB_STATUS_OK     — token not consumed, not ok.
 */
static bool
lxb_css_property_state_color_handler(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token,
                                     lxb_css_value_color_t *color,
                                     lxb_status_t *status)
{
    bool res;
    lxb_css_value_type_t type;

    *status = LXB_STATUS_OK;

    switch (token->type) {
        case LXB_CSS_SYNTAX_TOKEN_HASH:
            color->type = LXB_CSS_VALUE_HEX;

            return lxb_css_property_state_color_hex(parser, token, color);

        case LXB_CSS_SYNTAX_TOKEN_FUNCTION:
            type = lxb_css_value_by_name(lxb_css_syntax_token_function(token)->data,
                                         lxb_css_syntax_token_function(token)->length);
            color->type = type;

            switch (type) {
                /* <color> */
                case LXB_CSS_VALUE_RGB:
                case LXB_CSS_VALUE_RGBA:
                    res = lxb_css_property_state_color_rgba(parser, color);
                    break;

                case LXB_CSS_VALUE_HSL:
                case LXB_CSS_VALUE_HSLA:
                case LXB_CSS_VALUE_HWB:
                    res = lxb_css_property_state_color_hsla(parser, color);
                    break;

                case LXB_CSS_VALUE_LAB:
                case LXB_CSS_VALUE_OKLAB:
                    res = lxb_css_property_state_color_lab(parser, color);
                    break;

                case LXB_CSS_VALUE_LCH:
                case LXB_CSS_VALUE_OKLCH:
                    res = lxb_css_property_state_color_lch(parser, color);
                    break;

                case LXB_CSS_VALUE_COLOR:
                default:
                    *status = LXB_STATUS_OK;
                    return false;
            }

            if (!res) {
                *status = LXB_STATUS_ERROR_UNEXPECTED_DATA;
            }

            return res;

        case LXB_CSS_SYNTAX_TOKEN_IDENT:
            type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                         lxb_css_syntax_token_ident(token)->length);
            switch (type) {
                /* <color> */
                case LXB_CSS_VALUE_CURRENTCOLOR:
                /* <system-color> */
                case LXB_CSS_VALUE_CANVAS:
                case LXB_CSS_VALUE_CANVASTEXT:
                case LXB_CSS_VALUE_LINKTEXT:
                case LXB_CSS_VALUE_VISITEDTEXT:
                case LXB_CSS_VALUE_ACTIVETEXT:
                case LXB_CSS_VALUE_BUTTONFACE:
                case LXB_CSS_VALUE_BUTTONTEXT:
                case LXB_CSS_VALUE_BUTTONBORDER:
                case LXB_CSS_VALUE_FIELD:
                case LXB_CSS_VALUE_FIELDTEXT:
                case LXB_CSS_VALUE_HIGHLIGHT:
                case LXB_CSS_VALUE_HIGHLIGHTTEXT:
                case LXB_CSS_VALUE_SELECTEDITEM:
                case LXB_CSS_VALUE_SELECTEDITEMTEXT:
                case LXB_CSS_VALUE_MARK:
                case LXB_CSS_VALUE_MARKTEXT:
                case LXB_CSS_VALUE_GRAYTEXT:
                case LXB_CSS_VALUE_ACCENTCOLOR:
                case LXB_CSS_VALUE_ACCENTCOLORTEXT:
                /* <absolute-color-base> */
                case LXB_CSS_VALUE_TRANSPARENT:
                /* <named-color> */
                case LXB_CSS_VALUE_ALICEBLUE:
                case LXB_CSS_VALUE_ANTIQUEWHITE:
                case LXB_CSS_VALUE_AQUA:
                case LXB_CSS_VALUE_AQUAMARINE:
                case LXB_CSS_VALUE_AZURE:
                case LXB_CSS_VALUE_BEIGE:
                case LXB_CSS_VALUE_BISQUE:
                case LXB_CSS_VALUE_BLACK:
                case LXB_CSS_VALUE_BLANCHEDALMOND:
                case LXB_CSS_VALUE_BLUE:
                case LXB_CSS_VALUE_BLUEVIOLET:
                case LXB_CSS_VALUE_BROWN:
                case LXB_CSS_VALUE_BURLYWOOD:
                case LXB_CSS_VALUE_CADETBLUE:
                case LXB_CSS_VALUE_CHARTREUSE:
                case LXB_CSS_VALUE_CHOCOLATE:
                case LXB_CSS_VALUE_CORAL:
                case LXB_CSS_VALUE_CORNFLOWERBLUE:
                case LXB_CSS_VALUE_CORNSILK:
                case LXB_CSS_VALUE_CRIMSON:
                case LXB_CSS_VALUE_CYAN:
                case LXB_CSS_VALUE_DARKBLUE:
                case LXB_CSS_VALUE_DARKCYAN:
                case LXB_CSS_VALUE_DARKGOLDENROD:
                case LXB_CSS_VALUE_DARKGRAY:
                case LXB_CSS_VALUE_DARKGREEN:
                case LXB_CSS_VALUE_DARKGREY:
                case LXB_CSS_VALUE_DARKKHAKI:
                case LXB_CSS_VALUE_DARKMAGENTA:
                case LXB_CSS_VALUE_DARKOLIVEGREEN:
                case LXB_CSS_VALUE_DARKORANGE:
                case LXB_CSS_VALUE_DARKORCHID:
                case LXB_CSS_VALUE_DARKRED:
                case LXB_CSS_VALUE_DARKSALMON:
                case LXB_CSS_VALUE_DARKSEAGREEN:
                case LXB_CSS_VALUE_DARKSLATEBLUE:
                case LXB_CSS_VALUE_DARKSLATEGRAY:
                case LXB_CSS_VALUE_DARKSLATEGREY:
                case LXB_CSS_VALUE_DARKTURQUOISE:
                case LXB_CSS_VALUE_DARKVIOLET:
                case LXB_CSS_VALUE_DEEPPINK:
                case LXB_CSS_VALUE_DEEPSKYBLUE:
                case LXB_CSS_VALUE_DIMGRAY:
                case LXB_CSS_VALUE_DIMGREY:
                case LXB_CSS_VALUE_DODGERBLUE:
                case LXB_CSS_VALUE_FIREBRICK:
                case LXB_CSS_VALUE_FLORALWHITE:
                case LXB_CSS_VALUE_FORESTGREEN:
                case LXB_CSS_VALUE_FUCHSIA:
                case LXB_CSS_VALUE_GAINSBORO:
                case LXB_CSS_VALUE_GHOSTWHITE:
                case LXB_CSS_VALUE_GOLD:
                case LXB_CSS_VALUE_GOLDENROD:
                case LXB_CSS_VALUE_GRAY:
                case LXB_CSS_VALUE_GREEN:
                case LXB_CSS_VALUE_GREENYELLOW:
                case LXB_CSS_VALUE_GREY:
                case LXB_CSS_VALUE_HONEYDEW:
                case LXB_CSS_VALUE_HOTPINK:
                case LXB_CSS_VALUE_INDIANRED:
                case LXB_CSS_VALUE_INDIGO:
                case LXB_CSS_VALUE_IVORY:
                case LXB_CSS_VALUE_KHAKI:
                case LXB_CSS_VALUE_LAVENDER:
                case LXB_CSS_VALUE_LAVENDERBLUSH:
                case LXB_CSS_VALUE_LAWNGREEN:
                case LXB_CSS_VALUE_LEMONCHIFFON:
                case LXB_CSS_VALUE_LIGHTBLUE:
                case LXB_CSS_VALUE_LIGHTCORAL:
                case LXB_CSS_VALUE_LIGHTCYAN:
                case LXB_CSS_VALUE_LIGHTGOLDENRODYELLOW:
                case LXB_CSS_VALUE_LIGHTGRAY:
                case LXB_CSS_VALUE_LIGHTGREEN:
                case LXB_CSS_VALUE_LIGHTGREY:
                case LXB_CSS_VALUE_LIGHTPINK:
                case LXB_CSS_VALUE_LIGHTSALMON:
                case LXB_CSS_VALUE_LIGHTSEAGREEN:
                case LXB_CSS_VALUE_LIGHTSKYBLUE:
                case LXB_CSS_VALUE_LIGHTSLATEGRAY:
                case LXB_CSS_VALUE_LIGHTSLATEGREY:
                case LXB_CSS_VALUE_LIGHTSTEELBLUE:
                case LXB_CSS_VALUE_LIGHTYELLOW:
                case LXB_CSS_VALUE_LIME:
                case LXB_CSS_VALUE_LIMEGREEN:
                case LXB_CSS_VALUE_LINEN:
                case LXB_CSS_VALUE_MAGENTA:
                case LXB_CSS_VALUE_MAROON:
                case LXB_CSS_VALUE_MEDIUMAQUAMARINE:
                case LXB_CSS_VALUE_MEDIUMBLUE:
                case LXB_CSS_VALUE_MEDIUMORCHID:
                case LXB_CSS_VALUE_MEDIUMPURPLE:
                case LXB_CSS_VALUE_MEDIUMSEAGREEN:
                case LXB_CSS_VALUE_MEDIUMSLATEBLUE:
                case LXB_CSS_VALUE_MEDIUMSPRINGGREEN:
                case LXB_CSS_VALUE_MEDIUMTURQUOISE:
                case LXB_CSS_VALUE_MEDIUMVIOLETRED:
                case LXB_CSS_VALUE_MIDNIGHTBLUE:
                case LXB_CSS_VALUE_MINTCREAM:
                case LXB_CSS_VALUE_MISTYROSE:
                case LXB_CSS_VALUE_MOCCASIN:
                case LXB_CSS_VALUE_NAVAJOWHITE:
                case LXB_CSS_VALUE_NAVY:
                case LXB_CSS_VALUE_OLDLACE:
                case LXB_CSS_VALUE_OLIVE:
                case LXB_CSS_VALUE_OLIVEDRAB:
                case LXB_CSS_VALUE_ORANGE:
                case LXB_CSS_VALUE_ORANGERED:
                case LXB_CSS_VALUE_ORCHID:
                case LXB_CSS_VALUE_PALEGOLDENROD:
                case LXB_CSS_VALUE_PALEGREEN:
                case LXB_CSS_VALUE_PALETURQUOISE:
                case LXB_CSS_VALUE_PALEVIOLETRED:
                case LXB_CSS_VALUE_PAPAYAWHIP:
                case LXB_CSS_VALUE_PEACHPUFF:
                case LXB_CSS_VALUE_PERU:
                case LXB_CSS_VALUE_PINK:
                case LXB_CSS_VALUE_PLUM:
                case LXB_CSS_VALUE_POWDERBLUE:
                case LXB_CSS_VALUE_PURPLE:
                case LXB_CSS_VALUE_REBECCAPURPLE:
                case LXB_CSS_VALUE_RED:
                case LXB_CSS_VALUE_ROSYBROWN:
                case LXB_CSS_VALUE_ROYALBLUE:
                case LXB_CSS_VALUE_SADDLEBROWN:
                case LXB_CSS_VALUE_SALMON:
                case LXB_CSS_VALUE_SANDYBROWN:
                case LXB_CSS_VALUE_SEAGREEN:
                case LXB_CSS_VALUE_SEASHELL:
                case LXB_CSS_VALUE_SIENNA:
                case LXB_CSS_VALUE_SILVER:
                case LXB_CSS_VALUE_SKYBLUE:
                case LXB_CSS_VALUE_SLATEBLUE:
                case LXB_CSS_VALUE_SLATEGRAY:
                case LXB_CSS_VALUE_SLATEGREY:
                case LXB_CSS_VALUE_SNOW:
                case LXB_CSS_VALUE_SPRINGGREEN:
                case LXB_CSS_VALUE_STEELBLUE:
                case LXB_CSS_VALUE_TAN:
                case LXB_CSS_VALUE_TEAL:
                case LXB_CSS_VALUE_THISTLE:
                case LXB_CSS_VALUE_TOMATO:
                case LXB_CSS_VALUE_TURQUOISE:
                case LXB_CSS_VALUE_VIOLET:
                case LXB_CSS_VALUE_WHEAT:
                case LXB_CSS_VALUE_WHITE:
                case LXB_CSS_VALUE_WHITESMOKE:
                case LXB_CSS_VALUE_YELLOW:
                case LXB_CSS_VALUE_YELLOWGREEN:
                    color->type = type;
                    break;

                default:
                    return false;
            }

            break;

        default:
            return false;
    }

    lxb_css_syntax_parser_consume(parser);

    return true;
}

bool
lxb_css_property_state__undef(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_parser_failed(parser);
}

bool
lxb_css_property_state__custom(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_status_t status;
    lxb_css_rule_declaration_t *declar = ctx;
    lxb_css_property__custom_t *custom = declar->u.custom;

    (void) lexbor_str_init(&custom->value, parser->memory->mraw, 0);
    if (custom->value.data == NULL) {
        return lxb_css_parser_memory_fail(parser);
    }

    while (token != NULL && token->type != LXB_CSS_SYNTAX_TOKEN__END) {
        status = lxb_css_syntax_token_serialize_str(token, &custom->value,
                                                    parser->memory->mraw);
        if (status != LXB_STATUS_OK) {
            return lxb_css_parser_memory_fail(parser);
        }

        lxb_css_syntax_parser_consume(parser);
        token = lxb_css_syntax_parser_token(parser);
    }

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_display(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_property_display_t *display;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    display = declar->u.display;

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);

    switch (type) {
        /* <display-outside> */
        case LXB_CSS_DISPLAY_BLOCK:
        case LXB_CSS_DISPLAY_INLINE:
        case LXB_CSS_DISPLAY_RUN_IN:
            display->a = type;
            goto inside_listitem;

        /* <display-inside> */
        case LXB_CSS_DISPLAY_FLOW:
        case LXB_CSS_DISPLAY_FLOW_ROOT:
            display->a = type;
            goto outside_listitem;

        case LXB_CSS_DISPLAY_TABLE:
        case LXB_CSS_DISPLAY_FLEX:
        case LXB_CSS_DISPLAY_GRID:
        case LXB_CSS_DISPLAY_RUBY:
            display->a = type;
            goto outside;

        /* <display-internal> */
        case LXB_CSS_DISPLAY_LIST_ITEM:
            display->a = type;
            goto listitem_only;

        /* <display-internal> */
        case LXB_CSS_DISPLAY_TABLE_ROW_GROUP:
        case LXB_CSS_DISPLAY_TABLE_HEADER_GROUP:
        case LXB_CSS_DISPLAY_TABLE_FOOTER_GROUP:
        case LXB_CSS_DISPLAY_TABLE_ROW:
        case LXB_CSS_DISPLAY_TABLE_CELL:
        case LXB_CSS_DISPLAY_TABLE_COLUMN_GROUP:
        case LXB_CSS_DISPLAY_TABLE_COLUMN:
        case LXB_CSS_DISPLAY_TABLE_CAPTION:
        case LXB_CSS_DISPLAY_RUBY_BASE:
        case LXB_CSS_DISPLAY_RUBY_TEXT:
        case LXB_CSS_DISPLAY_RUBY_BASE_CONTAINER:
        case LXB_CSS_DISPLAY_RUBY_TEXT_CONTAINER:
        /* <display-box> */
        case LXB_CSS_DISPLAY_CONTENTS:
        case LXB_CSS_DISPLAY_NONE:
        /* <display-legacy> */
        case LXB_CSS_DISPLAY_INLINE_BLOCK:
        case LXB_CSS_DISPLAY_INLINE_TABLE:
        case LXB_CSS_DISPLAY_INLINE_FLEX:
        case LXB_CSS_DISPLAY_INLINE_GRID:
            display->a = type;
            goto done;

        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
            display->a = type;
            goto done;

        default:
            return lxb_css_parser_failed(parser);
    }

inside_listitem:

    lxb_css_property_state_get_type(parser, token, type);

    switch (type) {
        /* <display-inside> */
        case LXB_CSS_DISPLAY_FLOW:
        case LXB_CSS_DISPLAY_FLOW_ROOT:
            display->b = type;
            break;

        case LXB_CSS_DISPLAY_TABLE:
        case LXB_CSS_DISPLAY_FLEX:
        case LXB_CSS_DISPLAY_GRID:
        case LXB_CSS_DISPLAY_RUBY:
            display->b = type;
            goto done;

        case LXB_CSS_DISPLAY_LIST_ITEM:
            display->b = type;
            goto flow_only;

        default:
            return lxb_css_parser_failed(parser);
    }

listitem:

    lxb_css_property_state_get_type(parser, token, type);

    if (type == LXB_CSS_DISPLAY_LIST_ITEM) {
        display->c = type;
        goto done;
    }

    return lxb_css_parser_failed(parser);

outside:

    lxb_css_property_state_get_type(parser, token, type);

    switch (type) {
        /* <display-outside> */
        case LXB_CSS_DISPLAY_BLOCK:
        case LXB_CSS_DISPLAY_INLINE:
        case LXB_CSS_DISPLAY_RUN_IN:
            if (display->b == LXB_CSS_PROPERTY__UNDEF) {
                display->b = type;
            }
            else {
                display->c = type;
            }

            goto done;

        default:
            return lxb_css_parser_failed(parser);
    }

outside_listitem:

    lxb_css_property_state_get_type(parser, token, type);

    switch (type) {
        /* <display-outside> */
        case LXB_CSS_DISPLAY_BLOCK:
        case LXB_CSS_DISPLAY_INLINE:
        case LXB_CSS_DISPLAY_RUN_IN:
            display->b = type;
            goto listitem;

        case LXB_CSS_DISPLAY_LIST_ITEM:
            display->b = type;
            goto outside;

        default:
            return lxb_css_parser_failed(parser);
    }

listitem_only:

    lxb_css_property_state_get_type(parser, token, type);

    switch (type) {
        /* <display-outside> */
        case LXB_CSS_DISPLAY_BLOCK:
        case LXB_CSS_DISPLAY_INLINE:
        case LXB_CSS_DISPLAY_RUN_IN:
            display->b = type;
            break;

        /* <display-listitem> */
        case LXB_CSS_DISPLAY_FLOW:
        case LXB_CSS_DISPLAY_FLOW_ROOT:
            display->b = type;
            goto outside;

        default:
            return lxb_css_parser_failed(parser);
    }

flow_only:

    lxb_css_property_state_get_type(parser, token, type);

    switch (type) {
        /* <display-listitem> */
        case LXB_CSS_DISPLAY_FLOW:
        case LXB_CSS_DISPLAY_FLOW_ROOT:
            display->c = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

done:

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_order(lxb_css_parser_t *parser,
                             const lxb_css_syntax_token_t *token, void *ctx)
{
    bool res;
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    res = lxb_css_property_state_integer(parser, token,
                                         &declar->u.order->integer);
    if (res) {
        declar->u.order->type = LXB_CSS_ORDER__INTEGER;

        return lxb_css_parser_success(parser);
    }

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
            declar->u.order->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_visibility(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_VISIBILITY_VISIBLE:
        case LXB_CSS_VISIBILITY_HIDDEN:
        case LXB_CSS_VISIBILITY_COLLAPSE:
            declar->u.visibility->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_width(lxb_css_parser_t *parser,
                             const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type == LXB_CSS_SYNTAX_TOKEN_IDENT) {
        type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                     lxb_css_syntax_token_ident(token)->length);
        switch (type) {
            case LXB_CSS_VALUE_INITIAL:
            case LXB_CSS_VALUE_INHERIT:
            case LXB_CSS_VALUE_UNSET:
            case LXB_CSS_VALUE_REVERT:
            case LXB_CSS_VALUE_AUTO:
            case LXB_CSS_VALUE_MIN_CONTENT:
            case LXB_CSS_VALUE_MAX_CONTENT:
                declar->u.width->type = type;
                break;

            default:
                return lxb_css_parser_failed(parser);
        }

        lxb_css_syntax_parser_consume(parser);

        return lxb_css_parser_success(parser);
    }

    if (!lxb_css_property_state_length_percentage(parser, token,
                                                  declar->u.user))
    {
        return lxb_css_parser_failed(parser);
    }

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_height(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_width(parser, token, ctx);
}

bool
lxb_css_property_state_box_sizing(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        case LXB_CSS_VALUE_CONTENT_BOX:
        case LXB_CSS_VALUE_BORDER_BOX:
            declar->u.box_sizing->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_min_width(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_width(parser, token, ctx);
}

bool
lxb_css_property_state_min_height(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_width(parser, token, ctx);
}

bool
lxb_css_property_state_max_width(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type == LXB_CSS_SYNTAX_TOKEN_IDENT) {
        type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                     lxb_css_syntax_token_ident(token)->length);
        switch (type) {
            case LXB_CSS_VALUE_INITIAL:
            case LXB_CSS_VALUE_INHERIT:
            case LXB_CSS_VALUE_UNSET:
            case LXB_CSS_VALUE_REVERT:
            case LXB_CSS_VALUE_NONE:
            case LXB_CSS_VALUE_MIN_CONTENT:
            case LXB_CSS_VALUE_MAX_CONTENT:
                declar->u.width->type = type;
                break;

            default:
                return lxb_css_parser_failed(parser);
        }

        lxb_css_syntax_parser_consume(parser);

        return lxb_css_parser_success(parser);
    }

    if (!lxb_css_property_state_length_percentage(parser, token,
                                                  declar->u.user))
    {
        return lxb_css_parser_failed(parser);
    }

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_max_height(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_max_width(parser, token, ctx);
}

static bool
lxb_css_property_state_mp(lxb_css_parser_t *parser,
                          const lxb_css_syntax_token_t *token,
                          lxb_css_rule_declaration_t *declar, bool with_auto)
{
    unsigned int state;
    lxb_css_value_type_t type;
    lxb_css_property_margin_top_t *top;

    state = 1;

next:

    switch (state) {
        case 1:
            top = &declar->u.margin->top;
            break;

        case 2:
            top = &declar->u.margin->right;
            break;

        case 3:
            top = &declar->u.margin->bottom;
            break;

        case 4:
            top = &declar->u.margin->left;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    if (token->type == LXB_CSS_SYNTAX_TOKEN_IDENT) {
        type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                     lxb_css_syntax_token_ident(token)->length);
        switch (type) {
            case LXB_CSS_VALUE_INITIAL:
            case LXB_CSS_VALUE_INHERIT:
            case LXB_CSS_VALUE_UNSET:
            case LXB_CSS_VALUE_REVERT:
                top->type = type;
                break;

            case LXB_CSS_VALUE_AUTO:
                if (with_auto) {
                    top->type = type;
                    break;
                }

                /* Fall through. */

            default:
                return lxb_css_parser_failed(parser);
        }

        lxb_css_syntax_parser_consume(parser);
    }
    else if (!lxb_css_property_state_length_percentage(parser, token,
                                    (lxb_css_value_length_percentage_t *) top))
    {
        return lxb_css_parser_failed(parser);
    }

    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    if (token->type == LXB_CSS_SYNTAX_TOKEN__END) {
        return lxb_css_parser_success(parser);
    }

    state++;

    goto next;
}

static bool
lxb_css_property_state_mp_top(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token,
                              lxb_css_rule_declaration_t *declar, bool with_auto)
{
    lxb_css_value_type_t type;

    if (token->type == LXB_CSS_SYNTAX_TOKEN_IDENT) {
        type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                     lxb_css_syntax_token_ident(token)->length);
        switch (type) {
            case LXB_CSS_VALUE_INITIAL:
            case LXB_CSS_VALUE_INHERIT:
            case LXB_CSS_VALUE_UNSET:
            case LXB_CSS_VALUE_REVERT:
                declar->u.margin_top->type = type;
                break;

            case LXB_CSS_VALUE_AUTO:
                if (with_auto) {
                    declar->u.margin_top->type = type;
                    break;
                }

                /* Fall through. */

            default:
                return lxb_css_parser_failed(parser);
        }

        lxb_css_syntax_parser_consume(parser);

        return lxb_css_parser_success(parser);
    }

    if (!lxb_css_property_state_length_percentage(parser, token,
                                                  declar->u.user))
    {
        return lxb_css_parser_failed(parser);
    }

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_margin(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_mp(parser, token, ctx, true);
}

bool
lxb_css_property_state_margin_top(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_mp_top(parser, token, ctx, true);
}

bool
lxb_css_property_state_margin_right(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_mp_top(parser, token, ctx, true);
}

bool
lxb_css_property_state_margin_bottom(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_mp_top(parser, token, ctx, true);
}

bool
lxb_css_property_state_margin_left(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_mp_top(parser, token, ctx, true);
}

bool
lxb_css_property_state_padding(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_mp(parser, token, ctx, false);
}

bool
lxb_css_property_state_padding_top(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_mp_top(parser, token, ctx, false);
}

bool
lxb_css_property_state_padding_right(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_mp_top(parser, token, ctx, false);
}

bool
lxb_css_property_state_padding_bottom(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_mp_top(parser, token, ctx, false);
}

bool
lxb_css_property_state_padding_left(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_mp_top(parser, token, ctx, false);
}

static bool
lxb_css_property_state_line_width_style_color(lxb_css_parser_t *parser,
                                              const lxb_css_syntax_token_t *token,
                                              lxb_css_property_border_t *border)
{
    lxb_status_t status;
    lxb_css_value_type_t type;
    const lxb_css_data_t *unit;
    lxb_css_value_length_t *length;
    lxb_css_syntax_token_string_t *str;

    switch (token->type) {
        case LXB_CSS_SYNTAX_TOKEN_DIMENSION:
            if (border->width.type != LXB_CSS_VALUE__UNDEF) {
                return false;
            }

            str = &lxb_css_syntax_token_dimension(token)->str;

            unit = lxb_css_unit_absolute_relative_by_name(str->data,
                                                          str->length);
            if (unit == NULL) {
                return false;
            }

            length = &border->width.length;

            border->width.type = LXB_CSS_VALUE__LENGTH;
            length->num = lxb_css_syntax_token_dimension(token)->num.num;
            length->is_float = lxb_css_syntax_token_dimension(token)->num.is_float;
            length->unit = (lxb_css_unit_t) unit->unique;
            break;

        case LXB_CSS_SYNTAX_TOKEN_NUMBER:
            if (border->width.type != LXB_CSS_VALUE__UNDEF) {
                return false;
            }

            length = &border->width.length;

            border->width.type = LXB_CSS_VALUE__NUMBER;
            length->num = lxb_css_syntax_token_number(token)->num;
            length->is_float = lxb_css_syntax_token_number(token)->is_float;
            break;

        case LXB_CSS_SYNTAX_TOKEN_IDENT:
            type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                         lxb_css_syntax_token_ident(token)->length);
            switch (type) {
                case LXB_CSS_VALUE_THIN:
                case LXB_CSS_VALUE_MEDIUM:
                case LXB_CSS_VALUE_THICK:
                    if (border->width.type != LXB_CSS_VALUE__UNDEF) {
                        return false;
                    }

                    border->width.type = type;
                    break;

                case LXB_CSS_VALUE_NONE:
                case LXB_CSS_VALUE_HIDDEN:
                case LXB_CSS_VALUE_DOTTED:
                case LXB_CSS_VALUE_DASHED:
                case LXB_CSS_VALUE_SOLID:
                case LXB_CSS_VALUE_DOUBLE:
                case LXB_CSS_VALUE_GROOVE:
                case LXB_CSS_VALUE_RIDGE:
                case LXB_CSS_VALUE_INSET:
                case LXB_CSS_VALUE_OUTSET:
                    if (border->style != LXB_CSS_VALUE__UNDEF) {
                        return false;
                    }

                    border->style = type;
                    break;

                default:
                    goto color;
            }

            break;

        default:
            goto color;
    }

    lxb_css_syntax_parser_consume(parser);

    return true;

color:

    if (border->color.type != LXB_CSS_VALUE__UNDEF) {
        return false;
    }

    return lxb_css_property_state_color_handler(parser, token, &border->color,
                                                &status);
}

bool
lxb_css_property_state_border(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token, void *ctx)
{
    bool res;
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type == LXB_CSS_SYNTAX_TOKEN_IDENT) {
        type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                     lxb_css_syntax_token_ident(token)->length);
        switch (type) {
            case LXB_CSS_VALUE_INITIAL:
            case LXB_CSS_VALUE_INHERIT:
            case LXB_CSS_VALUE_UNSET:
            case LXB_CSS_VALUE_REVERT:
                declar->u.border->style = type;

                lxb_css_syntax_parser_consume(parser);
                return lxb_css_parser_success(parser);

            default:
                break;
        }
    }

    res = lxb_css_property_state_line_width_style_color(parser, token,
                                                        declar->u.border);
    if (!res) {
        return lxb_css_parser_failed(parser);
    }

    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    if (token->type == LXB_CSS_SYNTAX_TOKEN__END) {
        return lxb_css_parser_success(parser);
    }

    res = lxb_css_property_state_line_width_style_color(parser, token,
                                                        declar->u.border);
    if (!res) {
        return lxb_css_parser_failed(parser);
    }

    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    if (token->type == LXB_CSS_SYNTAX_TOKEN__END) {
        return lxb_css_parser_success(parser);
    }

    res = lxb_css_property_state_line_width_style_color(parser, token,
                                                        declar->u.border);
    if (!res) {
        return lxb_css_parser_failed(parser);
    }

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_border_top(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_border(parser, token, ctx);
}

bool
lxb_css_property_state_border_right(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_border(parser, token, ctx);
}

bool
lxb_css_property_state_border_bottom(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_border(parser, token, ctx);
}

bool
lxb_css_property_state_border_left(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_border(parser, token, ctx);
}

bool
lxb_css_property_state_border_top_color(lxb_css_parser_t *parser,
                                        const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_color(parser, token, ctx);
}

bool
lxb_css_property_state_border_right_color(lxb_css_parser_t *parser,
                                          const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_color(parser, token, ctx);
}

bool
lxb_css_property_state_border_bottom_color(lxb_css_parser_t *parser,
                                           const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_color(parser, token, ctx);
}

bool
lxb_css_property_state_border_left_color(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_color(parser, token, ctx);
}

bool
lxb_css_property_state_background_color(lxb_css_parser_t *parser,
                                        const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_color(parser, token, ctx);
}

bool
lxb_css_property_state_color(lxb_css_parser_t *parser,
                             const lxb_css_syntax_token_t *token, void *ctx)
{
    bool res;
    lxb_status_t status;
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type == LXB_CSS_SYNTAX_TOKEN_IDENT) {
        type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                     lxb_css_syntax_token_ident(token)->length);
        switch (type) {
            /* Global. */
            case LXB_CSS_VALUE_INITIAL:
            case LXB_CSS_VALUE_INHERIT:
            case LXB_CSS_VALUE_UNSET:
            case LXB_CSS_VALUE_REVERT:
                declar->u.color->type = type;

                lxb_css_syntax_parser_consume(parser);
                return lxb_css_parser_success(parser);

            default:
                break;
        }
    }

    res = lxb_css_property_state_color_handler(parser, token,
                                    (lxb_css_value_color_t *) declar->u.color,
                                    &status);
    if (!res) {
        return lxb_css_parser_failed(parser);
    }

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_opacity(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token, void *ctx)
{
    bool res;
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;
    lxb_css_property_opacity_t *opacity = declar->u.opacity;

    res = lxb_css_property_state_number_percentage(parser, token,
                                (lxb_css_value_number_percentage_t *) opacity);
    if (res) {
        return lxb_css_parser_success(parser);
    }

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
            opacity->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_position(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_POSITION_STATIC:
        case LXB_CSS_POSITION_RELATIVE:
        case LXB_CSS_POSITION_ABSOLUTE:
        case LXB_CSS_POSITION_STICKY:
        case LXB_CSS_POSITION_FIXED:
            declar->u.position->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_top(lxb_css_parser_t *parser,
                           const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_mp_top(parser, token, ctx, true);
}

bool
lxb_css_property_state_right(lxb_css_parser_t *parser,
                             const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_top(parser, token, ctx);
}

bool
lxb_css_property_state_bottom(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_top(parser, token, ctx);
}

bool
lxb_css_property_state_left(lxb_css_parser_t *parser,
                            const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_top(parser, token, ctx);
}

bool
lxb_css_property_state_inset_block_start(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_top(parser, token, ctx);
}

bool
lxb_css_property_state_inset_inline_start(lxb_css_parser_t *parser,
                                          const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_top(parser, token, ctx);
}

bool
lxb_css_property_state_inset_block_end(lxb_css_parser_t *parser,
                                       const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_top(parser, token, ctx);
}

bool
lxb_css_property_state_inset_inline_end(lxb_css_parser_t *parser,
                                        const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_top(parser, token, ctx);
}

bool
lxb_css_property_state_text_transform(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;
    lxb_css_property_text_transform_t *tt;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    tt = declar->u.text_transform;

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_TEXT_TRANSFORM_NONE:
            tt->type_case = type;
            break;

        case LXB_CSS_TEXT_TRANSFORM_CAPITALIZE:
        case LXB_CSS_TEXT_TRANSFORM_UPPERCASE:
        case LXB_CSS_TEXT_TRANSFORM_LOWERCASE:
            tt->type_case = type;
            goto next;

        case LXB_CSS_TEXT_TRANSFORM_FULL_WIDTH:
            tt->full_width = type;
            goto next;

        case LXB_CSS_TEXT_TRANSFORM_FULL_SIZE_KANA:
            tt->full_size_kana = type;
            goto next;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);

next:

    lxb_css_syntax_parser_consume(parser);
    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_success(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);

    switch (type) {
        case LXB_CSS_TEXT_TRANSFORM_CAPITALIZE:
        case LXB_CSS_TEXT_TRANSFORM_UPPERCASE:
        case LXB_CSS_TEXT_TRANSFORM_LOWERCASE:
            if (tt->type_case != LXB_CSS_VALUE__UNDEF) {
                return lxb_css_parser_failed(parser);
            }

            tt->type_case = type;
            goto next;

        case LXB_CSS_TEXT_TRANSFORM_FULL_WIDTH:
            if (tt->full_width != LXB_CSS_VALUE__UNDEF) {
                return lxb_css_parser_failed(parser);
            }

            tt->full_width = type;
            goto next;

        case LXB_CSS_TEXT_TRANSFORM_FULL_SIZE_KANA:
            if (tt->full_size_kana != LXB_CSS_VALUE__UNDEF) {
                return lxb_css_parser_failed(parser);
            }

            tt->full_size_kana = type;
            goto next;

        default:
            return lxb_css_parser_failed(parser);
    }
}

bool
lxb_css_property_state_text_align(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_TEXT_ALIGN_START:
        case LXB_CSS_TEXT_ALIGN_END:
        case LXB_CSS_TEXT_ALIGN_LEFT:
        case LXB_CSS_TEXT_ALIGN_RIGHT:
        case LXB_CSS_TEXT_ALIGN_CENTER:
        case LXB_CSS_TEXT_ALIGN_JUSTIFY:
        case LXB_CSS_TEXT_ALIGN_MATCH_PARENT:
        case LXB_CSS_TEXT_ALIGN_JUSTIFY_ALL:
            declar->u.text_align->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_text_align_all(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_TEXT_ALIGN_ALL_START:
        case LXB_CSS_TEXT_ALIGN_ALL_END:
        case LXB_CSS_TEXT_ALIGN_ALL_LEFT:
        case LXB_CSS_TEXT_ALIGN_ALL_RIGHT:
        case LXB_CSS_TEXT_ALIGN_ALL_CENTER:
        case LXB_CSS_TEXT_ALIGN_ALL_JUSTIFY:
        case LXB_CSS_TEXT_ALIGN_ALL_MATCH_PARENT:
            declar->u.text_align_all->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_text_align_last(lxb_css_parser_t *parser,
                                       const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_TEXT_ALIGN_LAST_AUTO:
        case LXB_CSS_TEXT_ALIGN_LAST_START:
        case LXB_CSS_TEXT_ALIGN_LAST_END:
        case LXB_CSS_TEXT_ALIGN_LAST_LEFT:
        case LXB_CSS_TEXT_ALIGN_LAST_RIGHT:
        case LXB_CSS_TEXT_ALIGN_LAST_CENTER:
        case LXB_CSS_TEXT_ALIGN_LAST_JUSTIFY:
        case LXB_CSS_TEXT_ALIGN_LAST_MATCH_PARENT:
            declar->u.text_align_last->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_text_justify(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_TEXT_JUSTIFY_AUTO:
        case LXB_CSS_TEXT_JUSTIFY_NONE:
        case LXB_CSS_TEXT_JUSTIFY_INTER_WORD:
        case LXB_CSS_TEXT_JUSTIFY_INTER_CHARACTER:
            declar->u.text_justify->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_text_indent(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token, void *ctx)
{
    bool res;
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;
    lxb_css_property_text_indent_t *text_indent;

    text_indent = declar->u.text_indent;

    res = lxb_css_property_state_length_percentage(parser, token,
                                                   &text_indent->length);
    if (res) {
        token = lxb_css_syntax_parser_token_wo_ws(parser);
        lxb_css_property_state_check_token(parser, token);

        text_indent->type = LXB_CSS_VALUE__LENGTH;
    }

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        if (!res) {
            return lxb_css_parser_failed(parser);
        }

        return lxb_css_parser_success(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
            if (res) {
                return lxb_css_parser_failed(parser);
            }

            text_indent->type = type;
            break;

        /* Local. */
        case LXB_CSS_TEXT_INDENT_HANGING:
            text_indent->hanging = type;
            goto next;

        case LXB_CSS_TEXT_INDENT_EACH_LINE:
            text_indent->each_line = type;
            goto next;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);

next:

    lxb_css_syntax_parser_consume(parser);
    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    res = lxb_css_property_state_length_percentage(parser, token,
                                                   &text_indent->length);
    if (res) {
        if (text_indent->type != LXB_CSS_VALUE__UNDEF) {
            return lxb_css_parser_failed(parser);
        }

        token = lxb_css_syntax_parser_token_wo_ws(parser);
        lxb_css_property_state_check_token(parser, token);

        text_indent->type = LXB_CSS_VALUE__LENGTH;
    }

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        if (text_indent->type == LXB_CSS_VALUE__UNDEF) {
            return lxb_css_parser_failed(parser);
        }

        return lxb_css_parser_success(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);

    switch (type) {
        case LXB_CSS_TEXT_INDENT_HANGING:
            if (text_indent->hanging != LXB_CSS_VALUE__UNDEF) {
                return lxb_css_parser_failed(parser);
            }

            text_indent->hanging = type;
            goto next;

        case LXB_CSS_TEXT_INDENT_EACH_LINE:
            if (text_indent->each_line != LXB_CSS_VALUE__UNDEF) {
                return lxb_css_parser_failed(parser);
            }

            text_indent->each_line = type;
            goto next;

        default:
            return lxb_css_parser_failed(parser);
    }
}

bool
lxb_css_property_state_white_space(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_WHITE_SPACE_NORMAL:
        case LXB_CSS_WHITE_SPACE_PRE:
        case LXB_CSS_WHITE_SPACE_NOWRAP:
        case LXB_CSS_WHITE_SPACE_PRE_WRAP:
        case LXB_CSS_WHITE_SPACE_BREAK_SPACES:
        case LXB_CSS_WHITE_SPACE_PRE_LINE:
            declar->u.white_space->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_tab_size(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token, void *ctx)
{
    bool res;
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    res = lxb_css_property_state_number_length(parser, token,
                         (lxb_css_value_number_length_t *) declar->u.tab_size);
    if (res) {
        return lxb_css_parser_success(parser);
    }

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
            declar->u.tab_size->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_word_break(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_WORD_BREAK_NORMAL:
        case LXB_CSS_WORD_BREAK_KEEP_ALL:
        case LXB_CSS_WORD_BREAK_BREAK_ALL:
        case LXB_CSS_WORD_BREAK_BREAK_WORD:
            declar->u.word_break->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_line_break(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_LINE_BREAK_AUTO:
        case LXB_CSS_LINE_BREAK_LOOSE:
        case LXB_CSS_LINE_BREAK_NORMAL:
        case LXB_CSS_LINE_BREAK_STRICT:
        case LXB_CSS_LINE_BREAK_ANYWHERE:
            declar->u.line_break->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_hyphens(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_HYPHENS_NONE:
        case LXB_CSS_HYPHENS_MANUAL:
        case LXB_CSS_HYPHENS_AUTO:
            declar->u.hyphens->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_overflow_wrap(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_OVERFLOW_WRAP_NORMAL:
        case LXB_CSS_OVERFLOW_WRAP_BREAK_WORD:
        case LXB_CSS_OVERFLOW_WRAP_ANYWHERE:
            declar->u.overflow_wrap->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_word_wrap(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_overflow_wrap(parser, token, ctx);
}

bool
lxb_css_property_state_word_spacing(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token, void *ctx)
{
    bool res;
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    res = lxb_css_property_state_length(parser, token,
                                        &declar->u.word_spacing->length);
    if (res) {
        declar->u.word_spacing->type = LXB_CSS_VALUE__LENGTH;

        return lxb_css_parser_success(parser);
    }

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_WORD_SPACING_NORMAL:
            declar->u.word_spacing->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_letter_spacing(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_word_spacing(parser, token, ctx);
}

bool
lxb_css_property_state_hanging_punctuation(lxb_css_parser_t *parser,
                                           const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;
    lxb_css_property_hanging_punctuation_t *hp;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    hp = declar->u.hanging_punctuation;

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
            /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
            /* Local. */
        case LXB_CSS_HANGING_PUNCTUATION_NONE:
            hp->type_first = type;
            break;

        case LXB_CSS_HANGING_PUNCTUATION_FIRST:
            hp->type_first = type;
            goto next;

        case LXB_CSS_HANGING_PUNCTUATION_FORCE_END:
        case LXB_CSS_HANGING_PUNCTUATION_ALLOW_END:
            hp->force_allow = type;
            goto next;

        case LXB_CSS_HANGING_PUNCTUATION_LAST:
            hp->last = type;
            goto next;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);

next:

    lxb_css_syntax_parser_consume(parser);
    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_success(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);

    switch (type) {
        case LXB_CSS_HANGING_PUNCTUATION_FIRST:
            if (hp->type_first != LXB_CSS_VALUE__UNDEF) {
                return lxb_css_parser_failed(parser);
            }

            hp->type_first = type;
            goto next;

        case LXB_CSS_HANGING_PUNCTUATION_FORCE_END:
        case LXB_CSS_HANGING_PUNCTUATION_ALLOW_END:
            if (hp->force_allow != LXB_CSS_VALUE__UNDEF) {
                return lxb_css_parser_failed(parser);
            }

            hp->force_allow = type;
            goto next;

        case LXB_CSS_HANGING_PUNCTUATION_LAST:
            if (hp->last != LXB_CSS_VALUE__UNDEF) {
                return lxb_css_parser_failed(parser);
            }

            hp->last = type;
            goto next;

        default:
            return lxb_css_parser_failed(parser);
    }
}

bool
lxb_css_property_state_font_family(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token, void *ctx)
{
    size_t length;
    const lxb_char_t *data;
    lexbor_str_t *str;
    lexbor_mraw_t *mraw;
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;
    lxb_css_property_font_family_t *ff;
    lxb_css_property_family_name_t *name;

    mraw = parser->memory->mraw;
    ff = declar->u.font_family;

    while (token != NULL) {
        name = lexbor_mraw_alloc(mraw, sizeof(lxb_css_property_family_name_t));
        if (name == NULL) {
            return lxb_css_parser_memory_fail(parser);
        }

        if (token->type == LXB_CSS_SYNTAX_TOKEN_IDENT) {
            data = lxb_css_syntax_token_ident(token)->data;
            length = lxb_css_syntax_token_ident(token)->length;

            type = lxb_css_value_by_name(data, length);
            if (type != LXB_CSS_VALUE__UNDEF) {
                name->generic = true;
                name->u.type = type;

                goto next;
            }
        }
        else if (token->type == LXB_CSS_SYNTAX_TOKEN_STRING) {
            data = lxb_css_syntax_token_string(token)->data;
            length = lxb_css_syntax_token_string(token)->length;
        }
        else {
            return lxb_css_parser_failed(parser);
        }

        name->generic = false;

        str = &name->u.str;

        (void) lexbor_str_init(str, mraw, length);
        if (name->u.str.data == NULL) {
            return lxb_css_parser_memory_fail(parser);
        }

        memcpy(str->data, data, length);

        str->data[length] = '\0';
        str->length = length;

    next:

        if (ff->first == NULL) {
            ff->first = name;
        }
        else {
            ff->last->next = name;
        }

        name->next = NULL;
        name->prev = ff->last;
        ff->last = name;

        lxb_css_syntax_parser_consume(parser);
        token = lxb_css_syntax_parser_token_wo_ws(parser);
        lxb_css_property_state_check_token(parser, token);

        if (token->type != LXB_CSS_SYNTAX_TOKEN_COMMA) {
            if (token->type == LXB_CSS_SYNTAX_TOKEN__END) {
                return lxb_css_parser_success(parser);
            }

            return lxb_css_parser_failed(parser);
        }

        lxb_css_syntax_parser_consume(parser);
        token = lxb_css_syntax_parser_token_wo_ws(parser);
    }

    lxb_css_property_state_check_token(parser, token);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_font_weight(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token, void *ctx)\
{
    bool res;
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;
    lxb_css_property_font_weight_t *fw = declar->u.font_weight;

    res = lxb_css_property_state_number(parser, token, &fw->number);

    if (res) {
        if (fw->number.num < 1 || fw->number.num > 1000) {
            return lxb_css_parser_failed(parser);
        }

        fw->type = LXB_CSS_FONT_WEIGHT__NUMBER;
        return lxb_css_parser_success(parser);
    }

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_FONT_WEIGHT_NORMAL:
        case LXB_CSS_FONT_WEIGHT_BOLD:
        case LXB_CSS_FONT_WEIGHT_BOLDER:
        case LXB_CSS_FONT_WEIGHT_LIGHTER:
            fw->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_font_stretch(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token, void *ctx)
{
    bool res;
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;
    lxb_css_property_font_stretch_t *fs = declar->u.font_stretch;

    res = lxb_css_property_state_percentage(parser, token, &fs->percentage);

    if (res) {
        if (fs->percentage.num < 0) {
            return lxb_css_parser_failed(parser);
        }

        fs->type = LXB_CSS_FONT_STRETCH__PERCENTAGE;
        return lxb_css_parser_success(parser);
    }

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_FONT_STRETCH_NORMAL:
        case LXB_CSS_FONT_STRETCH_ULTRA_CONDENSED:
        case LXB_CSS_FONT_STRETCH_EXTRA_CONDENSED:
        case LXB_CSS_FONT_STRETCH_CONDENSED:
        case LXB_CSS_FONT_STRETCH_SEMI_CONDENSED:
        case LXB_CSS_FONT_STRETCH_SEMI_EXPANDED:
        case LXB_CSS_FONT_STRETCH_EXPANDED:
        case LXB_CSS_FONT_STRETCH_EXTRA_EXPANDED:
        case LXB_CSS_FONT_STRETCH_ULTRA_EXPANDED:
            fs->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_font_style(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token, void *ctx)
{
    bool res;
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;
    lxb_css_property_font_style_t *fs = declar->u.font_style;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
            /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
            /* Local. */
        case LXB_CSS_FONT_STYLE_NORMAL:
        case LXB_CSS_FONT_STYLE_ITALIC:
            fs->type = type;
            break;

        case LXB_CSS_FONT_STYLE_OBLIQUE:
            fs->type = type;

            lxb_css_syntax_parser_consume(parser);
            token = lxb_css_syntax_parser_token_wo_ws(parser);
            lxb_css_property_state_check_token(parser, token);

            res = lxb_css_property_state_angle(parser, token, &fs->angle);

            if (res) {
                if (fs->angle.num < -90 || fs->angle.num > 90) {
                    return lxb_css_parser_failed(parser);
                }

                return lxb_css_parser_success(parser);
            }
            else {
                fs->angle.unit = (lxb_css_unit_angle_t) LXB_CSS_UNIT__UNDEF;
            }

            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_font_size(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token, void *ctx)
{
    bool res;
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;
    lxb_css_property_font_size_t *fs = declar->u.font_size;

    res = lxb_css_property_state_length_percentage(parser, token, &fs->length);

    if (res) {
        if (fs->length.u.length.num < 0) {
            return lxb_css_parser_failed(parser);
        }

        fs->type = LXB_CSS_FONT_SIZE__LENGTH;

        return lxb_css_parser_success(parser);
    }

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_FONT_SIZE_XX_SMALL:
        case LXB_CSS_FONT_SIZE_X_SMALL:
        case LXB_CSS_FONT_SIZE_SMALL:
        case LXB_CSS_FONT_SIZE_MEDIUM:
        case LXB_CSS_FONT_SIZE_LARGE:
        case LXB_CSS_FONT_SIZE_X_LARGE:
        case LXB_CSS_FONT_SIZE_XX_LARGE:
        case LXB_CSS_FONT_SIZE_XXX_LARGE:
        case LXB_CSS_FONT_SIZE_MATH:
        case LXB_CSS_FONT_SIZE_LARGER:
        case LXB_CSS_FONT_SIZE_SMALLER:
            fs->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_float_reference(lxb_css_parser_t *parser,
                                       const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_FLOAT_REFERENCE_INLINE:
        case LXB_CSS_FLOAT_REFERENCE_COLUMN:
        case LXB_CSS_FLOAT_REFERENCE_REGION:
        case LXB_CSS_FLOAT_REFERENCE_PAGE:
            declar->u.float_reference->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_float(lxb_css_parser_t *parser,
                             const lxb_css_syntax_token_t *token, void *ctx)
{
    bool res;
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;
    lxb_css_property_float_t *fp = declar->u.floatp;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        if (token->type == LXB_CSS_SYNTAX_TOKEN_FUNCTION) {
            goto snap;
        }

        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_FLOAT_BLOCK_START:
        case LXB_CSS_FLOAT_BLOCK_END:
        case LXB_CSS_FLOAT_INLINE_START:
        case LXB_CSS_FLOAT_INLINE_END:
        case LXB_CSS_FLOAT_SNAP_BLOCK:
        case LXB_CSS_FLOAT_SNAP_INLINE:
        case LXB_CSS_FLOAT_LEFT:
        case LXB_CSS_FLOAT_RIGHT:
        case LXB_CSS_FLOAT_TOP:
        case LXB_CSS_FLOAT_BOTTOM:
        case LXB_CSS_FLOAT_NONE:
            fp->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);

snap:

    type = lxb_css_value_by_name(lxb_css_syntax_token_function(token)->data,
                                 lxb_css_syntax_token_function(token)->length);

    if (type != LXB_CSS_FLOAT_SNAP_BLOCK
        && type != LXB_CSS_FLOAT_SNAP_INLINE)
    {
        return lxb_css_parser_failed(parser);
    }

    fp->type = type;

    lxb_css_syntax_parser_consume(parser);
    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    res = lxb_css_property_state_length(parser, token, &fp->length.length);
    if (!res) {
        return lxb_css_parser_failed(parser);
    }

    fp->length.type = LXB_CSS_VALUE__LENGTH;

    lxb_css_syntax_parser_consume(parser);
    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    if (token->type != LXB_CSS_SYNTAX_TOKEN_COMMA) {
        if (token->type == LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS) {
            fp->snap_type = LXB_CSS_VALUE__UNDEF;

            lxb_css_syntax_parser_consume(parser);
            return lxb_css_parser_success(parser);
        }

        return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);
    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        case LXB_CSS_FLOAT_START:
        case LXB_CSS_FLOAT_END:
            if (fp->type != LXB_CSS_FLOAT_SNAP_BLOCK) {
                return lxb_css_parser_failed(parser);
            }

            fp->snap_type = type;
            break;

        case LXB_CSS_FLOAT_LEFT:
        case LXB_CSS_FLOAT_RIGHT:
            if (fp->type != LXB_CSS_FLOAT_SNAP_INLINE) {
                return lxb_css_parser_failed(parser);
            }

            fp->snap_type = type;
            break;

        case LXB_CSS_FLOAT_NEAR:
            fp->snap_type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);
    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    if (token->type == LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS) {
        lxb_css_syntax_parser_consume(parser);

        return lxb_css_parser_success(parser);
    }

    return lxb_css_parser_failed(parser);
}

bool
lxb_css_property_state_clear(lxb_css_parser_t *parser,
                             const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_CLEAR_INLINE_START:
        case LXB_CSS_CLEAR_INLINE_END:
        case LXB_CSS_CLEAR_BLOCK_START:
        case LXB_CSS_CLEAR_BLOCK_END:
        case LXB_CSS_CLEAR_LEFT:
        case LXB_CSS_CLEAR_RIGHT:
        case LXB_CSS_CLEAR_TOP:
        case LXB_CSS_CLEAR_BOTTOM:
        case LXB_CSS_CLEAR_NONE:
            declar->u.clear->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_float_defer(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token, void *ctx)
{
    bool res;
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;
    lxb_css_property_float_defer_t *fd = declar->u.float_defer;

    res = lxb_css_property_state_integer(parser, token, &fd->integer);
    if (res) {
        fd->type = LXB_CSS_FLOAT_DEFER__INTEGER;

        return lxb_css_parser_success(parser);
    }

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_FLOAT_DEFER_LAST:
        case LXB_CSS_FLOAT_DEFER_NONE:
            fd->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_float_offset(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token, void *ctx)
{
    bool res;
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;
    lxb_css_property_float_offset_t *fo = declar->u.float_offset;

    res = lxb_css_property_state_length_percentage(parser, token,
                                     (lxb_css_value_length_percentage_t *) fo);
    if (res) {
        return lxb_css_parser_success(parser);
    }

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
            fo->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_wrap_flow(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_WRAP_FLOW_AUTO:
        case LXB_CSS_WRAP_FLOW_BOTH:
        case LXB_CSS_WRAP_FLOW_START:
        case LXB_CSS_WRAP_FLOW_END:
        case LXB_CSS_WRAP_FLOW_MINIMUM:
        case LXB_CSS_WRAP_FLOW_MAXIMUM:
        case LXB_CSS_WRAP_FLOW_CLEAR:
            declar->u.wrap_flow->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_wrap_through(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_WRAP_THROUGH_WRAP:
        case LXB_CSS_WRAP_THROUGH_NONE:
            declar->u.wrap_through->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_flex_direction(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_FLEX_DIRECTION_ROW:
        case LXB_CSS_FLEX_DIRECTION_ROW_REVERSE:
        case LXB_CSS_FLEX_DIRECTION_COLUMN:
        case LXB_CSS_FLEX_DIRECTION_COLUMN_REVERSE:
            declar->u.flex_direction->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_flex_wrap(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
            /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
            /* Local. */
        case LXB_CSS_FLEX_WRAP_NOWRAP:
        case LXB_CSS_FLEX_WRAP_WRAP:
        case LXB_CSS_FLEX_WRAP_WRAP_REVERSE:
            declar->u.flex_wrap->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_flex_flow(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;
    lxb_css_property_flex_flow_t *ff = declar->u.flex_flow;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_FLEX_DIRECTION_ROW:
        case LXB_CSS_FLEX_DIRECTION_ROW_REVERSE:
        case LXB_CSS_FLEX_DIRECTION_COLUMN:
        case LXB_CSS_FLEX_DIRECTION_COLUMN_REVERSE:
            ff->type_direction = type;
            goto direction;

        case LXB_CSS_FLEX_WRAP_NOWRAP:
        case LXB_CSS_FLEX_WRAP_WRAP:
        case LXB_CSS_FLEX_WRAP_WRAP_REVERSE:
            ff->wrap = type;
            goto wrap;

        default:
            return lxb_css_parser_failed(parser);
    }

direction:

    lxb_css_syntax_parser_consume(parser);
    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_success(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        case LXB_CSS_FLEX_WRAP_NOWRAP:
        case LXB_CSS_FLEX_WRAP_WRAP:
        case LXB_CSS_FLEX_WRAP_WRAP_REVERSE:
            ff->wrap = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    goto done;

wrap:

    lxb_css_syntax_parser_consume(parser);
    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_success(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        case LXB_CSS_FLEX_DIRECTION_ROW:
        case LXB_CSS_FLEX_DIRECTION_ROW_REVERSE:
        case LXB_CSS_FLEX_DIRECTION_COLUMN:
        case LXB_CSS_FLEX_DIRECTION_COLUMN_REVERSE:
            ff->type_direction = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

done:

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

lxb_inline bool
lxb_css_property_state_flex_grow_shrink(lxb_css_parser_t *parser,
                                        const lxb_css_syntax_token_t *token,
                                        lxb_css_property_flex_t *flex)
{
    bool res;

    res = lxb_css_property_state_number(parser, token, &flex->grow.number);
    if (!res) {
        return false;
    }

    flex->grow.type = LXB_CSS_FLEX_GROW__NUMBER;

    lxb_css_syntax_parser_consume(parser);
    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    res = lxb_css_property_state_number(parser, token, &flex->shrink.number);
    if (res) {
        flex->shrink.type = LXB_CSS_FLEX_SHRINK__NUMBER;
    }

    return true;
}

lxb_inline bool
lxb_css_property_state_flex_grow_basis(lxb_css_parser_t *parser,
                                       const lxb_css_syntax_token_t *token,
                                       lxb_css_property_flex_t *flex)
{
    bool res;
    lxb_css_value_type_t type;

    res = lxb_css_property_state_width_handler(parser, token,
                               (lxb_css_property_flex_basis_t *) &flex->basis);
    if (res) {
        return true;
    }

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return false;
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);

    if (type == LXB_CSS_FLEX_BASIS_CONTENT) {
        flex->basis.type = type;

        lxb_css_syntax_parser_consume(parser);
        return true;
    }

    return false;
}

bool
lxb_css_property_state_flex(lxb_css_parser_t *parser,
                            const lxb_css_syntax_token_t *token, void *ctx)
{
    bool res;
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;
    lxb_css_property_flex_t *flex = declar->u.flex;

    res = lxb_css_property_state_flex_grow_shrink(parser, token, flex);

    if (res) {
        token = lxb_css_syntax_parser_token_wo_ws(parser);
        lxb_css_property_state_check_token(parser, token);

        res = lxb_css_property_state_flex_grow_basis(parser, token, flex);

        if (!res && token->type != LXB_CSS_SYNTAX_TOKEN__END) {
            flex->basis.type = LXB_CSS_VALUE__NUMBER;
            flex->basis.u.length.num = flex->grow.number.num;
            flex->basis.u.length.unit = LXB_CSS_UNIT__UNDEF;
            flex->basis.u.length.is_float = flex->grow.number.is_float;

            flex->grow.type = LXB_CSS_VALUE__UNDEF;

            if (flex->shrink.type != LXB_CSS_VALUE__UNDEF) {
                flex->grow = flex->shrink;
                flex->shrink.type = LXB_CSS_VALUE__UNDEF;

                goto try_shrink_last;
            }

            res = lxb_css_property_state_flex_grow_shrink(parser, token, flex);
            if (!res) {
                return lxb_css_parser_failed(parser);
            }
        }

        return lxb_css_parser_success(parser);
    }
    else {
        res = lxb_css_property_state_flex_grow_basis(parser, token, flex);

        if (res) {
            token = lxb_css_syntax_parser_token_wo_ws(parser);
            lxb_css_property_state_check_token(parser, token);

            (void) lxb_css_property_state_flex_grow_shrink(parser, token, flex);

            return lxb_css_parser_success(parser);
        }
    }

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_FLEX_NONE:
            flex->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);

try_shrink_last:

    res = lxb_css_property_state_number(parser, token, &flex->shrink.number);
    if (res) {
        flex->shrink.type = LXB_CSS_FLEX_SHRINK__NUMBER;
    }

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_flex_grow(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token, void *ctx)
{
    bool res;
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;
    lxb_css_property_flex_grow_t *fg = declar->u.flex_grow;

    res = lxb_css_property_state_number(parser, token, &fg->number);
    if (res) {
        if (fg->number.num < 0) {
            return lxb_css_parser_failed(parser);
        }

        fg->type = LXB_CSS_FLEX_GROW__NUMBER;

        return lxb_css_parser_success(parser);
    }

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
            fg->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_flex_shrink(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token, void *ctx)
{
    bool res;
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;
    lxb_css_property_flex_grow_t *fs = declar->u.flex_shrink;

    res = lxb_css_property_state_number(parser, token, &fs->number);
    if (res) {
        if (fs->number.num < 0) {
            return lxb_css_parser_failed(parser);
        }

        fs->type = LXB_CSS_FLEX_SHRINK__NUMBER;

        return lxb_css_parser_success(parser);
    }

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
            fs->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_flex_basis(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token, void *ctx)
{
    bool res;
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;
    lxb_css_property_flex_basis_t *fb = declar->u.flex_basis;

    res = lxb_css_property_state_width_handler(parser, token,
                                               (lxb_css_property_width_t *) fb);
    if (res) {
        return lxb_css_parser_success(parser);
    }

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        case LXB_CSS_FLEX_BASIS_CONTENT:
            fb->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_justify_content(lxb_css_parser_t *parser,
                                       const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_JUSTIFY_CONTENT_FLEX_START:
        case LXB_CSS_JUSTIFY_CONTENT_FLEX_END:
        case LXB_CSS_JUSTIFY_CONTENT_CENTER:
        case LXB_CSS_JUSTIFY_CONTENT_SPACE_BETWEEN:
        case LXB_CSS_JUSTIFY_CONTENT_SPACE_AROUND:
            declar->u.justify_content->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_align_items(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_ALIGN_ITEMS_FLEX_START:
        case LXB_CSS_ALIGN_ITEMS_FLEX_END:
        case LXB_CSS_ALIGN_ITEMS_CENTER:
        case LXB_CSS_ALIGN_ITEMS_BASELINE:
        case LXB_CSS_ALIGN_ITEMS_STRETCH:
            declar->u.align_items->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_align_self(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_ALIGN_SELF_AUTO:
        case LXB_CSS_ALIGN_SELF_FLEX_START:
        case LXB_CSS_ALIGN_SELF_FLEX_END:
        case LXB_CSS_ALIGN_SELF_CENTER:
        case LXB_CSS_ALIGN_SELF_BASELINE:
        case LXB_CSS_ALIGN_SELF_STRETCH:
            declar->u.align_self->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_align_content(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_ALIGN_CONTENT_FLEX_START:
        case LXB_CSS_ALIGN_CONTENT_FLEX_END:
        case LXB_CSS_ALIGN_CONTENT_CENTER:
        case LXB_CSS_ALIGN_CONTENT_SPACE_BETWEEN:
        case LXB_CSS_ALIGN_CONTENT_SPACE_AROUND:
        case LXB_CSS_ALIGN_CONTENT_STRETCH:
            declar->u.align_content->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_dominant_baseline(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_DOMINANT_BASELINE_AUTO:
        case LXB_CSS_DOMINANT_BASELINE_TEXT_BOTTOM:
        case LXB_CSS_DOMINANT_BASELINE_ALPHABETIC:
        case LXB_CSS_DOMINANT_BASELINE_IDEOGRAPHIC:
        case LXB_CSS_DOMINANT_BASELINE_MIDDLE:
        case LXB_CSS_DOMINANT_BASELINE_CENTRAL:
        case LXB_CSS_DOMINANT_BASELINE_MATHEMATICAL:
        case LXB_CSS_DOMINANT_BASELINE_HANGING:
        case LXB_CSS_DOMINANT_BASELINE_TEXT_TOP:
            declar->u.dominant_baseline->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_alignment_baseline_h(lxb_css_parser_t *parser,
                                            const lxb_css_syntax_token_t *token,
                                            lxb_css_property_alignment_baseline_t *ab)
{
    lxb_css_value_type_t type;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return false;
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        case LXB_CSS_ALIGNMENT_BASELINE_BASELINE:
        case LXB_CSS_ALIGNMENT_BASELINE_TEXT_BOTTOM:
        case LXB_CSS_ALIGNMENT_BASELINE_ALPHABETIC:
        case LXB_CSS_ALIGNMENT_BASELINE_IDEOGRAPHIC:
        case LXB_CSS_ALIGNMENT_BASELINE_MIDDLE:
        case LXB_CSS_ALIGNMENT_BASELINE_CENTRAL:
        case LXB_CSS_ALIGNMENT_BASELINE_MATHEMATICAL:
        case LXB_CSS_ALIGNMENT_BASELINE_TEXT_TOP:
            ab->type = type;

            lxb_css_syntax_parser_consume(parser);
            return true;

        default:
            return false;
    }
}

bool
lxb_css_property_state_baseline_shift_h(lxb_css_parser_t *parser,
                                        const lxb_css_syntax_token_t *token,
                                        lxb_css_property_baseline_shift_t *bs)
{
    bool res;
    lxb_css_value_type_t type;

    res = lxb_css_property_state_length_percentage(parser, token, bs);

    if (res) {
        return true;
    }

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return false;
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        case LXB_CSS_BASELINE_SHIFT_SUB:
        case LXB_CSS_BASELINE_SHIFT_SUPER:
        case LXB_CSS_BASELINE_SHIFT_TOP:
        case LXB_CSS_BASELINE_SHIFT_CENTER:
        case LXB_CSS_BASELINE_SHIFT_BOTTOM:
            bs->type = type;

            lxb_css_syntax_parser_consume(parser);
            return true;

        default:
            return false;
    }
}

bool
lxb_css_property_state_vertical_align(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token, void *ctx)
{
    bool res;
    uint8_t maps;
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;
    lxb_css_property_vertical_align_t *va = declar->u.vertical_align;

    maps = 0;

again:

    res = lxb_css_property_state_alignment_baseline_h(parser, token,
                                                      &va->alignment);
    if (res) {
        if (maps & 1 << 1) {
            return lxb_css_parser_failed(parser);
        }

        maps |= 1 << 1;

        token = lxb_css_syntax_parser_token_wo_ws(parser);
        lxb_css_property_state_check_token(parser, token);

        res = lxb_css_property_state_baseline_shift_h(parser, token,
                                                      &va->shift);
        if (res) {
            if (maps & 1 << 2) {
                return lxb_css_parser_failed(parser);
            }

            maps |= 1 << 2;

            token = lxb_css_syntax_parser_token_wo_ws(parser);
            lxb_css_property_state_check_token(parser, token);
        }
    }
    else {
        res = lxb_css_property_state_baseline_shift_h(parser, token,
                                                      &va->shift);
        if (res) {
            if (maps & 1 << 2) {
                return lxb_css_parser_failed(parser);
            }

            maps |= 1 << 2;

            token = lxb_css_syntax_parser_token_wo_ws(parser);
            lxb_css_property_state_check_token(parser, token);

            res = lxb_css_property_state_alignment_baseline_h(parser, token,
                                                              &va->alignment);
            if (res) {
                if (maps & 1 << 1) {
                    return lxb_css_parser_failed(parser);
                }

                maps |= 1 << 1;

                token = lxb_css_syntax_parser_token_wo_ws(parser);
                lxb_css_property_state_check_token(parser, token);
            }
        }
    }

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        if (maps != 0) {
            return lxb_css_parser_success(parser);
        }

        return lxb_css_parser_failed(parser);
    }

    if (maps & 1 << 3) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_VERTICAL_ALIGN_FIRST:
        case LXB_CSS_VERTICAL_ALIGN_LAST:
            va->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    token = lxb_css_syntax_parser_token_wo_ws(parser);
    lxb_css_property_state_check_token(parser, token);

    maps = 1 << 3;

    goto again;
}

bool
lxb_css_property_state_baseline_source(lxb_css_parser_t *parser,
                                       const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_BASELINE_SOURCE_AUTO:
        case LXB_CSS_BASELINE_SOURCE_FIRST:
        case LXB_CSS_BASELINE_SOURCE_LAST:
            declar->u.baseline_source->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_alignment_baseline(lxb_css_parser_t *parser,
                                          const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_ALIGNMENT_BASELINE_BASELINE:
        case LXB_CSS_ALIGNMENT_BASELINE_TEXT_BOTTOM:
        case LXB_CSS_ALIGNMENT_BASELINE_ALPHABETIC:
        case LXB_CSS_ALIGNMENT_BASELINE_IDEOGRAPHIC:
        case LXB_CSS_ALIGNMENT_BASELINE_MIDDLE:
        case LXB_CSS_ALIGNMENT_BASELINE_CENTRAL:
        case LXB_CSS_ALIGNMENT_BASELINE_MATHEMATICAL:
        case LXB_CSS_ALIGNMENT_BASELINE_TEXT_TOP:
            declar->u.alignment_baseline->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_baseline_shift(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token, void *ctx)
{
    bool res;
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    res = lxb_css_property_state_length_percentage(parser, token,
                                                   declar->u.baseline_shift);
    if (res) {
        return lxb_css_parser_success(parser);
    }

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_BASELINE_SHIFT_SUB:
        case LXB_CSS_BASELINE_SHIFT_SUPER:
        case LXB_CSS_BASELINE_SHIFT_TOP:
        case LXB_CSS_BASELINE_SHIFT_CENTER:
        case LXB_CSS_BASELINE_SHIFT_BOTTOM:
            declar->u.baseline_shift->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_line_height(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token, void *ctx)
{
    bool res;
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    res = lxb_css_property_state_number_length_percentage(parser, token,
                                                          declar->u.line_height);
    if (res) {
        return lxb_css_parser_success(parser);
    }

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_LINE_HEIGHT_NORMAL:
            declar->u.line_height->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_z_index(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token, void *ctx)
{
    bool res;
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    res = lxb_css_property_state_integer(parser, token,
                                         &declar->u.z_index->integer);
    if (res) {
        declar->u.z_index->type = LXB_CSS_VALUE__INTEGER;
        return lxb_css_parser_success(parser);
    }

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_Z_INDEX_AUTO:
            declar->u.line_height->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_direction(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_DIRECTION_LTR:
        case LXB_CSS_DIRECTION_RTL:
            declar->u.direction->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_unicode_bidi(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_UNICODE_BIDI_NORMAL:
        case LXB_CSS_UNICODE_BIDI_EMBED:
        case LXB_CSS_UNICODE_BIDI_ISOLATE:
        case LXB_CSS_UNICODE_BIDI_BIDI_OVERRIDE:
        case LXB_CSS_UNICODE_BIDI_ISOLATE_OVERRIDE:
        case LXB_CSS_UNICODE_BIDI_PLAINTEXT:
            declar->u.unicode_bidi->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_writing_mode(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_WRITING_MODE_HORIZONTAL_TB:
        case LXB_CSS_WRITING_MODE_VERTICAL_RL:
        case LXB_CSS_WRITING_MODE_VERTICAL_LR:
        case LXB_CSS_WRITING_MODE_SIDEWAYS_RL:
        case LXB_CSS_WRITING_MODE_SIDEWAYS_LR:
            declar->u.writing_mode->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_text_orientation(lxb_css_parser_t *parser,
                                        const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_TEXT_ORIENTATION_MIXED:
        case LXB_CSS_TEXT_ORIENTATION_UPRIGHT:
        case LXB_CSS_TEXT_ORIENTATION_SIDEWAYS:
            declar->u.text_orientation->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_text_combine_upright(lxb_css_parser_t *parser,
                                            const lxb_css_syntax_token_t *token, void *ctx)
{
    bool res;
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;
    lxb_css_property_text_combine_upright_t *tcu = declar->u.text_combine_upright;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_TEXT_COMBINE_UPRIGHT_NONE:
        case LXB_CSS_TEXT_COMBINE_UPRIGHT_ALL:
            tcu->type = type;
            break;

        case LXB_CSS_TEXT_COMBINE_UPRIGHT_DIGITS:
            tcu->type = type;

            lxb_css_syntax_parser_consume(parser);
            token = lxb_css_syntax_parser_token_wo_ws(parser);
            lxb_css_property_state_check_token(parser, token);

            res = lxb_css_property_state_integer(parser, token,
                                                 &tcu->digits);
            if (res) {
                if (tcu->digits.num != 2 && tcu->digits.num != 4) {
                    return lxb_css_parser_failed(parser);
                }
            }

            return lxb_css_parser_success(parser);

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_overflow_x(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_OVERFLOW_X_VISIBLE:
        case LXB_CSS_OVERFLOW_X_HIDDEN:
        case LXB_CSS_OVERFLOW_X_CLIP:
        case LXB_CSS_OVERFLOW_X_SCROLL:
        case LXB_CSS_OVERFLOW_X_AUTO:
            declar->u.overflow_x->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_overflow_y(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_overflow_x(parser, token, ctx);
}

bool
lxb_css_property_state_overflow_block(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_overflow_x(parser, token, ctx);
}

bool
lxb_css_property_state_overflow_inline(lxb_css_parser_t *parser,
                                       const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_overflow_x(parser, token, ctx);
}

bool
lxb_css_property_state_text_overflow(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_TEXT_OVERFLOW_CLIP:
        case LXB_CSS_TEXT_OVERFLOW_ELLIPSIS:
            declar->u.text_overflow->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

lxb_status_t
lxb_css_property_state_text_decoration_line_h(lxb_css_parser_t *parser,
                                              const lxb_css_syntax_token_t *token,
                                              lxb_css_property_text_decoration_line_t *tdl)
{
    lxb_css_value_type_t type;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return LXB_STATUS_NEXT;
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        case LXB_CSS_TEXT_DECORATION_LINE_NONE:
            tdl->type = type;

            lxb_css_syntax_parser_consume(parser);
            return LXB_STATUS_OK;

        default:
            goto first;
    }

next:

    lxb_css_syntax_parser_consume(parser);
    token = lxb_css_syntax_parser_token_wo_ws(parser);
    if (token == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return LXB_STATUS_OK;
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);

first:

    switch (type) {
        case LXB_CSS_TEXT_DECORATION_LINE_UNDERLINE:
            if (tdl->underline != LXB_CSS_VALUE__UNDEF) {
                return LXB_STATUS_STOP;
            }

            tdl->underline = type;
            goto next;

        case LXB_CSS_TEXT_DECORATION_LINE_OVERLINE:
            if (tdl->overline != LXB_CSS_VALUE__UNDEF) {
                return LXB_STATUS_STOP;
            }

            tdl->overline = type;
            goto next;

        case LXB_CSS_TEXT_DECORATION_LINE_LINE_THROUGH:
            if (tdl->line_through != LXB_CSS_VALUE__UNDEF) {
                return LXB_STATUS_STOP;
            }

            tdl->line_through = type;
            goto next;

        case LXB_CSS_TEXT_DECORATION_LINE_BLINK:
            if (tdl->blink != LXB_CSS_VALUE__UNDEF) {
                return LXB_STATUS_STOP;
            }

            tdl->blink = type;
            goto next;

        default:
            if (tdl->underline != LXB_CSS_VALUE__UNDEF
                || tdl->overline != LXB_CSS_VALUE__UNDEF
                || tdl->line_through != LXB_CSS_VALUE__UNDEF
                || tdl->blink != LXB_CSS_VALUE__UNDEF)
            {
                return LXB_STATUS_OK;
            }

            return LXB_STATUS_NEXT;
    }
}

bool
lxb_css_property_state_text_decoration_line(lxb_css_parser_t *parser,
                                            const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_status_t status;
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;
    lxb_css_property_text_decoration_line_t *tdl = declar->u.text_decoration_line;

    status = lxb_css_property_state_text_decoration_line_h(parser, token, tdl);

    if (status == LXB_STATUS_OK) {
        return lxb_css_parser_success(parser);
    }
    else if (status == LXB_STATUS_STOP) {
        return lxb_css_parser_failed(parser);
    }
    else if (status != LXB_STATUS_NEXT) {
        return lxb_css_parser_memory_fail(parser);
    }

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
            tdl->type = type;

            lxb_css_syntax_parser_consume(parser);
            return lxb_css_parser_success(parser);

        default:
            return lxb_css_parser_failed(parser);
    }
}

bool
lxb_css_property_state_text_decoration_style_h(lxb_css_parser_t *parser,
                            const lxb_css_syntax_token_t *token,
                            lxb_css_property_text_decoration_style_t *tds)
{
    lxb_css_value_type_t type;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return false;
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        case LXB_CSS_TEXT_DECORATION_STYLE_SOLID:
        case LXB_CSS_TEXT_DECORATION_STYLE_DOUBLE:
        case LXB_CSS_TEXT_DECORATION_STYLE_DOTTED:
        case LXB_CSS_TEXT_DECORATION_STYLE_DASHED:
        case LXB_CSS_TEXT_DECORATION_STYLE_WAVY:
            tds->type = type;

            lxb_css_syntax_parser_consume(parser);
            return true;

        default:
            return false;
    }
}

bool
lxb_css_property_state_text_decoration_style(lxb_css_parser_t *parser,
                                             const lxb_css_syntax_token_t *token, void *ctx)
{
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        return lxb_css_parser_failed(parser);
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
        /* Local. */
        case LXB_CSS_TEXT_DECORATION_STYLE_SOLID:
        case LXB_CSS_TEXT_DECORATION_STYLE_DOUBLE:
        case LXB_CSS_TEXT_DECORATION_STYLE_DOTTED:
        case LXB_CSS_TEXT_DECORATION_STYLE_DASHED:
        case LXB_CSS_TEXT_DECORATION_STYLE_WAVY:
            declar->u.text_decoration_style->type = type;
            break;

        default:
            return lxb_css_parser_failed(parser);
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_parser_success(parser);
}

bool
lxb_css_property_state_text_decoration_color(lxb_css_parser_t *parser,
                                             const lxb_css_syntax_token_t *token, void *ctx)
{
    return lxb_css_property_state_color(parser, token, ctx);
}

bool
lxb_css_property_state_text_decoration(lxb_css_parser_t *parser,
                                       const lxb_css_syntax_token_t *token, void *ctx)
{
    bool res, line, style, color;
    lxb_status_t status;
    lxb_css_value_type_t type;
    lxb_css_rule_declaration_t *declar = ctx;
    lxb_css_property_text_decoration_t *td = declar->u.text_decoration;

    if (token->type != LXB_CSS_SYNTAX_TOKEN_IDENT) {
        goto lsc;
    }

    type = lxb_css_value_by_name(lxb_css_syntax_token_ident(token)->data,
                                 lxb_css_syntax_token_ident(token)->length);
    switch (type) {
        /* Global. */
        case LXB_CSS_VALUE_INITIAL:
        case LXB_CSS_VALUE_INHERIT:
        case LXB_CSS_VALUE_UNSET:
        case LXB_CSS_VALUE_REVERT:
            td->line.type = type;

            lxb_css_syntax_parser_consume(parser);
            return lxb_css_parser_success(parser);

        default:
            break;
    }

lsc:

    line = false;
    style = false;
    color = false;

    for (size_t i = 0; i < 3; i++) {
        if (!line) {
            status = lxb_css_property_state_text_decoration_line_h(parser, token,
                                                                   &td->line);
            if (status == LXB_STATUS_OK) {
                token = lxb_css_syntax_parser_token_wo_ws(parser);
                lxb_css_property_state_check_token(parser, token);

                line = true;
            }
            else if (status == LXB_STATUS_STOP) {
                return lxb_css_parser_failed(parser);
            }
            else if (status != LXB_STATUS_NEXT) {
                return lxb_css_parser_memory_fail(parser);
            }
        }

        if (!style) {
            res = lxb_css_property_state_text_decoration_style_h(parser, token,
                                                                 &td->style);
            if (res) {
                token = lxb_css_syntax_parser_token_wo_ws(parser);
                lxb_css_property_state_check_token(parser, token);

                style = true;
            }
        }

        if (!color) {
            res = lxb_css_property_state_color_handler(parser, token,
                                         (lxb_css_value_color_t *) &td->color,
                                         &status);
            if (res) {
                token = lxb_css_syntax_parser_token_wo_ws(parser);
                lxb_css_property_state_check_token(parser, token);

                color = true;
            }
            else {
                if (status != LXB_STATUS_OK) {
                    return lxb_css_parser_failed(parser);
                }
            }
        }
    }

    if (!line && !style && !color) {
        return lxb_css_parser_failed(parser);
    }

    return lxb_css_parser_success(parser);
}
