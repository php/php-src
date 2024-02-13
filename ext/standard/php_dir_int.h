/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_DIR_INT_H
#define PHP_DIR_INT_H

#ifdef HAVE_GLOB
#ifndef PHP_WIN32
#include <glob.h>
#else
#include "win32/glob.h"
#endif
#endif

#ifdef HAVE_GLOB

#ifndef GLOB_BRACE
#define GLOB_BRACE 0
#endif

#ifndef GLOB_ERR
#define GLOB_ERR 0
#endif

#ifndef GLOB_MARK
#define GLOB_MARK 0
#endif

#ifndef GLOB_NOCHECK
#define GLOB_NOCHECK 0
#endif

#ifndef GLOB_NOESCAPE
#define GLOB_NOESCAPE 0
#endif

#ifndef GLOB_NOSORT
#define GLOB_NOSORT 0
#endif

#ifndef GLOB_ONLYDIR
#define GLOB_ONLYDIR (1<<30)
#define GLOB_EMULATE_ONLYDIR
#define GLOB_FLAGMASK (~GLOB_ONLYDIR)
#else
#define GLOB_FLAGMASK (~0)
#endif

/* This is used for checking validity of passed flags (passing invalid flags causes segfault in glob()!! */
#define GLOB_AVAILABLE_FLAGS (0 | GLOB_BRACE | GLOB_MARK | GLOB_NOSORT | GLOB_NOCHECK | GLOB_NOESCAPE | GLOB_ERR | GLOB_ONLYDIR)

#endif /* HAVE_GLOB */

#endif /* PHP_DIR_INT_H */
