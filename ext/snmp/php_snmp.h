/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2009 The PHP Group                                |
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

#ifndef PHP_SNMP_H
#define PHP_SNMP_H

#if HAVE_SNMP

#ifndef DLEXPORT
#define DLEXPORT
#endif

extern zend_module_entry snmp_module_entry;
#define snmp_module_ptr &snmp_module_entry

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(snmp);
PHP_MSHUTDOWN_FUNCTION(snmp);
PHP_MINFO_FUNCTION(snmp);

PHP_FUNCTION(snmpget);
PHP_FUNCTION(snmpgetnext);
PHP_FUNCTION(snmpwalk);
PHP_FUNCTION(snmprealwalk);
PHP_FUNCTION(snmp_get_quick_print);
PHP_FUNCTION(snmp_set_quick_print);
PHP_FUNCTION(snmp_set_enum_print);
PHP_FUNCTION(snmp_set_oid_output_format);
PHP_FUNCTION(snmpset);

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


ZEND_BEGIN_MODULE_GLOBALS(snmp)
      int valueretrieval;
ZEND_END_MODULE_GLOBALS(snmp)

#ifdef ZTS
#define SNMP_G(v) TSRMG(snmp_globals_id, zend_snmp_globals *, v)
#else
#define SNMP_G(v) (snmp_globals.v)
#endif

#else

#define snmp_module_ptr NULL

#endif /* HAVE_SNMP */

#define phpext_snmp_ptr snmp_module_ptr

#endif  /* PHP_SNMP_H */
