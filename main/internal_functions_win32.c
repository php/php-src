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

#include "ext/bcmath/php3_bcmath.h"
#include "ext/db/php3_db.h"
#include "ext/gd/php3_gd.h"
#include "ext/standard/dl.h"
#include "ext/standard/file.h"
#include "ext/standard/fsock.h"
#include "ext/standard/head.h"
#include "ext/standard/pack.h"
#include "ext/standard/php3_browscap.h"
#include "ext/standard/php3_crypt.h"
#include "ext/standard/php3_dir.h"
#include "ext/standard/php3_filestat.h"
#include "ext/standard/php3_mail.h"
#include "ext/standard/php3_syslog.h"
#include "ext/standard/php3_standard.h"
#include "ext/COM/php3_COM.h"
#include "ext/standard/reg.h"
#include "ext/pcre/php_pcre.h"

/* SNMP has to be moved to ext */
/* #include "dl/snmp/php3_snmp.h" */

unsigned char first_arg_force_ref[] = { 1, BYREF_FORCE };
unsigned char first_arg_allow_ref[] = { 1, BYREF_ALLOW };
unsigned char second_arg_force_ref[] = { 2, BYREF_NONE, BYREF_FORCE };
unsigned char second_arg_allow_ref[] = { 2, BYREF_NONE, BYREF_ALLOW };

zend_module_entry *php3_builtin_modules[] = {
    phpext_dl_ptr,
    phpext_file_ptr,
    phpext_fsock_ptr,
    phpext_head_ptr,
    phpext_pack_ptr,
    phpext_browscap_ptr,
    phpext_crypt_ptr,
    phpext_dir_ptr,
    phpext_filestat_ptr,
    phpext_mail_ptr,
    phpext_syslog_ptr,
	phpext_bcmath_ptr,
	phpext_standard_ptr,
	COM_module_ptr,
	phpext_regex_ptr,
	phpext_pcre_ptr
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
