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
   | Authors: Evan Klinger <evan715@sirius.com>                           |
   +----------------------------------------------------------------------+
 */

#ifndef CYBERCASH_H
#define CYBERCASH_H


#if HAVE_MCK

extern zend_module_entry cybercash_module_entry;
#define cybercash_module_ptr &cybercash_module_entry

PHP_MINFO_FUNCTION(cybercash);
PHP_FUNCTION(cybercash_encr);
PHP_FUNCTION(cybercash_decr);
PHP_FUNCTION(cybercash_base64_encode);
PHP_FUNCTION(cybercash_base64_decode);

#else
#define cybercash_module_ptr NULL
#endif

#define phpext_cybercash_ptr cybercash_module_ptr

#endif
