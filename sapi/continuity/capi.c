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
  | Author: Alex Leigh <php (at) postfin (dot) com>                      |
  +----------------------------------------------------------------------+
*/

/* For more information on Continuity: http://www.ashpool.com/ */

/*
 * This code is based on the PHP5 SAPI module for NSAPI by Jayakumar
 * Muthukumarasamy
 */

/* PHP includes */
#define CONTINUITY 1
#define CAPI_DEBUG

/* Define for CDP specific extensions */
#undef CONTINUITY_CDPEXT

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
 * CAPI includes
 */
#include <continuity.h>
#include <http.h>

#define NSLS_D		struct capi_request_context *request_context
#define NSLS_DC		, NSLS_D
#define NSLS_C		request_context
#define NSLS_CC		, NSLS_C
#define NSG(v)		(request_context->v)

/*
 * ZTS needs to be defined for CAPI to work
 */
#if !defined(ZTS)
#error "CAPI module needs ZTS to be defined"
#endif

/*
 * Structure to encapsulate the CAPI request in SAPI
 */
typedef struct capi_request_context {
   httpTtrans *t;
   int read_post_bytes;
} capi_request_context;

/**************/

PHP_MINIT_FUNCTION(continuity);
PHP_MSHUTDOWN_FUNCTION(continuity);
PHP_RINIT_FUNCTION(continuity);
PHP_RSHUTDOWN_FUNCTION(continuity);
PHP_MINFO_FUNCTION(continuity);
        
PHP_FUNCTION(continuity_virtual);
PHP_FUNCTION(continuity_request_headers);
PHP_FUNCTION(continuity_response_headers);

const zend_function_entry continuity_functions[] = {
        {NULL, NULL, NULL}
};

zend_module_entry continuity_module_entry = {
        STANDARD_MODULE_HEADER,
        "continuity",
        continuity_functions,   
        PHP_MINIT(continuity),
        PHP_MSHUTDOWN(continuity),
        NULL,
        NULL,
        PHP_MINFO(continuity),
        NO_VERSION_YET,
        STANDARD_MODULE_PROPERTIES
};

PHP_MINIT_FUNCTION(continuity)
{
        return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(continuity)
{
        return SUCCESS;
}

PHP_MINFO_FUNCTION(continuity)
{
        php_info_print_table_start();
        php_info_print_table_row(2, "Continuity Module Revision", "$Id$");
        php_info_print_table_row(2, "Server Version", conFget_build());
#ifdef CONTINUITY_CDPEXT
	php_info_print_table_row(2,"CDP Extensions", "enabled");
#else
	php_info_print_table_row(2,"CDP Extensions", "disabled");
#endif
        php_info_print_table_end();
        
/*        DISPLAY_INI_ENTRIES(); */
}

/**************/

/*
 * sapi_capi_ub_write: Write len bytes to the connection output.
 */
static int sapi_capi_ub_write(const char *str, unsigned int str_length TSRMLS_DC)
{
   int retval;
   capi_request_context *rc;

   rc = (capi_request_context *) SG(server_context);
   retval = httpFwrite(rc->t, (char *) str, str_length);
   if (retval == -1 || retval == 0)
      php_handle_aborted_connection();
   return retval;
}

/*
 * sapi_capi_header_handler: Add/update response headers with those provided
 * by the PHP engine.
 */
static int sapi_capi_header_handler(sapi_header_struct * sapi_header, sapi_headers_struct * sapi_headers TSRMLS_DC)
{
   char *header_name, *header_content, *p;
   capi_request_context *rc = (capi_request_context *) SG(server_context);

   lstFset_delete_key(rc->t->res_hdrs, "Content-Type");

   header_name = sapi_header->header;
   header_content = p = strchr(header_name, ':');
   if (p == NULL) {
      return 0;
   }
   *p = 0;
   do {
      header_content++;
   } while (*header_content == ' ');

   lstFset_add(rc->t->res_hdrs, header_name, header_content);

   *p = ':';			/* restore '*p' */

   efree(sapi_header->header);

   return 0;			/* don't use the default SAPI mechanism, CAPI
				 * duplicates this functionality */
}

/*
 * sapi_capi_send_headers: Transmit the headers to the client. This has the
 * effect of starting the response under Continuity.
 */
static int sapi_capi_send_headers(sapi_headers_struct * sapi_headers TSRMLS_DC)
{
   int retval;
   capi_request_context *rc = (capi_request_context *) SG(server_context);

   /*
    * We could probably just do this in the header_handler. But, I don't know
    * what the implication of doing it there is.
    */

   if (SG(sapi_headers).send_default_content_type) {
      /* lstFset_delete_key(rc->t->res_hdrs, "Content-Type"); */
      lstFset_update(rc->t->res_hdrs, "Content-Type", "text/html");
   }
   httpFset_status(rc->t, SG(sapi_headers).http_response_code, NULL);
   httpFstart_response(rc->t);

   return SAPI_HEADER_SENT_SUCCESSFULLY;

}

static int sapi_capi_read_post(char *buffer, uint count_bytes TSRMLS_DC)
{
   unsigned int max_read, total_read = 0;
   capi_request_context *rc = (capi_request_context *) SG(server_context);

   if (rc->read_post_bytes == -1) {
      max_read = MIN(count_bytes, SG(request_info).content_length);
   } else {
      if (rc->read_post_bytes == 0)
	 return 0;
      max_read = MIN(count_bytes, (SG(request_info).content_length - rc->read_post_bytes));
   }

   total_read = httpFread(rc->t, buffer, max_read);

   if (total_read < 0)
      total_read = -1;
   else
      rc->read_post_bytes = total_read;

   return total_read;
}

/*
 * sapi_capi_read_cookies: Return cookie information into PHP.
 */
static char *sapi_capi_read_cookies(TSRMLS_D)
{
   char *cookie_string;
   capi_request_context *rc = (capi_request_context *) SG(server_context);

   cookie_string = lstFset_get(rc->t->req_hdrs, "cookie");
   return cookie_string;
}

static void sapi_capi_register_server_variables(zval * track_vars_array TSRMLS_DC)
{
   capi_request_context *rc = (capi_request_context *) SG(server_context);
   size_t i;
   char *value;
   char buf[128];

   /* PHP_SELF and REQUEST_URI */
   value = lstFset_get(rc->t->vars, "uri");
   if (value != NULL) {
      php_register_variable("PHP_SELF", value, track_vars_array TSRMLS_CC);
      php_register_variable("REQUEST_URI", value, track_vars_array TSRMLS_CC);
   }
 
   /* COUNTRY CODE */
   value = lstFset_get(rc->t->vars, "ccode");
   if(value!=NULL)
     php_register_variable("COUNTRY_CODE", value, track_vars_array TSRMLS_CC);

   /* argv */
   value = lstFset_get(rc->t->vars, "query");
   if (value != NULL)
      php_register_variable("argv", value, track_vars_array TSRMLS_CC);

   /* GATEWAY_INTERFACE */
   php_register_variable("GATEWAY_INTERFACE", "CGI/1.1", track_vars_array TSRMLS_CC);

   /* SERVER_NAME and HTTP_HOST */
   value = lstFset_get(rc->t->req_hdrs, "host");
   if (value != NULL) {
      php_register_variable("HTTP_HOST", value, track_vars_array TSRMLS_CC);
      /* TODO: This should probably scrub the port value if one is present. */
      php_register_variable("SERVER_NAME", value, track_vars_array TSRMLS_CC);
   }
   /* SERVER_SOFTWARE */
   value = lstFset_get(rc->t->res_hdrs, "Server");
   if (value != NULL)
      php_register_variable("SERVER_SOFTWARE", value, track_vars_array TSRMLS_CC);

   /* SERVER_PROTOCOL */
   value = lstFset_get(rc->t->vars, "protocol");
   if (value != NULL)
      php_register_variable("SERVER_PROTOCOL", value, track_vars_array TSRMLS_CC);

   /* REQUEST_METHOD */
   value = lstFset_get(rc->t->vars, "method");
   if (value != NULL)
      php_register_variable("REQUEST_METHOD", value, track_vars_array TSRMLS_CC);

   /* QUERY_STRING */
   value = lstFset_get(rc->t->vars, "query");
   if (value != NULL)
      php_register_variable("QUERY_STRING", value, track_vars_array TSRMLS_CC);

   /* DOCUMENT_ROOT */
   value = lstFset_get(rc->t->vars, "docroot");
   if (value != NULL)
      php_register_variable("DOCUMENT_ROOT", value, track_vars_array TSRMLS_CC);

   /* HTTP_ACCEPT */
   value = lstFset_get(rc->t->req_hdrs, "accept");
   if (value != NULL)
      php_register_variable("HTTP_ACCEPT", value, track_vars_array TSRMLS_CC);

   /* HTTP_ACCEPT_CHARSET */
   value = lstFset_get(rc->t->req_hdrs, "accept-charset");
   if (value != NULL)
      php_register_variable("HTTP_ACCEPT_CHARSET", value, track_vars_array TSRMLS_CC);

   /* HTTP_ACCEPT_ENCODING */
   value = lstFset_get(rc->t->req_hdrs, "accept-encoding");
   if (value != NULL)
      php_register_variable("HTTP_ACCEPT_ENCODING", value, track_vars_array TSRMLS_CC);

   /* HTTP_ACCEPT_LANGUAGE */
   value = lstFset_get(rc->t->req_hdrs, "accept-language");
   if (value != NULL)
      php_register_variable("HTTP_ACCEPT_LANGUAGE", value, track_vars_array TSRMLS_CC);

   /* HTTP_CONNECTION */
   value = lstFset_get(rc->t->req_hdrs, "connection");
   if (value != NULL)
      php_register_variable("HTTP_CONNECTION", value, track_vars_array TSRMLS_CC);

   /* HTTP_REFERER */
   value = lstFset_get(rc->t->req_hdrs, "referer");
   if (value != NULL)
      php_register_variable("HTTP_REFERER", value, track_vars_array TSRMLS_CC);

   /* HTTP_USER_AGENT */
   value = lstFset_get(rc->t->req_hdrs, "user-agent");
   if (value != NULL)
      php_register_variable("HTTP_USER_AGENT", value, track_vars_array TSRMLS_CC);

   /* REMOTE_ADDR */
   utlFip_to_str(rc->t->cli_ipv4_addr, buf, sizeof(buf));
   php_register_variable("REMOTE_ADDR", buf, track_vars_array TSRMLS_CC);

   /* REMOTE_PORT */

   /* SCRIPT_FILENAME and PATH_TRANSLATED */
   value = lstFset_get(rc->t->vars, "path");
   if (value != NULL) {
      php_register_variable("SCRIPT_FILENAME", value, track_vars_array TSRMLS_CC);
      php_register_variable("PATH_TRANSLATED", value, track_vars_array TSRMLS_CC);
   }
   /* SERVER_ADMIN */
   /* Not applicable */

   /* SERVER_PORT */

}

static void capi_log_message(char *message TSRMLS_DC)
{
   capi_request_context *rc = (capi_request_context *) SG(server_context);
   logFmsg(0, "mod/php: %s", message);
}

static int php_capi_startup(sapi_module_struct *sapi_module);

sapi_module_struct capi_sapi_module = {
   "Continuity",			/* name */
   "Continuity Server Enterprise Edition",	/* pretty name */

   php_capi_startup,		/* startup */
   php_module_shutdown_wrapper,	/* shutdown */

   NULL,			/* activate */
   NULL,			/* deactivate */

   sapi_capi_ub_write,		/* unbuffered write */
   NULL,			/* flush */
   NULL,			/* get uid */
   NULL,			/* getenv */

   php_error,			/* error handler */

   sapi_capi_header_handler,	/* header handler */
   sapi_capi_send_headers,	/* send headers handler */
   NULL,			/* send header handler */

   sapi_capi_read_post,		/* read POST data */
   sapi_capi_read_cookies,	/* read Cookies */

   sapi_capi_register_server_variables,	/* register server variables */
   capi_log_message,		/* Log message */
   NULL,			/* Get request time */
   NULL,			/* Child terminate */

   NULL,			/* Block interruptions */
   NULL,			/* Unblock interruptions */

   STANDARD_SAPI_MODULE_PROPERTIES
};

static int php_capi_startup(sapi_module_struct *sapi_module) {
  if(php_module_startup(sapi_module,&continuity_module_entry,1)==FAILURE) {
    return FAILURE;
  }
  return SUCCESS;
}


static char *
 capi_strdup(char *str)
{
   if (str != NULL)
      return strFcopy(str);
   return NULL;
}

static void capi_free(void *addr)
{
   if (addr != NULL)
      free(addr);
}

static void capi_request_ctor(NSLS_D TSRMLS_DC)
{
   char *query_string = lstFset_get(NSG(t->vars), "query");
   char *uri = lstFset_get(NSG(t->vars), "uri");
   char *path_info = lstFset_get(NSG(t->vars), "path-info");
   char *path_translated = lstFset_get(NSG(t->vars), "path");
   char *request_method = lstFset_get(NSG(t->vars), "method");
   char *content_type = lstFset_get(NSG(t->req_hdrs), "content-type");
   char *content_length = lstFset_get(NSG(t->req_hdrs), "content-length");

   SG(request_info).query_string = capi_strdup(query_string);
   SG(request_info).request_uri = capi_strdup(uri);
   SG(request_info).request_method = capi_strdup(request_method);
   SG(request_info).path_translated = capi_strdup(path_translated);
   SG(request_info).content_type = capi_strdup(content_type);
   SG(request_info).content_length = (content_length == NULL) ? 0 : strtoul(content_length, 0, 0);
   SG(sapi_headers).http_response_code = 200;
}

static void capi_request_dtor(NSLS_D TSRMLS_DC)
{
   capi_free(SG(request_info).query_string);
   capi_free(SG(request_info).request_uri);
   capi_free(SG(request_info).request_method);
   capi_free(SG(request_info).path_translated);
   capi_free(SG(request_info).content_type);
}

int capi_module_main(NSLS_D TSRMLS_DC)
{
   zend_file_handle file_handle;

   if (php_request_startup(TSRMLS_C) == FAILURE) {
      return FAILURE;
   }
   file_handle.type = ZEND_HANDLE_FILENAME;
   file_handle.filename = SG(request_info).path_translated;
   file_handle.free_filename = 0;
   file_handle.opened_path = NULL;

   php_execute_script(&file_handle TSRMLS_CC);
   php_request_shutdown(NULL);

   return SUCCESS;
}

int phpFinit(lstTset * opt)
{
   php_core_globals *core_globals;

   tsrm_startup(128, 1, 0, NULL);
   core_globals = ts_resource(core_globals_id);

   logFmsg(0, "mod/php: PHP Interface v3 (module)");
   logFmsg(0, "mod/php: Copyright (c) 1999-2005 The PHP Group. All rights reserved.");

   sapi_startup(&capi_sapi_module);
   capi_sapi_module.startup(&capi_sapi_module);

   return STATUS_PROCEED;
}

int phpFservice(httpTtrans * t, lstTset * opts)
{
   int retval;
   capi_request_context *request_context;

   TSRMLS_FETCH();

   request_context = (capi_request_context *) malloc(sizeof(capi_request_context));
   request_context->t = t;
   request_context->read_post_bytes = -1;

   SG(server_context) = request_context;

   capi_request_ctor(NSLS_C TSRMLS_CC);
   retval = capi_module_main(NSLS_C TSRMLS_CC);
   capi_request_dtor(NSLS_C TSRMLS_CC);

   free(request_context);

   /*
    * This call is ostensibly provided to free the memory from PHP/TSRM when
    * the thread terminated, but, it leaks a structure in some hash list
    * according to the developers. Not calling this will leak the entire
    * interpreter, around 100k, but calling it and then terminating the
    * thread will leak the struct (around a k). The only answer with the
    * current TSRM implementation is to reuse the threads that allocate TSRM
    * resources.
    */
   /* ts_free_thread(); */

   if (retval == SUCCESS) {
      return STATUS_EXIT;
   } else {
      return STATUS_ERROR;
   }
}
