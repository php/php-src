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

#ifndef ZEND_ACCELERATOR_BLOCKLIST_H
#define ZEND_ACCELERATOR_BLOCKLIST_H

typedef struct _zend_regexp_list zend_regexp_list;

typedef struct _zend_exclude_list_entry {
    char *path;
    int   path_length;
	int   id;
} zend_exclude_list_entry;

typedef struct _zend_exclude_list {
	zend_exclude_list_entry *entries;
	int                   size;
	int                   pos;
	zend_regexp_list     *regexp_list;
} zend_exclude_list;

typedef int (*exclude_list_apply_func_arg_t)(zend_exclude_list_entry *, zval *);

extern zend_exclude_list accel_exclude_list;

void zend_accel_exclude_list_init(zend_exclude_list *exclude_list);
void zend_accel_exclude_list_shutdown(zend_exclude_list *exclude_list);

void zend_accel_exclude_list_load(zend_exclude_list *exclude_list, char *filename);
zend_bool zend_accel_exclude_list_is_excluded(zend_exclude_list *exclude_list, char *verify_path, size_t verify_path_len);
void zend_accel_exclude_list_apply(zend_exclude_list *exclude_list, exclude_list_apply_func_arg_t func, void *argument);

#endif /* ZEND_ACCELERATOR_BLOCKLIST_H */
