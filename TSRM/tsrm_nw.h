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
   | Authors: Venkat Raghavan S <rvenkat@novell.com>                      |
   |          Anantha Kesari H Y <hyanantha@novell.com>                   |
   +----------------------------------------------------------------------+
*/


#ifndef TSRM_NW_H
#define TSRM_NW_H

#include "TSRM.h"

TSRM_API FILE* popen(const char *command, const char *type);
TSRM_API int pclose(FILE* stream);

#endif
