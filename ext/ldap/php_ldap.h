/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Amitay Isaacs <amitay@w-o-i.com>                            |
   |          Eric Warnke   <ericw@albany.edu>                            |
   +----------------------------------------------------------------------+
 */


/* $Id$ */

#ifndef PHP_LDAP_H
#define PHP_LDAP_H

#include <lber.h>
#include <ldap.h>

extern zend_module_entry ldap_module_entry;
#define ldap_module_ptr &ldap_module_entry

/* LDAP functions */
PHP_MINIT_FUNCTION(ldap);
PHP_MSHUTDOWN_FUNCTION(ldap);

PHP_MINFO_FUNCTION(ldap);

PHP_FUNCTION(ldap_connect);

PHP_FUNCTION(ldap_bind);
PHP_FUNCTION(ldap_unbind);

PHP_FUNCTION(ldap_read);
PHP_FUNCTION(ldap_list);
PHP_FUNCTION(ldap_search);

PHP_FUNCTION(ldap_free_result);
PHP_FUNCTION(ldap_count_entries);

PHP_FUNCTION(ldap_first_entry);
PHP_FUNCTION(ldap_next_entry);
PHP_FUNCTION(ldap_get_entries);
#if 0
PHP_FUNCTION(ldap_free_entry);
#endif
PHP_FUNCTION(ldap_first_attribute);
PHP_FUNCTION(ldap_next_attribute);
PHP_FUNCTION(ldap_get_attributes);

PHP_FUNCTION(ldap_get_values);
PHP_FUNCTION(ldap_get_values_len);

PHP_FUNCTION(ber_free);
PHP_FUNCTION(ldap_get_dn);
PHP_FUNCTION(ldap_explode_dn);
PHP_FUNCTION(ldap_dn2ufn);

PHP_FUNCTION(ldap_add);
PHP_FUNCTION(ldap_delete);
PHP_FUNCTION(ldap_modify);

PHP_FUNCTION(ldap_mod_add);
PHP_FUNCTION(ldap_mod_replace);
PHP_FUNCTION(ldap_mod_del);

PHP_FUNCTION(ldap_errno);
PHP_FUNCTION(ldap_err2str);
PHP_FUNCTION(ldap_error);

ZEND_BEGIN_MODULE_GLOBALS(ldap)
	long default_link;
	long num_links, max_links;
	char *base_dn;
ZEND_END_MODULE_GLOBALS(ldap)

#ifdef ZTS
# define LDAPLS_D	zend_ldap_globals *ldap_globals
# define LDAPLS_DC	, LDAPLS_D
# define LDAPLS_C	ldap_globals
# define LDAPLS_CC , LDAPLS_C
# define LDAPG(v) (ldap_globals->v)
# define LDAPLS_FETCH()	zend_ldap_globals *ldap_globals = ts_resource(ldap_globals_id)
#else
# define LDAPLS_D
# define LDAPLS_DC
# define LDAPLS_C
# define LDAPLS_CC
# define LDAPG(v) (ldap_globals.v)
# define LDAPLS_FETCH()
#endif


#define phpext_ldap_ptr ldap_module_ptr

#endif /* PHP_LDAP_H */
