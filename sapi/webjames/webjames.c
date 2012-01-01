/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Alex Waugh <alex@alexwaugh.com>                              |
   +----------------------------------------------------------------------+
*/


#include "php.h"
#include "SAPI.h"
#include "php_main.h"
#include "php_variables.h"

#define WEBJAMES_PHP_ONLY
#include "php_webjames.h"

#include <unixlib/local.h>

#define WEBJAMES_SAPI_VERSION "1.0.2"

typedef struct {
	struct connection *conn; /*structure holding all the details of the current request*/
	int bodyread; /*amount of POST body read*/
	closefn oldclose; /*function to call to close the connection*/
} php_webjames_globals;

static php_webjames_globals webjames_globals;

#define WG(v) (webjames_globals.v)

static int sapi_webjames_ub_write(const char *str, uint str_length TSRMLS_DC)
/*unbuffered write - send data straight out to socket*/
{
	int totalbytes = 0;

	do {
		int bytes;
		bytes = webjames_writebuffer(WG(conn),str,str_length);
		if (bytes<0) {
			PG(connection_status) = PHP_CONNECTION_ABORTED;
			if (!PG(ignore_user_abort)) {
				zend_bailout();
			}
			return bytes;
		}
		str += bytes;
		str_length -= bytes;
		totalbytes += bytes;
	} while (str_length);
	return totalbytes;
}

static void sapi_webjames_send_header(sapi_header_struct *sapi_header, void *server_context TSRMLS_DC)
/*send an HTTP header*/
{
	char *header = sapi_header->header;
	int len = sapi_header->header_len;
	if (WG(conn)->flags.outputheaders) {
		while (sapi_header && len > 0) {
			int bytes;
			bytes = webjames_writebuffer(WG(conn), header, len);
			if (bytes<0) {
				PG(connection_status) = PHP_CONNECTION_ABORTED;
				if (!PG(ignore_user_abort)) {
					zend_bailout();
				}
				return;
			}
			header += bytes;
			len -= bytes;
		}
		webjames_writestring(WG(conn), "\r\n");
	}
}

static int sapi_webjames_read_post(char *buffer, uint count_bytes TSRMLS_DC)
/*read some of the post data*/
{
	if (WG(conn)->body==NULL) return 0;
	if (count_bytes+WG(bodyread)>WG(conn)->bodysize) count_bytes=WG(conn)->bodysize-WG(bodyread);
	memcpy(buffer, WG(conn)->body+WG(bodyread), count_bytes);
	WG(bodyread)+=count_bytes;
	return count_bytes;
}

static char *sapi_webjames_read_cookies(TSRMLS_D)
{
	return WG(conn)->cookie;
}

#define BUF_SIZE 512
#define ADD_STRING(name,string)\
	php_register_variable(name, string, track_vars_array TSRMLS_CC)

#define ADD_NUM(name,field) {\
	snprintf(buf, BUF_SIZE, "%d", WG(conn)->field);\
	php_register_variable(name, buf, track_vars_array TSRMLS_CC);\
}

#define ADD_FIELD(name, field) \
	if (WG(conn)->field) { \
		php_register_variable(name, WG(conn)->field, track_vars_array TSRMLS_CC); \
	}

static void sapi_webjames_register_variables(zval *track_vars_array TSRMLS_DC)
{
	char buf[BUF_SIZE + 1];
	char *docroot;

	buf[BUF_SIZE] = '\0';

	ADD_STRING("SERVER_SOFTWARE", configuration.server);
	ADD_STRING("SERVER_NAME", configuration.serverip);
	ADD_FIELD("SERVER_PROTOCOL", protocol);
	ADD_NUM("SERVER_PORT", port);
	ADD_STRING("SERVER_ADMIN",configuration.webmaster);
	ADD_STRING("GATEWAY_INTERFACE", "CGI/1.1");

	docroot = __unixify(WG(conn)->homedir,0,NULL,1024,0);
	if (docroot) ADD_STRING("DOCUMENT_ROOT", docroot);

	ADD_FIELD("REQUEST_METHOD", methodstr);
	ADD_FIELD("REQUEST_URI", requesturi);
	ADD_STRING("PATH_TRANSLATED", SG(request_info).path_translated);
	ADD_FIELD("SCRIPT_NAME", uri);
	ADD_FIELD("PHP_SELF", uri);
	ADD_FIELD("QUERY_STRING", args);

	
	snprintf(buf, BUF_SIZE, "%d.%d.%d.%d", WG(conn)->ipaddr[0], WG(conn)->ipaddr[1], WG(conn)->ipaddr[2], WG(conn)->ipaddr[3]);
	ADD_STRING("REMOTE_ADDR", buf);
	if (WG(conn)->dnsstatus == DNS_OK) ADD_FIELD("REMOTE_HOST", host);

	if ((WG(conn)->method == METHOD_POST) || (WG(conn)->method == METHOD_PUT)) {
		ADD_NUM("CONTENT_LENGTH", bodysize);
		ADD_FIELD("CONTENT_TYPE", type);
	}

	if ((WG(conn)->method == METHOD_PUT) || (WG(conn)->method == METHOD_DELETE)) ADD_FIELD("ENTITY_PATH", requesturi);

	if (WG(conn)->pwd) {
		ADD_STRING("AUTH_TYPE", "basic");
		ADD_FIELD("REMOTE_USER", authorization);
	}

	ADD_FIELD("HTTP_COOKIE", cookie);
	ADD_FIELD("HTTP_USER_AGENT", useragent);
	ADD_FIELD("HTTP_REFERER", referer);
	ADD_FIELD("HTTP_ACCEPT", accept);
	ADD_FIELD("HTTP_ACCEPT_LANGUAGE", acceptlanguage);
	ADD_FIELD("HTTP_ACCEPT_CHARSET", acceptcharset);
	ADD_FIELD("HTTP_ACCEPT_ENCODING", acceptencoding);
}

static void webjames_module_main(TSRMLS_D)
{
	zend_file_handle file_handle;
	FILE *fp=NULL;
	char *path;

	/* Convert filename to Unix format*/
	__riscosify_control|=__RISCOSIFY_STRICT_UNIX_SPECS;
	path = __unixify(WG(conn)->filename,0,NULL,1024,0);
	if (path) SG(request_info).path_translated = estrdup(path);

	SG(request_info).query_string = WG(conn)->args;
	SG(request_info).request_uri = WG(conn)->requesturi;
	SG(request_info).request_method = WG(conn)->methodstr;
	if (WG(conn)->method==METHOD_HEAD) {
		SG(request_info).headers_only = 1;
	} else {
		SG(request_info).headers_only = 0;
	}
	SG(sapi_headers).http_response_code = 200;
	SG(request_info).content_type = WG(conn)->type;
	SG(request_info).content_length = WG(conn)->bodysize;

	SG(request_info).auth_user = NULL;
	SG(request_info).auth_password = NULL;
	if (WG(conn)->authorization) {
		char *colon=strchr(WG(conn)->authorization,':');
		if (colon) {
			SG(request_info).auth_user = emalloc(colon-WG(conn)->authorization+1);
			if (SG(request_info).auth_user) {
				memcpy(SG(request_info).auth_user,WG(conn)->authorization,colon-WG(conn)->authorization);
				SG(request_info).auth_user[colon-WG(conn)->authorization]='\0';
				SG(request_info).auth_password = estrdup(colon+1);
			}
		}
	}

	/*ensure that syslog calls get logged separately from WebJames' main log */
	openlog("PHP",0,0);

	file_handle.type = ZEND_HANDLE_FILENAME;
	file_handle.filename = SG(request_info).path_translated;
	file_handle.free_filename = 0;
	file_handle.opened_path = NULL;

	if (php_request_startup(TSRMLS_C) == FAILURE) {
		return;
	}
	
	php_execute_script(&file_handle TSRMLS_CC);
	php_request_shutdown(NULL);
}

static void webjames_php_close(struct connection *conn, int force)
/*called by webjames if it wants to close the connection*/
{
	TSRMLS_FETCH();

	php_request_shutdown(NULL);
	WG(oldclose)(conn,force);
}

void webjames_php_request(struct connection *conn)
/*called by WebJames to start handler*/
{
	TSRMLS_FETCH();

	WG(conn) = conn;
	WG(bodyread) = 0;
	WG(oldclose) = conn->close;
	conn->close=webjames_php_close;

	webjames_module_main(TSRMLS_C);

	WG(oldclose)(WG(conn), 0);
}

static void php_info_webjames(ZEND_MODULE_INFO_FUNC_ARGS)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "SAPI module version", WEBJAMES_SAPI_VERSION);
	php_info_print_table_row(2, "WebJames version", WEBJAMES_VERSION " (" WEBJAMES_DATE ")");
	php_info_print_table_end();
}

static zend_module_entry php_webjames_module = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
  "WebJames",
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  php_info_webjames,
#if ZEND_MODULE_API_NO >= 20010901
  WEBJAMES_SAPI_VERSION,          
#endif
  STANDARD_MODULE_PROPERTIES
};


static int php_webjames_startup(sapi_module_struct *sapi_module)
{
  if(php_module_startup(sapi_module, &php_webjames_module, 1) == FAILURE) {
    return FAILURE;
  } else {
    return SUCCESS;
  }
}

static sapi_module_struct sapi_module = {
	"webjames",                             /* name */
	"WebJames",                             /* pretty name */

	php_webjames_startup,                   /* startup */
	php_module_shutdown_wrapper,            /* shutdown */

	NULL,									/* activate */
	NULL,									/* deactivate */

	sapi_webjames_ub_write,                 /* unbuffered write */
	NULL,                                   /* flush */
	NULL,									/* get uid */
	NULL,									/* getenv */

	php_error,                              /* error handler */

	NULL,                                   /* header handler */
	NULL,             						/* send headers handler */
	sapi_webjames_send_header,              /* send header handler */
	sapi_webjames_read_post,                /* read POST data */
	sapi_webjames_read_cookies,             /* read Cookies */

	sapi_webjames_register_variables,       /* register server variables */
	NULL,									/* Log message */
	NULL,									/* Get request time */
	NULL,									/* Child terminate */

	STANDARD_SAPI_MODULE_PROPERTIES
};

int webjames_php_init(void)
/*called when WebJames initialises*/
{
	TSRMLS_FETCH();
	if (strcmp(configuration.webjames_h_revision,WEBJAMES_H_REVISION)!=0) {
		/*This file was compiled against a different revision of
		  webjames.h than webjames was, which could be bad news*/
		webjames_writelog(0,"PHP module is compiled for WebJames (%s) and was linked with a different version (%s)",WEBJAMES_H_REVISION,configuration.webjames_h_revision);
		return 0; /*failed to initialise*/
	}
	sapi_startup(&sapi_module);
	sapi_module.startup(&sapi_module);
	SG(server_context) = (void *) 1;
	return 1; /*initialised correctly*/
}

void webjames_php_shutdown(void)
/*called when WebJames is about to quit*/
{
	sapi_module.shutdown(&sapi_module);
	sapi_shutdown();
}
