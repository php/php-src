/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 88c2a02073fc4cebabb4ace0d8db25f9bb017209 */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_tidy_parse_string, 0, 1, tidy, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, config, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_NULL, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_tidy_get_error_buffer, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, tidy, tidy, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_tidy_get_output, 0, 1, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, tidy, tidy, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_tidy_parse_file, 0, 1, tidy, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, config, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_NULL, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, useIncludePath, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_tidy_clean_repair, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, tidy, tidy, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_tidy_repair_string, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, config, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_NULL, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_tidy_repair_file, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, config, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_NULL, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, useIncludePath, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#define arginfo_tidy_diagnose arginfo_tidy_clean_repair

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_tidy_get_release, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_TIDYOPTGETDOC)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_tidy_get_opt_doc, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, tidy, tidy, 0)
	ZEND_ARG_TYPE_INFO(0, option, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_tidy_get_config, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_OBJ_INFO(0, tidy, tidy, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_tidy_get_status, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, tidy, tidy, 0)
ZEND_END_ARG_INFO()

#define arginfo_tidy_get_html_ver arginfo_tidy_get_status

#define arginfo_tidy_is_xhtml arginfo_tidy_clean_repair

#define arginfo_tidy_is_xml arginfo_tidy_clean_repair

#define arginfo_tidy_error_count arginfo_tidy_get_status

#define arginfo_tidy_warning_count arginfo_tidy_get_status

#define arginfo_tidy_access_count arginfo_tidy_get_status

#define arginfo_tidy_config_count arginfo_tidy_get_status

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_tidy_getopt, 0, 2, MAY_BE_STRING|MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_OBJ_INFO(0, tidy, tidy, 0)
	ZEND_ARG_TYPE_INFO(0, option, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_tidy_get_root, 0, 1, tidyNode, 1)
	ZEND_ARG_OBJ_INFO(0, tidy, tidy, 0)
ZEND_END_ARG_INFO()

#define arginfo_tidy_get_html arginfo_tidy_get_root

#define arginfo_tidy_get_head arginfo_tidy_get_root

#define arginfo_tidy_get_body arginfo_tidy_get_root

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_tidy___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, filename, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_MASK(0, config, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_NULL, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, useIncludePath, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_tidy_getOpt, 0, 1, MAY_BE_STRING|MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, option, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_tidy_cleanRepair, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_tidy_parseFile, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, config, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_NULL, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, useIncludePath, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_tidy_parseString, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, config, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_NULL, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_tidy_repairString, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, config, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_NULL, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_tidy_repairFile, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, config, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_NULL, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, useIncludePath, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#define arginfo_class_tidy_diagnose arginfo_class_tidy_cleanRepair

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_tidy_getRelease, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_tidy_getConfig, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_tidy_getStatus, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_tidy_getHtmlVer arginfo_class_tidy_getStatus

#if defined(HAVE_TIDYOPTGETDOC)
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_tidy_getOptDoc, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, option, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#define arginfo_class_tidy_isXhtml arginfo_class_tidy_cleanRepair

#define arginfo_class_tidy_isXml arginfo_class_tidy_cleanRepair

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_tidy_root, 0, 0, tidyNode, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_tidy_head arginfo_class_tidy_root

#define arginfo_class_tidy_html arginfo_class_tidy_root

#define arginfo_class_tidy_body arginfo_class_tidy_root

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_tidyNode___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_tidyNode_hasChildren, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_tidyNode_hasSiblings arginfo_class_tidyNode_hasChildren

#define arginfo_class_tidyNode_isComment arginfo_class_tidyNode_hasChildren

#define arginfo_class_tidyNode_isHtml arginfo_class_tidyNode_hasChildren

#define arginfo_class_tidyNode_isText arginfo_class_tidyNode_hasChildren

#define arginfo_class_tidyNode_isJste arginfo_class_tidyNode_hasChildren

#define arginfo_class_tidyNode_isAsp arginfo_class_tidyNode_hasChildren

#define arginfo_class_tidyNode_isPhp arginfo_class_tidyNode_hasChildren

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_tidyNode_getParent, 0, 0, tidyNode, 1)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(tidy_parse_string);
ZEND_FUNCTION(tidy_get_error_buffer);
ZEND_FUNCTION(tidy_get_output);
ZEND_FUNCTION(tidy_parse_file);
ZEND_FUNCTION(tidy_clean_repair);
ZEND_FUNCTION(tidy_repair_string);
ZEND_FUNCTION(tidy_repair_file);
ZEND_FUNCTION(tidy_diagnose);
ZEND_FUNCTION(tidy_get_release);
#if defined(HAVE_TIDYOPTGETDOC)
ZEND_FUNCTION(tidy_get_opt_doc);
#endif
ZEND_FUNCTION(tidy_get_config);
ZEND_FUNCTION(tidy_get_status);
ZEND_FUNCTION(tidy_get_html_ver);
ZEND_FUNCTION(tidy_is_xhtml);
ZEND_FUNCTION(tidy_is_xml);
ZEND_FUNCTION(tidy_error_count);
ZEND_FUNCTION(tidy_warning_count);
ZEND_FUNCTION(tidy_access_count);
ZEND_FUNCTION(tidy_config_count);
ZEND_FUNCTION(tidy_getopt);
ZEND_FUNCTION(tidy_get_root);
ZEND_FUNCTION(tidy_get_html);
ZEND_FUNCTION(tidy_get_head);
ZEND_FUNCTION(tidy_get_body);
ZEND_METHOD(tidy, __construct);
ZEND_METHOD(tidy, parseFile);
ZEND_METHOD(tidy, parseString);
ZEND_METHOD(tidyNode, __construct);
ZEND_METHOD(tidyNode, hasChildren);
ZEND_METHOD(tidyNode, hasSiblings);
ZEND_METHOD(tidyNode, isComment);
ZEND_METHOD(tidyNode, isHtml);
ZEND_METHOD(tidyNode, isText);
ZEND_METHOD(tidyNode, isJste);
ZEND_METHOD(tidyNode, isAsp);
ZEND_METHOD(tidyNode, isPhp);
ZEND_METHOD(tidyNode, getParent);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(tidy_parse_string, arginfo_tidy_parse_string)
	ZEND_FE(tidy_get_error_buffer, arginfo_tidy_get_error_buffer)
	ZEND_FE(tidy_get_output, arginfo_tidy_get_output)
	ZEND_FE(tidy_parse_file, arginfo_tidy_parse_file)
	ZEND_FE(tidy_clean_repair, arginfo_tidy_clean_repair)
	ZEND_FE(tidy_repair_string, arginfo_tidy_repair_string)
	ZEND_FE(tidy_repair_file, arginfo_tidy_repair_file)
	ZEND_FE(tidy_diagnose, arginfo_tidy_diagnose)
	ZEND_FE(tidy_get_release, arginfo_tidy_get_release)
#if defined(HAVE_TIDYOPTGETDOC)
	ZEND_FE(tidy_get_opt_doc, arginfo_tidy_get_opt_doc)
#endif
	ZEND_FE(tidy_get_config, arginfo_tidy_get_config)
	ZEND_FE(tidy_get_status, arginfo_tidy_get_status)
	ZEND_FE(tidy_get_html_ver, arginfo_tidy_get_html_ver)
	ZEND_FE(tidy_is_xhtml, arginfo_tidy_is_xhtml)
	ZEND_FE(tidy_is_xml, arginfo_tidy_is_xml)
	ZEND_FE(tidy_error_count, arginfo_tidy_error_count)
	ZEND_FE(tidy_warning_count, arginfo_tidy_warning_count)
	ZEND_FE(tidy_access_count, arginfo_tidy_access_count)
	ZEND_FE(tidy_config_count, arginfo_tidy_config_count)
	ZEND_FE(tidy_getopt, arginfo_tidy_getopt)
	ZEND_FE(tidy_get_root, arginfo_tidy_get_root)
	ZEND_FE(tidy_get_html, arginfo_tidy_get_html)
	ZEND_FE(tidy_get_head, arginfo_tidy_get_head)
	ZEND_FE(tidy_get_body, arginfo_tidy_get_body)
	ZEND_FE_END
};


static const zend_function_entry class_tidy_methods[] = {
	ZEND_ME(tidy, __construct, arginfo_class_tidy___construct, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getOpt, tidy_getopt, arginfo_class_tidy_getOpt, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(cleanRepair, tidy_clean_repair, arginfo_class_tidy_cleanRepair, ZEND_ACC_PUBLIC)
	ZEND_ME(tidy, parseFile, arginfo_class_tidy_parseFile, ZEND_ACC_PUBLIC)
	ZEND_ME(tidy, parseString, arginfo_class_tidy_parseString, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(repairString, tidy_repair_string, arginfo_class_tidy_repairString, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(repairFile, tidy_repair_file, arginfo_class_tidy_repairFile, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(diagnose, tidy_diagnose, arginfo_class_tidy_diagnose, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getRelease, tidy_get_release, arginfo_class_tidy_getRelease, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getConfig, tidy_get_config, arginfo_class_tidy_getConfig, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getStatus, tidy_get_status, arginfo_class_tidy_getStatus, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getHtmlVer, tidy_get_html_ver, arginfo_class_tidy_getHtmlVer, ZEND_ACC_PUBLIC)
#if defined(HAVE_TIDYOPTGETDOC)
	ZEND_ME_MAPPING(getOptDoc, tidy_get_opt_doc, arginfo_class_tidy_getOptDoc, ZEND_ACC_PUBLIC)
#endif
	ZEND_ME_MAPPING(isXhtml, tidy_is_xhtml, arginfo_class_tidy_isXhtml, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(isXml, tidy_is_xml, arginfo_class_tidy_isXml, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(root, tidy_get_root, arginfo_class_tidy_root, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(head, tidy_get_head, arginfo_class_tidy_head, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(html, tidy_get_html, arginfo_class_tidy_html, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(body, tidy_get_body, arginfo_class_tidy_body, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_tidyNode_methods[] = {
	ZEND_ME(tidyNode, __construct, arginfo_class_tidyNode___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(tidyNode, hasChildren, arginfo_class_tidyNode_hasChildren, ZEND_ACC_PUBLIC)
	ZEND_ME(tidyNode, hasSiblings, arginfo_class_tidyNode_hasSiblings, ZEND_ACC_PUBLIC)
	ZEND_ME(tidyNode, isComment, arginfo_class_tidyNode_isComment, ZEND_ACC_PUBLIC)
	ZEND_ME(tidyNode, isHtml, arginfo_class_tidyNode_isHtml, ZEND_ACC_PUBLIC)
	ZEND_ME(tidyNode, isText, arginfo_class_tidyNode_isText, ZEND_ACC_PUBLIC)
	ZEND_ME(tidyNode, isJste, arginfo_class_tidyNode_isJste, ZEND_ACC_PUBLIC)
	ZEND_ME(tidyNode, isAsp, arginfo_class_tidyNode_isAsp, ZEND_ACC_PUBLIC)
	ZEND_ME(tidyNode, isPhp, arginfo_class_tidyNode_isPhp, ZEND_ACC_PUBLIC)
	ZEND_ME(tidyNode, getParent, arginfo_class_tidyNode_getParent, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_tidy(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "tidy", class_tidy_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	zval property_errorBuffer_default_value;
	ZVAL_NULL(&property_errorBuffer_default_value);
	zend_string *property_errorBuffer_name = zend_string_init("errorBuffer", sizeof("errorBuffer") - 1, 1);
	zend_declare_typed_property(class_entry, property_errorBuffer_name, &property_errorBuffer_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_errorBuffer_name);

	zval property_value_default_value;
	ZVAL_NULL(&property_value_default_value);
	zend_string *property_value_name = zend_string_init("value", sizeof("value") - 1, 1);
	zend_declare_typed_property(class_entry, property_value_name, &property_value_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_value_name);

	return class_entry;
}

static zend_class_entry *register_class_tidyNode(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "tidyNode", class_tidyNode_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	zval property_value_default_value;
	ZVAL_UNDEF(&property_value_default_value);
	zend_string *property_value_name = zend_string_init("value", sizeof("value") - 1, 1);
	zend_declare_typed_property(class_entry, property_value_name, &property_value_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_value_name);

	zval property_name_default_value;
	ZVAL_UNDEF(&property_name_default_value);
	zend_string *property_name_name = zend_string_init("name", sizeof("name") - 1, 1);
	zend_declare_typed_property(class_entry, property_name_name, &property_name_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_name_name);

	zval property_type_default_value;
	ZVAL_UNDEF(&property_type_default_value);
	zend_string *property_type_name = zend_string_init("type", sizeof("type") - 1, 1);
	zend_declare_typed_property(class_entry, property_type_name, &property_type_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_type_name);

	zval property_line_default_value;
	ZVAL_UNDEF(&property_line_default_value);
	zend_string *property_line_name = zend_string_init("line", sizeof("line") - 1, 1);
	zend_declare_typed_property(class_entry, property_line_name, &property_line_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_line_name);

	zval property_column_default_value;
	ZVAL_UNDEF(&property_column_default_value);
	zend_string *property_column_name = zend_string_init("column", sizeof("column") - 1, 1);
	zend_declare_typed_property(class_entry, property_column_name, &property_column_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_column_name);

	zval property_proprietary_default_value;
	ZVAL_UNDEF(&property_proprietary_default_value);
	zend_string *property_proprietary_name = zend_string_init("proprietary", sizeof("proprietary") - 1, 1);
	zend_declare_typed_property(class_entry, property_proprietary_name, &property_proprietary_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_proprietary_name);

	zval property_id_default_value;
	ZVAL_UNDEF(&property_id_default_value);
	zend_string *property_id_name = zend_string_init("id", sizeof("id") - 1, 1);
	zend_declare_typed_property(class_entry, property_id_name, &property_id_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG|MAY_BE_NULL));
	zend_string_release(property_id_name);

	zval property_attribute_default_value;
	ZVAL_UNDEF(&property_attribute_default_value);
	zend_string *property_attribute_name = zend_string_init("attribute", sizeof("attribute") - 1, 1);
	zend_declare_typed_property(class_entry, property_attribute_name, &property_attribute_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ARRAY|MAY_BE_NULL));
	zend_string_release(property_attribute_name);

	zval property_child_default_value;
	ZVAL_UNDEF(&property_child_default_value);
	zend_string *property_child_name = zend_string_init("child", sizeof("child") - 1, 1);
	zend_declare_typed_property(class_entry, property_child_name, &property_child_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ARRAY|MAY_BE_NULL));
	zend_string_release(property_child_name);

	return class_entry;
}
