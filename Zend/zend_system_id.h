/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sammy Kaye Powers <sammyk@php.net>                           |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_SYSTEM_ID_H
#define ZEND_SYSTEM_ID_H

BEGIN_EXTERN_C()
/* True global; Write-only during MINIT/startup */
extern ZEND_API char zend_system_id[32];

ZEND_API ZEND_RESULT_CODE zend_add_system_entropy(const char *module_name, const char *hook_name, const void *data, size_t size);
END_EXTERN_C()

void zend_startup_system_id(void);
void zend_finalize_system_id(void);

#endif /* ZEND_SYSTEM_ID_H */
