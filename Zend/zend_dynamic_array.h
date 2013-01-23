/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2013 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_DYNAMIC_ARRAY_H
#define ZEND_DYNAMIC_ARRAY_H

typedef struct _dynamic_array {
	char *array;
	unsigned int element_size;
	unsigned int last_used;
	unsigned int allocated;
} dynamic_array;

BEGIN_EXTERN_C()
ZEND_API int zend_dynamic_array_init(dynamic_array *da, unsigned int element_size, unsigned int size);
ZEND_API void *zend_dynamic_array_push(dynamic_array *da);
ZEND_API void *zend_dynamic_array_pop(dynamic_array *da);
ZEND_API void *zend_dynamic_array_get_element(dynamic_array *da, unsigned int index);
END_EXTERN_C()

#endif /* ZEND_DYNAMIC_ARRAY_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
