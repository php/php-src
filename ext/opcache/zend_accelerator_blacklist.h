/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Stanislav Malyshev <stas@zend.com>                          |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_ACCELERATOR_BLACKLIST_H
#define ZEND_ACCELERATOR_BLACKLIST_H

typedef struct _zend_regexp_list zend_regexp_list;

typedef struct _zend_blacklist_entry {
    char *path;
    int   path_length;
	int   id;
} zend_blacklist_entry;

typedef struct _zend_blacklist {
	zend_blacklist_entry *entries;
	int                   size;
	int                   pos;
	zend_regexp_list     *regexp_list;
} zend_blacklist;

typedef int (*blacklist_apply_func_arg_t)(zend_blacklist_entry *, zval *);

extern zend_blacklist accel_blacklist;

void zend_accel_blacklist_init(zend_blacklist *blacklist);
void zend_accel_blacklist_shutdown(zend_blacklist *blacklist);

void zend_accel_blacklist_load(zend_blacklist *blacklist, char *filename);
zend_bool zend_accel_blacklist_is_blacklisted(zend_blacklist *blacklist, char *verify_path, size_t verify_path_len);
void zend_accel_blacklist_apply(zend_blacklist *blacklist, blacklist_apply_func_arg_t func, void *argument);

#endif /* ZEND_ACCELERATOR_BLACKLIST_H */
