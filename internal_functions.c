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
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */


/* $Id$ */


#include "php.h"
#include "modules.h"
#include "internal_functions_registry.h"
#include "zend_compile.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "ext/standard/php3_standard.h"
#include "ext/gd/php3_gd.h"
#include "ext/dbase/dbase.h"
#include "ext/odbc/php3_odbc.h"
#include "ext/gettext/php3_gettext.h"

#include "functions/php3_ifx.h"
#include "functions/php3_crypt.h"
#include "functions/php3_ldap.h"
#include "functions/php3_mysql.h"
#include "functions/php3_bcmath.h"
#include "functions/php3_msql.h"
#include "functions/php3_oci8.h"
#include "functions/oracle.h"
#include "functions/php3_pgsql.h"
#include "functions/php3_sybase.h"
#include "functions/php3_sybase-ct.h"
#include "functions/imap.h"
#include "functions/dl.h"
#include "functions/head.h"
#include "functions/post.h"
#include "functions/hw.h"
#include "functions/filepro.h"
#include "functions/db.h"
#include "dl/snmp/php3_snmp.h"
#include "functions/php3_zlib.h"
#include "functions/php3_COM.h"
#include "functions/php3_interbase.h"
#include "functions/php3_xml.h"
#include "functions/php3_pdf.h"
#include "functions/php3_fdf.h"
#include "functions/php3_sysvsem.h"
#include "functions/php3_sysvshm.h"
#include "functions/php3_dav.h"

unsigned char first_arg_force_ref[] = { 1, BYREF_FORCE };
unsigned char first_arg_allow_ref[] = { 1, BYREF_ALLOW };
unsigned char second_arg_force_ref[] = { 2, BYREF_NONE, BYREF_FORCE };
unsigned char second_arg_allow_ref[] = { 2, BYREF_NONE, BYREF_ALLOW };

zend_module_entry *php3_builtin_modules[] = 
{
	basic_functions_module_ptr,
	dl_module_ptr,
	php3_dir_module_ptr,
	php3_filestat_module_ptr,
	php3_file_module_ptr,
	php3_header_module_ptr,
	mail_module_ptr,
	syslog_module_ptr,
	mysql_module_ptr,
	msql_module_ptr,
	pgsql_module_ptr,
	ifx_module_ptr,
	ldap_module_ptr,
	filepro_module_ptr,
	sybase_module_ptr,
	sybct_module_ptr,
	odbc_module_ptr,
	dbase_module_ptr,
	hw_module_ptr,
	regexp_module_ptr,
	gd_module_ptr,
	oci8_module_ptr,
	oracle_module_ptr,
	apache_module_ptr,
	crypt_module_ptr,
	dbm_module_ptr,
	bcmath_module_ptr,
	snmp_module_ptr,
	pack_module_ptr,
	php3_zlib_module_ptr,
	COM_module_ptr,
	php3_imap_module_ptr,
	php3_ibase_module_ptr,
	xml_module_ptr,
	pdf_module_ptr,
	fdf_module_ptr,
	sysvsem_module_ptr,
	sysvshm_module_ptr,
	phpdav_module_ptr,
	php3_gettext_module_ptr,
};

	
int module_startup_modules(void)
{
	zend_module_entry **ptr = php3_builtin_modules, **end = ptr+(sizeof(php3_builtin_modules)/sizeof(zend_module_entry *));

	while (ptr < end) {
		if (*ptr) {
			if (zend_startup_module(*ptr)==FAILURE) {
				return FAILURE;
			}
		}
		ptr++;
	}
	return SUCCESS;
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
