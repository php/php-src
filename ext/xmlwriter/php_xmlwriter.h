/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2006 The PHP Group                                |
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

/* Resource struct, not the object :) */
typedef struct _xmlwriter_object {
	xmlTextWriterPtr ptr;
	xmlBufferPtr output;
#ifndef ZEND_ENGINE_2
	xmlOutputBufferPtr uri_output;
#endif
} xmlwriter_object;


/* Extends zend object */
typedef struct _ze_xmlwriter_object {
	zend_object zo;
	xmlwriter_object *xmlwriter_ptr;
} ze_xmlwriter_object;

zend_class_entry *xmlwriter_class_entry_ce;

#if LIBXML_VERSION >= 20605
PHP_FUNCTION(xmlwriter_set_indent);
PHP_FUNCTION(xmlwriter_set_indent_string);
#endif
PHP_FUNCTION(xmlwriter_start_attribute);
PHP_FUNCTION(xmlwriter_end_attribute);
PHP_FUNCTION(xmlwriter_write_attribute);
#if LIBXML_VERSION > 20617
PHP_FUNCTION(xmlwriter_start_attribute_ns);
PHP_FUNCTION(xmlwriter_write_attribute_ns);
#endif
PHP_FUNCTION(xmlwriter_start_element);
PHP_FUNCTION(xmlwriter_end_element);
PHP_FUNCTION(xmlwriter_full_end_element);
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
PHP_FUNCTION(xmlwriter_write_raw);
PHP_FUNCTION(xmlwriter_start_document);
PHP_FUNCTION(xmlwriter_end_document);
#if LIBXML_VERSION >= 20607
PHP_FUNCTION(xmlwriter_start_comment);
PHP_FUNCTION(xmlwriter_end_comment);
#endif
PHP_FUNCTION(xmlwriter_write_comment);
PHP_FUNCTION(xmlwriter_start_dtd);
PHP_FUNCTION(xmlwriter_end_dtd);
PHP_FUNCTION(xmlwriter_write_dtd);
PHP_FUNCTION(xmlwriter_start_dtd_element);
PHP_FUNCTION(xmlwriter_end_dtd_element);
PHP_FUNCTION(xmlwriter_write_dtd_element);
#if LIBXML_VERSION > 20608
PHP_FUNCTION(xmlwriter_start_dtd_attlist);
PHP_FUNCTION(xmlwriter_end_dtd_attlist);
PHP_FUNCTION(xmlwriter_write_dtd_attlist);
#endif
PHP_FUNCTION(xmlwriter_open_uri);
PHP_FUNCTION(xmlwriter_open_memory);
PHP_FUNCTION(xmlwriter_output_memory);
PHP_FUNCTION(xmlwriter_flush);

#endif	/* PHP_XMLWRITER_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
