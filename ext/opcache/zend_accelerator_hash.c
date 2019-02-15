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

#include "ZendAccelerator.h"
#include "zend_accelerator_hash.h"
#include "zend_hash.h"
#include "zend_shared_alloc.h"

/* Generated on an Octa-ALPHA 300MHz CPU & 2.5GB RAM monster */
static uint32_t prime_numbers[] =
	{5, 11, 19, 53, 107, 223, 463, 983, 1979, 3907, 7963, 16229, 32531, 65407, 130987, 262237, 524521, 1048793 };
static uint32_t num_prime_numbers = sizeof(prime_numbers) / sizeof(uint32_t);

void zend_accel_hash_clean(zend_accel_hash *accel_hash)
{
	accel_hash->num_entries = 0;
	accel_hash->num_direct_entries = 0;
	memset(accel_hash->hash_table, 0, sizeof(zend_accel_hash_entry *)*accel_hash->max_num_entries);
}

void zend_accel_hash_init(zend_accel_hash *accel_hash, uint32_t hash_size)
{
	uint32_t i;

	for (i=0; i<num_prime_numbers; i++) {
		if (hash_size <= prime_numbers[i]) {
			hash_size = prime_numbers[i];
			break;
		}
	}

	accel_hash->num_entries = 0;
	accel_hash->num_direct_entries = 0;
	accel_hash->max_num_entries = hash_size;

	/* set up hash pointers table */
	accel_hash->hash_table = zend_shared_alloc(sizeof(zend_accel_hash_entry *)*accel_hash->max_num_entries);
	if (!accel_hash->hash_table) {
		zend_accel_error(ACCEL_LOG_FATAL, "Insufficient shared memory!");
		return;
	}

	/* set up hash values table */
	accel_hash->hash_entries = zend_shared_alloc(sizeof(zend_accel_hash_entry)*accel_hash->max_num_entries);
	if (!accel_hash->hash_entries) {
		zend_accel_error(ACCEL_LOG_FATAL, "Insufficient shared memory!");
		return;
	}
	memset(accel_hash->hash_table, 0, sizeof(zend_accel_hash_entry *)*accel_hash->max_num_entries);
}

/* Returns NULL if hash is full
 * Returns pointer the actual hash entry on success
 * key needs to be already allocated as it is not copied
 */
zend_accel_hash_entry* zend_accel_hash_update(zend_accel_hash *accel_hash, const char *key, uint32_t key_length, zend_bool indirect, void *data)
{
	zend_ulong hash_value;
	zend_ulong index;
	zend_accel_hash_entry *entry;
	zend_accel_hash_entry *indirect_bucket = NULL;

	if (indirect) {
		indirect_bucket = (zend_accel_hash_entry*)data;
		while (indirect_bucket->indirect) {
			indirect_bucket = (zend_accel_hash_entry*)indirect_bucket->data;
		}
	}

	hash_value = zend_inline_hash_func(key, key_length);
#ifndef ZEND_WIN32
	hash_value ^= ZCG(root_hash);
#endif
	index = hash_value % accel_hash->max_num_entries;

	/* try to see if the element already exists in the hash */
	entry = accel_hash->hash_table[index];
	while (entry) {
		if (entry->hash_value == hash_value
			&& entry->key_length == key_length
			&& !memcmp(entry->key, key, key_length)) {

			if (entry->indirect) {
				if (indirect_bucket) {
					entry->data = indirect_bucket;
				} else {
					((zend_accel_hash_entry*)entry->data)->data = data;
				}
			} else {
				if (indirect_bucket) {
					accel_hash->num_direct_entries--;
					entry->data = indirect_bucket;
					entry->indirect = 1;
				} else {
					entry->data = data;
				}
			}
			return entry;
		}
		entry = entry->next;
	}

	/* Does not exist, add a new entry */
	if (accel_hash->num_entries == accel_hash->max_num_entries) {
		return NULL;
	}

	entry = &accel_hash->hash_entries[accel_hash->num_entries++];
	if (indirect) {
		entry->data = indirect_bucket;
		entry->indirect = 1;
	} else {
		accel_hash->num_direct_entries++;
		entry->data = data;
		entry->indirect = 0;
	}
	entry->hash_value = hash_value;
	entry->key = key;
	entry->key_length = key_length;
	entry->next = accel_hash->hash_table[index];
	accel_hash->hash_table[index] = entry;
	return entry;
}

static zend_always_inline void* zend_accel_hash_find_ex(zend_accel_hash *accel_hash, const char *key, uint32_t key_length, zend_ulong hash_value, int data)
{
	zend_ulong index;
	zend_accel_hash_entry *entry;

#ifndef ZEND_WIN32
	hash_value ^= ZCG(root_hash);
#endif
	index = hash_value % accel_hash->max_num_entries;

	entry = accel_hash->hash_table[index];
	while (entry) {
		if (entry->hash_value == hash_value
			&& entry->key_length == key_length
			&& !memcmp(entry->key, key, key_length)) {
			if (entry->indirect) {
				if (data) {
					return ((zend_accel_hash_entry*)entry->data)->data;
				} else {
					return entry->data;
				}
			} else {
				if (data) {
					return entry->data;
				} else {
					return entry;
				}
			}
		}
		entry = entry->next;
	}
	return NULL;
}

/* Returns the data associated with key on success
 * Returns NULL if data doesn't exist
 */
void* zend_accel_hash_find(zend_accel_hash *accel_hash, zend_string *key)
{
	return zend_accel_hash_find_ex(
		accel_hash,
		ZSTR_VAL(key),
		ZSTR_LEN(key),
		zend_string_hash_val(key),
		1);
}

/* Returns the hash entry associated with key on success
 * Returns NULL if it doesn't exist
 */
zend_accel_hash_entry* zend_accel_hash_find_entry(zend_accel_hash *accel_hash, zend_string *key)
{
	return (zend_accel_hash_entry *)zend_accel_hash_find_ex(
		accel_hash,
		ZSTR_VAL(key),
		ZSTR_LEN(key),
		zend_string_hash_val(key),
		0);
}

/* Returns the data associated with key on success
 * Returns NULL if data doesn't exist
 */
void* zend_accel_hash_str_find(zend_accel_hash *accel_hash, const char *key, uint32_t key_length)
{
	return zend_accel_hash_find_ex(
		accel_hash,
		key,
		key_length,
		zend_inline_hash_func(key, key_length),
		1);
}

/* Returns the hash entry associated with key on success
 * Returns NULL if it doesn't exist
 */
zend_accel_hash_entry* zend_accel_hash_str_find_entry(zend_accel_hash *accel_hash, const char *key, uint32_t key_length)
{
	return (zend_accel_hash_entry *)zend_accel_hash_find_ex(
		accel_hash,
		key,
		key_length,
		zend_inline_hash_func(key, key_length),
		0);
}

int zend_accel_hash_unlink(zend_accel_hash *accel_hash, const char *key, uint32_t key_length)
{
    zend_ulong hash_value;
    zend_ulong index;
    zend_accel_hash_entry *entry, *last_entry=NULL;

	hash_value = zend_inline_hash_func(key, key_length);
#ifndef ZEND_WIN32
	hash_value ^= ZCG(root_hash);
#endif
	index = hash_value % accel_hash->max_num_entries;

	entry = accel_hash->hash_table[index];
	while (entry) {
		if (entry->hash_value == hash_value
			&& entry->key_length == key_length
			&& !memcmp(entry->key, key, key_length)) {
			if (!entry->indirect) {
				accel_hash->num_direct_entries--;
			}
			if (last_entry) {
				last_entry->next = entry->next;
			} else {
				accel_hash->hash_table[index] = entry->next;
			}
			return SUCCESS;
		}
		last_entry = entry;
		entry = entry->next;
	}
	return FAILURE;
}
