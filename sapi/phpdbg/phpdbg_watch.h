/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
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
#	include "phpdbg_win.h"
#endif

#define PHPDBG_WATCH(name) PHPDBG_COMMAND(watch_##name)

/**
 * Printer Forward Declarations
 */
PHPDBG_WATCH(array);
PHPDBG_WATCH(delete);
PHPDBG_WATCH(recursive);

extern const phpdbg_command_t phpdbg_watch_commands[];

/* Watchpoint functions/typedefs */

typedef enum {
	WATCH_ON_ZVAL,
	WATCH_ON_HASHTABLE,
	WATCH_ON_REFCOUNTED,
} phpdbg_watchtype;


#define PHPDBG_WATCH_SIMPLE     0x0
#define PHPDBG_WATCH_RECURSIVE  0x1
#define PHPDBG_WATCH_ARRAY      0x2
#define PHPDBG_WATCH_OBJECT     0x4

typedef struct _phpdbg_watchpoint_t phpdbg_watchpoint_t;

struct _phpdbg_watchpoint_t {
	union {
		zval *zv;
		HashTable *ht;
		zend_refcounted *ref;
		void *ptr;
	} addr;
	size_t size;
	phpdbg_watchtype type;
	char flags;
	phpdbg_watchpoint_t *parent;
	HashTable *parent_container;
	char *name_in_parent;
	size_t name_in_parent_len;
	char *str;
	size_t str_len;
};

typedef struct {
	phpdbg_watchpoint_t watch;
	unsigned int num;
	unsigned int refs;
	HashTable watches;
} phpdbg_watch_collision;

void phpdbg_setup_watchpoints(void);

#ifndef _WIN32
int phpdbg_watchpoint_segfault_handler(siginfo_t *info, void *context);
#else
int phpdbg_watchpoint_segfault_handler(void *addr);
#endif

void phpdbg_create_addr_watchpoint(void *addr, size_t size, phpdbg_watchpoint_t *watch);
void phpdbg_create_zval_watchpoint(zval *zv, phpdbg_watchpoint_t *watch);

int phpdbg_delete_var_watchpoint(char *input, size_t len);
int phpdbg_create_var_watchpoint(char *input, size_t len);

int phpdbg_print_changed_zvals(void);

void phpdbg_list_watchpoints(void);

void phpdbg_watch_efree(void *ptr);


static long phpdbg_pagesize;

static zend_always_inline void *phpdbg_get_page_boundary(void *addr) {
	return (void *) ((size_t) addr & ~(phpdbg_pagesize - 1));
}

static zend_always_inline size_t phpdbg_get_total_page_size(void *addr, size_t size) {
	return (size_t) phpdbg_get_page_boundary((void *) ((size_t) addr + size - 1)) - (size_t) phpdbg_get_page_boundary(addr) + phpdbg_pagesize;
}

#endif
