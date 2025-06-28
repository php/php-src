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
   | Author: Anatol Belski <ab@php.net>                                   |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_WIN32_IPC_H
#define PHP_WIN32_IPC_H 1

#ifdef PHP_EXPORTS
# define PHP_WIN32_IPC_API __declspec(dllexport)
#else
# define PHP_WIN32_IPC_API __declspec(dllimport)
#endif

typedef int key_t;

PHP_WIN32_IPC_API key_t ftok(const char *path, int id);


#endif /* PHP_WIN32_IPC_H */
