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
#ifndef PHP_SNMP_H
#define PHP_SNMP_H

#if HAVE_SNMP
#ifndef DLEXPORT
#define DLEXPORT
#endif

extern zend_module_entry snmp_module_entry;
#define snmp_module_ptr &snmp_module_entry

PHP_MINIT_FUNCTION(snmp);
PHP_FUNCTION(snmpget);
PHP_FUNCTION(snmpwalk);
PHP_FUNCTION(snmprealwalk);
PHP_FUNCTION(snmp_get_quick_print);
PHP_FUNCTION(snmp_set_quick_print);
PHP_FUNCTION(snmpset);
PHP_FUNCTION(snmpv3get);
PHP_FUNCTION(snmpv3getnext);
PHP_FUNCTION(snmpv3walk);
PHP_FUNCTION(snmpv3realwalk);
PHP_FUNCTION(snmpv3set);
PHP_FUNCTION(snmpv3getbulk);
PHP_MINFO_FUNCTION(snmp);
#else

#define snmp_module_ptr NULL

#endif /* HAVE_SNMP */

#define phpext_snmp_ptr snmp_module_ptr

#endif  /* PHP_SNMP_H */
