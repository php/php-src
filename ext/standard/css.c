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

	PUTS("body {");
	PUTS("background-color: #ffffff;");
	PUTS("color: #000000;");
	PUTS("font-family: verdana, arial, helvetica, sans-serif;");
	PUTS("}\n");
	PUTS("pre {");
	PUTS("font-family: \"andale mono\", \"monotype.com\", \"courier new\", courier, monospace;");
	PUTS("}\n");
	PUTS("a[href] {");
	PUTS("color: #000099;");
	PUTS("text-decoration: none;");
	PUTS("}\n");
	PUTS("a[href]:hover {");
	PUTS("text-decoration: underline;");
	PUTS("}\n");
	PUTS("table {");
	PUTS("border-collapse: collapse;");
	PUTS("width: 600px;");
	PUTS("font-size: 80%;");
	PUTS("}\n");
	PUTS(".centered {");
	PUTS("text-align: center;");
	PUTS("}\n");
	PUTS(".centered table {");
	PUTS("margin-left: auto;");
	PUTS("margin-right: auto;");
	PUTS("text-align: left;");
	PUTS("}\n");
	PUTS("td, th {");
	PUTS("vertical-align: baseline;");
	PUTS("padding: 3px;");
	PUTS("border: 1px solid #000000;");
	PUTS("}\n");
	PUTS("h1 {");
	PUTS("text-align: center;");
	PUTS("font-size: 160%;");
	PUTS("}\n");
	PUTS(".phpver {");
	PUTS("text-align: left;");
	PUTS("}\n");
	PUTS("h2 {");
	PUTS("text-align: center;");
	PUTS("font-size: 130%;");
	PUTS("}\n");
	PUTS(".entry {");
	PUTS("background-color: #ccccff;");
	PUTS("font-weight: bold;");
	PUTS("}\n");
	PUTS(".header {");
	PUTS("background-color: #9999cc;");
	PUTS("font-weight: bold;");
	PUTS("margin-left: auto;");
	PUTS("margin-right: auto;");
	PUTS("}\n");
	PUTS(".value {");
	PUTS("background-color: #cccccc;");
	PUTS("margin-left: auto;");
	PUTS("margin-right: auto;");
	PUTS("}\n");
	PUTS(".infobox {");
	PUTS("background-color: #cccccc;");
	PUTS("}\n");
	PUTS("img {");
	PUTS("float: right;");
	PUTS("border: 0px;");
	PUTS("}\n");
	PUTS("hr {");
	PUTS("width: 600px;");
	PUTS("background-color: #cccccc;");
	PUTS("border: 0px;");
	PUTS("height: 1px;");
	PUTS("}\n");

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
