/*
 * Copyright (C) 2019-2023 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_CSS_BASE_H
#define LEXBOR_CSS_BASE_H

#ifdef __cplusplus
extern "C" {
#endif


#include "lexbor/core/base.h"
#include "lexbor/core/mraw.h"
#include "lexbor/core/str.h"


#define LXB_CSS_VERSION_MAJOR 1
#define LXB_CSS_VERSION_MINOR 1
#define LXB_CSS_VERSION_PATCH 0

#define LXB_CSS_VERSION_STRING                                                 \
    LEXBOR_STRINGIZE(LXB_CSS_VERSION_MAJOR) "."                                \
    LEXBOR_STRINGIZE(LXB_CSS_VERSION_MINOR) "."                                \
    LEXBOR_STRINGIZE(LXB_CSS_VERSION_PATCH)


typedef struct lxb_css_memory {
    lexbor_dobject_t *objs;
    lexbor_mraw_t    *mraw;
    lexbor_mraw_t    *tree;

    size_t           ref_count;
}
lxb_css_memory_t;

typedef uint32_t lxb_css_type_t;

typedef struct lxb_css_parser lxb_css_parser_t;
typedef struct lxb_css_parser_state lxb_css_parser_state_t;
typedef struct lxb_css_parser_error lxb_css_parser_error_t;

typedef struct lxb_css_syntax_tokenizer lxb_css_syntax_tokenizer_t;
typedef struct lxb_css_syntax_token lxb_css_syntax_token_t;

typedef bool
(*lxb_css_parser_state_f)(lxb_css_parser_t *parser,
                          const lxb_css_syntax_token_t *token, void *ctx);

typedef void *
(*lxb_css_style_create_f)(lxb_css_memory_t *memory);

typedef lxb_status_t
(*lxb_css_style_serialize_f)(const void *style, lexbor_serialize_cb_f cb,
                             void *ctx);

typedef void *
(*lxb_css_style_destroy_f)(lxb_css_memory_t *memory,
                           void *style, bool self_destroy);

/* StyleSheet tree structures. */

typedef struct lxb_css_stylesheet lxb_css_stylesheet_t;
typedef struct lxb_css_rule_list lxb_css_rule_list_t;
typedef struct lxb_css_rule_style lxb_css_rule_style_t;
typedef struct lxb_css_rule_bad_style lxb_css_rule_bad_style_t;
typedef struct lxb_css_rule_declaration_list lxb_css_rule_declaration_list_t;
typedef struct lxb_css_rule_declaration lxb_css_rule_declaration_t;
typedef struct lxb_css_rule_at lxb_css_rule_at_t;

typedef struct {
    lxb_char_t                *name;
    size_t                    length;
    uintptr_t                 unique;
    lxb_css_parser_state_f    state;
    lxb_css_style_create_f    create;
    lxb_css_style_destroy_f   destroy;
    lxb_css_style_serialize_f serialize;
    void                      *initial;
}
lxb_css_entry_data_t;

typedef struct {
    lxb_char_t *name;
    size_t     length;
    uintptr_t  unique;
}
lxb_css_data_t;


LXB_API lxb_css_memory_t *
lxb_css_memory_create(void);

LXB_API lxb_status_t
lxb_css_memory_init(lxb_css_memory_t *memory, size_t prepare_count);

LXB_API void
lxb_css_memory_clean(lxb_css_memory_t *memory);

LXB_API lxb_css_memory_t *
lxb_css_memory_destroy(lxb_css_memory_t *memory, bool self_destroy);

LXB_API lxb_css_memory_t *
lxb_css_memory_ref_inc(lxb_css_memory_t *memory);

LXB_API void
lxb_css_memory_ref_dec(lxb_css_memory_t *memory);

LXB_API lxb_css_memory_t *
lxb_css_memory_ref_dec_destroy(lxb_css_memory_t *memory);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_CSS_BASE_H */
