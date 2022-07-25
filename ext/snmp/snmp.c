/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "main/php_network.h"
#include "ext/standard/info.h"
#include "php_snmp.h"

#include "zend_exceptions.h"
#include "zend_smart_string.h"
#include "ext/spl/spl_exceptions.h"

#ifdef HAVE_SNMP

#include <sys/types.h>
#include <errno.h>
#ifdef PHP_WIN32
#include <winsock2.h>
#include <process.h>
#include "win32/time.h"
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <locale.h>

#ifndef __P
#ifdef __GNUC__
#define __P(args) args
#else
#define __P(args) ()
#endif
#endif

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include "snmp_arginfo.h"

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

typedef struct snmp_session php_snmp_session;

#define PHP_SNMP_ADD_PROPERTIES(a, b) \
{ \
	int i = 0; \
	while (b[i].name != NULL) { \
		php_snmp_add_property((a), (b)[i].name, (b)[i].name_length, \
							(php_snmp_read_t)(b)[i].read_func, (php_snmp_write_t)(b)[i].write_func); \
		i++; \
	} \
}

ZEND_DECLARE_MODULE_GLOBALS(snmp)
static PHP_GINIT_FUNCTION(snmp);

/* constant - can be shared among threads */
static oid objid_mib[] = {1, 3, 6, 1, 2, 1};

/* Handlers */
static zend_object_handlers php_snmp_object_handlers;

/* Class entries */
zend_class_entry *php_snmp_ce;
zend_class_entry *php_snmp_exception_ce;

/* Class object properties */
static HashTable php_snmp_properties;

struct objid_query {
	int count;
	int offset;
	int step;
	zend_long non_repeaters;
	zend_long max_repetitions;
	int valueretrieval;
	bool array_output;
	bool oid_increasing_check;
	snmpobjarg *vars;
};

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
/* use user-supplied OID names for keys in array output mode in GET method */
#define SNMP_ORIGINAL_NAMES_AS_KEYS	(1<<8)
/* use OID suffix (`index') for keys in array output mode in WALK  method */
#define SNMP_USE_SUFFIX_AS_KEYS	(1<<9)

#ifdef COMPILE_DL_SNMP
ZEND_GET_MODULE(snmp)
#endif

/* {{{ PHP_GINIT_FUNCTION */
static PHP_GINIT_FUNCTION(snmp)
{
	snmp_globals->valueretrieval = SNMP_VALUE_LIBRARY;
}
/* }}} */

#define PHP_SNMP_SESSION_FREE(a) { \
	if ((*session)->a) { \
		efree((*session)->a); \
		(*session)->a = NULL; \
	} \
}

static void netsnmp_session_free(php_snmp_session **session) /* {{{ */
{
	if (*session) {
		PHP_SNMP_SESSION_FREE(peername);
		PHP_SNMP_SESSION_FREE(community);
		PHP_SNMP_SESSION_FREE(securityName);
		PHP_SNMP_SESSION_FREE(contextEngineID);
		efree(*session);
		*session = NULL;
	}
}
/* }}} */

static void php_snmp_object_free_storage(zend_object *object) /* {{{ */
{
	php_snmp_object *intern = php_snmp_fetch_object(object);

	if (!intern) {
		return;
	}

	netsnmp_session_free(&(intern->session));

	zend_object_std_dtor(&intern->zo);
}
/* }}} */

static zend_object *php_snmp_object_new(zend_class_entry *class_type) /* {{{ */
{
	php_snmp_object *intern;

	/* Allocate memory for it */
	intern = zend_object_alloc(sizeof(php_snmp_object), class_type);

	zend_object_std_init(&intern->zo, class_type);
	object_properties_init(&intern->zo, class_type);

	intern->zo.handlers = &php_snmp_object_handlers;

	return &intern->zo;

}
/* }}} */

/* {{{ php_snmp_error
 *
 * Record last SNMP-related error in object
 *
 */
static void php_snmp_error(zval *object, int type, const char *format, ...)
{
	va_list args;
	php_snmp_object *snmp_object = NULL;

	if (object) {
		snmp_object = Z_SNMP_P(object);
		if (type == PHP_SNMP_ERRNO_NOERROR) {
			memset(snmp_object->snmp_errstr, 0, sizeof(snmp_object->snmp_errstr));
		} else {
			va_start(args, format);
			vsnprintf(snmp_object->snmp_errstr, sizeof(snmp_object->snmp_errstr) - 1, format, args);
			va_end(args);
		}
		snmp_object->snmp_errno = type;
	}

	if (type == PHP_SNMP_ERRNO_NOERROR) {
		return;
	}

	if (object && (snmp_object->exceptions_enabled & type)) {
		zend_throw_exception_ex(php_snmp_exception_ce, type, "%s", snmp_object->snmp_errstr);
	} else {
		va_start(args, format);
		php_verror(NULL, "", E_WARNING, format, args);
		va_end(args);
	}
}

/* }}} */

/* {{{ php_snmp_getvalue
*
* SNMP value to zval converter
*
*/
static void php_snmp_getvalue(struct variable_list *vars, zval *snmpval, int valueretrieval)
{
	zval val;
	char sbuf[512];
	char *buf = &(sbuf[0]);
	char *dbuf = (char *)NULL;
	int buflen = sizeof(sbuf) - 1;
	int val_len = vars->val_len;

	/* use emalloc() for large values, use static array otherwise */

	/* There is no way to know the size of buffer snprint_value() needs in order to print a value there.
	 * So we are forced to probe it
	 */
	while ((valueretrieval & SNMP_VALUE_PLAIN) == 0) {
		*buf = '\0';
		if (snprint_value(buf, buflen, vars->name, vars->name_length, vars) == -1) {
			if (val_len > 512*1024) {
				php_error_docref(NULL, E_WARNING, "snprint_value() asks for a buffer more than 512k, Net-SNMP bug?");
				break;
			}
			 /* buffer is not long enough to hold full output, double it */
			val_len *= 2;
		} else {
			break;
		}

		if (buf == dbuf) {
			dbuf = (char *)erealloc(dbuf, val_len + 1);
		} else {
			dbuf = (char *)emalloc(val_len + 1);
		}

		buf = dbuf;
		buflen = val_len;
	}

	if((valueretrieval & SNMP_VALUE_PLAIN) && val_len > buflen){
		dbuf = (char *)emalloc(val_len + 1);
		buf = dbuf;
		buflen = val_len;
	}

	if (valueretrieval & SNMP_VALUE_PLAIN) {
		*buf = 0;
		switch (vars->type) {
		case ASN_BIT_STR:		/* 0x03, asn1.h */
			ZVAL_STRINGL(&val, (char *)vars->val.bitstring, vars->val_len);
			break;

		case ASN_OCTET_STR:		/* 0x04, asn1.h */
		case ASN_OPAQUE:		/* 0x44, snmp_impl.h */
			ZVAL_STRINGL(&val, (char *)vars->val.string, vars->val_len);
			break;

		case ASN_NULL:			/* 0x05, asn1.h */
			ZVAL_NULL(&val);
			break;

		case ASN_OBJECT_ID:		/* 0x06, asn1.h */
			snprint_objid(buf, buflen, vars->val.objid, vars->val_len / sizeof(oid));
			ZVAL_STRING(&val, buf);
			break;

		case ASN_IPADDRESS:		/* 0x40, snmp_impl.h */
			snprintf(buf, buflen, "%d.%d.%d.%d",
				 (vars->val.string)[0], (vars->val.string)[1],
				 (vars->val.string)[2], (vars->val.string)[3]);
			buf[buflen]=0;
			ZVAL_STRING(&val, buf);
			break;

		case ASN_COUNTER:		/* 0x41, snmp_impl.h */
		case ASN_GAUGE:			/* 0x42, snmp_impl.h */
		/* ASN_UNSIGNED is the same as ASN_GAUGE */
		case ASN_TIMETICKS:		/* 0x43, snmp_impl.h */
		case ASN_UINTEGER:		/* 0x47, snmp_impl.h */
			snprintf(buf, buflen, "%lu", *vars->val.integer);
			buf[buflen]=0;
			ZVAL_STRING(&val, buf);
			break;

		case ASN_INTEGER:		/* 0x02, asn1.h */
			snprintf(buf, buflen, "%ld", *vars->val.integer);
			buf[buflen]=0;
			ZVAL_STRING(&val, buf);
			break;

#if defined(NETSNMP_WITH_OPAQUE_SPECIAL_TYPES) || defined(OPAQUE_SPECIAL_TYPES)
		case ASN_OPAQUE_FLOAT:		/* 0x78, asn1.h */
			snprintf(buf, buflen, "%f", *vars->val.floatVal);
			ZVAL_STRING(&val, buf);
			break;

		case ASN_OPAQUE_DOUBLE:		/* 0x79, asn1.h */
			snprintf(buf, buflen, "%f", *vars->val.doubleVal);
			ZVAL_STRING(&val, buf);
			break;

		case ASN_OPAQUE_I64:		/* 0x80, asn1.h */
			printI64(buf, vars->val.counter64);
			ZVAL_STRING(&val, buf);
			break;

		case ASN_OPAQUE_U64:		/* 0x81, asn1.h */
#endif
		case ASN_COUNTER64:		/* 0x46, snmp_impl.h */
			printU64(buf, vars->val.counter64);
			ZVAL_STRING(&val, buf);
			break;

		default:
			ZVAL_STRING(&val, "Unknown value type");
			php_error_docref(NULL, E_WARNING, "Unknown value type: %u", vars->type);
			break;
		}
	} else /* use Net-SNMP value translation */ {
		/* we have desired string in buffer, just use it */
		ZVAL_STRING(&val, buf);
	}

	if (valueretrieval & SNMP_VALUE_OBJECT) {
		object_init(snmpval);
		add_property_long(snmpval, "type", vars->type);
		add_property_zval(snmpval, "value", &val);
	} else  {
		ZVAL_COPY(snmpval, &val);
	}
	zval_ptr_dtor(&val);

	if (dbuf){ /* malloc was used to store value */
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
							struct objid_query *objid_query)
{
	struct snmp_session *ss;
	struct snmp_pdu *pdu=NULL, *response;
	struct variable_list *vars;
	oid root[MAX_NAME_LEN];
	size_t rootlen = 0;
	int status, count, found;
	char buf[2048];
	char buf2[2048];
	bool keepwalking = true;
	char *err;
	zval snmpval;
	int snmp_errno;

	/* we start with retval=FALSE. If any actual data is acquired, retval will be set to appropriate type */
	RETVAL_FALSE;

	/* reset errno and errstr */
	php_snmp_error(getThis(), PHP_SNMP_ERRNO_NOERROR, "");

	if (st & SNMP_CMD_WALK) { /* remember root OID */
		memmove((char *)root, (char *)(objid_query->vars[0].name), (objid_query->vars[0].name_length) * sizeof(oid));
		rootlen = objid_query->vars[0].name_length;
		objid_query->offset = objid_query->count;
	}

	if ((ss = snmp_open(session)) == NULL) {
		snmp_error(session, NULL, NULL, &err);
		php_error_docref(NULL, E_WARNING, "Could not open snmp connection: %s", err);
		free(err);
		RETURN_FALSE;
	}

	if ((st & SNMP_CMD_SET) && objid_query->count > objid_query->step) {
		php_snmp_error(getThis(), PHP_SNMP_ERRNO_MULTIPLE_SET_QUERIES, "Cannot fit all OIDs for SET query into one packet, using multiple queries");
	}

	while (keepwalking) {
		keepwalking = false;
		if (st & SNMP_CMD_WALK) {
			if (session->version == SNMP_VERSION_1) {
				pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
			} else {
				pdu = snmp_pdu_create(SNMP_MSG_GETBULK);
				pdu->non_repeaters = objid_query->non_repeaters;
				pdu->max_repetitions = objid_query->max_repetitions;
			}
			snmp_add_null_var(pdu, objid_query->vars[0].name, objid_query->vars[0].name_length);
		} else {
			if (st & SNMP_CMD_GET) {
				pdu = snmp_pdu_create(SNMP_MSG_GET);
			} else if (st & SNMP_CMD_GETNEXT) {
				pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
			} else if (st & SNMP_CMD_SET) {
				pdu = snmp_pdu_create(SNMP_MSG_SET);
			} else {
				snmp_close(ss);
				php_error_docref(NULL, E_ERROR, "Unknown SNMP command (internals)");
				RETURN_FALSE;
			}
			for (count = 0; objid_query->offset < objid_query->count && count < objid_query->step; objid_query->offset++, count++){
				if (st & SNMP_CMD_SET) {
					if ((snmp_errno = snmp_add_var(pdu, objid_query->vars[objid_query->offset].name, objid_query->vars[objid_query->offset].name_length, objid_query->vars[objid_query->offset].type, objid_query->vars[objid_query->offset].value))) {
						snprint_objid(buf, sizeof(buf), objid_query->vars[objid_query->offset].name, objid_query->vars[objid_query->offset].name_length);
						php_snmp_error(getThis(), PHP_SNMP_ERRNO_OID_PARSING_ERROR, "Could not add variable: OID='%s' type='%c' value='%s': %s", buf, objid_query->vars[objid_query->offset].type, objid_query->vars[objid_query->offset].value, snmp_api_errstring(snmp_errno));
						snmp_free_pdu(pdu);
						snmp_close(ss);
						RETURN_FALSE;
					}
				} else {
					snmp_add_null_var(pdu, objid_query->vars[objid_query->offset].name, objid_query->vars[objid_query->offset].name_length);
				}
			}
			if(pdu->variables == NULL){
				snmp_free_pdu(pdu);
				snmp_close(ss);
				RETURN_FALSE;
			}
		}

retry:
		status = snmp_synch_response(ss, pdu, &response);
		if (status == STAT_SUCCESS) {
			if (response->errstat == SNMP_ERR_NOERROR) {
				if (st & SNMP_CMD_SET) {
					if (objid_query->offset < objid_query->count) { /* we have unprocessed OIDs */
						keepwalking = true;
						snmp_free_pdu(response);
						continue;
					}
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
						snprint_objid(buf, sizeof(buf), vars->name, vars->name_length);
						snprint_value(buf2, sizeof(buf2), vars->name, vars->name_length, vars);
						php_snmp_error(getThis(), PHP_SNMP_ERRNO_ERROR_IN_REPLY, "Error in packet at '%s': %s", buf, buf2);
						continue;
					}

					if ((st & SNMP_CMD_WALK) &&
						(vars->name_length < rootlen || memcmp(root, vars->name, rootlen * sizeof(oid)))) { /* not part of this subtree */
						if (Z_TYPE_P(return_value) == IS_ARRAY) { /* some records are fetched already, shut down further lookup */
							keepwalking = false;
						} else {
							/* first fetched OID is out of subtree, fallback to GET query */
							st |= SNMP_CMD_GET;
							st ^= SNMP_CMD_WALK;
							objid_query->offset = 0;
							keepwalking = true;
						}
						break;
					}

					ZVAL_NULL(&snmpval);
					php_snmp_getvalue(vars, &snmpval, objid_query->valueretrieval);

					if (objid_query->array_output) {
						if (Z_TYPE_P(return_value) == IS_TRUE || Z_TYPE_P(return_value) == IS_FALSE) {
							array_init(return_value);
						}
						if (st & SNMP_NUMERIC_KEYS) {
							add_next_index_zval(return_value, &snmpval);
						} else if (st & SNMP_ORIGINAL_NAMES_AS_KEYS && st & SNMP_CMD_GET) {
							found = 0;
							for (count = 0; count < objid_query->count; count++) {
								if (objid_query->vars[count].name_length == vars->name_length && snmp_oid_compare(objid_query->vars[count].name, objid_query->vars[count].name_length, vars->name, vars->name_length) == 0) {
									found = 1;
									objid_query->vars[count].name_length = 0; /* mark this name as used */
									break;
								}
							}
							if (found) {
								add_assoc_zval(return_value, objid_query->vars[count].oid, &snmpval);
							} else {
								snprint_objid(buf2, sizeof(buf2), vars->name, vars->name_length);
								php_error_docref(NULL, E_WARNING, "Could not find original OID name for '%s'", buf2);
							}
						} else if (st & SNMP_USE_SUFFIX_AS_KEYS && st & SNMP_CMD_WALK) {
							snprint_objid(buf2, sizeof(buf2), vars->name, vars->name_length);
							if (rootlen <= vars->name_length && snmp_oid_compare(root, rootlen, vars->name, rootlen) == 0) {
								buf2[0] = '\0';
								count = rootlen;
								while(count < vars->name_length){
									sprintf(buf, "%lu.", vars->name[count]);
									strcat(buf2, buf);
									count++;
								}
								buf2[strlen(buf2) - 1] = '\0'; /* remove trailing '.' */
							}
							add_assoc_zval(return_value, buf2, &snmpval);
						} else {
							snprint_objid(buf2, sizeof(buf2), vars->name, vars->name_length);
							add_assoc_zval(return_value, buf2, &snmpval);
						}
					} else {
						ZVAL_COPY_VALUE(return_value, &snmpval);
						break;
					}

					/* OID increase check */
					if (st & SNMP_CMD_WALK) {
						if (objid_query->oid_increasing_check && snmp_oid_compare(objid_query->vars[0].name, objid_query->vars[0].name_length, vars->name, vars->name_length) >= 0) {
							snprint_objid(buf2, sizeof(buf2), vars->name, vars->name_length);
							php_snmp_error(getThis(), PHP_SNMP_ERRNO_OID_NOT_INCREASING, "Error: OID not increasing: %s", buf2);
							keepwalking = false;
						} else {
							memmove((char *)(objid_query->vars[0].name), (char *)vars->name, vars->name_length * sizeof(oid));
							objid_query->vars[0].name_length = vars->name_length;
							keepwalking = true;
						}
					}
				}
				if (objid_query->offset < objid_query->count) { /* we have unprocessed OIDs */
					keepwalking = true;
				}
			} else {
				if (st & SNMP_CMD_WALK && response->errstat == SNMP_ERR_TOOBIG && objid_query->max_repetitions > 1) { /* Answer will not fit into single packet */
					objid_query->max_repetitions /= 2;
					snmp_free_pdu(response);
					keepwalking = true;
					continue;
				}
				if (!(st & SNMP_CMD_WALK) || response->errstat != SNMP_ERR_NOSUCHNAME || Z_TYPE_P(return_value) == IS_TRUE || Z_TYPE_P(return_value) == IS_FALSE) {
					for (count=1, vars = response->variables;
						vars && count != response->errindex;
						vars = vars->next_variable, count++);

					if (st & (SNMP_CMD_GET | SNMP_CMD_GETNEXT) && response->errstat == SNMP_ERR_TOOBIG && objid_query->step > 1) { /* Answer will not fit into single packet */
						objid_query->offset = ((objid_query->offset > objid_query->step) ? (objid_query->offset - objid_query->step) : 0 );
						objid_query->step /= 2;
						snmp_free_pdu(response);
						keepwalking = true;
						continue;
					}
					if (vars) {
						snprint_objid(buf, sizeof(buf), vars->name, vars->name_length);
						php_snmp_error(getThis(), PHP_SNMP_ERRNO_ERROR_IN_REPLY, "Error in packet at '%s': %s", buf, snmp_errstring(response->errstat));
					} else {
						php_snmp_error(getThis(), PHP_SNMP_ERRNO_ERROR_IN_REPLY, "Error in packet at %u object_id: %s", response->errindex, snmp_errstring(response->errstat));
					}
					if (st & (SNMP_CMD_GET | SNMP_CMD_GETNEXT)) { /* cut out bogus OID and retry */
						if ((pdu = snmp_fix_pdu(response, ((st & SNMP_CMD_GET) ? SNMP_MSG_GET : SNMP_MSG_GETNEXT) )) != NULL) {
							snmp_free_pdu(response);
							goto retry;
						}
					}
					snmp_free_pdu(response);
					snmp_close(ss);
					if (objid_query->array_output) {
						zval_ptr_dtor(return_value);
					}
					RETURN_FALSE;
				}
			}
		} else if (status == STAT_TIMEOUT) {
			php_snmp_error(getThis(), PHP_SNMP_ERRNO_TIMEOUT, "No response from %s", session->peername);
			if (objid_query->array_output) {
				zval_ptr_dtor(return_value);
			}
			snmp_close(ss);
			RETURN_FALSE;
		} else {    /* status == STAT_ERROR */
			snmp_error(ss, NULL, NULL, &err);
			php_snmp_error(getThis(), PHP_SNMP_ERRNO_GENERIC, "Fatal error: %s", err);
			free(err);
			if (objid_query->array_output) {
				zval_ptr_dtor(return_value);
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
static bool php_snmp_parse_oid(
	zval *object, int st, struct objid_query *objid_query, zend_string *oid_str, HashTable *oid_ht,
	zend_string *type_str, HashTable *type_ht, zend_string *value_str, HashTable *value_ht
) {
	char *pptr;
	uint32_t idx_type = 0, idx_value = 0;
	zval *tmp_oid, *tmp_type, *tmp_value;

	objid_query->count = 0;
	objid_query->array_output = (st & SNMP_CMD_WALK) != 0;
	if (oid_str) {
		objid_query->vars = (snmpobjarg *)emalloc(sizeof(snmpobjarg));
		objid_query->vars[objid_query->count].oid = ZSTR_VAL(oid_str);
		if (st & SNMP_CMD_SET) {
			if (type_ht) {
				zend_type_error("Type must be of type string when object ID is a string");
				efree(objid_query->vars);
				return false;
			}
			if (value_ht) {
				zend_type_error("Value must be of type string when object ID is a string");
				efree(objid_query->vars);
				return false;
			}

			/* Both type and value must be valid strings */
			ZEND_ASSERT(type_str && value_str);

			if (ZSTR_LEN(type_str) != 1) {
				zend_value_error("Type must be a single character");
				efree(objid_query->vars);
				return false;
			}
			pptr = ZSTR_VAL(type_str);
			objid_query->vars[objid_query->count].type = *pptr;
			objid_query->vars[objid_query->count].value = ZSTR_VAL(value_str);
		}
		objid_query->count++;
	} else if (oid_ht) { /* we got objid array */
		if (zend_hash_num_elements(oid_ht) == 0) {
			zend_value_error("Array of object IDs cannot be empty");
			return false;
		}
		objid_query->vars = (snmpobjarg *)safe_emalloc(sizeof(snmpobjarg), zend_hash_num_elements(oid_ht), 0);
		objid_query->array_output = (st & SNMP_CMD_SET) == 0;
		ZEND_HASH_FOREACH_VAL(oid_ht, tmp_oid) {
			convert_to_string(tmp_oid);
			objid_query->vars[objid_query->count].oid = Z_STRVAL_P(tmp_oid);
			if (st & SNMP_CMD_SET) {
				if (type_str) {
					pptr = ZSTR_VAL(type_str);
					objid_query->vars[objid_query->count].type = *pptr;
				} else if (type_ht) {
					if (HT_IS_PACKED(type_ht)) {
						while (idx_type < type_ht->nNumUsed) {
							tmp_type = &type_ht->arPacked[idx_type];
							if (Z_TYPE_P(tmp_type) != IS_UNDEF) {
								break;
							}
							idx_type++;
						}
					} else {
						while (idx_type < type_ht->nNumUsed) {
							tmp_type = &type_ht->arData[idx_type].val;
							if (Z_TYPE_P(tmp_type) != IS_UNDEF) {
								break;
							}
							idx_type++;
						}
					}
					if (idx_type < type_ht->nNumUsed) {
						convert_to_string(tmp_type);
						if (Z_STRLEN_P(tmp_type) != 1) {
							zend_value_error("Type must be a single character");
							efree(objid_query->vars);
							return false;
						}
						pptr = Z_STRVAL_P(tmp_type);
						objid_query->vars[objid_query->count].type = *pptr;
						idx_type++;
					} else {
						php_error_docref(NULL, E_WARNING, "'%s': no type set", Z_STRVAL_P(tmp_oid));
						efree(objid_query->vars);
						return false;
					}
				}

				if (value_str) {
					objid_query->vars[objid_query->count].value = ZSTR_VAL(value_str);
				} else if (value_ht) {
					if (HT_IS_PACKED(value_ht)) {
						while (idx_value < value_ht->nNumUsed) {
							tmp_value = &value_ht->arPacked[idx_value];
							if (Z_TYPE_P(tmp_value) != IS_UNDEF) {
								break;
							}
							idx_value++;
						}
					} else {
						while (idx_value < value_ht->nNumUsed) {
							tmp_value = &value_ht->arData[idx_value].val;
							if (Z_TYPE_P(tmp_value) != IS_UNDEF) {
								break;
							}
							idx_value++;
						}
					}
					if (idx_value < value_ht->nNumUsed) {
						convert_to_string(tmp_value);
						objid_query->vars[objid_query->count].value = Z_STRVAL_P(tmp_value);
						idx_value++;
					} else {
						php_error_docref(NULL, E_WARNING, "'%s': no value set", Z_STRVAL_P(tmp_oid));
						efree(objid_query->vars);
						return false;
					}
				}
			}
			objid_query->count++;
		} ZEND_HASH_FOREACH_END();
	}

	/* now parse all OIDs */
	if (st & SNMP_CMD_WALK) {
		if (objid_query->count > 1) {
			php_snmp_error(object, PHP_SNMP_ERRNO_OID_PARSING_ERROR, "Multi OID walks are not supported!");
			efree(objid_query->vars);
			return false;
		}
		objid_query->vars[0].name_length = MAX_NAME_LEN;
		if (strlen(objid_query->vars[0].oid)) { /* on a walk, an empty string means top of tree - no error */
			if (!snmp_parse_oid(objid_query->vars[0].oid, objid_query->vars[0].name, &(objid_query->vars[0].name_length))) {
				php_snmp_error(object, PHP_SNMP_ERRNO_OID_PARSING_ERROR, "Invalid object identifier: %s", objid_query->vars[0].oid);
				efree(objid_query->vars);
				return false;
			}
		} else {
			memmove((char *)objid_query->vars[0].name, (char *)objid_mib, sizeof(objid_mib));
			objid_query->vars[0].name_length = sizeof(objid_mib) / sizeof(oid);
		}
	} else {
		for (objid_query->offset = 0; objid_query->offset < objid_query->count; objid_query->offset++) {
			objid_query->vars[objid_query->offset].name_length = MAX_OID_LEN;
			if (!snmp_parse_oid(objid_query->vars[objid_query->offset].oid, objid_query->vars[objid_query->offset].name, &(objid_query->vars[objid_query->offset].name_length))) {
				php_snmp_error(object, PHP_SNMP_ERRNO_OID_PARSING_ERROR, "Invalid object identifier: %s", objid_query->vars[objid_query->offset].oid);
				efree(objid_query->vars);
				return false;
			}
		}
	}
	objid_query->offset = 0;
	objid_query->step = objid_query->count;
	return (objid_query->count > 0);
}
/* }}} */

/* {{{ netsnmp_session_init
	allocates memory for session and session->peername, caller should free it manually using netsnmp_session_free() and efree()
*/
static bool netsnmp_session_init(php_snmp_session **session_p, int version, zend_string *hostname, zend_string *community, int timeout, int retries)
{
	php_snmp_session *session;
	char *pptr, *host_ptr;
	bool force_ipv6 = false;
	int n;
	struct sockaddr **psal;
	struct sockaddr **res;
	// TODO: Do not strip and re-add the port in peername?
	unsigned remote_port = SNMP_PORT;

	*session_p = (php_snmp_session *)emalloc(sizeof(php_snmp_session));
	session = *session_p;
	memset(session, 0, sizeof(php_snmp_session));

	snmp_sess_init(session);

	session->version = version;

	session->peername = emalloc(MAX_NAME_LEN);
	/* we copy original hostname for further processing */
	strlcpy(session->peername, ZSTR_VAL(hostname), MAX_NAME_LEN);
	host_ptr = session->peername;

	/* Reading the hostname and its optional non-default port number */
	if (*host_ptr == '[') { /* IPv6 address */
		force_ipv6 = true;
		host_ptr++;
		if ((pptr = strchr(host_ptr, ']'))) {
			if (pptr[1] == ':') {
				remote_port = atoi(pptr + 2);
			}
			*pptr = '\0';
		} else {
			php_error_docref(NULL, E_WARNING, "Malformed IPv6 address, closing square bracket missing");
			return false;
		}
	} else { /* IPv4 address */
		if ((pptr = strchr(host_ptr, ':'))) {
			remote_port = atoi(pptr + 1);
			*pptr = '\0';
		}
	}

	/* since Net-SNMP library requires 'udp6:' prefix for all IPv6 addresses (in FQDN form too) we need to
	   perform possible name resolution before running any SNMP queries */
	if ((n = php_network_getaddresses(host_ptr, SOCK_DGRAM, &psal, NULL)) == 0) { /* some resolver error */
		/* warnings sent, bailing out */
		return false;
	}

	/* we have everything we need in psal, flush peername and fill it properly */
	*(session->peername) = '\0';
	res = psal;
	while (n-- > 0) {
		pptr = session->peername;
#if defined(HAVE_GETADDRINFO) && defined(HAVE_IPV6) && defined(HAVE_INET_NTOP)
		if (force_ipv6 && (*res)->sa_family != AF_INET6) {
			res++;
			continue;
		}
		if ((*res)->sa_family == AF_INET6) {
			strcpy(session->peername, "udp6:[");
			pptr = session->peername + strlen(session->peername);
			inet_ntop((*res)->sa_family, &(((struct sockaddr_in6*)(*res))->sin6_addr), pptr, MAX_NAME_LEN);
			strcat(pptr, "]");
		} else if ((*res)->sa_family == AF_INET) {
			inet_ntop((*res)->sa_family, &(((struct sockaddr_in*)(*res))->sin_addr), pptr, MAX_NAME_LEN);
		} else {
			res++;
			continue;
		}
#else
		if ((*res)->sa_family != AF_INET) {
			res++;
			continue;
		}
		strcat(pptr, inet_ntoa(((struct sockaddr_in*)(*res))->sin_addr));
#endif
		break;
	}

	if (strlen(session->peername) == 0) {
		php_error_docref(NULL, E_WARNING, "Unknown failure while resolving '%s'", ZSTR_VAL(hostname));
		return false;
	}
	/* XXX FIXME
		There should be check for non-empty session->peername!
	*/

	/* put back non-standard SNMP port */
	if (remote_port != SNMP_PORT) {
		pptr = session->peername + strlen(session->peername);
		sprintf(pptr, ":%d", remote_port);
	}

	php_network_freeaddresses(psal);

	if (version == SNMP_VERSION_3) {
		/* Setting the security name. */
		session->securityName = estrdup(ZSTR_VAL(community));
		session->securityNameLen = ZSTR_LEN(community);
	} else {
		session->authenticator = NULL;
		session->community = (uint8_t *)estrdup(ZSTR_VAL(community));
		session->community_len = ZSTR_LEN(community);
	}

	session->retries = retries;
	session->timeout = timeout;
	return true;
}
/* }}} */

/* {{{ Set the security level in the snmpv3 session */
static bool netsnmp_session_set_sec_level(struct snmp_session *s, zend_string *level)
{
	if (zend_string_equals_literal_ci(level, "noAuthNoPriv") || zend_string_equals_literal_ci(level, "nanp")) {
		s->securityLevel = SNMP_SEC_LEVEL_NOAUTH;
	} else if (zend_string_equals_literal_ci(level, "authNoPriv") || zend_string_equals_literal_ci(level, "anp")) {
		s->securityLevel = SNMP_SEC_LEVEL_AUTHNOPRIV;
	} else if (zend_string_equals_literal_ci(level, "authPriv") || zend_string_equals_literal_ci(level, "ap")) {
		s->securityLevel = SNMP_SEC_LEVEL_AUTHPRIV;
	} else {
		zend_value_error("Security level must be one of \"noAuthNoPriv\", \"authNoPriv\", or \"authPriv\"");
		return false;
	}
	return true;
}
/* }}} */

/* {{{ Set the authentication protocol in the snmpv3 session */
static bool netsnmp_session_set_auth_protocol(struct snmp_session *s, zend_string *prot)
{
#ifndef DISABLE_MD5
	if (zend_string_equals_literal_ci(prot, "MD5")) {
		s->securityAuthProto = usmHMACMD5AuthProtocol;
		s->securityAuthProtoLen = USM_AUTH_PROTO_MD5_LEN;
		return true;
	}
#endif

	if (zend_string_equals_literal_ci(prot, "SHA")) {
		s->securityAuthProto = usmHMACSHA1AuthProtocol;
		s->securityAuthProtoLen = USM_AUTH_PROTO_SHA_LEN;
		return true;
	}

#ifdef HAVE_SNMP_SHA256
	if (zend_string_equals_literal_ci(prot, "SHA256")) {
		s->securityAuthProto = usmHMAC192SHA256AuthProtocol;
		s->securityAuthProtoLen = sizeof(usmHMAC192SHA256AuthProtocol) / sizeof(oid);
		return true;
	}
#endif

#ifdef HAVE_SNMP_SHA512
	if (zend_string_equals_literal_ci(prot, "SHA512")) {
		s->securityAuthProto = usmHMAC384SHA512AuthProtocol;
		s->securityAuthProtoLen = sizeof(usmHMAC384SHA512AuthProtocol) / sizeof(oid);
		return true;
	}
#endif

	smart_string err = {0};

	smart_string_appends(&err, "Authentication protocol must be \"SHA\"");
#ifdef HAVE_SNMP_SHA256
	smart_string_appends(&err, " or \"SHA256\"");
#endif
#ifdef HAVE_SNMP_SHA512
	smart_string_appends(&err, " or \"SHA512\"");
#endif
#ifndef DISABLE_MD5
	smart_string_appends(&err, " or \"MD5\"");
#endif
	smart_string_0(&err);
	zend_value_error("%s", err.c);
	smart_string_free(&err);
	return false;
}
/* }}} */

/* {{{ Set the security protocol in the snmpv3 session */
static bool netsnmp_session_set_sec_protocol(struct snmp_session *s, zend_string *prot)
{
#ifndef NETSNMP_DISABLE_DES
	if (zend_string_equals_literal_ci(prot, "DES")) {
		s->securityPrivProto = usmDESPrivProtocol;
		s->securityPrivProtoLen = USM_PRIV_PROTO_DES_LEN;
		return true;
	}
#endif

#ifdef HAVE_AES
	if (zend_string_equals_literal_ci(prot, "AES128")
			|| zend_string_equals_literal_ci(prot, "AES")) {
		s->securityPrivProto = usmAESPrivProtocol;
		s->securityPrivProtoLen = USM_PRIV_PROTO_AES_LEN;
		return true;
	}
#endif

#ifdef HAVE_AES
# ifndef NETSNMP_DISABLE_DES
	zend_value_error("Security protocol must be one of \"DES\", \"AES128\", or \"AES\"");
# else
	zend_value_error("Security protocol must be one of \"AES128\", or \"AES\"");
# endif
#else
# ifndef NETSNMP_DISABLE_DES
	zend_value_error("Security protocol must be \"DES\"");
# else
	zend_value_error("No security protocol supported");
# endif
#endif
	return false;
}
/* }}} */

/* {{{ Make key from pass phrase in the snmpv3 session */
static bool netsnmp_session_gen_auth_key(struct snmp_session *s, zend_string *pass)
{
	int snmp_errno;
	s->securityAuthKeyLen = USM_AUTH_KU_LEN;
	if ((snmp_errno = generate_Ku(s->securityAuthProto, s->securityAuthProtoLen,
			(uint8_t *) ZSTR_VAL(pass), ZSTR_LEN(pass),
			s->securityAuthKey, &(s->securityAuthKeyLen)))) {
		php_error_docref(NULL, E_WARNING, "Error generating a key for authentication pass phrase '%s': %s", ZSTR_VAL(pass), snmp_api_errstring(snmp_errno));
		return false;
	}
	return true;
}
/* }}} */

/* {{{ Make key from pass phrase in the snmpv3 session */
static bool netsnmp_session_gen_sec_key(struct snmp_session *s, zend_string *pass)
{
	int snmp_errno;

	s->securityPrivKeyLen = USM_PRIV_KU_LEN;
	if ((snmp_errno = generate_Ku(s->securityAuthProto, s->securityAuthProtoLen,
			(uint8_t *)ZSTR_VAL(pass), ZSTR_LEN(pass),
			s->securityPrivKey, &(s->securityPrivKeyLen)))) {
		php_error_docref(NULL, E_WARNING, "Error generating a key for privacy pass phrase '%s': %s", ZSTR_VAL(pass), snmp_api_errstring(snmp_errno));
		return false;
	}
	return true;
}
/* }}} */

/* {{{ Set context Engine Id in the snmpv3 session */
static bool netsnmp_session_set_contextEngineID(struct snmp_session *s, zend_string * contextEngineID)
{
	size_t	ebuf_len = 32, eout_len = 0;
	uint8_t	*ebuf = (uint8_t *) emalloc(ebuf_len);

	if (!snmp_hex_to_binary(&ebuf, &ebuf_len, &eout_len, 1, ZSTR_VAL(contextEngineID))) {
		// TODO Promote to Error?
		php_error_docref(NULL, E_WARNING, "Bad engine ID value '%s'", ZSTR_VAL(contextEngineID));
		efree(ebuf);
		return false;
	}

	if (s->contextEngineID) {
		efree(s->contextEngineID);
	}

	s->contextEngineID = ebuf;
	s->contextEngineIDLen = eout_len;
	return true;
}
/* }}} */

/* {{{ Set all snmpv3-related security options */
static bool netsnmp_session_set_security(struct snmp_session *session, zend_string *sec_level,
	zend_string *auth_protocol, zend_string *auth_passphrase, zend_string *priv_protocol,
	zend_string *priv_passphrase, zend_string *contextName, zend_string *contextEngineID)
{

	/* Setting the security level. */
	if (!netsnmp_session_set_sec_level(session, sec_level)) {
		/* ValueError already generated, just bail out */
		return false;
	}

	if (session->securityLevel == SNMP_SEC_LEVEL_AUTHNOPRIV || session->securityLevel == SNMP_SEC_LEVEL_AUTHPRIV) {

		/* Setting the authentication protocol. */
		if (!netsnmp_session_set_auth_protocol(session, auth_protocol)) {
			/* ValueError already generated, just bail out */
			return false;
		}

		/* Setting the authentication passphrase. */
		if (!netsnmp_session_gen_auth_key(session, auth_passphrase)) {
			/* Warning message sent already, just bail out */
			return false;
		}

		if (session->securityLevel == SNMP_SEC_LEVEL_AUTHPRIV) {
			/* Setting the security protocol. */
			if (!netsnmp_session_set_sec_protocol(session, priv_protocol)) {
				/* ValueError already generated, just bail out */
				return false;
			}

			/* Setting the security protocol passphrase. */
			if (!netsnmp_session_gen_sec_key(session, priv_passphrase)) {
				/* Warning message sent already, just bail out */
				return false;
			}
		}
	}

	/* Setting contextName if specified */
	if (contextName) {
		session->contextName = ZSTR_VAL(contextName);
		session->contextNameLen = ZSTR_LEN(contextName);
	}

	/* Setting contextEngineIS if specified */
	if (contextEngineID && ZSTR_LEN(contextEngineID) && !netsnmp_session_set_contextEngineID(session, contextEngineID)) {
		/* Warning message sent already, just bail out */
		return false;
	}

	return true;
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
	zend_string *oid_str, *type_str = NULL, *value_str = NULL;
	HashTable *oid_ht, *type_ht = NULL, *value_ht = NULL;
	zend_string *a1 = NULL, *a2 = NULL, *a3 = NULL, *a4 = NULL, *a5 = NULL, *a6 = NULL, *a7 = NULL;
	bool use_orignames = 0, suffix_keys = 0;
	zend_long timeout = SNMP_DEFAULT_TIMEOUT;
	zend_long retries = SNMP_DEFAULT_RETRIES;
	struct objid_query objid_query;
	php_snmp_session *session;
	int session_less_mode = (getThis() == NULL);
	php_snmp_object *snmp_object;
	php_snmp_object glob_snmp_object;

	objid_query.max_repetitions = -1;
	objid_query.non_repeaters = 0;
	objid_query.valueretrieval = SNMP_G(valueretrieval);
	objid_query.oid_increasing_check = true;

	if (session_less_mode) {
		if (version == SNMP_VERSION_3) {
			if (st & SNMP_CMD_SET) {
				ZEND_PARSE_PARAMETERS_START(10, 12)
					Z_PARAM_STR(a1)
					Z_PARAM_STR(a2)
					Z_PARAM_STR(a3)
					Z_PARAM_STR(a4)
					Z_PARAM_STR(a5)
					Z_PARAM_STR(a6)
					Z_PARAM_STR(a7)
					Z_PARAM_ARRAY_HT_OR_STR(oid_ht, oid_str)
					Z_PARAM_ARRAY_HT_OR_STR(type_ht, type_str)
					Z_PARAM_ARRAY_HT_OR_STR(value_ht, value_str)
					Z_PARAM_OPTIONAL
					Z_PARAM_LONG(timeout)
					Z_PARAM_LONG(retries)
				ZEND_PARSE_PARAMETERS_END();
			} else {
				/* SNMP_CMD_GET
				 * SNMP_CMD_GETNEXT
				 * SNMP_CMD_WALK
				 */
				ZEND_PARSE_PARAMETERS_START(8, 10)
					Z_PARAM_STR(a1)
					Z_PARAM_STR(a2)
					Z_PARAM_STR(a3)
					Z_PARAM_STR(a4)
					Z_PARAM_STR(a5)
					Z_PARAM_STR(a6)
					Z_PARAM_STR(a7)
					Z_PARAM_ARRAY_HT_OR_STR(oid_ht, oid_str)
					Z_PARAM_OPTIONAL
					Z_PARAM_LONG(timeout)
					Z_PARAM_LONG(retries)
				ZEND_PARSE_PARAMETERS_END();
			}
		} else {
			if (st & SNMP_CMD_SET) {
				ZEND_PARSE_PARAMETERS_START(5, 7)
					Z_PARAM_STR(a1)
					Z_PARAM_STR(a2)
					Z_PARAM_ARRAY_HT_OR_STR(oid_ht, oid_str)
					Z_PARAM_ARRAY_HT_OR_STR(type_ht, type_str)
					Z_PARAM_ARRAY_HT_OR_STR(value_ht, value_str)
					Z_PARAM_OPTIONAL
					Z_PARAM_LONG(timeout)
					Z_PARAM_LONG(retries)
				ZEND_PARSE_PARAMETERS_END();
			} else {
				/* SNMP_CMD_GET
				 * SNMP_CMD_GETNEXT
				 * SNMP_CMD_WALK
				 */
				ZEND_PARSE_PARAMETERS_START(3, 5)
					Z_PARAM_STR(a1)
					Z_PARAM_STR(a2)
					Z_PARAM_ARRAY_HT_OR_STR(oid_ht, oid_str)
					Z_PARAM_OPTIONAL
					Z_PARAM_LONG(timeout)
					Z_PARAM_LONG(retries)
				ZEND_PARSE_PARAMETERS_END();
			}
		}
	} else {
		if (st & SNMP_CMD_SET) {
			ZEND_PARSE_PARAMETERS_START(3, 3)
				Z_PARAM_ARRAY_HT_OR_STR(oid_ht, oid_str)
				Z_PARAM_ARRAY_HT_OR_STR(type_ht, type_str)
				Z_PARAM_ARRAY_HT_OR_STR(value_ht, value_str)
			ZEND_PARSE_PARAMETERS_END();
		} else if (st & SNMP_CMD_WALK) {
			ZEND_PARSE_PARAMETERS_START(1, 4)
				Z_PARAM_ARRAY_HT_OR_STR(oid_ht, oid_str)
				Z_PARAM_OPTIONAL
				Z_PARAM_BOOL(suffix_keys)
				Z_PARAM_LONG(objid_query.max_repetitions)
				Z_PARAM_LONG(objid_query.non_repeaters)
			ZEND_PARSE_PARAMETERS_END();
			if (suffix_keys) {
				st |= SNMP_USE_SUFFIX_AS_KEYS;
			}
		} else if (st & SNMP_CMD_GET) {
			ZEND_PARSE_PARAMETERS_START(1, 2)
				Z_PARAM_ARRAY_HT_OR_STR(oid_ht, oid_str)
				Z_PARAM_OPTIONAL
				Z_PARAM_BOOL(use_orignames)
			ZEND_PARSE_PARAMETERS_END();
			if (use_orignames) {
				st |= SNMP_ORIGINAL_NAMES_AS_KEYS;
			}
		} else {
			/* SNMP_CMD_GETNEXT
			 */
			ZEND_PARSE_PARAMETERS_START(1, 1)
				Z_PARAM_ARRAY_HT_OR_STR(oid_ht, oid_str)
			ZEND_PARSE_PARAMETERS_END();
		}
	}

	if (!php_snmp_parse_oid(getThis(), st, &objid_query, oid_str, oid_ht, type_str, type_ht, value_str, value_ht)) {
		RETURN_FALSE;
	}

	if (session_less_mode) {
		if (!netsnmp_session_init(&session, version, a1, a2, timeout, retries)) {
			efree(objid_query.vars);
			netsnmp_session_free(&session);
			RETURN_FALSE;
		}
		if (version == SNMP_VERSION_3 && !netsnmp_session_set_security(session, a3, a4, a5, a6, a7, NULL, NULL)) {
			efree(objid_query.vars);
			netsnmp_session_free(&session);
			/* Warning message sent already, just bail out */
			RETURN_FALSE;
		}
	} else {
		zval *object = getThis();
		snmp_object = Z_SNMP_P(object);
		session = snmp_object->session;
		if (!session) {
			zend_throw_error(NULL, "Invalid or uninitialized SNMP object");
			efree(objid_query.vars);
			RETURN_THROWS();
		}

		if (snmp_object->max_oids > 0) {
			objid_query.step = snmp_object->max_oids;
			if (objid_query.max_repetitions < 0) { /* unspecified in function call, use session-wise */
				objid_query.max_repetitions = snmp_object->max_oids;
			}
		}
		objid_query.oid_increasing_check = snmp_object->oid_increasing_check;
		objid_query.valueretrieval = snmp_object->valueretrieval;
		glob_snmp_object.enum_print = netsnmp_ds_get_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_PRINT_NUMERIC_ENUM);
		netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_PRINT_NUMERIC_ENUM, snmp_object->enum_print);
		glob_snmp_object.quick_print = netsnmp_ds_get_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_QUICK_PRINT);
		netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_QUICK_PRINT, snmp_object->quick_print);
		glob_snmp_object.oid_output_format = netsnmp_ds_get_int(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_OID_OUTPUT_FORMAT);
		netsnmp_ds_set_int(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_OID_OUTPUT_FORMAT, snmp_object->oid_output_format);
	}

	if (objid_query.max_repetitions < 0) {
		objid_query.max_repetitions = 20; /* provide correct default value */
	}

	php_snmp_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU, st, session, &objid_query);

	efree(objid_query.vars);

	if (session_less_mode) {
		netsnmp_session_free(&session);
	} else {
		netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_PRINT_NUMERIC_ENUM, glob_snmp_object.enum_print);
		netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_QUICK_PRINT, glob_snmp_object.quick_print);
		netsnmp_ds_set_int(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_OID_OUTPUT_FORMAT, glob_snmp_object.oid_output_format);
	}
}
/* }}} */

/* {{{ Fetch a SNMP object */
PHP_FUNCTION(snmpget)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_GET, SNMP_VERSION_1);
}
/* }}} */

/* {{{ Fetch a SNMP object */
PHP_FUNCTION(snmpgetnext)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_GETNEXT, SNMP_VERSION_1);
}
/* }}} */

/* {{{ Return all objects under the specified object id */
PHP_FUNCTION(snmpwalk)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, (SNMP_CMD_WALK | SNMP_NUMERIC_KEYS), SNMP_VERSION_1);
}
/* }}} */

/* {{{ Return all objects including their respective object id within the specified one */
PHP_FUNCTION(snmprealwalk)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_WALK, SNMP_VERSION_1);
}
/* }}} */

/* {{{ Set the value of a SNMP object */
PHP_FUNCTION(snmpset)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_SET, SNMP_VERSION_1);
}
/* }}} */

/* {{{ Return the current status of quick_print */
PHP_FUNCTION(snmp_get_quick_print)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_BOOL(netsnmp_ds_get_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_QUICK_PRINT));
}
/* }}} */

/* {{{ Return all objects including their respective object id within the specified one */
PHP_FUNCTION(snmp_set_quick_print)
{
	bool a1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "b", &a1) == FAILURE) {
		RETURN_THROWS();
	}

	netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_QUICK_PRINT, (int)a1);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Return all values that are enums with their enum value instead of the raw integer */
PHP_FUNCTION(snmp_set_enum_print)
{
	bool a1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "b", &a1) == FAILURE) {
		RETURN_THROWS();
	}

	netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_PRINT_NUMERIC_ENUM, (int) a1);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Set the OID output format. */
PHP_FUNCTION(snmp_set_oid_output_format)
{
	zend_long a1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &a1) == FAILURE) {
		RETURN_THROWS();
	}

	switch (a1) {
		case NETSNMP_OID_OUTPUT_SUFFIX:
		case NETSNMP_OID_OUTPUT_MODULE:
		case NETSNMP_OID_OUTPUT_FULL:
		case NETSNMP_OID_OUTPUT_NUMERIC:
		case NETSNMP_OID_OUTPUT_UCD:
		case NETSNMP_OID_OUTPUT_NONE:
			netsnmp_ds_set_int(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_OID_OUTPUT_FORMAT, a1);
			RETURN_TRUE;
		default:
			zend_argument_value_error(1, "must be an SNMP_OID_OUTPUT_* constant");
			RETURN_THROWS();
	}
}
/* }}} */

/* {{{ Fetch a SNMP object */
PHP_FUNCTION(snmp2_get)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_GET, SNMP_VERSION_2c);
}
/* }}} */

/* {{{ Fetch a SNMP object */
PHP_FUNCTION(snmp2_getnext)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_GETNEXT, SNMP_VERSION_2c);
}
/* }}} */

/* {{{ Return all objects under the specified object id */
PHP_FUNCTION(snmp2_walk)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, (SNMP_CMD_WALK | SNMP_NUMERIC_KEYS), SNMP_VERSION_2c);
}
/* }}} */

/* {{{ Return all objects including their respective object id within the specified one */
PHP_FUNCTION(snmp2_real_walk)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_WALK, SNMP_VERSION_2c);
}
/* }}} */

/* {{{ Set the value of a SNMP object */
PHP_FUNCTION(snmp2_set)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_SET, SNMP_VERSION_2c);
}
/* }}} */

/* {{{ Fetch the value of a SNMP object */
PHP_FUNCTION(snmp3_get)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_GET, SNMP_VERSION_3);
}
/* }}} */

/* {{{ Fetch the value of a SNMP object */
PHP_FUNCTION(snmp3_getnext)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_GETNEXT, SNMP_VERSION_3);
}
/* }}} */

/* {{{ Fetch the value of a SNMP object */
PHP_FUNCTION(snmp3_walk)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, (SNMP_CMD_WALK | SNMP_NUMERIC_KEYS), SNMP_VERSION_3);
}
/* }}} */

/* {{{ Fetch the value of a SNMP object */
PHP_FUNCTION(snmp3_real_walk)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_WALK, SNMP_VERSION_3);
}
/* }}} */

/* {{{ Fetch the value of a SNMP object */
PHP_FUNCTION(snmp3_set)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_SET, SNMP_VERSION_3);
}
/* }}} */

/* {{{ Specify the method how the SNMP values will be returned */
PHP_FUNCTION(snmp_set_valueretrieval)
{
	zend_long method;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &method) == FAILURE) {
		RETURN_THROWS();
	}

	if (method >= 0 && method <= (SNMP_VALUE_LIBRARY|SNMP_VALUE_PLAIN|SNMP_VALUE_OBJECT)) {
			SNMP_G(valueretrieval) = method;
			RETURN_TRUE;
	} else {
		zend_argument_value_error(1, "must be a bitmask of SNMP_VALUE_LIBRARY, SNMP_VALUE_PLAIN, and SNMP_VALUE_OBJECT");
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ Return the method how the SNMP values will be returned */
PHP_FUNCTION(snmp_get_valueretrieval)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_LONG(SNMP_G(valueretrieval));
}
/* }}} */

/* {{{ Reads and parses a MIB file into the active MIB tree. */
PHP_FUNCTION(snmp_read_mib)
{
	char *filename;
	size_t filename_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "p", &filename, &filename_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (!read_mib(filename)) {
		char *error = strerror(errno);
		php_error_docref(NULL, E_WARNING, "Error while reading MIB file '%s': %s", filename, error);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Creates a new SNMP session to specified host. */
PHP_METHOD(SNMP, __construct)
{
	php_snmp_object *snmp_object;
	zval *object = ZEND_THIS;
	zend_string *a1, *a2;
	zend_long timeout = SNMP_DEFAULT_TIMEOUT;
	zend_long retries = SNMP_DEFAULT_RETRIES;
	zend_long version = SNMP_DEFAULT_VERSION;

	snmp_object = Z_SNMP_P(object);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "lSS|ll", &version, &a1, &a2, &timeout, &retries) == FAILURE) {
		RETURN_THROWS();
	}

	switch (version) {
		case SNMP_VERSION_1:
		case SNMP_VERSION_2c:
		case SNMP_VERSION_3:
			break;
		default:
			zend_argument_value_error(1, "must be a valid SNMP protocol version");
			RETURN_THROWS();
	}

	/* handle re-open of snmp session */
	if (snmp_object->session) {
		netsnmp_session_free(&(snmp_object->session));
	}

	if (!netsnmp_session_init(&(snmp_object->session), version, a1, a2, timeout, retries)) {
		return;
	}
	snmp_object->max_oids = 0;
	snmp_object->valueretrieval = SNMP_G(valueretrieval);
	snmp_object->enum_print = netsnmp_ds_get_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_PRINT_NUMERIC_ENUM);
	snmp_object->oid_output_format = netsnmp_ds_get_int(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_OID_OUTPUT_FORMAT);
	snmp_object->quick_print = netsnmp_ds_get_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_QUICK_PRINT);
	snmp_object->oid_increasing_check = true;
	snmp_object->exceptions_enabled = 0;
}
/* }}} */

/* {{{ Close SNMP session */
PHP_METHOD(SNMP, close)
{
	php_snmp_object *snmp_object;
	zval *object = ZEND_THIS;

	snmp_object = Z_SNMP_P(object);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	netsnmp_session_free(&(snmp_object->session));

	RETURN_TRUE;
}
/* }}} */

/* {{{ Fetch a SNMP object returning scalar for single OID and array of oid->value pairs for multi OID request */
PHP_METHOD(SNMP, get)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_GET, (-1));
}
/* }}} */

/* {{{ Fetch a SNMP object returning scalar for single OID and array of oid->value pairs for multi OID request */
PHP_METHOD(SNMP, getnext)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_GETNEXT, (-1));
}
/* }}} */

/* {{{ Return all objects including their respective object id within the specified one as array of oid->value pairs */
PHP_METHOD(SNMP, walk)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_WALK, (-1));
}
/* }}} */

/* {{{ Set the value of a SNMP object */
PHP_METHOD(SNMP, set)
{
	php_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, SNMP_CMD_SET, (-1));
}
/* }}} */

/* {{{ Set SNMPv3 security-related session parameters */
PHP_METHOD(SNMP, setSecurity)
{
	php_snmp_object *snmp_object;
	zval *object = ZEND_THIS;
	zend_string *a1 = NULL, *a2 = NULL, *a3 = NULL, *a4 = NULL, *a5 = NULL, *a6 = NULL, *a7 = NULL;

	snmp_object = Z_SNMP_P(object);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|SSSSSS", &a1, &a2, &a3, &a4,&a5, &a6, &a7) == FAILURE) {
		RETURN_THROWS();
	}

	if (!netsnmp_session_set_security(snmp_object->session, a1, a2, a3, a4, a5, a6, a7)) {
		/* Warning message sent already, just bail out */
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Get last error code number */
PHP_METHOD(SNMP, getErrno)
{
	php_snmp_object *snmp_object;
	zval *object = ZEND_THIS;

	snmp_object = Z_SNMP_P(object);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_LONG(snmp_object->snmp_errno);
}
/* }}} */

/* {{{ Get last error message */
PHP_METHOD(SNMP, getError)
{
	php_snmp_object *snmp_object;
	zval *object = ZEND_THIS;

	snmp_object = Z_SNMP_P(object);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_STRING(snmp_object->snmp_errstr);
}
/* }}} */

/* {{{ */
void php_snmp_add_property(HashTable *h, const char *name, size_t name_length, php_snmp_read_t read_func, php_snmp_write_t write_func)
{
	php_snmp_prop_handler p;
	zend_string *str;

	p.name = (char*) name;
	p.name_length = name_length;
	p.read_func = (read_func) ? read_func : NULL;
	p.write_func = (write_func) ? write_func : NULL;
	str = zend_string_init_interned(name, name_length, 1);
	zend_hash_add_mem(h, str, &p, sizeof(php_snmp_prop_handler));
	zend_string_release_ex(str, 1);
}
/* }}} */

/* {{{ php_snmp_read_property(zval *object, zval *member, int type[, const zend_literal *key])
   Generic object property reader */
zval *php_snmp_read_property(zend_object *object, zend_string *name, int type, void **cache_slot, zval *rv)
{
	zval *retval;
	php_snmp_object *obj;
	php_snmp_prop_handler *hnd;
	int ret;

	obj = php_snmp_fetch_object(object);
	hnd = zend_hash_find_ptr(&php_snmp_properties, name);

	if (hnd && hnd->read_func) {
		ret = hnd->read_func(obj, rv);
		if (ret == SUCCESS) {
			retval = rv;
		} else {
			retval = &EG(uninitialized_zval);
		}
	} else {
		retval = zend_std_read_property(object, name, type, cache_slot, rv);
	}

	return retval;
}
/* }}} */

/* {{{ Generic object property writer */
zval *php_snmp_write_property(zend_object *object, zend_string *name, zval *value, void **cache_slot)
{
	php_snmp_object *obj = php_snmp_fetch_object(object);
	php_snmp_prop_handler *hnd = zend_hash_find_ptr(&php_snmp_properties, name);

	if (hnd) {
		if (!hnd->write_func) {
			zend_throw_error(NULL, "Cannot write read-only property %s::$%s", ZSTR_VAL(object->ce->name), ZSTR_VAL(name));
			return &EG(error_zval);
		}

		zend_property_info *prop = zend_get_property_info(object->ce, name, /* silent */ true);
		if (prop && ZEND_TYPE_IS_SET(prop->type)) {
			zval tmp;
			ZVAL_COPY(&tmp, value);
			if (!zend_verify_property_type(prop, &tmp,
						ZEND_CALL_USES_STRICT_TYPES(EG(current_execute_data)))) {
				zval_ptr_dtor(&tmp);
				return &EG(error_zval);
			}
			hnd->write_func(obj, &tmp);
			zval_ptr_dtor(&tmp);
		} else {
			hnd->write_func(obj, value);
		}
		return value;
	}

	return zend_std_write_property(object, name, value, cache_slot);
}
/* }}} */

/* {{{ php_snmp_has_property(zval *object, zval *member, int has_set_exists[, const zend_literal *key])
   Generic object property checker */
static int php_snmp_has_property(zend_object *object, zend_string *name, int has_set_exists, void **cache_slot)
{
	zval rv;
	php_snmp_prop_handler *hnd;
	int ret = 0;

	if ((hnd = zend_hash_find_ptr(&php_snmp_properties, name)) != NULL) {
		switch (has_set_exists) {
			case ZEND_PROPERTY_EXISTS:
				ret = 1;
				break;
			case ZEND_PROPERTY_ISSET: {
				zval *value = php_snmp_read_property(object, name, BP_VAR_IS, cache_slot, &rv);
				if (value != &EG(uninitialized_zval)) {
					ret = Z_TYPE_P(value) != IS_NULL? 1 : 0;
					zval_ptr_dtor(value);
				}
				break;
			}
			default: {
				zval *value = php_snmp_read_property(object, name, BP_VAR_IS, cache_slot, &rv);
				if (value != &EG(uninitialized_zval)) {
					convert_to_boolean(value);
					ret = Z_TYPE_P(value) == IS_TRUE? 1:0;
				}
				break;
			}
		}
	} else {
		ret = zend_std_has_property(object, name, has_set_exists, cache_slot);
	}
	return ret;
}
/* }}} */

static HashTable *php_snmp_get_gc(zend_object *object, zval **gc_data, int *gc_data_count) /* {{{ */
{
	*gc_data = NULL;
	*gc_data_count = 0;
	return zend_std_get_properties(object);
}
/* }}} */

/* {{{ php_snmp_get_properties(zval *object)
   Returns all object properties. Injects SNMP properties into object on first call */
static HashTable *php_snmp_get_properties(zend_object *object)
{
	php_snmp_object *obj;
	php_snmp_prop_handler *hnd;
	HashTable *props;
	zval rv;
	zend_string *key;

	obj = php_snmp_fetch_object(object);
	props = zend_std_get_properties(object);

	ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&php_snmp_properties, key, hnd) {
		if (!hnd->read_func || hnd->read_func(obj, &rv) != SUCCESS) {
			ZVAL_NULL(&rv);
		}
		zend_hash_update(props, key, &rv);
	} ZEND_HASH_FOREACH_END();

	return obj->zo.properties;
}
/* }}} */

static zval *php_snmp_get_property_ptr_ptr(zend_object *object, zend_string *name, int type, void **cache_slot)
{
	php_snmp_prop_handler *hnd = zend_hash_find_ptr(&php_snmp_properties, name);
	if (hnd == NULL) {
		return zend_std_get_property_ptr_ptr(object, name, type, cache_slot);
	}

	return NULL;
}

/* {{{ */
static int php_snmp_read_info(php_snmp_object *snmp_object, zval *retval)
{
	zval val;

	array_init(retval);

	if (snmp_object->session == NULL) {
		return SUCCESS;
	}

	ZVAL_STRINGL(&val, snmp_object->session->peername, strlen(snmp_object->session->peername));
	add_assoc_zval(retval, "hostname", &val);

	ZVAL_LONG(&val, snmp_object->session->timeout);
	add_assoc_zval(retval, "timeout", &val);

	ZVAL_LONG(&val, snmp_object->session->retries);
	add_assoc_zval(retval, "retries", &val);

	return SUCCESS;
}
/* }}} */

/* {{{ */
static int php_snmp_read_max_oids(php_snmp_object *snmp_object, zval *retval)
{
	if (snmp_object->max_oids > 0) {
		ZVAL_LONG(retval, snmp_object->max_oids);
	} else {
		ZVAL_NULL(retval);
	}
	return SUCCESS;
}
/* }}} */

#define PHP_SNMP_BOOL_PROPERTY_READER_FUNCTION(name) \
	static int php_snmp_read_##name(php_snmp_object *snmp_object, zval *retval) \
	{ \
		ZVAL_BOOL(retval, snmp_object->name); \
		return SUCCESS; \
	}

PHP_SNMP_BOOL_PROPERTY_READER_FUNCTION(oid_increasing_check)
PHP_SNMP_BOOL_PROPERTY_READER_FUNCTION(quick_print)
PHP_SNMP_BOOL_PROPERTY_READER_FUNCTION(enum_print)

#define PHP_SNMP_LONG_PROPERTY_READER_FUNCTION(name) \
	static int php_snmp_read_##name(php_snmp_object *snmp_object, zval *retval) \
	{ \
		ZVAL_LONG(retval, snmp_object->name); \
		return SUCCESS; \
	}

PHP_SNMP_LONG_PROPERTY_READER_FUNCTION(valueretrieval)
PHP_SNMP_LONG_PROPERTY_READER_FUNCTION(oid_output_format)
PHP_SNMP_LONG_PROPERTY_READER_FUNCTION(exceptions_enabled)

/* {{{ */
static int php_snmp_write_max_oids(php_snmp_object *snmp_object, zval *newval)
{
	zend_long lval;

	if (Z_TYPE_P(newval) == IS_NULL) {
		snmp_object->max_oids = 0;
		return SUCCESS;
	}

	lval = zval_get_long(newval);

	if (lval <= 0) {
		zend_value_error("SNMP::$max_oids must be greater than 0 or null");
		return FAILURE;
	}
	snmp_object->max_oids = lval;

	return SUCCESS;
}
/* }}} */

/* {{{ */
static int php_snmp_write_valueretrieval(php_snmp_object *snmp_object, zval *newval)
{
	zend_long lval = zval_get_long(newval);

	if (lval >= 0 && lval <= (SNMP_VALUE_LIBRARY|SNMP_VALUE_PLAIN|SNMP_VALUE_OBJECT)) {
		snmp_object->valueretrieval = lval;
	} else {
		zend_value_error("SNMP retrieval method must be a bitmask of SNMP_VALUE_LIBRARY, SNMP_VALUE_PLAIN, and SNMP_VALUE_OBJECT");
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

#define PHP_SNMP_BOOL_PROPERTY_WRITER_FUNCTION(name) \
static int php_snmp_write_##name(php_snmp_object *snmp_object, zval *newval) \
{ \
	zval ztmp; \
	ZVAL_COPY(&ztmp, newval); \
	convert_to_boolean(&ztmp); \
	newval = &ztmp; \
\
	snmp_object->name = Z_TYPE_P(newval) == IS_TRUE? 1 : 0; \
\
	return SUCCESS; \
}

PHP_SNMP_BOOL_PROPERTY_WRITER_FUNCTION(quick_print)
PHP_SNMP_BOOL_PROPERTY_WRITER_FUNCTION(enum_print)
PHP_SNMP_BOOL_PROPERTY_WRITER_FUNCTION(oid_increasing_check)

/* {{{ */
static int php_snmp_write_oid_output_format(php_snmp_object *snmp_object, zval *newval)
{
	zend_long lval = zval_get_long(newval);

	switch(lval) {
		case NETSNMP_OID_OUTPUT_SUFFIX:
		case NETSNMP_OID_OUTPUT_MODULE:
		case NETSNMP_OID_OUTPUT_FULL:
		case NETSNMP_OID_OUTPUT_NUMERIC:
		case NETSNMP_OID_OUTPUT_UCD:
		case NETSNMP_OID_OUTPUT_NONE:
			snmp_object->oid_output_format = lval;
			return SUCCESS;
		default:
			zend_value_error("SNMP output print format must be an SNMP_OID_OUTPUT_* constant");
			return FAILURE;
	}
}
/* }}} */

/* {{{ */
static int php_snmp_write_exceptions_enabled(php_snmp_object *snmp_object, zval *newval)
{
	int ret = SUCCESS;

	snmp_object->exceptions_enabled = zval_get_long(newval);

	return ret;
}
/* }}} */

static void free_php_snmp_properties(zval *el)  /* {{{ */
{
	pefree(Z_PTR_P(el), 1);
}
/* }}} */

#define PHP_SNMP_PROPERTY_ENTRY_RECORD(name) \
	{ "" #name "",		sizeof("" #name "") - 1,	php_snmp_read_##name,	php_snmp_write_##name }

#define PHP_SNMP_READONLY_PROPERTY_ENTRY_RECORD(name) \
	{ "" #name "",		sizeof("" #name "") - 1,	php_snmp_read_##name,	NULL }

const php_snmp_prop_handler php_snmp_property_entries[] = {
	PHP_SNMP_READONLY_PROPERTY_ENTRY_RECORD(info),
	PHP_SNMP_PROPERTY_ENTRY_RECORD(max_oids),
	PHP_SNMP_PROPERTY_ENTRY_RECORD(valueretrieval),
	PHP_SNMP_PROPERTY_ENTRY_RECORD(quick_print),
	PHP_SNMP_PROPERTY_ENTRY_RECORD(enum_print),
	PHP_SNMP_PROPERTY_ENTRY_RECORD(oid_output_format),
	PHP_SNMP_PROPERTY_ENTRY_RECORD(oid_increasing_check),
	PHP_SNMP_PROPERTY_ENTRY_RECORD(exceptions_enabled),
	{ NULL, 0, NULL, NULL}
};
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(snmp)
{
	netsnmp_log_handler *logh;

	init_snmp("snmpapp");
	/* net-snmp corrupts the CTYPE locale during initialization. */
	zend_reset_lc_ctype_locale();

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

	memcpy(&php_snmp_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	php_snmp_object_handlers.read_property = php_snmp_read_property;
	php_snmp_object_handlers.write_property = php_snmp_write_property;
	php_snmp_object_handlers.get_property_ptr_ptr = php_snmp_get_property_ptr_ptr;
	php_snmp_object_handlers.has_property = php_snmp_has_property;
	php_snmp_object_handlers.get_properties = php_snmp_get_properties;
	php_snmp_object_handlers.get_gc = php_snmp_get_gc;

	/* Register SNMP Class */
	php_snmp_ce = register_class_SNMP();
	php_snmp_ce->create_object = php_snmp_object_new;
	php_snmp_object_handlers.offset = XtOffsetOf(php_snmp_object, zo);
	php_snmp_object_handlers.clone_obj = NULL;
	php_snmp_object_handlers.free_obj = php_snmp_object_free_storage;

	/* Register SNMP Class properties */
	zend_hash_init(&php_snmp_properties, 0, NULL, free_php_snmp_properties, 1);
	PHP_SNMP_ADD_PROPERTIES(&php_snmp_properties, php_snmp_property_entries);

	/* Register SNMPException class */
	php_snmp_exception_ce = register_class_SNMPException(spl_ce_RuntimeException);

	register_snmp_symbols(module_number);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(snmp)
{
	snmp_shutdown("snmpapp");

	zend_hash_destroy(&php_snmp_properties);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(snmp)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "NET-SNMP Support", "enabled");
	php_info_print_table_row(2, "NET-SNMP Version", netsnmp_get_version());
	php_info_print_table_end();
}
/* }}} */

/* {{{ snmp_module_deps[] */
static const zend_module_dep snmp_module_deps[] = {
	ZEND_MOD_REQUIRED("spl")
	ZEND_MOD_END
};
/* }}} */

/* {{{ snmp_module_entry */
zend_module_entry snmp_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	NULL,
	snmp_module_deps,
	"snmp",
	ext_functions,
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
