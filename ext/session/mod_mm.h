/* 
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   +----------------------------------------------------------------------+
 */

#ifndef MOD_MM_H
#define MOD_MM_H

#ifdef HAVE_LIBMM

#include "php_session.h"

extern ps_module ps_mod_mm;
#define ps_mm_ptr &ps_mod_mm

extern zend_module_entry php_session_mm_module;
#define phpext_ps_mm_ptr &php_session_mm_module

PS_FUNCS(mm);

#else

#define ps_mm_ptr NULL
#define phpext_ps_mm_ptr NULL

#endif

#endif
