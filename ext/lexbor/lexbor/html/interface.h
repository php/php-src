/*
 * Copyright (C) 2018-2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_HTML_INTERFACES_H
#define LEXBOR_HTML_INTERFACES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/html/base.h"
#include "lexbor/tag/const.h"
#include "lexbor/ns/const.h"
#include "lexbor/dom/interface.h"


#define lxb_html_interface_document(obj) ((lxb_html_document_t *) (obj))
#define lxb_html_interface_anchor(obj) ((lxb_html_anchor_element_t *) (obj))
#define lxb_html_interface_area(obj) ((lxb_html_area_element_t *) (obj))
#define lxb_html_interface_audio(obj) ((lxb_html_audio_element_t *) (obj))
#define lxb_html_interface_br(obj) ((lxb_html_br_element_t *) (obj))
#define lxb_html_interface_base(obj) ((lxb_html_base_element_t *) (obj))
#define lxb_html_interface_body(obj) ((lxb_html_body_element_t *) (obj))
#define lxb_html_interface_button(obj) ((lxb_html_button_element_t *) (obj))
#define lxb_html_interface_canvas(obj) ((lxb_html_canvas_element_t *) (obj))
#define lxb_html_interface_d_list(obj) ((lxb_html_d_list_element_t *) (obj))
#define lxb_html_interface_data(obj) ((lxb_html_data_element_t *) (obj))
#define lxb_html_interface_data_list(obj) ((lxb_html_data_list_element_t *) (obj))
#define lxb_html_interface_details(obj) ((lxb_html_details_element_t *) (obj))
#define lxb_html_interface_dialog(obj) ((lxb_html_dialog_element_t *) (obj))
#define lxb_html_interface_directory(obj) ((lxb_html_directory_element_t *) (obj))
#define lxb_html_interface_div(obj) ((lxb_html_div_element_t *) (obj))
#define lxb_html_interface_element(obj) ((lxb_html_element_t *) (obj))
#define lxb_html_interface_embed(obj) ((lxb_html_embed_element_t *) (obj))
#define lxb_html_interface_field_set(obj) ((lxb_html_field_set_element_t *) (obj))
#define lxb_html_interface_font(obj) ((lxb_html_font_element_t *) (obj))
#define lxb_html_interface_form(obj) ((lxb_html_form_element_t *) (obj))
#define lxb_html_interface_frame(obj) ((lxb_html_frame_element_t *) (obj))
#define lxb_html_interface_frame_set(obj) ((lxb_html_frame_set_element_t *) (obj))
#define lxb_html_interface_hr(obj) ((lxb_html_hr_element_t *) (obj))
#define lxb_html_interface_head(obj) ((lxb_html_head_element_t *) (obj))
#define lxb_html_interface_heading(obj) ((lxb_html_heading_element_t *) (obj))
#define lxb_html_interface_html(obj) ((lxb_html_html_element_t *) (obj))
#define lxb_html_interface_iframe(obj) ((lxb_html_iframe_element_t *) (obj))
#define lxb_html_interface_image(obj) ((lxb_html_image_element_t *) (obj))
#define lxb_html_interface_input(obj) ((lxb_html_input_element_t *) (obj))
#define lxb_html_interface_li(obj) ((lxb_html_li_element_t *) (obj))
#define lxb_html_interface_label(obj) ((lxb_html_label_element_t *) (obj))
#define lxb_html_interface_legend(obj) ((lxb_html_legend_element_t *) (obj))
#define lxb_html_interface_link(obj) ((lxb_html_link_element_t *) (obj))
#define lxb_html_interface_map(obj) ((lxb_html_map_element_t *) (obj))
#define lxb_html_interface_marquee(obj) ((lxb_html_marquee_element_t *) (obj))
#define lxb_html_interface_media(obj) ((lxb_html_media_element_t *) (obj))
#define lxb_html_interface_menu(obj) ((lxb_html_menu_element_t *) (obj))
#define lxb_html_interface_meta(obj) ((lxb_html_meta_element_t *) (obj))
#define lxb_html_interface_meter(obj) ((lxb_html_meter_element_t *) (obj))
#define lxb_html_interface_mod(obj) ((lxb_html_mod_element_t *) (obj))
#define lxb_html_interface_o_list(obj) ((lxb_html_o_list_element_t *) (obj))
#define lxb_html_interface_object(obj) ((lxb_html_object_element_t *) (obj))
#define lxb_html_interface_opt_group(obj) ((lxb_html_opt_group_element_t *) (obj))
#define lxb_html_interface_option(obj) ((lxb_html_option_element_t *) (obj))
#define lxb_html_interface_output(obj) ((lxb_html_output_element_t *) (obj))
#define lxb_html_interface_paragraph(obj) ((lxb_html_paragraph_element_t *) (obj))
#define lxb_html_interface_param(obj) ((lxb_html_param_element_t *) (obj))
#define lxb_html_interface_picture(obj) ((lxb_html_picture_element_t *) (obj))
#define lxb_html_interface_pre(obj) ((lxb_html_pre_element_t *) (obj))
#define lxb_html_interface_progress(obj) ((lxb_html_progress_element_t *) (obj))
#define lxb_html_interface_quote(obj) ((lxb_html_quote_element_t *) (obj))
#define lxb_html_interface_script(obj) ((lxb_html_script_element_t *) (obj))
#define lxb_html_interface_select(obj) ((lxb_html_select_element_t *) (obj))
#define lxb_html_interface_slot(obj) ((lxb_html_slot_element_t *) (obj))
#define lxb_html_interface_source(obj) ((lxb_html_source_element_t *) (obj))
#define lxb_html_interface_span(obj) ((lxb_html_span_element_t *) (obj))
#define lxb_html_interface_style(obj) ((lxb_html_style_element_t *) (obj))
#define lxb_html_interface_table_caption(obj) ((lxb_html_table_caption_element_t *) (obj))
#define lxb_html_interface_table_cell(obj) ((lxb_html_table_cell_element_t *) (obj))
#define lxb_html_interface_table_col(obj) ((lxb_html_table_col_element_t *) (obj))
#define lxb_html_interface_table(obj) ((lxb_html_table_element_t *) (obj))
#define lxb_html_interface_table_row(obj) ((lxb_html_table_row_element_t *) (obj))
#define lxb_html_interface_table_section(obj) ((lxb_html_table_section_element_t *) (obj))
#define lxb_html_interface_template(obj) ((lxb_html_template_element_t *) (obj))
#define lxb_html_interface_text_area(obj) ((lxb_html_text_area_element_t *) (obj))
#define lxb_html_interface_time(obj) ((lxb_html_time_element_t *) (obj))
#define lxb_html_interface_title(obj) ((lxb_html_title_element_t *) (obj))
#define lxb_html_interface_track(obj) ((lxb_html_track_element_t *) (obj))
#define lxb_html_interface_u_list(obj) ((lxb_html_u_list_element_t *) (obj))
#define lxb_html_interface_unknown(obj) ((lxb_html_unknown_element_t *) (obj))
#define lxb_html_interface_video(obj) ((lxb_html_video_element_t *) (obj))
#define lxb_html_interface_window(obj) ((lxb_html_window_t *) (obj))


typedef struct lxb_html_document lxb_html_document_t;
typedef struct lxb_html_anchor_element lxb_html_anchor_element_t;
typedef struct lxb_html_area_element lxb_html_area_element_t;
typedef struct lxb_html_audio_element lxb_html_audio_element_t;
typedef struct lxb_html_br_element lxb_html_br_element_t;
typedef struct lxb_html_base_element lxb_html_base_element_t;
typedef struct lxb_html_body_element lxb_html_body_element_t;
typedef struct lxb_html_button_element lxb_html_button_element_t;
typedef struct lxb_html_canvas_element lxb_html_canvas_element_t;
typedef struct lxb_html_d_list_element lxb_html_d_list_element_t;
typedef struct lxb_html_data_element lxb_html_data_element_t;
typedef struct lxb_html_data_list_element lxb_html_data_list_element_t;
typedef struct lxb_html_details_element lxb_html_details_element_t;
typedef struct lxb_html_dialog_element lxb_html_dialog_element_t;
typedef struct lxb_html_directory_element lxb_html_directory_element_t;
typedef struct lxb_html_div_element lxb_html_div_element_t;
typedef struct lxb_html_element lxb_html_element_t;
typedef struct lxb_html_embed_element lxb_html_embed_element_t;
typedef struct lxb_html_field_set_element lxb_html_field_set_element_t;
typedef struct lxb_html_font_element lxb_html_font_element_t;
typedef struct lxb_html_form_element lxb_html_form_element_t;
typedef struct lxb_html_frame_element lxb_html_frame_element_t;
typedef struct lxb_html_frame_set_element lxb_html_frame_set_element_t;
typedef struct lxb_html_hr_element lxb_html_hr_element_t;
typedef struct lxb_html_head_element lxb_html_head_element_t;
typedef struct lxb_html_heading_element lxb_html_heading_element_t;
typedef struct lxb_html_html_element lxb_html_html_element_t;
typedef struct lxb_html_iframe_element lxb_html_iframe_element_t;
typedef struct lxb_html_image_element lxb_html_image_element_t;
typedef struct lxb_html_input_element lxb_html_input_element_t;
typedef struct lxb_html_li_element lxb_html_li_element_t;
typedef struct lxb_html_label_element lxb_html_label_element_t;
typedef struct lxb_html_legend_element lxb_html_legend_element_t;
typedef struct lxb_html_link_element lxb_html_link_element_t;
typedef struct lxb_html_map_element lxb_html_map_element_t;
typedef struct lxb_html_marquee_element lxb_html_marquee_element_t;
typedef struct lxb_html_media_element lxb_html_media_element_t;
typedef struct lxb_html_menu_element lxb_html_menu_element_t;
typedef struct lxb_html_meta_element lxb_html_meta_element_t;
typedef struct lxb_html_meter_element lxb_html_meter_element_t;
typedef struct lxb_html_mod_element lxb_html_mod_element_t;
typedef struct lxb_html_o_list_element lxb_html_o_list_element_t;
typedef struct lxb_html_object_element lxb_html_object_element_t;
typedef struct lxb_html_opt_group_element lxb_html_opt_group_element_t;
typedef struct lxb_html_option_element lxb_html_option_element_t;
typedef struct lxb_html_output_element lxb_html_output_element_t;
typedef struct lxb_html_paragraph_element lxb_html_paragraph_element_t;
typedef struct lxb_html_param_element lxb_html_param_element_t;
typedef struct lxb_html_picture_element lxb_html_picture_element_t;
typedef struct lxb_html_pre_element lxb_html_pre_element_t;
typedef struct lxb_html_progress_element lxb_html_progress_element_t;
typedef struct lxb_html_quote_element lxb_html_quote_element_t;
typedef struct lxb_html_script_element lxb_html_script_element_t;
typedef struct lxb_html_select_element lxb_html_select_element_t;
typedef struct lxb_html_slot_element lxb_html_slot_element_t;
typedef struct lxb_html_source_element lxb_html_source_element_t;
typedef struct lxb_html_span_element lxb_html_span_element_t;
typedef struct lxb_html_style_element lxb_html_style_element_t;
typedef struct lxb_html_table_caption_element lxb_html_table_caption_element_t;
typedef struct lxb_html_table_cell_element lxb_html_table_cell_element_t;
typedef struct lxb_html_table_col_element lxb_html_table_col_element_t;
typedef struct lxb_html_table_element lxb_html_table_element_t;
typedef struct lxb_html_table_row_element lxb_html_table_row_element_t;
typedef struct lxb_html_table_section_element lxb_html_table_section_element_t;
typedef struct lxb_html_template_element lxb_html_template_element_t;
typedef struct lxb_html_text_area_element lxb_html_text_area_element_t;
typedef struct lxb_html_time_element lxb_html_time_element_t;
typedef struct lxb_html_title_element lxb_html_title_element_t;
typedef struct lxb_html_track_element lxb_html_track_element_t;
typedef struct lxb_html_u_list_element lxb_html_u_list_element_t;
typedef struct lxb_html_unknown_element lxb_html_unknown_element_t;
typedef struct lxb_html_video_element lxb_html_video_element_t;
typedef struct lxb_html_window lxb_html_window_t;


LXB_API lxb_dom_interface_t *
lxb_html_interface_create(lxb_html_document_t *document, lxb_tag_id_t tag_id,
                          lxb_ns_id_t ns);

LXB_API lxb_dom_interface_t *
lxb_html_interface_clone(lxb_dom_document_t *document,
                         const lxb_dom_interface_t *intrfc);

LXB_API lxb_dom_interface_t *
lxb_html_interface_destroy(lxb_dom_interface_t *intrfc);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_HTML_INTERFACES_H */
