/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
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
PHPAPI char *php_replace_controlchars(char *str)
{
	unsigned char *s = (unsigned char *)str;
	
	if (!str) {
		return (NULL);
	}
	
	while (*s) {
	    
		if (iscntrl(*s)) {
			*s='_';
		}	
		s++;
	}
	
	return (str);
} 
/* }}} */
 

/* {{{ php_url_parse
 */
PHPAPI php_url *php_url_parse(char *str)
{
	regex_t re;
	regmatch_t subs[11];
	int err;
	int length = strlen(str);
	char *result;
	php_url *ret = ecalloc(1, sizeof(php_url));

	/* from Appendix B of draft-fielding-url-syntax-09,
	   http://www.ics.uci.edu/~fielding/url/url.txt */
	err = regcomp(&re, "^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\\?([^#]*))?(#(.*))?", REG_EXTENDED);
	if (err) {
		/*php_error(E_WARNING, "Unable to compile regex: %d\n", err);*/
		efree(ret);
		return NULL;
	}
	err = regexec(&re, str, 10, subs, 0);
	if (err) {
		/*php_error(E_WARNING, "Error with regex\n");*/
		efree(ret);
		regfree(&re);
		return NULL;
	}
	/* no processing necessary on the scheme */
	if (subs[2].rm_so != -1 && subs[2].rm_so <= length) {
		ret->scheme = estrndup(str + subs[2].rm_so, subs[2].rm_eo - subs[2].rm_so);
		php_replace_controlchars(ret->scheme);
	}

	/* the path to the resource */
	if (subs[5].rm_so != -1 && subs[5].rm_so <= length) {
		ret->path = estrndup(str + subs[5].rm_so, subs[5].rm_eo - subs[5].rm_so);
		php_replace_controlchars(ret->path);
	}

	/* the query part */
	if (subs[7].rm_so != -1 && subs[7].rm_so <= length) {
		ret->query = estrndup(str + subs[7].rm_so, subs[7].rm_eo - subs[7].rm_so);
		php_replace_controlchars(ret->query);
	}

	/* the fragment */
	if (subs[9].rm_so != -1 && subs[9].rm_so <= length) {
		ret->fragment = estrndup(str + subs[9].rm_so, subs[9].rm_eo - subs[9].rm_so);
		php_replace_controlchars(ret->fragment);
	}

	/* extract the username, pass, and port from the hostname */
	if (subs[4].rm_so != -1 && subs[4].rm_so <= length) {

		int cerr;
		/* extract username:pass@host:port from regex results */
		result = estrndup(str + subs[4].rm_so, subs[4].rm_eo - subs[4].rm_so);
		length = strlen(result);

		regfree(&re);			/* free the old regex */
		
		if (length) {
			if ((cerr=regcomp(&re, "^(([^@:]+)(:([^@:]+))?@)?((\\[([^]]+)\\])|([^:@]+))(:([^:@]+))?", REG_EXTENDED))
				|| (err=regexec(&re, result, 11, subs, 0))) {
				STR_FREE(ret->scheme);
				STR_FREE(ret->path);
				STR_FREE(ret->query);
				STR_FREE(ret->fragment);
				efree(ret);
				efree(result);
				/*php_error(E_WARNING, "Unable to compile regex: %d\n", err);*/
				if (!cerr) regfree(&re); 
				return NULL;
			}
			/* now deal with all of the results */
			if (subs[2].rm_so != -1 && subs[2].rm_so < length) {
				ret->user = estrndup(result + subs[2].rm_so, subs[2].rm_eo - subs[2].rm_so);
				php_replace_controlchars(ret->user);
			}
			if (subs[4].rm_so != -1 && subs[4].rm_so < length) {
				ret->pass = estrndup(result + subs[4].rm_so, subs[4].rm_eo - subs[4].rm_so);
				php_replace_controlchars(ret->pass);
			}
			if (subs[7].rm_so != -1 && subs[7].rm_so < length) {
				ret->host = estrndup(result + subs[7].rm_so, subs[7].rm_eo - subs[7].rm_so);
				php_replace_controlchars(ret->host);
			} else if (subs[8].rm_so != -1 && subs[8].rm_so < length) {
				ret->host = estrndup(result + subs[8].rm_so, subs[8].rm_eo - subs[8].rm_so);
				php_replace_controlchars(ret->host);
			}
			if (subs[10].rm_so != -1 && subs[10].rm_so < length) {
				ret->port = (unsigned short) strtol(result + subs[10].rm_so, NULL, 10);
			}
		}
		efree(result);
	}
	else if (ret->scheme && !strcmp(ret->scheme, "http")) {
		STR_FREE(ret->scheme);
		STR_FREE(ret->path);
		STR_FREE(ret->query);
		STR_FREE(ret->fragment);
		efree(ret);
		regfree(&re);
		return NULL;
	}
	regfree(&re);
	return ret;
}
/* }}} */

/* {{{ proto array parse_url(string url)
   Parse a URL and return its components */
PHP_FUNCTION(parse_url)
{
	char *str;
	int str_len;
	php_url *resource;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		return;
	}

	resource = php_url_parse(str);
	if (resource == NULL) {
		php_error(E_WARNING, "unable to parse url (%s)", str);
		RETURN_FALSE;
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
	
    php_url_free(resource);
}
/* }}} */

/* {{{ php_htoi
 */
static int php_htoi(char *s)
{
	int value;
	int c;

	c = s[0];
	if (isupper(c))
		c = tolower(c);
	value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;

	c = s[1];
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
PHPAPI char *php_url_encode(char *s, int len, int *new_length)
{
	register int x, y;
	unsigned char *str;

	str = (unsigned char *) emalloc(3 * len + 1);
	for (x = 0, y = 0; len--; x++, y++) {
		str[y] = (unsigned char) s[x];
		if (str[y] == ' ') {
			str[y] = '+';
#ifndef CHARSET_EBCDIC
		} else if ((str[y] < '0' && str[y] != '-' && str[y] != '.') ||
				   (str[y] < 'A' && str[y] > '9') ||
				   (str[y] > 'Z' && str[y] < 'a' && str[y] != '_') ||
				   (str[y] > 'z')) {
			str[y++] = '%';
			str[y++] = hexchars[(unsigned char) s[x] >> 4];
			str[y] = hexchars[(unsigned char) s[x] & 15];
		}
#else /*CHARSET_EBCDIC*/
		} else if (!isalnum(str[y]) && strchr("_-.", str[y]) == NULL) {
			/* Allow only alphanumeric chars and '_', '-', '.'; escape the rest */
			str[y++] = '%';
			str[y++] = hexchars[os_toascii[(unsigned char) s[x]] >> 4];
			str[y] = hexchars[os_toascii[(unsigned char) s[x]] & 0x0F];
		}
#endif /*CHARSET_EBCDIC*/
	}
	str[y] = '\0';
	if (new_length) {
		*new_length = y;
	}
	return ((char *) str);
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
		if (*data == '+')
			*dest = ' ';
		else if (*data == '%' && len >= 2 && isxdigit((int) *(data + 1)) && isxdigit((int) *(data + 2))) {
#ifndef CHARSET_EBCDIC
			*dest = (char) php_htoi(data + 1);
#else
			*dest = os_toebcdic[(char) php_htoi(data + 1)];
#endif
			data += 2;
			len -= 2;
		} else
			*dest = *data;
		data++;
		dest++;
	}
	*dest = '\0';
	return dest - str;
}
/* }}} */

/* {{{ php_raw_url_encode
 */
PHPAPI char *php_raw_url_encode(char *s, int len, int *new_length)
{
	register int x, y;
	unsigned char *str;

	str = (unsigned char *) emalloc(3 * len + 1);
	for (x = 0, y = 0; len--; x++, y++) {
		str[y] = (unsigned char) s[x];
#ifndef CHARSET_EBCDIC
		if ((str[y] < '0' && str[y] != '-' && str[y] != '.') ||
			(str[y] < 'A' && str[y] > '9') ||
			(str[y] > 'Z' && str[y] < 'a' && str[y] != '_') ||
			(str[y] > 'z')) {
			str[y++] = '%';
			str[y++] = hexchars[(unsigned char) s[x] >> 4];
			str[y] = hexchars[(unsigned char) s[x] & 15];
#else /*CHARSET_EBCDIC*/
		if (!isalnum(str[y]) && strchr("_-.", str[y]) != NULL) {
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
		if (*data == '%' && len >= 2 && isxdigit((int) *(data + 1)) && isxdigit((int) *(data + 2))) {
#ifndef CHARSET_EBCDIC
			*dest = (char) php_htoi(data + 1);
#else
			*dest = os_toebcdic[(char) php_htoi(data + 1)];
#endif
			data += 2;
			len -= 2;
		} else
			*dest = *data;
		data++;
		dest++;
	}
	*dest = '\0';
	return dest - str;
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
