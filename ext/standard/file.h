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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef _FILE_H
#define _FILE_H

extern php3_module_entry php3_file_module_entry;
#define php3_file_module_ptr &php3_file_module_entry

extern int php3_minit_file(INIT_FUNC_ARGS);
extern void php3_tempnam(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fopen(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fclose(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_popen(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pclose(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_feof(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fread(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fgetc(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fgets(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fgetss(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fgetcsv(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fwrite(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_rewind(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ftell(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fseek(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_mkdir(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_rmdir(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fpassthru(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_readfile(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fileumask(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_rename(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_file_copy(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_file(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_set_socket_blocking(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_set_socket_timeout(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_get_meta_tags(INTERNAL_FUNCTION_PARAMETERS);
extern PHP_FUNCTION(flock);

#endif /* _FILE_H */
