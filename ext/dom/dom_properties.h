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
   | Authors: Christian Stocker <chregu@php.net>                          |
   |          Rob Richards <rrichards@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifndef DOM_PROPERTIES_H
#define DOM_PROPERTIES_H

/* attr properties */
zend_result dom_attr_name_read(dom_object *obj, zval *retval);
zend_result dom_attr_specified_read(dom_object *obj, zval *retval);
zend_result dom_attr_value_read(dom_object *obj, zval *retval);
zend_result dom_attr_value_write(dom_object *obj, zval *newval);
zend_result dom_attr_owner_element_read(dom_object *obj, zval *retval);
zend_result dom_attr_schema_type_info_read(dom_object *obj, zval *retval);

/* characterdata properties */
zend_result dom_characterdata_data_read(dom_object *obj, zval *retval);
zend_result dom_characterdata_data_write(dom_object *obj, zval *newval);
zend_result dom_characterdata_length_read(dom_object *obj, zval *retval);

/* document properties */
zend_result dom_document_doctype_read(dom_object *obj, zval *retval);
zend_result dom_document_implementation_read(dom_object *obj, zval *retval);
zend_result dom_modern_document_implementation_read(dom_object *obj, zval *retval);
zend_result dom_document_document_element_read(dom_object *obj, zval *retval);
zend_result dom_document_encoding_read(dom_object *obj, zval *retval);
zend_result dom_document_encoding_write(dom_object *obj, zval *newval);
zend_result dom_document_standalone_read(dom_object *obj, zval *retval);
zend_result dom_document_standalone_write(dom_object *obj, zval *newval);
zend_result dom_document_version_read(dom_object *obj, zval *retval);
zend_result dom_document_version_write(dom_object *obj, zval *newval);
zend_result dom_document_strict_error_checking_read(dom_object *obj, zval *retval);
zend_result dom_document_strict_error_checking_write(dom_object *obj, zval *newval);
zend_result dom_document_document_uri_read(dom_object *obj, zval *retval);
zend_result dom_document_document_uri_write(dom_object *obj, zval *newval);
zend_result dom_document_config_read(dom_object *obj, zval *retval);
zend_result dom_document_format_output_read(dom_object *obj, zval *retval);
zend_result dom_document_format_output_write(dom_object *obj, zval *newval);
zend_result dom_document_validate_on_parse_read(dom_object *obj, zval *retval);
zend_result dom_document_validate_on_parse_write(dom_object *obj, zval *newval);
zend_result dom_document_resolve_externals_read(dom_object *obj, zval *retval);
zend_result dom_document_resolve_externals_write(dom_object *obj, zval *newval);
zend_result dom_document_preserve_whitespace_read(dom_object *obj, zval *retval);
zend_result dom_document_preserve_whitespace_write(dom_object *obj, zval *newval);
zend_result dom_document_recover_read(dom_object *obj, zval *retval);
zend_result dom_document_recover_write(dom_object *obj, zval *newval);
zend_result dom_document_substitue_entities_read(dom_object *obj, zval *retval);
zend_result dom_document_substitue_entities_write(dom_object *obj, zval *newval);

/* html5 document properties */
zend_result dom_html_document_encoding_write(dom_object *obj, zval *retval);
zend_result dom_html_document_body_read(dom_object *obj, zval *retval);
zend_result dom_html_document_body_write(dom_object *obj, zval *newval);
zend_result dom_html_document_head_read(dom_object *obj, zval *retval);
zend_result dom_html_document_title_read(dom_object *obj, zval *retval);
zend_result dom_html_document_title_write(dom_object *obj, zval *newval);

/* documenttype properties */
zend_result dom_documenttype_name_read(dom_object *obj, zval *retval);
zend_result dom_documenttype_entities_read(dom_object *obj, zval *retval);
zend_result dom_documenttype_notations_read(dom_object *obj, zval *retval);
zend_result dom_documenttype_public_id_read(dom_object *obj, zval *retval);
zend_result dom_documenttype_system_id_read(dom_object *obj, zval *retval);
zend_result dom_documenttype_internal_subset_read(dom_object *obj, zval *retval);

/* element properties */
zend_result dom_element_tag_name_read(dom_object *obj, zval *retval);
zend_result dom_element_class_name_read(dom_object *obj, zval *retval);
zend_result dom_element_class_name_write(dom_object *obj, zval *newval);
zend_result dom_element_id_read(dom_object *obj, zval *retval);
zend_result dom_element_id_write(dom_object *obj, zval *newval);
zend_result dom_element_schema_type_info_read(dom_object *obj, zval *retval);
zend_result dom_modern_element_substituted_node_value_read(dom_object *obj, zval *retval);
zend_result dom_modern_element_substituted_node_value_write(dom_object *obj, zval *newval);

/* entity properties */
zend_result dom_entity_public_id_read(dom_object *obj, zval *retval);
zend_result dom_entity_system_id_read(dom_object *obj, zval *retval);
zend_result dom_entity_notation_name_read(dom_object *obj, zval *retval);
zend_result dom_entity_actual_encoding_read(dom_object *obj, zval *retval);
zend_result dom_entity_encoding_read(dom_object *obj, zval *retval);
zend_result dom_entity_version_read(dom_object *obj, zval *retval);

/* entity reference properties */
zend_result dom_entity_reference_child_read(dom_object *obj, zval *retval);
zend_result dom_entity_reference_text_content_read(dom_object *obj, zval *retval);
zend_result dom_entity_reference_child_nodes_read(dom_object *obj, zval *retval);

/* namednodemap properties */
zend_result dom_namednodemap_length_read(dom_object *obj, zval *retval);

/* parent node properties */
zend_result dom_parent_node_first_element_child_read(dom_object *obj, zval *retval);
zend_result dom_parent_node_last_element_child_read(dom_object *obj, zval *retval);
zend_result dom_parent_node_child_element_count(dom_object *obj, zval *retval);

/* node properties */
zend_result dom_node_node_name_read(dom_object *obj, zval *retval);
zend_result dom_node_node_value_read(dom_object *obj, zval *retval);
zend_result dom_node_node_value_write(dom_object *obj, zval *newval);
zend_result dom_node_node_type_read(dom_object *obj, zval *retval);
zend_result dom_node_parent_node_read(dom_object *obj, zval *retval);
zend_result dom_node_parent_element_read(dom_object *obj, zval *retval);
zend_result dom_node_child_nodes_read(dom_object *obj, zval *retval);
zend_result dom_node_first_child_read(dom_object *obj, zval *retval);
zend_result dom_node_last_child_read(dom_object *obj, zval *retval);
zend_result dom_node_previous_sibling_read(dom_object *obj, zval *retval);
zend_result dom_node_next_sibling_read(dom_object *obj, zval *retval);
zend_result dom_node_previous_element_sibling_read(dom_object *obj, zval *retval);
zend_result dom_node_next_element_sibling_read(dom_object *obj, zval *retval);
zend_result dom_node_attributes_read(dom_object *obj, zval *retval);
zend_result dom_node_is_connected_read(dom_object *obj, zval *retval);
zend_result dom_node_owner_document_read(dom_object *obj, zval *retval);
zend_result dom_node_namespace_uri_read(dom_object *obj, zval *retval);
zend_result dom_node_prefix_read(dom_object *obj, zval *retval);
zend_result dom_modern_node_prefix_read(dom_object *obj, zval *retval);
zend_result dom_node_prefix_write(dom_object *obj, zval *newval);
zend_result dom_node_local_name_read(dom_object *obj, zval *retval);
zend_result dom_node_base_uri_read(dom_object *obj, zval *retval);
zend_result dom_node_text_content_read(dom_object *obj, zval *retval);
zend_result dom_node_text_content_write(dom_object *obj, zval *newval);

/* nodelist properties */
zend_result dom_nodelist_length_read(dom_object *obj, zval *retval);

/* notation properties */
zend_result dom_notation_public_id_read(dom_object *obj, zval *retval);
zend_result dom_notation_system_id_read(dom_object *obj, zval *retval);

/* processinginstruction properties */
zend_result dom_processinginstruction_target_read(dom_object *obj, zval *retval);
zend_result dom_processinginstruction_data_read(dom_object *obj, zval *retval);
zend_result dom_processinginstruction_data_write(dom_object *obj, zval *newval);

/* text properties */
zend_result dom_text_whole_text_read(dom_object *obj, zval *retval);

#ifdef LIBXML_XPATH_ENABLED
/* xpath properties */
zend_result dom_xpath_document_read(dom_object *obj, zval *retval);
zend_result dom_xpath_register_node_ns_read(dom_object *obj, zval *retval);
zend_result dom_xpath_register_node_ns_write(dom_object *obj, zval *newval);
#endif

#define DOM_PROP_NODE(type, name, obj) \
	type name = (type) dom_object_get_node(obj); \
	if (UNEXPECTED(name == NULL)) { \
		php_dom_throw_error(INVALID_STATE_ERR, true); \
		return FAILURE; \
	}

#endif /* DOM_PROPERTIES_H */
