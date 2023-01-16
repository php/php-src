/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
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
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Stanislav Malyshev <stas@zend.com>                          |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_ACCELERATOR_HASH_H
#define ZEND_ACCELERATOR_HASH_H

#include "zend.h"

/*
	zend_accel_hash - is a hash table allocated in shared memory and
	distributed across simultaneously running processes. The hash tables have
	fixed sizen selected during construction by zend_accel_hash_init(). All the
	hash entries are preallocated in the 'hash_entries' array. 'num_entries' is
	initialized by zero and grows when new data is added.
	zend_accel_hash_update() just takes the next entry from 'hash_entries'
	array and puts it into appropriate place of 'hash_table'.
	Hash collisions are resolved by separate chaining with linked lists,
	however, entries are still taken from the same 'hash_entries' array.
	'key' and 'data' passed to zend_accel_hash_update() must be already
	allocated in shared memory. Few keys may be resolved to the same data.
	using 'indirect' entries, that point to other entries ('data' is actually
	a pointer to another zend_accel_hash_entry).
	zend_accel_hash_update() requires exclusive lock, however,
	zend_accel_hash_find() does not.
*/

typedef struct _zend_accel_hash_entry zend_accel_hash_entry;

struct _zend_accel_hash_entry {
	zend_ulong             hash_value;
	zend_string           *key;
	zend_accel_hash_entry *next;
	void                  *data;
	bool                   indirect;
};

typedef struct _zend_accel_hash {
	zend_accel_hash_entry **hash_table;
	zend_accel_hash_entry  *hash_entries;
	uint32_t               num_entries;
	uint32_t               max_num_entries;
	uint32_t               num_direct_entries;
} zend_accel_hash;

BEGIN_EXTERN_C()

void zend_accel_hash_init(zend_accel_hash *accel_hash, uint32_t hash_size);
void zend_accel_hash_clean(zend_accel_hash *accel_hash);

zend_accel_hash_entry* zend_accel_hash_update(
		zend_accel_hash        *accel_hash,
		zend_string            *key,
		bool                   indirect,
		void                   *data);

void* zend_accel_hash_find(
		zend_accel_hash        *accel_hash,
		zend_string            *key);

zend_accel_hash_entry* zend_accel_hash_find_entry(
		zend_accel_hash        *accel_hash,
		zend_string            *key);

int zend_accel_hash_unlink(
		zend_accel_hash        *accel_hash,
		zend_string            *key);

static inline bool zend_accel_hash_is_full(zend_accel_hash *accel_hash)
{
	if (accel_hash->num_entries == accel_hash->max_num_entries) {
		return 1;
	} else {
		return 0;
	}
}

END_EXTERN_C()

#endif /* ZEND_ACCELERATOR_HASH_H */
