/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stig Bakken <ssb@fast.no>                                   |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "php_ticks.h"

int php_startup_ticks(PLS_D)
{
	zend_llist_init(&PG(tick_functions), sizeof(PG(tick_functions)), NULL, 1);
	return SUCCESS;
}

void php_shutdown_ticks(PLS_D)
{
	zend_llist_destroy(&PG(tick_functions));
}

static int php_compare_tick_functions(void *elem1, void *elem2)
{
	return ((void (*)(int))elem1 == (void (*)(int))elem2);
}

PHPAPI int php_add_tick_function(void (*func)(int))
{
	PLS_FETCH();

	zend_llist_add_element(&PG(tick_functions), func);

	return SUCCESS;
}

PHPAPI int php_remove_tick_function(void (*func)(int))
{
	PLS_FETCH();

	zend_llist_del_element(&PG(tick_functions), func,
						   (int(*)(void*,void*))php_compare_tick_functions);
	return SUCCESS;
}

void php_tick_iterator(void *data, void *arg)
{
	void (*func)(int);
	func = (void(*)(int))data;
	func(*((int *)arg));
}

void php_run_ticks(int count)
{
	PLS_FETCH();

	zend_llist_apply_with_argument(&PG(tick_functions), (void(*)(void*,void*))php_tick_iterator, &count);
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
