/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Sterling Hughes <sterling@php.net>                           |
  |         Marcus Boerger <helly@php.net>                               |
  |         Rob Richards <rrichards@php.net>                             |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_SIMPLEXML_EXPORTS_H
#define PHP_SIMPLEXML_EXPORTS_H

#include "php_simplexml.h"

#define SKIP_TEXT(__p) \
	if ((__p)->type == XML_TEXT_NODE) { \
		goto next_iter; \
	}

#define GET_NODE(__s, __n) { \
	if ((__s)->node && (__s)->node->node) { \
		__n = (__s)->node->node; \
	} else { \
		__n = NULL; \
		php_error_docref(NULL, E_WARNING, "Node no longer exists"); \
	} \
}

PHP_SXE_API zend_object *sxe_object_new(zend_class_entry *ce);

static inline php_sxe_object *php_sxe_fetch_object(zend_object *obj) /* {{{ */ {
	return (php_sxe_object *)((char*)(obj) - XtOffsetOf(php_sxe_object, zo));
}
/* }}} */

#define Z_SXEOBJ_P(zv) php_sxe_fetch_object(Z_OBJ_P((zv)))

typedef struct {
	zend_object_iterator  intern;
	php_sxe_object        *sxe;
} php_sxe_iterator;

PHP_SXE_API void php_sxe_rewind_iterator(php_sxe_object *sxe);
PHP_SXE_API void php_sxe_move_forward_iterator(php_sxe_object *sxe);

#endif /* PHP_SIMPLEXML_EXPORTS_H */
