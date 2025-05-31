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

#ifndef ZEND_ATOM_H
#define ZEND_ATOM_H

#include "zend_types.h"
#include "zend_hash.h"

BEGIN_EXTERN_C()

typedef struct _zend_atom {
	zend_string *name;		/* atom name */
	uint32_t id;			/* unique atom ID */
} zend_atom;

/* Atom table globals */
ZEND_API extern HashTable atom_table;
ZEND_API extern uint32_t next_atom_id;

/* Core atom management functions */
ZEND_API void zend_atoms_init(void);
ZEND_API void zend_atoms_shutdown(void);
ZEND_API uint32_t zend_atom_create(zend_string *name);
ZEND_API uint32_t zend_atom_find(zend_string *name);
ZEND_API zend_string *zend_atom_name(uint32_t atom_id);
ZEND_API bool zend_atom_exists(zend_string *name);
ZEND_API void zend_startup_atoms(void);

/* Atom creation from C string (for internal use) */
ZEND_API uint32_t zend_atom_create_cstr(const char *name, size_t name_len);
ZEND_API uint32_t zend_atom_find_cstr(const char *name, size_t name_len);

/* Validation */
ZEND_API bool zend_atom_name_is_valid(const char *name, size_t name_len);

/* Utility macros */
#define ZEND_ATOM_INVALID_ID 0

END_EXTERN_C()

#endif /* ZEND_ATOM_H */