/*
 * Copyright (C) 2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/core/plog.h"


lxb_status_t
lexbor_plog_init(lexbor_plog_t *plog, size_t init_size, size_t struct_size)
{
    lxb_status_t status;

    if (plog == NULL) {
        return LXB_STATUS_ERROR_OBJECT_IS_NULL;
    }

    if (struct_size < sizeof(lexbor_plog_entry_t)) {
        struct_size = sizeof(lexbor_plog_entry_t);
    }

    status = lexbor_array_obj_init(&plog->list, init_size, struct_size);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    return LXB_STATUS_OK;
}

lexbor_plog_t *
lexbor_plog_destroy(lexbor_plog_t *plog, bool self_destroy)
{
    if (plog == NULL) {
        return NULL;
    }

    lexbor_array_obj_destroy(&plog->list, false);

    if (self_destroy) {
        return lexbor_free(plog);
    }

    return plog;
}

/*
 * No inline functions.
 */
lexbor_plog_t *
lexbor_plog_create_noi(void)
{
    return lexbor_plog_create();
}

void
lexbor_plog_clean_noi(lexbor_plog_t *plog)
{
    lexbor_plog_clean(plog);
}

void *
lexbor_plog_push_noi(lexbor_plog_t *plog, const lxb_char_t *data, void *ctx,
                     unsigned id)
{
    return lexbor_plog_push(plog, data, ctx, id);
}

size_t
lexbor_plog_length_noi(lexbor_plog_t *plog)
{
    return lexbor_plog_length(plog);
}
