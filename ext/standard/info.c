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


#define PHP3_CONF_STR(directive,value1,value2) \
	PUTS("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">"); \
	PUTS(directive); \
	PUTS("</td><td bgcolor=\"" CONTENTS_COLOR "\">&nbsp;"); \
	if (value1) PUTS(value1); \
	else PUTS("<i>none</i>"); \
	PUTS("</td><td bgcolor=\"" CONTENTS_COLOR "\">&nbsp;"); \
	if (value2) PUTS(value2); \
	else PUTS("<i>none</i>"); \
	PUTS("</td></tr>\n");

#define PHP3_CONF_LONG(directive,value1,value2) \
	php3_printf("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">%s</td><td bgcolor=\"" CONTENTS_COLOR "\">%ld</td><td bgcolor=\"" CONTENTS_COLOR "\">%ld</td></tr>\n",directive,value1,value2);

#define SECTION(name)  PUTS("<hr><h2>" name "</h2>\n")

#define ENTRY_NAME_COLOR "#999999"
#define CONTENTS_COLOR "#DDDDDD"
#define HEADER_COLOR "#00DDDD"

static int _display_module_info(php3_module_entry *module)
{
	PUTS("<tr><th align=left bgcolor=\"" ENTRY_NAME_COLOR "\">");
	PUTS(module->name);
	PUTS("</th><td bgcolor=\"" CONTENTS_COLOR "\">");
	if (module->info_func) {
		module->info_func();
	} else {
		PUTS("No additional information.");
	}
	PUTS("</td></tr>\n");
	return 0;
}


PHPAPI void _php3_info(void)
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


	PUTS("<img src=\"");
	if (SG(request_info).request_uri) {
		PUTS(SG(request_info).request_uri);
	}
	PUTS("?=PHPE9568F34-D428-11d2-A769-00AA001ACF42\" border=\"0\" width=\"100\" height=\"56\" align=\"right\">\n");
	php3_printf("<center><h1>PHP Version %s</h1></center>\n", PHP_VERSION);
	PUTS("<p>by <a href=\"mailto:rasmus@lerdorf.on.ca\">Rasmus Lerdorf</a>,\n");
	PUTS("<a href=\"mailto:andi@zend.com\">Andi Gutmans</a>,\n");
	PUTS("<a href=\"mailto:zeev@zend.com\">Zeev Suraski</a>,\n");
	PUTS("<a href=\"mailto:ssb@guardian.no\">Stig Bakken</a>,\n");
	PUTS("<a href=\"mailto:shane@caraveo.com\">Shane Caraveo</a>,\n");
	PUTS("<a href=\"mailto:jimw@php.net\">Jim Winstead</a>, and countless others.</P>\n");

	PUTS("<P><a href=\"http://www.zend.com/\">Zend</a>:</a>  ");
	PUTS("<a href=\"mailto:andi@zend.com\">Andi Gutmans</a> and \n");
	PUTS("<a href=\"mailto:zeev@zend.com\">Zeev Suraski</a></p>\n");

	PUTS("<hr>");
	php3_printf("<center>System: %s<br>Build Date: %s</center>\n", php3_uname, __DATE__);
	PUTS("<center>\n");
	
	SECTION("Extensions");
	PUTS("<table border=5 width=\"600\">\n");
	PUTS("<tr><th bgcolor=\"" HEADER_COLOR "\">Extensions</th><th bgcolor=\"" HEADER_COLOR "\">Additional Information</th></tr>\n");
	
#ifndef MSVC5
	PUTS("<tr><th align=left bgcolor=\"" ENTRY_NAME_COLOR "\">PHP core</th>\n");
	PUTS("<td bgcolor=\"" CONTENTS_COLOR "\"><tt>CFLAGS=" PHP_CFLAGS "<br>\n");
	PUTS("HSREGEX=" PHP_HSREGEX "</td></tr>\n");
#endif

	_php3_hash_apply(&module_registry,(int (*)(void *))_display_module_info);
	PUTS("</table>\n");

	SECTION("Configuration");
	PUTS("php3.ini file path is set to: ");
	PUTS(CONFIGURATION_FILE_PATH);
	PUTS("<br>\n");
	PUTS("<table border=5 width=\"600\">\n");
	PUTS("<tr><th bgcolor=\"" HEADER_COLOR "\">Directive</th><th bgcolor=\"" HEADER_COLOR "\">Master Value</th><th bgcolor=\"" HEADER_COLOR "\">Local Value</th></tr>\n");
	PHP3_CONF_STR("arg_separator", INI_ORIG_STR("arg_separator"), PG(arg_separator));
	PHP3_CONF_LONG("asp_tags", INI_ORIG_INT("asp_tags"), PG(asp_tags));
	PHP3_CONF_STR("auto_prepend_file", INI_ORIG_STR("auto_prepend_file"), PG(auto_prepend_file));
	PHP3_CONF_STR("auto_append_file", INI_ORIG_STR("auto_append_file"), PG(auto_append_file));
	PHP3_CONF_STR("browscap", INI_ORIG_STR("browscap"), INI_STR("browscap"));
	PHP3_CONF_LONG("define_syslog_variables", INI_ORIG_STR("define_syslog_variables"), INI_STR("define_syslog_variables"));
	PHP3_CONF_LONG("display_errors", INI_ORIG_INT("display_errors"), PG(display_errors));
	PHP3_CONF_STR("doc_root", INI_ORIG_STR("doc_root"), PG(doc_root));
	PHP3_CONF_LONG("enable_dl", INI_ORIG_INT("enable_dl"), PG(enable_dl));
	PHP3_CONF_STR("error_log", INI_ORIG_STR("error_log"), PG(error_log));
	PHP3_CONF_STR("error_prepend_string", INI_ORIG_STR("error_prepend_string"), INI_STR("error_prepend_string"));
	PHP3_CONF_STR("error_append_string", INI_ORIG_STR("error_append_string"), INI_STR("error_append_string"));
	PHP3_CONF_LONG("error_reporting", INI_ORIG_INT("error_reporting"), EG(error_reporting));
	PHP3_CONF_STR("extension_dir", INI_ORIG_STR("extension_dir"), PG(extension_dir));
	PHP3_CONF_STR("gpc_order", INI_ORIG_STR("gpc_order"), PG(gpc_order));
	PHP3_CONF_STR("include_path", INI_ORIG_STR("include_path"), PG(include_path));
	PHP3_CONF_LONG("log_errors", INI_ORIG_INT("log_errors"), PG(log_errors));
	PHP3_CONF_LONG("max_execution_time", INI_ORIG_INT("max_execution_time"), PG(max_execution_time));
	PHP3_CONF_LONG("magic_quotes_gpc", INI_ORIG_INT("magic_quotes_gpc"), PG(magic_quotes_gpc));
	PHP3_CONF_LONG("magic_quotes_runtime", INI_ORIG_INT("magic_quotes_runtime"), PG(magic_quotes_runtime));
	PHP3_CONF_LONG("magic_quotes_sybase", INI_ORIG_INT("magic_quotes_sybase"), PG(magic_quotes_sybase));
	PHP3_CONF_LONG("memory limit", INI_ORIG_INT("memory_limit"), PG(memory_limit));
	PHP3_CONF_STR("open_basedir", INI_ORIG_STR("open_basedir"), PG(open_basedir));
	PHP3_CONF_LONG("precision", INI_ORIG_INT("precision"), EG(precision));
	PHP3_CONF_LONG("safe_mode", INI_ORIG_INT("safe_mode"), PG(safe_mode));
	PHP3_CONF_STR("safe_mode_exec_dir", INI_ORIG_STR("safe_mode_exec_dir"), PG(safe_mode_exec_dir));
	PHP3_CONF_STR("sendmail_from", INI_ORIG_STR("sendmail_from"), INI_STR("sendmail_from"));
	PHP3_CONF_STR("sendmail_path", INI_ORIG_STR("sendmail_path"), INI_STR("sendmail_path"));
	PHP3_CONF_LONG("short_open_tag", INI_ORIG_INT("short_open_tag"), PG(short_tags));
	PHP3_CONF_STR("SMTP", INI_ORIG_STR("SMTP"), INI_STR("SMTP"));
	PHP3_CONF_LONG("sql_safe_mode", INI_ORIG_INT("sql.safe_mode"), PG(sql_safe_mode));
	PHP3_CONF_LONG("track_errors", INI_ORIG_INT("track_errors"), PG(track_errors));
	PHP3_CONF_LONG("track_vars", INI_ORIG_INT("track_vars"), PG(track_vars));
	PHP3_CONF_LONG("upload_max_filesize", INI_ORIG_INT("upload_max_filesize"), PG(upload_max_filesize));
	PHP3_CONF_STR("upload_tmp_dir", INI_ORIG_STR("upload_tmp_dir"), PG(upload_tmp_dir));
	PHP3_CONF_STR("user_dir", INI_ORIG_STR("user_dir"), PG(user_dir));
	PHP3_CONF_LONG("y2k_compliance", INI_ORIG_INT("y2k_compliance"), PG(y2k_compliance));
	/* apache only directives */
	PHP3_CONF_LONG("engine", INI_ORIG_INT("engine"), INI_INT("engine")); /* apache only */
	PHP3_CONF_LONG("xbithack", INI_ORIG_INT("xbithack"), INI_INT("xbithack"));	/* apache only */
	PHP3_CONF_LONG("last_modified", INI_ORIG_INT("last_modified"), INI_INT("last_modified"));  /* apache only */
	/* end of apache only directives */

	/* And now for the highlight colours */
	php3_printf("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">highlight_comment</td><td bgcolor=\"" CONTENTS_COLOR "\"><font color=%s>&nbsp;%s</font></td><td bgcolor=\"" CONTENTS_COLOR "\"><font color=%s>&nbsp;%s</font></td></tr>\n",INI_ORIG_STR("highlight.comment"), INI_ORIG_STR("highlight.comment"), INI_STR("highlight.comment"), INI_STR("highlight.comment"));
	php3_printf("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">highlight_default</td><td bgcolor=\"" CONTENTS_COLOR "\"><font color=%s>&nbsp;%s</font></td><td bgcolor=\"" CONTENTS_COLOR "\"><font color=%s>&nbsp;%s</font></td></tr>\n",INI_ORIG_STR("highlight.default"), INI_ORIG_STR("highlight.default"), INI_STR("highlight.default"), INI_STR("highlight.default"));
	php3_printf("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">highlight_html</td><td bgcolor=\"" CONTENTS_COLOR "\"><font color=%s>&nbsp;%s</font></td><td bgcolor=\"" CONTENTS_COLOR "\"><font color=%s>&nbsp;%s</font></td></tr>\n",INI_ORIG_STR("highlight.html"), INI_ORIG_STR("highlight.html"), INI_STR("highlight.html"), INI_STR("highlight.html"));
	php3_printf("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">highlight_string</td><td bgcolor=\"" CONTENTS_COLOR "\"><font color=%s>&nbsp;%s</font></td><td bgcolor=\"" CONTENTS_COLOR "\"><font color=%s>&nbsp;%s</font></td></tr>\n",INI_ORIG_STR("highlight.string"), INI_ORIG_STR("highlight.string"), INI_STR("highlight.string"), INI_STR("highlight.string"));
	php3_printf("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">highlight_bg</td><td bgcolor=\"" CONTENTS_COLOR "\"><font color=%s>&nbsp;%s</font></td><td bgcolor=\"" CONTENTS_COLOR "\"><font color=%s>&nbsp;%s</font></td></tr>\n",INI_ORIG_STR("highlight.bg"), INI_ORIG_STR("highlight.bg"), INI_STR("highlight.bg"), INI_STR("highlight.bg"));
	php3_printf("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">highlight_keyword</td><td bgcolor=\"" CONTENTS_COLOR "\"><font color=%s>&nbsp;%s</font></td><td bgcolor=\"" CONTENTS_COLOR "\"><font color=%s>&nbsp;%s</font></td></tr>\n",INI_ORIG_STR("highlight.keyword"), INI_ORIG_STR("highlight.keyword"), INI_STR("highlight.keyword"), INI_STR("highlight.keyword"));
	PUTS("</table>");

#if USE_SAPI /* call a server module specific info function */
	sapi_rqst->info(sapi_rqst);
#endif

	SECTION("Environment");
	PUTS("<table border=5 width=\"600\">\n");
	PUTS("<tr><th bgcolor=\"" HEADER_COLOR "\">Variable</th><th bgcolor=\"" HEADER_COLOR "\">Value</th></tr>\n");
	for (env=environ; env!=NULL && *env !=NULL; env++) {
		tmp1 = estrdup(*env);
		if (!(tmp2=strchr(tmp1,'='))) { /* malformed entry? */
			efree(tmp1);
			continue;
		}
		*tmp2 = 0;
		tmp2++;
		PUTS("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">");
		PUTS(tmp1);
		PUTS("</td><td bgcolor=\"" CONTENTS_COLOR "\">");
		if (tmp2 && *tmp2) {
			PUTS(tmp2);
		} else {
			PUTS("&nbsp;");
		}
		PUTS("</td></tr>\n");
		efree(tmp1);
	}
	PUTS("</table>\n");

	{
		pval **data, **tmp;
		char *string_key;
		ulong num_key;

		SECTION("PHP Variables");

		PUTS("<table border=5 width=\"600\">\n");
		PUTS("<tr><th bgcolor=\"" HEADER_COLOR "\">Variable</th><th bgcolor=\"" HEADER_COLOR "\">Value</th></tr>\n");
		if (_php3_hash_find(&EG(symbol_table), "PHP_SELF", sizeof("PHP_SELF"), (void **) &data) != FAILURE) {
			PUTS("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">");
			PUTS("PHP_SELF");
			PUTS("</td><td bgcolor=\"" CONTENTS_COLOR "\">");
			PUTS((*data)->value.str.val);
			PUTS("</td></tr>\n");
		}
		if (_php3_hash_find(&EG(symbol_table), "PHP_AUTH_TYPE", sizeof("PHP_AUTH_TYPE"), (void **) &data) != FAILURE) {
			PUTS("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">");
			PUTS("PHP_AUTH_TYPE");
			PUTS("</td><td bgcolor=\"" CONTENTS_COLOR "\">");
			PUTS((*data)->value.str.val);
			PUTS("</td></tr>\n");
		}
		if (_php3_hash_find(&EG(symbol_table), "PHP_AUTH_USER", sizeof("PHP_AUTH_USER"), (void **) &data) != FAILURE) {
			PUTS("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">");
			PUTS("PHP_AUTH_USER");
			PUTS("</td><td bgcolor=\"" CONTENTS_COLOR "\">");
			PUTS((*data)->value.str.val);
			PUTS("</td></tr>\n");
		}
		if (_php3_hash_find(&EG(symbol_table), "PHP_AUTH_PW", sizeof("PHP_AUTH_PW"), (void **) &data) != FAILURE) {
			PUTS("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">");
			PUTS("PHP_AUTH_PW");
			PUTS("</td><td bgcolor=\"" CONTENTS_COLOR "\">");
			PUTS((*data)->value.str.val);
			PUTS("</td></tr>\n");
		}
		if (_php3_hash_find(&EG(symbol_table), "HTTP_GET_VARS", sizeof("HTTP_GET_VARS"), (void **) &data) != FAILURE) {
			_php3_hash_internal_pointer_reset((*data)->value.ht);
			while (_php3_hash_get_current_data((*data)->value.ht, (void **) &tmp) == SUCCESS) {
				convert_to_string(*tmp);
				PUTS("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">HTTP_GET_VARS[\"");
				switch (_php3_hash_get_current_key((*data)->value.ht, &string_key, &num_key)) {
					case HASH_KEY_IS_STRING:
						PUTS(string_key);
						break;
					case HASH_KEY_IS_LONG:
						php3_printf("%ld",num_key);
						break;
				}
				PUTS("\"]</td><td bgcolor=\"" CONTENTS_COLOR "\">");
				PUTS((*tmp)->value.str.val); /* This could be "Array" - too ugly to expand that for now */
				PUTS("</td></tr>\n");
				_php3_hash_move_forward((*data)->value.ht);
			}
		}
		if (_php3_hash_find(&EG(symbol_table), "HTTP_POST_VARS", sizeof("HTTP_POST_VARS"), (void **) &data) != FAILURE) {
			_php3_hash_internal_pointer_reset((*data)->value.ht);
			while (_php3_hash_get_current_data((*data)->value.ht, (void **) &tmp) == SUCCESS) {
				convert_to_string(*tmp);
				PUTS("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">HTTP_POST_VARS[\"");
				switch (_php3_hash_get_current_key((*data)->value.ht, &string_key, &num_key)) {
					case HASH_KEY_IS_STRING:
						PUTS(string_key);
						break;
					case HASH_KEY_IS_LONG:
						php3_printf("%ld",num_key);
						break;
				}
				PUTS("\"]</td><td bgcolor=\"" CONTENTS_COLOR "\">");
				PUTS((*tmp)->value.str.val);
				PUTS("</td></tr>\n");
				_php3_hash_move_forward((*data)->value.ht);
			}
		}
		if (_php3_hash_find(&EG(symbol_table), "HTTP_COOKIE_VARS", sizeof("HTTP_COOKIE_VARS"), (void **) &data) != FAILURE) {
			_php3_hash_internal_pointer_reset((*data)->value.ht);
			while (_php3_hash_get_current_data((*data)->value.ht, (void **) &tmp) == SUCCESS) {
				convert_to_string(*tmp);
				PUTS("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">HTTP_COOKIE_VARS[\"");
				switch (_php3_hash_get_current_key((*data)->value.ht, &string_key, &num_key)) {
					case HASH_KEY_IS_STRING:
						PUTS(string_key);
						break;
					case HASH_KEY_IS_LONG:
						php3_printf("%ld",num_key);
						break;
				}
				PUTS("\"]</td><td bgcolor=\"" CONTENTS_COLOR "\">");
				PUTS((*tmp)->value.str.val);
				PUTS("</td></tr>\n");
				_php3_hash_move_forward((*data)->value.ht);
			}
		}
		PUTS("</table>\n");
	}

#if APACHE
	{
		register int i;
		array_header *arr;
		table_entry *elts;
		request_rec *r;
		SLS_FETCH();

		r = ((request_rec *) SG(server_context));
		arr = table_elts(r->subprocess_env);
		elts = (table_entry *)arr->elts;
		
		SECTION("Apache Environment");	
		PUTS("<table border=5 width=\"600\">\n");
		PUTS("<tr><th bgcolor=\"" HEADER_COLOR "\">Variable</th><th bgcolor=\"" HEADER_COLOR "\">Value</th></tr>\n");
		for (i=0; i < arr->nelts; i++) {
			PUTS("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">");
			PUTS(elts[i].key);
			PUTS("</td><td bgcolor=\"" CONTENTS_COLOR "\">");
			PUTS(elts[i].val);
			PUTS("&nbsp;</td></tr>\n");
		}
		PUTS("</table>\n");
	}
#endif		

#if APACHE
	{
		array_header *env_arr;
		table_entry *env;
		int i;
		request_rec *r;
		SLS_FETCH();
		
		r = ((request_rec *) SG(server_context));
		SECTION("HTTP Headers Information");
		PUTS("<table border=5 width=\"600\">\n");
		PUTS(" <tr><th colspan=2 bgcolor=\"" HEADER_COLOR "\">HTTP Request Headers</th></tr>\n");
		PUTS("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">HTTP Request</td><td bgcolor=\"" CONTENTS_COLOR "\">");
		PUTS(r->the_request);
		PUTS("&nbsp;</td></tr>\n");
		env_arr = table_elts(r->headers_in);
		env = (table_entry *)env_arr->elts;
		for (i = 0; i < env_arr->nelts; ++i) {
			if (env[i].key) {
				PUTS("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">");
				PUTS(env[i].key);
				PUTS("</td><td bgcolor=\"" CONTENTS_COLOR "\">");
				PUTS(env[i].val);
				PUTS("&nbsp;</td></tr>\n");
			}
		}
		PUTS(" <tr><th colspan=2  bgcolor=\"" HEADER_COLOR "\">HTTP Response Headers</th></tr>\n");
		env_arr = table_elts(r->headers_out);
		env = (table_entry *)env_arr->elts;
		for(i = 0; i < env_arr->nelts; ++i) {
			if (env[i].key) {
				PUTS("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">");
				PUTS(env[i].key);
				PUTS("</td><td bgcolor=\"" CONTENTS_COLOR "\">");
				PUTS(env[i].val);
				PUTS("&nbsp;</td></tr>\n");
			}
		}
		PUTS("</table>\n\n");
	}
#endif

	PUTS("</center>");

	PUTS("<hr>\n");
	PUTS("<table width=\"100%%\"><tr>\n");
	php3_printf("<td><h2>Zend</h2>This program makes use of the Zend scripting language engine:<br><pre>%s</pre></td>", get_zend_version());
	PUTS("<td width=\"100\"><a href=\"http://www.zend.com/\"><img src=\"");
	if (SG(request_info).request_uri) {
		PUTS(SG(request_info).request_uri);
	}
	PUTS("?=PHPE9568F35-D428-11d2-A769-00AA001ACF42\" border=\"0\" width=\"100\" height=\"89\"></a></td>\n");
	PUTS("</tr></table>\n");

	SECTION("PHP License");
	PUTS("<PRE>This program is free software; you can redistribute it and/or modify\n");
	PUTS("it under the terms of:\n");
	PUTS("\n");
	PUTS("A) the GNU General Public License as published by the Free Software\n");
    PUTS("   Foundation; either version 2 of the License, or (at your option)\n");
    PUTS("   any later version.\n");
	PUTS("\n");
	PUTS("B) the PHP License as published by the PHP Development Team and\n");
    PUTS("   included in the distribution in the file: LICENSE\n");
	PUTS("\n");
	PUTS("This program is distributed in the hope that it will be useful,\n");
	PUTS("but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
	PUTS("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
	PUTS("GNU General Public License for more details.\n");
	PUTS("\n");
	PUTS("You should have received a copy of both licenses referred to here.\n");
	PUTS("If you did not, or have any questions about PHP licensing, please\n");
	PUTS("contact core@php.net.</PRE>\n");
	
}

/* {{{ proto void phpinfo(void)
   Output a page of useful information about PHP and the current request */
void php3_info(INTERNAL_FUNCTION_PARAMETERS)
{
	_php3_info();
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string phpversion(void)
   Return the current PHP version */
void php3_version(INTERNAL_FUNCTION_PARAMETERS)
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
