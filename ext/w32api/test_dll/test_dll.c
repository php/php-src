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
   | Authors: James Moore <jmoore@php.net>		                          |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include <stdio.h>
#include "dll_test.h"

/**
 * Test DLL for w32api functions. Functions below are used to test
 * Various parts of the extension.
 */

/* Test for complex type passing */
TEST_DLL_API void print_names(name *n)
{
	printf("%s %s", n->fname, n->lname);
}