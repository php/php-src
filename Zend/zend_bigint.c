/*
  +----------------------------------------------------------------------+
  | Zend Big Integer Supporti                                            |
  +----------------------------------------------------------------------+
  | Copyright (c) 2014 The PHP Group                                     |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Andrea Faulds <ajf@ajf.me>                                  |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend.h"
#include "zend_bigint.h"

/* There are two different bigint backends: GMP and libtommath
 * - libtommath is enabled and built by default, with its source in the repo
 * - GMP can be optionally enabled
 * This file will include the libtommath backend unless the GMP is available */

#if defined(ZEND_HAVE_LIBTOMMATH)
#	include "zend_bigint_libtommath.c"
#elif defined(ZEND_HAVE_GMP)
#	include "zend_bigint_gmp.c" 
#else
#	error Neither LibTomMath nor GMP available
#endif
