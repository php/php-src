/*
 * Copyright (C) 2021-2025 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */


#ifndef LEXBOR_SELECTORS_H
#define LEXBOR_SELECTORS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/selectors/base.h"
#include "lexbor/dom/dom.h"
#include "lexbor/css/selectors/selectors.h"
#include "lexbor/core/array_obj.h"


typedef enum {
    LXB_SELECTORS_OPT_DEFAULT = 0x00,

    /*
     * Includes the passed (root) node in the search.
     *
     * By default, the root node does not participate in selector searches,
     * only its children.
     *
     * This behavior is logical, if you have found a node and then you want to
     * search for other nodes in it, you don't need to check it again.
     *
     * But there are cases when it is necessary for root node to participate
     * in the search.  That's what this option is for.
     */
    LXB_SELECTORS_OPT_MATCH_ROOT = 1 << 1,

    /*
     * Stop searching after the first match with any of the selectors
     * in the list.
     *
     * By default, the callback will be triggered for each selector list.
     * That is, if your node matches different selector lists, it will be
     * returned multiple times in the callback.
     *
     * For example:
     *    HTML: <div id="ok"><span>test</span></div>
     *    Selectors: div, div[id="ok"], div:has(:not(a))
     *
     * The default behavior will cause three callbacks with the same node (div).
     * Because it will be found by every selector in the list.
     *
     * This option allows you to end the element check after the first match on
     * any of the selectors.  That is, the callback will be called only once
     * for example above.  This way we get rid of duplicates in the search.
     */
    LXB_SELECTORS_OPT_MATCH_FIRST = 1 << 2
}
lxb_selectors_opt_t;

typedef struct lxb_selectors lxb_selectors_t;
typedef struct lxb_selectors_entry lxb_selectors_entry_t;
typedef struct lxb_selectors_nested lxb_selectors_nested_t;

typedef lxb_status_t
(*lxb_selectors_cb_f)(lxb_dom_node_t *node,
                      lxb_css_selector_specificity_t spec, void *ctx);

typedef lxb_selectors_entry_t *
(*lxb_selectors_state_cb_f)(lxb_selectors_t *selectors,
                            lxb_selectors_entry_t *entry);

struct lxb_selectors_entry {
    uintptr_t                     id;
    lxb_css_selector_combinator_t combinator;
    const lxb_css_selector_t      *selector;
    lxb_dom_node_t                *node;
    lxb_selectors_entry_t         *next;
    lxb_selectors_entry_t         *prev;
    lxb_selectors_entry_t         *following;
    lxb_selectors_nested_t        *nested;
};

struct lxb_selectors_nested {
    lxb_selectors_entry_t    *entry;
    lxb_selectors_state_cb_f return_state;

    lxb_selectors_cb_f       cb;
    void                     *ctx;

    lxb_dom_node_t           *root;
    lxb_selectors_nested_t   *parent;
    lxb_selectors_entry_t    *first;
    lxb_selectors_entry_t    *top;

    size_t                   index;

    bool                     forward;
};

struct lxb_selectors {
    lxb_selectors_state_cb_f state;
    lexbor_dobject_t         *objs;
    lexbor_dobject_t         *nested;

    lxb_selectors_nested_t   *current;

    lxb_selectors_opt_t      options;
    lxb_status_t             status;
};


/*
 * Create lxb_selectors_t object.
 *
 * @return lxb_selectors_t * if successful, otherwise NULL.
 */
LXB_API lxb_selectors_t *
lxb_selectors_create(void);

/*
 * Initialization of lxb_selectors_t object.
 *
 * Caches are initialized in this function.
 *
 * @param[in] lxb_selectors_t *
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_selectors_init(lxb_selectors_t *selectors);

/*
 * Clears the object. Returns object to states as after initialization.
 *
 * After each call to lxb_selectors_find() and lxb_selectors_find_for_node(),
 * the lxb_selectors_t object is cleared. That is, you don't need to call this
 * function every time after searching by a selector.
 *
 * @param[in] lxb_url_parser_t *
 */
LXB_API void
lxb_selectors_clean(lxb_selectors_t *selectors);

/*
 * Destroy lxb_selectors_t object.
 *
 * Destroying all caches.
 *
 * @param[in] lxb_selectors_t *. Can be NULL.
 * @param[in] if false: only destroys internal caches.
 * if true: destroys the lxb_selectors_t object and all internal caches.
 *
 * @return lxb_selectors_t * if self_destroy = false, otherwise NULL.
 */
LXB_API lxb_selectors_t *
lxb_selectors_destroy(lxb_selectors_t *selectors, bool self_destroy);

/*
 * Search for nodes by selector list.
 *
 * Default Behavior:
 *    1. The root node does not participate in the search, only its child nodes.
 *    2. If a node matches multiple selector lists, a callback with that node
 *       will be called on each list.
 *       For example:
 *           HTML: <div id="ok"><span></span></div>
 *           Selectors: div, div[id="ok"], div:has(:not(a))
 *       For each selector list, a callback with a "div" node will be called.
 *
 * To change the search behavior, see lxb_selectors_opt_set().
 *
 * @param[in] lxb_selectors_t *.
 * @param[in] lxb_dom_node_t *.  The node from which the search will begin.
 * @param[in] const lxb_css_selector_list_t *.  Selectors List.
 * @param[in] lxb_selectors_cb_f.  Callback for a found node.
 * @param[in] void *.  Context for the callback.
 * if true: destroys the lxb_selectors_t object and all internal caches.
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_selectors_find(lxb_selectors_t *selectors, lxb_dom_node_t *root,
                   const lxb_css_selector_list_t *list,
                   lxb_selectors_cb_f cb, void *ctx);

/*
 * Match a node to a Selectors List.
 *
 * In other words, the function checks which selector lists will find the
 * specified node.
 *
 * Default Behavior:
 *    1. If a node matches multiple selector lists, a callback with that node
 *       will be called on each list.
 *       For example:
 *           HTML: <div id="ok"><span></span></div>
 *           Node: div
 *           Selectors: div, div[id="ok"], div:has(:not(a))
 *       For each selector list, a callback with a "div" node will be called.
 *
 * To change the search behavior, see lxb_selectors_opt_set().
 *
 * @param[in] lxb_selectors_t *.
 * @param[in] lxb_dom_node_t *.  The node from which the search will begin.
 * @param[in] const lxb_css_selector_list_t *.  Selectors List.
 * @param[in] lxb_selectors_cb_f.  Callback for a found node.
 * @param[in] void *.  Context for the callback.
 * if true: destroys the lxb_selectors_t object and all internal caches.
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_selectors_match_node(lxb_selectors_t *selectors, lxb_dom_node_t *node,
                         const lxb_css_selector_list_t *list,
                         lxb_selectors_cb_f cb, void *ctx);

/*
 * Deprecated!
 * This function does exactly the same thing as lxb_selectors_match_node().
 */
LXB_API LXB_DEPRECATED(lxb_status_t
lxb_selectors_find_reverse(lxb_selectors_t *selectors, lxb_dom_node_t *root,
                           const lxb_css_selector_list_t *list,
                           lxb_selectors_cb_f cb, void *ctx));

/*
 * Inline functions.
 */

/*
 * The function sets the node search options.
 *
 * For more information, see lxb_selectors_opt_t.
 *
 * @param[in] lxb_selectors_t *.
 * @param[in] lxb_selectors_opt_t.
 */
lxb_inline void
lxb_selectors_opt_set(lxb_selectors_t *selectors, lxb_selectors_opt_t opt)
{
    selectors->options = opt;
}

/*
 * Get the current selector.
 *
 * Function to get the selector by which the node was found.
 * Use context (void *ctx) to pass the lxb_selectors_t object to the callback.
 *
 * @param[in] const lxb_selectors_t *.
 *
 * @return const lxb_css_selector_list_t *.
 */
lxb_inline const lxb_css_selector_list_t *
lxb_selectors_selector(const lxb_selectors_t *selectors)
{
    return selectors->current->entry->selector->list;
}

/*
 * Not inline for inline.
 */

/*
 * Same as lxb_selectors_opt_set() function, but not inline.
 */
LXB_API void
lxb_selectors_opt_set_noi(lxb_selectors_t *selectors, lxb_selectors_opt_t opt);

/*
 * Same as lxb_selectors_selector() function, but not inline.
 */
LXB_API const lxb_css_selector_list_t *
lxb_selectors_selector_noi(const lxb_selectors_t *selectors);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_SELECTORS_H */
