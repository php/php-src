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
   | Author: Stig Bakken <ssb@php.net>                                    |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "php_ticks.h"

struct st_tick_function
{
	void (*func)(int, void *);
	void *arg;
};

int php_startup_ticks(void)
{
	zend_llist_init(&PG(tick_functions), sizeof(struct st_tick_function), NULL, 1);
	return SUCCESS;
}

void php_deactivate_ticks(void)
{
	zend_llist_clean(&PG(tick_functions));
}

void php_shutdown_ticks(php_core_globals *core_globals)
{
	zend_llist_destroy(&core_globals->tick_functions);
}

static int php_compare_tick_functions(void *elem1, void *elem2)
{
	struct st_tick_function *e1 = (struct st_tick_function *)elem1;
	struct st_tick_function *e2 = (struct st_tick_function *)elem2;
	return e1->func == e2->func && e1->arg == e2->arg;
}

PHPAPI void php_add_tick_function(void (*func)(int, void*), void * arg)
{
	struct st_tick_function tmp = {func, arg};
	zend_llist_add_element(&PG(tick_functions), (void *)&tmp);
}

PHPAPI void php_remove_tick_function(void (*func)(int, void *), void * arg)
{
	struct st_tick_function tmp = {func, arg};
	zend_llist_del_element(&PG(tick_functions), (void *)&tmp, (int(*)(void*, void*))php_compare_tick_functions);
}

static void php_tick_iterator(void *d, void *arg)
{
	struct st_tick_function *data = (struct st_tick_function *)d;
	data->func(*((int *)arg), data->arg);
}

void php_run_ticks(int count)
{
	zend_llist_apply_with_argument(&PG(tick_functions), (llist_apply_with_arg_func_t) php_tick_iterator, &count);
}
