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

#ifndef PHP_SNMP_H
#define PHP_SNMP_H

#define PHP_SNMP_VERSION PHP_VERSION

#ifdef HAVE_SNMP

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

#define SNMP_VALUE_LIBRARY	(0 << 0)
#define SNMP_VALUE_PLAIN	(1 << 0)
#define SNMP_VALUE_OBJECT	(1 << 1)

#define PHP_SNMP_ERRNO_NOERROR			0
#define PHP_SNMP_ERRNO_GENERIC			(1 << 1)
#define PHP_SNMP_ERRNO_TIMEOUT			(1 << 2)
#define PHP_SNMP_ERRNO_ERROR_IN_REPLY		(1 << 3)
#define PHP_SNMP_ERRNO_OID_NOT_INCREASING	(1 << 4)
#define PHP_SNMP_ERRNO_OID_PARSING_ERROR	(1 << 5)
#define PHP_SNMP_ERRNO_MULTIPLE_SET_QUERIES	(1 << 6)
#define PHP_SNMP_ERRNO_ANY	( \
		PHP_SNMP_ERRNO_GENERIC | \
		PHP_SNMP_ERRNO_TIMEOUT | \
		PHP_SNMP_ERRNO_ERROR_IN_REPLY | \
		PHP_SNMP_ERRNO_OID_NOT_INCREASING | \
		PHP_SNMP_ERRNO_OID_PARSING_ERROR | \
		PHP_SNMP_ERRNO_MULTIPLE_SET_QUERIES | \
		PHP_SNMP_ERRNO_NOERROR \
	)

#else

#define snmp_module_ptr NULL

#endif /* HAVE_SNMP */

#define phpext_snmp_ptr snmp_module_ptr

#endif  /* PHP_SNMP_H */
