/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
 */

#include "php.h"

int fcgi_is_fastcgi(void) {
	return 0;
}

struct _fcgi_request {};
typedef struct _fcgi_request fcgi_request;

enum _fcgi_request_type { FCGI_NONE = 1 };
typedef enum _fcgi_request_type fcgi_request_type;

typedef void (*fcgi_apply_func)(const char *var, unsigned int var_len, char *val, unsigned int val_len, void *arg);

void fcgi_terminate(void) {}

void fcgi_loadenv(fcgi_request *req, fcgi_apply_func func, zval *array) {}

int fcgi_listen(const char *path, int backlog) {
	return -1;
}

fcgi_request *fcgi_init_request(int listen_socket, void(*on_accept)(void), void(*on_read)(void), void(*on_close)(void)) {
	return NULL;
}

void fcgi_set_mgmt_var(const char * name, size_t name_len, const char * value, size_t value_len) {}

void fcgi_destroy_request(fcgi_request *req) {}

void fcgi_shutdown(void) {}

int fcgi_accept_request(fcgi_request *req) {
	return 0;
}

int fcgi_has_env(fcgi_request *req) {
	return 0;
}

char* fcgi_quick_getenv(fcgi_request *req, const char* var, int var_len, unsigned int hash_value) {
	return NULL;
}

char* fcgi_quick_putenv(fcgi_request *req, char* var, int var_len, unsigned int hash_value, char* val) {
	return NULL;
}

int fcgi_finish_request(fcgi_request *req, int force_close) {
	return 0;
}

char* fcgi_getenv(fcgi_request *req, const char* var, int var_len) {
	return NULL;
}

int fcgi_read(fcgi_request *req, char *str, int len) {
	return 0;
}

int fcgi_write(fcgi_request *req, fcgi_request_type type, const char *str, int len) {
	return 0;
}

int fcgi_flush(fcgi_request *req, int end) {
	return 0;
}
