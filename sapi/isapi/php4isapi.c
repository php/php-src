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
   | Authors: Zeev Suraski <zeev@zend.com>                                |
   |          Ben Mansell <ben@zeus.com> (Zeus Support)                   |
   +----------------------------------------------------------------------+
 */

#ifdef PHP_WIN32
# include <windows.h>
# include <process.h>
#else
# define __try
# define __except(val)
# define __declspec(foo)
#endif

#include <httpext.h>
#include <httpfilt.h>
#include <httpext.h>
#include "php.h"
#include "php_main.h"
#include "SAPI.h"
#include "php_globals.h"
#include "ext/standard/info.h"
#include "php_variables.h"
#include "php_ini.h"

#ifdef WITH_ZEUS
# include "httpext.h"
# include <errno.h>
# define GetLastError() errno
#endif

#ifdef PHP_WIN32
#define PHP_ENABLE_SEH
#endif

/* 
uncomment the following lines to turn off 
exception trapping when running under a debugger 

#ifdef _DEBUG
#undef PHP_ENABLE_SEH
#endif
*/

#define MAX_STATUS_LENGTH sizeof("xxxx LONGEST STATUS DESCRIPTION")
#define ISAPI_SERVER_VAR_BUF_SIZE 1024
#define ISAPI_POST_DATA_BUF 1024

static zend_bool bFilterLoaded=0;
static zend_bool bTerminateThreadsOnError=0;

static char *isapi_special_server_variable_names[] = {
	"ALL_HTTP",
	"HTTPS",
#ifndef WITH_ZEUS
	"SCRIPT_NAME",
#endif
	NULL
};

#define NUM_SPECIAL_VARS		(sizeof(isapi_special_server_variable_names)/sizeof(char *))
#define SPECIAL_VAR_ALL_HTTP	0
#define SPECIAL_VAR_HTTPS		1
#define SPECIAL_VAR_PHP_SELF	2

static char *isapi_server_variable_names[] = {
	"AUTH_PASSWORD",
	"AUTH_TYPE",
	"AUTH_USER",
	"CONTENT_LENGTH",
	"CONTENT_TYPE",
	"PATH_TRANSLATED",
	"QUERY_STRING",
	"REMOTE_ADDR",
	"REMOTE_HOST",
	"REMOTE_USER",
	"REQUEST_METHOD",
	"SERVER_NAME",
	"SERVER_PORT",
	"SERVER_PROTOCOL",
	"SERVER_SOFTWARE",
#ifndef WITH_ZEUS
	"APPL_MD_PATH",
	"APPL_PHYSICAL_PATH",
	"INSTANCE_ID",
	"INSTANCE_META_PATH",
	"LOGON_USER",
	"REQUEST_URI",
	"URL",
#else
	"DOCUMENT_ROOT",
#endif
	NULL
};


static char *isapi_secure_server_variable_names[] = {
	"CERT_COOKIE",
	"CERT_FLAGS",
	"CERT_ISSUER",
	"CERT_KEYSIZE",
	"CERT_SECRETKEYSIZE",
	"CERT_SERIALNUMBER",
	"CERT_SERVER_ISSUER",
	"CERT_SERVER_SUBJECT",
	"CERT_SUBJECT",
	"HTTPS_KEYSIZE",
	"HTTPS_SECRETKEYSIZE",
	"HTTPS_SERVER_ISSUER",
	"HTTPS_SERVER_SUBJECT",
	"SERVER_PORT_SECURE",
#ifdef WITH_ZEUS
	"SSL_CLIENT_CN",
	"SSL_CLIENT_EMAIL",
	"SSL_CLIENT_OU",
	"SSL_CLIENT_O",
	"SSL_CLIENT_L",
	"SSL_CLIENT_ST",
	"SSL_CLIENT_C",
	"SSL_CLIENT_I_CN",
	"SSL_CLIENT_I_EMAIL",
	"SSL_CLIENT_I_OU",
	"SSL_CLIENT_I_O",
	"SSL_CLIENT_I_L",
	"SSL_CLIENT_I_ST",
	"SSL_CLIENT_I_C",	
#endif
	NULL
};


static void php_info_isapi(ZEND_MODULE_INFO_FUNC_ARGS)
{
	char **p;
	char variable_buf[ISAPI_SERVER_VAR_BUF_SIZE];
	DWORD variable_len;
	char **all_variables[] = {
		isapi_server_variable_names,
		isapi_special_server_variable_names,
		isapi_secure_server_variable_names,
		NULL
	};
	char ***server_variable_names;
	LPEXTENSION_CONTROL_BLOCK lpECB;

	lpECB = (LPEXTENSION_CONTROL_BLOCK) SG(server_context);

	php_info_print_table_start();
	php_info_print_table_header(2, "Server Variable", "Value");
	server_variable_names = all_variables;
	while (*server_variable_names) {
		p = *server_variable_names;
		while (*p) {
			variable_len = ISAPI_SERVER_VAR_BUF_SIZE;
			if (lpECB->GetServerVariable(lpECB->ConnID, *p, variable_buf, &variable_len)
				&& variable_buf[0]) {
				php_info_print_table_row(2, *p, variable_buf);
			} else if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
				char *tmp_variable_buf;

				tmp_variable_buf = (char *) emalloc(variable_len);
				if (lpECB->GetServerVariable(lpECB->ConnID, *p, tmp_variable_buf, &variable_len)
					&& variable_buf[0]) {
					php_info_print_table_row(2, *p, tmp_variable_buf);
				}
				efree(tmp_variable_buf);
			}
			p++;
		}
		server_variable_names++;
	}
	php_info_print_table_end();
}


static zend_module_entry php_isapi_module = {
    STANDARD_MODULE_HEADER,
	"ISAPI",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	php_info_isapi,
    NULL,
	STANDARD_MODULE_PROPERTIES
};


static int sapi_isapi_ub_write(const char *str, uint str_length TSRMLS_DC)
{
	DWORD num_bytes = str_length;
	LPEXTENSION_CONTROL_BLOCK ecb;
	
	ecb = (LPEXTENSION_CONTROL_BLOCK) SG(server_context);
	if (ecb->WriteClient(ecb->ConnID, (char *) str, &num_bytes, HSE_IO_SYNC) == FALSE) {
		php_handle_aborted_connection();
	}
	return num_bytes;
}


static int sapi_isapi_header_handler(sapi_header_struct *sapi_header, sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	return SAPI_HEADER_ADD;
}



static void accumulate_header_length(sapi_header_struct *sapi_header, uint *total_length TSRMLS_DC)
{
	*total_length += sapi_header->header_len+2;
}


static void concat_header(sapi_header_struct *sapi_header, char **combined_headers_ptr TSRMLS_DC)
{
	memcpy(*combined_headers_ptr, sapi_header->header, sapi_header->header_len);
	*combined_headers_ptr += sapi_header->header_len;
	**combined_headers_ptr = '\r';
	(*combined_headers_ptr)++;
	**combined_headers_ptr = '\n';
	(*combined_headers_ptr)++;
}


static int sapi_isapi_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	uint total_length = 2;		/* account for the trailing \r\n */
	char *combined_headers, *combined_headers_ptr;
	LPEXTENSION_CONTROL_BLOCK lpECB = (LPEXTENSION_CONTROL_BLOCK) SG(server_context);
	HSE_SEND_HEADER_EX_INFO header_info;
	char status_buf[MAX_STATUS_LENGTH];
	sapi_header_struct default_content_type;

	/* Obtain headers length */
	if (SG(sapi_headers).send_default_content_type) {
		sapi_get_default_content_type_header(&default_content_type TSRMLS_CC);
		accumulate_header_length(&default_content_type, (void *) &total_length TSRMLS_CC);
	}
	zend_llist_apply_with_argument(&SG(sapi_headers).headers, (llist_apply_with_arg_func_t) accumulate_header_length, (void *) &total_length TSRMLS_CC);

	/* Generate headers */
	combined_headers = (char *) emalloc(total_length+1);
	combined_headers_ptr = combined_headers;
	if (SG(sapi_headers).send_default_content_type) {
		concat_header(&default_content_type, (void *) &combined_headers_ptr TSRMLS_CC);
		sapi_free_header(&default_content_type); /* we no longer need it */
	}
	zend_llist_apply_with_argument(&SG(sapi_headers).headers, (llist_apply_with_arg_func_t) concat_header, (void *) &combined_headers_ptr TSRMLS_CC);
	*combined_headers_ptr++ = '\r';
	*combined_headers_ptr++ = '\n';
	*combined_headers_ptr = 0;

	switch (SG(sapi_headers).http_response_code) {
		case 200:
			header_info.pszStatus = "200 OK";
			break;
		case 302:
			header_info.pszStatus = "302 Moved Temporarily";
			break;
		case 401:
			header_info.pszStatus = "401 Authorization Required";
			break;
		default:
			snprintf(status_buf, MAX_STATUS_LENGTH, "%d Undescribed", SG(sapi_headers).http_response_code);
			header_info.pszStatus = status_buf;
			break;
	}
	header_info.cchStatus = strlen(header_info.pszStatus);
	header_info.pszHeader = combined_headers;
	header_info.cchHeader = total_length;
	header_info.fKeepConn = FALSE;
	lpECB->dwHttpStatusCode = SG(sapi_headers).http_response_code;

	lpECB->ServerSupportFunction(lpECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER_EX, &header_info, NULL, NULL);

	efree(combined_headers);
	return SAPI_HEADER_SENT_SUCCESSFULLY;
}


static int php_isapi_startup(sapi_module_struct *sapi_module)
{
	if (php_module_startup(sapi_module, &php_isapi_module, 1)==FAILURE) {
		return FAILURE;
	} else {
		bTerminateThreadsOnError = (zend_bool) INI_INT("isapi.terminate_threads_on_error");
		return SUCCESS;
	}
}



static int sapi_isapi_read_post(char *buffer, uint count_bytes TSRMLS_DC)
{
	LPEXTENSION_CONTROL_BLOCK lpECB = (LPEXTENSION_CONTROL_BLOCK) SG(server_context);
	DWORD read_from_buf=0;
	DWORD read_from_input=0;
	DWORD total_read=0;

	if ((DWORD) SG(read_post_bytes) < lpECB->cbAvailable) {
		read_from_buf = MIN(lpECB->cbAvailable-SG(read_post_bytes), count_bytes);
		memcpy(buffer, lpECB->lpbData+SG(read_post_bytes), read_from_buf);
		total_read += read_from_buf;
	}
	if (read_from_buf<count_bytes
		&& (SG(read_post_bytes)+read_from_buf) < lpECB->cbTotalBytes) {
		DWORD cbRead=0, cbSize;

		read_from_input = MIN(count_bytes-read_from_buf, lpECB->cbTotalBytes-SG(read_post_bytes)-read_from_buf);
		while (cbRead < read_from_input) {
			cbSize = read_from_input - cbRead;
			if (!lpECB->ReadClient(lpECB->ConnID, buffer+read_from_buf+cbRead, &cbSize) || cbSize==0) {
				break;
			}
			cbRead += cbSize;
		}
		total_read += cbRead;
	}
	return total_read;
}


static char *sapi_isapi_read_cookies(TSRMLS_D)
{
	LPEXTENSION_CONTROL_BLOCK lpECB = (LPEXTENSION_CONTROL_BLOCK) SG(server_context);
	char variable_buf[ISAPI_SERVER_VAR_BUF_SIZE];
	DWORD variable_len = ISAPI_SERVER_VAR_BUF_SIZE;

	if (lpECB->GetServerVariable(lpECB->ConnID, "HTTP_COOKIE", variable_buf, &variable_len)) {
		return estrndup(variable_buf, variable_len);
	} else if (GetLastError()==ERROR_INSUFFICIENT_BUFFER) {
		char *tmp_variable_buf = (char *) emalloc(variable_len+1);

		if (lpECB->GetServerVariable(lpECB->ConnID, "HTTP_COOKIE", tmp_variable_buf, &variable_len)) {
			tmp_variable_buf[variable_len] = 0;
			return tmp_variable_buf;
		} else {
			efree(tmp_variable_buf);
		}
	}
	return NULL;
}


#ifdef WITH_ZEUS

static void sapi_isapi_register_zeus_ssl_variables(LPEXTENSION_CONTROL_BLOCK lpECB, zval *track_vars_array TSRMLS_DC)
{
	char static_variable_buf[ISAPI_SERVER_VAR_BUF_SIZE];
	DWORD variable_len = ISAPI_SERVER_VAR_BUF_SIZE;
	char static_cons_buf[ISAPI_SERVER_VAR_BUF_SIZE];
	/*
	 * We need to construct the /C=.../ST=...
	 * DN's for SSL_CLIENT_DN and SSL_CLIENT_I_DN
	 */
	strcpy( static_cons_buf, "/C=" );
	if( lpECB->GetServerVariable( lpECB->ConnID, "SSL_CLIENT_C", static_variable_buf, &variable_len ) && static_variable_buf[0] ) {
		strcat( static_cons_buf, static_variable_buf );
	}
	strcat( static_cons_buf, "/ST=" );
	variable_len = ISAPI_SERVER_VAR_BUF_SIZE;
	if( lpECB->GetServerVariable( lpECB->ConnID, "SSL_CLIENT_ST", static_variable_buf, &variable_len ) && static_variable_buf[0] ) {
		strcat( static_cons_buf, static_variable_buf );
	}
	php_register_variable( "SSL_CLIENT_DN", static_cons_buf, track_vars_array TSRMLS_CC );
	
	strcpy( static_cons_buf, "/C=" );
	variable_len = ISAPI_SERVER_VAR_BUF_SIZE;
	if( lpECB->GetServerVariable( lpECB->ConnID, "SSL_CLIENT_I_C", static_variable_buf, &variable_len ) && static_variable_buf[0] ) {
		strcat( static_cons_buf, static_variable_buf );
	}
	strcat( static_cons_buf, "/ST=" );
	variable_len = ISAPI_SERVER_VAR_BUF_SIZE;
	if( lpECB->GetServerVariable( lpECB->ConnID, "SSL_CLIENT_I_ST", static_variable_buf, &variable_len ) && static_variable_buf[0] ) {
		strcat( static_cons_buf, static_variable_buf );
	}
	php_register_variable( "SSL_CLIENT_I_DN", static_cons_buf, track_vars_array TSRMLS_CC );	
}

static void sapi_isapi_register_zeus_variables(LPEXTENSION_CONTROL_BLOCK lpECB, zval *track_vars_array TSRMLS_DC)
{
	char static_variable_buf[ISAPI_SERVER_VAR_BUF_SIZE];
	DWORD variable_len = ISAPI_SERVER_VAR_BUF_SIZE;
	DWORD scriptname_len = ISAPI_SERVER_VAR_BUF_SIZE;
	DWORD pathinfo_len = 0;
	char *strtok_buf = NULL;

	/* Get SCRIPT_NAME, we use this to work out which bit of the URL
	 * belongs in PHP's version of PATH_INFO
	 */
	lpECB->GetServerVariable(lpECB->ConnID, "SCRIPT_NAME", static_variable_buf, &scriptname_len);

	/* Adjust Zeus' version of PATH_INFO, set PHP_SELF,
	 * and generate REQUEST_URI
	 */
	if ( lpECB->GetServerVariable(lpECB->ConnID, "PATH_INFO", static_variable_buf, &variable_len) && static_variable_buf[0] ) {

		/* PHP_SELF is just PATH_INFO */
		php_register_variable( "PHP_SELF", static_variable_buf, track_vars_array TSRMLS_CC );

		/* Chop off filename to get just the 'real' PATH_INFO' */
		pathinfo_len = variable_len - scriptname_len;
		php_register_variable( "PATH_INFO", static_variable_buf + scriptname_len - 1, track_vars_array TSRMLS_CC );
		/* append query string to give url... extra byte for '?' */
		if ( strlen(lpECB->lpszQueryString) + variable_len + 1 < ISAPI_SERVER_VAR_BUF_SIZE ) {
			/* append query string only if it is present... */
			if ( strlen(lpECB->lpszQueryString) ) {
				static_variable_buf[ variable_len - 1 ] = '?';
				strcpy( static_variable_buf + variable_len, lpECB->lpszQueryString );
			}
			php_register_variable( "URL", static_variable_buf, track_vars_array TSRMLS_CC );
			php_register_variable( "REQUEST_URI", static_variable_buf, track_vars_array TSRMLS_CC );
		}
	}

	/* Get and adjust PATH_TRANSLATED to what PHP wants */
	variable_len = ISAPI_SERVER_VAR_BUF_SIZE;
	if ( lpECB->GetServerVariable(lpECB->ConnID, "PATH_TRANSLATED", static_variable_buf, &variable_len) && static_variable_buf[0] ) {
		static_variable_buf[ variable_len - pathinfo_len - 1 ] = '\0';
		php_register_variable( "PATH_TRANSLATED", static_variable_buf, track_vars_array TSRMLS_CC );
	}

	/* Bring in the AUTHENTICATION stuff as needed */
	variable_len = ISAPI_SERVER_VAR_BUF_SIZE;
	if ( lpECB->GetServerVariable(lpECB->ConnID, "AUTH_USER", static_variable_buf, &variable_len) && static_variable_buf[0] )  {
		php_register_variable( "PHP_AUTH_USER", static_variable_buf, track_vars_array TSRMLS_CC );
	}
	variable_len = ISAPI_SERVER_VAR_BUF_SIZE;
	if ( lpECB->GetServerVariable(lpECB->ConnID, "AUTH_PASSWORD", static_variable_buf, &variable_len) && static_variable_buf[0] )  {
		php_register_variable( "PHP_AUTH_PW", static_variable_buf, track_vars_array TSRMLS_CC );
	}
	variable_len = ISAPI_SERVER_VAR_BUF_SIZE;
	if ( lpECB->GetServerVariable(lpECB->ConnID, "AUTH_TYPE", static_variable_buf, &variable_len) && static_variable_buf[0] )  {
		php_register_variable( "AUTH_TYPE", static_variable_buf, track_vars_array TSRMLS_CC );
	}
	
	/* And now, for the SSL variables (if applicable) */
	variable_len = ISAPI_SERVER_VAR_BUF_SIZE;
	if ( lpECB->GetServerVariable(lpECB->ConnID, "CERT_COOKIE", static_variable_buf, &variable_len) && static_variable_buf[0] ) {
		sapi_isapi_register_zeus_ssl_variables( lpECB, track_vars_array TSRMLS_CC );
	}
	/* Copy some of the variables we need to meet Apache specs */
	variable_len = ISAPI_SERVER_VAR_BUF_SIZE;
	if ( lpECB->GetServerVariable(lpECB->ConnID, "SERVER_SOFTWARE", static_variable_buf, &variable_len) && static_variable_buf[0] )  {
		php_register_variable( "SERVER_SIGNATURE", static_variable_buf, track_vars_array TSRMLS_CC );
	}
}
#endif


static void sapi_isapi_register_server_variables2(char **server_variables, LPEXTENSION_CONTROL_BLOCK lpECB, zval *track_vars_array, char **recorded_values TSRMLS_DC)
{
	char **p=server_variables;
	DWORD variable_len;
	char static_variable_buf[ISAPI_SERVER_VAR_BUF_SIZE];
	char *variable_buf;

	while (*p) {
		variable_len = ISAPI_SERVER_VAR_BUF_SIZE;
		if (lpECB->GetServerVariable(lpECB->ConnID, *p, static_variable_buf, &variable_len)
			&& static_variable_buf[0]) {
			php_register_variable(*p, static_variable_buf, track_vars_array TSRMLS_CC);
			if (recorded_values) {
				recorded_values[p-server_variables] = estrndup(static_variable_buf, variable_len);
			}
		} else if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
			variable_buf = (char *) emalloc(variable_len+1);
			if (lpECB->GetServerVariable(lpECB->ConnID, *p, variable_buf, &variable_len)
				&& variable_buf[0]) {
				php_register_variable(*p, variable_buf, track_vars_array TSRMLS_CC);
			}
			if (recorded_values) {
				recorded_values[p-server_variables] = variable_buf;
			} else {
				efree(variable_buf);
			}
		}
		p++;
	}
}


static void sapi_isapi_register_server_variables(zval *track_vars_array TSRMLS_DC)
{
	DWORD variable_len = ISAPI_SERVER_VAR_BUF_SIZE;
	char *variable;
	char *strtok_buf = NULL;
	char *isapi_special_server_variables[NUM_SPECIAL_VARS];
	LPEXTENSION_CONTROL_BLOCK lpECB;

	lpECB = (LPEXTENSION_CONTROL_BLOCK) SG(server_context);

	/* Register the special ISAPI variables */
	memset(isapi_special_server_variables, 0, sizeof(isapi_special_server_variables));
	sapi_isapi_register_server_variables2(isapi_special_server_variable_names, lpECB, track_vars_array, isapi_special_server_variables TSRMLS_CC);
	if (SG(request_info).cookie_data) {
		php_register_variable("HTTP_COOKIE", SG(request_info).cookie_data, track_vars_array TSRMLS_CC);
	}

	/* Register the standard ISAPI variables */
	sapi_isapi_register_server_variables2(isapi_server_variable_names, lpECB, track_vars_array, NULL TSRMLS_CC);

	if (isapi_special_server_variables[SPECIAL_VAR_HTTPS]
		&& (atoi(isapi_special_server_variables[SPECIAL_VAR_HTTPS])
		|| !strcasecmp(isapi_special_server_variables[SPECIAL_VAR_HTTPS], "on"))
	) {
		/* Register SSL ISAPI variables */
		sapi_isapi_register_server_variables2(isapi_secure_server_variable_names, lpECB, track_vars_array, NULL TSRMLS_CC);
	}

	if (isapi_special_server_variables[SPECIAL_VAR_HTTPS]) {
		efree(isapi_special_server_variables[SPECIAL_VAR_HTTPS]);
	}


#ifdef WITH_ZEUS
	sapi_isapi_register_zeus_variables(lpECB, track_vars_array TSRMLS_CC);
#endif

	/* PHP_SELF support */
	if (isapi_special_server_variables[SPECIAL_VAR_PHP_SELF]) {
		php_register_variable("PHP_SELF", isapi_special_server_variables[SPECIAL_VAR_PHP_SELF], track_vars_array TSRMLS_CC);
		efree(isapi_special_server_variables[SPECIAL_VAR_PHP_SELF]);
	}

	if (isapi_special_server_variables[SPECIAL_VAR_ALL_HTTP]) {
		/* Register the internal bits of ALL_HTTP */
		variable = php_strtok_r(isapi_special_server_variables[SPECIAL_VAR_ALL_HTTP], "\r\n", &strtok_buf);
		while (variable) {
			char *colon = strchr(variable, ':');

			if (colon) {
				char *value = colon+1;

				while (*value==' ') {
					value++;
				}
				*colon = 0;
				php_register_variable(variable, value, track_vars_array TSRMLS_CC);
				*colon = ':';
			}
			variable = php_strtok_r(NULL, "\r\n", &strtok_buf);
		}
		efree(isapi_special_server_variables[SPECIAL_VAR_ALL_HTTP]);
	}

#ifdef PHP_WIN32
	{
		HSE_URL_MAPEX_INFO humi;
		DWORD path_len = 2;
		char path[] = "/";

		if (lpECB->ServerSupportFunction(lpECB->ConnID, HSE_REQ_MAP_URL_TO_PATH_EX, path, &path_len, (LPDWORD) &humi)) {
			/* Remove trailing \  */
			if (humi.lpszPath[path_len-2] == '\\') {
				humi.lpszPath[path_len-2] = 0;
			}
			php_register_variable("DOCUMENT_ROOT", humi.lpszPath, track_vars_array TSRMLS_CC);
		}
	}
#endif
}


static sapi_module_struct isapi_sapi_module = {
	"isapi",						/* name */
	"ISAPI",						/* pretty name */
									
	php_isapi_startup,				/* startup */
	php_module_shutdown_wrapper,	/* shutdown */

	NULL,							/* activate */
	NULL,							/* deactivate */

	sapi_isapi_ub_write,			/* unbuffered write */
	NULL,							/* flush */
	NULL,							/* get uid */
	NULL,							/* getenv */

	php_error,						/* error handler */

	sapi_isapi_header_handler,		/* header handler */
	sapi_isapi_send_headers,		/* send headers handler */
	NULL,							/* send header handler */

	sapi_isapi_read_post,			/* read POST data */
	sapi_isapi_read_cookies,		/* read Cookies */

	sapi_isapi_register_server_variables,	/* register server variables */
	NULL,									/* Log message */

	NULL,									/* Block interruptions */
	NULL,									/* Unblock interruptions */

	STANDARD_SAPI_MODULE_PROPERTIES
};


BOOL WINAPI GetFilterVersion(PHTTP_FILTER_VERSION pFilterVersion)
{
	bFilterLoaded = 1;
	pFilterVersion->dwFilterVersion = HTTP_FILTER_REVISION;
	strcpy(pFilterVersion->lpszFilterDesc, isapi_sapi_module.pretty_name);
	pFilterVersion->dwFlags= (SF_NOTIFY_AUTHENTICATION | SF_NOTIFY_PREPROC_HEADERS);
	return TRUE;
}


DWORD WINAPI HttpFilterProc(PHTTP_FILTER_CONTEXT pfc, DWORD notificationType, LPVOID pvNotification)
{
	TSRMLS_FETCH();

	switch (notificationType) {
		case SF_NOTIFY_PREPROC_HEADERS:
			SG(request_info).auth_user = NULL;
			SG(request_info).auth_password = NULL;
			break;
		case SF_NOTIFY_AUTHENTICATION: {
				char *auth_user = ((HTTP_FILTER_AUTHENT *) pvNotification)->pszUser;
				char *auth_password = ((HTTP_FILTER_AUTHENT *) pvNotification)->pszPassword;

				if (auth_user && auth_user[0]) {
					SG(request_info).auth_user = estrdup(auth_user);
				}	
				if (auth_password && auth_password[0]) {
					SG(request_info).auth_password = estrdup(auth_password);
				}
				return SF_STATUS_REQ_HANDLED_NOTIFICATION;
			}
			break;
	}
	return SF_STATUS_REQ_NEXT_NOTIFICATION;
}


static void init_request_info(LPEXTENSION_CONTROL_BLOCK lpECB TSRMLS_DC)
{
	SG(request_info).request_method = lpECB->lpszMethod;
	SG(request_info).query_string = lpECB->lpszQueryString;
	SG(request_info).path_translated = lpECB->lpszPathTranslated;
	SG(request_info).request_uri = lpECB->lpszPathInfo;
	SG(request_info).content_type = lpECB->lpszContentType;
	SG(request_info).content_length = lpECB->cbTotalBytes;
	SG(sapi_headers).http_response_code = 200;  /* I think dwHttpStatusCode is invalid at this stage -RL */
	if (!bFilterLoaded) { /* we don't have valid ISAPI Filter information */
		SG(request_info).auth_user = SG(request_info).auth_password = NULL;
	}
}


static void php_isapi_report_exception(char *message, int message_len TSRMLS_DC)
{
	if (!SG(headers_sent)) {
		HSE_SEND_HEADER_EX_INFO header_info;
		LPEXTENSION_CONTROL_BLOCK lpECB = (LPEXTENSION_CONTROL_BLOCK) SG(server_context);

		header_info.pszStatus = "500 Internal Server Error";
		header_info.cchStatus = strlen(header_info.pszStatus);
		header_info.pszHeader = "Content-Type: text/html\r\n\r\n";
		header_info.cchHeader = strlen(header_info.pszHeader);

		lpECB->dwHttpStatusCode = 500;
		lpECB->ServerSupportFunction(lpECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER_EX, &header_info, NULL, NULL);
		SG(headers_sent)=1;
	}
	sapi_isapi_ub_write(message, message_len TSRMLS_CC);
}


BOOL WINAPI GetExtensionVersion(HSE_VERSION_INFO *pVer)
{
	pVer->dwExtensionVersion = HSE_VERSION;
#ifdef WITH_ZEUS
	strncpy( pVer->lpszExtensionDesc, isapi_sapi_module.name, HSE_MAX_EXT_DLL_NAME_LEN);
#else
	lstrcpyn(pVer->lpszExtensionDesc, isapi_sapi_module.name, HSE_MAX_EXT_DLL_NAME_LEN);
#endif
	return TRUE;
}


static void my_endthread()
{
#ifdef PHP_WIN32
	if (bTerminateThreadsOnError) {
		_endthread();
	}
#endif
}

#ifdef PHP_WIN32
/* ep is accessible only in the context of the __except expression,
 * so we have to call this function to obtain it.
 */
BOOL exceptionhandler(LPEXCEPTION_POINTERS *e, LPEXCEPTION_POINTERS ep)
{
	*e=ep;
	return TRUE;
}
#endif

DWORD WINAPI HttpExtensionProc(LPEXTENSION_CONTROL_BLOCK lpECB)
{
	zend_file_handle file_handle = {0};
	zend_bool stack_overflown=0;
#ifdef PHP_ENABLE_SEH
	LPEXCEPTION_POINTERS e;
#endif
	TSRMLS_FETCH();

	zend_first_try {
#ifdef PHP_ENABLE_SEH
		__try {
#endif
			init_request_info(lpECB TSRMLS_CC);
			SG(server_context) = lpECB;

#ifdef WITH_ZEUS
			/* PATH_TRANSLATED can contain extra PATH_INFO stuff after the
			 * file being loaded, so we must use SCRIPT_FILENAME instead
			 */
			file_handle.filename = (char *)emalloc( ISAPI_SERVER_VAR_BUF_SIZE );
			file_handle.free_filename = 1;
			{
				DWORD filename_len = ISAPI_SERVER_VAR_BUF_SIZE;
				if( !lpECB->GetServerVariable(lpECB->ConnID, "SCRIPT_FILENAME", file_handle.filename, &filename_len) || file_handle.filename[ 0 ] == '\0' ) {
					/* If we're running on an earlier version of Zeus, this
					 * variable won't be present, so fall back to old behaviour.
					 */
					efree( file_handle.filename );
					file_handle.filename = SG(request_info).path_translated;
					file_handle.free_filename = 0;
				}
			}
#else
			file_handle.filename = SG(request_info).path_translated;
			file_handle.free_filename = 0;
#endif
			file_handle.type = ZEND_HANDLE_FILENAME;
			file_handle.opened_path = NULL;
			/* some server configurations allow '..' to slip through in the
			   translated path.   We'll just refuse to handle such a path. */
			if (strstr(SG(request_info).path_translated,"..")) {
				SG(sapi_headers).http_response_code = 404;
				SG(request_info).path_translated = NULL;
			}

			php_request_startup(TSRMLS_C);
			php_execute_script(&file_handle TSRMLS_CC);
			if (SG(request_info).cookie_data) {
				efree(SG(request_info).cookie_data);
			}
#ifdef PHP_ENABLE_SEH
		} __except(exceptionhandler(&e, GetExceptionInformation())) {
			char buf[1024];
			if (_exception_code()==EXCEPTION_STACK_OVERFLOW) {
				LPBYTE lpPage;
				static SYSTEM_INFO si;
				static MEMORY_BASIC_INFORMATION mi;
				static DWORD dwOldProtect;

				GetSystemInfo(&si);

				/* Get page ESP is pointing to */
				_asm mov lpPage, esp;

				/* Get stack allocation base */
				VirtualQuery(lpPage, &mi, sizeof(mi));

				/* Go to the page below the current page */
				lpPage = (LPBYTE) (mi.BaseAddress) - si.dwPageSize;

				/* Free pages below current page */
				if (!VirtualFree(mi.AllocationBase, (LPBYTE)lpPage - (LPBYTE) mi.AllocationBase, MEM_DECOMMIT)) {
					_endthread();
				}

				/* Restore the guard page */
				if (!VirtualProtect(lpPage, si.dwPageSize, PAGE_GUARD | PAGE_READWRITE, &dwOldProtect)) {
					_endthread();
				}

				CG(unclean_shutdown)=1;
				_snprintf(buf, sizeof(buf)-1,"PHP has encountered a Stack overflow");
				php_isapi_report_exception(buf, strlen(buf) TSRMLS_CC);
			} else if (_exception_code()==EXCEPTION_ACCESS_VIOLATION) {
				_snprintf(buf, sizeof(buf)-1,"PHP has encountered an Access Violation at %p", e->ExceptionRecord->ExceptionAddress);
				php_isapi_report_exception(buf, strlen(buf) TSRMLS_CC);
				my_endthread();
			} else {
				_snprintf(buf, sizeof(buf)-1,"PHP has encountered an Unhandled Exception Code %d at %p", e->ExceptionRecord->ExceptionCode , e->ExceptionRecord->ExceptionAddress);
				php_isapi_report_exception(buf, strlen(buf) TSRMLS_CC);
				my_endthread();
			}
		}
#endif
#ifdef PHP_ENABLE_SEH
		__try {
			php_request_shutdown(NULL);
		} __except(EXCEPTION_EXECUTE_HANDLER) {
			my_endthread();
		}
#else
		php_request_shutdown(NULL);
#endif
	} zend_catch {
		zend_try {
			php_request_shutdown(NULL);
		} zend_end_try();
		return HSE_STATUS_ERROR;
	} zend_end_try();

	return HSE_STATUS_SUCCESS;
}



__declspec(dllexport) BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason) {
		case DLL_PROCESS_ATTACH:
#ifdef WITH_ZEUS
			tsrm_startup(128, 1, TSRM_ERROR_LEVEL_CORE, "TSRM.log");
#else
			tsrm_startup(128, 1, TSRM_ERROR_LEVEL_CORE, "C:\\TSRM.log");
#endif
			sapi_startup(&isapi_sapi_module);
			if (isapi_sapi_module.startup) {
				isapi_sapi_module.startup(&sapi_module);
			}
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			ts_free_thread();
			break;
		case DLL_PROCESS_DETACH:
			if (isapi_sapi_module.shutdown) {
				isapi_sapi_module.shutdown(&sapi_module);
			}
			tsrm_shutdown();
			break;
	}
	return TRUE;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
