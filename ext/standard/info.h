/* 
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef INFO_H
#define INFO_H

#define PHP_ENTRY_NAME_COLOR "#ccccff"
#define PHP_CONTENTS_COLOR "#cccccc"
#define PHP_HEADER_COLOR "#9999cc"

#define PHP_INFO_GENERAL			(1<<0)
#define PHP_INFO_CREDITS			(1<<1)
#define PHP_INFO_CONFIGURATION		(1<<2)
#define PHP_INFO_MODULES			(1<<3)
#define PHP_INFO_ENVIRONMENT		(1<<4)
#define PHP_INFO_VARIABLES			(1<<5)
#define PHP_INFO_LICENSE			(1<<6)
#define PHP_INFO_ALL				0xFFFFFFFF

#ifndef HAVE_CREDITS_DEFS
#define HAVE_CREDITS_DEFS

#define PHP_CREDITS_GROUP			(1<<0)
#define PHP_CREDITS_GENERAL			(1<<1)
#define PHP_CREDITS_SAPI			(1<<2)
#define PHP_CREDITS_MODULES			(1<<3)
#define PHP_CREDITS_DOCS			(1<<4)
#define PHP_CREDITS_FULLPAGE		(1<<5)
#define PHP_CREDITS_QA				(1<<6)
#define PHP_CREDITS_WEB             (1<<7)
#define PHP_CREDITS_ALL				0xFFFFFFFF

#endif /* HAVE_CREDITS_DEFS */

#define PHP_LOGO_GUID		  "PHPE9568F34-D428-11d2-A769-00AA001ACF42"
#define PHP_EGG_LOGO_GUID	"PHPE9568F36-D428-11d2-A769-00AA001ACF42"
#define ZEND_LOGO_GUID		"PHPE9568F35-D428-11d2-A769-00AA001ACF42"
#define PHP_CREDITS_GUID  "PHPB8B5F2A0-3C92-11d3-A3A9-4C7B08C10000"

PHP_FUNCTION(phpversion);
PHP_FUNCTION(phpinfo);
PHP_FUNCTION(phpcredits);
PHP_FUNCTION(php_logo_guid);
PHP_FUNCTION(zend_logo_guid);
PHP_FUNCTION(php_egg_logo_guid);
PHP_FUNCTION(php_sapi_name);
PHP_FUNCTION(php_uname);
PHP_FUNCTION(php_ini_scanned_files);
PHPAPI char *php_info_html_esc(char *string TSRMLS_DC);
PHPAPI void php_print_info_htmlhead(TSRMLS_D);
PHPAPI void php_print_info(int flag TSRMLS_DC);
PHPAPI void php_print_style(void);
PHPAPI void php_info_print_style(void);
PHPAPI void php_info_print_table_colspan_header(int num_cols, char *header);
PHPAPI void php_info_print_table_header(int num_cols, ...);
PHPAPI void php_info_print_table_row(int num_cols, ...);
PHPAPI void php_info_print_table_start(void);
PHPAPI void php_info_print_table_end(void);
PHPAPI void php_info_print_box_start(int bg);
PHPAPI void php_info_print_box_end(void);
PHPAPI void php_info_print_hr(void);

void register_phpinfo_constants(INIT_FUNC_ARGS);

#endif /* INFO_H */
