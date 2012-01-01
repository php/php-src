/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   | Modified for NetWare: Novell, Inc.                                   |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* {{{ includes
 */
#include "php.h"
#include "php_main.h"
#include "zend_modules.h"
#include "zend_compile.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "ext/bcmath/php_bcmath.h"
#include "ext/gd/php_gd.h"
#include "ext/standard/dl.h"
#include "ext/standard/file.h"
#include "ext/standard/fsock.h"
#include "ext/standard/head.h"
#include "ext/standard/pack.h"
#include "ext/standard/php_browscap.h"
/*#include "ext/standard/php_crypt.h"*/
#include "ext/standard/php_dir.h"
#include "ext/standard/php_filestat.h"
#include "ext/standard/php_mail.h"
/*#include "ext/standard/php_ext_syslog.h"*/
#include "ext/standard/php_standard.h"
#include "ext/standard/php_lcg.h"
#include "ext/standard/php_array.h"
#include "ext/standard/php_assert.h"
#include "ext/calendar/php_calendar.h"
/*#include "ext/com/php_COM.h"
#include "ext/com/php_VARIANT.h"*/
#include "ext/ftp/php_ftp.h"
#include "ext/standard/reg.h"
#include "ext/pcre/php_pcre.h"
/*#include "ext/odbc/php_odbc.h"*/	/* Commented out for now */
#include "ext/session/php_session.h"
/*#include "ext/xml/php_xml.h"
#include "ext/wddx/php_wddx.h"
#include "ext/mysql/php_mysql.h"*/	/* Commented out for now */
/* }}} */

/* {{{ php_builtin_extensions[]
 */
static zend_module_entry *php_builtin_extensions[] = {
	phpext_standard_ptr,
#if HAVE_BCMATH
	phpext_bcmath_ptr,
#endif
	phpext_calendar_ptr,
/*	COM_module_ptr,*/
	phpext_ftp_ptr,
#if defined(MBSTR_ENC_TRANS)
	phpext_mbstring_ptr,
#endif
/*	phpext_mysql_ptr,*/	/* Commented out for now */
/*	phpext_odbc_ptr, */	/* Commented out for now */
	phpext_pcre_ptr,
	phpext_session_ptr,
/*	phpext_xml_ptr,
	phpext_wddx_ptr */	/* Commented out for now */
};
/* }}} */

#define EXTCOUNT (sizeof(php_builtin_extensions)/sizeof(zend_module_entry *))

PHPAPI int php_register_internal_extensions(TSRMLS_D)
{
	return php_register_extensions(php_builtin_extensions, EXTCOUNT TSRMLS_CC);
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
