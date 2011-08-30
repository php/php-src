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

#ifndef YAF_REQUEST_H
#define YAF_REQUEST_H

#define YAF_REQUEST_PROPERTY_NAME_MODULE		"module"
#define YAF_REQUEST_PROPERTY_NAME_CONTROLLER	"controller"
#define YAF_REQUEST_PROPERTY_NAME_ACTION		"action"
#define YAF_REQUEST_PROPERTY_NAME_METHOD		"method"
#define YAF_REQUEST_PROPERTY_NAME_PARAMS		"params"
#define YAF_REQUEST_PROPERTY_NAME_URI		"uri"
#define YAF_REQUEST_PROPERTY_NAME_STATE		"dispatched"
#define YAF_REQUEST_PROPERTY_NAME_LANG		"language"
#define YAF_REQUEST_PROPERTY_NAME_ROUTED		"routed"
#define YAF_REQUEST_PROPERTY_NAME_BASE		"_base_uri"
#define YAF_REQUEST_PROPERTY_NAME_EXCEPTION  "_exception"

#define YAF_REQUEST_SERVER_URI				"request_uri="

#define YAF_GLOBAL_VARS_TYPE					unsigned int
#define YAF_GLOBAL_VARS_POST 				TRACK_VARS_POST
#define YAF_GLOBAL_VARS_GET     				TRACK_VARS_GET
#define YAF_GLOBAL_VARS_ENV     				TRACK_VARS_ENV
#define YAF_GLOBAL_VARS_FILES   				TRACK_VARS_FILES
#define YAF_GLOBAL_VARS_SERVER  				TRACK_VARS_SERVER
#define YAF_GLOBAL_VARS_REQUEST 				TRACK_VARS_REQUEST
#define YAF_GLOBAL_VARS_COOKIE  				TRACK_VARS_COOKIE

#define YAF_REQUEST_IS_METHOD(x) \
PHP_METHOD(yaf_request, is##x) {\
	zval * method  = zend_read_property(Z_OBJCE_P(getThis()), getThis(), ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_METHOD), 1 TSRMLS_CC);\
	if (strncasecmp(#x, Z_STRVAL_P(method), Z_STRLEN_P(method)) == 0) { \
		RETURN_TRUE; \
	} \
	RETURN_FALSE; \
}

#define YAF_REQUEST_METHOD(ce, x, type) \
PHP_METHOD(ce, get##x) { \
	char *name; \
	int  len; \
    zval *ret; \
	zval *def = NULL; \
	if (ZEND_NUM_ARGS() == 0) {\
		ret = yaf_request_query(type, NULL, 0 TSRMLS_CC);\
	}else if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|z", &name, &len, &def) == FAILURE) {\
		WRONG_PARAM_COUNT;\
	} else { \
		ret = yaf_request_query(type, name, len TSRMLS_CC); \
		if (ZVAL_IS_NULL(ret)) {\
			if (def != NULL) {\
				RETURN_ZVAL(def, 1, 0); \
			}\
		}\
	}\
	RETURN_ZVAL(ret, 1, 0);\
}

extern zend_class_entry * yaf_request_ce;

zval * yaf_request_query(uint type, char * name, uint len TSRMLS_DC);
yaf_request_t * yaf_request_instance(yaf_request_t *this_ptr, char *info TSRMLS_DC);
int yaf_request_set_base_uri(yaf_request_t *request, char *base_uri, char *request_uri TSRMLS_DC);
PHPAPI void php_session_start(TSRMLS_D);

inline zval * yaf_request_get_method(yaf_request_t *instance TSRMLS_DC);
inline zval * yaf_request_get_param(yaf_request_t *instance, char *key, int len TSRMLS_DC);
inline zval * yaf_request_get_language(yaf_request_t *instance TSRMLS_DC);

inline int yaf_request_is_routed(yaf_request_t *request TSRMLS_DC);
inline int yaf_request_is_dispatched(yaf_request_t *request TSRMLS_DC);
inline int yaf_request_set_dispatched(yaf_request_t *request, int flag TSRMLS_DC);
inline int yaf_request_set_routed(yaf_request_t *request, int flag TSRMLS_DC);
inline int yaf_request_set_params_single(yaf_request_t *instance, char *key, int len, zval *value TSRMLS_DC);
inline int yaf_request_set_params_multi(yaf_request_t *instance, zval *values TSRMLS_DC);

YAF_STARTUP_FUNCTION(request);

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
