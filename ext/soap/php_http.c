#include "php_soap.h"

void send_http_soap_request(zval *this_ptr, xmlDoc *doc, char *function_name, char *soapaction TSRMLS_DC)
{
	xmlChar *buf;
	char *soap_headers;
	int buf_size,err,ret;
	sdlPtr sdl;
	php_url *phpurl = NULL;
	SOAP_STREAM stream;
	zval **trace;

/*	TSRMLS_FETCH();*/

	FETCH_THIS_SOCKET(stream);
	FETCH_THIS_URL(phpurl);
	FETCH_THIS_SDL(sdl);

	xmlDocDumpMemory(doc, &buf, &buf_size);

	if(!buf)
		php_error(E_ERROR, "Error build soap request");

	if(zend_hash_find(Z_OBJPROP_P(this_ptr), "trace", sizeof("trace"), (void **) &trace) == SUCCESS
		&& Z_LVAL_PP(trace) > 0)
		add_property_stringl(this_ptr, "__last_request", buf, buf_size, 1);

	if(!stream)
	{
		char *url;

		if(!sdl)
		{
			zval **location;
			if(zend_hash_find(Z_OBJPROP_P(this_ptr), "location", sizeof("location"),(void **) &location) == FAILURE)
				php_error(E_ERROR, "Error could not find location");
			url = Z_STRVAL_PP(location);
		}
		else
		{
			sdlBindingPtr binding;
			FETCH_THIS_PORT(binding);
			url = binding->location;
		}

		phpurl = php_url_parse(url);

#ifdef PHP_STREAMS
		stream = php_stream_sock_open_host(phpurl->host, (unsigned short)(phpurl->port == 0 ? 80 : phpurl->port), SOCK_STREAM, 0, 0);
#else
		stream = get_socket(phpurl->host, (phpurl->port == 0 ? 80 : phpurl->port), 10);
#endif
		if(stream)
		{
			ret = zend_list_insert((void *)stream, le_http_socket);
			add_property_resource(this_ptr, "httpsocket", ret);
			zend_list_addref(ret);

			ret = zend_list_insert(phpurl, le_url);
			add_property_resource(this_ptr, "httpurl", ret);
			zend_list_addref(ret);
		}
		else
			php_error(E_ERROR,"Could not connect to host");
		/*
		 php_url_free(phpurl);
		 */
	}

	if(stream)
	{
		zval **cookies;
		char *header = "POST %s HTTP/1.1\r\nConnection: close\r\nAccept: text/html; text/xml; text/plain\r\nUser-Agent: PHP SOAP 0.1\r\nHost: %s\r\nContent-Type: text/xml\r\nContent-Length: %d\r\nSOAPAction: \"%s\"\r\n";
		int size = strlen(header) + strlen(phpurl->host) + strlen(phpurl->path) + 10;

		/* TODO: Add authentication */
		if(sdl != NULL)
		{
			/* TODO: need to grab soap action from wsdl....*/
			soap_headers = emalloc(size + strlen(soapaction));
			sprintf(soap_headers, header, phpurl->path, phpurl->host, buf_size, soapaction);
		}
		else
		{
			soap_headers = emalloc(size + strlen(soapaction));
			sprintf(soap_headers, header, phpurl->path, phpurl->host, buf_size, soapaction);
		}

#ifdef PHP_STREAMS
		err = php_stream_write(stream, soap_headers, strlen(soap_headers));
#else
		err = send(stream, soap_headers, strlen(soap_headers), 0);
#endif
		if(err != (int)strlen(soap_headers))
			php_error(E_ERROR,"Failed Sending HTTP Headers");

		/* Send cookies along with request */
		if(zend_hash_find(Z_OBJPROP_P(this_ptr), "_cookies", sizeof("_cookies"), (void **)&cookies) == SUCCESS)
		{
			smart_str cookie_str = {0};
			zval **data;
			char *key;
			int index, i;

			zend_hash_internal_pointer_reset(Z_ARRVAL_PP(cookies));
			for(i = 0;i < (int)Z_ARRVAL_PP(cookies)->nNumOfElements;i++)
			{
				zend_hash_get_current_data(Z_ARRVAL_PP(cookies), (void **)&data);
				zend_hash_get_current_key(Z_ARRVAL_PP(cookies), &key, (long *)&index, FALSE);

				smart_str_appendl(&cookie_str, "Cookie: ", 8);
				smart_str_appendl(&cookie_str, key, strlen(key));
				smart_str_appendc(&cookie_str, '=');
				smart_str_appendl(&cookie_str, Z_STRVAL_PP(data), Z_STRLEN_PP(data));
				smart_str_appendc(&cookie_str, ';');
				zend_hash_move_forward(Z_ARRVAL_PP(cookies));
			}
			smart_str_appendl(&cookie_str, "\r\n", 2);
			smart_str_0(&cookie_str);

#ifdef PHP_STREAMS
			err = php_stream_write(stream, cookie_str.c, cookie_str.len);
#else
			err = send(stream, cookie_str.c, cookie_str.len,0);
#endif
			if(err != (int)cookie_str.len)
				php_error(E_ERROR,"Failed Sending HTTP Headers");

			smart_str_free(&cookie_str);
		}

#ifdef PHP_STREAMS
		err = php_stream_write(stream, "\r\n", 2);
#else
		err = send(stream, "\r\n", 2, 0);
#endif
		if(err != 2)
			php_error(E_ERROR,"Failed Sending HTTP Headers");


#ifdef PHP_STREAMS
		err = php_stream_write(stream, buf, buf_size);
#else
		err = send(stream, buf, buf_size, 0);
#endif
		if(err != (int)strlen(buf))
			php_error(E_ERROR,"Failed Sending HTTP Content");

		efree(soap_headers);
	}
	xmlFree(buf);
}

void get_http_soap_response(zval *this_ptr, char **buffer, int *buffer_len TSRMLS_DC)
{
	char *http_headers, *http_body, *content_type, *http_version, http_status[4], *cookie_itt;
	int http_header_size, http_body_size, http_close;
	sdlPtr sdl;
	zval **socket_ref;
	SOAP_STREAM stream;
	zval **trace;

/*	TSRMLS_FETCH();*/

	FETCH_THIS_SDL(sdl);

	if(FIND_SOCKET_PROPERTY(this_ptr, socket_ref) != FAILURE)
	{
		FETCH_SOCKET_RES(stream, socket_ref);
	}

	if(!get_http_headers(stream, &http_headers, &http_header_size TSRMLS_CC))
		php_error(E_ERROR, "Error Fetching http headers");

	/* Check to see what HTTP status was sent */
	http_version = get_http_header_value(http_headers,"HTTP/");
	if(http_version)
	{
		char *tmp;

		tmp = strstr(http_version," ");

		if(tmp != NULL)
		{
			tmp++;
			strncpy(http_status,tmp,4);
			http_status[3] = '\0';
		}

		/*
		Try and process any respsone that is xml might contain fault code

		Maybe try and test for some of the 300's 400's specfics but not
		right now.

		if(strcmp(http_status,"200"))
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
		if(!strcmp(http_status, "100"))
		{
			if(!get_http_headers(stream, &http_headers, &http_header_size TSRMLS_CC))
				php_error(E_ERROR, "Error Fetching http headers");
		}

		efree(http_version);
	}
	
	if(!get_http_body(stream, http_headers, &http_body, &http_body_size TSRMLS_CC))
		php_error(E_ERROR, "Error Fetching http body");

	if(zend_hash_find(Z_OBJPROP_P(this_ptr), "trace", sizeof("trace"), (void **) &trace) == SUCCESS
		&& Z_LVAL_PP(trace) > 0)
		add_property_stringl(this_ptr, "__last_response", http_body, http_body_size, 1);

	/* Close every time right now till i can spend more time on it
	   it works.. it's just slower??
	*/
	/* See if the server requested a close */
	http_close = TRUE;
	/*
	connection = get_http_header_value(http_headers,"Connection: ");
	if(connection)
	{
		if(!strcmp(connection, "Keep-Alive"))
			http_close = FALSE;
		efree(connection);
	}
	else
	{
		if(!strncmp(http_version,"1.1", 3))
				http_close = FALSE;
	}
	*/

	if(http_close)
	{
#ifdef PHP_STREAMS
		php_stream_close(stream);
#else
		SOCK_CLOSE(stream);
#endif
		zend_list_delete(Z_RESVAL_PP(socket_ref));
		zend_hash_del(Z_OBJPROP_P(this_ptr), "httpsocket", strlen("httpsocket") + 1);
	}

	/* Check and see if the server even sent a xml document */
	content_type = get_http_header_value(http_headers,"Content-Type: ");
	if(content_type)
	{
		char *pos = NULL;
		int cmplen;
		pos = strstr(content_type,";");
		if(pos != NULL)
			cmplen = pos - content_type;
		else
			cmplen = strlen(content_type);

		if(strncmp(content_type, "text/xml", cmplen))
		{
			if(strncmp(http_body, "<?xml", 5))
			{
				zval *err;
				MAKE_STD_ZVAL(err);
				ZVAL_STRINGL(err, http_body, http_body_size, 1);
				add_soap_fault(this_ptr, "SOAP-ENV:Client", "Didn't recieve an xml document", NULL, err TSRMLS_CC);
				efree(content_type);
				return;
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
	while(cookie_itt)
	{
		char *end_pos, *cookie;
		char *eqpos, *sempos;
		smart_str name = {0}, value = {0};
		zval **cookies, *z_cookie;

		if(zend_hash_find(Z_OBJPROP_P(this_ptr), "_cookies", sizeof("_cookies"), (void **)&cookies) == FAILURE)
		{
			zval *tmp_cookies;
			MAKE_STD_ZVAL(tmp_cookies);
			array_init(tmp_cookies);
			zend_hash_update(Z_OBJPROP_P(this_ptr), "_cookies", sizeof("_cookies"), &tmp_cookies, sizeof(zval *), (void **)&cookies);
		}

		end_pos = strstr(cookie_itt,"\r\n");
		cookie = get_http_header_value(cookie_itt,"Set-Cookie: ");

		eqpos = strstr(cookie, "=");
		sempos = strstr(cookie, ";");

		smart_str_appendl(&name, cookie, eqpos - cookie);
		smart_str_0(&name);

		smart_str_appendl(&value, eqpos + 1, sempos - (eqpos + 1));
		smart_str_0(&value);

		MAKE_STD_ZVAL(z_cookie);
		ZVAL_STRINGL(z_cookie, value.c, value.len, 1);

		zend_hash_update(Z_ARRVAL_PP(cookies), name.c, name.len + 1, &z_cookie, sizeof(zval *), NULL);

		cookie_itt = strstr(cookie_itt + sizeof("Set-Cookie: "), "Set-Cookie: ");

		smart_str_free(&value);
		smart_str_free(&name);
		efree(cookie);
		cookie_itt = FALSE;
	}

	*buffer = http_body;
	*buffer_len = http_body_size;
	efree(http_headers);
}

char *get_http_header_value(char *headers, char *type)
{
	char *tmp = NULL,*var = NULL;
	int size;

	tmp = strstr(headers, type);
	if(tmp != NULL)
	{
		tmp += strlen(type);
		size = strstr(tmp, "\r\n") - tmp;
		var = emalloc(size + 1);
		strncpy(var, tmp, size);
		var[size] = '\0';
	}
	return var;
}

int get_http_body(SOAP_STREAM stream, char *headers,  char **response, int *out_size TSRMLS_DC)
{
	char *trans_enc, *content_length, *http_buf;
	int http_buf_size = 0;
/*	TSRMLS_FETCH();*/

	trans_enc = get_http_header_value(headers, "Transfer-Encoding: ");
	content_length = get_http_header_value(headers, "Content-Length: ");

	/* this is temp...
	  netscape enterprise server sends in lowercase???
	*/
	if(content_length == NULL)
		content_length = get_http_header_value(headers, "Content-length: ");

	if(trans_enc && !strcmp(trans_enc, "chunked"))
	{
		int cur = 0, size = 0, buf_size = 0, len_size;
		char done, chunk_size[10];

		done = FALSE;
		http_buf = emalloc(1);
		*http_buf = '\0';
		while(!done)
		{
			for (cur = 0; cur < 3 || !(chunk_size[cur - 2] == '\r' && chunk_size[cur - 1] == '\n'); cur++)
#ifdef PHP_STREAMS
				chunk_size[cur] = php_stream_getc(stream);
#else
				chunk_size[cur] = php_sock_fgetc(stream);
#endif
			chunk_size[cur] = '\0';
			if(sscanf(chunk_size,"%x",&buf_size) != -1)
			{
				http_buf = erealloc(http_buf,http_buf_size + buf_size);
				len_size = 0;
				while(http_buf_size < buf_size)
				{
#ifdef PHP_STREAMS
					len_size += php_stream_read(stream, &http_buf[http_buf_size], buf_size - len_size);
#else
					len_size += php_sock_fread(&http_buf[http_buf_size], buf_size - len_size, stream);
#endif
					http_buf_size += len_size;
				}
#ifdef PHP_STREAMS
				php_stream_getc(stream);php_stream_getc(stream);
#else
				/* Eat up '\r' '\n' */
				php_sock_fgetc(stream);php_sock_fgetc(stream);
#endif
			}
			if(buf_size == 0)
				done = TRUE;
		}
		efree(trans_enc);
	}
	else if(content_length)
	{
		int size;
		size = atoi(content_length);
		http_buf = emalloc(size + 1);

		while(http_buf_size < size)
#ifdef PHP_STREAMS
			http_buf_size += php_stream_read(stream, &http_buf[http_buf_size], size - http_buf_size);
#else
			http_buf_size += php_sock_fread(&http_buf[http_buf_size], size - http_buf_size, stream);
#endif
		http_buf[size] = '\0';
		efree(content_length);
	}
	else
		php_error(E_ERROR,"Don't know how to read http body, No Content-Length or chunked data");

	(*response) = http_buf;
	(*out_size) = http_buf_size;
	return TRUE;
}

int get_http_headers(SOAP_STREAM stream, char **response, int *out_size TSRMLS_DC)
{
	int done;
	char chr;
	smart_str tmp_response = {0};
/*	TSRMLS_FETCH();//i think this is not needed - even the parameter */

	done = FALSE;

	while(!done)
	{
#ifdef PHP_STREAMS
		chr = php_stream_getc(stream);
#else
		chr = php_sock_fgetc(stream);
#endif
		if(chr != EOF)
		{
			smart_str_appendc(&tmp_response, chr);
			if(tmp_response.c[tmp_response.len - 2] == '\r' && tmp_response.c[tmp_response.len - 1] == '\n' &&
				tmp_response.c[tmp_response.len - 4] == '\r' && tmp_response.c[tmp_response.len - 3] == '\n')
			{
				smart_str_0(&tmp_response);
				done = TRUE;
			}
		}
		else
			return FALSE;
	}
	(*response) = tmp_response.c;
	(*out_size) = tmp_response.len;
	return TRUE;
}

#ifndef PHP_STREAMS
SOCKET get_socket(char* host,int portno,int time)
{
	SOCKET socketd = -1;
	struct sockaddr_in server;
	struct timeval timeout;

	memset(&server, 0, sizeof(server));
	socketd = socket(AF_INET,SOCK_STREAM,0);
	if (socketd == SOCK_ERR) {
		if(socketd > 0)
			SOCK_CLOSE(socketd);
		return FALSE;
	}
	server.sin_family = AF_INET;

	if(php_lookup_hostname(host,&server.sin_addr)) {
		if(socketd > 0)
			SOCK_CLOSE(socketd);
		return FALSE;
	}
	server.sin_port = htons((unsigned short)portno);
	timeout.tv_sec = time;
	timeout.tv_usec = 0;
	if (php_connect_nonb(socketd, (struct sockaddr *)&server, sizeof(struct sockaddr_in), &timeout) == SOCK_CONN_ERR) {
		if(socketd > 0)
			SOCK_CLOSE(socketd);
		return FALSE;
	}

	return socketd;
}
#endif
