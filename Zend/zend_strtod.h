/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2004 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Derick Rethans <derick@php.net>                             |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/* This is a header file for the strtod implementation by David M. Gay which
 * can be found in zend_strtod.c */
#ifndef ZEND_STRTOD_H
#define ZEND_STRTOD_H
#include <zend.h>

BEGIN_EXTERN_C()
ZEND_API double zend_strtod(const char *s00, char **se);
END_EXTERN_C()

#endif
