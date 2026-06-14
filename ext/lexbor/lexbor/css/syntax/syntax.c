/*
 * Copyright (C) 2018-2026 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/css/syntax/syntax.h"
#include "lexbor/css/parser.h"

#include "lexbor/core/str.h"

#ifndef LEXBOR_DISABLE_INTERNAL_EXTERN
    LXB_EXTERN const lxb_char_t lexbor_str_res_ansi_replacement_character[4];
    LXB_EXTERN const lxb_char_t lexbor_str_res_map_hex[256];
    LXB_EXTERN const lxb_char_t lexbor_str_res_map_hex_to_char_lowercase[17];
    LXB_EXTERN const char *lexbor_str_res_char_to_two_hex_value_lowercase[257];
    LXB_EXTERN const lxb_char_t lxb_css_syntax_res_name_map[256];
#endif

static const lexbor_str_t lxb_css_syntax_str_ws = lexbor_str(" ");


lxb_css_rule_list_t *
lxb_css_syntax_parse_list_rules(lxb_css_parser_t *parser,
                                const lxb_css_syntax_cb_list_rules_t *cb,
                                const lxb_char_t *data, size_t length)
{
    lxb_css_rule_list_t *list;
    lxb_css_syntax_rule_t *rule;

    if (lxb_css_parser_is_running(parser)) {
        parser->status = LXB_STATUS_ERROR_WRONG_STAGE;
        return NULL;
    }

    lxb_css_parser_clean(parser);
    lxb_css_parser_buffer_set(parser, data, length);

    list = lxb_css_rule_list_create(parser->memory);
    if (list == NULL) {
        parser->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        goto failed;
    }

    rule = lxb_css_syntax_parser_list_rules_push(parser, cb, NULL, list,
                                                 LXB_CSS_SYNTAX_TOKEN_UNDEF);
    if (rule == NULL) {
        goto failed;
    }

    parser->tkz->with_comment = false;
    parser->stage = LXB_CSS_PARSER_RUN;

    parser->status = lxb_css_syntax_parser_run(parser);
    if (parser->status != LXB_STATUS_OK) {
        goto failed;
    }

    parser->stage = LXB_CSS_PARSER_END;

    return list;

failed:

    if (list != NULL) {
        lxb_css_rule_list_destroy(list, true);
    }

    parser->stage = LXB_CSS_PARSER_END;

    return NULL;
}

lxb_css_rule_declaration_list_t *
lxb_css_syntax_parse_declarations(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_cb_declarations_t *cb,
                                  const lxb_char_t *data, size_t length)
{
    lxb_css_rule_declaration_list_t *list;
    lxb_css_syntax_rule_t *rule;

    if (lxb_css_parser_is_running(parser)) {
        parser->status = LXB_STATUS_ERROR_WRONG_STAGE;
        return NULL;
    }

    lxb_css_parser_clean(parser);
    lxb_css_parser_buffer_set(parser, data, length);

    list = lxb_css_rule_declaration_list_create(parser->memory);
    if (list == NULL) {
        parser->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        goto failed;
    }

    rule = lxb_css_syntax_parser_declarations_push(parser, cb, NULL, list,
                                                   LXB_CSS_SYNTAX_TOKEN_UNDEF,
                                                   true, false);
    if (rule == NULL) {
        goto failed;
    }

    parser->tkz->with_comment = false;
    parser->stage = LXB_CSS_PARSER_RUN;

    parser->status = lxb_css_syntax_parser_run(parser);
    if (parser->status != LXB_STATUS_OK) {
        goto failed;
    }

    parser->stage = LXB_CSS_PARSER_END;

    return list;

failed:

    if (list != NULL) {
        lxb_css_rule_declaration_list_destroy(list, true);
    }

    parser->stage = LXB_CSS_PARSER_END;

    return NULL;
}

lxb_css_syntax_rule_t *
lxb_css_syntax_consume_list_rules(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_cb_list_rules_t *list_rules,
                                  lxb_css_parser_state_f back,
                                  void *ctx, lxb_css_syntax_token_type_t stop)
{
    return lxb_css_syntax_parser_list_rules_push(parser, list_rules, back,
                                                 ctx, stop);
}

lxb_css_syntax_rule_t *
lxb_css_syntax_consume_at_rule(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token,
                               const lxb_css_syntax_cb_at_rule_t *at_rule,
                               lxb_css_parser_state_f back,
                               void *ctx, lxb_css_syntax_token_type_t stop)
{
    if (token->type != LXB_CSS_SYNTAX_TOKEN_AT_KEYWORD) {
        return NULL;
    }

    if (parser->rules > parser->rules_begin && parser->rules->deep != 0
        && parser->types_pos[-1] == LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS)
    {
        parser->types_pos -= 1;
        parser->rules->deep -= 1;
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_syntax_parser_at_rule_push(parser, at_rule, back, ctx,
                                              stop, false);
}

lxb_css_syntax_rule_t *
lxb_css_syntax_consume_qualified_rule(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token,
                                      const lxb_css_syntax_cb_qualified_rule_t *qualified,
                                      lxb_css_parser_state_f back,
                                      void *ctx, lxb_css_syntax_token_type_t stop)
{
    lxb_css_syntax_token_type_t type;

    if (parser->rules > parser->rules_begin && parser->rules->deep != 0) {
        switch (token->type) {
            case LXB_CSS_SYNTAX_TOKEN_LS_BRACKET:
                type = LXB_CSS_SYNTAX_TOKEN_RS_BRACKET;
                break;

            case LXB_CSS_SYNTAX_TOKEN_FUNCTION:
            case LXB_CSS_SYNTAX_TOKEN_L_PARENTHESIS:
                type = LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS;
                break;

            case LXB_CSS_SYNTAX_TOKEN_LC_BRACKET:
                type = LXB_CSS_SYNTAX_TOKEN_RC_BRACKET;
                break;

            default:
                type = LXB_CSS_SYNTAX_TOKEN_UNDEF;
                break;
        }


        if (parser->types_pos[-1] == type) {
            parser->types_pos -= 1;
            parser->rules->deep -= 1;
        }
    }

    return lxb_css_syntax_parser_qualified_push(parser, qualified, back,
                                                ctx, stop, false);
}

lxb_css_syntax_rule_t *
lxb_css_syntax_consume_block(lxb_css_parser_t *parser,
                             const lxb_css_syntax_token_t *token,
                             const lxb_css_syntax_cb_block_t *block,
                             lxb_css_parser_state_f back, void *ctx)
{
    if (token->type != LXB_CSS_SYNTAX_TOKEN_LC_BRACKET) {
        return NULL;
    }

    if (parser->rules > parser->rules_begin && parser->rules->deep != 0
        && parser->types_pos[-1] == LXB_CSS_SYNTAX_TOKEN_RC_BRACKET)
    {
        parser->types_pos -= 1;
        parser->rules->deep -= 1;
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_syntax_parser_block_push(parser, block, back, ctx);
}

lxb_css_syntax_rule_t *
lxb_css_syntax_consume_declarations(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token,
                                    const lxb_css_syntax_cb_declarations_t *declr,
                                    lxb_css_parser_state_f back, void *ctx,
                                    lxb_css_syntax_token_type_t stop)
{
    lxb_css_syntax_token_type_t type;

    if (parser->rules > parser->rules_begin && parser->rules->deep != 0) {
        switch (token->type) {
            case LXB_CSS_SYNTAX_TOKEN_LS_BRACKET:
                type = LXB_CSS_SYNTAX_TOKEN_RS_BRACKET;
                break;

            case LXB_CSS_SYNTAX_TOKEN_FUNCTION:
            case LXB_CSS_SYNTAX_TOKEN_L_PARENTHESIS:
                type = LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS;
                break;

            case LXB_CSS_SYNTAX_TOKEN_LC_BRACKET:
                type = LXB_CSS_SYNTAX_TOKEN_RC_BRACKET;
                break;

            default:
                type = LXB_CSS_SYNTAX_TOKEN_UNDEF;
                break;
        }


        if (parser->types_pos[-1] == type) {
            parser->types_pos -= 1;
            parser->rules->deep -= 1;
        }
    }

    return lxb_css_syntax_parser_declarations_push(parser, declr, back,
                                                   ctx, stop, true, false);
}

lxb_css_syntax_rule_t *
lxb_css_syntax_consume_components(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token,
                                  const lxb_css_syntax_cb_components_t *comp,
                                  lxb_css_parser_state_f back,
                                  void *ctx, lxb_css_syntax_token_type_t stop)
{
    lxb_css_syntax_token_type_t type;

    if (parser->rules > parser->rules_begin && parser->rules->deep != 0) {
        switch (token->type) {
            case LXB_CSS_SYNTAX_TOKEN_LS_BRACKET:
                type = LXB_CSS_SYNTAX_TOKEN_RS_BRACKET;
                break;

            case LXB_CSS_SYNTAX_TOKEN_FUNCTION:
            case LXB_CSS_SYNTAX_TOKEN_L_PARENTHESIS:
                type = LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS;
                break;

            case LXB_CSS_SYNTAX_TOKEN_LC_BRACKET:
                type = LXB_CSS_SYNTAX_TOKEN_RC_BRACKET;
                break;

            default:
                type = LXB_CSS_SYNTAX_TOKEN_UNDEF;
                break;
        }


        if (parser->types_pos[-1] == type) {
            parser->types_pos -= 1;
            parser->rules->deep -= 1;
        }
    }

    return lxb_css_syntax_parser_components_push(parser, comp, back, ctx, stop);
}

lxb_css_syntax_rule_t *
lxb_css_syntax_consume_function(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token,
                                const lxb_css_syntax_cb_function_t *func,
                                lxb_css_parser_state_f back, void *ctx)
{
    if (token->type != LXB_CSS_SYNTAX_TOKEN_FUNCTION) {
        return NULL;
    }

    if (parser->rules > parser->rules_begin && parser->rules->deep != 0
        && parser->types_pos[-1] == LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS)
    {
        parser->types_pos -= 1;
        parser->rules->deep -= 1;
    }

    lxb_css_syntax_parser_consume(parser);

    return lxb_css_syntax_parser_function_push(parser, func, back, ctx);
}

lxb_status_t
lxb_css_syntax_stack_expand(lxb_css_parser_t *parser, size_t count)
{
    size_t length, cur_len, size;
    lxb_css_syntax_rule_t *p;

    cur_len = parser->rules - parser->rules_begin;

    length = cur_len + count + 1024;
    size = length * sizeof(lxb_css_syntax_rule_t);

    p = lexbor_realloc(parser->rules_begin, size);
    if (p == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    parser->rules_begin = p;
    parser->rules_end = p + length;
    parser->rules = p + cur_len;

    return LXB_STATUS_OK;
}

void
lxb_css_syntax_codepoint_to_ascii(lxb_css_syntax_tokenizer_t *tkz,
                                  lxb_codepoint_t cp)
{
    /*
     * Zero, or is for a surrogate, or is greater than
     * the maximum allowed code point (tkz->num > 0x10FFFF).
     */
    if (cp == 0 || cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDFFF)) {
        memcpy(tkz->pos, lexbor_str_res_ansi_replacement_character, 3);

        tkz->pos += 3;
        *tkz->pos = '\0';

        return;
    }

    lxb_char_t *data = tkz->pos;

    if (cp <= 0x0000007F) {
        /* 0xxxxxxx */
        data[0] = (lxb_char_t) cp;

        tkz->pos += 1;
    }
    else if (cp <= 0x000007FF) {
        /* 110xxxxx 10xxxxxx */
        data[0] = (char)(0xC0 | (cp >> 6  ));
        data[1] = (char)(0x80 | (cp & 0x3F));

        tkz->pos += 2;
    }
    else if (cp <= 0x0000FFFF) {
        /* 1110xxxx 10xxxxxx 10xxxxxx */
        data[0] = (char)(0xE0 | ((cp >> 12)));
        data[1] = (char)(0x80 | ((cp >> 6 ) & 0x3F));
        data[2] = (char)(0x80 | ( cp & 0x3F));

        tkz->pos += 3;
    }
    else if (cp <= 0x001FFFFF) {
        /* 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
        data[0] = (char)(0xF0 | ( cp >> 18));
        data[1] = (char)(0x80 | ((cp >> 12) & 0x3F));
        data[2] = (char)(0x80 | ((cp >> 6 ) & 0x3F));
        data[3] = (char)(0x80 | ( cp & 0x3F));

        tkz->pos += 4;
    }

    *tkz->pos = '\0';
}

lxb_status_t
lxb_css_syntax_ident_serialize(const lxb_char_t *data, size_t length,
                               lexbor_serialize_cb_f cb, void *ctx)
{
    lxb_char_t ch;
    lxb_status_t status;
    const char **hex_map;
    const lxb_char_t *p = data, *end;

    static const lexbor_str_t str_s = lexbor_str("\\");

    end = data + length;
    hex_map = lexbor_str_res_char_to_two_hex_value_lowercase;

    while (p < end) {
        ch = *p;

        if (lxb_css_syntax_res_name_map[ch] == 0x00) {
            lexbor_serialize_write(cb, data, p - data, ctx, status);
            lexbor_serialize_write(cb, str_s.data, str_s.length, ctx, status);
            lexbor_serialize_write(cb, hex_map[ch], 2, ctx, status);

            data = ++p;

            if (p < end && lexbor_str_res_map_hex[*p] != 0xff) {
                lexbor_serialize_write(cb, lxb_css_syntax_str_ws.data,
                                       lxb_css_syntax_str_ws.length,
                                       ctx, status);
            }

            continue;
        }

        p++;
    }

    if (data < p) {
        lexbor_serialize_write(cb, data, p - data, ctx, status);
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_css_syntax_string_serialize(const lxb_char_t *data, size_t length,
                                lexbor_serialize_cb_f cb, void *ctx)
{
    lxb_char_t ch;
    lxb_status_t status;
    const char **hex_map;
    const lxb_char_t *p, *end;

    static const lexbor_str_t str_s = lexbor_str("\\");
    static const lexbor_str_t str_dk = lexbor_str("\"");
    static const lexbor_str_t str_ds = lexbor_str("\\\\");
    static const lexbor_str_t str_dks = lexbor_str("\\\"");

    p = data;
    end = data + length;
    hex_map = lexbor_str_res_char_to_two_hex_value_lowercase;

    lexbor_serialize_write(cb, str_dk.data, str_dk.length, ctx, status);

    while (p < end) {
        ch = *p;

        if (lxb_css_syntax_res_name_map[ch] == 0x00) {
            switch (ch) {
                case '\\':
                    lexbor_serialize_write(cb, data, p - data, ctx, status);
                    lexbor_serialize_write(cb, str_ds.data, str_ds.length,
                                           ctx, status);
                    break;

                case '"':
                    lexbor_serialize_write(cb, data, p - data, ctx, status);
                    lexbor_serialize_write(cb, str_dks.data, str_dks.length,
                                           ctx, status);
                    break;

                case '\n':
                case '\t':
                case '\r':
                    lexbor_serialize_write(cb, data, p - data, ctx, status);
                    lexbor_serialize_write(cb, str_s.data, str_s.length,
                                           ctx, status);
                    lexbor_serialize_write(cb, hex_map[ch], 2, ctx, status);

                    p++;

                    if (p < end && lexbor_str_res_map_hex[*p] != 0xff) {
                        lexbor_serialize_write(cb, lxb_css_syntax_str_ws.data,
                                               lxb_css_syntax_str_ws.length,
                                               ctx, status);
                    }

                    data = p;
                    continue;

                default:
                    p++;
                    continue;
            }

            data = ++p;
            continue;
        }

        p++;
    }

    if (data < p) {
        lexbor_serialize_write(cb, data, p - data, ctx, status);
    }

    lexbor_serialize_write(cb, str_dk.data, str_dk.length, ctx, status);

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_css_syntax_ident_or_string_serialize(const lxb_char_t *data, size_t length,
                                         lexbor_serialize_cb_f cb, void *ctx)
{
    const lxb_char_t *p, *end;

    p = data;
    end = data + length;

    while (p < end) {
        if (lxb_css_syntax_res_name_map[*p++] == 0x00) {
            return lxb_css_syntax_string_serialize(data, length, cb, ctx);
        }
    }

    return cb(data, length, ctx);
}
