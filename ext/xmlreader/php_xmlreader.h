/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Rob Richards <rrichards@php.net>                             |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

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
	HashTable *prop_handler;
	zend_object  std;
} xmlreader_object;

static inline xmlreader_object *php_xmlreader_fetch_object(zend_object *obj) {
	return (xmlreader_object *)((char*)(obj) - XtOffsetOf(xmlreader_object, std));
}

#define Z_XMLREADER_P(zv) php_xmlreader_fetch_object(Z_OBJ_P((zv)))

PHP_MINIT_FUNCTION(xmlreader);
PHP_MSHUTDOWN_FUNCTION(xmlreader);
PHP_MINFO_FUNCTION(xmlreader);

#define REGISTER_XMLREADER_CLASS_CONST_LONG(const_name, value) \
	zend_declare_class_constant_long(xmlreader_class_entry, const_name, sizeof(const_name)-1, (zend_long)value);

#ifdef ZTS
#define XMLREADER_G(v) TSRMG(xmlreader_globals_id, zend_xmlreader_globals *, v)
#else
#define XMLREADER_G(v) (xmlreader_globals.v)
#endif

#endif	/* PHP_XMLREADER_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
