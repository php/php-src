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
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
 */

#ifndef MOD_MM_H
#define MOD_MM_H

#ifdef HAVE_LIBMM

#include "php_session.h"

PHP_MINIT_FUNCTION(ps_mm);
PHP_MSHUTDOWN_FUNCTION(ps_mm);

extern const ps_module ps_mod_mm;
#define ps_mm_ptr &ps_mod_mm

PS_FUNCS(mm);

#endif
#endif
