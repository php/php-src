/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
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

#ifdef ZTS
#include "TSRM.h"
#endif

#include "ext/libxml/php_libxml.h"
#include <libxml/xmlreader.h>

typedef struct _xmlreader_object {
	zend_object  std;
	xmlTextReaderPtr ptr;
	/* strings must be set in input buffer as copy is required */
	xmlParserInputBufferPtr input;
	void *schema;
	HashTable *prop_handler;
	zend_object_handle handle;
} xmlreader_object;

PHP_MINIT_FUNCTION(xmlreader);
PHP_MSHUTDOWN_FUNCTION(xmlreader);
PHP_MINFO_FUNCTION(xmlreader);

#define REGISTER_XMLREADER_CLASS_CONST_LONG(const_name, value) \
	zend_declare_class_constant_long(xmlreader_class_entry, const_name, sizeof(const_name)-1, (long)value TSRMLS_CC);

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
