/* 
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"

#ifdef HAVE_AOLSERVER

#include "php_ini.h"
#include "php_globals.h"
#include "SAPI.h"
#include "main.h"

/* conflict between PHP and aolserver */
#define Debug DEBUG_UNUSED
#include "ns.h"
#undef Debug

#include "php_version.h"

#if 0
#define HERE \
	Ns_Log(Notice, "in %s:%d", __FUNCTION__, __LINE__);
#else
#define HERE
#endif

int Ns_ModuleVersion = 1;

typedef struct {
	sapi_module_struct *sapi_module;
	Ns_DString content_type;
	Ns_Conn *conn;
} php_aol_context;

static int
sapi_ub_write(const char *str, uint str_length)
{
	Ns_DString dstr;
	php_aol_context *ctx;
	SLS_FETCH();

	HERE;
	ctx = (php_aol_context *) SG(server_context);

	Ns_DStringInit(&dstr);
	Ns_Log(Notice, "writing %d bytes\n", str_length);
	Ns_DStringNAppend(&dstr, (char *) str, str_length);
	Ns_ConnSendDString(ctx->conn, &dstr);
	Ns_DStringFree(&dstr);

	return str_length;
}

static int
sapi_header_handler(sapi_header_struct *sapi_header, sapi_headers_struct *sapi_headers SLS_DC)
{
	char *header_name, *header_content;
	char *p;
	php_aol_context *ctx = (php_aol_context *) SG(server_context);

	header_name = sapi_header->header;
	header_content = p = strchr(header_name, ':');

	if(!p) return 0;

	*p = '\0';
	do {
		header_content++;
	} while(*header_content == ' ');

	if(!strcasecmp(header_name, "Content-type")) {
		Ns_ConnSetTypeHeader(ctx->conn, header_content);
	} else {
		Ns_ConnSetHeaders(ctx->conn, header_name, header_content);
	}
	
	*p = ':';
	
	efree(sapi_header->header);
	
	return 0;
}

static int
sapi_send_headers(sapi_headers_struct *sapi_headers SLS_DC)
{
	php_aol_context *ctx;
	
	ctx = (php_aol_context *) SG(server_context);
	Ns_ConnFlushHeaders(ctx->conn, 200);
	return SAPI_HEADER_SENT_SUCCESSFULLY;
}

static int
sapi_read_post(char *buf, uint count_bytes SLS_DC)
{
	uint total_read = 0;
	php_aol_context *ctx = (php_aol_context *) SG(server_context);

	total_read = Ns_ConnRead(ctx->conn, buf, total_read);
	return total_read;
}
	
static char *
sapi_read_cookies(SLS_D)
{
	HERE;
	return NULL;
}

static sapi_module_struct sapi_module = {
	"PHP Language",

	php_module_startup,				/* startup */
	php_module_shutdown_wrapper,			/* shutdown */

	sapi_ub_write,					/* unbuffered write */

	php_error,						/* error handler */

	sapi_header_handler,			/* header handler */
	sapi_send_headers,				/* send headers handler */
	NULL,							/* send header handler */

	sapi_read_post,					/* read POST data */
	sapi_read_cookies,				/* read Cookies */

	STANDARD_SAPI_MODULE_PROPERTIES
};

static void
php_server_shutdown(void *context)
{
	php_aol_context *ctx = (php_aol_context *) context;
	
	HERE;
	ctx->sapi_module->shutdown(ctx->sapi_module);

	free(ctx);
}

static int
module_main(php_aol_context *ctx SLS_DC)
{
	zend_file_handle file_handle;
	CLS_FETCH();
	ELS_FETCH();
	PLS_FETCH();

	HERE;

	file_handle.type = ZEND_HANDLE_FILENAME;
	file_handle.filename = SG(request_info).path_translated;
	
	php_request_startup(CLS_C ELS_CC PLS_CC SLS_CC);
	php_execute_script(&file_handle CLS_CC ELS_CC PLS_CC);
	php_request_shutdown(NULL);

	return NS_OK;
}

static void 
request_ctor(php_aol_context *ctx SLS_DC)
{
	char *server;
	Ns_DString ds;
	char *root;
	HERE;
	
	server = Ns_ConnServer(ctx->conn);
	
	SG(server_context) = ctx;
	SG(request_info).query_string = ctx->conn->request->query;

	Ns_DStringInit(&ds);
	Ns_UrlToFile(&ds, server, ctx->conn->request->url);
	SG(request_info).path_translated = strdup(Ns_DStringValue(&ds));
	Ns_DStringFree(&ds);
	root = Ns_PageRoot(server);
	SG(request_info).request_uri = SG(request_info).path_translated + strlen(root);
	SG(request_info).request_method = ctx->conn->request->method;
	SG(request_info).content_length = Ns_ConnContentLength(ctx->conn);
	Ns_DStringInit(&ctx->content_type);
	Ns_ConnCopyToDString(ctx->conn, SG(request_info).content_length, &ctx->content_type);
	SG(request_info).content_type = Ns_DStringValue(&ctx->content_type);
	SG(request_info).auth_user = NULL;
	SG(request_info).auth_password = NULL;
}

static void
request_dtor(php_aol_context *ctx SLS_DC)
{
	HERE;
	
	free(SG(request_info).path_translated);
	Ns_DStringFree(&ctx->content_type);
}

static int
request_handler(void *context, Ns_Conn *conn)
{
	php_aol_context *ctx = (php_aol_context *) context;
	int status = NS_OK;
	SLS_FETCH();
	
	HERE;
	ctx->conn = conn;
	
	request_ctor(ctx SLS_CC);
	
	status = module_main(ctx SLS_CC);
	
	request_dtor(ctx SLS_CC);
	
	ctx->conn = NULL;

	return status;
}

static void config(char *server, char *module, php_aol_context *ctx)
{
	int i;
	char *path;
	Ns_Set *set;
	char *map;

	path = Ns_ConfigPath(server, NULL, "php", NULL);
	map = NULL;
	set = Ns_ConfigGetSection(path);

	for(i = 0; set && i < Ns_SetSize(set); i++) {
		char *key = Ns_SetValue(set, i);

		if(!strcasecmp(key, "map")) {
			map = Ns_SetValue(set, i);
			Ns_RegisterRequest(server, "GET", map, request_handler, NULL, ctx, 0);
			Ns_RegisterRequest(server, "POST", map, request_handler, NULL, ctx, 0);
			Ns_RegisterRequest(server, "HEAD", map, request_handler, NULL, ctx, 0);
		}
	}
}
	

int Ns_ModuleInit(char *server, char *module)
{
	php_aol_context *ctx;
	
	tsrm_startup(10, 10, 0);
	sapi_startup(&sapi_module);
	sapi_module.startup(&sapi_module);
	
	ctx = malloc(sizeof *ctx);
	ctx->sapi_module = &sapi_module;

	config(server, module, ctx);

	Ns_RegisterServerShutdown(server, php_server_shutdown, ctx);

	return NS_OK;
}

#endif
