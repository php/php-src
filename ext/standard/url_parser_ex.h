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
 */

/**
 * Url Parser used from https://github.com/staskobzar/url_parser_re2c
 * Licensed under MIT License.
 *
 * Modified for more correct RFC 3986 requirements & PHP
 */
#ifndef URL_PARSER_EX_H
#define URL_PARSER_EX_H

void url_parse_scheme(php_url *url);
void url_parse_authority(php_url *url);
void url_parse_path(php_url *url);
void url_parse_query_frag(php_url *url);

#endif /* URL_PARSER_EX_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
