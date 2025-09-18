/*
 * Copyright (C) 2021-2023 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LXB_CSS_PROPERTY_H
#define LXB_CSS_PROPERTY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/serialize.h"
#include "lexbor/core/array_obj.h"
#include "lexbor/css/base.h"
#include "lexbor/css/value.h"
#include "lexbor/css/unit/const.h"
#include "lexbor/css/property/const.h"


typedef struct {
    lxb_css_property_type_t type;
    lexbor_str_t            value;
}
lxb_css_property__undef_t;

typedef struct {
    lexbor_str_t name;
    lexbor_str_t value;
}
lxb_css_property__custom_t;

typedef struct {
    lxb_css_display_type_t a;
    lxb_css_display_type_t b;
    lxb_css_display_type_t c;
}
lxb_css_property_display_t;

typedef lxb_css_value_integer_type_t lxb_css_property_order_t;

typedef struct {
    lxb_css_visibility_type_t type;
}
lxb_css_property_visibility_t;

typedef lxb_css_value_length_percentage_t lxb_css_property_width_t;
typedef lxb_css_value_length_percentage_t lxb_css_property_height_t;
typedef lxb_css_value_length_percentage_t lxb_css_property_min_width_t;
typedef lxb_css_value_length_percentage_t lxb_css_property_min_height_t;
typedef lxb_css_value_length_percentage_t lxb_css_property_max_width_t;
typedef lxb_css_value_length_percentage_t lxb_css_property_max_height_t;
typedef lxb_css_value_length_percentage_t lxb_css_property_margin_top_t;
typedef lxb_css_value_length_percentage_t lxb_css_property_margin_right_t;
typedef lxb_css_value_length_percentage_t lxb_css_property_margin_bottom_t;
typedef lxb_css_value_length_percentage_t lxb_css_property_margin_left_t;
typedef lxb_css_value_length_percentage_t lxb_css_property_padding_top_t;
typedef lxb_css_value_length_percentage_t lxb_css_property_padding_right_t;
typedef lxb_css_value_length_percentage_t lxb_css_property_padding_bottom_t;
typedef lxb_css_value_length_percentage_t lxb_css_property_padding_left_t;

typedef struct {
    lxb_css_box_sizing_type_t type;
}
lxb_css_property_box_sizing_t;

typedef struct {
    lxb_css_property_margin_top_t    top;
    lxb_css_property_margin_right_t  right;
    lxb_css_property_margin_bottom_t bottom;
    lxb_css_property_margin_left_t   left;
}
lxb_css_property_margin_t;

typedef struct {
    lxb_css_property_padding_top_t    top;
    lxb_css_property_padding_right_t  right;
    lxb_css_property_padding_bottom_t bottom;
    lxb_css_property_padding_left_t   left;
}
lxb_css_property_padding_t;

typedef struct {
    lxb_css_value_type_t        style;
    lxb_css_value_length_type_t width;
    lxb_css_value_color_t       color;
}
lxb_css_property_border_t;

typedef lxb_css_property_border_t lxb_css_property_border_top_t;
typedef lxb_css_property_border_t lxb_css_property_border_right_t;
typedef lxb_css_property_border_t lxb_css_property_border_bottom_t;
typedef lxb_css_property_border_t lxb_css_property_border_left_t;

typedef lxb_css_value_color_t lxb_css_property_border_top_color_t;
typedef lxb_css_value_color_t lxb_css_property_border_right_color_t;
typedef lxb_css_value_color_t lxb_css_property_border_bottom_color_t;
typedef lxb_css_value_color_t lxb_css_property_border_left_color_t;

typedef lxb_css_value_color_t lxb_css_property_background_color_t;

typedef lxb_css_value_color_t lxb_css_property_color_t;
typedef lxb_css_value_number_percentage_t lxb_css_property_opacity_t;

typedef struct {
    lxb_css_position_type_t type;
}
lxb_css_property_position_t;

typedef lxb_css_value_length_percentage_t lxb_css_property_top_t;
typedef lxb_css_value_length_percentage_t lxb_css_property_right_t;
typedef lxb_css_value_length_percentage_t lxb_css_property_bottom_t;
typedef lxb_css_value_length_percentage_t lxb_css_property_left_t;

typedef lxb_css_value_length_percentage_t lxb_css_property_inset_block_start_t;
typedef lxb_css_value_length_percentage_t lxb_css_property_inset_inline_start_t;
typedef lxb_css_value_length_percentage_t lxb_css_property_inset_block_end_t;
typedef lxb_css_value_length_percentage_t lxb_css_property_inset_inline_end_t;

typedef struct {
    lxb_css_text_transform_type_t type_case;
    lxb_css_text_transform_type_t full_width;
    lxb_css_text_transform_type_t full_size_kana;
}
lxb_css_property_text_transform_t;

typedef struct {
    lxb_css_text_align_type_t type;
}
lxb_css_property_text_align_t;

typedef struct {
    lxb_css_text_align_all_type_t type;
}
lxb_css_property_text_align_all_t;

typedef struct {
    lxb_css_text_align_last_type_t type;
}
lxb_css_property_text_align_last_t;

typedef struct {
    lxb_css_text_justify_type_t type;
}
lxb_css_property_text_justify_t;

typedef struct {
    lxb_css_value_length_percentage_t length;
    lxb_css_text_indent_type_t        type;
    lxb_css_text_indent_type_t        hanging;
    lxb_css_text_indent_type_t        each_line;
}
lxb_css_property_text_indent_t;

typedef struct {
    lxb_css_white_space_type_t type;
}
lxb_css_property_white_space_t;

typedef lxb_css_value_number_length_t lxb_css_property_tab_size_t;

typedef struct {
    lxb_css_word_break_type_t type;
}
lxb_css_property_word_break_t;

typedef struct {
    lxb_css_line_break_type_t type;
}
lxb_css_property_line_break_t;

typedef struct {
    lxb_css_hyphens_type_t type;
}
lxb_css_property_hyphens_t;

typedef struct {
    lxb_css_overflow_wrap_type_t type;
}
lxb_css_property_overflow_wrap_t;

typedef struct {
    lxb_css_word_wrap_type_t type;
}
lxb_css_property_word_wrap_t;

typedef lxb_css_value_length_type_t lxb_css_property_word_spacing_t;
typedef lxb_css_value_length_type_t lxb_css_property_letter_spacing_t;

typedef struct {
    lxb_css_hanging_punctuation_type_t type_first;
    lxb_css_hanging_punctuation_type_t force_allow;
    lxb_css_hanging_punctuation_type_t last;
}
lxb_css_property_hanging_punctuation_t;

typedef struct lxb_css_property_family_name lxb_css_property_family_name_t;

struct lxb_css_property_family_name {
    bool generic;

    union {
        lxb_css_font_family_type_t type;
        lexbor_str_t               str;
    } u;

    lxb_css_property_family_name_t *next;
    lxb_css_property_family_name_t *prev;
};

typedef struct {
    lxb_css_property_family_name_t *first;
    lxb_css_property_family_name_t *last;

    size_t                         count;
}
lxb_css_property_font_family_t;

typedef lxb_css_value_number_type_t lxb_css_property_font_weight_t;
typedef lxb_css_value_percentage_type_t lxb_css_property_font_stretch_t;
typedef lxb_css_value_angle_type_t lxb_css_property_font_style_t;
typedef lxb_css_value_length_percentage_type_t lxb_css_property_font_size_t;

typedef struct {
    lxb_css_float_reference_type_t type;
}
lxb_css_property_float_reference_t;

typedef struct {
    lxb_css_float_type_t        type;
    lxb_css_value_length_type_t length;
    lxb_css_float_type_t        snap_type;
}
lxb_css_property_float_t;

typedef struct {
    lxb_css_clear_type_t type;
}
lxb_css_property_clear_t;

typedef lxb_css_value_length_percentage_t lxb_css_property_float_offset_t;
typedef lxb_css_value_integer_type_t lxb_css_property_float_defer_t;

typedef struct {
    lxb_css_wrap_flow_type_t type;
}
lxb_css_property_wrap_flow_t;

typedef struct {
    lxb_css_wrap_through_type_t type;
}
lxb_css_property_wrap_through_t;

typedef struct {
    lxb_css_flex_direction_type_t type;
}
lxb_css_property_flex_direction_t;

typedef struct {
    lxb_css_flex_wrap_type_t type;
}
lxb_css_property_flex_wrap_t;

typedef struct {
    lxb_css_flex_direction_type_t type_direction;
    lxb_css_flex_wrap_type_t      wrap;
}
lxb_css_property_flex_flow_t;

typedef lxb_css_value_number_type_t lxb_css_property_flex_grow_t;
typedef lxb_css_value_number_type_t lxb_css_property_flex_shrink_t;
typedef lxb_css_property_width_t lxb_css_property_flex_basis_t;

typedef struct {
    lxb_css_flex_type_t            type;
    lxb_css_property_flex_grow_t   grow;
    lxb_css_property_flex_shrink_t shrink;
    lxb_css_property_flex_basis_t  basis;
}
lxb_css_property_flex_t;

typedef struct {
    lxb_css_justify_content_type_t type;
}
lxb_css_property_justify_content_t;

typedef struct {
    lxb_css_align_items_type_t type;
}
lxb_css_property_align_items_t;

typedef struct {
    lxb_css_align_self_type_t type;
}
lxb_css_property_align_self_t;

typedef struct {
    lxb_css_align_content_type_t type;
}
lxb_css_property_align_content_t;

typedef struct {
    lxb_css_dominant_baseline_type_t type;
}
lxb_css_property_dominant_baseline_t;

typedef struct {
    lxb_css_baseline_source_type_t type;
}
lxb_css_property_baseline_source_t;

typedef struct {
    lxb_css_alignment_baseline_type_t type;
}
lxb_css_property_alignment_baseline_t;

typedef lxb_css_value_length_percentage_t lxb_css_property_baseline_shift_t;

typedef struct {
    lxb_css_vertical_align_type_t         type;
    lxb_css_property_alignment_baseline_t alignment;
    lxb_css_property_baseline_shift_t     shift;
}
lxb_css_property_vertical_align_t;

typedef lxb_css_value_number_length_percentage_t lxb_css_property_line_height_t;

typedef lxb_css_value_integer_type_t lxb_css_property_z_index_t;

typedef struct {
    lxb_css_direction_type_t type;
}
lxb_css_property_direction_t;

typedef struct {
    lxb_css_unicode_bidi_type_t type;
}
lxb_css_property_unicode_bidi_t;

typedef struct {
    lxb_css_writing_mode_type_t type;
}
lxb_css_property_writing_mode_t;

typedef struct {
    lxb_css_text_orientation_type_t type;
}
lxb_css_property_text_orientation_t;

typedef struct {
    lxb_css_text_combine_upright_type_t type;
     /*
      * If the integer is omitted, it computes to 2.
      * Integers outside the range 2-4 are invalid.
      */
    lxb_css_value_integer_t             digits;
}
lxb_css_property_text_combine_upright_t;

typedef struct {
    lxb_css_overflow_x_type_t type;
}
lxb_css_property_overflow_x_t;

typedef struct {
    lxb_css_overflow_y_type_t type;
}
lxb_css_property_overflow_y_t;

typedef struct {
    lxb_css_overflow_block_type_t type;
}
lxb_css_property_overflow_block_t;

typedef struct {
    lxb_css_overflow_inline_type_t type;
}
lxb_css_property_overflow_inline_t;

typedef struct {
    lxb_css_text_overflow_type_t type;
}
lxb_css_property_text_overflow_t;

typedef struct {
    lxb_css_text_decoration_line_type_t type;
    lxb_css_text_decoration_line_type_t underline;
    lxb_css_text_decoration_line_type_t overline;
    lxb_css_text_decoration_line_type_t line_through;
    lxb_css_text_decoration_line_type_t blink;
}
lxb_css_property_text_decoration_line_t;

typedef struct {
    lxb_css_text_decoration_style_type_t type;
}
lxb_css_property_text_decoration_style_t;

typedef lxb_css_value_color_t lxb_css_property_text_decoration_color_t;

typedef struct {
    lxb_css_property_text_decoration_line_t  line;
    lxb_css_property_text_decoration_style_t style;
    lxb_css_property_text_decoration_color_t color;
}
lxb_css_property_text_decoration_t;


LXB_API const lxb_css_entry_data_t *
lxb_css_property_by_name(const lxb_char_t *name, size_t length);

LXB_API const lxb_css_entry_data_t *
lxb_css_property_by_id(uintptr_t id);

LXB_API const void *
lxb_css_property_initial_by_id(uintptr_t id);

LXB_API void *
lxb_css_property_destroy(lxb_css_memory_t *memory, void *style,
                         lxb_css_property_type_t type, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_serialize(const void *style, lxb_css_property_type_t type,
                           lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_css_property_serialize_str(const void *style, lxb_css_property_type_t type,
                               lexbor_mraw_t *mraw, lexbor_str_t *str);

LXB_API lxb_status_t
lxb_css_property_serialize_name(const void *style, lxb_css_property_type_t type,
                                lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_css_property_serialize_name_str(const void *style, lxb_css_property_type_t type,
                                    lexbor_mraw_t *mraw, lexbor_str_t *str);

/* _undef. */

LXB_API void *
lxb_css_property__undef_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property__undef_destroy(lxb_css_memory_t *memory,
                                void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property__undef_serialize(const void *style,
                                  lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_css_property__undef_serialize_name(const void *style,
                                       lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_css_property__undef_serialize_value(const void *style,
                                        lexbor_serialize_cb_f cb, void *ctx);

/* _custom. */

LXB_API void *
lxb_css_property__custom_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property__custom_destroy(lxb_css_memory_t *memory,
                                 void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property__custom_serialize(const void *style,
                                   lexbor_serialize_cb_f cb, void *ctx);
LXB_API lxb_status_t
lxb_css_property__custom_serialize_name(const void *style,
                                        lexbor_serialize_cb_f cb, void *ctx);
LXB_API lxb_status_t
lxb_css_property__custom_serialize_value(const void *style,
                                         lexbor_serialize_cb_f cb, void *ctx);

/* Display. */

LXB_API void *
lxb_css_property_display_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_display_destroy(lxb_css_memory_t *memory,
                                 void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_display_serialize(const void *style,
                                   lexbor_serialize_cb_f cb, void *ctx);

/* Order. */

LXB_API void *
lxb_css_property_order_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_order_destroy(lxb_css_memory_t *memory,
                               void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_order_serialize(const void *style,
                                 lexbor_serialize_cb_f cb, void *ctx);

/* Visibility. */

LXB_API void *
lxb_css_property_visibility_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_visibility_destroy(lxb_css_memory_t *memory,
                                    void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_visibility_serialize(const void *style,
                                      lexbor_serialize_cb_f cb, void *ctx);

/* Width. */

LXB_API void *
lxb_css_property_width_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_width_destroy(lxb_css_memory_t *memory,
                               void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_width_serialize(const void *style,
                                 lexbor_serialize_cb_f cb, void *ctx);

/* Height. */

LXB_API void *
lxb_css_property_height_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_height_destroy(lxb_css_memory_t *memory,
                                void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_height_serialize(const void *style,
                                  lexbor_serialize_cb_f cb, void *ctx);

/* Box-sizing. */

LXB_API void *
lxb_css_property_box_sizing_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_box_sizing_destroy(lxb_css_memory_t *memory,
                                    void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_box_sizing_serialize(const void *style,
                                      lexbor_serialize_cb_f cb, void *ctx);

/* Min-width. */

LXB_API void *
lxb_css_property_min_width_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_min_width_destroy(lxb_css_memory_t *memory,
                                   void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_min_width_serialize(const void *style,
                                     lexbor_serialize_cb_f cb, void *ctx);

/* Min-height. */

LXB_API void *
lxb_css_property_min_height_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_min_height_destroy(lxb_css_memory_t *memory,
                                    void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_min_height_serialize(const void *style,
                                      lexbor_serialize_cb_f cb, void *ctx);

/* Max-width. */

LXB_API void *
lxb_css_property_max_width_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_max_width_destroy(lxb_css_memory_t *memory,
                                   void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_max_width_serialize(const void *style,
                                     lexbor_serialize_cb_f cb, void *ctx);

/* Max-height. */

LXB_API void *
lxb_css_property_max_height_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_max_height_destroy(lxb_css_memory_t *memory,
                                    void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_max_height_serialize(const void *style,
                                      lexbor_serialize_cb_f cb, void *ctx);

/* Margin. */

LXB_API void *
lxb_css_property_margin_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_margin_destroy(lxb_css_memory_t *memory,
                                void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_margin_serialize(const void *style,
                                  lexbor_serialize_cb_f cb, void *ctx);

/* Margin-top. */

LXB_API void *
lxb_css_property_margin_top_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_margin_top_destroy(lxb_css_memory_t *memory,
                                    void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_margin_top_serialize(const void *style,
                                      lexbor_serialize_cb_f cb, void *ctx);

/* Margin-right. */

LXB_API void *
lxb_css_property_margin_right_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_margin_right_destroy(lxb_css_memory_t *memory,
                                      void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_margin_right_serialize(const void *style,
                                        lexbor_serialize_cb_f cb, void *ctx);

/* Margin-bottom. */

LXB_API void *
lxb_css_property_margin_bottom_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_margin_bottom_destroy(lxb_css_memory_t *memory,
                                       void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_margin_bottom_serialize(const void *style,
                                         lexbor_serialize_cb_f cb, void *ctx);

/* Margin-left. */

LXB_API void *
lxb_css_property_margin_left_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_margin_left_destroy(lxb_css_memory_t *memory,
                                     void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_margin_left_serialize(const void *style,
                                       lexbor_serialize_cb_f cb, void *ctx);

/* Padding. */

LXB_API void *
lxb_css_property_padding_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_padding_destroy(lxb_css_memory_t *memory,
                                 void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_padding_serialize(const void *style,
                                   lexbor_serialize_cb_f cb, void *ctx);

/* Padding-top. */

LXB_API void *
lxb_css_property_padding_top_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_padding_top_destroy(lxb_css_memory_t *memory,
                                     void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_padding_top_serialize(const void *style,
                                       lexbor_serialize_cb_f cb, void *ctx);

/* Padding-right. */

LXB_API void *
lxb_css_property_padding_right_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_padding_right_destroy(lxb_css_memory_t *memory,
                                       void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_padding_right_serialize(const void *style,
                                         lexbor_serialize_cb_f cb, void *ctx);

/* Padding-bottom. */

LXB_API void *
lxb_css_property_padding_bottom_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_padding_bottom_destroy(lxb_css_memory_t *memory,
                                        void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_padding_bottom_serialize(const void *style,
                                          lexbor_serialize_cb_f cb, void *ctx);

/* Padding-left. */

LXB_API void *
lxb_css_property_padding_left_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_padding_left_destroy(lxb_css_memory_t *memory,
                                      void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_padding_left_serialize(const void *style,
                                        lexbor_serialize_cb_f cb, void *ctx);

/* Border. */

LXB_API void *
lxb_css_property_border_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_border_destroy(lxb_css_memory_t *memory,
                                void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_border_serialize(const void *style,
                                  lexbor_serialize_cb_f cb, void *ctx);

/* Border-top. */

LXB_API void *
lxb_css_property_border_top_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_border_top_destroy(lxb_css_memory_t *memory,
                                    void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_border_top_serialize(const void *style,
                                      lexbor_serialize_cb_f cb, void *ctx);

/* Border-right. */

LXB_API void *
lxb_css_property_border_right_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_border_right_destroy(lxb_css_memory_t *memory,
                                      void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_border_right_serialize(const void *style,
                                        lexbor_serialize_cb_f cb, void *ctx);

/* Border-bottom. */

LXB_API void *
lxb_css_property_border_bottom_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_border_bottom_destroy(lxb_css_memory_t *memory,
                                       void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_border_bottom_serialize(const void *style,
                                         lexbor_serialize_cb_f cb, void *ctx);

/* Border-left. */

LXB_API void *
lxb_css_property_border_left_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_border_left_destroy(lxb_css_memory_t *memory,
                                     void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_border_left_serialize(const void *style,
                                       lexbor_serialize_cb_f cb, void *ctx);

/* Border-top-color. */

LXB_API void *
lxb_css_property_border_top_color_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_border_top_color_destroy(lxb_css_memory_t *memory,
                                          void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_border_top_color_serialize(const void *style,
                                            lexbor_serialize_cb_f cb, void *ctx);

/* Border-right-color. */

LXB_API void *
lxb_css_property_border_right_color_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_border_right_color_destroy(lxb_css_memory_t *memory,
                                            void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_border_right_color_serialize(const void *style,
                                              lexbor_serialize_cb_f cb, void *ctx);

/* Border-bottom-color. */

LXB_API void *
lxb_css_property_border_bottom_color_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_border_bottom_color_destroy(lxb_css_memory_t *memory,
                                             void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_border_bottom_color_serialize(const void *style,
                                               lexbor_serialize_cb_f cb, void *ctx);

/* Border-left-color. */

LXB_API void *
lxb_css_property_border_left_color_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_border_left_color_destroy(lxb_css_memory_t *memory,
                                           void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_border_left_color_serialize(const void *style,
                                             lexbor_serialize_cb_f cb, void *ctx);

/* Background-color. */

LXB_API void *
lxb_css_property_background_color_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_background_color_destroy(lxb_css_memory_t *memory,
                                          void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_background_color_serialize(const void *style,
                                            lexbor_serialize_cb_f cb, void *ctx);

/* Color. */

LXB_API void *
lxb_css_property_color_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_color_destroy(lxb_css_memory_t *memory,
                               void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_color_serialize(const void *style,
                                 lexbor_serialize_cb_f cb, void *ctx);

/* Opacity. */

LXB_API void *
lxb_css_property_opacity_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_opacity_destroy(lxb_css_memory_t *memory,
                                 void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_opacity_serialize(const void *style,
                                   lexbor_serialize_cb_f cb, void *ctx);

/* Position. */

LXB_API void *
lxb_css_property_position_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_position_destroy(lxb_css_memory_t *memory,
                                  void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_position_serialize(const void *style,
                                    lexbor_serialize_cb_f cb, void *ctx);

/* Top. */

LXB_API void *
lxb_css_property_top_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_top_destroy(lxb_css_memory_t *memory,
                             void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_top_serialize(const void *style,
                               lexbor_serialize_cb_f cb, void *ctx);

/* Right. */

LXB_API void *
lxb_css_property_right_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_right_destroy(lxb_css_memory_t *memory,
                               void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_right_serialize(const void *style,
                                 lexbor_serialize_cb_f cb, void *ctx);

/* Bottom. */

LXB_API void *
lxb_css_property_bottom_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_bottom_destroy(lxb_css_memory_t *memory,
                                void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_bottom_serialize(const void *style,
                                  lexbor_serialize_cb_f cb, void *ctx);

/* Left. */

LXB_API void *
lxb_css_property_left_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_left_destroy(lxb_css_memory_t *memory,
                              void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_left_serialize(const void *style,
                                lexbor_serialize_cb_f cb, void *ctx);

/* Inset-block-start. */

LXB_API void *
lxb_css_property_inset_block_start_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_inset_block_start_destroy(lxb_css_memory_t *memory,
                                           void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_inset_block_start_serialize(const void *style,
                                             lexbor_serialize_cb_f cb, void *ctx);

/* Inset-inline-start. */

LXB_API void *
lxb_css_property_inset_inline_start_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_inset_inline_start_destroy(lxb_css_memory_t *memory,
                                            void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_inset_inline_start_serialize(const void *style,
                                              lexbor_serialize_cb_f cb, void *ctx);

/* Inset-block-end. */

LXB_API void *
lxb_css_property_inset_block_end_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_inset_block_end_destroy(lxb_css_memory_t *memory,
                                         void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_inset_block_end_serialize(const void *style,
                                           lexbor_serialize_cb_f cb, void *ctx);

/* Inset-inline-end. */

LXB_API void *
lxb_css_property_inset_inline_end_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_inset_inline_end_destroy(lxb_css_memory_t *memory,
                                          void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_property_inset_inline_end_serialize(const void *style,
                                            lexbor_serialize_cb_f cb, void *ctx);

/* Text-transform. */

LXB_API void *
lxb_css_property_text_transform_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_text_transform_destroy(lxb_css_memory_t *memory,
                                        void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_text_transform_serialize(const void *style,
                                          lexbor_serialize_cb_f cb, void *ctx);

/* Text-align. */

LXB_API void *
lxb_css_property_text_align_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_text_align_destroy(lxb_css_memory_t *memory,
                                    void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_text_align_serialize(const void *style,
                                      lexbor_serialize_cb_f cb, void *ctx);

/* Text-align-all. */

LXB_API void *
lxb_css_property_text_align_all_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_text_align_all_destroy(lxb_css_memory_t *memory,
                                        void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_text_align_all_serialize(const void *style,
                                          lexbor_serialize_cb_f cb, void *ctx);

/* Text-align-last. */

LXB_API void *
lxb_css_property_text_align_last_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_text_align_last_destroy(lxb_css_memory_t *memory,
                                         void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_text_align_last_serialize(const void *style,
                                           lexbor_serialize_cb_f cb, void *ctx);

/* Text-justify. */

LXB_API void *
lxb_css_property_text_justify_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_text_justify_destroy(lxb_css_memory_t *memory,
                                      void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_text_justify_serialize(const void *style,
                                        lexbor_serialize_cb_f cb, void *ctx);

/* Text-indent. */

LXB_API void *
lxb_css_property_text_indent_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_text_indent_destroy(lxb_css_memory_t *memory,
                                     void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_text_indent_serialize(const void *style,
                                       lexbor_serialize_cb_f cb, void *ctx);

/* White-space. */

LXB_API void *
lxb_css_property_white_space_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_white_space_destroy(lxb_css_memory_t *memory,
                                     void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_white_space_serialize(const void *style,
                                       lexbor_serialize_cb_f cb, void *ctx);

/* Tab-size. */

LXB_API void *
lxb_css_property_tab_size_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_tab_size_destroy(lxb_css_memory_t *memory,
                                  void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_tab_size_serialize(const void *style,
                                    lexbor_serialize_cb_f cb, void *ctx);

/* Word-break. */

LXB_API void *
lxb_css_property_word_break_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_word_break_destroy(lxb_css_memory_t *memory,
                                    void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_word_break_serialize(const void *style,
                                      lexbor_serialize_cb_f cb, void *ctx);

/* Line-break. */

LXB_API void *
lxb_css_property_line_break_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_line_break_destroy(lxb_css_memory_t *memory,
                                    void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_line_break_serialize(const void *style,
                                      lexbor_serialize_cb_f cb, void *ctx);

/* Hyphens. */

LXB_API void *
lxb_css_property_hyphens_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_hyphens_destroy(lxb_css_memory_t *memory,
                                 void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_hyphens_serialize(const void *style,
                                   lexbor_serialize_cb_f cb, void *ctx);

/* Overflow-wrap. */

LXB_API void *
lxb_css_property_overflow_wrap_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_overflow_wrap_destroy(lxb_css_memory_t *memory,
                                       void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_overflow_wrap_serialize(const void *style,
                                         lexbor_serialize_cb_f cb, void *ctx);

/* Word-wrap. */

LXB_API void *
lxb_css_property_word_wrap_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_word_wrap_destroy(lxb_css_memory_t *memory,
                                   void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_word_wrap_serialize(const void *style,
                                     lexbor_serialize_cb_f cb, void *ctx);

/* Word-spacing. */

LXB_API void *
lxb_css_property_word_spacing_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_word_spacing_destroy(lxb_css_memory_t *memory,
                                      void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_word_spacing_serialize(const void *style,
                                        lexbor_serialize_cb_f cb, void *ctx);

/* Letter-spacing. */

LXB_API void *
lxb_css_property_letter_spacing_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_letter_spacing_destroy(lxb_css_memory_t *memory,
                                        void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_letter_spacing_serialize(const void *style,
                                          lexbor_serialize_cb_f cb, void *ctx);

/* Hanging-punctuation. */

LXB_API void *
lxb_css_property_hanging_punctuation_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_hanging_punctuation_destroy(lxb_css_memory_t *memory,
                                             void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_hanging_punctuation_serialize(const void *style,
                                               lexbor_serialize_cb_f cb, void *ctx);

/* Font-family. */

LXB_API void *
lxb_css_property_font_family_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_font_family_destroy(lxb_css_memory_t *memory,
                                     void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_font_family_serialize(const void *style,
                                       lexbor_serialize_cb_f cb, void *ctx);

/* Font-weight. */

LXB_API void *
lxb_css_property_font_weight_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_font_weight_destroy(lxb_css_memory_t *memory,
                                     void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_font_weight_serialize(const void *style,
                                       lexbor_serialize_cb_f cb, void *ctx);

/* Font-stretch. */

LXB_API void *
lxb_css_property_font_stretch_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_font_stretch_destroy(lxb_css_memory_t *memory,
                                      void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_font_stretch_serialize(const void *style,
                                        lexbor_serialize_cb_f cb, void *ctx);

/* Font-style. */

LXB_API void *
lxb_css_property_font_style_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_font_style_destroy(lxb_css_memory_t *memory,
                                    void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_font_style_serialize(const void *style,
                                      lexbor_serialize_cb_f cb, void *ctx);

/* Font-size. */

LXB_API void *
lxb_css_property_font_size_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_font_size_destroy(lxb_css_memory_t *memory,
                                   void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_font_size_serialize(const void *style,
                                     lexbor_serialize_cb_f cb, void *ctx);

/* Float-reference. */

LXB_API void *
lxb_css_property_float_reference_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_float_reference_destroy(lxb_css_memory_t *memory,
                                         void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_float_reference_serialize(const void *style,
                                           lexbor_serialize_cb_f cb, void *ctx);

/* Float. */

LXB_API void *
lxb_css_property_float_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_float_destroy(lxb_css_memory_t *memory,
                               void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_float_serialize(const void *style,
                                 lexbor_serialize_cb_f cb, void *ctx);

/* Clear. */

LXB_API void *
lxb_css_property_clear_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_clear_destroy(lxb_css_memory_t *memory,
                               void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_clear_serialize(const void *style,
                                 lexbor_serialize_cb_f cb, void *ctx);

/* Float-defer. */

LXB_API void *
lxb_css_property_float_defer_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_float_defer_destroy(lxb_css_memory_t *memory,
                                     void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_float_defer_serialize(const void *style,
                                       lexbor_serialize_cb_f cb, void *ctx);

/* Float-offset. */

LXB_API void *
lxb_css_property_float_offset_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_float_offset_destroy(lxb_css_memory_t *memory,
                                      void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_float_offset_serialize(const void *style,
                                        lexbor_serialize_cb_f cb, void *ctx);

/* Wrap-flow. */

LXB_API void *
lxb_css_property_wrap_flow_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_wrap_flow_destroy(lxb_css_memory_t *memory,
                                   void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_wrap_flow_serialize(const void *style,
                                     lexbor_serialize_cb_f cb, void *ctx);

/* Wrap-through. */

LXB_API void *
lxb_css_property_wrap_through_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_wrap_through_destroy(lxb_css_memory_t *memory,
                                      void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_wrap_through_serialize(const void *style,
                                        lexbor_serialize_cb_f cb, void *ctx);

/* Flex-direction. */

LXB_API void *
lxb_css_property_flex_direction_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_flex_direction_destroy(lxb_css_memory_t *memory,
                                        void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_flex_direction_serialize(const void *style,
                                          lexbor_serialize_cb_f cb, void *ctx);

/* Flex-wrap. */

LXB_API void *
lxb_css_property_flex_wrap_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_flex_wrap_destroy(lxb_css_memory_t *memory,
                                   void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_flex_wrap_serialize(const void *style,
                                     lexbor_serialize_cb_f cb, void *ctx);

/* Flex-flow. */

LXB_API void *
lxb_css_property_flex_flow_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_flex_flow_destroy(lxb_css_memory_t *memory,
                                   void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_flex_flow_serialize(const void *style,
                                     lexbor_serialize_cb_f cb, void *ctx);

/* Flex. */

LXB_API void *
lxb_css_property_flex_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_flex_destroy(lxb_css_memory_t *memory,
                              void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_flex_serialize(const void *style,
                                lexbor_serialize_cb_f cb, void *ctx);

/* Flex-grow. */

LXB_API void *
lxb_css_property_flex_grow_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_flex_grow_destroy(lxb_css_memory_t *memory,
                                   void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_flex_grow_serialize(const void *style,
                                     lexbor_serialize_cb_f cb, void *ctx);

/* Flex-shrink. */

LXB_API void *
lxb_css_property_flex_shrink_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_flex_shrink_destroy(lxb_css_memory_t *memory,
                                     void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_flex_shrink_serialize(const void *style,
                                       lexbor_serialize_cb_f cb, void *ctx);

/* Flex-basis. */

LXB_API void *
lxb_css_property_flex_basis_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_flex_basis_destroy(lxb_css_memory_t *memory,
                                    void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_flex_basis_serialize(const void *style,
                                      lexbor_serialize_cb_f cb, void *ctx);

/* Justify-content. */

LXB_API void *
lxb_css_property_justify_content_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_justify_content_destroy(lxb_css_memory_t *memory,
                                         void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_justify_content_serialize(const void *style,
                                           lexbor_serialize_cb_f cb, void *ctx);

/* Align-items. */

LXB_API void *
lxb_css_property_align_items_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_align_items_destroy(lxb_css_memory_t *memory,
                                     void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_align_items_serialize(const void *style,
                                       lexbor_serialize_cb_f cb, void *ctx);

/* Align-self. */

LXB_API void *
lxb_css_property_align_self_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_align_self_destroy(lxb_css_memory_t *memory,
                                    void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_align_self_serialize(const void *style,
                                      lexbor_serialize_cb_f cb, void *ctx);

/* Align-content. */

LXB_API void *
lxb_css_property_align_content_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_align_content_destroy(lxb_css_memory_t *memory,
                                       void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_align_content_serialize(const void *style,
                                         lexbor_serialize_cb_f cb, void *ctx);

/* Dominant-baseline. */

LXB_API void *
lxb_css_property_dominant_baseline_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_dominant_baseline_destroy(lxb_css_memory_t *memory,
                                           void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_dominant_baseline_serialize(const void *style,
                                             lexbor_serialize_cb_f cb, void *ctx);

/* Vertical-align. */

LXB_API void *
lxb_css_property_vertical_align_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_vertical_align_destroy(lxb_css_memory_t *memory,
                                        void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_vertical_align_serialize(const void *style,
                                          lexbor_serialize_cb_f cb, void *ctx);

/* Baseline-source. */

LXB_API void *
lxb_css_property_baseline_source_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_baseline_source_destroy(lxb_css_memory_t *memory,
                                         void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_baseline_source_serialize(const void *style,
                                           lexbor_serialize_cb_f cb, void *ctx);

/* Alignment-baseline. */

LXB_API void *
lxb_css_property_alignment_baseline_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_alignment_baseline_destroy(lxb_css_memory_t *memory,
                                            void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_alignment_baseline_serialize(const void *style,
                                              lexbor_serialize_cb_f cb, void *ctx);

/* Baseline-shift. */

LXB_API void *
lxb_css_property_baseline_shift_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_baseline_shift_destroy(lxb_css_memory_t *memory,
                                        void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_baseline_shift_serialize(const void *style,
                                          lexbor_serialize_cb_f cb, void *ctx);

/* Line-height. */

LXB_API void *
lxb_css_property_line_height_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_line_height_destroy(lxb_css_memory_t *memory,
                                     void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_line_height_serialize(const void *style,
                                       lexbor_serialize_cb_f cb, void *ctx);

/* Z-index. */

LXB_API void *
lxb_css_property_z_index_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_z_index_destroy(lxb_css_memory_t *memory,
                                 void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_z_index_serialize(const void *style,
                                   lexbor_serialize_cb_f cb, void *ctx);

/* Direction. */

LXB_API void *
lxb_css_property_direction_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_direction_destroy(lxb_css_memory_t *memory,
                                   void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_direction_serialize(const void *style,
                                     lexbor_serialize_cb_f cb, void *ctx);

/* Unicode-bidi. */

LXB_API void *
lxb_css_property_unicode_bidi_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_unicode_bidi_destroy(lxb_css_memory_t *memory,
                                      void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_unicode_bidi_serialize(const void *style,
                                        lexbor_serialize_cb_f cb, void *ctx);

/* Writing-mode. */

LXB_API void *
lxb_css_property_writing_mode_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_writing_mode_destroy(lxb_css_memory_t *memory,
                                      void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_writing_mode_serialize(const void *style,
                                        lexbor_serialize_cb_f cb, void *ctx);

/* Text-orientation. */

LXB_API void *
lxb_css_property_text_orientation_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_text_orientation_destroy(lxb_css_memory_t *memory,
                                          void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_text_orientation_serialize(const void *style,
                                            lexbor_serialize_cb_f cb, void *ctx);

/* Text-combine-upright. */

LXB_API void *
lxb_css_property_text_combine_upright_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_text_combine_upright_destroy(lxb_css_memory_t *memory,
                                              void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_text_combine_upright_serialize(const void *style,
                                                lexbor_serialize_cb_f cb, void *ctx);

/* Overflow-x. */

LXB_API void *
lxb_css_property_overflow_x_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_overflow_x_destroy(lxb_css_memory_t *memory,
                                    void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_overflow_x_serialize(const void *style,
                                      lexbor_serialize_cb_f cb, void *ctx);

/* Overflow-y. */

LXB_API void *
lxb_css_property_overflow_y_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_overflow_y_destroy(lxb_css_memory_t *memory,
                                    void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_overflow_y_serialize(const void *style,
                                      lexbor_serialize_cb_f cb, void *ctx);

/* Overflow-block. */

LXB_API void *
lxb_css_property_overflow_block_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_overflow_block_destroy(lxb_css_memory_t *memory,
                                        void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_overflow_block_serialize(const void *style,
                                          lexbor_serialize_cb_f cb, void *ctx);

/* Overflow-inline. */

LXB_API void *
lxb_css_property_overflow_inline_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_overflow_inline_destroy(lxb_css_memory_t *memory,
                                         void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_overflow_inline_serialize(const void *style,
                                           lexbor_serialize_cb_f cb, void *ctx);

/* Text-overflow. */

LXB_API void *
lxb_css_property_text_overflow_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_text_overflow_destroy(lxb_css_memory_t *memory,
                                       void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_text_overflow_serialize(const void *style,
                                         lexbor_serialize_cb_f cb, void *ctx);

/* Text-decoration-line. */

LXB_API void *
lxb_css_property_text_decoration_line_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_text_decoration_line_destroy(lxb_css_memory_t *memory,
                                              void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_text_decoration_line_serialize(const void *style,
                                                lexbor_serialize_cb_f cb, void *ctx);

/* Text-decoration-style. */

LXB_API void *
lxb_css_property_text_decoration_style_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_text_decoration_style_destroy(lxb_css_memory_t *memory,
                                               void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_text_decoration_style_serialize(const void *style,
                                                 lexbor_serialize_cb_f cb, void *ctx);

/* Text-decoration-color. */

LXB_API void *
lxb_css_property_text_decoration_color_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_text_decoration_color_destroy(lxb_css_memory_t *memory,
                                               void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_text_decoration_color_serialize(const void *style,
                                                 lexbor_serialize_cb_f cb, void *ctx);

/* Text-decoration. */

LXB_API void *
lxb_css_property_text_decoration_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_property_text_decoration_destroy(lxb_css_memory_t *memory,
                                         void *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_property_text_decoration_serialize(const void *style,
                                           lexbor_serialize_cb_f cb, void *ctx);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LXB_CSS_PROPERTY_H */
