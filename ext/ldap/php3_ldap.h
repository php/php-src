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

#ifndef _PHP3_LDAP_H
#define _PHP3_LDAP_H

#if COMPILE_DL
#undef HAVE_LDAP
#define HAVE_LDAP 1
#endif

#if HAVE_LDAP
#include <lber.h>
#include <ldap.h>

extern php3_module_entry ldap_module_entry;
#define ldap_module_ptr &ldap_module_entry

/* LDAP functions */
int php3_minit_ldap(INIT_FUNC_ARGS);
int php3_mshutdown_ldap(SHUTDOWN_FUNC_ARGS);

void php3_info_ldap(ZEND_MODULE_INFO_FUNC_ARGS);

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

PHP_FUNCTION(ber_free);
PHP_FUNCTION(ldap_get_dn);
PHP_FUNCTION(ldap_explode_dn);
PHP_FUNCTION(ldap_dn2ufn);

PHP_FUNCTION(ldap_add);
PHP_FUNCTION(ldap_delete);
PHP_FUNCTION(ldap_modify);

typedef struct {
	long default_link;
	long num_links, max_links;
	char *base_dn;
	int le_result, le_result_entry, le_ber_entry;
	int le_link;

	/* I just found out that the thread safe features
		of the netscape ldap library are only required if
		multiple threads are accessing the same LDAP
		structure.  Since we are not doing that, we do
		not need to use this feature.  I am leaving the
		code here anyway just in case.  smc
		*/
#if 0
	struct ldap_thread_fns tfns;
	int le_errno; /* Corresponds to the LDAP error code */
	char *le_matched; /* Matching components of the DN, 
                  if an NO_SUCH_OBJECT error occurred */
	char *le_errmsg; /* Error message */
#endif
} ldap_module;

#ifndef THREAD_SAFE
extern ldap_module php3_ldap_module;
#endif

#else

#define ldap_module_ptr NULL

#endif

#define phpext_ldap_ptr ldap_module_ptr

#endif /* _PHP3_LDAP_H */
