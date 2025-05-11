/*
 * Copyright (C) 2018-2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_CSS_SYNTAX_TOKEN_RES_H
#define LEXBOR_CSS_SYNTAX_TOKEN_RES_H


#ifdef LXB_CSS_SYNTAX_TOKEN_RES_NAME_SHS_MAP
#ifndef LXB_CSS_SYNTAX_TOKEN_RES_NAME_SHS_MAP_ENABLED
#define LXB_CSS_SYNTAX_TOKEN_RES_NAME_SHS_MAP_ENABLED
static const lexbor_shs_entry_t lxb_css_syntax_token_res_name_shs_map[] =
{
    {NULL, NULL, 92, 0}, {NULL, NULL, 0, 0},
    {NULL, NULL, 0, 0}, {NULL, NULL, 0, 0},
    {NULL, NULL, 0, 0}, {"end-of-file", (void *) LXB_CSS_SYNTAX_TOKEN__EOF, 11, 0},
    {NULL, NULL, 0, 0}, {NULL, NULL, 0, 0},
    {NULL, NULL, 0, 0}, {NULL, NULL, 0, 0},
    {NULL, NULL, 0, 0}, {NULL, NULL, 0, 0},
    {NULL, NULL, 0, 0}, {NULL, NULL, 0, 0},
    {"ident", (void *) LXB_CSS_SYNTAX_TOKEN_IDENT, 5, 0}, {"cdo", (void *) LXB_CSS_SYNTAX_TOKEN_CDO, 3, 0},
    {NULL, NULL, 0, 0}, {"left-parenthesis", (void *) LXB_CSS_SYNTAX_TOKEN_L_PARENTHESIS, 16, 0},
    {"right-parenthesis", (void *) LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS, 17, 0}, {NULL, NULL, 0, 0},
    {NULL, NULL, 0, 0}, {NULL, NULL, 0, 0},
    {NULL, NULL, 0, 0}, {"percentage", (void *) LXB_CSS_SYNTAX_TOKEN_PERCENTAGE, 10, 0},
    {NULL, NULL, 0, 0}, {NULL, NULL, 0, 0},
    {NULL, NULL, 0, 0}, {"at-keyword", (void *) LXB_CSS_SYNTAX_TOKEN_AT_KEYWORD, 10, 0},
    {NULL, NULL, 0, 0}, {NULL, NULL, 0, 0},
    {"string", (void *) LXB_CSS_SYNTAX_TOKEN_STRING, 6, 0}, {NULL, NULL, 0, 0},
    {"bad-url", (void *) LXB_CSS_SYNTAX_TOKEN_BAD_URL, 7, 0}, {NULL, NULL, 0, 0},
    {NULL, NULL, 0, 0}, {NULL, NULL, 0, 0},
    {NULL, NULL, 0, 0}, {NULL, NULL, 0, 0},
    {NULL, NULL, 0, 0}, {"bad-string", (void *) LXB_CSS_SYNTAX_TOKEN_BAD_STRING, 10, 0},
    {"whitespace", (void *) LXB_CSS_SYNTAX_TOKEN_WHITESPACE, 10, 0}, {NULL, NULL, 0, 0},
    {"undefined", (void *) LXB_CSS_SYNTAX_TOKEN_UNDEF, 9, 0}, {NULL, NULL, 0, 0},
    {"right-curly-bracket", (void *) LXB_CSS_SYNTAX_TOKEN_RC_BRACKET, 19, 0}, {"right-square-bracket", (void *) LXB_CSS_SYNTAX_TOKEN_RS_BRACKET, 20, 0},
    {NULL, NULL, 0, 0}, {NULL, NULL, 0, 0},
    {NULL, NULL, 0, 0}, {NULL, NULL, 0, 0},
    {NULL, NULL, 0, 0}, {"number", (void *) LXB_CSS_SYNTAX_TOKEN_NUMBER, 6, 0},
    {NULL, NULL, 0, 0}, {NULL, NULL, 0, 0},
    {NULL, NULL, 0, 0}, {NULL, NULL, 0, 0},
    {"semicolon", (void *) LXB_CSS_SYNTAX_TOKEN_SEMICOLON, 9, 0}, {NULL, NULL, 0, 0},
    {"dimension", (void *) LXB_CSS_SYNTAX_TOKEN_DIMENSION, 9, 0}, {NULL, NULL, 0, 0},
    {"colon", (void *) LXB_CSS_SYNTAX_TOKEN_COLON, 5, 0}, {"function", (void *) LXB_CSS_SYNTAX_TOKEN_FUNCTION, 8, 0},
    {NULL, NULL, 0, 0}, {NULL, NULL, 0, 0},
    {NULL, NULL, 0, 0}, {NULL, NULL, 0, 0},
    {NULL, NULL, 0, 0}, {"comma", (void *) LXB_CSS_SYNTAX_TOKEN_COMMA, 5, 0},
    {"url", (void *) LXB_CSS_SYNTAX_TOKEN_URL, 3, 0}, {NULL, NULL, 0, 0},
    {NULL, NULL, 0, 0}, {"cdc", (void *) LXB_CSS_SYNTAX_TOKEN_CDC, 3, 0},
    {NULL, NULL, 0, 0}, {NULL, NULL, 0, 0},
    {NULL, NULL, 0, 0}, {NULL, NULL, 0, 0},
    {NULL, NULL, 0, 0}, {"hash", (void *) LXB_CSS_SYNTAX_TOKEN_HASH, 4, 0},
    {NULL, NULL, 0, 0}, {NULL, NULL, 0, 0},
    {"comment", (void *) LXB_CSS_SYNTAX_TOKEN_COMMENT, 7, 0}, {NULL, NULL, 0, 0},
    {"delim", (void *) LXB_CSS_SYNTAX_TOKEN_DELIM, 5, 0}, {NULL, NULL, 0, 0},
    {NULL, NULL, 0, 0}, {NULL, NULL, 0, 0},
    {NULL, NULL, 0, 0}, {NULL, NULL, 0, 0},
    {NULL, NULL, 0, 0}, {NULL, NULL, 0, 0},
    {NULL, NULL, 0, 0}, {"left-curly-bracket", (void *) LXB_CSS_SYNTAX_TOKEN_LC_BRACKET, 18, 0},
    {"left-square-bracket", (void *) LXB_CSS_SYNTAX_TOKEN_LS_BRACKET, 19, 0}
};
#endif /* LXB_CSS_SYNTAX_TOKEN_RES_NAME_SHS_MAP_ENABLED */
#endif /* LXB_CSS_SYNTAX_TOKEN_RES_NAME_SHS_MAP */


#endif /* LEXBOR_CSS_SYNTAX_TOKEN_RES_H */
