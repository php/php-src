/* Copyright 2009,2010 Ryan Dahl <ry@tinyclouds.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
/* modified by Moriyoshi Koizumi <moriyoshi@php.net> to make it fit to PHP source tree. */
#ifndef php_http_parser_h
#define php_http_parser_h
#ifdef __cplusplus
extern "C" {
#endif


#include <sys/types.h>
#if defined(_WIN32) && !defined(__MINGW32__)
# include <windows.h>
# include "config.w32.h"
#else
# include <php_config.h>
#endif

#include <stdint.h>

/* Compile with -DPHP_HTTP_PARSER_STRICT=0 to make less checks, but run
 * faster
 */
#ifndef PHP_HTTP_PARSER_STRICT
# define PHP_HTTP_PARSER_STRICT 1
#else
# define PHP_HTTP_PARSER_STRICT 0
#endif


/* Maximum header size allowed */
#define PHP_HTTP_MAX_HEADER_SIZE (80*1024)


typedef struct php_http_parser php_http_parser;
typedef struct php_http_parser_settings php_http_parser_settings;


/* Callbacks should return non-zero to indicate an error. The parser will
 * then halt execution.
 *
 * The one exception is on_headers_complete. In a PHP_HTTP_RESPONSE parser
 * returning '1' from on_headers_complete will tell the parser that it
 * should not expect a body. This is used when receiving a response to a
 * HEAD request which may contain 'Content-Length' or 'Transfer-Encoding:
 * chunked' headers that indicate the presence of a body.
 *
 * http_data_cb does not return data chunks. It will be call arbitrarily
 * many times for each string. E.G. you might get 10 callbacks for "on_path"
 * each providing just a few characters more data.
 */
typedef int (*php_http_data_cb) (php_http_parser*, const char *at, size_t length);
typedef int (*php_http_cb) (php_http_parser*);


/* Request Methods */
enum php_http_method
  { PHP_HTTP_DELETE    = 0
  , PHP_HTTP_GET
  , PHP_HTTP_HEAD
  , PHP_HTTP_POST
  , PHP_HTTP_PUT
  , PHP_HTTP_PATCH
  /* pathological */
  , PHP_HTTP_CONNECT
  , PHP_HTTP_OPTIONS
  , PHP_HTTP_TRACE
  /* webdav */
  , PHP_HTTP_COPY
  , PHP_HTTP_LOCK
  , PHP_HTTP_MKCOL
  , PHP_HTTP_MOVE
  , PHP_HTTP_MKCALENDAR
  , PHP_HTTP_PROPFIND
  , PHP_HTTP_PROPPATCH
  , PHP_HTTP_SEARCH
  , PHP_HTTP_UNLOCK
  /* subversion */
  , PHP_HTTP_REPORT
  , PHP_HTTP_MKACTIVITY
  , PHP_HTTP_CHECKOUT
  , PHP_HTTP_MERGE
  /* upnp */
  , PHP_HTTP_MSEARCH
  , PHP_HTTP_NOTIFY
  , PHP_HTTP_SUBSCRIBE
  , PHP_HTTP_UNSUBSCRIBE
  /* unknown, not implemented */
  , PHP_HTTP_NOT_IMPLEMENTED
  };


enum php_http_parser_type { PHP_HTTP_REQUEST, PHP_HTTP_RESPONSE, PHP_HTTP_BOTH };

enum state
  { s_dead = 1 /* important that this is > 0 */

  , s_start_req_or_res
  , s_res_or_resp_H
  , s_start_res
  , s_res_H
  , s_res_HT
  , s_res_HTT
  , s_res_HTTP
  , s_res_first_http_major
  , s_res_http_major
  , s_res_first_http_minor
  , s_res_http_minor
  , s_res_first_status_code
  , s_res_status_code
  , s_res_status
  , s_res_line_almost_done

  , s_start_req

  , s_req_method
  , s_req_spaces_before_url
  , s_req_schema
  , s_req_schema_slash
  , s_req_schema_slash_slash
  , s_req_host
  , s_req_port
  , s_req_path
  , s_req_query_string_start
  , s_req_query_string
  , s_req_fragment_start
  , s_req_fragment
  , s_req_http_start
  , s_req_http_H
  , s_req_http_HT
  , s_req_http_HTT
  , s_req_http_HTTP
  , s_req_first_http_major
  , s_req_http_major
  , s_req_first_http_minor
  , s_req_http_minor
  , s_req_line_almost_done

  , s_header_field_start
  , s_header_field
  , s_header_value_start
  , s_header_value

  , s_header_almost_done

  , s_headers_almost_done
  /* Important: 's_headers_almost_done' must be the last 'header' state. All
   * states beyond this must be 'body' states. It is used for overflow
   * checking. See the PARSING_HEADER() macro.
   */
  , s_chunk_size_start
  , s_chunk_size
  , s_chunk_size_almost_done
  , s_chunk_parameters
  , s_chunk_data
  , s_chunk_data_almost_done
  , s_chunk_data_done

  , s_body_identity
  , s_body_identity_eof
  };

struct php_http_parser {
  /** PRIVATE **/
  unsigned char type : 2;
  unsigned char flags : 6;
  unsigned char state;
  unsigned char header_state;
  unsigned char index;

  uint32_t nread;
  ssize_t  content_length;

  /** READ-ONLY **/
  unsigned short http_major;
  unsigned short http_minor;
  unsigned short status_code; /* responses only */
  unsigned char method;    /* requests only */

  /* 1 = Upgrade header was present and the parser has exited because of that.
   * 0 = No upgrade header present.
   * Should be checked when http_parser_execute() returns in addition to
   * error checking.
   */
  char upgrade;

  /** PUBLIC **/
  void *data; /* A pointer to get hook to the "connection" or "socket" object */
};


struct php_http_parser_settings {
  php_http_cb      on_message_begin;
  php_http_data_cb on_path;
  php_http_data_cb on_query_string;
  php_http_data_cb on_url;
  php_http_data_cb on_fragment;
  php_http_data_cb on_header_field;
  php_http_data_cb on_header_value;
  php_http_cb      on_headers_complete;
  php_http_data_cb on_body;
  php_http_cb      on_message_complete;
};


void php_http_parser_init(php_http_parser *parser, enum php_http_parser_type type);


size_t php_http_parser_execute(php_http_parser *parser,
                           const php_http_parser_settings *settings,
                           const char *data,
                           size_t len);


/* If php_http_should_keep_alive() in the on_headers_complete or
 * on_message_complete callback returns true, then this will be should be
 * the last message on the connection.
 * If you are the server, respond with the "Connection: close" header.
 * If you are the client, close the connection.
 */
int php_http_should_keep_alive(php_http_parser *parser);

/* Returns a string version of the HTTP method. */
const char *php_http_method_str(enum php_http_method);

#ifdef __cplusplus
}
#endif
#endif
