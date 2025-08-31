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
