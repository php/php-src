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
  | Authors: Brad Lafountain <rodif_bl@yahoo.com>                        |
  |          Shane Caraveo <shane@caraveo.com>                           |
  |          Dmitry Stogov <dmitry@php.net>                              |
  +----------------------------------------------------------------------+
*/

#include "php_soap.h"
#include "ext/standard/base64.h"
#include "ext/standard/md5.h"
#include "ext/random/php_random.h"
#include "ext/hash/php_hash.h"

static char *get_http_header_value_nodup(char *headers, char *type, size_t *len);
static char *get_http_header_value(char *headers, char *type);
static zend_string *get_http_body(php_stream *socketd, int close, char *headers);
static zend_string *get_http_headers(php_stream *socketd);

#define smart_str_append_const(str, const) \
	smart_str_appendl(str,const,sizeof(const)-1)

/* Proxy HTTP Authentication */
int proxy_authentication(zval* this_ptr, smart_str* soap_headers)
{
	zval *login = Z_CLIENT_PROXY_LOGIN_P(this_ptr);
	if (Z_TYPE_P(login) == IS_STRING) {
		smart_str auth = {0};
		smart_str_append(&auth, Z_STR_P(login));
		smart_str_appendc(&auth, ':');

		zval *password = Z_CLIENT_PROXY_PASSWORD_P(this_ptr);
		if (Z_TYPE_P(password) == IS_STRING) {
			smart_str_append(&auth, Z_STR_P(password));
		}
		smart_str_0(&auth);
		zend_string *buf = php_base64_encode((unsigned char*)ZSTR_VAL(auth.s), ZSTR_LEN(auth.s));
		smart_str_append_const(soap_headers, "Proxy-Authorization: Basic ");
		smart_str_append(soap_headers, buf);
		smart_str_append_const(soap_headers, "\r\n");
		zend_string_release_ex(buf, 0);
		smart_str_free(&auth);
		return 1;
	}
	return 0;
}

/* HTTP Authentication */
int basic_authentication(zval* this_ptr, smart_str* soap_headers)
{
	zval *login = Z_CLIENT_LOGIN_P(this_ptr);
	zval *use_digest = Z_CLIENT_USE_DIGEST_P(this_ptr);
	if (Z_TYPE_P(login) == IS_STRING && Z_TYPE_P(use_digest) != IS_TRUE) {
		smart_str auth = {0};
		smart_str_append(&auth, Z_STR_P(login));
		smart_str_appendc(&auth, ':');

		zval *password = Z_CLIENT_PASSWORD_P(this_ptr);
		if (Z_TYPE_P(password) == IS_STRING) {
			smart_str_append(&auth, Z_STR_P(password));
		}
		smart_str_0(&auth);
		zend_string *buf = php_base64_encode((unsigned char*)ZSTR_VAL(auth.s), ZSTR_LEN(auth.s));
		smart_str_append_const(soap_headers, "Authorization: Basic ");
		smart_str_append(soap_headers, buf);
		smart_str_append_const(soap_headers, "\r\n");
		zend_string_release_ex(buf, 0);
		smart_str_free(&auth);
		return 1;
	}
	return 0;
}

/* Additional HTTP headers */
void http_context_headers(php_stream_context* context,
                          bool has_authorization,
                          bool has_proxy_authorization,
                          bool has_cookies,
                          smart_str* soap_headers)
{
	zval *tmp;

	if (context &&
		(tmp = php_stream_context_get_option(context, "http", "header")) != NULL &&
		Z_TYPE_P(tmp) == IS_STRING && Z_STRLEN_P(tmp)) {
		char *s = Z_STRVAL_P(tmp);
		char *p;
		int name_len;

		while (*s) {
			/* skip leading newlines and spaces */
			while (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n') {
				s++;
			}
			/* extract header name */
			p = s;
			name_len = -1;
			while (*p) {
				if (*p == ':') {
					if (name_len < 0) name_len = p - s;
					break;
				} else if (*p == ' ' || *p == '\t') {
					if (name_len < 0) name_len = p - s;
				} else if (*p == '\r' || *p == '\n') {
					break;
				}
				p++;
			}
			if (*p == ':') {
				/* extract header value */
				while (*p && *p != '\r' && *p != '\n') {
					p++;
				}
				/* skip some predefined headers */
				if ((name_len != sizeof("host")-1 ||
				     strncasecmp(s, "host", sizeof("host")-1) != 0) &&
				    (name_len != sizeof("connection")-1 ||
				     strncasecmp(s, "connection", sizeof("connection")-1) != 0) &&
				    (name_len != sizeof("user-agent")-1 ||
				     strncasecmp(s, "user-agent", sizeof("user-agent")-1) != 0) &&
				    (name_len != sizeof("content-length")-1 ||
				     strncasecmp(s, "content-length", sizeof("content-length")-1) != 0) &&
				    (name_len != sizeof("content-type")-1 ||
				     strncasecmp(s, "content-type", sizeof("content-type")-1) != 0) &&
				    (!has_cookies ||
				     name_len != sizeof("cookie")-1 ||
				     strncasecmp(s, "cookie", sizeof("cookie")-1) != 0) &&
				    (!has_authorization ||
				     name_len != sizeof("authorization")-1 ||
				     strncasecmp(s, "authorization", sizeof("authorization")-1) != 0) &&
				    (!has_proxy_authorization ||
				     name_len != sizeof("proxy-authorization")-1 ||
				     strncasecmp(s, "proxy-authorization", sizeof("proxy-authorization")-1) != 0)) {
				    /* add header */
					smart_str_appendl(soap_headers, s, p-s);
					smart_str_append_const(soap_headers, "\r\n");
				}
			}
			s = (*p) ? (p + 1) : p;
		}
	}
}

static php_stream* http_connect(zval* this_ptr, php_url *phpurl, int use_ssl, php_stream_context *context, int *use_proxy)
{
	php_stream *stream;
	zval *tmp, ssl_proxy_peer_name;
	char *host;
	char *name;
	char *protocol;
	zend_long namelen;
	int port;
	int old_error_reporting;
	struct timeval tv;
	struct timeval *timeout = NULL;

	zval *proxy_host = Z_CLIENT_PROXY_HOST_P(this_ptr);
	zval *proxy_port = Z_CLIENT_PROXY_PORT_P(this_ptr);
	if (Z_TYPE_P(proxy_host) == IS_STRING && Z_TYPE_P(proxy_port) == IS_LONG) {
		host = Z_STRVAL_P(proxy_host);
		port = Z_LVAL_P(proxy_port);
		*use_proxy = 1;
	} else {
		host = ZSTR_VAL(phpurl->host);
		port = phpurl->port;
	}

	tmp = Z_CLIENT_CONNECTION_TIMEOUT_P(this_ptr);
	if (Z_TYPE_P(tmp) == IS_LONG && Z_LVAL_P(tmp) > 0) {
		tv.tv_sec = Z_LVAL_P(tmp);
		tv.tv_usec = 0;
		timeout = &tv;
	}

	old_error_reporting = EG(error_reporting);
	EG(error_reporting) &= ~(E_WARNING|E_NOTICE|E_USER_WARNING|E_USER_NOTICE);

	/* Changed ternary operator to an if/else so that additional comparisons can be done on the ssl_method property */
	if (use_ssl && !*use_proxy) {
		tmp = Z_CLIENT_SSL_METHOD_P(this_ptr);
		if (Z_TYPE_P(tmp) == IS_LONG) {
			/* uses constants declared in soap.c to determine ssl uri protocol */
			switch (Z_LVAL_P(tmp)) {
				case SOAP_SSL_METHOD_TLS:
					protocol = "tls";
					break;

				case SOAP_SSL_METHOD_SSLv2:
					protocol = "sslv2";
					break;

				case SOAP_SSL_METHOD_SSLv3:
					protocol = "sslv3";
					break;

				case SOAP_SSL_METHOD_SSLv23:
					protocol = "ssl";
					break;

				default:
					protocol = "ssl";
					break;

			}
		} else {
			protocol = "ssl";
		}
	} else {
		protocol = "tcp";
	}

	namelen = spprintf(&name, 0, "%s://%s:%d", protocol, host, port);

	stream = php_stream_xport_create(name, namelen,
		REPORT_ERRORS,
		STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT,
		NULL /*persistent_id*/,
		timeout,
		context,
		NULL, NULL);
	efree(name);

	/* SSL & proxy */
	if (stream && *use_proxy && use_ssl) {
		smart_str soap_headers = {0};

		/* Set peer_name or name verification will try to use the proxy server name */
		if (!context || (tmp = php_stream_context_get_option(context, "ssl", "peer_name")) == NULL) {
			ZVAL_STR_COPY(&ssl_proxy_peer_name, phpurl->host);
			php_stream_context_set_option(PHP_STREAM_CONTEXT(stream), "ssl", "peer_name", &ssl_proxy_peer_name);
			zval_ptr_dtor(&ssl_proxy_peer_name);
		}

		smart_str_append_const(&soap_headers, "CONNECT ");
		smart_str_appends(&soap_headers, ZSTR_VAL(phpurl->host));
		smart_str_appendc(&soap_headers, ':');
		smart_str_append_unsigned(&soap_headers, phpurl->port);
		smart_str_append_const(&soap_headers, " HTTP/1.1\r\n");
		smart_str_append_const(&soap_headers, "Host: ");
		smart_str_appends(&soap_headers, ZSTR_VAL(phpurl->host));
		if (phpurl->port != 80) {
			smart_str_appendc(&soap_headers, ':');
			smart_str_append_unsigned(&soap_headers, phpurl->port);
		}
		smart_str_append_const(&soap_headers, "\r\n");
		proxy_authentication(this_ptr, &soap_headers);
		smart_str_append_const(&soap_headers, "\r\n");
		if (php_stream_write(stream, ZSTR_VAL(soap_headers.s), ZSTR_LEN(soap_headers.s)) != ZSTR_LEN(soap_headers.s)) {
			php_stream_close(stream);
			stream = NULL;
		}
		smart_str_free(&soap_headers);

		if (stream) {
			zend_string *http_headers = get_http_headers(stream);
			if (http_headers) {
				zend_string_free(http_headers);
			} else {
				php_stream_close(stream);
				stream = NULL;
			}
		}
		/* enable SSL transport layer */
		if (stream) {
			/* if a stream is created without encryption, check to see if SSL method parameter is specified and use
			   proper encrypyion method based on constants defined in soap.c */
			int crypto_method = STREAM_CRYPTO_METHOD_SSLv23_CLIENT;
			tmp = Z_CLIENT_SSL_METHOD_P(this_ptr);
			if (Z_TYPE_P(tmp) == IS_LONG) {
				switch (Z_LVAL_P(tmp)) {
					case SOAP_SSL_METHOD_TLS:
						crypto_method = STREAM_CRYPTO_METHOD_TLS_CLIENT;
						break;

					case SOAP_SSL_METHOD_SSLv2:
						crypto_method = STREAM_CRYPTO_METHOD_SSLv2_CLIENT;
						break;

					case SOAP_SSL_METHOD_SSLv3:
						crypto_method = STREAM_CRYPTO_METHOD_SSLv3_CLIENT;
						break;

					case SOAP_SSL_METHOD_SSLv23:
						crypto_method = STREAM_CRYPTO_METHOD_SSLv23_CLIENT;
						break;

					default:
						crypto_method = STREAM_CRYPTO_METHOD_TLS_CLIENT;
						break;
				}
			}
			if (php_stream_xport_crypto_setup(stream, crypto_method, NULL) < 0 ||
			    php_stream_xport_crypto_enable(stream, 1) < 0) {
				php_stream_close(stream);
				stream = NULL;
			}
		}
	}

	EG(error_reporting) = old_error_reporting;
	return stream;
}

static int in_domain(const char *host, const char *domain)
{
	if (domain[0] == '.') {
		int l1 = strlen(host);
		int l2 = strlen(domain);
		if (l1 > l2) {
			return strcmp(host+l1-l2,domain) == 0;
		} else {
			return 0;
		}
	} else {
		return strcmp(host,domain) == 0;
	}
}

int make_http_soap_request(zval        *this_ptr,
                           zend_string *buf,
                           char        *location,
                           char        *soapaction,
                           int          soap_version,
                           zval        *return_value)
{
	zend_string *request;
	smart_str soap_headers = {0};
	smart_str soap_headers_z = {0};
	size_t err;
	php_url *phpurl = NULL;
	php_stream *stream;
	zval *tmp;
	int use_proxy = 0;
	int use_ssl;
	zend_string *http_body;
	char *content_type, *http_version, *cookie_itt;
	size_t cookie_len;
	int http_close;
	zend_string *http_headers;
	char *connection;
	int http_1_1;
	int http_status;
	int content_type_xml = 0;
	zend_long redirect_max = 20;
	char *content_encoding;
	char *http_msg = NULL;
	bool old_allow_url_fopen;
	php_stream_context *context = NULL;
	bool has_authorization = 0;
	bool has_proxy_authorization = 0;
	bool has_cookies = 0;

	if (this_ptr == NULL || Z_TYPE_P(this_ptr) != IS_OBJECT) {
		return FALSE;
	}

	request = buf;
	/* Compress request */
	tmp = Z_CLIENT_COMPRESSION_P(this_ptr);
	if (Z_TYPE_P(tmp) == IS_LONG) {
		int level = Z_LVAL_P(tmp) & 0x0f;
		int kind  = Z_LVAL_P(tmp) & SOAP_COMPRESSION_DEFLATE;

		if (level > 9) {level = 9;}

	  if ((Z_LVAL_P(tmp) & SOAP_COMPRESSION_ACCEPT) != 0) {
			smart_str_append_const(&soap_headers_z,"Accept-Encoding: gzip, deflate\r\n");
	  }
	  if (level > 0) {
			zval func;
			zval retval;
			zval params[3];
			int n;

			ZVAL_STR_COPY(&params[0], buf);
			ZVAL_LONG(&params[1], level);
			if (kind == SOAP_COMPRESSION_DEFLATE) {
				n = 2;
				ZVAL_STRING(&func, "gzcompress");
				smart_str_append_const(&soap_headers_z,"Content-Encoding: deflate\r\n");
			} else {
				n = 3;
				ZVAL_STRING(&func, "gzencode");
				smart_str_append_const(&soap_headers_z,"Content-Encoding: gzip\r\n");
				ZVAL_LONG(&params[2], 0x1f);
			}
			if (call_user_function(CG(function_table), (zval*)NULL, &func, &retval, n, params) == SUCCESS &&
			    Z_TYPE(retval) == IS_STRING) {
				zval_ptr_dtor(&params[0]);
				zval_ptr_dtor(&func);
				request = Z_STR(retval);
			} else {
				zval_ptr_dtor(&params[0]);
				zval_ptr_dtor(&func);
				if (request != buf) {
					zend_string_release_ex(request, 0);
				}
				smart_str_free(&soap_headers_z);
				return FALSE;
			}
	  }
	}

	tmp = Z_CLIENT_HTTPSOCKET_P(this_ptr);
	if (Z_TYPE_P(tmp) == IS_RESOURCE) {
		php_stream_from_zval_no_verify(stream,tmp);
		tmp = Z_CLIENT_USE_PROXY_P(this_ptr);
		if (Z_TYPE_P(tmp) == IS_LONG) {
			use_proxy = Z_LVAL_P(tmp);
		}
	} else {
		stream = NULL;
	}

	if (location != NULL && location[0] != '\000') {
		phpurl = php_url_parse(location);
	}

	tmp = Z_CLIENT_STREAM_CONTEXT_P(this_ptr);
	if (Z_TYPE_P(tmp) == IS_RESOURCE) {
		context = php_stream_context_from_zval(tmp, 0);
	}

	if (context &&
		(tmp = php_stream_context_get_option(context, "http", "max_redirects")) != NULL) {
		if (Z_TYPE_P(tmp) != IS_STRING || !is_numeric_string(Z_STRVAL_P(tmp), Z_STRLEN_P(tmp), &redirect_max, NULL, 1)) {
			if (Z_TYPE_P(tmp) == IS_LONG)
				redirect_max = Z_LVAL_P(tmp);
		}
	}

try_again:
	if (phpurl == NULL || phpurl->host == NULL) {
	  if (phpurl != NULL) {php_url_free(phpurl);}
		if (request != buf) {
			zend_string_release_ex(request, 0);
		}
		add_soap_fault(this_ptr, "HTTP", "Unable to parse URL", NULL, NULL);
		smart_str_free(&soap_headers_z);
		return FALSE;
	}

	use_ssl = 0;
	if (phpurl->scheme != NULL && zend_string_equals_literal(phpurl->scheme, "https")) {
		use_ssl = 1;
	} else if (phpurl->scheme == NULL || !zend_string_equals_literal(phpurl->scheme, "http")) {
		php_url_free(phpurl);
		if (request != buf) {
			zend_string_release_ex(request, 0);
		}
		add_soap_fault(this_ptr, "HTTP", "Unknown protocol. Only http and https are allowed.", NULL, NULL);
		smart_str_free(&soap_headers_z);
		return FALSE;
	}

	old_allow_url_fopen = PG(allow_url_fopen);
	PG(allow_url_fopen) = 1;
	if (use_ssl && php_stream_locate_url_wrapper("https://", NULL, STREAM_LOCATE_WRAPPERS_ONLY) == NULL) {
		php_url_free(phpurl);
		if (request != buf) {
			zend_string_release_ex(request, 0);
		}
		add_soap_fault(this_ptr, "HTTP", "SSL support is not available in this build", NULL, NULL);
		PG(allow_url_fopen) = old_allow_url_fopen;
		smart_str_free(&soap_headers_z);
		return FALSE;
	}

	if (phpurl->port == 0) {
		phpurl->port = use_ssl ? 443 : 80;
	}

	/* Check if request to the same host */
	if (stream != NULL) {
		php_url *orig;
		tmp = Z_CLIENT_HTTPURL_P(this_ptr);
		if (Z_TYPE_P(tmp) == IS_RESOURCE &&
			(orig = (php_url *) zend_fetch_resource_ex(tmp, "httpurl", le_url)) != NULL &&
		    ((use_proxy && !use_ssl) ||
		     (((use_ssl && orig->scheme != NULL && zend_string_equals_literal(orig->scheme, "https")) ||
		      (!use_ssl && orig->scheme == NULL) ||
		      (!use_ssl && !zend_string_equals_literal(orig->scheme, "https"))) &&
		     zend_string_equals(orig->host, phpurl->host) &&
		     orig->port == phpurl->port))) {
		} else {
			php_stream_close(stream);
			convert_to_null(Z_CLIENT_HTTPURL_P(this_ptr));
			convert_to_null(Z_CLIENT_HTTPSOCKET_P(this_ptr));
			convert_to_null(Z_CLIENT_USE_PROXY_P(this_ptr));
			stream = NULL;
			use_proxy = 0;
		}
	}

	/* Check if keep-alive connection is still opened */
	if (stream != NULL && php_stream_eof(stream)) {
		php_stream_close(stream);
		convert_to_null(Z_CLIENT_HTTPURL_P(this_ptr));
		convert_to_null(Z_CLIENT_HTTPSOCKET_P(this_ptr));
		convert_to_null(Z_CLIENT_USE_PROXY_P(this_ptr));
		stream = NULL;
		use_proxy = 0;
	}

	if (!stream) {
		stream = http_connect(this_ptr, phpurl, use_ssl, context, &use_proxy);
		if (stream) {
			php_stream_auto_cleanup(stream);
			ZVAL_RES(Z_CLIENT_HTTPSOCKET_P(this_ptr), stream->res);
			GC_ADDREF(stream->res);
			ZVAL_LONG(Z_CLIENT_USE_PROXY_P(this_ptr), use_proxy);
		} else {
			php_url_free(phpurl);
			if (request != buf) {
				zend_string_release_ex(request, 0);
			}
			add_soap_fault(this_ptr, "HTTP", "Could not connect to host", NULL, NULL);
			PG(allow_url_fopen) = old_allow_url_fopen;
			smart_str_free(&soap_headers_z);
			return FALSE;
		}
	}
	PG(allow_url_fopen) = old_allow_url_fopen;

	if (stream) {
		zval *cookies, *login, *password;
		zend_resource *ret = zend_register_resource(phpurl, le_url);
		ZVAL_RES(Z_CLIENT_HTTPURL_P(this_ptr), ret);
		GC_ADDREF(ret);

		if (context &&
		    (tmp = php_stream_context_get_option(context, "http", "protocol_version")) != NULL &&
		    Z_TYPE_P(tmp) == IS_DOUBLE &&
		    Z_DVAL_P(tmp) == 1.0) {
			http_1_1 = 0;
		} else {
			http_1_1 = 1;
		}

		smart_str_append_const(&soap_headers, "POST ");
		if (use_proxy && !use_ssl) {
			smart_str_appends(&soap_headers, ZSTR_VAL(phpurl->scheme));
			smart_str_append_const(&soap_headers, "://");
			smart_str_appends(&soap_headers, ZSTR_VAL(phpurl->host));
			smart_str_appendc(&soap_headers, ':');
			smart_str_append_unsigned(&soap_headers, phpurl->port);
		}
		if (phpurl->path) {
			smart_str_appends(&soap_headers, ZSTR_VAL(phpurl->path));
		} else {
			smart_str_appendc(&soap_headers, '/');
		}
		if (phpurl->query) {
			smart_str_appendc(&soap_headers, '?');
			smart_str_appends(&soap_headers, ZSTR_VAL(phpurl->query));
		}
		if (phpurl->fragment) {
			smart_str_appendc(&soap_headers, '#');
			smart_str_appends(&soap_headers, ZSTR_VAL(phpurl->fragment));
		}
		if (http_1_1) {
			smart_str_append_const(&soap_headers, " HTTP/1.1\r\n");
		} else {
			smart_str_append_const(&soap_headers, " HTTP/1.0\r\n");
		}
		smart_str_append_const(&soap_headers, "Host: ");
		smart_str_appends(&soap_headers, ZSTR_VAL(phpurl->host));
		if (phpurl->port != (use_ssl?443:80)) {
			smart_str_appendc(&soap_headers, ':');
			smart_str_append_unsigned(&soap_headers, phpurl->port);
		}
		if (!http_1_1 || Z_TYPE_P(Z_CLIENT_KEEP_ALIVE_P(this_ptr)) == IS_FALSE) {
			smart_str_append_const(&soap_headers, "\r\n"
				"Connection: close\r\n");
		} else {
			smart_str_append_const(&soap_headers, "\r\n"
				"Connection: Keep-Alive\r\n");
		}
		tmp = Z_CLIENT_USER_AGENT_P(this_ptr);
		if (Z_TYPE_P(tmp) == IS_STRING) {
			if (Z_STRLEN_P(tmp) > 0) {
				smart_str_append_const(&soap_headers, "User-Agent: ");
				smart_str_appendl(&soap_headers, Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
				smart_str_append_const(&soap_headers, "\r\n");
			}
		} else if (context &&
		           (tmp = php_stream_context_get_option(context, "http", "user_agent")) != NULL &&
		           Z_TYPE_P(tmp) == IS_STRING) {
			if (Z_STRLEN_P(tmp) > 0) {
				smart_str_append_const(&soap_headers, "User-Agent: ");
				smart_str_appendl(&soap_headers, Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
				smart_str_append_const(&soap_headers, "\r\n");
			}
		} else if (FG(user_agent)) {
			smart_str_append_const(&soap_headers, "User-Agent: ");
			smart_str_appends(&soap_headers, FG(user_agent));
			smart_str_append_const(&soap_headers, "\r\n");
		} else {
			smart_str_append_const(&soap_headers, "User-Agent: PHP-SOAP/"PHP_VERSION"\r\n");
		}

		smart_str_append_smart_str(&soap_headers, &soap_headers_z);

		if (soap_version == SOAP_1_2) {
			if (context &&
				(tmp = php_stream_context_get_option(context, "http", "content_type")) != NULL &&
				Z_TYPE_P(tmp) == IS_STRING &&
				Z_STRLEN_P(tmp) > 0
			) {
				smart_str_append_const(&soap_headers, "Content-Type: ");
				smart_str_appendl(&soap_headers, Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
			} else {
				smart_str_append_const(&soap_headers, "Content-Type: application/soap+xml; charset=utf-8");
			}
			if (soapaction) {
				smart_str_append_const(&soap_headers,"; action=\"");
				smart_str_appends(&soap_headers, soapaction);
				smart_str_append_const(&soap_headers,"\"");
			}
			smart_str_append_const(&soap_headers,"\r\n");
		} else {
			if (context &&
				(tmp = php_stream_context_get_option(context, "http", "content_type")) != NULL &&
				Z_TYPE_P(tmp) == IS_STRING &&
				Z_STRLEN_P(tmp) > 0
			) {
				smart_str_append_const(&soap_headers, "Content-Type: ");
				smart_str_appendl(&soap_headers, Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
				smart_str_append_const(&soap_headers, "\r\n");
			} else {
				smart_str_append_const(&soap_headers, "Content-Type: text/xml; charset=utf-8\r\n");
			}
			if (soapaction) {
				smart_str_append_const(&soap_headers, "SOAPAction: \"");
				smart_str_appends(&soap_headers, soapaction);
				smart_str_append_const(&soap_headers, "\"\r\n");
			}
		}
		smart_str_append_const(&soap_headers,"Content-Length: ");
		smart_str_append_long(&soap_headers, request->len);
		smart_str_append_const(&soap_headers, "\r\n");

		/* HTTP Authentication */
		login = Z_CLIENT_LOGIN_P(this_ptr);
		if (Z_TYPE_P(login) == IS_STRING) {
			zval *digest = Z_CLIENT_DIGEST_P(this_ptr);

			has_authorization = 1;
			if (Z_TYPE_P(digest) == IS_ARRAY) {
				char          HA1[33], HA2[33], response[33], cnonce[33], nc[9];
				unsigned char nonce[16];
				PHP_MD5_CTX   md5ctx;
				unsigned char hash[16];

				if (UNEXPECTED(php_random_bytes_throw(&nonce, sizeof(nonce)) != SUCCESS)) {
					ZEND_ASSERT(EG(exception));
					php_stream_close(stream);
					convert_to_null(Z_CLIENT_HTTPURL_P(this_ptr));
					convert_to_null(Z_CLIENT_HTTPSOCKET_P(this_ptr));
					convert_to_null(Z_CLIENT_USE_PROXY_P(this_ptr));
					smart_str_free(&soap_headers_z);
					smart_str_free(&soap_headers);
					return FALSE;
				}

				php_hash_bin2hex(cnonce, nonce, sizeof(nonce));
				cnonce[32] = 0;

				if ((tmp = zend_hash_str_find(Z_ARRVAL_P(digest), "nc", sizeof("nc")-1)) != NULL &&
					Z_TYPE_P(tmp) == IS_LONG) {
					Z_LVAL_P(tmp)++;
					snprintf(nc, sizeof(nc), "%08" ZEND_LONG_FMT_SPEC, Z_LVAL_P(tmp));
				} else {
					add_assoc_long(digest, "nc", 1);
					strcpy(nc, "00000001");
				}

				PHP_MD5Init(&md5ctx);
				PHP_MD5Update(&md5ctx, (unsigned char*)Z_STRVAL_P(login), Z_STRLEN_P(login));
				PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
				if ((tmp = zend_hash_str_find(Z_ARRVAL_P(digest), "realm", sizeof("realm")-1)) != NULL &&
					Z_TYPE_P(tmp) == IS_STRING) {
					PHP_MD5Update(&md5ctx, (unsigned char*)Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
				}
				PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
				password = Z_CLIENT_PASSWORD_P(this_ptr);
				if (Z_TYPE_P(password) == IS_STRING) {
					PHP_MD5Update(&md5ctx, (unsigned char*)Z_STRVAL_P(password), Z_STRLEN_P(password));
				}
				PHP_MD5Final(hash, &md5ctx);
				make_digest(HA1, hash);
				if ((tmp = zend_hash_str_find(Z_ARRVAL_P(digest), "algorithm", sizeof("algorithm")-1)) != NULL &&
					Z_TYPE_P(tmp) == IS_STRING &&
					Z_STRLEN_P(tmp) == sizeof("md5-sess")-1 &&
					stricmp(Z_STRVAL_P(tmp), "md5-sess") == 0) {
					PHP_MD5Init(&md5ctx);
					PHP_MD5Update(&md5ctx, (unsigned char*)HA1, 32);
					PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
					if ((tmp = zend_hash_str_find(Z_ARRVAL_P(digest), "nonce", sizeof("nonce")-1)) != NULL &&
						Z_TYPE_P(tmp) == IS_STRING) {
						PHP_MD5Update(&md5ctx, (unsigned char*)Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
					}
					PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
					PHP_MD5Update(&md5ctx, (unsigned char*)cnonce, 8);
					PHP_MD5Final(hash, &md5ctx);
					make_digest(HA1, hash);
				}

				PHP_MD5Init(&md5ctx);
				PHP_MD5Update(&md5ctx, (unsigned char*)"POST:", sizeof("POST:")-1);
				if (phpurl->path) {
					PHP_MD5Update(&md5ctx, (unsigned char*)ZSTR_VAL(phpurl->path), ZSTR_LEN(phpurl->path));
				} else {
					PHP_MD5Update(&md5ctx, (unsigned char*)"/", 1);
				}
				if (phpurl->query) {
					PHP_MD5Update(&md5ctx, (unsigned char*)"?", 1);
					PHP_MD5Update(&md5ctx, (unsigned char*)ZSTR_VAL(phpurl->query), ZSTR_LEN(phpurl->query));
				}

				PHP_MD5Final(hash, &md5ctx);
				make_digest(HA2, hash);

				PHP_MD5Init(&md5ctx);
				PHP_MD5Update(&md5ctx, (unsigned char*)HA1, 32);
				PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
				if ((tmp = zend_hash_str_find(Z_ARRVAL_P(digest), "nonce", sizeof("nonce")-1)) != NULL &&
					Z_TYPE_P(tmp) == IS_STRING) {
					PHP_MD5Update(&md5ctx, (unsigned char*)Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
				}
				PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
				if ((tmp = zend_hash_str_find(Z_ARRVAL_P(digest), "qop", sizeof("qop")-1)) != NULL &&
					Z_TYPE_P(tmp) == IS_STRING) {
					PHP_MD5Update(&md5ctx, (unsigned char*)nc, 8);
					PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
					PHP_MD5Update(&md5ctx, (unsigned char*)cnonce, 8);
					PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
					/* TODO: Support for qop="auth-int" */
					PHP_MD5Update(&md5ctx, (unsigned char*)"auth", sizeof("auth")-1);
					PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
				}
				PHP_MD5Update(&md5ctx, (unsigned char*)HA2, 32);
				PHP_MD5Final(hash, &md5ctx);
				make_digest(response, hash);

				smart_str_append_const(&soap_headers, "Authorization: Digest username=\"");
				smart_str_appendl(&soap_headers, Z_STRVAL_P(login), Z_STRLEN_P(login));
				if ((tmp = zend_hash_str_find(Z_ARRVAL_P(digest), "realm", sizeof("realm")-1)) != NULL &&
					Z_TYPE_P(tmp) == IS_STRING) {
					smart_str_append_const(&soap_headers, "\", realm=\"");
					smart_str_appendl(&soap_headers, Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
				}
				if ((tmp = zend_hash_str_find(Z_ARRVAL_P(digest), "nonce", sizeof("nonce")-1)) != NULL &&
					Z_TYPE_P(tmp) == IS_STRING) {
					smart_str_append_const(&soap_headers, "\", nonce=\"");
					smart_str_appendl(&soap_headers, Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
				}
				smart_str_append_const(&soap_headers, "\", uri=\"");
				if (phpurl->path) {
					smart_str_appends(&soap_headers, ZSTR_VAL(phpurl->path));
				} else {
					smart_str_appendc(&soap_headers, '/');
				}
				if (phpurl->query) {
					smart_str_appendc(&soap_headers, '?');
					smart_str_appends(&soap_headers, ZSTR_VAL(phpurl->query));
				}
				if (phpurl->fragment) {
					smart_str_appendc(&soap_headers, '#');
					smart_str_appends(&soap_headers, ZSTR_VAL(phpurl->fragment));
				}
				if ((tmp = zend_hash_str_find(Z_ARRVAL_P(digest), "qop", sizeof("qop")-1)) != NULL &&
					Z_TYPE_P(tmp) == IS_STRING) {
				/* TODO: Support for qop="auth-int" */
					smart_str_append_const(&soap_headers, "\", qop=\"auth");
					smart_str_append_const(&soap_headers, "\", nc=\"");
					smart_str_appendl(&soap_headers, nc, 8);
					smart_str_append_const(&soap_headers, "\", cnonce=\"");
					smart_str_appendl(&soap_headers, cnonce, 8);
				}
				smart_str_append_const(&soap_headers, "\", response=\"");
				smart_str_appendl(&soap_headers, response, 32);
				if ((tmp = zend_hash_str_find(Z_ARRVAL_P(digest), "opaque", sizeof("opaque")-1)) != NULL &&
					Z_TYPE_P(tmp) == IS_STRING) {
					smart_str_append_const(&soap_headers, "\", opaque=\"");
					smart_str_appendl(&soap_headers, Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
				}
				if ((tmp = zend_hash_str_find(Z_ARRVAL_P(digest), "algorithm", sizeof("algorithm")-1)) != NULL &&
					Z_TYPE_P(tmp) == IS_STRING) {
					smart_str_append_const(&soap_headers, "\", algorithm=\"");
					smart_str_appendl(&soap_headers, Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
				}
				smart_str_append_const(&soap_headers, "\"\r\n");
			} else {
				zend_string *buf;

				smart_str auth = {0};
				smart_str_append(&auth, Z_STR_P(login));
				smart_str_appendc(&auth, ':');
				password = Z_CLIENT_PASSWORD_P(this_ptr);
				if (Z_TYPE_P(password) == IS_STRING) {
					smart_str_append(&auth, Z_STR_P(password));
				}
				smart_str_0(&auth);
				buf = php_base64_encode((unsigned char*)ZSTR_VAL(auth.s), ZSTR_LEN(auth.s));
				smart_str_append_const(&soap_headers, "Authorization: Basic ");
				smart_str_append(&soap_headers, buf);
				smart_str_append_const(&soap_headers, "\r\n");
				zend_string_release_ex(buf, 0);
				smart_str_free(&auth);
			}
		}

		/* Proxy HTTP Authentication */
		if (use_proxy && !use_ssl) {
			has_proxy_authorization = proxy_authentication(this_ptr, &soap_headers);
		}

		/* Send cookies along with request */
		cookies = Z_CLIENT_COOKIES_P(this_ptr);
		ZEND_ASSERT(Z_TYPE_P(cookies) == IS_ARRAY);
		if (zend_hash_num_elements(Z_ARRVAL_P(cookies)) != 0 && !HT_IS_PACKED(Z_ARRVAL_P(cookies))) {
			zval *data;
			zend_string *key;
			has_cookies = 1;
			smart_str_append_const(&soap_headers, "Cookie: ");
			ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(cookies), key, data) {
				if (key && Z_TYPE_P(data) == IS_ARRAY) {
					zval *value;

					if ((value = zend_hash_index_find(Z_ARRVAL_P(data), 0)) != NULL &&
						Z_TYPE_P(value) == IS_STRING) {
					  zval *tmp;
					  if (((tmp = zend_hash_index_find(Z_ARRVAL_P(data), 1)) == NULL ||
						   Z_TYPE_P(tmp) != IS_STRING ||
						   strncmp(phpurl->path?ZSTR_VAL(phpurl->path):"/",Z_STRVAL_P(tmp),Z_STRLEN_P(tmp)) == 0) &&
						  ((tmp = zend_hash_index_find(Z_ARRVAL_P(data), 2)) == NULL ||
						   Z_TYPE_P(tmp) != IS_STRING ||
						   in_domain(ZSTR_VAL(phpurl->host),Z_STRVAL_P(tmp))) &&
						  (use_ssl || (tmp = zend_hash_index_find(Z_ARRVAL_P(data), 3)) == NULL)) {
							smart_str_append(&soap_headers, key);
							smart_str_appendc(&soap_headers, '=');
							smart_str_append(&soap_headers, Z_STR_P(value));
							smart_str_appendc(&soap_headers, ';');
						}
					}
				}
			} ZEND_HASH_FOREACH_END();
			smart_str_append_const(&soap_headers, "\r\n");
		}

		http_context_headers(context, has_authorization, has_proxy_authorization, has_cookies, &soap_headers);

		smart_str_append_const(&soap_headers, "\r\n");
		smart_str_0(&soap_headers);
		if (Z_TYPE_P(Z_CLIENT_TRACE_P(this_ptr)) == IS_TRUE) {
			zval_ptr_dtor(Z_CLIENT_LAST_REQUEST_HEADERS_P(this_ptr));
			/* Need to copy the string here, as we continue appending to soap_headers below. */
			ZVAL_STRINGL(Z_CLIENT_LAST_REQUEST_HEADERS_P(this_ptr),
				ZSTR_VAL(soap_headers.s), ZSTR_LEN(soap_headers.s));
		}
		smart_str_appendl(&soap_headers, request->val, request->len);
		smart_str_0(&soap_headers);

		err = php_stream_write(stream, ZSTR_VAL(soap_headers.s), ZSTR_LEN(soap_headers.s));
		if (err != ZSTR_LEN(soap_headers.s)) {
			if (request != buf) {
				zend_string_release_ex(request, 0);
			}
			php_stream_close(stream);
			convert_to_null(Z_CLIENT_HTTPURL_P(this_ptr));
			convert_to_null(Z_CLIENT_HTTPSOCKET_P(this_ptr));
			convert_to_null(Z_CLIENT_USE_PROXY_P(this_ptr));
			add_soap_fault(this_ptr, "HTTP", "Failed Sending HTTP SOAP request", NULL, NULL);
			smart_str_free(&soap_headers_z);
			return FALSE;
		}
		smart_str_free(&soap_headers);
	} else {
		add_soap_fault(this_ptr, "HTTP", "Failed to create stream??", NULL, NULL);
		smart_str_free(&soap_headers_z);
		return FALSE;
	}

	if (!return_value) {
		php_stream_close(stream);
		convert_to_null(Z_CLIENT_HTTPSOCKET_P(this_ptr));
		convert_to_null(Z_CLIENT_USE_PROXY_P(this_ptr));
		smart_str_free(&soap_headers_z);
		return TRUE;
	}

	do {
		http_headers = get_http_headers(stream);
		if (!http_headers) {
			if (request != buf) {
				zend_string_release_ex(request, 0);
			}
			php_stream_close(stream);
			convert_to_null(Z_CLIENT_HTTPSOCKET_P(this_ptr));
			convert_to_null(Z_CLIENT_USE_PROXY_P(this_ptr));
			add_soap_fault(this_ptr, "HTTP", "Error Fetching http headers", NULL, NULL);
			smart_str_free(&soap_headers_z);
			return FALSE;
		}

		if (Z_TYPE_P(Z_CLIENT_TRACE_P(this_ptr)) == IS_TRUE) {
			zval_ptr_dtor(Z_CLIENT_LAST_RESPONSE_HEADERS_P(this_ptr));
			ZVAL_STR_COPY(Z_CLIENT_LAST_RESPONSE_HEADERS_P(this_ptr), http_headers);
		}

		/* Check to see what HTTP status was sent */
		http_1_1 = 0;
		http_status = 0;
		http_version = get_http_header_value(ZSTR_VAL(http_headers), "HTTP/");
		if (http_version) {
			char *tmp;

			if (!strncmp(http_version,"1.1", 3)) {
				http_1_1 = 1;
			}

			tmp = strstr(http_version," ");
			if (tmp != NULL) {
				tmp++;
				http_status = atoi(tmp);
			}
			tmp = strstr(tmp," ");
			if (tmp != NULL) {
				tmp++;
				if (http_msg) {
					efree(http_msg);
				}
				http_msg = estrdup(tmp);
			}
			efree(http_version);

			/* Try and get headers again */
			if (http_status == 100) {
				zend_string_release_ex(http_headers, 0);
			}
		}
	} while (http_status == 100);

	/* Grab and send back every cookie */

	/* Not going to worry about Path: because
	   we shouldn't be changing urls so path doesn't
	   matter too much
	*/
	cookie_itt = ZSTR_VAL(http_headers);

	while ((cookie_itt = get_http_header_value_nodup(cookie_itt, "Set-Cookie: ", &cookie_len))) {
		zval *cookies = Z_CLIENT_COOKIES_P(this_ptr);
		SEPARATE_ARRAY(cookies);

		char *cookie = estrndup(cookie_itt, cookie_len);
		char *eqpos = strstr(cookie, "=");
		char *sempos = strstr(cookie, ";");
		if (eqpos != NULL && (sempos == NULL || sempos > eqpos)) {
			smart_str name = {0};
			int cookie_len;
			zval zcookie;

			if (sempos != NULL) {
				cookie_len = sempos-(eqpos+1);
			} else {
				cookie_len = strlen(cookie)-(eqpos-cookie)-1;
			}

			smart_str_appendl(&name, cookie, eqpos - cookie);
			smart_str_0(&name);

			array_init(&zcookie);
			add_index_stringl(&zcookie, 0, eqpos + 1, cookie_len);

			if (sempos != NULL) {
				char *options = cookie + cookie_len+1;
				while (*options) {
					while (*options == ' ') {options++;}
					sempos = strstr(options, ";");
					if (strstr(options,"path=") == options) {
						eqpos = options + sizeof("path=")-1;
						add_index_stringl(&zcookie, 1, eqpos, sempos?(size_t)(sempos-eqpos):strlen(eqpos));
					} else if (strstr(options,"domain=") == options) {
						eqpos = options + sizeof("domain=")-1;
						add_index_stringl(&zcookie, 2, eqpos, sempos?(size_t)(sempos-eqpos):strlen(eqpos));
					} else if (strstr(options,"secure") == options) {
						add_index_bool(&zcookie, 3, 1);
					}
					if (sempos != NULL) {
						options = sempos+1;
					} else {
					  break;
					}
				}
			}
			if (!zend_hash_index_exists(Z_ARRVAL(zcookie), 1)) {
				char *t = phpurl->path?ZSTR_VAL(phpurl->path):"/";
				char *c = strrchr(t, '/');
				if (c) {
					add_index_stringl(&zcookie, 1, t, c-t);
				}
			}
			if (!zend_hash_index_exists(Z_ARRVAL(zcookie), 2)) {
				add_index_str(&zcookie, 2, phpurl->host);
				GC_ADDREF(phpurl->host);
			}

			zend_symtable_update(Z_ARRVAL_P(cookies), name.s, &zcookie);
			smart_str_free(&name);
		}

		cookie_itt = cookie_itt + cookie_len;
		efree(cookie);
	}

	/* See if the server requested a close */
	if (http_1_1) {
		http_close = FALSE;
		if (use_proxy && !use_ssl) {
			connection = get_http_header_value(ZSTR_VAL(http_headers), "Proxy-Connection: ");
			if (connection) {
				if (strncasecmp(connection, "close", sizeof("close")-1) == 0) {
					http_close = TRUE;
				}
				efree(connection);
			}
		}
		if (http_close == FALSE) {
			connection = get_http_header_value(ZSTR_VAL(http_headers), "Connection: ");
			if (connection) {
				if (strncasecmp(connection, "close", sizeof("close")-1) == 0) {
					http_close = TRUE;
				}
				efree(connection);
			}
		}
	} else {
		http_close = TRUE;
		if (use_proxy && !use_ssl) {
			connection = get_http_header_value(ZSTR_VAL(http_headers), "Proxy-Connection: ");
			if (connection) {
				if (strncasecmp(connection, "Keep-Alive", sizeof("Keep-Alive")-1) == 0) {
					http_close = FALSE;
				}
				efree(connection);
			}
		}
		if (http_close == TRUE) {
			connection = get_http_header_value(ZSTR_VAL(http_headers), "Connection: ");
			if (connection) {
				if (strncasecmp(connection, "Keep-Alive", sizeof("Keep-Alive")-1) == 0) {
					http_close = FALSE;
				}
				efree(connection);
			}
		}
	}


	http_body = get_http_body(stream, http_close, ZSTR_VAL(http_headers));
	if (!http_body) {
		if (request != buf) {
			zend_string_release_ex(request, 0);
		}
		php_stream_close(stream);
		zend_string_release_ex(http_headers, 0);
		convert_to_null(Z_CLIENT_HTTPSOCKET_P(this_ptr));
		convert_to_null(Z_CLIENT_USE_PROXY_P(this_ptr));
		add_soap_fault(this_ptr, "HTTP", "Error Fetching http body, No Content-Length, connection closed or chunked data", NULL, NULL);
		if (http_msg) {
			efree(http_msg);
		}
		smart_str_free(&soap_headers_z);
		return FALSE;
	}

	if (request != buf) {
		zend_string_release_ex(request, 0);
	}

	if (http_close) {
		php_stream_close(stream);
		convert_to_null(Z_CLIENT_HTTPSOCKET_P(this_ptr));
		convert_to_null(Z_CLIENT_USE_PROXY_P(this_ptr));
		stream = NULL;
	}

	/* Process HTTP status codes */
	if (http_status >= 300 && http_status < 400) {
		char *loc;

		if ((loc = get_http_header_value(ZSTR_VAL(http_headers), "Location: ")) != NULL) {
			php_url *new_url  = php_url_parse(loc);

			if (new_url != NULL) {
				zend_string_release_ex(http_headers, 0);
				zend_string_release_ex(http_body, 0);
				efree(loc);
				if (new_url->scheme == NULL && new_url->path != NULL) {
					new_url->scheme = phpurl->scheme ? zend_string_copy(phpurl->scheme) : NULL;
					new_url->host = phpurl->host ? zend_string_copy(phpurl->host) : NULL;
					new_url->port = phpurl->port;
					if (new_url->path && ZSTR_VAL(new_url->path)[0] != '/') {
						if (phpurl->path) {
							char *t = ZSTR_VAL(phpurl->path);
							char *p = strrchr(t, '/');
							if (p) {
								zend_string *s = zend_string_alloc((p - t) + ZSTR_LEN(new_url->path) + 2, 0);
								strncpy(ZSTR_VAL(s), t, (p - t) + 1);
								ZSTR_VAL(s)[(p - t) + 1] = 0;
								strcat(ZSTR_VAL(s), ZSTR_VAL(new_url->path));
								zend_string_release_ex(new_url->path, 0);
								new_url->path = s;
							}
						} else {
							zend_string *s = zend_string_alloc(ZSTR_LEN(new_url->path) + 2, 0);
							ZSTR_VAL(s)[0] = '/';
							ZSTR_VAL(s)[1] = 0;
							strcat(ZSTR_VAL(s), ZSTR_VAL(new_url->path));
							zend_string_release_ex(new_url->path, 0);
							new_url->path = s;
						}
					}
				}
				phpurl = new_url;

				if (--redirect_max < 1) {
					add_soap_fault(this_ptr, "HTTP", "Redirection limit reached, aborting", NULL, NULL);
					smart_str_free(&soap_headers_z);
					return FALSE;
				}

				goto try_again;
			}
		}
	} else if (http_status == 401) {
		/* Digest authentication */
		zval *digest = Z_CLIENT_DIGEST_P(this_ptr);
		zval *login = Z_CLIENT_LOGIN_P(this_ptr);
		zval *password = Z_CLIENT_PASSWORD_P(this_ptr);
		char *auth = get_http_header_value(ZSTR_VAL(http_headers), "WWW-Authenticate: ");
		if (auth && strstr(auth, "Digest") == auth && Z_TYPE_P(digest) != IS_ARRAY
				&& Z_TYPE_P(login) == IS_STRING && Z_TYPE_P(password) == IS_STRING) {
			char *s;
			zval digest;

			ZVAL_UNDEF(&digest);
			s = auth + sizeof("Digest")-1;
			while (*s != '\0') {
				char *name, *val;
				while (*s == ' ') ++s;
				name = s;
				while (*s != '\0' && *s != '=') ++s;
				if (*s == '=') {
					*s = '\0';
					++s;
					if (*s == '"') {
						++s;
						val = s;
						while (*s != '\0' && *s != '"') ++s;
					} else {
						val = s;
						while (*s != '\0' && *s != ' ' && *s != ',') ++s;
					}
					if (*s != '\0') {
						if (*s != ',') {
							*s = '\0';
							++s;
							while (*s != '\0' && *s != ',') ++s;
							if (*s != '\0') ++s;
						} else {
							*s = '\0';
							++s;
						}
					}
					if (Z_TYPE(digest) == IS_UNDEF) {
						array_init(&digest);
					}
					add_assoc_string(&digest, name, val);
				}
			}

			if (Z_TYPE(digest) != IS_UNDEF) {
				php_url *new_url = emalloc(sizeof(php_url));

				zval_ptr_dtor(Z_CLIENT_DIGEST_P(this_ptr));
				ZVAL_COPY_VALUE(Z_CLIENT_DIGEST_P(this_ptr), &digest);

				*new_url = *phpurl;
				if (phpurl->scheme) phpurl->scheme = zend_string_copy(phpurl->scheme);
				if (phpurl->user) phpurl->user = zend_string_copy(phpurl->user);
				if (phpurl->pass) phpurl->pass = zend_string_copy(phpurl->pass);
				if (phpurl->host) phpurl->host = zend_string_copy(phpurl->host);
				if (phpurl->path) phpurl->path = zend_string_copy(phpurl->path);
				if (phpurl->query) phpurl->query = zend_string_copy(phpurl->query);
				if (phpurl->fragment) phpurl->fragment = zend_string_copy(phpurl->fragment);
				phpurl = new_url;

				efree(auth);
				zend_string_release_ex(http_headers, 0);
				zend_string_release_ex(http_body, 0);

				goto try_again;
			}
		}
		if (auth) efree(auth);
	}
	smart_str_free(&soap_headers_z);

	/* Check and see if the server even sent a xml document */
	content_type = get_http_header_value(ZSTR_VAL(http_headers), "Content-Type: ");
	if (content_type) {
		char *pos = NULL;
		int cmplen;
		pos = strstr(content_type,";");
		if (pos != NULL) {
			cmplen = pos - content_type;
		} else {
			cmplen = strlen(content_type);
		}
		if (strncmp(content_type, "text/xml", cmplen) == 0 ||
		    strncmp(content_type, "application/soap+xml", cmplen) == 0) {
			content_type_xml = 1;
/*
			if (strncmp(http_body, "<?xml", 5)) {
				zval *err;
				MAKE_STD_ZVAL(err);
				ZVAL_STRINGL(err, http_body, http_body_size, 1);
				add_soap_fault(this_ptr, "HTTP", "Didn't receive an xml document", NULL, err);
				efree(content_type);
				zend_string_release_ex(http_headers, 0);
				efree(http_body);
				return FALSE;
			}
*/
		}
		efree(content_type);
	}

	/* Decompress response */
	content_encoding = get_http_header_value(ZSTR_VAL(http_headers), "Content-Encoding: ");
	if (content_encoding) {
		zval func;
		zval retval;
		zval params[1];

		if ((strcmp(content_encoding,"gzip") == 0 ||
		     strcmp(content_encoding,"x-gzip") == 0) &&
		     zend_hash_str_exists(EG(function_table), "gzinflate", sizeof("gzinflate")-1)) {
			ZVAL_STRING(&func, "gzinflate");
			ZVAL_STRINGL(&params[0], http_body->val+10, http_body->len-10);
		} else if (strcmp(content_encoding,"deflate") == 0 &&
		           zend_hash_str_exists(EG(function_table), "gzuncompress", sizeof("gzuncompress")-1)) {
			ZVAL_STRING(&func, "gzuncompress");
			ZVAL_STR_COPY(&params[0], http_body);
		} else {
			efree(content_encoding);
			zend_string_release_ex(http_headers, 0);
			zend_string_release_ex(http_body, 0);
			if (http_msg) {
				efree(http_msg);
			}
			add_soap_fault(this_ptr, "HTTP", "Unknown Content-Encoding", NULL, NULL);
			return FALSE;
		}
		if (call_user_function(CG(function_table), (zval*)NULL, &func, &retval, 1, params) == SUCCESS &&
		    Z_TYPE(retval) == IS_STRING) {
			zval_ptr_dtor(&params[0]);
			zval_ptr_dtor(&func);
			zend_string_release_ex(http_body, 0);
			ZVAL_COPY_VALUE(return_value, &retval);
		} else {
			zval_ptr_dtor(&params[0]);
			zval_ptr_dtor(&func);
			efree(content_encoding);
			zend_string_release_ex(http_headers, 0);
			zend_string_release_ex(http_body, 0);
			add_soap_fault(this_ptr, "HTTP", "Can't uncompress compressed response", NULL, NULL);
			if (http_msg) {
				efree(http_msg);
			}
			return FALSE;
		}
		efree(content_encoding);
	} else {
		ZVAL_STR(return_value, http_body);
	}

	zend_string_release_ex(http_headers, 0);

	if (http_status >= 400) {
		int error = 0;

		if (Z_STRLEN_P(return_value) == 0) {
			error = 1;
		} else if (Z_STRLEN_P(return_value) > 0) {
			if (!content_type_xml) {
				char *s = Z_STRVAL_P(return_value);

				while (*s != '\0' && *s < ' ') {
					s++;
				}
				if (strncmp(s, "<?xml", 5)) {
					error = 1;
				}
			}
		}

		if (error) {
			zval_ptr_dtor(return_value);
			ZVAL_UNDEF(return_value);
			add_soap_fault(this_ptr, "HTTP", http_msg, NULL, NULL);
			efree(http_msg);
			return FALSE;
		}
	}

	if (http_msg) {
		efree(http_msg);
	}

	return TRUE;
}

static char *get_http_header_value_nodup(char *headers, char *type, size_t *len)
{
	char *pos, *tmp = NULL;
	int typelen, headerslen;

	typelen = strlen(type);
	headerslen = strlen(headers);

	/* header `titles' can be lower case, or any case combination, according
	 * to the various RFC's. */
	pos = headers;
	do {
		/* start of buffer or start of line */
		if (strncasecmp(pos, type, typelen) == 0) {
			char *eol;

			/* match */
			tmp = pos + typelen;

			/* strip leading whitespace */
			while (*tmp == ' ' || *tmp == '\t') {
				tmp++;
			}

			eol = strchr(tmp, '\n');
			if (eol == NULL) {
				eol = headers + headerslen;
			} else if (eol > tmp) {
				if (*(eol-1) == '\r') {
					eol--;
				}

				/* strip trailing whitespace */
				while (eol > tmp && (*(eol-1) == ' ' || *(eol-1) == '\t')) {
					eol--;
				}
			}

			*len = eol - tmp;
			return tmp;
		}

		/* find next line */
		pos = strchr(pos, '\n');
		if (pos) {
			pos++;
		}

	} while (pos);

	return NULL;
}

static char *get_http_header_value(char *headers, char *type)
{
	size_t len;
	char *value;

	value = get_http_header_value_nodup(headers, type, &len);

	if (value) {
		return estrndup(value, len);
	}

	return NULL;
}

static zend_string* get_http_body(php_stream *stream, int close, char *headers)
{
	zend_string *http_buf = NULL;
	char *header;
	int header_close = close, header_chunked = 0, header_length = 0, http_buf_size = 0;

	if (!close) {
		header = get_http_header_value(headers, "Connection: ");
		if (header) {
			if(!strncasecmp(header, "close", sizeof("close")-1)) header_close = 1;
			efree(header);
		}
	}
	header = get_http_header_value(headers, "Transfer-Encoding: ");
	if (header) {
		if(!strncasecmp(header, "chunked", sizeof("chunked")-1)) header_chunked = 1;
		efree(header);
	}
	header = get_http_header_value(headers, "Content-Length: ");
	if (header) {
		header_length = atoi(header);
		efree(header);
		if (!header_length && !header_chunked) {
			/* Empty response */
			return ZSTR_EMPTY_ALLOC();
		}
	}

	if (header_chunked) {
		char ch, done, headerbuf[8192];

		done = FALSE;

		while (!done) {
			int buf_size = 0;

			php_stream_gets(stream, headerbuf, sizeof(headerbuf));
			if (sscanf(headerbuf, "%x", &buf_size) > 0 ) {
				if (buf_size > 0) {
					size_t len_size = 0;

					if (http_buf_size + buf_size + 1 < 0) {
						if (http_buf) {
							zend_string_release_ex(http_buf, 0);
						}
						return NULL;
					}

					if (http_buf) {
						http_buf = zend_string_realloc(http_buf, http_buf_size + buf_size, 0);
					} else {
						http_buf = zend_string_alloc(buf_size, 0);
					}

					while (len_size < buf_size) {
						ssize_t len_read = php_stream_read(stream, http_buf->val + http_buf_size, buf_size - len_size);
						if (len_read <= 0) {
							/* Error or EOF */
							done = TRUE;
						  break;
						}
						len_size += len_read;
	 					http_buf_size += len_read;
					}

					/* Eat up '\r' '\n' */
					ch = php_stream_getc(stream);
					if (ch == '\r') {
						ch = php_stream_getc(stream);
					}
					if (ch != '\n') {
						/* Something wrong in chunked encoding */
						if (http_buf) {
							zend_string_release_ex(http_buf, 0);
						}
						return NULL;
					}
				}
			} else {
				/* Something wrong in chunked encoding */
				if (http_buf) {
					zend_string_release_ex(http_buf, 0);
				}
				return NULL;
			}
			if (buf_size == 0) {
				done = TRUE;
			}
		}

		/* Ignore trailer headers */
		while (1) {
			if (!php_stream_gets(stream, headerbuf, sizeof(headerbuf))) {
				break;
			}

			if ((headerbuf[0] == '\r' && headerbuf[1] == '\n') ||
			    (headerbuf[0] == '\n')) {
				/* empty line marks end of headers */
				break;
			}
		}

		if (http_buf == NULL) {
			return ZSTR_EMPTY_ALLOC();
		}

	} else if (header_length) {
		if (header_length < 0 || header_length >= INT_MAX) {
			return NULL;
		}
		http_buf = zend_string_alloc(header_length, 0);
		while (http_buf_size < header_length) {
			ssize_t len_read = php_stream_read(stream, http_buf->val + http_buf_size, header_length - http_buf_size);
			if (len_read <= 0) {
				break;
			}
			http_buf_size += len_read;
		}
	} else if (header_close) {
		do {
			ssize_t len_read;
			if (http_buf) {
				http_buf = zend_string_realloc(http_buf, http_buf_size + 4096, 0);
			} else {
				http_buf = zend_string_alloc(4096, 0);
			}
			len_read = php_stream_read(stream, http_buf->val + http_buf_size, 4096);
			if (len_read > 0) {
				http_buf_size += len_read;
			}
		} while(!php_stream_eof(stream));
	} else {
		return NULL;
	}

	http_buf->val[http_buf_size] = '\0';
	http_buf->len = http_buf_size;
	return http_buf;
}

static zend_string *get_http_headers(php_stream *stream)
{
	smart_str tmp_response = {0};
	char headerbuf[8192];

	while (php_stream_gets(stream, headerbuf, sizeof(headerbuf))) {
		if ((headerbuf[0] == '\r' && headerbuf[1] == '\n') ||
		    (headerbuf[0] == '\n')) {
			/* empty line marks end of headers */
			smart_str_0(&tmp_response);
			return tmp_response.s;
		}

		/* add header to collection */
		smart_str_appends(&tmp_response, headerbuf);
	}

	smart_str_free(&tmp_response);
	return NULL;
}
