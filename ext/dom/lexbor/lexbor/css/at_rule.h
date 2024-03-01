/*
 * Copyright (C) 2021-2022 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LXB_CSS_AT_RULE_H
#define LXB_CSS_AT_RULE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/css/base.h"
#include "lexbor/css/syntax/syntax.h"
#include "lexbor/css/at_rule/const.h"


typedef struct {
    lxb_css_at_rule_type_t type;
    lexbor_str_t           prelude;
    lexbor_str_t           block;
}
lxb_css_at_rule__undef_t;

typedef struct {
    lexbor_str_t name;
    lexbor_str_t prelude;
    lexbor_str_t block;
}
lxb_css_at_rule__custom_t;

typedef struct {
    uintptr_t reserved;
}
lxb_css_at_rule_media_t;

typedef struct {
    uintptr_t reserved;
}
lxb_css_at_rule_namespace_t;


LXB_API const lxb_css_entry_data_t *
lxb_css_at_rule_by_name(const lxb_char_t *name, size_t length);

LXB_API const lxb_css_entry_data_t *
lxb_css_at_rule_by_id(uintptr_t id);

LXB_API void *
lxb_css_at_rule_destroy(lxb_css_memory_t *memory, void *value,
                        lxb_css_at_rule_type_t type, bool self_destroy);

LXB_API lxb_status_t
lxb_css_at_rule_serialize(const void *style, lxb_css_at_rule_type_t type,
                          lexbor_serialize_cb_f cb, void *ctx);
LXB_API lxb_status_t
lxb_css_at_rule_serialize_str(const void *style, lxb_css_at_rule_type_t type,
                              lexbor_mraw_t *mraw, lexbor_str_t *str);
LXB_API lxb_status_t
lxb_css_at_rule_serialize_name(const void *style, lxb_css_at_rule_type_t type,
                               lexbor_serialize_cb_f cb, void *ctx);
LXB_API lxb_status_t
lxb_css_at_rule_serialize_name_str(const void *style, lxb_css_at_rule_type_t type,
                                   lexbor_mraw_t *mraw, lexbor_str_t *str);

/* _undef. */

LXB_API void *
lxb_css_at_rule__undef_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_at_rule__undef_destroy(lxb_css_memory_t *memory,
                               void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_at_rule__undef_make(lxb_css_parser_t *parser,
                            lxb_css_at_rule__undef_t *undef,
                            const lxb_css_syntax_at_rule_offset_t *at_rule);
LXB_API lxb_status_t
lxb_css_at_rule__undef_serialize(const void *style, lexbor_serialize_cb_f cb,
                                 void *ctx);
LXB_API lxb_status_t
lxb_css_at_rule__undef_serialize_name(const void *at, lexbor_serialize_cb_f cb,
                                      void *ctx);

/* _custom. */

LXB_API void *
lxb_css_at_rule__custom_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_at_rule__custom_destroy(lxb_css_memory_t *memory,
                                void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_at_rule__custom_make(lxb_css_parser_t *parser,
                             lxb_css_at_rule__custom_t *custom,
                             const lxb_css_syntax_at_rule_offset_t *at_rule);
LXB_API lxb_status_t
lxb_css_at_rule__custom_serialize(const void *style, lexbor_serialize_cb_f cb,
                                  void *ctx);
LXB_API lxb_status_t
lxb_css_at_rule__custom_serialize_name(const void *at, lexbor_serialize_cb_f cb,
                                       void *ctx);

/* Media. */

LXB_API void *
lxb_css_at_rule_media_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_at_rule_media_destroy(lxb_css_memory_t *memory,
                              void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_at_rule_media_serialize(const void *style, lexbor_serialize_cb_f cb,
                                void *ctx);

/* Namespace. */

LXB_API void *
lxb_css_at_rule_namespace_create(lxb_css_memory_t *memory);

LXB_API void *
lxb_css_at_rule_namespace_destroy(lxb_css_memory_t *memory,
                                  void *style, bool self_destroy);
LXB_API lxb_status_t
lxb_css_at_rule_namespace_serialize(const void *style, lexbor_serialize_cb_f cb,
                                    void *ctx);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LXB_CSS_AT_RULE_H */
