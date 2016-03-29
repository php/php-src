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

/* {{{ free_url
 */
PHPAPI void php_url_free(php_url *theurl)
{
	if (theurl->scheme)
		efree(theurl->scheme);
	if (theurl->user)
		efree(theurl->user);
	if (theurl->pass)
		efree(theurl->pass);
	if (theurl->host)
		efree(theurl->host);
	if (theurl->path)
		efree(theurl->path);
	if (theurl->query)
		efree(theurl->query);
	if (theurl->fragment)
		efree(theurl->fragment);
	efree(theurl);
}
/* }}} */

/* {{{ php_replace_controlchars
 */
PHPAPI char *php_replace_controlchars_ex(char *str, size_t len)
{
	unsigned char *s = (unsigned char *)str;
	unsigned char *e = (unsigned char *)str + len;

	if (!str) {
		return (NULL);
	}

	while (s < e) {

		if (iscntrl(*s)) {
			*s='_';
		}
		s++;
	}

	return (str);
}
/* }}} */

PHPAPI char *php_replace_controlchars(char *str)
{
	return php_replace_controlchars_ex(str, strlen(str));
}

PHPAPI php_url *php_url_parse(char const *str)
{
	return php_url_parse_ex(str, strlen(str));
}

/* {{{ php_url_parse
 */
PHPAPI php_url *php_url_parse_ex(char const *str, size_t length)
{
	char port_buf[6];
	php_url *ret = ecalloc(1, sizeof(php_url));
	char const *s, *e, *p, *pp, *ue;

	s = str;
	ue = s + length;

	/* parse scheme */
	if ((e = memchr(s, ':', length)) && (e - s)) {
		/* validate scheme */
		p = s;
		while (p < e) {
			/* scheme = 1*[ lowalpha | digit | "+" | "-" | "." ] */
			if (!isalpha(*p) && !isdigit(*p) && *p != '+' && *p != '.' && *p != '-') {
				if (e + 1 < ue && e < s + strcspn(s, "?#")) {
					goto parse_port;
				} else {
					goto just_path;
				}
			}
			p++;
		}

		if (*(e + 1) == '\0') { /* only scheme is available */
			ret->scheme = estrndup(s, (e - s));
			php_replace_controlchars_ex(ret->scheme, (e - s));
			goto end;
		}

		/*
		 * certain schemas like mailto: and zlib: may not have any / after them
		 * this check ensures we support those.
		 */
		if (*(e+1) != '/') {
			/* check if the data we get is a port this allows us to
			 * correctly parse things like a.com:80
			 */
			p = e + 1;
			while (isdigit(*p)) {
				p++;
			}

			if ((*p == '\0' || *p == '/') && (p - e) < 7) {
				goto parse_port;
			}

			ret->scheme = estrndup(s, (e-s));
			php_replace_controlchars_ex(ret->scheme, (e - s));

			length -= ++e - s;
			s = e;
			goto just_path;
		} else {
			ret->scheme = estrndup(s, (e-s));
			php_replace_controlchars_ex(ret->scheme, (e - s));

			if (*(e+2) == '/') {
				s = e + 3;
				if (!strncasecmp("file", ret->scheme, sizeof("file"))) {
					if (*(e + 3) == '/') {
						/* support windows drive letters as in:
						   file:///c:/somedir/file.txt
						*/
						if (*(e + 5) == ':') {
							s = e + 4;
						}
						goto nohost;
					}
				}
			} else {
				if (!strncasecmp("file", ret->scheme, sizeof("file"))) {
					s = e + 1;
					goto nohost;
				} else {
					length -= ++e - s;
					s = e;
					goto just_path;
				}
			}
		}
	} else if (e) { /* no scheme; starts with colon: look for port */
		parse_port:
		p = e + 1;
		pp = p;

		while (pp-p < 6 && isdigit(*pp)) {
			pp++;
		}

		if (pp - p > 0 && pp - p < 6 && (*pp == '/' || *pp == '\0')) {
			zend_long port;
			memcpy(port_buf, p, (pp - p));
			port_buf[pp - p] = '\0';
			port = ZEND_STRTOL(port_buf, NULL, 10);
			if (port > 0 && port <= 65535) {
				ret->port = (unsigned short) port;
				if (*s == '/' && *(s + 1) == '/') { /* relative-scheme URL */
				    s += 2;
				}
			} else {
				if (ret->scheme) efree(ret->scheme);
				efree(ret);
				return NULL;
			}
		} else if (p == pp && *pp == '\0') {
			if (ret->scheme) efree(ret->scheme);
			efree(ret);
			return NULL;
		} else if (*s == '/' && *(s + 1) == '/') { /* relative-scheme URL */
			s += 2;
		} else {
			goto just_path;
		}
	} else if (*s == '/' && *(s + 1) == '/') { /* relative-scheme URL */
		s += 2;
	} else {
		just_path:
		ue = s + length;
		goto nohost;
	}

	e = ue;

	if (!(p = memchr(s, '/', (ue - s)))) {
		char *query, *fragment;

		query = memchr(s, '?', (ue - s));
		fragment = memchr(s, '#', (ue - s));

		if (query && fragment) {
			if (query > fragment) {
				e = fragment;
			} else {
				e = query;
			}
		} else if (query) {
			e = query;
		} else if (fragment) {
			e = fragment;
		}
	} else {
		e = p;
	}

	/* check for login and password */
	if ((p = zend_memrchr(s, '@', (e-s)))) {
		if ((pp = memchr(s, ':', (p-s)))) {
			ret->user = estrndup(s, (pp-s));
			php_replace_controlchars_ex(ret->user, (pp - s));

			pp++;
			ret->pass = estrndup(pp, (p-pp));
			php_replace_controlchars_ex(ret->pass, (p-pp));
		} else {
			ret->user = estrndup(s, (p-s));
			php_replace_controlchars_ex(ret->user, (p-s));
		}

		s = p + 1;
	}

	/* check for port */
	if (*s == '[' && *(e-1) == ']') {
		/* Short circuit portscan,
		   we're dealing with an
		   IPv6 embedded address */
		p = s;
	} else {
		/* memrchr is a GNU specific extension
		   Emulate for wide compatibility */
		for(p = e; p >= s && *p != ':'; p--);
	}

	if (p >= s && *p == ':') {
		if (!ret->port) {
			p++;
			if (e-p > 5) { /* port cannot be longer then 5 characters */
				if (ret->scheme) efree(ret->scheme);
				if (ret->user) efree(ret->user);
				if (ret->pass) efree(ret->pass);
				efree(ret);
				return NULL;
			} else if (e - p > 0) {
				zend_long port;
				memcpy(port_buf, p, (e - p));
				port_buf[e - p] = '\0';
				port = ZEND_STRTOL(port_buf, NULL, 10);
				if (port > 0 && port <= 65535) {
					ret->port = (unsigned short)port;
				} else {
					if (ret->scheme) efree(ret->scheme);
					if (ret->user) efree(ret->user);
					if (ret->pass) efree(ret->pass);
					efree(ret);
					return NULL;
				}
			}
			p--;
		}
	} else {
		p = e;
	}

	/* check if we have a valid host, if we don't reject the string as url */
	if ((p-s) < 1) {
		if (ret->scheme) efree(ret->scheme);
		if (ret->user) efree(ret->user);
		if (ret->pass) efree(ret->pass);
		efree(ret);
		return NULL;
	}

	ret->host = estrndup(s, (p-s));
	php_replace_controlchars_ex(ret->host, (p - s));

	if (e == ue) {
		return ret;
	}

	s = e;

	nohost:

	if ((p = memchr(s, '?', (ue - s)))) {
		pp = memchr(s, '#', (ue - s));

		if (pp && pp < p) {
			if (pp - s) {
				ret->path = estrndup(s, (pp-s));
				php_replace_controlchars_ex(ret->path, (pp - s));
			}
			p = pp;
			goto label_parse;
		}

		if (p - s) {
			ret->path = estrndup(s, (p-s));
			php_replace_controlchars_ex(ret->path, (p - s));
		}

		if (pp) {
			if (pp - ++p) {
				ret->query = estrndup(p, (pp-p));
				php_replace_controlchars_ex(ret->query, (pp - p));
			}
			p = pp;
			goto label_parse;
		} else if (++p - ue) {
			ret->query = estrndup(p, (ue-p));
			php_replace_controlchars_ex(ret->query, (ue - p));
		}
	} else if ((p = memchr(s, '#', (ue - s)))) {
		if (p - s) {
			ret->path = estrndup(s, (p-s));
			php_replace_controlchars_ex(ret->path, (p - s));
		}

		label_parse:
		p++;

		if (ue - p) {
			ret->fragment = estrndup(p, (ue-p));
			php_replace_controlchars_ex(ret->fragment, (ue - p));
		}
	} else {
		ret->path = estrndup(s, (ue-s));
		php_replace_controlchars_ex(ret->path, (ue - s));
	}
end:
	return ret;
}
/* }}} */

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

	resource = php_url_parse_ex(str, str_len);
	if (resource == NULL) {
		/* @todo Find a method to determine why php_url_parse_ex() failed */
		RETURN_FALSE;
	}

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

/* {{{ proto array get_headers(string url[, int format])
   fetches all the headers sent by the server in response to a HTTP request */
PHP_FUNCTION(get_headers)
{
	char *url;
	size_t url_len;
	php_stream_context *context;
	php_stream *stream;
	zval *prev_val, *hdr = NULL, *h;
	HashTable *hashT;
	zend_long format = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|l", &url, &url_len, &format) == FAILURE) {
		return;
	}
	context = FG(default_context) ? FG(default_context) : (FG(default_context) = php_stream_context_alloc());

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
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
