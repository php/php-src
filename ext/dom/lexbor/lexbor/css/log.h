/*
 * Copyright (C) 2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_CSS_LOG_H
#define LEXBOR_CSS_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/mraw.h"
#include "lexbor/core/str.h"
#include "lexbor/core/array_obj.h"
#include "lexbor/css/base.h"


typedef enum {
    LXB_CSS_LOG_INFO = 0,
    LXB_CSS_LOG_WARNING,
    LXB_CSS_LOG_ERROR,
    LXB_CSS_LOG_SYNTAX_ERROR
}
lxb_css_log_type_t;

typedef struct {
    lexbor_str_t       text;
    lxb_css_log_type_t type;
}
lxb_css_log_message_t;

typedef struct {
    lexbor_array_obj_t messages;
    lexbor_mraw_t      *mraw;
    bool               self_mraw;
}
lxb_css_log_t;


LXB_API lxb_css_log_t *
lxb_css_log_create(void);

LXB_API lxb_status_t
lxb_css_log_init(lxb_css_log_t *log, lexbor_mraw_t *mraw);

LXB_API void
lxb_css_log_clean(lxb_css_log_t *log);

LXB_API lxb_css_log_t *
lxb_css_log_destroy(lxb_css_log_t *log, bool self_destroy);

LXB_API lxb_css_log_message_t *
lxb_css_log_append(lxb_css_log_t *log, lxb_css_log_type_t type,
                   const lxb_char_t *str, size_t length);

LXB_API lxb_css_log_message_t *
lxb_css_log_push(lxb_css_log_t *log, lxb_css_log_type_t type, size_t length);

LXB_API lxb_css_log_message_t *
lxb_css_log_format(lxb_css_log_t *log, lxb_css_log_type_t type,
                   const char *format, ...);

LXB_API lxb_css_log_message_t *
lxb_css_log_not_supported(lxb_css_log_t *log,
                          const char *module_name, const char *description);

LXB_API const lxb_char_t *
lxb_css_log_type_by_id(lxb_css_log_type_t type, size_t *out_length);

LXB_API lxb_status_t
lxb_css_log_serialize(lxb_css_log_t *log, lexbor_serialize_cb_f cb, void *ctx,
                      const lxb_char_t *indent, size_t indent_length);

LXB_API lxb_char_t *
lxb_css_log_serialize_char(lxb_css_log_t *log, size_t *out_length,
                           const lxb_char_t *indent, size_t indent_length);

LXB_API lxb_status_t
lxb_css_log_message_serialize(lxb_css_log_message_t *msg,
                              lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_char_t *
lxb_css_log_message_serialize_char(lxb_css_log_message_t *msg,
                                   size_t *out_length);


/*
 * Inline functions
 */
lxb_inline size_t
lxb_css_log_length(lxb_css_log_t *log)
{
    return lexbor_array_obj_length(&log->messages);
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_CSS_LOG_H */
