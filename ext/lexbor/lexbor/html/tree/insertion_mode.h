/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_HTML_TREE_INSERTION_MODE_H
#define LEXBOR_HTML_TREE_INSERTION_MODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/html/token.h"
#include "lexbor/html/tree.h"


LXB_API bool
lxb_html_tree_insertion_mode_initial(lxb_html_tree_t *tree,
                                     lxb_html_token_t *token);

LXB_API bool
lxb_html_tree_insertion_mode_before_html(lxb_html_tree_t *tree,
                                         lxb_html_token_t *token);

LXB_API bool
lxb_html_tree_insertion_mode_before_head(lxb_html_tree_t *tree,
                                         lxb_html_token_t *token);

LXB_API bool
lxb_html_tree_insertion_mode_in_head(lxb_html_tree_t *tree,
                                     lxb_html_token_t *token);

LXB_API bool
lxb_html_tree_insertion_mode_in_head_noscript(lxb_html_tree_t *tree,
                                              lxb_html_token_t *token);

LXB_API bool
lxb_html_tree_insertion_mode_after_head(lxb_html_tree_t *tree,
                                        lxb_html_token_t *token);

LXB_API bool
lxb_html_tree_insertion_mode_in_body(lxb_html_tree_t *tree,
                                     lxb_html_token_t *token);

LXB_API bool
lxb_html_tree_insertion_mode_in_body_skip_new_line(lxb_html_tree_t *tree,
                                                   lxb_html_token_t *token);

LXB_API bool
lxb_html_tree_insertion_mode_in_body_skip_new_line_textarea(lxb_html_tree_t *tree,
                                                            lxb_html_token_t *token);

LXB_API lxb_status_t
lxb_html_tree_insertion_mode_in_body_text_append(lxb_html_tree_t *tree,
                                                 lexbor_str_t *str);

LXB_API bool
lxb_html_tree_insertion_mode_text(lxb_html_tree_t *tree,
                                  lxb_html_token_t *token);

LXB_API bool
lxb_html_tree_insertion_mode_in_table(lxb_html_tree_t *tree,
                                      lxb_html_token_t *token);

LXB_API bool
lxb_html_tree_insertion_mode_in_table_anything_else(lxb_html_tree_t *tree,
                                                    lxb_html_token_t *token);

LXB_API bool
lxb_html_tree_insertion_mode_in_table_text(lxb_html_tree_t *tree,
                                           lxb_html_token_t *token);

LXB_API bool
lxb_html_tree_insertion_mode_in_caption(lxb_html_tree_t *tree,
                                        lxb_html_token_t *token);

LXB_API bool
lxb_html_tree_insertion_mode_in_column_group(lxb_html_tree_t *tree,
                                             lxb_html_token_t *token);

LXB_API bool
lxb_html_tree_insertion_mode_in_table_body(lxb_html_tree_t *tree,
                                           lxb_html_token_t *token);

LXB_API bool
lxb_html_tree_insertion_mode_in_row(lxb_html_tree_t *tree,
                                    lxb_html_token_t *token);

LXB_API bool
lxb_html_tree_insertion_mode_in_cell(lxb_html_tree_t *tree,
                                     lxb_html_token_t *token);

LXB_API bool
lxb_html_tree_insertion_mode_in_select(lxb_html_tree_t *tree,
                                       lxb_html_token_t *token);

LXB_API bool
lxb_html_tree_insertion_mode_in_select_in_table(lxb_html_tree_t *tree,
                                                lxb_html_token_t *token);

LXB_API bool
lxb_html_tree_insertion_mode_in_template(lxb_html_tree_t *tree,
                                         lxb_html_token_t *token);

LXB_API bool
lxb_html_tree_insertion_mode_after_body(lxb_html_tree_t *tree,
                                        lxb_html_token_t *token);

LXB_API bool
lxb_html_tree_insertion_mode_in_frameset(lxb_html_tree_t *tree,
                                         lxb_html_token_t *token);

LXB_API bool
lxb_html_tree_insertion_mode_after_frameset(lxb_html_tree_t *tree,
                                            lxb_html_token_t *token);

LXB_API bool
lxb_html_tree_insertion_mode_after_after_body(lxb_html_tree_t *tree,
                                              lxb_html_token_t *token);

LXB_API bool
lxb_html_tree_insertion_mode_after_after_frameset(lxb_html_tree_t *tree,
                                                  lxb_html_token_t *token);

LXB_API bool
lxb_html_tree_insertion_mode_foreign_content(lxb_html_tree_t *tree,
                                             lxb_html_token_t *token);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_HTML_TREE_INSERTION_MODE_H */
