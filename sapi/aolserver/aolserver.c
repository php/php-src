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
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
 */

/*
 * TODO:
 * - write documentation
 * - CGI/1.1 conformance
 */

/* $Id$ */

/* conflict between PHP and AOLserver headers */
#define Debug php_Debug
#include "php.h"
#undef Debug

#ifdef HAVE_AOLSERVER

#ifndef ZTS
#error AOLserver module is only useable in thread-safe mode
#endif

#include "ext/standard/info.h"
#define SECTION(name)  PUTS("<h2>" name "</h2>\n")

#define NS_BUF_SIZE 511

#include "php_ini.h"
#include "php_globals.h"
#include "SAPI.h"
#include "php_main.h"
#include "php_variables.h"

#include "ns.h"

#include "php_version.h"

/* This symbol is used by AOLserver to tell the API version we expect */

int Ns_ModuleVersion = 1;

#define NSG(v) TSRMG(ns_globals_id, ns_globals_struct *, v)

/* php_ns_context is per-server (thus only once at all) */

typedef struct {
	sapi_module_struct *sapi_module;
	char *ns_server;
	char *ns_module;
} php_ns_context;

/* ns_globals_struct is per-thread */

typedef struct {
	Ns_Conn *conn;
	size_t data_avail;
} ns_globals_struct;

/* TSRM id */

static int ns_globals_id;

/* global context */

static php_ns_context *global_context;

static void php_ns_config(php_ns_context *ctx, char global);

/*
 * php_ns_sapi_ub_write() writes data to the client connection.
 */

static int
php_ns_sapi_ub_write(const char *str, uint str_length TSRMLS_DC)
{
	int n;
	uint sent = 0;

	while (str_length > 0) {
		n = Ns_ConnWrite(NSG(conn), (void *) str, str_length);

		if (n == -1)
			php_handle_aborted_connection();

		str += n;
		sent += n;
		str_length -= n;
	}
	
	return sent;
}

/*
 * php_ns_sapi_header_handler() sets a HTTP reply header to be 
 * sent to the client.
 */

static int
php_ns_sapi_header_handler(sapi_header_struct *sapi_header, sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	char *header_name, *header_content;
	char *p;

	header_name = sapi_header->header;
	header_content = p = strchr(header_name, ':');

	if (p) {
		*p = '\0';
		do {
			header_content++;
		} while (*header_content == ' ');

		if (!strcasecmp(header_name, "Content-type")) {
			Ns_ConnSetTypeHeader(NSG(conn), header_content);
		} else {
			Ns_ConnSetHeaders(NSG(conn), header_name, header_content);
		}

		*p = ':';
	}

	sapi_free_header(sapi_header);
	
	return 0;
}

/*
 * php_ns_sapi_send_headers() flushes the headers to the client.
 * Called before real content is sent by PHP.
 */

static int
php_ns_sapi_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	if(SG(sapi_headers).send_default_content_type) {
		Ns_ConnSetRequiredHeaders(NSG(conn), "text/html", 0);
	}
	
	Ns_ConnFlushHeaders(NSG(conn), SG(sapi_headers).http_response_code);
	
	return SAPI_HEADER_SENT_SUCCESSFULLY;
}

/*
 * php_ns_sapi_read_post() reads a specified number of bytes from
 * the client. Used for POST/PUT requests.
 */

static int
php_ns_sapi_read_post(char *buf, uint count_bytes TSRMLS_DC)
{
	uint max_read;
	uint total_read = 0;

	max_read = MIN(NSG(data_avail), count_bytes);
	
	total_read = Ns_ConnRead(NSG(conn), buf, max_read);
	
	if(total_read == NS_ERROR) {
		total_read = -1;
	} else {
		NSG(data_avail) -= total_read;
	}

	return total_read;
}

/* 
 * php_ns_sapi_read_cookies() returns the Cookie header from
 * the HTTP request header
 */
	
static char *php_ns_sapi_read_cookies(TSRMLS_D)
{
	int i;
	char *http_cookie = NULL;
	
	i = Ns_SetIFind(NSG(conn->headers), "cookie");
	if(i != -1) {
		http_cookie = Ns_SetValue(NSG(conn->headers), i);
	}

	return http_cookie;
}

static void php_info_aolserver(ZEND_MODULE_INFO_FUNC_ARGS)
{
	char buf[512];
	int uptime = Ns_InfoUptime();
	int i;
	
	php_info_print_table_start();
	php_info_print_table_row(2, "SAPI module version", "$Id$");
	php_info_print_table_row(2, "Build date", Ns_InfoBuildDate());
	php_info_print_table_row(2, "Config file path", Ns_InfoConfigFile());
	php_info_print_table_row(2, "Error Log path", Ns_InfoErrorLog());
	php_info_print_table_row(2, "Installation path", Ns_InfoHomePath());
	php_info_print_table_row(2, "Hostname of server", Ns_InfoHostname());
	php_info_print_table_row(2, "Source code label", Ns_InfoLabel());
	php_info_print_table_row(2, "Server platform", Ns_InfoPlatform());
	snprintf(buf, 511, "%s/%s", Ns_InfoServerName(), Ns_InfoServerVersion());
	php_info_print_table_row(2, "Server version", buf);
	snprintf(buf, 511, "%d day(s), %02d:%02d:%02d", 
			uptime / 86400,
			(uptime / 3600) % 24,
			(uptime / 60) % 60,
			uptime % 60);
	php_info_print_table_row(2, "Server uptime", buf);
	php_info_print_table_end();

	SECTION("HTTP Headers Information");
	php_info_print_table_start();
	php_info_print_table_colspan_header(2, "HTTP Request Headers");
	php_info_print_table_row(2, "HTTP Request", NSG(conn)->request->line);
	for (i = 0; i < Ns_SetSize(NSG(conn)->headers); i++) {
		php_info_print_table_row(2, Ns_SetKey(NSG(conn)->headers, i), Ns_SetValue(NSG(conn)->headers, i));
	}

	php_info_print_table_colspan_header(2, "HTTP Response Headers");
	for (i = 0; i < Ns_SetSize(NSG(conn)->outputheaders); i++) {
		php_info_print_table_row(2, Ns_SetKey(NSG(conn)->outputheaders, i), Ns_SetValue(NSG(conn)->outputheaders, i));
	}
	php_info_print_table_end();
}

PHP_FUNCTION(getallheaders);

static function_entry aolserver_functions[] = {
	PHP_FE(getallheaders, NULL)
	{NULL, NULL, NULL}
};

static zend_module_entry php_aolserver_module = {
	STANDARD_MODULE_HEADER,
	"AOLserver",
	aolserver_functions,
	NULL,
	NULL,
	NULL,
	NULL,
	php_info_aolserver,
	NULL,
	STANDARD_MODULE_PROPERTIES
};

PHP_FUNCTION(getallheaders)
{
	int i;

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	
	for (i = 0; i < Ns_SetSize(NSG(conn->headers)); i++) {
		char *key = Ns_SetKey(NSG(conn->headers), i);
		char *value = Ns_SetValue(NSG(conn->headers), i);
		
		add_assoc_string(return_value, key, value, 1);
	}
}

static int
php_ns_startup(sapi_module_struct *sapi_module)
{
	if (php_module_startup(sapi_module, &php_aolserver_module, 1) == FAILURE) {
		return FAILURE;
	} else {
		return SUCCESS;
	}
}


/*
 * php_ns_sapi_register_variables() populates the php script environment
 * with a number of variables. HTTP_* variables are created for
 * the HTTP header data, so that a script can access these.
 */

#define ADD_STRINGX(name, buf)										\
	php_register_variable(name, buf, track_vars_array TSRMLS_CC)

#define ADD_STRING(name)										\
	ADD_STRINGX(name, buf)

static void
php_ns_sapi_register_variables(zval *track_vars_array TSRMLS_DC)
{
	int i;
	char buf[NS_BUF_SIZE + 1];
	char *tmp;

	for(i = 0; i < Ns_SetSize(NSG(conn->headers)); i++) {
		char *key = Ns_SetKey(NSG(conn->headers), i);
		char *value = Ns_SetValue(NSG(conn->headers), i);
		char *p;
		char c;

		snprintf(buf, NS_BUF_SIZE, "HTTP_%s", key);
		
		for(p = buf + 5; (c = *p); p++) {
			c = toupper(c);
			if(c < 'A' || c > 'Z') {
				c = '_';
			}
			*p = c;
		}

		ADD_STRINGX(buf, value);
	}
	
	snprintf(buf, NS_BUF_SIZE, "%s/%s", Ns_InfoServerName(), Ns_InfoServerVersion());
	ADD_STRING("SERVER_SOFTWARE");
	snprintf(buf, NS_BUF_SIZE, "HTTP/%1.1f", NSG(conn)->request->version);
	ADD_STRING("SERVER_PROTOCOL");

	ADD_STRINGX("REQUEST_METHOD", NSG(conn)->request->method);

	if(NSG(conn)->request->query)
		ADD_STRINGX("QUERY_STRING", NSG(conn)->request->query);
	
	ADD_STRINGX("SERVER_BUILDDATE", Ns_InfoBuildDate());

	ADD_STRINGX("REMOTE_ADDR", Ns_ConnPeer(NSG(conn)));

	snprintf(buf, NS_BUF_SIZE, "%d", Ns_ConnPeerPort(NSG(conn)));
	ADD_STRING("REMOTE_PORT");

	snprintf(buf, NS_BUF_SIZE, "%d", Ns_ConnPort(NSG(conn)));
	ADD_STRING("SERVER_PORT");

	tmp = Ns_ConnHost(NSG(conn));
	if (tmp)
		ADD_STRINGX("SERVER_NAME", tmp);

	ADD_STRINGX("PATH_TRANSLATED", SG(request_info).path_translated);
	ADD_STRINGX("REQUEST_URI", SG(request_info).request_uri);
	ADD_STRINGX("PHP_SELF", SG(request_info).request_uri);

	ADD_STRINGX("GATEWAY_INTERFACE", "CGI/1.1");

	snprintf(buf, NS_BUF_SIZE, "%d", Ns_InfoBootTime());
	ADD_STRING("SERVER_BOOTTIME");
}



/* this structure is static (as in "it does not change") */

static sapi_module_struct aolserver_sapi_module = {
	"aolserver",
	"AOLserver",

	php_ns_startup,							/* startup */
	php_module_shutdown_wrapper,			/* shutdown */

	NULL,									/* activate */
	NULL,									/* deactivate */

	php_ns_sapi_ub_write,					/* unbuffered write */
	NULL,									/* flush */
	NULL,									/* get uid */
	NULL,									/* getenv */

	php_error,								/* error handler */

	php_ns_sapi_header_handler,				/* header handler */
	php_ns_sapi_send_headers,				/* send headers handler */
	NULL,									/* send header handler */

	php_ns_sapi_read_post,					/* read POST data */
	php_ns_sapi_read_cookies,				/* read Cookies */

	php_ns_sapi_register_variables,
	NULL,									/* Log message */

	NULL,									/* php.ini path override */

	NULL,									/* Block interruptions */
	NULL,									/* Unblock interruptions */

	STANDARD_SAPI_MODULE_PROPERTIES
};

/*
 * php_ns_module_main() is called by the per-request handler and
 * "executes" the script
 */

static int
php_ns_module_main(TSRMLS_D)
{
	zend_file_handle file_handle;

	file_handle.type = ZEND_HANDLE_FILENAME;
	file_handle.filename = SG(request_info).path_translated;
	file_handle.free_filename = 0;
	file_handle.opened_path = NULL;
	
	php_ns_config(global_context, 0);
	if (php_request_startup(TSRMLS_C) == FAILURE) {
		return NS_ERROR;
	}
	
	php_execute_script(&file_handle TSRMLS_CC);
	php_request_shutdown(NULL);

	return NS_OK;
}

/*
 * php_ns_request_ctor() initializes the per-request data structure
 * and fills it with data provided by the web server
 */

static void 
php_ns_request_ctor(TSRMLS_D)
{
	char *server;
	Ns_DString ds;
	char *root;
	int index;
	char *tmp;
	
	server = Ns_ConnServer(NSG(conn));
	
#define safe_strdup(x) ((x)?strdup((x)):NULL)
	SG(request_info).query_string = safe_strdup(NSG(conn->request->query));

	Ns_DStringInit(&ds);
	Ns_UrlToFile(&ds, server, NSG(conn->request->url));
	
	/* path_translated is the absolute path to the file */
	SG(request_info).path_translated = safe_strdup(Ns_DStringValue(&ds));
	Ns_DStringFree(&ds);
	root = Ns_PageRoot(server);
	SG(request_info).request_uri = strdup(SG(request_info).path_translated + strlen(root));
	SG(request_info).request_method = NSG(conn)->request->method;
	SG(request_info).content_length = Ns_ConnContentLength(NSG(conn));
	index = Ns_SetIFind(NSG(conn)->headers, "content-type");
	SG(request_info).content_type = index == -1 ? NULL : 
		Ns_SetValue(NSG(conn)->headers, index);
	SG(sapi_headers).http_response_code = 200;

	tmp = Ns_ConnAuthUser(NSG(conn));
	if (tmp)
		tmp = estrdup(tmp);
	SG(request_info).auth_user = tmp;

	tmp = Ns_ConnAuthPasswd(NSG(conn));
	if (tmp)
		tmp = estrdup(tmp);
	SG(request_info).auth_password = tmp;

	NSG(data_avail) = SG(request_info).content_length;
}

/*
 * php_ns_request_dtor() destroys all data associated with
 * the per-request structure 
 */

static void
php_ns_request_dtor(TSRMLS_D)
{
	free(SG(request_info).path_translated);
	if (SG(request_info).query_string)
		free(SG(request_info).query_string);
	free(SG(request_info).request_uri);
}

/*
 * The php_ns_request_handler() is called per request and handles
 * everything for one request.
 */

static int
php_ns_request_handler(void *context, Ns_Conn *conn)
{
	int status = NS_OK;
	TSRMLS_FETCH();
	
	NSG(conn) = conn;
	
	SG(server_context) = global_context;

	php_ns_request_ctor(TSRMLS_C);
	
	status = php_ns_module_main(TSRMLS_C);
	
	php_ns_request_dtor(TSRMLS_C);

	return status;
}

/*
 * php_ns_config() fetches the configuration data.
 *
 * It understands the "map" and "php_value" command.
 */

static void 
php_ns_config(php_ns_context *ctx, char global)
{
	int i;
	char *path;
	Ns_Set *set;

	path = Ns_ConfigGetPath(ctx->ns_server, ctx->ns_module, NULL);
	set = Ns_ConfigGetSection(path);

	for (i = 0; set && i < Ns_SetSize(set); i++) {
		char *key = Ns_SetKey(set, i);
		char *value = Ns_SetValue(set, i);

		if (global && !strcasecmp(key, "map")) {
			Ns_Log(Notice, "Registering PHP for \"%s\"", value);
			Ns_RegisterRequest(ctx->ns_server, "GET", value, php_ns_request_handler, NULL, ctx, 0);
			Ns_RegisterRequest(ctx->ns_server, "POST", value, php_ns_request_handler, NULL, ctx, 0);
			Ns_RegisterRequest(ctx->ns_server, "HEAD", value, php_ns_request_handler, NULL, ctx, 0);

	/* 
	 * Deactivated for now. The ini system will cause random crashes when 
	 * accessed from here (since there are no locks to protect the global 
	 * known_directives) 
	 */

		} else if (!global && !strcasecmp(key, "php_value")) {
			Ns_Log(Notice, "php_value has been deactivated temporarily. Please use a php.ini file to pass directives to PHP. Thanks.");
#if 0
			char *val;

			val = strchr(value, ' ');
			if (val) {
				char *new_key;
				
				new_key = estrndup(value, val - value);
				
				do { 
					val++; 
				} while(*val == ' ');

				Ns_Log(Debug, "PHP configuration option '%s=%s'", new_key, val);
				zend_alter_ini_entry(new_key, strlen(new_key) + 1, val, 
						strlen(val) + 1, PHP_INI_SYSTEM, PHP_INI_STAGE_RUNTIME);
				
				efree(new_key);
			}
#endif
		}
		
	}
}
	
/*
 * php_ns_server_shutdown() performs the last steps before the
 * server exits. Shutdowns basic services and frees memory
 */

static void
php_ns_server_shutdown(void *context)
{
	php_ns_context *ctx = (php_ns_context *) context;
	
	ctx->sapi_module->shutdown(ctx->sapi_module);
	sapi_shutdown();
	tsrm_shutdown();

	free(ctx->ns_module);
	free(ctx->ns_server);
	free(ctx);
}

/*
 * Ns_ModuleInit() is called by AOLserver once at startup
 *
 * This functions allocates basic structures and initializes
 * basic services.
 */

int Ns_ModuleInit(char *server, char *module)
{
	php_ns_context *ctx;
	
	tsrm_startup(1, 1, 0, NULL);
	sapi_startup(&aolserver_sapi_module);
	sapi_module.startup(&aolserver_sapi_module);
	
	/* TSRM is used to allocate a per-thread structure */
	ts_allocate_id(&ns_globals_id, sizeof(ns_globals_struct), NULL, NULL);
	
	/* the context contains data valid for all threads */
	ctx = malloc(sizeof *ctx);
	ctx->sapi_module = &aolserver_sapi_module;
	ctx->ns_server = strdup(server);
	ctx->ns_module = strdup(module);
	
	/* read the configuration */
	php_ns_config(ctx, 1);

	global_context = ctx;

	/* register shutdown handler */
	Ns_RegisterServerShutdown(server, php_ns_server_shutdown, ctx);

	return NS_OK;
}

#endif
