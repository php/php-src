/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2004 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Rob Richards <rrichards@php.net>                             |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_XMLWRITER_H
#define PHP_XMLWRITER_H

extern zend_module_entry xmlwriter_module_entry;
#define phpext_xmlwriter_ptr &xmlwriter_module_entry

#ifdef PHP_WIN32
#define PHP_XMLWRITER_API __declspec(dllexport)
#else
#define PHP_XMLWRITER_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#include <libxml/tree.h>
#include <libxml/xmlwriter.h>
#include <libxml/uri.h>

typedef struct _xmlwriter_object {
	xmlTextWriterPtr ptr;
	xmlBufferPtr output;
} xmlwriter_object;

#if LIBXML_VERSION >= 20605
PHP_FUNCTION(xmlwriter_set_indent);
PHP_FUNCTION(xmlwriter_set_indent_string);
#endif
PHP_FUNCTION(xmlwriter_start_attribute);
PHP_FUNCTION(xmlwriter_end_attribute);
PHP_FUNCTION(xmlwriter_start_attribute_ns);
PHP_FUNCTION(xmlwriter_write_attribute);
PHP_FUNCTION(xmlwriter_write_attribute_ns);
PHP_FUNCTION(xmlwriter_start_element);
PHP_FUNCTION(xmlwriter_end_element);
PHP_FUNCTION(xmlwriter_start_element_ns);
PHP_FUNCTION(xmlwriter_write_element);
PHP_FUNCTION(xmlwriter_write_element_ns);
PHP_FUNCTION(xmlwriter_start_pi);
PHP_FUNCTION(xmlwriter_end_pi);
PHP_FUNCTION(xmlwriter_write_pi);
PHP_FUNCTION(xmlwriter_start_cdata);
PHP_FUNCTION(xmlwriter_end_cdata);
PHP_FUNCTION(xmlwriter_write_cdata);
PHP_FUNCTION(xmlwriter_text);
PHP_FUNCTION(xmlwriter_start_document);
PHP_FUNCTION(xmlwriter_end_document);
PHP_FUNCTION(xmlwriter_write_comment);
PHP_FUNCTION(xmlwriter_start_dtd);
PHP_FUNCTION(xmlwriter_end_dtd);
PHP_FUNCTION(xmlwriter_write_dtd);
PHP_FUNCTION(xmlwriter_start_dtd_element);
PHP_FUNCTION(xmlwriter_end_dtd_element);
PHP_FUNCTION(xmlwriter_open_uri);
PHP_FUNCTION(xmlwriter_open_memory);
PHP_FUNCTION(xmlwriter_output_memory);

#ifdef ZTS
#define XMLWRITER_G(v) TSRMG(xmlwriter_globals_id, zend_xmlwriter_globals *, v)
#else
#define XMLWRITER_G(v) (xmlwriter_globals.v)
#endif

#endif	/* PHP_XMLWRITER_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
