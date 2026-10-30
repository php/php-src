/*
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_HTML_TOKENIZER_STATE_SCRIPT_H
#define LEXBOR_HTML_TOKENIZER_STATE_SCRIPT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/html/tokenizer.h"


/*
 * Before call function:
 * Must be initialized:
 *     tkz->tmp_tag_id
 */
LXB_API const lxb_char_t *
lxb_html_tokenizer_state_script_data_before(lxb_html_tokenizer_t *tkz,
                                            const lxb_char_t *data,
                                            const lxb_char_t *end);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_HTML_TOKENIZER_STATE_SCRIPT_H */
