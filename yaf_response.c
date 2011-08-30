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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "main/SAPI.h"
#include "Zend/zend_interfaces.h"

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_response.h"
#include "yaf_exception.h"

zend_class_entry *yaf_response_ce;

#include "response/http.c"
#include "response/cli.c"

/** {{{ yaf_response_t * yaf_response_instance(yaf_response_t *this_ptr, char *sapi_name TSRMLS_DC)
 */
yaf_response_t * yaf_response_instance(yaf_response_t *this_ptr, char *sapi_name TSRMLS_DC) {
	zval 		  		*header;
	zend_class_entry 	*ce;
	yaf_response_t 		*instance;

	if (strncasecmp(sapi_name, "cli", 3)) {
		ce = yaf_response_http_ce;
	} else {
		ce = yaf_response_cli_ce;
	}

	if (this_ptr) {
		instance = this_ptr;
	} else {
		MAKE_STD_ZVAL(instance);
		object_init_ex(instance, ce);
	}
	
	MAKE_STD_ZVAL(header);
	array_init(header);
	zend_update_property(ce, instance, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_HEADER), header TSRMLS_CC);
	zval_ptr_dtor(&header);

	zend_update_property_string(ce, instance, YAF_STRL(YAF_RESPONSE_PROPERTY_NAME_BODY), "" TSRMLS_CC);

	return instance;
}
/* }}} */

/** {{{ static int yaf_response_set_body(yaf_response_t *response, char *name, int name_len, char *body, long body_len TSRMLS_DC)
 */
#if 0
static int yaf_response_set_body(yaf_response_t *response, char *name, int name_len, char *body, long body_len TSRMLS_DC) {
	zval *zbody;
	zend_class_entry *response_ce;

	if (!body_len) {
		return 1;
	}

	response_ce = Z_OBJCE_P(response);

	zbody = zend_read_property(response_ce, response, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_BODY), 1 TSRMLS_CC);

	zval_dtor(zbody);
	efree(zbody);

	MAKE_STD_ZVAL(zbody);
	ZVAL_STRINGL(zbody, body, body_len, 1);

	zend_update_property(response_ce, response, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_BODY), zbody TSRMLS_CC);

	return 1;
}
#endif
/* }}} */

/** {{{ int yaf_response_alter_body(yaf_response_t *response, char *name, int name_len, char *body, long body_len, int prepend TSRMLS_DC)
 */
int yaf_response_alter_body(yaf_response_t *response, char *name, int name_len, char *body, long body_len, int prepend TSRMLS_DC) {
	zval *zbody;
	char *obody;

	if (!body_len) {
		return 1;
	}

	zbody = zend_read_property(yaf_response_ce, response, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_BODY), 1 TSRMLS_CC);
	obody = Z_STRVAL_P(zbody);
	
	if (prepend) {
		Z_STRLEN_P(zbody) = spprintf(&Z_STRVAL_P(zbody), 0, "%s%s", body, obody);
	}  else {
		Z_STRLEN_P(zbody) = spprintf(&Z_STRVAL_P(zbody), 0, "%s%s", obody, body);
	}

	efree(obody);

	return 1;
}
/* }}} */

/** {{{ int yaf_response_clear_body(yaf_response_t *response TSRMLS_DC)
 */
int yaf_response_clear_body(yaf_response_t *response TSRMLS_DC) {
	zend_update_property_string(yaf_response_ce, response, YAF_STRL(YAF_RESPONSE_PROPERTY_NAME_BODY), "" TSRMLS_CC);
	return 1;
}
/* }}} */

/** {{{ int yaf_response_set_redirect(yaf_response_t *response, char *url, int len TSRMLS_DC)
 */
int yaf_response_set_redirect(yaf_response_t *response, char *url, int len TSRMLS_DC) {
	sapi_header_line ctr = {0};

	ctr.line_len 		= spprintf(&(ctr.line), 0, "%s %s", "Location:", url);
	ctr.response_code 	= 0;
	if (sapi_header_op(SAPI_HEADER_REPLACE, &ctr TSRMLS_CC) == SUCCESS) {
		efree(ctr.line);
		return 1;
	}
	efree(ctr.line);
	return 0;
}
/* }}} */

/** {{{ int yaf_response_send(yaf_response_t *response TSRMLS_DC)
 */
int yaf_response_send(yaf_response_t *response TSRMLS_DC) {
	zval *body = zend_read_property(yaf_response_ce, response, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_BODY), 1 TSRMLS_CC);

	if (IS_STRING == Z_TYPE_P(body) && Z_STRLEN_P(body) > 0) {
		if (php_write(Z_STRVAL_P(body), Z_STRLEN_P(body) TSRMLS_CC) == FAILURE) {
			return 0;
		}
	}
	return 1;
}
/* }}} */

/** {{{ zval * yaf_response_get_body(yaf_response_t *response TSRMLS_DC)
 */
zval * yaf_response_get_body(yaf_response_t *response TSRMLS_DC) {
	zval *body = zend_read_property(yaf_response_ce, response, ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_BODY), 1 TSRMLS_CC);
	return body;
}
/* }}} */

/** {{{ proto private Yaf_Response_Abstract::__construct() 
*/
PHP_METHOD(yaf_response, __construct) {
	(void)yaf_response_instance(getThis(), sapi_module.name TSRMLS_CC);
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::__desctruct(void)
*/
PHP_METHOD(yaf_response, __destruct) {
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::appenBody($body, $name = NULL)
*/
PHP_METHOD(yaf_response, appendBody) {
	char		  	*body, *name = NULL;
	uint			body_len, name_len = 0;
	yaf_response_t 	*self;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &body, &body_len, &name, &name_len) == FAILURE) {
		return;
	}

	self = getThis();

	if (yaf_response_alter_body(self, name, name_len, body, body_len, 0 TSRMLS_CC)) {
		RETURN_ZVAL(self, 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::prependBody($body, $name = NULL)
*/
PHP_METHOD(yaf_response, prependBody) {
	char		  	*body, *name = NULL;
	uint			body_len, name_len = 0;
	yaf_response_t 	*self;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &body, &body_len, &name, &name_len) == FAILURE) {
		return;
	}

	self = getThis();

	if (yaf_response_alter_body(self, name, name_len, body, body_len, 1 TSRMLS_CC)) {
		RETURN_ZVAL(self, 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::setHeader($name, $value, $replace = 0)
*/
PHP_METHOD(yaf_response, setHeader) {
	RETURN_FALSE;
}
/* }}} */

/** {{{ proto parotected Yaf_Response_Abstract::setAllHeaders(void)
*/
PHP_METHOD(yaf_response, setAllHeaders) {
	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::getHeader(void)
*/
PHP_METHOD(yaf_response, getHeader) {
	RETURN_NULL();
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::clearHeaders(void)
*/
PHP_METHOD(yaf_response, clearHeaders) {
	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::setRedirect(string $url)
*/
PHP_METHOD(yaf_response, setRedirect) {
	char *url;
	uint  url_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &url, &url_len) == FAILURE) {
		return;
	}	

	if (!url_len) {
		RETURN_FALSE;
	}

	RETURN_BOOL(yaf_response_set_redirect(getThis(), url, url_len TSRMLS_CC));
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::setBody($body, $name = NULL)
*/
PHP_METHOD(yaf_response, setBody) {
	char		  	*body, *name = NULL;
	uint			body_len, name_len = 0;
	yaf_response_t 	*self;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &body, &body_len, &name, &name_len) == FAILURE) {
		return;
	}

	self = getThis();

	if (yaf_response_alter_body(self, name, name_len, body, body_len, 0 TSRMLS_CC)) {
		RETURN_ZVAL(self, 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::clearBody(void)
*/
PHP_METHOD(yaf_response, clearBody) {
	if (yaf_response_clear_body(getThis() TSRMLS_CC)) {
		RETURN_ZVAL(getThis(), 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::getBody(void)
 */
PHP_METHOD(yaf_response, getBody) {
	zval *body;

	body = yaf_response_get_body(getThis() TSRMLS_CC);
	
	RETURN_ZVAL(body, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::response(void)
 */
PHP_METHOD(yaf_response, response) {
	RETURN_BOOL(yaf_response_send(getThis() TSRMLS_CC));
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::__toString(void)
 */
PHP_METHOD(yaf_response, __toString) {
	zval *body = zend_read_property(yaf_response_ce, getThis(), ZEND_STRL(YAF_RESPONSE_PROPERTY_NAME_BODY), 1 TSRMLS_CC);
	RETURN_ZVAL(body, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Response_Abstract::__clone(void)
*/
PHP_METHOD(yaf_response, __clone) {
}
/* }}} */

/** {{{ yaf_response_methods 
*/
zend_function_entry yaf_response_methods[] = {
	PHP_ME(yaf_response, __construct, 	NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR) 
	PHP_ME(yaf_response, __destruct,  	NULL, ZEND_ACC_PUBLIC|ZEND_ACC_DTOR) 
	PHP_ME(yaf_response, __clone,		NULL, ZEND_ACC_PRIVATE)
	PHP_ME(yaf_response, __toString,		NULL, ZEND_ACC_PRIVATE)
	PHP_ME(yaf_response, setBody,		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, appendBody,		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, prependBody,	NULL, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, clearBody,		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, getBody,		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, setHeader,		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, setAllHeaders,	NULL, ZEND_ACC_PROTECTED)
	PHP_ME(yaf_response, getHeader,		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, clearHeaders,   NULL, ZEND_ACC_PUBLIC)	
	PHP_ME(yaf_response, setRedirect,	NULL, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_response, response,		NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(response) {
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Response_Abstract", "Yaf\\Response_Abstract", yaf_response_methods);

	yaf_response_ce = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);
	yaf_response_ce->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

	zend_declare_property_null(yaf_response_ce, YAF_STRL(YAF_RESPONSE_PROPERTY_NAME_HEADER), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(yaf_response_ce, YAF_STRL(YAF_RESPONSE_PROPERTY_NAME_BODY), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_bool(yaf_response_ce, YAF_STRL(YAF_RESPONSE_PROPERTY_NAME_HEADEREXCEPTION), 0, ZEND_ACC_PROTECTED TSRMLS_CC);

	YAF_STARTUP(response_http);
	YAF_STARTUP(response_cli);

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
