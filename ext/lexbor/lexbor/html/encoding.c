/*
 * Copyright (C) 2019-2026 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/encoding.h"

#include "lexbor/core/str.h"


typedef struct {
    lexbor_str_t alias;
    lexbor_str_t name;
}
lxb_html_encoding_name_t;

static const lxb_html_encoding_name_t lxb_html_encoding_names[] = {
    /* UTF-16BE */
    { lexbor_str("unicodefffe"),     lexbor_str("UTF-8") },
    { lexbor_str("utf-16be"),        lexbor_str("UTF-8") },

    /* UTF-16LE */
    { lexbor_str("csunicode"),       lexbor_str("UTF-8") },
    { lexbor_str("iso-10646-ucs-2"), lexbor_str("UTF-8") },
    { lexbor_str("ucs-2"),           lexbor_str("UTF-8") },
    { lexbor_str("unicode"),         lexbor_str("UTF-8") },
    { lexbor_str("unicodefeff"),     lexbor_str("UTF-8") },
    { lexbor_str("utf-16"),          lexbor_str("UTF-8") },
    { lexbor_str("utf-16le"),        lexbor_str("UTF-8") },

    /* x-user-defined */
    { lexbor_str("x-user-defined"),  lexbor_str("windows-1252") }
};


static const lxb_char_t *
lxb_html_encoding_meta(lxb_html_encoding_t *em,
                       const lxb_char_t *data, const lxb_char_t *end);

static const lxb_char_t *
lxb_html_get_attribute(const lxb_char_t *data, const lxb_char_t *end,
                       const lxb_char_t **name, const lxb_char_t **name_end,
                       const lxb_char_t **value, const lxb_char_t **value_end);


lxb_inline const lxb_char_t *
lxb_html_encoding_skip_spaces(const lxb_char_t *data, const lxb_char_t *end)
{
    for (; data < end; data++) {
        switch (*data) {
            case 0x09: case 0x0A:
            case 0x0C: case 0x0D:
            case 0x20:
                break;

            default:
                return data;
        }
    }

    return end;
}

lxb_inline const lxb_char_t *
lxb_html_encoding_skip_name(const lxb_char_t *data, const lxb_char_t *end)
{
    for (; data < end; data++) {
        switch (*data) {
            case 0x09: case 0x0A:
            case 0x0C: case 0x0D:
            case 0x20: case 0x3E:
                return data;
        }
    }

    return end;
}

lxb_inline const lxb_char_t *
lxb_html_encoding_tag_end(const lxb_char_t *data, const lxb_char_t *end)
{
    data = memchr(data, '>', (end - data));
    if (data == NULL) {
        return end;
    }

    return data + 1;
}

lxb_status_t
lxb_html_encoding_init(lxb_html_encoding_t *em)
{
    lxb_status_t status;

    if (em == NULL) {
        return LXB_STATUS_ERROR_WRONG_ARGS;
    }

    status = lexbor_array_obj_init(&em->cache, 12,
                                   sizeof(lxb_html_encoding_entry_t));
    if (status != LXB_STATUS_OK) {
        return status;
    }

    return lexbor_array_obj_init(&em->result, 12,
                                 sizeof(lxb_html_encoding_entry_t));
}

lxb_html_encoding_t *
lxb_html_encoding_destroy(lxb_html_encoding_t *em, bool self_destroy)
{
    if (em == NULL) {
        return NULL;
    }

    lexbor_array_obj_destroy(&em->cache, false);
    lexbor_array_obj_destroy(&em->result, false);

    if (self_destroy) {
        return lexbor_free(em);
    }

    return em;
}

const lxb_char_t *
lxb_html_encoding_prescan(lxb_html_encoding_t *em, const lxb_char_t *data,
                          const lxb_char_t *end, size_t *out_length)
{
    size_t len, length;
    lxb_status_t status;
    lxb_html_encoding_entry_t *entry;
    const lxb_html_encoding_name_t *name;

    static const lexbor_str_t lxb_html_encoding_utf_16le = lexbor_str("UTF-16LE");
    static const lexbor_str_t lxb_html_encoding_utf_16be = lexbor_str("UTF-16BE");

    len = end - data;

    /* Prescan for UTF-16 XML declarations: If position points to. */
    if (len >= 6) {
        if (data[0] == 0x3C && data[1] == 0x00 && data[2] == 0x3F
            && data[3] == 0x00 && data[4] == 0x78 && data[5] == 0x00)
        {
            *out_length = lxb_html_encoding_utf_16le.length;
            return lxb_html_encoding_utf_16le.data;
        }

        if (data[0] == 0x00 && data[1] == 0x3C && data[2] == 0x00
            && data[3] == 0x3F && data[4] == 0x00 && data[5] == 0x78)
        {
            *out_length = lxb_html_encoding_utf_16be.length;
            return lxb_html_encoding_utf_16be.data;
        }
    }

    status = lxb_html_encoding_determine(em, data, end);
    if (status != LXB_STATUS_OK) {
        goto EMPTY;
    }

    if (lxb_html_encoding_meta_length(em) == 0) {
        goto EMPTY;
    }

    entry = lxb_html_encoding_meta_entry(em, 0);
    len = entry->end - entry->name;
    length = sizeof(lxb_html_encoding_names) / sizeof(lxb_html_encoding_name_t);

    for (size_t i = 0; i < length; i++) {
        name = &lxb_html_encoding_names[i];

        if (len == name->alias.length
            && lexbor_str_data_ncasecmp(entry->name, name->alias.data,
                                        name->alias.length))
        {
            *out_length = lxb_html_encoding_names[i].name.length;
            return lxb_html_encoding_names[i].name.data;
        }
    }

    *out_length = entry->end - entry->name;

    return entry->name;

EMPTY:

    *out_length = 0;

    return NULL;
}

lxb_status_t
lxb_html_encoding_determine(lxb_html_encoding_t *em,
                            const lxb_char_t *data, const lxb_char_t *end)
{
    const lxb_char_t *name, *name_end;
    const lxb_char_t *value, *value_end;

    while (data < end) {
        /* Find tag beginning */
        data = memchr(data, '<', (end - data));
        if (data == NULL) {
            return LXB_STATUS_OK;
        }

        if (++data == end) {
            return LXB_STATUS_OK;
        }

        switch (*data) {
            /* Comment or broken tag */
            case '!':
                if ((data + 5) > end) {
                    return LXB_STATUS_OK;
                }

                if (data[1] != '-' || data[2] != '-') {
                    data = lxb_html_encoding_tag_end(data, end);
                    continue;
                }

                while (data < end) {
                    data = lxb_html_encoding_tag_end(data, end);

                    if (data[-3] == '-' && data[-2] == '-') {
                        break;
                    }
                }

                break;

            case '?':
                data = lxb_html_encoding_tag_end(data, end);
                break;

            case '/':
                data++;

                if ((data + 3) > end) {
                    return LXB_STATUS_OK;
                }

                if ((unsigned) (*data - 0x41) <= (0x5A - 0x41)
                    || (unsigned) (*data - 0x61) <= (0x7A - 0x61))
                {
                    goto skip_attributes;
                }

                data = lxb_html_encoding_tag_end(data, end);
                break;

            default:

                if ((unsigned) (*data - 0x41) > (0x5A - 0x41)
                    && (unsigned) (*data - 0x61) > (0x7A - 0x61))
                {
                    break;
                }

                if ((data + 6) > end) {
                    return LXB_STATUS_OK;
                }

                if (!lexbor_str_data_ncasecmp(data, (lxb_char_t *) "meta", 4)) {
                    goto skip_attributes;
                }

                data += 4;

                switch (*data++) {
                    case 0x09: case 0x0A: case 0x0C:
                    case 0x0D: case 0x20: case 0x2F:
                        break;

                    default:
                        goto skip_attributes;
                }

                data = lxb_html_encoding_meta(em, data, end);
                if (data == NULL) {
                    return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
                }

                break;

            skip_attributes:

                data = lxb_html_encoding_skip_name(data, end);
                if (data >= end) {
                    return LXB_STATUS_OK;
                }

                if (*data == '>') {
                    data++;
                    continue;
                }

                /* Skip attributes */
                while (data < end) {
                    data = lxb_html_get_attribute(data, end, &name, &name_end,
                                                  &value, &value_end);
                    if (name == NULL) {
                        break;
                    }
                }

                break;
        }
    }

    return LXB_STATUS_OK;
}

static const lxb_char_t *
lxb_html_encoding_meta(lxb_html_encoding_t *em,
                       const lxb_char_t *data, const lxb_char_t *end)
{
    size_t i, len, cur;
    bool got_pragma, have_content;
    uint8_t need_pragma;
    const lxb_char_t *name, *name_end;
    const lxb_char_t *value, *value_end;
    lxb_html_encoding_entry_t *attr;

    got_pragma = false;
    have_content = false;
    need_pragma = 0x00;
    cur = lexbor_array_obj_length(&em->result);

    lexbor_array_obj_clean(&em->cache);

    while (data < end) {

    find_attr:

        data = lxb_html_get_attribute(data, end, &name, &name_end,
                                      &value, &value_end);
        if (name == NULL) {
            break;
        }

        len = name_end - name;

        if (len < 7) {
            continue;
        }

        /* Exists check */
        for (i = 0; i < lexbor_array_obj_length(&em->cache); i++) {
            attr = lexbor_array_obj_get(&em->cache, i);

            if ((size_t) (attr->end - attr->name) == len
                && lexbor_str_data_ncasecmp(attr->name, name, len))
            {
                goto find_attr;
            }
        }

        /* Append attribute to cache */
        attr = lexbor_array_obj_push(&em->cache);
        if (attr == NULL) {
            return NULL;
        }

        attr->name = name;
        attr->end = name_end;

        if (value == NULL) {
            continue;
        }

        /* http-equiv check */
        if (len == (sizeof("http-equiv") - 1)) {
            if (!lexbor_str_data_ncasecmp((lxb_char_t *) "http-equiv", name, len)) {
                continue;
            }

            if ((value_end - value) == (sizeof("content-type") - 1)
                && lexbor_str_data_ncasecmp((lxb_char_t *) "content-type",
                                            value, (sizeof("content-type") - 1)))
            {
                got_pragma = true;
            }

            continue;
        }

        if (lexbor_str_data_ncasecmp((lxb_char_t *) "content", name, 7)) {
            if (have_content == false) {

                name = lxb_html_encoding_content(value, value_end, &name_end);
                if (name == NULL) {
                    continue;
                }

                attr = lexbor_array_obj_push(&em->result);
                if (attr == NULL) {
                    return NULL;
                }

                attr->name = name;
                attr->end = name_end;

                need_pragma = 0x02;
                have_content = true;
            }

            continue;
        }

        if (lexbor_str_data_ncasecmp((lxb_char_t *) "charset", name, 7)) {
            attr = lexbor_array_obj_push(&em->result);
            if (attr == NULL) {
                return NULL;
            }

            attr->name = value;
            attr->end = value_end;

            need_pragma = 0x01;
        }
    }

    if (need_pragma == 0x00 || (need_pragma == 0x02 && got_pragma == false)) {
        if (cur != lexbor_array_obj_length(&em->result)) {
            lexbor_array_obj_pop(&em->result);
        }
    }

    return data;
}

const lxb_char_t *
lxb_html_encoding_content(const lxb_char_t *data, const lxb_char_t *end,
                          const lxb_char_t **name_end)
{
    const lxb_char_t *name;

    do {
        for (; (data + 7) < end; data++) {
            if (lexbor_str_data_ncasecmp((lxb_char_t *) "charset", data, 7)) {
                goto found;
            }
        }

        return NULL;

    found:

        data = lxb_html_encoding_skip_spaces((data + 7), end);
        if (data >= end) {
            return NULL;
        }

        if (*data != '=') {
            continue;
        }

        data = lxb_html_encoding_skip_spaces((data + 1), end);
        if (data >= end) {
            return NULL;
        }

        break;
    }
    while (true);

    if (*data == '\'' || *data == '"') {
        *name_end = data++;
        name = data;

        for (; data < end; data++) {
            if (*data == **name_end) {
                *name_end = data;
                goto done;
            }
        }

        return NULL;
    }

    name = data;
    *name_end = data;

    for (; data < end; data++) {
        switch (*data) {
            case ';':
                goto done;

            case 0x09: case 0x0A:
            case 0x0C: case 0x0D:
            case 0x20:
                goto done;

            case '"':
            case '\'':
                return NULL;
        }
    }

    if (data == name) {
        return NULL;
    }

done:

    *name_end = data;

    return name;
}

static const lxb_char_t *
lxb_html_get_attribute(const lxb_char_t *data, const lxb_char_t *end,
                       const lxb_char_t **name, const lxb_char_t **name_end,
                       const lxb_char_t **value, const lxb_char_t **value_end)
{
    lxb_char_t ch;

    *name = NULL;
    *value = NULL;

    for (; data < end; data++) {
        switch (*data) {
            case 0x09: case 0x0A:
            case 0x0C: case 0x0D:
            case 0x20: case 0x2F:
                break;

            case 0x3E:
                return (data + 1);

            default:
                goto name_state;
        }
    }

    if (data == end) {
        return data;
    }

name_state:

    /* Attribute name */
    *name = data;

    while (data < end) {
        switch (*data) {
            case 0x09: case 0x0A:
            case 0x0C: case 0x0D:
            case 0x20:
                *name_end = data;

                data++;
                goto spaces_state;

            case '/': case '>':
                *name_end = data;
                return data;

            case '=':
                if (*name != NULL) {
                    *name_end = data++;
                    goto value_state;
                }
        }

        data++;
    }

    *name_end = data;

spaces_state:

    data = lxb_html_encoding_skip_spaces(data, end);
    if (data == end) {
        return data;
    }

    if (*data != '=') {
        return data;
    }

    data += 1;

value_state:

    data = lxb_html_encoding_skip_spaces(data, end);
    if (data == end) {
        return data;
    }

    switch (*data) {
        case '"':
        case '\'':
            ch = *data++;
            if (data == end) {
                return data;
            }

            *value = data;

            do {
                if (*data == ch) {
                    *value_end = data;
                    return data + 1;
                }
            }
            while (++data < end);

            *value = NULL;

            return data;

        case '>':
            return data;

        default:
            *value = data++;
            break;
    }

    for (; data < end; data++) {
        switch (*data) {
            case 0x09: case 0x0A:
            case 0x0C: case 0x0D:
            case 0x20: case 0x3E:
                *value_end = data;
                return data;
        }
    }

    *value = NULL;

    return data;
}

/*
 * No inline functions for ABI.
 */
lxb_html_encoding_t *
lxb_html_encoding_create_noi(void)
{
    return lxb_html_encoding_create();
}

void
lxb_html_encoding_clean_noi(lxb_html_encoding_t *em)
{
    lxb_html_encoding_clean(em);
}

lxb_html_encoding_entry_t *
lxb_html_encoding_meta_entry_noi(lxb_html_encoding_t *em, size_t idx)
{
    return lxb_html_encoding_meta_entry(em, idx);
}

size_t
lxb_html_encoding_meta_length_noi(lxb_html_encoding_t *em)
{
    return lxb_html_encoding_meta_length(em);
}

lexbor_array_obj_t *
lxb_html_encoding_meta_result_noi(lxb_html_encoding_t *em)
{
    return lxb_html_encoding_meta_result(em);
}
