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
   | Authors: Chad Robinson <chadr@brttech.com>                           |
   +----------------------------------------------------------------------+

  filePro 4.x support developed by Chad Robinson, chadr@brttech.com
  Contact Chad Robinson at BRT Technical Services Corp. for details.
  filePro is a registered trademark by Fiserv, Inc.  This file contains
  no code or information that is not freely available from the filePro
  web site at http://www.fileproplus.com/

 */

/* $Id$ */

#ifndef PHP_FILEPRO_H
#define PHP_FILEPRO_H
#if HAVE_FILEPRO
extern zend_module_entry filepro_module_entry;
#define phpext_filepro_ptr &filepro_module_entry

PHP_FUNCTION(filepro);
PHP_FUNCTION(filepro_rowcount);
PHP_FUNCTION(filepro_fieldname);
PHP_FUNCTION(filepro_fieldtype);
PHP_FUNCTION(filepro_fieldwidth);
PHP_FUNCTION(filepro_fieldcount);
PHP_FUNCTION(filepro_retrieve);

extern PHP_MINIT_FUNCTION(filepro);
extern PHP_MSHUTDOWN_FUNCTION(filepro);
#else
#define phpext_filepro_ptr NULL
#endif
#endif /* PHP_FILEPRO_H */
