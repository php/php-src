/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
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


#include "php.h"
#include "SAPI.h"
#include "php_main.h"
#include "php_thttpd.h"
#include "php_variables.h"
#include "version.h"

#include "ext/standard/php_smart_str.h"

#include <sys/uio.h>

typedef struct {
	httpd_conn *hc;
	int post_off;
	void (*on_close)(int);
} php_thttpd_globals;


#ifdef ZTS
static int thttpd_globals_id;
#define TG(v) TSRMG(thttpd_globals_id, php_thttpd_globals *, v)
#else
static php_thttpd_globals thttpd_globals;
#define TG(v) (thttpd_globals.v)
#endif

static int sapi_thttpd_ub_write(const char *str, uint str_length TSRMLS_DC)
{
	int n;
	uint sent = 0;	
	
	while (str_length > 0) {
		n = send(TG(hc)->conn_fd, str, str_length, 0);

		if (n == -1 && errno == EPIPE)
			php_handle_aborted_connection();
		if (n == -1 && errno == EAGAIN)
			continue;
		if (n <= 0) 
			return n;

		TG(hc)->bytes_sent += n;
		str += n;
		sent += n;
		str_length -= n;
	}

	return sent;
}

#define COMBINE_HEADERS 30

static int sapi_thttpd_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	char buf[1024];
	struct iovec vec[COMBINE_HEADERS];
	int n = 0;
	zend_llist_position pos;
	sapi_header_struct *h;
	size_t len;
	
	if (!SG(sapi_headers).http_status_line) {
		snprintf(buf, 1023, "HTTP/1.0 %d Something\r\n", SG(sapi_headers).http_response_code);
		len = strlen(buf);
		vec[n].iov_base = buf;
		vec[n].iov_len = len;
	} else {
		vec[n].iov_base = SG(sapi_headers).http_status_line;
		len = strlen(vec[n].iov_base);
		vec[n].iov_len = len;
		vec[++n].iov_base = "\r\n";
		vec[n].iov_len = 2;
		len += 2;
	}
	TG(hc)->status = SG(sapi_headers).http_response_code;
	TG(hc)->bytes_sent += len;
	n++;

#define DEF_CONTENT_TYPE_LINE "Content-Type: text/html"
	if (SG(sapi_headers).send_default_content_type) {
		vec[n].iov_base = DEF_CONTENT_TYPE_LINE;
		vec[n].iov_len = sizeof(DEF_CONTENT_TYPE_LINE) - 1;
		n++;
	}

	h = zend_llist_get_first_ex(&sapi_headers->headers, &pos);
	while (h) {
		vec[n].iov_base = h->header;
		vec[n++].iov_len = h->header_len;
		if (n >= COMBINE_HEADERS - 1) {
			if (writev(TG(hc)->conn_fd, vec, n) == -1 && errno == EPIPE)
				php_handle_aborted_connection();
			n = 0;
		}
		vec[n].iov_base = "\r\n";
		vec[n++].iov_len = 2;
		
		h = zend_llist_get_next_ex(&sapi_headers->headers, &pos);
	}

	vec[n].iov_base = "\r\n";
	vec[n++].iov_len = 2;

	if (n) {
		if (writev(TG(hc)->conn_fd, vec, n) == -1 && errno == EPIPE)
			php_handle_aborted_connection();
	}
	
	return SAPI_HEADER_SENT_SUCCESSFULLY;
}

static int sapi_thttpd_read_post(char *buffer, uint count_bytes TSRMLS_DC)
{
	size_t read_bytes = 0, tmp;
	int c;

	/* to understand this, read cgi_interpose_input() in libhttpd.c */
	c = TG(hc)->read_idx - TG(hc)->checked_idx;
	if (c > 0) {
		read_bytes = MIN(c, count_bytes);
		memcpy(buffer, TG(hc)->read_buf + TG(hc)->checked_idx, read_bytes);
		TG(hc)->checked_idx += read_bytes;
		count_bytes -= read_bytes;
	}
	
	count_bytes = MIN(count_bytes, 
			SG(request_info).content_length - SG(read_post_bytes) - TG(post_off));

	while (read_bytes < count_bytes) {
		tmp = recv(TG(hc)->conn_fd, buffer + read_bytes, 
				count_bytes - read_bytes, 0);
		if (tmp <= 0) 
			break;
		read_bytes += tmp;
	}
	
	return read_bytes;
}

static char *sapi_thttpd_read_cookies(TSRMLS_D)
{
	return TG(hc)->cookie;
}

#define BUF_SIZE 512
#define ADD_STRING(name)										\
	php_register_variable(name, buf, track_vars_array TSRMLS_CC)

static void sapi_thttpd_register_variables(zval *track_vars_array TSRMLS_DC)
{
	char buf[BUF_SIZE + 1];
	char *p;

	php_register_variable("PHP_SELF", SG(request_info).request_uri, track_vars_array TSRMLS_CC);
	php_register_variable("SERVER_SOFTWARE", SERVER_SOFTWARE, track_vars_array TSRMLS_CC);
	php_register_variable("GATEWAY_INTERFACE", "CGI/1.1", track_vars_array TSRMLS_CC);
	php_register_variable("REQUEST_METHOD", (char *) SG(request_info).request_method, track_vars_array TSRMLS_CC);
	php_register_variable("REQUEST_URI", SG(request_info).request_uri, track_vars_array TSRMLS_CC);
	php_register_variable("PATH_TRANSLATED", SG(request_info).path_translated, track_vars_array TSRMLS_CC);

	p = inet_ntoa(TG(hc)->client_addr.sa_in.sin_addr);
	/* string representation of IPs are never larger than 512 bytes */
	if (p) {
		memcpy(buf, p, strlen(p) + 1);
		ADD_STRING("REMOTE_ADDR");
		ADD_STRING("REMOTE_HOST");
	}

	snprintf(buf, BUF_SIZE, "%d", TG(hc)->hs->port);
	ADD_STRING("SERVER_PORT");

	snprintf(buf, BUF_SIZE, "/%s", TG(hc)->pathinfo);
	ADD_STRING("PATH_INFO");

	snprintf(buf, BUF_SIZE, "/%s", TG(hc)->origfilename);
	ADD_STRING("SCRIPT_NAME");

#define CONDADD(name, field) 							\
	if (TG(hc)->field[0]) {								\
		php_register_variable(#name, TG(hc)->field, track_vars_array TSRMLS_CC); \
	}

	CONDADD(HTTP_REFERER, referer);
	CONDADD(HTTP_USER_AGENT, useragent);
	CONDADD(HTTP_ACCEPT, accept);
	CONDADD(HTTP_ACCEPT_ENCODING, accepte);
	CONDADD(HTTP_COOKIE, cookie);
	CONDADD(CONTENT_TYPE, contenttype);
	CONDADD(REMOTE_USER, remoteuser);
	CONDADD(SERVER_PROTOCOL, protocol);

	if (TG(hc)->contentlength != -1) {
		sprintf(buf, "%ld", (long) TG(hc)->contentlength);
		ADD_STRING("CONTENT_LENGTH");
	}

	if (TG(hc)->authorization[0])
		php_register_variable("AUTH_TYPE", "Basic", track_vars_array TSRMLS_CC);
}

static sapi_module_struct thttpd_sapi_module = {
	"thttpd",
	"thttpd",
	
	php_module_startup,
	php_module_shutdown_wrapper,
	
	NULL,									/* activate */
	NULL,									/* deactivate */

	sapi_thttpd_ub_write,
	NULL,
	NULL,									/* get uid */
	NULL,									/* getenv */

	php_error,
	
	NULL,
	sapi_thttpd_send_headers,
	NULL,
	sapi_thttpd_read_post,
	sapi_thttpd_read_cookies,

	sapi_thttpd_register_variables,
	NULL,									/* Log message */

	NULL,									/* Block interruptions */
	NULL,									/* Unblock interruptions */

	STANDARD_SAPI_MODULE_PROPERTIES
};

static void thttpd_module_main(TSRMLS_D)
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

static void thttpd_request_ctor(TSRMLS_D)
{
	int offset;
	smart_str s = {0};

	SG(request_info).query_string = TG(hc)->query?strdup(TG(hc)->query):NULL;

	smart_str_appends_ex(&s, TG(hc)->hs->cwd, 1);
	smart_str_appends_ex(&s, TG(hc)->expnfilename, 1);
	smart_str_0(&s);
	SG(request_info).path_translated = s.c;
	
	s.c = NULL;
	smart_str_appendc_ex(&s, '/', 1);
	smart_str_appends_ex(&s, TG(hc)->origfilename, 1);
	smart_str_0(&s);
	SG(request_info).request_uri = s.c;
	SG(request_info).request_method = httpd_method_str(TG(hc)->method);
	SG(sapi_headers).http_response_code = 200;
	SG(request_info).content_type = TG(hc)->contenttype;
	SG(request_info).content_length = TG(hc)->contentlength;
	
	php_handle_auth_data(TG(hc)->authorization TSRMLS_CC);

	TG(post_off) = TG(hc)->read_idx - TG(hc)->checked_idx;

	/* avoid feeding \r\n from POST data to SAPI */
	offset = TG(post_off) - SG(request_info).content_length;

	if (offset > 0) {
		TG(post_off) -= offset;
		TG(hc)->read_idx -= offset;
	}
}

static void thttpd_request_dtor(TSRMLS_D)
{
	if (SG(request_info).query_string)
		free(SG(request_info).query_string);
	free(SG(request_info).request_uri);
	free(SG(request_info).path_translated);
}

#ifdef ZTS

#ifdef TSRM_ST
#define thread_create_simple_detached(n) st_thread_create(n, NULL, 0, 0)
#define thread_usleep(n) st_usleep(n)
#define thread_exit() st_thread_exit(NULL)
/* No preemption, simple operations are safe */
#define thread_atomic_inc(n) (++n)
#define thread_atomic_dec(n) (--n)
#else
#error No thread primitives available
#endif

/* We might want to replace this with a STAILQ */
typedef struct qreq {
	httpd_conn *hc;
	struct qreq *next;
} qreq_t;

static MUTEX_T qr_lock;
static qreq_t *queued_requests;
static qreq_t *last_qr;
static int nr_free_threads;
static int nr_threads;
static int max_threads = 50;

#define HANDLE_STRINGS() { \
	HANDLE_STR(encodedurl); \
	HANDLE_STR(decodedurl); \
	HANDLE_STR(origfilename); \
	HANDLE_STR(expnfilename); \
	HANDLE_STR(pathinfo); \
	HANDLE_STR(query); \
	HANDLE_STR(referer); \
	HANDLE_STR(useragent); \
	HANDLE_STR(accept); \
	HANDLE_STR(accepte); \
	HANDLE_STR(acceptl); \
	HANDLE_STR(cookie); \
	HANDLE_STR(contenttype); \
	HANDLE_STR(authorization); \
	HANDLE_STR(remoteuser); \
	}

static httpd_conn *duplicate_conn(httpd_conn *hc, httpd_conn *nhc)
{
	memcpy(nhc, hc, sizeof(*nhc));

#define HANDLE_STR(m) nhc->m = nhc->m ? strdup(nhc->m) : NULL
	HANDLE_STRINGS();
#undef HANDLE_STR
	
	return nhc;
}

static void destroy_conn(httpd_conn *hc)
{
#define HANDLE_STR(m) if (hc->m) free(hc->m)
	HANDLE_STRINGS();
#undef HANDLE_STR
}

static httpd_conn *dequeue_request(void)
{
	httpd_conn *ret = NULL;
	qreq_t *m;
	
	tsrm_mutex_lock(qr_lock);
	if (queued_requests) {
		m = queued_requests;
		ret = m->hc;
		if (!(queued_requests = m->next))
			last_qr = NULL;
		free(m);
	}
	tsrm_mutex_unlock(qr_lock);
	
	return ret;
}

static void *worker_thread(void *);

static void queue_request(httpd_conn *hc)
{
	qreq_t *m;
	httpd_conn *nhc;
	
	/* Mark as long-running request */
	hc->file_address = (char *) 1;

	/*
     * We cannot synchronously revoke accesses to hc in the worker
	 * thread, so we need to pass a copy of hc to the worker thread.
	 */
	nhc = malloc(sizeof *nhc);
	duplicate_conn(hc, nhc);
	
	/* Allocate request queue container */
	m = malloc(sizeof *m);
	m->hc = nhc;
	m->next = NULL;
	
	tsrm_mutex_lock(qr_lock);
	/* Create new threads when reaching a certain threshhold */
	if (nr_threads < max_threads && nr_free_threads < 2) {
		nr_threads++; /* protected by qr_lock */
		
		thread_atomic_inc(nr_free_threads);
		thread_create_simple_detached(worker_thread);
	}
	/* Insert container into request queue */
	if (queued_requests)
		last_qr->next = m;
	else
		queued_requests = m;
	last_qr = m;
	tsrm_mutex_unlock(qr_lock);
}

static off_t thttpd_real_php_request(httpd_conn *hc TSRMLS_DC);

static void *worker_thread(void *dummy)
{
	int do_work = 50;
	httpd_conn *hc;

	while (do_work) {
		hc = dequeue_request();

		if (!hc) {
/*			do_work--; */
			thread_usleep(500000);
			continue;
		}
/*		do_work = 50; */

		thread_atomic_dec(nr_free_threads);

		thttpd_real_php_request(hc TSRMLS_CC);
		shutdown(hc->conn_fd, 0);
		destroy_conn(hc);
		free(hc);

		thread_atomic_inc(nr_free_threads);
	}
	thread_atomic_dec(nr_free_threads);
	thread_atomic_dec(nr_threads);
	thread_exit();
}

static void remove_dead_conn(int fd)
{
	qreq_t *m, *prev = NULL;

	tsrm_mutex_lock(qr_lock);
	m = queued_requests;
	while (m) {
		if (m->hc->conn_fd == fd) {
			if (prev)
				if (!(prev->next = m->next))
					last_qr = prev;
			else
				if (!(queued_requests = m->next))
					last_qr = NULL;
			destroy_conn(m->hc);
			free(m->hc);
			free(m);
			break;
		}
		prev = m;
		m = m->next;
	}
	tsrm_mutex_unlock(qr_lock);
}

#endif

static off_t thttpd_real_php_request(httpd_conn *hc TSRMLS_DC)
{
	TG(hc) = hc;
	hc->bytes_sent = 0;
	
	thttpd_request_ctor(TSRMLS_C);

	thttpd_module_main(TSRMLS_C);

	thttpd_request_dtor(TSRMLS_C);

	return 0;
}

off_t thttpd_php_request(httpd_conn *hc)
{
#ifdef ZTS
	queue_request(hc);
#else
	TSRMLS_FETCH();
	return thttpd_real_php_request(hc TSRMLS_CC);
#endif
}

void thttpd_register_on_close(void (*arg)(int)) 
{
	TSRMLS_FETCH();
	TG(on_close) = arg;
}

void thttpd_closed_conn(int fd)
{
	TSRMLS_FETCH();
	if (TG(on_close)) TG(on_close)(fd);
}

int thttpd_get_fd(void)
{
	TSRMLS_FETCH();
	return TG(hc)->conn_fd;
}

void thttpd_set_dont_close(void)
{
	TSRMLS_FETCH();
	TG(hc)->file_address = (char *) 1;
}


void thttpd_php_init(void)
{
#ifdef ZTS
	tsrm_startup(1, 1, 0, NULL);
	ts_allocate_id(&thttpd_globals_id, sizeof(php_thttpd_globals), NULL, NULL);
	qr_lock = tsrm_mutex_alloc();
	thttpd_register_on_close(remove_dead_conn);
#endif
	sapi_startup(&thttpd_sapi_module);
	thttpd_sapi_module.startup(&thttpd_sapi_module);
	{
		TSRMLS_FETCH();

		SG(server_context) = (void *) 1;
	}
}

void thttpd_php_shutdown(void)
{
	TSRMLS_FETCH();

	if (SG(server_context) != NULL) {
		thttpd_sapi_module.shutdown(&thttpd_sapi_module);
		sapi_shutdown();
#ifdef ZTS
		tsrm_shutdown();
#endif
	}
}
