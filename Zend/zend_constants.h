/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | This source file is subject to the Zend license, that is bundled     |
   | with this package in the file LICENSE.  If you did not receive a     |
   | copy of the Zend license, please mail us at zend@zend.com so we can  |
   | send you a copy immediately.                                         |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

#ifndef _CONSTANTS_H
#define _CONSTANTS_H

#define CONST_CS 0x1				/* Case Sensitive */
#define CONST_PERSISTENT 0x2

typedef struct {
	zval value;
	int flags;
	char *name;
	uint name_len;
	int module_number;
} zend_constant;

#define REGISTER_LONG_CONSTANT(name,lval,flags)  zend_register_long_constant((name),sizeof(name),(lval),(flags),module_number)
#define REGISTER_DOUBLE_CONSTANT(name,dval,flags)  zend_register_double_constant((name),sizeof(name),(dval),(flags),module_number)
#define REGISTER_STRING_CONSTANT(name,str,flags)  zend_register_string_constant((name),sizeof(name),(str),(flags),module_number)
#define REGISTER_STRINGL_CONSTANT(name,str,len,flags)  zend_register_stringl_constant((name),sizeof(name),(str),(len),(flags),module_number)

#define REGISTER_MAIN_LONG_CONSTANT(name,lval,flags)  zend_register_long_constant((name),sizeof(name),(lval),(flags),0)
#define REGISTER_MAIN_DOUBLE_CONSTANT(name,dval,flags)  zend_register_double_constant((name),sizeof(name),(dval),(flags),0)
#define REGISTER_MAIN_STRING_CONSTANT(name,str,flags)  zend_register_string_constant((name),sizeof(name),(str),(flags),0)
#define REGISTER_MAIN_STRINGL_CONSTANT(name,str,len,flags)  zend_register_stringl_constant((name),sizeof(name),(str),(len),(flags),0)

void clean_module_constants(int module_number);
void free_zend_constant(zend_constant *c);
int zend_startup_constants(void);
int zend_shutdown_constants(void);
void clean_non_persistent_constants(void);
ZEND_API int zend_get_constant(char *name, uint name_len, zval *result);
ZEND_API void zend_register_long_constant(char *name, uint name_len, long lval, int flags, int module_number);
ZEND_API void zend_register_double_constant(char *name, uint name_len, double dval, int flags, int module_number);
ZEND_API void zend_register_string_constant(char *name, uint name_len, char *strval, int flags, int module_number);
ZEND_API void zend_register_stringl_constant(char *name, uint name_len, char *strval, uint strlen, int flags, int module_number);
ZEND_API void zend_register_constant(zend_constant *c);

#endif
