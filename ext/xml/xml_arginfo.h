/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: d42215062c41775bae538cd310bc60e63fa06a8e */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_xml_parser_create, 0, 0, XMLParser, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_xml_parser_create_ns, 0, 0, XMLParser, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, separator, IS_STRING, 0, "\":\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xml_set_object, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, parser, XMLParser, 0)
	ZEND_ARG_TYPE_INFO(0, object, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xml_set_element_handler, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, parser, XMLParser, 0)
	ZEND_ARG_INFO(0, start_handler)
	ZEND_ARG_INFO(0, end_handler)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xml_set_character_data_handler, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, parser, XMLParser, 0)
	ZEND_ARG_INFO(0, handler)
ZEND_END_ARG_INFO()

#define arginfo_xml_set_processing_instruction_handler arginfo_xml_set_character_data_handler

#define arginfo_xml_set_default_handler arginfo_xml_set_character_data_handler

#define arginfo_xml_set_unparsed_entity_decl_handler arginfo_xml_set_character_data_handler

#define arginfo_xml_set_notation_decl_handler arginfo_xml_set_character_data_handler

#define arginfo_xml_set_external_entity_ref_handler arginfo_xml_set_character_data_handler

#define arginfo_xml_set_start_namespace_decl_handler arginfo_xml_set_character_data_handler

#define arginfo_xml_set_end_namespace_decl_handler arginfo_xml_set_character_data_handler

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xml_parse, 0, 2, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, parser, XMLParser, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, is_final, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xml_parse_into_struct, 0, 3, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, parser, XMLParser, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_INFO(1, values)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, index, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xml_get_error_code, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, parser, XMLParser, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xml_error_string, 0, 1, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, error_code, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_xml_get_current_line_number arginfo_xml_get_error_code

#define arginfo_xml_get_current_column_number arginfo_xml_get_error_code

#define arginfo_xml_get_current_byte_index arginfo_xml_get_error_code

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xml_parser_free, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, parser, XMLParser, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xml_parser_set_option, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, parser, XMLParser, 0)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_xml_parser_get_option, 0, 2, MAY_BE_STRING|MAY_BE_LONG)
	ZEND_ARG_OBJ_INFO(0, parser, XMLParser, 0)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(xml_parser_create);
ZEND_FUNCTION(xml_parser_create_ns);
ZEND_FUNCTION(xml_set_object);
ZEND_FUNCTION(xml_set_element_handler);
ZEND_FUNCTION(xml_set_character_data_handler);
ZEND_FUNCTION(xml_set_processing_instruction_handler);
ZEND_FUNCTION(xml_set_default_handler);
ZEND_FUNCTION(xml_set_unparsed_entity_decl_handler);
ZEND_FUNCTION(xml_set_notation_decl_handler);
ZEND_FUNCTION(xml_set_external_entity_ref_handler);
ZEND_FUNCTION(xml_set_start_namespace_decl_handler);
ZEND_FUNCTION(xml_set_end_namespace_decl_handler);
ZEND_FUNCTION(xml_parse);
ZEND_FUNCTION(xml_parse_into_struct);
ZEND_FUNCTION(xml_get_error_code);
ZEND_FUNCTION(xml_error_string);
ZEND_FUNCTION(xml_get_current_line_number);
ZEND_FUNCTION(xml_get_current_column_number);
ZEND_FUNCTION(xml_get_current_byte_index);
ZEND_FUNCTION(xml_parser_free);
ZEND_FUNCTION(xml_parser_set_option);
ZEND_FUNCTION(xml_parser_get_option);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(xml_parser_create, arginfo_xml_parser_create)
	ZEND_FE(xml_parser_create_ns, arginfo_xml_parser_create_ns)
	ZEND_FE(xml_set_object, arginfo_xml_set_object)
	ZEND_FE(xml_set_element_handler, arginfo_xml_set_element_handler)
	ZEND_FE(xml_set_character_data_handler, arginfo_xml_set_character_data_handler)
	ZEND_FE(xml_set_processing_instruction_handler, arginfo_xml_set_processing_instruction_handler)
	ZEND_FE(xml_set_default_handler, arginfo_xml_set_default_handler)
	ZEND_FE(xml_set_unparsed_entity_decl_handler, arginfo_xml_set_unparsed_entity_decl_handler)
	ZEND_FE(xml_set_notation_decl_handler, arginfo_xml_set_notation_decl_handler)
	ZEND_FE(xml_set_external_entity_ref_handler, arginfo_xml_set_external_entity_ref_handler)
	ZEND_FE(xml_set_start_namespace_decl_handler, arginfo_xml_set_start_namespace_decl_handler)
	ZEND_FE(xml_set_end_namespace_decl_handler, arginfo_xml_set_end_namespace_decl_handler)
	ZEND_FE(xml_parse, arginfo_xml_parse)
	ZEND_FE(xml_parse_into_struct, arginfo_xml_parse_into_struct)
	ZEND_FE(xml_get_error_code, arginfo_xml_get_error_code)
	ZEND_FE(xml_error_string, arginfo_xml_error_string)
	ZEND_FE(xml_get_current_line_number, arginfo_xml_get_current_line_number)
	ZEND_FE(xml_get_current_column_number, arginfo_xml_get_current_column_number)
	ZEND_FE(xml_get_current_byte_index, arginfo_xml_get_current_byte_index)
	ZEND_FE(xml_parser_free, arginfo_xml_parser_free)
	ZEND_FE(xml_parser_set_option, arginfo_xml_parser_set_option)
	ZEND_FE(xml_parser_get_option, arginfo_xml_parser_get_option)
	ZEND_FE_END
};


static const zend_function_entry class_XMLParser_methods[] = {
	ZEND_FE_END
};
