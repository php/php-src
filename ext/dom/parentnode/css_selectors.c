/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Niels Dossche <nielsdos@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#if defined(HAVE_LIBXML) && defined(HAVE_DOM)
#include "../php_dom.h"

#include "lexbor/css/parser.h"
#include "lexbor/selectors-adapted/selectors.h"

typedef struct {
	HashTable *list;
	dom_object *intern;
} dom_query_selector_all_ctx;

typedef struct {
	const xmlNode *reference;
	bool result;
} dom_query_selector_matches_ctx;

lxb_status_t php_dom_query_selector_find_single_callback(const xmlNode *node, lxb_css_selector_specificity_t spec, void *ctx)
{
	xmlNodePtr *result = (xmlNodePtr *) ctx;
	*result = (xmlNodePtr) node;
	return LXB_STATUS_STOP;
}

lxb_status_t php_dom_query_selector_find_array_callback(const xmlNode *node, lxb_css_selector_specificity_t spec, void *ctx)
{
	dom_query_selector_all_ctx *qsa_ctx = (dom_query_selector_all_ctx *) ctx;
	zval object;
	php_dom_create_object((xmlNodePtr) node, &object, qsa_ctx->intern);
	zend_hash_next_index_insert_new(qsa_ctx->list, &object);
	return LXB_STATUS_OK;
}

lxb_status_t php_dom_query_selector_find_matches_callback(const xmlNode *node, lxb_css_selector_specificity_t spec, void *ctx)
{
	dom_query_selector_matches_ctx *matches_ctx = (dom_query_selector_matches_ctx *) ctx;
	if (node == matches_ctx->reference) {
		matches_ctx->result = true;
		return LXB_STATUS_STOP;
	}
	return LXB_STATUS_OK;
}

static lxb_status_t php_dom_query_selector_common(
	zval *return_value,
	const xmlNode *root,
	zend_string *selectors_str,
	lxb_selectors_cb_f cb,
	void *ctx,
	lxb_selectors_opt_t options
)
{
	lxb_status_t status;

	lxb_css_parser_t parser;
	memset(&parser, 0, sizeof(lxb_css_parser_t));
	status = lxb_css_parser_init(&parser, NULL);
	ZEND_ASSERT(status == LXB_STATUS_OK);

	lxb_selectors_t selectors;
	memset(&selectors, 0, sizeof(lxb_selectors_t));
	status = lxb_selectors_init(&selectors);
	ZEND_ASSERT(status == LXB_STATUS_OK);
	lxb_selectors_opt_set(&selectors, options);

	lxb_css_selector_list_t *list = lxb_css_selectors_parse(&parser, (const lxb_char_t *) ZSTR_VAL(selectors_str), ZSTR_LEN(selectors_str));
	if (UNEXPECTED(list == NULL)) {
		size_t nr_of_messages = lexbor_array_obj_length(&parser.log->messages);
		if (nr_of_messages > 0) {
			lxb_css_log_message_t *msg = lexbor_array_obj_get(&parser.log->messages, 0);
			char *error;
			zend_spprintf(&error, 0, "Invalid selector (%.*s)", (int) msg->text.length, msg->text.data);
			php_dom_throw_error_with_message(SYNTAX_ERR, error, true);
			efree(error);
		} else {
			php_dom_throw_error_with_message(SYNTAX_ERR, "Invalid selector", true);
		}
		status = LXB_STATUS_ERROR;
	} else {
		status = lxb_selectors_find(&selectors, root, list, cb, ctx);
		if (UNEXPECTED(status != LXB_STATUS_OK && status != LXB_STATUS_STOP)) {
			zend_argument_value_error(1, "contains an unsupported selector");
		}
	}

	lxb_css_selector_list_destroy_memory(list);
	lxb_selectors_destroy(&selectors);
	(void) lxb_css_parser_destroy(&parser, false);

	return status;
}

/* https://dom.spec.whatwg.org/#dom-parentnode-queryselector */
void dom_parent_node_query_selector(xmlNodePtr thisp, dom_object *intern, zval *return_value, zend_string *selectors_str)
{
	xmlNodePtr result = NULL;

	if (php_dom_query_selector_common(
		return_value,
		thisp,
		selectors_str,
		php_dom_query_selector_find_single_callback,
		&result,
		LXB_SELECTORS_OPT_MATCH_FIRST
	) != LXB_STATUS_OK || result == NULL) {
		RETURN_NULL();
	} else {
		DOM_RET_OBJ(result, intern);
	}
}

/* https://dom.spec.whatwg.org/#dom-parentnode-queryselectorall */
void dom_parent_node_query_selector_all(xmlNodePtr thisp, dom_object *intern, zval *return_value, zend_string *selectors_str)
{
	HashTable *list = zend_new_array(0);
	dom_query_selector_all_ctx ctx = { list, intern };

	if (php_dom_query_selector_common(
		return_value,
		thisp,
		selectors_str,
		php_dom_query_selector_find_array_callback,
		&ctx,
		LXB_SELECTORS_OPT_DEFAULT
	) != LXB_STATUS_OK) {
		zend_array_destroy(list);
		RETURN_THROWS();
	} else {
		php_dom_create_iterator(return_value, DOM_NODELIST, true);
		dom_object *ret_obj = Z_DOMOBJ_P(return_value);
		dom_nnodemap_object *mapptr = (dom_nnodemap_object *) ret_obj->ptr;
		ZVAL_ARR(&mapptr->baseobj_zv, list);
		mapptr->nodetype = DOM_NODESET;
	}
}

/* https://dom.spec.whatwg.org/#dom-element-matches */
void dom_element_matches(xmlNodePtr thisp, dom_object *intern, zval *return_value, zend_string *selectors_str)
{
	dom_query_selector_matches_ctx ctx = { thisp, false };

	const xmlNode *root = thisp;
	while (root->parent != NULL) {
		root = root->parent;
	}

	if (php_dom_query_selector_common(
		return_value,
		root,
		selectors_str,
		php_dom_query_selector_find_matches_callback,
		&ctx,
		LXB_SELECTORS_OPT_DEFAULT
	) != LXB_STATUS_OK) {
		RETURN_THROWS();
	} else {
		RETURN_BOOL(ctx.result);
	}
}

#endif
