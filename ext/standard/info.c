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
   |          Colin Viebrock <colin@easydns.com>                          |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "php_ini.h"
#include "php_globals.h"
#include "ext/standard/head.h"
#include "ext/standard/html.h"
#include "info.h"
#include "credits.h"
#include "css.h"
#include "SAPI.h"
#include <time.h>
#include "php_main.h"
#include "zend_globals.h"		/* needs ELS */
#include "zend_extensions.h"
#ifdef HAVE_SYS_UTSNAME_H
#include <sys/utsname.h>
#endif

#if HAVE_MBSTRING
#include "ext/mbstring/mbstring.h"
ZEND_EXTERN_MODULE_GLOBALS(mbstring)
#endif

#if HAVE_ICONV
#include "ext/iconv/php_iconv.h"
ZEND_EXTERN_MODULE_GLOBALS(iconv)
#endif

#define SECTION(name)	if (PG(html_errors)) { \
							PUTS("<h2>" name "</h2>\n"); \
						} else { \
							php_info_print_table_start(); \
							php_info_print_table_header(1, name); \
							php_info_print_table_end(); \
						} \

PHPAPI extern char *php_ini_opened_path;
PHPAPI extern char *php_ini_scanned_files;

/* {{{ _display_module_info
 */
static int _display_module_info(zend_module_entry *module, void *arg TSRMLS_DC)
{
	int show_info_func = *((int *) arg);

	if (show_info_func && module->info_func) {
		if (PG(html_errors)) {
			php_printf("<h2><a name=\"module_%s\">%s</a></h2>\n", module->name, module->name);
		} else {
			php_info_print_table_start();
			php_info_print_table_header(1, module->name);
			php_info_print_table_end();
		}
		module->info_func(module TSRMLS_CC);
	} else if (!show_info_func && !module->info_func) {
		if (PG(html_errors)) {
			php_printf("<tr>");
			php_printf("<td>");
			php_printf("%s", module->name);
			php_printf("</td></tr>\n");
		} else {
			php_printf(module->name);
			php_printf("\n");
		}	
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
	uint string_len;
	ulong num_key;
	char *elem_esc = NULL;

	if (zend_hash_find(&EG(symbol_table), name, name_length+1, (void **) &data)!=FAILURE
		&& (Z_TYPE_PP(data)==IS_ARRAY)) {
		zend_hash_internal_pointer_reset(Z_ARRVAL_PP(data));
		while (zend_hash_get_current_data(Z_ARRVAL_PP(data), (void **) &tmp) == SUCCESS) {
			if (PG(html_errors)) {
				PUTS("<tr>");
				PUTS("<td class=\"e\">");

			}

			PUTS(name);
			PUTS("[\"");
			
			switch (zend_hash_get_current_key_ex(Z_ARRVAL_PP(data), &string_key, &string_len, &num_key, 0, NULL)) {
				case HASH_KEY_IS_STRING:
					if (PG(html_errors)) {
						elem_esc = php_info_html_esc(string_key TSRMLS_CC);
						PUTS(elem_esc);
						efree(elem_esc);
					} else {
						PUTS(string_key);
					}	
					break;
				case HASH_KEY_IS_LONG:
					php_printf("%ld", num_key);
					break;
			}
			PUTS("\"]");
			if (PG(html_errors)) {
				PUTS("</td><td class=\"v\">");
			} else {
				PUTS(" => ");
			}
			if (Z_TYPE_PP(tmp) == IS_ARRAY) {
				if (PG(html_errors)) {
					PUTS("<pre>");
				}
				zend_print_zval_r(*tmp, 0 TSRMLS_CC);
				if (PG(html_errors)) {
					PUTS("</pre>");
				}
			} else if (Z_TYPE_PP(tmp) != IS_STRING) {
				tmp2 = **tmp;
				zval_copy_ctor(&tmp2);
				convert_to_string(&tmp2);
				if (PG(html_errors)) {
					if (Z_STRLEN(tmp2) == 0) {
						PUTS("<i>no value</i>");
					} else {
						elem_esc = php_info_html_esc(Z_STRVAL(tmp2) TSRMLS_CC);
						PUTS(elem_esc);
						efree(elem_esc);
					} 
				} else {
					PUTS(Z_STRVAL(tmp2));
				}	
				zval_dtor(&tmp2);
			} else {
				if (PG(html_errors)) {
					if (Z_STRLEN_PP(tmp) == 0) {
						PUTS("<i>no value</i>");
					} else {
						elem_esc = php_info_html_esc(Z_STRVAL_PP(tmp) TSRMLS_CC);
						PUTS(elem_esc);
						efree(elem_esc);
					}
				} else {
					PUTS(Z_STRVAL_PP(tmp));
				}	
			}
			if (PG(html_errors)) {
				PUTS("</td></tr>\n");
			} else {
				PUTS("\n");
			}	
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
	php_info_print_css();
	php_printf("//--></style>\n");
}
/* }}} */


/* {{{ php_info_html_esc
 */
PHPAPI char *php_info_html_esc(char *string TSRMLS_DC)
{
	int new_len;
	return php_escape_html_entities(string, strlen(string), &new_len, 0, ENT_NOQUOTES, NULL TSRMLS_CC);
}
/* }}} */


/* {{{ php_get_uname
 */
PHPAPI char *php_get_uname(char mode)
{
	char *php_uname;
	char tmp_uname[256];
#ifdef PHP_WIN32
	DWORD dwBuild=0;
	DWORD dwVersion = GetVersion();
	DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));

	if (mode == 's') {
		if (dwVersion < 0x80000000) {
			php_uname = "Windows NT";
		} else {
			php_uname = "Windows 9x";
		}
	} else if (mode == 'r') {
		snprintf(tmp_uname, sizeof(tmp_uname), "%d.%d", dwWindowsMajorVersion, dwWindowsMinorVersion);
		php_uname = tmp_uname;
	} else if (mode == 'n') {
		/* XXX HOW TO GET THIS ON WINDOWS? */
		php_uname = "localhost";
	} else if (mode == 'v') {
		dwBuild = (DWORD)(HIWORD(dwVersion));
		snprintf(tmp_uname, sizeof(tmp_uname), "build %d", dwBuild);
		php_uname = tmp_uname;
	} else if (mode == 'm') {
		/* XXX HOW TO GET THIS ON WINDOWS? */
		php_uname = "i386";
	} else { /* assume mode == 'a' */
		/* Get build numbers for Windows NT or Win95 */
		if (dwVersion < 0x80000000){
			dwBuild = (DWORD)(HIWORD(dwVersion));
			snprintf(tmp_uname, sizeof(tmp_uname), "%s %s %d.%d build %d",
					 "Windows NT", "localhost",
					 dwWindowsMajorVersion, dwWindowsMinorVersion, dwBuild);
		} else {
			snprintf(tmp_uname, sizeof(tmp_uname), "%s %s %d.%d",
					 "Windows 9x", "localhost",
					 dwWindowsMajorVersion, dwWindowsMinorVersion);
		}
		php_uname = tmp_uname;
	}
#else
#ifdef HAVE_SYS_UTSNAME_H
	struct utsname buf;
	if (uname((struct utsname *)&buf) == -1) {
		php_uname = PHP_UNAME;
	} else {
		if (mode == 's') {
			php_uname = buf.sysname;
		} else if (mode == 'r') {
			php_uname = buf.release;
		} else if (mode == 'n') {
			php_uname = buf.nodename;
		} else if (mode == 'v') {
			php_uname = buf.version;
		} else if (mode == 'm') {
			php_uname = buf.machine;
		} else { /* assume mode == 'a' */
			snprintf(tmp_uname, sizeof(tmp_uname), "%s %s %s %s %s",
					 buf.sysname, buf.nodename, buf.release, buf.version,
					 buf.machine);
			php_uname = tmp_uname;
		}
	}
#else
	php_uname = PHP_UNAME;
#endif
#endif
	return estrdup(php_uname);
}
/* }}} */


/* {{{ php_print_info_htmlhead
 */
PHPAPI void php_print_info_htmlhead(TSRMLS_D)
{

/*** none of this is needed now ***

	const char *charset = NULL;

	if (SG(default_charset)) {
		charset = SG(default_charset);
	}

#if HAVE_MBSTRING
	if (php_ob_handler_used("mb_output_handler" TSRMLS_CC)) {
		if (MBSTRG(current_http_output_encoding) == mbfl_no_encoding_pass) {
			charset = "US-ASCII";
		} else {
			charset = mbfl_no2preferred_mime_name(MBSTRG(current_http_output_encoding));
		}
	}
#endif   

#if HAVE_ICONV
	if (php_ob_handler_used("ob_iconv_handler" TSRMLS_CC)) {
		charset = ICONVG(output_encoding);
	}
#endif

	if (!charset || !charset[0]) {
		charset = "US-ASCII";
	}

*** none of that is needed now ***/


	PUTS("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"DTD/xhtml1-transitional.dtd\">\n");
	PUTS("<html>");
	PUTS("<head>\n");
	php_info_print_style();
	PUTS("<title>phpinfo()</title>");
/*
	php_printf("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=%s\" />\n", charset);
*/
	PUTS("</head>\n");
	PUTS("<body><div class=\"center\">\n");
}
/* }}} */

/* {{{ module_name_cmp */
static int module_name_cmp(const void *a, const void *b TSRMLS_DC)
{
	Bucket *f = *((Bucket **) a);
	Bucket *s = *((Bucket **) b);

	return strcmp(((zend_module_entry *)f->pData)->name,
				  ((zend_module_entry *)s->pData)->name);
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

	if (PG(html_errors)) {
		php_print_info_htmlhead(TSRMLS_C);
	} else {
		PUTS("phpinfo()\n");
	}	

	if (flag & PHP_INFO_GENERAL) {
		char *zend_version = get_zend_version();
		char temp_api[9];

		php_uname = php_get_uname('a');
		
		if (PG(html_errors)) {
			php_info_print_box_start(1);
		}

		if (expose_php && PG(html_errors)) {
			PUTS("<a href=\"http://www.php.net/\"><img border=\"0\" src=\"");
			if (SG(request_info).request_uri) {
				PUTS(SG(request_info).request_uri);
			}
			if ((ta->tm_mon==3) && (ta->tm_mday==1)) {
				PUTS("?="PHP_EGG_LOGO_GUID"\" alt=\"Thies!\" /></a>");
			} else {
				PUTS("?="PHP_LOGO_GUID"\" alt=\"PHP Logo\" /></a>");
			}
		}

		if (PG(html_errors)) {
			php_printf("<h1 class=\"p\">PHP Version %s</h1>\n", PHP_VERSION);
		} else {
			php_info_print_table_row(2, "PHP Version", PHP_VERSION);
		}	
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

		if (strlen(PHP_CONFIG_FILE_SCAN_DIR)) {
			php_info_print_table_row(2, "Scan this dir for additional .ini files", PHP_CONFIG_FILE_SCAN_DIR);
			if (php_ini_scanned_files) {
				php_info_print_table_row(2, "additional .ini files parsed", php_ini_scanned_files);
			}
		}
		
		snprintf(temp_api, sizeof(temp_api), "%d", PHP_API_VERSION);
		php_info_print_table_row(2, "PHP API", temp_api);

		snprintf(temp_api, sizeof(temp_api), "%d", ZEND_MODULE_API_NO);
		php_info_print_table_row(2, "PHP Extension", temp_api);

		snprintf(temp_api, sizeof(temp_api), "%d", ZEND_EXTENSION_API_NO);
		php_info_print_table_row(2, "Zend Extension", temp_api);

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

		{
			HashTable *url_stream_wrappers_hash;
			char *stream_protocol, *stream_protocols_buf = NULL;
			int stream_protocol_len, stream_protocols_buf_len = 0;

			if ((url_stream_wrappers_hash = php_stream_get_url_stream_wrappers_hash())) {
				for (zend_hash_internal_pointer_reset(url_stream_wrappers_hash);
						zend_hash_get_current_key_ex(url_stream_wrappers_hash, &stream_protocol, &stream_protocol_len, NULL, 0, NULL) == HASH_KEY_IS_STRING;
						zend_hash_move_forward(url_stream_wrappers_hash)) {
					if (NULL == (stream_protocols_buf = erealloc(stream_protocols_buf,
									stream_protocols_buf_len + stream_protocol_len + 2 /* ", " */ + 1 /* 0 byte at end */))) {
						break;
					}
					memcpy(stream_protocols_buf + stream_protocols_buf_len, stream_protocol, stream_protocol_len);
					stream_protocols_buf[stream_protocols_buf_len + stream_protocol_len] = ',';
					stream_protocols_buf[stream_protocols_buf_len + stream_protocol_len + 1] = ' ';
					stream_protocols_buf_len += stream_protocol_len + 2;
				}
				if (stream_protocols_buf) {
					stream_protocols_buf[stream_protocols_buf_len - 2] = ' ';
					stream_protocols_buf[stream_protocols_buf_len] = 0;
					php_info_print_table_row(2, "Registered PHP Streams", stream_protocols_buf);
					efree(stream_protocols_buf);
				} else {
					/* Any chances we will ever hit this? */
					php_info_print_table_row(2, "Registered PHP Streams", "no streams registered");
				}
			} else {
				/* Any chances we will ever hit this? */
				php_info_print_table_row(2, "PHP Streams", "disabled"); /* ?? */
			}
		}
		
		php_info_print_table_end();

		/* Zend Engine */
		php_info_print_box_start(0);
		if (expose_php && PG(html_errors)) {
			PUTS("<a href=\"http://www.zend.com/\"><img border=\"0\" src=\"");
			if (SG(request_info).request_uri) {
				PUTS(SG(request_info).request_uri);
			}
			PUTS("?="ZEND_LOGO_GUID"\" alt=\"Zend logo\" /></a>\n");
		}
		PUTS("This program makes use of the Zend Scripting Language Engine:");
		PUTS(PG(html_errors)?"<br />":"\n");
		PUTS(zend_version);
		php_info_print_box_end();
		efree(php_uname);
	}

	if ((flag & PHP_INFO_CREDITS) && expose_php && PG(html_errors)) {	
		php_info_print_hr();
		PUTS("<h1><a href=\"");
		if (SG(request_info).request_uri) {
			PUTS(SG(request_info).request_uri);
		}
		PUTS("?=PHPB8B5F2A0-3C92-11d3-A3A9-4C7B08C10000\">");
		PUTS("PHP Credits");
		PUTS("</a></h1>\n");
	}

	zend_ini_sort_entries(TSRMLS_C);

	if (flag & PHP_INFO_CONFIGURATION) {
		php_info_print_hr();
		if (PG(html_errors)) {
			PUTS("<h1>Configuration</h1>\n");
		} else {
			SECTION("Configuration");
		}	
		SECTION("PHP Core");
		display_ini_entries(NULL);
	}

	if (flag & PHP_INFO_MODULES) {
		int show_info_func;
		HashTable sorted_registry;
		zend_module_entry tmp;

		zend_hash_init(&sorted_registry, 50, NULL, NULL, 1);
		zend_hash_copy(&sorted_registry, &module_registry, NULL, &tmp, sizeof(zend_module_entry));
		zend_hash_sort(&sorted_registry, zend_qsort, module_name_cmp, 0 TSRMLS_CC);

		show_info_func = 1;
		zend_hash_apply_with_argument(&sorted_registry, (apply_func_arg_t) _display_module_info, &show_info_func TSRMLS_CC);

		SECTION("Additional Modules");
		php_info_print_table_start();
		php_info_print_table_header(1, "Module Name");
		show_info_func = 0;
		zend_hash_apply_with_argument(&sorted_registry, (apply_func_arg_t) _display_module_info, &show_info_func TSRMLS_CC);
		php_info_print_table_end();

		zend_hash_destroy(&sorted_registry);
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
		if (PG(html_errors)) {
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
		} else {
			PUTS("\nPHP License\n");
			PUTS("This program is free software; you can redistribute it and/or modify\n");
			PUTS("it under the terms of the PHP License as published by the PHP Group\n");
			PUTS("and included in the distribution in the file:  LICENSE\n");
			PUTS("\n");
			PUTS("This program is distributed in the hope that it will be useful,\n");
			PUTS("but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
			PUTS("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
			PUTS("\n");
			PUTS("If you did not receive a copy of the PHP license, or have any\n");
			PUTS("questions about PHP licensing, please contact license@php.net.\n");
		}
	}
	if (PG(html_errors)) {
		PUTS("</div></body></html>");
	}	
}
/* }}} */


PHPAPI void php_info_print_table_start()
{
	TSRMLS_FETCH();

	if (PG(html_errors)) {
		php_printf("<table border=\"0\" cellpadding=\"3\" width=\"600\">\n");
	} else {
		php_printf("\n");
	}	
}

PHPAPI void php_info_print_table_end()
{
	TSRMLS_FETCH();

	if (PG(html_errors)) {
		php_printf("</table><br />\n");
	}

}

PHPAPI void php_info_print_box_start(int flag)
{
	TSRMLS_FETCH();

	php_info_print_table_start();
	if (flag) {
		if (PG(html_errors)) {
			php_printf("<tr class=\"h\"><td>\n");
		}
	} else {
		if (PG(html_errors)) {
			php_printf("<tr class=\"v\"><td>\n");
		} else {
			php_printf("\n");
		}	
	}
}

PHPAPI void php_info_print_box_end()
{
	TSRMLS_FETCH();

	if (PG(html_errors)) {
		php_printf("</td></tr>\n");
	}
	php_info_print_table_end();
}

PHPAPI void php_info_print_hr()
{
	TSRMLS_FETCH();

	if (PG(html_errors)) {
		php_printf("<hr />\n");
	} else {
		php_printf("\n\n _______________________________________________________________________\n\n");
	}
}

PHPAPI void php_info_print_table_colspan_header(int num_cols, char *header)
{
	int spaces;

	TSRMLS_FETCH();
	
	if (PG(html_errors)) {
		php_printf("<tr class=\"h\"><th colspan=\"%d\">%s</th></tr>\n", num_cols, header );
	} else {
		spaces = (74 - strlen(header));
		php_printf("%*s%s%*s\n", (int)(spaces/2), " ", header, (int)(spaces/2), " ");
	}	
}

/* {{{ php_info_print_table_header
 */
PHPAPI void php_info_print_table_header(int num_cols, ...)
{
	int i;
	va_list row_elements;
	char *row_element;

	TSRMLS_FETCH();

	va_start(row_elements, num_cols);
	if (PG(html_errors)) {
		php_printf("<tr class=\"h\">");
	}	
	for (i=0; i<num_cols; i++) {
		row_element = va_arg(row_elements, char *);
		if (!row_element || !*row_element) {
			row_element = " ";
		}
		if (PG(html_errors)) {
			PUTS("<th>");
			PUTS(row_element);
			PUTS("</th>");
		} else {
			PUTS(row_element);
			if (i < num_cols-1) {
				PUTS(" => ");
			} else {
				PUTS("\n");
			}	
		}	
	}
	if (PG(html_errors)) {
		php_printf("</tr>\n");
	}	

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
	char *elem_esc = NULL;
/*
	int elem_esc_len;
*/

	TSRMLS_FETCH();

	va_start(row_elements, num_cols);
	if (PG(html_errors)) {
		php_printf("<tr>");
	}	
	for (i=0; i<num_cols; i++) {
		if (PG(html_errors)) {
			php_printf("<td class=\"%s\">",
			   (i==0 ? "e" : "v" )
			);
		}	
		row_element = va_arg(row_elements, char *);
		if (!row_element || !*row_element) {
			if (PG(html_errors)) {
				PUTS( "<i>no value</i>" );
			} else {
				PUTS( " " );
			}
		} else {
			if (PG(html_errors)) {
				elem_esc = php_info_html_esc(row_element TSRMLS_CC);
				PUTS(elem_esc);
				efree(elem_esc);
			} else {
				PUTS(row_element);
				if (i < num_cols-1) {
					PUTS(" => ");
				} else {
					PUTS("\n");
				}	
			}
		}
		if (PG(html_errors)) {
			php_printf(" </td>");
		}	
	}
	if (PG(html_errors)) {
		php_printf("</tr>\n");
	}
	
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

	/* Andale!  Andale!  Yee-Hah! */
	php_start_ob_buffer(NULL, 4096, 0 TSRMLS_CC);
	php_print_info(flag TSRMLS_CC);
	php_end_ob_buffer(1, 0 TSRMLS_CC);

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
	char *mode = "a";
	int modelen;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &mode, &modelen) == FAILURE) {
		return;
	}
	RETURN_STRING(php_get_uname(*mode), 0);
}

/* }}} */

/* {{{ proto string php_ini_scanned_files(void)
   Return comma-separated string of .ini files parsed from the additional ini dir */
PHP_FUNCTION(php_ini_scanned_files)
{
	if (strlen(PHP_CONFIG_FILE_SCAN_DIR)) {
		RETURN_STRING(php_ini_scanned_files, 1);
	} else {
		RETURN_FALSE;
	}
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
