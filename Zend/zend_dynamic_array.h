/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2003 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#ifndef ZEND_DYNAMIC_ARRAY_H
#define ZEND_DYNAMIC_ARRAY_H

typedef struct _dynamic_array {
	char *array;
	unsigned int element_size;
	unsigned int last_used;
	unsigned int allocated;
} dynamic_array;

BEGIN_EXTERN_C()
ZEND_API int zend_dynamic_array_init(dynamic_array *da);
ZEND_API void *zend_dynamic_array_push(dynamic_array *da);
ZEND_API void *zend_dynamic_array_pop(dynamic_array *da);
ZEND_API void *zend_dynamic_array_get_element(dynamic_array *da, unsigned int index);
END_EXTERN_C()

#endif /* ZEND_DYNAMIC_ARRAY_H */
