/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
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

#ifdef _WIN32
# include "phpdbg_win.h"
#endif

#define PHPDBG_WATCH(name) PHPDBG_COMMAND(watch_##name)

/**
 * Printer Forward Declarations
 */
PHPDBG_WATCH(array);
PHPDBG_WATCH(delete);
PHPDBG_WATCH(recursive);

/**
 * Commands
 */

static const phpdbg_command_t phpdbg_watch_commands[] = {
	PHPDBG_COMMAND_D_EX(array,      "create watchpoint on an array", 'a', watch_array,     NULL, "s"),
	PHPDBG_COMMAND_D_EX(delete,     "delete watchpoint",             'd', watch_delete,    NULL, "s"),
	PHPDBG_COMMAND_D_EX(recursive,  "create recursive watchpoints",  'r', watch_recursive, NULL, "s"),
	PHPDBG_END_COMMAND
};

/* Watchpoint functions/typedefs */

typedef enum {
	WATCH_ON_ZVAL,
	WATCH_ON_HASHTABLE,
} phpdbg_watchtype;


#define PHPDBG_WATCH_SIMPLE	0x0
#define PHPDBG_WATCH_RECURSIVE	0x1

typedef struct _phpdbg_watchpoint_t phpdbg_watchpoint_t;

struct _phpdbg_watchpoint_t {
	phpdbg_watchpoint_t *parent;
	HashTable *parent_container;
	char *name_in_parent;
	size_t name_in_parent_len;
	char *str;
	size_t str_len;
	union {
		zval *zv;
		HashTable *ht;
		void *ptr;
	} addr;
	size_t size;
	phpdbg_watchtype type;
	char flags;
};

void phpdbg_setup_watchpoints(TSRMLS_D);

#ifndef _WIN32
int phpdbg_watchpoint_segfault_handler(siginfo_t *info, void *context TSRMLS_DC);
#else
int phpdbg_watchpoint_segfault_handler(void *addr TSRMLS_DC);
#endif

void phpdbg_create_addr_watchpoint(void *addr, size_t size, phpdbg_watchpoint_t *watch);
void phpdbg_create_zval_watchpoint(zval *zv, phpdbg_watchpoint_t *watch);

int phpdbg_delete_var_watchpoint(char *input, size_t len TSRMLS_DC);
int phpdbg_create_var_watchpoint(char *input, size_t len TSRMLS_DC);

int phpdbg_print_changed_zvals(TSRMLS_D);

void phpdbg_list_watchpoints(TSRMLS_D);

void phpdbg_watch_efree(void *ptr);


static long phpdbg_pagesize;

static zend_always_inline void *phpdbg_get_page_boundary(void *addr) {
	return (void *)((size_t)addr & ~(phpdbg_pagesize - 1));
}

static zend_always_inline size_t phpdbg_get_total_page_size(void *addr, size_t size) {
	return (size_t)phpdbg_get_page_boundary((void *)((size_t)addr + size - 1)) - (size_t)phpdbg_get_page_boundary(addr) + phpdbg_pagesize;
}

#endif
