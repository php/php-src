/* 
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
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

PHP_FUNCTION(ldap_mod_add);
PHP_FUNCTION(ldap_mod_replace);
PHP_FUNCTION(ldap_mod_del);

PHP_FUNCTION(ldap_errno);
PHP_FUNCTION(ldap_err2str);
PHP_FUNCTION(ldap_error);

PHP_FUNCTION(ldap_compare);

PHP_FUNCTION(ldap_sort);

#if ( LDAP_API_VERSION > 2000 ) || HAVE_NSLDAP
PHP_FUNCTION(ldap_get_option);
PHP_FUNCTION(ldap_set_option);
PHP_FUNCTION(ldap_parse_result);
PHP_FUNCTION(ldap_first_reference);
PHP_FUNCTION(ldap_next_reference);
PHP_FUNCTION(ldap_parse_reference);
PHP_FUNCTION(ldap_rename);
#endif

#if LDAP_API_VERSION > 2000
PHP_FUNCTION(ldap_start_tls);
#endif

#if defined(LDAP_API_FEATURE_X_OPENLDAP) && defined(HAVE_3ARG_SETREBINDPROC)
PHP_FUNCTION(ldap_set_rebind_proc);
#endif

#ifdef STR_TRANSLATION
PHP_FUNCTION(ldap_t61_to_8859);
PHP_FUNCTION(ldap_8859_to_t61);
#endif

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

#endif /* PHP_LDAP_H */
