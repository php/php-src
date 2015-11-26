/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Christian Stocker <chregu@php.net>                          |
   |          Rob Richards <rrichards@php.net>                            |
   +----------------------------------------------------------------------+
*/

/* $Id$ */
#ifndef DOM_FE_H
#define DOM_FE_H

extern const zend_function_entry php_dom_domexception_class_functions[];
extern const zend_function_entry php_dom_domstringlist_class_functions[];
extern const zend_function_entry php_dom_namelist_class_functions[];
extern const zend_function_entry php_dom_domimplementationlist_class_functions[];
extern const zend_function_entry php_dom_domimplementationsource_class_functions[];
extern const zend_function_entry php_dom_domimplementation_class_functions[];
extern const zend_function_entry php_dom_documentfragment_class_functions[];
extern const zend_function_entry php_dom_document_class_functions[];
extern const zend_function_entry php_dom_node_class_functions[];
extern const zend_function_entry php_dom_nodelist_class_functions[];
extern const zend_function_entry php_dom_namednodemap_class_functions[];
extern const zend_function_entry php_dom_characterdata_class_functions[];
extern const zend_function_entry php_dom_attr_class_functions[];
extern const zend_function_entry php_dom_element_class_functions[];
extern const zend_function_entry php_dom_text_class_functions[];
extern const zend_function_entry php_dom_comment_class_functions[];
extern const zend_function_entry php_dom_typeinfo_class_functions[];
extern const zend_function_entry php_dom_userdatahandler_class_functions[];
extern const zend_function_entry php_dom_domerror_class_functions[];
extern const zend_function_entry php_dom_domerrorhandler_class_functions[];
extern const zend_function_entry php_dom_domlocator_class_functions[];
extern const zend_function_entry php_dom_domconfiguration_class_functions[];
extern const zend_function_entry php_dom_cdatasection_class_functions[];
extern const zend_function_entry php_dom_documenttype_class_functions[];
extern const zend_function_entry php_dom_notation_class_functions[];
extern const zend_function_entry php_dom_entity_class_functions[];
extern const zend_function_entry php_dom_entityreference_class_functions[];
extern const zend_function_entry php_dom_processinginstruction_class_functions[];
extern const zend_function_entry php_dom_string_extend_class_functions[];
extern const zend_function_entry php_dom_xpath_class_functions[];

/* domexception errors */
typedef enum {
/* PHP_ERR is non-spec code for PHP errors: */
	PHP_ERR                        = 0,
	INDEX_SIZE_ERR                 = 1,
	DOMSTRING_SIZE_ERR             = 2,
	HIERARCHY_REQUEST_ERR          = 3,
	WRONG_DOCUMENT_ERR             = 4,
	INVALID_CHARACTER_ERR          = 5,
	NO_DATA_ALLOWED_ERR            = 6,
	NO_MODIFICATION_ALLOWED_ERR    = 7,
	NOT_FOUND_ERR                  = 8,
	NOT_SUPPORTED_ERR              = 9,
	INUSE_ATTRIBUTE_ERR            = 10,
/* Introduced in DOM Level 2: */
	INVALID_STATE_ERR              = 11,
/* Introduced in DOM Level 2: */
	SYNTAX_ERR                     = 12,
/* Introduced in DOM Level 2: */
	INVALID_MODIFICATION_ERR       = 13,
/* Introduced in DOM Level 2: */
	NAMESPACE_ERR                  = 14,
/* Introduced in DOM Level 2: */
	INVALID_ACCESS_ERR             = 15,
/* Introduced in DOM Level 3: */
	VALIDATION_ERR                 = 16
} dom_exception_code;

/* domstringlist methods */
PHP_FUNCTION(dom_domstringlist_item);

/* domnamelist methods */
PHP_FUNCTION(dom_namelist_get_name);
PHP_FUNCTION(dom_namelist_get_namespace_uri);

/* domimplementationlist methods */
PHP_FUNCTION(dom_domimplementationlist_item);

/* domimplementationsource methods */
PHP_FUNCTION(dom_domimplementationsource_get_domimplementation);
PHP_FUNCTION(dom_domimplementationsource_get_domimplementations);

/* domimplementation methods */
PHP_METHOD(domimplementation, hasFeature);
PHP_METHOD(domimplementation, createDocumentType);
PHP_METHOD(domimplementation, createDocument);
PHP_METHOD(domimplementation, getFeature);

/* domdocumentfragment methods */
PHP_METHOD(domdocumentfragment, __construct);
PHP_METHOD(domdocumentfragment, appendXML);

/* domdocument methods */
PHP_FUNCTION(dom_document_create_element);
PHP_FUNCTION(dom_document_create_document_fragment);
PHP_FUNCTION(dom_document_create_text_node);
PHP_FUNCTION(dom_document_create_comment);
PHP_FUNCTION(dom_document_create_cdatasection);
PHP_FUNCTION(dom_document_create_processing_instruction);
PHP_FUNCTION(dom_document_create_attribute);
PHP_FUNCTION(dom_document_create_entity_reference);
PHP_FUNCTION(dom_document_get_elements_by_tag_name);
PHP_FUNCTION(dom_document_import_node);
PHP_FUNCTION(dom_document_create_element_ns);
PHP_FUNCTION(dom_document_create_attribute_ns);
PHP_FUNCTION(dom_document_get_elements_by_tag_name_ns);
PHP_FUNCTION(dom_document_get_element_by_id);
PHP_FUNCTION(dom_document_adopt_node);
PHP_FUNCTION(dom_document_normalize_document);
PHP_FUNCTION(dom_document_rename_node);
PHP_METHOD(domdocument, __construct);
	/* convienience methods */
PHP_METHOD(domdocument, load);
PHP_FUNCTION(dom_document_save);
PHP_METHOD(domdocument, loadXML);
PHP_FUNCTION(dom_document_savexml);
PHP_FUNCTION(dom_document_validate);
PHP_FUNCTION(dom_document_xinclude);
PHP_METHOD(domdocument, registerNodeClass);

#if defined(LIBXML_HTML_ENABLED)
PHP_METHOD(domdocument, loadHTML);
PHP_METHOD(domdocument, loadHTMLFile);
PHP_FUNCTION(dom_document_save_html);
PHP_FUNCTION(dom_document_save_html_file);
#endif  /* defined(LIBXML_HTML_ENABLED) */

#if defined(LIBXML_SCHEMAS_ENABLED)
PHP_FUNCTION(dom_document_schema_validate_file);
PHP_FUNCTION(dom_document_schema_validate_xml);
PHP_FUNCTION(dom_document_relaxNG_validate_file);
PHP_FUNCTION(dom_document_relaxNG_validate_xml);
#endif

/* domnode methods */
PHP_FUNCTION(dom_node_insert_before);
PHP_FUNCTION(dom_node_replace_child);
PHP_FUNCTION(dom_node_remove_child);
PHP_FUNCTION(dom_node_append_child);
PHP_FUNCTION(dom_node_has_child_nodes);
PHP_FUNCTION(dom_node_clone_node);
PHP_FUNCTION(dom_node_normalize);
PHP_FUNCTION(dom_node_is_supported);
PHP_FUNCTION(dom_node_has_attributes);
PHP_FUNCTION(dom_node_compare_document_position);
PHP_FUNCTION(dom_node_is_same_node);
PHP_FUNCTION(dom_node_lookup_prefix);
PHP_FUNCTION(dom_node_is_default_namespace);
PHP_FUNCTION(dom_node_lookup_namespace_uri);
PHP_FUNCTION(dom_node_is_equal_node);
PHP_FUNCTION(dom_node_get_feature);
PHP_FUNCTION(dom_node_set_user_data);
PHP_FUNCTION(dom_node_get_user_data);
PHP_METHOD(domnode, C14N);
PHP_METHOD(domnode, C14NFile);
PHP_METHOD(domnode, getNodePath);
PHP_METHOD(domnode, getLineNo);

/* domnodelist methods */
PHP_FUNCTION(dom_nodelist_item);

/* domnamednodemap methods */
PHP_FUNCTION(dom_namednodemap_get_named_item);
PHP_FUNCTION(dom_namednodemap_set_named_item);
PHP_FUNCTION(dom_namednodemap_remove_named_item);
PHP_FUNCTION(dom_namednodemap_item);
PHP_FUNCTION(dom_namednodemap_get_named_item_ns);
PHP_FUNCTION(dom_namednodemap_set_named_item_ns);
PHP_FUNCTION(dom_namednodemap_remove_named_item_ns);

/* domcharacterdata methods */
PHP_FUNCTION(dom_characterdata_substring_data);
PHP_FUNCTION(dom_characterdata_append_data);
PHP_FUNCTION(dom_characterdata_insert_data);
PHP_FUNCTION(dom_characterdata_delete_data);
PHP_FUNCTION(dom_characterdata_replace_data);

/* domattr methods */
PHP_FUNCTION(dom_attr_is_id);
PHP_METHOD(domattr, __construct);

/* domelement methods */
PHP_FUNCTION(dom_element_get_attribute);
PHP_FUNCTION(dom_element_set_attribute);
PHP_FUNCTION(dom_element_remove_attribute);
PHP_FUNCTION(dom_element_get_attribute_node);
PHP_FUNCTION(dom_element_set_attribute_node);
PHP_FUNCTION(dom_element_remove_attribute_node);
PHP_FUNCTION(dom_element_get_elements_by_tag_name);
PHP_FUNCTION(dom_element_get_attribute_ns);
PHP_FUNCTION(dom_element_set_attribute_ns);
PHP_FUNCTION(dom_element_remove_attribute_ns);
PHP_FUNCTION(dom_element_get_attribute_node_ns);
PHP_FUNCTION(dom_element_set_attribute_node_ns);
PHP_FUNCTION(dom_element_get_elements_by_tag_name_ns);
PHP_FUNCTION(dom_element_has_attribute);
PHP_FUNCTION(dom_element_has_attribute_ns);
PHP_FUNCTION(dom_element_set_id_attribute);
PHP_FUNCTION(dom_element_set_id_attribute_ns);
PHP_FUNCTION(dom_element_set_id_attribute_node);
PHP_METHOD(domelement, __construct);

/* domtext methods */
PHP_FUNCTION(dom_text_split_text);
PHP_FUNCTION(dom_text_is_whitespace_in_element_content);
PHP_FUNCTION(dom_text_replace_whole_text);
PHP_METHOD(domtext, __construct);

/* domcomment methods */
PHP_METHOD(domcomment, __construct);

/* domtypeinfo methods */

/* domuserdatahandler methods */
PHP_FUNCTION(dom_userdatahandler_handle);

/* domdomerror methods */

/* domerrorhandler methods */
PHP_FUNCTION(dom_domerrorhandler_handle_error);

/* domlocator methods */

/* domconfiguration methods */
PHP_FUNCTION(dom_domconfiguration_set_parameter);
PHP_FUNCTION(dom_domconfiguration_get_parameter);
PHP_FUNCTION(dom_domconfiguration_can_set_parameter);

/* domcdatasection methods */
PHP_METHOD(domcdatasection, __construct);

/* domdocumenttype methods */

/* domnotation methods */

/* domentity methods */

/* domentityreference methods */
PHP_METHOD(domentityreference, __construct);

/* domprocessinginstruction methods */
PHP_METHOD(domprocessinginstruction, __construct);

/* string_extend methods */
PHP_FUNCTION(dom_string_extend_find_offset16);
PHP_FUNCTION(dom_string_extend_find_offset32);

#if defined(LIBXML_XPATH_ENABLED)
/* xpath methods */
PHP_METHOD(domxpath, __construct);
PHP_FUNCTION(dom_xpath_register_ns);
PHP_FUNCTION(dom_xpath_query);
PHP_FUNCTION(dom_xpath_evaluate);
PHP_FUNCTION(dom_xpath_register_php_functions);
#endif

#endif /* DOM_FE_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
