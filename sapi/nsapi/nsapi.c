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
   |         Uwe Schindler <uwe@thetaphi.de>                              |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/*
 * PHP includes
 */
#define NSAPI 1

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

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
#include <sys/types.h>
#include <sys/stat.h>

#ifndef RTLD_DEFAULT
#define RTLD_DEFAULT NULL
#endif

/*
 * If neither XP_UNIX not XP_WIN32 is defined use PHP_WIN32
 */
#if !defined(XP_UNIX) && !defined(XP_WIN32)
#ifdef PHP_WIN32
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

#define NSLS_D		struct nsapi_request_context *request_context
#define NSLS_DC		, NSLS_D
#define NSLS_C		request_context
#define NSLS_CC		, NSLS_C
#define NSG(v)		(request_context->v)

#define NS_BUF_SIZE 2048

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
	{ "CONTENT_TYPE",			"content-type" }
};
static size_t nsapi_headers_size = sizeof(nsapi_headers)/sizeof(nsapi_headers[0]);

static nsapi_equiv nsapi_reqpb[] = {
	{ "QUERY_STRING",		"query" },
	{ "REQUEST_LINE",		"clf-request" },
	{ "REQUEST_METHOD",		"method" },
	{ "PHP_SELF",			"uri" },
	{ "SERVER_PROTOCOL",	"protocol" }
};
static size_t nsapi_reqpb_size = sizeof(nsapi_reqpb)/sizeof(nsapi_reqpb[0]);

static nsapi_equiv nsapi_vars[] = {
	{ "PATH_INFO",			"path-info" },
	{ "SCRIPT_FILENAME",	"path" },
	{ "AUTH_TYPE",			"auth-type" },
	{ "CLIENT_CERT",		"auth-cert" },
	{ "REMOTE_USER",		"auth-user" }
};
static size_t nsapi_vars_size = sizeof(nsapi_vars)/sizeof(nsapi_vars[0]);

static nsapi_equiv nsapi_client[] = {
	{ "HTTPS_KEYSIZE",		"keysize" },
	{ "HTTPS_SECRETSIZE",	"secret-keysize" },
	{ "REMOTE_ADDR",		"ip" },
	{ "REMOTE_HOST",		"ip" }
};
static size_t nsapi_client_size = sizeof(nsapi_client)/sizeof(nsapi_client[0]);

static char *nsapi_strdup(char *str)
{
	if (str != NULL) {
		return STRDUP(str);
	}
	return NULL;
}

static void nsapi_free(void *addr)
{
	if (addr != NULL) {
		FREE(addr);
	}
}


/*******************/
/* PHP module part */
/*******************/

PHP_MINIT_FUNCTION(nsapi);
PHP_MSHUTDOWN_FUNCTION(nsapi);
PHP_RINIT_FUNCTION(nsapi);
PHP_RSHUTDOWN_FUNCTION(nsapi);
PHP_MINFO_FUNCTION(nsapi);

PHP_FUNCTION(virtual);

ZEND_BEGIN_MODULE_GLOBALS(nsapi)
	long read_timeout;
ZEND_END_MODULE_GLOBALS(nsapi)

ZEND_DECLARE_MODULE_GLOBALS(nsapi)

#define NSAPI_G(v) TSRMG(nsapi_globals_id, zend_nsapi_globals *, v)

/* compatibility with PHP4_3 */
#if !defined(OnUpdateLong)
#define OnUpdateLong OnUpdateInt
#endif

/* {{{ nsapi_functions[]
 *
 * Every user visible function must have an entry in nsapi_functions[].
 */
function_entry nsapi_functions[] = {
	PHP_FE(virtual,	NULL)		/* Make subrequest */
	{NULL, NULL, NULL}	/* Must be the last line in nsapi_functions[] */
};
/* }}} */

/* {{{ nsapi_module_entry
 */
zend_module_entry nsapi_module_entry = {
	STANDARD_MODULE_HEADER,
	"nsapi",
	nsapi_functions,
	PHP_MINIT(nsapi),
	PHP_MSHUTDOWN(nsapi),
	NULL,
	NULL,
	PHP_MINFO(nsapi),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("nsapi.read_timeout", "60", PHP_INI_ALL, OnUpdateLong, read_timeout, zend_nsapi_globals, nsapi_globals)
PHP_INI_END()
/* }}} */

/* newer servers hide this functions from the programmer so redefine the functions dynamically
   thanks to Chris Elving from Sun for the function declarations */
typedef int (*nsapi_servact_prototype)(Session *sn, Request *rq);
nsapi_servact_prototype nsapi_servact_uri2path = NULL;
nsapi_servact_prototype nsapi_servact_pathchecks = NULL;
nsapi_servact_prototype nsapi_servact_fileinfo = NULL;
nsapi_servact_prototype nsapi_servact_service = NULL;

#ifdef PHP_WIN32
/* The following dll-names for nsapi are in use at this time. The undocumented
 * servact_* functions are always in the newest one, older ones are supported by
 * the server only by wrapping the function table nothing else. So choose
 * the newest one found in process space for dynamic linking */
char *nsapi_dlls[] = { "ns-httpd40.dll", "ns-httpd36.dll", "ns-httpd35.dll", "ns-httpd30.dll", NULL };
#endif

/* {{{ php_nsapi_init_dynamic_symbols
 */
static void php_nsapi_init_dynamic_symbols(void)
{
#if defined(servact_uri2path) && defined(servact_pathchecks) && defined(servact_fileinfo) && defined(servact_service)
	/* use functions from nsapi.h if available */
	nsapi_servact_uri2path = &servact_uri2path;
	nsapi_servact_pathchecks = &servact_pathchecks;
	nsapi_servact_fileinfo = &servact_fileinfo;
	nsapi_servact_service = &servact_service;
#else
	/* find address of internal NSAPI functions */
#ifdef PHP_WIN32
	register int i;
	DL_HANDLE module = NULL;
	/* find a LOADED dll module from nsapi_dlls */
	for (i=0; nsapi_dlls[i]; i++) {
		if (module = GetModuleHandle(nsapi_dlls[i])) {
			break;
		}
	}
	if (!module) return;
#else
	DL_HANDLE module = RTLD_DEFAULT;
#endif
	nsapi_servact_uri2path = (nsapi_servact_prototype)DL_FETCH_SYMBOL(module, "INTservact_uri2path");
	nsapi_servact_pathchecks = (nsapi_servact_prototype)DL_FETCH_SYMBOL(module, "INTservact_pathchecks");
	nsapi_servact_fileinfo = (nsapi_servact_prototype)DL_FETCH_SYMBOL(module, "INTservact_fileinfo");
	nsapi_servact_service = (nsapi_servact_prototype)DL_FETCH_SYMBOL(module, "INTservact_service");
	if (!(nsapi_servact_uri2path && nsapi_servact_pathchecks && nsapi_servact_fileinfo && nsapi_servact_service)) {
		/* not found - could be cause they are undocumented */
		nsapi_servact_uri2path = NULL;
		nsapi_servact_pathchecks = NULL;
		nsapi_servact_fileinfo = NULL;
		nsapi_servact_service = NULL;
	}
#endif
}
/* }}} */

/* {{{ php_nsapi_init_globals
 */
static void php_nsapi_init_globals(zend_nsapi_globals *nsapi_globals)
{
	nsapi_globals->read_timeout = 60;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(nsapi)
{
	php_nsapi_init_dynamic_symbols();
	ZEND_INIT_MODULE_GLOBALS(nsapi, php_nsapi_init_globals, NULL);
	REGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(nsapi)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(nsapi)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "NSAPI support", "enabled");
	php_info_print_table_row(2, "Server Software", system_version());
	php_info_print_table_row(2, "Sub-requests with virtual()",
	 (nsapi_servact_service)?((zend_ini_long("zlib.output_compression", sizeof("zlib.output_compression"), 0))?"not supported with zlib.output_compression":"enabled"):"not supported on this platform" );
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ proto bool virtual(string filename)
   Perform an NSAPI sub-request */
/* This function is equivalent to <!--#include virtual...-->
 * in SSI. It does an NSAPI sub-request. It is useful
 * for including CGI scripts or .shtml files, or anything else
 * that you'd parse through webserver.
 */
PHP_FUNCTION(virtual)
{
	pval **uri;
	int rv;
	char *value;
	Request *rq;
	nsapi_request_context *rc = (nsapi_request_context *)SG(server_context);

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &uri) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(uri);

	if (!nsapi_servact_service) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to include uri '%s' - Sub-requests not supported on this platform", (*uri)->value.str.val);
		RETURN_FALSE;
	} else if (zend_ini_long("zlib.output_compression", sizeof("zlib.output_compression"), 0)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to include uri '%s' - Sub-requests do not work with zlib.output_compression", (*uri)->value.str.val);
		RETURN_FALSE;
	} else {
		php_end_ob_buffers(1 TSRMLS_CC);
		php_header();

		/* do the sub-request */
		/* thanks to Chris Elving from Sun for this code sniplet */
		if ((rq = request_restart_internal((*uri)->value.str.val, NULL)) == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to include uri '%s' - Internal request creation failed", (*uri)->value.str.val);
			RETURN_FALSE;
		}

		/* insert host of current request to get page from same vhost */
		param_free(pblock_remove("host", rq->headers));
		if (value = pblock_findval("host", rc->rq->headers)) {
			pblock_nvinsert("host", value, rq->headers);
		}

		/* go through the normal request stages as given in obj.conf,
		   but leave out the logging/error section */
		do {
			rv = (*nsapi_servact_uri2path)(rc->sn, rq);
			if (rv != REQ_PROCEED) {
				continue;
			}

			rv = (*nsapi_servact_pathchecks)(rc->sn, rq);
			if (rv != REQ_PROCEED) {
				continue;
			}

			rv = (*nsapi_servact_fileinfo)(rc->sn, rq);
			if (rv != REQ_PROCEED) {
				continue;
			}

			rv = (*nsapi_servact_service)(rc->sn, rq);
		} while (rv == REQ_RESTART);

		if (rq->status_num != 200) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to include uri '%s' - HTTP status code %d during subrequest", (*uri)->value.str.val, rq->status_num);
			request_free(rq);
			RETURN_FALSE;
		}

		request_free(rq);

		RETURN_TRUE;
	}
}
/* }}} */


/*************/
/* SAPI part */
/*************/

static int sapi_nsapi_ub_write(const char *str, unsigned int str_length TSRMLS_DC)
{
	int retval;
	nsapi_request_context *rc;

	rc = (nsapi_request_context *)SG(server_context);
	retval = net_write(rc->sn->csd, (char *)str, str_length);
	if (retval == IO_ERROR /* -1 */ || retval == IO_EOF /* 0 */) {
		php_handle_aborted_connection();
	}
	return retval;
}

static int sapi_nsapi_header_handler(sapi_header_struct *sapi_header, sapi_headers_struct *sapi_headers TSRMLS_DC)
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
	} while (*header_content == ' ');

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

static int sapi_nsapi_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC)
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

	if (retval == REQ_PROCEED || retval == REQ_NOACTION) {
		return SAPI_HEADER_SENT_SUCCESSFULLY;
	} else {
		return SAPI_HEADER_SEND_FAILED;
	}
}

static int sapi_nsapi_read_post(char *buffer, uint count_bytes TSRMLS_DC)
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

	if (content_length <= 0) {
		return 0;
	}

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

		if (content_length < bytes_to_read) {
			bytes_to_read = content_length;
		}

		length = net_read(rc->sn->csd, read_ptr, bytes_to_read, NSAPI_G(read_timeout));

		if (length == IO_ERROR || length == IO_EOF) {
			break;
		}

		bytes_read += length;
		read_ptr += length;
		content_length -= length;
	}

	if ( bytes_read > 0 ) {
		rc->read_post_bytes += bytes_read;
	}
	return bytes_read;
}

static char *sapi_nsapi_read_cookies(TSRMLS_D)
{
	char *cookie_string;
	nsapi_request_context *rc = (nsapi_request_context *)SG(server_context);

	cookie_string = pblock_findval("cookie", rc->rq->headers);
	return cookie_string;
}

static void sapi_nsapi_register_server_variables(zval *track_vars_array TSRMLS_DC)
{
	nsapi_request_context *rc = (nsapi_request_context *)SG(server_context);
	register size_t i;
	char *value,*p;
	char buf[NS_BUF_SIZE + 1];
	struct pb_entry *entry;

	for (i = 0; i < nsapi_reqpb_size; i++) {
		value = pblock_findval(nsapi_reqpb[i].nsapi_eq, rc->rq->reqpb);
		if (value) {
			php_register_variable((char *)nsapi_reqpb[i].env_var, value, track_vars_array TSRMLS_CC);
		}
	}

	for (i = 0; i < nsapi_headers_size; i++) {
		value = pblock_findval(nsapi_headers[i].nsapi_eq, rc->rq->headers);
		if (value) {
			php_register_variable((char *)nsapi_headers[i].env_var, value, track_vars_array TSRMLS_CC);
		}
	}

	for (i=0; i < rc->rq->headers->hsize; i++) {
		entry=rc->rq->headers->ht[i];
		while (entry) {
			snprintf(buf, NS_BUF_SIZE, "HTTP_%s", entry->param->name);
			for(p = buf + 5; *p; p++) {
				*p = toupper(*p);
				if (*p < 'A' || *p > 'Z') {
					*p = '_';
				}
			}
			buf[NS_BUF_SIZE]='\0';
			php_register_variable(buf, entry->param->value, track_vars_array TSRMLS_CC);
			entry=entry->next;
		}
  	}

	for (i = 0; i < nsapi_vars_size; i++) {
		value = pblock_findval(nsapi_vars[i].nsapi_eq, rc->rq->vars);
		if (value) {
			php_register_variable((char *)nsapi_vars[i].env_var, value, track_vars_array TSRMLS_CC);
		}
	}

	for (i = 0; i < nsapi_client_size; i++) {
		value = pblock_findval(nsapi_client[i].nsapi_eq, rc->sn->client);
		if (value) {
			php_register_variable((char *)nsapi_client[i].env_var, value, track_vars_array TSRMLS_CC);
		}
	}

	if (value = session_dns(rc->sn)) {
		php_register_variable("REMOTE_HOST", value, track_vars_array TSRMLS_CC);
		nsapi_free(value);
	}

	sprintf(buf, "%d", conf_getglobals()->Vport);
	php_register_variable("SERVER_PORT", buf, track_vars_array TSRMLS_CC);
	php_register_variable("SERVER_NAME", conf_getglobals()->Vserver_hostname, track_vars_array TSRMLS_CC);

	value = http_uri2url_dynamic("", "", rc->sn, rc->rq);
	php_register_variable("SERVER_URL", value, track_vars_array TSRMLS_CC);
	nsapi_free(value);

	php_register_variable("SERVER_SOFTWARE", system_version(), track_vars_array TSRMLS_CC);
	php_register_variable("HTTPS", (security_active ? "ON" : "OFF"), track_vars_array TSRMLS_CC);
	php_register_variable("GATEWAY_INTERFACE", "CGI/1.1", track_vars_array TSRMLS_CC);

	/* DOCUMENT_ROOT */
	if (value = request_translate_uri("/", rc->sn)) {
	  	value[strlen(value) - 1] = 0;
		php_register_variable("DOCUMENT_ROOT", value, track_vars_array TSRMLS_CC);
		nsapi_free(value);
	}

	/* PATH_TRANSLATED */
	if (value = pblock_findval("path-info", rc->rq->vars)) {
		if (value = request_translate_uri(value, rc->sn)) {
			php_register_variable("PATH_TRANSLATED", value, track_vars_array TSRMLS_CC);
			nsapi_free(value);
		}
	}

	/* Create full Request-URI */
	if (value = pblock_findval("uri", rc->rq->reqpb)) {
		strncpy(buf, value, NS_BUF_SIZE);
		buf[NS_BUF_SIZE]='\0';
		if (value = pblock_findval("query", rc->rq->reqpb)) {
		  	p = strchr(buf, 0);
			snprintf(p, NS_BUF_SIZE-(p-buf), "?%s", value);
			buf[NS_BUF_SIZE]='\0';
		}
		php_register_variable("REQUEST_URI", buf, track_vars_array TSRMLS_CC);
  	}

	/* Create Script-Name */
	if (value = pblock_findval("uri", rc->rq->reqpb)) {
		strncpy(buf, value, NS_BUF_SIZE);
		buf[NS_BUF_SIZE]='\0';
		if (value = pblock_findval("path-info", rc->rq->vars)) {
			buf[strlen(buf) - strlen(value)] = '\0';
		}
		php_register_variable("SCRIPT_NAME", buf, track_vars_array TSRMLS_CC);
	}
}

static void nsapi_log_message(char *message)
{
	TSRMLS_FETCH();
	nsapi_request_context *rc = (nsapi_request_context *)SG(server_context);

	log_error(LOG_INFORM, "PHP module", rc->sn, rc->rq, "%s", message);
}

static int php_nsapi_startup(sapi_module_struct *sapi_module)
{
	if (php_module_startup(sapi_module, &nsapi_module_entry, 1)==FAILURE) {
		return FAILURE;
	}
	return SUCCESS;
}


static sapi_module_struct nsapi_sapi_module = {
	"nsapi",                                /* name */
	"NSAPI",                                /* pretty name */

	php_nsapi_startup,                      /* startup */
	php_module_shutdown_wrapper,            /* shutdown */

	NULL,                                   /* activate */
	NULL,                                   /* deactivate */

	sapi_nsapi_ub_write,                    /* unbuffered write */
	NULL,                                   /* flush */
	NULL,                                   /* get uid */
	NULL,                                   /* getenv */

	php_error,                              /* error handler */

	sapi_nsapi_header_handler,              /* header handler */
	sapi_nsapi_send_headers,                /* send headers handler */
	NULL,                                   /* send header handler */

	sapi_nsapi_read_post,                   /* read POST data */
	sapi_nsapi_read_cookies,                /* read Cookies */

	sapi_nsapi_register_server_variables,   /* register server variables */
	nsapi_log_message,                      /* Log message */

	NULL,                                   /* Block interruptions */
	NULL,                                   /* Unblock interruptions */

	STANDARD_SAPI_MODULE_PROPERTIES
};

static void nsapi_php_ini_entries(NSLS_D TSRMLS_DC)
{
	struct pb_entry *entry;
	register int i;

	for (i=0; i < NSG(pb)->hsize; i++) {
		entry=NSG(pb)->ht[i];
		while (entry) {
			/* exclude standard entries given to "Service" which should not go into ini entries */
			if (strcasecmp(entry->param->name,"fn") && strcasecmp(entry->param->name,"type")
			 && strcasecmp(entry->param->name,"method")  && strcasecmp(entry->param->name,"directive")) {
				/* change the ini entry */
				if (zend_alter_ini_entry(entry->param->name, strlen(entry->param->name)+1,
				 entry->param->value, strlen(entry->param->value),
				 PHP_INI_SYSTEM, PHP_INI_STAGE_RUNTIME)==FAILURE) {
					log_error(LOG_WARN, "php4_execute", NSG(sn), NSG(rq), "Cannot change php.ini key \"%s\" to \"%s\"", entry->param->name, entry->param->value);
				}
			}
			entry=entry->next;
		}
  	}
}

void NSAPI_PUBLIC php4_close(void *vparam)
{
	if (nsapi_sapi_module.shutdown) {
		nsapi_sapi_module.shutdown(&nsapi_sapi_module);
	}

	if (nsapi_sapi_module.php_ini_path_override) {
		free(nsapi_sapi_module.php_ini_path_override);
	}

	tsrm_shutdown();

	log_error(LOG_INFORM, "php4_close", NULL, NULL, "Shutdown PHP Module");
}

int NSAPI_PUBLIC php4_init(pblock *pb, Session *sn, Request *rq)
{
	php_core_globals *core_globals;
	char *ini_path;
	int threads=128; /* default for server */

	/* fetch max threads from NSAPI and initialize TSRM with it */
#if defined(pool_maxthreads)
	threads=pool_maxthreads;
	if (threads<1) {
		threads=128; /* default for server */
	}
#endif
	tsrm_startup(threads, 1, 0, NULL);

	core_globals = ts_resource(core_globals_id);

	/* look if php_ini parameter is given to php4_init */
	if (ini_path = pblock_findval("php_ini", pb)) {
		nsapi_sapi_module.php_ini_path_override = strdup(ini_path);
	}

	/* start SAPI */
	sapi_startup(&nsapi_sapi_module);
	nsapi_sapi_module.startup(&nsapi_sapi_module);

	daemon_atrestart(&php4_close, NULL);

	log_error(LOG_INFORM, "php4_init", sn, rq, "Initialized PHP Module (%d threads exspected)", threads);
	return REQ_PROCEED;
}

int NSAPI_PUBLIC php4_execute(pblock *pb, Session *sn, Request *rq)
{
	int retval;
	nsapi_request_context *request_context;
	zend_file_handle file_handle = {0};
	struct stat fst;

	char *query_string    = pblock_findval("query", rq->reqpb);
	char *uri             = pblock_findval("uri", rq->reqpb);
	char *path_info       = pblock_findval("path-info", rq->vars);
	char *request_method  = pblock_findval("method", rq->reqpb);
	char *content_type    = pblock_findval("content-type", rq->headers);
	char *content_length  = pblock_findval("content-length", rq->headers);
	char *path_translated = pblock_findval("path", rq->vars);

	TSRMLS_FETCH();

	request_context = (nsapi_request_context *)MALLOC(sizeof(nsapi_request_context));
	request_context->pb = pb;
	request_context->sn = sn;
	request_context->rq = rq;
	request_context->read_post_bytes = 0;

	SG(server_context) = request_context;
	SG(request_info).query_string = nsapi_strdup(query_string);
	SG(request_info).request_uri = nsapi_strdup(uri);
	SG(request_info).request_method = nsapi_strdup(request_method);
	SG(request_info).path_translated = nsapi_strdup(path_translated);
	SG(request_info).content_type = nsapi_strdup(content_type);
	SG(request_info).content_length = (content_length == NULL) ? 0 : strtoul(content_length, 0, 0);
	SG(sapi_headers).http_response_code = 200;

	nsapi_php_ini_entries(NSLS_C TSRMLS_CC);

	file_handle.type = ZEND_HANDLE_FILENAME;
	file_handle.filename = SG(request_info).path_translated;
	file_handle.free_filename = 0;
	file_handle.opened_path = NULL;

	if (stat(SG(request_info).path_translated, &fst)==0 && S_ISREG(fst.st_mode)) {
		if (php_request_startup(TSRMLS_C) == SUCCESS) {
			php_execute_script(&file_handle TSRMLS_CC);
			php_request_shutdown(NULL);
			retval=REQ_PROCEED;
		} else {
			/* send 500 internal server error */
			log_error(LOG_WARN, "php4_execute", sn, rq, "Cannot prepare PHP engine!");
			protocol_status(sn, rq, 500, NULL);
			retval=REQ_ABORTED;
		}
	} else {
		/* send 404 because file not found */
		log_error(LOG_WARN, "php4_execute", sn, rq, "Cannot execute PHP script: %s", SG(request_info).path_translated);
		protocol_status(sn, rq, 404, NULL);
		retval=REQ_ABORTED;
	}

	nsapi_free(SG(request_info).query_string);
	nsapi_free(SG(request_info).request_uri);
	nsapi_free((void*)(SG(request_info).request_method));
	nsapi_free(SG(request_info).path_translated);
	nsapi_free((void*)(SG(request_info).content_type));

	FREE(request_context);

	return retval;
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
int NSAPI_PUBLIC php4_auth_trans(pblock * pb, Session * sn, Request * rq)
{
	/* This is a DO NOTHING function that allows authentication
	 * information
	 * to be passed through to PHP scripts.
	 */
	return REQ_PROCEED;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
