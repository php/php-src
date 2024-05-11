/*
 * Copyright (C) 2022-2023 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LXB_CSS_VALUE_H
#define LXB_CSS_VALUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/css/base.h"
#include "lexbor/css/value/const.h"
#include "lexbor/css/unit/const.h"

typedef struct {
    double              num;
    bool                is_float;
}
lxb_css_value_number_t;

typedef struct {
    long num;
}
lxb_css_value_integer_t;

typedef lxb_css_value_number_t lxb_css_value_percentage_t;

typedef struct {
    double              num;
    bool                is_float;
    lxb_css_unit_t      unit;
}
lxb_css_value_length_t;

typedef struct {
    lxb_css_value_type_t type;

    union {
        lxb_css_value_length_t     length;
        lxb_css_value_percentage_t percentage;
    } u;
}
lxb_css_value_length_percentage_t;

typedef struct {
    lxb_css_value_type_t type;

    union {
        lxb_css_value_number_t     number;
        lxb_css_value_length_t     length;
        lxb_css_value_percentage_t percentage;
    } u;
}
lxb_css_value_number_length_percentage_t;

typedef struct {
    lxb_css_value_type_t type;

    union {
        lxb_css_value_number_t number;
        lxb_css_value_length_t length;
    } u;
}
lxb_css_value_number_length_t;

typedef struct {
    lxb_css_value_type_t type;

    union {
        lxb_css_value_number_t     number;
        lxb_css_value_percentage_t percentage;
    } u;
}
lxb_css_value_number_percentage_t;

typedef struct {
    lxb_css_value_type_t   type;
    lxb_css_value_number_t number;
}
lxb_css_value_number_type_t;

typedef struct {
    lxb_css_value_type_t    type;
    lxb_css_value_integer_t integer;
}
lxb_css_value_integer_type_t;

typedef struct {
    lxb_css_value_type_t       type;
    lxb_css_value_percentage_t percentage;
}
lxb_css_value_percentage_type_t;

typedef struct {
    lxb_css_value_type_t   type;
    lxb_css_value_length_t length;
}
lxb_css_value_length_type_t;

typedef struct {
    lxb_css_value_type_t              type;
    lxb_css_value_length_percentage_t length;
}
lxb_css_value_length_percentage_type_t;

typedef struct {
    double               num;
    bool                 is_float;
    lxb_css_unit_angel_t unit;
}
lxb_css_value_angle_t;

typedef struct {
    lxb_css_value_type_t  type;
    lxb_css_value_angle_t angle;
}
lxb_css_value_angle_type_t;

typedef struct {
    lxb_css_value_type_t type;

    union {
        lxb_css_value_number_t number;
        lxb_css_value_angle_t  angle;
    } u;
}
lxb_css_value_hue_t;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
}
lxb_css_value_color_hex_rgba_t;

typedef enum {
    LXB_CSS_PROPERTY_COLOR_HEX_TYPE_3 = 0x00,
    LXB_CSS_PROPERTY_COLOR_HEX_TYPE_4,
    LXB_CSS_PROPERTY_COLOR_HEX_TYPE_6,
    LXB_CSS_PROPERTY_COLOR_HEX_TYPE_8
}
lxb_css_value_color_hex_type_t;

typedef struct {
    lxb_css_value_color_hex_rgba_t rgba;
    lxb_css_value_color_hex_type_t type;
}
lxb_css_value_color_hex_t;

typedef struct {
    /*
     * If R is <percent> when G and B should be <percent> to.
     * If R is <number> when G and B should be <number> to.
     * R, G, B can be NONE regardless of neighboring values.
     * 'A' can be <percentage> or <number> or NONE.
     */

    lxb_css_value_number_percentage_t r;
    lxb_css_value_number_percentage_t g;
    lxb_css_value_number_percentage_t b;
    lxb_css_value_number_percentage_t a;

    bool                              old;
}
lxb_css_value_color_rgba_t;

typedef struct {
    lxb_css_value_hue_t               h;
    lxb_css_value_percentage_type_t   s;
    lxb_css_value_percentage_type_t   l;
    lxb_css_value_number_percentage_t a;

    bool                              old;
}
lxb_css_value_color_hsla_t;

typedef struct {
    lxb_css_value_number_percentage_t l;
    lxb_css_value_number_percentage_t a;
    lxb_css_value_number_percentage_t b;
    lxb_css_value_number_percentage_t alpha;
}
lxb_css_value_color_lab_t;

typedef struct {
    lxb_css_value_number_percentage_t l;
    lxb_css_value_number_percentage_t c;
    lxb_css_value_hue_t               h;
    lxb_css_value_number_percentage_t a;
}
lxb_css_value_color_lch_t;

typedef struct {
    lxb_css_value_type_t type;

    union {
        lxb_css_value_color_hex_t  hex;
        lxb_css_value_color_rgba_t rgb;
        lxb_css_value_color_hsla_t hsl;
        lxb_css_value_color_hsla_t hwb;
        lxb_css_value_color_lab_t  lab;
        lxb_css_value_color_lch_t  lch;
    } u;
}
lxb_css_value_color_t;


LXB_API const lxb_css_data_t *
lxb_css_value_by_id(uintptr_t id);

LXB_API lxb_css_value_type_t
lxb_css_value_by_name(const lxb_char_t *name, size_t length);

LXB_API lxb_status_t
lxb_css_value_serialize(lxb_css_value_type_t type,
                        lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_css_value_percentage_sr(const lxb_css_value_percentage_t *percent,
                            lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_css_value_length_sr(const lxb_css_value_length_t *len,
                        lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_css_value_number_sr(const lxb_css_value_number_t *number,
                        lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_css_value_integer_sr(const lxb_css_value_integer_t *integer,
                         lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_css_value_length_percentage_sr(const lxb_css_value_length_percentage_t *lp,
                                   lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_css_value_number_length_sr(const lxb_css_value_number_length_t *nl,
                               lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_css_value_number_percentage_sr(const lxb_css_value_number_percentage_t *np,
                                   lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_css_value_length_type_sr(const lxb_css_value_length_type_t *lt,
                             lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_css_value_percentage_type_sr(const lxb_css_value_percentage_type_t *pt,
                                 lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_css_value_number_type_sr(const lxb_css_value_number_type_t *num,
                             lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_css_value_integer_type_sr(const lxb_css_value_integer_type_t *num,
                              lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_css_value_length_percentage_type_sr(const lxb_css_value_length_percentage_type_t *lpt,
                                        lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_css_value_number_length_percentage_type_sr(const lxb_css_value_number_length_percentage_t *nlp,
                                               lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_css_value_angle_sr(const lxb_css_value_angle_t *angle,
                       lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_css_value_hue_sr(const lxb_css_value_hue_t *hue,
                     lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_css_value_color_serialize(const lxb_css_value_color_t *color,
                              lexbor_serialize_cb_f cb, void *ctx);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LXB_CSS_VALUE_H */
