/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,	  |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Felipe Pena <felipe@php.net>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#ifndef PHPDBG_WATCH_H
#define PHPDBG_WATCH_H

#include "TSRM.h"
#include "phpdbg_cmd.h"

#define PHPDBG_WATCH(name) PHPDBG_COMMAND(watch_##name)

/**
* Printer Forward Declarations
*/
/*PHPDBG_WATCH();*/


/* Watchpoint functions/typedefs */

typedef enum {
	WATCH_ON_ZVAL,
	WATCH_ON_HASHTABLE,
	WATCH_ON_PTR
} phpdbg_watchtype;

typedef struct _phpdbg_watchpoint_t phpdbg_watchpoint_t;

struct _phpdbg_watchpoint_t {
	phpdbg_watchpoint_t *parent;
	char *str;
	union {
		zval *zv;
		HashTable *ht;
		void *ptr;
	} addr;
	size_t size;
	phpdbg_watchtype type;
};

void phpdbg_setup_watchpoints(TSRMLS_D);

int phpdbg_watchpoint_segfault_handler(siginfo_t *info, void *context TSRMLS_DC);

void phpdbg_create_addr_watchpoint(void *addr, size_t size, phpdbg_watchpoint_t *watch);
void phpdbg_create_zval_watchpoint(zval *zv, phpdbg_watchpoint_t *watch);
int phpdbg_create_var_watchpoint(char *name, size_t len TSRMLS_DC);

int phpdbg_print_changed_zvals(TSRMLS_D);

#endif
