/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Colin Viebrock <colin@viebrock.ca>                          |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "info.h"

PHPAPI void php_info_print_css(void) /* {{{ */
{
	PUTS("body {background-color: #fff; color: #222; font-family: sans-serif;}\n");
	PUTS("pre {margin: 0; font-family: monospace;}\n");
	PUTS("a:link {color: #009; text-decoration: none; background-color: #fff;}\n");
	PUTS("a:hover {text-decoration: underline;}\n");
	PUTS("table {border-collapse: collapse; border: 0; width: 934px; box-shadow: 1px 2px 3px #ccc;}\n");
	PUTS(".center {text-align: center;}\n");
	PUTS(".center table {margin: 1em auto; text-align: left;}\n");
	PUTS(".center th {text-align: center !important;}\n");
	PUTS("td, th {border: 1px solid #666; font-size: 75%; vertical-align: baseline; padding: 4px 5px;}\n");
	PUTS("h1 {font-size: 150%;}\n");
	PUTS("h2 {font-size: 125%;}\n");
	PUTS(".p {text-align: left;}\n");
	PUTS(".e {background-color: #ccf; width: 300px; font-weight: bold;}\n");
	PUTS(".h {background-color: #99c; font-weight: bold;}\n");
	PUTS(".v {background-color: #ddd; max-width: 300px; overflow-x: auto; word-wrap: break-word;}\n");
	PUTS(".v i {color: #999;}\n");
	PUTS("img {float: right; border: 0;}\n");
	PUTS("hr {width: 934px; background-color: #ccc; border: 0; height: 1px;}\n");
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
