/*
 * Copyright (C) 2022-2023 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/style.h"


uintptr_t
lxb_html_style_id_by_name(lxb_html_document_t *doc,
                          const lxb_char_t *name, size_t size)
{
    const lxb_css_entry_data_t *data;

    data = lxb_css_property_by_name(name, size);

    if (data == NULL) {
        return lxb_html_document_css_customs_find_id(doc, name, size);
    }

    return data->unique;
}
