/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include <httpext.h>
#include <httpfilt.h>
#include <httpext.h>
#include "php_main.h"
#include "SAPI.h"
#include "php_globals.h"
#include "ext/standard/info.h"
#include "php_variables.h"
#include "php_ini.h"
#include <process.h>

#define PHP_ENABLE_SEH

/*
uncomment the following lines to turn off
exception trapping when running under a debugger

#ifdef _DEBUG
#undef PHP_ENABLE_SEH
#endif
*/

#define MAX_STATUS_LENGTH sizeof("xxxx LONGEST POSSIBLE STATUS DESCRIPTION")
#define ISAPI_SERVER_VAR_BUF_SIZE 1024

static zend_bool bFilterLoaded=0;
static zend_bool bTerminateThreadsOnError=0;

static char *isapi_special_server_variable_names[] = {
	"ALL_HTTP",
	"HTTPS",
	"SCRIPT_NAME",
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
	"APPL_MD_PATH",
	"APPL_PHYSICAL_PATH",
	"INSTANCE_ID",
	"INSTANCE_META_PATH",
	"LOGON_USER",
	"REQUEST_URI",
	"URL",
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
	NULL
};

typedef struct http_status {
	int code;
	const char* msg;
  } http_status;
  
  static const http_status http_status_codes[] = {
	{100, "Continue"},
	{101, "Switching Protocols"},
	{200, "OK"},
	{201, "Created"},
	{202, "Accepted"},
	{203, "Non-Authoritative Information"},
	{204, "No Content"},
	{205, "Reset Content"},
	{206, "Partial Content"},
	{300, "Multiple Choices"},
	{301, "Moved Permanently"},
	{302, "Moved Temporarily"},
	{303, "See Other"},
	{304, "Not Modified"},
	{305, "Use Proxy"},
	{400, "Bad Request"},
	{401, "Unauthorized"},
	{402, "Payment Required"},
	{403, "Forbidden"},
	{404, "Not Found"},
	{405, "Method Not Allowed"},
	{406, "Not Acceptable"},
	{407, "Proxy Authentication Required"},
	{408, "Request Time-out"},
	{409, "Conflict"},
	{410, "Gone"},
	{411, "Length Required"},
	{412, "Precondition Failed"},
	{413, "Request Entity Too Large"},
	{414, "Request-URI Too Large"},
	{415, "Unsupported Media Type"},
	{428, "Precondition Required"},
	{429, "Too Many Requests"},
	{431, "Request Header Fields Too Large"},
	{451, "Unavailable For Legal Reasons"},
	{500, "Internal Server Error"},
	{501, "Not Implemented"},
	{502, "Bad Gateway"},
	{503, "Service Unavailable"},
	{504, "Gateway Time-out"},
	{505, "HTTP Version not supported"},
	{511, "Network Authentication Required"},
	{0, NULL}
};

#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif

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


static size_t sapi_isapi_ub_write(const char *str, size_t str_length)
{
	DWORD num_bytes = str_length;
	LPEXTENSION_CONTROL_BLOCK ecb;

	ecb = (LPEXTENSION_CONTROL_BLOCK) SG(server_context);
	if (ecb->WriteClient(ecb->ConnID, (char *) str, &num_bytes, HSE_IO_SYNC) == FALSE) {
		php_handle_aborted_connection();
	}
	return num_bytes;
}


static int sapi_isapi_header_handler(sapi_header_struct *sapi_header, sapi_header_op_enum op, sapi_headers_struct *sapi_headers)
{
	return SAPI_HEADER_ADD;
}



static void accumulate_header_length(sapi_header_struct *sapi_header, unsigned int *total_length)
{
	*total_length += sapi_header->header_len+2;
}


static void concat_header(sapi_header_struct *sapi_header, char **combined_headers_ptr)
{
	memcpy(*combined_headers_ptr, sapi_header->header, sapi_header->header_len);
	*combined_headers_ptr += sapi_header->header_len;
	**combined_headers_ptr = '\r';
	(*combined_headers_ptr)++;
	**combined_headers_ptr = '\n';
	(*combined_headers_ptr)++;
}


static int sapi_isapi_send_headers(sapi_headers_struct *sapi_headers)
{
	unsigned int total_length = 2;		/* account for the trailing \r\n */
	char *combined_headers, *combined_headers_ptr;
	LPEXTENSION_CONTROL_BLOCK lpECB = (LPEXTENSION_CONTROL_BLOCK) SG(server_context);
	HSE_SEND_HEADER_EX_INFO header_info;
	sapi_header_struct default_content_type;
	char *status_buf = NULL;

	/* Obtain headers length */
	if (SG(sapi_headers).send_default_content_type) {
		sapi_get_default_content_type_header(&default_content_type);
		accumulate_header_length(&default_content_type, (void *) &total_length);
	}
	zend_llist_apply_with_argument(&SG(sapi_headers).headers, (llist_apply_with_arg_func_t) accumulate_header_length, (void *) &total_length);

	/* Generate headers */
	combined_headers = (char *) emalloc(total_length+1);
	combined_headers_ptr = combined_headers;
	if (SG(sapi_headers).send_default_content_type) {
		concat_header(&default_content_type, (void *) &combined_headers_ptr);
		sapi_free_header(&default_content_type); /* we no longer need it */
	}
	zend_llist_apply_with_argument(&SG(sapi_headers).headers, (llist_apply_with_arg_func_t) concat_header, (void *) &combined_headers_ptr);
	*combined_headers_ptr++ = '\r';
	*combined_headers_ptr++ = '\n';
	*combined_headers_ptr = 0;

	const char *sline = SG(sapi_headers).http_status_line;
	int sline_len;

	/* httpd requires that r->status_line is set to the first digit of
		* the status-code: */
	if (sline && ((sline_len = strlen(sline)) > 12) && strncmp(sline, "HTTP/1.", 7) == 0 && sline[8] == ' ') {
		if ((sline_len - 9) > MAX_STATUS_LENGTH) {
			status_buf = estrndup(sline + 9, MAX_STATUS_LENGTH);
		} else {
			status_buf = estrndup(sline + 9, sline_len - 9);
		}
	} else {
		http_status *status = (http_status*) http_status_codes;

		while (status->code != 0 && status->code != SG(sapi_headers).http_response_code) {
			status++;
		}
		status_buf = emalloc(MAX_STATUS_LENGTH + 1);
		if (status->code) {
			snprintf(status_buf, MAX_STATUS_LENGTH, "%d %s", status->code, status->msg);
		} else {
			snprintf(status_buf, MAX_STATUS_LENGTH, "%d Undefined", SG(sapi_headers).http_response_code);
		}
	}
	header_info.pszStatus = status_buf;
	header_info.cchStatus = strlen(header_info.pszStatus);
	header_info.pszHeader = combined_headers;
	header_info.cchHeader = total_length;
	header_info.fKeepConn = FALSE;
	lpECB->dwHttpStatusCode = SG(sapi_headers).http_response_code;

	lpECB->ServerSupportFunction(lpECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER_EX, &header_info, NULL, NULL);

	efree(combined_headers);
	if (status_buf) {
		efree(status_buf);
	}
	return SAPI_HEADER_SENT_SUCCESSFULLY;
}


static int php_isapi_startup(sapi_module_struct *sapi_module)
{
	if (php_module_startup(sapi_module, &php_isapi_module)==FAILURE) {
		return FAILURE;
	} else {
		bTerminateThreadsOnError = (zend_bool) INI_INT("isapi.terminate_threads_on_error");
		return SUCCESS;
	}
}


static size_t sapi_isapi_read_post(char *buffer, size_t count_bytes)
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


static char *sapi_isapi_read_cookies(void)
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

static void sapi_isapi_register_iis_variables(LPEXTENSION_CONTROL_BLOCK lpECB, zval *track_vars_array)
{
	char static_variable_buf[ISAPI_SERVER_VAR_BUF_SIZE];
	char path_info_buf[ISAPI_SERVER_VAR_BUF_SIZE];
	DWORD variable_len = ISAPI_SERVER_VAR_BUF_SIZE;
	DWORD scriptname_len = ISAPI_SERVER_VAR_BUF_SIZE;
	DWORD pathinfo_len = 0;
	HSE_URL_MAPEX_INFO humi;

	/* Get SCRIPT_NAME, we use this to work out which bit of the URL
	 * belongs in PHP's version of PATH_INFO.  SCRIPT_NAME also becomes PHP_SELF.
	 */
	lpECB->GetServerVariable(lpECB->ConnID, "SCRIPT_NAME", static_variable_buf, &scriptname_len);
	php_register_variable("SCRIPT_FILENAME", SG(request_info).path_translated, track_vars_array);

	/* Adjust IIS' version of PATH_INFO, set PHP_SELF,
	 * and generate REQUEST_URI
	 * Get and adjust PATH_TRANSLATED to what PHP wants
	 */
	if ( lpECB->GetServerVariable(lpECB->ConnID, "PATH_INFO", static_variable_buf, &variable_len) && static_variable_buf[0] ) {

		/* Chop off filename to get just the 'real' PATH_INFO' */
		php_register_variable( "ORIG_PATH_INFO", static_variable_buf, track_vars_array );
		pathinfo_len = variable_len - scriptname_len;
		strncpy(path_info_buf, static_variable_buf + scriptname_len - 1, sizeof(path_info_buf)-1);
		php_register_variable( "PATH_INFO", path_info_buf, track_vars_array );
		/* append query string to give url... extra byte for '?' */
		if ( strlen(lpECB->lpszQueryString) + variable_len + 1 < ISAPI_SERVER_VAR_BUF_SIZE ) {
			/* append query string only if it is present... */
			if ( strlen(lpECB->lpszQueryString) ) {
				static_variable_buf[ variable_len - 1 ] = '?';
				strcpy( static_variable_buf + variable_len, lpECB->lpszQueryString );
			}
			php_register_variable( "URL", static_variable_buf, track_vars_array );
			php_register_variable( "REQUEST_URI", static_variable_buf, track_vars_array );
		}
		variable_len = ISAPI_SERVER_VAR_BUF_SIZE;
		if ( lpECB->GetServerVariable(lpECB->ConnID, "PATH_TRANSLATED", static_variable_buf, &variable_len) && static_variable_buf[0] ) {
			php_register_variable( "ORIG_PATH_TRANSLATED", static_variable_buf, track_vars_array );
		}
		if (lpECB->ServerSupportFunction(lpECB->ConnID, HSE_REQ_MAP_URL_TO_PATH_EX, path_info_buf, &pathinfo_len, (LPDWORD) &humi)) {
			/* Remove trailing \  */
			if (humi.lpszPath[variable_len-2] == '\\') {
				humi.lpszPath[variable_len-2] = 0;
			}
			php_register_variable("PATH_TRANSLATED", humi.lpszPath, track_vars_array);
		}
	}

	static_variable_buf[0] = '/';
	static_variable_buf[1] = 0;
	variable_len = 2;
	if (lpECB->ServerSupportFunction(lpECB->ConnID, HSE_REQ_MAP_URL_TO_PATH_EX, static_variable_buf, &variable_len, (LPDWORD) &humi)) {
		/* Remove trailing \  */
		if (humi.lpszPath[variable_len-2] == '\\') {
			humi.lpszPath[variable_len-2] = 0;
		}
		php_register_variable("DOCUMENT_ROOT", humi.lpszPath, track_vars_array);
	}

	if (!SG(request_info).auth_user || !SG(request_info).auth_password ||
		!SG(request_info).auth_user[0] || !SG(request_info).auth_password[0]) {
		variable_len = ISAPI_SERVER_VAR_BUF_SIZE;
		if (lpECB->GetServerVariable(lpECB->ConnID, "HTTP_AUTHORIZATION", static_variable_buf, &variable_len)
			&& static_variable_buf[0]) {
			php_handle_auth_data(static_variable_buf);
		}
	}

	if (SG(request_info).auth_user)  {
		php_register_variable("PHP_AUTH_USER", SG(request_info).auth_user, track_vars_array );
	}
	if (SG(request_info).auth_password) {
		php_register_variable("PHP_AUTH_PW", SG(request_info).auth_password, track_vars_array );
	}
}

static void sapi_isapi_register_server_variables2(char **server_variables, LPEXTENSION_CONTROL_BLOCK lpECB, zval *track_vars_array, char **recorded_values)
{
	char **p=server_variables;
	DWORD variable_len;
	char static_variable_buf[ISAPI_SERVER_VAR_BUF_SIZE];
	char *variable_buf;

	while (*p) {
		variable_len = ISAPI_SERVER_VAR_BUF_SIZE;
		if (lpECB->GetServerVariable(lpECB->ConnID, *p, static_variable_buf, &variable_len)
			&& static_variable_buf[0]) {
			php_register_variable(*p, static_variable_buf, track_vars_array);
			if (recorded_values) {
				recorded_values[p-server_variables] = estrndup(static_variable_buf, variable_len);
			}
		} else if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
			variable_buf = (char *) emalloc(variable_len+1);
			if (lpECB->GetServerVariable(lpECB->ConnID, *p, variable_buf, &variable_len)
				&& variable_buf[0]) {
				php_register_variable(*p, variable_buf, track_vars_array);
			}
			if (recorded_values) {
				recorded_values[p-server_variables] = variable_buf;
			} else {
				efree(variable_buf);
			}
		} else { /* for compatibility with Apache SAPIs */
			php_register_variable(*p, "", track_vars_array);
		}
		p++;
	}
}


static void sapi_isapi_register_server_variables(zval *track_vars_array)
{
	DWORD variable_len = ISAPI_SERVER_VAR_BUF_SIZE;
	char *variable;
	char *strtok_buf = NULL;
	char *isapi_special_server_variables[NUM_SPECIAL_VARS];
	LPEXTENSION_CONTROL_BLOCK lpECB;

	lpECB = (LPEXTENSION_CONTROL_BLOCK) SG(server_context);

	/* Register the special ISAPI variables */
	memset(isapi_special_server_variables, 0, sizeof(isapi_special_server_variables));
	sapi_isapi_register_server_variables2(isapi_special_server_variable_names, lpECB, track_vars_array, isapi_special_server_variables);
	if (SG(request_info).cookie_data) {
		php_register_variable("HTTP_COOKIE", SG(request_info).cookie_data, track_vars_array);
	}

	/* Register the standard ISAPI variables */
	sapi_isapi_register_server_variables2(isapi_server_variable_names, lpECB, track_vars_array, NULL);

	if (isapi_special_server_variables[SPECIAL_VAR_HTTPS]
		&& (atoi(isapi_special_server_variables[SPECIAL_VAR_HTTPS])
		|| !strcasecmp(isapi_special_server_variables[SPECIAL_VAR_HTTPS], "on"))
	) {
		/* Register SSL ISAPI variables */
		sapi_isapi_register_server_variables2(isapi_secure_server_variable_names, lpECB, track_vars_array, NULL);
	}

	if (isapi_special_server_variables[SPECIAL_VAR_HTTPS]) {
		efree(isapi_special_server_variables[SPECIAL_VAR_HTTPS]);
	}

	sapi_isapi_register_iis_variables(lpECB, track_vars_array);

	/* PHP_SELF support */
	if (isapi_special_server_variables[SPECIAL_VAR_PHP_SELF]) {
		php_register_variable("PHP_SELF", isapi_special_server_variables[SPECIAL_VAR_PHP_SELF], track_vars_array);
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
				php_register_variable(variable, value, track_vars_array);
				*colon = ':';
			}
			variable = php_strtok_r(NULL, "\r\n", &strtok_buf);
		}
		efree(isapi_special_server_variables[SPECIAL_VAR_ALL_HTTP]);
	}
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
	NULL,							/* Log message */
	NULL,							/* Get request time */
	NULL,							/* Child terminate */

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

	switch (notificationType) {
		case SF_NOTIFY_PREPROC_HEADERS:
			SG(request_info).auth_user = NULL;
			SG(request_info).auth_password = NULL;
			SG(request_info).auth_digest = NULL;
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


static void init_request_info(LPEXTENSION_CONTROL_BLOCK lpECB)
{
	DWORD variable_len = ISAPI_SERVER_VAR_BUF_SIZE;
	char static_variable_buf[ISAPI_SERVER_VAR_BUF_SIZE];
	HSE_URL_MAPEX_INFO humi;

	SG(request_info).request_method = lpECB->lpszMethod;
	SG(request_info).query_string = lpECB->lpszQueryString;
	SG(request_info).request_uri = lpECB->lpszPathInfo;
	SG(request_info).content_type = lpECB->lpszContentType;
	SG(request_info).content_length = lpECB->cbTotalBytes;
	SG(sapi_headers).http_response_code = 200;  /* I think dwHttpStatusCode is invalid at this stage -RL */
	if (!bFilterLoaded) { /* we don't have valid ISAPI Filter information */
		SG(request_info).auth_user = SG(request_info).auth_password = SG(request_info).auth_digest = NULL;
	}

	/* happily, IIS gives us SCRIPT_NAME which is correct (without PATH_INFO stuff)
	   so we can just map that to the physical path and we have our filename */

	lpECB->GetServerVariable(lpECB->ConnID, "SCRIPT_NAME", static_variable_buf, &variable_len);
	if (lpECB->ServerSupportFunction(lpECB->ConnID, HSE_REQ_MAP_URL_TO_PATH_EX, static_variable_buf, &variable_len, (LPDWORD) &humi)) {
		SG(request_info).path_translated = estrdup(humi.lpszPath);
	} else
		/* if mapping fails, default to what the server tells us */
		SG(request_info).path_translated = estrdup(lpECB->lpszPathTranslated);

	/* some server configurations allow '..' to slip through in the
	   translated path.   We'll just refuse to handle such a path. */
	if (strstr(SG(request_info).path_translated,"..")) {
		SG(sapi_headers).http_response_code = 404;
		efree(SG(request_info).path_translated);
		SG(request_info).path_translated = NULL;
	}
}


static void php_isapi_report_exception(char *message, int message_len)
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
	sapi_isapi_ub_write(message, message_len);
}


BOOL WINAPI GetExtensionVersion(HSE_VERSION_INFO *pVer)
{
	pVer->dwExtensionVersion = HSE_VERSION;
	lstrcpyn(pVer->lpszExtensionDesc, isapi_sapi_module.name, HSE_MAX_EXT_DLL_NAME_LEN);
	return TRUE;
}


static void my_endthread()
{
	if (bTerminateThreadsOnError) {
		_endthread();
	}
}

/* ep is accessible only in the context of the __except expression,
 * so we have to call this function to obtain it.
 */
BOOL exceptionhandler(LPEXCEPTION_POINTERS *e, LPEXCEPTION_POINTERS ep)
{
	*e=ep;
	return TRUE;
}

DWORD WINAPI HttpExtensionProc(LPEXTENSION_CONTROL_BLOCK lpECB)
{
	zend_file_handle file_handle;
	zend_bool stack_overflown=0;
	int retval = FAILURE;
#ifdef PHP_ENABLE_SEH
	LPEXCEPTION_POINTERS e;
#endif
#ifdef ZTS
	/* initial resource fetch */
	(void)ts_resource(0);
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	zend_first_try {
#ifdef PHP_ENABLE_SEH
		__try {
#endif
			init_request_info(lpECB);
			SG(server_context) = lpECB;
			php_request_startup();

			// FIXME: we're leaking the file_handle.filename
			zend_stream_init_filename(&file_handle, (char *) SG(request_info).path_translated);
			file_handle.primary_script = 1;

			/* open the script here so we can 404 if it fails */
			if (file_handle.filename)
				retval = php_fopen_primary_script(&file_handle);

			if (!file_handle.filename || retval == FAILURE) {
				SG(sapi_headers).http_response_code = 404;
				PUTS("No input file specified.\n");
			} else {
				php_execute_script(&file_handle);
			}
			zend_destroy_file_handle(&file_handle);

			if (SG(request_info).cookie_data) {
				efree(SG(request_info).cookie_data);
			}
			if (SG(request_info).path_translated) {
				efree(SG(request_info).path_translated);
			}
#ifdef PHP_ENABLE_SEH
		} __except(exceptionhandler(&e, GetExceptionInformation())) {
			char buf[1024];
			if (_exception_code()==EXCEPTION_STACK_OVERFLOW) {
				if (!_resetstkoflw()) {
					_endthread();
				}

				CG(unclean_shutdown)=1;
				_snprintf(buf, sizeof(buf)-1,"PHP has encountered a Stack overflow");
				php_isapi_report_exception(buf, strlen(buf));
			} else if (_exception_code()==EXCEPTION_ACCESS_VIOLATION) {
				_snprintf(buf, sizeof(buf)-1,"PHP has encountered an Access Violation at %p", e->ExceptionRecord->ExceptionAddress);
				php_isapi_report_exception(buf, strlen(buf));
				my_endthread();
			} else {
				_snprintf(buf, sizeof(buf)-1,"PHP has encountered an Unhandled Exception Code %d at %p", e->ExceptionRecord->ExceptionCode , e->ExceptionRecord->ExceptionAddress);
				php_isapi_report_exception(buf, strlen(buf));
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
			php_tsrm_startup_ex(128);
			ZEND_TSRMLS_CACHE_UPDATE();
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
			sapi_shutdown();
			tsrm_shutdown();
			break;
	}
	return TRUE;
}
