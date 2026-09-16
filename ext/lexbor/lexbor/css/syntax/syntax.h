/*
 * Copyright (C) 2022-2026 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_CSS_SYNTAX_H
#define LEXBOR_CSS_SYNTAX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/css/syntax/tokenizer.h"
#include "lexbor/css/syntax/tokenizer.h"


typedef struct lxb_css_syntax_rule lxb_css_syntax_rule_t;

typedef const lxb_css_syntax_token_t *
(*lxb_css_syntax_state_f)(lxb_css_parser_t *parser,
                          const lxb_css_syntax_token_t *token,
                          lxb_css_syntax_rule_t *rule);


typedef struct lxb_css_syntax_cb_base lxb_css_syntax_cb_base_t;
typedef struct lxb_css_syntax_cb_list_rules lxb_css_syntax_cb_list_rules_t;
typedef struct lxb_css_syntax_cb_at_rule lxb_css_syntax_cb_at_rule_t;
typedef struct lxb_css_syntax_cb_qualified_rule lxb_css_syntax_cb_qualified_rule_t;
typedef struct lxb_css_syntax_cb_block lxb_css_syntax_cb_block_t;
typedef struct lxb_css_syntax_cb_declarations lxb_css_syntax_cb_declarations_t;
typedef struct lxb_css_syntax_cb_function lxb_css_syntax_cb_function_t;
typedef struct lxb_css_syntax_cb_components lxb_css_syntax_cb_components_t;
typedef struct lxb_css_syntax_cb_pipe lxb_css_syntax_cb_pipe_t;

typedef struct lxb_css_syntax_declaration_offset lxb_css_syntax_declaration_offset_t;

typedef lxb_status_t
(*lxb_css_syntax_cb_done_f)(lxb_css_parser_t *parser,
                            const lxb_css_syntax_token_t *token,
                            void *ctx, bool failed);

typedef const lxb_css_syntax_cb_at_rule_t *
(*lxb_css_syntax_begin_at_rule_f)(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token,
                                  void *ctx, void **out_rule);

typedef const lxb_css_syntax_cb_qualified_rule_t *
(*lxb_css_syntax_begin_qualified_rule_f)(lxb_css_parser_t *parser,
                                         const lxb_css_syntax_token_t *token,
                                         void *ctx, void **out_rule);

typedef const lxb_css_syntax_cb_block_t *
(*lxb_css_syntax_begin_block_f)(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token,
                                void *ctx, void **out_rule);

typedef const lxb_css_syntax_cb_declarations_t *
(*lxb_css_syntax_begin_declarations_f)(lxb_css_parser_t *parser,
                                       const lxb_css_syntax_token_t *token,
                                       void *ctx, void **out_rule);
typedef lxb_css_parser_state_f
(*lxb_css_syntax_declaration_name_f)(lxb_css_parser_t *parser,
                                     const lxb_css_syntax_token_t *token,
                                     void *ctx, void **out_rule);
typedef lxb_status_t
(*lxb_css_syntax_declaration_end_f)(lxb_css_parser_t *parser,
                                    void *declaration, void *ctx,
                                    const lxb_css_syntax_token_t *token,
                                    lxb_css_syntax_declaration_offset_t *offset,
                                    bool important, bool failed);

struct lxb_css_syntax_cb_base {
    lxb_css_parser_state_f   failed;
    lxb_css_syntax_cb_done_f end;
};

struct lxb_css_syntax_cb_list_rules {
    lxb_css_syntax_cb_base_t              cb;
    lxb_css_parser_state_f                next;
    lxb_css_syntax_begin_at_rule_f        at_rule;
    lxb_css_syntax_begin_qualified_rule_f qualified_rule;
};

struct lxb_css_syntax_cb_at_rule {
    lxb_css_syntax_cb_base_t     cb;
    lxb_css_parser_state_f       prelude;
    lxb_css_syntax_cb_done_f     prelude_end;
    lxb_css_syntax_begin_block_f block;
};

struct lxb_css_syntax_cb_qualified_rule {
    lxb_css_syntax_cb_base_t     cb;
    lxb_css_parser_state_f       prelude;
    lxb_css_syntax_cb_done_f     prelude_end;
    lxb_css_syntax_begin_block_f block;
};

struct lxb_css_syntax_cb_block {
    lxb_css_syntax_cb_base_t              cb;
    lxb_css_parser_state_f                next;
    lxb_css_syntax_begin_at_rule_f        at_rule;
    lxb_css_syntax_begin_declarations_f   declarations;
    lxb_css_syntax_begin_qualified_rule_f qualified_rule;
};

struct lxb_css_syntax_cb_declarations {
    lxb_css_syntax_cb_base_t          cb;
    lxb_css_syntax_declaration_name_f name;
    lxb_css_syntax_declaration_end_f  end;
};

struct lxb_css_syntax_cb_function {
    lxb_css_syntax_cb_base_t cb;
    lxb_css_parser_state_f   value;
};

struct lxb_css_syntax_cb_components {
    lxb_css_syntax_cb_base_t cb;
    lxb_css_parser_state_f   prelude;
};

struct lxb_css_syntax_cb_pipe {
    lxb_css_syntax_cb_base_t cb;
    lxb_css_parser_state_f   prelude;
};

struct lxb_css_syntax_declaration_offset {
    size_t value_begin;
    size_t value_end;
    size_t important_begin;
    size_t important_end;
    size_t end;
};

struct lxb_css_syntax_rule {
    lxb_css_syntax_state_f       phase;
    lxb_css_parser_state_f       state;

    /*
     * This callback will be called before rule->state is called.
     * Exclusively from the lxb_css_parser_run(...).
     */
    lxb_css_syntax_state_f       back;
    lxb_css_parser_state_f       back_state;
    void                         *context;
    void                         *context_old;
    void                         *returned;

    union {
        const lxb_css_syntax_cb_base_t           *cb;
        const lxb_css_syntax_cb_list_rules_t     *list_rules;
        const lxb_css_syntax_cb_at_rule_t        *at_rule;
        const lxb_css_syntax_cb_qualified_rule_t *qualified_rule;
        const lxb_css_syntax_cb_components_t     *components;
        const lxb_css_syntax_cb_declarations_t   *declarations;
        const lxb_css_syntax_cb_function_t       *func;
        const lxb_css_syntax_cb_block_t          *block;
        const lxb_css_syntax_cb_pipe_t           *pipe;
        void                                     *user;
    } cbx;

    size_t                       offset;
    size_t                       deep;
    size_t                       begin;
    lxb_css_syntax_token_type_t  block_end;
    bool                         nested;
    bool                         skip_consume;
    bool                         important;
    bool                         failed;
};


LXB_API lxb_css_rule_list_t *
lxb_css_syntax_parse_list_rules(lxb_css_parser_t *parser,
                                const lxb_css_syntax_cb_list_rules_t *cb,
                                const lxb_char_t *data, size_t length);

LXB_API lxb_css_rule_declaration_list_t *
lxb_css_syntax_parse_declarations(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_cb_declarations_t *cb,
                                  const lxb_char_t *data, size_t length);

LXB_API lxb_css_syntax_rule_t *
lxb_css_syntax_consume_list_rules(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_cb_list_rules_t *list_rules,
                                  lxb_css_parser_state_f back,
                                  void *ctx, lxb_css_syntax_token_type_t stop);

LXB_API lxb_css_syntax_rule_t *
lxb_css_syntax_consume_at_rule(lxb_css_parser_t *parser,
                               const lxb_css_syntax_token_t *token,
                               const lxb_css_syntax_cb_at_rule_t *at_rule,
                               lxb_css_parser_state_f back,
                               void *ctx, lxb_css_syntax_token_type_t stop);

LXB_API lxb_css_syntax_rule_t *
lxb_css_syntax_consume_qualified_rule(lxb_css_parser_t *parser,
                                      const lxb_css_syntax_token_t *token,
                                      const lxb_css_syntax_cb_qualified_rule_t *qualified,
                                      lxb_css_parser_state_f back,
                                      void *ctx, lxb_css_syntax_token_type_t stop);

LXB_API lxb_css_syntax_rule_t *
lxb_css_syntax_consume_block(lxb_css_parser_t *parser,
                             const lxb_css_syntax_token_t *token,
                             const lxb_css_syntax_cb_block_t *block,
                             lxb_css_parser_state_f back, void *ctx);

LXB_API lxb_css_syntax_rule_t *
lxb_css_syntax_consume_declarations(lxb_css_parser_t *parser,
                                    const lxb_css_syntax_token_t *token,
                                    const lxb_css_syntax_cb_declarations_t *declr,
                                    lxb_css_parser_state_f back, void *ctx,
                                    lxb_css_syntax_token_type_t stop);

LXB_API lxb_css_syntax_rule_t *
lxb_css_syntax_consume_components(lxb_css_parser_t *parser,
                                  const lxb_css_syntax_token_t *token,
                                  const lxb_css_syntax_cb_components_t *comp,
                                  lxb_css_parser_state_f back,
                                  void *ctx, lxb_css_syntax_token_type_t stop);

LXB_API lxb_css_syntax_rule_t *
lxb_css_syntax_consume_function(lxb_css_parser_t *parser,
                                const lxb_css_syntax_token_t *token,
                                const lxb_css_syntax_cb_function_t *func,
                                lxb_css_parser_state_f back, void *ctx);


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
