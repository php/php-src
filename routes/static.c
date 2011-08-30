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

zend_class_entry * yaf_route_static_ce;

/** {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_route_static_match_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, uri)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ int yaf_route_static_route(yaf_route_t *route, yaf_request_t *request TSRMLS_DC)
 */
int yaf_route_static_route(yaf_route_t *route, yaf_request_t *request TSRMLS_DC) {
	zval *zuri, *base_uri, *params;
	char *req_uri, *module = NULL, *controller = NULL, *action = NULL, *rest = NULL;

	zuri 	 = zend_read_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_URI), 1 TSRMLS_CC);
	base_uri = zend_read_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_BASE), 1 TSRMLS_CC);

	if (base_uri && IS_STRING == Z_TYPE_P(base_uri)
			&& strstr(Z_STRVAL_P(zuri), Z_STRVAL_P(base_uri)) == Z_STRVAL_P(zuri)) {
		req_uri  = estrdup(Z_STRVAL_P(zuri) + Z_STRLEN_P(base_uri));
	} else {
		req_uri  = estrdup(Z_STRVAL_P(zuri));
	}

	do {
		char *s, *p;
		char *uri;
		int request_uri_len = Z_STRLEN_P(zuri);

		if (request_uri_len == 0
				|| (request_uri_len == 1 && *req_uri == '/')) {
			break;
		}

		uri = req_uri;
		s = p = uri;

		while(*p == ' ' || *p == '/') {
			++p;
		}

		if ((s = strstr(p, "/")) != NULL) {
			if (yaf_application_is_module_name(p, s-p TSRMLS_CC)) {
				module = estrndup(p, s - p);
				p  = s + 1;
			} 
		}

		if ((s = strstr(p, "/")) != NULL) {
			controller = estrndup(p, s - p);
			p  = s + 1;
		}

		if ((s = strstr(p, "/")) != NULL) {
			action = estrndup(p, s - p);
			p  = s + 1;
		}

		if (*p != '\0') {
			rest = estrdup(p);
		}
		
		if (module == NULL
				&& controller == NULL
				&& action == NULL ) {
			/* /one */
			if (YAF_G(action_prefer)) {
				action = rest;
			} else {
				controller = rest;
			}
			rest  = NULL;
		} else if (module == NULL
				&& action == NULL
				&& rest  == NULL) {
			/* /one/ */
			if (YAF_G(action_prefer)) {
				action = controller;
				controller = NULL;
			} 
		} else if (controller == NULL
				&& action == NULL
				&& rest != NULL) {
			/* /controller/action */
			controller = module;
			action     = rest;
			module	   = NULL;
			rest	   = NULL;
		} else if (action == NULL
				&& rest == NULL) {
			/* /module/controller/ */
			action	   = controller;
			controller = module;
			module 	   = NULL;
		} else if (controller == NULL
				&& action == NULL)	{
			/* /module/rest */
			controller = module;
			action	   = rest;
			module 	   = NULL;
			rest       = NULL;
		} else if (action == NULL) {
			/* /module/controller/action */
			action = rest;
			rest   = NULL;
		} 
		
	} while (0);

	efree(req_uri);

	if (module != NULL) {
		zend_update_property_string(yaf_request_ce, request, YAF_STRL(YAF_REQUEST_PROPERTY_NAME_MODULE), module TSRMLS_CC);
		efree(module);
	} 	
	if (controller != NULL) {
		zend_update_property_string(yaf_request_ce, request, YAF_STRL(YAF_REQUEST_PROPERTY_NAME_CONTROLLER), controller TSRMLS_CC);
		efree(controller);
	} 	

	if (action != NULL) {
		zend_update_property_string(yaf_request_ce, request, YAF_STRL(YAF_REQUEST_PROPERTY_NAME_ACTION), action TSRMLS_CC);
		efree(action);
	} 

	if (rest) {
		params = yaf_router_parse_parameters(rest TSRMLS_CC);
		(void)yaf_request_set_params_multi(request, params TSRMLS_CC);
		zval_ptr_dtor(&params);
		efree(rest);
	}

	return 1;
}
/* }}} */

/** {{{ proto public Yaf_Router_Classical::route(Yaf_Request $req)
*/
PHP_METHOD(yaf_route_static, route) {
	yaf_request_t *request;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &request) == FAILURE) {
		return;
	} else {
		RETURN_BOOL(yaf_route_static_route(getThis(), request TSRMLS_CC));
	}
}
/* }}} */

/** {{{ proto public Yaf_Router_Classical::match(string $uri)
*/
PHP_METHOD(yaf_route_static, match) {
	RETURN_TRUE;
}
/* }}} */

/** {{{ yaf_route_static_methods
 */
zend_function_entry yaf_route_static_methods[] = {
	PHP_ME(yaf_route_static, match, yaf_route_static_match_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_route_static, route, yaf_route_route_arginfo, 		ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
 */
YAF_STARTUP_FUNCTION(route_static) {
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Route_Static", "Yaf\\Route\\Static", yaf_route_static_methods);
	yaf_route_static_ce = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);
	zend_class_implements(yaf_route_static_ce TSRMLS_CC, 1, yaf_router_ce);

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

