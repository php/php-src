/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Mike Jackson <mhjack@tscnet.com>                            |
   |          Steven Lawrance <slawrance@technologist.com>                |
   |          Harrie Hazewinkel <harrie@lisanza.net>                      |
   |          Johann Hanne <jonny@nurfuerspam.de>                         |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_snmp.h"

#if HAVE_SNMP

#include <sys/types.h>
#ifdef PHP_WIN32
#include <winsock2.h>
#include <errno.h>
#include <process.h>
#include "win32/time.h"
#elif defined(NETWARE)
#ifdef USE_WINSOCK
#include <novsock2.h>
#else
#include <sys/socket.h>
#endif
#include <errno.h>
#include <sys/timeval.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#ifndef _OSD_POSIX
#include <sys/errno.h>
#else
#include <errno.h>  /* BS2000/OSD uses <errno.h>, not <sys/errno.h> */
#endif
#include <netdb.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifndef __P
#ifdef __GNUC__
#define __P(args) args
#else
#define __P(args) ()
#endif
#endif

#ifdef HAVE_NET_SNMP
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#else
#ifdef HAVE_DEFAULT_STORE_H
#include "default_store.h"
#endif
#include "asn1.h"
#include "snmp_api.h"
#include "snmp_client.h"
#include "snmp_impl.h"
#include "snmp.h"
#include "snmpv3.h"
#include "keytools.h"
#include "parse.h"
#include "mib.h"
#ifndef PHP_WIN32
/* this doesn't appear to be needed under win32 (perhaps at all)
 * and the header file is not present in my UCD-SNMP headers */
# include "version.h"
#endif
#include "transform_oids.h"
#endif
/* Ugly macro, since the length of OIDs in UCD-SNMP and NET-SNMP
 * is different and this way the code is not full of 'ifdef's.
 */
#define OIDSIZE(p) (sizeof(p)/sizeof(oid))

/* For really old ucd-snmp versions.. */
#ifndef HAVE_SNMP_PARSE_OID
#define snmp_parse_oid read_objid
#endif

#define SNMP_VALUE_LIBRARY	0
#define SNMP_VALUE_PLAIN	1
#define SNMP_VALUE_OBJECT	2

ZEND_DECLARE_MODULE_GLOBALS(snmp)
static PHP_GINIT_FUNCTION(snmp);

/* constant - can be shared among threads */
static oid objid_mib[] = {1, 3, 6, 1, 2, 1};

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_snmpget, 0, 0, 3)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, community)
	ZEND_ARG_INFO(0, object_id)
	ZEND_ARG_INFO(0, timeout)
	ZEND_ARG_INFO(0, retries)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_snmpgetnext, 0, 0, 3)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, community)
	ZEND_ARG_INFO(0, object_id)
	ZEND_ARG_INFO(0, timeout)
	ZEND_ARG_INFO(0, retries)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_snmpwalk, 0, 0, 3)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, community)
	ZEND_ARG_INFO(0, object_id)
	ZEND_ARG_INFO(0, timeout)
	ZEND_ARG_INFO(0, retries)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_snmprealwalk, 0, 0, 3)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, community)
	ZEND_ARG_INFO(0, object_id)
	ZEND_ARG_INFO(0, timeout)
	ZEND_ARG_INFO(0, retries)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_snmp_get_quick_print, 0, 0, 1)
	ZEND_ARG_INFO(0, d)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_snmp_set_quick_print, 0, 0, 1)
	ZEND_ARG_INFO(0, quick_print)
ZEND_END_ARG_INFO()

#ifdef HAVE_NET_SNMP
ZEND_BEGIN_ARG_INFO_EX(arginfo_snmp_set_enum_print, 0, 0, 1)
	ZEND_ARG_INFO(0, enum_print)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_snmp_set_oid_output_format, 0, 0, 1)
	ZEND_ARG_INFO(0, oid_format)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_snmpset, 0, 0, 5)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, community)
	ZEND_ARG_INFO(0, object_id)
	ZEND_ARG_INFO(0, type)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, timeout)
	ZEND_ARG_INFO(0, retries)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_snmp2_get, 0, 0, 3)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, community)
	ZEND_ARG_INFO(0, object_id)
	ZEND_ARG_INFO(0, timeout)
	ZEND_ARG_INFO(0, retries)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_snmp2_getnext, 0, 0, 3)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, community)
	ZEND_ARG_INFO(0, object_id)
	ZEND_ARG_INFO(0, timeout)
	ZEND_ARG_INFO(0, retries)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_snmp2_walk, 0, 0, 3)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, community)
	ZEND_ARG_INFO(0, object_id)
	ZEND_ARG_INFO(0, timeout)
	ZEND_ARG_INFO(0, retries)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_snmp2_real_walk, 0, 0, 3)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, community)
	ZEND_ARG_INFO(0, object_id)
	ZEND_ARG_INFO(0, timeout)
	ZEND_ARG_INFO(0, retries)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_snmp2_set, 0, 0, 5)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, community)
	ZEND_ARG_INFO(0, object_id)
	ZEND_ARG_INFO(0, type)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, timeout)
	ZEND_ARG_INFO(0, retries)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_php_snmpv3, 0, 0, 2)
	ZEND_ARG_INFO(0, s)
	ZEND_ARG_INFO(0, st)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_snmp3_get, 0, 0, 8)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, sec_name)
	ZEND_ARG_INFO(0, sec_level)
	ZEND_ARG_INFO(0, auth_protocol)
	ZEND_ARG_INFO(0, auth_passphrase)
	ZEND_ARG_INFO(0, priv_protocol)
	ZEND_ARG_INFO(0, priv_passphrase)
	ZEND_ARG_INFO(0, object_id)
	ZEND_ARG_INFO(0, timeout)
	ZEND_ARG_INFO(0, retries)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_snmp3_getnext, 0, 0, 8)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, sec_name)
	ZEND_ARG_INFO(0, sec_level)
	ZEND_ARG_INFO(0, auth_protocol)
	ZEND_ARG_INFO(0, auth_passphrase)
	ZEND_ARG_INFO(0, priv_protocol)
	ZEND_ARG_INFO(0, priv_passphrase)
	ZEND_ARG_INFO(0, object_id)
	ZEND_ARG_INFO(0, timeout)
	ZEND_ARG_INFO(0, retries)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_snmp3_walk, 0, 0, 8)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, sec_name)
	ZEND_ARG_INFO(0, sec_level)
	ZEND_ARG_INFO(0, auth_protocol)
	ZEND_ARG_INFO(0, auth_passphrase)
	ZEND_ARG_INFO(0, priv_protocol)
	ZEND_ARG_INFO(0, priv_passphrase)
	ZEND_ARG_INFO(0, object_id)
	ZEND_ARG_INFO(0, timeout)
	ZEND_ARG_INFO(0, retries)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_snmp3_real_walk, 0, 0, 8)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, sec_name)
	ZEND_ARG_INFO(0, sec_level)
	ZEND_ARG_INFO(0, auth_protocol)
	ZEND_ARG_INFO(0, auth_passphrase)
	ZEND_ARG_INFO(0, priv_protocol)
	ZEND_ARG_INFO(0, priv_passphrase)
	ZEND_ARG_INFO(0, object_id)
	ZEND_ARG_INFO(0, timeout)
	ZEND_ARG_INFO(0, retries)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_snmp3_set, 0, 0, 10)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, sec_name)
	ZEND_ARG_INFO(0, sec_level)
	ZEND_ARG_INFO(0, auth_protocol)
	ZEND_ARG_INFO(0, auth_passphrase)
	ZEND_ARG_INFO(0, priv_protocol)
	ZEND_ARG_INFO(0, priv_passphrase)
	ZEND_ARG_INFO(0, object_id)
	ZEND_ARG_INFO(0, type)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, timeout)
	ZEND_ARG_INFO(0, retries)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_snmp_set_valueretrieval, 0, 0, 1)
	ZEND_ARG_INFO(0, method)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_snmp_get_valueretrieval, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_snmp_read_mib, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ snmp_functions[]
 */
const zend_function_entry snmp_functions[] = {
	PHP_FE(snmpget, 						arginfo_snmpget)
	PHP_FE(snmpgetnext, 					arginfo_snmpgetnext)
	PHP_FE(snmpwalk, 						arginfo_snmpwalk)
	PHP_FE(snmprealwalk, 					arginfo_snmprealwalk)
	PHP_FALIAS(snmpwalkoid, snmprealwalk, 	arginfo_snmprealwalk)
	PHP_FE(snmp_get_quick_print, 			arginfo_snmp_get_quick_print)
	PHP_FE(snmp_set_quick_print, 			arginfo_snmp_set_quick_print)
#ifdef HAVE_NET_SNMP
	PHP_FE(snmp_set_enum_print, 			arginfo_snmp_set_enum_print)
	PHP_FE(snmp_set_oid_output_format, 		arginfo_snmp_set_oid_output_format)
	PHP_FALIAS(snmp_set_oid_numeric_print, snmp_set_oid_output_format, arginfo_snmp_set_oid_output_format)
#endif
	PHP_FE(snmpset, 				arginfo_snmpset)

	PHP_FE(snmp2_get, 				arginfo_snmp2_get)
	PHP_FE(snmp2_getnext, 			arginfo_snmp2_getnext)
	PHP_FE(snmp2_walk, 				arginfo_snmp2_walk)
	PHP_FE(snmp2_real_walk, 		arginfo_snmp2_real_walk)
	PHP_FE(snmp2_set, 				arginfo_snmp2_set)

	PHP_FE(snmp3_get, 				arginfo_snmp3_get)
	PHP_FE(snmp3_getnext, 			arginfo_snmp3_getnext)
	PHP_FE(snmp3_walk, 				arginfo_snmp3_walk)
	PHP_FE(snmp3_real_walk, 		arginfo_snmp3_real_walk)
	PHP_FE(snmp3_set, 				arginfo_snmp3_set)
	PHP_FE(snmp_set_valueretrieval, arginfo_snmp_set_valueretrieval)
	PHP_FE(snmp_get_valueretrieval, arginfo_snmp_get_valueretrieval)

	PHP_FE(snmp_read_mib, 			arginfo_snmp_read_mib)
	PHP_FE_END
};
/* }}} */

#define SNMP_CMD_GET		1
#define SNMP_CMD_GETNEXT	2
#define SNMP_CMD_WALK		3
#define SNMP_CMD_REALWALK	4
#define SNMP_CMD_SET		11

/* {{{ snmp_module_entry
 */
zend_module_entry snmp_module_entry = {
	STANDARD_MODULE_HEADER,
	"snmp",
	snmp_functions,
	PHP_MINIT(snmp),
	PHP_MSHUTDOWN(snmp),
	NULL,
	NULL,
	PHP_MINFO(snmp),
	NO_VERSION_YET,
	PHP_MODULE_GLOBALS(snmp),
	PHP_GINIT(snmp),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_SNMP
ZEND_GET_MODULE(snmp)
#endif

/* THREAD_LS snmp_module php_snmp_module; - may need one of these at some point */

/* {{{ PHP_GINIT_FUNCTION
 */
static PHP_GINIT_FUNCTION(snmp)
{
	snmp_globals->valueretrieval = SNMP_VALUE_LIBRARY;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(snmp)
{
	init_snmp("snmpapp");

#ifdef NETSNMP_DS_LIB_DONT_PERSIST_STATE
	/* Prevent update of the snmpapp.conf file */
	netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_DONT_PERSIST_STATE, 1);
#endif

#ifdef HAVE_NET_SNMP
	REGISTER_LONG_CONSTANT("SNMP_OID_OUTPUT_FULL", NETSNMP_OID_OUTPUT_FULL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_OID_OUTPUT_NUMERIC", NETSNMP_OID_OUTPUT_NUMERIC, CONST_CS | CONST_PERSISTENT);
#endif

	REGISTER_LONG_CONSTANT("SNMP_VALUE_LIBRARY", SNMP_VALUE_LIBRARY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_VALUE_PLAIN", SNMP_VALUE_PLAIN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_VALUE_OBJECT", SNMP_VALUE_OBJECT, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("SNMP_BIT_STR", ASN_BIT_STR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_OCTET_STR", ASN_OCTET_STR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_OPAQUE", ASN_OPAQUE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_NULL", ASN_NULL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_OBJECT_ID", ASN_OBJECT_ID, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_IPADDRESS", ASN_IPADDRESS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_COUNTER", ASN_GAUGE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_UNSIGNED", ASN_UNSIGNED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_TIMETICKS", ASN_TIMETICKS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_UINTEGER", ASN_UINTEGER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_INTEGER", ASN_INTEGER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_COUNTER64", ASN_COUNTER64, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(snmp)
{
	snmp_shutdown("snmpapp");

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(snmp)
{
	php_info_print_table_start();
#ifdef HAVE_NET_SNMP
	php_info_print_table_row(2, "NET-SNMP Support", "enabled");
	php_info_print_table_row(2, "NET-SNMP Version", netsnmp_get_version());
#else
	php_info_print_table_row(2, "UCD-SNMP Support", "enabled");
	php_info_print_table_row(2, "UCD-SNMP Version", VersionInfo);
#endif
	php_info_print_table_end();
}
/* }}} */

static void php_snmp_getvalue(struct variable_list *vars, zval *snmpval TSRMLS_DC)
{
	zval *val;
#if I64CHARSZ > 2047
	char buf[I64CHARSZ + 1];
#else
	char buf[2048];
#endif

	buf[0] = 0;

	if (SNMP_G(valueretrieval) == SNMP_VALUE_LIBRARY) {
#ifdef HAVE_NET_SNMP
		snprint_value(buf, sizeof(buf), vars->name, vars->name_length, vars);
#else
		sprint_value(buf,vars->name, vars->name_length, vars);
#endif
		ZVAL_STRING(snmpval, buf, 1);
		return;
	}

	MAKE_STD_ZVAL(val);

	switch (vars->type) {
	case ASN_BIT_STR:		/* 0x03, asn1.h */
		ZVAL_STRINGL(val, vars->val.bitstring, vars->val_len, 1);
		break;

	case ASN_OCTET_STR:		/* 0x04, asn1.h */
	case ASN_OPAQUE:		/* 0x44, snmp_impl.h */
		ZVAL_STRINGL(val, vars->val.string, vars->val_len, 1);
		break;

	case ASN_NULL:			/* 0x05, asn1.h */
		ZVAL_NULL(val);
		break;

	case ASN_OBJECT_ID:		/* 0x06, asn1.h */
#ifdef HAVE_NET_SNMP
		snprint_objid(buf, sizeof(buf), vars->val.objid, vars->val_len / sizeof(oid));
#else
		sprint_objid(buf, vars->val.objid, vars->val_len / sizeof(oid));
#endif

		ZVAL_STRING(val, buf, 1);
		break;

	case ASN_IPADDRESS:		/* 0x40, snmp_impl.h */
		snprintf(buf, sizeof(buf)-1, "%d.%d.%d.%d",
		         (vars->val.string)[0], (vars->val.string)[1],
		         (vars->val.string)[2], (vars->val.string)[3]);
		buf[sizeof(buf)-1]=0;
		ZVAL_STRING(val, buf, 1);
		break;

	case ASN_COUNTER:		/* 0x41, snmp_impl.h */
	case ASN_GAUGE:			/* 0x42, snmp_impl.h */
	/* ASN_UNSIGNED is the same as ASN_GAUGE */
	case ASN_TIMETICKS:		/* 0x43, snmp_impl.h */
	case ASN_UINTEGER:		/* 0x47, snmp_impl.h */
		snprintf(buf, sizeof(buf)-1, "%lu", *vars->val.integer);
		buf[sizeof(buf)-1]=0;
		ZVAL_STRING(val, buf, 1);
		break;

	case ASN_INTEGER:		/* 0x02, asn1.h */
		snprintf(buf, sizeof(buf)-1, "%ld", *vars->val.integer);
		buf[sizeof(buf)-1]=0;
		ZVAL_STRING(val, buf, 1);
		break;

	case ASN_COUNTER64:		/* 0x46, snmp_impl.h */
		printU64(buf, vars->val.counter64);
		ZVAL_STRING(val, buf, 1);
		break;

	default:
		ZVAL_STRING(val, "Unknown value type", 1);
		break;
	}

	if (SNMP_G(valueretrieval) == SNMP_VALUE_PLAIN) {
		*snmpval = *val;
		zval_copy_ctor(snmpval);
	} else {
		object_init(snmpval);
		add_property_long(snmpval, "type", vars->type);
		add_property_zval(snmpval, "value", val);
	}
}

/* {{{ php_snmp_internal
*
* Generic SNMP object fetcher (for all SNMP versions)
*
* st=SNMP_CMD_GET   get - query an agent with SNMP-GET.
* st=SNMP_CMD_GETNEXT   getnext - query an agent with SNMP-GETNEXT.
* st=SNMP_CMD_WALK   walk - walk the mib and return a single dimensional array 
*          containing the values.
* st=SNMP_CMD_REALWALK   realwalk() and walkoid() - walk the mib and return an 
*          array of oid,value pairs.
* st=SNMP_CMD_SET  set() - query an agent and set a single value
*
*/
static void php_snmp_internal(INTERNAL_FUNCTION_PARAMETERS, int st, 
							struct snmp_session *session,
							char *objid,
							char type,
							char* value) 
{
	struct snmp_session *ss;
	struct snmp_pdu *pdu=NULL, *response;
	struct variable_list *vars;
	oid name[MAX_NAME_LEN];
	size_t name_length;
	oid root[MAX_NAME_LEN];
	size_t rootlen = 0;
	int gotroot = 0;
	int status, count;
	char buf[2048];
	char buf2[2048];
	int keepwalking=1;
	char *err;
	zval *snmpval = NULL;

	if (st >= SNMP_CMD_WALK) { /* walk */
		rootlen = MAX_NAME_LEN;
		if (strlen(objid)) { /* on a walk, an empty string means top of tree - no error */
			if (snmp_parse_oid(objid, root, &rootlen)) {
				gotroot = 1;
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid object identifier: %s", objid);
			}
		}

		if (!gotroot) {
			memmove((char *) root, (char *) objid_mib, sizeof(objid_mib));
			rootlen = sizeof(objid_mib) / sizeof(oid);
			gotroot = 1;
		}
	}

	if ((ss = snmp_open(session)) == NULL) {
		snmp_error(session, NULL, NULL, &err);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not open snmp connection: %s", err);
		free(err);
		RETURN_FALSE;
	}

	if (st >= SNMP_CMD_WALK) {
		memmove((char *)name, (char *)root, rootlen * sizeof(oid));
		name_length = rootlen;
		switch(st) {
			case SNMP_CMD_WALK:
			case SNMP_CMD_REALWALK:
				array_init(return_value);
				break;
			default:
				RETVAL_TRUE;
				break;
		}
	}

	while (keepwalking) {
		keepwalking = 0;
		if ((st == SNMP_CMD_GET) || (st == SNMP_CMD_GETNEXT)) {
			name_length = MAX_OID_LEN;
			if (!snmp_parse_oid(objid, name, &name_length)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid object identifier: %s", objid);
				snmp_close(ss);
				RETURN_FALSE;
			}
			pdu = snmp_pdu_create((st == SNMP_CMD_GET) ? SNMP_MSG_GET : SNMP_MSG_GETNEXT);
			snmp_add_null_var(pdu, name, name_length);
		} else if (st == SNMP_CMD_SET) {
			pdu = snmp_pdu_create(SNMP_MSG_SET);
			if (snmp_add_var(pdu, name, name_length, type, value)) {
#ifdef HAVE_NET_SNMP
				snprint_objid(buf, sizeof(buf), name, name_length);
#else
				sprint_objid(buf, name, name_length);
#endif
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not add variable: %s %c %s", buf, type, value);
				snmp_free_pdu(pdu);
				snmp_close(ss);
				RETURN_FALSE;
			}
		} else if (st >= SNMP_CMD_WALK) {
			if (session->version == SNMP_VERSION_1) {
				pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
			} else {
				pdu = snmp_pdu_create(SNMP_MSG_GETBULK);
				pdu->non_repeaters = 0;
				pdu->max_repetitions = 20;
			}
			snmp_add_null_var(pdu, name, name_length);
		}

retry:
		status = snmp_synch_response(ss, pdu, &response);
		if (status == STAT_SUCCESS) {
			if (response->errstat == SNMP_ERR_NOERROR) {
				for (vars = response->variables; vars; vars = vars->next_variable) {
					if (st >= SNMP_CMD_WALK && st != SNMP_CMD_SET && 
						(vars->name_length < rootlen || memcmp(root, vars->name, rootlen * sizeof(oid)))) {
						continue;       /* not part of this subtree */
					}

					if (st != SNMP_CMD_SET) {
						MAKE_STD_ZVAL(snmpval);
						php_snmp_getvalue(vars, snmpval TSRMLS_CC);
					}

					if (st == SNMP_CMD_GET) {
						*return_value = *snmpval;
						zval_copy_ctor(return_value);
						zval_ptr_dtor(&snmpval);
						snmp_free_pdu(response);
						snmp_close(ss);
						return;
					} else if (st == SNMP_CMD_GETNEXT) {
						*return_value = *snmpval;
						zval_copy_ctor(return_value);
						snmp_free_pdu(response);
						snmp_close(ss);
						return;
					} else if (st == SNMP_CMD_WALK) {
						add_next_index_zval(return_value,snmpval); /* Add to returned array */
					} else if (st == SNMP_CMD_REALWALK && vars->type != SNMP_ENDOFMIBVIEW && vars->type != SNMP_NOSUCHOBJECT && vars->type != SNMP_NOSUCHINSTANCE) {
#ifdef HAVE_NET_SNMP
						snprint_objid(buf2, sizeof(buf2), vars->name, vars->name_length);
#else
						sprint_objid(buf2, vars->name, vars->name_length);
#endif
						add_assoc_zval(return_value,buf2,snmpval);
					}
					if (st >= SNMP_CMD_WALK && st != SNMP_CMD_SET) {
						if (vars->type != SNMP_ENDOFMIBVIEW && 
							vars->type != SNMP_NOSUCHOBJECT && vars->type != SNMP_NOSUCHINSTANCE) {
							if (snmp_oid_compare(name, name_length, vars->name, vars->name_length) >= 0) {
								php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error: OID not increasing: %s",name);
								keepwalking = 0;
							} else {
								memmove((char *)name, (char *)vars->name,vars->name_length * sizeof(oid));
								name_length = vars->name_length;
								keepwalking = 1;
							}
						}
					}
				}	
			} else {
				if ((st != SNMP_CMD_WALK && st != SNMP_CMD_REALWALK) || response->errstat != SNMP_ERR_NOSUCHNAME) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error in packet: %s", snmp_errstring(response->errstat));
					if (response->errstat == SNMP_ERR_NOSUCHNAME) {
						for (count=1, vars = response->variables; vars && count != response->errindex;
						vars = vars->next_variable, count++);
						if (vars) {
#ifdef HAVE_NET_SNMP
							snprint_objid(buf, sizeof(buf), vars->name, vars->name_length);
#else
							sprint_objid(buf,vars->name, vars->name_length);
#endif
						}
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "This name does not exist: %s",buf);
					}
					if (st == SNMP_CMD_GET) {
						if ((pdu = snmp_fix_pdu(response, SNMP_MSG_GET)) != NULL) {
							snmp_free_pdu(response);
							goto retry;
						}
					} else if (st == SNMP_CMD_SET) {
						if ((pdu = snmp_fix_pdu(response, SNMP_MSG_SET)) != NULL) {
							snmp_free_pdu(response);
							goto retry;
						}
					} else if (st == SNMP_CMD_GETNEXT) {
						if ((pdu = snmp_fix_pdu(response, SNMP_MSG_GETNEXT)) != NULL) {
							snmp_free_pdu(response);
							goto retry;
						}
					} else if (st >= SNMP_CMD_WALK) { /* Here we do walks. */
						if ((pdu = snmp_fix_pdu(response, ((session->version == SNMP_VERSION_1)
										? SNMP_MSG_GETNEXT
										: SNMP_MSG_GETBULK))) != NULL) {
							snmp_free_pdu(response);
							goto retry;
						}
					}
					snmp_free_pdu(response);
					snmp_close(ss);
					if (st == SNMP_CMD_WALK || st == SNMP_CMD_REALWALK) {
						zval_dtor(return_value);
					}
					RETURN_FALSE;
				}
			}
		} else if (status == STAT_TIMEOUT) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "No response from %s", session->peername);
			if (st == SNMP_CMD_WALK || st == SNMP_CMD_REALWALK) {
				zval_dtor(return_value);
			}
			snmp_close(ss);
			RETURN_FALSE;
		} else {    /* status == STAT_ERROR */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "An error occurred, quitting");
			if (st == SNMP_CMD_WALK || st == SNMP_CMD_REALWALK) {
				zval_dtor(return_value);
			}
			snmp_close(ss);
			RETURN_FALSE;
		}
		if (response) {
			snmp_free_pdu(response);
		}
	} /* keepwalking */
	snmp_close(ss);
}
/* }}} */

/* {{{ php_snmp
*
* Generic community based SNMP handler for version 1 and 2.
* This function makes use of the internal SNMP object fetcher.
* The object fetcher is shared with SNMPv3.
*
* st=SNMP_CMD_GET   get - query an agent with SNMP-GET.
* st=SNMP_CMD_GETNEXT   getnext - query an agent with SNMP-GETNEXT.
* st=SNMP_CMD_WALK   walk - walk the mib and return a single dimensional array 
*          containing the values.
* st=SNMP_CMD_REALWALK   realwalk() and walkoid() - walk the mib and return an 
*          array of oid,value pairs.
* st=5-8 ** Reserved **
* st=SNMP_CMD_SET  set() - query an agent and set a single value
*
*/
static void php_snmp(INTERNAL_FUNCTION_PARAMETERS, int st, int version) 
{
	char *a1, *a2, *a3;
	int a1_len, a2_len, a3_len;
	struct snmp_session session;
	long timeout = SNMP_DEFAULT_TIMEOUT;
	long retries = SNMP_DEFAULT_RETRIES;
	char type = (char) 0;
	char *value = (char *) 0, *stype = "";
	int value_len, stype_len;
	char hostname[MAX_NAME_LEN];
	int remote_port = 161;
	char *pptr;
	int argc = ZEND_NUM_ARGS();

	if (st == SNMP_CMD_SET) {
		if (zend_parse_parameters(argc TSRMLS_CC, "sssss|ll", &a1, &a1_len, &a2, &a2_len, &a3, &a3_len, &stype, &stype_len, &value, &value_len, &timeout, &retries) == FAILURE) {
			return;
		}
	} else {
		/* SNMP_CMD_GET
		 * SNMP_CMD_GETNEXT
		 * SNMP_CMD_WALK
		 * SNMP_CMD_REALWALK
		 */
		if (zend_parse_parameters(argc TSRMLS_CC, "sss|ll", &a1, &a1_len, &a2, &a2_len, &a3, &a3_len, &timeout, &retries) == FAILURE) {
			return;
		}
	}
	
	if (st == SNMP_CMD_SET) {
		type = stype[0];
	} 

	snmp_sess_init(&session);
	strlcpy(hostname, a1, sizeof(hostname));
	if ((pptr = strchr (hostname, ':'))) {
		remote_port = strtol (pptr + 1, NULL, 0);
	}

	session.peername = hostname;
	session.remote_port = remote_port;
	session.version = version;
	/*
	* FIXME: potential memory leak
	* This is a workaround for an "artifact" (Mike Slifcak)
	* in (at least) ucd-snmp 3.6.1 which frees
	* memory it did not allocate
	*/
#ifdef UCD_SNMP_HACK
	session.community = (u_char *)strdup(a2); /* memory freed by SNMP library, strdup NOT estrdup */
#else
	session.community = (u_char *)a2;
#endif
	session.community_len = a2_len;
	session.retries = retries;
	session.timeout = timeout;
	
	session.authenticator = NULL;

	php_snmp_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU, st, &session, a3, type, value);
}
/* }}} */

/* {{{ proto string snmpget(string host, string community, string object_id [, int timeout [, int retries]]) 
   Fetch a SNMP object */
PHP_FUNCTION(snmpget)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU,SNMP_CMD_GET, SNMP_VERSION_1);
}
/* }}} */

/* {{{ proto string snmpgetnext(string host, string community, string object_id [, int timeout [, int retries]]) 
   Fetch a SNMP object */
PHP_FUNCTION(snmpgetnext)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU,SNMP_CMD_GETNEXT, SNMP_VERSION_1);
}
/* }}} */

/* {{{ proto array snmpwalk(string host, string community, string object_id [, int timeout [, int retries]]) 
   Return all objects under the specified object id */
PHP_FUNCTION(snmpwalk)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU,SNMP_CMD_WALK, SNMP_VERSION_1);
}
/* }}} */

/* {{{ proto array snmprealwalk(string host, string community, string object_id [, int timeout [, int retries]])
   Return all objects including their respective object id withing the specified one */
PHP_FUNCTION(snmprealwalk)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU,SNMP_CMD_REALWALK, SNMP_VERSION_1);
}
/* }}} */

/* {{{ proto bool snmp_get_quick_print(void)
   Return the current status of quick_print */
PHP_FUNCTION(snmp_get_quick_print)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

#ifdef HAVE_NET_SNMP
	RETURN_BOOL(netsnmp_ds_get_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_QUICK_PRINT));
#else
	RETURN_BOOL(snmp_get_quick_print());
#endif
}
/* }}} */

/* {{{ proto void snmp_set_quick_print(int quick_print)
   Return all objects including their respective object id withing the specified one */
PHP_FUNCTION(snmp_set_quick_print)
{
	long a1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &a1) == FAILURE) {
		return;
	}

#ifdef HAVE_NET_SNMP
	netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_QUICK_PRINT, (int) a1);
#else
	snmp_set_quick_print((int)a1);
#endif
}
/* }}} */

#ifdef HAVE_NET_SNMP
/* {{{ proto void snmp_set_enum_print(int enum_print)
   Return all values that are enums with their enum value instead of the raw integer */
PHP_FUNCTION(snmp_set_enum_print)
{
	long a1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &a1) == FAILURE) {
		return;
	}

	netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_PRINT_NUMERIC_ENUM, (int) a1);
} 
/* }}} */

/* {{{ proto void snmp_set_oid_output_format(int oid_format)
   Set the OID output format. */
PHP_FUNCTION(snmp_set_oid_output_format)
{
	long a1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &a1) == FAILURE) {
		return;
	}

	switch ((int) a1) {
		case 0:
		case NETSNMP_OID_OUTPUT_FULL:
			a1 = NETSNMP_OID_OUTPUT_FULL;
			break;

		default:
		case NETSNMP_OID_OUTPUT_NUMERIC:
			a1 = NETSNMP_OID_OUTPUT_NUMERIC;
			break;
	}

	netsnmp_ds_set_int(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_OID_OUTPUT_FORMAT, a1);
} 
/* }}} */
#endif

/* {{{ proto int snmpset(string host, string community, string object_id, string type, mixed value [, int timeout [, int retries]]) 
   Set the value of a SNMP object */
PHP_FUNCTION(snmpset)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU,SNMP_CMD_SET, SNMP_VERSION_1);
}
/* }}} */

/* {{{ int netsnmp_session_set_sec_name(struct snmp_session *s, char *name)
   Set the security name in the snmpv3 session */
static int netsnmp_session_set_sec_name(struct snmp_session *s, char *name)
{
	if ((s) && (name)) {
		s->securityName = strdup(name);
		s->securityNameLen = strlen(s->securityName);
		return (0);
	}
	return (-1);
}
/* }}} */

/* {{{ int netsnmp_session_set_sec_level(struct snmp_session *s, char *level)
   Set the security level in the snmpv3 session */
static int netsnmp_session_set_sec_level(struct snmp_session *s, char *level TSRMLS_DC)
{
	if ((s) && (level)) {
		if (!strcasecmp(level, "noAuthNoPriv") || !strcasecmp(level, "nanp")) {
			s->securityLevel = SNMP_SEC_LEVEL_NOAUTH;
			return (0);
		} else if (!strcasecmp(level, "authNoPriv") || !strcasecmp(level, "anp")) {
			s->securityLevel = SNMP_SEC_LEVEL_AUTHNOPRIV;
			return (0);
		} else if (!strcasecmp(level, "authPriv") || !strcasecmp(level, "ap")) {
			s->securityLevel = SNMP_SEC_LEVEL_AUTHPRIV;
			return (0);
		}
	}
	return (-1);
}
/* }}} */

/* {{{ int netsnmp_session_set_auth_protocol(struct snmp_session *s, char *prot)
   Set the authentication protocol in the snmpv3 session */
static int netsnmp_session_set_auth_protocol(struct snmp_session *s, char *prot TSRMLS_DC)
{
	if ((s) && (prot)) {
		if (!strcasecmp(prot, "MD5")) {
			s->securityAuthProto = usmHMACMD5AuthProtocol;
			s->securityAuthProtoLen = OIDSIZE(usmHMACMD5AuthProtocol);
			return (0);
		} else if (!strcasecmp(prot, "SHA")) {
			s->securityAuthProto = usmHMACSHA1AuthProtocol;
			s->securityAuthProtoLen = OIDSIZE(usmHMACSHA1AuthProtocol);
			return (0);
		}
	}
	return (-1);
}
/* }}} */

/* {{{ int netsnmp_session_set_sec_protocol(struct snmp_session *s, char *prot)
   Set the security protocol in the snmpv3 session */
static int netsnmp_session_set_sec_protocol(struct snmp_session *s, char *prot TSRMLS_DC)
{
	if ((s) && (prot)) {
		if (!strcasecmp(prot, "DES")) {
			s->securityPrivProto = usmDESPrivProtocol;
			s->securityPrivProtoLen = OIDSIZE(usmDESPrivProtocol);
			return (0);
#ifdef HAVE_AES
		} else if (!strcasecmp(prot, "AES128")
#ifdef SNMP_VALIDATE_ERR
/* 
* In Net-SNMP before 5.2, the following symbols exist:
* usmAES128PrivProtocol, usmAES192PrivProtocol, usmAES256PrivProtocol
* In an effort to be more standards-compliant, 5.2 removed the last two.
* As of 5.2, the symbols are:
* usmAESPrivProtocol, usmAES128PrivProtocol
* 
* As we want this extension to compile on both versions, we use the latter
* symbol on purpose, as it's defined to be the same as the former.
*
* However, in 5.2 the type of usmAES128PrivProtocol is a pointer, not an
* array, so we cannot use the OIDSIZE macro because it uses sizeof().
*
*/
			|| !strcasecmp(prot, "AES")) {
			s->securityPrivProto = usmAES128PrivProtocol;
			s->securityPrivProtoLen = USM_PRIV_PROTO_AES128_LEN;
			return (0);
#else			
		) {
			s->securityPrivProto = usmAES128PrivProtocol;
			s->securityPrivProtoLen = OIDSIZE(usmAES128PrivProtocol);
			return (0);
		} else if (!strcasecmp(prot, "AES192")) {
			s->securityPrivProto = usmAES192PrivProtocol;
			s->securityPrivProtoLen = OIDSIZE(usmAES192PrivProtocol);
			return (0);
		} else if (!strcasecmp(prot, "AES256")) {
			s->securityPrivProto = usmAES256PrivProtocol;
			s->securityPrivProtoLen = OIDSIZE(usmAES256PrivProtocol);
			return (0);
#endif
#endif
		}
	}
	return (-1);
}
/* }}} */

/* {{{ int netsnmp_session_gen_auth_key(struct snmp_session *s, char *pass)
   Make key from pass phrase in the snmpv3 session */
static int netsnmp_session_gen_auth_key(struct snmp_session *s, char *pass TSRMLS_DC)
{
	/*
	 * make master key from pass phrases 
	 */
	if ((s) && (pass) && strlen(pass)) {
		s->securityAuthKeyLen = USM_AUTH_KU_LEN;
		if (s->securityAuthProto == NULL) {
			/* get .conf set default */
			const oid *def = get_default_authtype(&(s->securityAuthProtoLen));
			s->securityAuthProto = snmp_duplicate_objid(def, s->securityAuthProtoLen);
		}
		if (s->securityAuthProto == NULL) {
			/* assume MD5 */
			s->securityAuthProto =
				snmp_duplicate_objid(usmHMACMD5AuthProtocol, OIDSIZE(usmHMACMD5AuthProtocol));
			s->securityAuthProtoLen = OIDSIZE(usmHMACMD5AuthProtocol);
		}
		if (generate_Ku(s->securityAuthProto, s->securityAuthProtoLen,
				(u_char *) pass, strlen(pass),
				s->securityAuthKey, &(s->securityAuthKeyLen)) != SNMPERR_SUCCESS) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error generating a key for authentication pass phrase");
			return (-2);
		}
		return (0);
	}
	return (-1);
}
/* }}} */

/* {{{ int netsnmp_session_gen_sec_key(struct snmp_session *s, u_char *pass)
   Make key from pass phrase in the snmpv3 session */
static int netsnmp_session_gen_sec_key(struct snmp_session *s, u_char *pass TSRMLS_DC)
{
	if ((s) && (pass) && strlen(pass)) {
		s->securityPrivKeyLen = USM_PRIV_KU_LEN;
		if (s->securityPrivProto == NULL) {
			/* get .conf set default */
			const oid *def = get_default_privtype(&(s->securityPrivProtoLen));
			s->securityPrivProto = snmp_duplicate_objid(def, s->securityPrivProtoLen);
		}
		if (s->securityPrivProto == NULL) {
			/* assume DES */
			s->securityPrivProto = snmp_duplicate_objid(usmDESPrivProtocol,
				OIDSIZE(usmDESPrivProtocol));
			s->securityPrivProtoLen = OIDSIZE(usmDESPrivProtocol);
		}
		if (generate_Ku(s->securityAuthProto, s->securityAuthProtoLen,
				pass, strlen(pass),
				s->securityPrivKey, &(s->securityPrivKeyLen)) != SNMPERR_SUCCESS) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error generating a key for privacy pass phrase");
			return (-2);
		}
		return (0);
	}
	return (-1);
}
/* }}} */

/* {{{ proto string snmp2_get(string host, string community, string object_id [, int timeout [, int retries]]) 
   Fetch a SNMP object */
PHP_FUNCTION(snmp2_get)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU,SNMP_CMD_GET, SNMP_VERSION_2c);
}
/* }}} */

/* {{{ proto string snmp2_getnext(string host, string community, string object_id [, int timeout [, int retries]]) 
   Fetch a SNMP object */
PHP_FUNCTION(snmp2_getnext)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU,SNMP_CMD_GETNEXT, SNMP_VERSION_2c);
}
/* }}} */

/* {{{ proto array snmp2_walk(string host, string community, string object_id [, int timeout [, int retries]]) 
   Return all objects under the specified object id */
PHP_FUNCTION(snmp2_walk)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU,SNMP_CMD_WALK, SNMP_VERSION_2c);
}
/* }}} */

/* {{{ proto array snmp2_real_walk(string host, string community, string object_id [, int timeout [, int retries]])
   Return all objects including their respective object id withing the specified one */
PHP_FUNCTION(snmp2_real_walk)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU,SNMP_CMD_REALWALK, SNMP_VERSION_2c);
}
/* }}} */

/* {{{ proto int snmp2_set(string host, string community, string object_id, string type, mixed value [, int timeout [, int retries]]) 
   Set the value of a SNMP object */
PHP_FUNCTION(snmp2_set)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU,SNMP_CMD_SET, SNMP_VERSION_2c);
}
/* }}} */

/* {{{ proto void php_snmpv3(INTERNAL_FUNCTION_PARAMETERS, int st)
*
* Generic SNMPv3 object fetcher
* From here is passed on the common internal object fetcher.
*
* st=SNMP_CMD_GET   snmp3_get() - query an agent and return a single value.
* st=SNMP_CMD_GETNEXT   snmp3_getnext() - query an agent and return the next single value.
* st=SNMP_CMD_WALK   snmp3_walk() - walk the mib and return a single dimensional array 
*                       containing the values.
* st=SNMP_CMD_REALWALK   snmp3_real_walk() - walk the mib and return an 
*                            array of oid,value pairs.
* st=SNMP_CMD_SET  snmp3_set() - query an agent and set a single value
*
*/
static void php_snmpv3(INTERNAL_FUNCTION_PARAMETERS, int st)
{
	char *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8;
	int a1_len, a2_len, a3_len, a4_len, a5_len, a6_len, a7_len, a8_len;
	struct snmp_session session;
	long timeout = SNMP_DEFAULT_TIMEOUT;
	long retries = SNMP_DEFAULT_RETRIES;
	char type = (char) 0;
	char *value = (char *) 0, *stype = "";
	int stype_len, value_len;	
	char hostname[MAX_NAME_LEN];
	int remote_port = 161;
	char *pptr;
	int argc = ZEND_NUM_ARGS();
	
	if (st == SNMP_CMD_SET) {	
		if (zend_parse_parameters(argc TSRMLS_CC, "ssssssssss|ll", &a1, &a1_len, &a2, &a2_len, &a3, &a3_len,
			&a4, &a4_len, &a5, &a5_len, &a6, &a6_len, &a7, &a7_len, &a8, &a8_len, &stype, &stype_len, &value, &value_len, &timeout, &retries) == FAILURE) {
			return;
		}
	} else {
		/* SNMP_CMD_GET
		 * SNMP_CMD_GETNEXT
		 * SNMP_CMD_WALK
		 * SNMP_CMD_REALWALK
		 */
		if (zend_parse_parameters(argc TSRMLS_CC, "ssssssss|ll", &a1, &a1_len, &a2, &a2_len, &a3, &a3_len,
			&a4, &a4_len, &a5, &a5_len, &a6, &a6_len, &a7, &a7_len, &a8, &a8_len, &timeout, &retries) == FAILURE) {
			return;
		}
	}

	snmp_sess_init(&session);
	/* This is all SNMPv3 */
	session.version = SNMP_VERSION_3;

	/* Reading the hostname and its optional non-default port number */
	strlcpy(hostname, a1, sizeof(hostname));
	if ((pptr = strchr(hostname, ':'))) {
		remote_port = strtol(pptr + 1, NULL, 0);
	}
	session.peername = hostname;
	session.remote_port = remote_port;

	/* Setting the security name. */
	if (netsnmp_session_set_sec_name(&session, a2)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could net set security name: %s", a2);
		RETURN_FALSE;
	}

	/* Setting the security level. */
	if (netsnmp_session_set_sec_level(&session, a3 TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid security level: %s", a3);
		RETURN_FALSE;
	}

	/* Setting the authentication protocol. */
	if (netsnmp_session_set_auth_protocol(&session, a4 TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid authentication protocol: %s", a4);
		RETURN_FALSE;
	}

	/* Setting the authentication passphrase. */
	if (netsnmp_session_gen_auth_key(&session, a5 TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not generate key for authentication pass phrase: %s", a5);
		RETURN_FALSE;
	}

	/* Setting the security protocol. */
	if (netsnmp_session_set_sec_protocol(&session, a6 TSRMLS_CC) && a6_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid security protocol: %s", a6);
		RETURN_FALSE;
	}

	/* Setting the security protocol passphrase. */
	if (netsnmp_session_gen_sec_key(&session, a7 TSRMLS_CC) && a7_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not generate key for security pass phrase: %s", a7);
		RETURN_FALSE;
	}

	if (st == SNMP_CMD_SET) {
		type = stype[0];
	}

	session.retries = retries;
	session.timeout = timeout;

	php_snmp_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU, st, &session, a8, type, value);
}
/* }}} */

/* {{{ proto int snmp3_get(string host, string sec_name, string sec_level, string auth_protocol, string auth_passphrase, string priv_protocol, string priv_passphrase, string object_id [, int timeout [, int retries]])
   Fetch the value of a SNMP object */
PHP_FUNCTION(snmp3_get)
{
	php_snmpv3(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_GET);
}
/* }}} */

/* {{{ proto int snmp3_getnext(string host, string sec_name, string sec_level, string auth_protocol, string auth_passphrase, string priv_protocol, string priv_passphrase, string object_id [, int timeout [, int retries]])
   Fetch the value of a SNMP object */
PHP_FUNCTION(snmp3_getnext)
{
	php_snmpv3(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_GETNEXT);
}
/* }}} */

/* {{{ proto int snmp3_walk(string host, string sec_name, string sec_level, string auth_protocol, string auth_passphrase, string priv_protocol, string priv_passphrase, string object_id [, int timeout [, int retries]])
   Fetch the value of a SNMP object */
PHP_FUNCTION(snmp3_walk)
{
	php_snmpv3(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_WALK);
}
/* }}} */

/* {{{ proto int snmp3_real_walk(string host, string sec_name, string sec_level, string auth_protocol, string auth_passphrase, string priv_protocol, string priv_passphrase, string object_id [, int timeout [, int retries]])
   Fetch the value of a SNMP object */
PHP_FUNCTION(snmp3_real_walk)
{
	php_snmpv3(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_REALWALK);
}
/* }}} */

/* {{{ proto int snmp3_set(string host, string sec_name, string sec_level, string auth_protocol, string auth_passphrase, string priv_protocol, string priv_passphrase, string object_id, string type, mixed value [, int timeout [, int retries]])
   Fetch the value of a SNMP object */
PHP_FUNCTION(snmp3_set)
{
	php_snmpv3(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_SET);
}
/* }}} */

/* {{{ proto void snmp_set_valueretrieval(int method)
   Specify the method how the SNMP values will be returned */
PHP_FUNCTION(snmp_set_valueretrieval)
{
	long method;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &method) == FAILURE) {
		return;
	}

	if ((method == SNMP_VALUE_LIBRARY) || (method == SNMP_VALUE_PLAIN) || (method == SNMP_VALUE_OBJECT)) {
		SNMP_G(valueretrieval) = method;
	}
}
/* }}} */

/* {{{ proto int snmp_get_valueretrieval()
   Return the method how the SNMP values will be returned */
PHP_FUNCTION(snmp_get_valueretrieval)
{
	RETURN_LONG(SNMP_G(valueretrieval));
}
/* }}} */

/* {{{ proto int snmp_read_mib(string filename)
   Reads and parses a MIB file into the active MIB tree. */
PHP_FUNCTION(snmp_read_mib)
{
	char *filename;
	int filename_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename, &filename_len) == FAILURE) {
		return;
	}

	/* Prevent read_mib() from printing any errors. */
	snmp_disable_stderrlog();
	
	if (!read_mib(filename)) {
		char *error = strerror(errno);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error while reading MIB file '%s': %s", filename, error);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
