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
   | Authors: Colin Viebrock <colin@easydns.com>                          |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "info.h"


/* {{{ php_info_print_css
 */
PHPAPI void php_info_print_css(void)
{
	TSRMLS_FETCH();

	PUTS("body {background-color: #ffffff; color: #000000;}\n");
	PUTS("body, td, th, h1, h2 {font-family: sans-serif;}\n");
	PUTS("pre {margin: 0px; font-family: monospace;}\n");
	PUTS("a:link {color: #000099; text-decoration: none;}\n");
	PUTS("a:hover {text-decoration: underline;}\n");
	PUTS("table {border-collapse: collapse;}\n");
	PUTS("td, th { border: 1px solid #000000; font-size: 75%; vertical-align: baseline;}\n");
	PUTS("h1 {font-size: 150%;}\n");
	PUTS("h2 {font-size: 125%;}\n");
	PUTS(".p {text-align: left;}\n");
	PUTS(".e {background-color: #ccccff; font-weight: bold;}\n");
	PUTS(".h {background-color: #9999cc; font-weight: bold;}\n");
	PUTS(".v {background-color: #cccccc;}\n");
	PUTS("i {color: #666666;}\n");
	PUTS("img {float: right; border: 0px;}\n");
	PUTS("hr {width: 600px; align: center; background-color: #cccccc; border: 0px; height: 1px;}\n");

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
