/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef _ZEND_INI_SCANNER_H
#define _ZEND_INI_SCANNER_H

/* Scanner modes */
#define ZEND_INI_SCANNER_NORMAL 0 /* Normal mode. [DEFAULT] */
#define ZEND_INI_SCANNER_RAW    1 /* Raw mode. Option values are not parsed */
#define ZEND_INI_SCANNER_TYPED  2 /* Typed mode. */

BEGIN_EXTERN_C()
int zend_ini_scanner_get_lineno(TSRMLS_D);
char *zend_ini_scanner_get_filename(TSRMLS_D);
int zend_ini_open_file_for_scanning(zend_file_handle *fh, int scanner_mode TSRMLS_DC);
int zend_ini_prepare_string_for_scanning(char *str, int scanner_mode TSRMLS_DC);
int ini_lex(zval *ini_lval TSRMLS_DC);
void shutdown_ini_scanner(TSRMLS_D);
END_EXTERN_C()

#endif /* _ZEND_INI_SCANNER_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
