/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Author: Sammy Kaye Powers <sammyk@php.net>                           |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_SYSTEM_ID_H
#define ZEND_SYSTEM_ID_H

#include "zend_types.h"

BEGIN_EXTERN_C()
/* True global; Write-only during MINIT/startup */
extern ZEND_API char zend_system_id[32];

ZEND_API zend_result zend_add_system_entropy(const char *module_name, const char *hook_name, const void *data, size_t size);
END_EXTERN_C()

void zend_startup_system_id(void);
void zend_finalize_system_id(void);

#endif /* ZEND_SYSTEM_ID_H */
