/*
 * Copyright (C) 2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/core/print.h"
#include "lexbor/core/serialize.h"
#include "lexbor/css/log.h"


typedef struct {
    const char *msg;
    size_t length;
}
lxb_css_log_type_str_t;


static const lxb_css_log_type_str_t lxb_css_log_types_map[] = {
    {"Info", 4},
    {"Warning", 7},
    {"Error", 5},
    {"Syntax error", 12}
};


lxb_css_log_t *
lxb_css_log_create(void)
{
    return lexbor_calloc(1, sizeof(lxb_css_log_t));
}

lxb_status_t
lxb_css_log_init(lxb_css_log_t *log, lexbor_mraw_t *mraw)
{
    lxb_status_t status;

    if (log == NULL) {
        return LXB_STATUS_ERROR_OBJECT_IS_NULL;
    }

    status = lexbor_array_obj_init(&log->messages, 64,
                                   sizeof(lxb_css_log_message_t));
    if (status != LXB_STATUS_OK) {
        memset(&log->mraw, 0, sizeof(lexbor_mraw_t));
        return status;
    }

    if (mraw != NULL) {
        log->mraw = mraw;
        log->self_mraw = false;
        return LXB_STATUS_OK;
    }

    log->self_mraw = true;

    log->mraw = lexbor_mraw_create();

    return lexbor_mraw_init(log->mraw, 4096);
}

void
lxb_css_log_clean(lxb_css_log_t *log)
{
    if (log != NULL) {
        lexbor_array_obj_clean(&log->messages);

        if (log->self_mraw) {
            lexbor_mraw_clean(log->mraw);
        }
    }
}

lxb_css_log_t *
lxb_css_log_destroy(lxb_css_log_t *log, bool self_destroy)
{
    if (log == NULL) {
        return NULL;
    }

    (void) lexbor_array_obj_destroy(&log->messages, false);

    if (log->self_mraw) {
        (void) lexbor_mraw_destroy(log->mraw, true);
    }

    if (self_destroy) {
        log = lexbor_free(log);
    }

    return log;
}

lxb_css_log_message_t *
lxb_css_log_append(lxb_css_log_t *log, lxb_css_log_type_t type,
                   const lxb_char_t *str, size_t length)
{
    lxb_css_log_message_t *msg;

    msg = lexbor_array_obj_push(&log->messages);
    if (msg == NULL) {
        return NULL;
    }

    if (lexbor_str_init(&msg->text, log->mraw, length) == NULL) {
        lexbor_array_obj_pop(&log->messages);
        return NULL;
    }

    memcpy(msg->text.data, str, length);
    msg->text.length = length;

    msg->text.data[length] = '\0';

    msg->type = type;

    return msg;
}

lxb_css_log_message_t *
lxb_css_log_push(lxb_css_log_t *log, lxb_css_log_type_t type, size_t length)
{
    lxb_css_log_message_t *msg;

    msg = lexbor_array_obj_push(&log->messages);
    if (msg == NULL) {
        return NULL;
    }

    if (lexbor_str_init(&msg->text, log->mraw, length) == NULL) {
        lexbor_array_obj_pop(&log->messages);
        return NULL;
    }

    msg->type = type;

    return msg;
}

lxb_css_log_message_t *
lxb_css_log_format(lxb_css_log_t *log, lxb_css_log_type_t type,
                   const char *format, ...)
{
    size_t psize;
    lxb_css_log_message_t *msg;
    va_list va;

    va_start(va, format);
    psize = lexbor_vprintf_size(format, va);
    va_end(va);

    if (psize == LXB_PRINT_ERROR) {
        return NULL;
    }

    msg = lxb_css_log_push(log, LXB_CSS_LOG_SYNTAX_ERROR, psize);
    if (msg == NULL) {
        return NULL;
    }

    va_start(va, format);
    (void) lexbor_vsprintf(msg->text.data, psize, format, va);
    va_end(va);

    msg->text.length = psize;

    return msg;
}

lxb_css_log_message_t *
lxb_css_log_not_supported(lxb_css_log_t *log,
                          const char *module_name, const char *description)
{
    static const char unexpected[] = "%s. Not supported: %s";

    return lxb_css_log_format(log, LXB_CSS_LOG_SYNTAX_ERROR, unexpected,
                              module_name, description);
}

const lxb_char_t *
lxb_css_log_type_by_id(lxb_css_log_type_t type, size_t *out_length)
{
    if (out_length != NULL) {
        *out_length = lxb_css_log_types_map[type].length;
    }

    return (const lxb_char_t *) lxb_css_log_types_map[type].msg;
}

lxb_status_t
lxb_css_log_serialize(lxb_css_log_t *log, lexbor_serialize_cb_f cb, void *ctx,
                      const lxb_char_t *indent, size_t indent_length)
{
    size_t i;
    lxb_status_t status;
    lxb_css_log_message_t *msg;

    if (log->messages.length == 0) {
        return LXB_STATUS_OK;
    }

    i = 0;

    do {
        msg = lexbor_array_obj_get(&log->messages, i);

        if (indent != NULL) {
            lexbor_serialize_write(cb, indent, indent_length, ctx, status);
        }

        status = lxb_css_log_message_serialize(msg, cb, ctx);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        i++;

        if (i == log->messages.length) {
            break;
        }

        lexbor_serialize_write(cb, "\n", 1, ctx, status);
    }
    while (true);

    return LXB_STATUS_OK;
}

lxb_char_t *
lxb_css_log_serialize_char(lxb_css_log_t *log, size_t *out_length,
                           const lxb_char_t *indent, size_t indent_length)
{
    size_t length = 0;
    lxb_status_t status;
    lexbor_str_t str;

    status = lxb_css_log_serialize(log, lexbor_serialize_length_cb, &length,
                                   indent, indent_length);
    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    /* + 1 == '\0' */
    str.data = lexbor_malloc(length + 1);
    if (str.data == NULL) {
        goto failed;
    }

    str.length = 0;

    status = lxb_css_log_serialize(log, lexbor_serialize_copy_cb, &str,
                                   indent, indent_length);
    if (status != LXB_STATUS_OK) {
        lexbor_free(str.data);
        goto failed;
    }

    str.data[str.length] = '\0';

    if (out_length != NULL) {
        *out_length = str.length;
    }

    return str.data;

failed:

    if (out_length != NULL) {
        *out_length = 0;
    }

    return NULL;
}


lxb_status_t
lxb_css_log_message_serialize(lxb_css_log_message_t *msg,
                              lexbor_serialize_cb_f cb, void *ctx)
{
    size_t length;
    lxb_status_t status;
    const lxb_char_t *type_name;

    type_name = lxb_css_log_type_by_id(msg->type, &length);

    lexbor_serialize_write(cb, type_name, length, ctx, status);
    lexbor_serialize_write(cb, ". ", 2, ctx, status);
    lexbor_serialize_write(cb, msg->text.data, msg->text.length, ctx, status);

    return LXB_STATUS_OK;
}

lxb_char_t *
lxb_css_log_message_serialize_char(lxb_css_log_message_t *msg,
                                   size_t *out_length)
{
    size_t length = 0;
    lxb_status_t status;
    lexbor_str_t str;

    status = lxb_css_log_message_serialize(msg, lexbor_serialize_length_cb,
                                           &length);
    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    /* + 1 == '\0' */
    str.data = lexbor_malloc(length + 1);
    if (str.data == NULL) {
        goto failed;
    }

    str.length = 0;

    status = lxb_css_log_message_serialize(msg, lexbor_serialize_copy_cb, &str);
    if (status != LXB_STATUS_OK) {
        lexbor_free(str.data);
        goto failed;
    }

    str.data[str.length] = '\0';

    if (out_length != NULL) {
        *out_length = str.length;
    }

    return str.data;

failed:

    if (out_length != NULL) {
        *out_length = 0;
    }

    return NULL;
}
