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
   | Authors: Daniel Scherzer <daniel.e.scherzer@gmail.com>               |
   +----------------------------------------------------------------------+
*/

#include "zend_class_alias.h"

zend_class_alias * zend_class_alias_init(zend_class_entry *ce) {
	zend_class_alias *alias = malloc(sizeof(zend_class_alias));
	// refcount field is only there for compatibility with other structures
	GC_SET_REFCOUNT(alias, 1);

	alias->ce = ce;
	alias->attributes = NULL;
	alias->alias_flags = 0;

	return alias;
}
