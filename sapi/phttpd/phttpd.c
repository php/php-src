/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Thies C. Arntzen <thies@thieso.net>						  |
   | Based on aolserver SAPI by Sascha Schumann <sascha@schumann.cx>      |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "SAPI.h"
#include "php_main.h"

#ifdef HAVE_PHTTPD

#include "ext/standard/info.h"
 
#ifndef ZTS
#error PHTTPD module is only useable in thread-safe mode
#endif

#include "php_phttpd.h"

typedef struct {
    struct connectioninfo *cip;
	struct stat sb;
} phttpd_globals_struct; 

static int ph_globals_id;

#define PHLS_D phttpd_globals_struct *ph_context
#define PHLS_DC , PHLS_D
#define PHLS_C ph_context
#define PHLS_CC , PHLS_C
#define PHG(v) (ph_context->v)
#define PHLS_FETCH() phttpd_globals_struct *ph_context = ts_resource(ph_globals_id) 

static int
php_phttpd_startup(sapi_module_struct *sapi_module)
{
/*
    if(php_module_startup(sapi_module) == FAILURE
            || zend_startup_module(&php_aolserver_module) == FAILURE) {
*/
	fprintf(stderr,"***php_phttpd_startup\n");

    if (php_module_startup(sapi_module)) {
        return FAILURE;
    } else {
        return SUCCESS;
    }
}

static int
php_phttpd_sapi_ub_write(const char *str, uint str_length)
{
    int sent_bytes;
	PHLS_FETCH();

	sent_bytes = fd_write(PHG(cip)->fd,str,str_length);

	if (sent_bytes == -1) {
		php_handle_aborted_connection();
	}

    return sent_bytes;
}

static int
php_phttpd_sapi_header_handler(sapi_header_struct *sapi_header, sapi_headers_struct *sapi_headers SLS_DC)
{
    char *header_name, *header_content;
    char *p;
    PHLS_FETCH();
 
	http_sendheaders(PHG(cip)->fd,PHG(cip), SG(sapi_headers).http_response_code, NULL);

    header_name = sapi_header->header;
    header_content = p = strchr(header_name, ':');
 
    if (p) {
        *p = '\0';
        do {
            header_content++;
        } while (*header_content == ' ');

		fd_printf(PHG(cip)->fd,"%s: %s\n",header_name,header_content);
 
        *p = ':';
    }

    sapi_free_header(sapi_header);

    return 0;
}

static int
php_phttpd_sapi_send_headers(sapi_headers_struct *sapi_headers SLS_DC)
{
    PHLS_FETCH();
 
    if (SG(sapi_headers).send_default_content_type) {
		fd_printf(PHG(cip)->fd,"Content-Type: text/html\n");
    }
 
	fd_putc('\n', PHG(cip)->fd);
 
    return SAPI_HEADER_SENT_SUCCESSFULLY;
}

static char *
php_phttpd_sapi_read_cookies(SLS_D)
{

/*
    int i;
    char *http_cookie = NULL;
    NSLS_FETCH();
 
    i = Ns_SetIFind(NSG(conn->headers), "cookie");
    if(i != -1) {
        http_cookie = Ns_SetValue(NSG(conn->headers), i);
    }
 
    return http_cookie;
*/
	fprintf(stderr,"***php_phttpd_sapi_read_cookies\n");
 
	return 0;
}

static int
php_phttpd_sapi_read_post(char *buf, uint count_bytes SLS_DC)
{
/*
    uint max_read;
    uint total_read = 0;
    NSLS_FETCH();
 
    max_read = MIN(NSG(data_avail), count_bytes);
 
    total_read = Ns_ConnRead(NSG(conn), buf, max_read);
 
    if(total_read == NS_ERROR) {
        total_read = -1;
    } else {
        NSG(data_avail) -= total_read;
    }
 
    return total_read;
*/
	fprintf(stderr,"***php_phttpd_sapi_read_post\n");
	return 0;
}

static sapi_module_struct sapi_module = {
    "phttpd",
    "PHTTPD",
 
    php_phttpd_startup,                     /* startup */
    php_module_shutdown_wrapper,            /* shutdown */
 
	NULL,									/* activate */
	NULL,									/* deactivate */

    php_phttpd_sapi_ub_write,               /* unbuffered write */
	NULL,									/* flush */
	NULL,									/* get uid */
 	NULL,									/* getenv */

    php_error,                              /* error handler */
 
    php_phttpd_sapi_header_handler,         /* header handler */
    php_phttpd_sapi_send_headers,           /* send headers handler */
    NULL,                                   /* send header handler */
 
    php_phttpd_sapi_read_post,              /* read POST data */
    php_phttpd_sapi_read_cookies,           /* read Cookies */
 
	NULL,									/* register server variables */
	NULL,									/* Log message */

	NULL,									/* Block interruptions */
	NULL,									/* Unblock interruptions */

    STANDARD_SAPI_MODULE_PROPERTIES
};

static void
php_phttpd_request_ctor(PHLS_D SLS_DC)
{
	memset(&SG(request_info),0,sizeof(sapi_globals_struct)); /* pfusch! */

    SG(request_info).query_string = PHG(cip)->hip->request;
    SG(request_info).request_method = PHG(cip)->hip->method;
	SG(request_info).path_translated = malloc(MAXPATHLEN+1);
    SG(sapi_headers).http_response_code = 200;
	if (url_expand(PHG(cip)->hip->url, SG(request_info).path_translated, MAXPATHLEN, &PHG(sb), NULL, NULL) == NULL) {
		/* handle error */
	}

#if 0
    char *server;
    Ns_DString ds;
    char *root;
    int index;
    char *tmp;
 
    server = Ns_ConnServer(NSG(conn));
 
    Ns_DStringInit(&ds);
    Ns_UrlToFile(&ds, server, NSG(conn->request->url));
 
    /* path_translated is the absolute path to the file */
    SG(request_info).path_translated = strdup(Ns_DStringValue(&ds));
    Ns_DStringFree(&ds);
    root = Ns_PageRoot(server);
    SG(request_info).request_uri = SG(request_info).path_translated + strlen(root);
    SG(request_info).content_length = Ns_ConnContentLength(NSG(conn));
    index = Ns_SetIFind(NSG(conn)->headers, "content-type");
    SG(request_info).content_type = index == -1 ? NULL :
        Ns_SetValue(NSG(conn)->headers, index);
 
    tmp = Ns_ConnAuthUser(NSG(conn));
    if(tmp) {
        tmp = estrdup(tmp);
    }
    SG(request_info).auth_user = tmp;
 
    tmp = Ns_ConnAuthPasswd(NSG(conn));
    if(tmp) {
        tmp = estrdup(tmp);
    }
    SG(request_info).auth_password = tmp;
 
    NSG(data_avail) = SG(request_info).content_length;
#endif
}

static void
php_phttpd_request_dtor(PHLS_D SLS_DC)
{
    free(SG(request_info).path_translated);
}


int php_doit(PHLS_D SLS_DC)
{
	struct stat sb;
	zend_file_handle file_handle;
	struct httpinfo *hip = PHG(cip)->hip;

	CLS_FETCH();
	ELS_FETCH();
	PLS_FETCH();

	if (php_request_startup(CLS_C ELS_CC PLS_CC SLS_CC) == FAILURE) {
        return -1;
    }

    file_handle.type = ZEND_HANDLE_FILENAME;
    file_handle.filename = SG(request_info).path_translated;
    file_handle.free_filename = 0;

/*
	php_phttpd_hash_environment(PHLS_C CLS_CC ELS_CC PLS_CC SLS_CC);
*/
	php_execute_script(&file_handle CLS_CC ELS_CC PLS_CC);
	php_request_shutdown(NULL);

	return SG(sapi_headers).http_response_code;
}

int pm_init(const char **argv)
{
	tsrm_startup(1, 1, 0, NULL);
	sapi_startup(&sapi_module);
    sapi_module.startup(&sapi_module);

	ph_globals_id = ts_allocate_id(sizeof(phttpd_globals_struct), NULL, NULL);

	return 0;
}

void pm_exit(void)
{
	fprintf(stderr,"***pm_exit\n");
}

int pm_request(struct connectioninfo *cip)
{
	struct httpinfo *hip = cip->hip;
	int status;
	PHLS_FETCH();
	SLS_FETCH();

	if (strcasecmp(hip->method, "GET") == 0 || 
	    strcasecmp(hip->method, "HEAD") == 0 ||
	    strcasecmp(hip->method, "POST") == 0) {
		PHG(cip) = cip;
		
		php_phttpd_request_ctor(PHLS_C SLS_CC);
		status = php_doit(PHLS_C SLS_CC);
		php_phttpd_request_dtor(PHLS_C SLS_CC);

		return status;	
	} else {
		return -2;
	}
}

#endif
