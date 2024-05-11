/*
 * Copyright (C) 2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_HTML_NODE_H
#define LEXBOR_HTML_NODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/tag/tag.h"
#include "lexbor/dom/interfaces/node.h"


/*
 * Inline functions
 */
lxb_inline bool
lxb_html_node_is_void(lxb_dom_node_t *node)
{
    if (node->ns != LXB_NS_HTML) {
        return false;
    }

    switch (node->local_name) {
        case LXB_TAG_AREA:
        case LXB_TAG_BASE:
        case LXB_TAG_BASEFONT:
        case LXB_TAG_BGSOUND:
        case LXB_TAG_BR:
        case LXB_TAG_COL:
        case LXB_TAG_EMBED:
        case LXB_TAG_FRAME:
        case LXB_TAG_HR:
        case LXB_TAG_IMG:
        case LXB_TAG_INPUT:
        case LXB_TAG_KEYGEN:
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

/*
 * No inline functions for ABI.
 */
LXB_API bool
lxb_html_node_is_void_noi(lxb_dom_node_t *node);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_HTML_NODE_H */
