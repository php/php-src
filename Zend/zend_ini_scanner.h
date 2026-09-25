/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright © Zend Technologies Ltd., a subsidiary company of          |
   |     Perforce Software, Inc., and Contributors.                       |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#ifndef _ZEND_INI_SCANNER_H
#define _ZEND_INI_SCANNER_H

#include "zend_types.h"

typedef struct _zend_file_handle zend_file_handle;

/* Scanner modes */
C23_ENUM(zend_ini_scanner_mode, uint8_t) {
   ZEND_INI_SCANNER_NORMAL = 0, /* Normal mode. [DEFAULT] */
   ZEND_INI_SCANNER_RAW    = 1, /* Raw mode. Option values are not parsed */
   ZEND_INI_SCANNER_TYPED  = 2, /* Typed mode. */
};

BEGIN_EXTERN_C()
ZEND_COLD uint32_t zend_ini_scanner_get_lineno(void);
ZEND_COLD const char *zend_ini_scanner_get_filename(void);
zend_result zend_ini_open_file_for_scanning(zend_file_handle *fh, zend_ini_scanner_mode scanner_mode);
void zend_ini_prepare_string_for_scanning(const char *str, zend_ini_scanner_mode scanner_mode);
int ini_lex(zval *ini_lval);
void shutdown_ini_scanner(void);
END_EXTERN_C()

#endif /* _ZEND_INI_SCANNER_H */
