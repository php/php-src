/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2015 Zend Technologies Ltd. (http://www.zend.com) |
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

#ifndef ZEND_INHERITANCE_H
#define ZEND_INHERITANCE_H

#include "zend.h"

BEGIN_EXTERN_C()

ZEND_API void zend_do_inherit_interfaces(zend_class_entry *ce, const zend_class_entry *iface);
ZEND_API void zend_do_implement_interface(zend_class_entry *ce, zend_class_entry *iface);

ZEND_API void zend_do_implement_trait(zend_class_entry *ce, zend_class_entry *trait);
ZEND_API void zend_do_bind_traits(zend_class_entry *ce);

ZEND_API void zend_do_inheritance(zend_class_entry *ce, zend_class_entry *parent_ce);
void zend_do_early_binding(void);

END_EXTERN_C()

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
