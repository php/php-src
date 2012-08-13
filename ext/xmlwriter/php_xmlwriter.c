/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_xmlwriter.h"
#include "ext/standard/php_string.h"

#if LIBXML_VERSION >= 20605
static PHP_FUNCTION(xmlwriter_set_indent);
static PHP_FUNCTION(xmlwriter_set_indent_string);
#endif
static PHP_FUNCTION(xmlwriter_start_attribute);
static PHP_FUNCTION(xmlwriter_end_attribute);
static PHP_FUNCTION(xmlwriter_write_attribute);
#if LIBXML_VERSION > 20617
static PHP_FUNCTION(xmlwriter_start_attribute_ns);
static PHP_FUNCTION(xmlwriter_write_attribute_ns);
#endif
static PHP_FUNCTION(xmlwriter_start_element);
static PHP_FUNCTION(xmlwriter_end_element);
static PHP_FUNCTION(xmlwriter_full_end_element);
static PHP_FUNCTION(xmlwriter_start_element_ns);
static PHP_FUNCTION(xmlwriter_write_element);
static PHP_FUNCTION(xmlwriter_write_element_ns);
static PHP_FUNCTION(xmlwriter_start_pi);
static PHP_FUNCTION(xmlwriter_end_pi);
static PHP_FUNCTION(xmlwriter_write_pi);
static PHP_FUNCTION(xmlwriter_start_cdata);
static PHP_FUNCTION(xmlwriter_end_cdata);
static PHP_FUNCTION(xmlwriter_write_cdata);
static PHP_FUNCTION(xmlwriter_text);
static PHP_FUNCTION(xmlwriter_write_raw);
static PHP_FUNCTION(xmlwriter_start_document);
static PHP_FUNCTION(xmlwriter_end_document);
#if LIBXML_VERSION >= 20607
static PHP_FUNCTION(xmlwriter_start_comment);
static PHP_FUNCTION(xmlwriter_end_comment);
#endif
static PHP_FUNCTION(xmlwriter_write_comment);
static PHP_FUNCTION(xmlwriter_start_dtd);
static PHP_FUNCTION(xmlwriter_end_dtd);
static PHP_FUNCTION(xmlwriter_write_dtd);
static PHP_FUNCTION(xmlwriter_start_dtd_element);
static PHP_FUNCTION(xmlwriter_end_dtd_element);
static PHP_FUNCTION(xmlwriter_write_dtd_element);
#if LIBXML_VERSION > 20608
static PHP_FUNCTION(xmlwriter_start_dtd_attlist);
static PHP_FUNCTION(xmlwriter_end_dtd_attlist);
static PHP_FUNCTION(xmlwriter_write_dtd_attlist);
static PHP_FUNCTION(xmlwriter_start_dtd_entity);
static PHP_FUNCTION(xmlwriter_end_dtd_entity);
static PHP_FUNCTION(xmlwriter_write_dtd_entity);
#endif
static PHP_FUNCTION(xmlwriter_open_uri);
static PHP_FUNCTION(xmlwriter_open_memory);
static PHP_FUNCTION(xmlwriter_output_memory);
static PHP_FUNCTION(xmlwriter_flush);

static zend_class_entry *xmlwriter_class_entry_ce;

static void xmlwriter_free_resource_ptr(xmlwriter_object *intern TSRMLS_DC);
static void xmlwriter_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC);

typedef int (*xmlwriter_read_one_char_t)(xmlTextWriterPtr writer, const xmlChar *content);
typedef int (*xmlwriter_read_int_t)(xmlTextWriterPtr writer);

/* {{{ xmlwriter_object_free_storage */
static void xmlwriter_free_resource_ptr(xmlwriter_object *intern TSRMLS_DC) 
{
	if (intern) {
		if (intern->ptr) {
			xmlFreeTextWriter(intern->ptr);
			intern->ptr = NULL;
		}
		if (intern->output) {
			xmlBufferFree(intern->output);
			intern->output = NULL;
		}
		efree(intern);
	}
}
/* }}} */

#ifdef ZEND_ENGINE_2
/* {{{ XMLWRITER_FROM_OBJECT */
#define XMLWRITER_FROM_OBJECT(intern, object) \
	{ \
		ze_xmlwriter_object *obj = (ze_xmlwriter_object*) zend_object_store_get_object(object TSRMLS_CC); \
		intern = obj->xmlwriter_ptr; \
		if (!intern) { \
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid or unitialized XMLWriter object"); \
			RETURN_FALSE; \
		} \
	}
/* }}} */

static zend_object_handlers xmlwriter_object_handlers;

/* {{{ xmlwriter_object_free_storage */
static void xmlwriter_object_free_storage(void *object TSRMLS_DC)
{
	ze_xmlwriter_object * intern = (ze_xmlwriter_object *) object;
	if (!intern) {
		return;
	}
	if (intern->xmlwriter_ptr) {
		xmlwriter_free_resource_ptr(intern->xmlwriter_ptr TSRMLS_CC);
	}
	intern->xmlwriter_ptr = NULL;
	zend_object_std_dtor(&intern->zo TSRMLS_CC);
	
	efree(intern);
}
/* }}} */


/* {{{ xmlwriter_object_new */
static zend_object_value xmlwriter_object_new(zend_class_entry *class_type TSRMLS_DC)
{
	ze_xmlwriter_object *intern;
	zend_object_value retval;

	intern = emalloc(sizeof(ze_xmlwriter_object));
	memset(&intern->zo, 0, sizeof(zend_object));
	intern->xmlwriter_ptr = NULL;
	
	zend_object_std_init(&intern->zo, class_type TSRMLS_CC);
	object_properties_init(&intern->zo, class_type);

	retval.handle = zend_objects_store_put(intern,
						NULL,
						(zend_objects_free_object_storage_t) xmlwriter_object_free_storage,
						NULL TSRMLS_CC);
	
	retval.handlers = (zend_object_handlers *) & xmlwriter_object_handlers;
	
	return retval;
}
/* }}} */
#endif

#define XMLW_NAME_CHK(__err) \
	if (xmlValidateName((xmlChar *) name, 0) != 0) {	\
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", __err);	\
		RETURN_FALSE;	\
	}	\
	
/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO(arginfo_xmlwriter_void, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_resource, 0, 0, 1)
	ZEND_ARG_INFO(0, xmlwriter)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_open_uri, 0, 0, 1)
	ZEND_ARG_INFO(0, uri)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_set_indent, 0, 0, 2)
	ZEND_ARG_INFO(0, xmlwriter)
	ZEND_ARG_INFO(0, indent)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_method_set_indent, 0, 0, 1)
	ZEND_ARG_INFO(0, indent)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_set_indent_string, 0, 0, 2)
	ZEND_ARG_INFO(0, xmlwriter)
	ZEND_ARG_INFO(0, indentString)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_method_set_indent_string, 0, 0, 1)
	ZEND_ARG_INFO(0, indentString)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_start_attribute, 0, 0, 2)
	ZEND_ARG_INFO(0, xmlwriter)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_method_start_attribute, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_start_attribute_ns, 0, 0, 4)
	ZEND_ARG_INFO(0, xmlwriter)
	ZEND_ARG_INFO(0, prefix)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, uri)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_method_start_attribute_ns, 0, 0, 3)
	ZEND_ARG_INFO(0, prefix)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, uri)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_write_attribute_ns, 0, 0, 5)
	ZEND_ARG_INFO(0, xmlwriter)
	ZEND_ARG_INFO(0, prefix)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, uri)
	ZEND_ARG_INFO(0, content)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_method_write_attribute_ns, 0, 0, 4)
	ZEND_ARG_INFO(0, prefix)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, uri)
	ZEND_ARG_INFO(0, content)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_write_attribute, 0, 0, 3)
	ZEND_ARG_INFO(0, xmlwriter)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_method_write_attribute, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_start_element, 0, 0, 2)
	ZEND_ARG_INFO(0, xmlwriter)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_method_start_element, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_start_element_ns, 0, 0, 4)
	ZEND_ARG_INFO(0, xmlwriter)
	ZEND_ARG_INFO(0, prefix)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, uri)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_method_start_element_ns, 0, 0, 3)
	ZEND_ARG_INFO(0, prefix)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, uri)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_write_element, 0, 0, 2)
	ZEND_ARG_INFO(0, xmlwriter)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, content)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_method_write_element, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, content)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_write_element_ns, 0, 0, 4)
	ZEND_ARG_INFO(0, xmlwriter)
	ZEND_ARG_INFO(0, prefix)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, uri)
	ZEND_ARG_INFO(0, content)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_method_write_element_ns, 0, 0, 3)
	ZEND_ARG_INFO(0, prefix)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, uri)
	ZEND_ARG_INFO(0, content)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_start_pi, 0, 0, 2)
	ZEND_ARG_INFO(0, xmlwriter)
	ZEND_ARG_INFO(0, target)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_method_start_pi, 0, 0, 1)
	ZEND_ARG_INFO(0, target)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_write_pi, 0, 0, 3)
	ZEND_ARG_INFO(0, xmlwriter)
	ZEND_ARG_INFO(0, target)
	ZEND_ARG_INFO(0, content)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_method_write_pi, 0, 0, 2)
	ZEND_ARG_INFO(0, target)
	ZEND_ARG_INFO(0, content)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_write_cdata, 0, 0, 2)
	ZEND_ARG_INFO(0, xmlwriter)
	ZEND_ARG_INFO(0, content)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_method_write_cdata, 0, 0, 1)
	ZEND_ARG_INFO(0, content)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_text, 0, 0, 2)
	ZEND_ARG_INFO(0, xmlwriter)
	ZEND_ARG_INFO(0, content)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_method_text, 0, 0, 1)
	ZEND_ARG_INFO(0, content)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_write_raw, 0, 0, 2)
	ZEND_ARG_INFO(0, xmlwriter)
	ZEND_ARG_INFO(0, content)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_method_write_raw, 0, 0, 1)
	ZEND_ARG_INFO(0, content)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_start_document, 0, 0, 1)
	ZEND_ARG_INFO(0, xmlwriter)
	ZEND_ARG_INFO(0, version)
	ZEND_ARG_INFO(0, encoding)
	ZEND_ARG_INFO(0, standalone)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_method_start_document, 0, 0, 0)
	ZEND_ARG_INFO(0, version)
	ZEND_ARG_INFO(0, encoding)
	ZEND_ARG_INFO(0, standalone)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_write_comment, 0, 0, 2)
	ZEND_ARG_INFO(0, xmlwriter)
	ZEND_ARG_INFO(0, content)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_method_write_comment, 0, 0, 1)
	ZEND_ARG_INFO(0, content)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_start_dtd, 0, 0, 2)
	ZEND_ARG_INFO(0, xmlwriter)
	ZEND_ARG_INFO(0, qualifiedName)
	ZEND_ARG_INFO(0, publicId)
	ZEND_ARG_INFO(0, systemId)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_method_start_dtd, 0, 0, 1)
	ZEND_ARG_INFO(0, qualifiedName)
	ZEND_ARG_INFO(0, publicId)
	ZEND_ARG_INFO(0, systemId)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_write_dtd, 0, 0, 2)
	ZEND_ARG_INFO(0, xmlwriter)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, publicId)
	ZEND_ARG_INFO(0, systemId)
	ZEND_ARG_INFO(0, subset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_method_write_dtd, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, publicId)
	ZEND_ARG_INFO(0, systemId)
	ZEND_ARG_INFO(0, subset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_start_dtd_element, 0, 0, 2)
	ZEND_ARG_INFO(0, xmlwriter)
	ZEND_ARG_INFO(0, qualifiedName)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_method_start_dtd_element, 0, 0, 1)
	ZEND_ARG_INFO(0, qualifiedName)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_write_dtd_element, 0, 0, 3)
	ZEND_ARG_INFO(0, xmlwriter)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, content)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_method_write_dtd_element, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, content)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_start_dtd_attlist, 0, 0, 2)
	ZEND_ARG_INFO(0, xmlwriter)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_method_start_dtd_attlist, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_write_dtd_attlist, 0, 0, 3)
	ZEND_ARG_INFO(0, xmlwriter)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, content)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_method_write_dtd_attlist, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, content)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_start_dtd_entity, 0, 0, 3)
	ZEND_ARG_INFO(0, xmlwriter)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, isparam)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_method_start_dtd_entity, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, isparam)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_write_dtd_entity, 0, 0, 3)
	ZEND_ARG_INFO(0, xmlwriter)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, content)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_method_write_dtd_entity, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, content)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_output_memory, 0, 0, 1)
	ZEND_ARG_INFO(0, xmlwriter)
	ZEND_ARG_INFO(0, flush)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_method_output_memory, 0, 0, 0)
	ZEND_ARG_INFO(0, flush)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_flush, 0, 0, 1)
	ZEND_ARG_INFO(0, xmlwriter)
	ZEND_ARG_INFO(0, empty)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_method_flush, 0, 0, 0)
	ZEND_ARG_INFO(0, empty)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ xmlwriter_functions */
static const zend_function_entry xmlwriter_functions[] = {
	PHP_FE(xmlwriter_open_uri,			arginfo_xmlwriter_open_uri)
	PHP_FE(xmlwriter_open_memory,		arginfo_xmlwriter_void)
#if LIBXML_VERSION >= 20605
	PHP_FE(xmlwriter_set_indent,		arginfo_xmlwriter_set_indent)
	PHP_FE(xmlwriter_set_indent_string, arginfo_xmlwriter_set_indent_string)
#endif
#if LIBXML_VERSION >= 20607
	PHP_FE(xmlwriter_start_comment,		arginfo_xmlwriter_resource)
	PHP_FE(xmlwriter_end_comment,		arginfo_xmlwriter_resource)
#endif
	PHP_FE(xmlwriter_start_attribute,	arginfo_xmlwriter_start_attribute)
	PHP_FE(xmlwriter_end_attribute,		arginfo_xmlwriter_resource)
	PHP_FE(xmlwriter_write_attribute,	arginfo_xmlwriter_write_attribute)
#if LIBXML_VERSION > 20617
	PHP_FE(xmlwriter_start_attribute_ns,arginfo_xmlwriter_start_attribute_ns)
	PHP_FE(xmlwriter_write_attribute_ns,arginfo_xmlwriter_write_attribute_ns)
#endif
	PHP_FE(xmlwriter_start_element,		arginfo_xmlwriter_start_element)
	PHP_FE(xmlwriter_end_element,		arginfo_xmlwriter_resource)
	PHP_FE(xmlwriter_full_end_element,	arginfo_xmlwriter_resource)
	PHP_FE(xmlwriter_start_element_ns,	arginfo_xmlwriter_start_element_ns)
	PHP_FE(xmlwriter_write_element,		arginfo_xmlwriter_write_element)
	PHP_FE(xmlwriter_write_element_ns,	arginfo_xmlwriter_write_element_ns)
	PHP_FE(xmlwriter_start_pi,			arginfo_xmlwriter_start_pi)
	PHP_FE(xmlwriter_end_pi,			arginfo_xmlwriter_resource)
	PHP_FE(xmlwriter_write_pi,			arginfo_xmlwriter_write_pi)
	PHP_FE(xmlwriter_start_cdata,		arginfo_xmlwriter_resource)
	PHP_FE(xmlwriter_end_cdata,			arginfo_xmlwriter_resource)
	PHP_FE(xmlwriter_write_cdata,		arginfo_xmlwriter_write_cdata)
	PHP_FE(xmlwriter_text,				arginfo_xmlwriter_text)
	PHP_FE(xmlwriter_write_raw,			arginfo_xmlwriter_write_raw)
	PHP_FE(xmlwriter_start_document,	arginfo_xmlwriter_start_document)
	PHP_FE(xmlwriter_end_document,		arginfo_xmlwriter_resource)
	PHP_FE(xmlwriter_write_comment,		arginfo_xmlwriter_write_comment)
	PHP_FE(xmlwriter_start_dtd,			arginfo_xmlwriter_start_dtd)
	PHP_FE(xmlwriter_end_dtd,			arginfo_xmlwriter_resource)
	PHP_FE(xmlwriter_write_dtd,			arginfo_xmlwriter_write_dtd)
	PHP_FE(xmlwriter_start_dtd_element,	arginfo_xmlwriter_start_dtd_element)
	PHP_FE(xmlwriter_end_dtd_element,	arginfo_xmlwriter_resource)
	PHP_FE(xmlwriter_write_dtd_element,	arginfo_xmlwriter_write_dtd_element)
#if LIBXML_VERSION > 20608
	PHP_FE(xmlwriter_start_dtd_attlist,	arginfo_xmlwriter_start_dtd_attlist)
	PHP_FE(xmlwriter_end_dtd_attlist,	arginfo_xmlwriter_resource)
	PHP_FE(xmlwriter_write_dtd_attlist,	arginfo_xmlwriter_write_dtd_attlist)
	PHP_FE(xmlwriter_start_dtd_entity,	arginfo_xmlwriter_start_dtd_entity)
	PHP_FE(xmlwriter_end_dtd_entity,	arginfo_xmlwriter_resource)
	PHP_FE(xmlwriter_write_dtd_entity,	arginfo_xmlwriter_write_dtd_entity)
#endif
	PHP_FE(xmlwriter_output_memory,		arginfo_xmlwriter_output_memory)
	PHP_FE(xmlwriter_flush,				arginfo_xmlwriter_flush)
	PHP_FE_END
};
/* }}} */

#ifdef ZEND_ENGINE_2
/* {{{ xmlwriter_class_functions */
static const zend_function_entry xmlwriter_class_functions[] = {
	PHP_ME_MAPPING(openUri,		xmlwriter_open_uri,		arginfo_xmlwriter_open_uri, 0)
	PHP_ME_MAPPING(openMemory,	xmlwriter_open_memory, 	arginfo_xmlwriter_void, 0)
#if LIBXML_VERSION >= 20605
	PHP_ME_MAPPING(setIndent,	xmlwriter_set_indent,	arginfo_xmlwriter_method_set_indent, 0)
	PHP_ME_MAPPING(setIndentString,	xmlwriter_set_indent_string, arginfo_xmlwriter_method_set_indent_string, 0)
#endif
#if LIBXML_VERSION >= 20607
	PHP_ME_MAPPING(startComment,	xmlwriter_start_comment,	arginfo_xmlwriter_void, 0)
	PHP_ME_MAPPING(endComment,		xmlwriter_end_comment,		arginfo_xmlwriter_void, 0)
#endif
	PHP_ME_MAPPING(startAttribute,	xmlwriter_start_attribute,	arginfo_xmlwriter_method_start_attribute, 0)
	PHP_ME_MAPPING(endAttribute,	xmlwriter_end_attribute,	arginfo_xmlwriter_void, 0)
	PHP_ME_MAPPING(writeAttribute,	xmlwriter_write_attribute,	arginfo_xmlwriter_method_write_attribute, 0)
#if LIBXML_VERSION > 20617
	PHP_ME_MAPPING(startAttributeNs,	xmlwriter_start_attribute_ns,arginfo_xmlwriter_method_start_attribute_ns, 0)
	PHP_ME_MAPPING(writeAttributeNs,	xmlwriter_write_attribute_ns,arginfo_xmlwriter_method_write_attribute_ns, 0)
#endif
	PHP_ME_MAPPING(startElement,	xmlwriter_start_element,	arginfo_xmlwriter_method_start_element, 0)
	PHP_ME_MAPPING(endElement,		xmlwriter_end_element,		arginfo_xmlwriter_void, 0)
	PHP_ME_MAPPING(fullEndElement,	xmlwriter_full_end_element,	arginfo_xmlwriter_void, 0)
	PHP_ME_MAPPING(startElementNs,	xmlwriter_start_element_ns,	arginfo_xmlwriter_method_start_element_ns, 0)
	PHP_ME_MAPPING(writeElement,	xmlwriter_write_element,	arginfo_xmlwriter_method_write_element, 0)
	PHP_ME_MAPPING(writeElementNs,	xmlwriter_write_element_ns,	arginfo_xmlwriter_method_write_element_ns, 0)
	PHP_ME_MAPPING(startPi,			xmlwriter_start_pi,			arginfo_xmlwriter_method_start_pi, 0)
	PHP_ME_MAPPING(endPi,			xmlwriter_end_pi,			arginfo_xmlwriter_void, 0)
	PHP_ME_MAPPING(writePi,			xmlwriter_write_pi,			arginfo_xmlwriter_method_write_pi, 0)
	PHP_ME_MAPPING(startCdata,		xmlwriter_start_cdata,		arginfo_xmlwriter_void, 0)
	PHP_ME_MAPPING(endCdata,		xmlwriter_end_cdata,		arginfo_xmlwriter_void, 0)
	PHP_ME_MAPPING(writeCdata,		xmlwriter_write_cdata,		arginfo_xmlwriter_method_write_cdata, 0)
	PHP_ME_MAPPING(text,			xmlwriter_text,				arginfo_xmlwriter_method_text, 0)
	PHP_ME_MAPPING(writeRaw,		xmlwriter_write_raw,		arginfo_xmlwriter_method_write_raw, 0)
	PHP_ME_MAPPING(startDocument,	xmlwriter_start_document,	arginfo_xmlwriter_method_start_document, 0)
	PHP_ME_MAPPING(endDocument,		xmlwriter_end_document,		arginfo_xmlwriter_void, 0)
	PHP_ME_MAPPING(writeComment,	xmlwriter_write_comment,	arginfo_xmlwriter_method_write_comment, 0)
	PHP_ME_MAPPING(startDtd,		xmlwriter_start_dtd,		arginfo_xmlwriter_method_start_dtd, 0)
	PHP_ME_MAPPING(endDtd,			xmlwriter_end_dtd,			arginfo_xmlwriter_void, 0)
	PHP_ME_MAPPING(writeDtd,		xmlwriter_write_dtd,		arginfo_xmlwriter_method_write_dtd, 0)
	PHP_ME_MAPPING(startDtdElement,	xmlwriter_start_dtd_element,arginfo_xmlwriter_method_start_dtd_element, 0)
	PHP_ME_MAPPING(endDtdElement,	xmlwriter_end_dtd_element,	arginfo_xmlwriter_void, 0)
	PHP_ME_MAPPING(writeDtdElement,	xmlwriter_write_dtd_element,	arginfo_xmlwriter_method_write_dtd_element, 0)
#if LIBXML_VERSION > 20608
	PHP_ME_MAPPING(startDtdAttlist,	xmlwriter_start_dtd_attlist,	arginfo_xmlwriter_method_start_dtd_attlist, 0)
	PHP_ME_MAPPING(endDtdAttlist,	xmlwriter_end_dtd_attlist,	arginfo_xmlwriter_void, 0)
	PHP_ME_MAPPING(writeDtdAttlist,	xmlwriter_write_dtd_attlist,	arginfo_xmlwriter_method_write_dtd_attlist, 0)
	PHP_ME_MAPPING(startDtdEntity,	xmlwriter_start_dtd_entity,	arginfo_xmlwriter_method_start_dtd_entity, 0)
	PHP_ME_MAPPING(endDtdEntity,	xmlwriter_end_dtd_entity,	arginfo_xmlwriter_void, 0)
	PHP_ME_MAPPING(writeDtdEntity,	xmlwriter_write_dtd_entity,	arginfo_xmlwriter_method_write_dtd_entity, 0)
#endif
	PHP_ME_MAPPING(outputMemory,	xmlwriter_output_memory,	arginfo_xmlwriter_method_output_memory, 0)
	PHP_ME_MAPPING(flush,			xmlwriter_flush,			arginfo_xmlwriter_method_flush, 0)
	{NULL, NULL, NULL}
};
/* }}} */
#endif

/* {{{ function prototypes */
static PHP_MINIT_FUNCTION(xmlwriter);
static PHP_MSHUTDOWN_FUNCTION(xmlwriter);
static PHP_MINFO_FUNCTION(xmlwriter);

static int le_xmlwriter;
/* }}} */

/* _xmlwriter_get_valid_file_path should be made a 
	common function in libxml extension as code is common to a few xml extensions */
/* {{{ _xmlwriter_get_valid_file_path */
static char *_xmlwriter_get_valid_file_path(char *source, char *resolved_path, int resolved_path_len  TSRMLS_DC) {
	xmlURI *uri;
	xmlChar *escsource;
	char *file_dest;
	int isFileUri = 0;

	uri = xmlCreateURI();
	escsource = xmlURIEscapeStr((xmlChar *)source, (xmlChar *) ":");
	xmlParseURIReference(uri, (char *)escsource);
	xmlFree(escsource);

	if (uri->scheme != NULL) {
		/* absolute file uris - libxml only supports localhost or empty host */
		if (strncasecmp(source, "file:///", 8) == 0) {
			if (source[sizeof("file:///") - 1] == '\0') {
				xmlFreeURI(uri);
				return NULL;
			}
			isFileUri = 1;
#ifdef PHP_WIN32
			source += 8;
#else
			source += 7;
#endif
		} else if (strncasecmp(source, "file://localhost/",17) == 0) {
			if (source[sizeof("file://localhost/") - 1] == '\0') {
				xmlFreeURI(uri);
				return NULL;
			}

			isFileUri = 1;
#ifdef PHP_WIN32
			source += 17;
#else
			source += 16;
#endif
		}
	}

	if ((uri->scheme == NULL || isFileUri)) {
		char file_dirname[MAXPATHLEN];
		size_t dir_len;

		if (!VCWD_REALPATH(source, resolved_path) && !expand_filepath(source, resolved_path TSRMLS_CC)) {
			xmlFreeURI(uri);
			return NULL;
		}

		memcpy(file_dirname, source, strlen(source));
		dir_len = php_dirname(file_dirname, strlen(source));

		if (dir_len > 0) {
			struct stat buf;
			if (php_sys_stat(file_dirname, &buf) != 0) {
				xmlFreeURI(uri);
				return NULL;
			}
		}

		file_dest = resolved_path;
	} else {
		file_dest = source;
	}

	xmlFreeURI(uri);

	return file_dest;
}
/* }}} */

#ifndef ZEND_ENGINE_2
/* Channel libxml file io layer through the PHP streams subsystem.
 * This allows use of ftps:// and https:// urls */

/* {{{ php_xmlwriter_streams_IO_open_write_wrapper */
static void *php_xmlwriter_streams_IO_open_write_wrapper(const char *filename TSRMLS_DC)
{
	php_stream_wrapper *wrapper = NULL;
	void *ret_val = NULL;

	ret_val = php_stream_open_wrapper_ex((char *)filename, "wb", REPORT_ERRORS, NULL, NULL);
	return ret_val;
}
/* }}} */

/* {{{ php_xmlwriter_streams_IO_write */
static int php_xmlwriter_streams_IO_write(void *context, const char *buffer, int len)
{
	TSRMLS_FETCH();
	return php_stream_write((php_stream*)context, buffer, len);
}
/* }}} */

/* {{{ php_xmlwriter_streams_IO_close */
static int php_xmlwriter_streams_IO_close(void *context)
{
	TSRMLS_FETCH();
	return php_stream_close((php_stream*)context);
}
/* }}} */
#endif

/* {{{ xmlwriter_module_entry
 */
zend_module_entry xmlwriter_module_entry = {
	STANDARD_MODULE_HEADER,
	"xmlwriter",
	xmlwriter_functions,
	PHP_MINIT(xmlwriter),
	PHP_MSHUTDOWN(xmlwriter),
	NULL,
	NULL,
	PHP_MINFO(xmlwriter),
	"0.1",
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_XMLWRITER
ZEND_GET_MODULE(xmlwriter)
#endif

/* {{{ xmlwriter_objects_clone 
static void xmlwriter_objects_clone(void *object, void **object_clone TSRMLS_DC)
{
	TODO
}
}}} */

/* {{{ xmlwriter_dtor */
static void xmlwriter_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
	xmlwriter_object *intern;

	intern = (xmlwriter_object *) rsrc->ptr;
	xmlwriter_free_resource_ptr(intern TSRMLS_CC);
}
/* }}} */

static void php_xmlwriter_string_arg(INTERNAL_FUNCTION_PARAMETERS, xmlwriter_read_one_char_t internal_function, char *err_string)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name;
	int name_len, retval;

#ifdef ZEND_ENGINE_2
	zval *this = getThis();
	
	if (this) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, this);
	} else
#endif
	{
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &pind, &name, &name_len) == FAILURE) {
			return;
		}
	
		ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	}

	if (err_string != NULL) {
		XMLW_NAME_CHK(err_string);
	}

	ptr = intern->ptr;

	if (ptr) {
		retval = internal_function(ptr, (xmlChar *) name);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

static void php_xmlwriter_end(INTERNAL_FUNCTION_PARAMETERS, xmlwriter_read_int_t internal_function)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	int retval;
#ifdef ZEND_ENGINE_2
	zval *this = getThis();
	
	if (this) {
		XMLWRITER_FROM_OBJECT(intern, this);
		if (zend_parse_parameters_none() == FAILURE) {
			return;
		}
	} else 
#endif
	{
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &pind) == FAILURE) {
			return;
		}
		ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	}

	ptr = intern->ptr;

	if (ptr) {
		retval = internal_function(ptr);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}

#if LIBXML_VERSION >= 20605
/* {{{ proto bool xmlwriter_set_indent(resource xmlwriter, bool indent)
Toggle indentation on/off - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_set_indent)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	int retval;
	zend_bool indent;

#ifdef ZEND_ENGINE_2
	zval *this = getThis();
	
	if (this) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "b", &indent) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, this);
	} else
#endif
	{
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rb", &pind, &indent) == FAILURE) {
			return;
		}
		ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	}


	ptr = intern->ptr;
	if (ptr) {
		retval = xmlTextWriterSetIndent(ptr, indent);
		if (retval == 0) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool xmlwriter_set_indent_string(resource xmlwriter, string indentString)
Set string used for indenting - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_set_indent_string)
{
	php_xmlwriter_string_arg(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextWriterSetIndentString, NULL);
}
/* }}} */

#endif

/* {{{ proto bool xmlwriter_start_attribute(resource xmlwriter, string name)
Create start attribute - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_start_attribute)
{
	php_xmlwriter_string_arg(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextWriterStartAttribute, "Invalid Attribute Name");
}
/* }}} */

/* {{{ proto bool xmlwriter_end_attribute(resource xmlwriter)
End attribute - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_end_attribute)
{
	php_xmlwriter_end(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextWriterEndAttribute);
}
/* }}} */

#if LIBXML_VERSION > 20617
/* {{{ proto bool xmlwriter_start_attribute_ns(resource xmlwriter, string prefix, string name, string uri)
Create start namespaced attribute - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_start_attribute_ns)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name, *prefix, *uri;
	int name_len, prefix_len, uri_len, retval;
#ifdef ZEND_ENGINE_2
	zval *this = getThis();
	
	if (this) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss!", 
			&prefix, &prefix_len, &name, &name_len, &uri, &uri_len) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, this);
	} else
#endif
	{
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsss!", &pind, 
			&prefix, &prefix_len, &name, &name_len, &uri, &uri_len) == FAILURE) {
			return;
		}
		ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	}

	XMLW_NAME_CHK("Invalid Attribute Name");

	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterStartAttributeNS(ptr, (xmlChar *)prefix, (xmlChar *)name, (xmlChar *)uri);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}
/* }}} */
#endif

/* {{{ proto bool xmlwriter_write_attribute(resource xmlwriter, string name, string content)
Write full attribute - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_write_attribute)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name, *content;
	int name_len, content_len, retval;

#ifdef ZEND_ENGINE_2
	zval *this = getThis();
	
	if (this) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", 
			&name, &name_len, &content, &content_len) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, this);
	} else
#endif
	{
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &pind, 
			&name, &name_len, &content, &content_len) == FAILURE) {
			return;
		}
		ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	}

	XMLW_NAME_CHK("Invalid Attribute Name");

	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterWriteAttribute(ptr, (xmlChar *)name, (xmlChar *)content);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}
/* }}} */

#if LIBXML_VERSION > 20617
/* {{{ proto bool xmlwriter_write_attribute_ns(resource xmlwriter, string prefix, string name, string uri, string content)
Write full namespaced attribute - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_write_attribute_ns)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name, *prefix, *uri, *content;
	int name_len, prefix_len, uri_len, content_len, retval;

#ifdef ZEND_ENGINE_2
	zval *this = getThis();
	
	if (this) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss!s", 
			&prefix, &prefix_len, &name, &name_len, &uri, &uri_len, &content, &content_len) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, this);
	} else
#endif
	{
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsss!s", &pind, 
			&prefix, &prefix_len, &name, &name_len, &uri, &uri_len, &content, &content_len) == FAILURE) {
			return;
		}
		ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	}

	XMLW_NAME_CHK("Invalid Attribute Name");

	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterWriteAttributeNS(ptr, (xmlChar *)prefix, (xmlChar *)name, (xmlChar *)uri, (xmlChar *)content);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}
/* }}} */
#endif

/* {{{ proto bool xmlwriter_start_element(resource xmlwriter, string name)
Create start element tag - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_start_element)
{
	php_xmlwriter_string_arg(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextWriterStartElement, "Invalid Element Name");
}
/* }}} */

/* {{{ proto bool xmlwriter_start_element_ns(resource xmlwriter, string prefix, string name, string uri)
Create start namespaced element tag - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_start_element_ns)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name, *prefix, *uri;
	int name_len, prefix_len, uri_len, retval;
#ifdef ZEND_ENGINE_2
	zval *this = getThis();
	
	if (this) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s!ss!",
			&prefix, &prefix_len, &name, &name_len, &uri, &uri_len) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, this);
	} else
#endif
	{
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs!ss!", &pind, 
			&prefix, &prefix_len, &name, &name_len, &uri, &uri_len) == FAILURE) {
			return;
		}
		ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	}

	XMLW_NAME_CHK("Invalid Element Name");

	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterStartElementNS(ptr, (xmlChar *)prefix, (xmlChar *)name, (xmlChar *)uri);
		if (retval != -1) {
			RETURN_TRUE;
		}
		
	}
	
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool xmlwriter_end_element(resource xmlwriter)
End current element - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_end_element)
{
	php_xmlwriter_end(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextWriterEndElement);
}
/* }}} */

/* {{{ proto bool xmlwriter_full_end_element(resource xmlwriter)
End current element - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_full_end_element)
{
	php_xmlwriter_end(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextWriterFullEndElement);
}
/* }}} */

/* {{{ proto bool xmlwriter_write_element(resource xmlwriter, string name[, string content])
Write full element tag - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_write_element)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name, *content = NULL;
	int name_len, content_len, retval;

#ifdef ZEND_ENGINE_2
	zval *this = getThis();
	
	if (this) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s!",
			&name, &name_len, &content, &content_len) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, this);
	} else
#endif
	{
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|s!", &pind, 
			&name, &name_len, &content, &content_len) == FAILURE) {
			return;
		}
		ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	}

	XMLW_NAME_CHK("Invalid Element Name");

	ptr = intern->ptr;

	if (ptr) {
		if (!content) {
			retval = xmlTextWriterStartElement(ptr, (xmlChar *)name);
            if (retval == -1) {
                RETURN_FALSE;
            }
			xmlTextWriterEndElement(ptr);
            if (retval == -1) {
                RETURN_FALSE;
            }
		} else {
			retval = xmlTextWriterWriteElement(ptr, (xmlChar *)name, (xmlChar *)content);
		}
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool xmlwriter_write_element_ns(resource xmlwriter, string prefix, string name, string uri[, string content])
Write full namesapced element tag - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_write_element_ns)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name, *prefix, *uri, *content = NULL;
	int name_len, prefix_len, uri_len, content_len, retval;

#ifdef ZEND_ENGINE_2
	zval *this = getThis();
	
	if (this) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s!ss!|s!", 
			&prefix, &prefix_len, &name, &name_len, &uri, &uri_len, &content, &content_len) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, this);
	} else
#endif
	{
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs!ss!|s!", &pind, 
			&prefix, &prefix_len, &name, &name_len, &uri, &uri_len, &content, &content_len) == FAILURE) {
			return;
		}
		ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	}

	XMLW_NAME_CHK("Invalid Element Name");

	ptr = intern->ptr;

	if (ptr) {
		if (!content) {
			retval = xmlTextWriterStartElementNS(ptr,(xmlChar *)prefix, (xmlChar *)name, (xmlChar *)uri);
            if (retval == -1) {
                RETURN_FALSE;
            }
			retval = xmlTextWriterEndElement(ptr);
            if (retval == -1) {
                RETURN_FALSE;
            }
		} else {
			retval = xmlTextWriterWriteElementNS(ptr, (xmlChar *)prefix, (xmlChar *)name, (xmlChar *)uri, (xmlChar *)content);
		}
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool xmlwriter_start_pi(resource xmlwriter, string target)
Create start PI tag - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_start_pi)
{
	php_xmlwriter_string_arg(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextWriterStartPI, "Invalid PI Target");
}
/* }}} */

/* {{{ proto bool xmlwriter_end_pi(resource xmlwriter)
End current PI - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_end_pi)
{
	php_xmlwriter_end(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextWriterEndPI);
}
/* }}} */

/* {{{ proto bool xmlwriter_write_pi(resource xmlwriter, string target, string content)
Write full PI tag - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_write_pi)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name, *content;
	int name_len, content_len, retval;

#ifdef ZEND_ENGINE_2
	zval *this = getThis();
	
	if (this) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
			&name, &name_len, &content, &content_len) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, this);
	} else
#endif
	{
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &pind, 
			&name, &name_len, &content, &content_len) == FAILURE) {
			return;
		}
		ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	}

	XMLW_NAME_CHK("Invalid PI Target");

	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterWritePI(ptr, (xmlChar *)name, (xmlChar *)content);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool xmlwriter_start_cdata(resource xmlwriter)
Create start CDATA tag - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_start_cdata)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	int retval;
#ifdef ZEND_ENGINE_2
	zval *this = getThis();
	
	if (this) {
		XMLWRITER_FROM_OBJECT(intern, this);
	} else
#endif
	{
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &pind) == FAILURE) {
			return;
		}
		ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	}

	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterStartCDATA(ptr);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool xmlwriter_end_cdata(resource xmlwriter)
End current CDATA - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_end_cdata)
{
	php_xmlwriter_end(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextWriterEndCDATA);
}
/* }}} */

/* {{{ proto bool xmlwriter_write_cdata(resource xmlwriter, string content)
Write full CDATA tag - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_write_cdata)
{
	php_xmlwriter_string_arg(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextWriterWriteCDATA, NULL);
}
/* }}} */

/* {{{ proto bool xmlwriter_write_raw(resource xmlwriter, string content)
Write text - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_write_raw)
{
	php_xmlwriter_string_arg(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextWriterWriteRaw, NULL);
}
/* }}} */

/* {{{ proto bool xmlwriter_text(resource xmlwriter, string content)
Write text - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_text)
{
	php_xmlwriter_string_arg(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextWriterWriteString, NULL);
}
/* }}} */

#if LIBXML_VERSION >= 20607
/* {{{ proto bool xmlwriter_start_comment(resource xmlwriter)
Create start comment - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_start_comment)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	int retval;
#ifdef ZEND_ENGINE_2
	zval *this = getThis();
	
	if (this) {
		XMLWRITER_FROM_OBJECT(intern, this);
	} else
#endif
	{
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &pind) == FAILURE) {
			return;
		}
		ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	}

	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterStartComment(ptr);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool xmlwriter_end_comment(resource xmlwriter)
Create end comment - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_end_comment)
{
	php_xmlwriter_end(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextWriterEndComment);
}
/* }}} */
#endif  /* LIBXML_VERSION >= 20607 */


/* {{{ proto bool xmlwriter_write_comment(resource xmlwriter, string content)
Write full comment tag - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_write_comment)
{
	php_xmlwriter_string_arg(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextWriterWriteComment, NULL);
}
/* }}} */

/* {{{ proto bool xmlwriter_start_document(resource xmlwriter, string version, string encoding, string standalone)
Create document tag - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_start_document)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *version = NULL, *enc = NULL, *alone = NULL;
	int version_len, enc_len, alone_len, retval;

#ifdef ZEND_ENGINE_2
	zval *this = getThis();

	if (this) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s!s!s!", &version, &version_len, &enc, &enc_len, &alone, &alone_len) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, this);
	} else
#endif
	{
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|s!s!s!", &pind, &version, &version_len, &enc, &enc_len, &alone, &alone_len) == FAILURE) {
			return;
		}
		ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	}

	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterStartDocument(ptr, version, enc, alone);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool xmlwriter_end_document(resource xmlwriter)
End current document - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_end_document)
{
	php_xmlwriter_end(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextWriterEndDocument);
}
/* }}} */

/* {{{ proto bool xmlwriter_start_dtd(resource xmlwriter, string name, string pubid, string sysid)
Create start DTD tag - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_start_dtd)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name, *pubid = NULL, *sysid = NULL;
	int name_len, pubid_len, sysid_len, retval;

#ifdef ZEND_ENGINE_2
	zval *this = getThis();

	if (this) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s!s!", &name, &name_len, &pubid, &pubid_len, &sysid, &sysid_len) == FAILURE) {
			return;
		}

		XMLWRITER_FROM_OBJECT(intern, this);
	} else
#endif
	{
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|s!s!", &pind, &name, &name_len, &pubid, &pubid_len, &sysid, &sysid_len) == FAILURE) {
			return;
		}
	
		ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	}
	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterStartDTD(ptr, (xmlChar *)name, (xmlChar *)pubid, (xmlChar *)sysid);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool xmlwriter_end_dtd(resource xmlwriter)
End current DTD - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_end_dtd)
{
	php_xmlwriter_end(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextWriterEndDTD);
}
/* }}} */

/* {{{ proto bool xmlwriter_write_dtd(resource xmlwriter, string name, string pubid, string sysid, string subset)
Write full DTD tag - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_write_dtd)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name, *pubid = NULL, *sysid = NULL, *subset = NULL;
	int name_len, pubid_len, sysid_len, subset_len, retval;

#ifdef ZEND_ENGINE_2
	zval *this = getThis();

	if (this) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s!s!s!", &name, &name_len, &pubid, &pubid_len, &sysid, &sysid_len, &subset, &subset_len) == FAILURE) {
			return;
		}

		XMLWRITER_FROM_OBJECT(intern, this);
	} else
#endif
	{
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|s!s!s!", &pind, &name, &name_len, &pubid, &pubid_len, &sysid, &sysid_len, &subset, &subset_len) == FAILURE) {
			return;
		}
	
		ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	}

	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterWriteDTD(ptr, (xmlChar *)name, (xmlChar *)pubid, (xmlChar *)sysid, (xmlChar *)subset);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool xmlwriter_start_dtd_element(resource xmlwriter, string name)
Create start DTD element - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_start_dtd_element)
{
	php_xmlwriter_string_arg(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextWriterStartDTDElement, "Invalid Element Name");
}
/* }}} */

/* {{{ proto bool xmlwriter_end_dtd_element(resource xmlwriter)
End current DTD element - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_end_dtd_element)
{
	php_xmlwriter_end(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextWriterEndDTDElement);
}
/* }}} */

/* {{{ proto bool xmlwriter_write_dtd_element(resource xmlwriter, string name, string content)
Write full DTD element tag - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_write_dtd_element)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name, *content;
	int name_len, content_len, retval;

#ifdef ZEND_ENGINE_2
	zval *this = getThis();

	if (this) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name, &name_len, &content, &content_len) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, this);
	} else
#endif
	{
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &pind, 
			&name, &name_len, &content, &content_len) == FAILURE) {
			return;
		}
		ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	}

	XMLW_NAME_CHK("Invalid Element Name");

	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterWriteDTDElement(ptr, (xmlChar *)name, (xmlChar *)content);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}
/* }}} */

#if LIBXML_VERSION > 20608
/* {{{ proto bool xmlwriter_start_dtd_attlist(resource xmlwriter, string name)
Create start DTD AttList - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_start_dtd_attlist)
{
	php_xmlwriter_string_arg(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextWriterStartDTDAttlist, "Invalid Element Name");
}
/* }}} */

/* {{{ proto bool xmlwriter_end_dtd_attlist(resource xmlwriter)
End current DTD AttList - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_end_dtd_attlist)
{
	php_xmlwriter_end(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextWriterEndDTDAttlist);
}
/* }}} */

/* {{{ proto bool xmlwriter_write_dtd_attlist(resource xmlwriter, string name, string content)
Write full DTD AttList tag - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_write_dtd_attlist)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name, *content;
	int name_len, content_len, retval;

	
#ifdef ZEND_ENGINE_2
	zval *this = getThis();

	if (this) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
			&name, &name_len, &content, &content_len) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, this);
	} else
#endif
	{
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &pind, 
			&name, &name_len, &content, &content_len) == FAILURE) {
			return;
		}
		ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	}

	XMLW_NAME_CHK("Invalid Element Name");

	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterWriteDTDAttlist(ptr, (xmlChar *)name, (xmlChar *)content);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool xmlwriter_start_dtd_entity(resource xmlwriter, string name, bool isparam)
Create start DTD Entity - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_start_dtd_entity)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name;
	int name_len, retval;
	zend_bool isparm;

	
#ifdef ZEND_ENGINE_2
	zval *this = getThis();

	if (this) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sb", &name, &name_len, &isparm) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, this);
	} else
#endif
	{
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsb", &pind, &name, &name_len, &isparm) == FAILURE) {
			return;
		}
		ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	}

	XMLW_NAME_CHK("Invalid Attribute Name");

	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterStartDTDEntity(ptr, isparm, (xmlChar *)name);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool xmlwriter_end_dtd_entity(resource xmlwriter)
End current DTD Entity - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_end_dtd_entity)
{
	php_xmlwriter_end(INTERNAL_FUNCTION_PARAM_PASSTHRU, xmlTextWriterEndDTDEntity);
}
/* }}} */

/* {{{ proto bool xmlwriter_write_dtd_entity(resource xmlwriter, string name, string content [, int pe [, string pubid [, string sysid [, string ndataid]]]])
Write full DTD Entity tag - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_write_dtd_entity)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name, *content;
	int name_len, content_len, retval;
	/* Optional parameters */
	char *pubid = NULL, *sysid = NULL, *ndataid = NULL;
	zend_bool pe = 0;
	int pubid_len, sysid_len, ndataid_len;

#ifdef ZEND_ENGINE_2
	zval *this = getThis();

	if (this) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|bsss",
			&name, &name_len, &content, &content_len, &pe, &pubid, &pubid_len,
			&sysid, &sysid_len, &ndataid, &ndataid_len) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, this);
	} else
#endif
	{
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss|bsss", &pind, 
			&name, &name_len, &content, &content_len, &pe, &pubid, &pubid_len,
			&sysid, &sysid_len, &ndataid, &ndataid_len) == FAILURE) {
			return;
		}
		ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	}

	XMLW_NAME_CHK("Invalid Element Name");

	ptr = intern->ptr;

	if (ptr) {
		retval = xmlTextWriterWriteDTDEntity(ptr, pe, (xmlChar *)name, (xmlChar *)pubid, (xmlChar *)sysid, (xmlChar *)ndataid, (xmlChar *)content);
		if (retval != -1) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}
/* }}} */
#endif

/* {{{ proto resource xmlwriter_open_uri(resource xmlwriter, string source)
Create new xmlwriter using source uri for output */
static PHP_FUNCTION(xmlwriter_open_uri)
{
	char *valid_file = NULL;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *source;
	char resolved_path[MAXPATHLEN + 1];
	int source_len;

#ifdef ZEND_ENGINE_2
	zval *this = getThis();
	ze_xmlwriter_object *ze_obj = NULL;
#endif

#ifndef ZEND_ENGINE_2
	xmlOutputBufferPtr out_buffer;
	void *ioctx;
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &source, &source_len) == FAILURE) {
		return;
	}
	
#ifdef ZEND_ENGINE_2
	if (this) {
		/* We do not use XMLWRITER_FROM_OBJECT, xmlwriter init function here */
		ze_obj = (ze_xmlwriter_object*) zend_object_store_get_object(this TSRMLS_CC); 
	}
#endif

	if (source_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty string as source");
		RETURN_FALSE;
	}

	valid_file = _xmlwriter_get_valid_file_path(source, resolved_path, MAXPATHLEN TSRMLS_CC);
	if (!valid_file) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to resolve file path");
		RETURN_FALSE;
	}

	/* TODO: Fix either the PHP stream or libxml APIs: it can then detect when a given 
		 path is valid and not report out of memory error. Once it is done, remove the
		 directory check in _xmlwriter_get_valid_file_path */
#ifndef ZEND_ENGINE_2
	ioctx = php_xmlwriter_streams_IO_open_write_wrapper(valid_file TSRMLS_CC);
	if (ioctx == NULL) {
		RETURN_FALSE;
	}

	out_buffer = xmlOutputBufferCreateIO(php_xmlwriter_streams_IO_write, 
		php_xmlwriter_streams_IO_close, ioctx, NULL);

	if (out_buffer == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to create output buffer");
		RETURN_FALSE;
	}
	ptr = xmlNewTextWriter(out_buffer);
#else
	ptr = xmlNewTextWriterFilename(valid_file, 0);
#endif

	if (!ptr) {
		RETURN_FALSE;
	}

	intern = emalloc(sizeof(xmlwriter_object));
	intern->ptr = ptr;
	intern->output = NULL;
#ifndef ZEND_ENGINE_2
	intern->uri_output = out_buffer;
#else
	if (this) {
		if (ze_obj->xmlwriter_ptr) {
			xmlwriter_free_resource_ptr(ze_obj->xmlwriter_ptr TSRMLS_CC);
		}
		ze_obj->xmlwriter_ptr = intern;
		RETURN_TRUE;
	} else
#endif
	{
		ZEND_REGISTER_RESOURCE(return_value,intern,le_xmlwriter);
	}
}
/* }}} */

/* {{{ proto resource xmlwriter_open_memory()
Create new xmlwriter using memory for string output */
static PHP_FUNCTION(xmlwriter_open_memory)
{
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	xmlBufferPtr buffer;

#ifdef ZEND_ENGINE_2
	zval *this = getThis();
	ze_xmlwriter_object *ze_obj = NULL;
#endif

#ifdef ZEND_ENGINE_2
	if (this) {
		/* We do not use XMLWRITER_FROM_OBJECT, xmlwriter init function here */
		ze_obj = (ze_xmlwriter_object*) zend_object_store_get_object(this TSRMLS_CC); 
	}
#endif

	buffer = xmlBufferCreate();

	if (buffer == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to create output buffer");
		RETURN_FALSE;
	}

	ptr = xmlNewTextWriterMemory(buffer, 0);
	if (! ptr) {
		xmlBufferFree(buffer);
		RETURN_FALSE;
	}

	intern = emalloc(sizeof(xmlwriter_object));
	intern->ptr = ptr;
	intern->output = buffer;
#ifndef ZEND_ENGINE_2
	intern->uri_output = NULL;
#else
	if (this) {
		if (ze_obj->xmlwriter_ptr) {
			xmlwriter_free_resource_ptr(ze_obj->xmlwriter_ptr TSRMLS_CC);
		}
		ze_obj->xmlwriter_ptr = intern;
		RETURN_TRUE;
	} else
#endif
	{
		ZEND_REGISTER_RESOURCE(return_value,intern,le_xmlwriter);
	}

}
/* }}} */

/* {{{ php_xmlwriter_flush */
static void php_xmlwriter_flush(INTERNAL_FUNCTION_PARAMETERS, int force_string) {
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	xmlBufferPtr buffer;
	zend_bool empty = 1;
	int output_bytes;


#ifdef ZEND_ENGINE_2
	zval *this = getThis();

	if (this) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &empty) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, this);
	} else
#endif
	{
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|b", &pind, &empty) == FAILURE) {
			return;
		}

		ZEND_FETCH_RESOURCE(intern,xmlwriter_object *, &pind, -1, "XMLWriter", le_xmlwriter);
	}
	ptr = intern->ptr;

	if (ptr) {
		buffer = intern->output;
		if (force_string == 1 && buffer == NULL) {
			RETURN_EMPTY_STRING();
		}
		output_bytes = xmlTextWriterFlush(ptr);
		if (buffer) {
			RETVAL_STRING((char *) buffer->content, 1);
			if (empty) {
				xmlBufferEmpty(buffer);
			}
		} else {
			RETVAL_LONG(output_bytes);
		}
		return;
	}
	
	RETURN_EMPTY_STRING();
}
/* }}} */

/* {{{ proto string xmlwriter_output_memory(resource xmlwriter [,bool flush])
Output current buffer as string */
static PHP_FUNCTION(xmlwriter_output_memory)
{
	php_xmlwriter_flush(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto mixed xmlwriter_flush(resource xmlwriter [,bool empty])
Output current buffer */
static PHP_FUNCTION(xmlwriter_flush)
{
	php_xmlwriter_flush(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
static PHP_MINIT_FUNCTION(xmlwriter)
{
#ifdef ZEND_ENGINE_2
	zend_class_entry ce;
#endif

	le_xmlwriter = zend_register_list_destructors_ex(xmlwriter_dtor, NULL, "xmlwriter", module_number);

#ifdef ZEND_ENGINE_2
	memcpy(&xmlwriter_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	xmlwriter_object_handlers.clone_obj      = NULL;
	INIT_CLASS_ENTRY(ce, "XMLWriter", xmlwriter_class_functions);
	ce.create_object = xmlwriter_object_new;
	xmlwriter_class_entry_ce = zend_register_internal_class(&ce TSRMLS_CC);
#endif
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
static PHP_MSHUTDOWN_FUNCTION(xmlwriter)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
static PHP_MINFO_FUNCTION(xmlwriter)
{
	php_info_print_table_start();
	{
		php_info_print_table_row(2, "XMLWriter", "enabled");
	}
	php_info_print_table_end();
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
