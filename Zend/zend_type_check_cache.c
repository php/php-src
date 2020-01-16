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
   | Authors: Dik Takken <d.h.j.takken@freedom.nl>                        |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_globals_macros.h"
#include "zend_compile.h"

/**
 * Configures the type check cache to use no more than the
 * specified amount of memory.
 */
ZEND_API int zend_set_tcc_memory_limit(zend_long memory_limit)
{
	EG(tcc_memory_limit) = memory_limit;
	return SUCCESS;
}

/**
 * Configures the length of the rows in the type check cache,
 * determining how many class entries it can store.
 *
 * Note: Must not be changed at run time.
 */
ZEND_API int zend_set_tcc_class_slots(zend_long num_classes)
{
	EG(tcc_num_class_slots) = num_classes;
	return SUCCESS;
}

/**
 * Clears the type check cache
 */
ZEND_API void tcc_clear()
{
	for (int i=0; i<zend_array_count(CG(type_names_table)); i++) {
		memset(CG(type_check_cache)[i], 0, EG(tcc_num_class_slots) * sizeof(uint8_t));
	}
}

/**
 * Returns a pointer to the row in the type check cache
 * that corresponds with specified type. Each row is an
 * array of bytes containing either one (match) or zero
 * (unknown) for each of the class entries represented
 * by the columns.
 */
ZEND_API uint8_t *tcc_get_row(zend_type *type)
{
	zval type_name_z;
	zend_string *type_name;
	zend_string *type_name_copy;

	type_name = zend_type_to_string(*type);

	ZVAL_STR(&type_name_z, type_name);

	// Look up the type name in CG(type_names_table)

	int exists = 0;
	zval *entry;
	zend_ulong index = 0;
	ZEND_HASH_FOREACH_NUM_KEY_VAL(CG(type_names_table), index, entry) {
		if (fast_is_identical_function(&type_name_z, entry)) {
			// We found the type name.
			exists = 1;
			break;
		}
	} ZEND_HASH_FOREACH_END();

	if (!exists) {
		// Type name not found. We should add it to the hash table.

		if ((zend_array_count(CG(type_names_table)) + 1) * EG(tcc_num_class_slots) * sizeof(uint8_t) > EG(tcc_memory_limit)) {
			// Adding the type name would exceed memory limits.
			// We return the dummy cache row which yields a cache
			// miss for any CE.
			if (!CG(tcc_dummy_row)) {
				CG(tcc_dummy_row) = calloc(EG(tcc_num_class_slots), sizeof(uint8_t));
			}
			zend_string_release(type_name);
			return CG(tcc_dummy_row);
		}

		if (CG(type_names_table)->nNumOfElements > 0) {
			index++;
		}

		type_name_copy = zend_string_dup(type_name, 0);
		ZVAL_STR(&type_name_z, type_name_copy);
		zend_hash_index_add(CG(type_names_table), index, &type_name_z);
		zend_string_release(type_name_copy);

		// Now that we have a new type name, we must also add
		// a new row for it in the cache matrix.
		CG(type_check_cache) = realloc(CG(type_check_cache), zend_array_count(CG(type_names_table)) * sizeof(uint8_t *));
		CG(type_check_cache)[index] = calloc(EG(tcc_num_class_slots), sizeof(uint8_t));
	}

	zend_string_release(type_name);

	return CG(type_check_cache)[index];
}

/**
 * Assigns column indices to class entries. Classes that have
 * been assigned an index before are skipped.
 */
ZEND_API void tcc_assign_ce_columns()
{
	// Below we assign type check cache column indices to
	// the classes in the class table. Note that column index
	// zero is special: It is assigned to classes which are
	// not covered by the cache. This column contains zeros
	// indicating a cache miss. It is special because it is
	// not written in case of a cache miss and because it may
	// be shared by multiple classes.
	// Note that we traverse the class table in reverse order
	// for efficiency.
	zend_class_entry *ce;
	ZEND_HASH_REVERSE_FOREACH_PTR(CG(class_table), ce) {
		if (ce->tcc_column_index > 0) {
			// We arrived at the classes that were
			// assigned an index previously, we are done.
			break;
		}

		// Below we skip interfaces, traits and abstract classes.
		// We do not need to assign them a cache column because no
		// object can ever be an instance of any of them.
		if (ce->ce_flags & ZEND_ACC_INTERFACE) {
			ce->tcc_column_index = 0;
			continue;
		}
		if (ce->ce_flags & ZEND_ACC_TRAIT) {
			ce->tcc_column_index = 0;
			continue;
		}
		if (ce->ce_flags & ZEND_ACC_ABSTRACT) {
			ce->tcc_column_index = 0;
			continue;
		}
		if (ce->ce_flags & ZEND_ACC_EXPLICIT_ABSTRACT_CLASS) {
			// TODO: This case is never hit in the tests. Remove?
			ce->tcc_column_index = 0;
			continue;
		}

		CG(last_assigned_tcc_column)++;
		ce->tcc_column_index = CG(last_assigned_tcc_column);

		// The cache has a fixed number of columns. In case
		// there are more classes than there are columns we
		// simply exclude them. This means that type
		// checks involving objects of these classes will
		// always result in a cache miss.
		if (CG(last_assigned_tcc_column) > EG(tcc_num_class_slots)) {
			CG(last_assigned_tcc_column) = EG(tcc_num_class_slots);
			ce->tcc_column_index = 0;
		}
	} ZEND_HASH_FOREACH_END();

	tcc_clear();
}
