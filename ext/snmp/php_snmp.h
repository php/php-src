/*
+----------------------------------------------------------------------+
| PHP version 4.0                                                      |
+----------------------------------------------------------------------+
| Copyright (c) 1997, 1998, 1999 The PHP Group                         |
+----------------------------------------------------------------------+
| This source file is subject to version 2.0 of the PHP license,       |
| that is bundled with this package in the file LICENSE, and is        |
| available at through the world-wide-web at                           |
| http://www.php.net/license/2_0.txt.                                  |
| If you did not receive a copy of the PHP license and are unable to   |
| obtain it through the world-wide-web, please send a note to          |
| license@php.net so we can mail you a copy immediately.               |
+----------------------------------------------------------------------+
| Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
|          Mike Jackson <mhjack@tscnet.com>                            |
|          Steven Lawrance <slawrance@technologist.com>                |
+----------------------------------------------------------------------+
*/

/* $Id$ */
#ifndef _PHP_SNMP_H
#define _PHP_SNMP_H

#if COMPILE_DL
#undef HAVE_SNMP
#define HAVE_SNMP 1
#endif
#if HAVE_SNMP
#ifndef DLEXPORT
#define DLEXPORT
#endif

extern php3_module_entry snmp_module_entry;
#define snmp_module_ptr &snmp_module_entry

extern int php3i_snmp_init(INIT_FUNC_ARGS);
PHP_FUNCTION(snmpget);
PHP_FUNCTION(snmpwalk);
PHP_FUNCTION(snmprealwalk);
PHP_FUNCTION(snmpwalkoid);
PHP_FUNCTION(snmp_get_quick_print);
PHP_FUNCTION(snmp_set_quick_print);
PHP_FUNCTION(snmpset);
void php3_info_snmp(ZEND_MODULE_INFO_FUNC_ARGS);
#else

#define snmp_module_ptr NULL

#endif /* HAVE_SNMP */

#define phpext_snmp_ptr snmp_module_ptr

#endif  /* _PHP_SNMP_H */
