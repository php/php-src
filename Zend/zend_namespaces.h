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
   | Authors: Rob Landers <rob@bottled.codes>                             |
   |                                                                      |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_NAMESPACES_H
#define ZEND_NAMESPACES_H

#include "zend_compile.h"

ZEND_API zend_class_entry *zend_resolve_namespace(zend_string *name);
ZEND_API zend_class_entry *zend_lookup_namespace(zend_string *name);
ZEND_API void zend_destroy_namespaces(void);

#endif //ZEND_NAMESPACES_H
