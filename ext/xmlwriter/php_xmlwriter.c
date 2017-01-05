/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2017 The PHP Group                                |
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

static void xmlwriter_free_resource_ptr(xmlwriter_object *intern);
static void xmlwriter_dtor(zend_resource *rsrc);

typedef int (*xmlwriter_read_one_char_t)(xmlTextWriterPtr writer, const xmlChar *content);
typedef int (*xmlwriter_read_int_t)(xmlTextWriterPtr writer);

/* {{{ xmlwriter_object_free_storage */
static void xmlwriter_free_resource_ptr(xmlwriter_object *intern)
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

/* {{{ XMLWRITER_FROM_OBJECT */
#define XMLWRITER_FROM_OBJECT(intern, object) \
	{ \
		ze_xmlwriter_object *obj = Z_XMLWRITER_P(object); \
		intern = obj->xmlwriter_ptr; \
		if (!intern) { \
			php_error_docref(NULL, E_WARNING, "Invalid or uninitialized XMLWriter object"); \
			RETURN_FALSE; \
		} \
	}
/* }}} */

static zend_object_handlers xmlwriter_object_handlers;

/* {{{ xmlwriter_object_free_storage */
static void xmlwriter_object_free_storage(zend_object *object)
{
	ze_xmlwriter_object *intern = php_xmlwriter_fetch_object(object);
	if (!intern) {
		return;
	}
	if (intern->xmlwriter_ptr) {
		xmlwriter_free_resource_ptr(intern->xmlwriter_ptr);
	}
	intern->xmlwriter_ptr = NULL;
	zend_object_std_dtor(&intern->std);
}
/* }}} */


/* {{{ xmlwriter_object_new */
static zend_object *xmlwriter_object_new(zend_class_entry *class_type)
{
	ze_xmlwriter_object *intern;

	intern = ecalloc(1, sizeof(ze_xmlwriter_object) + zend_object_properties_size(class_type));
	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &xmlwriter_object_handlers;

	return &intern->std;
}
/* }}} */

#define XMLW_NAME_CHK(__err) \
	if (xmlValidateName((xmlChar *) name, 0) != 0) {	\
		php_error_docref(NULL, E_WARNING, "%s", __err);	\
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

/* {{{ function prototypes */
static PHP_MINIT_FUNCTION(xmlwriter);
static PHP_MSHUTDOWN_FUNCTION(xmlwriter);
static PHP_MINFO_FUNCTION(xmlwriter);

static int le_xmlwriter;
/* }}} */

/* _xmlwriter_get_valid_file_path should be made a
	common function in libxml extension as code is common to a few xml extensions */
/* {{{ _xmlwriter_get_valid_file_path */
static char *_xmlwriter_get_valid_file_path(char *source, char *resolved_path, int resolved_path_len ) {
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

		if (!VCWD_REALPATH(source, resolved_path) && !expand_filepath(source, resolved_path)) {
			xmlFreeURI(uri);
			return NULL;
		}

		memcpy(file_dirname, source, strlen(source));
		dir_len = php_dirname(file_dirname, strlen(source));

		if (dir_len > 0) {
			zend_stat_t buf;
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
	PHP_XMLWRITER_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_XMLWRITER
ZEND_GET_MODULE(xmlwriter)
#endif

/* {{{ xmlwriter_objects_clone
static void xmlwriter_objects_clone(void *object, void **object_clone)
{
	TODO
}
}}} */

/* {{{ xmlwriter_dtor */
static void xmlwriter_dtor(zend_resource *rsrc) {
	xmlwriter_object *intern;

	intern = (xmlwriter_object *) rsrc->ptr;
	xmlwriter_free_resource_ptr(intern);
}
/* }}} */

static void php_xmlwriter_string_arg(INTERNAL_FUNCTION_PARAMETERS, xmlwriter_read_one_char_t internal_function, char *err_string)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name;
	size_t name_len, retval;

	zval *self = getThis();

	if (self) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, self);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs", &pind, &name, &name_len) == FAILURE) {
			return;
		}

		if ((intern = (xmlwriter_object *)zend_fetch_resource(Z_RES_P(pind), "XMLWriter", le_xmlwriter)) == NULL) {
			RETURN_FALSE;
		}
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
	zval *self = getThis();

	if (self) {
		XMLWRITER_FROM_OBJECT(intern, self);
		if (zend_parse_parameters_none() == FAILURE) {
			return;
		}
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pind) == FAILURE) {
			return;
		}
		if ((intern = (xmlwriter_object *)zend_fetch_resource(Z_RES_P(pind), "XMLWriter", le_xmlwriter)) == NULL) {
			RETURN_FALSE;
		}
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

	zval *self = getThis();

	if (self) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "b", &indent) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, self);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "rb", &pind, &indent) == FAILURE) {
			return;
		}
		if ((intern = (xmlwriter_object *)zend_fetch_resource(Z_RES_P(pind), "XMLWriter", le_xmlwriter)) == NULL) {
			RETURN_FALSE;
		}
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
	size_t name_len, prefix_len, uri_len;
	int retval;
	zval *self = getThis();

	if (self) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "sss!",
			&prefix, &prefix_len, &name, &name_len, &uri, &uri_len) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, self);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "rsss!", &pind,
			&prefix, &prefix_len, &name, &name_len, &uri, &uri_len) == FAILURE) {
			return;
		}
		if ((intern = (xmlwriter_object *)zend_fetch_resource(Z_RES_P(pind), "XMLWriter", le_xmlwriter)) == NULL) {
			RETURN_FALSE;
		}
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
	size_t name_len, content_len;
	int retval;
	zval *self = getThis();

	if (self) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss",
			&name, &name_len, &content, &content_len) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, self);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "rss", &pind,
			&name, &name_len, &content, &content_len) == FAILURE) {
			return;
		}
		if ((intern = (xmlwriter_object *)zend_fetch_resource(Z_RES_P(pind), "XMLWriter", le_xmlwriter)) == NULL) {
			RETURN_FALSE;
		}
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
	size_t name_len, prefix_len, uri_len, content_len;
	int retval;

	zval *self = getThis();

	if (self) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "sss!s",
			&prefix, &prefix_len, &name, &name_len, &uri, &uri_len, &content, &content_len) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, self);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "rsss!s", &pind,
			&prefix, &prefix_len, &name, &name_len, &uri, &uri_len, &content, &content_len) == FAILURE) {
			return;
		}
		if ((intern = (xmlwriter_object *)zend_fetch_resource(Z_RES_P(pind), "XMLWriter", le_xmlwriter)) == NULL) {
			RETURN_FALSE;
		}
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
	size_t name_len, prefix_len, uri_len;
	int retval;
	zval *self = getThis();

	if (self) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "s!ss!",
			&prefix, &prefix_len, &name, &name_len, &uri, &uri_len) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, self);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs!ss!", &pind,
			&prefix, &prefix_len, &name, &name_len, &uri, &uri_len) == FAILURE) {
			return;
		}
		if ((intern = (xmlwriter_object *)zend_fetch_resource(Z_RES_P(pind), "XMLWriter", le_xmlwriter)) == NULL) {
			RETURN_FALSE;
		}
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
	size_t name_len, content_len;
	int retval;
	zval *self = getThis();

	if (self) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|s!",
			&name, &name_len, &content, &content_len) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, self);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs|s!", &pind,
			&name, &name_len, &content, &content_len) == FAILURE) {
			return;
		}
		if ((intern = (xmlwriter_object *)zend_fetch_resource(Z_RES_P(pind), "XMLWriter", le_xmlwriter)) == NULL) {
			RETURN_FALSE;
		}
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
	size_t name_len, prefix_len, uri_len, content_len;
	int retval;
	zval *self = getThis();

	if (self) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "s!ss!|s!",
			&prefix, &prefix_len, &name, &name_len, &uri, &uri_len, &content, &content_len) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, self);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs!ss!|s!", &pind,
			&prefix, &prefix_len, &name, &name_len, &uri, &uri_len, &content, &content_len) == FAILURE) {
			return;
		}
		if ((intern = (xmlwriter_object *)zend_fetch_resource(Z_RES_P(pind), "XMLWriter", le_xmlwriter)) == NULL) {
			RETURN_FALSE;
		}
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
	size_t name_len, content_len;
	int retval;

	zval *self = getThis();

	if (self) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss",
			&name, &name_len, &content, &content_len) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, self);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "rss", &pind,
			&name, &name_len, &content, &content_len) == FAILURE) {
			return;
		}
		if ((intern = (xmlwriter_object *)zend_fetch_resource(Z_RES_P(pind), "XMLWriter", le_xmlwriter)) == NULL) {
			RETURN_FALSE;
		}
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
	zval *self = getThis();

	if (self) {
		XMLWRITER_FROM_OBJECT(intern, self);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pind) == FAILURE) {
			return;
		}
		if ((intern = (xmlwriter_object *)zend_fetch_resource(Z_RES_P(pind), "XMLWriter", le_xmlwriter)) == NULL) {
			RETURN_FALSE;
		}
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
	zval *self = getThis();

	if (self) {
		XMLWRITER_FROM_OBJECT(intern, self);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pind) == FAILURE) {
			return;
		}
		if ((intern = (xmlwriter_object *)zend_fetch_resource(Z_RES_P(pind), "XMLWriter", le_xmlwriter)) == NULL) {
			RETURN_FALSE;
		}
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
	size_t version_len, enc_len, alone_len;
	int retval;

	zval *self = getThis();

	if (self) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s!s!s!", &version, &version_len, &enc, &enc_len, &alone, &alone_len) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, self);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|s!s!s!", &pind, &version, &version_len, &enc, &enc_len, &alone, &alone_len) == FAILURE) {
			return;
		}
		if ((intern = (xmlwriter_object *)zend_fetch_resource(Z_RES_P(pind), "XMLWriter", le_xmlwriter)) == NULL) {
			RETURN_FALSE;
		}
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
	size_t name_len, pubid_len, sysid_len;
	int retval;
	zval *self = getThis();

	if (self) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|s!s!", &name, &name_len, &pubid, &pubid_len, &sysid, &sysid_len) == FAILURE) {
			return;
		}

		XMLWRITER_FROM_OBJECT(intern, self);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs|s!s!", &pind, &name, &name_len, &pubid, &pubid_len, &sysid, &sysid_len) == FAILURE) {
			return;
		}
		if ((intern = (xmlwriter_object *)zend_fetch_resource(Z_RES_P(pind), "XMLWriter", le_xmlwriter)) == NULL) {
			RETURN_FALSE;
		}
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
	size_t name_len, pubid_len, sysid_len, subset_len;
	int retval;
	zval *self = getThis();

	if (self) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|s!s!s!", &name, &name_len, &pubid, &pubid_len, &sysid, &sysid_len, &subset, &subset_len) == FAILURE) {
			return;
		}

		XMLWRITER_FROM_OBJECT(intern, self);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs|s!s!s!", &pind, &name, &name_len, &pubid, &pubid_len, &sysid, &sysid_len, &subset, &subset_len) == FAILURE) {
			return;
		}

		if ((intern = (xmlwriter_object *)zend_fetch_resource(Z_RES_P(pind), "XMLWriter", le_xmlwriter)) == NULL) {
			RETURN_FALSE;
		}
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
	size_t name_len, content_len;
	int retval;
	zval *self = getThis();

	if (self) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &name, &name_len, &content, &content_len) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, self);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "rss", &pind,
			&name, &name_len, &content, &content_len) == FAILURE) {
			return;
		}
		if ((intern = (xmlwriter_object *)zend_fetch_resource(Z_RES_P(pind), "XMLWriter", le_xmlwriter)) == NULL) {
			RETURN_FALSE;
		}
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
	size_t name_len, content_len;
	int retval;
	zval *self = getThis();

	if (self) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss",
			&name, &name_len, &content, &content_len) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, self);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "rss", &pind,
			&name, &name_len, &content, &content_len) == FAILURE) {
			return;
		}
		if ((intern = (xmlwriter_object *)zend_fetch_resource(Z_RES_P(pind), "XMLWriter", le_xmlwriter)) == NULL) {
			RETURN_FALSE;
		}
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
	size_t name_len, retval;
	zend_bool isparm;
	zval *self = getThis();

	if (self) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "sb", &name, &name_len, &isparm) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, self);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "rsb", &pind, &name, &name_len, &isparm) == FAILURE) {
			return;
		}
		if ((intern = (xmlwriter_object *)zend_fetch_resource(Z_RES_P(pind), "XMLWriter", le_xmlwriter)) == NULL) {
			RETURN_FALSE;
		}
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

/* {{{ proto bool xmlwriter_write_dtd_entity(resource xmlwriter, string name, string content [, bool pe [, string pubid [, string sysid [, string ndataid]]]])
Write full DTD Entity tag - returns FALSE on error */
static PHP_FUNCTION(xmlwriter_write_dtd_entity)
{
	zval *pind;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *name, *content;
	size_t name_len, content_len;
	int retval;
	/* Optional parameters */
	char *pubid = NULL, *sysid = NULL, *ndataid = NULL;
	zend_bool pe = 0;
	size_t pubid_len, sysid_len, ndataid_len;
	zval *self = getThis();

	if (self) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|bsss",
			&name, &name_len, &content, &content_len, &pe, &pubid, &pubid_len,
			&sysid, &sysid_len, &ndataid, &ndataid_len) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, self);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "rss|bsss", &pind,
			&name, &name_len, &content, &content_len, &pe, &pubid, &pubid_len,
			&sysid, &sysid_len, &ndataid, &ndataid_len) == FAILURE) {
			return;
		}
		if ((intern = (xmlwriter_object *)zend_fetch_resource(Z_RES_P(pind), "XMLWriter", le_xmlwriter)) == NULL) {
			RETURN_FALSE;
		}
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

/* {{{ proto resource xmlwriter_open_uri(string source)
Create new xmlwriter using source uri for output */
static PHP_FUNCTION(xmlwriter_open_uri)
{
	char *valid_file = NULL;
	xmlwriter_object *intern;
	xmlTextWriterPtr ptr;
	char *source;
	char resolved_path[MAXPATHLEN + 1];
	size_t source_len;
	zval *self = getThis();
	ze_xmlwriter_object *ze_obj = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "p", &source, &source_len) == FAILURE) {
		return;
	}

	if (self) {
		/* We do not use XMLWRITER_FROM_OBJECT, xmlwriter init function here */
		ze_obj = Z_XMLWRITER_P(self);
	}

	if (source_len == 0) {
		php_error_docref(NULL, E_WARNING, "Empty string as source");
		RETURN_FALSE;
	}

	valid_file = _xmlwriter_get_valid_file_path(source, resolved_path, MAXPATHLEN);
	if (!valid_file) {
		php_error_docref(NULL, E_WARNING, "Unable to resolve file path");
		RETURN_FALSE;
	}

	ptr = xmlNewTextWriterFilename(valid_file, 0);

	if (!ptr) {
		RETURN_FALSE;
	}

	intern = emalloc(sizeof(xmlwriter_object));
	intern->ptr = ptr;
	intern->output = NULL;
	if (self) {
		if (ze_obj->xmlwriter_ptr) {
			xmlwriter_free_resource_ptr(ze_obj->xmlwriter_ptr);
		}
		ze_obj->xmlwriter_ptr = intern;
		RETURN_TRUE;
	} else {
		RETURN_RES(zend_register_resource(intern, le_xmlwriter));
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
	zval *self = getThis();
	ze_xmlwriter_object *ze_obj = NULL;

	if (self) {
		/* We do not use XMLWRITER_FROM_OBJECT, xmlwriter init function here */
		ze_obj = Z_XMLWRITER_P(self);
	}

	buffer = xmlBufferCreate();

	if (buffer == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to create output buffer");
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
	if (self) {
		if (ze_obj->xmlwriter_ptr) {
			xmlwriter_free_resource_ptr(ze_obj->xmlwriter_ptr);
		}
		ze_obj->xmlwriter_ptr = intern;
		RETURN_TRUE;
	} else {
		RETURN_RES(zend_register_resource(intern, le_xmlwriter));
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
	zval *self = getThis();

	if (self) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &empty) == FAILURE) {
			return;
		}
		XMLWRITER_FROM_OBJECT(intern, self);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|b", &pind, &empty) == FAILURE) {
			return;
		}

		if ((intern = (xmlwriter_object *)zend_fetch_resource(Z_RES_P(pind), "XMLWriter", le_xmlwriter)) == NULL) {
			RETURN_FALSE;
		}
	}
	ptr = intern->ptr;

	if (ptr) {
		buffer = intern->output;
		if (force_string == 1 && buffer == NULL) {
			RETURN_EMPTY_STRING();
		}
		output_bytes = xmlTextWriterFlush(ptr);
		if (buffer) {
			RETVAL_STRING((char *) buffer->content);
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
	zend_class_entry ce;
	le_xmlwriter = zend_register_list_destructors_ex(xmlwriter_dtor, NULL, "xmlwriter", module_number);

	memcpy(&xmlwriter_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	xmlwriter_object_handlers.offset = XtOffsetOf(ze_xmlwriter_object, std);
	xmlwriter_object_handlers.free_obj = xmlwriter_object_free_storage;
	xmlwriter_object_handlers.clone_obj = NULL;
	INIT_CLASS_ENTRY(ce, "XMLWriter", xmlwriter_class_functions);
	ce.create_object = xmlwriter_object_new;
	xmlwriter_class_entry_ce = zend_register_internal_class(&ce);

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
