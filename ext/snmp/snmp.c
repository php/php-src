/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2011 The PHP Group                                |
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
   |          Boris Lytockin <lytboris@gmail.com>                         |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_snmp.h"

#include "zend_exceptions.h"

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

/* Another ugly macros, since UCD-SNMP has no snprint_* */
#ifdef HAVE_NET_SNMP
#define SNMP_SNPRINT_OBJID(dst, dstlen, src, srclen) (snprint_objid((dst), (dstlen), (src), (srclen)))
#define SNMP_SNPRINT_VALUE(dst, dstlen, srcname, srcnamelen, src) (snprint_value((dst), (dstlen), (srcname), (srcnamelen), (src)))
#else
#define SNMP_SNPRINT_OBJID(dst, dstlen, src, srclen) (sprint_objid((dst), (src), (srclen)))
#define SNMP_SNPRINT_VALUE(dst, dstlen, srcname, srcnamelen, src) (sprint_value((dst), (srcname), (srcnamelen), (src)))
#endif

#if PHP_VERSION_ID < 50300
#define Z_ADDREF_P(pz) pz->refcount++
#define Z_ISREF_PP(oid) (PZVAL_IS_REF(*(oid)))
#define Z_REFCOUNT_P(pz) pz->refcount
#define Z_SET_REFCOUNT_P(pz, rc) pz->refcount = rc
#define zend_parse_parameters_none() zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "")
#endif

/* For net-snmp prior to 5.4 */
#ifndef HAVE_SHUTDOWN_SNMP_LOGGING
extern netsnmp_log_handler *logh_head;
#define shutdown_snmp_logging() \
	{ \
		snmp_disable_log(); \
		while(NULL != logh_head) \
			netsnmp_remove_loghandler( logh_head ); \
	}
#endif

/* For really old ucd-snmp versions.. */
#ifndef HAVE_SNMP_PARSE_OID
#define snmp_parse_oid read_objid
#endif

#define SNMP_VALUE_LIBRARY	0
#define SNMP_VALUE_PLAIN	1
#define SNMP_VALUE_OBJECT	2

typedef struct snmp_session php_snmp_session;
#define PHP_SNMP_SESSION_RES_NAME "SNMP session"

#define PHP_SNMP_SESSION_FROM_OBJECT(session, object) \
	{ \
		php_snmp_object *snmp_object; \
		snmp_object = (php_snmp_object *)zend_object_store_get_object(object TSRMLS_CC); \
		session = snmp_object->session; \
		if (!session) { \
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid or uninitialized SNMP object"); \
			RETURN_FALSE; \
		} \
	}

#define PHP_SNMP_ADD_PROPERTIES(a, b) \
{ \
	int i = 0; \
	while (b[i].name != NULL) { \
		php_snmp_add_property((a), (b)[i].name, (b)[i].name_length, \
							(php_snmp_read_t)(b)[i].read_func, (php_snmp_write_t)(b)[i].write_func TSRMLS_CC); \
		i++; \
	} \
}



ZEND_DECLARE_MODULE_GLOBALS(snmp)
static PHP_GINIT_FUNCTION(snmp);

/* constant - can be shared among threads */
static oid objid_mib[] = {1, 3, 6, 1, 2, 1};

static int le_snmp_session;

/* Handlers */
static zend_object_handlers php_snmp_object_handlers;

/* Class entries */
zend_class_entry *php_snmp_class_entry;

/* Class object properties */
static HashTable php_snmp_properties;

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

ZEND_BEGIN_ARG_INFO_EX(arginfo_snmpset, 0, 0, 5)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, community)
	ZEND_ARG_INFO(0, object_id)
	ZEND_ARG_INFO(0, type)
	ZEND_ARG_INFO(0, value)
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

/* OO arginfo */

ZEND_BEGIN_ARG_INFO_EX(arginfo_snmp_open, 0, 0, 3)
	ZEND_ARG_INFO(0, version)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, community)
	ZEND_ARG_INFO(0, timeout)
	ZEND_ARG_INFO(0, retries)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_snmp_void, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_snmp_set_security, 0, 0, 8)
	ZEND_ARG_INFO(0, session)
	ZEND_ARG_INFO(0, sec_level)
	ZEND_ARG_INFO(0, auth_protocol)
	ZEND_ARG_INFO(0, auth_passphrase)
	ZEND_ARG_INFO(0, priv_protocol)
	ZEND_ARG_INFO(0, priv_passphrase)
	ZEND_ARG_INFO(0, contextName)
	ZEND_ARG_INFO(0, contextEngineID)
	ZEND_ARG_INFO(0, )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_snmp_get, 0, 0, 1)
	ZEND_ARG_INFO(0, object_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_snmp_walk, 0, 0, 3)
	ZEND_ARG_INFO(0, object_id)
	ZEND_ARG_INFO(0, non_repeaters)
	ZEND_ARG_INFO(0, max_repetitions)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_snmp_set, 0, 0, 3)
	ZEND_ARG_INFO(0, object_id)
	ZEND_ARG_INFO(0, type)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_snmp_class_set_quick_print, 0, 0, 1)
	ZEND_ARG_INFO(0, quick_print)
ZEND_END_ARG_INFO()
/* }}} */

typedef struct _snmpobjarg {
	char *oid;
	char type;
	char *value;

} snmpobjarg;

struct objid_set {
	int count;
	int offset;
	int step;
	int array_output;
	snmpobjarg *vars;
};

/* {{{ snmp_functions[]
 */
const zend_function_entry snmp_functions[] = {
	PHP_FE(snmpget,					arginfo_snmpget)
	PHP_FE(snmpgetnext, 				arginfo_snmpgetnext)
	PHP_FE(snmpwalk, 				arginfo_snmpwalk)
	PHP_FE(snmprealwalk, 				arginfo_snmprealwalk)
	PHP_FALIAS(snmpwalkoid, snmprealwalk, 		arginfo_snmprealwalk)
	PHP_FE(snmpset, 				arginfo_snmpset)
	PHP_FE(snmp_get_quick_print, 			arginfo_snmp_get_quick_print)
	PHP_FE(snmp_set_quick_print, 			arginfo_snmp_set_quick_print)
#ifdef HAVE_NET_SNMP
	PHP_FE(snmp_set_enum_print, 			arginfo_snmp_set_enum_print)
	PHP_FE(snmp_set_oid_output_format, 		arginfo_snmp_set_oid_output_format)
	PHP_FALIAS(snmp_set_oid_numeric_print, snmp_set_oid_output_format, arginfo_snmp_set_oid_output_format)
#endif

	PHP_FE(snmp2_get, 				arginfo_snmp2_get)
	PHP_FE(snmp2_getnext, 				arginfo_snmp2_getnext)
	PHP_FE(snmp2_walk, 				arginfo_snmp2_walk)
	PHP_FE(snmp2_real_walk, 			arginfo_snmp2_real_walk)
	PHP_FE(snmp2_set, 				arginfo_snmp2_set)

	PHP_FE(snmp3_get, 				arginfo_snmp3_get)
	PHP_FE(snmp3_getnext, 				arginfo_snmp3_getnext)
	PHP_FE(snmp3_walk, 				arginfo_snmp3_walk)
	PHP_FE(snmp3_real_walk, 			arginfo_snmp3_real_walk)
	PHP_FE(snmp3_set, 				arginfo_snmp3_set)
	PHP_FE(snmp_set_valueretrieval,			arginfo_snmp_set_valueretrieval)
	PHP_FE(snmp_get_valueretrieval,			arginfo_snmp_get_valueretrieval)

	PHP_FE(snmp_read_mib, 				arginfo_snmp_read_mib)
	{NULL,NULL,NULL}
};
/* }}} */

/* query an agent with GET method */
#define SNMP_CMD_GET		(1<<0)
/* query an agent with GETNEXT method */
#define SNMP_CMD_GETNEXT	(1<<1)
/* query an agent with SET method */
#define SNMP_CMD_SET		(1<<2)
/* walk the mib */
#define SNMP_CMD_WALK		(1<<3)
/* force values-only output */
#define SNMP_NUMERIC_KEYS	(1<<7)

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

static void netsnmp_session_free(php_snmp_session **session)
{
	if (*session) {
		if ((*session)->peername) {
			efree((*session)->peername);
		}
		efree(*session);
		*session = NULL;
	}
}

static void php_snmp_session_destructor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_snmp_session *session = (php_snmp_session *)rsrc->ptr;
	netsnmp_session_free(&session);
}

static void php_snmp_object_free_storage(void *object TSRMLS_DC)
{
	php_snmp_object *intern = (php_snmp_object *)object;
	
	if (!intern) {
		return;
	}

	netsnmp_session_free(&(intern->session));

	zend_object_std_dtor(&intern->zo TSRMLS_CC);
	
	efree(intern);
}

static zend_object_value php_snmp_object_new(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
#if PHP_VERSION_ID < 50399
	zval *tmp;
#endif
	zend_object_value retval;
	php_snmp_object *intern;

	/* Allocate memory for it */
	intern = emalloc(sizeof(php_snmp_object));
	memset(&intern->zo, 0, sizeof(php_snmp_object));

	zend_object_std_init(&intern->zo, class_type TSRMLS_CC);
#if PHP_VERSION_ID < 50399
	zend_hash_copy(intern->zo.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref,(void *) &tmp, sizeof(zval *));
#else
	object_properties_init(&intern->zo, class_type);
#endif

	retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) php_snmp_object_free_storage, NULL TSRMLS_CC);
	retval.handlers = (zend_object_handlers *) &php_snmp_object_handlers;

	return retval;
	
}

/* {{{ php_snmp_getvalue
*
* SNMP value to zval converter
*
*/
static void php_snmp_getvalue(struct variable_list *vars, zval *snmpval TSRMLS_DC, int valueretrieval)
{
	zval *val;
	char sbuf[64];
	char *buf = &(sbuf[0]);
	char *dbuf = (char *)NULL;
	int buflen = sizeof(sbuf) - 1;
	int val_len = vars->val_len;
	
	if (valueretrieval == SNMP_VALUE_LIBRARY) {
		val_len += 32; /* snprint_value will add type info into value, make some space for it */
	}

	/* use emalloc() for large values, use static array otherwize */
	if(val_len > buflen){
		if ((dbuf = (char *)emalloc(val_len + 1))) {
			buf = dbuf;
			buflen = val_len;
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "emalloc() failed: %s, fallback to static array", strerror(errno));
		}
	}

	*buf = 0;

	if (valueretrieval == SNMP_VALUE_LIBRARY) {
		SNMP_SNPRINT_VALUE(buf, buflen, vars->name, vars->name_length, vars);
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
		SNMP_SNPRINT_OBJID(buf, buflen, vars->val.objid, vars->val_len / sizeof(oid));
		ZVAL_STRING(val, buf, 1);
		break;

	case ASN_IPADDRESS:		/* 0x40, snmp_impl.h */
		snprintf(buf, buflen, "%d.%d.%d.%d",
			 (vars->val.string)[0], (vars->val.string)[1],
			 (vars->val.string)[2], (vars->val.string)[3]);
		buf[buflen]=0;
		ZVAL_STRING(val, buf, 1);
		break;

	case ASN_COUNTER:		/* 0x41, snmp_impl.h */
	case ASN_GAUGE:			/* 0x42, snmp_impl.h */
	/* ASN_UNSIGNED is the same as ASN_GAUGE */
	case ASN_TIMETICKS:		/* 0x43, snmp_impl.h */
	case ASN_UINTEGER:		/* 0x47, snmp_impl.h */
		snprintf(buf, buflen, "%lu", *vars->val.integer);
		buf[buflen]=0;
		ZVAL_STRING(val, buf, 1);
		break;

	case ASN_INTEGER:		/* 0x02, asn1.h */
		snprintf(buf, buflen, "%ld", *vars->val.integer);
		buf[buflen]=0;
		ZVAL_STRING(val, buf, 1);
		break;

#ifdef NETSNMP_WITH_OPAQUE_SPECIAL_TYPES
	case ASN_OPAQUE_FLOAT:		/* 0x78, asn1.h */
		snprintf(buf, buflen, "%f", *vars->val.floatVal);
		ZVAL_STRING(val, buf, 1);
		break;

	case ASN_OPAQUE_DOUBLE:		/* 0x79, asn1.h */
		snprintf(buf, buflen, "%Lf", *vars->val.doubleVal);
		ZVAL_STRING(val, buf, 1);
		break;

	case ASN_OPAQUE_I64:		/* 0x80, asn1.h */
		printI64(buf, vars->val.counter64);
		ZVAL_STRING(val, buf, 1);
		break;

	case ASN_OPAQUE_U64:		/* 0x81, asn1.h */
#endif
	case ASN_COUNTER64:		/* 0x46, snmp_impl.h */
		printU64(buf, vars->val.counter64);
		ZVAL_STRING(val, buf, 1);
		break;

	default:
		ZVAL_STRING(val, "Unknown value type", 1);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown value type: %u", vars->type);
		break;
	}

	if (valueretrieval == SNMP_VALUE_PLAIN) {
		*snmpval = *val;
		zval_copy_ctor(snmpval);
	} else {
		object_init(snmpval);
		add_property_long(snmpval, "type", vars->type);
		add_property_zval(snmpval, "value", val);
	}
	if(dbuf){ /* malloc was used to store value */
		efree(dbuf);
	}
}
/* }}} */

/* {{{ php_snmp_internal
*
* SNMP object fetcher/setter for all SNMP versions
*
*/
static void php_snmp_internal(INTERNAL_FUNCTION_PARAMETERS, int st, 
							struct snmp_session *session,
							struct objid_set *objid_set,
							int non_repeaters, int max_repetitions,
							int valueretrieval)
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
	int snmp_errno;

	/* we start with retval=FALSE. If any actual data is aquired, retval will be set to appropriate type */
	RETVAL_FALSE;

	if (st & SNMP_CMD_WALK) {
		if (objid_set->count > 1) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Multi OID walks are not supported!");
			RETURN_FALSE;
		}
		rootlen = MAX_NAME_LEN;
		if (strlen(objid_set->vars[0].oid)) { /* on a walk, an empty string means top of tree - no error */
			if (snmp_parse_oid(objid_set->vars[0].oid, root, &rootlen)) {
				gotroot = 1;
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid object identifier: %s", objid_set->vars[0].oid);
				RETURN_FALSE;
			}
		}

		if (!gotroot) {
			memmove((char *) root, (char *) objid_mib, sizeof(objid_mib));
			rootlen = sizeof(objid_mib) / sizeof(oid);
			gotroot = 1;
		}

		memmove((char *)name, (char *)root, rootlen * sizeof(oid));
		name_length = rootlen;
		objid_set->offset = objid_set->count;
	}

	if ((ss = snmp_open(session)) == NULL) {
		snmp_error(session, NULL, NULL, &err);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not open snmp connection: %s", err);
		free(err);
		RETURN_FALSE;
	}

	while (keepwalking) {
		keepwalking = 0;
		if (st & (SNMP_CMD_GET | SNMP_CMD_GETNEXT)) {
			pdu = snmp_pdu_create((st & SNMP_CMD_GET) ? SNMP_MSG_GET : SNMP_MSG_GETNEXT);
			for (count = 0; objid_set->offset < objid_set->count && count < objid_set->step; objid_set->offset++, count++){
				name_length = MAX_OID_LEN;
				if (!snmp_parse_oid(objid_set->vars[objid_set->offset].oid, name, &name_length)) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid object identifier: %s", objid_set->vars[objid_set->offset].oid);
				} else {
					snmp_add_null_var(pdu, name, name_length);
				}
			}
			if(pdu->variables == NULL){
				snmp_free_pdu(pdu);
				snmp_close(ss);
				RETURN_FALSE;
			}
		} else if (st & SNMP_CMD_SET) {
			pdu = snmp_pdu_create(SNMP_MSG_SET);
			for (count = 0; objid_set->offset < objid_set->count && count < objid_set->step; objid_set->offset++, count++){
				name_length = MAX_OID_LEN;
				if (!snmp_parse_oid(objid_set->vars[objid_set->offset].oid, name, &name_length)) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid object identifier: %s", objid_set->vars[objid_set->offset].oid);
					snmp_free_pdu(pdu);
					snmp_close(ss);
					RETURN_FALSE;
				} else {
					if ((snmp_errno = snmp_add_var(pdu, name, name_length, objid_set->vars[objid_set->offset].type, objid_set->vars[objid_set->offset].value))) {
						SNMP_SNPRINT_OBJID(buf, sizeof(buf), name, name_length);
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not add variable: OID='%s' type='%c' value='%s': %s", buf, objid_set->vars[objid_set->offset].type, objid_set->vars[objid_set->offset].value, snmp_api_errstring(snmp_errno));
						snmp_free_pdu(pdu);
						snmp_close(ss);
						RETURN_FALSE;
					}
				}
			}
		} else if (st & SNMP_CMD_WALK) {
			if (session->version == SNMP_VERSION_1) {
				pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
			} else {
				pdu = snmp_pdu_create(SNMP_MSG_GETBULK);
				pdu->non_repeaters = non_repeaters;
				pdu->max_repetitions = max_repetitions;
			}
			snmp_add_null_var(pdu, name, name_length);
		}

retry:
		status = snmp_synch_response(ss, pdu, &response);
		if (status == STAT_SUCCESS) {
			if (response->errstat == SNMP_ERR_NOERROR) {
				if (st & SNMP_CMD_SET) {
					snmp_free_pdu(response);
					snmp_close(ss);
					RETURN_TRUE;
				}
				for (vars = response->variables; vars; vars = vars->next_variable) {
					/* do not output errors as values */
					if ( 	vars->type == SNMP_ENDOFMIBVIEW || 
						vars->type == SNMP_NOSUCHOBJECT || 
						vars->type == SNMP_NOSUCHINSTANCE ) {
						if ((st & SNMP_CMD_WALK) && Z_TYPE_P(return_value) == IS_ARRAY) {
							break;
						}
						SNMP_SNPRINT_OBJID(buf, sizeof(buf), vars->name, vars->name_length);
						SNMP_SNPRINT_VALUE(buf2, sizeof(buf2), vars->name, vars->name_length, vars);
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error in packet at '%s': %s", buf, buf2);
						continue;
					}
					
					if ((st & SNMP_CMD_WALK) && 
						(vars->name_length < rootlen || memcmp(root, vars->name, rootlen * sizeof(oid)))) { /* not part of this subtree */
						if (Z_TYPE_P(return_value) == IS_ARRAY) { /* some records are fetched already, shut down further lookup */
							keepwalking = 0;
						} else {
							/* first fetched OID is out of subtree, fallback to GET query */
							st |= SNMP_CMD_GET;
							st ^= SNMP_CMD_WALK;
							objid_set->offset = 0;
							keepwalking = 1;
						}
						break;
					}

					MAKE_STD_ZVAL(snmpval);
					php_snmp_getvalue(vars, snmpval TSRMLS_CC, valueretrieval);

					if (objid_set->array_output) {
						if (Z_TYPE_P(return_value) == IS_BOOL) {
							array_init(return_value);
						}
						if (st & SNMP_NUMERIC_KEYS) {
							add_next_index_zval(return_value, snmpval);
						} else {
							SNMP_SNPRINT_OBJID(buf2, sizeof(buf2), vars->name, vars->name_length);
							add_assoc_zval(return_value, buf2, snmpval);
						}
					} else {
						*return_value = *snmpval;
						zval_copy_ctor(return_value);
						zval_ptr_dtor(&snmpval);
						break;
					}

					/* OID increase check */
					if (st & SNMP_CMD_WALK) {
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
				if (objid_set->offset < objid_set->count) { /* we have unprocessed OIDs */
					keepwalking = 1;
				}
			} else {
				if (!(st & SNMP_CMD_WALK) || response->errstat != SNMP_ERR_NOSUCHNAME || Z_TYPE_P(return_value) == IS_BOOL) {
					for (	count=1, vars = response->variables;
						vars && count != response->errindex;
						vars = vars->next_variable, count++);

					if (st & (SNMP_CMD_GET | SNMP_CMD_GETNEXT) && response->errstat == SNMP_ERR_TOOBIG && objid_set->step > 1) { /* Answer will not fit into single packet */
						objid_set->offset = ((objid_set->offset > objid_set->step) ? (objid_set->offset - objid_set->step) : 0 );
						objid_set->step /= 2;
						snmp_free_pdu(response);
						keepwalking = 1;
						continue;
					}
					if (vars) {
						SNMP_SNPRINT_OBJID(buf, sizeof(buf), vars->name, vars->name_length);
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error in packet at '%s': %s", buf, snmp_errstring(response->errstat));
					} else {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error in packet at %u object_id: %s", response->errindex, snmp_errstring(response->errstat));
					}
					if (st & (SNMP_CMD_GET | SNMP_CMD_GETNEXT)) { /* cut out bogus OID and retry */
						if ((pdu = snmp_fix_pdu(response, ((st & SNMP_CMD_GET) ? SNMP_MSG_GET : SNMP_MSG_GETNEXT) )) != NULL) {
							snmp_free_pdu(response);
							goto retry;
						}
					}
					snmp_free_pdu(response);
					snmp_close(ss);
					if (objid_set->array_output) {
						zval_dtor(return_value);
					}
					RETURN_FALSE;
				}
			}
		} else if (status == STAT_TIMEOUT) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "No response from %s", session->peername);
			if (objid_set->array_output) {
				zval_dtor(return_value);
			}
			snmp_close(ss);
			RETURN_FALSE;
		} else {    /* status == STAT_ERROR */
			snmp_error(ss, NULL, NULL, &err);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Fatal error: %s", err);
			free(err);
			if (objid_set->array_output) {
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

/* {{{ php_snmp_parse_oid
*
* OID parser (and type, value for SNMP_SET command)
*/

static int php_snmp_parse_oid(int st, struct objid_set *objid_set, zval **oid, zval **type, zval **value TSRMLS_DC)
{
	char *pptr;
	HashPosition pos_oid, pos_type, pos_value;
	zval **tmp_oid, **tmp_type, **tmp_value;

	if (Z_TYPE_PP(oid) != IS_ARRAY) {
		if (Z_ISREF_PP(oid)) {
			SEPARATE_ZVAL(oid);
		}
		convert_to_string_ex(oid);
	} else if (Z_TYPE_PP(oid) == IS_ARRAY) {
		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(oid), &pos_oid);
	}

	if (st & SNMP_CMD_SET) {
		if (Z_TYPE_PP(type) != IS_ARRAY) {
			if (Z_ISREF_PP(type)) {
				SEPARATE_ZVAL(type);
			}
			convert_to_string_ex(type);
		} else if (Z_TYPE_PP(type) == IS_ARRAY) {
			zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(type), &pos_type);
		}

		if (Z_TYPE_PP(value) != IS_ARRAY) {
			if (Z_ISREF_PP(value)) {
				SEPARATE_ZVAL(value);
			}
			convert_to_string_ex(value);
		} else if (Z_TYPE_PP(value) == IS_ARRAY) {
			zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(value), &pos_value);
		}
	}

	objid_set->count = 0;
	objid_set->array_output = ((st & SNMP_CMD_WALK) ? TRUE : FALSE);
	if (Z_TYPE_PP(oid) == IS_STRING) {
		objid_set->vars = (snmpobjarg *)emalloc(sizeof(snmpobjarg));
		if (objid_set->vars == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "emalloc() failed while parsing oid: %s", strerror(errno));
			return FALSE;
		}
		objid_set->vars[objid_set->count].oid = Z_STRVAL_PP(oid);
		if (st & SNMP_CMD_SET) {
			if (Z_TYPE_PP(type) == IS_STRING && Z_TYPE_PP(value) == IS_STRING) {
				if (Z_STRLEN_PP(type) != 1) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Bogus type '%s', should be single char, got %u", Z_STRVAL_PP(type), Z_STRLEN_PP(type));
					return FALSE;
				}
				pptr = Z_STRVAL_PP(type);
				objid_set->vars[objid_set->count].type = *pptr;
				objid_set->vars[objid_set->count].value = Z_STRVAL_PP(value);
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Single objid and multiple type or values are not supported");
				return FALSE;
			}
		}
		objid_set->count++;
	} else if (Z_TYPE_PP(oid) == IS_ARRAY) { /* we got objid array */
		if (zend_hash_num_elements(Z_ARRVAL_PP(oid)) == 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Got empty OID array");
			return FALSE;
		}
		objid_set->vars = (snmpobjarg *)emalloc(sizeof(snmpobjarg) * zend_hash_num_elements(Z_ARRVAL_PP(oid)));
		if (objid_set->vars == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "emalloc() failed while parsing oid array: %s", strerror(errno));
			return FALSE;
		}
		objid_set->array_output = ( (st & SNMP_CMD_SET) ? FALSE : TRUE );
		for (	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(oid), &pos_oid);
			zend_hash_get_current_data_ex(Z_ARRVAL_PP(oid), (void **) &tmp_oid, &pos_oid) == SUCCESS;
			zend_hash_move_forward_ex(Z_ARRVAL_PP(oid), &pos_oid) ) {

			convert_to_string_ex(tmp_oid);
			objid_set->vars[objid_set->count].oid = Z_STRVAL_PP(tmp_oid);
			if (st & SNMP_CMD_SET) {
				if (Z_TYPE_PP(type) == IS_STRING) {
					pptr = Z_STRVAL_PP(type);
					objid_set->vars[objid_set->count].type = *pptr;
				} else if (Z_TYPE_PP(type) == IS_ARRAY) {
					if (SUCCESS == zend_hash_get_current_data_ex(Z_ARRVAL_PP(type), (void **) &tmp_type, &pos_type)) {
						convert_to_string_ex(tmp_type);
						if (Z_STRLEN_PP(tmp_type) != 1) {
							php_error_docref(NULL TSRMLS_CC, E_WARNING, "'%s': bogus type '%s', should be single char, got %u", Z_STRVAL_PP(tmp_oid), Z_STRVAL_PP(tmp_type), Z_STRLEN_PP(tmp_type));
							return FALSE;
						}
						pptr = Z_STRVAL_PP(tmp_type);
						objid_set->vars[objid_set->count].type = *pptr;
						zend_hash_move_forward_ex(Z_ARRVAL_PP(type), &pos_type);
					} else {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "'%s': no type set", Z_STRVAL_PP(tmp_oid));
						return FALSE;
					}
				}

				if (Z_TYPE_PP(value) == IS_STRING) {
					objid_set->vars[objid_set->count].value = Z_STRVAL_PP(value);
				} else if (Z_TYPE_PP(value) == IS_ARRAY) {
					if (SUCCESS == zend_hash_get_current_data_ex(Z_ARRVAL_PP(value), (void **) &tmp_value, &pos_value)) {
						convert_to_string_ex(tmp_value);
						objid_set->vars[objid_set->count].value = Z_STRVAL_PP(tmp_value);
						zend_hash_move_forward_ex(Z_ARRVAL_PP(value), &pos_value);
					} else {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "'%s': no value set", Z_STRVAL_PP(tmp_oid));
						return FALSE;
					}
				}
			}
			objid_set->count++;
		}
	}

	objid_set->offset = 0;
	objid_set->step = objid_set->count;
	return (objid_set->count > 0);
}
/* }}} */

/* {{{ netsnmp_session_init
	allocates memory for session and session->peername, caller should free it manually using netsnmp_session_free() and efree()
*/
static int netsnmp_session_init(php_snmp_session **session_p, int version, char *hostname, char *community, int timeout, int retries TSRMLS_DC)
{
	int remote_port = SNMP_PORT;
	php_snmp_session *session;
	char *pptr;

	*session_p = (php_snmp_session *)emalloc(sizeof(php_snmp_session));
	session = *session_p;
	if (session == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "emalloc() failed allocating session");
		return (-1);
	}

	snmp_sess_init(session);

	session->version = version;

	session->peername = emalloc(MAX_NAME_LEN);
	if(session->peername == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "emalloc() failed while copying hostname");
		netsnmp_session_free(&session);
		return (-1);
	}

	/* Reading the hostname and its optional non-default port number */
	strlcpy(session->peername, hostname, MAX_NAME_LEN);
	if ((pptr = strchr(session->peername, ':'))) {
		remote_port = strtol(pptr + 1, NULL, 0);
	}

	session->remote_port = remote_port;

	if (version == SNMP_VERSION_3) {
		/* Setting the security name. */
		session->securityName = strdup(community);
		session->securityNameLen = strlen(session->securityName);
	} else {
		session->authenticator = NULL;
#ifdef UCD_SNMP_HACK
		session->community = (u_char *)strdup(community); /* memory freed by SNMP library, strdup NOT estrdup */
#else
		session->community = (u_char *)community;
#endif
		session->community_len = strlen(community);
	}

	session->retries = retries;
	session->timeout = timeout;
	return (0);
}
/* }}} */

/* {{{ int netsnmp_session_set_sec_level(struct snmp_session *s, char *level)
   Set the security level in the snmpv3 session */
static int netsnmp_session_set_sec_level(struct snmp_session *s, char *level)
{
	if (!strcasecmp(level, "noAuthNoPriv") || !strcasecmp(level, "nanp")) {
		s->securityLevel = SNMP_SEC_LEVEL_NOAUTH;
	} else if (!strcasecmp(level, "authNoPriv") || !strcasecmp(level, "anp")) {
		s->securityLevel = SNMP_SEC_LEVEL_AUTHNOPRIV;
	} else if (!strcasecmp(level, "authPriv") || !strcasecmp(level, "ap")) {
		s->securityLevel = SNMP_SEC_LEVEL_AUTHPRIV;
	} else {
		return (-1);
	}
	return (0);
}
/* }}} */

/* {{{ int netsnmp_session_set_auth_protocol(struct snmp_session *s, char *prot)
   Set the authentication protocol in the snmpv3 session */
static int netsnmp_session_set_auth_protocol(struct snmp_session *s, char *prot TSRMLS_DC)
{
	if (!strcasecmp(prot, "MD5")) {
		s->securityAuthProto = usmHMACMD5AuthProtocol;
		s->securityAuthProtoLen = OIDSIZE(usmHMACMD5AuthProtocol);
	} else if (!strcasecmp(prot, "SHA")) {
		s->securityAuthProto = usmHMACSHA1AuthProtocol;
		s->securityAuthProtoLen = OIDSIZE(usmHMACSHA1AuthProtocol);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown authentication protocol '%s'", prot);
		return (-1);
	}
	return (0);
}
/* }}} */

/* {{{ int netsnmp_session_set_sec_protocol(struct snmp_session *s, char *prot)
   Set the security protocol in the snmpv3 session */
static int netsnmp_session_set_sec_protocol(struct snmp_session *s, char *prot TSRMLS_DC)
{
	if (!strcasecmp(prot, "DES")) {
		s->securityPrivProto = usmDESPrivProtocol;
		s->securityPrivProtoLen = OIDSIZE(usmDESPrivProtocol);
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
#else			
	) {
		s->securityPrivProto = usmAES128PrivProtocol;
		s->securityPrivProtoLen = OIDSIZE(usmAES128PrivProtocol);
	} else if (!strcasecmp(prot, "AES192")) {
		s->securityPrivProto = usmAES192PrivProtocol;
		s->securityPrivProtoLen = OIDSIZE(usmAES192PrivProtocol);
	} else if (!strcasecmp(prot, "AES256")) {
		s->securityPrivProto = usmAES256PrivProtocol;
		s->securityPrivProtoLen = OIDSIZE(usmAES256PrivProtocol);
#endif
#endif
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown security protocol '%s'", prot);
		return (-1);
	}
	return (0);
}
/* }}} */

/* {{{ int netsnmp_session_gen_auth_key(struct snmp_session *s, char *pass)
   Make key from pass phrase in the snmpv3 session */
static int netsnmp_session_gen_auth_key(struct snmp_session *s, char *pass TSRMLS_DC)
{
	int snmp_errno;
	s->securityAuthKeyLen = USM_AUTH_KU_LEN;
	if ((snmp_errno = generate_Ku(s->securityAuthProto, s->securityAuthProtoLen,
			(u_char *) pass, strlen(pass),
			s->securityAuthKey, &(s->securityAuthKeyLen)))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error generating a key for authentication pass phrase '%s': %s", pass, snmp_api_errstring(snmp_errno));
		return (-1);
	}
	return (0);
}
/* }}} */

/* {{{ int netsnmp_session_gen_sec_key(struct snmp_session *s, u_char *pass)
   Make key from pass phrase in the snmpv3 session */
static int netsnmp_session_gen_sec_key(struct snmp_session *s, u_char *pass TSRMLS_DC)
{
	int snmp_errno;

	s->securityPrivKeyLen = USM_PRIV_KU_LEN;
	if ((snmp_errno = generate_Ku(s->securityAuthProto, s->securityAuthProtoLen,
			pass, strlen(pass),
			s->securityPrivKey, &(s->securityPrivKeyLen)))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error generating a key for privacy pass phrase '%s': %s", pass, snmp_api_errstring(snmp_errno));
		return (-2);
	}
	return (0);
}
/* }}} */

/* {{{ in netsnmp_session_set_contextEngineID(struct snmp_session *s, u_char * contextEngineID)
   Set context Engine Id in the snmpv3 session */
static int netsnmp_session_set_contextEngineID(struct snmp_session *s, u_char * contextEngineID TSRMLS_DC)
{
	size_t	ebuf_len = 32, eout_len = 0;
	u_char	*ebuf = (u_char *) malloc(ebuf_len); /* memory freed by SNMP library, malloc NOT emalloc */

	if (ebuf == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "malloc failure setting contextEngineID");
		return (-1);
	}
	if (!snmp_hex_to_binary(&ebuf, &ebuf_len, &eout_len, 1, contextEngineID)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Bad engine ID value '%s'", contextEngineID);
		free(ebuf);
		return (-1);
	}

	if (s->contextEngineID) {
		free(s->contextEngineID);
	}

	s->contextEngineID = ebuf;
	s->contextEngineIDLen = eout_len;
	return (0);
}
/* }}} */

/* {{{ php_set_security(struct snmp_session *session, char *sec_level, char *auth_protocol, char *auth_passphrase, char *priv_protocol, char *priv_passphrase, char *contextName, char *contextEngineID)
   Set all snmpv3-related security options */
static int netsnmp_session_set_security(struct snmp_session *session, char *sec_level, char *auth_protocol, char *auth_passphrase, char *priv_protocol, char *priv_passphrase, char *contextName, char *contextEngineID TSRMLS_DC)
{

	/* Setting the security level. */
	if (netsnmp_session_set_sec_level(session, sec_level)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid security level '%s'", sec_level);
		return (-1);
	}

	if (session->securityLevel == SNMP_SEC_LEVEL_AUTHNOPRIV || session->securityLevel == SNMP_SEC_LEVEL_AUTHPRIV) {

		/* Setting the authentication protocol. */
		if (netsnmp_session_set_auth_protocol(session, auth_protocol TSRMLS_CC)) {
			/* Warning message sent already, just bail out */
			return (-1);
		}

		/* Setting the authentication passphrase. */
		if (netsnmp_session_gen_auth_key(session, auth_passphrase TSRMLS_CC)) {
			/* Warning message sent already, just bail out */
			return (-1);
		}

		if (session->securityLevel == SNMP_SEC_LEVEL_AUTHPRIV) {
			/* Setting the security protocol. */
			if (netsnmp_session_set_sec_protocol(session, priv_protocol TSRMLS_CC)) {
				/* Warning message sent already, just bail out */
				return (-1);
			}

			/* Setting the security protocol passphrase. */
			if (netsnmp_session_gen_sec_key(session, priv_passphrase TSRMLS_CC)) {
				/* Warning message sent already, just bail out */
				return (-1);
			}
		}
	}

	/* Setting contextName if specified */
	if (contextName) {
		session->contextName = contextName;
		session->contextNameLen = strlen(contextName);
	}

	/* Setting contextEngineIS if specified */
	if (contextEngineID && strlen(contextEngineID) && netsnmp_session_set_contextEngineID(session, contextEngineID TSRMLS_CC)) {
		/* Warning message sent already, just bail out */
		return (-1);
	}

	return (0);
}
/* }}} */

/* {{{ php_snmp
*
* Generic SNMP handler for all versions.
* This function makes use of the internal SNMP object fetcher.
* Used both in old (non-OO) and OO API
*
*/
static void php_snmp(INTERNAL_FUNCTION_PARAMETERS, int st, int version)
{
	zval **oid, **value, **type;
	char *a1, *a2, *a3, *a4, *a5, *a6, *a7;
	int a1_len, a2_len, a3_len, a4_len, a5_len, a6_len, a7_len;
	long timeout = SNMP_DEFAULT_TIMEOUT;
	long retries = SNMP_DEFAULT_RETRIES;
	int non_repeaters = 0;
	int max_repetitions = -1;
	int argc = ZEND_NUM_ARGS();
	struct objid_set objid_set;
	php_snmp_session *session;
	int session_less_mode = (getThis() == NULL);
	int valueretrieval = SNMP_G(valueretrieval);
	php_snmp_object *snmp_object;
	php_snmp_object glob_snmp_object;

	if (session_less_mode) {
		if (version == SNMP_VERSION_3) {
			if (st & SNMP_CMD_SET) {
				if (zend_parse_parameters(argc TSRMLS_CC, "sssssssZZZ|ll", &a1, &a1_len, &a2, &a2_len, &a3, &a3_len,
					&a4, &a4_len, &a5, &a5_len, &a6, &a6_len, &a7, &a7_len, &oid, &type, &value, &timeout, &retries) == FAILURE) {
					RETURN_FALSE;
				}
			} else {
				/* SNMP_CMD_GET
				 * SNMP_CMD_GETNEXT
				 * SNMP_CMD_WALK
				 */
				if (zend_parse_parameters(argc TSRMLS_CC, "sssssssZ|ll", &a1, &a1_len, &a2, &a2_len, &a3, &a3_len,
					&a4, &a4_len, &a5, &a5_len, &a6, &a6_len, &a7, &a7_len, &oid, &timeout, &retries) == FAILURE) {
					RETURN_FALSE;
				}
			}
		} else {
			if (st & SNMP_CMD_SET) {
				if (zend_parse_parameters(argc TSRMLS_CC, "ssZZZ|ll", &a1, &a1_len, &a2, &a2_len, &oid, &type, &value, &timeout, &retries) == FAILURE) {
					RETURN_FALSE;
				}
			} else {
				/* SNMP_CMD_GET
				 * SNMP_CMD_GETNEXT
				 * SNMP_CMD_WALK
				 */
				if (zend_parse_parameters(argc TSRMLS_CC, "ssZ|ll", &a1, &a1_len, &a2, &a2_len, &oid, &timeout, &retries) == FAILURE) {
					RETURN_FALSE;
				}
			}
		}
	} else {
		if (st & SNMP_CMD_SET) {
			if (zend_parse_parameters(argc TSRMLS_CC, "ZZZ", &oid, &type, &value) == FAILURE) {
				RETURN_FALSE;
			}
		} else if (st & SNMP_CMD_WALK) {
			if (zend_parse_parameters(argc TSRMLS_CC, "Z|ll", &oid, &non_repeaters, &max_repetitions) == FAILURE) {
				RETURN_FALSE;
			}
		} else {
			/* SNMP_CMD_GET
			 * SNMP_CMD_GETNEXT
			 */
			if (zend_parse_parameters(argc TSRMLS_CC, "Z", &oid) == FAILURE) {
				RETURN_FALSE;
			}
		}
	}

	if (!php_snmp_parse_oid(st, &objid_set, oid, type, value TSRMLS_CC)) {
		RETURN_FALSE;
	}

	if (session_less_mode) {
		if (netsnmp_session_init(&session, version, a1, a2, timeout, retries TSRMLS_CC)) {
			RETURN_FALSE;
		}
		if (version == SNMP_VERSION_3 && netsnmp_session_set_security(session, a3, a4, a5, a6, a7, NULL, NULL TSRMLS_CC)) {
			/* Warning message sent already, just bail out */
			RETURN_FALSE;
		}
	} else {
		zval *object = getThis();
		PHP_SNMP_SESSION_FROM_OBJECT(session, object);
		snmp_object = (php_snmp_object *)zend_object_store_get_object(object TSRMLS_CC);
		if (snmp_object->max_oids > 0) {
			objid_set.step = snmp_object->max_oids;
			if (max_repetitions < 0) { /* unspecified in function call, use session-wise */
				max_repetitions = snmp_object->max_oids;
			}
		}
		valueretrieval = snmp_object->valueretrieval;
#ifdef HAVE_NET_SNMP
		glob_snmp_object.enum_print = netsnmp_ds_get_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_PRINT_NUMERIC_ENUM);
		netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_PRINT_NUMERIC_ENUM, snmp_object->enum_print);
		glob_snmp_object.quick_print = netsnmp_ds_get_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_QUICK_PRINT);
		netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_QUICK_PRINT, snmp_object->quick_print);
		glob_snmp_object.oid_output_format = netsnmp_ds_get_int(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_OID_OUTPUT_FORMAT);
		netsnmp_ds_set_int(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_OID_OUTPUT_FORMAT, snmp_object->oid_output_format);
#else
		glob_snmp_object.quick_print = snmp_get_quick_print();
		snmp_set_quick_print(snmp_object->quick_print);
#endif
	}

	if (max_repetitions < 0) {
		max_repetitions = 20; /* provide correct default value */
	}

	php_snmp_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU, st, session, &objid_set, non_repeaters, max_repetitions, valueretrieval);
	
	efree(objid_set.vars);

	if (session_less_mode) {
		netsnmp_session_free(&session);
	} else {
#ifdef HAVE_NET_SNMP
		netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_PRINT_NUMERIC_ENUM, glob_snmp_object.enum_print);
		netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_QUICK_PRINT, glob_snmp_object.quick_print);
		netsnmp_ds_set_int(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_OID_OUTPUT_FORMAT, glob_snmp_object.oid_output_format);
#else
		snmp_set_quick_print(glob_snmp_object.quick_print);
#endif
	}
}
/* }}} */

/* {{{ proto mixed snmpget(string host, string community, mixed object_id [, int timeout [, int retries]]) 
   Fetch a SNMP object */
PHP_FUNCTION(snmpget)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_GET, SNMP_VERSION_1);
}
/* }}} */

/* {{{ proto mixed snmpgetnext(string host, string community, mixed object_id [, int timeout [, int retries]]) 
   Fetch a SNMP object */
PHP_FUNCTION(snmpgetnext)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_GETNEXT, SNMP_VERSION_1);
}
/* }}} */

/* {{{ proto mixed snmpwalk(string host, string community, mixed object_id [, int timeout [, int retries]]) 
   Return all objects under the specified object id */
PHP_FUNCTION(snmpwalk)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, (SNMP_CMD_WALK | SNMP_NUMERIC_KEYS), SNMP_VERSION_1);
}
/* }}} */

/* {{{ proto mixed snmprealwalk(string host, string community, mixed object_id [, int timeout [, int retries]])
   Return all objects including their respective object id withing the specified one */
PHP_FUNCTION(snmprealwalk)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_WALK, SNMP_VERSION_1);
}
/* }}} */

/* {{{ proto bool snmpset(string host, string community, mixed object_id, mixed type, mixed value [, int timeout [, int retries]]) 
   Set the value of a SNMP object */
PHP_FUNCTION(snmpset)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_SET, SNMP_VERSION_1);
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

/* {{{ proto bool snmp_set_quick_print(int quick_print)
   Return all objects including their respective object id withing the specified one */
PHP_FUNCTION(snmp_set_quick_print)
{
	long a1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &a1) == FAILURE) {
		RETURN_FALSE;
	}

#ifdef HAVE_NET_SNMP
	netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_QUICK_PRINT, (int)a1);
#else
	snmp_set_quick_print((int)a1);
#endif
	RETURN_TRUE;
}
/* }}} */

#ifdef HAVE_NET_SNMP
/* {{{ proto bool snmp_set_enum_print(int enum_print)
   Return all values that are enums with their enum value instead of the raw integer */
PHP_FUNCTION(snmp_set_enum_print)
{
	long a1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &a1) == FAILURE) {
		RETURN_FALSE;
	}

	netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_PRINT_NUMERIC_ENUM, (int) a1);
	RETURN_TRUE;
} 
/* }}} */

/* {{{ proto bool snmp_set_oid_output_format(int oid_format)
   Set the OID output format. */
PHP_FUNCTION(snmp_set_oid_output_format)
{
	long a1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &a1) == FAILURE) {
		RETURN_FALSE;
	}

	switch((int) a1) {
		case NETSNMP_OID_OUTPUT_SUFFIX:
		case NETSNMP_OID_OUTPUT_MODULE:
		case NETSNMP_OID_OUTPUT_FULL:
		case NETSNMP_OID_OUTPUT_NUMERIC:
		case NETSNMP_OID_OUTPUT_UCD:
		case NETSNMP_OID_OUTPUT_NONE:
			netsnmp_ds_set_int(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_OID_OUTPUT_FORMAT, a1);
			RETURN_TRUE;
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown SNMP output print format '%d'", (int) a1);
			RETURN_FALSE;
			break;
	}
} 
/* }}} */
#endif

/* {{{ proto mixed snmp2_get(string host, string community, mixed object_id [, int timeout [, int retries]]) 
   Fetch a SNMP object */
PHP_FUNCTION(snmp2_get)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_GET, SNMP_VERSION_2c);
}
/* }}} */

/* {{{ proto mixed snmp2_getnext(string host, string community, mixed object_id [, int timeout [, int retries]]) 
   Fetch a SNMP object */
PHP_FUNCTION(snmp2_getnext)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_GETNEXT, SNMP_VERSION_2c);
}
/* }}} */

/* {{{ proto mixed snmp2_walk(string host, string community, mixed object_id [, int timeout [, int retries]]) 
   Return all objects under the specified object id */
PHP_FUNCTION(snmp2_walk)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, (SNMP_CMD_WALK | SNMP_NUMERIC_KEYS), SNMP_VERSION_2c);
}
/* }}} */

/* {{{ proto mixed snmp2_real_walk(string host, string community, mixed object_id [, int timeout [, int retries]])
   Return all objects including their respective object id withing the specified one */
PHP_FUNCTION(snmp2_real_walk)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_WALK, SNMP_VERSION_2c);
}
/* }}} */

/* {{{ proto bool snmp2_set(string host, string community, mixed object_id, mixed type, mixed value [, int timeout [, int retries]]) 
   Set the value of a SNMP object */
PHP_FUNCTION(snmp2_set)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_SET, SNMP_VERSION_2c);
}
/* }}} */

/* {{{ proto mixed snmp3_get(string host, string sec_name, string sec_level, string auth_protocol, string auth_passphrase, string priv_protocol, string priv_passphrase, mixed object_id [, int timeout [, int retries]])
   Fetch the value of a SNMP object */
PHP_FUNCTION(snmp3_get)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_GET, SNMP_VERSION_3);
}
/* }}} */

/* {{{ proto mixed snmp3_getnext(string host, string sec_name, string sec_level, string auth_protocol, string auth_passphrase, string priv_protocol, string priv_passphrase, mixed object_id [, int timeout [, int retries]])
   Fetch the value of a SNMP object */
PHP_FUNCTION(snmp3_getnext)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_GETNEXT, SNMP_VERSION_3);
}
/* }}} */

/* {{{ proto mixed snmp3_walk(string host, string sec_name, string sec_level, string auth_protocol, string auth_passphrase, string priv_protocol, string priv_passphrase, mixed object_id [, int timeout [, int retries]])
   Fetch the value of a SNMP object */
PHP_FUNCTION(snmp3_walk)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, (SNMP_CMD_WALK | SNMP_NUMERIC_KEYS), SNMP_VERSION_3);
}
/* }}} */

/* {{{ proto mixed snmp3_real_walk(string host, string sec_name, string sec_level, string auth_protocol, string auth_passphrase, string priv_protocol, string priv_passphrase, mixed object_id [, int timeout [, int retries]])
   Fetch the value of a SNMP object */
PHP_FUNCTION(snmp3_real_walk)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_WALK, SNMP_VERSION_3);
}
/* }}} */

/* {{{ proto bool snmp3_set(string host, string sec_name, string sec_level, string auth_protocol, string auth_passphrase, string priv_protocol, string priv_passphrase, mixed object_id, mixed type, mixed value [, int timeout [, int retries]])
   Fetch the value of a SNMP object */
PHP_FUNCTION(snmp3_set)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_SET, SNMP_VERSION_3);
}
/* }}} */

/* {{{ proto bool snmp_set_valueretrieval(int method)
   Specify the method how the SNMP values will be returned */
PHP_FUNCTION(snmp_set_valueretrieval)
{
	long method;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &method) == FAILURE) {
		RETURN_FALSE;
	}

	switch(method) {
		case SNMP_VALUE_LIBRARY:
		case SNMP_VALUE_PLAIN:
		case SNMP_VALUE_OBJECT:
			SNMP_G(valueretrieval) = method;
			RETURN_TRUE;
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown SNMP value retrieval method '%ld'", method);
			RETURN_FALSE;
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

/* {{{ proto bool snmp_read_mib(string filename)
   Reads and parses a MIB file into the active MIB tree. */
PHP_FUNCTION(snmp_read_mib)
{
	char *filename;
	int filename_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename, &filename_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (!read_mib(filename)) {
		char *error = strerror(errno);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error while reading MIB file '%s': %s", filename, error);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto SNMP SNMP::open(int version, string hostname, string community|securityName [, long timeout [, long retries]])
	Creates a new SNMP session to specified host. */
PHP_METHOD(snmp, open)
{
	php_snmp_object *snmp_object;
	zval *object = getThis();
	char *a1, *a2;
	int a1_len, a2_len;
	long timeout = SNMP_DEFAULT_TIMEOUT;
	long retries = SNMP_DEFAULT_RETRIES;
	int version = SNMP_DEFAULT_VERSION;
	int argc = ZEND_NUM_ARGS();
#if PHP_VERSION_ID > 50300
	zend_error_handling error_handling;
#endif

	snmp_object = (php_snmp_object *)zend_object_store_get_object(object TSRMLS_CC);
#if PHP_VERSION_ID > 50300
	zend_replace_error_handling(EH_THROW, NULL, &error_handling TSRMLS_CC);
#else
	php_set_error_handling(EH_THROW, zend_exception_get_default(TSRMLS_C) TSRMLS_CC);
#endif
	
	if (zend_parse_parameters(argc TSRMLS_CC, "lss|ll", &version, &a1, &a1_len, &a2, &a2_len, &timeout, &retries) == FAILURE) {
#if PHP_VERSION_ID > 50300
		zend_restore_error_handling(&error_handling TSRMLS_CC);
#else
		php_std_error_handling();
#endif
		return;
	}

#if PHP_VERSION_ID > 50300
	zend_restore_error_handling(&error_handling TSRMLS_CC);
#else
	php_std_error_handling();
#endif

	switch(version) {
		case SNMP_VERSION_1:
		case SNMP_VERSION_2c:
		case SNMP_VERSION_3:
			break;
		default:
			zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Unknown SNMP protocol version", 0 TSRMLS_CC);
			return;
	}

	/* handle re-open of snmp session */
	if (snmp_object->session) {
		netsnmp_session_free(&(snmp_object->session));
	}
	
	if (netsnmp_session_init(&(snmp_object->session), version, a1, a2, timeout, retries TSRMLS_CC)) {
		return;
	}
	snmp_object->max_oids = 0;
	snmp_object->valueretrieval = SNMP_G(valueretrieval);
#ifdef HAVE_NET_SNMP
	snmp_object->enum_print = netsnmp_ds_get_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_PRINT_NUMERIC_ENUM);
	snmp_object->oid_output_format = netsnmp_ds_get_int(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_OID_OUTPUT_FORMAT);
	snmp_object->quick_print = netsnmp_ds_get_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_QUICK_PRINT);
#else
	snmp_object->quick_print = snmp_get_quick_print();
#endif
}
/* }}} */

/* {{{ proto bool SNMP::close() 
	Close SNMP session */
PHP_METHOD(snmp, close)
{
	php_snmp_object *snmp_object;
	zval *object = getThis();

	snmp_object = (php_snmp_object *)zend_object_store_get_object(object TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_FALSE;
	}

	netsnmp_session_free(&(snmp_object->session));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed SNMP::get_assoc(mixed object_id) 
   Fetch a SNMP object returing scalar for single OID and array of oid->value pairs for multi OID request */
PHP_METHOD(snmp, get)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_GET, (-1));
}
/* }}} */

/* {{{ proto mixed SNMP::getnext(mixed object_id) 
   Fetch a SNMP object returing scalar for single OID and array of oid->value pairs for multi OID request */
PHP_METHOD(snmp, getnext)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_GETNEXT, (-1));
}
/* }}} */

/* {{{ proto mixed SNMP::walk(mixed object_id)
   Return all objects including their respective object id withing the specified one as array of oid->value pairs */
PHP_METHOD(snmp, walk)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_WALK, (-1));
}
/* }}} */

/* {{{ proto bool SNMP::set(mixed object_id, mixed type, mixed value) 
   Set the value of a SNMP object */
PHP_METHOD(snmp, set)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_SET, (-1));
}

/* {{{ proto bool SNMP::set_security(resource session, string sec_level, [ string auth_protocol, string auth_passphrase [, string priv_protocol, string priv_passphrase [, string contextName [, string contextEngineID]]]])
	Set SNMPv3 security-related session parameters */
PHP_METHOD(snmp, set_security)
{
	php_snmp_object *snmp_object;
	zval *object = getThis();
	char *a1 = "", *a2 = "", *a3 = "", *a4 = "", *a5 = "", *a6 = "", *a7 = "";
	int a1_len, a2_len, a3_len, a4_len, a5_len, a6_len, a7_len;
	int argc = ZEND_NUM_ARGS();

	snmp_object = (php_snmp_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	if (zend_parse_parameters(argc TSRMLS_CC, "s|ssssss", &a1, &a1_len, &a2, &a2_len, &a3, &a3_len,
		&a4, &a4_len, &a5, &a5_len, &a6, &a6_len, &a7, &a7_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (netsnmp_session_set_security(snmp_object->session, a1, a2, a3, a4, a5, a6, a7 TSRMLS_CC)) {
		/* Warning message sent already, just bail out */
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ */
void php_snmp_add_property(HashTable *h, const char *name, size_t name_length, php_snmp_read_t read_func, php_snmp_write_t write_func TSRMLS_DC)
{
	php_snmp_prop_handler p;

	p.name = (char*) name;
	p.name_length = name_length;
	p.read_func = (read_func) ? read_func : NULL;
	p.write_func = (write_func) ? write_func : NULL;
	zend_hash_add(h, (char *)name, name_length + 1, &p, sizeof(php_snmp_prop_handler), NULL);
}
/* }}} */

/* {{{ php_snmp_read_property(zval *object, zval *member, int type[, const zend_literal *key])
   Generic object property reader */
#if PHP_VERSION_ID < 50399
zval *php_snmp_read_property(zval *object, zval *member, int type TSRMLS_DC)
#else
zval *php_snmp_read_property(zval *object, zval *member, int type, const zend_literal *key TSRMLS_DC)
#endif
{
	zval tmp_member;
	zval *retval;
	php_snmp_object *obj;
	php_snmp_prop_handler *hnd;
	int ret;

	ret = FAILURE;
	obj = (php_snmp_object *)zend_objects_get_address(object TSRMLS_CC);

	if (Z_TYPE_P(member) != IS_STRING) {
		tmp_member = *member;
		zval_copy_ctor(&tmp_member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
	}

	ret = zend_hash_find(&php_snmp_properties, Z_STRVAL_P(member), Z_STRLEN_P(member)+1, (void **) &hnd);

	if (ret == SUCCESS && hnd->read_func) {
		ret = hnd->read_func(obj, &retval TSRMLS_CC);
		if (ret == SUCCESS) {
			/* ensure we're creating a temporary variable */
			Z_SET_REFCOUNT_P(retval, 0);
		} else {
			retval = EG(uninitialized_zval_ptr);
		}
	} else {
		zend_object_handlers * std_hnd = zend_get_std_object_handlers();
#if PHP_VERSION_ID < 50399
		retval = std_hnd->read_property(object, member, type TSRMLS_CC);
#else
		retval = std_hnd->read_property(object, member, type, key TSRMLS_CC);
#endif
	}

	if (member == &tmp_member) {
		zval_dtor(member);
	}
	return(retval);
}
/* }}} */

/* {{{ php_snmp_write_property(zval *object, zval *member, zval *value[, const zend_literal *key])
   Generic object property writer */
#if PHP_VERSION_ID < 50399
void php_snmp_write_property(zval *object, zval *member, zval *value TSRMLS_DC)
#else
void php_snmp_write_property(zval *object, zval *member, zval *value, const zend_literal *key TSRMLS_DC)
#endif
{
	zval tmp_member;
	php_snmp_object *obj;
	php_snmp_prop_handler *hnd;
	int ret;

	if (Z_TYPE_P(member) != IS_STRING) {
		tmp_member = *member;
		zval_copy_ctor(&tmp_member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
	}

	ret = FAILURE;
	obj = (php_snmp_object *)zend_objects_get_address(object TSRMLS_CC);

	ret = zend_hash_find(&php_snmp_properties, Z_STRVAL_P(member), Z_STRLEN_P(member) + 1, (void **) &hnd);

	if (ret == SUCCESS && hnd->write_func) {
		hnd->write_func(obj, value TSRMLS_CC);
		if (! PZVAL_IS_REF(value) && Z_REFCOUNT_P(value) == 0) {
			Z_ADDREF_P(value);
			zval_ptr_dtor(&value);
		}
	} else {
		zend_object_handlers * std_hnd = zend_get_std_object_handlers();
#if PHP_VERSION_ID < 50399
		std_hnd->write_property(object, member, value TSRMLS_CC);
#else
		std_hnd->write_property(object, member, value, key TSRMLS_CC);
#endif
	}

	if (member == &tmp_member) {
		zval_dtor(member);
	}
}
/* }}} */

/* {{{ php_snmp_has_property(zval *object, zval *member, int has_set_exists[, const zend_literal *key])
   Generic object property checker */
#if PHP_VERSION_ID < 50399
static int php_snmp_has_property(zval *object, zval *member, int has_set_exists TSRMLS_DC)
#else
static int php_snmp_has_property(zval *object, zval *member, int has_set_exists, const zend_literal *key TSRMLS_DC)
#endif
{
	php_snmp_object *obj = (php_snmp_object *)zend_objects_get_address(object TSRMLS_CC);
	php_snmp_prop_handler *hnd;
	int ret = 0;

	if (zend_hash_find(&php_snmp_properties, Z_STRVAL_P(member), Z_STRLEN_P(member) + 1, (void **)&hnd) == SUCCESS) {
		switch (has_set_exists) {
			case 2:
				ret = 1;
				break;
			case 0: {
#if PHP_VERSION_ID < 50399
				zval *value = php_snmp_read_property(object, member, BP_VAR_IS TSRMLS_CC);
#else
				zval *value = php_snmp_read_property(object, member, BP_VAR_IS, key TSRMLS_CC);
#endif
				if (value != EG(uninitialized_zval_ptr)) {
					ret = Z_TYPE_P(value) != IS_NULL? 1:0;
					/* refcount is 0 */
					Z_ADDREF_P(value);
					zval_ptr_dtor(&value);
				}
				break;
			}
			default: {
#if PHP_VERSION_ID < 50399
				zval *value = php_snmp_read_property(object, member, BP_VAR_IS TSRMLS_CC);
#else
				zval *value = php_snmp_read_property(object, member, BP_VAR_IS, key TSRMLS_CC);
#endif
				if (value != EG(uninitialized_zval_ptr)) {
					convert_to_boolean(value);
					ret = Z_BVAL_P(value)? 1:0;
					/* refcount is 0 */
					Z_ADDREF_P(value);
					zval_ptr_dtor(&value);
				}
				break;
			}
		}
	} else {
		zend_object_handlers * std_hnd = zend_get_std_object_handlers();
#if PHP_VERSION_ID < 50399
		ret = std_hnd->has_property(object, member, has_set_exists TSRMLS_CC);
#else
		ret = std_hnd->has_property(object, member, has_set_exists, key TSRMLS_CC);
#endif
	}
	return ret;
}
/* }}} */

/* {{{ php_snmp_get_properties(zval *object)
   Returns all object properties. Injects SNMP properties into object on first call */
static HashTable *php_snmp_get_properties(zval *object TSRMLS_DC)
{
	php_snmp_object *obj;
	php_snmp_prop_handler *hnd;
	HashTable *props;
	zval *val;
	char *key;
	uint key_len;
	HashPosition pos;
	ulong num_key;

	obj = (php_snmp_object *)zend_objects_get_address(object TSRMLS_CC);
#if PHP_VERSION_ID < 50399
	props = obj->zo.properties;
#else
	props = zend_std_get_properties(object TSRMLS_CC);
#endif

	zend_hash_internal_pointer_reset_ex(&php_snmp_properties, &pos);

	while (zend_hash_get_current_data_ex(&php_snmp_properties, (void**)&hnd, &pos) == SUCCESS) {
		zend_hash_get_current_key_ex(&php_snmp_properties, &key, &key_len, &num_key, 0, &pos);
		if (!hnd->read_func || hnd->read_func(obj, &val TSRMLS_CC) != SUCCESS) {
			val = EG(uninitialized_zval_ptr);
			Z_ADDREF_P(val);
		}
		zend_hash_update(props, key, key_len, (void *)&val, sizeof(zval *), NULL);
		zend_hash_move_forward_ex(&php_snmp_properties, &pos);
	}
	return obj->zo.properties;
}
/* }}} */

/* {{{ */
static int php_snmp_read_info(php_snmp_object *snmp_object, zval **retval TSRMLS_DC)
{
	zval *val;

	MAKE_STD_ZVAL(*retval);
	array_init(*retval);

	if (snmp_object->session == NULL) {
		return SUCCESS;
	}
		
	MAKE_STD_ZVAL(val);
	ZVAL_STRINGL(val, snmp_object->session->peername, strlen(snmp_object->session->peername), 1);
	add_assoc_zval(*retval, "hostname", val);
	
	MAKE_STD_ZVAL(val);
	ZVAL_LONG(val, snmp_object->session->remote_port);
	add_assoc_zval(*retval, "port", val);
	
	MAKE_STD_ZVAL(val);
	ZVAL_LONG(val, snmp_object->session->timeout);
	add_assoc_zval(*retval, "timeout", val);
	
	MAKE_STD_ZVAL(val);
	ZVAL_LONG(val, snmp_object->session->retries);
	add_assoc_zval(*retval, "retries", val);
	
	return SUCCESS;
}
/* }}} */

/* {{{ */
static int php_snmp_read_max_oids(php_snmp_object *snmp_object, zval **retval TSRMLS_DC)
{
	MAKE_STD_ZVAL(*retval);
	ZVAL_LONG(*retval, snmp_object->max_oids);
	return SUCCESS;
}
/* }}} */

/* {{{ */
static int php_snmp_read_valueretrieval(php_snmp_object *snmp_object, zval **retval TSRMLS_DC)
{
	MAKE_STD_ZVAL(*retval);
	ZVAL_LONG(*retval, snmp_object->valueretrieval);
	return SUCCESS;
}
/* }}} */

/* {{{ */
static int php_snmp_read_quick_print(php_snmp_object *snmp_object, zval **retval TSRMLS_DC)
{
	MAKE_STD_ZVAL(*retval);
	ZVAL_BOOL(*retval, snmp_object->quick_print);
	return SUCCESS;
}
/* }}} */

#ifdef HAVE_NET_SNMP
/* {{{ */
static int php_snmp_read_enum_print(php_snmp_object *snmp_object, zval **retval TSRMLS_DC)
{
	MAKE_STD_ZVAL(*retval);
	ZVAL_BOOL(*retval, snmp_object->enum_print);
	return SUCCESS;
}
/* }}} */

/* {{{ */
static int php_snmp_read_oid_output_format(php_snmp_object *snmp_object, zval **retval TSRMLS_DC)
{
	MAKE_STD_ZVAL(*retval);
	ZVAL_LONG(*retval, snmp_object->oid_output_format);
	return SUCCESS;
}
/* }}} */
#endif

/* {{{ */
static int php_snmp_write_info(php_snmp_object *snmp_object, zval *newval TSRMLS_DC)
{
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "info property is read-only");
	return FAILURE;
}
/* }}} */

/* {{{ */
static int php_snmp_write_max_oids(php_snmp_object *snmp_object, zval *newval TSRMLS_DC)
{
	zval ztmp;
	int ret = SUCCESS;
	if (Z_TYPE_P(newval) != IS_LONG) {
		ztmp = *newval;
		zval_copy_ctor(&ztmp);
		convert_to_long(&ztmp);
		newval = &ztmp;
	}

	snmp_object->max_oids = Z_LVAL_P(newval);
	
	if (newval == &ztmp) {
		zval_dtor(newval);
	}

	return ret;
}
/* }}} */

/* {{{ */
static int php_snmp_write_valueretrieval(php_snmp_object *snmp_object, zval *newval TSRMLS_DC)
{
	zval ztmp;
	int ret = SUCCESS;
	if (Z_TYPE_P(newval) != IS_LONG) {
		ztmp = *newval;
		zval_copy_ctor(&ztmp);
		convert_to_long(&ztmp);
		newval = &ztmp;
	}

	switch(Z_LVAL_P(newval)) {
		case SNMP_VALUE_LIBRARY:
		case SNMP_VALUE_PLAIN:
		case SNMP_VALUE_OBJECT:
			snmp_object->valueretrieval = Z_LVAL_P(newval);
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown SNMP value retrieval method '%ld'", Z_LVAL_P(newval));
			ret = FAILURE;
			break;
	}
	
	if (newval == &ztmp) {
		zval_dtor(newval);
	}

	return ret;
}
/* }}} */

/* {{{ */
static int php_snmp_write_quick_print(php_snmp_object *snmp_object, zval *newval TSRMLS_DC)
{
	zval ztmp;
	if (Z_TYPE_P(newval) != IS_BOOL) {
		ztmp = *newval;
		zval_copy_ctor(&ztmp);
		convert_to_boolean(&ztmp);
		newval = &ztmp;
	}
	
	snmp_object->quick_print = Z_LVAL_P(newval);

	if (newval == &ztmp) {
		zval_dtor(newval);
	}
	return SUCCESS;
}
/* }}} */

#ifdef HAVE_NET_SNMP
/* {{{ */
static int php_snmp_write_enum_print(php_snmp_object *snmp_object, zval *newval TSRMLS_DC)
{
	zval ztmp;
	if (Z_TYPE_P(newval) != IS_BOOL) {
		ztmp = *newval;
		zval_copy_ctor(&ztmp);
		convert_to_boolean(&ztmp);
		newval = &ztmp;
	}

	snmp_object->enum_print = Z_LVAL_P(newval);
	
	if (newval == &ztmp) {
		zval_dtor(newval);
	}
	return SUCCESS;
}
/* }}} */

/* {{{ */
static int php_snmp_write_oid_output_format(php_snmp_object *snmp_object, zval *newval TSRMLS_DC)
{
	zval ztmp;
	int ret = SUCCESS;
	if (Z_TYPE_P(newval) != IS_LONG) {
		ztmp = *newval;
		zval_copy_ctor(&ztmp);
		convert_to_long(&ztmp);
		newval = &ztmp;
	}
	
	switch(Z_LVAL_P(newval)) {
		case NETSNMP_OID_OUTPUT_SUFFIX:
		case NETSNMP_OID_OUTPUT_MODULE:
		case NETSNMP_OID_OUTPUT_FULL:
		case NETSNMP_OID_OUTPUT_NUMERIC:
		case NETSNMP_OID_OUTPUT_UCD:
		case NETSNMP_OID_OUTPUT_NONE:
			snmp_object->oid_output_format = Z_LVAL_P(newval);
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown SNMP output print format '%ld'", Z_LVAL_P(newval));
			ret = FAILURE;
			break;
	}

	if (newval == &ztmp) {
		zval_dtor(newval);
	}
	return ret;
}
#endif
/* }}} */

/* {{{ php_snmp_class_methods[] */
static zend_function_entry php_snmp_class_methods[] = {
	PHP_ME(snmp,	 open,				arginfo_snmp_open,		ZEND_ACC_PUBLIC)
	PHP_ME(snmp,	 close,				arginfo_snmp_void,		ZEND_ACC_PUBLIC)
	PHP_ME(snmp,	 set_security,			arginfo_snmp_set_security,	ZEND_ACC_PUBLIC)

	PHP_ME(snmp,	 get,				arginfo_snmp_get,		ZEND_ACC_PUBLIC)
	PHP_ME(snmp,	 getnext,			arginfo_snmp_get,		ZEND_ACC_PUBLIC)
	PHP_ME(snmp,	 walk,				arginfo_snmp_walk,		ZEND_ACC_PUBLIC)
	PHP_ME(snmp,	 set,				arginfo_snmp_set,		ZEND_ACC_PUBLIC)

	PHP_MALIAS(snmp, __construct,	open,		arginfo_snmp_open,		ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	{NULL, NULL, NULL}
};

#define PHP_SNMP_PROPERTY_ENTRY_RECORD(name) \
	{ "" #name "",		sizeof("" #name "") - 1,	php_snmp_read_##name,	php_snmp_write_##name }

const php_snmp_prop_handler php_snmp_property_entries[] = {
	PHP_SNMP_PROPERTY_ENTRY_RECORD(info),
	PHP_SNMP_PROPERTY_ENTRY_RECORD(max_oids),
	PHP_SNMP_PROPERTY_ENTRY_RECORD(valueretrieval),
	PHP_SNMP_PROPERTY_ENTRY_RECORD(quick_print),
#ifdef HAVE_NET_SNMP
	PHP_SNMP_PROPERTY_ENTRY_RECORD(enum_print),
	PHP_SNMP_PROPERTY_ENTRY_RECORD(oid_output_format),
#endif
	{ NULL, 0, NULL, NULL}
};
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(snmp)
{
	netsnmp_log_handler *logh;
	zend_class_entry ce;

	le_snmp_session = zend_register_list_destructors_ex(php_snmp_session_destructor, NULL, PHP_SNMP_SESSION_RES_NAME, module_number);

	init_snmp("snmpapp");

#ifdef NETSNMP_DS_LIB_DONT_PERSIST_STATE
	/* Prevent update of the snmpapp.conf file */
	netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_DONT_PERSIST_STATE, 1);
#endif

	/* Disable logging, use exit status'es and related variabled to detect errors */
	shutdown_snmp_logging();
	logh = netsnmp_register_loghandler(NETSNMP_LOGHANDLER_NONE, LOG_ERR);
	if (logh) {
		logh->pri_max = LOG_ERR;
	}

	memcpy(&php_snmp_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	php_snmp_object_handlers.read_property = php_snmp_read_property;
	php_snmp_object_handlers.write_property = php_snmp_write_property;
	php_snmp_object_handlers.has_property = php_snmp_has_property;
	php_snmp_object_handlers.get_properties = php_snmp_get_properties;

	/* Register SNMP Class */
	INIT_CLASS_ENTRY(ce, "SNMP", php_snmp_class_methods);
	ce.create_object = php_snmp_object_new;
	php_snmp_object_handlers.clone_obj = NULL;
	php_snmp_class_entry = zend_register_internal_class(&ce TSRMLS_CC);

	/* Register SNMP Class properties */
	zend_hash_init(&php_snmp_properties, 0, NULL, NULL, 1);
	PHP_SNMP_ADD_PROPERTIES(&php_snmp_properties, php_snmp_property_entries);

#ifdef HAVE_NET_SNMP
	REGISTER_LONG_CONSTANT("SNMP_OID_OUTPUT_SUFFIX", NETSNMP_OID_OUTPUT_SUFFIX, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_OID_OUTPUT_MODULE", NETSNMP_OID_OUTPUT_MODULE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_OID_OUTPUT_FULL", NETSNMP_OID_OUTPUT_FULL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_OID_OUTPUT_NUMERIC", NETSNMP_OID_OUTPUT_NUMERIC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_OID_OUTPUT_UCD", NETSNMP_OID_OUTPUT_UCD, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_OID_OUTPUT_NONE", NETSNMP_OID_OUTPUT_NONE, CONST_CS | CONST_PERSISTENT);
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

	REGISTER_LONG_CONSTANT("SNMP_VERSION_1", SNMP_VERSION_1, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_VERSION_2c", SNMP_VERSION_2c, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SNMP_VERSION_3", SNMP_VERSION_3, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(snmp)
{
	snmp_shutdown("snmpapp");
	
	zend_hash_destroy(&php_snmp_properties);

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
	php_info_print_table_row(2, "PHP SNMP Version", PHP_SNMP_VERSION);
	php_info_print_table_end();
}
/* }}} */

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
	PHP_SNMP_VERSION,
	PHP_MODULE_GLOBALS(snmp),
	PHP_GINIT(snmp),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
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
