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
   | Authors: Christian Cartus <chc@idgruppe.de>                          |
   +----------------------------------------------------------------------+
 */


/* $Id$ */

#ifndef _PHP3_SYSVSHM_H
#define _PHP3_SYSVSHM_H

#if COMPILE_DL
#undef HAVE_SYSVSHM
#define HAVE_SYSVSHM 1
#endif


#if HAVE_SYSVSHM

extern php3_module_entry sysvshm_module_entry;
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



extern int php3_minit_sysvshm(INIT_FUNC_ARGS);
PHP_FUNCTION(sysvshm_attach);
PHP_FUNCTION(sysvshm_detach);
PHP_FUNCTION(sysvshm_remove);
PHP_FUNCTION(sysvshm_put_var);
PHP_FUNCTION(sysvshm_get_var);
PHP_FUNCTION(sysvshm_remove_var);
extern int php3int_put_shmdata(sysvshm_chunk_head *ptr,long key,char *data, long len);
extern long php3int_check_shmdata(sysvshm_chunk_head *ptr, long key);
extern int php3int_remove_shmdata(sysvshm_chunk_head *ptr, long shm_varpos);

extern sysvshm_module php3_sysvshm_module;

#else

#define sysvshm_module_ptr NULL

#endif

#define phpext_sysvshm_ptr sysvshm_module_ptr

#endif /* _PHP3_SYSVSHM_H */
