/* This is a generated file, edit the .stub.php file instead. */

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
	ZEND_ARG_TYPE_INFO(0, auth_protocol, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, auth_passphrase, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, priv_protocol, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, priv_passphrase, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, contextName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, contextEngineID, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SNMP_get, 0, 0, 1)
	ZEND_ARG_INFO(0, object_id)
	ZEND_ARG_TYPE_INFO(0, use_orignames, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SNMP_getnext, 0, 0, 1)
	ZEND_ARG_INFO(0, object_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SNMP_walk, 0, 0, 1)
	ZEND_ARG_INFO(0, object_id)
	ZEND_ARG_TYPE_INFO(0, suffix_keys, _IS_BOOL, 0)
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
