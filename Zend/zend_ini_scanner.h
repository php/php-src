/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#ifndef _ZEND_INI_SCANNER_H
#define _ZEND_INI_SCANNER_H

/* Scanner modes */
#define ZEND_INI_SCANNER_NORMAL 0 /* Normal mode. [DEFAULT] */
#define ZEND_INI_SCANNER_RAW    1 /* Raw mode. Option values are not parsed */
#define ZEND_INI_SCANNER_TYPED  2 /* Typed mode. */

BEGIN_EXTERN_C()
ZEND_COLD int zend_ini_scanner_get_lineno(void);
ZEND_COLD char *zend_ini_scanner_get_filename(void);
zend_result zend_ini_open_file_for_scanning(zend_file_handle *fh, int scanner_mode);
zend_result zend_ini_prepare_string_for_scanning(char *str, int scanner_mode);
int ini_lex(zval *ini_lval);
void shutdown_ini_scanner(void);
END_EXTERN_C()

#endif /* _ZEND_INI_SCANNER_H */
