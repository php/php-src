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
PHPAPI char *php_replace_controlchars_ex(char *str, int len)
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
PHPAPI php_url *php_url_parse_ex(char const *str, int length)
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
				if (e + 1 < ue) {
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
			long port;
			memcpy(port_buf, p, (pp - p));
			port_buf[pp - p] = '\0';
			port = strtol(port_buf, NULL, 10);
			if (port > 0 && port <= 65535) {
				ret->port = (unsigned short) port;
			} else {
				STR_FREE(ret->scheme);
				efree(ret);
				return NULL;
			}
		} else if (p == pp && *pp == '\0') {
			STR_FREE(ret->scheme);
			efree(ret);
			return NULL;
		} else if (*s == '/' && *(s+1) == '/') { /* relative-scheme URL */
			s += 2;
		} else {
			goto just_path;
		}
	} else if (*s == '/' && *(s+1) == '/') { /* relative-scheme URL */
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
			if ((pp-s) > 0) {
				ret->user = estrndup(s, (pp-s));
				php_replace_controlchars_ex(ret->user, (pp - s));
			}	
		
			pp++;
			if (p-pp > 0) {
				ret->pass = estrndup(pp, (p-pp));
				php_replace_controlchars_ex(ret->pass, (p-pp));
			}	
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
		for(p = e; *p != ':' && p >= s; p--);
	}

	if (p >= s && *p == ':') {
		if (!ret->port) {
			p++;
			if (e-p > 5) { /* port cannot be longer then 5 characters */
				STR_FREE(ret->scheme);
				STR_FREE(ret->user);
				STR_FREE(ret->pass);
				efree(ret);
				return NULL;
			} else if (e - p > 0) {
				long port;
				memcpy(port_buf, p, (e - p));
				port_buf[e - p] = '\0';
				port = strtol(port_buf, NULL, 10);
				if (port > 0 && port <= 65535) {
					ret->port = (unsigned short)port;
				} else {
					STR_FREE(ret->scheme);
					STR_FREE(ret->user);
					STR_FREE(ret->pass);
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
		STR_FREE(ret->scheme);
		STR_FREE(ret->user);
		STR_FREE(ret->pass);
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
		pp = strchr(s, '#');

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
	int str_len;
	php_url *resource;
	long key = -1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &str, &str_len, &key) == FAILURE) {
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
				if (resource->scheme != NULL) RETVAL_STRING(resource->scheme, 1);
				break;
			case PHP_URL_HOST:
				if (resource->host != NULL) RETVAL_STRING(resource->host, 1);
				break;
			case PHP_URL_PORT:
				if (resource->port != 0) RETVAL_LONG(resource->port);
				break;
			case PHP_URL_USER:
				if (resource->user != NULL) RETVAL_STRING(resource->user, 1);
				break;
			case PHP_URL_PASS:
				if (resource->pass != NULL) RETVAL_STRING(resource->pass, 1);
				break;
			case PHP_URL_PATH:
				if (resource->path != NULL) RETVAL_STRING(resource->path, 1);
				break;
			case PHP_URL_QUERY:
				if (resource->query != NULL) RETVAL_STRING(resource->query, 1);
				break;
			case PHP_URL_FRAGMENT:
				if (resource->fragment != NULL) RETVAL_STRING(resource->fragment, 1);
				break;
			default:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid URL component identifier %ld", key);
				RETVAL_FALSE;
		}
		goto done;
	}

	/* allocate an array for return */
	array_init(return_value);

    /* add the various elements to the array */
	if (resource->scheme != NULL)
		add_assoc_string(return_value, "scheme", resource->scheme, 1);
	if (resource->host != NULL)
		add_assoc_string(return_value, "host", resource->host, 1);
	if (resource->port != 0)
		add_assoc_long(return_value, "port", resource->port);
	if (resource->user != NULL)
		add_assoc_string(return_value, "user", resource->user, 1);
	if (resource->pass != NULL)
		add_assoc_string(return_value, "pass", resource->pass, 1);
	if (resource->path != NULL)
		add_assoc_string(return_value, "path", resource->path, 1);
	if (resource->query != NULL)
		add_assoc_string(return_value, "query", resource->query, 1);
	if (resource->fragment != NULL)
		add_assoc_string(return_value, "fragment", resource->fragment, 1);
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
PHPAPI char *php_url_encode(char const *s, int len, int *new_length)
{
	register unsigned char c;
	unsigned char *to, *start;
	unsigned char const *from, *end;
	
	from = (unsigned char *)s;
	end = (unsigned char *)s + len;
	start = to = (unsigned char *) safe_emalloc(3, len, 1);

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
	*to = 0;
	if (new_length) {
		*new_length = to - start;
	}
	return (char *) start;
}
/* }}} */

/* {{{ proto string urlencode(string str)
   URL-encodes string */
PHP_FUNCTION(urlencode)
{
	char *in_str, *out_str;
	int in_str_len, out_str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &in_str,
							  &in_str_len) == FAILURE) {
		return;
	}

	out_str = php_url_encode(in_str, in_str_len, &out_str_len);
	RETURN_STRINGL(out_str, out_str_len, 0);
}
/* }}} */

/* {{{ proto string urldecode(string str)
   Decodes URL-encoded string */
PHP_FUNCTION(urldecode)
{
	char *in_str, *out_str;
	int in_str_len, out_str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &in_str,
							  &in_str_len) == FAILURE) {
		return;
	}

	out_str = estrndup(in_str, in_str_len);
	out_str_len = php_url_decode(out_str, in_str_len);

    RETURN_STRINGL(out_str, out_str_len, 0);
}
/* }}} */

/* {{{ php_url_decode
 */
PHPAPI int php_url_decode(char *str, int len)
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
PHPAPI char *php_raw_url_encode(char const *s, int len, int *new_length)
{
	register int x, y;
	unsigned char *str;

	str = (unsigned char *) safe_emalloc(3, len, 1);
	for (x = 0, y = 0; len--; x++, y++) {
		str[y] = (unsigned char) s[x];
#ifndef CHARSET_EBCDIC
		if ((str[y] < '0' && str[y] != '-' && str[y] != '.') ||
			(str[y] < 'A' && str[y] > '9') ||
			(str[y] > 'Z' && str[y] < 'a' && str[y] != '_') ||
			(str[y] > 'z' && str[y] != '~')) {
			str[y++] = '%';
			str[y++] = hexchars[(unsigned char) s[x] >> 4];
			str[y] = hexchars[(unsigned char) s[x] & 15];
#else /*CHARSET_EBCDIC*/
		if (!isalnum(str[y]) && strchr("_-.~", str[y]) != NULL) {
			str[y++] = '%';
			str[y++] = hexchars[os_toascii[(unsigned char) s[x]] >> 4];
			str[y] = hexchars[os_toascii[(unsigned char) s[x]] & 15];
#endif /*CHARSET_EBCDIC*/
		}
	}
	str[y] = '\0';
	if (new_length) {
		*new_length = y;
	}
	return ((char *) str);
}
/* }}} */

/* {{{ proto string rawurlencode(string str)
   URL-encodes string */
PHP_FUNCTION(rawurlencode)
{
	char *in_str, *out_str;
	int in_str_len, out_str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &in_str,
							  &in_str_len) == FAILURE) {
		return;
	}

	out_str = php_raw_url_encode(in_str, in_str_len, &out_str_len);
	RETURN_STRINGL(out_str, out_str_len, 0);
}
/* }}} */

/* {{{ proto string rawurldecode(string str)
   Decodes URL-encodes string */
PHP_FUNCTION(rawurldecode)
{
	char *in_str, *out_str;
	int in_str_len, out_str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &in_str,
							  &in_str_len) == FAILURE) {
		return;
	}

	out_str = estrndup(in_str, in_str_len);
	out_str_len = php_raw_url_decode(out_str, in_str_len);

    RETURN_STRINGL(out_str, out_str_len, 0);
}
/* }}} */

/* {{{ php_raw_url_decode
 */
PHPAPI int php_raw_url_decode(char *str, int len)
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
	int url_len;
	php_stream_context *context;
	php_stream *stream;
	zval **prev_val, **hdr = NULL, **h;
	HashPosition pos;
	HashTable *hashT;
	long format = 0;
                
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &url, &url_len, &format) == FAILURE) {
		return;
	}
	context = FG(default_context) ? FG(default_context) : (FG(default_context) = php_stream_context_alloc(TSRMLS_C));

	if (!(stream = php_stream_open_wrapper_ex(url, "r", REPORT_ERRORS | STREAM_USE_URL | STREAM_ONLY_GET_HEADERS, NULL, context))) {
		RETURN_FALSE;
	}

	if (!stream->wrapperdata || Z_TYPE_P(stream->wrapperdata) != IS_ARRAY) {
		php_stream_close(stream);
		RETURN_FALSE;
	}

	array_init(return_value);

	/* check for curl-wrappers that provide headers via a special "headers" element */
	if (zend_hash_find(HASH_OF(stream->wrapperdata), "headers", sizeof("headers"), (void **)&h) != FAILURE && Z_TYPE_PP(h) == IS_ARRAY) {
		/* curl-wrappers don't load data until the 1st read */ 
		if (!Z_ARRVAL_PP(h)->nNumOfElements) {
			php_stream_getc(stream);
		}
		zend_hash_find(HASH_OF(stream->wrapperdata), "headers", sizeof("headers"), (void **)&h);
		hashT = Z_ARRVAL_PP(h);	
	} else {
		hashT = HASH_OF(stream->wrapperdata);
	}

	zend_hash_internal_pointer_reset_ex(hashT, &pos);
	while (zend_hash_get_current_data_ex(hashT, (void**)&hdr, &pos) != FAILURE) {
		if (!hdr || Z_TYPE_PP(hdr) != IS_STRING) {
			zend_hash_move_forward_ex(hashT, &pos);
			continue;
		}
		if (!format) {
no_name_header:
			add_next_index_stringl(return_value, Z_STRVAL_PP(hdr), Z_STRLEN_PP(hdr), 1);
		} else {
			char c;
			char *s, *p;

			if ((p = strchr(Z_STRVAL_PP(hdr), ':'))) {
				c = *p;
				*p = '\0';
				s = p + 1;
				while (isspace((int)*(unsigned char *)s)) {
					s++;
				}

				if (zend_hash_find(HASH_OF(return_value), Z_STRVAL_PP(hdr), (p - Z_STRVAL_PP(hdr) + 1), (void **) &prev_val) == FAILURE) {
					add_assoc_stringl_ex(return_value, Z_STRVAL_PP(hdr), (p - Z_STRVAL_PP(hdr) + 1), s, (Z_STRLEN_PP(hdr) - (s - Z_STRVAL_PP(hdr))), 1);
				} else { /* some headers may occur more then once, therefor we need to remake the string into an array */
					convert_to_array(*prev_val);
					add_next_index_stringl(*prev_val, s, (Z_STRLEN_PP(hdr) - (s - Z_STRVAL_PP(hdr))), 1);
				}

				*p = c;
			} else {
				goto no_name_header;
			}
		}
		zend_hash_move_forward_ex(hashT, &pos);
	}

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
