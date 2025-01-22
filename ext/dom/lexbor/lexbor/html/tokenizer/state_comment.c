/*
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/tokenizer/state_comment.h"
#include "lexbor/html/tokenizer/state.h"

#define LEXBOR_STR_RES_ANSI_REPLACEMENT_CHARACTER
#include "lexbor/core/str_res.h"


static const lxb_char_t *
lxb_html_tokenizer_state_comment_start(lxb_html_tokenizer_t *tkz,
                                       const lxb_char_t *data,
                                       const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_comment_start_dash(lxb_html_tokenizer_t *tkz,
                                            const lxb_char_t *data,
                                            const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_comment(lxb_html_tokenizer_t *tkz,
                                 const lxb_char_t *data,
                                 const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_comment_less_than_sign(lxb_html_tokenizer_t *tkz,
                                                const lxb_char_t *data,
                                                const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_comment_less_than_sign_bang(lxb_html_tokenizer_t *tkz,
                                                     const lxb_char_t *data,
                                                     const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_comment_less_than_sign_bang_dash(
                                                      lxb_html_tokenizer_t *tkz,
                                                      const lxb_char_t *data,
                                                      const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_comment_less_than_sign_bang_dash_dash(
                                                      lxb_html_tokenizer_t *tkz,
                                                      const lxb_char_t *data,
                                                      const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_comment_end_dash(lxb_html_tokenizer_t *tkz,
                                          const lxb_char_t *data,
                                          const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_comment_end(lxb_html_tokenizer_t *tkz,
                                     const lxb_char_t *data,
                                     const lxb_char_t *end);

static const lxb_char_t *
lxb_html_tokenizer_state_comment_end_bang(lxb_html_tokenizer_t *tkz,
                                          const lxb_char_t *data,
                                          const lxb_char_t *end);


/*
 * Helper function. No in the specification. For 12.2.5.43
 */
const lxb_char_t *
lxb_html_tokenizer_state_comment_before_start(lxb_html_tokenizer_t *tkz,
                                              const lxb_char_t *data,
                                              const lxb_char_t *end)
{
    if (tkz->is_eof == false) {
        lxb_html_tokenizer_state_token_set_begin(tkz, data);
        lxb_html_tokenizer_state_token_set_end(tkz, data);
    }

    tkz->token->tag_id = LXB_TAG__EM_COMMENT;

    return lxb_html_tokenizer_state_comment_start(tkz, data, end);
}

/*
 * 12.2.5.43 Comment start state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_comment_start(lxb_html_tokenizer_t *tkz,
                                       const lxb_char_t *data,
                                       const lxb_char_t *end)
{
    /* U+002D HYPHEN-MINUS (-) */
    if (*data == 0x2D) {
        data++;
        tkz->state = lxb_html_tokenizer_state_comment_start_dash;
    }
    /* U+003E GREATER-THAN SIGN (>) */
    else if (*data == 0x3E) {
        tkz->state = lxb_html_tokenizer_state_data_before;

        lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                     LXB_HTML_TOKENIZER_ERROR_ABCLOFEMCO);

        lxb_html_tokenizer_state_set_text(tkz);
        lxb_html_tokenizer_state_token_done_wo_check_m(tkz, end);

        data++;
    }
    else {
        tkz->state = lxb_html_tokenizer_state_comment;
    }

    return data;
}

/*
 * 12.2.5.44 Comment start dash state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_comment_start_dash(lxb_html_tokenizer_t *tkz,
                                            const lxb_char_t *data,
                                            const lxb_char_t *end)
{
    /* U+002D HYPHEN-MINUS (-) */
    if (*data == 0x2D) {
        tkz->state = lxb_html_tokenizer_state_comment_end;

        return (data + 1);
    }
    /* U+003E GREATER-THAN SIGN (>) */
    else if (*data == 0x3E) {
        tkz->state = lxb_html_tokenizer_state_data_before;

        lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                     LXB_HTML_TOKENIZER_ERROR_ABCLOFEMCO);

        lxb_html_tokenizer_state_set_text(tkz);
        lxb_html_tokenizer_state_token_done_wo_check_m(tkz, end);

        return (data + 1);
    }
    /* EOF */
    else if (*data == 0x00) {
        if (tkz->is_eof) {
            lxb_html_tokenizer_state_append_m(tkz, "-", 1);

            lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->last,
                                         LXB_HTML_TOKENIZER_ERROR_EOINCO);

            lxb_html_tokenizer_state_set_text(tkz);
            lxb_html_tokenizer_state_token_done_wo_check_m(tkz, end);

            return end;
        }
    }

    lxb_html_tokenizer_state_append_m(tkz, "-", 1);

    tkz->state = lxb_html_tokenizer_state_comment;

    return data;
}

/*
 * 12.2.5.45 Comment state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_comment(lxb_html_tokenizer_t *tkz,
                                 const lxb_char_t *data,
                                 const lxb_char_t *end)
{
    lxb_html_tokenizer_state_begin_set(tkz, data);

    while (data != end) {
        switch (*data) {
            /* U+003C LESS-THAN SIGN (<) */
            case 0x3C:
                data++;

                lxb_html_tokenizer_state_append_data_m(tkz, data);

                tkz->state = lxb_html_tokenizer_state_comment_less_than_sign;

                return data;

            /* U+002D HYPHEN-MINUS (-) */
            case 0x2D:
                lxb_html_tokenizer_state_token_set_end(tkz, data - 1);
                lxb_html_tokenizer_state_append_data_m(tkz, data);

                tkz->state = lxb_html_tokenizer_state_comment_end_dash;

                return (data + 1);

            /* U+000D CARRIAGE RETURN (CR) */
            case 0x0D:
                if (++data >= end) {
                    lxb_html_tokenizer_state_append_data_m(tkz, data - 1);

                    tkz->state = lxb_html_tokenizer_state_cr;
                    tkz->state_return = lxb_html_tokenizer_state_comment;

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

                    lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->token->end,
                                                 LXB_HTML_TOKENIZER_ERROR_EOINCO);

                    lxb_html_tokenizer_state_set_text(tkz);
                    lxb_html_tokenizer_state_token_done_m(tkz, end);

                    return end;
                }

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
 * 12.2.5.46 Comment less-than sign state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_comment_less_than_sign(lxb_html_tokenizer_t *tkz,
                                                const lxb_char_t *data,
                                                const lxb_char_t *end)
{
    /* U+0021 EXCLAMATION MARK (!) */
    if (*data == 0x21) {
        lxb_html_tokenizer_state_append_m(tkz, data, 1);

        tkz->state = lxb_html_tokenizer_state_comment_less_than_sign_bang;

        return (data + 1);
    }
    /* U+003C LESS-THAN SIGN (<) */
    else if (*data == 0x3C) {
        lxb_html_tokenizer_state_append_m(tkz, data, 1);

        return (data + 1);
    }

    tkz->state = lxb_html_tokenizer_state_comment;

    return data;
}

/*
 * 12.2.5.47 Comment less-than sign bang state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_comment_less_than_sign_bang(lxb_html_tokenizer_t *tkz,
                                                     const lxb_char_t *data,
                                                     const lxb_char_t *end)
{
    /* U+002D HYPHEN-MINUS (-) */
    if (*data == 0x2D) {
        tkz->state = lxb_html_tokenizer_state_comment_less_than_sign_bang_dash;

        return (data + 1);
    }

    tkz->state = lxb_html_tokenizer_state_comment;

    return data;
}

/*
 * 12.2.5.48 Comment less-than sign bang dash state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_comment_less_than_sign_bang_dash(
                                                      lxb_html_tokenizer_t *tkz,
                                                      const lxb_char_t *data,
                                                      const lxb_char_t *end)
{
    /* U+002D HYPHEN-MINUS (-) */
    if (*data == 0x2D) {
        tkz->state =
            lxb_html_tokenizer_state_comment_less_than_sign_bang_dash_dash;

        return (data + 1);
    }

    tkz->state = lxb_html_tokenizer_state_comment_end_dash;

    return data;
}

/*
 * 12.2.5.49 Comment less-than sign bang dash dash state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_comment_less_than_sign_bang_dash_dash(
                                                      lxb_html_tokenizer_t *tkz,
                                                      const lxb_char_t *data,
                                                      const lxb_char_t *end)
{
    /* U+003E GREATER-THAN SIGN (>) */
    if (*data == 0x3E) {
        tkz->state = lxb_html_tokenizer_state_comment_end;

        return data;
    }
    /* EOF */
    else if (*data == 0x00) {
        if (tkz->is_eof) {
            tkz->state = lxb_html_tokenizer_state_comment_end;

            return data;
        }
    }

    lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                 LXB_HTML_TOKENIZER_ERROR_NECO);

    tkz->state = lxb_html_tokenizer_state_comment_end;

    return data;
}

/*
 * 12.2.5.50 Comment end dash state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_comment_end_dash(lxb_html_tokenizer_t *tkz,
                                          const lxb_char_t *data,
                                          const lxb_char_t *end)
{
    /* U+002D HYPHEN-MINUS (-) */
    if (*data == 0x2D) {
        tkz->state = lxb_html_tokenizer_state_comment_end;

        return (data + 1);
    }
    /* EOF */
    else if (*data == 0x00) {
        if (tkz->is_eof) {
            lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->last,
                                         LXB_HTML_TOKENIZER_ERROR_EOINCO);

            lxb_html_tokenizer_state_set_text(tkz);
            lxb_html_tokenizer_state_token_done_wo_check_m(tkz, end);

            return end;
        }
    }

    lxb_html_tokenizer_state_append_m(tkz, "-", 1);

    tkz->state = lxb_html_tokenizer_state_comment;

    return data;
}

/*
 * 12.2.5.51 Comment end state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_comment_end(lxb_html_tokenizer_t *tkz,
                                     const lxb_char_t *data,
                                     const lxb_char_t *end)
{
    /* U+003E GREATER-THAN SIGN (>) */
    if (*data == 0x3E) {
        /* Skip two '-' characters in comment tag end "-->"
         * For <!----> or <!-----> ...
         */
        tkz->state = lxb_html_tokenizer_state_data_before;

        lxb_html_tokenizer_state_set_text(tkz);
        lxb_html_tokenizer_state_token_done_wo_check_m(tkz, end);

        return (data + 1);
    }
    /* U+0021 EXCLAMATION MARK (!) */
    else if (*data == 0x21) {
        tkz->state = lxb_html_tokenizer_state_comment_end_bang;

        return (data + 1);
    }
    /* U+002D HYPHEN-MINUS (-) */
    else if (*data == 0x2D) {
        lxb_html_tokenizer_state_append_m(tkz, data, 1);

        return (data + 1);
    }
    /* EOF */
    else if (*data == 0x00) {
        if (tkz->is_eof) {
            lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->last,
                                         LXB_HTML_TOKENIZER_ERROR_EOINCO);

            lxb_html_tokenizer_state_set_text(tkz);
            lxb_html_tokenizer_state_token_done_wo_check_m(tkz, end);

            return end;
        }
    }

    lxb_html_tokenizer_state_append_m(tkz, "--", 2);

    tkz->state = lxb_html_tokenizer_state_comment;

    return data;
}

/*
 * 12.2.5.52 Comment end bang state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_comment_end_bang(lxb_html_tokenizer_t *tkz,
                                          const lxb_char_t *data,
                                          const lxb_char_t *end)
{
    /* U+002D HYPHEN-MINUS (-) */
    if (*data == 0x2D) {
        tkz->state = lxb_html_tokenizer_state_comment_end_dash;

        return (data + 1);
    }
    /* U+003E GREATER-THAN SIGN (>) */
    else if (*data == 0x3E) {
        tkz->state = lxb_html_tokenizer_state_data_before;

        lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                     LXB_HTML_TOKENIZER_ERROR_INCLCO);

        lxb_html_tokenizer_state_set_text(tkz);
        lxb_html_tokenizer_state_token_done_wo_check_m(tkz, end);

        return (data + 1);
    }
    /* EOF */
    else if (*data == 0x00) {
        if (tkz->is_eof) {
            lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->last,
                                         LXB_HTML_TOKENIZER_ERROR_EOINCO);

            lxb_html_tokenizer_state_set_text(tkz);
            lxb_html_tokenizer_state_token_done_wo_check_m(tkz, end);

            return end;
        }
    }

    tkz->state = lxb_html_tokenizer_state_comment;

    return data;
}
