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

#include "php.h"
#include "php_ini.h"
#include "php_globals.h"
#include "ext/standard/head.h"
#include "info.h"
#include "SAPI.h"
#ifndef MSVC5
#include "build-defs.h"
#endif
#include "zend_globals.h"		/* needs ELS */


#define PHP3_CONF_LONG(directive,value1,value2) \
	php3_printf("<tr><td bgcolor=\"" PHP_ENTRY_NAME_COLOR "\">%s</td><td bgcolor=\"" PHP_CONTENTS_COLOR "\">%ld</td><td bgcolor=\"" PHP_CONTENTS_COLOR "\">%ld</td></tr>\n",directive,value1,value2);

#define SECTION(name)  PUTS("<hr><h2>" name "</h2>\n")

#define CREDIT_LINE(module, authors) php_info_print_table_row(2, module, authors)


static int _display_module_info(php3_module_entry *module)
{
	if (module->info_func) {
		php3_printf("<hr><h2>%s</h2>\n", module->name);
		module->info_func(module);
	}
	return 0;
}


PHPAPI void _php3_info(int flag)
{
	char **env,*tmp1,*tmp2;
	char *php3_uname;
#if WIN32|WINNT
	char php3_windows_uname[256];
	DWORD dwBuild=0;
	DWORD dwVersion = GetVersion();
	DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));
#endif
	ELS_FETCH();
	PLS_FETCH();
	SLS_FETCH();
	
	if (flag & PHP_INFO_GENERAL) {
#if WIN32|WINNT
		// Get build numbers for Windows NT or Win95
		if (dwVersion < 0x80000000){
			dwBuild = (DWORD)(HIWORD(dwVersion));
			snprintf(php3_windows_uname,255,"%s %d.%d build %d","Windows NT",dwWindowsMajorVersion,dwWindowsMinorVersion,dwBuild);
		} else {
			snprintf(php3_windows_uname,255,"%s %d.%d","Windows 95/98",dwWindowsMajorVersion,dwWindowsMinorVersion);
		}
		php3_uname = php3_windows_uname;
#else
		php3_uname=PHP_UNAME;
#endif


		php3_printf("<center><h1>PHP Version %s</h1></center>\n", PHP_VERSION);

		PUTS("<hr><a href=\"http://www.php.net/\"><img src=\"");
		if (SG(request_info).request_uri) {
			PUTS(SG(request_info).request_uri);
		}
		PUTS("?=PHPE9568F34-D428-11d2-A769-00AA001ACF42\" border=\"0\" align=\"right\"></a>\n");
		php3_printf("System: %s<br>Build Date: %s\n<br>", php3_uname, __DATE__);
		php3_printf("php.ini path:  %s<br>\n", CONFIGURATION_FILE_PATH);

		php3_printf("ZEND_DEBUG=%d<br>\n", ZEND_DEBUG);
#ifdef ZTS
		php3_printf("ZTS is defined");
#else
		php3_printf("ZTS is undefined");
#endif
		/* Zend Engine */
		PUTS("<hr><a href=\"http://www.zend.com/\"><img src=\"");
		if (SG(request_info).request_uri) {
			PUTS(SG(request_info).request_uri);
		}
		PUTS("?=PHPE9568F35-D428-11d2-A769-00AA001ACF42\" border=\"0\" align=\"right\"></a>\n");
		php3_printf("This program makes use of the Zend scripting language engine:<br><pre>%s</pre>", get_zend_version());
		PUTS("<hr>");
	}

	PUTS("<center>");

	if (flag & PHP_INFO_CREDITS) {	
		PUTS("<h1>Credits</h1>\n");

		PUTS("<table border=5 width=\"600\">\n");
		PUTS("<tr><th colspan=\"2\" bgcolor=\"" PHP_HEADER_COLOR "\">PHP 4.0 Authors</th></tr>\n");
		php_info_print_table_header(2, "Contribution", "Authors");
		CREDIT_LINE("Zend Scripting Language Engine", "Andi Gutmans, Zeev Suraski");
		CREDIT_LINE("Extension Module API", "Andi Gutmans, Zeev Suraski");
		CREDIT_LINE("UNIX Build and Modularization", "Stig Bakken");
		CREDIT_LINE("Win32 Port", "Shane Caraveo, Zeev Suraski");
		CREDIT_LINE("Server API (SAPI) Abstraction Layer", "Andi Gutmans, Shane Caraveo, Zeev Suraski");
		CREDIT_LINE("Apache SAPI Module", "Rasmus Lerdorf, Zeev Suraski");
		CREDIT_LINE("ISAPI SAPI Module", "Andi Gutmans, Zeev Suraski");
		CREDIT_LINE("CGI SAPI Module", "Rasmus Lerdorf, Stig Bakken");
		PUTS("</table>\n");
	}


	if (flag & PHP_INFO_CONFIGURATION) {
		PUTS("<hr><h1>Configuration</h1>\n");
		PUTS("<h2>PHP Core</h2>\n");
		display_ini_entries(NULL);
	}

	if (flag & PHP_INFO_MODULES) {
		_php3_hash_apply(&module_registry,(int (*)(void *)) _display_module_info);
	}

	if (flag & PHP_INFO_ENVIRONMENT) {
		SECTION("Environment");
		PUTS("<table border=5 width=\"600\">\n");
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
		PUTS("</table>\n");
	}

	if (flag & PHP_INFO_VARIABLES) {
		pval **data, **tmp;
		char *string_key;
		ulong num_key;

		SECTION("PHP Variables");

		PUTS("<table border=5 width=\"600\">\n");
		php_info_print_table_header(2, "Variable", "Value");
		if (_php3_hash_find(&EG(symbol_table), "PHP_SELF", sizeof("PHP_SELF"), (void **) &data) != FAILURE) {
			php_info_print_table_row(2, "PHP_SELF", (*data)->value.str.val);
		}
		if (_php3_hash_find(&EG(symbol_table), "PHP_AUTH_TYPE", sizeof("PHP_AUTH_TYPE"), (void **) &data) != FAILURE) {
			php_info_print_table_row(2, "PHP_AUTH_TYPE", (*data)->value.str.val);
		}
		if (_php3_hash_find(&EG(symbol_table), "PHP_AUTH_USER", sizeof("PHP_AUTH_USER"), (void **) &data) != FAILURE) {
			php_info_print_table_row(2, "PHP_AUTH_USER", (*data)->value.str.val);
		}
		if (_php3_hash_find(&EG(symbol_table), "PHP_AUTH_PW", sizeof("PHP_AUTH_PW"), (void **) &data) != FAILURE) {
			php_info_print_table_row(2, "PHP_AUTH_PW", (*data)->value.str.val);
		}
		if (_php3_hash_find(&EG(symbol_table), "HTTP_GET_VARS", sizeof("HTTP_GET_VARS"), (void **) &data) != FAILURE) {
			_php3_hash_internal_pointer_reset((*data)->value.ht);
			while (_php3_hash_get_current_data((*data)->value.ht, (void **) &tmp) == SUCCESS) {
				convert_to_string(*tmp);
				PUTS("<tr><td bgcolor=\"" PHP_ENTRY_NAME_COLOR "\"><b>HTTP_GET_VARS[\"");
				switch (_php3_hash_get_current_key((*data)->value.ht, &string_key, &num_key)) {
					case HASH_KEY_IS_STRING:
						PUTS(string_key);
						efree(string_key);
						break;
					case HASH_KEY_IS_LONG:
						php3_printf("%ld",num_key);
						break;
				}
				PUTS("\"]</b></td><td bgcolor=\"" PHP_CONTENTS_COLOR "\">");
				PUTS((*tmp)->value.str.val); /* This could be "Array" - too ugly to expand that for now */
				PUTS("</td></tr>\n");
				_php3_hash_move_forward((*data)->value.ht);
			}
		}
		if (_php3_hash_find(&EG(symbol_table), "HTTP_POST_VARS", sizeof("HTTP_POST_VARS"), (void **) &data) != FAILURE) {
			_php3_hash_internal_pointer_reset((*data)->value.ht);
			while (_php3_hash_get_current_data((*data)->value.ht, (void **) &tmp) == SUCCESS) {
				convert_to_string(*tmp);
				PUTS("<tr><td bgcolor=\"" PHP_ENTRY_NAME_COLOR "\"><b>HTTP_POST_VARS[\"");
				switch (_php3_hash_get_current_key((*data)->value.ht, &string_key, &num_key)) {
					case HASH_KEY_IS_STRING:
						PUTS(string_key);
						efree(string_key);
						break;
					case HASH_KEY_IS_LONG:
						php3_printf("%ld",num_key);
						break;
				}
				PUTS("\"]</b></td><td bgcolor=\"" PHP_CONTENTS_COLOR "\">");
				PUTS((*tmp)->value.str.val);
				PUTS("</td></tr>\n");
				_php3_hash_move_forward((*data)->value.ht);
			}
		}
		if (_php3_hash_find(&EG(symbol_table), "HTTP_COOKIE_VARS", sizeof("HTTP_COOKIE_VARS"), (void **) &data) != FAILURE) {
			_php3_hash_internal_pointer_reset((*data)->value.ht);
			while (_php3_hash_get_current_data((*data)->value.ht, (void **) &tmp) == SUCCESS) {
				convert_to_string(*tmp);
				PUTS("<tr><td bgcolor=\"" PHP_ENTRY_NAME_COLOR "\"><b>HTTP_COOKIE_VARS[\"");
				switch (_php3_hash_get_current_key((*data)->value.ht, &string_key, &num_key)) {
					case HASH_KEY_IS_STRING:
						PUTS(string_key);
						efree(string_key);
						break;
					case HASH_KEY_IS_LONG:
						php3_printf("%ld",num_key);
						break;
				}
				PUTS("\"]</b></td><td bgcolor=\"" PHP_CONTENTS_COLOR "\">");
				PUTS((*tmp)->value.str.val);
				PUTS("</td></tr>\n");
				_php3_hash_move_forward((*data)->value.ht);
			}
		}
		PUTS("</table>\n");
	}

	PUTS("</center>");


	if (flag & PHP_INFO_LICENSE) {
		SECTION("PHP License");
		PUTS("<PRE>This program is free software; you can redistribute it and/or modify\n");
		PUTS("it under the terms of the PHP License as published by the PHP Development Team\n");
		PUTS("and included in the distribution in the file:  LICENSE\n");
		PUTS("\n");
		PUTS("This program is distributed in the hope that it will be useful,\n");
		PUTS("but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
		PUTS("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
		PUTS("\n");
		PUTS("If you did not receive a copy of the PHP license, or have any questions about\n");
		PUTS("PHP licensing, please contact license@php.net.</PRE>\n");
	}
}


PHPAPI void php_info_print_table_header(int num_cols, ...)
{
	int i;
	va_list row_elements;
	char *row_element;

	va_start(row_elements, num_cols);

	php3_printf("<tr>");
	for (i=0; i<num_cols; i++) {
		row_element = va_arg(row_elements, char *);
		if (!row_element || !*row_element) {
			row_element = "&nbsp;";
		}
		php3_printf("<th bgcolor=\"" PHP_HEADER_COLOR "\" valign=\"top\">%s</th>", row_element);
	}
	php3_printf("</tr>\n");

	va_end(row_elements);
}


PHPAPI void php_info_print_table_row(int num_cols, ...)
{
	int i;
	va_list row_elements;
	char *color = PHP_ENTRY_NAME_COLOR;
	char *row_element;

	va_start(row_elements, num_cols);

	php3_printf("<tr>");
	for (i=0; i<num_cols; i++) {
		row_element = va_arg(row_elements, char *);
		if (!row_element || !*row_element) {
			row_element = "&nbsp;";
		}
		php3_printf("<td bgcolor=\"%s\" valign=\"top\">%s%s%s</td>", 
			color, (i==0?"<b>":""), row_element, (i==0?"</b>":""));
		color = PHP_CONTENTS_COLOR;
	}
	php3_printf("</tr>\n");

	va_end(row_elements);
}



void register_phpinfo_constants(INIT_FUNC_ARGS)
{
	ELS_FETCH();

	REGISTER_LONG_CONSTANT("INFO_GENERAL", PHP_INFO_GENERAL, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("INFO_CREDITS", PHP_INFO_CREDITS, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("INFO_CONFIGURATION", PHP_INFO_CONFIGURATION, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("INFO_MODULES", PHP_INFO_MODULES, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("INFO_ENVIRONMENT", PHP_INFO_ENVIRONMENT, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("INFO_VARIABLES", PHP_INFO_VARIABLES, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("INFO_LICENSE", PHP_INFO_LICENSE, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("INFO_ALL", PHP_INFO_ALL, CONST_PERSISTENT|CONST_CS);
}


/* {{{ proto void phpinfo(void)
   Output a page of useful information about PHP and the current request */
PHP_FUNCTION(info)
{
	int flag;
	zval *flag_arg;


	switch (ARG_COUNT(ht)) {
		case 0:
			flag = 0xFFFFFFFF;
			break;
		case 1:
			if (getParameters(ht, 1, &flag_arg)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(flag_arg);
			flag = flag_arg->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	_php3_info(flag);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string phpversion(void)
   Return the current PHP version */
PHP_FUNCTION(version)
{
    RETURN_STRING(PHP_VERSION,1);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
