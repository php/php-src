/* 
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
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

#include "php.h"
#include "php_ini.h"
#include "php_globals.h"
#include "ext/standard/head.h"
#include "info.h"
#include "credits.h"
#include "SAPI.h"
#include <time.h>
#include "php_main.h"
#if !defined(PHP_WIN32) && !defined(NETWARE)
#include "build-defs.h"
#endif
#include "zend_globals.h"		/* needs ELS */
#include "zend_highlight.h"

/*#ifdef NETWARE*/
/*#include "netware/env.h"*/	/* Temporary */
/*#endif*/

#define SECTION(name)  PUTS("<h2 align=\"center\">" name "</h2>\n")

PHPAPI extern char *php_ini_opened_path;

/* {{{ _display_module_info
 */
static int _display_module_info(zend_module_entry *module, void *arg TSRMLS_DC)
{
	int show_info_func = *((int *) arg);

	if (show_info_func && module->info_func) {
		php_printf("<h2 align=\"center\"><a name=\"module_%s\">%s</a></h2>\n", module->name, module->name);
		module->info_func(module TSRMLS_CC);
	} else if (!show_info_func && !module->info_func) {
		php_printf("<tr valign=\"baseline\" bgcolor=\"" PHP_CONTENTS_COLOR "\">");
		php_printf("<td>");
		php_printf("%s", module->name);
		php_printf("</td></tr>\n");
	}
	return 0;
}
/* }}} */

/* {{{ php_print_gpcse_array
 */
static void php_print_gpcse_array(char *name, uint name_length TSRMLS_DC)
{
	zval **data, **tmp, tmp2;
	char *string_key;
	ulong num_key;

	if (zend_hash_find(&EG(symbol_table), name, name_length+1, (void **) &data)!=FAILURE
		&& (Z_TYPE_PP(data)==IS_ARRAY)) {
		zend_hash_internal_pointer_reset(Z_ARRVAL_PP(data));
		while (zend_hash_get_current_data(Z_ARRVAL_PP(data), (void **) &tmp) == SUCCESS) {
			PUTS("<tr valign=\"baseline\" bgcolor=\"" PHP_CONTENTS_COLOR "\">");
			PUTS("<td bgcolor=\"" PHP_ENTRY_NAME_COLOR "\"><b>");
			PUTS(name);
			PUTS("[\"");
			switch (zend_hash_get_current_key(Z_ARRVAL_PP(data), &string_key, &num_key, 0)) {
				case HASH_KEY_IS_STRING:
					php_html_puts(string_key, strlen(string_key) TSRMLS_CC);
					break;
				case HASH_KEY_IS_LONG:
					php_printf("%ld", num_key);
					break;
			}
			PUTS("\"]</b></td><td>");
			if (Z_TYPE_PP(tmp) == IS_ARRAY) {
				PUTS("<pre>");
				zend_print_zval_r(*tmp, 0);
				PUTS("</pre>");
			} else if (Z_TYPE_PP(tmp) != IS_STRING) {
				tmp2 = **tmp;
				zval_copy_ctor(&tmp2);
				convert_to_string(&tmp2);
				php_html_puts(Z_STRVAL(tmp2), Z_STRLEN(tmp2) TSRMLS_CC);
				zval_dtor(&tmp2);
			} else {
				php_html_puts(Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp) TSRMLS_CC);
			}
			PUTS("&nbsp;</td></tr>\n");
			zend_hash_move_forward(Z_ARRVAL_PP(data));
		}
	}
}
/* }}} */

/* {{{ php_info_print_style
 */
void php_info_print_style(void)
{
	php_printf("<style type=\"text/css\"><!--\n");
	php_printf("a { text-decoration: none; }\n");
	php_printf("a:hover { text-decoration: underline; }\n");
	php_printf("h1 { font-family: arial, helvetica, sans-serif; font-size: 18pt; font-weight: bold;}\n");
	php_printf("h2 { font-family: arial, helvetica, sans-serif; font-size: 14pt; font-weight: bold;}\n");
	php_printf("body, td { font-family: arial, helvetica, sans-serif; font-size: 10pt; }\n");
	php_printf("th { font-family: arial, helvetica, sans-serif; font-size: 11pt; font-weight: bold; }\n");
	php_printf("//--></style>\n");
}
/* }}} */

/* {{{ php_get_uname
 */
PHPAPI char *php_get_uname()
{
	char *php_uname;
#ifdef PHP_WIN32
	char php_windows_uname[256];
	DWORD dwBuild=0;
	DWORD dwVersion = GetVersion();
	DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));

	/* Get build numbers for Windows NT or Win95 */
	if (dwVersion < 0x80000000){
		dwBuild = (DWORD)(HIWORD(dwVersion));
		snprintf(php_windows_uname, 255, "%s %d.%d build %d", "Windows NT", dwWindowsMajorVersion, dwWindowsMinorVersion, dwBuild);
	} else {
		snprintf(php_windows_uname, 255, "%s %d.%d", "Windows 95/98", dwWindowsMajorVersion, dwWindowsMinorVersion);
	}
	php_uname = php_windows_uname;
#else
	php_uname=PHP_UNAME;
#endif
	return estrdup(php_uname);
}
/* }}} */

/* {{{ php_print_info
 */
PHPAPI void php_print_info(int flag TSRMLS_DC)
{
	char **env, *tmp1, *tmp2;
	char *php_uname;
	int expose_php = INI_INT("expose_php");
	time_t the_time;
	struct tm *ta, tmbuf;

	the_time = time(NULL);
	ta = php_localtime_r(&the_time, &tmbuf);

	PUTS("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n<html>\n");

	if (flag & PHP_INFO_GENERAL) {
		char *zend_version = get_zend_version();

		php_uname = php_get_uname();
		PUTS("<head>");
		php_info_print_style();
		PUTS("<title>phpinfo()</title></head><body>");

		php_info_print_box_start(1);
		if (expose_php) {
			PUTS("<a href=\"http://www.php.net/\"><img src=\"");
			if (SG(request_info).request_uri) {
				PUTS(SG(request_info).request_uri);
			}
			if ((ta->tm_mon==3) && (ta->tm_mday==1)) {
				PUTS("?="PHP_EGG_LOGO_GUID"\" border=0 align=\"right\" alt=\"Thies!\"></a>");
			} else {
				PUTS("?="PHP_LOGO_GUID"\" border=0 align=\"right\" alt=\"PHP Logo\"></a>");
			}
		}
		php_printf("<h1>PHP Version %s</h1>\n", PHP_VERSION);
		php_info_print_box_end();
		php_info_print_table_start();
		php_info_print_table_row(2, "System", php_uname );
		php_info_print_table_row(2, "Build Date", __DATE__ " " __TIME__ );
#ifdef CONFIGURE_COMMAND
		php_info_print_table_row(2, "Configure Command", CONFIGURE_COMMAND );
#endif
		if (sapi_module.pretty_name) {
			php_info_print_table_row(2, "Server API", sapi_module.pretty_name );
		}

#ifdef VIRTUAL_DIR
		php_info_print_table_row(2, "Virtual Directory Support", "enabled" );
#else
		php_info_print_table_row(2, "Virtual Directory Support", "disabled" );
#endif

		php_info_print_table_row(2, "Configuration File (php.ini) Path", php_ini_opened_path?php_ini_opened_path:PHP_CONFIG_FILE_PATH);

#if ZEND_DEBUG
		php_info_print_table_row(2, "Debug Build", "yes" );
#else
		php_info_print_table_row(2, "Debug Build", "no" );
#endif

#ifdef ZTS
		php_info_print_table_row(2, "Thread Safety", "enabled" );
#else
		php_info_print_table_row(2, "Thread Safety", "disabled" );
#endif

#if HAVE_PHP_STREAM
		php_info_print_table_row(2, "Experimental PHP Streams", "enabled");
#endif
		
		php_info_print_table_end();

		/* Zend Engine */
		php_info_print_box_start(0);
		if (expose_php) {
			PUTS("<a href=\"http://www.zend.com/\"><img src=\"");
			if (SG(request_info).request_uri) {
				PUTS(SG(request_info).request_uri);
			}
			PUTS("?="ZEND_LOGO_GUID"\" border=\"0\" align=\"right\" alt=\"Zend logo\"></a>\n");
		}
		php_printf("This program makes use of the Zend Scripting Language Engine:<br />");
		php_html_puts(zend_version, strlen(zend_version) TSRMLS_CC);
		php_info_print_box_end();
		efree(php_uname);
	}

	if ((flag & PHP_INFO_CREDITS) && expose_php) {	
		php_info_print_hr();
		PUTS("<h1 align=\"center\"><a href=\"");
		if (SG(request_info).request_uri) {
			PUTS(SG(request_info).request_uri);
		}
		PUTS("?=PHPB8B5F2A0-3C92-11d3-A3A9-4C7B08C10000\">");
		PUTS("PHP 4 Credits");
		PUTS("</a></h1>\n");
	}

	zend_ini_sort_entries(TSRMLS_C);

	if (flag & PHP_INFO_CONFIGURATION) {
		php_info_print_hr();
		PUTS("<h1 align=\"center\">Configuration</h1>\n");
		SECTION("PHP Core\n");
		display_ini_entries(NULL);
	}

	if (flag & PHP_INFO_MODULES) {
		int show_info_func;

		show_info_func = 1;
		zend_hash_apply_with_argument(&module_registry, (apply_func_arg_t) _display_module_info, &show_info_func TSRMLS_CC);

		SECTION("Additional Modules");
		php_info_print_table_start();
		show_info_func = 0;
		zend_hash_apply_with_argument(&module_registry, (apply_func_arg_t) _display_module_info, &show_info_func TSRMLS_CC);
		php_info_print_table_end();
	}

	if (flag & PHP_INFO_ENVIRONMENT) {
		SECTION("Environment");
		php_info_print_table_start();
		php_info_print_table_header(2, "Variable", "Value");
		for (env=environ; env!=NULL && *env !=NULL; env++) {
			tmp1 = estrdup(*env);
			if (!(tmp2=strchr(tmp1,'='))) { /* malformed entry? */
				efree(tmp1);
				continue;
			}
			*tmp2 = 0;
			tmp2++;
			php_info_print_table_row(2, tmp1, tmp2);
			efree(tmp1);
		}
		php_info_print_table_end();
	}

	if (flag & PHP_INFO_VARIABLES) {
		pval **data;

		SECTION("PHP Variables");

		php_info_print_table_start();
		php_info_print_table_header(2, "Variable", "Value");
		if (zend_hash_find(&EG(symbol_table), "PHP_SELF", sizeof("PHP_SELF"), (void **) &data) != FAILURE) {
			php_info_print_table_row(2, "PHP_SELF", Z_STRVAL_PP(data));
		}
		if (zend_hash_find(&EG(symbol_table), "PHP_AUTH_TYPE", sizeof("PHP_AUTH_TYPE"), (void **) &data) != FAILURE) {
			php_info_print_table_row(2, "PHP_AUTH_TYPE", Z_STRVAL_PP(data));
		}
		if (zend_hash_find(&EG(symbol_table), "PHP_AUTH_USER", sizeof("PHP_AUTH_USER"), (void **) &data) != FAILURE) {
			php_info_print_table_row(2, "PHP_AUTH_USER", Z_STRVAL_PP(data));
		}
		if (zend_hash_find(&EG(symbol_table), "PHP_AUTH_PW", sizeof("PHP_AUTH_PW"), (void **) &data) != FAILURE) {
			php_info_print_table_row(2, "PHP_AUTH_PW", Z_STRVAL_PP(data));
		}
		php_print_gpcse_array("_FORM", sizeof("_FORM")-1 TSRMLS_CC);
		php_print_gpcse_array("_GET", sizeof("_GET")-1 TSRMLS_CC);
		php_print_gpcse_array("_POST", sizeof("_POST")-1 TSRMLS_CC);
		php_print_gpcse_array("_FILES", sizeof("_FILES")-1 TSRMLS_CC);
		php_print_gpcse_array("_COOKIE", sizeof("_COOKIE")-1 TSRMLS_CC);
		php_print_gpcse_array("_SERVER", sizeof("_SERVER")-1 TSRMLS_CC);
		php_print_gpcse_array("_ENV", sizeof("_ENV")-1 TSRMLS_CC);
		php_info_print_table_end();
	}

	if (flag & PHP_INFO_LICENSE) {
		SECTION("PHP License");
		php_info_print_box_start(0);
		PUTS("<p>\n");
		PUTS("This program is free software; you can redistribute it and/or modify ");
		PUTS("it under the terms of the PHP License as published by the PHP Group ");
		PUTS("and included in the distribution in the file:  LICENSE\n");
		PUTS("</p>\n");
		PUTS("<p>");
		PUTS("This program is distributed in the hope that it will be useful, ");
		PUTS("but WITHOUT ANY WARRANTY; without even the implied warranty of ");
		PUTS("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
		PUTS("</p>\n");
		PUTS("<p>");
		PUTS("If you did not receive a copy of the PHP license, or have any questions about ");
		PUTS("PHP licensing, please contact license@php.net.\n");
		PUTS("</p>\n");
		php_info_print_box_end();
	}

	PUTS("</body></html>");
}
/* }}} */


PHPAPI void php_info_print_table_start()
{
	php_printf("<table border=\"0\" cellpadding=\"3\" cellspacing=\"1\" width=\"600\" bgcolor=\"#000000\" align=\"center\">\n");
}

PHPAPI void php_info_print_table_end()
{
	php_printf("</table><br />\n");

}

PHPAPI void php_info_print_box_start(int flag)
{
	php_info_print_table_start();
	if (flag) {
		php_printf("<tr valign=\"middle\" bgcolor=\"" PHP_HEADER_COLOR "\"><td align=\"left\">\n");
	} else {
		php_printf("<tr valign=\"top\" bgcolor=\"" PHP_CONTENTS_COLOR "\"><td align=\"left\">\n");
	}
}

PHPAPI void php_info_print_box_end()
{
	php_printf("</td></tr>\n");
	php_info_print_table_end();
}

PHPAPI void php_info_print_hr()
{
	php_printf("<hr noshade size=\"1\" width=\"600\">\n");
}

PHPAPI void php_info_print_table_colspan_header(int num_cols, char *header)
{
	php_printf("<tr bgcolor=\"" PHP_HEADER_COLOR "\"><th colspan=\"%d\">%s</th></tr>\n", num_cols, header );
}

/* {{{ php_info_print_table_header
 */
PHPAPI void php_info_print_table_header(int num_cols, ...)
{
	int i;
	va_list row_elements;
	char *row_element;

	va_start(row_elements, num_cols);

	php_printf("<tr valign=\"middle\" bgcolor=\"" PHP_HEADER_COLOR "\">");
	for (i=0; i<num_cols; i++) {
		row_element = va_arg(row_elements, char *);
		if (!row_element || !*row_element) {
			row_element = "&nbsp;";
		}
		php_printf("<th>%s</th>", row_element);
	}
	php_printf("</tr>\n");

	va_end(row_elements);
}
/* }}} */

/* {{{ php_info_print_table_row
 */
PHPAPI void php_info_print_table_row(int num_cols, ...)
{
	int i;
	va_list row_elements;
	char *row_element;
	TSRMLS_FETCH();

	va_start(row_elements, num_cols);

	php_printf("<tr valign=\"baseline\" bgcolor=\"" PHP_CONTENTS_COLOR "\">");
	for (i=0; i<num_cols; i++) {
		php_printf("<td %s>%s",
			(i==0?"bgcolor=\"" PHP_ENTRY_NAME_COLOR "\" ":"align=\"left\""),
			(i==0?"<b>":""));

		row_element = va_arg(row_elements, char *);
		if (!row_element || !*row_element) {
			php_printf("&nbsp;");
		} else {
			php_html_puts(row_element, strlen(row_element) TSRMLS_CC);
		}

		php_printf("%s</td>", (i==0?"</b>":""));
	}
	php_printf("</tr>\n");

	va_end(row_elements);
}
/* }}} */

/* {{{ register_phpinfo_constants
 */
void register_phpinfo_constants(INIT_FUNC_ARGS)
{
	REGISTER_LONG_CONSTANT("INFO_GENERAL", PHP_INFO_GENERAL, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("INFO_CREDITS", PHP_INFO_CREDITS, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("INFO_CONFIGURATION", PHP_INFO_CONFIGURATION, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("INFO_MODULES", PHP_INFO_MODULES, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("INFO_ENVIRONMENT", PHP_INFO_ENVIRONMENT, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("INFO_VARIABLES", PHP_INFO_VARIABLES, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("INFO_LICENSE", PHP_INFO_LICENSE, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("INFO_ALL", PHP_INFO_ALL, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("CREDITS_GROUP",	PHP_CREDITS_GROUP, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("CREDITS_GENERAL",	PHP_CREDITS_GENERAL, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("CREDITS_SAPI",	PHP_CREDITS_SAPI, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("CREDITS_MODULES",	PHP_CREDITS_MODULES, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("CREDITS_DOCS",	PHP_CREDITS_DOCS, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("CREDITS_FULLPAGE",	PHP_CREDITS_FULLPAGE, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("CREDITS_QA",	PHP_CREDITS_QA, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("CREDITS_ALL",	PHP_CREDITS_ALL, CONST_PERSISTENT|CONST_CS);
}
/* }}} */

/* {{{ proto void phpinfo([int what])
   Output a page of useful information about PHP and the current request */
PHP_FUNCTION(phpinfo)
{
	int argc = ZEND_NUM_ARGS();
	long flag;

	if (zend_parse_parameters(argc TSRMLS_CC, "|l", &flag) == FAILURE) {
		return;
	}

	if(!argc) {
		flag = PHP_INFO_ALL;
	}

	php_print_info(flag TSRMLS_CC);

	RETURN_TRUE;
}

/* }}} */

/* {{{ proto string phpversion([string extension])
   Return the current PHP version */
PHP_FUNCTION(phpversion)
{
    zval **arg;
    int argc = ZEND_NUM_ARGS();

	if (argc == 0) {
        RETURN_STRING(PHP_VERSION, 1);
	} else if (argc == 1 && zend_get_parameters_ex(1, &arg) == SUCCESS) {
        char *version;
        convert_to_string_ex(arg);
        version = zend_get_module_version(Z_STRVAL_PP(arg));
        if (version == NULL) {
            RETURN_FALSE;
        }
        RETURN_STRING(version, 1);
    } else {
		WRONG_PARAM_COUNT;
    }
}
/* }}} */

/* {{{ proto void phpcredits([int flag])
   Prints the list of people who've contributed to the PHP project */
PHP_FUNCTION(phpcredits)
{
	int argc = ZEND_NUM_ARGS();
	long flag;

	if (zend_parse_parameters(argc TSRMLS_CC, "|l", &flag) == FAILURE) {
		return;
	}

	if(!argc) {
		flag = PHP_CREDITS_ALL;
	} 

	php_print_credits(flag);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string php_logo_guid(void)
   Return the special ID used to request the PHP logo in phpinfo screens*/
PHP_FUNCTION(php_logo_guid)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	RETURN_STRINGL(PHP_LOGO_GUID, sizeof(PHP_LOGO_GUID)-1, 1);
}
/* }}} */

/* {{{ proto string php_egg_logo_guid(void)
   Return the special ID used to request the PHP logo in phpinfo screens*/
PHP_FUNCTION(php_egg_logo_guid)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	RETURN_STRINGL(PHP_EGG_LOGO_GUID, sizeof(PHP_EGG_LOGO_GUID)-1, 1);
}
/* }}} */

/* {{{ proto string zend_logo_guid(void)
   Return the special ID used to request the Zend logo in phpinfo screens*/
PHP_FUNCTION(zend_logo_guid)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	RETURN_STRINGL(ZEND_LOGO_GUID, sizeof(ZEND_LOGO_GUID)-1, 1);
}
/* }}} */

/* {{{ proto string php_sapi_name(void)
   Return the current SAPI module name */
PHP_FUNCTION(php_sapi_name)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	if (sapi_module.name) {
		RETURN_STRING(sapi_module.name, 1);
	} else {
		RETURN_FALSE;
	}
}

/* }}} */

/* {{{ proto string php_uname(void)
   Return information about the system PHP was built on */
PHP_FUNCTION(php_uname)
{
    if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	RETURN_STRING(php_get_uname(), 0);
}

/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
