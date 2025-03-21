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
   | Author: Arpad Ray <arpad@php.net>                                    |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "php_session.h"

#define PS_SANITY_CHECK						\
	if (PS(session_status) != php_session_active) { \
		zend_throw_error(NULL, "Session is not active"); \
		RETURN_THROWS(); \
	} \
	if (PS(default_mod) == NULL) { \
		zend_throw_error(NULL, "Cannot call default session handler"); \
		RETURN_THROWS(); \
	}

#define PS_SANITY_CHECK_IS_OPEN				\
	PS_SANITY_CHECK; \
	if (!PS(mod_user_is_open)) {			\
		php_error_docref(NULL, E_WARNING, "Parent session handler is not open");	\
		RETURN_FALSE;						\
	}

PHP_METHOD(SessionHandler, open)
{
	char *save_path = NULL, *session_name = NULL;
	size_t save_path_len, session_name_len;
	zend_result ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &save_path, &save_path_len, &session_name, &session_name_len) == FAILURE) {
		RETURN_THROWS();
	}

	PS_SANITY_CHECK;

	zend_try {
		ret = PS(default_mod)->s_open(&PS(mod_data), save_path, session_name);
	} zend_catch {
		PS(session_status) = php_session_none;
		zend_bailout();
	} zend_end_try();

	if (SUCCESS == ret) {
		PS(mod_user_is_open) = 1;
	}

	RETURN_BOOL(SUCCESS == ret);
}

PHP_METHOD(SessionHandler, close)
{
	zend_result ret;

	// don't return on failure, since not closing the default handler
	// could result in memory leaks or other nasties
	zend_parse_parameters_none();

	PS_SANITY_CHECK_IS_OPEN;

	PS(mod_user_is_open) = 0;

	zend_try {
		ret = PS(default_mod)->s_close(&PS(mod_data));
	} zend_catch {
		PS(session_status) = php_session_none;
		zend_bailout();
	} zend_end_try();

	RETURN_BOOL(SUCCESS == ret);
}

PHP_METHOD(SessionHandler, read)
{
	zend_string *val;
	zend_string *key;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &key) == FAILURE) {
		RETURN_THROWS();
	}

	PS_SANITY_CHECK_IS_OPEN;

	if (PS(default_mod)->s_read(&PS(mod_data), key, &val, PS(gc_maxlifetime)) == FAILURE) {
		RETURN_FALSE;
	}

	RETURN_STR(val);
}

PHP_METHOD(SessionHandler, write)
{
	zend_string *key, *val;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS", &key, &val) == FAILURE) {
		RETURN_THROWS();
	}

	PS_SANITY_CHECK_IS_OPEN;

	RETURN_BOOL(SUCCESS == PS(default_mod)->s_write(&PS(mod_data), key, val, PS(gc_maxlifetime)));
}

PHP_METHOD(SessionHandler, destroy)
{
	zend_string *key;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &key) == FAILURE) {
		RETURN_THROWS();
	}

	PS_SANITY_CHECK_IS_OPEN;

	RETURN_BOOL(SUCCESS == PS(default_mod)->s_destroy(&PS(mod_data), key));
}

PHP_METHOD(SessionHandler, gc)
{
	zend_long maxlifetime;
	zend_long nrdels = -1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &maxlifetime) == FAILURE) {
		RETURN_THROWS();
	}

	PS_SANITY_CHECK_IS_OPEN;

	if (PS(default_mod)->s_gc(&PS(mod_data), maxlifetime, &nrdels) == FAILURE) {
		RETURN_FALSE;
	}
	RETURN_LONG(nrdels);
}

PHP_METHOD(SessionHandler, create_sid)
{
	zend_string *id;

	if (zend_parse_parameters_none() == FAILURE) {
	    RETURN_THROWS();
	}

	PS_SANITY_CHECK;

	id = PS(default_mod)->s_create_sid(&PS(mod_data));

	RETURN_STR(id);
}
