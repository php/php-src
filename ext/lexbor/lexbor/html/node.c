/*
* Copyright (C) 2020 Alexander Borisov
*
* Author: Alexander Borisov <borisov@lexbor.com>
*/

#include "lexbor/html/node.h"


bool
lxb_html_node_is_void_noi(lxb_dom_node_t *node)
{
    return lxb_html_node_is_void(node);
}
