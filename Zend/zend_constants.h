/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2000 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 0.92 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/0_92.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#ifndef ZEND_CONSTANTS_H
#define ZEND_CONSTANTS_H

#include "zend_globals.h"

#define CONST_CS 0x1				/* Case Sensitive */
#define CONST_PERSISTENT 0x2

typedef struct _zend_constant {
	zval value;
	int flags;
	char *name;
	uint name_len;
	int module_number;
} zend_constant;

#define REGISTER_LONG_CONSTANT(name,lval,flags)  zend_register_long_constant((name),sizeof(name),(lval),(flags),module_number ELS_CC)
#define REGISTER_DOUBLE_CONSTANT(name,dval,flags)  zend_register_double_constant((name),sizeof(name),(dval),(flags),module_number ELS_CC)
#define REGISTER_STRING_CONSTANT(name,str,flags)  zend_register_string_constant((name),sizeof(name),(str),(flags),module_number ELS_CC)
#define REGISTER_STRINGL_CONSTANT(name,str,len,flags)  zend_register_stringl_constant((name),sizeof(name),(str),(len),(flags),module_number ELS_CC)

#define REGISTER_MAIN_LONG_CONSTANT(name,lval,flags)  zend_register_long_constant((name),sizeof(name),(lval),(flags),0 ELS_CC)
#define REGISTER_MAIN_DOUBLE_CONSTANT(name,dval,flags)  zend_register_double_constant((name),sizeof(name),(dval),(flags),0 ELS_CC)
#define REGISTER_MAIN_STRING_CONSTANT(name,str,flags)  zend_register_string_constant((name),sizeof(name),(str),(flags),0 ELS_CC)
#define REGISTER_MAIN_STRINGL_CONSTANT(name,str,len,flags)  zend_register_stringl_constant((name),sizeof(name),(str),(len),(flags),0 ELS_CC)

void clean_module_constants(int module_number);
void free_zend_constant(zend_constant *c);
int zend_startup_constants(ELS_D);
int zend_shutdown_constants(ELS_D);
void zend_register_standard_constants(ELS_D);
void clean_non_persistent_constants(void);
ZEND_API int zend_get_constant(char *name, uint name_len, zval *result);
ZEND_API void zend_register_long_constant(char *name, uint name_len, long lval, int flags, int module_number ELS_DC);
ZEND_API void zend_register_double_constant(char *name, uint name_len, double dval, int flags, int module_number ELS_DC);
ZEND_API void zend_register_string_constant(char *name, uint name_len, char *strval, int flags, int module_number ELS_DC);
ZEND_API void zend_register_stringl_constant(char *name, uint name_len, char *strval, uint strlen, int flags, int module_number ELS_DC);
ZEND_API int zend_register_constant(zend_constant *c ELS_DC);
void zend_copy_constants(HashTable *target, HashTable *sourc);
void copy_zend_constant(zend_constant *c);

#define ZEND_CONSTANT_DTOR (void (*)(void *)) free_zend_constant

#endif
