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
#include "config.h"
#endif

#include "php.h"
#if defined(HAVE_LIBXML) && defined(HAVE_DOM)
#include "php_dom.h"
#include "namespace_compat.h"

static bool check_options_validity(uint32_t arg_num, zend_long options)
{
	const zend_long VALID_OPTIONS = XML_PARSE_NOENT
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
		zend_argument_value_error(2, "contains invalid flags (allowed flags: "
									 "LIBXML_NOENT, "
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
 * However, step 10 in https://html.spec.whatwg.org/multipage/parsing.html#create-an-element-for-the-token
 * requires us to have the declaration as an attribute available */
static void dom_mark_namespaces_as_attributes_too(xmlDocPtr doc)
{
	xmlNodePtr node = doc->children;
	while (node != NULL) {
		if (node->type == XML_ELEMENT_NODE) {
			dom_ns_compat_mark_attribute_list(node->nsDef);

			if (node->children) {
				node = node->children;
				continue;
			}
		}

		if (node->next) {
			node = node->next;
		} else {
			/* Go upwards, until we find a parent node with a next sibling, or until we hit the base. */
			do {
				node = node->parent;
				if (node == NULL) {
					return;
				}
			} while (node->next == NULL);
			node = node->next;
		}
	}
}

void dom_mark_namespaces_for_copy_based_on_copy(xmlNodePtr copy, const xmlNode *original)
{
	xmlNodePtr copy_current = copy;
	const xmlNode *original_current = original;
	while (copy_current != NULL) {
		ZEND_ASSERT(original_current != NULL);

		if (copy_current->type == XML_ELEMENT_NODE) {
			dom_ns_compat_copy_attribute_list_mark(copy_current->nsDef, original_current->nsDef);

			if (copy_current->children) {
				copy_current = copy_current->children;
				original_current = original_current->children;
				continue;
			}
		}

		if (copy_current->next) {
			copy_current = copy_current->next;
			original_current = original_current->next;
		} else {
			/* Go upwards, until we find a parent node with a next sibling, or until we hit the base. */
			do {
				copy_current = copy_current->parent;
				if (copy_current == NULL) {
					return;
				}
				original_current = original_current->parent;
			} while (copy_current->next == NULL);
			copy_current = copy_current->next;
			original_current = original_current->next;
		}
	}
}

PHP_METHOD(DOM_XMLDocument, createEmpty)
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
	intern->document->is_modern_api_class = true;
	return;

oom:
	php_dom_throw_error(INVALID_STATE_ERR, 1);
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
		zend_argument_value_error(1, "must not exceed INT_MAX in length");
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
	if (UNEXPECTED(lxml_doc == NULL)) {
		if (!EG(exception)) {
			if (mode == DOM_LOAD_FILE) {
				zend_throw_exception_ex(NULL, 0, "Cannot open file '%s'", source);
			} else {
				php_dom_throw_error(INVALID_STATE_ERR, 1);
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
	dom_object *intern = php_dom_instantiate_object_helper(
		return_value,
		dom_xml_document_class_entry,
		(xmlNodePtr) lxml_doc,
		NULL
	);
	intern->document->is_modern_api_class = true;
	dom_mark_namespaces_as_attributes_too(lxml_doc);
}

PHP_METHOD(DOM_XMLDocument, createFromString)
{
	load_from_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_STRING);
}

PHP_METHOD(DOM_XMLDocument, createFromFile)
{
	load_from_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_FILE);
}

#endif  /* HAVE_LIBXML && HAVE_DOM */
