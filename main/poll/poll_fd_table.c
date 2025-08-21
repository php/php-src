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

/* Initialize FD table */
php_poll_fd_table *php_poll_fd_table_init(int initial_capacity, bool persistent)
{
	php_poll_fd_table *table = pecalloc(1, sizeof(php_poll_fd_table), persistent);
	if (!table) {
		return NULL;
	}

	if (initial_capacity <= 0) {
		initial_capacity = 64;
	}

	table->entries = pecalloc(initial_capacity, sizeof(php_poll_fd_entry), persistent);
	if (!table->entries) {
		pefree(table, persistent);
		return NULL;
	}

	table->capacity = initial_capacity;
	table->count = 0;
	table->persistent = persistent;
	return table;
}

/* Cleanup FD table */
void php_poll_fd_table_cleanup(php_poll_fd_table *table)
{
	if (table) {
		pefree(table->entries, table->persistent);
		pefree(table, table->persistent);
	}
}

/* Find FD entry */
php_poll_fd_entry *php_poll_fd_table_find(php_poll_fd_table *table, int fd)
{
	for (int i = 0; i < table->capacity; i++) {
		if (table->entries[i].active && table->entries[i].fd == fd) {
			return &table->entries[i];
		}
	}
	return NULL;
}

/* Get or create FD entry */
php_poll_fd_entry *php_poll_fd_table_get(php_poll_fd_table *table, int fd)
{
	php_poll_fd_entry *entry = php_poll_fd_table_find(table, fd);
	if (entry) {
		return entry;
	}

	/* Find empty slot */
	for (int i = 0; i < table->capacity; i++) {
		if (!table->entries[i].active) {
			table->entries[i].fd = fd;
			table->entries[i].active = true;
			table->entries[i].last_revents = 0;
			table->count++;
			return &table->entries[i];
		}
	}

	/* Need to grow the array */
	int new_capacity = table->capacity * 2;
	php_poll_fd_entry *new_entries = perealloc(
			table->entries, new_capacity * sizeof(php_poll_fd_entry), table->persistent);
	if (!new_entries) {
		return NULL;
	}

	/* Initialize new entries */
	memset(new_entries + table->capacity, 0,
			(new_capacity - table->capacity) * sizeof(php_poll_fd_entry));

	table->entries = new_entries;

	/* Use first new slot */
	php_poll_fd_entry *new_entry = &table->entries[table->capacity];
	new_entry->fd = fd;
	new_entry->active = true;
	new_entry->last_revents = 0;
	table->count++;

	table->capacity = new_capacity;
	return new_entry;
}

/* Remove FD entry */
void php_poll_fd_table_remove(php_poll_fd_table *table, int fd)
{
	php_poll_fd_entry *entry = php_poll_fd_table_find(table, fd);
	if (entry) {
		entry->active = false;
		table->count--;
	}
}

/* Edge-trigger simulation helper */
int php_poll_simulate_edge_trigger(php_poll_fd_table *table, php_poll_event *events, int nfds)
{
	int filtered_count = 0;

	for (int i = 0; i < nfds; i++) {
		php_poll_fd_entry *entry = php_poll_fd_table_find(table, events[i].fd);
		if (!entry) {
			continue;
		}

		uint32_t new_events = events[i].revents;
		uint32_t reported_events = 0;

		if (entry->events & PHP_POLL_ET) {
			/* Edge-triggered: report edges only */
			if ((new_events & PHP_POLL_READ) && !(entry->last_revents & PHP_POLL_READ)) {
				reported_events |= PHP_POLL_READ;
			}
			if ((new_events & PHP_POLL_WRITE) && !(entry->last_revents & PHP_POLL_WRITE)) {
				reported_events |= PHP_POLL_WRITE;
			}
			/* Always report error and hangup events */
			reported_events |= (new_events & (PHP_POLL_ERROR | PHP_POLL_HUP | PHP_POLL_RDHUP));
		} else {
			/* Level-triggered: report all active events */
			reported_events = new_events;
		}

		entry->last_revents = new_events;

		/* Only include this event if we have something to report */
		if (reported_events != 0) {
			if (filtered_count != i) {
				events[filtered_count] = events[i];
			}
			events[filtered_count].revents = reported_events;
			filtered_count++;
		}
	}

	return filtered_count;
}
