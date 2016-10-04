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
   | Author: David Walker (dave@mudsite.com)                              |
   +----------------------------------------------------------------------+
 */

/**
 * Url Parser used from https://github.com/staskobzar/url_parser_re2c
 * Licensed under MIT License.
 *
 * Modified for more correct RFC 3986 requirements & PHP
 */

#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>

#include "php.h"
#include "url.h"
#include "url_parser_ex.h"

ZEND_DECLARE_MODULE_GLOBALS(url)

/* Since YYMARKER increments on wildcards, we'll track ourselves. */
const char *marker;
const char *YYMARKER;

char* get_token(int len)
{
    char *res = (char *) emalloc (len + 1);
    int i = 0;
    for (; i < len; i++) res[i] = marker[i];
    res[i] = '\0';
    return res;
}

/*!re2c
    re2c:define:YYCTYPE = "unsigned char";
    re2c:define:YYCURSOR = URLG(url_str);
    re2c:yyfill:enable = 0;

    EOF = "\x00";
    ALPHA = [a-zA-Z];
    DIGIT = [0-9];
    HEXDIG = [0-9a-fA-F];
    SUB_DELIMS = [!$&'()*+,;=];
    GEN_DELIMS = [:/?#\[\]@];
    RESERVED = GEN_DELIMS | SUB_DELIMS;
    UNRESERVED = ALPHA | DIGIT | [-._~];
    PCT_ENCODED = "%" HEXDIG HEXDIG;
    PCHAR = UNRESERVED | PCT_ENCODED | SUB_DELIMS | ":" | "@";
*/

void url_parse_scheme(php_url *url)
{
    marker = URLG(url_str);
/*!re2c
    SCHEME = ALPHA (ALPHA | DIGIT | [+-.])*;

    EOF { URLG(url_str)--; return; }
    "" { return; }
    SCHEME ":" {
        int len = URLG(url_str) - marker - 1;
        url->scheme = get_token(len);
        return;
    }
*/
}

void url_parse_authority_userinfo (php_url *url)
{
    marker = URLG(url_str);
/*!re2c
    USERINFO = (UNRESERVED | PCT_ENCODED | SUB_DELIMS | ":")*;

    EOF { URLG(url_str)--; return; }
    "" { return; }
    USERINFO "@" {
        int len = 0;
        len = URLG(url_str) - marker;

        char* split = memchr(marker, ':', len);
        if (split && split < URLG(url_str)) {
            len = split - marker;
            if (len > 0) url->user = get_token(len);
            marker += len + 1;
            len = URLG(url_str) - split - 2;
            if (len > 0) url->pass = get_token(len);
            return;
        } else {
            int len = URLG(url_str) - marker - 1;
            if (len > 0) url->user = get_token(len);
            return;
        }
    }
*/
}

void url_parse_authority_host (php_url *url)
{
    marker = URLG(url_str);
/*!re2c
    DEC_OCTET = DIGIT | [1-9] DIGIT | "1" DIGIT{2} | "2" [0-4] DIGIT | "25" [0-5];
    IPV4ADDR = (DEC_OCTET "."){3} DEC_OCTET;
    H16 = HEXDIG{1,4};
    LS32 = (H16 ":" H16) | IPV4ADDR;
    IPV6ADDR = "[" (
        (H16 ":"){7,7} H16|
        (H16 ":" ){1,7} ":" |
        (H16 ":" ){1,6} ":" H16|
        (H16 ":" ){1,5}( ":" H16){1,2}|
        (H16 ":" ){1,4}( ":" H16){1,3}|
        (H16 ":" ){1,3}( ":" H16){1,4}|
        (H16 ":" ){1,2}( ":" H16){1,5}|
        H16 ":" (( ":" H16){1,6})|
        ":" (( ":" H16){1,7}| ":" )|
        "fe80:" ( ":" H16){0,4} "%" [0-9a-zA-Z]{1,}|
        "::" ( "ffff" ( ":0" {1,4}){0,1} ":" ){0,1}IPV4ADDR|
        (H16 ":"){1,4} ":" IPV4ADDR
    ) "]";
    IPVFUTURE = "[v" HEXDIG+ "." (UNRESERVED | SUB_DELIMS | ":")+ "]";
    REG_NAME = (UNRESERVED | PCT_ENCODED | SUB_DELIMS)+;

    EOF { URLG(url_str) = marker; return; }
    "" { return; }
    IPV4ADDR | REG_NAME {
        goto host;
    }
    IPV6ADDR | IPVFUTURE {
        goto host;
    }
*/
host:;
    int len = URLG(url_str) - marker;
    url->host = get_token(len);
    return;
}

void url_parse_authority_port (php_url *url)
{
    marker = URLG(url_str);
/*!re2c

    EOF { URLG(url_str)--; return; }
    "" { return; }
    ":" DIGIT* {
        marker++;
        int len = URLG(url_str) - marker;
        char* port = get_token(len);
        url->port = atoi(port);
        efree(port);
        return;
    }
*/
}

void url_parse_authority (php_url *url)
{
/*!re2c
    EOF { URLG(url_str)--; return; }
    "" { return; }
    "//" {
        goto authority;
    }
*/
authority:;
    url_parse_authority_userinfo(url);
    url_parse_authority_host(url);
    url_parse_authority_port(url);
    return;
}

void url_parse_path (php_url *url)
{
    marker = URLG(url_str);
/*!re2c
    SEGMENT_NZ_NC = (UNRESERVED | PCT_ENCODED | SUB_DELIMS | "@")+;
    SEGMENT_NZ = PCHAR+;
    SEGMENT = PCHAR*;

    PATH_ABEMPTY = ("/" SEGMENT)*;
    PATH_ABSOLUTE = "/" (SEGMENT_NZ ("/" SEGMENT)*)?;
    PATH_NOSCHEME = SEGMENT_NZ_NC ("/" SEGMENT)*;
    PATH_ROOTLESS = SEGMENT_NZ ("/" SEGMENT)*;
    PATH_EMPTY = "";
    PATH = PATH_ABEMPTY | PATH_ABSOLUTE | PATH_NOSCHEME | PATH_ROOTLESS | PATH_EMPTY;

    EOF { URLG(url_str)--; return; }
    PATH {
        int len = URLG(url_str) - marker;

        if (len) {
            url->path = get_token(len);
        }

        return;
    }
*/
}

void url_parse_query_frag (php_url *url)
{
query_frag:;
    marker = URLG(url_str);

/*!re2c
    QUERY = "?" (PCHAR | "/" | "?")*;
    FRAGMENT = "#" (PCHAR | "/" | "?")*;

    EOF { URLG(url_str)--; return; }
    "" { return; }
    QUERY {
        marker++;
        int len = URLG(url_str) - marker;
        if (len) {
            url->query = get_token(len);
        }
        goto query_frag;
    }
    FRAGMENT {
        marker++;
        int len = URLG(url_str) - marker;
        if (len) {
            url->fragment = get_token(len);
        }
        return;
    }
*/
}
