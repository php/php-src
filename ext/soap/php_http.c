#include "php_soap.h"
#include "ext/standard/base64.h"

static char *get_http_header_value(char *headers, char *type);
static int get_http_body(php_stream *socketd, char *headers,  char **response, int *out_size TSRMLS_DC);
static int get_http_headers(php_stream *socketd,char **response, int *out_size TSRMLS_DC);

#define smart_str_append_const(str, const) \
	smart_str_appendl(str,const,sizeof(const)-1)

int send_http_soap_request(zval *this_ptr, xmlDoc *doc, char *soapaction TSRMLS_DC)
{
	xmlChar *buf;
	smart_str soap_headers = {0};
	int buf_size,err,ret;
	sdlPtr sdl;
	php_url *phpurl = NULL;
	php_stream *stream;
	zval **trace;

	FETCH_THIS_SOCKET(stream);
	FETCH_THIS_URL(phpurl);
	FETCH_THIS_SDL(sdl);

	xmlDocDumpMemory(doc, &buf, &buf_size);
	if (!buf) {
		add_soap_fault(this_ptr, "SOAP-ENV:Client", "Error build soap request", NULL, NULL TSRMLS_CC);
		return FALSE;
	}
	if (zend_hash_find(Z_OBJPROP_P(this_ptr), "trace", sizeof("trace"), (void **) &trace) == SUCCESS &&
	    Z_LVAL_PP(trace) > 0) {
		add_property_stringl(this_ptr, "__last_request", buf, buf_size, 1);
	}

	/* Check if keep-alive connection is still opened */
	if (stream != NULL) {
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 1;
		php_stream_set_option(stream, PHP_STREAM_OPTION_READ_TIMEOUT, 0, &tv);
		if (php_stream_set_option(stream, PHP_STREAM_OPTION_CHECK_LIVENESS, 0, NULL) != PHP_STREAM_OPTION_RETURN_OK) {
		    php_stream_close(stream);
			zend_hash_del(Z_OBJPROP_P(this_ptr), "httpsocket", sizeof("httpsocket"));
			stream = NULL;
		} else {
			tv.tv_sec = FG(default_socket_timeout);;
			tv.tv_usec = 0;
			php_stream_set_option(stream, PHP_STREAM_OPTION_READ_TIMEOUT, 0, &tv);
		}
	}

	if (!stream) {
		char *url;
		int use_ssl;

		if (!sdl) {
			zval **location;
			if (zend_hash_find(Z_OBJPROP_P(this_ptr), "location", sizeof("location"),(void **) &location) == FAILURE) {
				xmlFree(buf);
				add_soap_fault(this_ptr, "SOAP-ENV:Client", "Error could not find location", NULL, NULL TSRMLS_CC);
				return FALSE;
			}
			url = Z_STRVAL_PP(location);
		} else {
			sdlBindingPtr binding;
			FETCH_THIS_PORT(binding);
			url = binding->location;
		}

		if (url[0] != '\000') {
			phpurl = php_url_parse(url);
		}
		if (phpurl == NULL) {
			xmlFree(buf);
			add_soap_fault(this_ptr, "SOAP-ENV:Client", "Unable to parse URL", NULL, NULL TSRMLS_CC);
			return FALSE;
		}

		use_ssl = strcmp(phpurl->scheme, "https") == 0;
		if (use_ssl && php_stream_locate_url_wrapper("https://", NULL, STREAM_LOCATE_WRAPPERS_ONLY TSRMLS_CC) == NULL) {
			xmlFree(buf);
			php_url_free(phpurl);
 			add_soap_fault(this_ptr, "SOAP-ENV:Client", "SSL support not available in this build", NULL, NULL TSRMLS_CC);
 			return FALSE;
		}

		if (phpurl->port == 0) {
			phpurl->port = use_ssl ? 443 : 80;
		}

#ifdef ZEND_ENGINE_2
		{
			char *res;
			long reslen;

			reslen = spprintf(&res, 0, "%s://%s:%d", use_ssl ? "ssl" : "tcp", phpurl->host, phpurl->port);

			stream = php_stream_xport_create(res, reslen,
				ENFORCE_SAFE_MODE | REPORT_ERRORS,
				STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT,
				NULL /*persistent_id*/,
				NULL /*timeout*/,
				NULL, NULL, NULL);

			efree(res);
		}
#else
		stream = php_stream_sock_open_host(phpurl->host, (unsigned short)phpurl->port, SOCK_STREAM, NULL, NULL);
 		if (use_ssl) {
 			if (FAILURE == php_stream_sock_ssl_activate(stream, 1)) {
				xmlFree(buf);
 				php_url_free(phpurl);
 	 			add_soap_fault(this_ptr, "SOAP-ENV:Client", "SSL Connection attempt failed", NULL, NULL TSRMLS_CC);
 	 			return FALSE;
 			}
 		}
#endif

		if (stream) {
			php_stream_auto_cleanup(stream);
			add_property_resource(this_ptr, "httpsocket", php_stream_get_resource_id(stream));
			ret = zend_list_insert(phpurl, le_url);
			add_property_resource(this_ptr, "httpurl", ret);
			zend_list_addref(ret);
		} else {
			xmlFree(buf);
			php_url_free(phpurl);
			add_soap_fault(this_ptr, "SOAP-ENV:Client", "Could not connect to host", NULL, NULL TSRMLS_CC);
			return FALSE;
		}
	}

	if (stream) {
		zval **cookies, **login, **password;

		smart_str_append_const(&soap_headers, "POST ");
		smart_str_appends(&soap_headers, phpurl->path);
		smart_str_append_const(&soap_headers, " HTTP/1.1\r\n"
			"Host: ");
		smart_str_appends(&soap_headers, phpurl->host);
		smart_str_append_const(&soap_headers, "\r\n"
			"Connection: Keep-Alive\r\n"
/*
			"Connection: close\r\n"
			"Accept: text/html; text/xml; text/plain\r\n"
			"User-Agent: PHP SOAP 0.1\r\n"
*/
			"Content-Type: text/xml; charset=\"utf-8\"\r\n"
			"Content-Length: ");
		smart_str_append_long(&soap_headers, buf_size);
		smart_str_append_const(&soap_headers, "\r\n"
			"SOAPAction: \"");
		/* TODO: need to grab soap action from wsdl....*/
		smart_str_appends(&soap_headers, soapaction);
		smart_str_append_const(&soap_headers, "\"\r\n");

		/* HTTP Authentication */
		if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_login", sizeof("_login"), (void **)&login) == SUCCESS) {
			char* buf;
			int len;

			smart_str auth = {0};
			smart_str_appendl(&auth, Z_STRVAL_PP(login), Z_STRLEN_PP(login));
			smart_str_appendc(&auth, ':');
			if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_password", sizeof("_password"), (void **)&password) == SUCCESS) {
				smart_str_appendl(&auth, Z_STRVAL_PP(password), Z_STRLEN_PP(password));
			}
			smart_str_0(&auth);
			buf = php_base64_encode(auth.c, auth.len, &len);
			smart_str_append_const(&soap_headers, "Authorization: Basic ");
			smart_str_appendl(&soap_headers, buf, len);
			smart_str_append_const(&soap_headers, "\r\n");
			efree(buf);
			smart_str_free(&auth);
		}

		/* Send cookies along with request */
		if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_cookies", sizeof("_cookies"), (void **)&cookies) == SUCCESS) {
			zval **data;
			char *key;
			int i, n;

			n = zend_hash_num_elements(Z_ARRVAL_PP(cookies));
			if (n > 0) {
				zend_hash_internal_pointer_reset(Z_ARRVAL_PP(cookies));
				smart_str_append_const(&soap_headers, "Cookie: ");
				for (i = 0; i < n; i++) {
					zend_hash_get_current_data(Z_ARRVAL_PP(cookies), (void **)&data);
					zend_hash_get_current_key(Z_ARRVAL_PP(cookies), &key, NULL, FALSE);

					smart_str_appendl(&soap_headers, key, strlen(key));
					smart_str_appendc(&soap_headers, '=');
					smart_str_appendl(&soap_headers, Z_STRVAL_PP(data), Z_STRLEN_PP(data));
					smart_str_append_const(&soap_headers, ";");
					zend_hash_move_forward(Z_ARRVAL_PP(cookies));
				}
				smart_str_append_const(&soap_headers, "\r\n");
			}
		}
		smart_str_append_const(&soap_headers, "\r\n");
		smart_str_appendl(&soap_headers, buf, buf_size);
		smart_str_0(&soap_headers);

		err = php_stream_write(stream, soap_headers.c, soap_headers.len);
		if (err != soap_headers.len) {
			xmlFree(buf);
			smart_str_free(&soap_headers);
			php_stream_close(stream);
			zend_hash_del(Z_OBJPROP_P(this_ptr), "httpsocket", sizeof("httpsocket"));
			add_soap_fault(this_ptr, "SOAP-ENV:Client", "Failed Sending HTTP SOAP request", NULL, NULL TSRMLS_CC);
			return FALSE;
		}
		smart_str_free(&soap_headers);

	}
	xmlFree(buf);
	return TRUE;
}

int get_http_soap_response(zval *this_ptr, char **buffer, int *buffer_len TSRMLS_DC)
{
	char *http_headers, *http_body, *content_type, *http_version, http_status[4], *cookie_itt;
	int http_header_size, http_body_size, http_close;
	zval **socket_ref;
	php_stream *stream;
	zval **trace;
	char* connection;
	int http_1_1 = 0;

	if (FIND_SOCKET_PROPERTY(this_ptr, socket_ref) != FAILURE) {
		FETCH_SOCKET_RES(stream, socket_ref);
	}

	if (!get_http_headers(stream, &http_headers, &http_header_size TSRMLS_CC)) {
		php_stream_close(stream);
		zend_hash_del(Z_OBJPROP_P(this_ptr), "httpsocket", sizeof("httpsocket"));
		add_soap_fault(this_ptr, "SOAP-ENV:Client", "Error Fetching http headers", NULL, NULL TSRMLS_CC);
		return FALSE;
	}

	/* Check to see what HTTP status was sent */
	http_version = get_http_header_value(http_headers,"HTTP/");
	if (http_version) {
		char *tmp;

		tmp = strstr(http_version," ");

		if (tmp != NULL) {
			tmp++;
			strncpy(http_status,tmp,4);
			http_status[3] = '\0';
		}

		/*
		Try and process any respsone that is xml might contain fault code

		Maybe try and test for some of the 300's 400's specfics but not
		right now.

		if (strcmp(http_status,"200"))
		{
			zval *err;
			char *http_err;

			MAKE_STD_ZVAL(err);
			ZVAL_STRING(err, http_body, 1);
			http_err = emalloc(strlen("HTTP request failed ()") + 4);
			sprintf(http_err, "HTTP request failed (%s)", http_status);
			add_soap_fault(thisObj, "SOAP-ENV:Client", http_err, NULL, err TSRMLS_CC);
			efree(http_err);
			return;
		}*/

		/* Try and get headers again */
		if (!strcmp(http_status, "100")) {
			if (!get_http_headers(stream, &http_headers, &http_header_size TSRMLS_CC)) {
				php_stream_close(stream);
				zend_hash_del(Z_OBJPROP_P(this_ptr), "httpsocket", sizeof("httpsocket"));
				add_soap_fault(this_ptr, "SOAP-ENV:Client", "Error Fetching http headers", NULL, NULL TSRMLS_CC);
				return FALSE;
			}
		}

		if (strncmp(http_version,"1.1", 3)) {
			http_1_1 = 1;
		}
		efree(http_version);
	}

	if (!get_http_body(stream, http_headers, &http_body, &http_body_size TSRMLS_CC)) {
		php_stream_close(stream);
		zend_hash_del(Z_OBJPROP_P(this_ptr), "httpsocket", sizeof("httpsocket"));
		add_soap_fault(this_ptr, "SOAP-ENV:Client", "Error Fetching http body", NULL, NULL TSRMLS_CC);
		return FALSE;
	}

	if (zend_hash_find(Z_OBJPROP_P(this_ptr), "trace", sizeof("trace"), (void **) &trace) == SUCCESS &&
	    Z_LVAL_PP(trace) > 0) {
		add_property_stringl(this_ptr, "__last_response", http_body, http_body_size, 1);
	}

	/* Close every time right now till i can spend more time on it
	   it works.. it's just slower??
	*/
	/* See if the server requested a close */
	http_close = TRUE;
	connection = get_http_header_value(http_headers,"Connection: ");
	if (connection) {
		if (!strcmp(connection, "Keep-Alive")) {
			http_close = FALSE;
		}
		efree(connection);
	} else if (http_1_1) {
		http_close = FALSE;
	}

	if (http_close) {
		php_stream_close(stream);
		zend_hash_del(Z_OBJPROP_P(this_ptr), "httpsocket", sizeof("httpsocket"));
	}

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

		if (strncmp(content_type, "text/xml", cmplen)) {
			if (strncmp(http_body, "<?xml", 5)) {
				zval *err;
				MAKE_STD_ZVAL(err);
				ZVAL_STRINGL(err, http_body, http_body_size, 1);
				add_soap_fault(this_ptr, "SOAP-ENV:Client", "Didn't recieve an xml document", NULL, err TSRMLS_CC);
				efree(content_type);
				efree(http_headers);
				efree(http_body);
				return FALSE;
			}
		}
		efree(content_type);
	}

	/* Grab and send back every cookie */

	/* Not going to worry about Path: because
	   we shouldn't be changing urls so path dont
	   matter too much
	*/
	cookie_itt = strstr(http_headers,"Set-Cookie: ");
	while (cookie_itt) {
		char *end_pos, *cookie;
		char *eqpos, *sempos;
		smart_str name = {0}, value = {0};
		zval **cookies, *z_cookie;

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
			int cookie_len;

			if (sempos != NULL) {
				cookie_len = sempos-(eqpos+1);
			} else {	
				cookie_len = strlen(cookie)-(eqpos-cookie)-1;
			}

			smart_str_appendl(&name, cookie, eqpos - cookie);
			smart_str_0(&name);

			smart_str_appendl(&value, eqpos + 1, cookie_len);
			smart_str_0(&value);

			MAKE_STD_ZVAL(z_cookie);
			ZVAL_STRINGL(z_cookie, value.c, value.len, 1);

			zend_hash_update(Z_ARRVAL_PP(cookies), name.c, name.len + 1, &z_cookie, sizeof(zval *), NULL);
		}

		cookie_itt = strstr(cookie_itt + sizeof("Set-Cookie: "), "Set-Cookie: ");

		smart_str_free(&value);
		smart_str_free(&name);
		efree(cookie);
	}

	*buffer = http_body;
	*buffer_len = http_body_size;
	efree(http_headers);
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
			eol = strstr(tmp, "\r\n");
			if (eol == NULL) {
				eol = headers + headerslen;
			}
			return estrndup(tmp, eol - tmp);
		}

		/* find next line */
		pos = strstr(pos, "\r\n");
		if (pos) {
			pos += 2;
		}

	} while (pos);

	return NULL;
}

static int get_http_body(php_stream *stream, char *headers,  char **response, int *out_size TSRMLS_DC)
{
	char *trans_enc, *content_length, *http_buf = NULL;
	int http_buf_size = 0;

	trans_enc = get_http_header_value(headers, "Transfer-Encoding: ");
	content_length = get_http_header_value(headers, "Content-Length: ");

	if (trans_enc && !strcmp(trans_enc, "chunked")) {
		int buf_size = 0, len_size;
		char done, chunk_size[10];

		done = FALSE;
		http_buf = NULL;

		while (!done) {
			php_stream_gets(stream, chunk_size, sizeof(chunk_size));

			if (sscanf(chunk_size, "%x", &buf_size) != -1) {
				http_buf = erealloc(http_buf, http_buf_size + buf_size + 1);
				len_size = 0;

				while (http_buf_size < buf_size) {
					len_size += php_stream_read(stream, http_buf + http_buf_size, buf_size - len_size);
					http_buf_size += len_size;
				}

				/* Eat up '\r' '\n' */
				php_stream_getc(stream);php_stream_getc(stream);
			}
			if (buf_size == 0) {
				done = TRUE;
			}
		}
		efree(trans_enc);

		if (http_buf == NULL) {
			http_buf = estrndup("", 1);
			http_buf_size = 1;
		} else {
			http_buf[http_buf_size] = '\0';
		}

	} else if (content_length) {
		int size;
		size = atoi(content_length);
		http_buf = emalloc(size + 1);

		while (http_buf_size < size) {
			http_buf_size += php_stream_read(stream, http_buf + http_buf_size, size - http_buf_size);
		}
		http_buf[size] = '\0';
		efree(content_length);
	} else {
		php_error(E_ERROR, "Don't know how to read http body, No Content-Length or chunked data");
		return FALSE;
	}

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

		if (strcmp(headerbuf, "\r\n") == 0) {
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
