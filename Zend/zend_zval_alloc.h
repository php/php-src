/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | This source file is subject to version 0.91 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/0_91.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#ifndef _ZVAL_ALLOC_H
#define _ZVAL_ALLOC_H

#include "zend_globals.h"
#include "zend_globals_macros.h"
#include "zend_alloc.h"

typedef struct _zend_zval_list_entry {
	struct _zend_zval_list_entry *next;
} zend_zval_list_entry;


#ifndef ZTS
extern zend_alloc_globals alloc_globals;
#endif

#define ALLOC_ZVAL(z)									\
	{													\
		ALS_FETCH();									\
														\
		if (((z) = (void *) AG(zval_list_head))) {		\
			AG(zval_list_head) = ((zend_zval_list_entry *) AG(zval_list_head))->next;	\
		} else {										\
			(z) = emalloc(sizeof(zval));				\
		}												\
	}


#define FREE_ZVAL(z)												\
	{																\
		ALS_FETCH();												\
																	\
		((zend_zval_list_entry *) (z))->next = AG(zval_list_head);	\
		AG(zval_list_head) = (zend_zval_list_entry *) (z);			\
	}


#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
