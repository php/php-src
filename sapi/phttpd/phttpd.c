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
   | Author: Thies C. Arntzen <thies@digicol.de>						  |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "SAPI.h"
#include "main.h"

#ifdef HAVE_PHTTPD
 
#ifndef ZTS
#error PHTTPD module is only useable in thread-safe mode
#endif

#include "php_phttpd.h"

typedef struct {
    struct connectioninfo *cip;
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
            || zend_register_module(&php_aolserver_module) == FAILURE) {
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

	fprintf(stderr,"***php_phttpd_sapi_ub_write returned %d\n",sent_bytes);
 
    return sent_bytes;
}

static int
php_phttpd_sapi_header_handler(sapi_header_struct *sapi_header, sapi_headers_struct *sapi_headers SLS_DC)
{
#if 0
    char *header_name, *header_content;
    char *p;
    NSLS_FETCH();
 
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
#endif
	fprintf(stderr,"***php_phttpd_sapi_header_handler\n");

    return 0;
}

static int
php_phttpd_sapi_send_headers(sapi_headers_struct *sapi_headers SLS_DC)
{
/*
    NSLS_FETCH();
 
    if(SG(sapi_headers).send_default_content_type) {
        Ns_ConnSetRequiredHeaders(NSG(conn), "text/html", 0);
    }
 
    Ns_ConnFlushHeaders(NSG(conn), SG(sapi_headers).http_response_code);
*/
 
	fprintf(stderr,"***php_phttpd_sapi_send_headers\n");

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
    "PHTTPD",
 
    php_phttpd_startup,                     /* startup */
    php_module_shutdown_wrapper,            /* shutdown */
 
    php_phttpd_sapi_ub_write,               /* unbuffered write */
 
    php_error,                              /* error handler */
 
    php_phttpd_sapi_header_handler,         /* header handler */
    php_phttpd_sapi_send_headers,           /* send headers handler */
    NULL,                                   /* send header handler */
 
    php_phttpd_sapi_read_post,              /* read POST data */
    php_phttpd_sapi_read_cookies,           /* read Cookies */
 
    STANDARD_SAPI_MODULE_PROPERTIES
};


int pm_init(const char **argv)
{

	fprintf(stderr,"***pm_init\n");
	tsrm_startup(1, 1, 0);
	fprintf(stderr,"***tsrm_startup\n");
	reentrancy_startup();
	fprintf(stderr,"***reentrancy_startup\n");
	sapi_startup(&sapi_module);
	fprintf(stderr,"***sapi_startup\n");
    sapi_module.startup(&sapi_module);
	fprintf(stderr,"***sapi_module.startup\n");

	ph_globals_id = ts_allocate_id(sizeof(phttpd_globals_struct), NULL, NULL);

	return 0;
}

void pm_exit(void)
{
	fprintf(stderr,"***pm_exit\n");
}

int php_doit(struct connectioninfo *cip)
{
	char path[MAXPATHLEN];
	struct stat sb;
	zend_file_handle file_handle;
	struct httpinfo *hip = cip->hip;

	CLS_FETCH();
	ELS_FETCH();
	PLS_FETCH();
	SLS_FETCH();
	PHLS_FETCH();

	if (debug > 1) {
		fprintf(stderr, "*** php/php_doit() called ***\n");
	}

	if (url_expand(hip->url, path, sizeof(path), &sb, NULL, NULL) == NULL) {
		return -1;
	}

	PHG(cip) = cip;

	if (debug > 1) {
		fprintf(stderr, "*** php/php_request_startup for  %s ***\n",path);
	}

	memset(&SG(request_info),0,sizeof(sapi_globals_struct)); /* pfusch! */

	if (php_request_startup(CLS_C ELS_CC PLS_CC SLS_CC) == FAILURE) {
        return -1;
    }

	if (debug > 1) {
		fprintf(stderr, "*** php/php_doit/script %s ***\n",path);
	}

    file_handle.type = ZEND_HANDLE_FILENAME;
    file_handle.filename = path;
    file_handle.free_filename = 0;

	php_execute_script(&file_handle CLS_CC ELS_CC PLS_CC);

	php_request_shutdown(NULL);

	return -1;
}

int pm_request(struct connectioninfo *cip)
{
	struct httpinfo *hip = cip->hip;

	if (strcasecmp(hip->method, "GET") == 0 || 
	    strcasecmp(hip->method, "HEAD") == 0 ||
	    strcasecmp(hip->method, "POST") == 0) {
		return php_doit(cip);
	} else {
		return -2;
	}
}

#endif
