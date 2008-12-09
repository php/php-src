/* 
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2008 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "info.h"
#include "SAPI.h"

#define CREDIT_LINE(module, authors) php_info_print_table_row(2, module, authors)

/* {{{ php_print_credits
 */
PHPAPI void php_print_credits(int flag TSRMLS_DC)
{
	if (!sapi_module.phpinfo_as_text && flag & PHP_CREDITS_FULLPAGE) {
		php_print_info_htmlhead(TSRMLS_C);
	}

	if (!sapi_module.phpinfo_as_text) {
		PUTS("<h1>PHP Credits</h1>\n");
	} else {
		PUTS("PHP Credits\n");
	}

	if (flag & PHP_CREDITS_GROUP) {
		/* Group */

		php_info_print_table_start();
		php_info_print_table_header(1, "PHP Group");
		php_info_print_table_row(1, "Thies C. Arntzen, Stig Bakken, Shane Caraveo, Andi Gutmans, Rasmus Lerdorf, Sam Ruby, Sascha Schumann, Zeev Suraski, Jim Winstead, Andrei Zmievski");
		php_info_print_table_end();
	}

	if (flag & PHP_CREDITS_GENERAL) {
		/* Design & Concept */
		php_info_print_table_start();
		if (!sapi_module.phpinfo_as_text) {
			php_info_print_table_header(1, "Language Design &amp; Concept");
		} else {
			php_info_print_table_header(1, "Language Design & Concept");
		}
		php_info_print_table_row(1, "Andi Gutmans, Rasmus Lerdorf, Zeev Suraski");
		php_info_print_table_end();

		/* PHP Language */
		php_info_print_table_start();
		php_info_print_table_colspan_header(2, "PHP 5 Authors");
		php_info_print_table_header(2, "Contribution", "Authors");
		CREDIT_LINE("Zend Scripting Language Engine", "Andi Gutmans, Zeev Suraski");
		CREDIT_LINE("Extension Module API", "Andi Gutmans, Zeev Suraski, Andrei Zmievski");
		CREDIT_LINE("UNIX Build and Modularization", "Stig Bakken, Sascha Schumann, Jani Taskinen");
		CREDIT_LINE("Win32 Port", "Shane Caraveo, Zeev Suraski, Wez Furlong");
		CREDIT_LINE("Server API (SAPI) Abstraction Layer", "Andi Gutmans, Shane Caraveo, Zeev Suraski");
		CREDIT_LINE("Streams Abstraction Layer", "Wez Furlong, Sara Golemon");
		CREDIT_LINE("PHP Data Objects Layer", "Wez Furlong, Marcus Boerger, Sterling Hughes, George Schlossnagle, Ilia Alshanetsky");
		php_info_print_table_end();
	}

	if (flag & PHP_CREDITS_SAPI) {
		/* SAPI Modules */

		php_info_print_table_start();
		php_info_print_table_colspan_header(2, "SAPI Modules");
		php_info_print_table_header(2, "Contribution", "Authors");
#include "credits_sapi.h"
		php_info_print_table_end();
	}

	if (flag & PHP_CREDITS_MODULES) {
		/* Modules */

		php_info_print_table_start();
		php_info_print_table_colspan_header(2, "Module Authors");
		php_info_print_table_header(2, "Module", "Authors");
#include "credits_ext.h"
		php_info_print_table_end();
	}

	if (flag & PHP_CREDITS_DOCS) {
		php_info_print_table_start();
		php_info_print_table_colspan_header(2, "PHP Documentation");
		CREDIT_LINE("Authors", "Mehdi Achour, Friedhelm Betz, Antony Dovgal, Nuno Lopes, Hannes Magnusson, Georg Richter, Damien Seguy, Jakub Vrana");
		CREDIT_LINE("Editor", "Philip Olson");
		CREDIT_LINE("User Note Maintainers", "Friedhelm Betz, Etienne Kneuss, Nuno Lopes, Hannes Magnusson, Felipe Pena, Maciek Sokolewicz");
		CREDIT_LINE("Other Contributors", "Previously active authors, editors and other contributors are listed in the manual.");
		php_info_print_table_end();
	}

	if (flag & PHP_CREDITS_QA) {
		php_info_print_table_start();
		php_info_print_table_header(1, "PHP Quality Assurance Team");
		php_info_print_table_row(1, "Ilia Alshanetsky, Joerg Behrens, Antony Dovgal, Stefan Esser, Moriyoshi Koizumi, Magnus Maatta, Sebastian Nohn, Derick Rethans, Melvyn Sopacua, Jani Taskinen");
		php_info_print_table_end();
	}

	if (flag & PHP_CREDITS_WEB) {
		/* Website Team */
		php_info_print_table_start();
		php_info_print_table_header(1, "PHP Website Team");
		php_info_print_table_row(1, "Rasmus Lerdorf, Hannes Magnusson, Philip Olson");
		php_info_print_table_end();
	}

	if (!sapi_module.phpinfo_as_text && flag & PHP_CREDITS_FULLPAGE) {
		PUTS("</div></body></html>\n");
	}
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
