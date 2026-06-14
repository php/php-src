/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
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
bool zend_accel_blacklist_is_blacklisted(const zend_blacklist *blacklist, const char *verify_path, size_t verify_path_len);
void zend_accel_blacklist_apply(const zend_blacklist *blacklist, blacklist_apply_func_arg_t func, void *argument);

#endif /* ZEND_ACCELERATOR_BLACKLIST_H */
