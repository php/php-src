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
#include <assert.h>
#include <stddef.h>
#include "php_http_parser.h"


#ifndef MIN
# define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif


#define CALLBACK2(FOR)                                               \
do {                                                                 \
  if (settings->on_##FOR) {                                          \
    if (0 != settings->on_##FOR(parser)) return (p - data);          \
  }                                                                  \
} while (0)


#define MARK(FOR)                                                    \
do {                                                                 \
  FOR##_mark = p;                                                    \
} while (0)

#define CALLBACK_NOCLEAR(FOR)                                        \
do {                                                                 \
  if (FOR##_mark) {                                                  \
    if (settings->on_##FOR) {                                        \
      if (0 != settings->on_##FOR(parser,                            \
                                 FOR##_mark,                         \
                                 p - FOR##_mark))                    \
      {                                                              \
        return (p - data);                                           \
      }                                                              \
    }                                                                \
  }                                                                  \
} while (0)

#ifdef PHP_WIN32
# undef CALLBACK
#endif
#define CALLBACK(FOR)                                                \
do {                                                                 \
  CALLBACK_NOCLEAR(FOR);                                             \
  FOR##_mark = NULL;                                                 \
} while (0)


#define PROXY_CONNECTION "proxy-connection"
#define CONNECTION "connection"
#define CONTENT_LENGTH "content-length"
#define TRANSFER_ENCODING "transfer-encoding"
#define UPGRADE "upgrade"
#define CHUNKED "chunked"
#define KEEP_ALIVE "keep-alive"
#define CLOSE "close"


static const char *method_strings[] =
  { "DELETE"
  , "GET"
  , "HEAD"
  , "POST"
  , "PUT"
  , "PATCH"
  , "CONNECT"
  , "OPTIONS"
  , "TRACE"
  , "COPY"
  , "LOCK"
  , "MKCOL"
  , "MOVE"
  , "MKCALENDAR"
  , "PROPFIND"
  , "PROPPATCH"
  , "SEARCH"
  , "UNLOCK"
  , "REPORT"
  , "MKACTIVITY"
  , "CHECKOUT"
  , "MERGE"
  , "M-SEARCH"
  , "NOTIFY"
  , "SUBSCRIBE"
  , "UNSUBSCRIBE"
  , "NOTIMPLEMENTED"
  };


/* Tokens as defined by rfc 2616. Also lowercases them.
 *        token       = 1*<any CHAR except CTLs or separators>
 *     separators     = "(" | ")" | "<" | ">" | "@"
 *                    | "," | ";" | ":" | "\" | <">
 *                    | "/" | "[" | "]" | "?" | "="
 *                    | "{" | "}" | SP | HT
 */
static const char tokens[256] = {
/*   0 nul    1 soh    2 stx    3 etx    4 eot    5 enq    6 ack    7 bel  */
        0,       0,       0,       0,       0,       0,       0,       0,
/*   8 bs     9 ht    10 nl    11 vt    12 np    13 cr    14 so    15 si   */
        0,       0,       0,       0,       0,       0,       0,       0,
/*  16 dle   17 dc1   18 dc2   19 dc3   20 dc4   21 nak   22 syn   23 etb */
        0,       0,       0,       0,       0,       0,       0,       0,
/*  24 can   25 em    26 sub   27 esc   28 fs    29 gs    30 rs    31 us  */
        0,       0,       0,       0,       0,       0,       0,       0,
/*  32 sp    33  !    34  "    35  #    36  $    37  %    38  &    39  '  */
       ' ',      '!',     '"',     '#',     '$',     '%',     '&',    '\'',
/*  40  (    41  )    42  *    43  +    44  ,    45  -    46  .    47  /  */
        0,       0,      '*',     '+',      0,      '-',     '.',     '/',
/*  48  0    49  1    50  2    51  3    52  4    53  5    54  6    55  7  */
       '0',     '1',     '2',     '3',     '4',     '5',     '6',     '7',
/*  56  8    57  9    58  :    59  ;    60  <    61  =    62  >    63  ?  */
       '8',     '9',      0,       0,       0,       0,       0,       0,
/*  64  @    65  A    66  B    67  C    68  D    69  E    70  F    71  G  */
        0,      'a',     'b',     'c',     'd',     'e',     'f',     'g',
/*  72  H    73  I    74  J    75  K    76  L    77  M    78  N    79  O  */
       'h',     'i',     'j',     'k',     'l',     'm',     'n',     'o',
/*  80  P    81  Q    82  R    83  S    84  T    85  U    86  V    87  W  */
       'p',     'q',     'r',     's',     't',     'u',     'v',     'w',
/*  88  X    89  Y    90  Z    91  [    92  \    93  ]    94  ^    95  _  */
       'x',     'y',     'z',      0,       0,       0,      '^',     '_',
/*  96  `    97  a    98  b    99  c   100  d   101  e   102  f   103  g  */
       '`',     'a',     'b',     'c',     'd',     'e',     'f',     'g',
/* 104  h   105  i   106  j   107  k   108  l   109  m   110  n   111  o  */
       'h',     'i',     'j',     'k',     'l',     'm',     'n',     'o',
/* 112  p   113  q   114  r   115  s   116  t   117  u   118  v   119  w  */
       'p',     'q',     'r',     's',     't',     'u',     'v',     'w',
/* 120  x   121  y   122  z   123  {   124  |   125  }   126  ~   127 del */
       'x',     'y',     'z',      0,      '|',     '}',     '~',       0 };


static const int8_t unhex[256] =
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
  ,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
  ,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
  , 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,-1,-1,-1,-1,-1,-1
  ,-1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1
  ,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
  ,-1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1
  ,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
  };


static const uint8_t normal_url_char[256] = {
/*   0 nul    1 soh    2 stx    3 etx    4 eot    5 enq    6 ack    7 bel  */
        0,       0,       0,       0,       0,       0,       0,       0,
/*   8 bs     9 ht    10 nl    11 vt    12 np    13 cr    14 so    15 si   */
        0,       0,       0,       0,       0,       0,       0,       0,
/*  16 dle   17 dc1   18 dc2   19 dc3   20 dc4   21 nak   22 syn   23 etb */
        0,       0,       0,       0,       0,       0,       0,       0,
/*  24 can   25 em    26 sub   27 esc   28 fs    29 gs    30 rs    31 us  */
        0,       0,       0,       0,       0,       0,       0,       0,
/*  32 sp    33  !    34  "    35  #    36  $    37  %    38  &    39  '  */
        0,       1,       1,       0,       1,       1,       1,       1,
/*  40  (    41  )    42  *    43  +    44  ,    45  -    46  .    47  /  */
        1,       1,       1,       1,       1,       1,       1,       1,
/*  48  0    49  1    50  2    51  3    52  4    53  5    54  6    55  7  */
        1,       1,       1,       1,       1,       1,       1,       1,
/*  56  8    57  9    58  :    59  ;    60  <    61  =    62  >    63  ?  */
        1,       1,       1,       1,       1,       1,       1,       0,
/*  64  @    65  A    66  B    67  C    68  D    69  E    70  F    71  G  */
        1,       1,       1,       1,       1,       1,       1,       1,
/*  72  H    73  I    74  J    75  K    76  L    77  M    78  N    79  O  */
        1,       1,       1,       1,       1,       1,       1,       1,
/*  80  P    81  Q    82  R    83  S    84  T    85  U    86  V    87  W  */
        1,       1,       1,       1,       1,       1,       1,       1,
/*  88  X    89  Y    90  Z    91  [    92  \    93  ]    94  ^    95  _  */
        1,       1,       1,       1,       1,       1,       1,       1,
/*  96  `    97  a    98  b    99  c   100  d   101  e   102  f   103  g  */
        1,       1,       1,       1,       1,       1,       1,       1,
/* 104  h   105  i   106  j   107  k   108  l   109  m   110  n   111  o  */
        1,       1,       1,       1,       1,       1,       1,       1,
/* 112  p   113  q   114  r   115  s   116  t   117  u   118  v   119  w  */
        1,       1,       1,       1,       1,       1,       1,       1,
/* 120  x   121  y   122  z   123  {   124  |   125  }   126  ~   127 del */
        1,       1,       1,       1,       1,       1,       1,       0 };


#define PARSING_HEADER(state) (state <= s_headers_almost_done && 0 == (parser->flags & F_TRAILING))


enum header_states
  { h_general = 0
  , h_C
  , h_CO
  , h_CON

  , h_matching_connection
  , h_matching_proxy_connection
  , h_matching_content_length
  , h_matching_transfer_encoding
  , h_matching_upgrade

  , h_connection
  , h_content_length
  , h_transfer_encoding
  , h_upgrade

  , h_matching_transfer_encoding_chunked
  , h_matching_connection_keep_alive
  , h_matching_connection_close

  , h_transfer_encoding_chunked
  , h_connection_keep_alive
  , h_connection_close
  };


enum flags
  { F_CHUNKED               = 1 << 0
  , F_CONNECTION_KEEP_ALIVE = 1 << 1
  , F_CONNECTION_CLOSE      = 1 << 2
  , F_TRAILING              = 1 << 3
  , F_UPGRADE               = 1 << 4
  , F_SKIPBODY              = 1 << 5
  };


#define CR '\r'
#define LF '\n'
#define LOWER(c) (unsigned char)(c | 0x20)
#define TOKEN(c) tokens[(unsigned char)c]


#define start_state (parser->type == PHP_HTTP_REQUEST ? s_start_req : s_start_res)


#ifdef HTTP_PARSER_STRICT
# define STRICT_CHECK(cond) if (cond) goto error
# define NEW_MESSAGE() (http_should_keep_alive(parser) ? start_state : s_dead)
#else
# define STRICT_CHECK(cond)
# define NEW_MESSAGE() start_state
#endif


size_t php_http_parser_execute (php_http_parser *parser,
                            const php_http_parser_settings *settings,
                            const char *data,
                            size_t len)
{
  char ch;
  signed char c;
  const char *p = data, *pe;
  size_t to_read;

  enum state state = (enum state) parser->state;
  enum header_states header_state = (enum header_states) parser->header_state;
  uint32_t index = parser->index;
  uint32_t nread = parser->nread;

  /* technically we could combine all of these (except for url_mark) into one
     variable, saving stack space, but it seems more clear to have them
     separated. */
  const char *header_field_mark = 0;
  const char *header_value_mark = 0;
  const char *fragment_mark = 0;
  const char *query_string_mark = 0;
  const char *path_mark = 0;
  const char *url_mark = 0;

  if (len == 0) {
    if (state == s_body_identity_eof) {
      CALLBACK2(message_complete);
    }
    return 0;
  }

  if (state == s_header_field)
    header_field_mark = data;
  if (state == s_header_value)
    header_value_mark = data;
  if (state == s_req_fragment)
    fragment_mark = data;
  if (state == s_req_query_string)
    query_string_mark = data;
  if (state == s_req_path)
    path_mark = data;
  if (state == s_req_path || state == s_req_schema || state == s_req_schema_slash
      || state == s_req_schema_slash_slash || state == s_req_port
      || state == s_req_query_string_start || state == s_req_query_string
      || state == s_req_host
      || state == s_req_fragment_start || state == s_req_fragment)
    url_mark = data;

  for (p=data, pe=data+len; p != pe; p++) {
    ch = *p;

    if (PARSING_HEADER(state)) {
      ++nread;
      /* Buffer overflow attack */
      if (nread > PHP_HTTP_MAX_HEADER_SIZE) goto error;
    }

    switch (state) {

      case s_dead:
        /* this state is used after a 'Connection: close' message
         * the parser will error out if it reads another message
         */
        goto error;

      case s_start_req_or_res:
      {
        if (ch == CR || ch == LF)
          break;
        parser->flags = 0;
        parser->content_length = -1;

        CALLBACK2(message_begin);

        if (ch == 'H')
          state = s_res_or_resp_H;
        else {
          parser->type = PHP_HTTP_REQUEST;
          goto start_req_method_assign;
        }
        break;
      }

      case s_res_or_resp_H:
        if (ch == 'T') {
          parser->type = PHP_HTTP_RESPONSE;
          state = s_res_HT;
        } else {
          if (ch != 'E') goto error;
          parser->type = PHP_HTTP_REQUEST;
          parser->method = PHP_HTTP_HEAD;
          index = 2;
          state = s_req_method;
        }
        break;

      case s_start_res:
      {
        parser->flags = 0;
        parser->content_length = -1;

        CALLBACK2(message_begin);

        switch (ch) {
          case 'H':
            state = s_res_H;
            break;

          case CR:
          case LF:
            break;

          default:
            goto error;
        }
        break;
      }

      case s_res_H:
        STRICT_CHECK(ch != 'T');
        state = s_res_HT;
        break;

      case s_res_HT:
        STRICT_CHECK(ch != 'T');
        state = s_res_HTT;
        break;

      case s_res_HTT:
        STRICT_CHECK(ch != 'P');
        state = s_res_HTTP;
        break;

      case s_res_HTTP:
        STRICT_CHECK(ch != '/');
        state = s_res_first_http_major;
        break;

      case s_res_first_http_major:
        if (ch < '1' || ch > '9') goto error;
        parser->http_major = ch - '0';
        state = s_res_http_major;
        break;

      /* major HTTP version or dot */
      case s_res_http_major:
      {
        if (ch == '.') {
          state = s_res_first_http_minor;
          break;
        }

        if (ch < '0' || ch > '9') goto error;

        parser->http_major *= 10;
        parser->http_major += ch - '0';

        if (parser->http_major > 999) goto error;
        break;
      }

      /* first digit of minor HTTP version */
      case s_res_first_http_minor:
        if (ch < '0' || ch > '9') goto error;
        parser->http_minor = ch - '0';
        state = s_res_http_minor;
        break;

      /* minor HTTP version or end of request line */
      case s_res_http_minor:
      {
        if (ch == ' ') {
          state = s_res_first_status_code;
          break;
        }

        if (ch < '0' || ch > '9') goto error;

        parser->http_minor *= 10;
        parser->http_minor += ch - '0';

        if (parser->http_minor > 999) goto error;
        break;
      }

      case s_res_first_status_code:
      {
        if (ch < '0' || ch > '9') {
          if (ch == ' ') {
            break;
          }
          goto error;
        }
        parser->status_code = ch - '0';
        state = s_res_status_code;
        break;
      }

      case s_res_status_code:
      {
        if (ch < '0' || ch > '9') {
          switch (ch) {
            case ' ':
              state = s_res_status;
              break;
            case CR:
              state = s_res_line_almost_done;
              break;
            case LF:
              state = s_header_field_start;
              break;
            default:
              goto error;
          }
          break;
        }

        parser->status_code *= 10;
        parser->status_code += ch - '0';

        if (parser->status_code > 999) goto error;
        break;
      }

      case s_res_status:
        /* the human readable status. e.g. "NOT FOUND"
         * we are not humans so just ignore this */
        if (ch == CR) {
          state = s_res_line_almost_done;
          break;
        }

        if (ch == LF) {
          state = s_header_field_start;
          break;
        }
        break;

      case s_res_line_almost_done:
        STRICT_CHECK(ch != LF);
        state = s_header_field_start;
        break;

      case s_start_req:
      {
        if (ch == CR || ch == LF)
          break;
        parser->flags = 0;
        parser->content_length = -1;

        CALLBACK2(message_begin);

        if (ch < 'A' || 'Z' < ch) goto error;

      start_req_method_assign:
        parser->method = (enum php_http_method) 0;
        index = 1;
        switch (ch) {
          case 'C': parser->method = PHP_HTTP_CONNECT; /* or COPY, CHECKOUT */ break;
          case 'D': parser->method = PHP_HTTP_DELETE; break;
          case 'G': parser->method = PHP_HTTP_GET; break;
          case 'H': parser->method = PHP_HTTP_HEAD; break;
          case 'L': parser->method = PHP_HTTP_LOCK; break;
          case 'M': parser->method = PHP_HTTP_MKCOL; /* or MOVE, MKCALENDAR, MKACTIVITY, MERGE, M-SEARCH */ break;
          case 'N': parser->method = PHP_HTTP_NOTIFY; break;
          case 'O': parser->method = PHP_HTTP_OPTIONS; break;
          case 'P': parser->method = PHP_HTTP_POST; /* or PROPFIND or PROPPATCH or PUT */ break;
          case 'R': parser->method = PHP_HTTP_REPORT; break;
          case 'S': parser->method = PHP_HTTP_SUBSCRIBE; /* or SEARCH */ break;
          case 'T': parser->method = PHP_HTTP_TRACE; break;
          case 'U': parser->method = PHP_HTTP_UNLOCK; /* or UNSUBSCRIBE */ break;
          default: parser->method = PHP_HTTP_NOT_IMPLEMENTED; break;
        }
        state = s_req_method;
        break;
      }
      case s_req_method:
      {
        const char *matcher;
        if (ch == '\0')
          goto error;

        matcher = method_strings[parser->method];
        if (ch == ' ') {
          if (parser->method != PHP_HTTP_NOT_IMPLEMENTED && matcher[index] != '\0') {
            parser->method = PHP_HTTP_NOT_IMPLEMENTED;
          }
          state = s_req_spaces_before_url;
        } else if (parser->method == PHP_HTTP_NOT_IMPLEMENTED || ch == matcher[index]) {
          ; /* nada */
        } else if (parser->method == PHP_HTTP_CONNECT) {
          if (index == 1 && ch == 'H') {
            parser->method = PHP_HTTP_CHECKOUT;
          } else if (index == 2  && ch == 'P') {
            parser->method = PHP_HTTP_COPY;
          } else {
            parser->method = PHP_HTTP_NOT_IMPLEMENTED;
          }
        } else if (parser->method == PHP_HTTP_MKCOL) {
          if (index == 1 && ch == 'O') {
            parser->method = PHP_HTTP_MOVE;
          } else if (index == 3 && ch == 'A') {
            parser->method = PHP_HTTP_MKCALENDAR;
          } else if (index == 1 && ch == 'E') {
            parser->method = PHP_HTTP_MERGE;
          } else if (index == 1 && ch == '-') {
            parser->method = PHP_HTTP_MSEARCH;
          } else if (index == 2 && ch == 'A') {
            parser->method = PHP_HTTP_MKACTIVITY;
          } else {
            parser->method = PHP_HTTP_NOT_IMPLEMENTED;
          }
        } else if (index == 1 && parser->method == PHP_HTTP_POST && ch == 'R') {
          parser->method = PHP_HTTP_PROPFIND; /* or HTTP_PROPPATCH */
        } else if (index == 1 && parser->method == PHP_HTTP_POST && ch == 'U') {
          parser->method = PHP_HTTP_PUT;
        } else if (index == 1 && parser->method == PHP_HTTP_POST && ch == 'A') {
          parser->method = PHP_HTTP_PATCH;
        } else if (index == 1 && parser->method == PHP_HTTP_SUBSCRIBE && ch == 'E') {
          parser->method = PHP_HTTP_SEARCH;
        } else if (index == 2 && parser->method == PHP_HTTP_UNLOCK && ch == 'S') {
          parser->method = PHP_HTTP_UNSUBSCRIBE;
        } else if (index == 4 && parser->method == PHP_HTTP_PROPFIND && ch == 'P') {
          parser->method = PHP_HTTP_PROPPATCH;
        } else {
          parser->method = PHP_HTTP_NOT_IMPLEMENTED;
        }

        ++index;
        break;
      }
      case s_req_spaces_before_url:
      {
        if (ch == ' ') break;

        if (ch == '/' || ch == '*') {
          MARK(url);
          MARK(path);
          state = s_req_path;
          break;
        }

        c = LOWER(ch);

        if (c >= 'a' && c <= 'z') {
          MARK(url);
          state = s_req_schema;
          break;
        }

        goto error;
      }

      case s_req_schema:
      {
        c = LOWER(ch);

        if (c >= 'a' && c <= 'z') break;

        if (ch == ':') {
          state = s_req_schema_slash;
          break;
        } else if (ch == '.') {
          state = s_req_host;
          break;
        } else if ('0' <= ch && ch <= '9') {
          state = s_req_host;
          break;
        }

        goto error;
      }

      case s_req_schema_slash:
        STRICT_CHECK(ch != '/');
        state = s_req_schema_slash_slash;
        break;

      case s_req_schema_slash_slash:
        STRICT_CHECK(ch != '/');
        state = s_req_host;
        break;

      case s_req_host:
      {
        c = LOWER(ch);
        if (c >= 'a' && c <= 'z') break;
        if ((ch >= '0' && ch <= '9') || ch == '.' || ch == '-') break;
        switch (ch) {
          case ':':
            state = s_req_port;
            break;
          case '/':
            MARK(path);
            state = s_req_path;
            break;
          case ' ':
            /* The request line looks like:
             *   "GET http://foo.bar.com HTTP/1.1"
             * That is, there is no path.
             */
            CALLBACK(url);
            state = s_req_http_start;
            break;
          default:
            goto error;
        }
        break;
      }

      case s_req_port:
      {
        if (ch >= '0' && ch <= '9') break;
        switch (ch) {
          case '/':
            MARK(path);
            state = s_req_path;
            break;
          case ' ':
            /* The request line looks like:
             *   "GET http://foo.bar.com:1234 HTTP/1.1"
             * That is, there is no path.
             */
            CALLBACK(url);
            state = s_req_http_start;
            break;
          default:
            goto error;
        }
        break;
      }

      case s_req_path:
      {
        if (normal_url_char[(unsigned char)ch]) break;

        switch (ch) {
          case ' ':
            CALLBACK(url);
            CALLBACK(path);
            state = s_req_http_start;
            break;
          case CR:
            CALLBACK(url);
            CALLBACK(path);
            parser->http_major = 0;
            parser->http_minor = 9;
            state = s_req_line_almost_done;
            break;
          case LF:
            CALLBACK(url);
            CALLBACK(path);
            parser->http_major = 0;
            parser->http_minor = 9;
            state = s_header_field_start;
            break;
          case '?':
            CALLBACK(path);
            state = s_req_query_string_start;
            break;
          case '#':
            CALLBACK(path);
            state = s_req_fragment_start;
            break;
          default:
            goto error;
        }
        break;
      }

      case s_req_query_string_start:
      {
        if (normal_url_char[(unsigned char)ch]) {
          MARK(query_string);
          state = s_req_query_string;
          break;
        }

        switch (ch) {
          case '?':
            break; /* XXX ignore extra '?' ... is this right? */
          case ' ':
            CALLBACK(url);
            state = s_req_http_start;
            break;
          case CR:
            CALLBACK(url);
            parser->http_major = 0;
            parser->http_minor = 9;
            state = s_req_line_almost_done;
            break;
          case LF:
            CALLBACK(url);
            parser->http_major = 0;
            parser->http_minor = 9;
            state = s_header_field_start;
            break;
          case '#':
            state = s_req_fragment_start;
            break;
          default:
            goto error;
        }
        break;
      }

      case s_req_query_string:
      {
        if (normal_url_char[(unsigned char)ch]) break;

        switch (ch) {
          case '?':
            /* allow extra '?' in query string */
            break;
          case ' ':
            CALLBACK(url);
            CALLBACK(query_string);
            state = s_req_http_start;
            break;
          case CR:
            CALLBACK(url);
            CALLBACK(query_string);
            parser->http_major = 0;
            parser->http_minor = 9;
            state = s_req_line_almost_done;
            break;
          case LF:
            CALLBACK(url);
            CALLBACK(query_string);
            parser->http_major = 0;
            parser->http_minor = 9;
            state = s_header_field_start;
            break;
          case '#':
            CALLBACK(query_string);
            state = s_req_fragment_start;
            break;
          default:
            goto error;
        }
        break;
      }

      case s_req_fragment_start:
      {
        if (normal_url_char[(unsigned char)ch]) {
          MARK(fragment);
          state = s_req_fragment;
          break;
        }

        switch (ch) {
          case ' ':
            CALLBACK(url);
            state = s_req_http_start;
            break;
          case CR:
            CALLBACK(url);
            parser->http_major = 0;
            parser->http_minor = 9;
            state = s_req_line_almost_done;
            break;
          case LF:
            CALLBACK(url);
            parser->http_major = 0;
            parser->http_minor = 9;
            state = s_header_field_start;
            break;
          case '?':
            MARK(fragment);
            state = s_req_fragment;
            break;
          case '#':
            break;
          default:
            goto error;
        }
        break;
      }

      case s_req_fragment:
      {
        if (normal_url_char[(unsigned char)ch]) break;

        switch (ch) {
          case ' ':
            CALLBACK(url);
            CALLBACK(fragment);
            state = s_req_http_start;
            break;
          case CR:
            CALLBACK(url);
            CALLBACK(fragment);
            parser->http_major = 0;
            parser->http_minor = 9;
            state = s_req_line_almost_done;
            break;
          case LF:
            CALLBACK(url);
            CALLBACK(fragment);
            parser->http_major = 0;
            parser->http_minor = 9;
            state = s_header_field_start;
            break;
          case '?':
          case '#':
            break;
          default:
            goto error;
        }
        break;
      }

      case s_req_http_start:
        switch (ch) {
          case 'H':
            state = s_req_http_H;
            break;
          case ' ':
            break;
          default:
            goto error;
        }
        break;

      case s_req_http_H:
        STRICT_CHECK(ch != 'T');
        state = s_req_http_HT;
        break;

      case s_req_http_HT:
        STRICT_CHECK(ch != 'T');
        state = s_req_http_HTT;
        break;

      case s_req_http_HTT:
        STRICT_CHECK(ch != 'P');
        state = s_req_http_HTTP;
        break;

      case s_req_http_HTTP:
        STRICT_CHECK(ch != '/');
        state = s_req_first_http_major;
        break;

      /* first digit of major HTTP version */
      case s_req_first_http_major:
        if (ch < '1' || ch > '9') goto error;
        parser->http_major = ch - '0';
        state = s_req_http_major;
        break;

      /* major HTTP version or dot */
      case s_req_http_major:
      {
        if (ch == '.') {
          state = s_req_first_http_minor;
          break;
        }

        if (ch < '0' || ch > '9') goto error;

        parser->http_major *= 10;
        parser->http_major += ch - '0';

        if (parser->http_major > 999) goto error;
        break;
      }

      /* first digit of minor HTTP version */
      case s_req_first_http_minor:
        if (ch < '0' || ch > '9') goto error;
        parser->http_minor = ch - '0';
        state = s_req_http_minor;
        break;

      /* minor HTTP version or end of request line */
      case s_req_http_minor:
      {
        if (ch == CR) {
          state = s_req_line_almost_done;
          break;
        }

        if (ch == LF) {
          state = s_header_field_start;
          break;
        }

        /* XXX allow spaces after digit? */

        if (ch < '0' || ch > '9') goto error;

        parser->http_minor *= 10;
        parser->http_minor += ch - '0';

        if (parser->http_minor > 999) goto error;
        break;
      }

      /* end of request line */
      case s_req_line_almost_done:
      {
        if (ch != LF) goto error;
        state = s_header_field_start;
        break;
      }

      case s_header_field_start:
      {
        if (ch == CR) {
          state = s_headers_almost_done;
          break;
        }

        if (ch == LF) {
          /* they might be just sending \n instead of \r\n so this would be
           * the second \n to denote the end of headers*/
          state = s_headers_almost_done;
          goto headers_almost_done;
        }

        c = TOKEN(ch);

        if (!c) goto error;

        MARK(header_field);

        index = 0;
        state = s_header_field;

        switch (c) {
          case 'c':
            header_state = h_C;
            break;

          case 'p':
            header_state = h_matching_proxy_connection;
            break;

          case 't':
            header_state = h_matching_transfer_encoding;
            break;

          case 'u':
            header_state = h_matching_upgrade;
            break;

          default:
            header_state = h_general;
            break;
        }
        break;
      }

      case s_header_field:
      {
        c = TOKEN(ch);

        if (c) {
          switch (header_state) {
            case h_general:
              break;

            case h_C:
              index++;
              header_state = (c == 'o' ? h_CO : h_general);
              break;

            case h_CO:
              index++;
              header_state = (c == 'n' ? h_CON : h_general);
              break;

            case h_CON:
              index++;
              switch (c) {
                case 'n':
                  header_state = h_matching_connection;
                  break;
                case 't':
                  header_state = h_matching_content_length;
                  break;
                default:
                  header_state = h_general;
                  break;
              }
              break;

            /* connection */

            case h_matching_connection:
              index++;
              if (index > sizeof(CONNECTION)-1
                  || c != CONNECTION[index]) {
                header_state = h_general;
              } else if (index == sizeof(CONNECTION)-2) {
                header_state = h_connection;
              }
              break;

            /* proxy-connection */

            case h_matching_proxy_connection:
              index++;
              if (index > sizeof(PROXY_CONNECTION)-1
                  || c != PROXY_CONNECTION[index]) {
                header_state = h_general;
              } else if (index == sizeof(PROXY_CONNECTION)-2) {
                header_state = h_connection;
              }
              break;

            /* content-length */

            case h_matching_content_length:
              index++;
              if (index > sizeof(CONTENT_LENGTH)-1
                  || c != CONTENT_LENGTH[index]) {
                header_state = h_general;
              } else if (index == sizeof(CONTENT_LENGTH)-2) {
                header_state = h_content_length;
              }
              break;

            /* transfer-encoding */

            case h_matching_transfer_encoding:
              index++;
              if (index > sizeof(TRANSFER_ENCODING)-1
                  || c != TRANSFER_ENCODING[index]) {
                header_state = h_general;
              } else if (index == sizeof(TRANSFER_ENCODING)-2) {
                header_state = h_transfer_encoding;
              }
              break;

            /* upgrade */

            case h_matching_upgrade:
              index++;
              if (index > sizeof(UPGRADE)-1
                  || c != UPGRADE[index]) {
                header_state = h_general;
              } else if (index == sizeof(UPGRADE)-2) {
                header_state = h_upgrade;
              }
              break;

            case h_connection:
            case h_content_length:
            case h_transfer_encoding:
            case h_upgrade:
              if (ch != ' ') header_state = h_general;
              break;

            default:
              assert(0 && "Unknown header_state");
              break;
          }
          break;
        }

        if (ch == ':') {
          CALLBACK(header_field);
          state = s_header_value_start;
          break;
        }

        if (ch == CR) {
          state = s_header_almost_done;
          CALLBACK(header_field);
          break;
        }

        if (ch == LF) {
          CALLBACK(header_field);
          state = s_header_field_start;
          break;
        }

        goto error;
      }

      case s_header_value_start:
      {
        if (ch == ' ') break;

        MARK(header_value);

        state = s_header_value;
        index = 0;

        c = LOWER(ch);

        if (ch == CR) {
          CALLBACK(header_value);
          header_state = h_general;
          state = s_header_almost_done;
          break;
        }

        if (ch == LF) {
          CALLBACK(header_value);
          state = s_header_field_start;
          break;
        }

        switch (header_state) {
          case h_upgrade:
            parser->flags |= F_UPGRADE;
            header_state = h_general;
            break;

          case h_transfer_encoding:
            /* looking for 'Transfer-Encoding: chunked' */
            if ('c' == c) {
              header_state = h_matching_transfer_encoding_chunked;
            } else {
              header_state = h_general;
            }
            break;

          case h_content_length:
            if (ch < '0' || ch > '9') goto error;
            parser->content_length = ch - '0';
            break;

          case h_connection:
            /* looking for 'Connection: keep-alive' */
            if (c == 'k') {
              header_state = h_matching_connection_keep_alive;
            /* looking for 'Connection: close' */
            } else if (c == 'c') {
              header_state = h_matching_connection_close;
            } else {
              header_state = h_general;
            }
            break;

          default:
            header_state = h_general;
            break;
        }
        break;
      }

      case s_header_value:
      {
        c = LOWER(ch);

        if (ch == CR) {
          CALLBACK(header_value);
          state = s_header_almost_done;
          break;
        }

        if (ch == LF) {
          CALLBACK(header_value);
          goto header_almost_done;
        }

        switch (header_state) {
          case h_general:
            break;

          case h_connection:
          case h_transfer_encoding:
            assert(0 && "Shouldn't get here.");
            break;

          case h_content_length:
            if (ch == ' ') break;
            if (ch < '0' || ch > '9') goto error;
            parser->content_length *= 10;
            parser->content_length += ch - '0';
            break;

          /* Transfer-Encoding: chunked */
          case h_matching_transfer_encoding_chunked:
            index++;
            if (index > sizeof(CHUNKED)-1
                || c != CHUNKED[index]) {
              header_state = h_general;
            } else if (index == sizeof(CHUNKED)-2) {
              header_state = h_transfer_encoding_chunked;
            }
            break;

          /* looking for 'Connection: keep-alive' */
          case h_matching_connection_keep_alive:
            index++;
            if (index > sizeof(KEEP_ALIVE)-1
                || c != KEEP_ALIVE[index]) {
              header_state = h_general;
            } else if (index == sizeof(KEEP_ALIVE)-2) {
              header_state = h_connection_keep_alive;
            }
            break;

          /* looking for 'Connection: close' */
          case h_matching_connection_close:
            index++;
            if (index > sizeof(CLOSE)-1 || c != CLOSE[index]) {
              header_state = h_general;
            } else if (index == sizeof(CLOSE)-2) {
              header_state = h_connection_close;
            }
            break;

          case h_transfer_encoding_chunked:
          case h_connection_keep_alive:
          case h_connection_close:
            if (ch != ' ') header_state = h_general;
            break;

          default:
            state = s_header_value;
            header_state = h_general;
            break;
        }
        break;
      }

      case s_header_almost_done:
      header_almost_done:
      {
        STRICT_CHECK(ch != LF);

        state = s_header_field_start;

        switch (header_state) {
          case h_connection_keep_alive:
            parser->flags |= F_CONNECTION_KEEP_ALIVE;
            break;
          case h_connection_close:
            parser->flags |= F_CONNECTION_CLOSE;
            break;
          case h_transfer_encoding_chunked:
            parser->flags |= F_CHUNKED;
            break;
          default:
            break;
        }
        break;
      }

      case s_headers_almost_done:
      headers_almost_done:
      {
        STRICT_CHECK(ch != LF);

        if (parser->flags & F_TRAILING) {
          /* End of a chunked request */
          CALLBACK2(message_complete);
          state = NEW_MESSAGE();
          break;
        }

        nread = 0;

        if ((parser->flags & F_UPGRADE) || parser->method == PHP_HTTP_CONNECT) {
          parser->upgrade = 1;
        }

        /* Here we call the headers_complete callback. This is somewhat
         * different than other callbacks because if the user returns 1, we
         * will interpret that as saying that this message has no body. This
         * is needed for the annoying case of receiving a response to a HEAD
         * request.
         */
        if (settings->on_headers_complete) {
          switch (settings->on_headers_complete(parser)) {
            case 0:
              break;

            case 1:
              parser->flags |= F_SKIPBODY;
              break;

            default:
              return p - data; /* Error */
          }
        }

        /* We cannot meaningfully support upgrade requests, since we only
         * support HTTP/1 for now.
         */
#if 0
        /* Exit, the rest of the connect is in a different protocol. */
        if (parser->upgrade) {
          CALLBACK2(message_complete);
          return (p - data);
        }
#endif

        if (parser->flags & F_SKIPBODY) {
          CALLBACK2(message_complete);
          state = NEW_MESSAGE();
        } else if (parser->flags & F_CHUNKED) {
          /* chunked encoding - ignore Content-Length header */
          state = s_chunk_size_start;
        } else {
          if (parser->content_length == 0) {
            /* Content-Length header given but zero: Content-Length: 0\r\n */
            CALLBACK2(message_complete);
            state = NEW_MESSAGE();
          } else if (parser->content_length > 0) {
            /* Content-Length header given and non-zero */
            state = s_body_identity;
          } else {
            if (parser->type == PHP_HTTP_REQUEST || php_http_should_keep_alive(parser)) {
              /* Assume content-length 0 - read the next */
              CALLBACK2(message_complete);
              state = NEW_MESSAGE();
            } else {
              /* Read body until EOF */
              state = s_body_identity_eof;
            }
          }
        }

        break;
      }

      case s_body_identity:
        assert(pe >= p);

        to_read = MIN((size_t)(pe - p), (size_t)parser->content_length);
        if (to_read > 0) {
          if (settings->on_body) settings->on_body(parser, p, to_read);
          p += to_read - 1;
          parser->content_length -= to_read;
          if (parser->content_length == 0) {
            CALLBACK2(message_complete);
            state = NEW_MESSAGE();
          }
        }
        break;

      /* read until EOF */
      case s_body_identity_eof:
        to_read = pe - p;
        if (to_read > 0) {
          if (settings->on_body) settings->on_body(parser, p, to_read);
          p += to_read - 1;
        }
        break;

      case s_chunk_size_start:
      {
        assert(parser->flags & F_CHUNKED);

        c = unhex[(unsigned char)ch];
        if (c == -1) goto error;
        parser->content_length = c;
        state = s_chunk_size;
        break;
      }

      case s_chunk_size:
      {
        assert(parser->flags & F_CHUNKED);

        if (ch == CR) {
          state = s_chunk_size_almost_done;
          break;
        }

        c = unhex[(unsigned char)ch];

        if (c == -1) {
          if (ch == ';' || ch == ' ') {
            state = s_chunk_parameters;
            break;
          }
          goto error;
        }

        parser->content_length *= 16;
        parser->content_length += c;
        break;
      }

      case s_chunk_parameters:
      {
        assert(parser->flags & F_CHUNKED);
        /* just ignore this shit. TODO check for overflow */
        if (ch == CR) {
          state = s_chunk_size_almost_done;
          break;
        }
        break;
      }

      case s_chunk_size_almost_done:
      {
        assert(parser->flags & F_CHUNKED);
        STRICT_CHECK(ch != LF);

        if (parser->content_length == 0) {
          parser->flags |= F_TRAILING;
          state = s_header_field_start;
        } else {
          state = s_chunk_data;
        }
        break;
      }

      case s_chunk_data:
      {
        assert(parser->flags & F_CHUNKED);
        assert(pe >= p);

        to_read = MIN((size_t)(pe - p), (size_t)(parser->content_length));

        if (to_read > 0) {
          if (settings->on_body) settings->on_body(parser, p, to_read);
          p += to_read - 1;
        }

        if (to_read == (size_t)parser->content_length) {
          state = s_chunk_data_almost_done;
        }

        parser->content_length -= to_read;
        break;
      }

      case s_chunk_data_almost_done:
        assert(parser->flags & F_CHUNKED);
        STRICT_CHECK(ch != CR);
        state = s_chunk_data_done;
        break;

      case s_chunk_data_done:
        assert(parser->flags & F_CHUNKED);
        STRICT_CHECK(ch != LF);
        state = s_chunk_size_start;
        break;

      default:
        assert(0 && "unhandled state");
        goto error;
    }
  }

  CALLBACK_NOCLEAR(header_field);
  CALLBACK_NOCLEAR(header_value);
  CALLBACK_NOCLEAR(fragment);
  CALLBACK_NOCLEAR(query_string);
  CALLBACK_NOCLEAR(path);
  CALLBACK_NOCLEAR(url);

  parser->state = state;
  parser->header_state = header_state;
  parser->index = index;
  parser->nread = nread;

  return len;

error:
  parser->state = s_dead;
  return (p - data);
}


int
php_http_should_keep_alive (php_http_parser *parser)
{
  if (parser->http_major > 0 && parser->http_minor > 0) {
    /* HTTP/1.1 */
    if (parser->flags & F_CONNECTION_CLOSE) {
      return 0;
    } else {
      return 1;
    }
  } else {
    /* HTTP/1.0 or earlier */
    if (parser->flags & F_CONNECTION_KEEP_ALIVE) {
      return 1;
    } else {
      return 0;
    }
  }
}


const char * php_http_method_str (enum php_http_method m)
{
  return method_strings[m];
}


void
php_http_parser_init (php_http_parser *parser, enum php_http_parser_type t)
{
  parser->type = t;
  parser->state = (t == PHP_HTTP_REQUEST ? s_start_req : (t == PHP_HTTP_RESPONSE ? s_start_res : s_start_req_or_res));
  parser->nread = 0;
  parser->upgrade = 0;
  parser->flags = 0;
  parser->method = 0;
}
