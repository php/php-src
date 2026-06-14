/*
 * Copyright (C) 2018-2026 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_HTML_TOKENIZER_ERROR_H
#define LEXBOR_HTML_TOKENIZER_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/base.h"
#include "lexbor/core/array_obj.h"

#include "lexbor/html/tokenizer.h"


typedef enum {
    /* abrupt-closing-of-empty-comment */
    LXB_HTML_TOKENIZER_ERROR_ABCLOFEMCO         = 0x0000,
    /* abrupt-doctype-public-identifier */
    LXB_HTML_TOKENIZER_ERROR_ABDOPUID,
    /* abrupt-doctype-system-identifier */
    LXB_HTML_TOKENIZER_ERROR_ABDOSYID,
    /* absence-of-digits-in-numeric-character-reference */
    LXB_HTML_TOKENIZER_ERROR_ABOFDIINNUCHRE,
    /* cdata-in-html-content */
    LXB_HTML_TOKENIZER_ERROR_CDINHTCO,
    /* character-reference-outside-unicode-range */
    LXB_HTML_TOKENIZER_ERROR_CHREOUUNRA,
    /* control-character-in-input-stream */
    LXB_HTML_TOKENIZER_ERROR_COCHININST,
    /* control-character-reference */
    LXB_HTML_TOKENIZER_ERROR_COCHRE,
    /* end-tag-with-attributes */
    LXB_HTML_TOKENIZER_ERROR_ENTAWIAT,
    /* duplicate-attribute */
    LXB_HTML_TOKENIZER_ERROR_DUAT,
    /* end-tag-with-trailing-solidus */
    LXB_HTML_TOKENIZER_ERROR_ENTAWITRSO,
    /* eof-before-tag-name */
    LXB_HTML_TOKENIZER_ERROR_EOBETANA,
    /* eof-in-cdata */
    LXB_HTML_TOKENIZER_ERROR_EOINCD,
    /* eof-in-comment */
    LXB_HTML_TOKENIZER_ERROR_EOINCO,
    /* eof-in-doctype */
    LXB_HTML_TOKENIZER_ERROR_EOINDO,
    /* eof-in-script-html-comment-like-text */
    LXB_HTML_TOKENIZER_ERROR_EOINSCHTCOLITE,
    /* eof-in-tag */
    LXB_HTML_TOKENIZER_ERROR_EOINTA,
    /* incorrectly-closed-comment */
    LXB_HTML_TOKENIZER_ERROR_INCLCO,
    /* incorrectly-opened-comment */
    LXB_HTML_TOKENIZER_ERROR_INOPCO,
    /* invalid-character-sequence-after-doctype-name */
    LXB_HTML_TOKENIZER_ERROR_INCHSEAFDONA,
    /* invalid-first-character-of-tag-name */
    LXB_HTML_TOKENIZER_ERROR_INFICHOFTANA,
    /* missing-attribute-value */
    LXB_HTML_TOKENIZER_ERROR_MIATVA,
    /* missing-doctype-name */
    LXB_HTML_TOKENIZER_ERROR_MIDONA,
    /* missing-doctype-public-identifier */
    LXB_HTML_TOKENIZER_ERROR_MIDOPUID,
    /* missing-doctype-system-identifier */
    LXB_HTML_TOKENIZER_ERROR_MIDOSYID,
    /* missing-end-tag-name */
    LXB_HTML_TOKENIZER_ERROR_MIENTANA,
    /* missing-quote-before-doctype-public-identifier */
    LXB_HTML_TOKENIZER_ERROR_MIQUBEDOPUID,
    /* missing-quote-before-doctype-system-identifier */
    LXB_HTML_TOKENIZER_ERROR_MIQUBEDOSYID,
    /* missing-semicolon-after-character-reference */
    LXB_HTML_TOKENIZER_ERROR_MISEAFCHRE,
    /* missing-whitespace-after-doctype-public-keyword */
    LXB_HTML_TOKENIZER_ERROR_MIWHAFDOPUKE,
    /* missing-whitespace-after-doctype-system-keyword */
    LXB_HTML_TOKENIZER_ERROR_MIWHAFDOSYKE,
    /* missing-whitespace-before-doctype-name */
    LXB_HTML_TOKENIZER_ERROR_MIWHBEDONA,
    /* missing-whitespace-between-attributes */
    LXB_HTML_TOKENIZER_ERROR_MIWHBEAT,
    /* missing-whitespace-between-doctype-public-and-system-identifiers */
    LXB_HTML_TOKENIZER_ERROR_MIWHBEDOPUANSYID,
    /* nested-comment */
    LXB_HTML_TOKENIZER_ERROR_NECO,
    /* noncharacter-character-reference */
    LXB_HTML_TOKENIZER_ERROR_NOCHRE,
    /* noncharacter-in-input-stream */
    LXB_HTML_TOKENIZER_ERROR_NOININST,
    /* non-void-html-element-start-tag-with-trailing-solidus */
    LXB_HTML_TOKENIZER_ERROR_NOVOHTELSTTAWITRSO,
    /* null-character-reference */
    LXB_HTML_TOKENIZER_ERROR_NUCHRE,
    /* surrogate-character-reference */
    LXB_HTML_TOKENIZER_ERROR_SUCHRE,
    /* surrogate-in-input-stream */
    LXB_HTML_TOKENIZER_ERROR_SUININST,
    /* unexpected-character-after-doctype-system-identifier */
    LXB_HTML_TOKENIZER_ERROR_UNCHAFDOSYID,
    /* unexpected-character-in-attribute-name */
    LXB_HTML_TOKENIZER_ERROR_UNCHINATNA,
    /* unexpected-character-in-unquoted-attribute-value */
    LXB_HTML_TOKENIZER_ERROR_UNCHINUNATVA,
    /* unexpected-equals-sign-before-attribute-name */
    LXB_HTML_TOKENIZER_ERROR_UNEQSIBEATNA,
    /* unexpected-null-character */
    LXB_HTML_TOKENIZER_ERROR_UNNUCH,
    /* unexpected-question-mark-instead-of-tag-name */
    LXB_HTML_TOKENIZER_ERROR_UNQUMAINOFTANA,
    /* unexpected-solidus-in-tag */
    LXB_HTML_TOKENIZER_ERROR_UNSOINTA,
    /* unknown-named-character-reference */
    LXB_HTML_TOKENIZER_ERROR_UNNACHRE,

    LXB_HTML_TOKENIZER_ERROR_LAST_ENTRY
}
lxb_html_tokenizer_error_id_t;

typedef struct {
    const lxb_char_t              *pos;
    lxb_html_tokenizer_error_id_t id;
}
lxb_html_tokenizer_error_t;


LXB_API lxb_html_tokenizer_error_t *
lxb_html_tokenizer_error_add(lexbor_array_obj_t *parse_errors,
                             const lxb_char_t *pos,
                             lxb_html_tokenizer_error_id_t id);

LXB_API const lxb_char_t *
lxb_html_tokenizer_error_to_string(lxb_html_tokenizer_error_id_t id,
                                   size_t *len);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_HTML_TOKENIZER_ERROR_H */
