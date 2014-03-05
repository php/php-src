/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2014 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Brad Lafountain <rodif_bl@yahoo.com>                        |
  |          Shane Caraveo <shane@caraveo.com>                           |
  |          Dmitry Stogov <dmitry@zend.com>                             |
  +----------------------------------------------------------------------+
*/
/* $Id$ */

#include "php_soap.h"
#include "ext/standard/base64.h"
#include "ext/standard/md5.h"
#include "ext/standard/php_rand.h"

static char *get_http_header_value(char *headers, char *type);
static int get_http_body(php_stream *socketd, int close, char *headers,  char **response, int *out_size TSRMLS_DC);
static int get_http_headers(php_stream *socketd,char **response, int *out_size TSRMLS_DC);

#define smart_str_append_const(str, const) \
	smart_str_appendl(str,const,sizeof(const)-1)

/* Proxy HTTP Authentication */
int proxy_authentication(zval* this_ptr, smart_str* soap_headers TSRMLS_DC)
{
	zval **login, **password;

	if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_proxy_login", sizeof("_proxy_login"), (void **)&login) == SUCCESS) {
		unsigned char* buf;
		int len;
		smart_str auth = {0};

		smart_str_appendl(&auth, Z_STRVAL_PP(login), Z_STRLEN_PP(login));
		smart_str_appendc(&auth, ':');
		if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_proxy_password", sizeof("_proxy_password"), (void **)&password) == SUCCESS) {
			smart_str_appendl(&auth, Z_STRVAL_PP(password), Z_STRLEN_PP(password));
		}
		smart_str_0(&auth);
		buf = php_base64_encode((unsigned char*)auth.c, auth.len, &len);
		smart_str_append_const(soap_headers, "Proxy-Authorization: Basic ");
		smart_str_appendl(soap_headers, (char*)buf, len);
		smart_str_append_const(soap_headers, "\r\n");
		efree(buf);
		smart_str_free(&auth);
		return 1;
	}
	return 0;
}

/* HTTP Authentication */
int basic_authentication(zval* this_ptr, smart_str* soap_headers TSRMLS_DC)
{
	zval **login, **password;

	if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_login", sizeof("_login"), (void **)&login) == SUCCESS &&
			!zend_hash_exists(Z_OBJPROP_P(this_ptr), "_digest", sizeof("_digest"))) {
		unsigned char* buf;
		int len;
		smart_str auth = {0};

		smart_str_appendl(&auth, Z_STRVAL_PP(login), Z_STRLEN_PP(login));
		smart_str_appendc(&auth, ':');
		if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_password", sizeof("_password"), (void **)&password) == SUCCESS) {
			smart_str_appendl(&auth, Z_STRVAL_PP(password), Z_STRLEN_PP(password));
		}
		smart_str_0(&auth);
		buf = php_base64_encode((unsigned char*)auth.c, auth.len, &len);
		smart_str_append_const(soap_headers, "Authorization: Basic ");
		smart_str_appendl(soap_headers, (char*)buf, len);
		smart_str_append_const(soap_headers, "\r\n");
		efree(buf);
		smart_str_free(&auth);
		return 1;
	}
	return 0;
}

/* Additional HTTP headers */
void http_context_headers(php_stream_context* context,
                          zend_bool has_authorization,
                          zend_bool has_proxy_authorization,
                          zend_bool has_cookies,
                          smart_str* soap_headers TSRMLS_DC)
{
	zval **tmp;

	if (context &&
		php_stream_context_get_option(context, "http", "header", &tmp) == SUCCESS &&
		Z_TYPE_PP(tmp) == IS_STRING && Z_STRLEN_PP(tmp)) {
		char *s = Z_STRVAL_PP(tmp);
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

static php_stream* http_connect(zval* this_ptr, php_url *phpurl, int use_ssl, php_stream_context *context, int *use_proxy TSRMLS_DC)
{
	php_stream *stream;
	zval **proxy_host, **proxy_port, **tmp;
	char *host;
	char *name;
	char *protocol;
	long namelen;
	int port;
	int old_error_reporting;
	struct timeval tv;
	struct timeval *timeout = NULL;

	if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_proxy_host", sizeof("_proxy_host"), (void **) &proxy_host) == SUCCESS &&
	    Z_TYPE_PP(proxy_host) == IS_STRING &&
	    zend_hash_find(Z_OBJPROP_P(this_ptr), "_proxy_port", sizeof("_proxy_port"), (void **) &proxy_port) == SUCCESS &&
	    Z_TYPE_PP(proxy_port) == IS_LONG) {
		host = Z_STRVAL_PP(proxy_host);
		port = Z_LVAL_PP(proxy_port);
		*use_proxy = 1;
	} else {
		host = phpurl->host;
		port = phpurl->port;
	}
	if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_connection_timeout", sizeof("_connection_timeout"), (void **) &tmp) == SUCCESS &&
	    Z_TYPE_PP(tmp) == IS_LONG && Z_LVAL_PP(tmp) > 0) {
	  tv.tv_sec = Z_LVAL_PP(tmp);
	  tv.tv_usec = 0;
		timeout = &tv;
	}

	old_error_reporting = EG(error_reporting);
	EG(error_reporting) &= ~(E_WARNING|E_NOTICE|E_USER_WARNING|E_USER_NOTICE);

	/* Changed ternary operator to an if/else so that additional comparisons can be done on the ssl_method property */
	if (use_ssl && !*use_proxy) {
		if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_ssl_method", sizeof("_ssl_method"), (void **) &tmp) == SUCCESS &&
			Z_TYPE_PP(tmp) == IS_LONG) {
			/* uses constants declared in soap.c to determine ssl uri protocol */
			switch (Z_LVAL_PP(tmp)) {
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
		char *http_headers;
		int http_header_size;

		smart_str_append_const(&soap_headers, "CONNECT ");
		smart_str_appends(&soap_headers, phpurl->host);
		smart_str_appendc(&soap_headers, ':');
		smart_str_append_unsigned(&soap_headers, phpurl->port);
		smart_str_append_const(&soap_headers, " HTTP/1.1\r\n");
		smart_str_append_const(&soap_headers, "Host: ");
		smart_str_appends(&soap_headers, phpurl->host);
		if (phpurl->port != 80) {
			smart_str_appendc(&soap_headers, ':');
			smart_str_append_unsigned(&soap_headers, phpurl->port);
		}
		smart_str_append_const(&soap_headers, "\r\n");
		proxy_authentication(this_ptr, &soap_headers TSRMLS_CC);
		smart_str_append_const(&soap_headers, "\r\n");
		if (php_stream_write(stream, soap_headers.c, soap_headers.len) != soap_headers.len) {
			php_stream_close(stream);
			stream = NULL;
		}
 	 	smart_str_free(&soap_headers);

 	 	if (stream) {
			if (!get_http_headers(stream, &http_headers, &http_header_size TSRMLS_CC) || http_headers == NULL) {
				php_stream_close(stream);
				stream = NULL;
			}
			if (http_headers) {
				efree(http_headers);
			}
		}
		/* enable SSL transport layer */
		if (stream) {
			/* if a stream is created without encryption, check to see if SSL method parameter is specified and use
 			   proper encrypyion method based on constants defined in soap.c */
			int crypto_method = STREAM_CRYPTO_METHOD_SSLv23_CLIENT;
			if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_ssl_method", sizeof("_ssl_method"), (void **) &tmp) == SUCCESS &&
				Z_TYPE_PP(tmp) == IS_LONG) {
				switch (Z_LVAL_PP(tmp)) {
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
			if (php_stream_xport_crypto_setup(stream, crypto_method, NULL TSRMLS_CC) < 0 ||
			    php_stream_xport_crypto_enable(stream, 1 TSRMLS_CC) < 0) {
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

int make_http_soap_request(zval  *this_ptr,
                           char  *buf,
                           int    buf_size,
                           char  *location,
                           char  *soapaction,
                           int    soap_version,
                           char **buffer,
                           int   *buffer_len TSRMLS_DC)
{
	char *request;
	smart_str soap_headers = {0};
	smart_str soap_headers_z = {0};
	int request_size, err;
	php_url *phpurl = NULL;
	php_stream *stream;
	zval **trace, **tmp;
	int use_proxy = 0;
	int use_ssl;
	char *http_headers, *http_body, *content_type, *http_version, *cookie_itt;
	int http_header_size, http_body_size, http_close;
	char *connection;
	int http_1_1;
	int http_status;
	int content_type_xml = 0;
	long redirect_max = 20;
	char *content_encoding;
	char *http_msg = NULL;
	zend_bool old_allow_url_fopen;
	php_stream_context *context = NULL;
	zend_bool has_authorization = 0;
	zend_bool has_proxy_authorization = 0;
	zend_bool has_cookies = 0;

	if (this_ptr == NULL || Z_TYPE_P(this_ptr) != IS_OBJECT) {
		return FALSE;
	}

  request = buf;
  request_size = buf_size;
	/* Compress request */
	if (zend_hash_find(Z_OBJPROP_P(this_ptr), "compression", sizeof("compression"), (void **)&tmp) == SUCCESS && Z_TYPE_PP(tmp) == IS_LONG) {
		int level = Z_LVAL_PP(tmp) & 0x0f;
		int kind  = Z_LVAL_PP(tmp) & SOAP_COMPRESSION_DEFLATE;

		if (level > 9) {level = 9;}
		
	  if ((Z_LVAL_PP(tmp) & SOAP_COMPRESSION_ACCEPT) != 0) {
			smart_str_append_const(&soap_headers_z,"Accept-Encoding: gzip, deflate\r\n");
	  }
	  if (level > 0) {
			zval func;
			zval retval;
 			zval param1, param2, param3;
			zval *params[3];
			int n;

			params[0] = &param1;
			INIT_PZVAL(params[0]);
			params[1] = &param2;
			INIT_PZVAL(params[1]);
			params[2] = &param3;
			INIT_PZVAL(params[2]);
			ZVAL_STRINGL(params[0], buf, buf_size, 0);
			ZVAL_LONG(params[1], level);
	    if (kind == SOAP_COMPRESSION_DEFLATE) {
	    	n = 2;
				ZVAL_STRING(&func, "gzcompress", 0);
				smart_str_append_const(&soap_headers_z,"Content-Encoding: deflate\r\n");
	    } else {
	      n = 3;
				ZVAL_STRING(&func, "gzencode", 0);
				smart_str_append_const(&soap_headers_z,"Content-Encoding: gzip\r\n");
				ZVAL_LONG(params[2], 0x1f);
	    }
			if (call_user_function(CG(function_table), (zval**)NULL, &func, &retval, n, params TSRMLS_CC) == SUCCESS &&
			    Z_TYPE(retval) == IS_STRING) {
				request = Z_STRVAL(retval);
				request_size = Z_STRLEN(retval);
			} else {
				if (request != buf) {efree(request);}
				smart_str_free(&soap_headers_z);
				return FALSE;
			}
	  }
	}

	if (zend_hash_find(Z_OBJPROP_P(this_ptr), "httpsocket", sizeof("httpsocket"), (void **)&tmp) == SUCCESS) {
		php_stream_from_zval_no_verify(stream,tmp);
		if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_use_proxy", sizeof("_use_proxy"), (void **)&tmp) == SUCCESS && Z_TYPE_PP(tmp) == IS_LONG) {
			use_proxy = Z_LVAL_PP(tmp);
		}
	} else {
		stream = NULL;
	}

	if (location != NULL && location[0] != '\000') {
		phpurl = php_url_parse(location);
	}

	if (SUCCESS == zend_hash_find(Z_OBJPROP_P(this_ptr),
			"_stream_context", sizeof("_stream_context"), (void**)&tmp)) {
		context = php_stream_context_from_zval(*tmp, 0);
	}

	if (context && 
		php_stream_context_get_option(context, "http", "max_redirects", &tmp) == SUCCESS) {
		if (Z_TYPE_PP(tmp) != IS_STRING || !is_numeric_string(Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp), &redirect_max, NULL, 1)) {
			if (Z_TYPE_PP(tmp) == IS_LONG)
				redirect_max = Z_LVAL_PP(tmp);
		}
	}

try_again:
	if (phpurl == NULL || phpurl->host == NULL) {
	  if (phpurl != NULL) {php_url_free(phpurl);}
		if (request != buf) {efree(request);}
		add_soap_fault(this_ptr, "HTTP", "Unable to parse URL", NULL, NULL TSRMLS_CC);
		smart_str_free(&soap_headers_z);
		return FALSE;
	}

	use_ssl = 0;
	if (phpurl->scheme != NULL && strcmp(phpurl->scheme, "https") == 0) {
		use_ssl = 1;
	} else if (phpurl->scheme == NULL || strcmp(phpurl->scheme, "http") != 0) {
		php_url_free(phpurl);
		if (request != buf) {efree(request);}
		add_soap_fault(this_ptr, "HTTP", "Unknown protocol. Only http and https are allowed.", NULL, NULL TSRMLS_CC);
		smart_str_free(&soap_headers_z);
		return FALSE;
	}

	old_allow_url_fopen = PG(allow_url_fopen);
	PG(allow_url_fopen) = 1;
	if (use_ssl && php_stream_locate_url_wrapper("https://", NULL, STREAM_LOCATE_WRAPPERS_ONLY TSRMLS_CC) == NULL) {
		php_url_free(phpurl);
		if (request != buf) {efree(request);}
		add_soap_fault(this_ptr, "HTTP", "SSL support is not available in this build", NULL, NULL TSRMLS_CC);
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
		if (zend_hash_find(Z_OBJPROP_P(this_ptr), "httpurl", sizeof("httpurl"), (void **)&tmp) == SUCCESS &&
		    (orig = (php_url *) zend_fetch_resource(tmp TSRMLS_CC, -1, "httpurl", NULL, 1, le_url)) != NULL &&
		    ((use_proxy && !use_ssl) ||
		     (((use_ssl && orig->scheme != NULL && strcmp(orig->scheme, "https") == 0) ||
		      (!use_ssl && orig->scheme == NULL) ||
		      (!use_ssl && strcmp(orig->scheme, "https") != 0)) &&
		     strcmp(orig->host, phpurl->host) == 0 &&
		     orig->port == phpurl->port))) {
    } else {
			php_stream_close(stream);
			zend_hash_del(Z_OBJPROP_P(this_ptr), "httpurl", sizeof("httpurl"));
			zend_hash_del(Z_OBJPROP_P(this_ptr), "httpsocket", sizeof("httpsocket"));
			zend_hash_del(Z_OBJPROP_P(this_ptr), "_use_proxy", sizeof("_use_proxy"));
			stream = NULL;
			use_proxy = 0;
    }
	}

	/* Check if keep-alive connection is still opened */
	if (stream != NULL && php_stream_eof(stream)) {
		php_stream_close(stream);
		zend_hash_del(Z_OBJPROP_P(this_ptr), "httpurl", sizeof("httpurl"));
		zend_hash_del(Z_OBJPROP_P(this_ptr), "httpsocket", sizeof("httpsocket"));
		zend_hash_del(Z_OBJPROP_P(this_ptr), "_use_proxy", sizeof("_use_proxy"));
		stream = NULL;
		use_proxy = 0;
	}

	if (!stream) {
		stream = http_connect(this_ptr, phpurl, use_ssl, context, &use_proxy TSRMLS_CC);
		if (stream) {
			php_stream_auto_cleanup(stream);
			add_property_resource(this_ptr, "httpsocket", php_stream_get_resource_id(stream));
			add_property_long(this_ptr, "_use_proxy", use_proxy);
		} else {
			php_url_free(phpurl);
			if (request != buf) {efree(request);}
			add_soap_fault(this_ptr, "HTTP", "Could not connect to host", NULL, NULL TSRMLS_CC);
			PG(allow_url_fopen) = old_allow_url_fopen;
			smart_str_free(&soap_headers_z);
			return FALSE;
		}
	}
	PG(allow_url_fopen) = old_allow_url_fopen;

	if (stream) {
		zval **cookies, **login, **password;
	  int ret = zend_list_insert(phpurl, le_url TSRMLS_CC);

		add_property_resource(this_ptr, "httpurl", ret);
		/*zend_list_addref(ret);*/

		if (context && 
		    php_stream_context_get_option(context, "http", "protocol_version", &tmp) == SUCCESS &&
		    Z_TYPE_PP(tmp) == IS_DOUBLE &&
		    Z_DVAL_PP(tmp) == 1.0) {
			http_1_1 = 0;
		} else {
			http_1_1 = 1;
		}

		smart_str_append_const(&soap_headers, "POST ");
		if (use_proxy && !use_ssl) {
			smart_str_appends(&soap_headers, phpurl->scheme);
			smart_str_append_const(&soap_headers, "://");
			smart_str_appends(&soap_headers, phpurl->host);
			smart_str_appendc(&soap_headers, ':');
			smart_str_append_unsigned(&soap_headers, phpurl->port);
		}
		if (phpurl->path) {
			smart_str_appends(&soap_headers, phpurl->path);
		} else {
			smart_str_appendc(&soap_headers, '/');
		}
		if (phpurl->query) {
			smart_str_appendc(&soap_headers, '?');
			smart_str_appends(&soap_headers, phpurl->query);
		}
		if (phpurl->fragment) {
			smart_str_appendc(&soap_headers, '#');
			smart_str_appends(&soap_headers, phpurl->fragment);
		}
		if (http_1_1) {
			smart_str_append_const(&soap_headers, " HTTP/1.1\r\n");
		} else {
			smart_str_append_const(&soap_headers, " HTTP/1.0\r\n");
		}
		smart_str_append_const(&soap_headers, "Host: ");
		smart_str_appends(&soap_headers, phpurl->host);
		if (phpurl->port != (use_ssl?443:80)) {
			smart_str_appendc(&soap_headers, ':');
			smart_str_append_unsigned(&soap_headers, phpurl->port);
		}
		if (!http_1_1 ||
			(zend_hash_find(Z_OBJPROP_P(this_ptr), "_keep_alive", sizeof("_keep_alive"), (void **)&tmp) == SUCCESS &&
			 Z_LVAL_PP(tmp) == 0)) {
			smart_str_append_const(&soap_headers, "\r\n"
				"Connection: close\r\n");
		} else {
			smart_str_append_const(&soap_headers, "\r\n"
				"Connection: Keep-Alive\r\n");
		}
		if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_user_agent", sizeof("_user_agent"), (void **)&tmp) == SUCCESS &&
		    Z_TYPE_PP(tmp) == IS_STRING) {
			if (Z_STRLEN_PP(tmp) > 0) {
				smart_str_append_const(&soap_headers, "User-Agent: ");
				smart_str_appendl(&soap_headers, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
				smart_str_append_const(&soap_headers, "\r\n");
			}
		} else if (context && 
		           php_stream_context_get_option(context, "http", "user_agent", &tmp) == SUCCESS &&
		           Z_TYPE_PP(tmp) == IS_STRING) {
			if (Z_STRLEN_PP(tmp) > 0) {
				smart_str_append_const(&soap_headers, "User-Agent: ");
				smart_str_appendl(&soap_headers, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
				smart_str_append_const(&soap_headers, "\r\n");
			}
		} else if (FG(user_agent)) {
			smart_str_append_const(&soap_headers, "User-Agent: ");
			smart_str_appends(&soap_headers, FG(user_agent));
			smart_str_append_const(&soap_headers, "\r\n");
		} else {
			smart_str_append_const(&soap_headers, "User-Agent: PHP-SOAP/"PHP_VERSION"\r\n");
		}

		smart_str_append(&soap_headers, &soap_headers_z);

		if (soap_version == SOAP_1_2) {
			smart_str_append_const(&soap_headers,"Content-Type: application/soap+xml; charset=utf-8");
			if (soapaction) {
				smart_str_append_const(&soap_headers,"; action=\"");
				smart_str_appends(&soap_headers, soapaction);
				smart_str_append_const(&soap_headers,"\"");
			}
			smart_str_append_const(&soap_headers,"\r\n");
		} else {
			smart_str_append_const(&soap_headers,"Content-Type: text/xml; charset=utf-8\r\n");
			if (soapaction) {
				smart_str_append_const(&soap_headers, "SOAPAction: \"");
				smart_str_appends(&soap_headers, soapaction);
				smart_str_append_const(&soap_headers, "\"\r\n");
			}
		}
		smart_str_append_const(&soap_headers,"Content-Length: ");
		smart_str_append_long(&soap_headers, request_size);
		smart_str_append_const(&soap_headers, "\r\n");

		/* HTTP Authentication */
		if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_login", sizeof("_login"), (void **)&login) == SUCCESS &&
		    Z_TYPE_PP(login) == IS_STRING) {
			zval **digest;

			has_authorization = 1;
			if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_digest", sizeof("_digest"), (void **)&digest) == SUCCESS) {
				if (Z_TYPE_PP(digest) == IS_ARRAY) {
					char          HA1[33], HA2[33], response[33], cnonce[33], nc[9];
					PHP_MD5_CTX   md5ctx;
					unsigned char hash[16];

					PHP_MD5Init(&md5ctx);
					snprintf(cnonce, sizeof(cnonce), "%ld", php_rand(TSRMLS_C));
					PHP_MD5Update(&md5ctx, (unsigned char*)cnonce, strlen(cnonce));
					PHP_MD5Final(hash, &md5ctx);
					make_digest(cnonce, hash);

					if (zend_hash_find(Z_ARRVAL_PP(digest), "nc", sizeof("nc"), (void **)&tmp) == SUCCESS &&
					    Z_TYPE_PP(tmp) == IS_LONG) {
						Z_LVAL_PP(tmp)++;
						snprintf(nc, sizeof(nc), "%08ld", Z_LVAL_PP(tmp));
					} else {
						add_assoc_long(*digest, "nc", 1);
						strcpy(nc, "00000001");
					}

					PHP_MD5Init(&md5ctx);
					PHP_MD5Update(&md5ctx, (unsigned char*)Z_STRVAL_PP(login), Z_STRLEN_PP(login));
					PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
					if (zend_hash_find(Z_ARRVAL_PP(digest), "realm", sizeof("realm"), (void **)&tmp) == SUCCESS &&
					    Z_TYPE_PP(tmp) == IS_STRING) {
						PHP_MD5Update(&md5ctx, (unsigned char*)Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
					}
					PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
					if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_password", sizeof("_password"), (void **)&password) == SUCCESS &&
					    Z_TYPE_PP(password) == IS_STRING) {
						PHP_MD5Update(&md5ctx, (unsigned char*)Z_STRVAL_PP(password), Z_STRLEN_PP(password));
					}
					PHP_MD5Final(hash, &md5ctx);
					make_digest(HA1, hash);
					if (zend_hash_find(Z_ARRVAL_PP(digest), "algorithm", sizeof("algorithm"), (void **)&tmp) == SUCCESS &&
					    Z_TYPE_PP(tmp) == IS_STRING &&
					    Z_STRLEN_PP(tmp) == sizeof("md5-sess")-1 &&
					    stricmp(Z_STRVAL_PP(tmp), "md5-sess") == 0) {
						PHP_MD5Init(&md5ctx);
						PHP_MD5Update(&md5ctx, (unsigned char*)HA1, 32);
						PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
						if (zend_hash_find(Z_ARRVAL_PP(digest), "nonce", sizeof("nonce"), (void **)&tmp) == SUCCESS &&
						    Z_TYPE_PP(tmp) == IS_STRING) {
							PHP_MD5Update(&md5ctx, (unsigned char*)Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
						}
						PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
						PHP_MD5Update(&md5ctx, (unsigned char*)cnonce, 8);
						PHP_MD5Final(hash, &md5ctx);
						make_digest(HA1, hash);
					}

					PHP_MD5Init(&md5ctx);
					PHP_MD5Update(&md5ctx, (unsigned char*)"POST:", sizeof("POST:")-1);
					if (phpurl->path) {
						PHP_MD5Update(&md5ctx, (unsigned char*)phpurl->path, strlen(phpurl->path));
					} else {
						PHP_MD5Update(&md5ctx, (unsigned char*)"/", 1);
					}
					if (phpurl->query) {
						PHP_MD5Update(&md5ctx, (unsigned char*)"?", 1);
						PHP_MD5Update(&md5ctx, (unsigned char*)phpurl->query, strlen(phpurl->query));
					}

					/* TODO: Support for qop="auth-int" */
/*
					if (zend_hash_find(Z_ARRVAL_PP(digest), "qop", sizeof("qop"), (void **)&tmp) == SUCCESS &&
					    Z_TYPE_PP(tmp) == IS_STRING &&
					    Z_STRLEN_PP(tmp) == sizeof("auth-int")-1 &&
					    stricmp(Z_STRVAL_PP(tmp), "auth-int") == 0) {
						PHP_MD5Update(&md5ctx, ":", 1);
						PHP_MD5Update(&md5ctx, HEntity, HASHHEXLEN);
					}
*/
					PHP_MD5Final(hash, &md5ctx);
					make_digest(HA2, hash);

					PHP_MD5Init(&md5ctx);
					PHP_MD5Update(&md5ctx, (unsigned char*)HA1, 32);
					PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
					if (zend_hash_find(Z_ARRVAL_PP(digest), "nonce", sizeof("nonce"), (void **)&tmp) == SUCCESS &&
					    Z_TYPE_PP(tmp) == IS_STRING) {
						PHP_MD5Update(&md5ctx, (unsigned char*)Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
					}
					PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
					if (zend_hash_find(Z_ARRVAL_PP(digest), "qop", sizeof("qop"), (void **)&tmp) == SUCCESS &&
					    Z_TYPE_PP(tmp) == IS_STRING) {
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
					smart_str_appendl(&soap_headers, Z_STRVAL_PP(login), Z_STRLEN_PP(login));
					if (zend_hash_find(Z_ARRVAL_PP(digest), "realm", sizeof("realm"), (void **)&tmp) == SUCCESS &&
					    Z_TYPE_PP(tmp) == IS_STRING) {
						smart_str_append_const(&soap_headers, "\", realm=\"");
						smart_str_appendl(&soap_headers, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
					}
					if (zend_hash_find(Z_ARRVAL_PP(digest), "nonce", sizeof("nonce"), (void **)&tmp) == SUCCESS &&
					    Z_TYPE_PP(tmp) == IS_STRING) {
						smart_str_append_const(&soap_headers, "\", nonce=\"");
						smart_str_appendl(&soap_headers, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
					}
					smart_str_append_const(&soap_headers, "\", uri=\"");
					if (phpurl->path) {
						smart_str_appends(&soap_headers, phpurl->path);
					} else {
						smart_str_appendc(&soap_headers, '/');
					} 
					if (phpurl->query) {
						smart_str_appendc(&soap_headers, '?');
						smart_str_appends(&soap_headers, phpurl->query);
					}
					if (phpurl->fragment) {
						smart_str_appendc(&soap_headers, '#');
						smart_str_appends(&soap_headers, phpurl->fragment);
					}
					if (zend_hash_find(Z_ARRVAL_PP(digest), "qop", sizeof("qop"), (void **)&tmp) == SUCCESS &&
					    Z_TYPE_PP(tmp) == IS_STRING) {
					/* TODO: Support for qop="auth-int" */
						smart_str_append_const(&soap_headers, "\", qop=\"auth");
						smart_str_append_const(&soap_headers, "\", nc=\"");
						smart_str_appendl(&soap_headers, nc, 8);
						smart_str_append_const(&soap_headers, "\", cnonce=\"");
						smart_str_appendl(&soap_headers, cnonce, 8);
					}
					smart_str_append_const(&soap_headers, "\", response=\"");
					smart_str_appendl(&soap_headers, response, 32);
					if (zend_hash_find(Z_ARRVAL_PP(digest), "opaque", sizeof("opaque"), (void **)&tmp) == SUCCESS &&
					    Z_TYPE_PP(tmp) == IS_STRING) {
						smart_str_append_const(&soap_headers, "\", opaque=\"");
						smart_str_appendl(&soap_headers, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
					}
					if (zend_hash_find(Z_ARRVAL_PP(digest), "algorithm", sizeof("algorithm"), (void **)&tmp) == SUCCESS &&
						Z_TYPE_PP(tmp) == IS_STRING) {
						smart_str_append_const(&soap_headers, "\", algorithm=\"");
						smart_str_appendl(&soap_headers, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
					}
					smart_str_append_const(&soap_headers, "\"\r\n");
				}
			} else {
				unsigned char* buf;
				int len;

				smart_str auth = {0};
				smart_str_appendl(&auth, Z_STRVAL_PP(login), Z_STRLEN_PP(login));
				smart_str_appendc(&auth, ':');
				if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_password", sizeof("_password"), (void **)&password) == SUCCESS &&
				    Z_TYPE_PP(password) == IS_STRING) {
					smart_str_appendl(&auth, Z_STRVAL_PP(password), Z_STRLEN_PP(password));
				}
				smart_str_0(&auth);
				buf = php_base64_encode((unsigned char*)auth.c, auth.len, &len);
				smart_str_append_const(&soap_headers, "Authorization: Basic ");
				smart_str_appendl(&soap_headers, (char*)buf, len);
				smart_str_append_const(&soap_headers, "\r\n");
				efree(buf);
				smart_str_free(&auth);
			}
		}

		/* Proxy HTTP Authentication */
		if (use_proxy && !use_ssl) {
			has_proxy_authorization = proxy_authentication(this_ptr, &soap_headers TSRMLS_CC);
		}

		/* Send cookies along with request */
		if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_cookies", sizeof("_cookies"), (void **)&cookies) == SUCCESS) {
			zval **data;
			char *key;
			int i, n;

			has_cookies = 1;
			n = zend_hash_num_elements(Z_ARRVAL_PP(cookies));
			if (n > 0) {
				zend_hash_internal_pointer_reset(Z_ARRVAL_PP(cookies));
				smart_str_append_const(&soap_headers, "Cookie: ");
				for (i = 0; i < n; i++) {
					zend_hash_get_current_data(Z_ARRVAL_PP(cookies), (void **)&data);
					zend_hash_get_current_key(Z_ARRVAL_PP(cookies), &key, NULL, FALSE);

					if (Z_TYPE_PP(data) == IS_ARRAY) {
					  zval** value;

						if (zend_hash_index_find(Z_ARRVAL_PP(data), 0, (void**)&value) == SUCCESS &&
						    Z_TYPE_PP(value) == IS_STRING) {
						  zval **tmp;
						  if ((zend_hash_index_find(Z_ARRVAL_PP(data), 1, (void**)&tmp) == FAILURE ||
						       strncmp(phpurl->path?phpurl->path:"/",Z_STRVAL_PP(tmp),Z_STRLEN_PP(tmp)) == 0) &&
						      (zend_hash_index_find(Z_ARRVAL_PP(data), 2, (void**)&tmp) == FAILURE ||
						       in_domain(phpurl->host,Z_STRVAL_PP(tmp))) &&
						      (use_ssl || zend_hash_index_find(Z_ARRVAL_PP(data), 3, (void**)&tmp) == FAILURE)) {
								smart_str_appendl(&soap_headers, key, strlen(key));
								smart_str_appendc(&soap_headers, '=');
								smart_str_appendl(&soap_headers, Z_STRVAL_PP(value), Z_STRLEN_PP(value));
								smart_str_appendc(&soap_headers, ';');
							}
						}
					}
					zend_hash_move_forward(Z_ARRVAL_PP(cookies));
				}
				smart_str_append_const(&soap_headers, "\r\n");
			}
		}

		http_context_headers(context, has_authorization, has_proxy_authorization, has_cookies, &soap_headers TSRMLS_CC);

		smart_str_append_const(&soap_headers, "\r\n");
		smart_str_0(&soap_headers);
		if (zend_hash_find(Z_OBJPROP_P(this_ptr), "trace", sizeof("trace"), (void **) &trace) == SUCCESS &&
		    Z_LVAL_PP(trace) > 0) {
			add_property_stringl(this_ptr, "__last_request_headers", soap_headers.c, soap_headers.len, 1);
		}
		smart_str_appendl(&soap_headers, request, request_size);
		smart_str_0(&soap_headers);

		err = php_stream_write(stream, soap_headers.c, soap_headers.len);
		if (err != soap_headers.len) {
			if (request != buf) {efree(request);}
			php_stream_close(stream);
			zend_hash_del(Z_OBJPROP_P(this_ptr), "httpurl", sizeof("httpurl"));
			zend_hash_del(Z_OBJPROP_P(this_ptr), "httpsocket", sizeof("httpsocket"));
			zend_hash_del(Z_OBJPROP_P(this_ptr), "_use_proxy", sizeof("_use_proxy"));
			add_soap_fault(this_ptr, "HTTP", "Failed Sending HTTP SOAP request", NULL, NULL TSRMLS_CC);
			smart_str_free(&soap_headers_z);
			return FALSE;
		}
		smart_str_free(&soap_headers);
	} else {
		add_soap_fault(this_ptr, "HTTP", "Failed to create stream??", NULL, NULL TSRMLS_CC);
		smart_str_free(&soap_headers_z);
		return FALSE;
	}

	if (!buffer) {
		php_stream_close(stream);
		zend_hash_del(Z_OBJPROP_P(this_ptr), "httpsocket", sizeof("httpsocket"));
		zend_hash_del(Z_OBJPROP_P(this_ptr), "_use_proxy", sizeof("_use_proxy"));
		smart_str_free(&soap_headers_z);
		return TRUE;
	}

	do {
		if (!get_http_headers(stream, &http_headers, &http_header_size TSRMLS_CC)) {
			if (http_headers) {efree(http_headers);}
			if (request != buf) {efree(request);}
			php_stream_close(stream);
			zend_hash_del(Z_OBJPROP_P(this_ptr), "httpsocket", sizeof("httpsocket"));
			zend_hash_del(Z_OBJPROP_P(this_ptr), "_use_proxy", sizeof("_use_proxy"));
			add_soap_fault(this_ptr, "HTTP", "Error Fetching http headers", NULL, NULL TSRMLS_CC);
			smart_str_free(&soap_headers_z);
			return FALSE;
		}

		if (zend_hash_find(Z_OBJPROP_P(this_ptr), "trace", sizeof("trace"), (void **) &trace) == SUCCESS &&
		    Z_LVAL_PP(trace) > 0) {
			add_property_stringl(this_ptr, "__last_response_headers", http_headers, http_header_size, 1);
		}

		/* Check to see what HTTP status was sent */
		http_1_1 = 0;
		http_status = 0;
		http_version = get_http_header_value(http_headers,"HTTP/");
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
				efree(http_headers);
			}
		}
	} while (http_status == 100);

	/* Grab and send back every cookie */

	/* Not going to worry about Path: because
	   we shouldn't be changing urls so path dont
	   matter too much
	*/
	cookie_itt = strstr(http_headers,"Set-Cookie: ");
	while (cookie_itt) {
		char *end_pos, *cookie;
		char *eqpos, *sempos;
		zval **cookies;

		if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_cookies", sizeof("_cookies"), (void **)&cookies) == FAILURE) {
			zval *tmp_cookies;
			MAKE_STD_ZVAL(tmp_cookies);
			array_init(tmp_cookies);
			zend_hash_update(Z_OBJPROP_P(this_ptr), "_cookies", sizeof("_cookies"), &tmp_cookies, sizeof(zval *), (void **)&cookies);
		}

		end_pos = strstr(cookie_itt,"\r\n");
		cookie = get_http_header_value(cookie_itt,"Set-Cookie: ");

		eqpos = strstr(cookie, "=");
		sempos = strstr(cookie, ";");
		if (eqpos != NULL && (sempos == NULL || sempos > eqpos)) {
			smart_str name = {0};
			int cookie_len;
			zval *zcookie;

			if (sempos != NULL) {
				cookie_len = sempos-(eqpos+1);
			} else {
				cookie_len = strlen(cookie)-(eqpos-cookie)-1;
			}

			smart_str_appendl(&name, cookie, eqpos - cookie);
			smart_str_0(&name);

			ALLOC_INIT_ZVAL(zcookie);
			array_init(zcookie);
			add_index_stringl(zcookie, 0, eqpos + 1, cookie_len, 1);

			if (sempos != NULL) {
				char *options = cookie + cookie_len+1;
				while (*options) {
					while (*options == ' ') {options++;}
					sempos = strstr(options, ";");
					if (strstr(options,"path=") == options) {
						eqpos = options + sizeof("path=")-1;
						add_index_stringl(zcookie, 1, eqpos, sempos?(sempos-eqpos):strlen(eqpos), 1);
					} else if (strstr(options,"domain=") == options) {
						eqpos = options + sizeof("domain=")-1;
						add_index_stringl(zcookie, 2, eqpos, sempos?(sempos-eqpos):strlen(eqpos), 1);
					} else if (strstr(options,"secure") == options) {
						add_index_bool(zcookie, 3, 1);
					}
					if (sempos != NULL) {
						options = sempos+1;
					} else {
					  break;
					}
				}
			}
			if (!zend_hash_index_exists(Z_ARRVAL_P(zcookie), 1)) {
				char *t = phpurl->path?phpurl->path:"/";
				char *c = strrchr(t, '/');
				if (c) {
					add_index_stringl(zcookie, 1, t, c-t, 1);
				}
			}
			if (!zend_hash_index_exists(Z_ARRVAL_P(zcookie), 2)) {
				add_index_string(zcookie, 2, phpurl->host, 1);
			}

			add_assoc_zval_ex(*cookies, name.c, name.len+1, zcookie);
			smart_str_free(&name);
		}

		cookie_itt = strstr(cookie_itt + sizeof("Set-Cookie: "), "Set-Cookie: ");
		efree(cookie);
	}

	/* See if the server requested a close */
	if (http_1_1) {
		http_close = FALSE;
		if (use_proxy && !use_ssl) {
			connection = get_http_header_value(http_headers,"Proxy-Connection: ");
			if (connection) {
				if (strncasecmp(connection, "close", sizeof("close")-1) == 0) {
					http_close = TRUE;
				}
				efree(connection);
			}
		}
		if (http_close == FALSE) {
			connection = get_http_header_value(http_headers,"Connection: ");
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
			connection = get_http_header_value(http_headers,"Proxy-Connection: ");
			if (connection) {
				if (strncasecmp(connection, "Keep-Alive", sizeof("Keep-Alive")-1) == 0) {
					http_close = FALSE;
				}
				efree(connection);
			}
		}
		if (http_close == TRUE) {
			connection = get_http_header_value(http_headers,"Connection: ");
			if (connection) {
				if (strncasecmp(connection, "Keep-Alive", sizeof("Keep-Alive")-1) == 0) {
					http_close = FALSE;
				}
				efree(connection);
			}
		}
	}	

	if (!get_http_body(stream, http_close, http_headers, &http_body, &http_body_size TSRMLS_CC)) {
		if (request != buf) {efree(request);}
		php_stream_close(stream);
		efree(http_headers);
		zend_hash_del(Z_OBJPROP_P(this_ptr), "httpsocket", sizeof("httpsocket"));
		zend_hash_del(Z_OBJPROP_P(this_ptr), "_use_proxy", sizeof("_use_proxy"));
		add_soap_fault(this_ptr, "HTTP", "Error Fetching http body, No Content-Length, connection closed or chunked data", NULL, NULL TSRMLS_CC);
		if (http_msg) {
			efree(http_msg);
		}
		smart_str_free(&soap_headers_z);
		return FALSE;
	}

	if (request != buf) {efree(request);}

	if (http_close) {
		php_stream_close(stream);
		zend_hash_del(Z_OBJPROP_P(this_ptr), "httpsocket", sizeof("httpsocket"));
		zend_hash_del(Z_OBJPROP_P(this_ptr), "_use_proxy", sizeof("_use_proxy"));
		stream = NULL;
	}

	/* Process HTTP status codes */
	if (http_status >= 300 && http_status < 400) {
		char *loc;

		if ((loc = get_http_header_value(http_headers,"Location: ")) != NULL) {
			php_url *new_url  = php_url_parse(loc);

			if (new_url != NULL) {
				efree(http_headers);
				efree(http_body);
				efree(loc);
				if (new_url->scheme == NULL && new_url->path != NULL) {
					new_url->scheme = phpurl->scheme ? estrdup(phpurl->scheme) : NULL;
					new_url->host = phpurl->host ? estrdup(phpurl->host) : NULL;
					new_url->port = phpurl->port;
					if (new_url->path && new_url->path[0] != '/') {
						if (phpurl->path) {
							char *t = phpurl->path;
							char *p = strrchr(t, '/');
							if (p) {
								char *s = emalloc((p - t) + strlen(new_url->path) + 2);
								strncpy(s, t, (p - t) + 1);
								s[(p - t) + 1] = 0;
								strcat(s, new_url->path);
								efree(new_url->path);
								new_url->path = s;
							} 
						} else {
							char *s = emalloc(strlen(new_url->path) + 2);
							s[0] = '/'; s[1] = 0;
							strcat(s, new_url->path);
							efree(new_url->path);
							new_url->path = s;
						}
					}
				}
				phpurl = new_url;

				if (--redirect_max < 1) {
					add_soap_fault(this_ptr, "HTTP", "Redirection limit reached, aborting", NULL, NULL TSRMLS_CC);
					smart_str_free(&soap_headers_z);
					return FALSE;
				}

				goto try_again;
			}
		}
	} else if (http_status == 401) {
		/* Digest authentication */
		zval **digest, **login, **password;
		char *auth = get_http_header_value(http_headers, "WWW-Authenticate: ");

		if (auth &&
				strstr(auth, "Digest") == auth &&
		    (zend_hash_find(Z_OBJPROP_P(this_ptr), "_digest", sizeof("_digest"), (void **)&digest) == FAILURE ||
		     Z_TYPE_PP(digest) != IS_ARRAY) &&
		    zend_hash_find(Z_OBJPROP_P(this_ptr), "_login", sizeof("_login"), (void **)&login) == SUCCESS &&
		    Z_TYPE_PP(login) == IS_STRING &&
		    zend_hash_find(Z_OBJPROP_P(this_ptr), "_password", sizeof("_password"), (void **)&password) == SUCCESS &&
		    Z_TYPE_PP(password) == IS_STRING) {
			char *s;
			zval *digest = NULL;

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
					if (digest == NULL) {
						ALLOC_INIT_ZVAL(digest);
						array_init(digest);
					}
					add_assoc_string(digest, name, val ,1);
				}
			}

			if (digest != NULL) {
				php_url *new_url  = emalloc(sizeof(php_url));

				Z_DELREF_P(digest);
				add_property_zval_ex(this_ptr, "_digest", sizeof("_digest"), digest TSRMLS_CC);

				*new_url = *phpurl;
				if (phpurl->scheme) phpurl->scheme = estrdup(phpurl->scheme);
				if (phpurl->user) phpurl->user = estrdup(phpurl->user);
				if (phpurl->pass) phpurl->pass = estrdup(phpurl->pass);
				if (phpurl->host) phpurl->host = estrdup(phpurl->host);
				if (phpurl->path) phpurl->path = estrdup(phpurl->path);
				if (phpurl->query) phpurl->query = estrdup(phpurl->query);
				if (phpurl->fragment) phpurl->fragment = estrdup(phpurl->fragment);
				phpurl = new_url;

				efree(auth);
				efree(http_headers);
				efree(http_body);

				goto try_again;
			}
		}
		if (auth) efree(auth);
	}
	smart_str_free(&soap_headers_z);

	/* Check and see if the server even sent a xml document */
	content_type = get_http_header_value(http_headers,"Content-Type: ");
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
				add_soap_fault(this_ptr, "HTTP", "Didn't receive an xml document", NULL, err TSRMLS_CC);
				efree(content_type);
				efree(http_headers);
				efree(http_body);
				return FALSE;
			}
*/
		}
		efree(content_type);
	}

	/* Decompress response */
	content_encoding = get_http_header_value(http_headers,"Content-Encoding: ");
	if (content_encoding) {
		zval func;
		zval retval;
	  zval param;
		zval *params[1];

		if ((strcmp(content_encoding,"gzip") == 0 ||
		     strcmp(content_encoding,"x-gzip") == 0) &&
		     zend_hash_exists(EG(function_table), "gzinflate", sizeof("gzinflate"))) {
			ZVAL_STRING(&func, "gzinflate", 0);
			params[0] = &param;
			ZVAL_STRINGL(params[0], http_body+10, http_body_size-10, 0);
			INIT_PZVAL(params[0]);
		} else if (strcmp(content_encoding,"deflate") == 0 &&
		           zend_hash_exists(EG(function_table), "gzuncompress", sizeof("gzuncompress"))) {
			ZVAL_STRING(&func, "gzuncompress", 0);
			params[0] = &param;
			ZVAL_STRINGL(params[0], http_body, http_body_size, 0);
			INIT_PZVAL(params[0]);
		} else {
			efree(content_encoding);
			efree(http_headers);
			efree(http_body);
			if (http_msg) {
				efree(http_msg);
			}
			add_soap_fault(this_ptr, "HTTP", "Unknown Content-Encoding", NULL, NULL TSRMLS_CC);
			return FALSE;
		}
		if (call_user_function(CG(function_table), (zval**)NULL, &func, &retval, 1, params TSRMLS_CC) == SUCCESS &&
		    Z_TYPE(retval) == IS_STRING) {
			efree(http_body);
			*buffer = Z_STRVAL(retval);
			*buffer_len = Z_STRLEN(retval);
		} else {
			efree(content_encoding);
			efree(http_headers);
			efree(http_body);
			add_soap_fault(this_ptr, "HTTP", "Can't uncompress compressed response", NULL, NULL TSRMLS_CC);
			if (http_msg) {
				efree(http_msg);
			}
			return FALSE;
		}
		efree(content_encoding);
	} else {
		*buffer = http_body;
		*buffer_len = http_body_size;
	}

	efree(http_headers);

	if (http_status >= 400) {
		int error = 0;

		if (*buffer_len == 0) {
			error = 1;
		} else if (*buffer_len > 0) {
			if (!content_type_xml) {
				char *s = *buffer;

				while (*s != '\0' && *s < ' ') {
					s++;
				}
				if (strncmp(s, "<?xml", 5)) {
					error = 1;
				}
			}
		}

		if (error) {
			efree(*buffer);
			add_soap_fault(this_ptr, "HTTP", http_msg, NULL, NULL TSRMLS_CC);
			efree(http_msg);
			return FALSE;
		}
	}

	if (http_msg) {
		efree(http_msg);
	}

	return TRUE;
}

static char *get_http_header_value(char *headers, char *type)
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
			eol = strchr(tmp, '\n');
			if (eol == NULL) {
				eol = headers + headerslen;
			} else if (eol > tmp && *(eol-1) == '\r') {
				eol--;
			}
			return estrndup(tmp, eol - tmp);
		}

		/* find next line */
		pos = strchr(pos, '\n');
		if (pos) {
			pos++;
		}

	} while (pos);

	return NULL;
}

static int get_http_body(php_stream *stream, int close, char *headers,  char **response, int *out_size TSRMLS_DC)
{
	char *header, *http_buf = NULL;
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
			http_buf = emalloc(1);
			http_buf[0] = '\0';
			(*response) = http_buf;
			(*out_size) = 0;
			return TRUE;
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
					int len_size = 0;

					if (http_buf_size + buf_size + 1 < 0) {
						efree(http_buf);
						return FALSE;
					}
					http_buf = erealloc(http_buf, http_buf_size + buf_size + 1);

					while (len_size < buf_size) {
						int len_read = php_stream_read(stream, http_buf + http_buf_size, buf_size - len_size);
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
						/* Somthing wrong in chunked encoding */
						if (http_buf) {
							efree(http_buf);
						}
						return FALSE;
					}
 				}
			} else {
				/* Somthing wrong in chunked encoding */
				if (http_buf) {
					efree(http_buf);
				}
				return FALSE;
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
			http_buf = emalloc(1);
		}

	} else if (header_length) {
		if (header_length < 0 || header_length >= INT_MAX) {
			return FALSE;
		}
		http_buf = safe_emalloc(1, header_length, 1);
		while (http_buf_size < header_length) {
			int len_read = php_stream_read(stream, http_buf + http_buf_size, header_length - http_buf_size);
			if (len_read <= 0) {
				break;
			}
			http_buf_size += len_read;
		}
	} else if (header_close) {
		do {
			int len_read;
			http_buf = erealloc(http_buf, http_buf_size + 4096 + 1);
			len_read = php_stream_read(stream, http_buf + http_buf_size, 4096);
			if (len_read > 0) {
				http_buf_size += len_read;
			}
		} while(!php_stream_eof(stream));
	} else {
		return FALSE;
	}

	http_buf[http_buf_size] = '\0';
	(*response) = http_buf;
	(*out_size) = http_buf_size;
	return TRUE;
}

static int get_http_headers(php_stream *stream, char **response, int *out_size TSRMLS_DC)
{
	int done = FALSE;
	smart_str tmp_response = {0};
	char headerbuf[8192];

	while (!done) {
		if (!php_stream_gets(stream, headerbuf, sizeof(headerbuf))) {
			break;
		}

		if ((headerbuf[0] == '\r' && headerbuf[1] == '\n') ||
		    (headerbuf[0] == '\n')) {
			/* empty line marks end of headers */
			done = TRUE;
			break;
		}

		/* add header to collection */
		smart_str_appends(&tmp_response, headerbuf);
	}
	smart_str_0(&tmp_response);
	(*response) = tmp_response.c;
	(*out_size) = tmp_response.len;
	return done;
}
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
