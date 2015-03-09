/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Shane Caraveo <shane@caraveo.com>                           |
   |          Ilia Alshanetsky  <ilia@prohost.org>                        |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_SCANDIR_H
#define PHP_SCANDIR_H

#include <sys/types.h>

#ifdef HAVE_SYS_DIR_H
#include <sys/dir.h>
#endif

#ifdef PHP_WIN32
#include "config.w32.h"
#include "win32/readdir.h"
#else
#include <php_config.h>
#endif

#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif

#ifdef HAVE_SCANDIR
#define php_scandir		scandir
#else
PHPAPI int php_scandir(const char *dirname, struct dirent **namelist[], int (*selector) (const struct dirent *entry), int (*compare) (const struct dirent **a, const struct dirent **b));
#endif

#ifdef HAVE_ALPHASORT
#define php_alphasort	alphasort
#else
PHPAPI int php_alphasort(const struct dirent **a, const struct dirent **b);
#endif

#endif /* PHP_SCANDIR_H */
