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
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_MAP_PTR_H
#define ZEND_MAP_PTR_H

#include "zend_portability.h"

#define ZEND_MAP_PTR_KIND_PTR           0
#define ZEND_MAP_PTR_KIND_PTR_OR_OFFSET 1

#define ZEND_MAP_PTR_KIND ZEND_MAP_PTR_KIND_PTR_OR_OFFSET

#define ZEND_MAP_PTR(ptr) \
	ptr ## __ptr
#define ZEND_MAP_PTR_DEF(type, name) \
	type ZEND_MAP_PTR(name)
#define ZEND_MAP_PTR_OFFSET2PTR(offset) \
	((void**)((char*)CG(map_ptr_base) + offset))
#define ZEND_MAP_PTR_PTR2OFFSET(ptr) \
	((void*)(((char*)(ptr)) - ((char*)CG(map_ptr_base))))
#define ZEND_MAP_PTR_INIT(ptr, val) do { \
		ZEND_MAP_PTR(ptr) = (val); \
	} while (0)
#define ZEND_MAP_PTR_NEW(ptr) do { \
		ZEND_MAP_PTR(ptr) = zend_map_ptr_new(); \
	} while (0)

#if ZEND_MAP_PTR_KIND == ZEND_MAP_PTR_KIND_PTR_OR_OFFSET
# define ZEND_MAP_PTR_NEW_OFFSET() \
	((uint32_t)(uintptr_t)zend_map_ptr_new())
# define ZEND_MAP_PTR_IS_OFFSET(ptr) \
	(((uintptr_t)ZEND_MAP_PTR(ptr)) & 1L)
# define ZEND_MAP_PTR_GET(ptr) \
	((ZEND_MAP_PTR_IS_OFFSET(ptr) ? \
		ZEND_MAP_PTR_GET_IMM(ptr) : \
		((void*)(ZEND_MAP_PTR(ptr)))))
# define ZEND_MAP_PTR_GET_IMM(ptr) \
	(*ZEND_MAP_PTR_OFFSET2PTR((uintptr_t)ZEND_MAP_PTR(ptr)))
# define ZEND_MAP_PTR_SET(ptr, val) do { \
		if (ZEND_MAP_PTR_IS_OFFSET(ptr)) { \
			ZEND_MAP_PTR_SET_IMM(ptr, val); \
		} else { \
			ZEND_MAP_PTR_INIT(ptr, val); \
		} \
	} while (0)
# define ZEND_MAP_PTR_SET_IMM(ptr, val) do { \
		void **__p = ZEND_MAP_PTR_OFFSET2PTR((uintptr_t)ZEND_MAP_PTR(ptr)); \
		*__p = (val); \
	} while (0)
# define ZEND_MAP_PTR_BIASED_BASE(real_base) \
	((void*)(((uintptr_t)(real_base)) - 1))
#else
# error "Unknown ZEND_MAP_PTR_KIND"
#endif

BEGIN_EXTERN_C()

ZEND_API void  zend_map_ptr_reset(void);
ZEND_API void *zend_map_ptr_new(void);
ZEND_API void  zend_map_ptr_extend(size_t last);
ZEND_API void zend_alloc_ce_cache(zend_string *type_name);

END_EXTERN_C()

#endif /* ZEND_MAP_PTR_H */
