/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Jim Winstead <jimw@php.net>                                  |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>

#include "php.h"

#include "url.h"
#include "url_parser_ex.h"
#include "file.h"
#ifdef _OSD_POSIX
#ifndef APACHE
#error On this EBCDIC platform, PHP is only supported as an Apache module.
#else /*APACHE*/
#ifndef CHARSET_EBCDIC
#define CHARSET_EBCDIC /* this machine uses EBCDIC, not ASCII! */
#endif
#include "ebcdic.h"
#endif /*APACHE*/
#endif /*_OSD_POSIX*/

ZEND_DECLARE_MODULE_GLOBALS(url)

PHPAPI php_url *php_url_parse_ex (const char *str, int length)
{
	return php_url_parse(str);
}

PHPAPI php_url *php_url_parse (const char *str)
{
	php_url *url = (php_url*) ecalloc (1, sizeof(php_url));
	URLG(url_str) = str;

	url_parse_scheme(url);
	url_parse_authority(url);
	url_parse_path(url);
	url_parse_query_frag(url);

	return url;
}

PHPAPI void php_url_free (php_url *url)
{
	if (url->scheme) efree(url->scheme);
	if (url->user) efree(url->user);
	if (url->pass) efree(url->pass);
	if (url->host) efree(url->host);
	if (url->path) efree(url->path);
	if (url->query) efree(url->query);
	if (url->fragment) efree(url->fragment);
	efree(url);
}

/* {{{ proto mixed parse_url(string url, [int url_component])
   Parse a URL and return its components */
PHP_FUNCTION(parse_url)
{
	char *str;
	size_t str_len;
	php_url *resource;
	zend_long key = -1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|l", &str, &str_len, &key) == FAILURE) {
		return;
	}

	resource = php_url_parse(str);

	if (key > -1) {
		switch (key) {
			case PHP_URL_SCHEME:
				if (resource->scheme != NULL) RETVAL_STRING(resource->scheme);
				break;
			case PHP_URL_HOST:
				if (resource->host != NULL) RETVAL_STRING(resource->host);
				break;
			case PHP_URL_PORT:
				if (resource->port != 0) RETVAL_LONG(resource->port);
				break;
			case PHP_URL_USER:
				if (resource->user != NULL) RETVAL_STRING(resource->user);
				break;
			case PHP_URL_PASS:
				if (resource->pass != NULL) RETVAL_STRING(resource->pass);
				break;
			case PHP_URL_PATH:
				if (resource->path != NULL) RETVAL_STRING(resource->path);
				break;
			case PHP_URL_QUERY:
				if (resource->query != NULL) RETVAL_STRING(resource->query);
				break;
			case PHP_URL_FRAGMENT:
				if (resource->fragment != NULL) RETVAL_STRING(resource->fragment);
				break;
			default:
				php_error_docref(NULL, E_WARNING, "Invalid URL component identifier " ZEND_LONG_FMT, key);
				RETVAL_FALSE;
		}
		goto done;
	}

	/* allocate an array for return */
	array_init(return_value);

	/* add the various elements to the array */
	if (resource->scheme != NULL)
		add_assoc_string(return_value, "scheme", resource->scheme);
	if (resource->host != NULL)
		add_assoc_string(return_value, "host", resource->host);
	if (resource->port != 0)
		add_assoc_long(return_value, "port", resource->port);
	if (resource->user != NULL)
		add_assoc_string(return_value, "user", resource->user);
	if (resource->pass != NULL)
		add_assoc_string(return_value, "pass", resource->pass);
	if (resource->path != NULL)
		add_assoc_string(return_value, "path", resource->path);
	if (resource->query != NULL)
		add_assoc_string(return_value, "query", resource->query);
	if (resource->fragment != NULL)
		add_assoc_string(return_value, "fragment", resource->fragment);

	if (UNEXPECTED(!zend_array_count(HASH_OF(return_value)))) {
		efree(Z_ARRVAL_P(return_value));
		RETVAL_FALSE;
	}
done:
	php_url_free(resource);
}
/* }}} */

/* {{{ php_htoi
 */
static int php_htoi(char *s)
{
	int value;
	int c;

	c = ((unsigned char *)s)[0];
	if (isupper(c))
		c = tolower(c);
	value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;

	c = ((unsigned char *)s)[1];
	if (isupper(c))
		c = tolower(c);
	value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;

	return (value);
}
/* }}} */

/* rfc1738:

   ...The characters ";",
   "/", "?", ":", "@", "=" and "&" are the characters which may be
   reserved for special meaning within a scheme...

   ...Thus, only alphanumerics, the special characters "$-_.+!*'(),", and
   reserved characters used for their reserved purposes may be used
   unencoded within a URL...

   For added safety, we only leave -_. unencoded.
 */

static unsigned char hexchars[] = "0123456789ABCDEF";

/* {{{ php_url_encode
 */
PHPAPI zend_string *php_url_encode(char const *s, size_t len)
{
	register unsigned char c;
	unsigned char *to;
	unsigned char const *from, *end;
	zend_string *start;

	from = (unsigned char *)s;
	end = (unsigned char *)s + len;
	start = zend_string_safe_alloc(3, len, 0, 0);
	to = (unsigned char*)ZSTR_VAL(start);

	while (from < end) {
		c = *from++;

		if (c == ' ') {
			*to++ = '+';
#ifndef CHARSET_EBCDIC
		} else if ((c < '0' && c != '-' && c != '.') ||
				   (c < 'A' && c > '9') ||
				   (c > 'Z' && c < 'a' && c != '_') ||
				   (c > 'z')) {
			to[0] = '%';
			to[1] = hexchars[c >> 4];
			to[2] = hexchars[c & 15];
			to += 3;
#else /*CHARSET_EBCDIC*/
		} else if (!isalnum(c) && strchr("_-.", c) == NULL) {
			/* Allow only alphanumeric chars and '_', '-', '.'; escape the rest */
			to[0] = '%';
			to[1] = hexchars[os_toascii[c] >> 4];
			to[2] = hexchars[os_toascii[c] & 15];
			to += 3;
#endif /*CHARSET_EBCDIC*/
		} else {
			*to++ = c;
		}
	}
	*to = '\0';

	start = zend_string_truncate(start, to - (unsigned char*)ZSTR_VAL(start), 0);

	return start;
}
/* }}} */

/* {{{ proto string urlencode(string str)
   URL-encodes string */
PHP_FUNCTION(urlencode)
{
	zend_string *in_str;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &in_str) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(in_str)
	ZEND_PARSE_PARAMETERS_END();
#endif

	RETURN_STR(php_url_encode(ZSTR_VAL(in_str), ZSTR_LEN(in_str)));
}
/* }}} */

/* {{{ proto string urldecode(string str)
   Decodes URL-encoded string */
PHP_FUNCTION(urldecode)
{
	zend_string *in_str, *out_str;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &in_str) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(in_str)
	ZEND_PARSE_PARAMETERS_END();
#endif

	out_str = zend_string_init(ZSTR_VAL(in_str), ZSTR_LEN(in_str), 0);
	ZSTR_LEN(out_str) = php_url_decode(ZSTR_VAL(out_str), ZSTR_LEN(out_str));

    RETURN_NEW_STR(out_str);
}
/* }}} */

/* {{{ php_url_decode
 */
PHPAPI size_t php_url_decode(char *str, size_t len)
{
	char *dest = str;
	char *data = str;

	while (len--) {
		if (*data == '+') {
			*dest = ' ';
		}
		else if (*data == '%' && len >= 2 && isxdigit((int) *(data + 1))
				 && isxdigit((int) *(data + 2))) {
#ifndef CHARSET_EBCDIC
			*dest = (char) php_htoi(data + 1);
#else
			*dest = os_toebcdic[(char) php_htoi(data + 1)];
#endif
			data += 2;
			len -= 2;
		} else {
			*dest = *data;
		}
		data++;
		dest++;
	}
	*dest = '\0';
	return dest - str;
}
/* }}} */

/* {{{ php_raw_url_encode
 */
PHPAPI zend_string *php_raw_url_encode(char const *s, size_t len)
{
	register size_t x, y;
	zend_string *str;

	str = zend_string_safe_alloc(3, len, 0, 0);
	for (x = 0, y = 0; len--; x++, y++) {
		ZSTR_VAL(str)[y] = (unsigned char) s[x];
#ifndef CHARSET_EBCDIC
		if ((ZSTR_VAL(str)[y] < '0' && ZSTR_VAL(str)[y] != '-' && ZSTR_VAL(str)[y] != '.') ||
			(ZSTR_VAL(str)[y] < 'A' && ZSTR_VAL(str)[y] > '9') ||
			(ZSTR_VAL(str)[y] > 'Z' && ZSTR_VAL(str)[y] < 'a' && ZSTR_VAL(str)[y] != '_') ||
			(ZSTR_VAL(str)[y] > 'z' && ZSTR_VAL(str)[y] != '~')) {
			ZSTR_VAL(str)[y++] = '%';
			ZSTR_VAL(str)[y++] = hexchars[(unsigned char) s[x] >> 4];
			ZSTR_VAL(str)[y] = hexchars[(unsigned char) s[x] & 15];
#else /*CHARSET_EBCDIC*/
		if (!isalnum(ZSTR_VAL(str)[y]) && strchr("_-.~", ZSTR_VAL(str)[y]) != NULL) {
			ZSTR_VAL(str)[y++] = '%';
			ZSTR_VAL(str)[y++] = hexchars[os_toascii[(unsigned char) s[x]] >> 4];
			ZSTR_VAL(str)[y] = hexchars[os_toascii[(unsigned char) s[x]] & 15];
#endif /*CHARSET_EBCDIC*/
		}
	}
	ZSTR_VAL(str)[y] = '\0';
	str = zend_string_truncate(str, y, 0);

	return str;
}
/* }}} */

/* {{{ proto string rawurlencode(string str)
   URL-encodes string */
PHP_FUNCTION(rawurlencode)
{
	zend_string *in_str;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &in_str) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(in_str)
	ZEND_PARSE_PARAMETERS_END();
#endif

	RETURN_STR(php_raw_url_encode(ZSTR_VAL(in_str), ZSTR_LEN(in_str)));
}
/* }}} */

/* {{{ proto string rawurldecode(string str)
   Decodes URL-encodes string */
PHP_FUNCTION(rawurldecode)
{
	zend_string *in_str, *out_str;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &in_str) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(in_str)
	ZEND_PARSE_PARAMETERS_END();
#endif

	out_str = zend_string_init(ZSTR_VAL(in_str), ZSTR_LEN(in_str), 0);
	ZSTR_LEN(out_str) = php_raw_url_decode(ZSTR_VAL(out_str), ZSTR_LEN(out_str));

    RETURN_NEW_STR(out_str);
}
/* }}} */

/* {{{ php_raw_url_decode
 */
PHPAPI size_t php_raw_url_decode(char *str, size_t len)
{
	char *dest = str;
	char *data = str;

	while (len--) {
		if (*data == '%' && len >= 2 && isxdigit((int) *(data + 1))
			&& isxdigit((int) *(data + 2))) {
#ifndef CHARSET_EBCDIC
			*dest = (char) php_htoi(data + 1);
#else
			*dest = os_toebcdic[(char) php_htoi(data + 1)];
#endif
			data += 2;
			len -= 2;
		} else {
			*dest = *data;
		}
		data++;
		dest++;
	}
	*dest = '\0';
	return dest - str;
}
/* }}} */

/* {{{ proto array get_headers(string url[, int format[, resource context]])
   fetches all the headers sent by the server in response to a HTTP request */
PHP_FUNCTION(get_headers)
{
	char *url;
	size_t url_len;
	php_stream *stream;
	zval *prev_val, *hdr = NULL, *h;
	HashTable *hashT;
	zend_long format = 0;
	zval *zcontext = NULL;
	php_stream_context *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|lr!", &url, &url_len, &format, &zcontext) == FAILURE) {
		return;
	}

	context = php_stream_context_from_zval(zcontext, 0);

	if (!(stream = php_stream_open_wrapper_ex(url, "r", REPORT_ERRORS | STREAM_USE_URL | STREAM_ONLY_GET_HEADERS, NULL, context))) {
		RETURN_FALSE;
	}

	if (Z_TYPE(stream->wrapperdata) != IS_ARRAY) {
		php_stream_close(stream);
		RETURN_FALSE;
	}

	array_init(return_value);

	/* check for curl-wrappers that provide headers via a special "headers" element */
	if ((h = zend_hash_str_find(HASH_OF(&stream->wrapperdata), "headers", sizeof("headers")-1)) != NULL && Z_TYPE_P(h) == IS_ARRAY) {
		/* curl-wrappers don't load data until the 1st read */
		if (!Z_ARRVAL_P(h)->nNumOfElements) {
			php_stream_getc(stream);
		}
		h = zend_hash_str_find(HASH_OF(&stream->wrapperdata), "headers", sizeof("headers")-1);
		hashT = Z_ARRVAL_P(h);
	} else {
		hashT = HASH_OF(&stream->wrapperdata);
	}

	ZEND_HASH_FOREACH_VAL(hashT, hdr) {
		if (Z_TYPE_P(hdr) != IS_STRING) {
			continue;
		}
		if (!format) {
no_name_header:
			add_next_index_str(return_value, zend_string_copy(Z_STR_P(hdr)));
		} else {
			char c;
			char *s, *p;

			if ((p = strchr(Z_STRVAL_P(hdr), ':'))) {
				c = *p;
				*p = '\0';
				s = p + 1;
				while (isspace((int)*(unsigned char *)s)) {
					s++;
				}

				if ((prev_val = zend_hash_str_find(Z_ARRVAL_P(return_value), Z_STRVAL_P(hdr), (p - Z_STRVAL_P(hdr)))) == NULL) {
					add_assoc_stringl_ex(return_value, Z_STRVAL_P(hdr), (p - Z_STRVAL_P(hdr)), s, (Z_STRLEN_P(hdr) - (s - Z_STRVAL_P(hdr))));
				} else { /* some headers may occur more than once, therefor we need to remake the string into an array */
					convert_to_array(prev_val);
					add_next_index_stringl(prev_val, s, (Z_STRLEN_P(hdr) - (s - Z_STRVAL_P(hdr))));
				}

				*p = c;
			} else {
				goto no_name_header;
			}
		}
	} ZEND_HASH_FOREACH_END();

	php_stream_close(stream);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker noet
 * vim<600: sw=4 ts=4 noet
 */
