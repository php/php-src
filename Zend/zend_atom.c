/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: [Your Name]                                                  |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_atom.h"
#include "zend_hash.h"
#include "zend_string.h"
#include "zend_alloc.h"

ZEND_API HashTable atom_table;
ZEND_API uint32_t next_atom_id = 1; /* Start from 1, 0 is reserved for invalid */
static bool atoms_initialized = false;

static void atom_dtor(zval *zv)
{
	zend_atom *atom = (zend_atom *)Z_PTR_P(zv);
	zend_string_release(atom->name);
	efree(atom);
}

ZEND_API void zend_atoms_init(void)
{
	zend_hash_init(&atom_table, 64, NULL, atom_dtor, 0);
	next_atom_id = 1;
}

ZEND_API void zend_atoms_shutdown(void)
{
	zend_hash_destroy(&atom_table);
}

ZEND_API bool zend_atom_name_is_valid(const char *name, size_t name_len)
{
	if (name_len == 0) {
		return false;
	}

	/* First character must be [a-zA-Z_\x80-\xff] */
	unsigned char c = (unsigned char)name[0];
	if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c >= 0x80)) {
		return false;
	}

	/* Subsequent characters must be [a-zA-Z0-9_\x80-\xff] */
	for (size_t i = 1; i < name_len; i++) {
		c = (unsigned char)name[i];
		if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || 
			  (c >= '0' && c <= '9') || c == '_' || c >= 0x80)) {
			return false;
		}
	}

	return true;
}

ZEND_API uint32_t zend_atom_create(zend_string *name)
{
	zval *existing;
	zend_atom *atom;
	zval atom_zv;

	if (!atoms_initialized) {
		zend_startup_atoms();
	}

	/* Check if atom already exists */
	existing = zend_hash_find(&atom_table, name);
	if (existing) {
		atom = (zend_atom *)Z_PTR_P(existing);
		return atom->id;
	}

	/* Validate atom name */
	if (!zend_atom_name_is_valid(ZSTR_VAL(name), ZSTR_LEN(name))) {
		return ZEND_ATOM_INVALID_ID;
	}

	/* Create new atom */
	atom = emalloc(sizeof(zend_atom));
	atom->name = zend_string_copy(name);
	atom->id = next_atom_id++;

	ZVAL_PTR(&atom_zv, atom);
	zend_hash_add(&atom_table, name, &atom_zv);

	return atom->id;
}

ZEND_API uint32_t zend_atom_create_cstr(const char *name, size_t name_len)
{
	zend_string *str = zend_string_init(name, name_len, 0);
	uint32_t result = zend_atom_create(str);
	zend_string_release(str);
	return result;
}

ZEND_API uint32_t zend_atom_find(zend_string *name)
{
	if (!atoms_initialized) {
		return ZEND_ATOM_INVALID_ID;
	}
	
	zval *existing = zend_hash_find(&atom_table, name);
	if (existing) {
		zend_atom *atom = (zend_atom *)Z_PTR_P(existing);
		return atom->id;
	}
	return ZEND_ATOM_INVALID_ID;
}

ZEND_API uint32_t zend_atom_find_cstr(const char *name, size_t name_len)
{
	zend_string *str = zend_string_init(name, name_len, 0);
	uint32_t result = zend_atom_find(str);
	zend_string_release(str);
	return result;
}

ZEND_API zend_string *zend_atom_name(uint32_t atom_id)
{
	zval *entry;
	zend_atom *atom;

	if (atom_id == ZEND_ATOM_INVALID_ID) {
		return NULL;
	}

	/* Linear search through atom table to find by ID */
	/* TODO: Consider maintaining a reverse lookup table for performance */
	ZEND_HASH_FOREACH_VAL(&atom_table, entry) {
		atom = (zend_atom *)Z_PTR_P(entry);
		if (atom->id == atom_id) {
			return atom->name;
		}
	} ZEND_HASH_FOREACH_END();

	return NULL;
}

ZEND_API bool zend_atom_exists(zend_string *name)
{
	return zend_hash_exists(&atom_table, name);
}

ZEND_API void zend_startup_atoms(void)
{
	if (atoms_initialized) {
		return;
	}
	
	zend_hash_init(&atom_table, 0, NULL, atom_dtor, 1);
	atoms_initialized = true;
}