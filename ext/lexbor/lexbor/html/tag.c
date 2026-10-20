/*
 * Copyright (C) 2026 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/tag.h"
#include "lexbor/html/tag_res.h"


bool
lxb_html_tag_is_category(lxb_tag_id_t tag_id, lxb_ns_id_t ns,
                         lxb_html_tag_category_t cat)
{
    if (tag_id < LXB_TAG__LAST_ENTRY && ns < LXB_NS__LAST_ENTRY) {
        return lxb_html_tag_res_cats[tag_id][ns] & cat;
    }

    return (LXB_HTML_TAG_CATEGORY_ORDINARY) & cat;
}

const lxb_html_tag_fixname_t *
lxb_html_tag_fixname_svg(lxb_tag_id_t tag_id)
{
    if (tag_id >= LXB_TAG__LAST_ENTRY) {
        return NULL;
    }

    return &lxb_html_tag_res_fixname_svg[tag_id];
}
