/*
 * Copyright (C) 2018-2022 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_HTML_ELEMENT_H
#define LEXBOR_HTML_ELEMENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/avl.h"

#include "lexbor/html/interface.h"
#include "lexbor/html/style.h"
#include "lexbor/dom/interfaces/element.h"
#include "lexbor/css/rule.h"


struct lxb_html_element {
    lxb_dom_element_t               element;
    lexbor_avl_node_t               *style;
    lxb_css_rule_declaration_list_t *list;
};

typedef enum {
    LXB_HTML_ELEMENT_OPT_UNDEF = 0x00
}
lxb_html_element_style_opt_t;


typedef lxb_status_t
(*lxb_html_element_style_cb_f)(lxb_html_element_t *element,
                               const lxb_css_rule_declaration_t *declr,
                               void *ctx, lxb_css_selector_specificity_t spec,
                               bool is_weak);


LXB_API lxb_html_element_t *
lxb_html_element_interface_create(lxb_html_document_t *document);

LXB_API lxb_html_element_t *
lxb_html_element_interface_destroy(lxb_html_element_t *element);


LXB_API lxb_html_element_t *
lxb_html_element_inner_html_set(lxb_html_element_t *element,
                                const lxb_char_t *html, size_t size);

LXB_API const lxb_css_rule_declaration_t *
lxb_html_element_style_by_name(lxb_html_element_t *element,
                               const lxb_char_t *name, size_t size);

LXB_API const lxb_css_rule_declaration_t *
lxb_html_element_style_by_id(lxb_html_element_t *element, uintptr_t id);

LXB_API const lxb_html_style_node_t *
lxb_html_element_style_node_by_id(lxb_html_element_t *element, uintptr_t id);

LXB_API const lxb_html_style_node_t *
lxb_html_element_style_node_by_name(lxb_html_element_t *element,
                                    const lxb_char_t *name, size_t size);

LXB_API const void *
lxb_html_element_css_property_by_id(lxb_html_element_t *element, uintptr_t id);

LXB_API lxb_status_t
lxb_html_element_style_walk(lxb_html_element_t *element,
                            lxb_html_element_style_cb_f cb, void *ctx,
                            bool with_weak);

LXB_API lxb_status_t
lxb_html_element_style_parse(lxb_html_element_t *element,
                             const lxb_char_t *style, size_t size);

LXB_API lxb_status_t
lxb_html_element_style_append(lxb_html_element_t *element,
                              lxb_css_rule_declaration_t *declr,
                              lxb_css_selector_specificity_t spec);

LXB_API lxb_status_t
lxb_html_element_style_list_append(lxb_html_element_t *element,
                                   lxb_css_rule_declaration_list_t *list,
                                   lxb_css_selector_specificity_t spec);

LXB_API void
lxb_html_element_style_remove_by_id(lxb_html_element_t *element, uintptr_t id);

LXB_API void
lxb_html_element_style_remove_by_name(lxb_html_element_t *element,
                                      const lxb_char_t *name, size_t size);

LXB_API lxb_html_style_node_t *
lxb_html_element_style_remove_all_not(lxb_html_document_t *doc,
                                      lexbor_avl_node_t **root,
                                      lxb_html_style_node_t *style, bool bs);

LXB_API lxb_html_style_node_t *
lxb_html_element_style_remove_all(lxb_html_document_t *doc,
                                  lexbor_avl_node_t **root,
                                  lxb_html_style_node_t *style);

LXB_API lxb_html_style_node_t *
lxb_html_element_style_remove_by_list(lxb_html_document_t *doc,
                                      lexbor_avl_node_t **root,
                                      lxb_html_style_node_t *style,
                                      lxb_css_rule_declaration_list_t *list);

LXB_API lxb_status_t
lxb_html_element_style_serialize(lxb_html_element_t *element,
                                 lxb_html_element_style_opt_t opt,
                                 lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_html_element_style_serialize_str(lxb_html_element_t *element,
                                     lexbor_str_t *str,
                                     lxb_html_element_style_opt_t opt);


/*
 * Inline functions
 */
lxb_inline lxb_tag_id_t
lxb_html_element_tag_id(lxb_html_element_t *element)
{
    return lxb_dom_interface_node(element)->local_name;
}

lxb_inline lxb_ns_id_t
lxb_html_element_ns_id(lxb_html_element_t *element)
{
    return lxb_dom_interface_node(element)->ns;
}

lxb_inline void
lxb_html_element_insert_before(lxb_html_element_t *dst, lxb_html_element_t *src)
{
    lxb_dom_node_insert_before(lxb_dom_interface_node(dst),
                               lxb_dom_interface_node(src));
}

lxb_inline void
lxb_html_element_insert_after(lxb_html_element_t *dst, lxb_html_element_t *src)
{
    lxb_dom_node_insert_after(lxb_dom_interface_node(dst),
                              lxb_dom_interface_node(src));
}

lxb_inline void
lxb_html_element_insert_child(lxb_html_element_t *dst, lxb_html_element_t *src)
{
    lxb_dom_node_insert_child(lxb_dom_interface_node(dst),
                              lxb_dom_interface_node(src));
}

lxb_inline void
lxb_html_element_remove(lxb_html_element_t *element)
{
    lxb_dom_node_remove(lxb_dom_interface_node(element));
}

lxb_inline lxb_html_document_t *
lxb_html_element_document(lxb_html_element_t *element)
{
    return lxb_html_interface_document(lxb_dom_interface_node(element)->owner_document);
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_HTML_ELEMENT_H */
