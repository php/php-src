/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Uwe Steinmann <steinm@php.net>                               |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* TODO
 * - Support Notation Nodes
 * */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/php_rand.h"

#include "php_domxml.h"

#if HAVE_DOMXML
#include "ext/standard/info.h"
#define PHP_XPATH 1
#define PHP_XPTR 2

/* General macros used by domxml */

#define DOMXML_IS_TYPE(zval, ce)					(zval && Z_TYPE_P(zval) == IS_OBJECT && Z_OBJCE_P(zval)->refcount == ce->refcount)

#define DOMXML_DOMOBJ_NEW(zval, obj, ret)			if (NULL == (zval = php_domobject_new(obj, ret, zval TSRMLS_CC))) { \
														php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot create required DOM object"); \
														RETURN_FALSE; \
													}

#define DOMXML_RET_ZVAL(zval)						SEPARATE_ZVAL(&zval); \
													*return_value = *zval; \
													FREE_ZVAL(zval);

#define DOMXML_RET_OBJ(zval, obj, ret)				DOMXML_DOMOBJ_NEW(zval, obj, ret); \
													DOMXML_RET_ZVAL(zval);

#define DOMXML_GET_THIS(zval)						if (NULL == (zval = getThis())) { \
														php_error_docref(NULL TSRMLS_CC, E_WARNING, "Underlying object missing"); \
														RETURN_FALSE; \
													}

#define DOMXML_GET_OBJ(ret, zval, le)				if (NULL == (ret = php_dom_get_object(zval, le, 0 TSRMLS_CC))) { \
														php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot fetch DOM object"); \
														RETURN_FALSE; \
													}

#define DOMXML_GET_THIS_OBJ(ret, zval, le)			DOMXML_GET_THIS(zval); \
													DOMXML_GET_OBJ(ret, zval, le);

#define DOMXML_NO_ARGS()							if (ZEND_NUM_ARGS() != 0) { \
														php_error_docref(NULL TSRMLS_CC, E_WARNING, "Expects exactly 0 parameters, %d given", ZEND_NUM_ARGS()); \
														return; \
													}

#define DOMXML_NOT_IMPLEMENTED()					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Not yet implemented"); \
													return;

/* WARNING: The number of parameters is actually the
 * number of passed variables to zend_parse_parameters(),
 * *NOT* the number of parameters expected by the PHP function. */
#define DOMXML_PARAM_NONE(ret, zval, le)			if (NULL == (zval = getThis())) { \
														if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zval) == FAILURE) { \
															return; \
														} \
													} \
													DOMXML_GET_OBJ(ret, zval, le);

#define DOMXML_PARAM_ONE(ret, zval, le, s, p1)	if (NULL == (zval = getThis())) { \
														if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o"s, &zval, p1) == FAILURE) { \
															return; \
														} \
													} else { \
														if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, s, p1) == FAILURE) { \
															return; \
														} \
													} \
													DOMXML_GET_OBJ(ret, zval, le);

#define DOMXML_PARAM_TWO(ret, zval, le, s, p1, p2)	if (NULL == (zval = getThis())) { \
														if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o"s, &zval, p1, p2) == FAILURE) { \
															return; \
														} \
													} else { \
														if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, s, p1, p2) == FAILURE) { \
															return; \
														} \
													} \
													DOMXML_GET_OBJ(ret, zval, le);

#define DOMXML_PARAM_THREE(ret, zval, le, s, p1, p2, p3)		if (NULL == (zval = getThis())) { \
																	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o"s, &zval, p1, p2, p3) == FAILURE) { \
																		return; \
																	} \
																} else { \
																	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, s, p1, p2, p3) == FAILURE) { \
																		return; \
																	} \
																} \
																DOMXML_GET_OBJ(ret, zval, le);


#define DOMXML_PARAM_FOUR(ret, zval, le, s, p1, p2, p3, p4)		if (NULL == (zval = getThis())) { \
																	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o"s, &zval, p1, p2, p3, p4) == FAILURE) { \
																		return; \
																	} \
																} else { \
																	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, s, p1, p2, p3, p4) == FAILURE) { \
																		return; \
																	} \
																} \
																DOMXML_GET_OBJ(ret, zval, le);

#define DOMXML_PARAM_SIX(ret, zval, le, s, p1, p2, p3, p4, p5, p6)		if (NULL == (zval = getThis())) { \
																	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o"s, &zval, p1, p2, p3, p4, p5, p6) == FAILURE) { \
																		return; \
																	} \
																} else { \
																	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, s, p1, p2, p3, p4, p5, p6) == FAILURE) { \
																		return; \
																	} \
																} \
																DOMXML_GET_OBJ(ret, zval, le);

#define DOMXML_LOAD_PARSING  0
#define DOMXML_LOAD_VALIDATING 1
#define DOMXML_LOAD_RECOVERING 2
#define DOMXML_LOAD_SUBSTITUTE_ENTITIES 4
#define DOMXML_LOAD_COMPLETE_ATTRS 8
#define DOMXML_LOAD_DONT_KEEP_BLANKS 16
static int le_domxmldocp;
static int le_domxmldoctypep;
static int le_domxmldtdp;
static int le_domxmlnodep;
static int le_domxmlelementp;
static int le_domxmlattrp;
static int le_domxmlcdatap;
static int le_domxmltextp;
static int le_domxmlpip;
static int le_domxmlcommentp;
static int le_domxmlnotationp;
static int le_domxmlparserp;

/*static int le_domxmlentityp;*/
static int le_domxmlentityrefp;
/*static int le_domxmlnsp;*/


#if HAVE_DOMXSLT
static int le_domxsltstylesheetp;
#endif
static void domxml_error(void *ctx, const char *msg, ...);
static void domxml_error_ext(void *ctx, const char *msg, ...);
static void domxml_error_validate(void *ctx, const char *msg, ...);
static xmlDocPtr php_dom_xmlSAXParse(xmlSAXHandlerPtr sax, const char *buffer, int size, int recovery, void *data);

#if defined(LIBXML_XPATH_ENABLED)
static int le_xpathctxp;
static int le_xpathobjectp;
#endif

zend_class_entry *domxmldoc_class_entry;
zend_class_entry *domxmldoctype_class_entry;
zend_class_entry *domxmlelement_class_entry;
zend_class_entry *domxmldtd_class_entry;
zend_class_entry *domxmlnode_class_entry;
zend_class_entry *domxmlattr_class_entry;
zend_class_entry *domxmlcdata_class_entry;
zend_class_entry *domxmltext_class_entry;
zend_class_entry *domxmlpi_class_entry;
zend_class_entry *domxmlcomment_class_entry;
zend_class_entry *domxmlnotation_class_entry;
zend_class_entry *domxmlentity_class_entry;
zend_class_entry *domxmlentityref_class_entry;
zend_class_entry *domxmlns_class_entry;
zend_class_entry *domxmlparser_class_entry;
#if defined(LIBXML_XPATH_ENABLED)
zend_class_entry *xpathctx_class_entry;
zend_class_entry *xpathobject_class_entry;
#endif
#if HAVE_DOMXSLT
zend_class_entry *domxsltstylesheet_class_entry;
#endif


static int node_attributes(zval **attributes, xmlNode *nodep TSRMLS_DC);
static int node_children(zval **children, xmlNode *nodep TSRMLS_DC);

static unsigned char first_args_force_ref[]  = { 1, BYREF_FORCE };
static unsigned char second_args_force_ref[] = { 2, BYREF_NONE, BYREF_FORCE };
static unsigned char third_args_force_ref[]  = { 3, BYREF_NONE, BYREF_NONE, BYREF_FORCE };

static zend_function_entry domxml_functions[] = {
	PHP_FE(domxml_version,												NULL)
	PHP_FE(xmldoc,														third_args_force_ref)
	PHP_FALIAS(domxml_open_mem,				xmldoc,	third_args_force_ref)
	PHP_FE(xmldocfile,														third_args_force_ref)
	PHP_FALIAS(domxml_open_file,				xmldocfile,	third_args_force_ref)
#if defined(LIBXML_HTML_ENABLED)
	PHP_FE(html_doc,													NULL)
	PHP_FE(html_doc_file,												NULL)
#endif
	PHP_FE(domxml_xmltree,														NULL)
	PHP_FALIAS(xmltree,				domxml_xmltree,	NULL)
	PHP_FE(domxml_substitute_entities_default,														NULL)
	PHP_FE(domxml_doc_document_element,									NULL)
	PHP_FE(domxml_doc_add_root,											NULL)
	PHP_FE(domxml_doc_set_root,											NULL)
	PHP_FE(domxml_dump_mem,												NULL)
	PHP_FE(domxml_dump_mem_file,										NULL)
	PHP_FE(domxml_dump_node,											NULL)
#if defined(LIBXML_HTML_ENABLED)
	PHP_FE(domxml_html_dump_mem,										NULL)
#endif
	PHP_FE(domxml_node_attributes,										NULL)
	PHP_FE(domxml_elem_get_attribute,									NULL)
	PHP_FE(domxml_elem_set_attribute,									NULL)
	PHP_FE(domxml_node_children,										NULL)
	PHP_FE(domxml_node_has_attributes,									NULL)
	PHP_FE(domxml_node_new_child,										NULL)
	PHP_FE(domxml_node,													NULL)
	PHP_FE(domxml_node_unlink_node,										NULL)
	PHP_FE(domxml_node_set_content,										NULL)
	PHP_FE(domxml_node_get_content,										NULL)
	PHP_FE(domxml_node_add_namespace,									NULL)
	PHP_FE(domxml_node_set_namespace,									NULL)
	PHP_FE(domxml_new_xmldoc,											NULL)
	PHP_FALIAS(domxml_new_doc,				domxml_new_xmldoc,	NULL)
	PHP_FE(domxml_parser,												NULL)
	PHP_FE(domxml_parser_add_chunk,										NULL)
	PHP_FE(domxml_parser_end,											NULL)
	PHP_FE(domxml_parser_start_element,									NULL)
	PHP_FE(domxml_parser_end_element,									NULL)
	PHP_FE(domxml_parser_comment,										NULL)
	PHP_FE(domxml_parser_characters,									NULL)
	PHP_FE(domxml_parser_entity_reference,								NULL)
	PHP_FE(domxml_parser_processing_instruction,						NULL)
	PHP_FE(domxml_parser_cdata_section,									NULL)
	PHP_FE(domxml_parser_namespace_decl,								NULL)
	PHP_FE(domxml_parser_start_document,								NULL)
	PHP_FE(domxml_parser_end_document,									NULL)
	PHP_FE(domxml_parser_get_document,									NULL)

#if defined(LIBXML_XPATH_ENABLED)
	PHP_FE(xpath_new_context,											NULL)
	PHP_FE(xpath_eval,													NULL)
	PHP_FE(xpath_eval_expression,										NULL)
	PHP_FE(xpath_register_ns,											NULL)
	PHP_FE(xpath_register_ns_auto,										NULL)	
	PHP_FE(domxml_doc_get_elements_by_tagname,							NULL)
#endif

	PHP_FE(domxml_doc_get_element_by_id,								NULL)

#if defined(LIBXML_XPTR_ENABLED)
	PHP_FE(xptr_new_context,											NULL)
	PHP_FE(xptr_eval,													NULL)
#endif
#if HAVE_DOMXSLT
	PHP_FE(domxml_xslt_version,											NULL)
	PHP_FE(domxml_xslt_stylesheet,										NULL)
	PHP_FE(domxml_xslt_stylesheet_doc,									NULL)
	PHP_FE(domxml_xslt_stylesheet_file,									NULL)
	PHP_FE(domxml_xslt_process,											NULL)
	PHP_FE(domxml_xslt_result_dump_mem,							  			NULL)
	PHP_FE(domxml_xslt_result_dump_file,							  			NULL)
#endif

	PHP_FALIAS(domxml_add_root,			domxml_doc_add_root,			NULL)
	PHP_FALIAS(domxml_doc_get_root,		domxml_doc_document_element,	NULL)
	PHP_FALIAS(domxml_root,				domxml_doc_document_element,	NULL)
	PHP_FALIAS(domxml_attributes,		domxml_node_attributes,			NULL)
	PHP_FALIAS(domxml_get_attribute,	domxml_elem_get_attribute,		NULL)
	PHP_FALIAS(domxml_getattr,			domxml_elem_get_attribute,		NULL)
	PHP_FALIAS(domxml_set_attribute,	domxml_elem_set_attribute,		NULL)
	PHP_FALIAS(domxml_setattr,			domxml_elem_set_attribute,		NULL)
	PHP_FALIAS(domxml_children,			domxml_node_children,			NULL)
	PHP_FALIAS(domxml_new_child,		domxml_node_new_child,			NULL)
	PHP_FALIAS(domxml_unlink_node,		domxml_node_unlink_node,		NULL)
	PHP_FALIAS(set_content,				domxml_node_set_content,		NULL)
	PHP_FALIAS(new_xmldoc,				domxml_new_xmldoc,				NULL)
	PHP_FALIAS(domxml_dumpmem,			domxml_dump_mem,				NULL)
	PHP_FE(domxml_doc_validate,											second_args_force_ref)
	PHP_FE(domxml_doc_xinclude,											NULL)
	{NULL, NULL, NULL}
};


static function_entry php_domxmldoc_class_functions[] = {
	PHP_FALIAS(domdocument,				xmldoc,							NULL)
	PHP_FALIAS(doctype, 				domxml_doc_doctype, 			NULL)
	PHP_FALIAS(implementation,			domxml_doc_implementation,		NULL)
	PHP_FALIAS(document_element,		domxml_doc_document_element,	NULL)
	PHP_FALIAS(create_element,			domxml_doc_create_element,		NULL)
	PHP_FALIAS(create_element_ns,		domxml_doc_create_element_ns,	NULL)
	PHP_FALIAS(create_text_node,		domxml_doc_create_text_node,	NULL)
	PHP_FALIAS(create_comment,			domxml_doc_create_comment,		NULL)
	PHP_FALIAS(create_attribute,		domxml_doc_create_attribute,	NULL)
	PHP_FALIAS(create_cdata_section,	domxml_doc_create_cdata_section,	NULL)
	PHP_FALIAS(create_entity_reference,	domxml_doc_create_entity_reference,	NULL)
	PHP_FALIAS(create_processing_instruction,	domxml_doc_create_processing_instruction,	NULL)
	PHP_FALIAS(get_elements_by_tagname,	domxml_doc_get_elements_by_tagname,	NULL)
	PHP_FALIAS(get_element_by_id,		domxml_doc_get_element_by_id,	NULL)
	/* Everything below this comment is none DOM compliant */
	/* children is deprecated because it is inherited from DomNode */
/*	PHP_FALIAS(children,				domxml_node_children,			NULL) */
	PHP_FALIAS(add_root,				domxml_doc_add_root,			NULL)
	PHP_FALIAS(set_root,				domxml_doc_set_root,			NULL)
	PHP_FALIAS(get_root,				domxml_doc_document_element,	NULL)
	PHP_FALIAS(root,					domxml_doc_document_element,	NULL)
	PHP_FALIAS(imported_node,			domxml_doc_imported_node,		NULL)
	PHP_FALIAS(dtd,						domxml_intdtd,					NULL)
	PHP_FALIAS(ids,						domxml_doc_ids,					NULL)
	PHP_FALIAS(dumpmem,					domxml_dump_mem,				NULL)
	PHP_FALIAS(dump_mem,				domxml_dump_mem,				NULL)
	PHP_FALIAS(dump_mem_file,			domxml_dump_mem_file,			NULL)
	PHP_FALIAS(dump_file,				domxml_dump_mem_file,			NULL)
#if defined(LIBXML_HTML_ENABLED)
	PHP_FALIAS(html_dump_mem,			domxml_html_dump_mem,			NULL)
#endif
#if defined(LIBXML_XPATH_ENABLED)
	PHP_FALIAS(xpath_init,				xpath_init,						NULL)
	PHP_FALIAS(xpath_new_context,		xpath_new_context,				NULL)
	PHP_FALIAS(xptr_new_context,		xptr_new_context,				NULL)
#endif
	PHP_FALIAS(validate,				domxml_doc_validate,				first_args_force_ref)
	PHP_FALIAS(xinclude,				domxml_doc_xinclude,				NULL)

	{NULL, NULL, NULL}
};

static function_entry php_domxmlparser_class_functions[] = {
	PHP_FALIAS(add_chunk,				domxml_parser_add_chunk,		NULL)
	PHP_FALIAS(end,						domxml_parser_end,				NULL)
	PHP_FALIAS(set_keep_blanks,			domxml_parser_set_keep_blanks,	NULL)
	PHP_FALIAS(start_element,				domxml_parser_start_element,		NULL)
	PHP_FALIAS(end_element,				domxml_parser_end_element,		NULL)
	PHP_FALIAS(characters,				domxml_parser_characters,		NULL)
	PHP_FALIAS(entity_reference,				domxml_parser_entity_reference,		NULL)
	PHP_FALIAS(processing_instruction,				domxml_parser_processing_instruction,		NULL)
	PHP_FALIAS(cdata_section,				domxml_parser_cdata_section,		NULL)
	PHP_FALIAS(comment,				domxml_parser_comment,		NULL)
	PHP_FALIAS(namespace_decl,				domxml_parser_namespace_decl,		NULL)
	PHP_FALIAS(start_document,				domxml_parser_start_document,		NULL)
	PHP_FALIAS(end_document,				domxml_parser_end_document,		NULL)
	PHP_FALIAS(get_document,				domxml_parser_get_document,		NULL)

	{NULL, NULL, NULL}
};

static function_entry php_domxmldoctype_class_functions[] = {
	PHP_FALIAS(name,					domxml_doctype_name,			NULL)
	PHP_FALIAS(entities,				domxml_doctype_entities,		NULL)
	PHP_FALIAS(notations,				domxml_doctype_notations,		NULL)
	PHP_FALIAS(system_id,				domxml_doctype_system_id,		NULL)
	PHP_FALIAS(public_id,				domxml_doctype_public_id,		NULL)
/*
	PHP_FALIAS(internal_subset,			domxml_doctype_internal_subset,	NULL)
*/
	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmldtd_class_functions[] = {
	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmlnode_class_functions[] = {
	PHP_FALIAS(domnode,					domxml_node,					NULL)
	PHP_FALIAS(node_name,				domxml_node_name,				NULL)
	PHP_FALIAS(node_type,				domxml_node_type,				NULL)
	PHP_FALIAS(node_value,				domxml_node_value,				NULL)
	PHP_FALIAS(first_child,				domxml_node_first_child,		NULL)
	PHP_FALIAS(last_child,				domxml_node_last_child,			NULL)
	PHP_FALIAS(children,				domxml_node_children,			NULL)
	PHP_FALIAS(child_nodes,				domxml_node_children,			NULL)
	PHP_FALIAS(previous_sibling,		domxml_node_previous_sibling,	NULL)
	PHP_FALIAS(next_sibling,			domxml_node_next_sibling,		NULL)
	PHP_FALIAS(has_child_nodes,			domxml_node_has_child_nodes,	NULL)
	PHP_FALIAS(parent,					domxml_node_parent,				NULL)
	PHP_FALIAS(parent_node,				domxml_node_parent,				NULL)
	PHP_FALIAS(insert_before,			domxml_node_insert_before,		NULL)
	PHP_FALIAS(append_child,			domxml_node_append_child,		NULL)
	PHP_FALIAS(remove_child,			domxml_node_remove_child,		NULL)
	PHP_FALIAS(replace_child,			domxml_node_replace_child,		NULL)
	PHP_FALIAS(owner_document,			domxml_node_owner_document,		NULL)
	PHP_FALIAS(new_child,				domxml_node_new_child,			NULL)
	PHP_FALIAS(attributes,				domxml_node_attributes,			NULL)
	PHP_FALIAS(has_attributes,			domxml_node_has_attributes,		NULL)
	PHP_FALIAS(prefix,				domxml_node_prefix,				NULL)
	PHP_FALIAS(namespace_uri,			domxml_node_namespace_uri,				NULL)
	PHP_FALIAS(clone_node,				domxml_clone_node,				NULL)
/* Non DOM functions start here */
	PHP_FALIAS(add_namespace,			domxml_node_add_namespace,		NULL)
	PHP_FALIAS(set_namespace,			domxml_node_set_namespace,		NULL)
	PHP_FALIAS(add_child,				domxml_node_append_child,		NULL)
	PHP_FALIAS(append_sibling,			domxml_node_append_sibling,		NULL)
	PHP_FALIAS(node,					domxml_node,					NULL)
	PHP_FALIAS(unlink,					domxml_node_unlink_node,		NULL)
	PHP_FALIAS(unlink_node,				domxml_node_unlink_node,		NULL)
	PHP_FALIAS(replace_node,			domxml_node_replace_node,		NULL)
	PHP_FALIAS(set_content,				domxml_node_set_content,		NULL)
	PHP_FALIAS(get_content,				domxml_node_get_content,		NULL)
	PHP_FALIAS(text_concat,				domxml_node_text_concat,		NULL)
	PHP_FALIAS(set_name,				domxml_node_set_name,			NULL)
	PHP_FALIAS(is_blank_node,			domxml_is_blank_node,			NULL)
	PHP_FALIAS(dump_node,				domxml_dump_node,				NULL)
	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmlelement_class_functions[] = {
	PHP_FALIAS(domelement,				domxml_doc_create_element,		NULL)
	PHP_FALIAS(name,					domxml_elem_tagname,			NULL)
	PHP_FALIAS(tagname,					domxml_elem_tagname,			NULL)
	PHP_FALIAS(get_attribute,			domxml_elem_get_attribute,		NULL)
	PHP_FALIAS(set_attribute,			domxml_elem_set_attribute,		NULL)
	PHP_FALIAS(remove_attribute,		domxml_elem_remove_attribute,	NULL)
	PHP_FALIAS(get_attribute_node,		domxml_elem_get_attribute_node,	NULL)
/* since this function is not implemented, outcomment it for the time beeing
	PHP_FALIAS(set_attribute_node,		domxml_elem_set_attribute_node,	NULL)
*/
#if defined(LIBXML_XPATH_ENABLED)			
	PHP_FALIAS(get_elements_by_tagname,	domxml_elem_get_elements_by_tagname,	NULL)
#endif
	PHP_FALIAS(has_attribute,	domxml_elem_has_attribute,	NULL)
	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmlcdata_class_functions[] = {
	PHP_FALIAS(domcdata,				domxml_doc_create_cdata_section,NULL)
	PHP_FALIAS(length,					domxml_cdata_length,			NULL)
	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmltext_class_functions[] = {
	PHP_FALIAS(domtext,					domxml_doc_create_text_node,	NULL)
	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmlcomment_class_functions[] = {
	PHP_FALIAS(domcomment,				domxml_doc_create_comment,		NULL)
	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmlnotation_class_functions[] = {
	PHP_FALIAS(public_id,				domxml_notation_public_id,		NULL)
	PHP_FALIAS(system_id,				domxml_notation_system_id,		NULL)
	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmlentityref_class_functions[] = {
	PHP_FALIAS(domentityreference,		domxml_doc_create_entity_reference,	NULL)
	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmlentity_class_functions[] = {
/*
	PHP_FALIAS(public_id,				domxml_entity_public_id,		NULL)
	PHP_FALIAS(system_id,				domxml_entity_system_id,		NULL)
	PHP_FALIAS(notation_name,			domxml_entity_notation_name,	NULL)
*/
	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmlpi_class_functions[] = {
	PHP_FALIAS(domprocessinginstruction, domxml_doc_create_processing_instruction, NULL)
	PHP_FALIAS(target,					domxml_pi_target,				NULL)
	PHP_FALIAS(data,					domxml_pi_data,					NULL)
	{NULL, NULL, NULL}
};

#if defined(LIBXML_XPATH_ENABLED)
static zend_function_entry php_xpathctx_class_functions[] = {
	PHP_FALIAS(xpath_eval,				xpath_eval,						NULL)
	PHP_FALIAS(xpath_eval_expression,	xpath_eval_expression,			NULL)
	PHP_FALIAS(xpath_register_ns,		xpath_register_ns,				NULL)
	PHP_FALIAS(xpath_register_ns_auto,	xpath_register_ns_auto,				NULL)			
	{NULL, NULL, NULL}
};

static zend_function_entry php_xpathobject_class_functions[] = {
	{NULL, NULL, NULL}
};
#endif

static zend_function_entry php_domxmlattr_class_functions[] = {
	PHP_FALIAS(domattribute,			domxml_doc_create_attribute,	NULL)
	/* DOM_XML Consistent calls */
	PHP_FALIAS(node_name,				domxml_attr_name,				NULL)
	PHP_FALIAS(node_value,				domxml_attr_value,				NULL)
	PHP_FALIAS(node_specified,			domxml_attr_specified,			NULL)
	/* W3C compliant calls */
	PHP_FALIAS(name,					domxml_attr_name,				NULL)
	PHP_FALIAS(value,					domxml_attr_value,				NULL)
	PHP_FALIAS(specified,				domxml_attr_specified,			NULL)
/*
	PHP_FALIAS(owner_element,			domxml_attr_owner_element,		NULL)
*/
	{NULL, NULL, NULL}
};

static zend_function_entry php_domxmlns_class_functions[] = {
	{NULL, NULL, NULL}
};

#if HAVE_DOMXSLT
static zend_function_entry php_domxsltstylesheet_class_functions[] = {
/* TODO: maybe some more methods? */
	PHP_FALIAS(process, 				domxml_xslt_process, 			NULL)
	PHP_FALIAS(result_dump_mem, 			domxml_xslt_result_dump_mem, 		NULL)
	PHP_FALIAS(result_dump_file, 			domxml_xslt_result_dump_file, 		NULL)
	{NULL, NULL, NULL}
};
#endif

zend_module_entry domxml_module_entry = {
	STANDARD_MODULE_HEADER,
	"domxml",
	domxml_functions,
	PHP_MINIT(domxml),
	NULL,
	PHP_RINIT(domxml),
	NULL,
	PHP_MINFO(domxml),
	DOMXML_API_VERSION, /* Extension versionnumber */
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_DOMXML
ZEND_GET_MODULE(domxml)
#endif


static void dom_object_set_data(void *obj, zval *wrapper)
{
/*
	char tmp[20];
	sprintf(tmp, "%08X", obj);
	fprintf(stderr, "Adding %s to hash\n", tmp);
*/
	((xmlNodePtr) obj)->_private = wrapper;
}


static zval *dom_object_get_data(void *obj)
{
/*	char tmp[20];
	sprintf(tmp, "%08X", obj);
	fprintf(stderr, "Trying getting %s from object ...", tmp);
	if(((xmlNodePtr) obj)->_private)
		fprintf(stderr, " found\n");
	else
		fprintf(stderr, " not found\n");
*/
	return ((zval *) (((xmlNodePtr) obj)->_private));
}


static inline void node_wrapper_dtor(xmlNodePtr node)
{
	zval *wrapper;
	int refcount = 0;
	/* FIXME: type check probably unnecessary here? */
	if (!node) /* || Z_TYPE_P(node) == XML_DTD_NODE)*/
		return;

	wrapper = dom_object_get_data(node);

	if (wrapper != NULL ) {
		refcount = wrapper->refcount;
		zval_ptr_dtor(&wrapper);
		/*only set it to null, if refcount was 1 before, otherwise it has still needed references */
		if (refcount == 1) {
			dom_object_set_data(node, NULL);
		}
	}

}


static inline void attr_list_wrapper_dtor(xmlAttrPtr attr)
{
	while (attr != NULL) {
		node_wrapper_dtor((xmlNodePtr) attr);
		attr = attr->next;
	}
}


static inline void node_list_wrapper_dtor(xmlNodePtr node)
{
	while (node != NULL) {
		node_list_wrapper_dtor(node->children);
		switch (node->type) {
			/* Skip property freeing for the following types */
			case XML_ATTRIBUTE_DECL:
			case XML_DTD_NODE:
			case XML_ENTITY_DECL:
				break;
			default:
				attr_list_wrapper_dtor(node->properties);
		}
		node_wrapper_dtor(node);
		node = node->next;
	}
}

static xmlNodeSetPtr php_get_elements_by_tagname(xmlNodePtr n, xmlChar* name, xmlNodeSet *rv )
{
	xmlNodePtr cld = NULL;
	/* TODO
	   Namespace support
	 */
	if ( n != NULL && name != NULL ) {
		cld = n->children;
		while ( cld != NULL ) {
			if ( xmlStrcmp( name, cld->name ) == 0 ){
				if ( rv == NULL ) {
					rv = xmlXPathNodeSetCreate( cld ) ;
				}
				else {
					xmlXPathNodeSetAdd( rv, cld );
				}
			}
			rv = php_get_elements_by_tagname(cld, name, rv);
			cld = cld->next;
		}
	}
	return rv;
}

static void php_free_xml_doc(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	xmlDoc *doc = (xmlDoc *) rsrc->ptr;

	if (doc) {
		node_list_wrapper_dtor(doc->children);
		node_wrapper_dtor((xmlNodePtr) doc);
		xmlFreeDoc(doc);
	}
}


static void php_free_xml_node(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	xmlNodePtr node = (xmlNodePtr) rsrc->ptr;

	/* if node has no parent, it will not be freed by php_free_xml_doc, so do it here
	and for all children as well. */
	if (node->parent == NULL) {
		attr_list_wrapper_dtor(node->properties);
		node_list_wrapper_dtor(node->children);
		node_wrapper_dtor(node);
		xmlFreeNode(node);
	} else {
		node_wrapper_dtor(node);
	}

}

static void php_free_xml_attr(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	xmlNodePtr node = (xmlNodePtr) rsrc->ptr;
	if (node->parent == NULL) {
		node_wrapper_dtor(node);
		xmlFreeProp((xmlAttrPtr) node);
	} else {
		node_wrapper_dtor(node);
	}
}


#if defined(LIBXML_XPATH_ENABLED)
static void php_free_xpath_context(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	xmlXPathContextPtr ctx = (xmlXPathContextPtr) rsrc->ptr;
	if (ctx) {
		if (ctx->user) {
			zval *wrapper = ctx->user;
			zval_ptr_dtor(&wrapper);
		}
		xmlXPathFreeContext(ctx);
	}
}
#endif

static void php_free_xml_parser(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	xmlParserCtxtPtr parser = (xmlParserCtxtPtr) rsrc->ptr;

	if (parser) {
		zval *wrapper = dom_object_get_data(parser);
		zval_ptr_dtor(&wrapper);
		xmlFreeParserCtxt(parser);
	}
}


#if HAVE_DOMXSLT
static void php_free_xslt_stylesheet(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	xsltStylesheetPtr sheet = (xsltStylesheetPtr) rsrc->ptr;

	if (sheet) {
		node_wrapper_dtor((xmlNodePtr) sheet);
		xsltFreeStylesheet(sheet);
	}
}

static void xsltstylesheet_set_data(void *obj, zval *wrapper)
{
/*
	char tmp[20];
	sprintf(tmp, "%08X", obj);
	fprintf(stderr, "Adding %s to hash\n", tmp);
*/
	((xsltStylesheetPtr) obj)->_private = wrapper;
}


#ifdef HELLY_0
static zval *xsltstylesheet_get_data(void *obj)
{
/*
	char tmp[20];
	sprintf(tmp, "%08X", obj);
	fprintf(stderr, "Trying getting %s from object ...", tmp);
	if(((xmlNodePtr) obj)->_private)
		fprintf(stderr, " found\n");
	else
		fprintf(stderr, " not found\n");
*/
	return ((zval *) (((xsltStylesheetPtr) obj)->_private));
}
#endif

void *php_xsltstylesheet_get_object(zval *wrapper, int rsrc_type1, int rsrc_type2 TSRMLS_DC)
{
	void *obj;
	zval **handle;
	int type;

	if (NULL == wrapper) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "xsltstylesheet_get_object() invalid wrapper object passed");
		return NULL;
	}

	if (Z_TYPE_P(wrapper) != IS_OBJECT) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "wrapper is not an object");
		return NULL;
	}

	if (zend_hash_index_find(Z_OBJPROP_P(wrapper), 0, (void **) &handle) ==	FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Underlying object missing");
		return NULL;
	}

	obj = zend_list_find(Z_LVAL_PP(handle), &type);
	if (!obj || ((type != rsrc_type1) && (type != rsrc_type2))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Underlying object missing or of invalid type");
		return NULL;
	}

	return obj;
}

static void php_xsltstylesheet_set_object(zval *wrapper, void *obj, int rsrc_type)
{
	zval *handle, *addr;

	MAKE_STD_ZVAL(handle);
	Z_TYPE_P(handle) = IS_LONG;
	Z_LVAL_P(handle) = zend_list_insert(obj, rsrc_type);

	MAKE_STD_ZVAL(addr);
	Z_TYPE_P(addr) = IS_LONG;
	Z_LVAL_P(addr) = (int) obj;

	zend_hash_index_update(Z_OBJPROP_P(wrapper), 0, &handle, sizeof(zval *), NULL);
	zend_hash_index_update(Z_OBJPROP_P(wrapper), 1, &addr, sizeof(zval *), NULL);
	zval_add_ref(&wrapper);
	xsltstylesheet_set_data(obj, wrapper);
}
#endif  /* HAVE_DOMXSLT */


void *php_xpath_get_object(zval *wrapper, int rsrc_type1, int rsrc_type2 TSRMLS_DC)
{
	void *obj;
	zval **handle;
	int type;

	if (NULL == wrapper) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "php_xpath_get_object() invalid wrapper object passed");
		return NULL;
	}

	if (Z_TYPE_P(wrapper) != IS_OBJECT) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "wrapper is not an object");
		return NULL;
	}

	if (zend_hash_index_find(Z_OBJPROP_P(wrapper), 0, (void **) &handle) ==	FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Underlying object missing");
		return NULL;
	}

	obj = zend_list_find(Z_LVAL_PP(handle), &type);
	if (!obj || ((type != rsrc_type1) && (type != rsrc_type2))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Underlying object missing or of invalid type");
		return NULL;
	}

	return obj;
}

static zval *php_xpathobject_new(xmlXPathObjectPtr obj, int *found TSRMLS_DC)
{
	zval *wrapper;

	MAKE_STD_ZVAL(wrapper);
	object_init_ex(wrapper, xpathobject_class_entry);
	return (wrapper);
}

void *php_xpath_get_context(zval *wrapper, int rsrc_type1, int rsrc_type2 TSRMLS_DC)
{
	void *obj;
	zval **handle;
	int type;

	if (NULL == wrapper) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "php_xpath_get_context() invalid wrapper object passed");
		return NULL;
	}

	if (Z_TYPE_P(wrapper) != IS_OBJECT) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "wrapper is not an object");
		return NULL;
	}

	if (zend_hash_index_find(Z_OBJPROP_P(wrapper), 0, (void **) &handle) ==
		FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Underlying object missing");
		return NULL;
	}

	obj = zend_list_find(Z_LVAL_PP(handle), &type);
	if (!obj || ((type != rsrc_type1) && (type != rsrc_type2))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Underlying object missing or of invalid type");
		return NULL;
	}

	return obj;
}

static void xpath_context_set_data(void *obj, zval *wrapper)
{
/*
	char tmp[20];
	sprintf(tmp, "%08X", obj);
	fprintf(stderr, "Adding %s to hash\n", tmp);
*/
	((xmlXPathContextPtr) obj)->user = (void *) wrapper;
}

static zval *xpath_context_get_data(void *obj)
{
/*
	char tmp[20];
	sprintf(tmp, "%08X", obj);
	fprintf(stderr, "Trying getting %s from hash ...", tmp);
	if(((xmlXPathContextPtr) obj)->user)
		fprintf(stderr, " found\n");
	else
		fprintf(stderr, " not found\n");
*/
	return ((zval *) (((xmlXPathContextPtr) obj)->user));
}

static void php_xpath_set_context(zval *wrapper, void *obj, int rsrc_type)
{
	zval *handle, *addr;

	MAKE_STD_ZVAL(handle);
	Z_TYPE_P(handle) = IS_LONG;
	Z_LVAL_P(handle) = zend_list_insert(obj, rsrc_type);

	MAKE_STD_ZVAL(addr);
	Z_TYPE_P(addr) = IS_LONG;
	Z_LVAL_P(addr) = (int) obj;

	zend_hash_index_update(Z_OBJPROP_P(wrapper), 0, &handle, sizeof(zval *), NULL);
	zend_hash_index_update(Z_OBJPROP_P(wrapper), 1, &addr, sizeof(zval *), NULL);
	zval_add_ref(&wrapper);
	xpath_context_set_data(obj, wrapper);
}

static zval *php_xpathcontext_new(xmlXPathContextPtr obj, int *found TSRMLS_DC)
{
	zval *wrapper;
	int rsrc_type;

		*found = 0;

	if (!obj) {
		MAKE_STD_ZVAL(wrapper);
		ZVAL_NULL(wrapper);
		return wrapper;
	}

	if ((wrapper = (zval *) xpath_context_get_data((void *) obj))) {
		zval_add_ref(&wrapper);
		*found = 1;
		return wrapper;
	}

	MAKE_STD_ZVAL(wrapper);
/*
	fprintf(stderr, "Adding new XPath Context\n");
*/
	object_init_ex(wrapper, xpathctx_class_entry);
	rsrc_type = le_xpathctxp;
	php_xpath_set_context(wrapper, (void *) obj, rsrc_type);

	return (wrapper);
}

/* helper functions for xmlparser stuff */
static void xmlparser_set_data(void *obj, zval *wrapper)
{
	((xmlParserCtxtPtr) obj)->_private = wrapper;
}


static void php_xmlparser_set_object(zval *wrapper, void *obj, int rsrc_type)
{
	zval *handle, *addr;

	MAKE_STD_ZVAL(handle);
	Z_TYPE_P(handle) = IS_LONG;
	Z_LVAL_P(handle) = zend_list_insert(obj, rsrc_type);

	MAKE_STD_ZVAL(addr);
	Z_TYPE_P(addr) = IS_LONG;
	Z_LVAL_P(addr) = (int) obj;

	zend_hash_index_update(Z_OBJPROP_P(wrapper), 0, &handle, sizeof(zval *), NULL);
	zend_hash_index_update(Z_OBJPROP_P(wrapper), 1, &addr, sizeof(zval *), NULL);
	zval_add_ref(&wrapper);
	xmlparser_set_data(obj, wrapper);
}


static zval *php_xmlparser_new(xmlParserCtxtPtr obj, int *found TSRMLS_DC)
{
	zval *wrapper;
	int rsrc_type;

		*found = 0;

	if (!obj) {
		MAKE_STD_ZVAL(wrapper);
		ZVAL_NULL(wrapper);
		return wrapper;
	}

	MAKE_STD_ZVAL(wrapper);
	object_init_ex(wrapper, domxmlparser_class_entry);
	rsrc_type = le_domxmlparserp;
	php_xmlparser_set_object(wrapper, (void *) obj, rsrc_type);

	return (wrapper);
}

/* {{{ php_xmlparser_make_params()
   Translates a PHP array to a xmlparser parameters array */
static char **php_xmlparser_make_params(zval *idvars TSRMLS_DC)
{
	HashTable *parht;
	int parsize;
	zval **value;
	char *expr, *string_key = NULL;
	ulong num_key;
	char **params = NULL;
	int i = 0;

	parht = HASH_OF(idvars);
	parsize = (2 * zend_hash_num_elements(parht) + 1) * sizeof(char *);
	params = (char **)emalloc(parsize);
	memset((char *)params, 0, parsize);

	for (zend_hash_internal_pointer_reset(parht);
		zend_hash_get_current_data(parht, (void **)&value) == SUCCESS;
		zend_hash_move_forward(parht)) {

		if (zend_hash_get_current_key(parht, &string_key, &num_key, 1) != HASH_KEY_IS_STRING) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid argument or parameter array");
			return NULL;
		}
		else {
			SEPARATE_ZVAL(value);
			convert_to_string_ex(value);
			expr = Z_STRVAL_PP(value);
	
			if (expr) {
				params[i++] = string_key;
				params[i++] = expr;
			}
		}
	}

	params[i++] = NULL;

	return params;
}
/* }}} */
/* end parser stuff */

void *php_dom_get_object(zval *wrapper, int rsrc_type1, int rsrc_type2 TSRMLS_DC)
{
	void *obj;
	zval **handle;
	int type;

	if (NULL == wrapper) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "php_dom_get_object() invalid wrapper object passed");
		return NULL;
	}

	if (Z_TYPE_P(wrapper) != IS_OBJECT) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "wrapper is not an object");
		return NULL;
	}

	if (zend_hash_index_find(Z_OBJPROP_P(wrapper), 0, (void **) &handle) ==	FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Underlying object missing");
		return NULL;
	}

	obj = zend_list_find(Z_LVAL_PP(handle), &type);

/* The following test should be replaced with search in all parents */
	if (!obj) {		/* || ((type != rsrc_type1) && (type != rsrc_type2))) { */
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Underlying object missing or of invalid type");
		return NULL;
	}

	return obj;
}


static void php_dom_set_object(zval *wrapper, void *obj, int rsrc_type)
{
	zval *handle, *addr;

	MAKE_STD_ZVAL(handle);
	Z_TYPE_P(handle) = IS_LONG;
	Z_LVAL_P(handle) = zend_list_insert(obj, rsrc_type);

	MAKE_STD_ZVAL(addr);
	Z_TYPE_P(addr) = IS_LONG;
	Z_LVAL_P(addr) = (int) obj;

	zend_hash_index_update(Z_OBJPROP_P(wrapper), 0, &handle, sizeof(zval *), NULL);
	zend_hash_index_update(Z_OBJPROP_P(wrapper), 1, &addr, sizeof(zval *), NULL);
	zval_add_ref(&wrapper);
	dom_object_set_data(obj, wrapper);
}


PHPAPI zval *php_domobject_new(xmlNodePtr obj, int *found, zval *wrapper_in  TSRMLS_DC)
{
	zval *wrapper;
	char *content;
	int rsrc_type;

		*found = 0;

	if (!obj) {
		if(!wrapper_in)
		{
			MAKE_STD_ZVAL(wrapper);
		}
		else
			wrapper = wrapper_in;
		ZVAL_NULL(wrapper);
		return wrapper;
	}

	if ((wrapper = (zval *) dom_object_get_data((void *) obj))) {
		zval_add_ref(&wrapper);
		*found = 1;
		return wrapper;
	}

	if(!wrapper_in)
	{
		MAKE_STD_ZVAL(wrapper);
	}
	else
		wrapper = wrapper_in;

	switch (Z_TYPE_P(obj)) {

		case XML_ELEMENT_NODE:
		{
			xmlNodePtr nodep = obj;
			if(!wrapper_in)
				object_init_ex(wrapper, domxmlelement_class_entry);
			rsrc_type = le_domxmlelementp;
			add_property_long(wrapper, "type", Z_TYPE_P(nodep));
			add_property_stringl(wrapper, "tagname", (char *) nodep->name, strlen(nodep->name), 1);
			break;
		}

		case XML_TEXT_NODE:
		{
			xmlNodePtr nodep = obj;
			if(!wrapper_in)
				object_init_ex(wrapper, domxmltext_class_entry);
			rsrc_type = le_domxmltextp;
			content = xmlNodeGetContent(nodep);
			add_property_long(wrapper, "type", Z_TYPE_P(nodep));
			add_property_stringl(wrapper, "name", "#text", 5, 1);
			if (content)
				add_property_stringl(wrapper, "content", (char *) content, strlen(content), 1);
			xmlFree(content);
			break;
		}

		case XML_COMMENT_NODE:
		{
			xmlNodePtr nodep = obj;
			if(!wrapper_in)
				object_init_ex(wrapper, domxmlcomment_class_entry);
			rsrc_type = le_domxmlcommentp;
			content = xmlNodeGetContent(nodep);
			if (content) {
				add_property_long(wrapper, "type", Z_TYPE_P(nodep));
				add_property_stringl(wrapper, "name", "#comment", 8, 1);
				add_property_stringl(wrapper, "content", (char *) content, strlen(content), 1);
				xmlFree(content);
			}
			break;
		}

		case XML_PI_NODE:
		{
			xmlNodePtr nodep = obj;
			if(!wrapper_in)
				object_init_ex(wrapper, domxmlpi_class_entry);
			rsrc_type = le_domxmlpip;
			content = xmlNodeGetContent(nodep);
			add_property_stringl(wrapper, "name", (char *) nodep->name, strlen(nodep->name), 1);
			if (content) {
				add_property_stringl(wrapper, "value", (char *) content, strlen(content), 1);
				xmlFree(content);
			}
			break;
		}

		case XML_ENTITY_REF_NODE:
		{
			xmlNodePtr nodep = obj;
			if(!wrapper_in)
				object_init_ex(wrapper, domxmlentityref_class_entry);
			rsrc_type = le_domxmlentityrefp;
			add_property_stringl(wrapper, "name", (char *) nodep->name, strlen(nodep->name), 1);
			break;
		}

		case XML_ENTITY_DECL:
		case XML_ELEMENT_DECL:
		{
			xmlNodePtr nodep = obj;
			if(!wrapper_in)
				object_init_ex(wrapper, domxmlnode_class_entry);
			rsrc_type = le_domxmlnodep;
			add_property_long(wrapper, "type", Z_TYPE_P(nodep));
			add_property_stringl(wrapper, "name", (char *) nodep->name, strlen(nodep->name), 1);
			if (Z_TYPE_P(obj) == XML_ENTITY_REF_NODE) {
				content = xmlNodeGetContent(nodep);
				if (content) {
					add_property_stringl(wrapper, "content", (char *) content, strlen(content), 1);
					xmlFree(content);
				}
			}
			break;
		}

		case XML_ATTRIBUTE_NODE:
		{
			xmlAttrPtr attrp = (xmlAttrPtr) obj;
			if(!wrapper_in)
				object_init_ex(wrapper, domxmlattr_class_entry);
			rsrc_type = le_domxmlattrp;
			add_property_long(wrapper, "type", Z_TYPE_P(attrp));
			add_property_stringl(wrapper, "name", (char *) attrp->name, strlen(attrp->name), 1);
			content = xmlNodeGetContent((xmlNodePtr) attrp);
			if (content) {
				add_property_stringl(wrapper, "value", (char *) content, strlen(content), 1);
				xmlFree(content);
			}
			break;
		}

		case XML_DOCUMENT_NODE:
		case XML_HTML_DOCUMENT_NODE:
		{
			xmlDocPtr docp = (xmlDocPtr) obj;

			if(!wrapper_in)
				object_init_ex(wrapper, domxmldoc_class_entry);
			rsrc_type = le_domxmldocp;
			if (docp->name)
				add_property_stringl(wrapper, "name", (char *) docp->name, strlen(docp->name), 1);
			else
				add_property_stringl(wrapper, "name", "#document", 9, 1);
			if (docp->URL)
				add_property_stringl(wrapper, "url", (char *) docp->URL, strlen(docp->URL), 1);
			else
				add_property_stringl(wrapper, "url", "", 0, 1);
			if (docp->version)
				add_property_stringl(wrapper, "version", (char *) docp->version, strlen(docp->version), 1);
				else
				add_property_stringl(wrapper, "version", "", 0, 1);
			if (docp->encoding)
				add_property_stringl(wrapper, "encoding", (char *) docp->encoding, strlen(docp->encoding), 1);
			add_property_long(wrapper, "standalone", docp->standalone);
			add_property_long(wrapper, "type", Z_TYPE_P(docp));
			add_property_long(wrapper, "compression", docp->compression);
			add_property_long(wrapper, "charset", docp->charset);
			break;
		}

		/* FIXME: nodes of type XML_DTD_NODE used to be domxmldtd_class_entry.
		 * but the DOM Standard doesn't have a DomDtd class. The DocumentType
		 * class seems to be want we need and the libxml dtd functions are
		 * very much like the methods of DocumentType. I wonder what exactly
		 * is the difference between XML_DTD_NODE and XML_DOCUMENT_TYPE_NODE.
		 * Something like
		 * <!DOCTYPE chapter SYSTEM '/share/sgml/Norman_Walsh/db3xml10/db3xml10.dtd'
		 * [ <!ENTITY sp \"spanish\">
		 * ]>
		 * is considered a DTD by libxml, but from the DOM perspective it
		 * rather is a DocumentType
		 */
		case XML_DTD_NODE:
		case XML_DOCUMENT_TYPE_NODE:
		{
			xmlDtdPtr dtd = (xmlDtdPtr) obj;
			if(!wrapper_in)
				object_init_ex(wrapper, domxmldoctype_class_entry);
/*			rsrc_type = le_domxmldtdp; */
			rsrc_type = le_domxmldoctypep;
/*			add_property_long(wrapper, "type", Z_TYPE_P(dtd)); */
			add_property_long(wrapper, "type", XML_DOCUMENT_TYPE_NODE);
			if (dtd->ExternalID)
				add_property_string(wrapper, "publicId", (char *) dtd->ExternalID, 1);
			else
				add_property_string(wrapper, "publicId", "", 1);
			if (dtd->SystemID)
				add_property_string(wrapper, "systemId", (char *) dtd->SystemID, 1);
			else
				add_property_string(wrapper, "systemId", "", 1);
			if (dtd->name)
				add_property_string(wrapper, "name", (char *) dtd->name, 1);
			break;
		}

		case XML_CDATA_SECTION_NODE:
		{
			xmlNodePtr nodep = obj;
			if(!wrapper_in)
				object_init_ex(wrapper, domxmlcdata_class_entry);
			rsrc_type = le_domxmlcdatap;
			content = xmlNodeGetContent(nodep);
			add_property_long(wrapper, "type", Z_TYPE_P(nodep));
			if (content) {
				add_property_stringl(wrapper, "content", (char *) content, strlen(content), 1);
				xmlFree(content);
			}
			break;
		}

		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unsupported node type: %d\n", Z_TYPE_P(obj));
			FREE_ZVAL(wrapper);
			return NULL;
	}

	php_dom_set_object(wrapper, (void *) obj, rsrc_type);
	return (wrapper);
}

static void domxml_error(void *ctx, const char *msg, ...)
{
	char buf[1024];
	va_list ap;
	TSRMLS_FETCH();

	va_start(ap, msg);
	vsnprintf(buf, 1024, msg, ap);
	va_end(ap);
	php_error_docref(NULL TSRMLS_CC, E_WARNING, buf);
}

static void domxml_error_ext(void *ctx, const char *msg, ...)
{
	xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
	xmlParserInputPtr input = NULL;
	char buf[1024];
	va_list ap;
	TSRMLS_FETCH();

	va_start(ap, msg);
	vsnprintf(buf, 1024, msg, ap);
	va_end(ap);

	if (ctxt != NULL && ctxt->_private != NULL) {
		zval *errormessages;
	   	MAKE_STD_ZVAL(errormessages);

		if(array_init(errormessages) != SUCCESS) {
			/* do error handling here */
		} 
		add_assoc_string(errormessages,"errormessage",buf,1);				
		input = ctxt->input;
		if (ctxt->name) {
			add_assoc_string(errormessages,"nodename",ctxt->name,1); 
		}
		if (input != NULL) {
			add_assoc_long(errormessages,"line",input->line);
			add_assoc_long(errormessages,"col",input->col);
			if (input->filename != NULL) {
				add_assoc_string(errormessages,"directory",(char *) input->directory,1);				
				add_assoc_string(errormessages,"file",(char *) input->filename,1);
			}
		}
		add_next_index_zval(ctxt->_private,errormessages);
	}
   	php_error_docref(NULL TSRMLS_CC, E_WARNING, buf);
	
}

static void domxml_error_validate(void *ctx, const char *msg, ...)
{
	domxml_ErrorCtxt *ctxt ;
	char buf[1024];
	va_list ap;
	TSRMLS_FETCH();

	va_start(ap, msg);
	vsnprintf(buf, 1024, msg, ap);
	va_end(ap);
	ctxt  = (domxml_ErrorCtxt*) ctx;

   	if (ctxt != NULL && ctxt->errors != NULL) {
		zval *errormessages;
	   	MAKE_STD_ZVAL(errormessages);
		if(array_init(errormessages) != SUCCESS) {
			/* do error handling here */
		} 
		if (ctxt->parser != NULL) {
			if (ctxt->parser->name) {
				add_assoc_string(errormessages,"nodename",ctxt->parser->name,1); 
			}

			if (ctxt->parser->input != NULL) {
			add_assoc_long(errormessages,"line",ctxt->parser->input->line);
			add_assoc_long(errormessages,"col",ctxt->parser->input->col);
				if (ctxt->parser->input->filename != NULL) {
					add_assoc_string(errormessages,"directory",(char *) ctxt->parser->input->directory,1);				
					add_assoc_string(errormessages,"file",(char *) ctxt->parser->input->filename,1);
				}
			}
		}
		
		if (ctxt->valid->node != NULL)
		{
			/*php_error_docref(NULL TSRMLS_CC, E_WARNING,"Nodename %s",(char *) ctxt->valid->name);

			node = *ctxt->node;*/
		}
		add_assoc_string(errormessages,"errormessage",buf,1);			   
   		add_next_index_zval(ctxt->errors,errormessages);
	}

   	php_error_docref(NULL TSRMLS_CC, E_WARNING, buf);
	
}

xmlDocPtr php_dom_xmlSAXParse(xmlSAXHandlerPtr sax, const char *buffer, int size, int recovery, void *data) {

    xmlDocPtr ret;
    xmlParserCtxtPtr ctxt;
    domxml_ErrorCtxt errorCtxt;
    char *directory = NULL;
    
    xmlInitParser();
    /*if size == -1, we assume, it's a filename not a inmemory xml doc*/
    if (size == -1) {
		ctxt = (xmlParserCtxt *) xmlCreateFileParserCtxt( buffer);
    } else {
		ctxt = (xmlParserCtxt *) xmlCreateMemoryParserCtxt((xmlChar *) buffer, size);
    }
    if (ctxt == NULL) {
		return(NULL);
    }
	if (sax != NULL) {
		if (ctxt->sax != NULL)
			xmlFree(ctxt->sax);
		ctxt->sax = sax;
	}
	if (data!=NULL) {
		ctxt->_private=data;
	}

    /* store directory name */
	if (size == -1) {
	    if ((ctxt->directory == NULL) && (directory == NULL))
    	    directory = xmlParserGetDirectory(buffer);
	    if ((ctxt->directory == NULL) && (directory != NULL))
    	    ctxt->directory = (char *) xmlStrdup((xmlChar *) directory);
	}
 	errorCtxt.valid = &ctxt->vctxt;
	errorCtxt.errors = data;
	errorCtxt.parser = ctxt;   

	ctxt->sax->error = domxml_error_ext;
	ctxt->sax->warning = domxml_error_ext;
	ctxt->vctxt.userData= (void *) &errorCtxt;
	ctxt->vctxt.error    = (xmlValidityErrorFunc) domxml_error_validate;
	ctxt->vctxt.warning  = (xmlValidityWarningFunc) domxml_error_validate; 

	xmlParseDocument(ctxt);

	if ((ctxt->wellFormed) || recovery) {
		ret = ctxt->myDoc;
	} else {
		ret = NULL;
		xmlFreeDoc(ctxt->myDoc);
		ctxt->myDoc = NULL;
	}
    if (sax != NULL)
        ctxt->sax = NULL;

	xmlFreeParserCtxt(ctxt);
    
	return(ret);
}

PHP_RINIT_FUNCTION(domxml)
{
	return SUCCESS;
}

/* PHP_MINIT_FUNCTION(domxml)
 */
PHP_MINIT_FUNCTION(domxml)
{
	zend_class_entry ce;

	le_domxmldocp =	zend_register_list_destructors_ex(php_free_xml_doc, NULL, "domdocument", module_number);
	/* Freeing the document contains freeing the complete tree.
	   Therefore nodes, attributes etc. may not be freed seperately.
	 */
	le_domxmlnodep = zend_register_list_destructors_ex(php_free_xml_node, NULL, "domnode", module_number);
	le_domxmlcommentp = zend_register_list_destructors_ex(php_free_xml_node, NULL, "domcomment", module_number);
	le_domxmlattrp = zend_register_list_destructors_ex(php_free_xml_attr, NULL, "domattribute", module_number);
	le_domxmltextp = zend_register_list_destructors_ex(php_free_xml_node, NULL, "domtext", module_number);
	le_domxmlelementp =	zend_register_list_destructors_ex(php_free_xml_node, NULL, "domelement", module_number);
	le_domxmldtdp = zend_register_list_destructors_ex(php_free_xml_node, NULL, "domdtd", module_number);
	le_domxmlcdatap = zend_register_list_destructors_ex(php_free_xml_node, NULL, "domcdata", module_number);
	le_domxmlentityrefp = zend_register_list_destructors_ex(php_free_xml_node, NULL, "domentityref", module_number);
	le_domxmlpip = zend_register_list_destructors_ex(php_free_xml_node, NULL, "dompi", module_number);
	le_domxmlparserp =	zend_register_list_destructors_ex(php_free_xml_parser, NULL, "domparser", module_number);
	le_domxmldoctypep = zend_register_list_destructors_ex(php_free_xml_node, NULL, "domdocumenttype", module_number);
	/* Not yet initialized le_*s */
	le_domxmlnotationp  = -10003;

#if defined(LIBXML_XPATH_ENABLED)
	le_xpathctxp = zend_register_list_destructors_ex(php_free_xpath_context, NULL, "xpathcontext", module_number);
	le_xpathobjectp = zend_register_list_destructors_ex(NULL, NULL, "xpathobject", module_number);
#endif

/*	le_domxmlnsp = register_list_destructors(NULL, NULL); */

#if HAVE_DOMXSLT
	le_domxsltstylesheetp =	zend_register_list_destructors_ex(php_free_xslt_stylesheet, NULL, "xsltstylesheet", module_number);
#endif

	INIT_OVERLOADED_CLASS_ENTRY(ce, "domnode", php_domxmlnode_class_functions, NULL, NULL, NULL);
	domxmlnode_class_entry = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);

	INIT_OVERLOADED_CLASS_ENTRY(ce, "domdocument", php_domxmldoc_class_functions, NULL, NULL, NULL);
	domxmldoc_class_entry = zend_register_internal_class_ex(&ce, domxmlnode_class_entry, NULL TSRMLS_CC);

   	INIT_OVERLOADED_CLASS_ENTRY(ce, "domparser", php_domxmlparser_class_functions, NULL, NULL, NULL);
	domxmlparser_class_entry = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);

	INIT_OVERLOADED_CLASS_ENTRY(ce, "domdocumenttype", php_domxmldoctype_class_functions, NULL,	NULL, NULL);
	domxmldoctype_class_entry = zend_register_internal_class_ex(&ce, domxmlnode_class_entry, NULL TSRMLS_CC);

	INIT_OVERLOADED_CLASS_ENTRY(ce, "dtd", php_domxmldtd_class_functions, NULL, NULL, NULL);
	domxmldtd_class_entry = zend_register_internal_class_ex(&ce, domxmlnode_class_entry, NULL TSRMLS_CC);

	INIT_OVERLOADED_CLASS_ENTRY(ce, "domelement", php_domxmlelement_class_functions, NULL, NULL, NULL);
	domxmlelement_class_entry = zend_register_internal_class_ex(&ce, domxmlnode_class_entry, NULL TSRMLS_CC);

	INIT_OVERLOADED_CLASS_ENTRY(ce, "domattribute", php_domxmlattr_class_functions, NULL, NULL, NULL);
	domxmlattr_class_entry = zend_register_internal_class_ex(&ce, domxmlnode_class_entry, NULL TSRMLS_CC);

	INIT_OVERLOADED_CLASS_ENTRY(ce, "domcdata", php_domxmlcdata_class_functions, NULL, NULL, NULL);
	domxmlcdata_class_entry = zend_register_internal_class_ex(&ce, domxmlnode_class_entry, NULL TSRMLS_CC);

	INIT_OVERLOADED_CLASS_ENTRY(ce, "domtext", php_domxmltext_class_functions, NULL, NULL, NULL);
	domxmltext_class_entry = zend_register_internal_class_ex(&ce, domxmlcdata_class_entry, NULL TSRMLS_CC);

	INIT_OVERLOADED_CLASS_ENTRY(ce, "domcomment", php_domxmlcomment_class_functions, NULL, NULL, NULL);
	domxmlcomment_class_entry = zend_register_internal_class_ex(&ce, domxmlcdata_class_entry, NULL TSRMLS_CC);

	INIT_OVERLOADED_CLASS_ENTRY(ce, "domprocessinginstruction", php_domxmlpi_class_functions, NULL, NULL, NULL);
	domxmlpi_class_entry = zend_register_internal_class_ex(&ce, domxmlnode_class_entry, NULL TSRMLS_CC);

	INIT_OVERLOADED_CLASS_ENTRY(ce, "domnotation", php_domxmlnotation_class_functions, NULL, NULL, NULL);
	domxmlnotation_class_entry = zend_register_internal_class_ex(&ce, domxmlnode_class_entry, NULL TSRMLS_CC);

	INIT_OVERLOADED_CLASS_ENTRY(ce, "domentity", php_domxmlentity_class_functions, NULL, NULL, NULL);
	domxmlentity_class_entry = zend_register_internal_class_ex(&ce, domxmlnode_class_entry, NULL TSRMLS_CC);

	INIT_OVERLOADED_CLASS_ENTRY(ce, "domentityreference", php_domxmlentityref_class_functions, NULL, NULL, NULL);
	domxmlentityref_class_entry = zend_register_internal_class_ex(&ce, domxmlnode_class_entry, NULL TSRMLS_CC);

	INIT_OVERLOADED_CLASS_ENTRY(ce, "domnamespace", php_domxmlns_class_functions, NULL, NULL, NULL);
	domxmlns_class_entry = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);

#if defined(LIBXML_XPATH_ENABLED)
	INIT_OVERLOADED_CLASS_ENTRY(ce, "XPathContext", php_xpathctx_class_functions, NULL, NULL, NULL);
	xpathctx_class_entry = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);

	INIT_OVERLOADED_CLASS_ENTRY(ce, "XPathObject", php_xpathobject_class_functions, NULL, NULL, NULL);
	xpathobject_class_entry = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);
#endif

#if HAVE_DOMXSLT
	INIT_OVERLOADED_CLASS_ENTRY(ce, "XsltStylesheet", php_domxsltstylesheet_class_functions, NULL, NULL, NULL);
	domxsltstylesheet_class_entry = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);
#endif

	REGISTER_LONG_CONSTANT("XML_ELEMENT_NODE",			XML_ELEMENT_NODE,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_NODE",		XML_ATTRIBUTE_NODE,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_TEXT_NODE",				XML_TEXT_NODE,				CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_CDATA_SECTION_NODE",	XML_CDATA_SECTION_NODE,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ENTITY_REF_NODE",		XML_ENTITY_REF_NODE,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ENTITY_NODE",			XML_ENTITY_NODE,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_PI_NODE",				XML_PI_NODE,				CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_COMMENT_NODE",			XML_COMMENT_NODE,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_DOCUMENT_NODE",			XML_DOCUMENT_NODE,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_DOCUMENT_TYPE_NODE",	XML_DOCUMENT_TYPE_NODE,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_DOCUMENT_FRAG_NODE",	XML_DOCUMENT_FRAG_NODE,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_NOTATION_NODE",			XML_NOTATION_NODE,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_HTML_DOCUMENT_NODE",	XML_HTML_DOCUMENT_NODE,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_DTD_NODE",				XML_DTD_NODE,				CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ELEMENT_DECL_NODE", 	XML_ELEMENT_DECL,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_DECL_NODE",	XML_ATTRIBUTE_DECL,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ENTITY_DECL_NODE",		XML_ENTITY_DECL,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_NAMESPACE_DECL_NODE",	XML_NAMESPACE_DECL,			CONST_CS | CONST_PERSISTENT);
#ifdef XML_GLOBAL_NAMESPACE
	REGISTER_LONG_CONSTANT("XML_GLOBAL_NAMESPACE",		XML_GLOBAL_NAMESPACE,		CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("XML_LOCAL_NAMESPACE",		XML_LOCAL_NAMESPACE,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_CDATA",		XML_ATTRIBUTE_CDATA,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_ID",			XML_ATTRIBUTE_ID,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_IDREF",		XML_ATTRIBUTE_IDREF,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_IDREFS",		XML_ATTRIBUTE_IDREFS,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_ENTITY",		XML_ATTRIBUTE_ENTITIES,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_NMTOKEN",		XML_ATTRIBUTE_NMTOKEN,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_NMTOKENS",	XML_ATTRIBUTE_NMTOKENS,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_ENUMERATION",	XML_ATTRIBUTE_ENUMERATION,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_NOTATION",	XML_ATTRIBUTE_NOTATION,		CONST_CS | CONST_PERSISTENT);

#if defined(LIBXML_XPATH_ENABLED)
	REGISTER_LONG_CONSTANT("XPATH_UNDEFINED",			XPATH_UNDEFINED,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XPATH_NODESET",				XPATH_NODESET,				CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XPATH_BOOLEAN",				XPATH_BOOLEAN,				CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XPATH_NUMBER",				XPATH_NUMBER,				CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XPATH_STRING",				XPATH_STRING,				CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XPATH_POINT",				XPATH_POINT,				CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XPATH_RANGE",				XPATH_RANGE,				CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XPATH_LOCATIONSET",			XPATH_LOCATIONSET,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XPATH_USERS",				XPATH_USERS,				CONST_CS | CONST_PERSISTENT);
#endif

	REGISTER_LONG_CONSTANT("DOMXML_LOAD_PARSING",		DOMXML_LOAD_PARSING,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOMXML_LOAD_VALIDATING",	DOMXML_LOAD_VALIDATING,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOMXML_LOAD_RECOVERING",	DOMXML_LOAD_RECOVERING,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOMXML_LOAD_SUBSTITUTE_ENTITIES",	DOMXML_LOAD_SUBSTITUTE_ENTITIES,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOMXML_LOAD_COMPLETE_ATTRS",DOMXML_LOAD_COMPLETE_ATTRS,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOMXML_LOAD_DONT_KEEP_BLANKS",DOMXML_LOAD_DONT_KEEP_BLANKS,		CONST_CS | CONST_PERSISTENT);
	xmlSetGenericErrorFunc(xmlGenericErrorContext, (xmlGenericErrorFunc)domxml_error);
#if HAVE_DOMXSLT
	xsltSetGenericErrorFunc(xsltGenericErrorContext, (xmlGenericErrorFunc)domxml_error);
#if HAVE_DOMEXSLT
	exsltRegisterAll();
#endif
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ proto int domxml_test(int id)
   Unity function for testing */
PHP_FUNCTION(domxml_test)
{
	zval *id;

	if ((ZEND_NUM_ARGS() != 1) || getParameters(ht, 1, &id) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long(id);
	RETURN_LONG(Z_LVAL_P(id));
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION(domxml)
 */
PHP_MINFO_FUNCTION(domxml)
{
	/* don't know why that line was commented out in the previous version, so i left it (cmv) */
	php_info_print_table_start();
	php_info_print_table_row(2, "DOM/XML", "enabled");
	php_info_print_table_row(2, "DOM/XML API Version", DOMXML_API_VERSION);
/*	php_info_print_table_row(2, "libxml Version", LIBXML_DOTTED_VERSION); */
	php_info_print_table_row(2, "libxml Version", xmlParserVersion);
#if defined(LIBXML_HTML_ENABLED)
	php_info_print_table_row(2, "HTML Support", "enabled");
#endif
#if defined(LIBXML_XPATH_ENABLED)
	php_info_print_table_row(2, "XPath Support", "enabled");
#endif
#if defined(LIBXML_XPTR_ENABLED)
	php_info_print_table_row(2, "XPointer Support", "enabled");
#endif
#if HAVE_DOMXSLT
	{
	char buffer[128];
	int major, minor, subminor;

	php_info_print_table_row(2, "DOM/XSLT", "enabled");
/*	php_info_print_table_row(2, "libxslt Version", LIBXSLT_DOTTED_VERSION); */
	major = xsltLibxsltVersion/10000;
	minor = (xsltLibxsltVersion - major * 10000) / 100;
	subminor = (xsltLibxsltVersion - major * 10000 - minor * 100);
	snprintf(buffer, 128, "%d.%d.%d", major, minor, subminor);
	php_info_print_table_row(2, "libxslt Version", buffer);
	major = xsltLibxmlVersion/10000;
	minor = (xsltLibxmlVersion - major * 10000) / 100;
	subminor = (xsltLibxmlVersion - major * 10000 - minor * 100);
	snprintf(buffer, 128, "%d.%d.%d", major, minor, subminor);
	php_info_print_table_row(2, "libxslt compiled against libxml Version", buffer);
	}
#if HAVE_DOMEXSLT
	php_info_print_table_row(2, "DOM/EXSLT", "enabled");
	php_info_print_table_row(2, "libexslt Version", LIBEXSLT_DOTTED_VERSION);
#endif
#endif

	php_info_print_table_end();
}
/* }}} */

/* {{{ Methods of Class DomAttribute */

/* {{{ proto array domxml_attr_name(void)
   Returns list of attribute names
   Notice: domxml_node_name() does exactly the same for attribute-nodes,
           is this function here still needed, or would an alias be enough?
   */
PHP_FUNCTION(domxml_attr_name)
{
	zval *id;
	xmlAttrPtr attrp;

	DOMXML_GET_THIS_OBJ(attrp, id,le_domxmlattrp);

	DOMXML_NO_ARGS();

	RETURN_STRING((char *) (attrp->name), 1);
}
/* }}} */

/* {{{ proto array domxml_attr_value(void)
   Returns list of attribute names */
PHP_FUNCTION(domxml_attr_value)
{
	zval *id;
	xmlAttrPtr attrp;

	DOMXML_GET_THIS_OBJ(attrp, id, le_domxmlattrp);

	DOMXML_NO_ARGS();

	RETURN_STRING((char *) xmlNodeGetContent((xmlNodePtr) attrp), 1);
}
/* }}} */

/* {{{ proto array domxml_attr_specified(void)
   Returns list of attribute names */
PHP_FUNCTION(domxml_attr_specified)
{
	zval *id;
	xmlAttrPtr attrp;

	DOMXML_NOT_IMPLEMENTED();

	id = getThis();
	attrp = php_dom_get_object(id, le_domxmlattrp, 0 TSRMLS_CC);

	RETURN_TRUE;
}
/* }}} */

/* End of Methods DomAttr }}} */


/* {{{ Methods of Class DomProcessingInstruction */

/* {{{ proto array domxml_pi_target(void)
   Returns target of pi */
PHP_FUNCTION(domxml_pi_target)
{
	zval *id;
	xmlNodePtr nodep;

	DOMXML_GET_THIS_OBJ(nodep, id, le_domxmlpip);

	DOMXML_NO_ARGS();

	RETURN_STRING((char *) nodep->name, 1);
}
/* }}} */

/* {{{ proto array domxml_pi_data(void)
   Returns data of pi */
PHP_FUNCTION(domxml_pi_data)
{
	zval *id;
	xmlNodePtr nodep;

	DOMXML_GET_THIS_OBJ(nodep, id, le_domxmlpip);

	DOMXML_NO_ARGS();

	RETURN_STRING(xmlNodeGetContent(nodep), 1);
}
/* }}} */

/* End of Methods of DomProcessingInstruction }}} */


/* {{{ Methods of Class DomCData */

/* {{{ proto array domxml_cdata_length(void)
   Returns list of attribute names */
PHP_FUNCTION(domxml_cdata_length)
{
	zval *id;
	xmlNodePtr nodep;

	DOMXML_NOT_IMPLEMENTED();

	id = getThis();
	nodep = php_dom_get_object(id, le_domxmlcdatap, 0 TSRMLS_CC);

	RETURN_LONG(1);
}
/* }}} */

/* End of Methods DomCDdata }}} */


/* {{{ Methods of Class DomNode */

/* {{{ proto object domxml_node(string name)
   Creates node */
PHP_FUNCTION(domxml_node)
{
	zval *rv = NULL;
	xmlNode *node;
	int ret, name_len;
	char *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len)  == FAILURE) {
		return;
	}

	node = xmlNewNode(NULL, name);
	if (!node) {
		RETURN_FALSE;
	}

	if(DOMXML_IS_TYPE(getThis(), domxmlnode_class_entry)) {
		DOMXML_DOMOBJ_NEW(getThis(), node, &ret);
	} else {
		DOMXML_RET_OBJ(rv, node, &ret);
	}
}
/* }}} */

/* {{{ proto object domxml_node_name(void)
   Returns name of node */
PHP_FUNCTION(domxml_node_name)
{
	zval *id;
	xmlNode *n;
	int fullQName = 0;
	const char *str = NULL;

	DOMXML_PARAM_ONE(n, id, le_domxmlnodep,"|b",&fullQName);

	switch (Z_TYPE_P(n)) {
		case XML_ELEMENT_NODE:
			if (fullQName && n->ns && n->ns->prefix) {
				/* there is maybe a better way of doing this...*/
				char *tmpstr;
				tmpstr = (char*) emalloc((strlen(n->ns->prefix)+strlen(n->name))  * sizeof(char)) ;
				sprintf(tmpstr,"%s:%s", (char*) n->ns->prefix, (char*) n->name);
				str = strdup(tmpstr);
				efree(tmpstr);
			} else {
				str = n->name;
			}
			break;

		case XML_TEXT_NODE:
			str = "#text";
			break;

		case XML_ATTRIBUTE_NODE:
			str = n->name;
			break;

		case XML_CDATA_SECTION_NODE:
			str = "#cdata-section";
			break;

		case XML_ENTITY_REF_NODE:
			str = n->name;
			break;

		case XML_ENTITY_NODE:
			str = NULL;
			break;

		case XML_PI_NODE:
			str = n->name;
			break;

		case XML_COMMENT_NODE:
			str = "#comment";
			break;

		case XML_DOCUMENT_NODE:
			str = "#document";
			break;

		case XML_DOCUMENT_FRAG_NODE:
			str = "#document-fragment";
			break;

		default:
			str = NULL;
			break;
	}

	if(str != NULL) {
		RETURN_STRING((char *) str, 1);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ proto object domxml_node_value(void)
   Returns name of value */
PHP_FUNCTION(domxml_node_value)
{
	zval *id;
	xmlNode *n;
	char *str = NULL;

	DOMXML_GET_THIS_OBJ(n, id, le_domxmlnodep);

	DOMXML_NO_ARGS();

	switch (Z_TYPE_P(n)) {
		case XML_TEXT_NODE:
		case XML_COMMENT_NODE:
		case XML_CDATA_SECTION_NODE:
		case XML_PI_NODE:
			str = n->content;
			break;
		default:
			str = NULL;
			break;
	}
	if(str != NULL) {
		RETURN_STRING((char *) str, 1);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ proto bool domxml_is_blank_node(void)
   Returns true if node is blank */
PHP_FUNCTION(domxml_is_blank_node)
{
	zval *id;
	xmlNode *n;

	DOMXML_GET_THIS_OBJ(n, id, le_domxmlnodep);

	DOMXML_NO_ARGS();

	if(xmlIsBlankNode(n)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int domxml_node_type(void)
   Returns the type of the node */
PHP_FUNCTION(domxml_node_type)
{
	zval *id;
	xmlNode *node;

	DOMXML_GET_THIS_OBJ(node, id, le_domxmlnodep);

	DOMXML_NO_ARGS();

	RETURN_LONG(node->type);
}
/* }}} */

/* {{{ proto object domxml_clone_node([bool deep])
   Clones a node */
PHP_FUNCTION(domxml_clone_node)
{
	zval *rv = NULL;
	zval *id;
	xmlNode *n, *node;
	int ret, recursive = 0;;

	DOMXML_GET_THIS_OBJ(n, id, le_domxmlnodep);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &recursive) == FAILURE) {
		return;
	}

	node = xmlCopyNode(n, recursive);
	if (!node) {
		RETURN_FALSE;
	}

	DOMXML_RET_OBJ(rv, node, &ret);
}
/* }}} */

/* {{{ proto object domxml_node_first_child(void)
   Returns first child from list of children */
PHP_FUNCTION(domxml_node_first_child)
{
	zval *id, *rv = NULL;
	xmlNode *nodep, *first;
	int ret;

	DOMXML_GET_THIS_OBJ(nodep, id, le_domxmlnodep);

	DOMXML_NO_ARGS();

	first = nodep->children;
	if (!first) {
		return;
	}

	DOMXML_RET_OBJ(rv, first, &ret);
}
/* }}} */

/* {{{ proto object domxml_node_last_child(void)
   Returns last child from list of children */
PHP_FUNCTION(domxml_node_last_child)
{
	zval *id, *rv = NULL;
	xmlNode *nodep, *last;
	int ret;

	DOMXML_GET_THIS_OBJ(nodep, id, le_domxmlnodep);

	DOMXML_NO_ARGS();

	last = nodep->last;
	if (!last) {
		return;
	}

	DOMXML_RET_OBJ(rv, last, &ret);
}
/* }}} */

/* {{{ proto object domxml_node_next_sibling(void)
   Returns next child from list of children */
PHP_FUNCTION(domxml_node_next_sibling)
{
	zval *id, *rv = NULL;
	xmlNode *nodep, *first;
	int ret;

	DOMXML_GET_THIS_OBJ(nodep, id, le_domxmlnodep);

	DOMXML_NO_ARGS();

	first = nodep->next;
	if (!first) {
		rv = NULL;
		return;
	}

	DOMXML_RET_OBJ(rv, first, &ret);
}
/* }}} */

/* {{{ proto object domxml_node_previous_sibling(void)
   Returns previous child from list of children */
PHP_FUNCTION(domxml_node_previous_sibling)
{
	zval *id, *rv = NULL;
	xmlNode *nodep, *first;
	int ret;

	DOMXML_GET_THIS_OBJ(nodep, id, le_domxmlnodep);

	DOMXML_NO_ARGS();

	first = nodep->prev;
	if (!first) {
		rv = NULL;
		return;
	}

	DOMXML_RET_OBJ(rv, first, &ret);
}
/* }}} */

/* {{{ proto object domxml_node_owner_document(void)
   Returns document this node belongs to */
PHP_FUNCTION(domxml_node_owner_document)
{
	zval *id, *rv = NULL;
	xmlNode *nodep;
	xmlDocPtr docp;
	int ret;

	DOMXML_GET_THIS_OBJ(nodep, id, le_domxmlnodep);

	DOMXML_NO_ARGS();

	docp = nodep->doc;
	if (!docp) {
		RETURN_FALSE;
	}

	DOMXML_RET_OBJ(rv, (xmlNodePtr) docp, &ret);
}
/* }}} */

/* {{{ proto object domxml_node_has_child_nodes(void)
   Returns true if node has children */
PHP_FUNCTION(domxml_node_has_child_nodes)
{
	zval *id;
	xmlNode *nodep;

	DOMXML_GET_THIS_OBJ(nodep, id, le_domxmlnodep);

	DOMXML_NO_ARGS();

	if (nodep->children) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto object domxml_node_has_attributes(void)
   Returns true if node has attributes */
PHP_FUNCTION(domxml_node_has_attributes)
{
	zval *id;
	xmlNode *nodep;

	DOMXML_GET_THIS_OBJ(nodep, id, le_domxmlnodep);

	DOMXML_NO_ARGS();

	if (Z_TYPE_P(nodep) != XML_ELEMENT_NODE)
		RETURN_FALSE;

	if (nodep->properties) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string domxml_node_prefix(void)
   Returns namespace prefix of node */
PHP_FUNCTION(domxml_node_prefix)
{
	zval *id;
	xmlNode *nodep;
	xmlNsPtr ns;

	DOMXML_GET_THIS_OBJ(nodep, id, le_domxmlnodep);

	DOMXML_NO_ARGS();

	ns = nodep->ns;
	if (!ns) {
		RETURN_EMPTY_STRING();
	}

	if (ns->prefix) {
		RETURN_STRING((char *) (ns->prefix), 1);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ proto string domxml_node_namespace_uri(void)
   Returns namespace uri of node */
PHP_FUNCTION(domxml_node_namespace_uri)
{
	zval *id;
	xmlNode *nodep;
	xmlNsPtr ns;

	DOMXML_GET_THIS_OBJ(nodep, id, le_domxmlnodep);

	DOMXML_NO_ARGS();

	ns = nodep->ns;
	if (!ns) {
		/* return NULL if no ns is given...*/
		return;
	}

	if (ns->href) {
		RETURN_STRING((char *) (ns->href), 1);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */


/* {{{ proto object domxml_node_parent(void)
   Returns parent of node */
PHP_FUNCTION(domxml_node_parent)
{
	zval *id, *rv = NULL;
	xmlNode *nodep, *last;
	int ret;

	DOMXML_GET_THIS_OBJ(nodep, id, le_domxmlnodep);

	DOMXML_NO_ARGS();

	last = nodep->parent;
	if (!last) {
		return;
	}

	DOMXML_RET_OBJ(rv, last, &ret);
}
/* }}} */

/* {{{ proto array domxml_node_children(void)
   Returns list of children nodes */
PHP_FUNCTION(domxml_node_children)
{
	zval *id;
	xmlNode *nodep, *last;
	int ret;

	DOMXML_PARAM_NONE(nodep, id, le_domxmlnodep);

	/* Even if the nodep is a XML_DOCUMENT_NODE the type is at the
	   same position.
	 */
	if ((Z_TYPE_P(nodep) == XML_DOCUMENT_NODE) || (Z_TYPE_P(nodep) == XML_HTML_DOCUMENT_NODE))
		last = ((xmlDoc *) nodep)->children;
	else
		last = nodep->children;

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	
	if (last) {
		while (last) {
			zval *child;
			child = php_domobject_new(last, &ret, NULL TSRMLS_CC);
			add_next_index_zval(return_value, child);
			last = last->next;
		}
	}
}
/* }}} */

/* {{{ proto void domxml_node_unlink_node([object node])
   Deletes the node from tree, but not from memory*/
PHP_FUNCTION(domxml_node_unlink_node)
{
	zval *id;
	xmlNode *nodep;

	DOMXML_PARAM_NONE(nodep, id, le_domxmlnodep);

	xmlUnlinkNode(nodep);
	/*	This causes a Segmentation Fault for some reason.  Removing
		it allows the user to re-add the node at some other time, in
		addition to fixing the segfault.  Node will be freed at
		shutdown. */
	/*xmlFreeNode(nodep);
	zval_dtor(id);*/			/* This is not enough because the children won't be deleted */
}
/* }}} */

/* {{{ proto object domxml_node_replace_node(object domnode)
   Replaces one node with another node */
PHP_FUNCTION(domxml_node_replace_node)
{
	zval *id, *rv = NULL, *node;
	xmlNodePtr repnode, nodep, old_repnode;
	int ret;

	DOMXML_GET_THIS_OBJ(nodep, id, le_domxmlnodep);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &node) == FAILURE) {
		return;
	}

	DOMXML_GET_OBJ(repnode, node, le_domxmlnodep);

	old_repnode = xmlReplaceNode(nodep, repnode);

	DOMXML_RET_OBJ(rv, old_repnode, &ret);
}
/* }}} */

/* {{{ proto object domxml_node_append_child(object domnode)
   Adds node to list of children */
PHP_FUNCTION(domxml_node_append_child)
{
	zval *id, *rv = NULL, *node;
	xmlNodePtr child, parent, new_child = NULL;
	int ret;

	DOMXML_PARAM_ONE(parent, id, le_domxmlnodep, "o", &node);
	
	DOMXML_GET_OBJ(child, node, le_domxmlnodep);

	if (child->type == XML_ATTRIBUTE_NODE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't append attribute node");
		RETURN_FALSE;
	}
	
	if (!(child->doc == NULL || child->doc == parent->doc)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't append node, which is in a different document than the parent node");
		RETURN_FALSE;
	}
	
	/* first unlink node, if child is already a child of parent */
	if (child->parent == parent){
		xmlUnlinkNode(child);
	}
	
	/*
	 * The following code is from libxml2/tree.c and a fix for bug #20209
	 * libxml does free textnodes, if there are adjacent TEXT nodes
	 * This is bad behaviour for domxml, since then we have have reference
	 * to undefined nodes. The idea here is, that we do this text comparison
	 * by ourself and not free the nodes. and only if libxml2 won't do any harm
	 * call the function from libxml2.
	 * The code is exactly the same as in libxml2, only xmlFreeNode was taken away.
	 */

	if (child->type == XML_TEXT_NODE) {
		if ((parent->type == XML_TEXT_NODE) &&
			(parent->content != NULL)) {
			xmlNodeAddContent(parent, child->content);
			new_child = parent;
		}
		if ((parent->last != NULL) && (parent->last->type == XML_TEXT_NODE) &&
			(parent->last->name == child->name)) {
			xmlNodeAddContent(parent->last, child->content);
			new_child = parent->last;
		}
	}
	/* end libxml2 code */
	
	if (NULL == new_child) {
		new_child = xmlAddChild(parent, child);
	}

	if (NULL == new_child) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't append node");
		RETURN_FALSE;
	}

	DOMXML_RET_OBJ(rv, new_child, &ret);
}
/* }}} */

/* {{{ proto object domxml_node_append_sibling(object domnode)
   Adds node to list of siblings */
PHP_FUNCTION(domxml_node_append_sibling)
{
	zval *id, *rv = NULL, *node;
	xmlNodePtr child, nodep, new_child;
	int ret;

	DOMXML_GET_THIS_OBJ(nodep, id, le_domxmlnodep);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &node) == FAILURE) {
		return;
	}

	DOMXML_GET_OBJ(child, node, le_domxmlnodep);

	if (child->type == XML_ATTRIBUTE_NODE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't append attribute node");
		RETURN_FALSE;
	}

	if (NULL == (new_child = xmlCopyNode(child, 1))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to clone node");
		RETURN_FALSE;
	}

	/* FIXME reverted xmlAddChildList; crashes */
	child = xmlAddSibling(nodep, new_child);

	if (NULL == child) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't append node");
		RETURN_FALSE;
	}

	DOMXML_RET_OBJ(rv, child, &ret);
}
/* }}} */

/* {{{ proto object domxml_node_insert_before(object newnode, object refnode)
   Adds node in list of nodes before given node */
PHP_FUNCTION(domxml_node_insert_before)
{
	zval *id, *rv = NULL, *node, *ref;
	xmlNodePtr child, new_child, parent, refp;
	int ret;

	DOMXML_PARAM_TWO(parent, id, le_domxmlnodep, "oo!", &node, &ref);

	DOMXML_GET_OBJ(child, node, le_domxmlnodep);

	if (ref != NULL) {
		DOMXML_GET_OBJ(refp, ref, le_domxmlnodep);
		new_child = xmlAddPrevSibling(refp, child);
	} else {
		/* first unlink node, if child is already a child of parent
			for some strange reason, this is needed
		 */
		if (child->parent == parent){
			xmlUnlinkNode(child);
		}
		new_child = xmlAddChild(parent, child);
	}
		

	if (NULL == new_child) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't add newnode as the previous sibling of refnode");
		RETURN_FALSE;
	}

	DOMXML_RET_OBJ(rv, new_child, &ret);
}
/* }}} */

/* {{{ proto object domxml_node_remove_child(object domnode)
   Removes node from list of children */
PHP_FUNCTION(domxml_node_remove_child)
{
	zval *id, *node;
	xmlNodePtr children, child, nodep;
	int ret;

	DOMXML_GET_THIS_OBJ(nodep, id, le_domxmlnodep);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &node) == FAILURE) {
		return;
	}

	DOMXML_GET_OBJ(child, node, le_domxmlnodep);

	children = nodep->children;
	if (!children) {
		RETURN_FALSE;
	}

	while (children) {
		if (children == child) {
			zval *rv = NULL;
			xmlUnlinkNode(child);
			DOMXML_RET_OBJ(rv, child, &ret);
			return;
		}
		children = children->next;
	}
	RETURN_FALSE
}
/* }}} */

/* {{{ proto object domxml_node_replace_child(object newnode, object oldnode)
   Replaces node in list of children */
PHP_FUNCTION(domxml_node_replace_child)
{
	zval *id, *newnode, *oldnode;
	xmlNodePtr children, newchild, oldchild, nodep;
	int foundoldchild = 0;
	int ret;

	DOMXML_GET_THIS_OBJ(nodep, id, le_domxmlnodep);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "oo", &newnode, &oldnode) == FAILURE) {
		return;
	}

	DOMXML_GET_OBJ(newchild, newnode, le_domxmlnodep);
	DOMXML_GET_OBJ(oldchild, oldnode, le_domxmlnodep);

	children = nodep->children;
	if (!children) {
		RETURN_FALSE;
	}

	/* check for the old child and wether the new child is already a child */
	while (children) {
		if (children == oldchild) {
			foundoldchild = 1;
		}
		children = children->next;
	}
	/* if the child to replace is existent and the new child isn't already
	 * a child, then do the replacement
	 */
	if (foundoldchild) {
		zval *rv = NULL;
		xmlNodePtr node;
		node = xmlReplaceNode(oldchild, newchild);
		DOMXML_RET_OBJ(rv, oldchild, &ret);
		return;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool domxml_node_set_name(string name)
   Sets name of a node */
PHP_FUNCTION(domxml_node_set_name)
{
	zval *id;
	xmlNode *nodep;
	int name_len;
	char *name;

	DOMXML_GET_THIS_OBJ(nodep, id, le_domxmlnodep);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
		return;
	}

	xmlNodeSetName(nodep, name);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto array domxml_node_attributes(void)
   Returns list of attributes of node */
PHP_FUNCTION(domxml_node_attributes)
{
	zval *id, *attrs;
	xmlNode *nodep;
	int ret;

	DOMXML_PARAM_NONE(nodep, id, le_domxmlnodep);
	ret = node_attributes(&attrs, nodep TSRMLS_CC);
	if ( ret == -1) {
		RETURN_NULL();
	}

	if ( ret > -1) {
		*return_value = *attrs;
		FREE_ZVAL(attrs);
	} 
}
/* }}} */

/* {{{ proto object domxml_node_new_child(string name, string content)
   Adds child node to parent node */
PHP_FUNCTION(domxml_node_new_child)
{
	zval *id, *rv = NULL;
	xmlNodePtr child, nodep;
	int ret, name_len, content_len;
	char *name, *content = NULL;

	DOMXML_PARAM_FOUR(nodep, id, le_domxmlnodep, "s|s", &name, &name_len, &content, &content_len);

	child = xmlNewChild(nodep, NULL, name, content);

	if (!child) {
		RETURN_FALSE;
	}

	DOMXML_RET_OBJ(rv, child, &ret);
}
/* }}} */

/* {{{ proto bool domxml_node_set_content(string content)
   Sets content of a node */
PHP_FUNCTION(domxml_node_set_content)
{
	zval *id;
	xmlNode *nodep;
	int content_len;
	char *content;

	DOMXML_PARAM_TWO(nodep, id, le_domxmlnodep, "s", &content, &content_len);

	/* FIXME: another gotcha. If node has children, calling
	 * xmlNodeSetContent will remove the children -> we loose the zval's
	 * To prevent crash, append content if children are set
	 */
	if (nodep->children) {
		xmlNodeAddContentLen(nodep, content, content_len);
	} else {
		xmlNodeSetContentLen(nodep, content, content_len);
	}

	/* FIXME: Actually the property 'content' of the node has to be updated
	   as well. Since 'content' should disappear sooner or later and being
	   replaces by a function 'content()' I skip this for now
	 */
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string domxml_node_get_content()
   Gets content of a node.

   "Read the value of a node, this can be either the text carried directly by
this node if it's a TEXT node or the aggregate string of the values carried by
this node child's (TEXT and ENTITY_REF). Entity references are substituted."
   */
PHP_FUNCTION(domxml_node_get_content)
{
	zval *id;
	xmlNode *nodep;
	xmlChar *mem;

	DOMXML_PARAM_NONE(nodep, id, le_domxmlnodep);
	mem = xmlNodeGetContent(nodep);
	if (!mem) {
		RETURN_FALSE;
	}

	RETVAL_STRING(mem,1);
	xmlFree(mem);
}
/* }}} */


/* End of Methods DomNode }}} */


/* {{{ Methods of Class DomNotation */

/* {{{ proto string domxml_notation_public_id(void)
   Returns public id of notation node */
PHP_FUNCTION(domxml_notation_public_id)
{
	zval *id;
	xmlNotationPtr nodep;

	DOMXML_GET_THIS_OBJ(nodep, id, le_domxmlnotationp);

	DOMXML_NO_ARGS();

	if(nodep->PublicID) {
		RETURN_STRING((char *) (nodep->PublicID), 1);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ proto string domxml_notation_system_id(void)
   Returns system ID of notation node */
PHP_FUNCTION(domxml_notation_system_id)
{
	zval *id;
	xmlNotationPtr nodep;

	DOMXML_GET_THIS_OBJ(nodep, id, le_domxmlnotationp);

	DOMXML_NO_ARGS();

	if(nodep->SystemID) {
		RETURN_STRING((char *) (nodep->SystemID), 1);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* End of Methods DomNotation }}} */


/* {{{ Methods of Class DomElement */

/* {{{ proto object domxml_element(string name)
   Constructor of DomElement */
PHP_FUNCTION(domxml_element)
{
	zval *rv = NULL;
	xmlNode *node;
	int ret, name_len;
	char *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
		return;
	}

	node = xmlNewNode(NULL, name);
	if (!node) {
		RETURN_FALSE;
	}

	DOMXML_RET_OBJ(rv, node, &ret);
}

/* }}} */

/* {{{ proto string domxml_elem_tagname(void)
   Returns tag name of element node */
PHP_FUNCTION(domxml_elem_tagname)
{
	zval *id;
	xmlNode *nodep;

	DOMXML_NO_ARGS();

	DOMXML_GET_THIS_OBJ(nodep, id, le_domxmlelementp);

	DOMXML_NO_ARGS();

	RETURN_STRING((char *) (nodep->name), 1);
}
/* }}} */

/* {{{ proto string domxml_elem_get_attribute(string attrname)
   Returns value of given attribute */
PHP_FUNCTION(domxml_elem_get_attribute)
{
	zval *id;
	xmlNode *nodep;
	char *name, *value;
	int name_len;

	DOMXML_PARAM_TWO(nodep, id, le_domxmlelementp, "s", &name, &name_len);

	value = xmlGetProp(nodep, name);
	if (!value) {
		RETURN_EMPTY_STRING();
	} else {
		RETVAL_STRING(value, 1);
		xmlFree(value);
	}
}
/* }}} */

/* {{{ proto bool domxml_elem_set_attribute(string attrname, string value)
   Sets value of given attribute */
PHP_FUNCTION(domxml_elem_set_attribute)
{
	zval *id, *rv = NULL;
	xmlNode *nodep;
	xmlAttr *attr;
	int ret, name_len, value_len;
	char *name, *value;

	DOMXML_PARAM_FOUR(nodep, id, le_domxmlelementp, "ss", &name, &name_len, &value, &value_len);

	attr = xmlSetProp(nodep, name, value);
	if (!attr) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No such attribute '%s'", name);
		RETURN_FALSE;
	}

	DOMXML_RET_OBJ(rv, (xmlNodePtr) attr, &ret);
}
/* }}} */

/* {{{ proto string domxml_elem_remove_attribute(string attrname)
   Removes given attribute */
PHP_FUNCTION(domxml_elem_remove_attribute)
{
	zval *id;
	xmlNode *nodep;
	xmlAttr *attrp;
	int name_len;
	char *name;

	DOMXML_PARAM_TWO(nodep, id, le_domxmlelementp, "s", &name, &name_len);
	attrp = xmlHasProp(nodep,name);
	if (attrp == NULL) {
		RETURN_FALSE;
	}
	xmlUnlinkNode((xmlNodePtr)attrp);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string domxml_elem_get_attribute_node(string attrname)
   Returns value of given attribute */
PHP_FUNCTION(domxml_elem_get_attribute_node)
{
	zval *id, *rv = NULL;
	xmlNode *nodep;
	xmlAttr  *attrp;
	int name_len, ret;
	char *name;

	DOMXML_PARAM_TWO(nodep, id, le_domxmlelementp, "s", &name, &name_len);
	attrp = xmlHasProp(nodep,name);
	if (attrp == NULL) {
		RETURN_FALSE;
	}
	DOMXML_RET_OBJ(rv, (xmlNodePtr) attrp, &ret);
}
/* }}} */

/* {{{ proto bool domxml_elem_set_attribute_node(object attr)
   Sets value of given attribute */
/* since this function is not implemented, outcomment it for the time beeing
PHP_FUNCTION(domxml_elem_set_attribute_node)
{
	zval *id, *arg1, *rv = NULL;
	xmlNode *nodep;
	xmlAttr *attrp, *newattrp;
	int ret;

	if ((ZEND_NUM_ARGS() == 1) && getParameters(ht, 1, &arg1) == SUCCESS) {
		id = getThis();
		nodep = php_dom_get_object(id, le_domxmlelementp, 0 TSRMLS_CC);
		attrp = php_dom_get_object(arg1, le_domxmlattrp, 0 TSRMLS_CC);
	} else {
		WRONG_PARAM_COUNT;
	}

	FIXME: The following line doesn't work 
	newattrp = xmlCopyProp(nodep, attrp);
	if (!newattrp) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No such attribute '%s'", attrp->name);
		RETURN_FALSE;
	}

	DOMXML_RET_OBJ(rv, (xmlNodePtr) newattrp, &ret);
}
*/
/* }}} */

/* {{{ proto string domxml_elem_has_attribute(string attrname)
   Checks for existenz given attribute */
PHP_FUNCTION(domxml_elem_has_attribute)
{
	zval *id;
	xmlNode *nodep;
	char *name, *value;
	int name_len;

	DOMXML_PARAM_TWO(nodep, id, le_domxmlelementp, "s", &name, &name_len);

	value = xmlGetProp(nodep, name);
	if (!value) {
		RETURN_FALSE;
	} else {
		xmlFree(value);
		RETURN_TRUE;
	}
}
/* }}} */

#if defined(LIBXML_XPATH_ENABLED)
/* {{{ proto string domxml_doc_get_elements_by_tagname(string tagname [,object xpathctx_handle] )
   Returns array with nodes with given tagname in document or empty array, if not found*/
PHP_FUNCTION(domxml_doc_get_elements_by_tagname)
{
	zval *id, *rv, *contextnode = NULL,*ctxpin = NULL;
	xmlXPathContextPtr ctxp;
	xmlDocPtr docp;

	xmlXPathObjectPtr xpathobjp;
	xmlNode *contextnodep;
	int name_len;
	char *str,*name;

	contextnode = NULL;
	contextnodep = NULL;

	DOMXML_PARAM_FOUR(docp, id, le_domxmldocp, "s|oo", &name, &name_len,&ctxpin,&contextnodep);

	/* if no xpath_context was submitted, create a new one */
	if (ctxpin == NULL) {
		ctxp = xmlXPathNewContext(docp);
	} else {
		DOMXML_GET_OBJ(ctxp, ctxpin, le_xpathctxp);
	}

	if (contextnode) {
		DOMXML_GET_OBJ(contextnodep, contextnode, le_domxmlnodep);
	}
	ctxp->node = contextnodep;
	str = (char*) emalloc((name_len+3) * sizeof(char)) ;
	if (str == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot allocate memory for string");
	}
	sprintf(str ,"//%s",name);

	xpathobjp = xmlXPathEval(str, ctxp);
	efree(str);
	ctxp->node = NULL;
	if (!xpathobjp) {
		RETURN_FALSE;
	}
	MAKE_STD_ZVAL(rv);

	if(array_init(rv) != SUCCESS)
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot create required array");
		RETURN_FALSE;
	}

	switch (Z_TYPE_P(xpathobjp)) {

		case XPATH_NODESET:
		{
			int i;
			xmlNodeSetPtr nodesetp;

			if (NULL == (nodesetp = xpathobjp->nodesetval)) {
				zval_dtor(rv);
				RETURN_FALSE;
			}

			for (i = 0; i < nodesetp->nodeNr; i++) {
				xmlNodePtr node = nodesetp->nodeTab[i];
				zval *child;
				int retnode;

				/* construct a node object */
				child = php_domobject_new(node, &retnode, NULL TSRMLS_CC);
				zend_hash_next_index_insert(Z_ARRVAL_P(rv), &child, sizeof(zval *), NULL);
			}

			break;
		}
		default:
			break;
	}

	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */
#endif

typedef struct _idsIterator idsIterator;
struct _idsIterator {
	  xmlChar *elementId;
	    xmlNode *element;
};

static void idsHashScanner(void *payload, void *data, xmlChar *name) {
	idsIterator *priv = (idsIterator *)data;

	if (priv->element == NULL && xmlStrEqual (name, priv->elementId))
		priv->element = ((xmlNode *)((xmlID *)payload)->attr)->parent;
}

/* {{{ proto string domxml_doc_get_element_by_id(string id)
   Returns element for given id or false if not found */
PHP_FUNCTION(domxml_doc_get_element_by_id)
{
	zval *id, *rv = NULL;
	xmlDocPtr docp;
	idsIterator iter;
	xmlHashTable *ids = NULL;
	int retnode,idname_len;
	char *idname;
	
	DOMXML_PARAM_TWO(docp, id, le_domxmldocp, "s", &idname, &idname_len);

	ids = (xmlHashTable *) docp->ids;
	if(ids) {
		iter.elementId = (xmlChar *) idname;
		iter.element = NULL;
		xmlHashScan(ids, (void *)idsHashScanner, &iter);
		rv = php_domobject_new(iter.element, &retnode, NULL TSRMLS_CC);
		SEPARATE_ZVAL(&rv);
		*return_value = *rv;
		FREE_ZVAL(rv);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string domxml_elem_get_elements_by_tagname(string tagname)
   Returns array with nodes with given tagname in element or empty array, if not found */
PHP_FUNCTION(domxml_elem_get_elements_by_tagname)
{
	zval *id,*rv;
	xmlNode *nodep;
	int name_len,i;
	char *name;
	xmlNodeSet *nodesetp = NULL;

	DOMXML_PARAM_TWO(nodep, id, le_domxmlelementp, "s", &name, &name_len);

	MAKE_STD_ZVAL(rv);

	if(array_init(rv) != SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot create required array");
		RETURN_FALSE;
	}

	nodesetp = php_get_elements_by_tagname(nodep, name, NULL);

	if(nodesetp) {
		for (i = 0; i < nodesetp->nodeNr; i++) {
			xmlNodePtr node = nodesetp->nodeTab[i];
			zval *child;
			int retnode;

			child = php_domobject_new(node, &retnode, NULL TSRMLS_CC);
			zend_hash_next_index_insert(Z_ARRVAL_P(rv), &child, sizeof(zval *), NULL);
		}
	}
	*return_value = *rv;
	FREE_ZVAL(rv);

}
/* }}} */

/* End of Methods DomElement }}} */


/* {{{ Methods of Class DomDocumentType */

/* {{{ proto array domxml_doctype_name(void)
   Returns name of DocumentType */
PHP_FUNCTION(domxml_doctype_name)
{
	zval *id;
	xmlDtdPtr attrp;

	DOMXML_NO_ARGS();

	DOMXML_GET_THIS_OBJ(attrp, id, le_domxmldoctypep);

	RETURN_STRING((char *) (attrp->name), 1);
}
/* }}} */

/* {{{ proto array domxml_doctype_system_id(void)
   Returns system id of DocumentType */
PHP_FUNCTION(domxml_doctype_system_id)
{
	zval *id;
	xmlDtdPtr attrp;

	DOMXML_NO_ARGS();

	DOMXML_GET_THIS_OBJ(attrp, id, le_domxmldoctypep);

	if(attrp->SystemID) {
		RETURN_STRING((char *) (attrp->SystemID), 1);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ proto array domxml_doctype_public_id(void)
   Returns public id of DocumentType */
PHP_FUNCTION(domxml_doctype_public_id)
{
	zval *id;
	xmlDtdPtr attrp;

	DOMXML_NO_ARGS();

	DOMXML_GET_THIS_OBJ(attrp, id, le_domxmldoctypep);

	if(attrp->ExternalID) {
		RETURN_STRING((char *) (attrp->ExternalID), 1);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ proto array domxml_doctype_entities(void)
   Returns list of entities */
PHP_FUNCTION(domxml_doctype_entities)
{
	zval *id;
	xmlNode *last;
	xmlDtdPtr doctypep;
	int ret;

	DOMXML_NOT_IMPLEMENTED();

	DOMXML_PARAM_NONE(doctypep, id, le_domxmldoctypep);

	last = doctypep->entities;
	if (!last) {
		RETURN_FALSE;
	}

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	while (last) {
		zval *child;
		child = php_domobject_new(last, &ret, NULL TSRMLS_CC);
		add_next_index_zval(return_value, child);
		last = last->next;
	}
}
/* }}} */

/* {{{ proto array domxml_doctype_notations(void)
   Returns list of notations */
PHP_FUNCTION(domxml_doctype_notations)
{
	zval *id;
	xmlNode *last;
	xmlDtdPtr doctypep;
	int ret;

	DOMXML_NOT_IMPLEMENTED();

	DOMXML_PARAM_NONE(doctypep, id, le_domxmldoctypep);

	last = doctypep->notations;
	if (!last) {
		RETURN_FALSE;
	}

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	while (last) {
		zval *child;
		child = php_domobject_new(last, &ret, NULL TSRMLS_CC);
		add_next_index_zval(return_value, child);
		last = last->next;
	}
}
/* }}} */

/* End of Methods DomElementType }}} */


/* {{{ Methods of Class DomDocument */

/* {{{ proto object domxml_doc_doctype(void)
   Returns DomDocumentType */
PHP_FUNCTION(domxml_doc_doctype)
{
	zval *id, *rv = NULL;
	xmlDtdPtr dtd;
	xmlDocPtr docp;
	int ret;

	DOMXML_GET_THIS_OBJ(docp, id, le_domxmldocp);

	DOMXML_NO_ARGS();

	dtd = xmlGetIntSubset(docp);
	if (!dtd) {
		RETURN_FALSE;
	}

	DOMXML_RET_OBJ(rv, (xmlNodePtr) dtd, &ret);
}
/* }}} */

/* {{{ proto object domxml_doc_implementation(void)
   Returns DomeDOMImplementation */
PHP_FUNCTION(domxml_doc_implementation)
{
/*	zval *id;
	xmlDocPtr docp;*/

	DOMXML_NOT_IMPLEMENTED();

/*
	DOMXML_GET_THIS_OBJ(docp, id, le_domxmldocp);

	rv = php_domobject_new(node, &ret TSRMLS_CC);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
*/
}
/* }}} */

/* {{{ proto object domxml_doc_document_element(int domnode)
   Returns root node of document */
PHP_FUNCTION(domxml_doc_document_element)
{
	zval *id, *rv = NULL;
	xmlDoc *docp;
	xmlNode *root;
	int ret;

	DOMXML_PARAM_NONE(docp, id, le_domxmldocp);

	root = xmlDocGetRootElement(docp);
	if (!root) {
		RETURN_FALSE;
	}

	DOMXML_RET_OBJ(rv, root, &ret);
}
/* }}} */

/* {{{ proto object domxml_doc_create_element(string name)
   Creates new element node */
PHP_FUNCTION(domxml_doc_create_element)
{
	zval *id, *rv = NULL;
	xmlNode *node;
	xmlDocPtr docp = NULL;
	int ret, name_len;
	char *name;

	if(!DOMXML_IS_TYPE(getThis(), domxmlelement_class_entry)) {
		DOMXML_GET_THIS_OBJ(docp, id, le_domxmldocp);
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
		return;
	}

	node = xmlNewNode(NULL, name);
	if (!node) {
		RETURN_FALSE;
	}
	node->doc = docp;

	if(DOMXML_IS_TYPE(getThis(), domxmlelement_class_entry)) {
		DOMXML_DOMOBJ_NEW(getThis(), node, &ret);
	} else {
		DOMXML_RET_OBJ(rv, node, &ret);
	}
}
/* }}} */

/* {{{ proto object domxml_doc_create_element_ns(string uri, string name [, string prefix])
   Creates new element node with a namespace */
PHP_FUNCTION(domxml_doc_create_element_ns)
{
	zval *id, *rv = NULL;
	xmlNode *node;
	xmlNs	*nsptr;
	xmlDocPtr docp = NULL;
	int ret, name_len, uri_len, prefix_len=0;
	char *name, *uri, *prefix;

	DOMXML_PARAM_SIX(docp, id, le_domxmldocp, "ss|s", &uri, &uri_len, &name, &name_len, &prefix, &prefix_len);

	nsptr = xmlSearchNsByHref(docp, xmlDocGetRootElement(docp), (xmlChar*) uri);
	node = xmlNewNode(nsptr, name);

	if (!node) {
		RETURN_FALSE;
	}
	/* if no namespace with the same uri was found, we have to create a new one.
	     I do this here with "a" + a random number. this is not very sophisticated,
		 therefore if someone has a better idea in creating unique prefixes, here's your
		 chance (a0,a1, etc would be good enough, this is the way mozilla does it). I'm
		 to lazy right now to think of a better solution... */
	
	if (nsptr == NULL) {
		/* if there was a prefix provided, take that, otherwise generate a new one
			 this is not w3c-like, since the have no option to provide a prefix, but
			 i don't care :)
		*/
		if (prefix_len == 0)
		{	
			char prefixtmp[20];
			int random;
			random = (int) (10000.0*php_rand(TSRMLS_C)/(PHP_RAND_MAX));
			sprintf(prefixtmp, "a%d", random);
			prefix = prefixtmp;
		}
		nsptr = xmlNewNs(node, uri, prefix);
		xmlSetNs(node, nsptr);
	}
	
	node->doc = docp;

	if(DOMXML_IS_TYPE(getThis(), domxmlelement_class_entry)) {
		DOMXML_DOMOBJ_NEW(getThis(), node, &ret);
	} else {
		DOMXML_RET_OBJ(rv, node, &ret);
	}
}
/* }}} */

/* {{{ proto bool domxml_node_add_namespace(string uri, string prefix)
   Adds a namespace declaration to a node */
PHP_FUNCTION(domxml_node_add_namespace)
{
	zval *id;
	xmlNode *nodep;
	xmlNs	*nsptr;
	int prefix_len, uri_len;
	char *prefix, *uri;

	DOMXML_PARAM_FOUR(nodep, id, le_domxmldocp, "ss", &uri, &uri_len, &prefix, &prefix_len);

	if (NULL == (nsptr = xmlNewNs(nodep,uri,prefix))) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
	
}
/* }}} */

/* {{{ proto void domxml_node_set_namespace(string uri [, string prefix])
   Sets the namespace of a node */
PHP_FUNCTION(domxml_node_set_namespace)
{
	zval *id;
	xmlNode *nodep;
	xmlNs	*nsptr;
	int prefix_len = 0, uri_len;
	char *prefix, *uri;

	DOMXML_PARAM_FOUR(nodep, id, le_domxmldocp, "s|s", &uri, &uri_len, &prefix, &prefix_len);

	/* if node is in a document, search for an already existing namespace */
	if (nodep->doc != NULL) {
		nsptr = xmlSearchNsByHref(nodep->doc, nodep, (xmlChar*) uri);
	} else {
		nsptr = NULL;
	}

	/* if no namespace decleration was found in the parents of the node, generate one */
	if (nsptr == NULL) {
		/* if there was a prefix provided, take that, otherwise generate a new one */
		if (prefix_len == 0) {	
			char prefixtmp[20];
			int random;
			random = (int) (10000.0*php_rand(TSRMLS_C)/(PHP_RAND_MAX));
			sprintf(prefixtmp, "a%d", random);
			prefix = prefixtmp;
		}
		nsptr = xmlNewNs(nodep, uri, prefix);
	}
	
	xmlSetNs(nodep, nsptr);
}
/* }}} */

/* {{{ proto object domxml_doc_create_text_node(string content)
   Creates new text node */
PHP_FUNCTION(domxml_doc_create_text_node)
{
	zval *id, *rv = NULL;
	xmlNode *node;
	xmlDocPtr docp = NULL;
	int ret, content_len;
	char *content;

	if(!DOMXML_IS_TYPE(getThis(), domxmltext_class_entry)) {
		DOMXML_GET_THIS_OBJ(docp, id, le_domxmldocp);
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &content, &content_len) == FAILURE) {
		return;
	}

	node = xmlNewTextLen(content, content_len);
	if (!node) {
		RETURN_FALSE;
	}
	node->doc = docp;

	if(DOMXML_IS_TYPE(getThis(), domxmltext_class_entry)) {
		DOMXML_DOMOBJ_NEW(getThis(), node, &ret);
	} else {
		DOMXML_RET_OBJ(rv, node, &ret);
	}
}
/* }}} */

/* {{{ proto object domxml_doc_create_comment(string content)
   Creates new comment node */
PHP_FUNCTION(domxml_doc_create_comment)
{
	zval *id, *rv = NULL;
	xmlNode *node;
	xmlDocPtr docp = NULL;
	int ret, content_len;
	char *content;

	if(!DOMXML_IS_TYPE(getThis(), domxmlcomment_class_entry)) {
		DOMXML_GET_THIS_OBJ(docp, id, le_domxmldocp);
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &content, &content_len) == FAILURE) {
		return;
	}

	node = xmlNewComment(content);
	if (!node) {
		RETURN_FALSE;
	}
	node->doc = docp;

	if(DOMXML_IS_TYPE(getThis(), domxmlcomment_class_entry)) {
		DOMXML_DOMOBJ_NEW(getThis(), node, &ret);
	} else {
		DOMXML_RET_OBJ(rv, node, &ret);
	}
}
/* }}} */

/* {{{ proto object domxml_doc_create_attribute(string name, string value)
   Creates new attribute node */
PHP_FUNCTION(domxml_doc_create_attribute)
{
	zval *id, *rv = NULL;
	xmlAttrPtr node;
	xmlDocPtr docp = NULL;
	int ret, name_len, value_len;
	char *name, *value;

	if(!DOMXML_IS_TYPE(getThis(), domxmlattr_class_entry)) {
		DOMXML_GET_THIS_OBJ(docp, id, le_domxmldocp);
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name, &name_len, &value, &value_len) == FAILURE) {
		return;
	}

	node = xmlNewProp(NULL, name, value);
	if (!node) {
		RETURN_FALSE;
	}
	node->doc = docp;

	if(DOMXML_IS_TYPE(getThis(), domxmlattr_class_entry)) {
		DOMXML_DOMOBJ_NEW(getThis(), (xmlNodePtr) node, &ret);
	} else {
		DOMXML_RET_OBJ(rv, (xmlNodePtr) node, &ret);
	}
}
/* }}} */

/* {{{ proto object domxml_doc_create_cdata_section(string content)
   Creates new cdata node */
PHP_FUNCTION(domxml_doc_create_cdata_section)
{
	zval *id, *rv = NULL;
	xmlNode *node;
	xmlDocPtr docp = NULL;
	int ret, content_len;
	char *content;

	if(!DOMXML_IS_TYPE(getThis(), domxmlcdata_class_entry)) {
		DOMXML_GET_THIS_OBJ(docp, id, le_domxmldocp);
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &content, &content_len) == FAILURE) {
		return;
	}

	node = xmlNewCDataBlock(docp, content, content_len);
	if (!node) {
		RETURN_FALSE;
	}
	node->doc = docp;

	if(DOMXML_IS_TYPE(getThis(), domxmlcdata_class_entry)) {
		DOMXML_DOMOBJ_NEW(getThis(), node, &ret);
	} else {
		DOMXML_RET_OBJ(rv, node, &ret);
	}
}
/* }}} */

/* {{{ proto object domxml_doc_create_entity_reference(string name)
   Creates new cdata node */
PHP_FUNCTION(domxml_doc_create_entity_reference)
{
	zval *id, *rv = NULL;
	xmlNode *node;
	xmlDocPtr docp = NULL;
	int ret, name_len;
	char *name;

	if(!DOMXML_IS_TYPE(getThis(), domxmlentityref_class_entry)) {
		DOMXML_GET_THIS_OBJ(docp, id, le_domxmldocp);
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
		return;
	}
	node = xmlNewReference(docp, name);
	if (!node) {
		RETURN_FALSE;
	}
	node->doc = docp;

	if(DOMXML_IS_TYPE(getThis(), domxmlentityref_class_entry)) {
		DOMXML_DOMOBJ_NEW(getThis(), node, &ret);
	} else {
		DOMXML_RET_OBJ(rv, node, &ret);
	}
}
/* }}} */

/* {{{ proto object domxml_doc_create_processing_instruction(string name)
   Creates new processing_instruction node */
PHP_FUNCTION(domxml_doc_create_processing_instruction)
{
	zval *id, *rv = NULL;
	xmlNode *node;
	xmlDocPtr docp = NULL;
	int ret, name_len, content_len;
	char *name, *content;

	if(!DOMXML_IS_TYPE(getThis(), domxmlpi_class_entry)) {
		DOMXML_GET_THIS_OBJ(docp, id, le_domxmldocp);
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name, &name_len, &content, &content_len) == FAILURE) {
		return;
	}

	node = xmlNewPI(name, content);
	if (!node) {
		RETURN_FALSE;
	}
	node->doc = docp;

	if(DOMXML_IS_TYPE(getThis(), domxmlpi_class_entry)) {
		DOMXML_DOMOBJ_NEW(getThis(), node, &ret);
	} else {
		DOMXML_RET_OBJ(rv, node, &ret);
	}
}
/* }}} */

/* {{{ proto object domxml_doc_imported_node(object node, bool recursive)
   Creates new element node */
PHP_FUNCTION(domxml_doc_imported_node)
{
	zval *arg1, *id, *rv = NULL;
	xmlNodePtr node, srcnode;
	xmlDocPtr docp;
	int ret, recursive = 0;

	DOMXML_GET_THIS_OBJ(docp, id, le_domxmldocp);

	/* FIXME: which object type to expect? */
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o|l", &arg1, &recursive) == FAILURE) {
		return;
	}

	DOMXML_GET_OBJ(srcnode, arg1, le_domxmlnodep);

	node = xmlCopyNode(srcnode, recursive);
	if (!node) {
		RETURN_FALSE;
	}
	node->doc = docp;			/* Not enough because other nodes in the tree are not set */

	DOMXML_RET_OBJ(rv, node, &ret);
}
/* }}} */

/* {{{ proto object domxml_dtd(void)
   Returns DTD of document */
PHP_FUNCTION(domxml_intdtd)
{
	zval *id, *rv = NULL;
	xmlDoc *docp;
	xmlDtd *dtd;
	int ret;

	DOMXML_GET_THIS_OBJ(docp, id, le_domxmldocp);

	dtd = xmlGetIntSubset(docp);
	if (!dtd) {
		RETURN_FALSE;
	}

	DOMXML_RET_OBJ(rv, (xmlNodePtr) dtd, &ret);
}
/* }}} */

/* {{{ proto string domxml_dump_mem(object doc_handle [, int format][, encoding])
   Dumps document into string and optionally formats it */
PHP_FUNCTION(domxml_dump_mem)
{
	zval *id;
	xmlDoc *docp;
	xmlChar *mem;
	int format = 0;
	int size, keepblanks;
	int encoding_len = 0;
	char *encoding;


	DOMXML_PARAM_THREE(docp, id, le_domxmldocp, "|ls", &format, &encoding, &encoding_len);
	if (format) {
		keepblanks = xmlKeepBlanksDefault(0);
		if (encoding_len) {
			xmlDocDumpFormatMemoryEnc(docp, &mem, &size, encoding, format);
		} else {
			xmlDocDumpFormatMemory(docp, &mem, &size, format);
		}
		xmlKeepBlanksDefault(keepblanks);
	} else {
		if (encoding_len) {
			xmlDocDumpMemoryEnc(docp, &mem, &size, encoding);
		} else {
			xmlDocDumpMemory(docp, &mem, &size);
		}
	}

	if (!size) {
		RETURN_FALSE;
	}
	RETVAL_STRINGL(mem, size, 1);
	xmlFree(mem);
}
/* }}} */

/* {{{ proto int domxml_dump_mem_file(string filename [, int compressmode [, int format]])
   Dumps document into file and uses compression if specified. Returns false on error, otherwise the length of the xml-document (uncompressed) */
PHP_FUNCTION(domxml_dump_mem_file)
{
	zval *id;
	xmlDoc *docp;
	int file_len, bytes, keepblanks;
	int format = 0;
	int compressmode = 0;
	char *file;

	DOMXML_PARAM_FOUR(docp, id, le_domxmldocp, "s|ll", &file, &file_len, &compressmode, &format);

	xmlSetCompressMode(compressmode);

	if (format) {
		keepblanks = xmlKeepBlanksDefault(0);
		bytes = xmlSaveFormatFile(file, docp, format);
		xmlKeepBlanksDefault(keepblanks);
	} else {
		bytes = xmlSaveFile(file, docp);
	}

	if (bytes == -1) {
		RETURN_FALSE;
	}
	RETURN_LONG(bytes);
}
/* }}} */

/* {{{ proto string domxml_dump_node(object doc_handle, object node_handle [, int format [, int level]])
   Dumps node into string */
PHP_FUNCTION(domxml_dump_node)
{
	zval *id, *nodep;
	xmlDocPtr docp;
	xmlNodePtr elementp;
	xmlChar *mem ;
	xmlBufferPtr buf;
	int level = 0;
	int format = 0;

	DOMXML_PARAM_THREE(docp, id, le_domxmldocp, "o|ll", &nodep, &format, &level);

	DOMXML_GET_OBJ(elementp, nodep, le_domxmlnodep);

	if (Z_TYPE_P(elementp) == XML_DOCUMENT_NODE || Z_TYPE_P(elementp) == XML_HTML_DOCUMENT_NODE ) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot dump element with a document node");
		RETURN_FALSE;
	}

	buf = xmlBufferCreate();
	if (!buf) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not fetch buffer");
		RETURN_FALSE;
	}

	xmlNodeDump(buf, docp, elementp, level, format);

	mem = (xmlChar*) xmlBufferContent(buf);

	if (!mem) {
		xmlBufferFree(buf);
		RETURN_FALSE;
	}
	RETVAL_STRING(mem,  1);

	xmlBufferFree(buf);

}
/* }}} */

/* {{{ idsHashScanner2(void *payload, void *data, xmlChar *name)
 */
static void idsHashScanner2(void *payload, void *data, xmlChar *name)
{
	zval *return_value = (zval *) data;
	zval *child;
	int ret;
	xmlNode *nodep;

	TSRMLS_FETCH();

	nodep = ((xmlNode *)((xmlID *)payload)->attr)->parent;
	child = php_domobject_new(nodep, &ret, NULL TSRMLS_CC);
	add_next_index_zval(return_value, child);
}
/* }}} */

/* {{{ proto string domxml_doc_ids(object doc_handle)
   Returns array of ids */
PHP_FUNCTION(domxml_doc_ids)
{
	zval *id;
	xmlDoc *docp;
	xmlHashTable *ids = NULL;

	DOMXML_GET_THIS_OBJ(docp, id, le_domxmldocp);

	ids = docp->ids;

	if(ids) {
		if (array_init(return_value) == FAILURE) {
			RETURN_FALSE;
		}

		xmlHashScan(ids, (void *)idsHashScanner2, return_value);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto object xmldoc(string xmldoc[, int mode[, array error]])
   Creates DOM object of XML document */
PHP_FUNCTION(xmldoc)
{
	zval *rv = NULL;
	xmlDoc *docp = NULL;
	int ret; 
	char *buffer;
	int buffer_len;
	int mode = 0, prevSubstValue;
	int oldvalue =  xmlDoValidityCheckingDefaultValue;
	int oldvalue_keepblanks;
 	int prevLoadExtDtdValue = xmlLoadExtDtdDefaultValue;
	zval *errors ;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|lz", &buffer, &buffer_len, &mode, &errors) == FAILURE) {
		return;
	}
/*	Either of the following line force validation */
/*	xmlLoadExtDtdDefaultValue = XML_DETECT_IDS; */
/*	xmlDoValidityCheckingDefaultValue = 1; */
	if (ZEND_NUM_ARGS() == 3 ) {
		zval_dtor(errors);
		array_init(errors);
	}
	 
	if (mode & DOMXML_LOAD_DONT_KEEP_BLANKS) 
		oldvalue_keepblanks  =  xmlKeepBlanksDefault(0);
	else 
		oldvalue_keepblanks  =  xmlKeepBlanksDefault(1);

	if(mode & DOMXML_LOAD_SUBSTITUTE_ENTITIES)
		prevSubstValue = xmlSubstituteEntitiesDefault (1);
	else
		prevSubstValue = xmlSubstituteEntitiesDefault (0);

	if(mode & DOMXML_LOAD_COMPLETE_ATTRS)
		xmlLoadExtDtdDefaultValue |= XML_COMPLETE_ATTRS;

	switch (mode & (DOMXML_LOAD_PARSING | DOMXML_LOAD_VALIDATING | DOMXML_LOAD_RECOVERING)) {
		case DOMXML_LOAD_PARSING:
			xmlDoValidityCheckingDefaultValue = 0;
			if (ZEND_NUM_ARGS() == 3) {
				docp = php_dom_xmlSAXParse( NULL, (char *) buffer, buffer_len, 0 , errors);
			} else {
				docp = xmlParseDoc(buffer);
			}
			break;
		case DOMXML_LOAD_VALIDATING:
			xmlDoValidityCheckingDefaultValue = 1;
			if (ZEND_NUM_ARGS() == 3) {
				docp = php_dom_xmlSAXParse(NULL, (char *) buffer, buffer_len, 0, errors);
			} else {
				docp = xmlParseDoc(buffer);
			}
			break;
		case DOMXML_LOAD_RECOVERING:
			xmlDoValidityCheckingDefaultValue = 0;
			if (ZEND_NUM_ARGS() == 3) {
				docp = php_dom_xmlSAXParse(NULL, (char *) buffer, buffer_len, 1, errors);
			} else {
				docp = xmlRecoverDoc(buffer);
			}
			break;
	}
	xmlSubstituteEntitiesDefault (prevSubstValue);
	xmlDoValidityCheckingDefaultValue = oldvalue;
	xmlLoadExtDtdDefaultValue = prevLoadExtDtdValue;
	xmlKeepBlanksDefault(oldvalue_keepblanks);

	if (!docp)
		RETURN_FALSE;

/*	dtd = xmlGetIntSubset(docp);
	if(dtd) {
		xmlParseDTD(dtd->ExternalID, dtd->SystemID);
	}
*/

	if(DOMXML_IS_TYPE(getThis(), domxmldoc_class_entry)) {
		DOMXML_DOMOBJ_NEW(getThis(), (xmlNodePtr) docp, &ret);
	} else {
		DOMXML_RET_OBJ(rv, (xmlNodePtr) docp, &ret);
	}
}
/* }}} */

/* {{{ proto object xmldocfile(string filename[, int mode[, array error])
   Creates DOM object of XML document in file */
PHP_FUNCTION(xmldocfile)
{
	zval *rv = NULL;
	xmlDoc *docp = NULL;
	int ret, file_len;
	char *file;
	int mode = 0, prevSubstValue;
	int oldvalue =  xmlDoValidityCheckingDefaultValue;
	int oldvalue_keepblanks;
	zval *errors = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|lz", &file, &file_len, &mode,  &errors) == FAILURE) {
		return;
	}
			
	if (ZEND_NUM_ARGS() == 3 ) {
		zval_dtor(errors);
		array_init(errors);
	}
	 
	if (mode & DOMXML_LOAD_DONT_KEEP_BLANKS) 
		oldvalue_keepblanks  =  xmlKeepBlanksDefault(0);
	else 
		oldvalue_keepblanks  =  xmlKeepBlanksDefault(1);

	if(mode & DOMXML_LOAD_SUBSTITUTE_ENTITIES)
		prevSubstValue = xmlSubstituteEntitiesDefault (1);
	else
		prevSubstValue = xmlSubstituteEntitiesDefault (0);

	if(mode & DOMXML_LOAD_COMPLETE_ATTRS)
		xmlLoadExtDtdDefaultValue |= XML_COMPLETE_ATTRS;

	switch (mode & (DOMXML_LOAD_PARSING | DOMXML_LOAD_VALIDATING | DOMXML_LOAD_RECOVERING)) {
		case DOMXML_LOAD_PARSING:
			xmlDoValidityCheckingDefaultValue = 0;
			if (ZEND_NUM_ARGS() == 3) {
				docp = php_dom_xmlSAXParse( NULL, (char *) file, -1 , 0 , errors);
			} else {
				docp = xmlParseFile(file);
			}
			break;
		case DOMXML_LOAD_VALIDATING:
			xmlDoValidityCheckingDefaultValue = 1;
			if (ZEND_NUM_ARGS() == 3) {
				docp = php_dom_xmlSAXParse(NULL, (char *) file, -1, 0, errors);
			} else {
				docp = xmlParseFile(file);
			}
			break;
		case DOMXML_LOAD_RECOVERING:
			xmlDoValidityCheckingDefaultValue = 0;
			if (ZEND_NUM_ARGS() == 3) {
				docp = php_dom_xmlSAXParse(NULL, (char*) file, -1, 1, errors);
			} else {
				docp = xmlRecoverFile(file);
			}
			break;
	}
	xmlSubstituteEntitiesDefault (prevSubstValue);
	xmlDoValidityCheckingDefaultValue = oldvalue;
	xmlKeepBlanksDefault(oldvalue_keepblanks);

	if (!docp) {
		RETURN_FALSE;
	}


	if(DOMXML_IS_TYPE(getThis(), domxmldoc_class_entry)) {
		DOMXML_DOMOBJ_NEW(getThis(), (xmlNodePtr) docp, &ret);
	} else {
		DOMXML_RET_OBJ(rv, (xmlNodePtr) docp, &ret);
	}

}
/* }}} */

#if defined(LIBXML_HTML_ENABLED)
/* {{{ proto string domxml_html_dump_mem([int doc_handle])
   Dumps document into string as HTML */
PHP_FUNCTION(domxml_html_dump_mem)
{
	zval *id;
	xmlDoc *docp;
	xmlChar *mem;
	int size;

	DOMXML_PARAM_NONE(docp, id, le_domxmldocp);

	htmlDocDumpMemory(docp, &mem, &size);
	if (!size) {
		if (mem)
			xmlFree(mem);
		RETURN_FALSE;
	}
	RETVAL_STRINGL(mem, size, 1);
	xmlFree(mem);
}
/* }}} */

/* {{{ proto object html_doc(string html_doc [, bool from_file])
   Creates DOM object of HTML document */
PHP_FUNCTION(html_doc)
{
	zval *rv = NULL;
	xmlDoc *docp;
	int ret;
	char *buffer;
	int buffer_len;
	zend_bool from_file = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|b", &buffer, &buffer_len, &from_file) == FAILURE) {
		return;
	}

	if (from_file) {
		docp = htmlParseFile(buffer, NULL);
	} else {
		docp = htmlParseDoc(buffer, NULL);
	}
	if (!docp)
		RETURN_FALSE;

	DOMXML_RET_OBJ(rv, (xmlNodePtr) docp, &ret);
}
/* }}} */

/* {{{ proto object html_doc_file(string filename)
   Creates DOM object of HTML document in file */
PHP_FUNCTION(html_doc_file)
{
	zval *rv = NULL;
	xmlDoc *docp;
	int ret, file_len;
	char *file;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &file, &file_len) == FAILURE) {
		return;
	}

	docp = htmlParseFile(file, NULL);
	if (!docp) {
		RETURN_FALSE;
	}

	DOMXML_RET_OBJ(rv, (xmlNodePtr) docp, &ret);

	add_property_resource(return_value, "doc", ret);
	if (docp->name)
		add_property_stringl(return_value, "name", (char *) docp->name, strlen(docp->name), 1);
	if (docp->URL)
		add_property_stringl(return_value, "url", (char *) docp->URL, strlen(docp->URL), 1);
	if (docp->version)
		add_property_stringl(return_value, "version", (char *) docp->version, strlen(docp->version), 1);
/*	add_property_stringl(return_value, "version", (char *) docp->version, strlen(docp->version), 1);*/
	if (docp->encoding)
		add_property_stringl(return_value, "encoding", (char *) docp->encoding, strlen(docp->encoding), 1);
	add_property_long(return_value, "standalone", docp->standalone);
	add_property_long(return_value, "type", Z_TYPE_P(docp));
	add_property_long(return_value, "compression", docp->compression);
	add_property_long(return_value, "charset", docp->charset);
	zend_list_addref(ret);
}
/* }}} */
#endif  /* defined(LIBXML_HTML_ENABLED) */

/* {{{ proto bool domxml_substitute_entities_default(bool enable)
   Set and return the previous value for default entity support */
PHP_FUNCTION(domxml_substitute_entities_default)
{
	zend_bool enable;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "b", &enable) == FAILURE) {
		return;
	}

	RETURN_BOOL(xmlSubstituteEntitiesDefault(enable));
}
/* }}} */

/* {{{ proto bool domxml_node_text_concat(string content)
   Add string tocontent of a node */
PHP_FUNCTION(domxml_node_text_concat)
{
	zval *id;
	xmlNode *nodep;
	char *content;
	int content_len;

	DOMXML_GET_THIS_OBJ(nodep, id, le_domxmlnodep);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &content, &content_len) == FAILURE) {
		return;
	}

	if (content_len)
		xmlTextConcat(nodep, content, content_len);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto object domxml_add_root(string name)
   Adds root node to document */
PHP_FUNCTION(domxml_doc_add_root)
{
	zval *id, *rv = NULL;
	xmlDoc *docp;
	xmlNode *nodep;
	int ret, name_len;
	char *name;

	DOMXML_PARAM_TWO(docp, id, le_domxmldocp, "s", &name, &name_len);

	nodep = xmlNewDocNode(docp, NULL, name, NULL);
	if (!nodep) {
		RETURN_FALSE;
	}

	xmlDocSetRootElement(docp, nodep);

	DOMXML_RET_OBJ(rv, nodep, &ret);
}
/* }}} */

/* {{{ proto bool domxml_set_root(int domnode)
   Sets root node of document */
PHP_FUNCTION(domxml_doc_set_root)
{
	zval *id, *rv, *node;
	xmlDoc *docp;
	xmlNode *root;

	DOMXML_PARAM_TWO(docp, id, le_domxmldocp, "o", &node, &rv);
	DOMXML_GET_OBJ(root, node, le_domxmlnodep);

	if (!root) {
		RETURN_FALSE;
	}

	xmlDocSetRootElement(docp, root);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool domxml_doc_validate(array &error)
   Validates a DomDocument according to his DTD*/
PHP_FUNCTION(domxml_doc_validate)
{
	zval *id;
	xmlValidCtxt cvp;
	xmlDoc *docp;
	domxml_ErrorCtxt errorCtxt;
	zval *errors ;
	int oldvalue =  xmlDoValidityCheckingDefaultValue;
	    
	DOMXML_PARAM_ONE(docp, id, le_domxmldocp,"|z",&errors);
	errorCtxt.valid = &cvp;

	if (ZEND_NUM_ARGS() == 1) {
		zval_dtor(errors);
		array_init(errors);
		errorCtxt.errors = errors;
	} else {
		errorCtxt.errors = NULL;
	}

	errorCtxt.parser = NULL;
	xmlDoValidityCheckingDefaultValue = 1;
	cvp.userData = (void *) &errorCtxt;
	cvp.error    = (xmlValidityErrorFunc) domxml_error_validate;
	cvp.warning  = (xmlValidityWarningFunc) domxml_error_validate;
	if (docp->intSubset == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "No DTD given in XML-Document");
	}
    
	if (xmlValidateDocument(&cvp, docp)) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
	xmlDoValidityCheckingDefaultValue = oldvalue;
    
}
/* }}} */

/* {{{ proto object domxml_new_xmldoc(string version)
   Creates new xmldoc */
PHP_FUNCTION(domxml_new_xmldoc)
{
	zval *rv = NULL;
	xmlDoc *docp;
	int ret, buf_len;
	char *buf;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &buf, &buf_len) == FAILURE) {
		return;
	}

	docp = xmlNewDoc(buf);
	if (!docp) {
		RETURN_FALSE;
	}

	DOMXML_RET_OBJ(rv, (xmlNodePtr) docp, &ret);
}
/* }}} */

/* {{{ proto object domxml_parser([string buf[,string filename]])
   Creates new xmlparser */
PHP_FUNCTION(domxml_parser)
{
	zval *rv;
	xmlParserCtxtPtr parserp;
	int ret, buf_len = 0;
	char *buf = "";
	char *filename = NULL;
	int filename_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|ss", &buf, &buf_len, &filename, &filename_len) == FAILURE) {
		return;
	}

	parserp =  xmlCreatePushParserCtxt(NULL, NULL, buf, buf_len, filename);
	if (!parserp) {
		RETURN_FALSE;
	}
/*	parserp->loadsubset = XML_DETECT_IDS; */

	rv = php_xmlparser_new(parserp, &ret TSRMLS_CC);
	DOMXML_RET_ZVAL(rv);
}
/* }}} */

/* {{{ proto bool domxml_parser_start_document()
   starts a document*/
PHP_FUNCTION(domxml_parser_start_document)
{
	zval *id;
	xmlParserCtxtPtr parserp;
	
	DOMXML_PARAM_NONE(parserp, id, le_domxmlparserp);
	startDocument(parserp);
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool domxml_parser_end_document()
   ends a document */
PHP_FUNCTION(domxml_parser_end_document)
{
	zval *id;
	xmlParserCtxtPtr parserp;
	
	DOMXML_PARAM_NONE(parserp, id, le_domxmlparserp);
	endDocument(parserp);
	
}
/* }}} */

/* {{{ proto bool domxml_parser_start_element(string tagname, array attributes)
   Starts an element and adds attributes*/
PHP_FUNCTION(domxml_parser_start_element)
{
	zval *id,*params = NULL;
	xmlParserCtxtPtr parserp;
	char *tagname;
	int tagname_len;
	char **atts = NULL;
		
	DOMXML_PARAM_THREE(parserp, id, le_domxmlparserp,"s|a", &tagname, &tagname_len, &params);
	if (params != NULL) {
		atts = php_xmlparser_make_params(params TSRMLS_CC);
	}
	if (parserp->myDoc == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Document was not started");
		RETURN_FALSE;
	}
	startElement(parserp, (xmlChar *) tagname,  (const xmlChar **) atts);
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool domxml_parser_end_element(string tagname)
   Ends an element */
PHP_FUNCTION(domxml_parser_end_element)
{
	zval *id;
	xmlParserCtxtPtr parserp;
	char *tagname;
	int tagname_len;
	
	DOMXML_PARAM_TWO(parserp, id, le_domxmlparserp,"s", &tagname, &tagname_len);

	if (parserp->myDoc == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Document was not started");
		RETURN_FALSE;
	}

	endElement(parserp, (xmlChar *) tagname);
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool domxml_parser_comment(string comment)
   Adds a comment */
PHP_FUNCTION(domxml_parser_comment)
{
	zval *id;
	xmlParserCtxtPtr parserp;
	char *commentstring;
	int commentstring_len;
	
	DOMXML_PARAM_TWO(parserp, id, le_domxmlparserp,"s", &commentstring, &commentstring_len);

	if (parserp->myDoc == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Document was not started");
		RETURN_FALSE;
	}

	comment(parserp, (xmlChar *) commentstring);
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool domxml_parser_cdata_section(string chunk)
   adds a cdata block */
PHP_FUNCTION(domxml_parser_cdata_section)
{
	zval *id;
	xmlParserCtxtPtr parserp;
	char *chunk;
	int chunk_len;
	
	DOMXML_PARAM_TWO(parserp, id, le_domxmlparserp,"s", &chunk, &chunk_len);

	if (parserp->myDoc == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Document was not started");
		RETURN_FALSE;
	}

	cdataBlock(parserp, (xmlChar *) chunk, chunk_len);
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool domxml_parser_characters(string characters)
   Adds characters */
PHP_FUNCTION(domxml_parser_characters)
{
	zval *id;
	xmlParserCtxtPtr parserp;
	char *charactersstring;
	int characters_len;
	
	DOMXML_PARAM_TWO(parserp, id, le_domxmlparserp,"s", &charactersstring, &characters_len);

	if (parserp->myDoc == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Document was not started");
		RETURN_FALSE;
	}

	characters(parserp, (xmlChar *) charactersstring, characters_len);
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool domxml_parser_entity_reference(string reference)
   Adds entity reference */
PHP_FUNCTION(domxml_parser_entity_reference)
{
	zval *id;
	xmlParserCtxtPtr parserp;
	char *referencestring;
	int reference_len;
	
	DOMXML_PARAM_TWO(parserp, id, le_domxmlparserp,"s", &referencestring, &reference_len);

	if (parserp->myDoc == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Document was not started");
		RETURN_FALSE;
	}

	reference(parserp, (xmlChar *) referencestring);
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool domxml_parser_processing_instruction(string target, string data)
   Adds processing instruction */
PHP_FUNCTION(domxml_parser_processing_instruction)
{
	zval *id;
	xmlParserCtxtPtr parserp;
	char *data,*target;
	int data_len, target_len;
	
	DOMXML_PARAM_FOUR(parserp, id, le_domxmlparserp,"ss", &target, &target_len, &data, &data_len);

	if (parserp->myDoc == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Document was not started");
		RETURN_FALSE;
	}

	processingInstruction(parserp, (xmlChar *) target, (xmlChar *) data);
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool domxml_parser_namespace_decl(string href, string prefix)
   Adds namespace declaration */
PHP_FUNCTION(domxml_parser_namespace_decl)
{
	zval *id;
	xmlParserCtxtPtr parserp;
	char *href,*prefix;
	int href_len, prefix_len;
	
	DOMXML_PARAM_FOUR(parserp, id, le_domxmlparserp,"ss", &href, &href_len, &prefix, &prefix_len);

	if (parserp->myDoc == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Document was not started");
		RETURN_FALSE;
	}

	namespaceDecl(parserp, (xmlChar *) href, (xmlChar *) prefix);
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool domxml_parser_add_chunk(string chunk)
   adds xml-chunk to parser */
PHP_FUNCTION(domxml_parser_add_chunk)
{
	zval *id;
	xmlParserCtxtPtr parserp;
	char *chunk;
	int chunk_len, error;

	DOMXML_PARAM_TWO(parserp, id, le_domxmlparserp,"s", &chunk, &chunk_len);
	error = xmlParseChunk(parserp, chunk, chunk_len , 0);
	if (error != 0) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto object domxml_parser_end([string chunk])
   Ends parsing and returns DomDocument*/
PHP_FUNCTION(domxml_parser_end)
{
	zval *id,*rv = NULL;
	xmlParserCtxtPtr parserp;
	char *chunk = NULL;
	int chunk_len = 0, error;
	int ret;


	DOMXML_PARAM_TWO(parserp, id, le_domxmlparserp,"|s", &chunk, &chunk_len);
	error = xmlParseChunk(parserp, chunk, chunk_len, 1);
	if (error != 0) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Error: %d", error);
		RETURN_FALSE;
	}
	if (parserp->myDoc != NULL) {
		DOMXML_RET_OBJ(rv, (xmlNodePtr) parserp->myDoc, &ret);
	}
	else {
		RETVAL_FALSE
	}
}
/* }}} */

/* {{{ proto object domxml_parser_get_document()
   Returns DomDocument from parser */
PHP_FUNCTION(domxml_parser_get_document)
{
	zval *id,*rv = NULL;
	xmlParserCtxtPtr parserp;
	int ret;

	DOMXML_PARAM_NONE(parserp, id, le_domxmlparserp);

	if (parserp->myDoc != NULL) {
		DOMXML_RET_OBJ(rv, (xmlNodePtr) parserp->myDoc, &ret);
	}
	else {
		RETVAL_FALSE
	}
}
/* }}} */

/* {{{ proto bool domxml_parser_set_keep_blanks(bool mode)
   Determines how to handle blanks */
PHP_FUNCTION(domxml_parser_set_keep_blanks)
{
	zval *id;
	xmlParserCtxtPtr parserp;
	zend_bool mode;

	DOMXML_PARAM_ONE(parserp, id, le_domxmlparserp, "b", &mode);
	parserp->keepBlanks = mode;

	RETURN_TRUE;
}
/* }}} */

#ifdef newcode
/* {{{ proto int node_namespace([int node])
   Returns list of namespaces */
static int node_namespace(zval **attributes, xmlNode *nodep TSRMLS_DC)
{
	xmlNs *ns;

	/* Get the children of the current node */
	ns = nodep->ns;
	if (!ns) {
		return -1;
	}

	/* create an php array for the children */
	MAKE_STD_ZVAL(*attributes);
	if (array_init(*attributes) == FAILURE) {
		return -1;
	}

	while (ns) {
		zval *pattr;
		int ret;

		pattr = php_domobject_new((xmlNodePtr) ns, &ret, NULL TSRMLS_CC);
		SEPARATE_ZVAL(&pattr);

/*		if(!ret) { */
		if (ns->href)
			add_property_stringl(pattr, "href", (char *) ns->href, strlen(ns->href), 1);
		if (ns->prefix)
			add_property_stringl(pattr, "prefix", (char *) ns->prefix, strlen(ns->prefix), 1);
		add_property_long(pattr, "type", Z_TYPE_P(ns));
/*		} */

		zend_hash_next_index_insert(Z_ARRVAL_PP(attributes), &pattr, sizeof(zval *), NULL);
		ns = ns->next;
	}
	return 0;
}
/* }}} */
#endif

/* We don't have a type zval. **attributes is also very unusual. */

/* {{{ proto int node_attributes(zval **attributes, int node)
   Returns list of children nodes */
static int node_attributes(zval **attributes, xmlNode *nodep TSRMLS_DC)
{
	xmlAttr *attr;
	int count = 0;

	/* Get the children of the current node */
	if (Z_TYPE_P(nodep) != XML_ELEMENT_NODE)
		return -1;
	attr = nodep->properties;
	if (!attr)
		return -2;

	/* create an php array for the children */
	MAKE_STD_ZVAL(*attributes);
	array_init(*attributes);

	while (attr) {
		zval *pattr;
		int ret;

		pattr = php_domobject_new((xmlNodePtr) attr, &ret, NULL TSRMLS_CC);
		/** XXX FIXME XXX */
/*		if(0 <= (n = node_children(&children, attr->children TSRMLS_CC))) {
			zend_hash_update(Z_OBJPROP_P(value), "children", sizeof("children"), (void *) &children, sizeof(zval *), NULL);
		}
*/		add_property_string(pattr, "name", (char *) (attr->name), 1);
		add_property_string(pattr, "value", xmlNodeGetContent((xmlNodePtr) attr), 1);
		zend_hash_next_index_insert(Z_ARRVAL_PP(attributes), &pattr, sizeof(zval *), NULL);
		attr = attr->next;
		count++;
	}
	return count;
}
/* }}} */

/* {{{ proto int node_children([int node])
   Returns list of children nodes */
static int node_children(zval **children, xmlNode *nodep TSRMLS_DC)
{
	zval *mchildren, *attributes;
	/* zval *namespace; */
	xmlNode *last;
	int count = 0;

	/* Get the children of the current node */
	last = nodep;
	if (!last) {
		return -1;
	}

	/* create an php array for the children */
	MAKE_STD_ZVAL(*children);
	array_init(*children);

	while (last) {
		zval *child;
		int ret;

		if (NULL != (child = php_domobject_new(last, &ret, NULL TSRMLS_CC))) {
			zend_hash_next_index_insert(Z_ARRVAL_PP(children), &child, sizeof(zval *), NULL);

			/* Get the namespace of the current node and add it as a property */
			/* XXX FIXME XXX */
/*
			if(!node_namespace(&namespace, last))
				zend_hash_update(Z_OBJPROP_P(child), "namespace", sizeof("namespace"), (void *) &namespace, sizeof(zval *), NULL);
*/

			/* Get the attributes of the current node and add it as a property */
			if (node_attributes(&attributes, last TSRMLS_CC) >= 0)
				zend_hash_update(Z_OBJPROP_P(child), "attributes", sizeof("attributes"), (void *) &attributes, sizeof(zval *), NULL);

			/* Get recursively the children of the current node and add it as a property */
			if (node_children(&mchildren, last->children TSRMLS_CC) >= 0)
				zend_hash_update(Z_OBJPROP_P(child), "children", sizeof("children"), (void *) &mchildren, sizeof(zval *), NULL);

			count++;
		}
		last = last->next;
	}
	return count;
}
/* }}} */

/* {{{ proto object domxml_xmltree(string xmltree)
   Creates a tree of PHP objects from an XML document */
PHP_FUNCTION(domxml_xmltree)
{
	zval *children, *rv = NULL;
	xmlDoc *docp;
	xmlNode *root;
	int ret, buf_len;
	char *buf;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &buf, &buf_len) == FAILURE) {
		return;
	}

	/* Create a new xml document */
	docp = xmlParseDoc(buf);
	if (!docp) {
		RETURN_FALSE;
	}

	/* get the root and add as a property to the document */
	root = docp->children;
	if (!root) {
		xmlFreeDoc(docp);
		RETURN_FALSE;
	}

	DOMXML_RET_OBJ(rv, (xmlNodePtr) docp, &ret);

	/* The root itself maybe an array. Though you may not have two Elements
	   as root, you may have a comment, pi and and element as root.
	   Thanks to Paul DuBois for pointing me at this.
	 */
	if (node_children(&children, root TSRMLS_CC) >= 0) {
		zend_hash_update(Z_OBJPROP_P(return_value), "children",sizeof("children"), (void *) &children, sizeof(zval *), NULL);
	}
/*	xmlFreeDoc(docp); */
}
/* }}} */

#if defined(LIBXML_XPATH_ENABLED)
/* {{{ proto bool xpath_init(void)
   Initializing XPath environment */
PHP_FUNCTION(xpath_init)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	xmlXPathInit();
	RETURN_TRUE;
}
/* }}} */

/* {{{ php_xpathptr_new_context()
 */
static void php_xpathptr_new_context(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	zval *id, *rv;
	xmlXPathContextPtr ctx;
	xmlDocPtr docp;
	int ret;

	DOMXML_PARAM_NONE(docp, id, le_domxmldocp);

#if defined(LIBXML_XPTR_ENABLED)
	if (mode == PHP_XPTR)
		ctx = xmlXPtrNewContext(docp, NULL, NULL);
	else
#endif
		ctx = xmlXPathNewContext(docp);
	if (!ctx) {
		RETURN_FALSE;
	}

	rv = php_xpathcontext_new(ctx, &ret TSRMLS_CC);
	DOMXML_RET_ZVAL(rv);
}
/* }}} */

/* {{{ proto object xpath_new_context([int doc_handle])
   Creates new XPath context */
PHP_FUNCTION(xpath_new_context)
{
	php_xpathptr_new_context(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_XPATH);
}
/* }}} */

/* {{{ proto object xptr_new_context([int doc_handle])
   Creates new XPath context */
PHP_FUNCTION(xptr_new_context)
{
	php_xpathptr_new_context(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_XPTR);
}
/* }}} */

/* {{{ php_xpathptr_eval()
 */
static void php_xpathptr_eval(INTERNAL_FUNCTION_PARAMETERS, int mode, int expr)
{
	zval *id, *rv, *contextnode = NULL;
	xmlXPathContextPtr ctxp;
	xmlXPathObjectPtr xpathobjp;
	xmlNode *contextnodep;
	int ret, str_len;
	char *str;
	contextnode = NULL;
	contextnodep = NULL;

	if (NULL == (id = getThis())) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "os|o", &id, &str, &str_len, &contextnode) == FAILURE) {
			return;
		}
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|o", &str, &str_len, &contextnode) == FAILURE) {
			return;
		}
	}

	ctxp = php_xpath_get_context(id, le_xpathctxp, 0 TSRMLS_CC);
	if (!ctxp) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot fetch XPATH context");
		RETURN_FALSE;
	}

	if (contextnode) {
		DOMXML_GET_OBJ(contextnodep, contextnode, le_domxmlnodep);
	}
	ctxp->node = contextnodep;
  
#if defined(LIBXML_XPTR_ENABLED)
	if (mode == PHP_XPTR) {
		xpathobjp = xmlXPtrEval(BAD_CAST str, ctxp);
	} else {
#endif
		if (expr) {
			xpathobjp = xmlXPathEvalExpression(str, ctxp);
		} else {
			xpathobjp = xmlXPathEval(str, ctxp);
		}
#if defined(LIBXML_XPTR_ENABLED)
	}
#endif

	ctxp->node = NULL;
	if (!xpathobjp) {
		RETURN_FALSE;
	}

	if (NULL == (rv = php_xpathobject_new(xpathobjp, &ret TSRMLS_CC))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot create required XPATH objcet");
		RETURN_FALSE;
	}
	SEPARATE_ZVAL(&rv);

	add_property_long(rv, "type", Z_TYPE_P(xpathobjp));

	switch (Z_TYPE_P(xpathobjp)) {

		case XPATH_UNDEFINED:
			break;

		case XPATH_NODESET:
		{
			int i;
			zval *arr;
			xmlNodeSetPtr nodesetp;

			MAKE_STD_ZVAL(arr);
			if (array_init(arr) == FAILURE) {
				zval_dtor(rv);
				RETURN_FALSE;
			}

			if (NULL == (nodesetp = xpathobjp->nodesetval)) {
				zval_dtor(rv);
				RETURN_FALSE;
			}

			for (i = 0; i < nodesetp->nodeNr; i++) {
				xmlNodePtr node = nodesetp->nodeTab[i];
				zval *child;
				int retnode;

				/* construct a node object */
				child = php_domobject_new(node, &retnode, NULL TSRMLS_CC);
				zend_hash_next_index_insert(Z_ARRVAL_P(arr), &child, sizeof(zval *), NULL);
			}
			zend_hash_update(Z_OBJPROP_P(rv), "nodeset", sizeof("nodeset"), (void *) &arr, sizeof(zval *), NULL);
			break;
		}

		case XPATH_BOOLEAN:
			add_property_bool(rv, "value", xpathobjp->boolval);
			break;

		case XPATH_NUMBER:
			add_property_double(rv, "value", xpathobjp->floatval);
			break;

		case XPATH_STRING:
			add_property_string(rv, "value", xpathobjp->stringval, 1);
			break;

		case XPATH_POINT:
			break;

		case XPATH_RANGE:
			break;

		case XPATH_LOCATIONSET:
			break;

		case XPATH_USERS:
			break;

		case XPATH_XSLT_TREE:
			break;
	}

	xmlXPathFreeObject(xpathobjp);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto object xpath_eval([object xpathctx_handle,] string str)
   Evaluates the XPath Location Path in the given string */
PHP_FUNCTION(xpath_eval)
{
	php_xpathptr_eval(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_XPATH, 0);
}
/* }}} */

/* {{{ proto object xpath_eval_expression([object xpathctx_handle,] string str)
   Evaluates the XPath expression in the given string */
PHP_FUNCTION(xpath_eval_expression)
{
	php_xpathptr_eval(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_XPATH, 1);
}
/* }}} */

/* {{{ proto bool xpath_register_ns([object xpathctx_handle,] string namespace_prefix, string namespace_uri)
	Registeres the given namespace in the passed XPath context */
PHP_FUNCTION(xpath_register_ns)
{
	int prefix_len, uri_len, result;
	xmlXPathContextPtr ctxp;
	char *prefix, *uri;
	zval *id;

	DOMXML_PARAM_FOUR(ctxp, id, le_xpathctxp, "ss", &prefix, &prefix_len, &uri, &uri_len);

	ctxp->node = NULL;

	#ifdef CHREGU_0
	/* this leads to memleaks... commenting it out, as it works for me without copying
	   it. chregu */
	/*
		this is a hack - libxml2 doesn't copy the URI, it simply uses the string
		given in the parameter - which is normally deallocated after the function
	*/
	uri_static = estrndup(uri, uri_len);
	result = xmlXPathRegisterNs(ctxp, prefix, uri_static);
	#endif
	
	result = xmlXPathRegisterNs(ctxp, prefix, uri);

	if (0 == result) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool xpath_register_ns_auto([object xpathctx_handle,] [object contextnode])
	Registeres the given namespace in the passed XPath context */
PHP_FUNCTION(xpath_register_ns_auto)
{
	/* automatic namespace definitions registration.
	it's only done for the context node
	if you need namespaces defined in other nodes, 
	you have to specify them explicitely with
	xpath_register_ns();
	*/

	zval *contextnode = NULL, *id;
	xmlXPathContextPtr ctxp;
	xmlNodePtr contextnodep;
	xmlNsPtr *namespaces;	
	int nsNr;
			
	DOMXML_PARAM_ONE(ctxp, id, le_xpathctxp, "|o", &contextnode);

	if (contextnode == NULL) {
		namespaces = xmlGetNsList(ctxp->doc, xmlDocGetRootElement(ctxp->doc));
	} else {
		DOMXML_GET_OBJ(contextnodep, contextnode, le_domxmlnodep);
		namespaces = xmlGetNsList(ctxp->doc, contextnodep);
	}
				
	nsNr = 0;
	if (namespaces != NULL) {
		while (namespaces[nsNr] != NULL) {
			xmlXPathRegisterNs(ctxp, namespaces[nsNr]->prefix, namespaces[nsNr]->href); 
			nsNr++;
		}
	}

	RETURN_TRUE;
}
/* }}} */

#endif	/* defined(LIBXML_XPATH_ENABLED) */

#if defined(LIBXML_XPTR_ENABLED)
/* {{{ proto int xptr_eval([int xpathctx_handle,] string str)
   Evaluates the XPtr Location Path in the given string */
PHP_FUNCTION(xptr_eval)
{
	php_xpathptr_eval(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_XPTR, 0);
}
/* }}} */
#endif	/* LIBXML_XPTR_ENABLED */

/* {{{ proto string domxml_version(void)
   Get XML library version */
PHP_FUNCTION(domxml_version)
{
	RETURN_STRING((char *) xmlParserVersion,1);
}
/* }}} */

/* {{{ proto int domxml_doc_xinclude()
   Substitutues xincludes in a DomDocument */
PHP_FUNCTION(domxml_doc_xinclude)
{
	zval *id;
	xmlDoc *docp;
	int err; 
	
	DOMXML_PARAM_NONE(docp, id, le_domxmldocp);
	
	err = xmlXIncludeProcess (docp);
	
	if (err) {
		RETVAL_LONG(err);
	} else {
		RETVAL_FALSE;
	}
    
}
/* }}} */

#if HAVE_DOMXSLT
static zval *php_xsltstylesheet_new(xsltStylesheetPtr obj, int *found TSRMLS_DC)
{
	zval *wrapper;
	int rsrc_type;

	*found = 0;

	if (!obj) {
		MAKE_STD_ZVAL(wrapper);
		ZVAL_NULL(wrapper);
		return wrapper;
	}

	if ((wrapper = (zval *) dom_object_get_data((void *) obj))) {
		zval_add_ref(&wrapper);
		*found = 1;
		return wrapper;
	}

	MAKE_STD_ZVAL(wrapper);

	object_init_ex(wrapper, domxsltstylesheet_class_entry);
	rsrc_type = le_domxsltstylesheetp;
	php_xsltstylesheet_set_object(wrapper, (void *) obj, rsrc_type);

	return (wrapper);
}

/* {{{ proto object domxml_xslt_stylesheet(string xsltstylesheet)
   Creates XSLT Stylesheet object from string */
PHP_FUNCTION(domxml_xslt_stylesheet)
{
	zval *rv;
	xmlDocPtr docp;
	xsltStylesheetPtr sheetp;
	int ret;
	char *buffer;
	int buffer_len;
	int prevSubstValue, prevExtDtdValue;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &buffer, &buffer_len) == FAILURE) {
		RETURN_FALSE;
	}

	prevSubstValue = xmlSubstituteEntitiesDefault (1);
	prevExtDtdValue = xmlLoadExtDtdDefaultValue;
	xmlLoadExtDtdDefaultValue = XML_DETECT_IDS | XML_COMPLETE_ATTRS;

	docp = xmlParseDoc(buffer);

	xmlSubstituteEntitiesDefault (prevSubstValue);
	xmlLoadExtDtdDefaultValue = prevExtDtdValue;

	if (!docp)
		RETURN_FALSE;

	sheetp = xsltParseStylesheetDoc(docp);

	if (!sheetp)
		RETURN_FALSE;

	rv = php_xsltstylesheet_new(sheetp, &ret TSRMLS_CC);
	DOMXML_RET_ZVAL(rv);
}
/* }}} */

/* {{{ proto object domxml_xslt_stylesheet_doc(object xmldoc)
   Creates XSLT Stylesheet object from DOM Document object */
PHP_FUNCTION(domxml_xslt_stylesheet_doc)
{
	zval *rv, *idxml;
	xmlDocPtr docp;
	xmlDocPtr newdocp;
	xsltStylesheetPtr sheetp;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &idxml) == FAILURE) {
		RETURN_FALSE;
	}

	DOMXML_GET_OBJ(docp, idxml, le_domxmldocp);

	newdocp = xmlCopyDoc(docp, 1);

	if (!newdocp)
		RETURN_FALSE;

	sheetp = xsltParseStylesheetDoc(newdocp);

	if (!sheetp)
		RETURN_FALSE;

	rv = php_xsltstylesheet_new(sheetp, &ret TSRMLS_CC);
	DOMXML_RET_ZVAL(rv);
}
/* }}} */

/* {{{ proto object domxml_xslt_stylesheet_file(string filename)
   Creates XSLT Stylesheet object from file */
PHP_FUNCTION(domxml_xslt_stylesheet_file)
{
	zval *rv;
	xsltStylesheetPtr sheetp;
	int ret, file_len;
	char *file;
	int prevSubstValue, prevExtDtdValue;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &file, &file_len) == FAILURE) {
		RETURN_FALSE;
	}

	prevSubstValue = xmlSubstituteEntitiesDefault (1);
	prevExtDtdValue = xmlLoadExtDtdDefaultValue;
	xmlLoadExtDtdDefaultValue = XML_DETECT_IDS | XML_COMPLETE_ATTRS;

	sheetp = xsltParseStylesheetFile(file);

	xmlSubstituteEntitiesDefault (prevSubstValue);
	xmlLoadExtDtdDefaultValue = prevExtDtdValue;

	if (!sheetp)
		RETURN_FALSE;

	rv = php_xsltstylesheet_new(sheetp, &ret TSRMLS_CC);
	DOMXML_RET_ZVAL(rv);
}
/* }}} */

/* {{{ php_domxslt_string_to_xpathexpr()
   Translates a string to a XPath Expression */
static char *php_domxslt_string_to_xpathexpr(const char *str TSRMLS_DC)
{
	const xmlChar *string = (const xmlChar *)str;

	xmlChar *value;
	int str_len;
	
	str_len = xmlStrlen(string) + 3;
	
	if (xmlStrchr(string, '"')) {
		if (xmlStrchr(string, '\'')) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot create XPath expression (string contains both quote and double-quotes)");
			return NULL;
		}
		value = (xmlChar*) emalloc (str_len * sizeof(xmlChar) );
		snprintf(value, str_len, "'%s'", string);
	} else {
		value = (xmlChar*) emalloc (str_len * sizeof(xmlChar) );
		snprintf(value, str_len, "\"%s\"", string);
	}

	return (char *)value;
}

/* {{{ php_domxslt_make_params()
   Translates a PHP array to a libxslt parameters array */
static char **php_domxslt_make_params(zval *idvars, int xpath_params TSRMLS_DC)
{
	HashTable *parht;
	int parsize;
	zval **value;
	char *xpath_expr, *string_key = NULL;
	ulong num_key;
	char **params = NULL;
	int i = 0;

	parht = HASH_OF(idvars);
	parsize = (2 * zend_hash_num_elements(parht) + 1) * sizeof(char *);
	params = (char **)emalloc(parsize);
	memset((char *)params, 0, parsize);

	for (zend_hash_internal_pointer_reset(parht);
		zend_hash_get_current_data(parht, (void **)&value) == SUCCESS;
		zend_hash_move_forward(parht)) {

		if (zend_hash_get_current_key(parht, &string_key, &num_key, 1) != HASH_KEY_IS_STRING) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid argument or parameter array");
			return NULL;
		}
		else {
			SEPARATE_ZVAL(value);
			convert_to_string_ex(value);

			if (!xpath_params) {
				xpath_expr = php_domxslt_string_to_xpathexpr(Z_STRVAL_PP(value) TSRMLS_CC);
			}
			else {
				xpath_expr = Z_STRVAL_PP(value);
			}

			if (xpath_expr) {
				params[i++] = string_key;
				params[i++] = xpath_expr;
			}
		}
	}

	params[i++] = NULL;

	return params;
}
/* }}} */

/* {{{ proto object domxml_xslt_process(object xslstylesheet, object xmldoc [, array xslt_parameters [, bool xpath_parameters [, string profileFilename]]])
   Perform an XSLT transformation */
PHP_FUNCTION(domxml_xslt_process)
{
/* TODO:
	- test memory deallocation
	- test other stuff
	- check xsltsp->errors ???
*/
	zval *rv = NULL, *idxsl, *idxml, *idparams = NULL;
	zend_bool xpath_params = 0;
	xsltStylesheetPtr xsltstp;
	xmlDocPtr xmldocp;
	xmlDocPtr docp;
	char **params = NULL;
	int ret;
	char *filename;
	int filename_len = 0;

	DOMXML_GET_THIS(idxsl);

	xsltstp = php_xsltstylesheet_get_object(idxsl, le_domxsltstylesheetp, 0 TSRMLS_CC);
	if (!xsltstp) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Underlying object missing");
		RETURN_FALSE;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o|abs", &idxml, &idparams, &xpath_params, &filename, &filename_len) == FAILURE) {
		RETURN_FALSE;
	}

	DOMXML_GET_OBJ(xmldocp, idxml, le_domxmldocp);

	if (idparams) {
		params = php_domxslt_make_params(idparams, xpath_params TSRMLS_CC);
	}

	if (filename_len) {
		FILE *f;
		f = fopen (filename,"w");
		docp = xsltProfileStylesheet(xsltstp, xmldocp, (const char**)params, f);
		fclose(f);
	} else {
		docp = xsltApplyStylesheet(xsltstp, xmldocp, (const char**)params);
	}

	if (params) {
		efree(params);
	}

	if (!docp) {
		RETURN_FALSE;
	}

	DOMXML_RET_OBJ(rv, (xmlNodePtr) docp, &ret);
}
/* }}} */

/* {{{ proto string domxml_xslt_result_dump_mem(object xslstylesheet, object xmldoc)
   output XSLT result to memory */
PHP_FUNCTION(domxml_xslt_result_dump_mem)
{
	zval *idxsl, *idxml;
	xsltStylesheetPtr xsltstp;
	xmlDocPtr xmldocp;
	xmlChar *doc_txt_ptr;
	int doc_txt_len;
	int ret;

	DOMXML_GET_THIS(idxsl);

	xsltstp = php_xsltstylesheet_get_object(idxsl, le_domxsltstylesheetp, 0 TSRMLS_CC);
	if (!xsltstp) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Underlying object missing");
		RETURN_FALSE;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &idxml) == FAILURE) {
		RETURN_FALSE;
	}

	DOMXML_GET_OBJ(xmldocp, idxml, le_domxmldocp);

	ret = xsltSaveResultToString(&doc_txt_ptr, &doc_txt_len, xmldocp, xsltstp);

	if (ret < 0) {
		RETURN_FALSE;
	}

	RETVAL_STRINGL(doc_txt_ptr, doc_txt_len, 1);
	xmlFree(doc_txt_ptr);
}
/* }}} */

/* {{{ proto int domxml_xslt_result_dump_file(object xslstylesheet, object xmldoc, string filename[, int compression])
   output XSLT result to File */
PHP_FUNCTION(domxml_xslt_result_dump_file)
{
	zval *idxsl, *idxml;
	xsltStylesheetPtr xsltstp;
	xmlDocPtr xmldocp;
	char *filename;
	int filename_len;
	int ret, compression = 0;

	DOMXML_GET_THIS(idxsl);

	xsltstp = php_xsltstylesheet_get_object(idxsl, le_domxsltstylesheetp, 0 TSRMLS_CC);
	if (!xsltstp) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Underlying object missing");
		RETURN_FALSE;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "os|l", &idxml, &filename, &filename_len, &compression) == FAILURE) {
		RETURN_FALSE;
	}

	DOMXML_GET_OBJ(xmldocp, idxml, le_domxmldocp);

	ret = xsltSaveResultToFilename(filename, xmldocp, xsltstp, compression);

	if (ret < 0) {
		RETURN_FALSE;
	}

	RETURN_LONG(ret);
}
/* }}} */

/* {{{ proto string domxml_xslt_version(void)
   Get XSLT library version */
PHP_FUNCTION(domxml_xslt_version)
{
	RETURN_LONG(xsltLibxsltVersion);
}
/* }}} */
#endif /* HAVE_DOMXSLT */

#endif /* HAVE_DOMXML */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
