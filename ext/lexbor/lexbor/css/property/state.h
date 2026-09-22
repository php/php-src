/*
 * Copyright (C) 2021-2023 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LXB_CSS_PROPERTY_STATE_H
#define LXB_CSS_PROPERTY_STATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/css/base.h"


LXB_API bool
lxb_css_property_state__undef(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state__custom(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_display(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_order(lxb_css_parser_t *parser,
                             const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_visibility(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_width(lxb_css_parser_t *parser,
                             const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_height(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_box_sizing(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_min_width(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_min_height(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_max_width(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_max_height(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_margin(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_margin_top(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_margin_right(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_margin_bottom(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_margin_left(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_padding(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_padding_top(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_padding_right(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_padding_bottom(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_padding_left(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_border(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_border_top(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_border_right(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_border_bottom(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_border_left(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_border_top_color(lxb_css_parser_t *parser,
                                        const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_border_right_color(lxb_css_parser_t *parser,
                                          const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_border_bottom_color(lxb_css_parser_t *parser,
                                           const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_border_left_color(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_background_color(lxb_css_parser_t *parser,
                                        const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_color(lxb_css_parser_t *parser,
                             const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_opacity(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_position(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_top(lxb_css_parser_t *parser,
                           const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_right(lxb_css_parser_t *parser,
                             const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_bottom(lxb_css_parser_t *parser,
                              const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_left(lxb_css_parser_t *parser,
                            const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_inset_block_start(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_inset_inline_start(lxb_css_parser_t *parser,
                                          const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_inset_block_end(lxb_css_parser_t *parser,
                                       const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_inset_inline_end(lxb_css_parser_t *parser,
                                        const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_text_transform(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_text_align(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_text_align_all(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_text_align_last(lxb_css_parser_t *parser,
                                       const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_text_justify(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_text_indent(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_white_space(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_tab_size(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_word_break(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_line_break(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_hyphens(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_overflow_wrap(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_word_wrap(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_word_spacing(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_letter_spacing(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_hanging_punctuation(lxb_css_parser_t *parser,
                                           const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_font_family(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_font_weight(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_font_stretch(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_font_style(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_font_size(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_float_reference(lxb_css_parser_t *parser,
                                       const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_float(lxb_css_parser_t *parser,
                             const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_clear(lxb_css_parser_t *parser,
                             const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_float_defer(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_float_offset(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_wrap_flow(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_wrap_through(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_flex_direction(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_flex_wrap(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_flex_flow(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_flex(lxb_css_parser_t *parser,
                            const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_flex_grow(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_flex_shrink(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_flex_basis(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_justify_content(lxb_css_parser_t *parser,
                                       const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_align_items(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_align_self(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_align_content(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_z_index(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_direction(lxb_css_parser_t *parser,
                                 const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_unicode_bidi(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_writing_mode(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_text_orientation(lxb_css_parser_t *parser,
                                        const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_text_combine_upright(lxb_css_parser_t *parser,
                                            const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_overflow_x(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_overflow_y(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_overflow_block(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_overflow_inline(lxb_css_parser_t *parser,
                                       const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_text_overflow(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_text_decoration_line(lxb_css_parser_t *parser,
                                            const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_text_decoration_style(lxb_css_parser_t *parser,
                                             const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_text_decoration_color(lxb_css_parser_t *parser,
                                             const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_text_decoration(lxb_css_parser_t *parser,
                                       const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_dominant_baseline(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_vertical_align(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_baseline_source(lxb_css_parser_t *parser,
                                       const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_alignment_baseline(lxb_css_parser_t *parser,
                                          const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_baseline_shift(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token, void *ctx);
LXB_API bool
lxb_css_property_state_line_height(lxb_css_parser_t *parser,
                                   const lxb_css_syntax_token_t *token, void *ctx);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LXB_CSS_PROPERTY_STATE_H */
