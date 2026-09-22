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
   | Authors: Shane Caraveo <shane@caraveo.com>                           |
   |          Ilia Alshanetsky  <ilia@prohost.org>                        |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_SCANDIR_H
#define PHP_SCANDIR_H

#include <sys/types.h>

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
