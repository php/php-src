/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2018 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: krakjoe@php.net                                             |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_WEAKREFS_H
#define ZEND_WEAKREFS_H

BEGIN_EXTERN_C()

extern ZEND_API zend_class_entry *zend_ce_weakref;
extern ZEND_API void zend_weakref_construct(zval *weakref, zval *referent);

extern void zend_register_weakref_ce(void);

void zend_weakrefs_init();
void zend_weakrefs_notify(zend_object *object);
void zend_weakrefs_shutdown();

END_EXTERN_C()

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
