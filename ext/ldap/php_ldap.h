/* 
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
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
   | Authors: Amitay Isaacs <amitay@w-o-i.com>                            |
   |          Eric Warnke   <ericw@albany.edu>                            |
   |          Jani Taskinen <sniper@iki.fi>                               |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_LDAP_H
#define PHP_LDAP_H

#ifndef HAVE_ORALDAP
#include <lber.h>
#endif

#include <ldap.h>

extern zend_module_entry ldap_module_entry;
#define ldap_module_ptr &ldap_module_entry

/* LDAP functions */
PHP_MINIT_FUNCTION(ldap);
PHP_MSHUTDOWN_FUNCTION(ldap);
PHP_MINFO_FUNCTION(ldap);

ZEND_BEGIN_MODULE_GLOBALS(ldap)
	long num_links;
	long max_links;
ZEND_END_MODULE_GLOBALS(ldap)

#ifdef ZTS
# define LDAPG(v) TSRMG(ldap_globals_id, zend_ldap_globals *, v)
#else
# define LDAPG(v) (ldap_globals.v)
#endif

#define phpext_ldap_ptr ldap_module_ptr

/* Constants for ldap_modify_batch */
#define LDAP_MODIFY_BATCH_ADD        0x01
#define LDAP_MODIFY_BATCH_REMOVE     0x02
#define LDAP_MODIFY_BATCH_REMOVE_ALL 0x12
#define LDAP_MODIFY_BATCH_REPLACE    0x03

#define LDAP_MODIFY_BATCH_ATTRIB     "attrib"
#define LDAP_MODIFY_BATCH_MODTYPE    "modtype"
#define LDAP_MODIFY_BATCH_VALUES     "values"

#endif /* PHP_LDAP_H */
