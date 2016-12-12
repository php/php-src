/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
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

#ifndef PHP_SNMP_H
#define PHP_SNMP_H

#define PHP_SNMP_VERSION "0.1"

#if HAVE_SNMP

#ifndef DLEXPORT
#define DLEXPORT
#endif

extern zend_module_entry snmp_module_entry;
#define snmp_module_ptr &snmp_module_entry

#ifdef ZTS
#include "TSRM.h"
#endif

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

PHP_MINIT_FUNCTION(snmp);
PHP_MSHUTDOWN_FUNCTION(snmp);
PHP_MINFO_FUNCTION(snmp);

PHP_FUNCTION(snmpget);
PHP_FUNCTION(snmpgetnext);
PHP_FUNCTION(snmpwalk);
PHP_FUNCTION(snmprealwalk);
PHP_FUNCTION(snmpset);
PHP_FUNCTION(snmp_get_quick_print);
PHP_FUNCTION(snmp_set_quick_print);
PHP_FUNCTION(snmp_set_enum_print);
PHP_FUNCTION(snmp_set_oid_output_format);

PHP_FUNCTION(snmp2_get);
PHP_FUNCTION(snmp2_getnext);
PHP_FUNCTION(snmp2_walk);
PHP_FUNCTION(snmp2_real_walk);
PHP_FUNCTION(snmp2_set);

PHP_FUNCTION(snmp3_get);
PHP_FUNCTION(snmp3_getnext);
PHP_FUNCTION(snmp3_walk);
PHP_FUNCTION(snmp3_real_walk);
PHP_FUNCTION(snmp3_set);

PHP_FUNCTION(snmp_set_valueretrieval);
PHP_FUNCTION(snmp_get_valueretrieval);

PHP_FUNCTION(snmp_read_mib);

PHP_METHOD(SNMP, setSecurity);
PHP_METHOD(SNMP, close);
PHP_METHOD(SNMP, get);
PHP_METHOD(SNMP, getnext);
PHP_METHOD(SNMP, walk);
PHP_METHOD(SNMP, set);
PHP_METHOD(SNMP, getErrno);
PHP_METHOD(SNMP, getError);

typedef struct _php_snmp_object {
	struct snmp_session *session;
	int max_oids;
	int valueretrieval;
	int quick_print;
	int enum_print;
	int oid_output_format;
	int snmp_errno;
	int oid_increasing_check;
	int exceptions_enabled;
	char snmp_errstr[256];
	zend_object zo;
} php_snmp_object;

static inline php_snmp_object *php_snmp_fetch_object(zend_object *obj) {
	return (php_snmp_object *)((char*)(obj) - XtOffsetOf(php_snmp_object, zo));
}

#define Z_SNMP_P(zv) php_snmp_fetch_object(Z_OBJ_P((zv)))

typedef int (*php_snmp_read_t)(php_snmp_object *snmp_object, zval *retval);
typedef int (*php_snmp_write_t)(php_snmp_object *snmp_object, zval *newval);

typedef struct _ptp_snmp_prop_handler {
	const char *name;
	size_t name_length;
	php_snmp_read_t read_func;
	php_snmp_write_t write_func;
} php_snmp_prop_handler;

typedef struct _snmpobjarg {
	char *oid;
	char type;
	char *value;
	oid  name[MAX_OID_LEN];
	size_t name_length;
} snmpobjarg;

ZEND_BEGIN_MODULE_GLOBALS(snmp)
	int valueretrieval;
ZEND_END_MODULE_GLOBALS(snmp)

#ifdef ZTS
#define SNMP_G(v) TSRMG(snmp_globals_id, zend_snmp_globals *, v)
#else
#define SNMP_G(v) (snmp_globals.v)
#endif

#define REGISTER_SNMP_CLASS_CONST_LONG(const_name, value) \
	zend_declare_class_constant_long(php_snmp_ce, const_name, sizeof(const_name)-1, (zend_long)value);

#else

#define snmp_module_ptr NULL

#endif /* HAVE_SNMP */

#define phpext_snmp_ptr snmp_module_ptr

#endif  /* PHP_SNMP_H */
