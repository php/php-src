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
   | Author: Jayakumar Muthukumarasamy <jk@kasenna.com>                   |
   +----------------------------------------------------------------------+
*/

/*
 * PHP includes
 */
#define NSAPI 1

#include "php.h"
#include "php_variables.h"
#include "ext/standard/info.h"
#include "php_ini.h"
#include "php_globals.h"
#include "SAPI.h"
#include "php_main.h"
#include "php_version.h"
#include "TSRM.h"
#include "ext/standard/php_standard.h"

/*
 * If neither XP_UNIX not XP_WIN32 is defined, try to guess which one.
 * Ideally, this should be done by the configure script.
 */
#if !defined(XP_UNIX) && !defined(XP_WIN32)
	#if defined(WIN32)
		#define XP_WIN32
	#else
		#define XP_UNIX
	#endif
#endif

/*
 * NSAPI includes
 */
#include "nsapi.h"
#include "base/pblock.h"
#include "base/session.h"
#include "frame/req.h"
#include "frame/protocol.h"  /* protocol_start_response */
#include "base/util.h"       /* is_mozilla, getline */
#include "frame/log.h"       /* log_error */

/*
 * Timeout for net_read(). This should probably go into php.ini
 */
#define NSAPI_READ_TIMEOUT	60	/* 60 seconds */

#define NSLS_D		struct nsapi_request_context *request_context
#define NSLS_DC		, NSLS_D
#define NSLS_C		request_context
#define NSLS_CC		, NSLS_C
#define NSG(v)		(request_context->v)

/*
 * ZTS needs to be defined for NSAPI to work
 */
#if !defined(ZTS)
	#error "NSAPI module needs ZTS to be defined"
#endif

/*
 * Structure to encapsulate the NSAPI request in SAPI
 */
typedef struct nsapi_request_context {
	pblock	*pb;
	Session	*sn;
	Request	*rq;
	int	read_post_bytes;
} nsapi_request_context;

/*
 * Mappings between NSAPI names and environment variables. This
 * mapping was obtained from the sample programs at the iplanet
 * website.
 */
typedef struct nsapi_equiv {
	const char *env_var;
	const char *nsapi_eq;
} nsapi_equiv;

static nsapi_equiv nsapi_headers[] = {
	{ "CONTENT_LENGTH",			"content-length" },
	{ "CONTENT_TYPE",			"content-type" },
	{ "HTTP_ACCEPT",			"accept" },
	{ "HTTP_ACCEPT_ENCODING",	"accept-encoding" },
	{ "HTTP_ACCEPT_LANGUAGE",	"accept-language" },
	{ "HTTP_ACCEPT_CHARSET",	"accept-charset" },
	{ "HTTP_AUTHORIZATION",		"authorization" },
	{ "HTTP_COOKIE",			"cookie" },
	{ "HTTP_IF_MODIFIED_SINCE",	"if-modified-since" },
	{ "HTTP_REFERER",			"referer" },
	{ "HTTP_USER_AGENT",		"user-agent" },
	{ "HTTP_USER_DEFINED",		"user-defined" }
};
static size_t nsapi_headers_size = sizeof(nsapi_headers)/sizeof(nsapi_headers[0]);

static nsapi_equiv nsapi_reqpb[] = {
	{ "QUERY_STRING",		"query" },
	{ "REQUEST_LINE",		"clf-request" },
	{ "REQUEST_METHOD",		"method" },
	{ "SCRIPT_NAME",		"uri" },
	{ "SERVER_PROTOCOL",	"protocol" }
};
static size_t nsapi_reqpb_size = sizeof(nsapi_reqpb)/sizeof(nsapi_reqpb[0]);

static nsapi_equiv nsapi_vars[] = {
	{ "PATH_INFO",			"path-info" },
	{ "PATH_TRANSLATED",	"path" },
	{ "AUTH_TYPE",			"auth-type" },
	{ "CLIENT_CERT",		"auth-cert" },
	{ "REMOTE_USER",		"auth-user" }
};
static size_t nsapi_vars_size = sizeof(nsapi_vars)/sizeof(nsapi_vars[0]);

static nsapi_equiv nsapi_client[] = {
	{ "HTTPS_KEYSIZE",		"keysize" },
	{ "HTTPS_SECRETSIZE",	"secret-keysize" },
	{ "REMOTE_ADDR",		"ip" }
};
static size_t nsapi_client_size = sizeof(nsapi_client)/sizeof(nsapi_client[0]);

static int
sapi_nsapi_ub_write(const char *str, unsigned int str_length TSRMLS_DC)
{
	int retval;
	nsapi_request_context *rc;

	rc = (nsapi_request_context *)SG(server_context);
	retval = net_write(rc->sn->csd, (char *)str, str_length);
	if (retval == IO_ERROR /*-1*/ || retval == IO_EOF /*0*/)
		php_handle_aborted_connection();
	return retval;
}

static int
sapi_nsapi_header_handler(sapi_header_struct *sapi_header, sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	char *header_name, *header_content, *p;
	nsapi_request_context *rc = (nsapi_request_context *)SG(server_context);

	header_name = sapi_header->header;
	header_content = p = strchr(header_name, ':');
	if (p == NULL) {
		efree(sapi_header->header);
		return 0;
	}

	*p = 0;
	do {
		header_content++;
	} while (*header_content==' ');

	if (!strcasecmp(header_name, "Content-Type")) {
		param_free(pblock_remove("content-type", rc->rq->srvhdrs));
		pblock_nvinsert("content-type", header_content, rc->rq->srvhdrs);
	} else if (!strcasecmp(header_name, "Set-Cookie")) {
		pblock_nvinsert("set-cookie", header_content, rc->rq->srvhdrs);
	} else {
		pblock_nvinsert(header_name, header_content, rc->rq->srvhdrs);
	}

	*p = ':';	/* restore '*p' */

	efree(sapi_header->header);

	return 0;	/* don't use the default SAPI mechanism, NSAPI duplicates this functionality */
}

static int
sapi_nsapi_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	int retval;
	nsapi_request_context *rc = (nsapi_request_context *)SG(server_context);

	/*
	 * We could probably just do this in the header_handler. But, I
	 * don't know what the implication of doing it there is.
	 */
	if (SG(sapi_headers).send_default_content_type) {
		param_free(pblock_remove("content-type", rc->rq->srvhdrs));
		pblock_nvinsert("content-type", "text/html", rc->rq->srvhdrs);
	}

	protocol_status(rc->sn, rc->rq, SG(sapi_headers).http_response_code, NULL);
	retval = protocol_start_response(rc->sn, rc->rq);
	if (retval == REQ_PROCEED || retval == REQ_NOACTION)
		return SAPI_HEADER_SENT_SUCCESSFULLY;
	else
		return SAPI_HEADER_SEND_FAILED;
}

static int
sapi_nsapi_read_post(char *buffer, uint count_bytes TSRMLS_DC)
{
	nsapi_request_context *rc = (nsapi_request_context *)SG(server_context);
	char *read_ptr = buffer, *content_length_str = NULL;
	uint bytes_read = 0;
	int length, content_length = 0;
	netbuf *nbuf = rc->sn->inbuf;

	/*
	 *	Yesss!
	 */
	count_bytes = MIN(count_bytes, SG(request_info).content_length-rc->read_post_bytes);
	content_length = SG(request_info).content_length;

#if 0
	/*
	 * Determine the content-length. This will tell us the limit we can read.
	 */
	content_length_str = pblock_findval("content-length", rc->rq->headers);
	if (content_length_str != NULL) {
		content_length = strtol(content_length_str, 0, 0);
	}
#endif

	if (content_length <= 0)
		return 0;

	/*
	 * Gobble any pending data in the netbuf.
	 */
	length = nbuf->cursize - nbuf->pos;
	length = MIN(count_bytes, length);
	if (length > 0) {
		memcpy(read_ptr, nbuf->inbuf + nbuf->pos, length);
		bytes_read += length;
		read_ptr += length;
		content_length -= length;
		nbuf->pos += length;
	}

	/*
	 * Read the remaining from the socket.
	 */
	while (content_length > 0 && bytes_read < count_bytes) {
		int bytes_to_read = count_bytes - bytes_read;
		if (content_length < bytes_to_read)
			bytes_to_read = content_length;

		length = net_read(rc->sn->csd, read_ptr, bytes_to_read, NSAPI_READ_TIMEOUT);
		if (length == IO_ERROR || length == IO_EOF)
			break;

		bytes_read += length;
		read_ptr += length;
		content_length -= length;
	}

	if ( bytes_read > 0 )
		rc->read_post_bytes += bytes_read;
	return bytes_read;
}

static char *sapi_nsapi_read_cookies(TSRMLS_D)
{
	char *cookie_string;
	nsapi_request_context *rc = (nsapi_request_context *)SG(server_context);

	cookie_string = pblock_findval("cookie", rc->rq->headers);
	return cookie_string;
}

static void
sapi_nsapi_register_server_variables(zval *track_vars_array TSRMLS_DC)
{
	nsapi_request_context *rc = (nsapi_request_context *)SG(server_context);
	size_t i;
	char *value;
	char buf[128];

	for (i = 0; i < nsapi_reqpb_size; i++) {
		value = pblock_findval(nsapi_reqpb[i].nsapi_eq, rc->rq->reqpb);
		if (value) {
			php_register_variable( (char *)nsapi_reqpb[i].env_var, value, track_vars_array TSRMLS_CC );
		}
	}
	
	for (i = 0; i < nsapi_headers_size; i++) {
		value = pblock_findval(nsapi_headers[i].nsapi_eq, rc->rq->headers);
		if (value) {
			php_register_variable( (char *)nsapi_headers[i].env_var, value, track_vars_array TSRMLS_CC );
		}
	}

	for (i = 0; i < nsapi_vars_size; i++) {
		value = pblock_findval(nsapi_vars[i].nsapi_eq, rc->rq->vars);
		if (value) {
			php_register_variable( (char *)nsapi_vars[i].env_var, value, track_vars_array TSRMLS_CC );
		}
	}

	for (i = 0; i < nsapi_client_size; i++) {
		value = pblock_findval(nsapi_client[i].nsapi_eq, rc->sn->client);
		if (value) {
			php_register_variable( (char *)nsapi_client[i].env_var, value, track_vars_array TSRMLS_CC );
		}
	}
	
	value = session_dns(rc->sn);
	if (value) {
		php_register_variable("REMOTE_HOST", value, track_vars_array TSRMLS_CC );
	}
	sprintf(buf, "%d", conf_getglobals()->Vport);
	php_register_variable("SERVER_PORT", buf, track_vars_array TSRMLS_CC );
	if ((value = util_hostname())) {
		php_register_variable("SERVER_NAME", value, track_vars_array TSRMLS_CC );
	}	
	php_register_variable("SERVER_URL", http_uri2url("", ""), track_vars_array TSRMLS_CC );
	php_register_variable("HTTPS", (security_active ? "ON" : "OFF"), track_vars_array TSRMLS_CC );
/*	php_register_variable("SERVER_SOFTWARE", MAGNUS_VERSION_STRING, track_vars_array TSRMLS_CC ); */

	/*
	 * Special PHP_SELF variable.
	 */
	value = pblock_findval("uri", rc->rq->reqpb);
	if ( value != NULL ) {
		php_register_variable("PHP_SELF", value, track_vars_array TSRMLS_CC );
 	}
}
  
static void
nsapi_log_message(char *message)
{
	TSRMLS_FETCH();
	nsapi_request_context *rc = (nsapi_request_context *)SG(server_context);
	log_error(LOG_INFORM, "PHP_log_message", rc->sn, rc->rq,
		"%s", message);
}


static int php_nsapi_startup(sapi_module_struct *sapi_module)
{
	if (php_module_startup(sapi_module, NULL, 0)==FAILURE) {
		return FAILURE;
	}
	return SUCCESS;
}


static sapi_module_struct nsapi_sapi_module = {
	"nsapi",				/* name */
	"NSAPI",				/* pretty name */

	php_nsapi_startup,			/* startup */
	php_module_shutdown_wrapper,		/* shutdown */

	NULL,					/* activate */
	NULL,					/* deactivate */

	sapi_nsapi_ub_write,			/* unbuffered write */
	NULL,					/* flush */
	NULL,					/* get uid */
	NULL,					/* getenv */

	php_error,				/* error handler */

	sapi_nsapi_header_handler,		/* header handler */
	sapi_nsapi_send_headers,		/* send headers handler */
	NULL,					/* send header handler */

	sapi_nsapi_read_post,			/* read POST data */
	sapi_nsapi_read_cookies,		/* read Cookies */

	sapi_nsapi_register_server_variables,	/* register server variables */
	nsapi_log_message,			/* Log message */

	STANDARD_SAPI_MODULE_PROPERTIES
};

static char *
nsapi_strdup(char *str)
{
	if (str != NULL)
		return STRDUP(str);
	return NULL;
}

static void
nsapi_free(void *addr)
{
	if (addr != NULL)
		FREE(addr);
}

static void
nsapi_request_ctor(NSLS_D TSRMLS_DC)
{
	char *query_string = pblock_findval("query", NSG(rq)->reqpb);
	char *uri = pblock_findval("uri", NSG(rq)->reqpb);
	char *path_info = pblock_findval("path-info", NSG(rq)->vars);
	char *path_translated = pblock_findval("path", NSG(rq)->vars);
	char *request_method = pblock_findval("method", NSG(rq)->reqpb);
	char *content_type = pblock_findval("content-type", NSG(rq)->headers);
	char *content_length = pblock_findval("content-length", NSG(rq)->headers);

	if ((path_translated == NULL) && (uri != NULL))
		path_translated = request_translate_uri(uri, NSG(sn));

#if defined(NSAPI_DEBUG)
	log_error(LOG_INFORM, "nsapi_request_ctor", NSG(sn), NSG(rq),
		"query_string = %s, "
		"uri = %s, "
		"path_info = %s, "
		"path_translated = %s, "
		"request_method = %s, "
		"content_type = %s, "
		"content_length = %s",
		query_string,
		uri,
		path_info,
		path_translated,
		request_method,
		content_type,
		content_length);
#endif

	SG(request_info).query_string = nsapi_strdup(query_string);
	SG(request_info).request_uri = nsapi_strdup(uri);
	SG(request_info).request_method = nsapi_strdup(request_method);
	SG(request_info).path_translated = nsapi_strdup(path_translated);
	SG(request_info).content_type = nsapi_strdup(content_type);
	SG(request_info).content_length = (content_length == NULL) ? 0 : strtoul(content_length, 0, 0);
	SG(sapi_headers).http_response_code = 200;
}

static void
nsapi_request_dtor(NSLS_D TSRMLS_DC)
{
	nsapi_free(SG(request_info).query_string);
	nsapi_free(SG(request_info).request_uri);
	nsapi_free(SG(request_info).request_method);
	nsapi_free(SG(request_info).path_translated);
	nsapi_free(SG(request_info).content_type);
}

int 
nsapi_module_main(NSLS_D TSRMLS_DC)
{
	zend_file_handle file_handle;

	if (php_request_startup(TSRMLS_C) == FAILURE) {
		return FAILURE;
	}

	file_handle.type = ZEND_HANDLE_FILENAME;
	file_handle.filename = SG(request_info).path_translated;
	file_handle.free_filename = 0;
	file_handle.opened_path = NULL;

#if defined(NSAPI_DEBUG)
	log_error(LOG_INFORM, "nsapi_module_main", NSG(sn), NSG(rq),
		"Parsing [%s]", SG(request_info).path_translated);
#endif

	php_execute_script(&file_handle TSRMLS_CC);
	php_request_shutdown(NULL);

#if defined(NSAPI_DEBUG)
	log_error(LOG_INFORM, "nsapi_module_main", NSG(sn), NSG(rq),
		"PHP request finished Ok");
#endif
	return SUCCESS;
}

void NSAPI_PUBLIC 
php4_close(void *vparam)
{
	if (nsapi_sapi_module.shutdown) {
		nsapi_sapi_module.shutdown(&nsapi_sapi_module);
	}
	tsrm_shutdown();
}

int NSAPI_PUBLIC
php4_init(pblock *pb, Session *sn, Request *rq)
{
	php_core_globals *core_globals;

	tsrm_startup(1, 1, 0, NULL);
	core_globals = ts_resource(core_globals_id);

	sapi_startup(&nsapi_sapi_module);
	nsapi_sapi_module.startup(&nsapi_sapi_module);

	log_error(LOG_INFORM, "php4_init", sn, rq, "Initialized PHP Module\n");
	return REQ_PROCEED;
}

int NSAPI_PUBLIC
php4_execute(pblock *pb, Session *sn, Request *rq)
{
	int retval;
	nsapi_request_context *request_context;

	TSRMLS_FETCH();

	request_context = (nsapi_request_context *)MALLOC(sizeof(nsapi_request_context));
	request_context->pb = pb;
	request_context->sn = sn;
	request_context->rq = rq;
	request_context->read_post_bytes = 0;

	SG(server_context) = request_context;

	nsapi_request_ctor(NSLS_C TSRMLS_CC);
	retval = nsapi_module_main(NSLS_C TSRMLS_CC);
	nsapi_request_dtor(NSLS_C TSRMLS_CC);

	FREE(request_context);

	return (retval == SUCCESS) ? REQ_PROCEED : REQ_EXIT;
}

/*********************************************************
/ authentication
/
/ we have to make a 'fake' authenticator for netscape so it
/ will pass authentication through to php, and allow us to
/ check authentication with our scripts.
/
/ php4_auth_trans
/   main function called from netscape server to authenticate
/   a line in obj.conf:
/		funcs=php4_auth_trans shlib="path/to/this/phpnsapi.dll"
/	and:
/		<Object ppath="path/to/be/authenticated/by/php/*">
/		AuthTrans fn="php4_auth_trans"
/*********************************************************/
int NSAPI_PUBLIC 
php4_auth_trans(pblock * pb, Session * sn, Request * rq)
{
	/*This is a DO NOTHING function that allows authentication information
	to be passed through to PHP scripts.*/
	return REQ_PROCEED;
}
