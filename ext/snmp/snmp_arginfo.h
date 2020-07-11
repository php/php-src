/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 2b0a8233a01de36e93c025bc40c3a2f707825595 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_snmpget, 0, 3, MAY_BE_ARRAY|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, host, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, community, IS_STRING, 0)
	ZEND_ARG_INFO(0, object_id)
	ZEND_ARG_TYPE_INFO(0, timeout, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, retries, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_snmpgetnext arginfo_snmpget

#define arginfo_snmpwalk arginfo_snmpget

#define arginfo_snmprealwalk arginfo_snmpget

#define arginfo_snmpwalkoid arginfo_snmpget

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_snmpset, 0, 5, MAY_BE_ARRAY|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, host, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, community, IS_STRING, 0)
	ZEND_ARG_INFO(0, object_id)
	ZEND_ARG_INFO(0, type)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_TYPE_INFO(0, timeout, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, retries, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_snmp_get_quick_print, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_snmp_set_quick_print, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, quick_print, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_snmp_set_enum_print, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, enum_print, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_snmp_set_oid_output_format, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, oid_format, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_snmp_set_oid_numeric_print arginfo_snmp_set_oid_output_format

#define arginfo_snmp2_get arginfo_snmpget

#define arginfo_snmp2_getnext arginfo_snmpget

#define arginfo_snmp2_walk arginfo_snmpget

#define arginfo_snmp2_real_walk arginfo_snmpget

#define arginfo_snmp2_set arginfo_snmpset

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_snmp3_get, 0, 8, MAY_BE_ARRAY|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, host, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, sec_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, sec_level, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, auth_protocol, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, auth_passphrase, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, priv_protocol, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, priv_passphrase, IS_STRING, 0)
	ZEND_ARG_INFO(0, object_id)
	ZEND_ARG_TYPE_INFO(0, timeout, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, retries, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_snmp3_getnext arginfo_snmp3_get

#define arginfo_snmp3_walk arginfo_snmp3_get

#define arginfo_snmp3_real_walk arginfo_snmp3_get

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_snmp3_set, 0, 10, MAY_BE_ARRAY|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, host, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, sec_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, sec_level, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, auth_protocol, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, auth_passphrase, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, priv_protocol, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, priv_passphrase, IS_STRING, 0)
	ZEND_ARG_INFO(0, object_id)
	ZEND_ARG_INFO(0, type)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_TYPE_INFO(0, timeout, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, retries, IS_LONG, 0)
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
	ZEND_ARG_TYPE_INFO(0, host, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, community, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, timeout, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, retries, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SNMP_close, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SNMP_setSecurity, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, sec_level, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, auth_protocol, IS_STRING, 0, "\'\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, auth_passphrase, IS_STRING, 0, "\'\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, priv_protocol, IS_STRING, 0, "\'\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, priv_passphrase, IS_STRING, 0, "\'\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, contextName, IS_STRING, 0, "\'\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, contextEngineID, IS_STRING, 0, "\'\'")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SNMP_get, 0, 0, 1)
	ZEND_ARG_INFO(0, object_id)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, use_orignames, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SNMP_getnext, 0, 0, 1)
	ZEND_ARG_INFO(0, object_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SNMP_walk, 0, 0, 1)
	ZEND_ARG_INFO(0, object_id)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, suffix_keys, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO(0, max_repetitions, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, non_repeaters, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SNMP_set, 0, 0, 3)
	ZEND_ARG_INFO(0, object_id)
	ZEND_ARG_INFO(0, type)
	ZEND_ARG_INFO(0, value)
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
