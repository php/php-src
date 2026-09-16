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
 | Authors: Sara Golemon <pollita@php.net>                              |
 +----------------------------------------------------------------------+
*/

#ifndef PHP_INTL_CONVERTER_H
#define PHP_INTL_CONVERTER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"

#ifdef __cplusplus
extern "C" {
#endif
int php_converter_minit(INIT_FUNC_ARGS);
#ifdef __cplusplus
}
#endif

#endif /* PHP_INTL_CONVERTER_H */
