/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Tom May <tom@go2net.com>                                    |
   +----------------------------------------------------------------------+
 */


/* $Id$ */

#ifndef PHP_SYSVSEM_H
#define PHP_SYSVSEM_H

#if HAVE_SYSVSEM

extern zend_module_entry sysvsem_module_entry;
#define sysvsem_module_ptr &sysvsem_module_entry

PHP_MINIT_FUNCTION(sysvsem);
PHP_FUNCTION(sem_get);
PHP_FUNCTION(sem_acquire);
PHP_FUNCTION(sem_release);

typedef struct {
	int le_sem;
} sysvsem_module;

typedef struct {
	int id;						/* For error reporting. */
	int key;					/* For error reporting. */
	int semid;					/* Returned by semget(). */
	int count;					/* Acquire count for auto-release. */
} sysvsem_sem;

extern sysvsem_module php_sysvsem_module;

#else

#define sysvsem_module_ptr NULL

#endif

#define phpext_sysvsem_ptr sysvsem_module_ptr

#endif /* PHP_SYSVSEM_H */
