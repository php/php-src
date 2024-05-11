/*
 * Copyright (C) 2018 Alexander Borisov
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
    LXB_HTML_TOKENIZER_ERROR_ABDOPUID           = 0x0001,
    /* abrupt-doctype-system-identifier */
    LXB_HTML_TOKENIZER_ERROR_ABDOSYID           = 0x0002,
    /* absence-of-digits-in-numeric-character-reference */
    LXB_HTML_TOKENIZER_ERROR_ABOFDIINNUCHRE     = 0x0003,
    /* cdata-in-html-content */
    LXB_HTML_TOKENIZER_ERROR_CDINHTCO           = 0x0004,
    /* character-reference-outside-unicode-range */
    LXB_HTML_TOKENIZER_ERROR_CHREOUUNRA         = 0x0005,
    /* control-character-in-input-stream */
    LXB_HTML_TOKENIZER_ERROR_COCHININST         = 0x0006,
    /* control-character-reference */
    LXB_HTML_TOKENIZER_ERROR_COCHRE             = 0x0007,
    /* end-tag-with-attributes */
    LXB_HTML_TOKENIZER_ERROR_ENTAWIAT           = 0x0008,
    /* duplicate-attribute */
    LXB_HTML_TOKENIZER_ERROR_DUAT               = 0x0009,
    /* end-tag-with-trailing-solidus */
    LXB_HTML_TOKENIZER_ERROR_ENTAWITRSO         = 0x000A,
    /* eof-before-tag-name */
    LXB_HTML_TOKENIZER_ERROR_EOBETANA           = 0x000B,
    /* eof-in-cdata */
    LXB_HTML_TOKENIZER_ERROR_EOINCD             = 0x000C,
    /* eof-in-comment */
    LXB_HTML_TOKENIZER_ERROR_EOINCO             = 0x000D,
    /* eof-in-doctype */
    LXB_HTML_TOKENIZER_ERROR_EOINDO             = 0x000E,
    /* eof-in-script-html-comment-like-text */
    LXB_HTML_TOKENIZER_ERROR_EOINSCHTCOLITE     = 0x000F,
    /* eof-in-tag */
    LXB_HTML_TOKENIZER_ERROR_EOINTA             = 0x0010,
    /* incorrectly-closed-comment */
    LXB_HTML_TOKENIZER_ERROR_INCLCO             = 0x0011,
    /* incorrectly-opened-comment */
    LXB_HTML_TOKENIZER_ERROR_INOPCO             = 0x0012,
    /* invalid-character-sequence-after-doctype-name */
    LXB_HTML_TOKENIZER_ERROR_INCHSEAFDONA       = 0x0013,
    /* invalid-first-character-of-tag-name */
    LXB_HTML_TOKENIZER_ERROR_INFICHOFTANA       = 0x0014,
    /* missing-attribute-value */
    LXB_HTML_TOKENIZER_ERROR_MIATVA             = 0x0015,
    /* missing-doctype-name */
    LXB_HTML_TOKENIZER_ERROR_MIDONA             = 0x0016,
    /* missing-doctype-public-identifier */
    LXB_HTML_TOKENIZER_ERROR_MIDOPUID           = 0x0017,
    /* missing-doctype-system-identifier */
    LXB_HTML_TOKENIZER_ERROR_MIDOSYID           = 0x0018,
    /* missing-end-tag-name */
    LXB_HTML_TOKENIZER_ERROR_MIENTANA           = 0x0019,
    /* missing-quote-before-doctype-public-identifier */
    LXB_HTML_TOKENIZER_ERROR_MIQUBEDOPUID       = 0x001A,
    /* missing-quote-before-doctype-system-identifier */
    LXB_HTML_TOKENIZER_ERROR_MIQUBEDOSYID       = 0x001B,
    /* missing-semicolon-after-character-reference */
    LXB_HTML_TOKENIZER_ERROR_MISEAFCHRE         = 0x001C,
    /* missing-whitespace-after-doctype-public-keyword */
    LXB_HTML_TOKENIZER_ERROR_MIWHAFDOPUKE       = 0x001D,
    /* missing-whitespace-after-doctype-system-keyword */
    LXB_HTML_TOKENIZER_ERROR_MIWHAFDOSYKE       = 0x001E,
    /* missing-whitespace-before-doctype-name */
    LXB_HTML_TOKENIZER_ERROR_MIWHBEDONA         = 0x001F,
    /* missing-whitespace-between-attributes */
    LXB_HTML_TOKENIZER_ERROR_MIWHBEAT           = 0x0020,
    /* missing-whitespace-between-doctype-public-and-system-identifiers */
    LXB_HTML_TOKENIZER_ERROR_MIWHBEDOPUANSYID   = 0x0021,
    /* nested-comment */
    LXB_HTML_TOKENIZER_ERROR_NECO               = 0x0022,
    /* noncharacter-character-reference */
    LXB_HTML_TOKENIZER_ERROR_NOCHRE             = 0x0023,
    /* noncharacter-in-input-stream */
    LXB_HTML_TOKENIZER_ERROR_NOININST           = 0x0024,
    /* non-void-html-element-start-tag-with-trailing-solidus */
    LXB_HTML_TOKENIZER_ERROR_NOVOHTELSTTAWITRSO = 0x0025,
    /* null-character-reference */
    LXB_HTML_TOKENIZER_ERROR_NUCHRE             = 0x0026,
    /* surrogate-character-reference */
    LXB_HTML_TOKENIZER_ERROR_SUCHRE             = 0x0027,
    /* surrogate-in-input-stream */
    LXB_HTML_TOKENIZER_ERROR_SUININST           = 0x0028,
    /* unexpected-character-after-doctype-system-identifier */
    LXB_HTML_TOKENIZER_ERROR_UNCHAFDOSYID       = 0x0029,
    /* unexpected-character-in-attribute-name */
    LXB_HTML_TOKENIZER_ERROR_UNCHINATNA         = 0x002A,
    /* unexpected-character-in-unquoted-attribute-value */
    LXB_HTML_TOKENIZER_ERROR_UNCHINUNATVA       = 0x002B,
    /* unexpected-equals-sign-before-attribute-name */
    LXB_HTML_TOKENIZER_ERROR_UNEQSIBEATNA       = 0x002C,
    /* unexpected-null-character */
    LXB_HTML_TOKENIZER_ERROR_UNNUCH             = 0x002D,
    /* unexpected-question-mark-instead-of-tag-name */
    LXB_HTML_TOKENIZER_ERROR_UNQUMAINOFTANA     = 0x002E,
    /* unexpected-solidus-in-tag */
    LXB_HTML_TOKENIZER_ERROR_UNSOINTA           = 0x002F,
    /* unknown-named-character-reference */
    LXB_HTML_TOKENIZER_ERROR_UNNACHRE           = 0x0030,
    LXB_HTML_TOKENIZER_ERROR_LAST_ENTRY         = 0x0031,
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


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_HTML_TOKENIZER_ERROR_H */

