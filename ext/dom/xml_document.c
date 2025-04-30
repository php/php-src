/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Niels Dossche <nielsdos@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"
#if defined(HAVE_LIBXML) && defined(HAVE_DOM)
#include "php_dom.h"
#include "namespace_compat.h"
#include "private_data.h"
#include "xml_serializer.h"
#include <libxml/xmlsave.h>

static bool check_options_validity(uint32_t arg_num, zend_long options)
{
	const zend_long VALID_OPTIONS = XML_PARSE_RECOVER
								  | XML_PARSE_NOENT
#if LIBXML_VERSION >= 21300
								  | XML_PARSE_NO_XXE
#endif
								  | XML_PARSE_DTDLOAD
								  | XML_PARSE_DTDATTR
								  | XML_PARSE_DTDVALID
								  | XML_PARSE_NOERROR
								  | XML_PARSE_NOWARNING
								  | XML_PARSE_NOBLANKS
								  | XML_PARSE_XINCLUDE
								  | XML_PARSE_NSCLEAN
								  | XML_PARSE_NOCDATA
								  | XML_PARSE_NONET
								  | XML_PARSE_PEDANTIC
								  | XML_PARSE_COMPACT
								  | XML_PARSE_HUGE
								  | XML_PARSE_BIG_LINES;
	if ((options & ~VALID_OPTIONS) != 0) {
		zend_argument_value_error(arg_num, "contains invalid flags (allowed flags: "
										   "LIBXML_RECOVER, "
										   "LIBXML_NOENT, "
#if LIBXML_VERSION >= 21300
										   "LIBXML_NO_XXE, "
#endif
										   "LIBXML_DTDLOAD, "
										   "LIBXML_DTDATTR, "
										   "LIBXML_DTDVALID, "
										   "LIBXML_NOERROR, "
										   "LIBXML_NOWARNING, "
										   "LIBXML_NOBLANKS, "
										   "LIBXML_XINCLUDE, "
										   "LIBXML_NSCLEAN, "
										   "LIBXML_NOCDATA, "
										   "LIBXML_NONET, "
										   "LIBXML_PEDANTIC, "
										   "LIBXML_COMPACT, "
										   "LIBXML_PARSEHUGE, "
										   "LIBXML_BIGLINES)");
		return false;
	}
	return true;
}

/* Living spec never creates explicit namespace declaration nodes.
 * They are only written upon serialization but never appear in the tree.
 * So in principle we could just ignore them outright.
 * However, step 10 in https://html.spec.whatwg.org/multipage/parsing.html#create-an-element-for-the-token (Date 2023-12-15)
 * requires us to have the declaration as an attribute available */
void dom_mark_namespaces_as_attributes_too(php_dom_libxml_ns_mapper *ns_mapper, xmlDocPtr doc)
{
	xmlNodePtr node = doc->children;
	while (node != NULL) {
		if (node->type == XML_ELEMENT_NODE) {
			php_dom_ns_compat_mark_attribute_list(ns_mapper, node);
		}

		node = php_dom_next_in_tree_order(node, NULL);
	}
}

PHP_METHOD(Dom_XMLDocument, createEmpty)
{
	const char *version = NULL;
	size_t encoding_len = strlen("UTF-8");
	const char *encoding = "UTF-8";
	size_t version_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|sp", &version, &version_len, &encoding, &encoding_len) == FAILURE) {
		RETURN_THROWS();
	}

	xmlCharEncodingHandlerPtr handler = xmlFindCharEncodingHandler(encoding);

	if (handler != NULL) {
		xmlCharEncCloseFunc(handler);
	} else {
		zend_argument_value_error(2, "is not a valid document encoding");
		RETURN_THROWS();
	}

	xmlDocPtr lxml_doc = xmlNewDoc((const xmlChar *) version);
	if (UNEXPECTED(lxml_doc == NULL)) {
		goto oom;
	}

	lxml_doc->encoding = xmlStrdup((const xmlChar *) encoding);

	dom_object *intern = php_dom_instantiate_object_helper(
		return_value,
		dom_xml_document_class_entry,
		(xmlNodePtr) lxml_doc,
		NULL
	);
	dom_set_xml_class(intern->document);
	intern->document->private_data = php_dom_libxml_private_data_header(php_dom_private_data_create());
	return;

oom:
	php_dom_throw_error(INVALID_STATE_ERR, true);
	RETURN_THROWS();
}

static void load_from_helper(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	const char *source, *override_encoding = NULL;
	size_t source_len, override_encoding_len;
	zend_long options = 0;
	if (zend_parse_parameters(
		ZEND_NUM_ARGS(),
		"s|lp!",
		&source,
		&source_len,
		&options,
		&override_encoding,
		&override_encoding_len
	) == FAILURE) {
		RETURN_THROWS();
	}

	if (!source_len) {
		zend_argument_value_error(1, "must not be empty");
		RETURN_THROWS();
	}

	if (ZEND_SIZE_T_INT_OVFL(source_len)) {
		zend_argument_value_error(1, "is too long");
		RETURN_THROWS();
	}

	/* See php_libxml_streams_IO_open_wrapper(), apparently this caused issues in the past. */
	if (mode == DOM_LOAD_FILE && strstr(source, "%00")) {
		zend_argument_value_error(1, "must not contain percent-encoded NUL bytes");
		RETURN_THROWS();
	}

	if (!check_options_validity(2, options)) {
		RETURN_THROWS();
	}

	xmlCharEncodingHandlerPtr encoding = NULL;
	if (override_encoding != NULL) {
		encoding = xmlFindCharEncodingHandler(override_encoding);
		if (!encoding) {
			zend_argument_value_error(3, "must be a valid document encoding");
			RETURN_THROWS();
		}
		options |= XML_PARSE_IGNORE_ENC;
	}

	xmlDocPtr lxml_doc = dom_document_parser(NULL, mode, source, source_len, options, encoding);
	if (UNEXPECTED(lxml_doc == NULL || lxml_doc == DOM_DOCUMENT_MALFORMED)) {
		if (!EG(exception)) {
			if (lxml_doc == DOM_DOCUMENT_MALFORMED) {
				php_dom_throw_error_with_message(SYNTAX_ERR, "XML fragment is not well-formed", true);
			} else {
				if (mode == DOM_LOAD_FILE) {
					zend_throw_exception_ex(NULL, 0, "Cannot open file '%s'", source);
				} else {
					php_dom_throw_error(INVALID_STATE_ERR, true);
				}
			}
		}
		RETURN_THROWS();
	}
	if (lxml_doc->encoding == NULL) {
		if (override_encoding) {
			lxml_doc->encoding = xmlStrdup((const xmlChar *) override_encoding);
		} else {
			lxml_doc->encoding = xmlStrdup((const xmlChar *) "UTF-8");
		}
	}
	if (mode == DOM_LOAD_FILE && lxml_doc->URL != NULL) {
		if (!php_is_stream_path((char *) lxml_doc->URL)) {
			/* Check for "file:/" instead of "file://" because of libxml2 quirk */
			if (strncmp((const char *) lxml_doc->URL, "file:/", sizeof("file:/") - 1) != 0) {
#ifdef PHP_WIN32
				xmlChar *buffer = xmlStrdup((const xmlChar *) "file:///");
#else
				xmlChar *buffer = xmlStrdup((const xmlChar *) "file://");
#endif
				if (buffer != NULL) {
					xmlChar *new_buffer = xmlStrcat(buffer, lxml_doc->URL);
					if (new_buffer != NULL) {
						xmlFree(BAD_CAST lxml_doc->URL);
						lxml_doc->URL = new_buffer;
					} else {
						xmlFree(buffer);
					}
				}
			} else {
#ifdef PHP_WIN32
				lxml_doc->URL = php_dom_libxml_fix_file_path(BAD_CAST lxml_doc->URL);
#endif
			}
		}
	}
	dom_object *intern = php_dom_instantiate_object_helper(
		return_value,
		dom_xml_document_class_entry,
		(xmlNodePtr) lxml_doc,
		NULL
	);
	dom_set_xml_class(intern->document);
	dom_document_convert_to_modern(intern->document, lxml_doc);
}

void dom_document_convert_to_modern(php_libxml_ref_obj *document, xmlDocPtr lxml_doc)
{
	php_dom_private_data *private_data = php_dom_private_data_create();
	php_dom_libxml_ns_mapper *ns_mapper = php_dom_ns_mapper_from_private(private_data);
	document->private_data = php_dom_libxml_private_data_header(private_data);
	dom_mark_namespaces_as_attributes_too(ns_mapper, lxml_doc);
}

PHP_METHOD(Dom_XMLDocument, createFromString)
{
	load_from_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_STRING);
}

PHP_METHOD(Dom_XMLDocument, createFromFile)
{
	load_from_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_FILE);
}

static int php_new_dom_write_smart_str(void *context, const char *buffer, int len)
{
	smart_str *str = context;
	smart_str_appendl(str, buffer, len);
	return len;
}

static php_dom_private_data *get_private_data_from_node(xmlNodePtr node)
{
	dom_object *intern = php_dom_object_get_data(node);
	return intern != NULL ? php_dom_get_private_data(intern) : NULL;
}

static zend_string *php_new_dom_dump_node_to_str_ex(xmlNodePtr node, int options, bool format, const char *encoding)
{
	smart_str str = {0};

	int status = -1;
	xmlSaveCtxtPtr ctxt = xmlSaveToIO(php_new_dom_write_smart_str, NULL, &str, encoding, XML_SAVE_AS_XML | options);
	if (EXPECTED(ctxt != NULL)) {
		xmlCharEncodingHandlerPtr handler = xmlFindCharEncodingHandler(encoding);
		xmlOutputBufferPtr out = xmlOutputBufferCreateIO(php_new_dom_write_smart_str, NULL, &str, handler);
		if (EXPECTED(out != NULL)) {
			status = dom_xml_serialize(ctxt, out, node, format, false, get_private_data_from_node(node));
			status |= xmlOutputBufferFlush(out);
			status |= xmlOutputBufferClose(out);
		} else {
			xmlCharEncCloseFunc(handler);
		}
		(void) xmlSaveClose(ctxt);
	}

	if (UNEXPECTED(status < 0)) {
		smart_str_free_ex(&str, false);
		return NULL;
	}

	return smart_str_extract(&str);
}

static zend_string *php_new_dom_dump_node_to_str(xmlDocPtr doc, xmlNodePtr node, bool format, const char *encoding)
{
	return php_new_dom_dump_node_to_str_ex(node, 0, format, encoding);
}

static zend_string *php_new_dom_dump_doc_to_str(xmlDocPtr doc, int options, const char *encoding)
{
	return php_new_dom_dump_node_to_str_ex((xmlNodePtr) doc, options, options & XML_SAVE_FORMAT, encoding);
}

zend_long php_new_dom_dump_node_to_file(const char *filename, xmlDocPtr doc, xmlNodePtr node, bool format, const char *encoding)
{
	xmlCharEncodingHandlerPtr handler = xmlFindCharEncodingHandler(encoding);
	xmlOutputBufferPtr out = xmlOutputBufferCreateFilename(filename, handler, 0);
	if (!out) {
		xmlCharEncCloseFunc(handler);
		return -1;
	}

	php_stream *stream = out->context;

	int status = -1;
	xmlSaveCtxtPtr ctxt = xmlSaveToIO(out->writecallback, NULL, stream, encoding, XML_SAVE_AS_XML);
	if (EXPECTED(ctxt != NULL)) {
		status = dom_xml_serialize(ctxt, out, node, format, false, get_private_data_from_node(node));
		status |= xmlOutputBufferFlush(out);
		(void) xmlSaveClose(ctxt);
	}

	size_t offset = php_stream_tell(stream);

	(void) xmlOutputBufferClose(out);

	return status < 0 ? status : (zend_long) offset;
}

static zend_long php_new_dom_dump_doc_to_file(const char *filename, xmlDocPtr doc, bool format, const char *encoding)
{
	return php_new_dom_dump_node_to_file(filename, doc, (xmlNodePtr) doc, format, encoding);
}

static const php_libxml_document_handlers php_new_dom_default_document_handlers = {
	.dump_node_to_str = php_new_dom_dump_node_to_str,
	.dump_doc_to_str = php_new_dom_dump_doc_to_str,
	.dump_node_to_file = php_new_dom_dump_node_to_file,
	.dump_doc_to_file = php_new_dom_dump_doc_to_file,
};

void dom_set_xml_class(php_libxml_ref_obj *document)
{
	document->class_type = PHP_LIBXML_CLASS_MODERN;
	document->handlers = &php_new_dom_default_document_handlers;
}

#endif  /* HAVE_LIBXML && HAVE_DOM */
