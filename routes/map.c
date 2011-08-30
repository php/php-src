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

/* $Id$*/

zend_class_entry *yaf_route_map_ce;

#define YAF_ROUTE_MAP_VAR_NAME_DELIMETER	"_delimeter"
#define YAF_ROUTE_MAP_VAR_NAME_CTL_PREFER	"_ctl_router"

/** {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_route_map_construct_arginfo, 0, 0, 0)
    ZEND_ARG_INFO(0, controller_prefer)
	ZEND_ARG_INFO(0, delimiter)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ int yaf_route_map_route(yaf_route_t *route, yaf_request_t *request TSRMLS_DC)
*/
int yaf_route_map_route(yaf_route_t *route, yaf_request_t *request TSRMLS_DC) {
	zval *ctl_prefer, *delimer, *zuri, *base_uri, *params;
	char *req_uri, *tmp, *rest, *ptrptr, *seg;
	char *query_str = NULL;
	uint seg_len = 0;

	smart_str route_result = {0};

	zuri 	 = zend_read_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_URI), 1 TSRMLS_CC);
	base_uri = zend_read_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_BASE), 1 TSRMLS_CC);

	ctl_prefer = zend_read_property(yaf_route_map_ce, route, ZEND_STRL(YAF_ROUTE_MAP_VAR_NAME_CTL_PREFER), 1 TSRMLS_CC);
	delimer	   = zend_read_property(yaf_route_map_ce, route, ZEND_STRL(YAF_ROUTE_MAP_VAR_NAME_DELIMETER), 1 TSRMLS_CC);

	if (base_uri && IS_STRING == Z_TYPE_P(base_uri)
			&& strstr(Z_STRVAL_P(zuri), Z_STRVAL_P(base_uri)) == Z_STRVAL_P(zuri)) {
		req_uri  = estrdup(Z_STRVAL_P(zuri) + Z_STRLEN_P(base_uri));
	} else {
		req_uri  = estrdup(Z_STRVAL_P(zuri));
	}

	if (Z_TYPE_P(delimer) == IS_STRING 
			&& Z_STRLEN_P(delimer)) { 
		if ((query_str = strstr(req_uri, Z_STRVAL_P(delimer))) != NULL 
			&& *(query_str - 1) == '/') {
			tmp  = req_uri;
			rest = query_str + Z_STRLEN_P(delimer);
			if (*rest == '\0') {
				req_uri 	= estrndup(req_uri, query_str - req_uri);
				query_str 	= NULL;
				efree(tmp);
			} else if (*rest == '/') {
				req_uri 	= estrndup(req_uri, query_str - req_uri);
				query_str   = estrdup(rest);
				efree(tmp);
			} else {
				query_str = NULL;
			}
		}
	}

	seg = php_strtok_r(req_uri, YAF_ROUTER_URL_DELIMIETER, &ptrptr);
	while (seg) {
		seg_len = strlen(seg);
		if (seg_len) {
			smart_str_appendl(&route_result, seg, seg_len);
		}
		smart_str_appendc(&route_result, '_');
		seg = php_strtok_r(NULL, YAF_ROUTER_URL_DELIMIETER, &ptrptr);
	}

	if (route_result.len) {
		if (Z_BVAL_P(ctl_prefer)) {
			zend_update_property_stringl(yaf_request_ce, request, YAF_STRL(YAF_REQUEST_PROPERTY_NAME_CONTROLLER), route_result.c, route_result.len - 1 TSRMLS_CC);
		} else {
			zend_update_property_stringl(yaf_request_ce, request, YAF_STRL(YAF_REQUEST_PROPERTY_NAME_ACTION), route_result.c, route_result.len - 1 TSRMLS_CC);
		}
		efree(route_result.c);
	}

	if (query_str) {
		params = yaf_router_parse_parameters(query_str TSRMLS_CC);
		(void)yaf_request_set_params_multi(request, params TSRMLS_CC);
		zval_ptr_dtor(&params);
		efree(query_str);
	}

	efree(req_uri);

	return 1;
}
/* }}} */

/** {{{ proto public Yaf_Route_Simple::route(Yaf_Request $req)
*/
PHP_METHOD(yaf_route_map, route) {
	yaf_request_t *request;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &request) == FAILURE) {
		return;
	} else {
		RETURN_BOOL(yaf_route_map_route(getThis(), request TSRMLS_CC));
	}
}
/* }}} */

/** {{{ proto public Yaf_Route_Simple::__construct(int $controller_prefer=0, string $delimer = '#!')
*/
PHP_METHOD(yaf_route_map, __construct) {
	long controller_prefer 	= 0;
	char *delim		   		= NULL;
	uint delim_len	   		= 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|ls",
			   	&controller_prefer, &delim, &delim_len) == FAILURE) {
		return;
	}

	if (controller_prefer) {
		zend_update_property_bool(yaf_route_map_ce, getThis(),
				YAF_STRL(YAF_ROUTE_MAP_VAR_NAME_CTL_PREFER), 1 TSRMLS_CC);
	}

	if (delim && delim_len) {
		zend_update_property_stringl(yaf_route_map_ce, getThis(), 
				YAF_STRL(YAF_ROUTE_MAP_VAR_NAME_DELIMETER), delim, delim_len TSRMLS_CC);
	}
}
/* }}} */

/** {{{ yaf_route_map_methods
*/
zend_function_entry yaf_route_map_methods[] = {
	PHP_ME(yaf_route_map, __construct, yaf_route_map_construct_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(yaf_route_map, route, yaf_route_route_arginfo, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(route_map) {
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Route_Map", "Yaf\\Route\\Map", yaf_route_map_methods);
	yaf_route_map_ce = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);
	zend_class_implements(yaf_route_map_ce TSRMLS_CC, 1, yaf_route_ce);

	yaf_route_map_ce->ce_flags |= ZEND_ACC_FINAL_CLASS;

	zend_declare_property_bool(yaf_route_map_ce, YAF_STRL(YAF_ROUTE_MAP_VAR_NAME_CTL_PREFER), 0, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(yaf_route_map_ce, YAF_STRL(YAF_ROUTE_MAP_VAR_NAME_DELIMETER),  ZEND_ACC_PROTECTED TSRMLS_CC);

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
