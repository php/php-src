/*
 * Copyright (C) 2022 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_CSS_SYNTAX_H
#define LEXBOR_CSS_SYNTAX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/css/syntax/tokenizer.h"


typedef struct lxb_css_syntax_rule lxb_css_syntax_rule_t;

typedef const lxb_css_syntax_token_t *
(*lxb_css_syntax_state_f)(lxb_css_parser_t *parser,
                          const lxb_css_syntax_token_t *token,
                          lxb_css_syntax_rule_t *rule);

typedef lxb_status_t
(*lxb_css_syntax_declaration_end_f)(lxb_css_parser_t *parser, void *ctx,
                                    bool important, bool failed);

typedef lxb_status_t
(*lxb_css_syntax_cb_done_f)(lxb_css_parser_t *parser,
                            const lxb_css_syntax_token_t *token,
                            void *ctx, bool failed);

typedef struct {
    uintptr_t begin;
    uintptr_t end;
}
lxb_css_syntax_list_rules_offset_t;

typedef struct {
    uintptr_t name;
    uintptr_t prelude;
    uintptr_t prelude_end;
    uintptr_t block;
    uintptr_t block_end;
}
lxb_css_syntax_at_rule_offset_t;

typedef struct {
    uintptr_t prelude;
    uintptr_t prelude_end;
    uintptr_t block;
    uintptr_t block_end;
}
lxb_css_syntax_qualified_offset_t;

typedef struct {
    uintptr_t begin;
    uintptr_t end;
    uintptr_t name_begin;
    uintptr_t name_end;
    uintptr_t value_begin;
    uintptr_t before_important;
    uintptr_t value_end;
}
lxb_css_syntax_declarations_offset_t;

typedef struct {
    lxb_css_parser_state_f   state;
    lxb_css_parser_state_f   block;
    lxb_css_parser_state_f   failed;
    lxb_css_syntax_cb_done_f end;
}
lxb_css_syntax_cb_base_t;

typedef lxb_css_syntax_cb_base_t lxb_css_syntax_cb_pipe_t;
typedef lxb_css_syntax_cb_base_t lxb_css_syntax_cb_block_t;
typedef lxb_css_syntax_cb_base_t lxb_css_syntax_cb_function_t;
typedef lxb_css_syntax_cb_base_t lxb_css_syntax_cb_components_t;
typedef lxb_css_syntax_cb_base_t lxb_css_syntax_cb_at_rule_t;
typedef lxb_css_syntax_cb_base_t lxb_css_syntax_cb_qualified_rule_t;

typedef struct {
    lxb_css_syntax_cb_base_t          cb;
    lxb_css_syntax_declaration_end_f  declaration_end;
    const lxb_css_syntax_cb_at_rule_t *at_rule;
}
lxb_css_syntax_cb_declarations_t;

typedef struct {
    lxb_css_syntax_cb_base_t                 cb;
    lxb_css_parser_state_f                   next;
    const lxb_css_syntax_cb_at_rule_t        *at_rule;
    const lxb_css_syntax_cb_qualified_rule_t *qualified_rule;
}
lxb_css_syntax_cb_list_rules_t;

struct lxb_css_syntax_rule {
    lxb_css_syntax_state_f       phase;
    lxb_css_parser_state_f       state;
    lxb_css_parser_state_f       state_back;
    lxb_css_syntax_state_f       back;

    union {
        const lxb_css_syntax_cb_base_t           *cb;
        const lxb_css_syntax_cb_list_rules_t     *list_rules;
        const lxb_css_syntax_cb_at_rule_t        *at_rule;
        const lxb_css_syntax_cb_qualified_rule_t *qualified_rule;
        const lxb_css_syntax_cb_declarations_t   *declarations;
        const lxb_css_syntax_cb_components_t     *components;
        const lxb_css_syntax_cb_function_t       *func;
        const lxb_css_syntax_cb_block_t          *block;
        const lxb_css_syntax_cb_pipe_t           *pipe;
        void                                     *user;
    } cbx;

    void                        *context;

    uintptr_t                   offset;
    size_t                      deep;
    lxb_css_syntax_token_type_t block_end;
    bool                        skip_ending;
    bool                        skip_consume;
    bool                        important;
    bool                        failed;
    bool                        top_level;

    union {
        lxb_css_syntax_list_rules_offset_t   list_rules;
        lxb_css_syntax_at_rule_offset_t      at_rule;
        lxb_css_syntax_qualified_offset_t    qualified;
        lxb_css_syntax_declarations_offset_t declarations;
        void                                 *user;
    } u;
};


LXB_API lxb_status_t
lxb_css_syntax_parse_list_rules(lxb_css_parser_t *parser,
                                const lxb_css_syntax_cb_list_rules_t *cb,
                                const lxb_char_t *data, size_t length,
                                void *ctx, bool top_level);

LXB_API lxb_status_t
lxb_css_syntax_stack_expand(lxb_css_parser_t *parser, size_t count);

LXB_API void
lxb_css_syntax_codepoint_to_ascii(lxb_css_syntax_tokenizer_t *tkz,
                                  lxb_codepoint_t cp);

LXB_API lxb_status_t
lxb_css_syntax_ident_serialize(const lxb_char_t *data, size_t length,
                               lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_css_syntax_string_serialize(const lxb_char_t *data, size_t length,
                                lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_css_syntax_ident_or_string_serialize(const lxb_char_t *data, size_t length,
                                         lexbor_serialize_cb_f cb, void *ctx);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_CSS_SYNTAX_H */
