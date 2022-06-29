/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Colin Viebrock <colin@viebrock.ca>                          |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "info.h"


/* Encoded from (without newlines):
<svg xmlns='http://www.w3.org/2000/svg' width='4' height='4'>
<path style='stroke:#000;stroke-width:.5;stroke-opacity:.25' d='M 0,0 4,4 M 0,4 4,0'/>
</svg>
 */
#define BACKGROUND_SVG_ENCODED \
	"%3Csvg%20xmlns%3D%27http%3A%2F%2Fwww.w3.org%2F2000%2Fsvg%27%20width%3D%274%27%20height%3D%274%27%3E%3Cpath%20style%3D%27stroke%3A%23000%3Bstroke-width%3A.5%3Bstroke-opacity%3A.25%27%20d%3D%27M%200%2C0%204%2C4%20M%200%2C4%204%2C0%27%2F%3E%3C%2Fsvg%3E"

PHPAPI ZEND_COLD void php_info_print_css(void) /* {{{ */
{
	PUTS("body {background-color: #fff; color: #222; font-family: sans-serif;}\n");
	PUTS("pre {margin: 0; font-family: monospace;}\n");
	PUTS("a:link {color: #009; text-decoration: none; background-color: #fff;}\n");
	PUTS("a:hover {text-decoration: underline;}\n");
	PUTS("table {border-collapse: collapse; border: 0; width: 934px; box-shadow: 1px 2px 3px rgba(0, 0, 0, 0.2);}\n");
	PUTS(".center {text-align: center;}\n");
	PUTS(".center table {margin: 1em auto; text-align: left;}\n");
	PUTS(".center th {text-align: center !important;}\n");
	PUTS("td, th {border: 1px solid #666; font-size: 75%; vertical-align: baseline; padding: 4px 5px;}\n");
	PUTS("th {position: sticky; top: 0; background: inherit;}\n");
	PUTS("h1 {font-size: 150%;}\n");
	PUTS("h2 {font-size: 125%;}\n");
	PUTS(".p {text-align: left;}\n");
	PUTS(".e {background-color: #ccf; width: 300px; font-weight: bold;}\n");
	PUTS(".h {background-color: #99c; font-weight: bold;}\n");
	PUTS(".v {background-color: #ddd; max-width: 300px; overflow-x: auto; word-wrap: break-word;}\n");
	PUTS(".v i {color: #999;}\n");
	PUTS("img {float: right; border: 0;}\n");
	PUTS("hr {width: 934px; background-color: #ccc; border: 0; height: 1px;}\n");
	PUTS(":root {--php-dark-grey: #333; --php-dark-blue: #4F5B93; --php-medium-blue: #8892BF; --php-light-blue: #E2E4EF; --php-accent-purple: #793862}");
	PUTS(
		"@media (prefers-color-scheme: dark) {\n"
		"  body {background: var(--php-dark-grey) url('data:image/svg+xml;utf8," BACKGROUND_SVG_ENCODED "'); color: var(--php-light-blue)}\n"
		"  .h td, td.e, th {border-color: #606A90}\n"
		"  td {border-color: #505153}\n"
		"  .e {background-color: #404A77}\n"
		"  .h {background-color: var(--php-dark-blue)}\n"
		"  .v {background-color: var(--php-dark-grey)}\n"
		"  hr {background-color: #505153}\n"
		"}\n"
	);
}
/* }}} */
