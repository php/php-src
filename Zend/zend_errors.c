/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Benjamin Eberlei <beberlei@php.net>                         |
   +----------------------------------------------------------------------+
*/

#include "zend.h"

/**
 * A display callback is responsible for rendering an error. Extensions can
 * append to the list of core error handlers. Callbacks are triggered from the
 * tail of the list going to head.
 */
zend_llist zend_error_display_callbacks;

char *zend_error_get_type_string(int type)
{
	char *error_type_str;

	switch (type) {
		case E_ERROR:
		case E_CORE_ERROR:
		case E_COMPILE_ERROR:
		case E_USER_ERROR:
			error_type_str = "Fatal error";
			break;
		case E_RECOVERABLE_ERROR:
			error_type_str = "Recoverable fatal error";
			break;
		case E_WARNING:
		case E_CORE_WARNING:
		case E_COMPILE_WARNING:
		case E_USER_WARNING:
			error_type_str = "Warning";
			break;
		case E_PARSE:
			error_type_str = "Parse error";
			break;
		case E_NOTICE:
		case E_USER_NOTICE:
			error_type_str = "Notice";
			break;
		case E_STRICT:
			error_type_str = "Strict Standards";
			break;
		case E_DEPRECATED:
		case E_USER_DEPRECATED:
			error_type_str = "Deprecated";
			break;
		default:
			error_type_str = "Unknown error";
			break;
	}

	return error_type_str;
}

void zend_startup_error_display_functions(void)
{
	zend_llist_init(&zend_error_display_callbacks, sizeof(zend_error_display_callback), NULL, 1);
}

void zend_shutdown_error_display_functions(void)
{
	zend_llist_destroy(&zend_error_display_callbacks);
}

void zend_register_error_display_callback(zend_error_display_cb cb)
{
	zend_error_display_callback callback;

	callback.display_callback = cb;

	zend_llist_add_element(&zend_error_display_callbacks, &callback);
}

int zend_error_display_handle(int type, const char *error_filename, const uint32_t error_lineno, char *buffer, int buffer_len)
{
	zend_llist_element *element;
	zend_error_display_callback *callback;
	int handled = 0;

	for (element = zend_error_display_callbacks.tail; element; element = element->prev) {
		callback = (zend_error_display_callback*) element->data;
		handled = callback->display_callback(type, error_filename, error_lineno, buffer, buffer_len);

		if (handled == 1) {
			return 1;
		}
	}

	return 0;
}
