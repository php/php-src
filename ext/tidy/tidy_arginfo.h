/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_tidy_parse_string, 0, 1, tidy, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, input, IS_STRING, 0)
	ZEND_ARG_INFO(0, config_options)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_tidy_get_error_buffer, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, object, tidy, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_tidy_get_output, 0, 1, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, object, tidy, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_tidy_parse_file, 0, 1, tidy, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, file, IS_STRING, 0)
	ZEND_ARG_INFO(0, config_options)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, use_include_path, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_tidy_clean_repair, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, object, tidy, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_tidy_repair_string, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_INFO(0, config_options)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_tidy_repair_file, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_INFO(0, config_options)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, use_include_path, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_tidy_diagnose arginfo_tidy_clean_repair

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_tidy_get_release, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if HAVE_TIDYOPTGETDOC
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_tidy_get_opt_doc, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, object, tidy, 0)
	ZEND_ARG_TYPE_INFO(0, optname, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_tidy_get_config, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_OBJ_INFO(0, object, tidy, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_tidy_get_status, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, object, tidy, 0)
ZEND_END_ARG_INFO()

#define arginfo_tidy_get_html_ver arginfo_tidy_get_status

#define arginfo_tidy_is_xhtml arginfo_tidy_clean_repair

#define arginfo_tidy_is_xml arginfo_tidy_clean_repair

#define arginfo_tidy_error_count arginfo_tidy_get_status

#define arginfo_tidy_warning_count arginfo_tidy_get_status

#define arginfo_tidy_access_count arginfo_tidy_get_status

#define arginfo_tidy_config_count arginfo_tidy_get_status

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_tidy_getopt, 0, 2, MAY_BE_STRING|MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_OBJ_INFO(0, object, tidy, 0)
	ZEND_ARG_TYPE_INFO(0, option, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_tidy_get_root, 0, 1, tidyNode, 1)
	ZEND_ARG_OBJ_INFO(0, object, tidy, 0)
ZEND_END_ARG_INFO()

#define arginfo_tidy_get_html arginfo_tidy_get_root

#define arginfo_tidy_get_head arginfo_tidy_get_root

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_tidy_get_body, 0, 1, tidyNode, 1)
	ZEND_ARG_OBJ_INFO(0, tidy, tidy, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_tidy___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_INFO(0, config_options)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, use_include_path, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_tidy_getOpt, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, option, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_tidy_cleanRepair, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_tidy_parseFile, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, file, IS_STRING, 0)
	ZEND_ARG_INFO(0, config_options)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, use_include_path, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_tidy_parseString, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, input, IS_STRING, 0)
	ZEND_ARG_INFO(0, config_options)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_tidy_repairString, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_INFO(0, config_options)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_tidy_repairFile, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_INFO(0, config_options)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, use_include_path, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_tidy_diagnose arginfo_class_tidy_cleanRepair

#define arginfo_class_tidy_getRelease arginfo_tidy_get_release

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_tidy_getConfig, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_tidy_getStatus, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_tidy_getHtmlVer arginfo_class_tidy_getStatus

#if HAVE_TIDYOPTGETDOC
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_tidy_getOptDoc, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, optname, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#define arginfo_class_tidy_isXhtml arginfo_class_tidy_cleanRepair

#define arginfo_class_tidy_isXml arginfo_class_tidy_cleanRepair

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_tidy_root, 0, 0, tidyNode, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_tidy_head arginfo_class_tidy_root

#define arginfo_class_tidy_html arginfo_class_tidy_root

#define arginfo_class_tidy_body arginfo_class_tidy_root

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_tidyNode___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_tidyNode_hasChildren arginfo_class_tidy_cleanRepair

#define arginfo_class_tidyNode_hasSiblings arginfo_class_tidy_cleanRepair

#define arginfo_class_tidyNode_isComment arginfo_class_tidy_cleanRepair

#define arginfo_class_tidyNode_isHtml arginfo_class_tidy_cleanRepair

#define arginfo_class_tidyNode_isText arginfo_class_tidy_cleanRepair

#define arginfo_class_tidyNode_isJste arginfo_class_tidy_cleanRepair

#define arginfo_class_tidyNode_isAsp arginfo_class_tidy_cleanRepair

#define arginfo_class_tidyNode_isPhp arginfo_class_tidy_cleanRepair

#define arginfo_class_tidyNode_getParent arginfo_class_tidy_root
