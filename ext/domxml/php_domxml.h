/* 
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Uwe Steinmann (Uwe.Steinmann@fernuni-hagen.de                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_DOMXML_H
#define PHP_DOMXML_H

#if HAVE_DOMXML
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/tree.h>
#include <libxml/xmlerror.h>
#include <libxml/xinclude.h>
#if defined(LIBXML_HTML_ENABLED)
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#endif
#if defined(LIBXML_XPATH_ENABLED)
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#endif
#if defined(LIBXML_XPTR_ENABLED)
#include <libxml/xpointer.h>
#endif
#if HAVE_DOMXSLT
#include <libxslt/xsltconfig.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/xsltutils.h>
#include <libxslt/transform.h>
#if HAVE_DOMEXSLT
#include <libexslt/exslt.h>
#include <libexslt/exsltconfig.h>
#endif
#endif

/* DOMXML API_VERSION, please bump it up, if you change anything in the API
    therefore it's easier for the script-programmers to check, what's working how
   Can be checked with phpversion("domxml");
*/
#define DOMXML_API_VERSION "20020815"

extern zend_module_entry domxml_module_entry;
#define domxml_module_ptr &domxml_module_entry

#ifdef PHP_WIN32
#ifdef PHPAPI
#undef PHPAPI
#endif
#ifdef DOMXML_EXPORTS
#define PHPAPI __declspec(dllexport)
#else
#define PHPAPI __declspec(dllimport)
#endif /* DOMXML_EXPORTS */
#endif /* PHP_WIN32 */

PHPAPI zval *php_domobject_new(xmlNodePtr obj, int *found, zval* in TSRMLS_DC);

/* directory functions */
PHP_MINIT_FUNCTION(domxml);
PHP_MSHUTDOWN_FUNCTION(domxml);
PHP_MINFO_FUNCTION(domxml);
PHP_FUNCTION(domxml_version);
PHP_FUNCTION(xmldoc);
PHP_FUNCTION(xmldocfile);
#if defined(LIBXML_HTML_ENABLED)
PHP_FUNCTION(html_doc);
PHP_FUNCTION(html_doc_file);
#endif
PHP_FUNCTION(domxml_xmltree);
PHP_FUNCTION(domxml_new_xmldoc);
PHP_FUNCTION(domxml_substitute_entities_default);

/* Class Document methods */
PHP_FUNCTION(domxml_doc_doctype);
PHP_FUNCTION(domxml_doc_implementation);
PHP_FUNCTION(domxml_doc_create_element);
PHP_FUNCTION(domxml_doc_create_element_ns);
PHP_FUNCTION(domxml_doc_create_text_node);
PHP_FUNCTION(domxml_doc_create_comment);
PHP_FUNCTION(domxml_doc_create_processing_instruction);
PHP_FUNCTION(domxml_doc_create_attribute);
PHP_FUNCTION(domxml_doc_create_cdata_section);
PHP_FUNCTION(domxml_doc_create_entity_reference);
PHP_FUNCTION(domxml_doc_imported_node);
PHP_FUNCTION(domxml_doc_add_root);
PHP_FUNCTION(domxml_doc_document_element);
PHP_FUNCTION(domxml_doc_set_root);
PHP_FUNCTION(domxml_intdtd);
PHP_FUNCTION(domxml_doc_ids);
PHP_FUNCTION(domxml_dump_mem);
PHP_FUNCTION(domxml_dump_mem_file);
PHP_FUNCTION(domxml_dump_node);
PHP_FUNCTION(domxml_doc_validate);
PHP_FUNCTION(domxml_doc_xinclude);
PHP_FUNCTION(domxml_doc_free_doc);
#if defined(LIBXML_HTML_ENABLED)
PHP_FUNCTION(domxml_html_dump_mem);
#endif

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
PHP_FUNCTION(domxml_node_append_sibling);
PHP_FUNCTION(domxml_node_append_child);
PHP_FUNCTION(domxml_node_remove_child);
PHP_FUNCTION(domxml_node_replace_child);
PHP_FUNCTION(domxml_node_has_attributes);
PHP_FUNCTION(domxml_node_has_child_nodes);
PHP_FUNCTION(domxml_node_parent);
PHP_FUNCTION(domxml_node_prefix);
PHP_FUNCTION(domxml_node_namespace_uri);
PHP_FUNCTION(domxml_node_add_namespace);
PHP_FUNCTION(domxml_node_set_namespace);
PHP_FUNCTION(domxml_node);
PHP_FUNCTION(domxml_clone_node);
PHP_FUNCTION(domxml_node_unlink_node);
PHP_FUNCTION(domxml_node_replace_node);
PHP_FUNCTION(domxml_node_new_child);
PHP_FUNCTION(domxml_node_set_content);
PHP_FUNCTION(domxml_node_get_content);
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
PHP_FUNCTION(domxml_elem_tagname);
PHP_FUNCTION(domxml_elem_get_attribute);
PHP_FUNCTION(domxml_elem_set_attribute);
PHP_FUNCTION(domxml_elem_remove_attribute);
PHP_FUNCTION(domxml_elem_get_attribute_node);
/* since this function is not really implemented, outcomment it for the time beeing
PHP_FUNCTION(domxml_elem_set_attribute_node);
*/
PHP_FUNCTION(domxml_elem_get_elements_by_tagname);
PHP_FUNCTION(domxml_elem_has_attribute);
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

/* Class Parser methods */
PHP_FUNCTION(domxml_parser);
PHP_FUNCTION(domxml_parser_add_chunk);
PHP_FUNCTION(domxml_parser_end);
PHP_FUNCTION(domxml_parser_set_keep_blanks);
PHP_FUNCTION(domxml_parser_start_element);
PHP_FUNCTION(domxml_parser_end_element);
PHP_FUNCTION(domxml_parser_characters);
PHP_FUNCTION(domxml_parser_entity_reference);
PHP_FUNCTION(domxml_parser_comment);
PHP_FUNCTION(domxml_parser_cdata_section);
PHP_FUNCTION(domxml_parser_namespace_decl);
PHP_FUNCTION(domxml_parser_processing_instruction);
PHP_FUNCTION(domxml_parser_start_document);
PHP_FUNCTION(domxml_parser_end_document);
PHP_FUNCTION(domxml_parser_get_document);

/* Class XPathContext methods */
#if defined(LIBXML_XPATH_ENABLED)
PHP_FUNCTION(xpath_init);
PHP_FUNCTION(xpath_new_context);
PHP_FUNCTION(xpath_eval);
PHP_FUNCTION(xpath_eval_expression);
PHP_FUNCTION(xpath_register_ns);
PHP_FUNCTION(domxml_doc_get_elements_by_tagname);
PHP_FUNCTION(domxml_doc_get_element_by_id);
#endif
#if defined(LIBXML_XPTR_ENABLED)
PHP_FUNCTION(xptr_new_context);
PHP_FUNCTION(xptr_eval);
#endif
PHP_FUNCTION(domxml_test);

/* DOMXSLT functions */
#if HAVE_DOMXSLT
PHP_FUNCTION(domxml_xslt_stylesheet);
PHP_FUNCTION(domxml_xslt_stylesheet_doc);
PHP_FUNCTION(domxml_xslt_stylesheet_file);
PHP_FUNCTION(domxml_xslt_process);
PHP_FUNCTION(domxml_xslt_result_dump_mem);
PHP_FUNCTION(domxml_xslt_result_dump_file);
PHP_FUNCTION(domxml_xslt_version);
#endif
typedef struct {
   zval *errors;
   xmlValidCtxtPtr valid;
   xmlParserCtxtPtr parser;
} domxml_ErrorCtxt;
#else
#define domxml_module_ptr NULL

#endif /* HAVE_DOMXML */
#define phpext_domxml_ptr domxml_module_ptr

#endif /* _PHP_DIR_H */
