/*
 * Copyright (C) 2022-2026 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/css/declaration.h"


lxb_css_rule_declaration_t *
lxb_css_declaration_create(lxb_css_parser_t *parser,
                           const lxb_char_t *name, size_t length,
                           const lxb_css_entry_data_t **out_entry)
{
    void *prop;
    const lxb_css_entry_data_t *entry;
    lxb_css_property__custom_t *custom;
    lxb_css_rule_declaration_t *declar;

    declar = lxb_css_rule_declaration_create(parser->memory);
    if (declar == NULL) {
        return NULL;
    }

    entry = lxb_css_property_by_name(name, length);
    if (entry == NULL) {
        entry = lxb_css_property_by_id(LXB_CSS_PROPERTY__CUSTOM);

        prop = entry->create(parser->memory);
        if (prop == NULL) {
            goto failed;
        }

        custom = prop;

        (void) lexbor_str_init(&custom->name, parser->memory->mraw, length);
        if (custom->name.data == NULL) {
            goto failed;
        }

        memcpy(custom->name.data, name, length);

        custom->name.length = length;
        custom->name.data[custom->name.length] = 0x00;
    }
    else {
        prop = entry->create(parser->memory);
        if (prop == NULL) {
            goto failed;
        }
    }

    declar->type = entry->unique;
    declar->u.user = prop;

    if (out_entry != NULL) {
        *out_entry = entry;
    }

    return declar;

failed:

    if (prop != NULL) {
        (void) entry->destroy(parser->memory, prop, true);
    }

    if (declar != NULL) {
        (void) lxb_css_rule_declaration_destroy(declar, true);
    }

    if (out_entry != NULL) {
        *out_entry = NULL;
    }

    return NULL;
}

lxb_css_rule_declaration_list_t *
lxb_css_declaration_list_parse(lxb_css_parser_t *parser,
                               const lxb_char_t *data, size_t length)
{
    return lxb_css_syntax_parse_declarations(parser,
                                             lxb_css_state_cb_declarations(),
                                             data, length);
}
