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

/* $Id$ */

#ifndef ZEND_EXECUTE_LOCKS_H
#define ZEND_EXECUTE_LOCKS_H

#define PZVAL_LOCK(z) zend_pzval_lock_func(z)

static inline void zend_pzval_lock_func(zval *z)
{
	z->refcount++;
}

#define PZVAL_UNLOCK(z) zend_pzval_unlock_func(z TSRMLS_CC)

static inline void zend_pzval_unlock_func(zval *z TSRMLS_DC)
{
	z->refcount--;
	if (!z->refcount) {
		z->refcount = 1;
		z->is_ref = 0;
		EG(garbage)[EG(garbage_ptr)++] = z;
	}
}

static inline void zend_clean_garbage(TSRMLS_D)
{
	while (EG(garbage_ptr)) {
		zval_ptr_dtor(&EG(garbage)[--EG(garbage_ptr)]);
	}
}

#define SELECTIVE_PZVAL_LOCK(pzv, pzn)		if (!((pzn)->u.EA.type & EXT_TYPE_UNUSED)) { PZVAL_LOCK(pzv); }

#endif /* ZEND_EXECUTE_LOCKS_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
