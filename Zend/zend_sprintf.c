/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | This source file is subject to the Zend license, that is bundled     |
   | with this package in the file LICENSE.  If you did not receive a     |
   | copy of the Zend license, please mail us at zend@zend.com so we can  |
   | send you a copy immediately.                                         |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

#include <stdio.h>

#include "zend.h"

#if HAVE_STDARG_H
#include <stdarg.h>
#endif

#if BROKEN_SPRINTF
int zend_sprintf(char *buffer, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);

	return strlen(buffer);
}
#endif