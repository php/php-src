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
   | Authors: Christian Cartus <cartus@atrior.de>                         |
   +----------------------------------------------------------------------+
 */


/* $Id$ */

#ifndef PHP_SYSVSHM_H
#define PHP_SYSVSHM_H

#if HAVE_SYSVSHM

extern zend_module_entry sysvshm_module_entry;
#define sysvshm_module_ptr &sysvshm_module_entry

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

typedef struct {
	int le_shm;
	long init_mem;
} sysvshm_module;


typedef struct {
	long key;
	long length;
	long next;
	char mem;
} sysvshm_chunk;


typedef struct {
	char magic[8];
	long start;
	long end;
	long free;
	long total;
} sysvshm_chunk_head;


typedef struct {
	key_t key;					/* Key set by user */
	long id;					/* Returned by shmget. */
	sysvshm_chunk_head *ptr;			/* memoryaddress of shared memory */ 
} sysvshm_shm;



PHP_MINIT_FUNCTION(sysvshm);
PHP_FUNCTION(shm_attach);
PHP_FUNCTION(shm_detach);
PHP_FUNCTION(shm_remove);
PHP_FUNCTION(shm_put_var);
PHP_FUNCTION(shm_get_var);
PHP_FUNCTION(shm_remove_var);
extern int php_put_shm_data(sysvshm_chunk_head *ptr,long key,char *data, long len);
extern long php_check_shm_data(sysvshm_chunk_head *ptr, long key);
extern int php_remove_shm_data(sysvshm_chunk_head *ptr, long shm_varpos);

extern sysvshm_module php_sysvshm;

#else

#define sysvshm_module_ptr NULL

#endif

#define phpext_sysvshm_ptr sysvshm_module_ptr

#endif /* PHP_SYSVSHM_H */
