/*
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/tokenizer/state_doctype.h"
#include "lexbor/html/tokenizer/state.h"


#define LEXBOR_STR_RES_ANSI_REPLACEMENT_CHARACTER
#include "lexbor/core/str_res.h"


lxb_dom_attr_data_t *
lxb_dom_attr_local_name_append(lexbor_hash_t *hash,
                               const lxb_char_t *name, size_t length);


static const lxb_char_t *
lxb_html_tokenizer_state_doctype(lxb_html_tokenizer_t *tkz,
                                 const lxb_char_t *data,
                                 const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_doctype_before_name(lxb_html_tokenizer_t *tkz,
                                             const lxb_char_t *data,
                                             const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_doctype_name(lxb_html_tokenizer_t *tkz,
                                      const lxb_char_t *data,
                                      const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_doctype_after_name(lxb_html_tokenizer_t *tkz,
                                            const lxb_char_t *data,
                                            const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_doctype_after_name_public(lxb_html_tokenizer_t *tkz,
                                                   const lxb_char_t *data,
                                                   const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_doctype_after_name_system(lxb_html_tokenizer_t *tkz,
                                                   const lxb_char_t *data,
                                                   const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_doctype_after_public_keyword(lxb_html_tokenizer_t *tkz,
                                                      const lxb_char_t *data,
                                                      const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_doctype_before_public_identifier(
                                                      lxb_html_tokenizer_t *tkz,
                                                      const lxb_char_t *data,
                                                      const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_doctype_public_identifier_double_quoted(
                                                      lxb_html_tokenizer_t *tkz,
                                                      const lxb_char_t *data,
                                                      const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_doctype_public_identifier_single_quoted(
                                                      lxb_html_tokenizer_t *tkz,
                                                      const lxb_char_t *data,
                                                      const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_doctype_after_public_identifier(
                                                      lxb_html_tokenizer_t *tkz,
                                                      const lxb_char_t *data,
                                                      const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_doctype_between_public_and_system_identifiers(
                                                      lxb_html_tokenizer_t *tkz,
                                                      const lxb_char_t *data,
                                                      const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_doctype_after_system_keyword(lxb_html_tokenizer_t *tkz,
                                                      const lxb_char_t *data,
                                                      const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_doctype_before_system_identifier(
                                                      lxb_html_tokenizer_t *tkz,
                                                      const lxb_char_t *data,
                                                      const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_doctype_system_identifier_double_quoted(
                                                      lxb_html_tokenizer_t *tkz,
                                                      const lxb_char_t *data,
                                                      const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_doctype_system_identifier_single_quoted(
                                                      lxb_html_tokenizer_t *tkz,
                                                      const lxb_char_t *data,
                                                      const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_doctype_after_system_identifier(
                                                      lxb_html_tokenizer_t *tkz,
                                                      const lxb_char_t *data,
                                                      const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_doctype_bogus(lxb_html_tokenizer_t *tkz,
                                       const lxb_char_t *data,
                                       const lxb_char_t *end);


/*
 * Helper function. No in the specification. For 12.2.5.53
 */
const lxb_char_t *
lxb_html_tokenizer_state_doctype_before(lxb_html_tokenizer_t *tkz,
                                        const lxb_char_t *data,
                                        const lxb_char_t *end)
{
    if (tkz->is_eof == false) {
        lxb_html_tokenizer_state_token_set_end(tkz, data);
    }
    else {
        lxb_html_tokenizer_state_token_set_end_oef(tkz);
    }

    tkz->token->tag_id = LXB_TAG__EM_DOCTYPE;

    return lxb_html_tokenizer_state_doctype(tkz, data, end);
}

/*
 * 12.2.5.53 DOCTYPE state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_doctype(lxb_html_tokenizer_t *tkz,
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
            data++;
            break;

        /* U+003E GREATER-THAN SIGN (>) */
        case 0x3E:
            break;

        /* EOF */
        case 0x00:
            if (tkz->is_eof) {
                lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->last,
                                             LXB_HTML_TOKENIZER_ERROR_EOINDO);

                tkz->token->type |= LXB_HTML_TOKEN_TYPE_FORCE_QUIRKS;

                lxb_html_tokenizer_state_token_done_m(tkz, end);

                return end;
            }
            /* fall through */

        default:
            lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                         LXB_HTML_TOKENIZER_ERROR_MIWHBEDONA);
            break;
    }

    tkz->state = lxb_html_tokenizer_state_doctype_before_name;

    return data;
}

/*
 * 12.2.5.54 Before DOCTYPE name state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_doctype_before_name(lxb_html_tokenizer_t *tkz,
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

            /*
             * U+0000 NULL
             * EOF
             */
            case 0x00:
                if (tkz->is_eof) {
                    lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->last,
                                               LXB_HTML_TOKENIZER_ERROR_EOINDO);

                    tkz->token->type |= LXB_HTML_TOKEN_TYPE_FORCE_QUIRKS;

                    lxb_html_tokenizer_state_token_done_m(tkz, end);

                    return end;
                }

                lxb_html_tokenizer_state_token_attr_add_m(tkz, attr, end);
                lxb_html_tokenizer_state_token_attr_set_name_begin(tkz, data);
                lxb_html_tokenizer_state_append_replace_m(tkz);

                lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                             LXB_HTML_TOKENIZER_ERROR_UNNUCH);

                tkz->token->attr_last->type
                    |= LXB_HTML_TOKEN_ATTR_TYPE_NAME_NULL;

                tkz->state = lxb_html_tokenizer_state_doctype_name;

                return (data + 1);

            /* U+003E GREATER-THAN SIGN (>) */
            case 0x3E:
                tkz->token->type |= LXB_HTML_TOKEN_TYPE_FORCE_QUIRKS;
                tkz->state = lxb_html_tokenizer_state_data_before;

                lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                             LXB_HTML_TOKENIZER_ERROR_MIDONA);

                lxb_html_tokenizer_state_token_done_m(tkz, end);

                return (data + 1);

            /*
             * ASCII upper alpha
             * Anything else
             */
            default:
                lxb_html_tokenizer_state_token_attr_add_m(tkz, attr, end);
                lxb_html_tokenizer_state_token_attr_set_name_begin(tkz, data);

                tkz->state = lxb_html_tokenizer_state_doctype_name;

                return data;
        }

        data++;
    }

    return data;
}

/*
 * 12.2.5.55 DOCTYPE name state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_doctype_name(lxb_html_tokenizer_t *tkz,
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
             */
            case 0x09:
            case 0x0A:
            case 0x0C:
            case 0x0D:
            case 0x20:
                lxb_html_tokenizer_state_append_data_m(tkz, data);
                lxb_html_tokenizer_state_set_name_m(tkz);
                lxb_html_tokenizer_state_token_attr_set_name_end(tkz, data);

                tkz->state = lxb_html_tokenizer_state_doctype_after_name;

                return (data + 1);

            /* U+003E GREATER-THAN SIGN (>) */
            case 0x3E:
                tkz->state = lxb_html_tokenizer_state_data_before;

                lxb_html_tokenizer_state_append_data_m(tkz, data);
                lxb_html_tokenizer_state_set_name_m(tkz);
                lxb_html_tokenizer_state_token_attr_set_name_end(tkz, data);
                lxb_html_tokenizer_state_token_done_m(tkz, end);

                return (data + 1);

            /*
             * U+0000 NULL
             * EOF
             */
            case 0x00:
                lxb_html_tokenizer_state_append_data_m(tkz, data);

                if (tkz->is_eof) {
                    lxb_html_tokenizer_state_token_attr_set_name_end_oef(tkz);

                    lxb_html_tokenizer_error_add(tkz->parse_errors,
                                               tkz->token->attr_last->name_end,
                                               LXB_HTML_TOKENIZER_ERROR_EOINDO);

                    tkz->token->type |= LXB_HTML_TOKEN_TYPE_FORCE_QUIRKS;

                    lxb_html_tokenizer_state_set_name_m(tkz);
                    lxb_html_tokenizer_state_token_done_m(tkz, end);

                    return end;
                }

                lxb_html_tokenizer_state_begin_set(tkz, data + 1);
                lxb_html_tokenizer_state_append_replace_m(tkz);

                lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                             LXB_HTML_TOKENIZER_ERROR_UNNUCH);

                tkz->token->attr_last->type
                    |= LXB_HTML_TOKEN_ATTR_TYPE_NAME_NULL;

                break;

            /* Anything else */
            default:
                break;
        }

        data++;
    }

    lxb_html_tokenizer_state_append_data_m(tkz, data);

    return data;
}

/*
 * 12.2.5.56 After DOCTYPE name state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_doctype_after_name(lxb_html_tokenizer_t *tkz,
                                            const lxb_char_t *data,
                                            const lxb_char_t *end)
{
    lxb_html_token_attr_t *attr;
    const lxb_dom_attr_data_t *attr_data;

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

            /* U+003E GREATER-THAN SIGN (>) */
            case 0x3E:
                tkz->state = lxb_html_tokenizer_state_data_before;

                lxb_html_tokenizer_state_token_done_m(tkz, end);

                return (data + 1);

            /* EOF */
            case 0x00:
                if (tkz->is_eof) {
                    lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->last,
                                               LXB_HTML_TOKENIZER_ERROR_EOINDO);

                    tkz->token->type |= LXB_HTML_TOKEN_TYPE_FORCE_QUIRKS;

                    lxb_html_tokenizer_state_token_done_m(tkz, end);

                    return end;
                }
                /* fall through */

            /* Anything else */
            default:
                lxb_html_tokenizer_state_token_attr_add_m(tkz, attr, end);
                lxb_html_tokenizer_state_token_attr_set_name_begin(tkz, data);

                if ((data + 6) > end) {
                    /*
                     * ASCII case-insensitive match for the word "PUBLIC"
                     * U+0044 character (P) or U+0050 character (p)
                     */
                    if (*data == 0x50 || *data == 0x70) {
                        tkz->markup = (lxb_char_t *) "PUBLIC";

                        tkz->state =
                            lxb_html_tokenizer_state_doctype_after_name_public;

                        return data;
                    }

                    /*
                     * ASCII case-insensitive match for the word "SYSTEM"
                     * U+0044 character (S) or U+0053 character (s)
                     */
                    if (*data == 0x53 || *data == 0x73) {
                        tkz->markup = (lxb_char_t *) "SYSTEM";

                        tkz->state =
                            lxb_html_tokenizer_state_doctype_after_name_system;

                        return data;
                    }
                }
                else if (lexbor_str_data_ncasecmp((lxb_char_t *) "PUBLIC",
                                                  data, 6))
                {
                    lxb_html_tokenizer_state_token_attr_set_name_end(tkz,
                                                                    (data + 6));

                    attr_data = lxb_dom_attr_data_by_id(tkz->attrs,
                                                        LXB_DOM_ATTR_PUBLIC);
                    if (attr_data == NULL) {
                        tkz->status = LXB_STATUS_ERROR;
                        return end;
                    }

                    tkz->token->attr_last->name = attr_data;

                    tkz->state =
                        lxb_html_tokenizer_state_doctype_after_public_keyword;

                    return (data + 6);
                }
                else if (lexbor_str_data_ncasecmp((lxb_char_t *) "SYSTEM",
                                                  data, 6))
                {
                    lxb_html_tokenizer_state_token_attr_set_name_end(tkz,
                                                                    (data + 6));

                    attr_data = lxb_dom_attr_data_by_id(tkz->attrs,
                                                        LXB_DOM_ATTR_SYSTEM);
                    if (attr_data == NULL) {
                        tkz->status = LXB_STATUS_ERROR;
                        return end;
                    }

                    tkz->token->attr_last->name = attr_data;

                    tkz->state =
                        lxb_html_tokenizer_state_doctype_after_system_keyword;

                    return (data + 6);
                }

                lxb_html_token_attr_delete(tkz->token, attr,
                                           tkz->dobj_token_attr);

                lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                         LXB_HTML_TOKENIZER_ERROR_INCHSEAFDONA);

                tkz->token->type |= LXB_HTML_TOKEN_TYPE_FORCE_QUIRKS;
                tkz->state = lxb_html_tokenizer_state_doctype_bogus;

                return data;
        }

        data++;
    }

    return data;
}

/*
 * Helper function. No in the specification. For 12.2.5.56
 * For doctype PUBLIC
 */
static const lxb_char_t *
lxb_html_tokenizer_state_doctype_after_name_public(lxb_html_tokenizer_t *tkz,
                                                   const lxb_char_t *data,
                                                   const lxb_char_t *end)
{
    const lxb_char_t *pos;
    const lxb_dom_attr_data_t *attr_data;

    pos = lexbor_str_data_ncasecmp_first(tkz->markup, data, (end - data));

    if (pos == NULL) {
        lxb_html_token_attr_delete(tkz->token, tkz->token->attr_last,
                                   tkz->dobj_token_attr);

        lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                     LXB_HTML_TOKENIZER_ERROR_INCHSEAFDONA);

        tkz->state = lxb_html_tokenizer_state_doctype_bogus;

        return data;
    }

    if (*pos == '\0') {
        pos = data + (pos - tkz->markup);

        lxb_html_tokenizer_state_token_attr_set_name_end(tkz, pos);

        attr_data = lxb_dom_attr_data_by_id(tkz->attrs,
                                            LXB_DOM_ATTR_PUBLIC);
        if (attr_data == NULL) {
            tkz->status = LXB_STATUS_ERROR;
            return end;
        }

        tkz->token->attr_last->name = attr_data;

        tkz->state = lxb_html_tokenizer_state_doctype_after_public_keyword;

        return (pos + 1);
    }

    tkz->markup = pos;

    return end;
}

/*
 * Helper function. No in the specification. For 12.2.5.56
 * For doctype SYSTEM
 */
static const lxb_char_t *
lxb_html_tokenizer_state_doctype_after_name_system(lxb_html_tokenizer_t *tkz,
                                                   const lxb_char_t *data,
                                                   const lxb_char_t *end)
{
    const lxb_char_t *pos;
    const lxb_dom_attr_data_t *attr_data;

    pos = lexbor_str_data_ncasecmp_first(tkz->markup, data, (end - data));

    if (pos == NULL) {
        lxb_html_token_attr_delete(tkz->token, tkz->token->attr_last,
                                   tkz->dobj_token_attr);

        lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                     LXB_HTML_TOKENIZER_ERROR_INCHSEAFDONA);

        tkz->state = lxb_html_tokenizer_state_doctype_bogus;

        return data;
    }

    if (*pos == '\0') {
        pos = data + (pos - tkz->markup);

        lxb_html_tokenizer_state_token_attr_set_name_end(tkz, pos);

        attr_data = lxb_dom_attr_data_by_id(tkz->attrs,
                                            LXB_DOM_ATTR_SYSTEM);
        if (attr_data == NULL) {
            tkz->status = LXB_STATUS_ERROR;
            return end;
        }

        tkz->token->attr_last->name = attr_data;

        tkz->state = lxb_html_tokenizer_state_doctype_after_system_keyword;

        return (pos + 1);
    }

    tkz->markup = pos;

    return end;
}

/*
 * 12.2.5.57 After DOCTYPE public keyword state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_doctype_after_public_keyword(lxb_html_tokenizer_t *tkz,
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
            tkz->state =
                lxb_html_tokenizer_state_doctype_before_public_identifier;

            return (data + 1);

        /* U+0022 QUOTATION MARK (") */
        case 0x22:
            lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                         LXB_HTML_TOKENIZER_ERROR_MIWHAFDOPUKE);

            tkz->state =
               lxb_html_tokenizer_state_doctype_public_identifier_double_quoted;

            return (data + 1);

        /* U+0027 APOSTROPHE (') */
        case 0x27:
            lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                         LXB_HTML_TOKENIZER_ERROR_MIWHAFDOPUKE);

            tkz->state =
               lxb_html_tokenizer_state_doctype_public_identifier_single_quoted;

            return (data + 1);

        /* U+003E GREATER-THAN SIGN (>) */
        case 0x3E:
            tkz->token->type |= LXB_HTML_TOKEN_TYPE_FORCE_QUIRKS;
            tkz->state = lxb_html_tokenizer_state_data_before;

            lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                         LXB_HTML_TOKENIZER_ERROR_MIDOPUID);

            lxb_html_tokenizer_state_token_done_m(tkz, end);

            return (data + 1);

        /* EOF */
        case 0x00:
            if (tkz->is_eof) {
                tkz->token->type |= LXB_HTML_TOKEN_TYPE_FORCE_QUIRKS;

                lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->last,
                                             LXB_HTML_TOKENIZER_ERROR_EOINDO);

                lxb_html_tokenizer_state_token_done_m(tkz, end);

                return end;
            }
            /* fall through */

        default:
            tkz->token->type |= LXB_HTML_TOKEN_TYPE_FORCE_QUIRKS;
            tkz->state = lxb_html_tokenizer_state_doctype_bogus;

            lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                         LXB_HTML_TOKENIZER_ERROR_MIQUBEDOPUID);

            return data;
    }

    return data;
}

/*
 * 12.2.5.58 Before DOCTYPE public identifier state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_doctype_before_public_identifier(lxb_html_tokenizer_t *tkz,
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
            break;

        /* U+0022 QUOTATION MARK (") */
        case 0x22:
            tkz->state =
               lxb_html_tokenizer_state_doctype_public_identifier_double_quoted;

            break;

        /* U+0027 APOSTROPHE (') */
        case 0x27:
            tkz->state =
               lxb_html_tokenizer_state_doctype_public_identifier_single_quoted;

            break;

        /* U+003E GREATER-THAN SIGN (>) */
        case 0x3E:
            tkz->token->type |= LXB_HTML_TOKEN_TYPE_FORCE_QUIRKS;
            tkz->state = lxb_html_tokenizer_state_data_before;

            lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                         LXB_HTML_TOKENIZER_ERROR_MIDOPUID);

            lxb_html_tokenizer_state_token_done_m(tkz, end);

            break;

        /* EOF */
        case 0x00:
            if (tkz->is_eof) {
                lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->last,
                                             LXB_HTML_TOKENIZER_ERROR_EOINDO);

                tkz->token->type |= LXB_HTML_TOKEN_TYPE_FORCE_QUIRKS;

                lxb_html_tokenizer_state_token_done_m(tkz, end);

                return end;
            }
            /* fall through */

        default:
            lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                         LXB_HTML_TOKENIZER_ERROR_MIQUBEDOPUID);

            tkz->token->type |= LXB_HTML_TOKEN_TYPE_FORCE_QUIRKS;
            tkz->state = lxb_html_tokenizer_state_doctype_bogus;

            return data;
    }

    return (data + 1);
}

/*
 * 12.2.5.59 DOCTYPE public identifier (double-quoted) state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_doctype_public_identifier_double_quoted(lxb_html_tokenizer_t *tkz,
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
                lxb_html_tokenizer_state_set_value_m(tkz);
                lxb_html_tokenizer_state_token_attr_set_value_end(tkz, data);

                tkz->state =
                    lxb_html_tokenizer_state_doctype_after_public_identifier;

                return (data + 1);

            /* U+003E GREATER-THAN SIGN (>) */
            case 0x3E:
                tkz->state = lxb_html_tokenizer_state_data_before;

                lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                             LXB_HTML_TOKENIZER_ERROR_ABDOPUID);

                lxb_html_tokenizer_state_append_data_m(tkz, data);
                lxb_html_tokenizer_state_set_value_m(tkz);
                lxb_html_tokenizer_state_token_attr_set_value_end(tkz, data);
                lxb_html_tokenizer_state_token_done_m(tkz, end);

                return (data + 1);

            /* U+000D CARRIAGE RETURN (CR) */
            case 0x0D:
                if (++data >= end) {
                    lxb_html_tokenizer_state_append_data_m(tkz, data - 1);

                    tkz->state = lxb_html_tokenizer_state_cr;
                    tkz->state_return = lxb_html_tokenizer_state_doctype_public_identifier_double_quoted;

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
                    lxb_html_tokenizer_state_token_attr_set_value_end_oef(tkz);

                    if (tkz->token->attr_last->value_begin == NULL) {
                        lxb_html_tokenizer_state_token_attr_set_value_begin(tkz,
                                            tkz->token->attr_last->value_end);
                    }

                    lxb_html_tokenizer_error_add(tkz->parse_errors,
                                               tkz->token->attr_last->value_end,
                                               LXB_HTML_TOKENIZER_ERROR_EOINDO);

                    tkz->token->type |= LXB_HTML_TOKEN_TYPE_FORCE_QUIRKS;

                    lxb_html_tokenizer_state_set_value_m(tkz);
                    lxb_html_tokenizer_state_token_done_m(tkz, end);

                    return end;
                }

                lxb_html_tokenizer_state_begin_set(tkz, data + 1);
                lxb_html_tokenizer_state_append_replace_m(tkz);

                lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                             LXB_HTML_TOKENIZER_ERROR_UNNUCH);

                tkz->token->attr_last->type
                    |= LXB_HTML_TOKEN_ATTR_TYPE_VALUE_NULL;

                break;

            /* Anything else */
            default:
                break;
        }

        data++;
    }

    lxb_html_tokenizer_state_append_data_m(tkz, data);

    return data;
}

/*
 * 12.2.5.60 DOCTYPE public identifier (single-quoted) state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_doctype_public_identifier_single_quoted(lxb_html_tokenizer_t *tkz,
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
                lxb_html_tokenizer_state_set_value_m(tkz);
                lxb_html_tokenizer_state_token_attr_set_value_end(tkz, data);

                tkz->state =
                    lxb_html_tokenizer_state_doctype_after_public_identifier;

                return (data + 1);

            /* U+003E GREATER-THAN SIGN (>) */
            case 0x3E:
                tkz->state = lxb_html_tokenizer_state_data_before;

                lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                             LXB_HTML_TOKENIZER_ERROR_ABDOPUID);

                lxb_html_tokenizer_state_append_data_m(tkz, data);
                lxb_html_tokenizer_state_set_value_m(tkz);
                lxb_html_tokenizer_state_token_attr_set_value_end(tkz, data);
                lxb_html_tokenizer_state_token_done_m(tkz, end);

                return (data + 1);

            /* U+000D CARRIAGE RETURN (CR) */
            case 0x0D:
                if (++data >= end) {
                    lxb_html_tokenizer_state_append_data_m(tkz, data - 1);

                    tkz->state = lxb_html_tokenizer_state_cr;
                    tkz->state_return = lxb_html_tokenizer_state_doctype_public_identifier_single_quoted;

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
                    lxb_html_tokenizer_state_token_attr_set_value_end_oef(tkz);

                    if (tkz->token->attr_last->value_begin == NULL) {
                        lxb_html_tokenizer_state_token_attr_set_value_begin(tkz,
                                              tkz->token->attr_last->value_end);
                    }

                    lxb_html_tokenizer_error_add(tkz->parse_errors,
                                               tkz->token->attr_last->value_end,
                                               LXB_HTML_TOKENIZER_ERROR_EOINDO);

                    tkz->token->type |= LXB_HTML_TOKEN_TYPE_FORCE_QUIRKS;

                    lxb_html_tokenizer_state_set_value_m(tkz);
                    lxb_html_tokenizer_state_token_done_m(tkz, end);

                    return end;
                }

                lxb_html_tokenizer_state_begin_set(tkz, data + 1);
                lxb_html_tokenizer_state_append_replace_m(tkz);

                lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                             LXB_HTML_TOKENIZER_ERROR_UNNUCH);

                tkz->token->attr_last->type
                    |= LXB_HTML_TOKEN_ATTR_TYPE_VALUE_NULL;

                break;

            /* Anything else */
            default:
                break;
        }

        data++;
    }

    lxb_html_tokenizer_state_append_data_m(tkz, data);

    return data;
}

/*
 * 12.2.5.61 After DOCTYPE public identifier state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_doctype_after_public_identifier(lxb_html_tokenizer_t *tkz,
                                                         const lxb_char_t *data,
                                                         const lxb_char_t *end)
{
    lxb_html_token_attr_t *attr;

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
            tkz->state =
         lxb_html_tokenizer_state_doctype_between_public_and_system_identifiers;

            return (data + 1);

        /* U+003E GREATER-THAN SIGN (>) */
        case 0x3E:
            tkz->state = lxb_html_tokenizer_state_data_before;

            lxb_html_tokenizer_state_token_done_m(tkz, end);

            return (data + 1);

        /* U+0022 QUOTATION MARK (") */
        case 0x22:
            lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                     LXB_HTML_TOKENIZER_ERROR_MIWHBEDOPUANSYID);

            lxb_html_tokenizer_state_token_attr_add_m(tkz, attr, end);

            tkz->state =
               lxb_html_tokenizer_state_doctype_system_identifier_double_quoted;

            return (data + 1);

        /* U+0027 APOSTROPHE (') */
        case 0x27:
            lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                     LXB_HTML_TOKENIZER_ERROR_MIWHBEDOPUANSYID);

            lxb_html_tokenizer_state_token_attr_add_m(tkz, attr, end);

            tkz->state =
               lxb_html_tokenizer_state_doctype_system_identifier_single_quoted;

            return (data + 1);

        /* EOF */
        case 0x00:
            if (tkz->is_eof) {
                lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->last,
                                             LXB_HTML_TOKENIZER_ERROR_EOINDO);

                tkz->token->type |= LXB_HTML_TOKEN_TYPE_FORCE_QUIRKS;
                lxb_html_tokenizer_state_token_done_m(tkz, end);

                return end;
            }
            /* fall through */

        default:
            lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                         LXB_HTML_TOKENIZER_ERROR_MIQUBEDOSYID);

            tkz->token->type |= LXB_HTML_TOKEN_TYPE_FORCE_QUIRKS;
            tkz->state = lxb_html_tokenizer_state_doctype_bogus;

            return data;
    }

    return data;
}

/*
 * 12.2.5.62 Between DOCTYPE public and system identifiers state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_doctype_between_public_and_system_identifiers(lxb_html_tokenizer_t *tkz,
                                                                       const lxb_char_t *data,
                                                                       const lxb_char_t *end)
{
    lxb_html_token_attr_t *attr;

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
            return (data + 1);

        /* U+003E GREATER-THAN SIGN (>) */
        case 0x3E:
            tkz->state = lxb_html_tokenizer_state_data_before;

            lxb_html_tokenizer_state_token_done_m(tkz, end);

            return (data + 1);

        /* U+0022 QUOTATION MARK (") */
        case 0x22:
            lxb_html_tokenizer_state_token_attr_add_m(tkz, attr, end);

            tkz->state =
               lxb_html_tokenizer_state_doctype_system_identifier_double_quoted;

            return (data + 1);

        /* U+0027 APOSTROPHE (') */
        case 0x27:
            lxb_html_tokenizer_state_token_attr_add_m(tkz, attr, end);

            tkz->state =
               lxb_html_tokenizer_state_doctype_system_identifier_single_quoted;

            return (data + 1);

        /* EOF */
        case 0x00:
            if (tkz->is_eof) {
                lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->last,
                                             LXB_HTML_TOKENIZER_ERROR_EOINDO);

                tkz->token->type |= LXB_HTML_TOKEN_TYPE_FORCE_QUIRKS;
                lxb_html_tokenizer_state_token_done_m(tkz, end);

                return end;
            }
            /* fall through */

        default:
            lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                         LXB_HTML_TOKENIZER_ERROR_MIQUBEDOSYID);

            tkz->token->type |= LXB_HTML_TOKEN_TYPE_FORCE_QUIRKS;
            tkz->state = lxb_html_tokenizer_state_doctype_bogus;

            return data;
    }

    return data;
}

/*
 * 12.2.5.63 After DOCTYPE system keyword state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_doctype_after_system_keyword(lxb_html_tokenizer_t *tkz,
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
            tkz->state =
                lxb_html_tokenizer_state_doctype_before_system_identifier;

            return (data + 1);

        /* U+0022 QUOTATION MARK (") */
        case 0x22:
            lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                         LXB_HTML_TOKENIZER_ERROR_MIWHAFDOSYKE);

            tkz->state =
               lxb_html_tokenizer_state_doctype_system_identifier_double_quoted;

            return (data + 1);

        /* U+0027 APOSTROPHE (') */
        case 0x27:
            lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                         LXB_HTML_TOKENIZER_ERROR_MIWHAFDOSYKE);

            tkz->state =
               lxb_html_tokenizer_state_doctype_system_identifier_single_quoted;

            return (data + 1);

        /* U+003E GREATER-THAN SIGN (>) */
        case 0x3E:
            tkz->token->type |= LXB_HTML_TOKEN_TYPE_FORCE_QUIRKS;
            tkz->state = lxb_html_tokenizer_state_data_before;

            lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                         LXB_HTML_TOKENIZER_ERROR_MIDOSYID);

            lxb_html_tokenizer_state_token_done_m(tkz, end);

            return (data + 1);

        /* EOF */
        case 0x00:
            if (tkz->is_eof) {
                lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->last,
                                             LXB_HTML_TOKENIZER_ERROR_EOINDO);

                tkz->token->type |= LXB_HTML_TOKEN_TYPE_FORCE_QUIRKS;
                lxb_html_tokenizer_state_token_done_m(tkz, end);

                return end;
            }
            /* fall through */

        default:
            tkz->token->type |= LXB_HTML_TOKEN_TYPE_FORCE_QUIRKS;
            tkz->state = lxb_html_tokenizer_state_doctype_bogus;

            lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                         LXB_HTML_TOKENIZER_ERROR_MIQUBEDOSYID);

            return data;
    }

    return data;
}

/*
 * 12.2.5.64 Before DOCTYPE system identifier state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_doctype_before_system_identifier(lxb_html_tokenizer_t *tkz,
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
            return (data + 1);

        /* U+0022 QUOTATION MARK (") */
        case 0x22:
            tkz->state =
               lxb_html_tokenizer_state_doctype_system_identifier_double_quoted;

            return (data + 1);

        /* U+0027 APOSTROPHE (') */
        case 0x27:
            tkz->state =
               lxb_html_tokenizer_state_doctype_system_identifier_single_quoted;

            return (data + 1);

        /* U+003E GREATER-THAN SIGN (>) */
        case 0x3E:
            tkz->token->type |= LXB_HTML_TOKEN_TYPE_FORCE_QUIRKS;
            tkz->state = lxb_html_tokenizer_state_data_before;

            lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                         LXB_HTML_TOKENIZER_ERROR_MIDOSYID);

            lxb_html_tokenizer_state_token_done_m(tkz, end);

            return (data + 1);

        /* EOF */
        case 0x00:
            if (tkz->is_eof) {
                lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->last,
                                             LXB_HTML_TOKENIZER_ERROR_EOINDO);

                tkz->token->type |= LXB_HTML_TOKEN_TYPE_FORCE_QUIRKS;

                lxb_html_tokenizer_state_token_done_m(tkz, end);

                return end;
            }
            /* fall through */

        default:
            tkz->token->type |= LXB_HTML_TOKEN_TYPE_FORCE_QUIRKS;
            tkz->state = lxb_html_tokenizer_state_doctype_bogus;

            lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                         LXB_HTML_TOKENIZER_ERROR_MIQUBEDOSYID);

            return data;
    }

    return data;
}

/*
 * 12.2.5.65 DOCTYPE system identifier (double-quoted) state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_doctype_system_identifier_double_quoted(lxb_html_tokenizer_t *tkz,
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
                lxb_html_tokenizer_state_set_value_m(tkz);
                lxb_html_tokenizer_state_token_attr_set_value_end(tkz, data);

                tkz->state =
                    lxb_html_tokenizer_state_doctype_after_system_identifier;

                return (data + 1);

            /* U+003E GREATER-THAN SIGN (>) */
            case 0x3E:
                tkz->state = lxb_html_tokenizer_state_data_before;

                lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                             LXB_HTML_TOKENIZER_ERROR_ABDOSYID);

                lxb_html_tokenizer_state_append_data_m(tkz, data);
                lxb_html_tokenizer_state_set_value_m(tkz);
                lxb_html_tokenizer_state_token_attr_set_value_end(tkz, data);
                lxb_html_tokenizer_state_token_done_m(tkz, end);

                return (data + 1);

            /* U+000D CARRIAGE RETURN (CR) */
            case 0x0D:
                if (++data >= end) {
                    lxb_html_tokenizer_state_append_data_m(tkz, data - 1);

                    tkz->state = lxb_html_tokenizer_state_cr;
                    tkz->state_return = lxb_html_tokenizer_state_doctype_system_identifier_double_quoted;

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
                    lxb_html_tokenizer_state_token_attr_set_value_end_oef(tkz);

                    if (tkz->token->attr_last->value_begin == NULL) {
                        lxb_html_tokenizer_state_token_attr_set_value_begin(tkz,
                                              tkz->token->attr_last->value_end);
                    }

                    lxb_html_tokenizer_error_add(tkz->parse_errors,
                                               tkz->token->attr_last->value_end,
                                               LXB_HTML_TOKENIZER_ERROR_EOINDO);

                    tkz->token->type |= LXB_HTML_TOKEN_TYPE_FORCE_QUIRKS;

                    lxb_html_tokenizer_state_set_value_m(tkz);
                    lxb_html_tokenizer_state_token_done_m(tkz, end);

                    return end;
                }

                lxb_html_tokenizer_state_begin_set(tkz, data + 1);
                lxb_html_tokenizer_state_append_replace_m(tkz);

                lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                             LXB_HTML_TOKENIZER_ERROR_UNNUCH);

                tkz->token->attr_last->type
                    |= LXB_HTML_TOKEN_ATTR_TYPE_VALUE_NULL;

                break;

            /* Anything else */
            default:
                break;
        }

        data++;
    }

    lxb_html_tokenizer_state_append_data_m(tkz, data);

    return data;
}

/*
 * 12.2.5.66 DOCTYPE system identifier (single-quoted) state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_doctype_system_identifier_single_quoted(lxb_html_tokenizer_t *tkz,
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
                lxb_html_tokenizer_state_set_value_m(tkz);
                lxb_html_tokenizer_state_token_attr_set_value_end(tkz, data);

                tkz->state =
                    lxb_html_tokenizer_state_doctype_after_system_identifier;

                return (data + 1);

            /* U+003E GREATER-THAN SIGN (>) */
            case 0x3E:
                tkz->state = lxb_html_tokenizer_state_data_before;

                lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                             LXB_HTML_TOKENIZER_ERROR_ABDOSYID);

                lxb_html_tokenizer_state_append_data_m(tkz, data);
                lxb_html_tokenizer_state_set_value_m(tkz);
                lxb_html_tokenizer_state_token_attr_set_value_end(tkz, data);
                lxb_html_tokenizer_state_token_done_m(tkz, end);

                return (data + 1);

            /* U+000D CARRIAGE RETURN (CR) */
            case 0x0D:
                if (++data >= end) {
                    lxb_html_tokenizer_state_append_data_m(tkz, data - 1);

                    tkz->state = lxb_html_tokenizer_state_cr;
                    tkz->state_return = lxb_html_tokenizer_state_doctype_system_identifier_single_quoted;

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
                    lxb_html_tokenizer_state_token_attr_set_value_end_oef(tkz);

                    if (tkz->token->attr_last->value_begin == NULL) {
                        lxb_html_tokenizer_state_token_attr_set_value_begin(tkz,
                                              tkz->token->attr_last->value_end);
                    }

                    lxb_html_tokenizer_error_add(tkz->parse_errors,
                                               tkz->token->attr_last->value_end,
                                               LXB_HTML_TOKENIZER_ERROR_EOINDO);

                    tkz->token->type |= LXB_HTML_TOKEN_TYPE_FORCE_QUIRKS;

                    lxb_html_tokenizer_state_set_value_m(tkz);
                    lxb_html_tokenizer_state_token_done_m(tkz, end);

                    return end;
                }

                lxb_html_tokenizer_state_begin_set(tkz, data + 1);
                lxb_html_tokenizer_state_append_replace_m(tkz);

                lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                             LXB_HTML_TOKENIZER_ERROR_UNNUCH);

                tkz->token->attr_last->type
                    |= LXB_HTML_TOKEN_ATTR_TYPE_VALUE_NULL;

                break;

            /* Anything else */
            default:
                break;
        }

        data++;
    }

    lxb_html_tokenizer_state_append_data_m(tkz, data);

    return data;
}

/*
 * 12.2.5.67 After DOCTYPE system identifier state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_doctype_after_system_identifier(
                                                      lxb_html_tokenizer_t *tkz,
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
                                             LXB_HTML_TOKENIZER_ERROR_EOINDO);

                tkz->token->type |= LXB_HTML_TOKEN_TYPE_FORCE_QUIRKS;

                lxb_html_tokenizer_state_token_done_m(tkz, end);

                return end;
            }
            /* fall through */

        default:
            lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                         LXB_HTML_TOKENIZER_ERROR_UNCHAFDOSYID);

            tkz->state = lxb_html_tokenizer_state_doctype_bogus;

            return data;
    }

    return data;
}

/*
 * 12.2.5.68 Bogus DOCTYPE state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_doctype_bogus(lxb_html_tokenizer_t *tkz,
                                       const lxb_char_t *data,
                                       const lxb_char_t *end)
{
    while (data != end) {
        switch (*data) {
            /* U+003E GREATER-THAN SIGN (>) */
            case 0x3E:
                tkz->state = lxb_html_tokenizer_state_data_before;

                lxb_html_tokenizer_state_token_done_m(tkz, end);

                return (data + 1);

            /*
             * U+0000 NULL
             * EOF
             */
            case 0x00:
                if (tkz->is_eof) {
                    lxb_html_tokenizer_state_token_done_m(tkz, end);

                    return end;
                }

                lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                             LXB_HTML_TOKENIZER_ERROR_UNNUCH);

                break;

            /* Anything else */
            default:
                break;
        }

        data++;
    }

    return data;
}
