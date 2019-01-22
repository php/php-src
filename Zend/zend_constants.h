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
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_CONSTANTS_H
#define ZEND_CONSTANTS_H

#include "zend_globals.h"

#define CONST_CS				(1<<0)				/* Case Sensitive */
#define CONST_PERSISTENT		(1<<1)				/* Persistent */
#define CONST_CT_SUBST			(1<<2)				/* Allow compile-time substitution */
#define CONST_NO_FILE_CACHE		(1<<3)				/* Can't be saved in file cache */

#define	PHP_USER_CONSTANT   0x7fffff /* a constant defined in user space */

/* Flag for zend_get_constant_ex(). Must not class with ZEND_FETCH_CLASS_* flags. */
#define ZEND_GET_CONSTANT_NO_DEPRECATION_CHECK 0x1000

typedef struct _zend_constant {
	zval value;
	zend_string *name;
} zend_constant;

#define ZEND_CONSTANT_FLAGS(c) \
	(Z_CONSTANT_FLAGS((c)->value) & 0xff)

#define ZEND_CONSTANT_MODULE_NUMBER(c) \
	(Z_CONSTANT_FLAGS((c)->value) >> 8)

#define ZEND_CONSTANT_SET_FLAGS(c, _flags, _module_number) do { \
		Z_CONSTANT_FLAGS((c)->value) = \
			((_flags) & 0xff) | ((_module_number) << 8); \
	} while (0)

#define REGISTER_NULL_CONSTANT(name, flags)  zend_register_null_constant((name), sizeof(name)-1, (flags), module_number)
#define REGISTER_BOOL_CONSTANT(name, bval, flags)  zend_register_bool_constant((name), sizeof(name)-1, (bval), (flags), module_number)
#define REGISTER_LONG_CONSTANT(name, lval, flags)  zend_register_long_constant((name), sizeof(name)-1, (lval), (flags), module_number)
#define REGISTER_DOUBLE_CONSTANT(name, dval, flags)  zend_register_double_constant((name), sizeof(name)-1, (dval), (flags), module_number)
#define REGISTER_STRING_CONSTANT(name, str, flags)  zend_register_string_constant((name), sizeof(name)-1, (str), (flags), module_number)
#define REGISTER_STRINGL_CONSTANT(name, str, len, flags)  zend_register_stringl_constant((name), sizeof(name)-1, (str), (len), (flags), module_number)

#define REGISTER_NS_NULL_CONSTANT(ns, name, flags)  zend_register_null_constant(ZEND_NS_NAME(ns, name), sizeof(ZEND_NS_NAME(ns, name))-1, (flags), module_number)
#define REGISTER_NS_BOOL_CONSTANT(ns, name, bval, flags)  zend_register_bool_constant(ZEND_NS_NAME(ns, name), sizeof(ZEND_NS_NAME(ns, name))-1, (bval), (flags), module_number)
#define REGISTER_NS_LONG_CONSTANT(ns, name, lval, flags)  zend_register_long_constant(ZEND_NS_NAME(ns, name), sizeof(ZEND_NS_NAME(ns, name))-1, (lval), (flags), module_number)
#define REGISTER_NS_DOUBLE_CONSTANT(ns, name, dval, flags)  zend_register_double_constant(ZEND_NS_NAME(ns, name), sizeof(ZEND_NS_NAME(ns, name))-1, (dval), (flags), module_number)
#define REGISTER_NS_STRING_CONSTANT(ns, name, str, flags)  zend_register_string_constant(ZEND_NS_NAME(ns, name), sizeof(ZEND_NS_NAME(ns, name))-1, (str), (flags), module_number)
#define REGISTER_NS_STRINGL_CONSTANT(ns, name, str, len, flags)  zend_register_stringl_constant(ZEND_NS_NAME(ns, name), sizeof(ZEND_NS_NAME(ns, name))-1, (str), (len), (flags), module_number)

#define REGISTER_MAIN_NULL_CONSTANT(name, flags)  zend_register_null_constant((name), sizeof(name)-1, (flags), 0)
#define REGISTER_MAIN_BOOL_CONSTANT(name, bval, flags)  zend_register_bool_constant((name), sizeof(name)-1, (bval), (flags), 0)
#define REGISTER_MAIN_LONG_CONSTANT(name, lval, flags)  zend_register_long_constant((name), sizeof(name)-1, (lval), (flags), 0)
#define REGISTER_MAIN_DOUBLE_CONSTANT(name, dval, flags)  zend_register_double_constant((name), sizeof(name)-1, (dval), (flags), 0)
#define REGISTER_MAIN_STRING_CONSTANT(name, str, flags)  zend_register_string_constant((name), sizeof(name)-1, (str), (flags), 0)
#define REGISTER_MAIN_STRINGL_CONSTANT(name, str, len, flags)  zend_register_stringl_constant((name), sizeof(name)-1, (str), (len), (flags), 0)

BEGIN_EXTERN_C()
void clean_module_constants(int module_number);
void free_zend_constant(zval *zv);
int zend_startup_constants(void);
int zend_shutdown_constants(void);
void zend_register_standard_constants(void);
ZEND_API int zend_verify_const_access(zend_class_constant *c, zend_class_entry *ce);
ZEND_API zval *zend_get_constant(zend_string *name);
ZEND_API zval *zend_get_constant_str(const char *name, size_t name_len);
ZEND_API zval *zend_get_constant_ex(zend_string *name, zend_class_entry *scope, uint32_t flags);
ZEND_API void zend_register_bool_constant(const char *name, size_t name_len, zend_bool bval, int flags, int module_number);
ZEND_API void zend_register_null_constant(const char *name, size_t name_len, int flags, int module_number);
ZEND_API void zend_register_long_constant(const char *name, size_t name_len, zend_long lval, int flags, int module_number);
ZEND_API void zend_register_double_constant(const char *name, size_t name_len, double dval, int flags, int module_number);
ZEND_API void zend_register_string_constant(const char *name, size_t name_len, char *strval, int flags, int module_number);
ZEND_API void zend_register_stringl_constant(const char *name, size_t name_len, char *strval, size_t strlen, int flags, int module_number);
ZEND_API int zend_register_constant(zend_constant *c);
#ifdef ZTS
void zend_copy_constants(HashTable *target, HashTable *sourc);
#endif
END_EXTERN_C()

#define ZEND_CONSTANT_DTOR free_zend_constant

#endif
