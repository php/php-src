/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */


#ifndef LXB_HTML_TAG_H
#define LXB_HTML_TAG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/html/base.h"

#include "lexbor/tag/tag.h"
#include "lexbor/ns/ns.h"


typedef int lxb_html_tag_category_t;

enum lxb_html_tag_category {
    LXB_HTML_TAG_CATEGORY__UNDEF          = 0x0000,
    LXB_HTML_TAG_CATEGORY_ORDINARY        = 0x0001,
    LXB_HTML_TAG_CATEGORY_SPECIAL         = 0x0002,
    LXB_HTML_TAG_CATEGORY_FORMATTING      = 0x0004,
    LXB_HTML_TAG_CATEGORY_SCOPE           = 0x0008,
    LXB_HTML_TAG_CATEGORY_SCOPE_LIST_ITEM = 0x0010,
    LXB_HTML_TAG_CATEGORY_SCOPE_BUTTON    = 0x0020,
    LXB_HTML_TAG_CATEGORY_SCOPE_TABLE     = 0x0040,
    LXB_HTML_TAG_CATEGORY_SCOPE_SELECT    = 0x0080,
};

typedef struct {
    const lxb_char_t *name;
    unsigned int     len;
}
lxb_html_tag_fixname_t;


#define LXB_HTML_TAG_RES_CATS
#define LXB_HTML_TAG_RES_FIXNAME_SVG
#include "lexbor/html/tag_res.h"


/*
 * Inline functions
 */
lxb_inline bool
lxb_html_tag_is_category(lxb_tag_id_t tag_id, lxb_ns_id_t ns,
                         lxb_html_tag_category_t cat)
{
    if (tag_id < LXB_TAG__LAST_ENTRY && ns < LXB_NS__LAST_ENTRY) {
        return lxb_html_tag_res_cats[tag_id][ns] & cat;
    }

    return (LXB_HTML_TAG_CATEGORY_ORDINARY|LXB_HTML_TAG_CATEGORY_SCOPE_SELECT) & cat;
}

lxb_inline const lxb_html_tag_fixname_t *
lxb_html_tag_fixname_svg(lxb_tag_id_t tag_id)
{
    if (tag_id >= LXB_TAG__LAST_ENTRY) {
        return NULL;
    }

    return &lxb_html_tag_res_fixname_svg[tag_id];
}

lxb_inline bool
lxb_html_tag_is_void(lxb_tag_id_t tag_id)
{
    switch (tag_id) {
        case LXB_TAG_AREA:
        case LXB_TAG_BASE:
        case LXB_TAG_BR:
        case LXB_TAG_COL:
        case LXB_TAG_EMBED:
        case LXB_TAG_HR:
        case LXB_TAG_IMG:
        case LXB_TAG_INPUT:
        case LXB_TAG_LINK:
        case LXB_TAG_META:
        case LXB_TAG_PARAM:
        case LXB_TAG_SOURCE:
        case LXB_TAG_TRACK:
        case LXB_TAG_WBR:
            return true;

        default:
            return false;
    }

    return false;
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LXB_HTML_TAG_H */
