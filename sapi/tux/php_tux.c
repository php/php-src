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
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "SAPI.h"
#include "php_main.h"
#include "php_variables.h"

#include "ext/standard/php_smart_str.h"

#include "tuxmodule.h"

#include <sys/uio.h>

#if 0
#include <pthread.h>
#endif

void tux_closed_conn(int fd);

enum {
	PHP_TUX_BACKGROUND_CONN = 1
};

typedef struct {
	user_req_t *req;
	void (*on_close)(int);
	int tux_action;
	struct iovec *header_vec;
	int number_vec;
} php_tux_globals;

static php_tux_globals tux_globals;

#define TG(v) (tux_globals.v)

static int sapi_tux_ub_write(const char *str, uint str_length TSRMLS_DC)
{
	int n;
	int m;
	const char *estr;
	
	/* combine headers and body */
	if (TG(number_vec)) {
		struct iovec *vec = TG(header_vec);
		
		n = TG(number_vec);
		vec[n].iov_base = (void *) str;
		vec[n++].iov_len = str_length;
		
		/* XXX: this might need more complete error handling */
		if ((m = writev(TG(req)->sock, vec, n)) == -1 && errno == EPIPE)
			php_handle_aborted_connection();

		if (m > 0)
			TG(req)->bytes_sent += str_length;

		TG(number_vec) = 0;
		return str_length;
	}

	estr = str + str_length;
	
	while (str < estr) {
		n = send(TG(req)->sock, str, estr - str, 0);

		if (n == -1 && errno == EPIPE)
			php_handle_aborted_connection();
		if (n == -1 && errno == EAGAIN)
			continue;
		if (n <= 0) 
			return n;

		str += n;
	}

	n = str_length - (estr - str);
	
	TG(req)->bytes_sent += n;

	return n;
}

static int sapi_tux_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	char buf[1024];
	struct iovec *vec;
	int n;
	int max_headers;
	zend_llist_position pos;
	sapi_header_struct *h;
	size_t len;
	char *status_line;
	int locate_cl;
	
	max_headers = 30;
	n = 1;
	
	vec = malloc(sizeof(struct iovec) * max_headers);
	status_line = malloc(30);
	
	/* safe sprintf use */
	len = slprintf(status_line, 30, "HTTP/1.1 %d NA\r\n", SG(sapi_headers).http_response_code);
	
	vec[0].iov_base = status_line;
	vec[0].iov_len = len;
	
	TG(req)->http_status = SG(sapi_headers).http_response_code;

	if (TG(tux_action) == TUX_ACTION_FINISH_CLOSE_REQ && TG(req)->http_version == HTTP_1_1)
		locate_cl = 1;
	else
		locate_cl = 0;
	
	h = zend_llist_get_first_ex(&sapi_headers->headers, &pos);
	while (h) {
		if (locate_cl 
				&& strncasecmp(h->header, "Content-length:", sizeof("Content-length:")-1) == 0) {
			TG(tux_action) = TUX_ACTION_FINISH_REQ;
			locate_cl = 0;
		}
			
		vec[n].iov_base = h->header;
		vec[n++].iov_len = h->header_len;
		if (n >= max_headers - 3) {
			max_headers *= 2;
			vec = realloc(vec, sizeof(struct iovec) * max_headers);
		}
		vec[n].iov_base = "\r\n";
		vec[n++].iov_len = 2;
		
		h = zend_llist_get_next_ex(&sapi_headers->headers, &pos);
	}

	vec[n].iov_base = "\r\n";
	vec[n++].iov_len = 2;

	TG(number_vec) = n;
	TG(header_vec) = vec;

	
	return SAPI_HEADER_SENT_SUCCESSFULLY;
}

static int sapi_tux_read_post(char *buffer, uint count_bytes TSRMLS_DC)
{
#if 0
	int amount = 0;

	TG(req)->objectlen = count_bytes;
	TG(req)->object_addr = buffer;
	if (tux(TUX_ACTION_READ_POST_DATA, TG(req)))
		return 0;

	TG(read_post_data) = 1;
	
	return TG(req)->objectlen;
#else
	return 0;
#endif
}

static char *sapi_tux_read_cookies(TSRMLS_D)
{
	return TG(req)->cookies;
}

#define BUF_SIZE 512
#define ADD_STRING(name)										\
	php_register_variable(name, buf, track_vars_array TSRMLS_CC)

static void sapi_tux_register_variables(zval *track_vars_array TSRMLS_DC)
{
	char buf[BUF_SIZE + 1];
	char *p;
	sapi_header_line ctr = {0};
	
	ctr.line = buf;
	ctr.line_len = slprintf(buf, sizeof(buf), "Server: %s", TUXAPI_version);
	sapi_header_op(SAPI_HEADER_REPLACE, &ctr TSRMLS_CC);
	
	php_register_variable("PHP_SELF", SG(request_info).request_uri, track_vars_array TSRMLS_CC);
	php_register_variable("SERVER_SOFTWARE", TUXAPI_version, track_vars_array TSRMLS_CC);
	php_register_variable("GATEWAY_INTERFACE", "CGI/1.1", track_vars_array TSRMLS_CC);
	php_register_variable("REQUEST_METHOD", (char *) SG(request_info).request_method, track_vars_array TSRMLS_CC);
	php_register_variable("DOCUMENT_ROOT", TUXAPI_docroot, track_vars_array TSRMLS_CC);
	php_register_variable("SERVER_NAME", TUXAPI_servername, track_vars_array TSRMLS_CC);
	php_register_variable("REQUEST_URI", SG(request_info).request_uri, track_vars_array TSRMLS_CC);
	php_register_variable("PATH_TRANSLATED", SG(request_info).path_translated, track_vars_array TSRMLS_CC);

	p = inet_ntoa(TG(req)->client_host);
	/* string representation of IPs are never larger than 512 bytes */
	if (p) {
		memcpy(buf, p, strlen(p) + 1);
		ADD_STRING("REMOTE_ADDR");
		ADD_STRING("REMOTE_HOST");
	}

	snprintf(buf, sizeof(buf), "%d", CGI_SERVER_PORT(TG(req)));
	ADD_STRING("SERVER_PORT");

#if 0
	snprintf(buf, BUF_SIZE, "/%s", TG(hc)->pathinfo);
	ADD_STRING("PATH_INFO");

	snprintf(buf, BUF_SIZE, "/%s", TG(hc)->origfilename);
	ADD_STRING("SCRIPT_NAME");
#endif

#define CONDADD(name, field) 							\
	if (TG(req)->field[0]) {								\
		php_register_variable(#name, TG(req)->field, track_vars_array TSRMLS_CC); \
	}

	CONDADD(HTTP_REFERER, referer);
	CONDADD(HTTP_USER_AGENT, user_agent);
	CONDADD(HTTP_ACCEPT, accept);
	CONDADD(HTTP_ACCEPT_ENCODING, accept_encoding);
	CONDADD(HTTP_ACCEPT_LANGUAGE, accept_language);
	CONDADD(HTTP_COOKIE, cookies);
	CONDADD(CONTENT_TYPE, content_type);

#if 0
	if (TG(hc)->contentlength != -1) {
		snprintf(buf, sizeof(buf), "%ld", (long) TG(hc)->contentlength);
		ADD_STRING("CONTENT_LENGTH");
	}
#endif

#if 0
	if (TG(hc)->authorization[0])
		php_register_variable("AUTH_TYPE", "Basic", track_vars_array TSRMLS_CC);
#endif
}


static int php_tux_startup(sapi_module_struct *sapi_module)
{
	if (php_module_startup(sapi_module, NULL, 0)==FAILURE) {
		return FAILURE;
	} else {
		return SUCCESS;
	}
}

static sapi_module_struct tux_sapi_module = {
	"tux",
	"tux",
	
	php_tux_startup,
	php_module_shutdown_wrapper,
	
	NULL,									/* activate */
	NULL,									/* deactivate */

	sapi_tux_ub_write,
	NULL,
	NULL,									/* get uid */
	NULL,									/* getenv */

	php_error,
	
	NULL,
	sapi_tux_send_headers,
	NULL,
	sapi_tux_read_post,
	sapi_tux_read_cookies,

	sapi_tux_register_variables,
	NULL,									/* Log message */
	NULL,									/* Get request time */
	NULL,									/* Child terminate */

	STANDARD_SAPI_MODULE_PROPERTIES
};

static void tux_module_main(TSRMLS_D)
{
	zend_file_handle file_handle;

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

static void tux_request_ctor(TSRMLS_D)
{
	char buf[1024];
	int offset;
	size_t filename_len;
	size_t cwd_len;
	smart_str s = {0};
	char *p;

	TG(number_vec) = 0;	
	TG(header_vec) = NULL;
	SG(request_info).query_string = strdup(TG(req)->query);

	smart_str_appends_ex(&s, "/", 1);
	smart_str_appends_ex(&s, TG(req)->query, 1);
	smart_str_0(&s);
	p = strchr(s.c, '&');
	if (p)
		*p = '\0';
	SG(request_info).path_translated = s.c;
	
	s.c = NULL;
	smart_str_appendc_ex(&s, '/', 1);
	smart_str_appends_ex(&s, TG(req)->objectname, 1);
	smart_str_0(&s);
	SG(request_info).request_uri = s.c;
	SG(request_info).request_method = CGI_REQUEST_METHOD(TG(req));
	if(TG(req)->http_version == HTTP_1_1) SG(request_info).proto_num = 1001;
	else SG(request_info).proto_num = 1000;
	SG(sapi_headers).http_response_code = 200;
	SG(request_info).content_type = TG(req)->content_type;
	SG(request_info).content_length = 0; /* TG(req)->contentlength; */

#if 0
	php_handle_auth_data(TG(hc)->authorization TSRMLS_CC);
#endif
}

static void tux_request_dtor(TSRMLS_D)
{
	if (TG(header_vec)) {
		/* free status_line */
		free(TG(header_vec)[0].iov_base);
		free(TG(header_vec));
	}
	if (SG(request_info).query_string)
		free(SG(request_info).query_string);
	free(SG(request_info).request_uri);
	free(SG(request_info).path_translated);
}

#if 0
static void *separate_thread(void *bla)
{
	int fd;
	int i = 0;
	
	fd = (int) bla;

	while (i++ < 5) {
		send(fd, "test<br />\n", 9, 0);
		sleep(1);
	}
	
	tux(TUX_ACTION_CONTINUE_REQ, (user_req_t *) fd);
	/* We HAVE to trigger some event on the fd. Otherwise
	   fast_thread won't wake up, so that the eventloop
	   won't be entered -> TUX hangs */
	shutdown(fd, 2);
	pthread_exit(NULL);
}
#endif

int TUXAPI_handle_events(user_req_t *req)
{
	TSRMLS_FETCH();

	if (req->event == PHP_TUX_BACKGROUND_CONN) {
		tux_closed_conn(req->sock);
		return tux(TUX_ACTION_FINISH_CLOSE_REQ, req);
	}
	
	TG(req) = req;
	TG(tux_action) = TUX_ACTION_FINISH_CLOSE_REQ;
	
	tux_request_ctor(TSRMLS_C);

	tux_module_main(TSRMLS_C);

	tux_request_dtor(TSRMLS_C);

	return tux(TG(tux_action), req);
}

void tux_register_on_close(void (*arg)(int)) 
{
	TG(on_close) = arg;
}

void tux_closed_conn(int fd)
{
	TSRMLS_FETCH();

	if (TG(on_close)) TG(on_close)(fd);
}

int tux_get_fd(void)
{
	TSRMLS_FETCH();
	
	return TG(req)->sock;
}

void tux_set_dont_close(void)
{
	TSRMLS_FETCH();

	TG(req)->event = PHP_TUX_BACKGROUND_CONN;
	tux(TUX_ACTION_POSTPONE_REQ, TG(req));
	TG(tux_action) = TUX_ACTION_EVENTLOOP;
}

void TUXAPI_init(void)
{
	sapi_startup(&tux_sapi_module);
	tux_sapi_module.startup(&tux_sapi_module);
	SG(server_context) = (void *) 1;
}

void doesnotmatter_fini(void)
{
	if (SG(server_context) != NULL) {
		tux_sapi_module.shutdown(&tux_sapi_module);
		sapi_shutdown();
	}
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
