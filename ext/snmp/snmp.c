/*
+----------------------------------------------------------------------+
| PHP Version 4                                                        |
+----------------------------------------------------------------------+
| Copyright (c) 1997-2002 The PHP Group                                |
+----------------------------------------------------------------------+
| This source file is subject to version 2.02 of the PHP license,      |
| that is bundled with this package in the file LICENSE, and is        |
| available at through the world-wide-web at                           |
| http://www.php.net/license/2_02.txt.                                 |
| If you did not receive a copy of the PHP license and are unable to   |
| obtain it through the world-wide-web, please send a note to          |
| license@php.net so we can mail you a copy immediately.               |
+----------------------------------------------------------------------+
| Authors: Rasmus Lerdorf <rasmus@php.net>                             |
|          Mike Jackson <mhjack@tscnet.com>                            |
|          Steven Lawrance <slawrance@technologist.com>                |
|          Harrie Hazewinkel <harrie@lisanza.net>                      |
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
#include <winsock.h>
#include <errno.h>
#include <process.h>
#include "win32/time.h"
#elif defined(NETWARE)
#ifdef USE_WINSOCK
/*#include <ws2nlm.h>*/
#include <novsock2.h>
#else
#include <sys/socket.h>
#endif
#include <errno.h>
/*#include <process.h>*/
#ifdef NEW_LIBC
#include <sys/timeval.h>
#else
#include "netware/time_nw.h"
#endif
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
#include "version.h"
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

/* ucd-snmp 3.3.1 changed the name of a few #defines... They've been changed back to the original ones in 3.5.3! */
#ifndef SNMP_MSG_GET
#define SNMP_MSG_GET GET_REQ_MSG
#define SNMP_MSG_GETNEXT GETNEXT_REQ_MSG
#endif

/* constant - can be shared among threads */
static oid objid_mib[] = {1, 3, 6, 1, 2, 1};

/* {{{ snmp_functions[]
 */
function_entry snmp_functions[] = {
	PHP_FE(snmpget, NULL)
	PHP_FE(snmpwalk, NULL)
	PHP_FE(snmprealwalk, NULL)
	PHP_FALIAS(snmpwalkoid, snmprealwalk, NULL)
	PHP_FE(snmp_get_quick_print, NULL)
	PHP_FE(snmp_set_quick_print, NULL)
#ifdef HAVE_NET_SNMP
	PHP_FE(snmp_set_enum_print, NULL)
	PHP_FE(snmp_set_oid_numeric_print, NULL)
#endif
	PHP_FE(snmpset, NULL)

	PHP_FE(snmp3_get, NULL)
	PHP_FE(snmp3_walk, NULL)
	PHP_FE(snmp3_real_walk, NULL)
	PHP_FE(snmp3_set, NULL)
	{NULL,NULL,NULL}
};
/* }}} */

/* {{{ snmp_module_entry
 */
zend_module_entry snmp_module_entry = {
	STANDARD_MODULE_HEADER,
	"snmp",
	snmp_functions,
	PHP_MINIT(snmp),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(snmp),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_SNMP
ZEND_GET_MODULE(snmp)
#endif

/* THREAD_LS snmp_module php_snmp_module; - may need one of these at some point */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(snmp)
{
	init_snmp("snmpapp");
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

/* {{{ php_snmp_internal
*
* Generic SNMP object fetcher (for both v3 and v1)
*
* st=1   snmpget()  - query an agent and return a single value.
* st=2   snmpwalk() - walk the mib and return a single dimensional array 
*                     containing the values.
* st=3   snmprealwalk() and snmpwalkoid() - walk the mib and return an 
*                                           array of oid,value pairs.
* st=5-8 ** Reserved **
* st=11  snmpset()  - query an agent and set a single value
*
*/
static void php_snmp_internal(INTERNAL_FUNCTION_PARAMETERS,
		int st,
		struct snmp_session *session,
		char *objid) 
{
	struct snmp_session *ss;
	struct snmp_pdu *pdu=NULL, *response;
	struct variable_list *vars;
	oid name[MAX_NAME_LEN];
	int name_length;
	oid root[MAX_NAME_LEN];
	int rootlen = 0;
	int gotroot = 0;
	int status, count;
	char buf[2048];
	char buf2[2048];
	int keepwalking=1;
	char type = (char) 0;
	char *value = (char *) 0;
	char *err;

	if (st >= 2) { /* walk */
		rootlen = MAX_NAME_LEN;
		if (strlen(objid)) { /* on a walk, an empty string means top of tree - no error */
			if (read_objid(objid, root, &rootlen)) {
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

	if (st >= 2) {
		memmove((char *)name, (char *)root, rootlen * sizeof(oid));
		name_length = rootlen;
		switch(st) {
			case 2:
			case 3:
				array_init(return_value);
				break;
			default:
				RETVAL_TRUE;
				break;
		}
	}

	while (keepwalking) {
		keepwalking = 0;
		if (st == 1) {
			pdu = snmp_pdu_create(SNMP_MSG_GET);
			name_length = MAX_OID_LEN;
			if (!snmp_parse_oid(objid, name, &name_length)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid object identifier: %s", objid);
				snmp_close(ss);
				RETURN_FALSE;
			}
			snmp_add_null_var(pdu, name, name_length);
		} else if (st == 11) {
			pdu = snmp_pdu_create(SNMP_MSG_SET);
			if (snmp_add_var(pdu, name, name_length, type, value)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not add variable: %s", name);
				snmp_close(ss);
				RETURN_FALSE;
			}
		} else if (st >= 2) {
			pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
			snmp_add_null_var(pdu, name, name_length);
		}

retry:
		status = snmp_synch_response(ss, pdu, &response);
		if (status == STAT_SUCCESS) {
			if (response->errstat == SNMP_ERR_NOERROR) {
				for (vars = response->variables; vars; vars = vars->next_variable) {
					if (st >= 2 && st != 11 && 
						(vars->name_length < rootlen || memcmp(root, vars->name, rootlen * sizeof(oid)))) {
						continue;       /* not part of this subtree */
					}

					if (st != 11) {
#ifdef HAVE_NET_SNMP
						snprint_value(buf, sizeof(buf), vars->name, vars->name_length, vars);
#else
						sprint_value(buf,vars->name, vars->name_length, vars);
#endif
					}

					if (st == 1) {
						RETVAL_STRING(buf,1);
					} else if (st == 2) {
						add_next_index_string(return_value,buf,1); /* Add to returned array */
					} else if (st == 3)  {
#ifdef HAVE_NET_SNMP
						snprint_objid(buf2, sizeof(buf2), vars->name, vars->name_length);
#else
						sprint_objid(buf2, vars->name, vars->name_length);
#endif
						add_assoc_string(return_value,buf2,buf,1);
					}
					if (st >= 2 && st != 11) {
						if (vars->type != SNMP_ENDOFMIBVIEW && 
							vars->type != SNMP_NOSUCHOBJECT && vars->type != SNMP_NOSUCHINSTANCE) {
							memmove((char *)name, (char *)vars->name,vars->name_length * sizeof(oid));
							name_length = vars->name_length;
							keepwalking = 1;
						}
					}
				}	
			} else {
				if (st != 2 || response->errstat != SNMP_ERR_NOSUCHNAME) {
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
					if (st == 1) {
						if ((pdu = snmp_fix_pdu(response, SNMP_MSG_GET)) != NULL) {
							goto retry;
						}
					} else if (st == 11) {
						if ((pdu = snmp_fix_pdu(response, SNMP_MSG_SET)) != NULL) {
							goto retry;
						}
					} else if (st >= 2) {
						if ((pdu = snmp_fix_pdu(response, SNMP_MSG_GETNEXT)) != NULL) {
							goto retry;
						}
					}
					snmp_close(ss);
					RETURN_FALSE;
				}
			}
		} else if (status == STAT_TIMEOUT) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "No response from %s", session->peername);
			if (st == 2 || st == 3) {
				zval_dtor(return_value);
			}
			snmp_close(ss);
			RETURN_FALSE;
		} else {    /* status == STAT_ERROR */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "An error occurred, quitting");
			if (st == 2 || st == 3) {
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
* Generic SNMPv1 handler
* This function makes use of the internal SNMP object fetcher.
* The object fetcher is shared with SNMPv3.
*
* st=1   snmpget() - query an agent and return a single value.
* st=2   snmpwalk() - walk the mib and return a single dimensional array 
*          containing the values.
* st=3 snmprealwalk() and snmpwalkoid() - walk the mib and return an 
*          array of oid,value pairs.
* st=5-8 ** Reserved **
* st=11  snmpset() - query an agent and set a single value
*
*/
static void php_snmp(INTERNAL_FUNCTION_PARAMETERS, int st) 
{
	zval **a1, **a2, **a3, **a4, **a5, **a6, **a7;
	struct snmp_session session;
	long timeout=SNMP_DEFAULT_TIMEOUT;
	long retries=SNMP_DEFAULT_RETRIES;
	int myargc = ZEND_NUM_ARGS();
	char type = (char) 0;
	char *value = (char *) 0;
	char hostname[MAX_NAME_LEN];
	int remote_port = 161;
	char *pptr;

	if (myargc < 3 || myargc > 7 ||
		zend_get_parameters_ex(myargc, &a1, &a2, &a3, &a4, &a5, &a6, &a7) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(a1);
	convert_to_string_ex(a2);
	convert_to_string_ex(a3);
	
	if (st == 11) {
		if (myargc < 5) {
			WRONG_PARAM_COUNT;
		}

		convert_to_string_ex(a4);
		convert_to_string_ex(a5);

		if(myargc > 5) {
			convert_to_long_ex(a6);
			timeout = Z_LVAL_PP(a6);
		}

		if(myargc > 6) {
			convert_to_long_ex(a7);
			retries = Z_LVAL_PP(a7);
		}

		type = Z_STRVAL_PP(a4)[0];
		value = Z_STRVAL_PP(a5);
	} else {
		if(myargc > 3) {
			convert_to_long_ex(a4);
			timeout = Z_LVAL_PP(a4);
		}

		if(myargc > 4) {
			convert_to_long_ex(a5);
			retries = Z_LVAL_PP(a5);
		}
	}

	snmp_sess_init(&session);
	strcpy (hostname, Z_STRVAL_PP(a1));
	if ((pptr = strchr (hostname, ':'))) {
		remote_port = strtol (pptr + 1, NULL, 0);
		*pptr = 0;
	}

	session.peername = hostname;
	session.remote_port = remote_port;
	session.version = SNMP_VERSION_1;
	/*
	* FIXME: potential memory leak
	* This is a workaround for an "artifact" (Mike Slifcak)
	* in (at least) ucd-snmp 3.6.1 which frees
	* memory it did not allocate
	*/
#ifdef UCD_SNMP_HACK
	session.community = (u_char *)strdup(Z_STRVAL_PP(a2)); /* memory freed by SNMP library, strdup NOT estrdup */
#else
	session.community = (u_char *)Z_STRVAL_PP(a2);
#endif
	session.community_len = Z_STRLEN_PP(a2);
	session.retries = retries;
	session.timeout = timeout;
	
	session.authenticator = NULL;

	php_snmp_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU, st, &session, Z_STRVAL_PP(a3));
}
/* }}} */

/* {{{ proto string snmpget(string host, string community, string object_id [, int timeout [, int retries]]) 
   Fetch a SNMP object */
PHP_FUNCTION(snmpget)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);
}
/* }}} */

/* {{{ proto array snmpwalk(string host, string community, string object_id [, int timeout [, int retries]]) 
   Return all objects under the specified object id */
PHP_FUNCTION(snmpwalk)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU,2);
}
/* }}} */

/* {{{ proto array snmprealwalk(string host, string community, string object_id [, int timeout [, int retries]])
   Return all objects including their respective object id withing the specified one */
PHP_FUNCTION(snmprealwalk)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU,3);
}
/* }}} */

/* {{{ proto bool snmp_get_quick_print(void)
   Return the current status of quick_print */
PHP_FUNCTION(snmp_get_quick_print)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
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
	int argc = ZEND_NUM_ARGS();
	long a1;

	if (zend_parse_parameters(argc TSRMLS_CC, "l", &a1) == FAILURE) {
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
	int argc = ZEND_NUM_ARGS();
	long a1;

	if (zend_parse_parameters(argc TSRMLS_CC, "l", &a1) == FAILURE) {
		return;
	}

	netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_PRINT_NUMERIC_ENUM, (int) a1);
} 
/* }}} */

/* {{{ proto void snmp_set_oid_numeric_print(int oid_numeric_print)
   Return all objects including their respective object id withing the specified one */
PHP_FUNCTION(snmp_set_oid_numeric_print)
{
	int argc = ZEND_NUM_ARGS();
	long a1;

	if (zend_parse_parameters(argc TSRMLS_CC, "l", &a1) == FAILURE) {
		return;
	}
	if ((int) a1 != 0) {
		netsnmp_ds_set_int(NETSNMP_DS_LIBRARY_ID,
			NETSNMP_DS_LIB_OID_OUTPUT_FORMAT,
			NETSNMP_OID_OUTPUT_NUMERIC);
	}
} 
/* }}} */
#endif

/* {{{ proto int snmpset(string host, string community, string object_id, string type, mixed value [, int timeout [, int retries]]) 
   Set the value of a SNMP object */
PHP_FUNCTION(snmpset)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU,11);
}
/* }}} */

/* {{{ proto int netsnmp_session_set_sec_name(struct snmp_session *s, char *name)
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

/* {{{ proto int netsnmp_session_set_sec_level(struct snmp_session *s, char *level)
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
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid security level: %s", level);
	}
	return (-1);
}
/* }}} */

/* {{{ proto int netsnmp_session_set_auth_protocol(struct snmp_session *s, char *prot)
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
		} else if (strlen(prot)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid authentication protocol: %s", prot);
		}
	}
	return (-1);
}
/* }}} */

/* {{{ proto int netsnmp_session_set_sec_protocol(struct snmp_session *s, char *prot)
   Set the security protocol in the snmpv3 session */
static int netsnmp_session_set_sec_protocol(struct snmp_session *s, char *prot TSRMLS_DC)
{
	if ((s) && (prot)) {
		if (!strcasecmp(prot, "DES")) {
			s->securityPrivProto = usmDESPrivProtocol;
			s->securityPrivProtoLen = OIDSIZE(usmDESPrivProtocol);
			return (0);
#ifdef HAVE_AES
		} else if (!strcasecmp(prot, "AES128")) {
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
		} else if (strlen(prot)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid privacy protocol: %s", prot);
		}
	}
	return (-1);
}
/* }}} */

/* {{{ proto int netsnmp_session_gen_auth_key(struct snmp_session *s, char *pass)
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
			oid *def = get_default_authtype(&(s->securityAuthProtoLen));
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

/* {{{ proto int netsnmp_session_gen_sec_key(struct snmp_session *s, u_char *pass)
   Make key from pass phrase in the snmpv3 session */
static int netsnmp_session_gen_sec_key(struct snmp_session *s, u_char *pass TSRMLS_DC)
{
	if ((s) && (pass) && strlen(pass)) {
		s->securityPrivKeyLen = USM_PRIV_KU_LEN;
		if (s->securityPrivProto == NULL) {
			/* get .conf set default */
			oid *def = get_default_privtype(&(s->securityPrivProtoLen));
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


/* {{{ proto void php_snmpv3(INTERNAL_FUNCTION_PARAMETERS, int st)
*
* Generic SNMPv3 object fetcher
* From here is passed on the the common internal object fetcher.
*
* st=1   snmp3_get() - query an agent and return a single value.
* st=2   snmp3_walk() - walk the mib and return a single dimensional array 
*                       containing the values.
* st=3   snmp3_real_walk() - walk the mib and return an 
*                            array of oid,value pairs.
* st=11  snmp3_set() - query an agent and set a single value
*
*/
void php_snmpv3(INTERNAL_FUNCTION_PARAMETERS, int st) {
	zval **a1, **a2, **a3, **a4, **a5, **a6, **a7, **a8, **a9, **a10, **a11, **a12;
	struct snmp_session session;
	long timeout=SNMP_DEFAULT_TIMEOUT;
	long retries=SNMP_DEFAULT_RETRIES;
	int myargc = ZEND_NUM_ARGS();
	char type = (char) 0;
	char *value = (char *) 0;
	char hostname[MAX_NAME_LEN];
	int remote_port = 161;
	char *pptr;

	if (myargc < 8 || myargc > 12 ||
		zend_get_parameters_ex(myargc, &a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &a10, &a11, &a12) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	snmp_sess_init(&session);
	/* This is all SNMPv3 */
	session.version = SNMP_VERSION_3;

	/* Reading the hostname and its optional non-default port number */
	convert_to_string_ex(a1);
	strcpy(hostname, Z_STRVAL_PP(a1));
	if ((pptr = strchr (hostname, ':'))) {
		remote_port = strtol (pptr + 1, NULL, 0);
		*pptr = 0;
	}
	session.peername = hostname;
	session.remote_port = remote_port;

	/* Setting the security name. */
	convert_to_string_ex(a2);
	if (netsnmp_session_set_sec_name(&session, Z_STRVAL_PP(a2))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could net set security name: %s", Z_STRVAL_PP(a2));
		RETURN_FALSE;
	}

	/* Setting the security level. */
	convert_to_string_ex(a3);
	if (netsnmp_session_set_sec_level(&session, Z_STRVAL_PP(a3) TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid security level: %s", a3);
		RETURN_FALSE;
	}

	/* Setting the authentication protocol. */
	convert_to_string_ex(a4);
	if (netsnmp_session_set_auth_protocol(&session, Z_STRVAL_PP(a4) TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid authentication protocol: %s", Z_STRVAL_PP(a4));
		RETURN_FALSE;
	}
	/* Setting the authentication passphrase. */
	convert_to_string_ex(a5);
	if (netsnmp_session_gen_auth_key(&session, Z_STRVAL_PP(a5) TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not generate key for authentication pass phrase: %s", Z_STRVAL_PP(a4));
		RETURN_FALSE;
	}

	/* Setting the security protocol. */
	convert_to_string_ex(a6);
	if (netsnmp_session_set_sec_protocol(&session, Z_STRVAL_PP(a6) TSRMLS_CC) &&
			(0 != strlen(Z_STRVAL_PP(a6)))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid security protocol: %s", Z_STRVAL_PP(a6));
		RETURN_FALSE;
	}
	/* Setting the security protocol passphrase. */
	convert_to_string_ex(a7);
	if (netsnmp_session_gen_sec_key(&session, Z_STRVAL_PP(a7) TSRMLS_CC) &&
							(0 != strlen(Z_STRVAL_PP(a7)))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not generate key for security pass phrase: %s", Z_STRVAL_PP(a7));
		RETURN_FALSE;
	}

	if (st == 11) {
		if (myargc < 10) {
			WRONG_PARAM_COUNT;
		}
		if (myargc > 10) {
			convert_to_long_ex(a11);
			timeout = Z_LVAL_PP(a11);
		}
		if (myargc > 11) {
			convert_to_long_ex(a12);
			retries = Z_LVAL_PP(a12);
		}
		convert_to_string_ex(a9);
		convert_to_string_ex(a10);
		type = Z_STRVAL_PP(a9)[0];
		value = Z_STRVAL_PP(a10);
	} else {
		if (myargc > 8) {
			convert_to_long_ex(a9);
			timeout = Z_LVAL_PP(a9);
		}
		if (myargc > 9) {
			convert_to_long_ex(a10);
			retries = Z_LVAL_PP(a10);
		}
	}

	session.retries = retries;
	session.timeout = timeout;

	php_snmp_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU, st, &session, Z_STRVAL_PP(a8));
}
/* }}} */

/* {{{ proto int snmp3_get(string host, string sec_name, string sec_level, string auth_protocol, string auth_passphrase, string priv_protocol, string priv_passphrase, string object_id [, int timeout [, int retries]])
   Fetch the value of a SNMP object */
PHP_FUNCTION(snmp3_get)
{
	php_snmpv3(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */


/* {{{ proto int snmp3_walk(string host, string sec_name, string sec_level, string auth_protocol, string auth_passphrase, string priv_protocol, string priv_passphrase, string object_id [, int timeout [, int retries]])
   Fetch the value of a SNMP object */
PHP_FUNCTION(snmp3_walk)
{
	php_snmpv3(INTERNAL_FUNCTION_PARAM_PASSTHRU, 2);
}
/* }}} */

/* {{{ proto int snmp3_real_walk(string host, string sec_name, string sec_level, string auth_protocol, string auth_passphrase, string priv_protocol, string priv_passphrase, string object_id [, int timeout [, int retries]])
   Fetch the value of a SNMP object */
PHP_FUNCTION(snmp3_real_walk)
{
	php_snmpv3(INTERNAL_FUNCTION_PARAM_PASSTHRU, 3);
}
/* }}} */

/* {{{ proto int snmp3_set(string host, string sec_name, string sec_level, string auth_protocol, string auth_passphrase, string priv_protocol, string priv_passphrase, string object_id, string type, mixed value [, int timeout [, int retries]])
   Fetch the value of a SNMP object */
PHP_FUNCTION(snmp3_set)
{
	php_snmpv3(INTERNAL_FUNCTION_PARAM_PASSTHRU, 11);
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
