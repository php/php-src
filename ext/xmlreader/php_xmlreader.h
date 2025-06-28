/*
  +----------------------------------------------------------------------+
  | Copyright © The PHP Group and Contributors.                          |
  +----------------------------------------------------------------------+
  | This source file is subject to the Modified BSD License that is      |
  | bundled with this package in the file LICENSE, and is available      |
  | through the World Wide Web at <https://www.php.net/license/>.        |
  |                                                                      |
  | SPDX-License-Identifier: BSD-3-Clause                                |
  +----------------------------------------------------------------------+
  | Author: Rob Richards <rrichards@php.net>                             |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_XMLREADER_H
#define PHP_XMLREADER_H

extern zend_module_entry xmlreader_module_entry;
#define phpext_xmlreader_ptr &xmlreader_module_entry

#include "php_version.h"
#define PHP_XMLREADER_VERSION PHP_VERSION

#ifdef ZTS
#include "TSRM.h"
#endif

#include "ext/libxml/php_libxml.h"
#include <libxml/xmlreader.h>

/* If xmlreader and dom both are compiled statically,
   no DLL import should be used in xmlreader for dom symbols. */
#ifdef PHP_WIN32
# if defined(HAVE_DOM) && !defined(COMPILE_DL_DOM) && !defined(COMPILE_DL_XMLREADER)
#  define DOM_LOCAL_DEFINES 1
# endif
#endif

typedef struct _xmlreader_object {
	xmlTextReaderPtr ptr;
	/* strings must be set in input buffer as copy is required */
	xmlParserInputBufferPtr input;
	void *schema;
	zend_object  std;
} xmlreader_object;

static inline xmlreader_object *php_xmlreader_fetch_object(zend_object *obj) {
	return (xmlreader_object *)((char*)(obj) - XtOffsetOf(xmlreader_object, std));
}

#define Z_XMLREADER_P(zv) php_xmlreader_fetch_object(Z_OBJ_P((zv)))

PHP_MINIT_FUNCTION(xmlreader);
PHP_MSHUTDOWN_FUNCTION(xmlreader);
PHP_MINFO_FUNCTION(xmlreader);

#endif	/* PHP_XMLREADER_H */
