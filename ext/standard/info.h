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
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#ifndef _INFO_H
#define _INFO_H

#define PHP_ENTRY_NAME_COLOR "#CCCCFF"
#define PHP_CONTENTS_COLOR "#CCCCCC"
#define PHP_HEADER_COLOR "#9999CC"

#define PHP_INFO_GENERAL			(1<<0)
#define PHP_INFO_CREDITS			(1<<1)
#define PHP_INFO_CONFIGURATION		(1<<2)
#define PHP_INFO_MODULES			(1<<3)
#define PHP_INFO_ENVIRONMENT		(1<<4)
#define PHP_INFO_VARIABLES			(1<<5)
#define PHP_INFO_LICENSE			(1<<6)
#define PHP_INFO_ALL				0xFFFFFFFF


#define PHP_CREDITS_GROUP			(1<<0)
#define PHP_CREDITS_GENERAL			(1<<1)
#define PHP_CREDITS_SAPI			(1<<2)
#define PHP_CREDITS_MODULES			(1<<3)
#define PHP_CREDITS_DOCS			(1<<4)
#define PHP_CREDITS_FULLPAGE		(1<<5)
#define PHP_CREDITS_ALL				0xFFFFFFFF

#define PHP_LOGO_GUID		"PHPE9568F34-D428-11d2-A769-00AA001ACF42"
#define PHP_EGG_LOGO_GUID	"PHPE9568F36-D428-11d2-A769-00AA001ACF42"
#define ZEND_LOGO_GUID		"PHPE9568F35-D428-11d2-A769-00AA001ACF42"

PHP_FUNCTION(phpversion);
PHP_FUNCTION(phpinfo);
PHP_FUNCTION(phpcredits);
PHP_FUNCTION(php_logo_guid);
PHP_FUNCTION(zend_logo_guid);
PHP_FUNCTION(php_sapi_name);
PHPAPI void php_print_info(int flag);
PHPAPI void php_print_credits(int flag);
PHPAPI void php_print_style(void);
PHPAPI void php_info_print_table_colspan_header(int num_cols, char *header);
PHPAPI void php_info_print_table_header(int num_cols, ...);
PHPAPI void php_info_print_table_row(int num_cols, ...);
PHPAPI void php_info_print_table_start(void);
PHPAPI void php_info_print_table_end(void);
PHPAPI void php_info_print_box_start(int bg);
PHPAPI void php_info_print_box_end(void);
PHPAPI void php_info_print_hr(void);

void register_phpinfo_constants(INIT_FUNC_ARGS);

#endif /* _INFO_H */
