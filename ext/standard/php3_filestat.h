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
   | Author:  Jim Winstead <jimw@php.net>                                 |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef _FILESTAT_H
#define _FILESTAT_H

extern int php3_init_filestat(INIT_FUNC_ARGS);
extern int php3_shutdown_filestat(SHUTDOWN_FUNC_ARGS);
PHP_FUNCTION(clearstatcache);
PHP_FUNCTION(fileatime);
PHP_FUNCTION(filectime);
PHP_FUNCTION(filegroup);
PHP_FUNCTION(fileinode);
PHP_FUNCTION(filemtime);
PHP_FUNCTION(fileowner);
PHP_FUNCTION(fileperms);
PHP_FUNCTION(filesize);
PHP_FUNCTION(filetype);
PHP_FUNCTION(iswritable);
PHP_FUNCTION(isreadable);
PHP_FUNCTION(isexec);
PHP_FUNCTION(isfile);
PHP_FUNCTION(isdir);
PHP_FUNCTION(islink);
PHP_FUNCTION(fileexists);
PHP_FUNCTION(stat);
PHP_FUNCTION(lstat);
PHP_FUNCTION(chown);
PHP_FUNCTION(chgrp);
PHP_FUNCTION(chmod);
PHP_FUNCTION(touch);

extern php3_module_entry php3_filestat_module_entry;
#define php3_filestat_module_ptr &php3_filestat_module_entry

#define phpext_filestat_ptr php3_filestat_module_ptr

#endif /* _FILESTAT_H */
