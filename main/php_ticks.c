/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Stig Bakken <ssb@fast.no>                                    |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "php_ticks.h"

int php_startup_ticks(TSRMLS_D)
{
	zend_llist_init(&PG(tick_functions), sizeof(void(*)(int)), NULL, 1);
	return SUCCESS;
}

void php_shutdown_ticks(TSRMLS_D)
{
	zend_llist_destroy(&PG(tick_functions));
}

static int php_compare_tick_functions(void *elem1, void *elem2)
{
	void(*func1)(int);
	void(*func2)(int);
	memcpy(&func1, elem1, sizeof(void(*)(int)));
	memcpy(&func2, elem2, sizeof(void(*)(int)));
	return (func1 == func2);
}

PHPAPI void php_add_tick_function(void (*func)(int))
{
	TSRMLS_FETCH();

	zend_llist_add_element(&PG(tick_functions), (void *)&func);
}

PHPAPI void php_remove_tick_function(void (*func)(int))
{
	TSRMLS_FETCH();

	zend_llist_del_element(&PG(tick_functions), (void *)func,
						   (int(*)(void*, void*))php_compare_tick_functions);
}

static void php_tick_iterator(void *data, void *arg TSRMLS_DC)
{
	void (*func)(int);

	memcpy(&func, data, sizeof(void(*)(int)));
	func(*((int *)arg));
}

void php_run_ticks(int count)
{
	TSRMLS_FETCH();

	zend_llist_apply_with_argument(&PG(tick_functions), (llist_apply_with_arg_func_t) php_tick_iterator, &count TSRMLS_CC);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
