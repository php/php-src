/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: a79a697fa8c1ab2513bde03e0c2367d0caaec7d8 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_snmpget, 0, 3, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, community, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, object_id, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, retries, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

#define arginfo_snmpgetnext arginfo_snmpget

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_snmpwalk, 0, 3, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, community, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, object_id, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, retries, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

#define arginfo_snmprealwalk arginfo_snmpwalk

#define arginfo_snmpwalkoid arginfo_snmpwalk

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_snmpset, 0, 5, _IS_BOOL, 0)
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

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_snmp3_get, 0, 8, IS_MIXED, 0)
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

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_snmp3_walk, 0, 8, MAY_BE_ARRAY|MAY_BE_FALSE)
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

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_snmp3_set, 0, 10, _IS_BOOL, 0)
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

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SNMP_close, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SNMP_setSecurity, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, securityLevel, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, authProtocol, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, authPassphrase, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, privacyProtocol, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, privacyPassphrase, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, contextName, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, contextEngineId, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SNMP_get, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_MASK(0, objectId, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, preserveKeys, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SNMP_getnext, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_MASK(0, objectId, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_SNMP_walk, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_MASK(0, objectId, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, suffixAsKey, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, maxRepetitions, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, nonRepeaters, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SNMP_set, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_MASK(0, objectId, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_MASK(0, type, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_MASK(0, value, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SNMP_getErrno, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SNMP_getError, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()


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

static void register_snmp_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("SNMP_OID_OUTPUT_SUFFIX", NETSNMP_OID_OUTPUT_SUFFIX, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_OID_OUTPUT_MODULE", NETSNMP_OID_OUTPUT_MODULE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_OID_OUTPUT_FULL", NETSNMP_OID_OUTPUT_FULL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_OID_OUTPUT_NUMERIC", NETSNMP_OID_OUTPUT_NUMERIC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_OID_OUTPUT_UCD", NETSNMP_OID_OUTPUT_UCD, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_OID_OUTPUT_NONE", NETSNMP_OID_OUTPUT_NONE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_VALUE_LIBRARY", SNMP_VALUE_LIBRARY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_VALUE_PLAIN", SNMP_VALUE_PLAIN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_VALUE_OBJECT", SNMP_VALUE_OBJECT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_BIT_STR", ASN_BIT_STR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_OCTET_STR", ASN_OCTET_STR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_OPAQUE", ASN_OPAQUE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_NULL", ASN_NULL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_OBJECT_ID", ASN_OBJECT_ID, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_IPADDRESS", ASN_IPADDRESS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_COUNTER", ASN_GAUGE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_UNSIGNED", ASN_UNSIGNED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_TIMETICKS", ASN_TIMETICKS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_UINTEGER", ASN_UINTEGER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_INTEGER", ASN_INTEGER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_COUNTER64", ASN_COUNTER64, CONST_PERSISTENT);
}

static zend_class_entry *register_class_SNMP(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SNMP", class_SNMP_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	zval const_VERSION_1_value;
	ZVAL_LONG(&const_VERSION_1_value, SNMP_VERSION_1);
	zend_string *const_VERSION_1_name = zend_string_init_interned("VERSION_1", sizeof("VERSION_1") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_VERSION_1_name, &const_VERSION_1_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_VERSION_1_name);

	zval const_VERSION_2c_value;
	ZVAL_LONG(&const_VERSION_2c_value, SNMP_VERSION_2c);
	zend_string *const_VERSION_2c_name = zend_string_init_interned("VERSION_2c", sizeof("VERSION_2c") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_VERSION_2c_name, &const_VERSION_2c_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_VERSION_2c_name);

	zval const_VERSION_2C_value;
	ZVAL_LONG(&const_VERSION_2C_value, SNMP_VERSION_2c);
	zend_string *const_VERSION_2C_name = zend_string_init_interned("VERSION_2C", sizeof("VERSION_2C") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_VERSION_2C_name, &const_VERSION_2C_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_VERSION_2C_name);

	zval const_VERSION_3_value;
	ZVAL_LONG(&const_VERSION_3_value, SNMP_VERSION_3);
	zend_string *const_VERSION_3_name = zend_string_init_interned("VERSION_3", sizeof("VERSION_3") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_VERSION_3_name, &const_VERSION_3_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_VERSION_3_name);

	zval const_ERRNO_NOERROR_value;
	ZVAL_LONG(&const_ERRNO_NOERROR_value, PHP_SNMP_ERRNO_NOERROR);
	zend_string *const_ERRNO_NOERROR_name = zend_string_init_interned("ERRNO_NOERROR", sizeof("ERRNO_NOERROR") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_ERRNO_NOERROR_name, &const_ERRNO_NOERROR_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_ERRNO_NOERROR_name);

	zval const_ERRNO_ANY_value;
	ZVAL_LONG(&const_ERRNO_ANY_value, PHP_SNMP_ERRNO_ANY);
	zend_string *const_ERRNO_ANY_name = zend_string_init_interned("ERRNO_ANY", sizeof("ERRNO_ANY") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_ERRNO_ANY_name, &const_ERRNO_ANY_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_ERRNO_ANY_name);

	zval const_ERRNO_GENERIC_value;
	ZVAL_LONG(&const_ERRNO_GENERIC_value, PHP_SNMP_ERRNO_GENERIC);
	zend_string *const_ERRNO_GENERIC_name = zend_string_init_interned("ERRNO_GENERIC", sizeof("ERRNO_GENERIC") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_ERRNO_GENERIC_name, &const_ERRNO_GENERIC_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_ERRNO_GENERIC_name);

	zval const_ERRNO_TIMEOUT_value;
	ZVAL_LONG(&const_ERRNO_TIMEOUT_value, PHP_SNMP_ERRNO_TIMEOUT);
	zend_string *const_ERRNO_TIMEOUT_name = zend_string_init_interned("ERRNO_TIMEOUT", sizeof("ERRNO_TIMEOUT") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_ERRNO_TIMEOUT_name, &const_ERRNO_TIMEOUT_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_ERRNO_TIMEOUT_name);

	zval const_ERRNO_ERROR_IN_REPLY_value;
	ZVAL_LONG(&const_ERRNO_ERROR_IN_REPLY_value, PHP_SNMP_ERRNO_ERROR_IN_REPLY);
	zend_string *const_ERRNO_ERROR_IN_REPLY_name = zend_string_init_interned("ERRNO_ERROR_IN_REPLY", sizeof("ERRNO_ERROR_IN_REPLY") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_ERRNO_ERROR_IN_REPLY_name, &const_ERRNO_ERROR_IN_REPLY_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_ERRNO_ERROR_IN_REPLY_name);

	zval const_ERRNO_OID_NOT_INCREASING_value;
	ZVAL_LONG(&const_ERRNO_OID_NOT_INCREASING_value, PHP_SNMP_ERRNO_OID_NOT_INCREASING);
	zend_string *const_ERRNO_OID_NOT_INCREASING_name = zend_string_init_interned("ERRNO_OID_NOT_INCREASING", sizeof("ERRNO_OID_NOT_INCREASING") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_ERRNO_OID_NOT_INCREASING_name, &const_ERRNO_OID_NOT_INCREASING_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_ERRNO_OID_NOT_INCREASING_name);

	zval const_ERRNO_OID_PARSING_ERROR_value;
	ZVAL_LONG(&const_ERRNO_OID_PARSING_ERROR_value, PHP_SNMP_ERRNO_OID_PARSING_ERROR);
	zend_string *const_ERRNO_OID_PARSING_ERROR_name = zend_string_init_interned("ERRNO_OID_PARSING_ERROR", sizeof("ERRNO_OID_PARSING_ERROR") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_ERRNO_OID_PARSING_ERROR_name, &const_ERRNO_OID_PARSING_ERROR_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_ERRNO_OID_PARSING_ERROR_name);

	zval const_ERRNO_MULTIPLE_SET_QUERIES_value;
	ZVAL_LONG(&const_ERRNO_MULTIPLE_SET_QUERIES_value, PHP_SNMP_ERRNO_MULTIPLE_SET_QUERIES);
	zend_string *const_ERRNO_MULTIPLE_SET_QUERIES_name = zend_string_init_interned("ERRNO_MULTIPLE_SET_QUERIES", sizeof("ERRNO_MULTIPLE_SET_QUERIES") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_ERRNO_MULTIPLE_SET_QUERIES_name, &const_ERRNO_MULTIPLE_SET_QUERIES_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_ERRNO_MULTIPLE_SET_QUERIES_name);

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
