/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_xml_parser_create, 0, 0, XmlParser, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_xml_parser_create_ns, 0, 0, XmlParser, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, sep, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xml_set_object, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, parser, XmlParser, 0)
	ZEND_ARG_TYPE_INFO(0, obj, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xml_set_element_handler, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, parser, XmlParser, 0)
	ZEND_ARG_INFO(0, shdl)
	ZEND_ARG_INFO(0, ehdl)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xml_set_character_data_handler, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, parser, XmlParser, 0)
	ZEND_ARG_INFO(0, hdl)
ZEND_END_ARG_INFO()

#define arginfo_xml_set_processing_instruction_handler arginfo_xml_set_character_data_handler

#define arginfo_xml_set_default_handler arginfo_xml_set_character_data_handler

#define arginfo_xml_set_unparsed_entity_decl_handler arginfo_xml_set_character_data_handler

#define arginfo_xml_set_notation_decl_handler arginfo_xml_set_character_data_handler

#define arginfo_xml_set_external_entity_ref_handler arginfo_xml_set_character_data_handler

#define arginfo_xml_set_start_namespace_decl_handler arginfo_xml_set_character_data_handler

#define arginfo_xml_set_end_namespace_decl_handler arginfo_xml_set_character_data_handler

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xml_parse, 0, 2, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, parser, XmlParser, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, isfinal, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xml_parse_into_struct, 0, 3, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, parser, XmlParser, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_INFO(1, values)
	ZEND_ARG_INFO(1, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xml_get_error_code, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, parser, XmlParser, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xml_error_string, 0, 1, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, code, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_xml_get_current_line_number arginfo_xml_get_error_code

#define arginfo_xml_get_current_column_number arginfo_xml_get_error_code

#define arginfo_xml_get_current_byte_index arginfo_xml_get_error_code

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xml_parser_free, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, parser, XmlParser, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xml_parser_set_option, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, parser, XmlParser, 0)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xml_parser_get_option, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, parser, XmlParser, 0)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
ZEND_END_ARG_INFO()
