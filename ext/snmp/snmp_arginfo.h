/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 5258c5796aca15e369dd72c0a8ed4dc1df31ce9d */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_snmpget, 0, 3, stdClass, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, community, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, object_id, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, retries, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

#define arginfo_snmpgetnext arginfo_snmpget

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_snmpwalk, 0, 3, MAY_BE_ARRAY|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, community, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, object_id, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, retries, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

#define arginfo_snmprealwalk arginfo_snmpwalk

#define arginfo_snmpwalkoid arginfo_snmpwalk

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_snmpset, 0, 5, MAY_BE_ARRAY|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, community, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, object_id, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_MASK(0, type, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_MASK(0, value, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, retries, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_snmp_get_quick_print, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_snmp_set_quick_print, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, enable, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_snmp_set_enum_print arginfo_snmp_set_quick_print

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_snmp_set_oid_output_format, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, format, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_snmp_set_oid_numeric_print arginfo_snmp_set_oid_output_format

#define arginfo_snmp2_get arginfo_snmpget

#define arginfo_snmp2_getnext arginfo_snmpget

#define arginfo_snmp2_walk arginfo_snmpwalk

#define arginfo_snmp2_real_walk arginfo_snmpwalk

#define arginfo_snmp2_set arginfo_snmpset

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_snmp3_get, 0, 8, stdClass, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, security_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, security_level, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, auth_protocol, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, auth_passphrase, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, privacy_protocol, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, privacy_passphrase, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, object_id, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, retries, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

#define arginfo_snmp3_getnext arginfo_snmp3_get

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_snmp3_walk, 0, 8, MAY_BE_ARRAY|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, security_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, security_level, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, auth_protocol, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, auth_passphrase, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, privacy_protocol, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, privacy_passphrase, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, object_id, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, retries, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

#define arginfo_snmp3_real_walk arginfo_snmp3_walk

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_snmp3_set, 0, 10, MAY_BE_ARRAY|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, security_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, security_level, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, auth_protocol, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, auth_passphrase, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, privacy_protocol, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, privacy_passphrase, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, object_id, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_MASK(0, type, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_MASK(0, value, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, retries, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_snmp_set_valueretrieval, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, method, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_snmp_get_valueretrieval, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_snmp_read_mib, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SNMP___construct, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, version, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, community, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, retries, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SNMP_close, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SNMP_setSecurity, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, securityLevel, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, authProtocol, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, authPassphrase, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, privacyProtocol, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, privacyPassphrase, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, contextName, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, contextEngineId, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SNMP_get, 0, 0, 1)
	ZEND_ARG_TYPE_MASK(0, objectId, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, preserveKeys, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SNMP_getnext, 0, 0, 1)
	ZEND_ARG_TYPE_MASK(0, objectId, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SNMP_walk, 0, 0, 1)
	ZEND_ARG_TYPE_MASK(0, objectId, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, suffixAsKey, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, maxRepetitions, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, nonRepeaters, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SNMP_set, 0, 0, 3)
	ZEND_ARG_TYPE_MASK(0, objectId, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_MASK(0, type, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_MASK(0, value, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

#define arginfo_class_SNMP_getErrno arginfo_class_SNMP_close

#define arginfo_class_SNMP_getError arginfo_class_SNMP_close


ZEND_FUNCTION(snmpget);
ZEND_FUNCTION(snmpgetnext);
ZEND_FUNCTION(snmpwalk);
ZEND_FUNCTION(snmprealwalk);
ZEND_FUNCTION(snmpset);
ZEND_FUNCTION(snmp_get_quick_print);
ZEND_FUNCTION(snmp_set_quick_print);
ZEND_FUNCTION(snmp_set_enum_print);
ZEND_FUNCTION(snmp_set_oid_output_format);
ZEND_FUNCTION(snmp2_get);
ZEND_FUNCTION(snmp2_getnext);
ZEND_FUNCTION(snmp2_walk);
ZEND_FUNCTION(snmp2_real_walk);
ZEND_FUNCTION(snmp2_set);
ZEND_FUNCTION(snmp3_get);
ZEND_FUNCTION(snmp3_getnext);
ZEND_FUNCTION(snmp3_walk);
ZEND_FUNCTION(snmp3_real_walk);
ZEND_FUNCTION(snmp3_set);
ZEND_FUNCTION(snmp_set_valueretrieval);
ZEND_FUNCTION(snmp_get_valueretrieval);
ZEND_FUNCTION(snmp_read_mib);
ZEND_METHOD(SNMP, __construct);
ZEND_METHOD(SNMP, close);
ZEND_METHOD(SNMP, setSecurity);
ZEND_METHOD(SNMP, get);
ZEND_METHOD(SNMP, getnext);
ZEND_METHOD(SNMP, walk);
ZEND_METHOD(SNMP, set);
ZEND_METHOD(SNMP, getErrno);
ZEND_METHOD(SNMP, getError);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(snmpget, arginfo_snmpget)
	ZEND_FE(snmpgetnext, arginfo_snmpgetnext)
	ZEND_FE(snmpwalk, arginfo_snmpwalk)
	ZEND_FE(snmprealwalk, arginfo_snmprealwalk)
	ZEND_FALIAS(snmpwalkoid, snmprealwalk, arginfo_snmpwalkoid)
	ZEND_FE(snmpset, arginfo_snmpset)
	ZEND_FE(snmp_get_quick_print, arginfo_snmp_get_quick_print)
	ZEND_FE(snmp_set_quick_print, arginfo_snmp_set_quick_print)
	ZEND_FE(snmp_set_enum_print, arginfo_snmp_set_enum_print)
	ZEND_FE(snmp_set_oid_output_format, arginfo_snmp_set_oid_output_format)
	ZEND_FALIAS(snmp_set_oid_numeric_print, snmp_set_oid_output_format, arginfo_snmp_set_oid_numeric_print)
	ZEND_FE(snmp2_get, arginfo_snmp2_get)
	ZEND_FE(snmp2_getnext, arginfo_snmp2_getnext)
	ZEND_FE(snmp2_walk, arginfo_snmp2_walk)
	ZEND_FE(snmp2_real_walk, arginfo_snmp2_real_walk)
	ZEND_FE(snmp2_set, arginfo_snmp2_set)
	ZEND_FE(snmp3_get, arginfo_snmp3_get)
	ZEND_FE(snmp3_getnext, arginfo_snmp3_getnext)
	ZEND_FE(snmp3_walk, arginfo_snmp3_walk)
	ZEND_FE(snmp3_real_walk, arginfo_snmp3_real_walk)
	ZEND_FE(snmp3_set, arginfo_snmp3_set)
	ZEND_FE(snmp_set_valueretrieval, arginfo_snmp_set_valueretrieval)
	ZEND_FE(snmp_get_valueretrieval, arginfo_snmp_get_valueretrieval)
	ZEND_FE(snmp_read_mib, arginfo_snmp_read_mib)
	ZEND_FE_END
};


static const zend_function_entry class_SNMP_methods[] = {
	ZEND_ME(SNMP, __construct, arginfo_class_SNMP___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(SNMP, close, arginfo_class_SNMP_close, ZEND_ACC_PUBLIC)
	ZEND_ME(SNMP, setSecurity, arginfo_class_SNMP_setSecurity, ZEND_ACC_PUBLIC)
	ZEND_ME(SNMP, get, arginfo_class_SNMP_get, ZEND_ACC_PUBLIC)
	ZEND_ME(SNMP, getnext, arginfo_class_SNMP_getnext, ZEND_ACC_PUBLIC)
	ZEND_ME(SNMP, walk, arginfo_class_SNMP_walk, ZEND_ACC_PUBLIC)
	ZEND_ME(SNMP, set, arginfo_class_SNMP_set, ZEND_ACC_PUBLIC)
	ZEND_ME(SNMP, getErrno, arginfo_class_SNMP_getErrno, ZEND_ACC_PUBLIC)
	ZEND_ME(SNMP, getError, arginfo_class_SNMP_getError, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_SNMPException_methods[] = {
	ZEND_FE_END
};

static zend_class_entry *register_class_SNMP(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SNMP", class_SNMP_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	zval property_info_default_value;
	ZVAL_UNDEF(&property_info_default_value);
	zend_string *property_info_name = zend_string_init("info", sizeof("info") - 1, 1);
	zend_declare_typed_property(class_entry, property_info_name, &property_info_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ARRAY));
	zend_string_release(property_info_name);

	zval property_max_oids_default_value;
	ZVAL_UNDEF(&property_max_oids_default_value);
	zend_string *property_max_oids_name = zend_string_init("max_oids", sizeof("max_oids") - 1, 1);
	zend_declare_typed_property(class_entry, property_max_oids_name, &property_max_oids_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG|MAY_BE_NULL));
	zend_string_release(property_max_oids_name);

	zval property_valueretrieval_default_value;
	ZVAL_UNDEF(&property_valueretrieval_default_value);
	zend_string *property_valueretrieval_name = zend_string_init("valueretrieval", sizeof("valueretrieval") - 1, 1);
	zend_declare_typed_property(class_entry, property_valueretrieval_name, &property_valueretrieval_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_valueretrieval_name);

	zval property_quick_print_default_value;
	ZVAL_UNDEF(&property_quick_print_default_value);
	zend_string *property_quick_print_name = zend_string_init("quick_print", sizeof("quick_print") - 1, 1);
	zend_declare_typed_property(class_entry, property_quick_print_name, &property_quick_print_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_quick_print_name);

	zval property_enum_print_default_value;
	ZVAL_UNDEF(&property_enum_print_default_value);
	zend_string *property_enum_print_name = zend_string_init("enum_print", sizeof("enum_print") - 1, 1);
	zend_declare_typed_property(class_entry, property_enum_print_name, &property_enum_print_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_enum_print_name);

	zval property_oid_output_format_default_value;
	ZVAL_UNDEF(&property_oid_output_format_default_value);
	zend_string *property_oid_output_format_name = zend_string_init("oid_output_format", sizeof("oid_output_format") - 1, 1);
	zend_declare_typed_property(class_entry, property_oid_output_format_name, &property_oid_output_format_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_oid_output_format_name);

	zval property_oid_increasing_check_default_value;
	ZVAL_UNDEF(&property_oid_increasing_check_default_value);
	zend_string *property_oid_increasing_check_name = zend_string_init("oid_increasing_check", sizeof("oid_increasing_check") - 1, 1);
	zend_declare_typed_property(class_entry, property_oid_increasing_check_name, &property_oid_increasing_check_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_oid_increasing_check_name);

	zval property_exceptions_enabled_default_value;
	ZVAL_UNDEF(&property_exceptions_enabled_default_value);
	zend_string *property_exceptions_enabled_name = zend_string_init("exceptions_enabled", sizeof("exceptions_enabled") - 1, 1);
	zend_declare_typed_property(class_entry, property_exceptions_enabled_name, &property_exceptions_enabled_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_exceptions_enabled_name);

	return class_entry;
}

static zend_class_entry *register_class_SNMPException(zend_class_entry *class_entry_RuntimeException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SNMPException", class_SNMPException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_RuntimeException);

	return class_entry;
}
