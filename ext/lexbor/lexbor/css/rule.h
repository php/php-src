/*
 * Copyright (C) 2021-2023 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LXB_CSS_RULE_H
#define LXB_CSS_RULE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/css/base.h"
#include "lexbor/css/at_rule.h"
#include "lexbor/css/property.h"
#include "lexbor/css/selectors/selectors.h"


#define lxb_css_rule(rule)                  ((lxb_css_rule_t *) (rule))
#define lxb_css_rule_list(rule)             ((lxb_css_rule_list_t *) (rule))
#define lxb_css_rule_at(rule)               ((lxb_css_rule_at_t *) (rule))
#define lxb_css_rule_style(rule)            ((lxb_css_rule_style_t *) (rule))
#define lxb_css_rule_bad_style(rule)        ((lxb_css_rule_bad_style_t *) (rule))
#define lxb_css_rule_declaration_list(rule) ((lxb_css_rule_declaration_list_t *) (rule))
#define lxb_css_rule_declaration(rule)      ((lxb_css_rule_declaration_t *) (rule))


typedef enum {
    LXB_CSS_RULE_UNDEF = 0,
    LXB_CSS_RULE_STYLESHEET,
    LXB_CSS_RULE_LIST,
    LXB_CSS_RULE_AT_RULE,
    LXB_CSS_RULE_STYLE,
    LXB_CSS_RULE_BAD_STYLE,
    LXB_CSS_RULE_DECLARATION_LIST,
    LXB_CSS_RULE_DECLARATION
}
lxb_css_rule_type_t;

typedef struct lxb_css_rule lxb_css_rule_t;

struct lxb_css_rule {
    lxb_css_rule_type_t type;
    lxb_css_rule_t      *next;
    lxb_css_rule_t      *prev;
    lxb_css_rule_t      *parent;

    const lxb_char_t    *begin;
    const lxb_char_t    *end;

    lxb_css_memory_t    *memory;
    size_t              ref_count;
};

struct lxb_css_rule_list {
    lxb_css_rule_t rule;

    lxb_css_rule_t *first;
    lxb_css_rule_t *last;
};

struct lxb_css_rule_at {
    lxb_css_rule_t rule;

    uintptr_t      type;

    union {
        lxb_css_at_rule__undef_t    *undef;
        lxb_css_at_rule__custom_t   *custom;
        lxb_css_at_rule_media_t     *media;
        lxb_css_at_rule_namespace_t *ns;
        void                        *user;
    } u;
};

struct lxb_css_rule_style {
    lxb_css_rule_t                  rule;

    lxb_css_selector_list_t         *selector;
    lxb_css_rule_declaration_list_t *declarations;
};

struct lxb_css_rule_bad_style {
    lxb_css_rule_t                  rule;

    lexbor_str_t                    selectors;
    lxb_css_rule_declaration_list_t *declarations;
};

struct lxb_css_rule_declaration_list {
    lxb_css_rule_t rule;

    lxb_css_rule_t *first;
    lxb_css_rule_t *last;

    size_t         count;
};

struct lxb_css_rule_declaration {
    lxb_css_rule_t rule;
    uintptr_t      type;

    union {
        lxb_css_property__undef_t                *undef;
        lxb_css_property__custom_t               *custom;
        lxb_css_property_display_t               *display;
        lxb_css_property_order_t                 *order;
        lxb_css_property_visibility_t            *visibility;
        lxb_css_property_width_t                 *width;
        lxb_css_property_height_t                *height;
        lxb_css_property_box_sizing_t            *box_sizing;
        lxb_css_property_margin_t                *margin;
        lxb_css_property_margin_top_t            *margin_top;
        lxb_css_property_margin_right_t          *margin_right;
        lxb_css_property_margin_bottom_t         *margin_bottom;
        lxb_css_property_margin_left_t           *margin_left;
        lxb_css_property_padding_t               *padding;
        lxb_css_property_padding_top_t           *padding_top;
        lxb_css_property_padding_right_t         *padding_right;
        lxb_css_property_padding_bottom_t        *padding_bottom;
        lxb_css_property_padding_left_t          *padding_left;
        lxb_css_property_border_t                *border;
        lxb_css_property_border_top_t            *border_top;
        lxb_css_property_border_right_t          *border_right;
        lxb_css_property_border_bottom_t         *border_bottom;
        lxb_css_property_border_left_t           *border_left;
        lxb_css_property_border_top_color_t      *border_top_color;
        lxb_css_property_border_right_color_t    *border_right_color;
        lxb_css_property_border_bottom_color_t   *border_bottom_color;
        lxb_css_property_border_left_color_t     *border_left_color;
        lxb_css_property_background_color_t      *background_color;
        lxb_css_property_color_t                 *color;
        lxb_css_property_opacity_t               *opacity;
        lxb_css_property_position_t              *position;
        lxb_css_property_top_t                   *top;
        lxb_css_property_right_t                 *right;
        lxb_css_property_bottom_t                *bottom;
        lxb_css_property_left_t                  *left;
        lxb_css_property_inset_block_start_t     *inset_block_start;
        lxb_css_property_inset_inline_start_t    *inset_inline_start;
        lxb_css_property_inset_block_end_t       *inset_block_end;
        lxb_css_property_inset_inline_end_t      *inset_inline_end;
        lxb_css_property_text_transform_t        *text_transform;
        lxb_css_property_text_align_t            *text_align;
        lxb_css_property_text_align_all_t        *text_align_all;
        lxb_css_property_text_align_last_t       *text_align_last;
        lxb_css_property_text_justify_t          *text_justify;
        lxb_css_property_text_indent_t           *text_indent;
        lxb_css_property_white_space_t           *white_space;
        lxb_css_property_tab_size_t              *tab_size;
        lxb_css_property_word_break_t            *word_break;
        lxb_css_property_line_break_t            *line_break;
        lxb_css_property_hyphens_t               *hyphens;
        lxb_css_property_overflow_wrap_t         *overflow_wrap;
        lxb_css_property_word_wrap_t             *word_wrap;
        lxb_css_property_word_spacing_t          *word_spacing;
        lxb_css_property_letter_spacing_t        *letter_spacing;
        lxb_css_property_hanging_punctuation_t   *hanging_punctuation;
        lxb_css_property_font_family_t           *font_family;
        lxb_css_property_font_weight_t           *font_weight;
        lxb_css_property_font_stretch_t          *font_stretch;
        lxb_css_property_font_style_t            *font_style;
        lxb_css_property_font_size_t             *font_size;
        lxb_css_property_float_reference_t       *float_reference;
        lxb_css_property_float_t                 *floatp;
        lxb_css_property_clear_t                 *clear;
        lxb_css_property_float_defer_t           *float_defer;
        lxb_css_property_float_offset_t          *float_offset;
        lxb_css_property_wrap_flow_t             *wrap_flow;
        lxb_css_property_wrap_through_t          *wrap_through;
        lxb_css_property_flex_direction_t        *flex_direction;
        lxb_css_property_flex_wrap_t             *flex_wrap;
        lxb_css_property_flex_flow_t             *flex_flow;
        lxb_css_property_flex_t                  *flex;
        lxb_css_property_flex_grow_t             *flex_grow;
        lxb_css_property_flex_shrink_t           *flex_shrink;
        lxb_css_property_flex_basis_t            *flex_basis;
        lxb_css_property_justify_content_t       *justify_content;
        lxb_css_property_align_items_t           *align_items;
        lxb_css_property_align_self_t            *align_self;
        lxb_css_property_align_content_t         *align_content;
        lxb_css_property_dominant_baseline_t     *dominant_baseline;
        lxb_css_property_vertical_align_t        *vertical_align;
        lxb_css_property_baseline_source_t       *baseline_source;
        lxb_css_property_alignment_baseline_t    *alignment_baseline;
        lxb_css_property_baseline_shift_t        *baseline_shift;
        lxb_css_property_line_height_t           *line_height;
        lxb_css_property_z_index_t               *z_index;
        lxb_css_property_direction_t             *direction;
        lxb_css_property_unicode_bidi_t          *unicode_bidi;
        lxb_css_property_writing_mode_t          *writing_mode;
        lxb_css_property_text_orientation_t      *text_orientation;
        lxb_css_property_text_combine_upright_t  *text_combine_upright;
        lxb_css_property_overflow_x_t            *overflow_x;
        lxb_css_property_overflow_y_t            *overflow_y;
        lxb_css_property_overflow_block_t        *overflow_block;
        lxb_css_property_overflow_inline_t       *overflow_inline;
        lxb_css_property_text_overflow_t         *text_overflow;
        lxb_css_property_text_decoration_line_t  *text_decoration_line;
        lxb_css_property_text_decoration_style_t *text_decoration_style;
        lxb_css_property_text_decoration_color_t *text_decoration_color;
        lxb_css_property_text_decoration_t       *text_decoration;
        void                                     *user;
    } u;

    bool            important;
};


LXB_API void *
lxb_css_rule_destroy(lxb_css_rule_t *node, bool self_destroy);

LXB_API lxb_status_t
lxb_css_rule_serialize(const lxb_css_rule_t *rule,
                       lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_css_rule_serialize_chain(const lxb_css_rule_t *rule,
                             lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_css_rule_list_t *
lxb_css_rule_list_destroy(lxb_css_rule_list_t *list, bool self_destroy);

LXB_API lxb_status_t
lxb_css_rule_list_serialize(const lxb_css_rule_list_t *list,
                            lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_css_rule_at_t *
lxb_css_rule_at_destroy(lxb_css_rule_at_t *at, bool self_destroy);

LXB_API lxb_status_t
lxb_css_rule_at_serialize(const lxb_css_rule_at_t *at, lexbor_serialize_cb_f cb,
                          void *ctx);

LXB_API lxb_status_t
lxb_css_rule_at_serialize_name(const lxb_css_rule_at_t *at, lexbor_serialize_cb_f cb,
                               void *ctx);

LXB_API lxb_css_rule_style_t *
lxb_css_rule_style_destroy(lxb_css_rule_style_t *style, bool self_destroy);

LXB_API lxb_status_t
lxb_css_rule_style_serialize(const lxb_css_rule_style_t *style,
                             lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_css_rule_bad_style_t *
lxb_css_rule_bad_style_destroy(lxb_css_rule_bad_style_t *bad, bool self_destroy);

LXB_API lxb_status_t
lxb_css_rule_bad_style_serialize(const lxb_css_rule_bad_style_t *bad,
                                 lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_css_rule_declaration_list_t *
lxb_css_rule_declaration_list_destroy(lxb_css_rule_declaration_list_t *list,
                                      bool self_destroy);

LXB_API lxb_status_t
lxb_css_rule_declaration_list_serialize(const lxb_css_rule_declaration_list_t *list,
                                        lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_css_rule_declaration_t *
lxb_css_rule_declaration_destroy(lxb_css_rule_declaration_t *declr,
                                 bool self_destroy);

LXB_API lxb_status_t
lxb_css_rule_declaration_serialize(const lxb_css_rule_declaration_t *declaration,
                                   lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_css_rule_declaration_serialize_name(const lxb_css_rule_declaration_t *declr,
                                        lexbor_serialize_cb_f cb, void *ctx);

/*
 * Inline functions
 */
lxb_inline void
lxb_css_rule_insert_next(lxb_css_rule_t *root, lxb_css_rule_t *rule)
{
    if (root->next != NULL) {
        rule->next = root->next;
    }

    root->next = rule;
    rule->prev = root;
}

lxb_inline lxb_css_rule_t *
lxb_css_rule_create(lxb_css_memory_t *memory, size_t size,
                    lxb_css_rule_type_t type)
{
    lxb_css_rule_t *rule;

    rule = (lxb_css_rule_t *) lexbor_mraw_calloc(memory->tree, size);
    if (rule== NULL) {
        return NULL;
    }

    rule->type = type;
    rule->memory = memory;

    return rule;
}

lxb_inline lxb_css_rule_list_t *
lxb_css_rule_list_create(lxb_css_memory_t *memory)
{
    return (lxb_css_rule_list_t *) lxb_css_rule_create(memory,
                                                       sizeof(lxb_css_rule_list_t),
                                                       LXB_CSS_RULE_LIST);
}

lxb_inline size_t
lxb_css_rule_ref_count(lxb_css_rule_t *rule)
{
    return rule->ref_count;
}

lxb_inline lxb_status_t
lxb_css_rule_ref_inc(lxb_css_rule_t *rule)
{
    if (SIZE_MAX - rule->ref_count == 0) {
        return LXB_STATUS_ERROR_OVERFLOW;
    }

    rule->ref_count++;

    return LXB_STATUS_OK;
}

lxb_inline void
lxb_css_rule_ref_dec(lxb_css_rule_t *rule)
{
    if (rule->ref_count > 0) {
        rule->ref_count--;
    }
}

lxb_inline void
lxb_css_rule_ref_dec_destroy(lxb_css_rule_t *rule)
{
#if 0
    if (rule->ref_count > 0) {
        rule->ref_count--;
    }

    if (rule->ref_count == 0) {
        (void) lxb_css_rule_destroy(rule, true);
    }
#endif
}

lxb_inline void
lxb_css_rule_list_append(lxb_css_rule_list_t *list, lxb_css_rule_t *rule)
{
    if (list->first == NULL) {
        list->first = rule;
    }
    else {
        lxb_css_rule_insert_next(list->last, rule);
    }

    list->last = rule;
    rule->parent = lxb_css_rule(list);

    (void) lxb_css_rule_ref_inc(rule);
}

lxb_inline lxb_css_rule_at_t *
lxb_css_rule_at_create(lxb_css_memory_t *memory)
{
    return (lxb_css_rule_at_t *)
        lxb_css_rule_create(memory, sizeof(lxb_css_rule_at_t),
                            LXB_CSS_RULE_AT_RULE);
}

lxb_inline lxb_css_rule_style_t *
lxb_css_rule_style_create(lxb_css_memory_t *memory)
{
    return (lxb_css_rule_style_t *)
        lxb_css_rule_create(memory, sizeof(lxb_css_rule_style_t),
                            LXB_CSS_RULE_STYLE);
}

lxb_inline lxb_css_rule_bad_style_t *
lxb_css_rule_bad_style_create(lxb_css_memory_t *memory)
{
    return (lxb_css_rule_bad_style_t *)
        lxb_css_rule_create(memory, sizeof(lxb_css_rule_bad_style_t),
                            LXB_CSS_RULE_BAD_STYLE);
}

lxb_inline lxb_css_rule_declaration_list_t *
lxb_css_rule_declaration_list_create(lxb_css_memory_t *memory)
{
    return (lxb_css_rule_declaration_list_t *)
        lxb_css_rule_create(memory, sizeof(lxb_css_rule_declaration_list_t),
                            LXB_CSS_RULE_DECLARATION_LIST);
}

lxb_inline void
lxb_css_rule_declaration_list_append(lxb_css_rule_declaration_list_t *list,
                                     lxb_css_rule_t *rule)
{
    if (list->first == NULL) {
        list->first = rule;
    }
    else {
        lxb_css_rule_insert_next(list->last, rule);
    }

    list->last = rule;
    rule->parent = lxb_css_rule(list);

    (void) lxb_css_rule_ref_inc(rule);

    list->count++;
}

lxb_inline lxb_css_rule_declaration_t *
lxb_css_rule_declaration_create(lxb_css_memory_t *memory)
{
    return (lxb_css_rule_declaration_t *)
        lxb_css_rule_create(memory, sizeof(lxb_css_rule_declaration_t),
                            LXB_CSS_RULE_DECLARATION);
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LXB_CSS_RULE_H */
