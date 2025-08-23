/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Jakub Zelenka <bukka@php.net>                               |
   +----------------------------------------------------------------------+
*/

#include "php_poll_internal.h"

php_poll_fd_table *php_poll_fd_table_init(int initial_capacity, bool persistent)
{
	php_poll_fd_table *table = php_poll_calloc(1, sizeof(php_poll_fd_table), persistent);
	if (!table) {
		return NULL;
	}

	if (initial_capacity <= 0) {
		initial_capacity = 64;
	}

	_zend_hash_init(&table->entries_ht, initial_capacity, NULL, persistent);
	table->persistent = persistent;

	return table;
}

void php_poll_fd_table_cleanup(php_poll_fd_table *table)
{
	if (table) {
		zend_ulong fd;
		zval *zv;

		ZEND_HASH_FOREACH_NUM_KEY_VAL(&table->entries_ht, fd, zv)
		{
			php_poll_fd_entry *entry = Z_PTR_P(zv);
			pefree(entry, table->persistent);
		}
		ZEND_HASH_FOREACH_END();

		zend_hash_destroy(&table->entries_ht);
		pefree(table, table->persistent);
	}
}

php_poll_fd_entry *php_poll_fd_table_find(php_poll_fd_table *table, int fd)
{
	zval *zv = zend_hash_index_find(&table->entries_ht, (zend_ulong) fd);
	return zv ? Z_PTR_P(zv) : NULL;
}

php_poll_fd_entry *php_poll_fd_table_get(php_poll_fd_table *table, int fd)
{
	php_poll_fd_entry *entry = php_poll_fd_table_find(table, fd);
	if (entry) {
		return entry;
	}

	entry = php_poll_calloc(1, sizeof(php_poll_fd_entry), table->persistent);
	if (!entry) {
		return NULL;
	}

	entry->fd = fd;
	entry->active = true;
	entry->events = 0;
	entry->data = NULL;
	entry->last_revents = 0;

	zval zv;
	ZVAL_PTR(&zv, entry);
	if (!zend_hash_index_add(&table->entries_ht, (zend_ulong) fd, &zv)) {
		pefree(entry, table->persistent);
		return NULL;
	}

	return entry;
}

void php_poll_fd_table_remove(php_poll_fd_table *table, int fd)
{
	zval *zv = zend_hash_index_find(&table->entries_ht, (zend_ulong) fd);
	if (zv) {
		php_poll_fd_entry *entry = Z_PTR_P(zv);
		pefree(entry, table->persistent);
		zend_hash_index_del(&table->entries_ht, (zend_ulong) fd);
	}
}

/* Helper function for backends that need to iterate over all entries */
typedef bool (*php_poll_fd_iterator_func_t)(int fd, php_poll_fd_entry *entry, void *user_data);

/* Iterate over all active FD entries */
void php_poll_fd_table_foreach(
		php_poll_fd_table *table, php_poll_fd_iterator_func_t callback, void *user_data)
{
	zend_ulong fd;
	zval *zv;

	ZEND_HASH_FOREACH_NUM_KEY_VAL(&table->entries_ht, fd, zv)
	{
		php_poll_fd_entry *entry = Z_PTR_P(zv);
		if (entry->active && !callback((int) fd, entry, user_data)) {
			break; /* Callback returned false, stop iteration */
		}
	}
	ZEND_HASH_FOREACH_END();
}
