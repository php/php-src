/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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
 * The manual define of HPUX is to fix bug #46020, nsapi.h needs this to detect HPUX
 */
#ifdef __hpux
#define HPUX
#endif
 
/*
 * NSAPI includes
 */
#include "nsapi.h"

/* fix for gcc4 visibility issue */
#ifndef PHP_WIN32
# undef NSAPI_PUBLIC
# define NSAPI_PUBLIC PHPAPI
#endif

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
	char *path_info;
	int fixed_script; /* 0 if script is from URI, 1 if script is from "script" parameter */
	short http_error; /* 0 in normal mode; for errors the HTTP error code */
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

static nsapi_equiv nsapi_reqpb[] = {
	{ "QUERY_STRING",		"query" },
	{ "REQUEST_LINE",		"clf-request" },
	{ "REQUEST_METHOD",		"method" },
	{ "PHP_SELF",			"uri" },
	{ "SERVER_PROTOCOL",	"protocol" }
};
static size_t nsapi_reqpb_size = sizeof(nsapi_reqpb)/sizeof(nsapi_reqpb[0]);

static nsapi_equiv nsapi_vars[] = {
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

/* this parameters to "Service"/"Error" are NSAPI ones which should not be php.ini keys and are excluded */
static char *nsapi_exclude_from_ini_entries[] = { "fn", "type", "method", "directive", "code", "reason", "script", "bucket", NULL };

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

PHP_FUNCTION(nsapi_virtual);
PHP_FUNCTION(nsapi_request_headers);
PHP_FUNCTION(nsapi_response_headers);

ZEND_BEGIN_MODULE_GLOBALS(nsapi)
	long read_timeout;
ZEND_END_MODULE_GLOBALS(nsapi)

ZEND_DECLARE_MODULE_GLOBALS(nsapi)

#define NSAPI_G(v) TSRMG(nsapi_globals_id, zend_nsapi_globals *, v)


/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_nsapi_virtual, 0, 0, 1)
	ZEND_ARG_INFO(0, uri)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_nsapi_request_headers, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_nsapi_response_headers, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ nsapi_functions[]
 *
 * Every user visible function must have an entry in nsapi_functions[].
 */
const zend_function_entry nsapi_functions[] = {
	PHP_FE(nsapi_virtual,	arginfo_nsapi_virtual)						/* Make subrequest */
	PHP_FALIAS(virtual, nsapi_virtual, arginfo_nsapi_virtual)			/* compatibility */
	PHP_FE(nsapi_request_headers, arginfo_nsapi_request_headers)		/* get request headers */
	PHP_FALIAS(getallheaders, nsapi_request_headers, arginfo_nsapi_request_headers)	/* compatibility */
	PHP_FALIAS(apache_request_headers, nsapi_request_headers, arginfo_nsapi_request_headers)	/* compatibility */
	PHP_FE(nsapi_response_headers, arginfo_nsapi_response_headers)		/* get response headers */
	PHP_FALIAS(apache_response_headers, nsapi_response_headers, arginfo_nsapi_response_headers)	/* compatibility */
	{NULL, NULL, NULL}
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
static char *nsapi_dlls[] = { "ns-httpd40.dll", "ns-httpd36.dll", "ns-httpd35.dll", "ns-httpd30.dll", NULL };
/* if user specifies an other dll name by server_lib parameter 
 * it is placed in the following variable and only this DLL is
 * checked for the servact_* functions */
char *nsapi_dll = NULL;
#endif

/* {{{ php_nsapi_init_dynamic_symbols
 */
static void php_nsapi_init_dynamic_symbols(void)
{
	/* find address of internal NSAPI functions */
#ifdef PHP_WIN32
	register int i;
	DL_HANDLE module = NULL;
	if (nsapi_dll) {
		/* try user specified server_lib */
		module = GetModuleHandle(nsapi_dll);
		if (!module) {
			log_error(LOG_WARN, "php5_init", NULL, NULL, "Cannot find DLL specified by server_lib parameter: %s", nsapi_dll);
		}
	} else {
		/* find a LOADED dll module from nsapi_dlls */
		for (i=0; nsapi_dlls[i]; i++) {
			if (module = GetModuleHandle(nsapi_dlls[i])) {
				break;
			}
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
	php_info_print_table_row(2, "NSAPI Module Revision", "$Id$");
	php_info_print_table_row(2, "Server Software", system_version());
	php_info_print_table_row(2, "Sub-requests with nsapi_virtual()",
	 (nsapi_servact_service)?((zend_ini_long("zlib.output_compression", sizeof("zlib.output_compression"), 0))?"not supported with zlib.output_compression":"enabled"):"not supported on this platform" );
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ proto bool nsapi_virtual(string uri)
   Perform an NSAPI sub-request */
/* This function is equivalent to <!--#include virtual...-->
 * in SSI. It does an NSAPI sub-request. It is useful
 * for including CGI scripts or .shtml files, or anything else
 * that you'd parse through webserver.
 */
PHP_FUNCTION(nsapi_virtual)
{
	int uri_len,rv;
	char *uri,*value;
	Request *rq;
	nsapi_request_context *rc = (nsapi_request_context *)SG(server_context);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &uri, &uri_len) == FAILURE) {
		return;
	}

	if (!nsapi_servact_service) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to include uri '%s' - Sub-requests not supported on this platform", uri);
		RETURN_FALSE;
	} else if (zend_ini_long("zlib.output_compression", sizeof("zlib.output_compression"), 0)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to include uri '%s' - Sub-requests do not work with zlib.output_compression", uri);
		RETURN_FALSE;
	} else {
		php_output_end_all(TSRMLS_C);
		php_header(TSRMLS_C);

		/* do the sub-request */
		/* thanks to Chris Elving from Sun for this code sniplet */
		if ((rq = request_restart_internal(uri, NULL)) == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to include uri '%s' - Internal request creation failed", uri);
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
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to include uri '%s' - HTTP status code %d during subrequest", uri, rq->status_num);
			request_free(rq);
			RETURN_FALSE;
		}

		request_free(rq);

		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto array nsapi_request_headers(void)
   Get all headers from the request */
PHP_FUNCTION(nsapi_request_headers)
{
	register int i;
	struct pb_entry *entry;
	nsapi_request_context *rc = (nsapi_request_context *)SG(server_context);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	
	array_init(return_value);

	for (i=0; i < rc->rq->headers->hsize; i++) {
		entry=rc->rq->headers->ht[i];
		while (entry) {
			add_assoc_string(return_value, entry->param->name, entry->param->value, 1);
			entry=entry->next;
		}
  	}
}
/* }}} */

/* {{{ proto array nsapi_response_headers(void)
   Get all headers from the response */
PHP_FUNCTION(nsapi_response_headers)
{
	register int i;
	struct pb_entry *entry;
	nsapi_request_context *rc = (nsapi_request_context *)SG(server_context);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	
	array_init(return_value);

	for (i=0; i < rc->rq->srvhdrs->hsize; i++) {
		entry=rc->rq->srvhdrs->ht[i];
		while (entry) {
			add_assoc_string(return_value, entry->param->name, entry->param->value, 1);
			entry=entry->next;
		}
  	}
}
/* }}} */


/*************/
/* SAPI part */
/*************/

static int sapi_nsapi_ub_write(const char *str, unsigned int str_length TSRMLS_DC)
{
	int retval;
	nsapi_request_context *rc = (nsapi_request_context *)SG(server_context);
	
	if (!SG(headers_sent)) {
		sapi_send_headers(TSRMLS_C);
	}

	retval = net_write(rc->sn->csd, (char *)str, str_length);
	if (retval == IO_ERROR /* -1 */ || retval == IO_EOF /* 0 */) {
		php_handle_aborted_connection();
	}
	return retval;
}

/* modified version of apache2 */
static void sapi_nsapi_flush(void *server_context TSRMLS_DC)
{
	nsapi_request_context *rc = (nsapi_request_context *)server_context;
	
	if (!rc) {
		/* we have no context, so no flushing needed. This fixes a SIGSEGV on shutdown */
		return;
	}

	if (!SG(headers_sent)) {
		sapi_send_headers(TSRMLS_C);
	}

	/* flushing is only supported in iPlanet servers from version 6.1 on, make it conditional */
#if NSAPI_VERSION >= 302
	if (net_flush(rc->sn->csd) < 0) {
		php_handle_aborted_connection();
	}
#endif
}

/* callback for zend_llist_apply on SAPI_HEADER_DELETE_ALL operation */
static int php_nsapi_remove_header(sapi_header_struct *sapi_header TSRMLS_DC)
{
	char *header_name, *p;
	nsapi_request_context *rc = (nsapi_request_context *)SG(server_context);
	
	/* copy the header, because NSAPI needs reformatting and we do not want to change the parameter */
	header_name = pool_strdup(rc->sn->pool, sapi_header->header);

	/* extract name, this works, if only the header without ':' is given, too */
	if (p = strchr(header_name, ':')) {
		*p = 0;
	}
	
	/* header_name to lower case because NSAPI reformats the headers and wants lowercase */
	for (p=header_name; *p; p++) {
		*p=tolower(*p);
	}
	
	/* remove the header */
	param_free(pblock_remove(header_name, rc->rq->srvhdrs));
	pool_free(rc->sn->pool, header_name);
	
	return ZEND_HASH_APPLY_KEEP;
}

static int sapi_nsapi_header_handler(sapi_header_struct *sapi_header, sapi_header_op_enum op, sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	char *header_name, *header_content, *p;
	nsapi_request_context *rc = (nsapi_request_context *)SG(server_context);

	switch(op) {
		case SAPI_HEADER_DELETE_ALL:
			/* this only deletes headers set or overwritten by PHP, headers previously set by NSAPI are left intact */
			zend_llist_apply(&sapi_headers->headers, (llist_apply_func_t) php_nsapi_remove_header TSRMLS_CC);
			return 0;

		case SAPI_HEADER_DELETE:
			/* reuse the zend_llist_apply callback function for this, too */
			php_nsapi_remove_header(sapi_header TSRMLS_CC);
			return 0;

		case SAPI_HEADER_ADD:
		case SAPI_HEADER_REPLACE:
			/* copy the header, because NSAPI needs reformatting and we do not want to change the parameter */
			header_name = pool_strdup(rc->sn->pool, sapi_header->header);

			/* split header and align pointer for content */
			header_content = strchr(header_name, ':');
			if (header_content) {
				*header_content = 0;
				do {
					header_content++;
				} while (*header_content==' ');
				
				/* header_name to lower case because NSAPI reformats the headers and wants lowercase */
				for (p=header_name; *p; p++) {
					*p=tolower(*p);
				}

				/* if REPLACE, remove first.  "Content-type" is always removed, as SAPI has a bug according to this */
				if (op==SAPI_HEADER_REPLACE || strcmp(header_name, "content-type")==0) {
					param_free(pblock_remove(header_name, rc->rq->srvhdrs));
				}
				/* ADD header to nsapi table */
				pblock_nvinsert(header_name, header_content, rc->rq->srvhdrs);
			}
			
			pool_free(rc->sn->pool, header_name);
			return SAPI_HEADER_ADD;
			
		default:
			return 0;
	}
}

static int sapi_nsapi_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	int retval;
	nsapi_request_context *rc = (nsapi_request_context *)SG(server_context);

	if (SG(sapi_headers).send_default_content_type) {
		char *hd;
		param_free(pblock_remove("content-type", rc->rq->srvhdrs));
		hd = sapi_get_default_content_type(TSRMLS_C);
		pblock_nvinsert("content-type", hd, rc->rq->srvhdrs);
		efree(hd);
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
	int pos;
	char *value,*p;
	char buf[32];
	struct pb_entry *entry;

	for (i = 0; i < nsapi_reqpb_size; i++) {
		value = pblock_findval(nsapi_reqpb[i].nsapi_eq, rc->rq->reqpb);
		if (value) {
			php_register_variable((char *)nsapi_reqpb[i].env_var, value, track_vars_array TSRMLS_CC);
		}
	}

	for (i=0; i < rc->rq->headers->hsize; i++) {
		entry=rc->rq->headers->ht[i];
		while (entry) {
			if (strcasecmp(entry->param->name, "content-length")==0 || strcasecmp(entry->param->name, "content-type")==0) {
				value=estrdup(entry->param->name);
				pos = 0;
			} else {
				spprintf(&value, 0, "HTTP_%s", entry->param->name);
				pos = 5;
			}
			if (value) {
				for(p = value + pos; *p; p++) {
					*p = toupper(*p);
					if (!isalnum(*p)) {
						*p = '_';
					}
				}
				php_register_variable(value, entry->param->value, track_vars_array TSRMLS_CC);
				efree(value);
			}
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

	slprintf(buf, sizeof(buf), "%d", conf_getglobals()->Vport);
	php_register_variable("SERVER_PORT", buf, track_vars_array TSRMLS_CC);
	php_register_variable("SERVER_NAME", conf_getglobals()->Vserver_hostname, track_vars_array TSRMLS_CC);

	value = http_uri2url_dynamic("", "", rc->sn, rc->rq);
	php_register_variable("SERVER_URL", value, track_vars_array TSRMLS_CC);
	nsapi_free(value);

	php_register_variable("SERVER_SOFTWARE", system_version(), track_vars_array TSRMLS_CC);
	if (security_active) {
		php_register_variable("HTTPS", "ON", track_vars_array TSRMLS_CC);
	}
	php_register_variable("GATEWAY_INTERFACE", "CGI/1.1", track_vars_array TSRMLS_CC);

	/* DOCUMENT_ROOT */
	if (value = request_translate_uri("/", rc->sn)) {
		pos = strlen(value);
		php_register_variable_safe("DOCUMENT_ROOT", value, pos-1, track_vars_array TSRMLS_CC);
		nsapi_free(value);
	}

	/* PATH_INFO / PATH_TRANSLATED */
	if (rc->path_info) {
		if (value = request_translate_uri(rc->path_info, rc->sn)) {
			php_register_variable("PATH_TRANSLATED", value, track_vars_array TSRMLS_CC);
			nsapi_free(value);
		}
		php_register_variable("PATH_INFO", rc->path_info, track_vars_array TSRMLS_CC);
	}

	/* Create full Request-URI & Script-Name */
	if (SG(request_info).request_uri) {
		pos = strlen(SG(request_info).request_uri);
		
		if (SG(request_info).query_string) {
			spprintf(&value, 0, "%s?%s", SG(request_info).request_uri, SG(request_info).query_string);
			if (value) {
				php_register_variable("REQUEST_URI", value, track_vars_array TSRMLS_CC);
				efree(value);
			}
		} else {
			php_register_variable_safe("REQUEST_URI", SG(request_info).request_uri, pos, track_vars_array TSRMLS_CC);
		}

		if (rc->path_info) {
			pos -= strlen(rc->path_info);
			if (pos<0) {
				pos = 0;
			}
		}
		php_register_variable_safe("SCRIPT_NAME", SG(request_info).request_uri, pos, track_vars_array TSRMLS_CC);
	}
	php_register_variable("SCRIPT_FILENAME", SG(request_info).path_translated, track_vars_array TSRMLS_CC);

	/* special variables in error mode */
	if (rc->http_error) {
		slprintf(buf, sizeof(buf), "%d", rc->http_error);
		php_register_variable("ERROR_TYPE", buf, track_vars_array TSRMLS_CC);
	}
}

static void nsapi_log_message(char *message TSRMLS_DC)
{
	nsapi_request_context *rc = (nsapi_request_context *)SG(server_context);

	if (rc) {
		log_error(LOG_INFORM, pblock_findval("fn", rc->pb), rc->sn, rc->rq, "%s", message);
	} else {
		log_error(LOG_INFORM, "php5", NULL, NULL, "%s", message);
	}
}

static double sapi_nsapi_get_request_time(TSRMLS_D)
{
	return REQ_TIME( ((nsapi_request_context *)SG(server_context))->rq );
}

static int php_nsapi_startup(sapi_module_struct *sapi_module)
{
	if (php_module_startup(sapi_module, &nsapi_module_entry, 1)==FAILURE) {
		return FAILURE;
	}
	return SUCCESS;
}

static struct stat* sapi_nsapi_get_stat(TSRMLS_D)
{
	return request_stat_path(
		SG(request_info).path_translated,
		((nsapi_request_context *)SG(server_context))->rq
	);
}

static sapi_module_struct nsapi_sapi_module = {
	"nsapi",                                /* name */
	"NSAPI",                                /* pretty name */

	php_nsapi_startup,                      /* startup */
	php_module_shutdown_wrapper,            /* shutdown */

	NULL,                                   /* activate */
	NULL,                                   /* deactivate */

	sapi_nsapi_ub_write,                    /* unbuffered write */
	sapi_nsapi_flush,                       /* flush */
	sapi_nsapi_get_stat,                    /* get uid/stat */
	NULL,                                   /* getenv */

	php_error,                              /* error handler */

	sapi_nsapi_header_handler,              /* header handler */
	sapi_nsapi_send_headers,                /* send headers handler */
	NULL,                                   /* send header handler */

	sapi_nsapi_read_post,                   /* read POST data */
	sapi_nsapi_read_cookies,                /* read Cookies */

	sapi_nsapi_register_server_variables,   /* register server variables */
	nsapi_log_message,                      /* Log message */
	sapi_nsapi_get_request_time,			/* Get request time */
	NULL,									/* Child terminate */

	NULL,                                   /* Block interruptions */
	NULL,                                   /* Unblock interruptions */

	STANDARD_SAPI_MODULE_PROPERTIES
};

static void nsapi_php_ini_entries(NSLS_D TSRMLS_DC)
{
	struct pb_entry *entry;
	register int i,j,ok;

	for (i=0; i < NSG(pb)->hsize; i++) {
		entry=NSG(pb)->ht[i];
		while (entry) {
			/* exclude standard entries given to "Service" which should not go into ini entries */
			ok=1;
			for (j=0; nsapi_exclude_from_ini_entries[j]; j++) {
				ok&=(strcasecmp(entry->param->name, nsapi_exclude_from_ini_entries[j])!=0);
			}

			if (ok) {
				/* change the ini entry */
				if (zend_alter_ini_entry(entry->param->name, strlen(entry->param->name)+1,
				 entry->param->value, strlen(entry->param->value),
				 PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE)==FAILURE) {
					log_error(LOG_WARN, pblock_findval("fn", NSG(pb)), NSG(sn), NSG(rq), "Cannot change php.ini key \"%s\" to \"%s\"", entry->param->name, entry->param->value);
				}
			}
			entry=entry->next;
		}
  	}
}

void NSAPI_PUBLIC php5_close(void *vparam)
{
	if (nsapi_sapi_module.shutdown) {
		nsapi_sapi_module.shutdown(&nsapi_sapi_module);
	}

	if (nsapi_sapi_module.php_ini_path_override) {
		free(nsapi_sapi_module.php_ini_path_override);
	}
	
#ifdef PHP_WIN32
	if (nsapi_dll) {
		free(nsapi_dll);
		nsapi_dll = NULL;
	}
#endif	

	sapi_shutdown();
	tsrm_shutdown();

	log_error(LOG_INFORM, "php5_close", NULL, NULL, "Shutdown PHP Module");
}

/*********************************************************
/ init SAF
/
/ Init fn="php5_init" [php_ini="/path/to/php.ini"] [server_lib="ns-httpdXX.dll"]
/   Initialize the NSAPI module in magnus.conf
/
/ php_ini: gives path to php.ini file
/ server_lib: (only Win32) gives name of DLL (without path) to look for
/  servact_* functions
/
/*********************************************************/
int NSAPI_PUBLIC php5_init(pblock *pb, Session *sn, Request *rq)
{
	php_core_globals *core_globals;
	char *strval;
	int threads=128; /* default for server */

	/* fetch max threads from NSAPI and initialize TSRM with it */
	threads=conf_getglobals()->Vpool_maxthreads;
	if (threads<1) {
		threads=128; /* default for server */
	}
	tsrm_startup(threads, 1, 0, NULL);

	core_globals = ts_resource(core_globals_id);

	/* look if php_ini parameter is given to php5_init */
	if (strval = pblock_findval("php_ini", pb)) {
		nsapi_sapi_module.php_ini_path_override = strdup(strval);
	}
	
#ifdef PHP_WIN32
	/* look if server_lib parameter is given to php5_init
	 * (this disables the automatic search for the newest ns-httpdXX.dll) */
	if (strval = pblock_findval("server_lib", pb)) {
		nsapi_dll = strdup(strval);
	}
#endif	

	/* start SAPI */
	sapi_startup(&nsapi_sapi_module);
	nsapi_sapi_module.startup(&nsapi_sapi_module);

	daemon_atrestart(&php5_close, NULL);

	log_error(LOG_INFORM, pblock_findval("fn", pb), sn, rq, "Initialized PHP Module (%d threads expected)", threads);
	return REQ_PROCEED;
}

/*********************************************************
/ normal use in Service directive:
/
/ Service fn="php5_execute" type=... method=... [inikey=inivalue inikey=inivalue...]
/
/ use in Service for a directory to supply a php-made directory listing instead of server default:
/
/ Service fn="php5_execute" type="magnus-internal/directory" script="/path/to/script.php" [inikey=inivalue inikey=inivalue...]
/
/ use in Error SAF to display php script as error page:
/
/ Error fn="php5_execute" code=XXX script="/path/to/script.php" [inikey=inivalue inikey=inivalue...]
/ Error fn="php5_execute" reason="Reason" script="/path/to/script.php" [inikey=inivalue inikey=inivalue...]
/
/*********************************************************/
int NSAPI_PUBLIC php5_execute(pblock *pb, Session *sn, Request *rq)
{
	int retval;
	nsapi_request_context *request_context;
	zend_file_handle file_handle = {0};
	struct stat *fst;

	char *path_info;
	char *query_string    = pblock_findval("query", rq->reqpb);
	char *uri             = pblock_findval("uri", rq->reqpb);
	char *request_method  = pblock_findval("method", rq->reqpb);
	char *content_type    = pblock_findval("content-type", rq->headers);
	char *content_length  = pblock_findval("content-length", rq->headers);
	char *directive       = pblock_findval("Directive", pb);
	int error_directive   = (directive && !strcasecmp(directive, "error"));
	int fixed_script      = 1;

	/* try to use script parameter -> Error or Service for directory listing */
	char *path_translated = pblock_findval("script", pb);

	TSRMLS_FETCH();

	/* if script parameter is missing: normal use as Service SAF  */
	if (!path_translated) {
		path_translated = pblock_findval("path", rq->vars);
		path_info       = pblock_findval("path-info", rq->vars);
		fixed_script = 0;
		if (error_directive) {
			/* go to next error directive if script parameter is missing */
			log_error(LOG_WARN, pblock_findval("fn", pb), sn, rq, "Missing 'script' parameter");
			return REQ_NOACTION;
		}
	} else {
		/* in error the path_info is the uri to the requested page */
		path_info = pblock_findval("uri", rq->reqpb);
	}

	/* check if this uri was included in an other PHP script with nsapi_virtual()
	   by looking for a request context in the current thread */
	if (SG(server_context)) {
		/* send 500 internal server error */
		log_error(LOG_WARN, pblock_findval("fn", pb), sn, rq, "Cannot make nesting PHP requests with nsapi_virtual()");
		if (error_directive) {
			return REQ_NOACTION;
		} else {
			protocol_status(sn, rq, 500, NULL);
			return REQ_ABORTED;
		}
	}

	request_context = (nsapi_request_context *)pool_malloc(sn->pool, sizeof(nsapi_request_context));
	if (!request_context) {
		log_error(LOG_CATASTROPHE, pblock_findval("fn", pb), sn, rq, "Insufficient memory to process PHP request!");
		return REQ_ABORTED;
	}
	request_context->pb = pb;
	request_context->sn = sn;
	request_context->rq = rq;
	request_context->read_post_bytes = 0;
	request_context->fixed_script = fixed_script;
	request_context->http_error = (error_directive) ? rq->status_num : 0;
	request_context->path_info = path_info;

	SG(server_context) = request_context;
	SG(request_info).query_string = query_string;
	SG(request_info).request_uri = uri;
	SG(request_info).request_method = request_method;
	SG(request_info).path_translated = path_translated;
	SG(request_info).content_type = content_type;
	SG(request_info).content_length = (content_length == NULL) ? 0 : strtoul(content_length, 0, 0);
	SG(sapi_headers).http_response_code = (error_directive) ? rq->status_num : 200;
	
	nsapi_php_ini_entries(NSLS_C TSRMLS_CC);

	php_handle_auth_data(pblock_findval("authorization", rq->headers) TSRMLS_CC);

	file_handle.type = ZEND_HANDLE_FILENAME;
	file_handle.filename = SG(request_info).path_translated;
	file_handle.free_filename = 0;
	file_handle.opened_path = NULL;

	fst = request_stat_path(SG(request_info).path_translated, rq);
	if (fst && S_ISREG(fst->st_mode)) {
		if (php_request_startup(TSRMLS_C) == SUCCESS) {
			php_execute_script(&file_handle TSRMLS_CC);
			php_request_shutdown(NULL);
			retval=REQ_PROCEED;
		} else {
			/* send 500 internal server error */
			log_error(LOG_WARN, pblock_findval("fn", pb), sn, rq, "Cannot prepare PHP engine!");
			if (error_directive) {
				retval=REQ_NOACTION;
			} else {
				protocol_status(sn, rq, 500, NULL);
				retval=REQ_ABORTED;
			}
		}
	} else {
		/* send 404 because file not found */
		log_error(LOG_WARN, pblock_findval("fn", pb), sn, rq, "Cannot execute PHP script: %s (File not found)", SG(request_info).path_translated);
		if (error_directive) {
			retval=REQ_NOACTION;
		} else {
			protocol_status(sn, rq, 404, NULL);
			retval=REQ_ABORTED;
		}
	}

	pool_free(sn->pool, request_context);
	SG(server_context) = NULL;

	return retval;
}

/*********************************************************
/ authentication
/
/ we have to make a 'fake' authenticator for netscape so it
/ will pass authentication through to php, and allow us to
/ check authentication with our scripts.
/
/ php5_auth_trans
/   main function called from netscape server to authenticate
/   a line in obj.conf:
/		funcs=php5_auth_trans shlib="path/to/this/phpnsapi.dll"
/	and:
/		<Object ppath="path/to/be/authenticated/by/php/*">
/		AuthTrans fn="php5_auth_trans"
/*********************************************************/
int NSAPI_PUBLIC php5_auth_trans(pblock * pb, Session * sn, Request * rq)
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
