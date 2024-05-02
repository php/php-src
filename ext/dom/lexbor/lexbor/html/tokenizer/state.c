/*
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/tokenizer/state.h"
#include "lexbor/html/tokenizer/state_comment.h"
#include "lexbor/html/tokenizer/state_doctype.h"

#define LEXBOR_STR_RES_ANSI_REPLACEMENT_CHARACTER
#define LEXBOR_STR_RES_ALPHANUMERIC_CHARACTER
#define LEXBOR_STR_RES_REPLACEMENT_CHARACTER
#define LEXBOR_STR_RES_ALPHA_CHARACTER
#define LEXBOR_STR_RES_MAP_HEX
#define LEXBOR_STR_RES_MAP_NUM
#include "lexbor/core/str_res.h"

#define LXB_HTML_TOKENIZER_RES_ENTITIES_SBST
#include "lexbor/html/tokenizer/res.h"


const lxb_tag_data_t *
lxb_tag_append_lower(lexbor_hash_t *hash,
                     const lxb_char_t *name, size_t length);

lxb_dom_attr_data_t *
lxb_dom_attr_local_name_append(lexbor_hash_t *hash,
                               const lxb_char_t *name, size_t length);


static const lxb_char_t *
lxb_html_tokenizer_state_data(lxb_html_tokenizer_t *tkz,
                              const lxb_char_t *data, const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_plaintext(lxb_html_tokenizer_t *tkz,
                                   const lxb_char_t *data,
                                   const lxb_char_t *end);

/* Tag */
static const lxb_char_t *
lxb_html_tokenizer_state_tag_open(lxb_html_tokenizer_t *tkz,
                                  const lxb_char_t *data,
                                  const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_end_tag_open(lxb_html_tokenizer_t *tkz,
                                      const lxb_char_t *data,
                                      const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_tag_name(lxb_html_tokenizer_t *tkz,
                                  const lxb_char_t *data,
                                  const lxb_char_t *end);

/* Attribute */
static const lxb_char_t *
lxb_html_tokenizer_state_attribute_name(lxb_html_tokenizer_t *tkz,
                                        const lxb_char_t *data,
                                        const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_after_attribute_name(lxb_html_tokenizer_t *tkz,
                                              const lxb_char_t *data,
                                              const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_before_attribute_value(lxb_html_tokenizer_t *tkz,
                                                const lxb_char_t *data,
                                                const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_attribute_value_double_quoted(lxb_html_tokenizer_t *tkz,
                                                       const lxb_char_t *data,
                                                       const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_attribute_value_single_quoted(lxb_html_tokenizer_t *tkz,
                                                       const lxb_char_t *data,
                                                       const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_attribute_value_unquoted(lxb_html_tokenizer_t *tkz,
                                                  const lxb_char_t *data,
                                                  const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_after_attribute_value_quoted(lxb_html_tokenizer_t *tkz,
                                                      const lxb_char_t *data,
                                                      const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_bogus_comment_before(lxb_html_tokenizer_t *tkz,
                                              const lxb_char_t *data,
                                              const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_bogus_comment(lxb_html_tokenizer_t *tkz,
                                       const lxb_char_t *data,
                                       const lxb_char_t *end);

/* Markup declaration */
static const lxb_char_t *
lxb_html_tokenizer_state_markup_declaration_open(lxb_html_tokenizer_t *tkz,
                                                 const lxb_char_t *data,
                                                 const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_markup_declaration_comment(lxb_html_tokenizer_t *tkz,
                                                    const lxb_char_t *data,
                                                    const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_markup_declaration_doctype(lxb_html_tokenizer_t *tkz,
                                                    const lxb_char_t *data,
                                                    const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_markup_declaration_cdata(lxb_html_tokenizer_t *tkz,
                                                  const lxb_char_t *data,
                                                  const lxb_char_t *end);

/* CDATA Section */
static const lxb_char_t *
lxb_html_tokenizer_state_cdata_section_before(lxb_html_tokenizer_t *tkz,
                                              const lxb_char_t *data,
                                              const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_cdata_section(lxb_html_tokenizer_t *tkz,
                                       const lxb_char_t *data,
                                       const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_cdata_section_bracket(lxb_html_tokenizer_t *tkz,
                                               const lxb_char_t *data,
                                               const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_cdata_section_end(lxb_html_tokenizer_t *tkz,
                                           const lxb_char_t *data,
                                           const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_char_ref_attr(lxb_html_tokenizer_t *tkz,
                                       const lxb_char_t *data,
                                       const lxb_char_t *end);

static const lxb_char_t *
_lxb_html_tokenizer_state_char_ref(lxb_html_tokenizer_t *tkz,
                                   const lxb_char_t *data,
                                   const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_char_ref_named(lxb_html_tokenizer_t *tkz,
                                        const lxb_char_t *data,
                                        const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_char_ref_ambiguous_ampersand(lxb_html_tokenizer_t *tkz,
                                                      const lxb_char_t *data,
                                                      const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_char_ref_numeric(lxb_html_tokenizer_t *tkz,
                                          const lxb_char_t *data,
                                          const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_char_ref_hexademical_start(lxb_html_tokenizer_t *tkz,
                                                    const lxb_char_t *data,
                                                    const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_char_ref_decimal_start(lxb_html_tokenizer_t *tkz,
                                                const lxb_char_t *data,
                                                const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_char_ref_hexademical(lxb_html_tokenizer_t *tkz,
                                              const lxb_char_t *data,
                                              const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_char_ref_decimal(lxb_html_tokenizer_t *tkz,
                                          const lxb_char_t *data,
                                          const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_char_ref_numeric_end(lxb_html_tokenizer_t *tkz,
                                              const lxb_char_t *data,
                                              const lxb_char_t *end);

static size_t
lxb_html_tokenizer_state_to_ascii_utf_8(size_t codepoint, lxb_char_t *data);


/*
 * Helper function. No in the specification. For 12.2.5.1 Data state
 */
const lxb_char_t *
lxb_html_tokenizer_state_data_before(lxb_html_tokenizer_t *tkz,
                                     const lxb_char_t *data,
                                     const lxb_char_t *end)
{
    if (tkz->is_eof == false) {
        lxb_html_tokenizer_state_token_set_begin(tkz, data);
    }

    /*
     * Text node init param sets before emit token.
     */

    tkz->state = lxb_html_tokenizer_state_data;

    return data;
}

/*
 * 12.2.5.1 Data state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_data(lxb_html_tokenizer_t *tkz,
                              const lxb_char_t *data, const lxb_char_t *end)
{
    lxb_html_tokenizer_state_begin_set(tkz, data);

    while (data != end) {
        switch (*data) {
            /* U+003C LESS-THAN SIGN (<) */
            case 0x3C:
                lxb_html_tokenizer_state_append_data_m(tkz, data);
                lxb_html_tokenizer_state_token_set_end(tkz, data);

                tkz->state = lxb_html_tokenizer_state_tag_open;
                return (data + 1);

            /* U+0026 AMPERSAND (&) */
            case 0x26:
                lxb_html_tokenizer_state_append_data_m(tkz, data + 1);

                tkz->state = lxb_html_tokenizer_state_char_ref;
                tkz->state_return = lxb_html_tokenizer_state_data;

                return data + 1;

            /* U+000D CARRIAGE RETURN (CR) */
            case 0x0D:
                if (++data >= end) {
                    lxb_html_tokenizer_state_append_data_m(tkz, data - 1);

                    tkz->state = lxb_html_tokenizer_state_cr;
                    tkz->state_return = lxb_html_tokenizer_state_data;

                    return data;
                }

                lxb_html_tokenizer_state_append_data_m(tkz, data);
                tkz->pos[-1] = 0x0A;

                lxb_html_tokenizer_state_begin_set(tkz, data + 1);

                if (*data != 0x0A) {
                    lxb_html_tokenizer_state_begin_set(tkz, data);
                    data--;
                }

                break;

            /*
             * U+0000 NULL
             * EOF
             */
            case 0x00:
                if (tkz->is_eof) {
                    /* Emit TEXT node if not empty */
                    if (tkz->token->begin != NULL) {
                        lxb_html_tokenizer_state_token_set_end_oef(tkz);
                    }

                    if (tkz->token->begin != tkz->token->end) {
                        tkz->token->tag_id = LXB_TAG__TEXT;

                        lxb_html_tokenizer_state_append_data_m(tkz, data);

                        lxb_html_tokenizer_state_set_text(tkz);
                        lxb_html_tokenizer_state_token_done_wo_check_m(tkz,end);
                    }

                    return end;
                }

                if (SIZE_MAX - tkz->token->null_count < 1) {
                    tkz->status = LXB_STATUS_ERROR_OVERFLOW;
                    return end;
                }

                tkz->token->null_count++;

                lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                             LXB_HTML_TOKENIZER_ERROR_UNNUCH);
                break;
        }

        data++;
    }

    lxb_html_tokenizer_state_append_data_m(tkz, data);

    return data;
}

/*
 * Helper function. No in the specification. For 12.2.5.5 PLAINTEXT state
 */
const lxb_char_t *
lxb_html_tokenizer_state_plaintext_before(lxb_html_tokenizer_t *tkz,
                                          const lxb_char_t *data,
                                          const lxb_char_t *end)
{
    if (tkz->is_eof == false) {
        lxb_html_tokenizer_state_token_set_begin(tkz, data);
    }

    tkz->token->tag_id = LXB_TAG__TEXT;

    tkz->state = lxb_html_tokenizer_state_plaintext;

    return data;
}

/*
 * 12.2.5.5 PLAINTEXT state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_plaintext(lxb_html_tokenizer_t *tkz,
                                   const lxb_char_t *data,
                                   const lxb_char_t *end)
{
    lxb_html_tokenizer_state_begin_set(tkz, data);

    while (data != end) {
        switch (*data) {
            /* U+000D CARRIAGE RETURN (CR) */
            case 0x0D:
                if (++data >= end) {
                    lxb_html_tokenizer_state_append_data_m(tkz, data - 1);

                    tkz->state = lxb_html_tokenizer_state_cr;
                    tkz->state_return = lxb_html_tokenizer_state_plaintext;

                    return data;
                }

                lxb_html_tokenizer_state_append_data_m(tkz, data);
                tkz->pos[-1] = 0x0A;

                lxb_html_tokenizer_state_begin_set(tkz, data + 1);

                if (*data != 0x0A) {
                    lxb_html_tokenizer_state_begin_set(tkz, data);
                    data--;
                }

                break;

            /*
             * U+0000 NULL
             * EOF
             */
            case 0x00:
                lxb_html_tokenizer_state_append_data_m(tkz, data);

                if (tkz->is_eof) {
                    if (tkz->token->begin != NULL) {
                        lxb_html_tokenizer_state_token_set_end_oef(tkz);
                    }

                    lxb_html_tokenizer_state_set_text(tkz);
                    lxb_html_tokenizer_state_token_done_m(tkz, end);

                    return end;
                }

                lxb_html_tokenizer_state_begin_set(tkz, data + 1);
                lxb_html_tokenizer_state_append_replace_m(tkz);

                lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                             LXB_HTML_TOKENIZER_ERROR_UNNUCH);
                break;
        }

        data++;
    }

    lxb_html_tokenizer_state_append_data_m(tkz, data);

    return data;
}

/*
 * 12.2.5.6 Tag open state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_tag_open(lxb_html_tokenizer_t *tkz,
                                  const lxb_char_t *data, const lxb_char_t *end)
{
    /* ASCII alpha */
    if (lexbor_str_res_alpha_character[ *data ] != LEXBOR_STR_RES_SLIP) {
        tkz->state = lxb_html_tokenizer_state_tag_name;

        lxb_html_tokenizer_state_token_emit_text_not_empty_m(tkz, end);
        lxb_html_tokenizer_state_token_set_begin(tkz, data);

        return data;
    }

    /* U+002F SOLIDUS (/) */
    else if (*data == 0x2F) {
        tkz->state = lxb_html_tokenizer_state_end_tag_open;

        return (data + 1);
    }

    /* U+0021 EXCLAMATION MARK (!) */
    else if (*data == 0x21) {
        tkz->state = lxb_html_tokenizer_state_markup_declaration_open;

        lxb_html_tokenizer_state_token_emit_text_not_empty_m(tkz, end);

        return (data + 1);
    }

    /* U+003F QUESTION MARK (?) */
    else if (*data == 0x3F) {
        tkz->state = lxb_html_tokenizer_state_bogus_comment_before;

        lxb_html_tokenizer_state_token_emit_text_not_empty_m(tkz, end);
        lxb_html_tokenizer_state_token_set_begin(tkz, data);

        lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                     LXB_HTML_TOKENIZER_ERROR_UNQUMAINOFTANA);

        return data;
    }

    /* EOF */
    else if (*data == 0x00) {
        if (tkz->is_eof) {
            lxb_html_tokenizer_state_append_m(tkz, "<", 1);

            lxb_html_tokenizer_state_token_set_end_oef(tkz);
            lxb_html_tokenizer_state_token_emit_text_not_empty_m(tkz, end);

            lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->token->end,
                                         LXB_HTML_TOKENIZER_ERROR_EOBETANA);

            return end;
        }
    }

    lxb_html_tokenizer_state_append_m(tkz, "<", 1);

    lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                 LXB_HTML_TOKENIZER_ERROR_INFICHOFTANA);

    tkz->state = lxb_html_tokenizer_state_data;

    return data;
}

/*
 * 12.2.5.7 End tag open state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_end_tag_open(lxb_html_tokenizer_t *tkz,
                                      const lxb_char_t *data,
                                      const lxb_char_t *end)
{
    /* ASCII alpha */
    if (lexbor_str_res_alpha_character[ *data ] != LEXBOR_STR_RES_SLIP) {
        tkz->state = lxb_html_tokenizer_state_tag_name;

        lxb_html_tokenizer_state_token_emit_text_not_empty_m(tkz, end);
        lxb_html_tokenizer_state_token_set_begin(tkz, data);

        tkz->token->type |= LXB_HTML_TOKEN_TYPE_CLOSE;

        return data;
    }

    /* U+003E GREATER-THAN SIGN (>) */
    else if (*data == 0x3E) {
        tkz->state = lxb_html_tokenizer_state_data;

        lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                     LXB_HTML_TOKENIZER_ERROR_MIENTANA);

        return (data + 1);
    }

    /* Fake EOF */
    else if (*data == 0x00) {
        if (tkz->is_eof) {
            lxb_html_tokenizer_state_append_m(tkz, "</", 2);

            lxb_html_tokenizer_state_token_set_end_oef(tkz);
            lxb_html_tokenizer_state_token_emit_text_not_empty_m(tkz, end);

            lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->token->end,
                                         LXB_HTML_TOKENIZER_ERROR_EOBETANA);

            return end;
        }
    }

    tkz->state = lxb_html_tokenizer_state_bogus_comment_before;

    lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                 LXB_HTML_TOKENIZER_ERROR_INFICHOFTANA);

    lxb_html_tokenizer_state_token_emit_text_not_empty_m(tkz, end);
    lxb_html_tokenizer_state_token_set_begin(tkz, data);

    return data;
}

/*
 * 12.2.5.8 Tag name state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_tag_name(lxb_html_tokenizer_t *tkz,
                                  const lxb_char_t *data, const lxb_char_t *end)
{
    lxb_html_tokenizer_state_begin_set(tkz, data);

    while (data != end) {
        switch (*data) {
            /*
             * U+0009 CHARACTER TABULATION (tab)
             * U+000A LINE FEED (LF)
             * U+000C FORM FEED (FF)
             * U+000D CARRIAGE RETURN (CR)
             * U+0020 SPACE
             */
            case 0x09:
            case 0x0A:
            case 0x0C:
            case 0x0D:
            case 0x20:
                lxb_html_tokenizer_state_append_data_m(tkz, data);
                lxb_html_tokenizer_state_set_tag_m(tkz, tkz->start, tkz->pos);
                lxb_html_tokenizer_state_token_set_end(tkz, data);

                tkz->state = lxb_html_tokenizer_state_before_attribute_name;
                return (data + 1);

            /* U+002F SOLIDUS (/) */
            case 0x2F:
                lxb_html_tokenizer_state_append_data_m(tkz, data);
                lxb_html_tokenizer_state_set_tag_m(tkz, tkz->start, tkz->pos);
                lxb_html_tokenizer_state_token_set_end(tkz, data);

                tkz->state = lxb_html_tokenizer_state_self_closing_start_tag;
                return (data + 1);

            /* U+003E GREATER-THAN SIGN (>) */
            case 0x3E:
                tkz->state = lxb_html_tokenizer_state_data_before;

                lxb_html_tokenizer_state_append_data_m(tkz, data);
                lxb_html_tokenizer_state_set_tag_m(tkz, tkz->start, tkz->pos);
                lxb_html_tokenizer_state_token_set_end(tkz, data);
                lxb_html_tokenizer_state_token_done_m(tkz, end);

                return (data + 1);

            /* U+0000 NULL */
            case 0x00:
                if (tkz->is_eof) {
                    lxb_html_tokenizer_state_token_set_end_oef(tkz);

                    lxb_html_tokenizer_error_add(tkz->parse_errors,
                                               tkz->token->end,
                                               LXB_HTML_TOKENIZER_ERROR_EOINTA);
                    return end;
                }

                lxb_html_tokenizer_state_append_data_m(tkz, data);
                lxb_html_tokenizer_state_begin_set(tkz, data + 1);
                lxb_html_tokenizer_state_append_replace_m(tkz);

                lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                             LXB_HTML_TOKENIZER_ERROR_UNNUCH);
                break;

            default:
                break;
        }

        data++;
    }

    lxb_html_tokenizer_state_append_data_m(tkz, data);

    return data;
}

/*
 * 12.2.5.32 Before attribute name state
 */
const lxb_char_t *
lxb_html_tokenizer_state_before_attribute_name(lxb_html_tokenizer_t *tkz,
                                               const lxb_char_t *data,
                                               const lxb_char_t *end)
{
    lxb_html_token_attr_t *attr;

    while (data != end) {
        switch (*data) {
            /*
             * U+0009 CHARACTER TABULATION (tab)
             * U+000A LINE FEED (LF)
             * U+000C FORM FEED (FF)
             * U+000D CARRIAGE RETURN (CR)
             * U+0020 SPACE
             */
            case 0x09:
            case 0x0A:
            case 0x0C:
            case 0x0D:
            case 0x20:
                break;

            /* U+003D EQUALS SIGN (=) */
            case 0x3D:
                lxb_html_tokenizer_state_token_attr_add_m(tkz, attr, end);
                lxb_html_tokenizer_state_token_attr_set_name_begin(tkz, data);

                lxb_html_tokenizer_state_append_m(tkz, data, 1);

                lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                         LXB_HTML_TOKENIZER_ERROR_UNEQSIBEATNA);

                tkz->state = lxb_html_tokenizer_state_attribute_name;
                return (data + 1);

            /*
             * U+002F SOLIDUS (/)
             * U+003E GREATER-THAN SIGN (>)
             */
            case 0x2F:
            case 0x3E:
                tkz->state = lxb_html_tokenizer_state_after_attribute_name;
                return data;

            /* EOF */
            case 0x00:
                if (tkz->is_eof) {
                    tkz->state = lxb_html_tokenizer_state_after_attribute_name;
                    return data;
                }
                /* fall through */

            /* Anything else */
            default:
                lxb_html_tokenizer_state_token_attr_add_m(tkz, attr, end);
                lxb_html_tokenizer_state_token_attr_set_name_begin(tkz, data);

                tkz->state = lxb_html_tokenizer_state_attribute_name;
                return data;
        }

        data++;
    }

    return data;
}

/*
 * 12.2.5.33 Attribute name state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_attribute_name(lxb_html_tokenizer_t *tkz,
                                        const lxb_char_t *data,
                                        const lxb_char_t *end)
{
    lxb_html_tokenizer_state_begin_set(tkz, data);

    while (data != end) {
        switch (*data) {
            /*
             * U+0009 CHARACTER TABULATION (tab)
             * U+000A LINE FEED (LF)
             * U+000C FORM FEED (FF)
             * U+000D CARRIAGE RETURN (CR)
             * U+0020 SPACE
             * U+002F SOLIDUS (/)
             * U+003E GREATER-THAN SIGN (>)
             */
            case 0x09:
            case 0x0A:
            case 0x0C:
            case 0x0D:
            case 0x20:
            case 0x2F:
            case 0x3E:
                lxb_html_tokenizer_state_append_data_m(tkz, data);
                lxb_html_tokenizer_state_set_name_m(tkz);
                lxb_html_tokenizer_state_token_attr_set_name_end(tkz, data);

                tkz->state = lxb_html_tokenizer_state_after_attribute_name;
                return data;

            /*
             * U+0000 NULL
             * EOF
             */
            case 0x00:
                if (tkz->is_eof) {
                    lxb_html_tokenizer_state_token_attr_set_name_end_oef(tkz);

                    tkz->state = lxb_html_tokenizer_state_after_attribute_name;
                    return data;
                }

                lxb_html_tokenizer_state_append_data_m(tkz, data);
                lxb_html_tokenizer_state_begin_set(tkz, data + 1);
                lxb_html_tokenizer_state_append_replace_m(tkz);

                lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                             LXB_HTML_TOKENIZER_ERROR_UNNUCH);
                break;

            /* U+003D EQUALS SIGN (=) */
            case 0x3D:
                lxb_html_tokenizer_state_append_data_m(tkz, data);
                lxb_html_tokenizer_state_set_name_m(tkz);
                lxb_html_tokenizer_state_token_attr_set_name_end(tkz, data);

                tkz->state = lxb_html_tokenizer_state_before_attribute_value;
                return (data + 1);

            /*
             * U+0022 QUOTATION MARK (")
             * U+0027 APOSTROPHE (')
             * U+003C LESS-THAN SIGN (<)
             */
            case 0x22:
            case 0x27:
            case 0x3C:
                lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                           LXB_HTML_TOKENIZER_ERROR_UNCHINATNA);
                break;

            default:
                break;
        }

        data++;
    }

    lxb_html_tokenizer_state_append_data_m(tkz, data);

    return data;
}

/*
 * 12.2.5.34 After attribute name state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_after_attribute_name(lxb_html_tokenizer_t *tkz,
                                              const lxb_char_t *data,
                                              const lxb_char_t *end)
{
    lxb_html_token_attr_t *attr;

    while (data != end) {
        switch (*data) {
            /*
             * U+0009 CHARACTER TABULATION (tab)
             * U+000A LINE FEED (LF)
             * U+000C FORM FEED (FF)
             * U+000D CARRIAGE RETURN (CR)
             * U+0020 SPACE
             */
            case 0x09:
            case 0x0A:
            case 0x0C:
            case 0x0D:
            case 0x20:
                break;

            /* U+002F SOLIDUS (/) */
            case 0x2F:
                tkz->state = lxb_html_tokenizer_state_self_closing_start_tag;
                return (data + 1);

            /* U+003D EQUALS SIGN (=) */
            case 0x3D:
                tkz->state = lxb_html_tokenizer_state_before_attribute_value;
                return (data + 1);

            /* U+003E GREATER-THAN SIGN (>) */
            case 0x3E:
                tkz->state = lxb_html_tokenizer_state_data_before;

                lxb_html_tokenizer_state_token_done_m(tkz, end);

                return (data + 1);

            case 0x00:
                if (tkz->is_eof) {
                    lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->last,
                                               LXB_HTML_TOKENIZER_ERROR_EOINTA);
                    return end;
                }
                /* fall through */

            default:
                lxb_html_tokenizer_state_token_attr_add_m(tkz, attr, end);
                lxb_html_tokenizer_state_token_attr_set_name_begin(tkz, data);

                tkz->state = lxb_html_tokenizer_state_attribute_name;
                return data;
        }

        data++;
    }

    return data;
}

/*
 * 12.2.5.35 Before attribute value state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_before_attribute_value(lxb_html_tokenizer_t *tkz,
                                                const lxb_char_t *data,
                                                const lxb_char_t *end)
{
    while (data != end) {
        switch (*data) {
            /*
             * U+0009 CHARACTER TABULATION (tab)
             * U+000A LINE FEED (LF)
             * U+000C FORM FEED (FF)
             * U+000D CARRIAGE RETURN (CR)
             * U+0020 SPACE
             */
            case 0x09:
            case 0x0A:
            case 0x0C:
            case 0x0D:
            case 0x20:
                break;

            /* U+0022 QUOTATION MARK (") */
            case 0x22:
                tkz->state =
                    lxb_html_tokenizer_state_attribute_value_double_quoted;

                return (data + 1);

            /* U+0027 APOSTROPHE (') */
            case 0x27:
                tkz->state =
                    lxb_html_tokenizer_state_attribute_value_single_quoted;

                return (data + 1);

            /* U+003E GREATER-THAN SIGN (>) */
            case 0x3E:
                tkz->state = lxb_html_tokenizer_state_data_before;

                lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                             LXB_HTML_TOKENIZER_ERROR_MIATVA);

                lxb_html_tokenizer_state_token_done_m(tkz, end);

                return (data + 1);

            default:
                tkz->state = lxb_html_tokenizer_state_attribute_value_unquoted;
                return data;
        }

        data++;
    }

    return data;
}

/*
 * 12.2.5.36 Attribute value (double-quoted) state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_attribute_value_double_quoted(lxb_html_tokenizer_t *tkz,
                                                       const lxb_char_t *data,
                                                       const lxb_char_t *end)
{
    if (tkz->token->attr_last->value_begin == NULL && tkz->is_eof == false) {
        lxb_html_tokenizer_state_token_attr_set_value_begin(tkz, data);
    }

    lxb_html_tokenizer_state_begin_set(tkz, data);

    while (data != end) {
        switch (*data) {
            /* U+0022 QUOTATION MARK (") */
            case 0x22:
                lxb_html_tokenizer_state_append_data_m(tkz, data);
                lxb_html_tokenizer_state_token_attr_set_value_end(tkz, data);
                lxb_html_tokenizer_state_set_value_m(tkz);

                tkz->state =
                    lxb_html_tokenizer_state_after_attribute_value_quoted;

                return (data + 1);

            /* U+0026 AMPERSAND (&) */
            case 0x26:
                lxb_html_tokenizer_state_append_data_m(tkz, data + 1);

                tkz->state = lxb_html_tokenizer_state_char_ref_attr;
                tkz->state_return = lxb_html_tokenizer_state_attribute_value_double_quoted;

                return data + 1;

            /* U+000D CARRIAGE RETURN (CR) */
            case 0x0D:
                if (++data >= end) {
                    lxb_html_tokenizer_state_append_data_m(tkz, data - 1);

                    tkz->state = lxb_html_tokenizer_state_cr;
                    tkz->state_return = lxb_html_tokenizer_state_attribute_value_double_quoted;

                    return data;
                }

                lxb_html_tokenizer_state_append_data_m(tkz, data);
                tkz->pos[-1] = 0x0A;

                lxb_html_tokenizer_state_begin_set(tkz, data + 1);

                if (*data != 0x0A) {
                    lxb_html_tokenizer_state_begin_set(tkz, data);
                    data--;
                }

                break;

            /*
             * U+0000 NULL
             * EOF
             */
            case 0x00:
                if (tkz->is_eof) {
                    if (tkz->token->attr_last->value_begin != NULL) {
                     lxb_html_tokenizer_state_token_attr_set_value_end_oef(tkz);
                    }

                    lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->last,
                                               LXB_HTML_TOKENIZER_ERROR_EOINTA);
                    return end;
                }

                lxb_html_tokenizer_state_append_data_m(tkz, data);
                lxb_html_tokenizer_state_begin_set(tkz, data + 1);
                lxb_html_tokenizer_state_append_replace_m(tkz);

                lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                             LXB_HTML_TOKENIZER_ERROR_UNNUCH);
                break;

            default:
                break;
        }

        data++;
    }

    lxb_html_tokenizer_state_append_data_m(tkz, data);

    return data;
}

/*
 * 12.2.5.37 Attribute value (single-quoted) state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_attribute_value_single_quoted(lxb_html_tokenizer_t *tkz,
                                                       const lxb_char_t *data,
                                                       const lxb_char_t *end)
{
    if (tkz->token->attr_last->value_begin == NULL && tkz->is_eof == false) {
        lxb_html_tokenizer_state_token_attr_set_value_begin(tkz, data);
    }

    lxb_html_tokenizer_state_begin_set(tkz, data);

    while (data != end) {
        switch (*data) {
            /* U+0027 APOSTROPHE (') */
            case 0x27:
                lxb_html_tokenizer_state_append_data_m(tkz, data);
                lxb_html_tokenizer_state_token_attr_set_value_end(tkz, data);
                lxb_html_tokenizer_state_set_value_m(tkz);

                tkz->state =
                    lxb_html_tokenizer_state_after_attribute_value_quoted;

                return (data + 1);

            /* U+0026 AMPERSAND (&) */
            case 0x26:
                lxb_html_tokenizer_state_append_data_m(tkz, data + 1);

                tkz->state = lxb_html_tokenizer_state_char_ref_attr;
                tkz->state_return = lxb_html_tokenizer_state_attribute_value_single_quoted;

                return data + 1;

            /* U+000D CARRIAGE RETURN (CR) */
            case 0x0D:
                if (++data >= end) {
                    lxb_html_tokenizer_state_append_data_m(tkz, data - 1);

                    tkz->state = lxb_html_tokenizer_state_cr;
                    tkz->state_return = lxb_html_tokenizer_state_attribute_value_single_quoted;

                    return data;
                }

                lxb_html_tokenizer_state_append_data_m(tkz, data);
                tkz->pos[-1] = 0x0A;

                lxb_html_tokenizer_state_begin_set(tkz, data + 1);

                if (*data != 0x0A) {
                    lxb_html_tokenizer_state_begin_set(tkz, data);
                    data--;
                }

                break;

            /*
             * U+0000 NULL
             * EOF
             */
            case 0x00:
                if (tkz->is_eof) {
                    if (tkz->token->attr_last->value_begin != NULL) {
                     lxb_html_tokenizer_state_token_attr_set_value_end_oef(tkz);
                    }

                    lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->last,
                                               LXB_HTML_TOKENIZER_ERROR_EOINTA);
                    return end;
                }

                lxb_html_tokenizer_state_append_data_m(tkz, data);
                lxb_html_tokenizer_state_begin_set(tkz, data + 1);
                lxb_html_tokenizer_state_append_replace_m(tkz);

                lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                             LXB_HTML_TOKENIZER_ERROR_UNNUCH);
                break;

            default:
                break;
        }

        data++;
    }

    lxb_html_tokenizer_state_append_data_m(tkz, data);

    return data;
}

/*
 * 12.2.5.38 Attribute value (unquoted) state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_attribute_value_unquoted(lxb_html_tokenizer_t *tkz,
                                                  const lxb_char_t *data,
                                                  const lxb_char_t *end)
{
    if (tkz->token->attr_last->value_begin == NULL && tkz->is_eof == false) {
        lxb_html_tokenizer_state_token_attr_set_value_begin(tkz, data);
    }

    lxb_html_tokenizer_state_begin_set(tkz, data);

    while (data != end) {
        switch (*data) {
           /*
            * U+0009 CHARACTER TABULATION (tab)
            * U+000A LINE FEED (LF)
            * U+000C FORM FEED (FF)
            * U+000D CARRIAGE RETURN (CR)
            * U+0020 SPACE
            */
            case 0x09:
            case 0x0A:
            case 0x0C:
            case 0x0D:
            case 0x20:
                lxb_html_tokenizer_state_append_data_m(tkz, data);
                lxb_html_tokenizer_state_token_attr_set_value_end(tkz, data);
                lxb_html_tokenizer_state_set_value_m(tkz);

                tkz->state = lxb_html_tokenizer_state_before_attribute_name;
                return (data + 1);

            /* U+0026 AMPERSAND (&) */
            case 0x26:
                lxb_html_tokenizer_state_append_data_m(tkz, data + 1);

                tkz->state = lxb_html_tokenizer_state_char_ref_attr;
                tkz->state_return = lxb_html_tokenizer_state_attribute_value_unquoted;

                return data + 1;

            /* U+003E GREATER-THAN SIGN (>) */
            case 0x3E:
                tkz->state = lxb_html_tokenizer_state_data_before;

                lxb_html_tokenizer_state_append_data_m(tkz, data);
                lxb_html_tokenizer_state_token_attr_set_value_end(tkz, data);
                lxb_html_tokenizer_state_set_value_m(tkz);

                lxb_html_tokenizer_state_token_done_m(tkz, end);

                return (data + 1);

            /*
             * U+0000 NULL
             * EOF
             */
            case 0x00:
                if (tkz->is_eof) {
                    if (tkz->token->attr_last->value_begin != NULL) {
                     lxb_html_tokenizer_state_token_attr_set_value_end_oef(tkz);
                    }

                    lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->last,
                                                 LXB_HTML_TOKENIZER_ERROR_EOINTA);
                    return end;
                }

                lxb_html_tokenizer_state_append_data_m(tkz, data);
                lxb_html_tokenizer_state_begin_set(tkz, data + 1);
                lxb_html_tokenizer_state_append_replace_m(tkz);

                lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                             LXB_HTML_TOKENIZER_ERROR_UNNUCH);
                break;

            /*
             * U+0022 QUOTATION MARK (")
             * U+0027 APOSTROPHE (')
             * U+003C LESS-THAN SIGN (<)
             * U+003D EQUALS SIGN (=)
             * U+0060 GRAVE ACCENT (`)
             */
            case 0x22:
            case 0x27:
            case 0x3C:
            case 0x3D:
            case 0x60:
                lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->token->end,
                                             LXB_HTML_TOKENIZER_ERROR_UNCHINUNATVA);
                break;

            default:
                break;
        }

        data++;
    }

    lxb_html_tokenizer_state_append_data_m(tkz, data);

    return data;
}

/*
 * 12.2.5.39 After attribute value (quoted) state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_after_attribute_value_quoted(lxb_html_tokenizer_t *tkz,
                                                      const lxb_char_t *data,
                                                      const lxb_char_t *end)
{
    switch (*data) {
        /*
         * U+0009 CHARACTER TABULATION (tab)
         * U+000A LINE FEED (LF)
         * U+000C FORM FEED (FF)
         * U+000D CARRIAGE RETURN (CR)
         * U+0020 SPACE
         */
        case 0x09:
        case 0x0A:
        case 0x0C:
        case 0x0D:
        case 0x20:
            tkz->state = lxb_html_tokenizer_state_before_attribute_name;

            return (data + 1);

        /* U+002F SOLIDUS (/) */
        case 0x2F:
            tkz->state = lxb_html_tokenizer_state_self_closing_start_tag;

            return (data + 1);

        /* U+003E GREATER-THAN SIGN (>) */
        case 0x3E:
            tkz->state = lxb_html_tokenizer_state_data_before;

            lxb_html_tokenizer_state_token_done_m(tkz, end);

            return (data + 1);

        /* EOF */
        case 0x00:
            if (tkz->is_eof) {
                lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->last,
                                             LXB_HTML_TOKENIZER_ERROR_EOINTA);
                return end;
            }
            /* fall through */

        default:
            lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                         LXB_HTML_TOKENIZER_ERROR_MIWHBEAT);

            tkz->state = lxb_html_tokenizer_state_before_attribute_name;

            return data;
    }

    return data;
}


const lxb_char_t *
lxb_html_tokenizer_state_cr(lxb_html_tokenizer_t *tkz, const lxb_char_t *data,
                            const lxb_char_t *end)
{
    lxb_html_tokenizer_state_append_m(tkz, "\n", 1);

    if (*data == 0x0A) {
        data++;
    }

    tkz->state = tkz->state_return;

    return data;
}

/*
 * 12.2.5.40 Self-closing start tag state
 */
const lxb_char_t *
lxb_html_tokenizer_state_self_closing_start_tag(lxb_html_tokenizer_t *tkz,
                                                const lxb_char_t *data,
                                                const lxb_char_t *end)
{
    switch (*data) {
        /* U+003E GREATER-THAN SIGN (>) */
        case 0x3E:
            tkz->state = lxb_html_tokenizer_state_data_before;
            tkz->token->type |= LXB_HTML_TOKEN_TYPE_CLOSE_SELF;

            lxb_html_tokenizer_state_token_done_m(tkz, end);

            return (data + 1);

        /* EOF */
        case 0x00:
            if (tkz->is_eof) {
                lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->token->end,
                                             LXB_HTML_TOKENIZER_ERROR_EOINTA);
                return end;
            }
            /* fall through */

        default:
            lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                         LXB_HTML_TOKENIZER_ERROR_UNSOINTA);

            tkz->state = lxb_html_tokenizer_state_before_attribute_name;

            return data;
    }

    return data;
}

/*
 * Helper function. No in the specification. For 12.2.5.41 Bogus comment state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_bogus_comment_before(lxb_html_tokenizer_t *tkz,
                                              const lxb_char_t *data,
                                              const lxb_char_t *end)
{
    tkz->token->tag_id = LXB_TAG__EM_COMMENT;

    tkz->state = lxb_html_tokenizer_state_bogus_comment;

    return data;
}

/*
 * 12.2.5.41 Bogus comment state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_bogus_comment(lxb_html_tokenizer_t *tkz,
                                       const lxb_char_t *data,
                                       const lxb_char_t *end)
{
    lxb_html_tokenizer_state_begin_set(tkz, data);

    while (data != end) {
        switch (*data) {
            /* U+003E GREATER-THAN SIGN (>) */
            case 0x3E:
                tkz->state = lxb_html_tokenizer_state_data_before;

                lxb_html_tokenizer_state_append_data_m(tkz, data);
                lxb_html_tokenizer_state_token_set_end(tkz, data);
                lxb_html_tokenizer_state_set_text(tkz);
                lxb_html_tokenizer_state_token_done_wo_check_m(tkz, end);

                return (data + 1);

            /* U+000D CARRIAGE RETURN (CR) */
            case 0x0D:
                if (++data >= end) {
                    lxb_html_tokenizer_state_append_data_m(tkz, data - 1);

                    tkz->state = lxb_html_tokenizer_state_cr;
                    tkz->state_return = lxb_html_tokenizer_state_bogus_comment;

                    return data;
                }

                lxb_html_tokenizer_state_append_data_m(tkz, data);
                tkz->pos[-1] = 0x0A;

                lxb_html_tokenizer_state_begin_set(tkz, data + 1);

                if (*data != 0x0A) {
                    lxb_html_tokenizer_state_begin_set(tkz, data);
                    data--;
                }

                break;

            /*
             * EOF
             * U+0000 NULL
             */
            case 0x00:
                lxb_html_tokenizer_state_append_data_m(tkz, data);

                if (tkz->is_eof) {
                    if (tkz->token->begin != NULL) {
                        lxb_html_tokenizer_state_token_set_end_oef(tkz);
                    }

                    lxb_html_tokenizer_state_set_text(tkz);
                    lxb_html_tokenizer_state_token_done_wo_check_m(tkz, end);

                    return end;
                }

                lxb_html_tokenizer_state_append_replace_m(tkz);
                lxb_html_tokenizer_state_begin_set(tkz, data + 1);

                lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                             LXB_HTML_TOKENIZER_ERROR_UNNUCH);
                break;
        }

        data++;
    }

    lxb_html_tokenizer_state_append_data_m(tkz, data);

    return data;
}

/*
 * 12.2.5.42 Markup declaration open state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_markup_declaration_open(lxb_html_tokenizer_t *tkz,
                                                 const lxb_char_t *data,
                                                 const lxb_char_t *end)
{
    /* Check first char for change parse state */
    if (tkz->is_eof == false) {
        lxb_html_tokenizer_state_token_set_begin(tkz, data);
    }

    /* U+002D HYPHEN-MINUS characters (-) */
    if (*data == 0x2D) {
        if ((end - data) < 2) {
            tkz->state = lxb_html_tokenizer_state_markup_declaration_comment;
            return (data + 1);
        }

        if (data[1] == 0x2D) {
            tkz->state = lxb_html_tokenizer_state_comment_before_start;
            return (data + 2);
        }
    }
    /*
     * ASCII case-insensitive match for the word "DOCTYPE"
     * U+0044 character (D) or U+0064 character (d)
     */
    else if (*data == 0x44 || *data == 0x64) {
        if ((end - data) < 7) {
            tkz->markup = (lxb_char_t *) "doctype";

            tkz->state = lxb_html_tokenizer_state_markup_declaration_doctype;
            return data;
        }

        if (lexbor_str_data_ncasecmp((lxb_char_t *) "doctype", data, 7)) {
            tkz->state = lxb_html_tokenizer_state_doctype_before;
            return (data + 7);
        }
    }
    /* Case-sensitive match for the string "[CDATA["
     * (the five uppercase letters "CDATA" with a U+005B LEFT SQUARE BRACKET
     * character before and after)
     */
    else if (*data == 0x5B) {
        if ((end - data) < 7) {
            tkz->markup = (lxb_char_t *) "[CDATA[";

            tkz->state = lxb_html_tokenizer_state_markup_declaration_cdata;
            return data;
        }

        if (lexbor_str_data_ncmp((lxb_char_t *) "[CDATA[", data, 7)) {
            lxb_ns_id_t ns = lxb_html_tokenizer_current_namespace(tkz);

            if (ns != LXB_NS_HTML && ns != LXB_NS__UNDEF) {
                data += 7;

                lxb_html_tokenizer_state_token_set_begin(tkz, data);

                tkz->state = lxb_html_tokenizer_state_cdata_section_before;

                return data;
            }

            tkz->state = lxb_html_tokenizer_state_bogus_comment_before;

            return data;
        }
    }

    if (tkz->is_eof) {
        lxb_html_tokenizer_state_token_set_end_oef(tkz);

        tkz->token->begin = tkz->token->end;
    }

    lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                 LXB_HTML_TOKENIZER_ERROR_INOPCO);

    tkz->state = lxb_html_tokenizer_state_bogus_comment_before;

    return data;
}

/*
 * Helper function. No in the specification. For 12.2.5.42
 * For a comment tag <!--
 */
static const lxb_char_t *
lxb_html_tokenizer_state_markup_declaration_comment(lxb_html_tokenizer_t *tkz,
                                                    const lxb_char_t *data,
                                                    const lxb_char_t *end)
{
    /* U+002D HYPHEN-MINUS characters (-) */
    if (*data == 0x2D) {
        tkz->state = lxb_html_tokenizer_state_comment_before_start;
        return (data + 1);
    }

    lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                 LXB_HTML_TOKENIZER_ERROR_INOPCO);

    tkz->state = lxb_html_tokenizer_state_bogus_comment_before;
    return data;
}

/*
 * Helper function. No in the specification. For 12.2.5.42
 * For a DOCTYPE tag <!DOCTYPE
 */
static const lxb_char_t *
lxb_html_tokenizer_state_markup_declaration_doctype(lxb_html_tokenizer_t *tkz,
                                                    const lxb_char_t *data,
                                                    const lxb_char_t *end)
{
    const lxb_char_t *pos;
    pos = lexbor_str_data_ncasecmp_first(tkz->markup, data, (end - data));

    if (pos == NULL) {
        lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                     LXB_HTML_TOKENIZER_ERROR_INOPCO);

        tkz->state = lxb_html_tokenizer_state_bogus_comment_before;
        return data;
    }

    if (*pos == '\0') {
        data = (data + (pos - tkz->markup));

        tkz->state = lxb_html_tokenizer_state_doctype_before;
        return data;
    }

    tkz->markup = pos;

    return end;
}

/*
 * Helper function. No in the specification. For 12.2.5.42
 * For a CDATA tag <![CDATA[
 */
static const lxb_char_t *
lxb_html_tokenizer_state_markup_declaration_cdata(lxb_html_tokenizer_t *tkz,
                                                  const lxb_char_t *data,
                                                  const lxb_char_t *end)
{
    const lxb_char_t *pos;
    pos = lexbor_str_data_ncasecmp_first(tkz->markup, data, (end - data));

    if (pos == NULL) {
        lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                     LXB_HTML_TOKENIZER_ERROR_INOPCO);

        tkz->state = lxb_html_tokenizer_state_bogus_comment_before;
        return data;
    }

    if (*pos == '\0') {
        lxb_ns_id_t ns = lxb_html_tokenizer_current_namespace(tkz);

        if (ns != LXB_NS_HTML && ns != LXB_NS__UNDEF) {
            data = (data + (pos - tkz->markup));

            tkz->state = lxb_html_tokenizer_state_cdata_section_before;
            return data;
        }

        lxb_html_tokenizer_state_append_m(tkz, "[CDATA", 6);

        tkz->state = lxb_html_tokenizer_state_bogus_comment_before;
        return data;
    }

    tkz->markup = pos;

    return end;
}

/*
 * Helper function. No in the specification. For 12.2.5.69
 */
static const lxb_char_t *
lxb_html_tokenizer_state_cdata_section_before(lxb_html_tokenizer_t *tkz,
                                              const lxb_char_t *data,
                                              const lxb_char_t *end)
{
    if (tkz->is_eof == false) {
        lxb_html_tokenizer_state_token_set_begin(tkz, data);
    }
    else {
        lxb_html_tokenizer_state_token_set_begin(tkz, tkz->last);
    }

    tkz->token->tag_id = LXB_TAG__TEXT;

    tkz->state = lxb_html_tokenizer_state_cdata_section;

    return data;
}

/*
 * 12.2.5.69 CDATA section state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_cdata_section(lxb_html_tokenizer_t *tkz,
                                       const lxb_char_t *data,
                                       const lxb_char_t *end)
{
    lxb_html_tokenizer_state_begin_set(tkz, data);

    while (data != end) {
        switch (*data) {
            /* U+005D RIGHT SQUARE BRACKET (]) */
            case 0x5D:
                lxb_html_tokenizer_state_append_data_m(tkz, data);
                lxb_html_tokenizer_state_token_set_end(tkz, data);

                tkz->state = lxb_html_tokenizer_state_cdata_section_bracket;
                return (data + 1);

            /* U+000D CARRIAGE RETURN (CR) */
            case 0x0D:
                if (++data >= end) {
                    lxb_html_tokenizer_state_append_data_m(tkz, data - 1);

                    tkz->state = lxb_html_tokenizer_state_cr;
                    tkz->state_return = lxb_html_tokenizer_state_cdata_section;

                    return data;
                }

                lxb_html_tokenizer_state_append_data_m(tkz, data);
                tkz->pos[-1] = 0x0A;

                lxb_html_tokenizer_state_begin_set(tkz, data + 1);

                if (*data != 0x0A) {
                    lxb_html_tokenizer_state_begin_set(tkz, data);
                    data--;
                }

                break;

            /* EOF */
            case 0x00:
                if (tkz->is_eof) {
                    lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->last,
                                                 LXB_HTML_TOKENIZER_ERROR_EOINCD);

                    if (tkz->token->begin != NULL) {
                        lxb_html_tokenizer_state_append_data_m(tkz, data);
                        lxb_html_tokenizer_state_token_set_end_oef(tkz);
                    }

                    lxb_html_tokenizer_state_set_text(tkz);
                    lxb_html_tokenizer_state_token_done_m(tkz, end);

                    return end;
                }

                if (SIZE_MAX - tkz->token->null_count < 1) {
                    tkz->status = LXB_STATUS_ERROR_OVERFLOW;
                    return end;
                }

                tkz->token->null_count++;

                break;

            default:
                break;
        }

        data++;
    }

    lxb_html_tokenizer_state_append_data_m(tkz, data);

    return data;
}

/*
 * 12.2.5.70 CDATA section bracket state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_cdata_section_bracket(lxb_html_tokenizer_t *tkz,
                                               const lxb_char_t *data,
                                               const lxb_char_t *end)
{
    /* U+005D RIGHT SQUARE BRACKET (]) */
    if (*data == 0x5D) {
        tkz->state = lxb_html_tokenizer_state_cdata_section_end;
        return (data + 1);
    }

    lxb_html_tokenizer_state_append_m(tkz, "]", 1);

    tkz->state = lxb_html_tokenizer_state_cdata_section;

    return data;
}

/*
 * 12.2.5.71 CDATA section end state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_cdata_section_end(lxb_html_tokenizer_t *tkz,
                                           const lxb_char_t *data,
                                           const lxb_char_t *end)
{
    /* U+005D RIGHT SQUARE BRACKET (]) */
    if (*data == 0x5D) {
        lxb_html_tokenizer_state_append_m(tkz, data, 1);
        return (data + 1);
    }
    /* U+003E GREATER-THAN SIGN character */
    else if (*data == 0x3E) {
        tkz->state = lxb_html_tokenizer_state_data_before;

        lxb_html_tokenizer_state_set_text(tkz);
        lxb_html_tokenizer_state_token_done_m(tkz, end);

        return (data + 1);
    }

    lxb_html_tokenizer_state_append_m(tkz, "]]", 2);

    tkz->state = lxb_html_tokenizer_state_cdata_section;

    return data;
}

/*
 * 12.2.5.72 Character reference state
 */
const lxb_char_t *
lxb_html_tokenizer_state_char_ref(lxb_html_tokenizer_t *tkz,
                                  const lxb_char_t *data, const lxb_char_t *end)
{
    tkz->is_attribute = false;

    return _lxb_html_tokenizer_state_char_ref(tkz, data, end);
}

static const lxb_char_t *
lxb_html_tokenizer_state_char_ref_attr(lxb_html_tokenizer_t *tkz,
                                       const lxb_char_t *data,
                                       const lxb_char_t *end)
{
    tkz->is_attribute = true;

    return _lxb_html_tokenizer_state_char_ref(tkz, data, end);
}

static const lxb_char_t *
_lxb_html_tokenizer_state_char_ref(lxb_html_tokenizer_t *tkz,
                                   const lxb_char_t *data,
                                   const lxb_char_t *end)
{
    /* ASCII alphanumeric */
    if (lexbor_str_res_alphanumeric_character[ *data ] != LEXBOR_STR_RES_SLIP) {
        tkz->entity = &lxb_html_tokenizer_res_entities_sbst[1];
        tkz->entity_match = NULL;
        tkz->entity_start = (tkz->pos - 1) - tkz->start;

        tkz->state = lxb_html_tokenizer_state_char_ref_named;

        return data;
    }
    /* U+0023 NUMBER SIGN (#) */
    else if (*data == 0x23) {
        tkz->markup = data;
        tkz->entity_start = (tkz->pos - 1) - tkz->start;

        lxb_html_tokenizer_state_append_m(tkz, data, 1);

        tkz->state = lxb_html_tokenizer_state_char_ref_numeric;

        return (data + 1);
    }
    else {
        tkz->state = tkz->state_return;
    }

    return data;
}

/*
 * 12.2.5.73 Named character reference state
 *
 * The slowest part in HTML parsing!!!
 *
 * This option works correctly and passes all tests (stream parsing too).
 * We must seriously think about how to accelerate this part.
 */
static const lxb_char_t *
lxb_html_tokenizer_state_char_ref_named(lxb_html_tokenizer_t *tkz,
                                        const lxb_char_t *data,
                                        const lxb_char_t *end)
{
    size_t size, tail_size;
    lxb_char_t *start;
    const lexbor_sbst_entry_static_t *entry = tkz->entity;

    const lxb_char_t *begin = data;

    while (data < end) {
        entry = lexbor_sbst_entry_static_find(lxb_html_tokenizer_res_entities_sbst,
                                              entry, *data);
        if (entry == NULL) {
            lxb_html_tokenizer_state_append_m(tkz, begin, (data - begin));
            goto done;
        }

        if (entry->value[0] != 0) {
            tkz->entity_end = (tkz->pos + (data - begin)) - tkz->start;
            tkz->entity_match = entry;
        }

        entry = &lxb_html_tokenizer_res_entities_sbst[ entry->next ];

        data++;
    }

    /* If entry not NULL and buffer empty, then wait next buffer. */
    tkz->entity = entry;

    lxb_html_tokenizer_state_append_m(tkz, begin, (end - begin));
    return data;

done:

    /* If we have bad entity */
    if (tkz->entity_match == NULL) {
        tkz->state = lxb_html_tokenizer_state_char_ref_ambiguous_ampersand;

        return data;
    }

    tkz->state = tkz->state_return;

    /*
     * If the character reference was consumed as part of an attribute,
     * and the last character matched is not a U+003B SEMICOLON character (;),
     * and the next input character is either a U+003D EQUALS SIGN character (=)
     * or an ASCII alphanumeric, then, for historical reasons,
     * flush code points consumed as a character reference
     * and switch to the return state.
     */
    /* U+003B SEMICOLON character (;) */
    if (tkz->is_attribute && tkz->entity_match->key != 0x3B) {
        /* U+003D EQUALS SIGN character (=) or ASCII alphanumeric */
        if (*data == 0x3D
            || lexbor_str_res_alphanumeric_character[*data] != LEXBOR_STR_RES_SLIP)
        {
            return data;
        }
    }

    if (tkz->entity_match->key != 0x3B) {
        lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                     LXB_HTML_TOKENIZER_ERROR_MISEAFCHRE);
    }

    start = &tkz->start[tkz->entity_start];

    size = tkz->pos - start;
    tail_size = tkz->pos - &tkz->start[tkz->entity_end] - 1;

    if (tail_size != 0) {
        if ((size + tail_size) + start > tkz->end) {
            if (lxb_html_tokenizer_temp_realloc(tkz, size) != LXB_STATUS_OK) {
                return end;
            }
            start = &tkz->start[tkz->entity_start];
        }

        memmove(start + tkz->entity_match->value_len,
                tkz->pos - tail_size, tail_size);
    }

    memcpy(start, tkz->entity_match->value, tkz->entity_match->value_len);

    tkz->pos = start + (tkz->entity_match->value_len + tail_size);

    return data;
}

/*
 * 12.2.5.74 Ambiguous ampersand state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_char_ref_ambiguous_ampersand(lxb_html_tokenizer_t *tkz,
                                                      const lxb_char_t *data,
                                                      const lxb_char_t *end)
{
    /* ASCII alphanumeric */
    /* Skipped, not need */

    /* U+003B SEMICOLON (;) */
    if (*data == 0x3B) {
        lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                     LXB_HTML_TOKENIZER_ERROR_UNNACHRE);
    }

    tkz->state = tkz->state_return;

    return data;
}

/*
 * 12.2.5.75 Numeric character reference state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_char_ref_numeric(lxb_html_tokenizer_t *tkz,
                                          const lxb_char_t *data,
                                          const lxb_char_t *end)
{
    tkz->entity_number = 0;

    /*
     * U+0078 LATIN SMALL LETTER X
     * U+0058 LATIN CAPITAL LETTER X
     */
    if (*data == 0x78 || *data == 0x58) {
        lxb_html_tokenizer_state_append_m(tkz, data, 1);

        tkz->state = lxb_html_tokenizer_state_char_ref_hexademical_start;

        return (data + 1);
    }

    tkz->state = lxb_html_tokenizer_state_char_ref_decimal_start;

    return data;
}

/*
 * 12.2.5.76 Hexademical character reference start state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_char_ref_hexademical_start(lxb_html_tokenizer_t *tkz,
                                                    const lxb_char_t *data,
                                                    const lxb_char_t *end)
{
    /* ASCII hex digit */
    if (lexbor_str_res_map_hex[ *data ] != LEXBOR_STR_RES_SLIP) {
        tkz->state = lxb_html_tokenizer_state_char_ref_hexademical;
    }
    else {
        lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                     LXB_HTML_TOKENIZER_ERROR_ABOFDIINNUCHRE);

        tkz->state = tkz->state_return;
    }

    return data;
}

/*
 * 12.2.5.77 Decimal character reference start state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_char_ref_decimal_start(lxb_html_tokenizer_t *tkz,
                                                const lxb_char_t *data,
                                                const lxb_char_t *end)
{
    /* ASCII digit */
    if (lexbor_str_res_map_num[ *data ] != LEXBOR_STR_RES_SLIP) {
        tkz->state = lxb_html_tokenizer_state_char_ref_decimal;
    }
    else {
        lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                     LXB_HTML_TOKENIZER_ERROR_ABOFDIINNUCHRE);

        tkz->state = tkz->state_return;
    }

    return data;
}

/*
 * 12.2.5.78 Hexademical character reference state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_char_ref_hexademical(lxb_html_tokenizer_t *tkz,
                                              const lxb_char_t *data,
                                              const lxb_char_t *end)
{
    while (data != end) {
        if (lexbor_str_res_map_hex[ *data ] == LEXBOR_STR_RES_SLIP) {
            tkz->state = tkz->state_return;

            if (*data == ';') {
                data++;
            }

            return lxb_html_tokenizer_state_char_ref_numeric_end(tkz, data, end);
        }

        if (tkz->entity_number <= 0x10FFFF) {
            tkz->entity_number <<= 4;
            tkz->entity_number |= lexbor_str_res_map_hex[ *data ];
        }

        data++;
    }

    return data;
}

/*
 * 12.2.5.79 Decimal character reference state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_char_ref_decimal(lxb_html_tokenizer_t *tkz,
                                          const lxb_char_t *data,
                                          const lxb_char_t *end)
{
    while (data != end) {
        if (lexbor_str_res_map_num[ *data ] == LEXBOR_STR_RES_SLIP) {
            tkz->state = tkz->state_return;

            if (*data == ';') {
                data++;
            }

            return lxb_html_tokenizer_state_char_ref_numeric_end(tkz, data, end);
        }

        if (tkz->entity_number <= 0x10FFFF) {
            tkz->entity_number = lexbor_str_res_map_num[ *data ]
                                 + tkz->entity_number * 10;
        }

        data++;
    }

    return data;
}

/*
 * 12.2.5.80 Numeric character reference end state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_char_ref_numeric_end(lxb_html_tokenizer_t *tkz,
                                              const lxb_char_t *data,
                                              const lxb_char_t *end)
{
    lxb_char_t *start = &tkz->start[tkz->entity_start];

    if ((start + 4) > tkz->end) {
        if(lxb_html_tokenizer_temp_realloc(tkz, 4)) {
            return end;
        }

        start = &tkz->start[tkz->entity_start];
    }

    if (tkz->entity_number == 0x00) {
        lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->markup,
                                     LXB_HTML_TOKENIZER_ERROR_NUCHRE);

        goto xFFFD;
    }
    else if (tkz->entity_number > 0x10FFFF) {
        lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->markup,
                                     LXB_HTML_TOKENIZER_ERROR_CHREOUUNRA);

        goto xFFFD;
    }
    else if (tkz->entity_number >= 0xD800 && tkz->entity_number <= 0xDFFF) {
        lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->markup,
                                     LXB_HTML_TOKENIZER_ERROR_SUCHRE);

        goto xFFFD;
    }
    else if (tkz->entity_number >= 0xFDD0 && tkz->entity_number <= 0xFDEF) {
        lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->markup,
                                     LXB_HTML_TOKENIZER_ERROR_NOCHRE);
    }

    switch (tkz->entity_number) {
        case 0xFFFE:  case 0xFFFF:  case 0x1FFFE: case 0x1FFFF: case 0x2FFFE:
        case 0x2FFFF: case 0x3FFFE: case 0x3FFFF: case 0x4FFFE: case 0x4FFFF:
        case 0x5FFFE: case 0x5FFFF: case 0x6FFFE: case 0x6FFFF: case 0x7FFFE:
        case 0x7FFFF: case 0x8FFFE: case 0x8FFFF: case 0x9FFFE: case 0x9FFFF:
        case 0xAFFFE: case 0xAFFFF: case 0xBFFFE: case 0xBFFFF: case 0xCFFFE:
        case 0xCFFFF: case 0xDFFFE: case 0xDFFFF: case 0xEFFFE: case 0xEFFFF:
        case 0xFFFFE: case 0xFFFFF:
        case 0x10FFFE:
        case 0x10FFFF:
            lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->markup,
                                         LXB_HTML_TOKENIZER_ERROR_NOCHRE);
            break;

        default:
            break;
    }

    if (tkz->entity_number <= 0x1F
        || (tkz->entity_number >= 0x7F && tkz->entity_number <= 0x9F))
    {
        lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->markup,
                                     LXB_HTML_TOKENIZER_ERROR_COCHRE);
    }

    if (tkz->entity_number <= 0x9F) {
        tkz->entity_number = (uint32_t) lexbor_str_res_replacement_character[tkz->entity_number];
    }

    start += lxb_html_tokenizer_state_to_ascii_utf_8(tkz->entity_number, start);

    tkz->pos = start;

    return data;

xFFFD:

    memcpy(start, lexbor_str_res_ansi_replacement_character,
           sizeof(lexbor_str_res_ansi_replacement_character) - 1);

    tkz->pos = start + sizeof(lexbor_str_res_ansi_replacement_character) - 1;

    return data;
}

static size_t
lxb_html_tokenizer_state_to_ascii_utf_8(size_t codepoint, lxb_char_t *data)
{
    /* 0x80 -- 10xxxxxx */
    /* 0xC0 -- 110xxxxx */
    /* 0xE0 -- 1110xxxx */
    /* 0xF0 -- 11110xxx */

    if (codepoint <= 0x0000007F) {
        /* 0xxxxxxx */
        data[0] = (char) codepoint;

        return 1;
    }
    else if (codepoint <= 0x000007FF) {
        /* 110xxxxx 10xxxxxx */
        data[0] = (char) (0xC0 | (codepoint >> 6  ));
        data[1] = (char) (0x80 | (codepoint & 0x3F));

        return 2;
    }
    else if (codepoint <= 0x0000FFFF) {
        /* 1110xxxx 10xxxxxx 10xxxxxx */
        data[0] = (char) (0xE0 | ((codepoint >> 12)));
        data[1] = (char) (0x80 | ((codepoint >> 6 ) & 0x3F));
        data[2] = (char) (0x80 | ( codepoint & 0x3F));

        return 3;
    }
    else if (codepoint <= 0x001FFFFF) {
        /* 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
        data[0] = (char) (0xF0 | ( codepoint >> 18));
        data[1] = (char) (0x80 | ((codepoint >> 12) & 0x3F));
        data[2] = (char) (0x80 | ((codepoint >> 6 ) & 0x3F));
        data[3] = (char) (0x80 | ( codepoint & 0x3F));

        return 4;
    }

    return 0;
}
