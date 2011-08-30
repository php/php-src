/*
  +----------------------------------------------------------------------+
  | Yet Another Framework                                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Xinchen Hui  <laruence@php.net>                              |
  +----------------------------------------------------------------------+
*/
   
/* $Id$ */

#include "ext/standard/php_smart_str.h"

#define YAF_ROUTE_PROPETY_NAME_MATCH  	"_route"
#define YAF_ROUTE_PROPETY_NAME_ROUTE  	"_default"
#define YAF_ROUTE_PROPETY_NAME_MAP	 	"_maps"
#define YAF_ROUTE_PROPETY_NAME_VERIFY 	"_verify"

#define YAF_ROUTER_URL_DELIMIETER  	 "/"
#define YAF_ROUTE_REGEX_DILIMITER  	 '#'

/* {{{ YAF_ARG_INFO
 */
YAF_BEGIN_ARG_INFO_EX(yaf_route_route_arginfo, 0, 0, 1)
	YAF_ARG_INFO(0, request)
YAF_END_ARG_INFO()
/* }}} */

zend_class_entry *yaf_route_ce;

#include "static.c"
#include "simple.c"
#include "supervar.c"
#include "rewrite.c"
#include "regex.c"
#include "map.c"
 
/* {{{ yaf_route_t * yaf_route_instance(yaf_route_t *this_ptr,  zval *config TSRMLS_DC) 
 */
yaf_route_t * yaf_route_instance(yaf_route_t *this_ptr,  zval *config TSRMLS_DC) {
	zval **match, **def, **map, **ppzval;
	yaf_route_t *instance = NULL;

	if (!config || IS_ARRAY != Z_TYPE_P(config)) {
		return NULL;
	}

	if (zend_hash_find(Z_ARRVAL_P(config), ZEND_STRS("type"), (void **)&ppzval) == FAILURE
			|| IS_STRING != Z_TYPE_PP(ppzval)) {
		return NULL;
	}

	if (strncasecmp(Z_STRVAL_PP(ppzval), "rewrite", sizeof("rewrite") - 1) == 0) {
		if (zend_hash_find(Z_ARRVAL_P(config), ZEND_STRS("match"), (void **)&match) == FAILURE
				|| Z_TYPE_PP(match) != IS_STRING) {
			return NULL;
		}	
		if (zend_hash_find(Z_ARRVAL_P(config), ZEND_STRS("route"), (void **)&def) == FAILURE
				|| Z_TYPE_PP(def) != IS_ARRAY) {
			return NULL;
		}	

		instance = yaf_route_rewrite_instance(NULL, *match, *def, NULL TSRMLS_CC); 
	} else if (strncasecmp(Z_STRVAL_PP(ppzval), "regex", sizeof("regex") - 1) == 0) {
		if (zend_hash_find(Z_ARRVAL_P(config), ZEND_STRS("match"), (void **)&match) == FAILURE || Z_TYPE_PP(match) != IS_STRING) {
			return NULL;
		}	
		if (zend_hash_find(Z_ARRVAL_P(config), ZEND_STRS("route"), (void **)&def) == FAILURE
				|| Z_TYPE_PP(def) != IS_ARRAY) {
			return NULL;
		}	
		if (zend_hash_find(Z_ARRVAL_P(config), ZEND_STRS("map"), (void **)&map) == FAILURE || Z_TYPE_PP(map) != IS_ARRAY) {
			return NULL;
		}

		instance = yaf_route_regex_instance(NULL, *match, *def, *map, NULL TSRMLS_CC); 
	} else if (strncasecmp(Z_STRVAL_PP(ppzval), "simple", sizeof("simple") - 1) == 0) {
		if (zend_hash_find(Z_ARRVAL_P(config), ZEND_STRS("module"), (void **)&match) == FAILURE
				|| Z_TYPE_PP(match) != IS_STRING) {
			return NULL;
		}	
		if (zend_hash_find(Z_ARRVAL_P(config), ZEND_STRS("controller"), (void **)&def) == FAILURE
				|| Z_TYPE_PP(def) != IS_STRING) {
			return NULL;
		}	
		if (zend_hash_find(Z_ARRVAL_P(config), ZEND_STRS("action"), (void **)&map) == FAILURE
				|| Z_TYPE_PP(map) != IS_STRING) {
			return NULL;
		}	

		instance = yaf_route_simple_instance(NULL, *match, *def, *map TSRMLS_CC); 
	} else if (strncasecmp(Z_STRVAL_PP(ppzval), "supervar", sizeof("supervar") - 1) == 0) {
		if (zend_hash_find(Z_ARRVAL_P(config), ZEND_STRS("varname"), (void **)&match) == FAILURE
				|| Z_TYPE_PP(match) != IS_STRING) {
			return NULL;
		}	

		instance = yaf_route_supervar_instance(NULL, *match TSRMLS_CC); 
	} 
	
	return instance;
}
/* }}} */

/** {{{ yaf_route_methods
 */
zend_function_entry yaf_route_methods[] = {
	PHP_ABSTRACT_ME(yaf_route, route, yaf_route_route_arginfo)
    {NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
 */
YAF_STARTUP_FUNCTION(route) {
	zend_class_entry ce;
	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Route_Interface", "Yaf\\Route_Interface", yaf_route_methods);
	yaf_route_ce = zend_register_internal_interface(&ce TSRMLS_CC);

	YAF_STARTUP(route_static);
	YAF_STARTUP(route_simple);
	YAF_STARTUP(route_supervar);
	YAF_STARTUP(route_rewrite);
	YAF_STARTUP(route_regex);
	YAF_STARTUP(route_map);

	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
