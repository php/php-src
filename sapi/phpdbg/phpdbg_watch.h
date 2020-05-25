/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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

/* References are managed through their parent zval *, being a simple WATCH_ON_ZVAL and eventually WATCH_ON_REFCOUNTED */
typedef enum {
	WATCH_ON_ZVAL,
	WATCH_ON_HASHTABLE,
	WATCH_ON_REFCOUNTED,
	WATCH_ON_STR,
	WATCH_ON_HASHDATA,
	WATCH_ON_BUCKET,
} phpdbg_watchtype;


#define PHPDBG_WATCH_SIMPLE     0x01
#define PHPDBG_WATCH_RECURSIVE  0x02
#define PHPDBG_WATCH_ARRAY      0x04
#define PHPDBG_WATCH_OBJECT     0x08
#define PHPDBG_WATCH_NORMAL     (PHPDBG_WATCH_SIMPLE | PHPDBG_WATCH_RECURSIVE)
#define PHPDBG_WATCH_IMPLICIT   0x10
#define PHPDBG_WATCH_RECURSIVE_ROOT 0x20

typedef struct _phpdbg_watch_collision phpdbg_watch_collision;

typedef struct _phpdbg_watchpoint_t {
	union {
		zval *zv;
		zend_refcounted *ref;
		Bucket *bucket;
		void *ptr;
	} addr;
	size_t size;
	phpdbg_watchtype type;
	zend_refcounted *ref; /* key to fetch the collision on parents */
	HashTable elements;
	phpdbg_watch_collision *coll; /* only present on *children* */
	union {
		zval zv;
		Bucket bucket;
		zend_refcounted ref;
		HashTable ht;
		zend_string *str;
	} backup;
} phpdbg_watchpoint_t;

struct _phpdbg_watch_collision {
	phpdbg_watchpoint_t ref;
	phpdbg_watchpoint_t reference;
	HashTable parents;
};

typedef struct _phpdbg_watch_element {
	uint32_t id;
	phpdbg_watchpoint_t *watch;
	char flags;
	struct _phpdbg_watch_element *child; /* always set for implicit watches */
	struct _phpdbg_watch_element *parent;
	HashTable child_container; /* children of this watch element for recursive array elements */
	HashTable *parent_container; /* container of the value */
	zend_string *name_in_parent;
	zend_string *str;
	union {
		zval zv;
		zend_refcounted ref;
		HashTable ht;
	} backup; /* backup for when watchpoint gets dissociated */
} phpdbg_watch_element;

typedef struct {
	/* to watch rehashes (yes, this is not *perfect*, but good enough for everything in PHP...) */
	phpdbg_watchpoint_t hash_watch; /* must be first element */
	Bucket *last;
	zend_string *last_str;
	zend_ulong last_idx;

	HashTable *ht;
	size_t data_size;
	HashTable watches; /* contains phpdbg_watch_element */
} phpdbg_watch_ht_info;

void phpdbg_setup_watchpoints(void);
void phpdbg_destroy_watchpoints(void);
void phpdbg_purge_watchpoint_tree(void);

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
