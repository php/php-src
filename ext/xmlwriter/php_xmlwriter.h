/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Rob Richards <rrichards@php.net>                             |
  |         Pierre-A. Joye <pajoye@php.net>                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_XMLWRITER_H
#define PHP_XMLWRITER_H

extern zend_module_entry xmlwriter_module_entry;
#define phpext_xmlwriter_ptr &xmlwriter_module_entry

#ifdef ZTS
#include "TSRM.h"
#endif

#include <libxml/tree.h>
#include <libxml/xmlwriter.h>
#include <libxml/uri.h>

/* Resource struct, not the object :) */
typedef struct _xmlwriter_object {
	xmlTextWriterPtr ptr;
	xmlBufferPtr output;
} xmlwriter_object;


/* Extends zend object */
typedef struct _ze_xmlwriter_object {
	xmlwriter_object *xmlwriter_ptr;
	zend_object std;
} ze_xmlwriter_object;

static inline ze_xmlwriter_object *php_xmlwriter_fetch_object(zend_object *obj) {
	return (ze_xmlwriter_object *)((char*)(obj) - XtOffsetOf(ze_xmlwriter_object, std));
}

#define Z_XMLWRITER_P(zv) php_xmlwriter_fetch_object(Z_OBJ_P((zv)))

#endif	/* PHP_XMLWRITER_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
