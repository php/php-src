/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Amitay Isaacs <amitay@w-o-i.com>                            |
   |          Eric Warnke   <ericw@albany.edu>                            |
   |          Jani Taskinen <sniper@iki.fi>                               |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_LDAP_H
#define PHP_LDAP_H

#ifndef HAVE_ORALDAP
#include <lber.h>
#endif

#include <ldap.h>

extern zend_module_entry ldap_module_entry;
#define ldap_module_ptr &ldap_module_entry

#include "php_version.h"
#define PHP_LDAP_VERSION PHP_VERSION

/* LDAP functions */
PHP_MINIT_FUNCTION(ldap);
PHP_MSHUTDOWN_FUNCTION(ldap);
PHP_MINFO_FUNCTION(ldap);

ZEND_BEGIN_MODULE_GLOBALS(ldap)
	zend_long num_links;
	zend_long max_links;
	bool tls_newctx; /* create new TLS context before connect */
ZEND_END_MODULE_GLOBALS(ldap)

#if defined(ZTS) && defined(COMPILE_DL_LDAP)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

ZEND_EXTERN_MODULE_GLOBALS(ldap)
#define LDAPG(v) ZEND_MODULE_GLOBALS_ACCESSOR(ldap, v)

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
