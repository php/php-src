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
PHP_FUNCTION(tempnam);
PHP_FUNCTION(fopen);
PHP_FUNCTION(fclose);
PHP_FUNCTION(popen);
PHP_FUNCTION(pclose);
PHP_FUNCTION(feof);
PHP_FUNCTION(fread);
PHP_FUNCTION(fgetc);
PHP_FUNCTION(fgets);
PHP_FUNCTION(fgetss);
PHP_FUNCTION(fgetcsv);
PHP_FUNCTION(fwrite);
PHP_FUNCTION(rewind);
PHP_FUNCTION(ftell);
PHP_FUNCTION(fseek);
PHP_FUNCTION(mkdir);
PHP_FUNCTION(rmdir);
PHP_FUNCTION(fpassthru);
PHP_FUNCTION(readfile);
PHP_FUNCTION(fileumask);
PHP_FUNCTION(rename);
PHP_FUNCTION(file_copy);
PHP_FUNCTION(file);
PHP_FUNCTION(set_socket_blocking);
PHP_FUNCTION(set_socket_timeout);
PHP_FUNCTION(get_meta_tags);
PHP_FUNCTION(flock);

#define phpext_file_ptr php3_file_module_ptr
#endif /* _FILE_H */
