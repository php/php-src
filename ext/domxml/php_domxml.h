/* 
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Uwe Steinmann (Uwe.Steinmann@fernuni-hagen.de               |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_DOMXML_H
#define PHP_DOMXML_H

#if HAVE_DOMXML
#include <libxml/parser.h>
#if defined(LIBXML_XPATH_ENABLED)
#include <libxml/xpath.h>
#endif
#if defined(LIBXML_XPTR_ENABLED)
#include <libxml/xpointer.h>
#endif

extern zend_module_entry domxml_module_entry;
#define domxml_module_ptr &domxml_module_entry

/* directory functions */
PHP_MINIT_FUNCTION(domxml);
PHP_RINIT_FUNCTION(domxml);
PHP_MINFO_FUNCTION(domxml);
PHP_FUNCTION(domxml_version);
PHP_FUNCTION(xmldoc);
PHP_FUNCTION(xmldocfile);
PHP_FUNCTION(xmltree);
PHP_FUNCTION(domxml_new_xmldoc);

/* Class Document methods */
PHP_FUNCTION(domxml_doc_doctype);
PHP_FUNCTION(domxml_doc_implementation);
PHP_FUNCTION(domxml_doc_document_element);
PHP_FUNCTION(domxml_doc_create_element);
PHP_FUNCTION(domxml_doc_create_text_node);
PHP_FUNCTION(domxml_doc_create_comment);
PHP_FUNCTION(domxml_doc_create_processing_instruction);
PHP_FUNCTION(domxml_doc_create_attribute);
PHP_FUNCTION(domxml_doc_create_cdata_section);
PHP_FUNCTION(domxml_doc_create_entity_reference);
PHP_FUNCTION(domxml_doc_imported_node);
PHP_FUNCTION(domxml_add_root);
PHP_FUNCTION(domxml_intdtd);
PHP_FUNCTION(domxml_dumpmem);

/* Class DocumentType methods */
PHP_FUNCTION(domxml_doctype_name);
PHP_FUNCTION(domxml_doctype_entities);
PHP_FUNCTION(domxml_doctype_notations);
PHP_FUNCTION(domxml_doctype_public_id);
PHP_FUNCTION(domxml_doctype_system_id);
PHP_FUNCTION(domxml_doctype_internal_subset);

/* Class Notation methods */
PHP_FUNCTION(domxml_notation_public_id);
PHP_FUNCTION(domxml_notation_system_id);

/* Class Node methods */
PHP_FUNCTION(domxml_node_attributes);
PHP_FUNCTION(domxml_node_children);
PHP_FUNCTION(domxml_node_first_child);
PHP_FUNCTION(domxml_node_last_child);
PHP_FUNCTION(domxml_node_next_sibling);
PHP_FUNCTION(domxml_node_previous_sibling);
PHP_FUNCTION(domxml_node_owner_document);
PHP_FUNCTION(domxml_node_insert_before);
PHP_FUNCTION(domxml_node_append_child);
PHP_FUNCTION(domxml_node_add_child);
PHP_FUNCTION(domxml_node_has_attributes);
PHP_FUNCTION(domxml_node_has_child_nodes);
PHP_FUNCTION(domxml_node_parent);
PHP_FUNCTION(domxml_node_prefix);
PHP_FUNCTION(domxml_node);
PHP_FUNCTION(domxml_clone_node);
PHP_FUNCTION(domxml_node_unlink_node);
PHP_FUNCTION(domxml_node_new_child);
PHP_FUNCTION(domxml_node_set_content);
PHP_FUNCTION(domxml_node_text_concat);
PHP_FUNCTION(domxml_node_set_name);
PHP_FUNCTION(domxml_node_name);
PHP_FUNCTION(domxml_node_type);
PHP_FUNCTION(domxml_node_value);
PHP_FUNCTION(domxml_is_blank_node);

/* Class Attribute methods */
PHP_FUNCTION(domxml_attr_name);
PHP_FUNCTION(domxml_attr_value);
PHP_FUNCTION(domxml_attr_specified);

/* Class Element methods */
PHP_FUNCTION(domxml_element);
PHP_FUNCTION(domxml_elem_tagname);
PHP_FUNCTION(domxml_elem_get_attribute);
PHP_FUNCTION(domxml_elem_set_attribute);
PHP_FUNCTION(domxml_elem_remove_attribute);
PHP_FUNCTION(domxml_elem_get_attribute_node);
PHP_FUNCTION(domxml_elem_set_attribute_node);
PHP_FUNCTION(domxml_elem_get_element_by_tagname);

/* Class CData methods */
PHP_FUNCTION(domxml_cdata_length);

/* Class Notation methods */
PHP_FUNCTION(domxml_notation_public_id);
PHP_FUNCTION(domxml_notation_system_id);

/* Class Entity methods */
PHP_FUNCTION(domxml_entity_public_id);
PHP_FUNCTION(domxml_entity_system_id);
PHP_FUNCTION(domxml_entity_notation_name);

/* Class ProcessingInstructions */
PHP_FUNCTION(domxml_pi_target);
PHP_FUNCTION(domxml_pi_data);

/* Class XPathContext methods */
#if defined(LIBXML_XPATH_ENABLED)
PHP_FUNCTION(xpath_init);
PHP_FUNCTION(xpath_new_context);
PHP_FUNCTION(xpath_eval);
PHP_FUNCTION(xpath_eval_expression);
#endif
#if defined(LIBXML_XPTR_ENABLED)
PHP_FUNCTION(xptr_new_context);
PHP_FUNCTION(xptr_eval);
#endif
PHP_FUNCTION(domxml_test);

/* General macros used by domxml */
#define DOMXML_DOMOBJ_NEW(zval, obj, ret)			if (NULL == (zval = php_domobject_new(obj, ret TSRMLS_CC))) { \
														php_error(E_WARNING, "%s() cannot create required DOM object", \
																  get_active_function_name(TSRMLS_C)); \
														RETURN_FALSE; \
													}

#define DOMXML_RET_ZVAL(zval)						SEPARATE_ZVAL(&zval); \
													*return_value = *zval; \
													FREE_ZVAL(zval);

#define DOMXML_RET_OBJ(zval, obj, ret)				DOMXML_DOMOBJ_NEW(zval, obj, ret); \
													DOMXML_RET_ZVAL(zval);

#define DOMXML_GET_THIS(zval)						if (NULL == (zval = getThis())) { \
														php_error(E_WARNING, "%s() underlying object missing", \
																  get_active_function_name(TSRMLS_C)); \
														RETURN_FALSE; \
													}

#define DOMXML_GET_OBJ(ret, zval, le)				if (NULL == (ret = php_dom_get_object(zval, le, 0 TSRMLS_CC))) { \
														php_error(E_WARNING, "%s() cannot fetch DOM object", \
																  get_active_function_name(TSRMLS_C)); \
														RETURN_FALSE; \
													}

#define DOMXML_GET_THIS_OBJ(ret, zval, le)			DOMXML_GET_THIS(zval); \
													DOMXML_GET_OBJ(ret, zval, le);

#define DOMXML_NO_ARGS()							if (ZEND_NUM_ARGS() != 0) { \
														php_error(E_WARNING, "%s() expects exactly 0 parameters, %d given", \
																  get_active_function_name(TSRMLS_C), ZEND_NUM_ARGS()); \
														return; \
													}

#define DOMXML_NOT_IMPLEMENTED()					php_error(E_WARNING, "%s() not yet implemented", \
															  get_active_function_name(TSRMLS_C)); \
													return;

#else
#define domxml_module_ptr NULL
#endif /* HAVE_DOMXML */
#define phpext_domxml_ptr domxml_module_ptr

#endif /* _PHP_DIR_H */
