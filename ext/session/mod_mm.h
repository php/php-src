/* 
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
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

extern ps_module ps_mod_mm;
#define ps_mm_ptr &ps_mod_mm

PS_FUNCS(mm);

#endif
#endif
