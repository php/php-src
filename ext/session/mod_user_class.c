/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Arpad Ray <arpad@php.net>                                    |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"
#include "php_session.h"

#define PS_SANITY_CHECK						\
	if (PS(default_mod) == NULL) {				\
		php_error_docref(NULL, E_CORE_ERROR, "Cannot call default session handler"); \
		RETURN_FALSE;						\
	}

#define PS_SANITY_CHECK_IS_OPEN				\
	PS_SANITY_CHECK; \
	if (!PS(mod_user_is_open)) {			\
		php_error_docref(NULL, E_WARNING, "Parent session handler is not open");	\
		RETURN_FALSE;						\
	}

/* {{{ proto bool SessionHandler::open(string save_path, string session_name)
   Wraps the old open handler */
PHP_METHOD(SessionHandler, open)
{
	char *save_path = NULL, *session_name = NULL;
	size_t save_path_len, session_name_len;

	PS_SANITY_CHECK;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &save_path, &save_path_len, &session_name, &session_name_len) == FAILURE) {
		return;
	}

	PS(mod_user_is_open) = 1;
	RETVAL_BOOL(SUCCESS == PS(default_mod)->s_open(&PS(mod_data), save_path, session_name));
}
/* }}} */

/* {{{ proto bool SessionHandler::close()
   Wraps the old close handler */
PHP_METHOD(SessionHandler, close)
{
	PS_SANITY_CHECK_IS_OPEN;

	// don't return on failure, since not closing the default handler
	// could result in memory leaks or other nasties
	zend_parse_parameters_none();

	PS(mod_user_is_open) = 0;
	RETVAL_BOOL(SUCCESS == PS(default_mod)->s_close(&PS(mod_data)));
}
/* }}} */

/* {{{ proto bool SessionHandler::read(string id)
   Wraps the old read handler */
PHP_METHOD(SessionHandler, read)
{
	zend_string *val;
	zend_string *key;

	PS_SANITY_CHECK_IS_OPEN;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &key) == FAILURE) {
		return;
	}

	if (PS(default_mod)->s_read(&PS(mod_data), key, &val, PS(gc_maxlifetime)) == FAILURE) {
		RETURN_FALSE;
	}

	RETURN_STR(val);
}
/* }}} */

/* {{{ proto bool SessionHandler::write(string id, string data)
   Wraps the old write handler */
PHP_METHOD(SessionHandler, write)
{
	zend_string *key, *val;

	PS_SANITY_CHECK_IS_OPEN;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS", &key, &val) == FAILURE) {
		return;
	}

	RETURN_BOOL(SUCCESS == PS(default_mod)->s_write(&PS(mod_data), key, val, PS(gc_maxlifetime)));
}
/* }}} */

/* {{{ proto bool SessionHandler::destroy(string id)
   Wraps the old destroy handler */
PHP_METHOD(SessionHandler, destroy)
{
	zend_string *key;

	PS_SANITY_CHECK_IS_OPEN;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &key) == FAILURE) {
		return;
	}

	RETURN_BOOL(SUCCESS == PS(default_mod)->s_destroy(&PS(mod_data), key));
}
/* }}} */

/* {{{ proto bool SessionHandler::gc(int maxlifetime)
   Wraps the old gc handler */
PHP_METHOD(SessionHandler, gc)
{
	zend_long maxlifetime;
	int nrdels;

	PS_SANITY_CHECK_IS_OPEN;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &maxlifetime) == FAILURE) {
		return;
	}

	RETURN_BOOL(SUCCESS == PS(default_mod)->s_gc(&PS(mod_data), maxlifetime, &nrdels));
}
/* }}} */

/* {{{ proto char SessionHandler::create_sid()
   Wraps the old create_sid handler */
PHP_METHOD(SessionHandler, create_sid)
{
	zend_string *id;

	PS_SANITY_CHECK;

	if (zend_parse_parameters_none() == FAILURE) {
	    return;
	}

	id = PS(default_mod)->s_create_sid(&PS(mod_data));

	RETURN_STR(id);
}
/* }}} */

/* {{{ proto char SessionUpdateTimestampHandler::validateId(string id)
   Simply return TRUE */
PHP_METHOD(SessionHandler, validateId)
{
	zend_string *key;

	PS_SANITY_CHECK_IS_OPEN;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &key) == FAILURE) {
		return;
	}

	/* Legacy save handler may not support validate_sid API. Return TRUE. */
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool SessionUpdateTimestampHandler::updateTimestamp(string id, string data)
   Simply call update_timestamp */
PHP_METHOD(SessionHandler, updateTimestamp)
{
	zend_string *key, *val;

	PS_SANITY_CHECK_IS_OPEN;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS", &key, &val) == FAILURE) {
		return;
	}

	/* Legacy save handler may not support update_timestamp API. Just write. */
	RETVAL_BOOL(SUCCESS == PS(default_mod)->s_write(&PS(mod_data), key, val, PS(gc_maxlifetime)));
}
/* }}} */
