/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "info.h"
#include "SAPI.h"

#define CREDIT_LINE(module, authors) php_info_print_table_row(2, module, authors)

PHPAPI ZEND_COLD void php_print_credits(int flag) /* {{{ */
{
	if (!sapi_module.phpinfo_as_text && flag & PHP_CREDITS_FULLPAGE) {
		php_print_info_htmlhead();
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
		php_info_print_table_row(1, "Andi Gutmans, Rasmus Lerdorf, Zeev Suraski, Marcus Boerger");
		php_info_print_table_end();

		/* PHP Language */
		php_info_print_table_start();
		php_info_print_table_colspan_header(2, "PHP Authors");
		php_info_print_table_header(2, "Contribution", "Authors");
		CREDIT_LINE("Zend Scripting Language Engine", "Andi Gutmans, Zeev Suraski, Stanislav Malyshev, Marcus Boerger, Dmitry Stogov, Xinchen Hui, Nikita Popov");
		CREDIT_LINE("Extension Module API", "Andi Gutmans, Zeev Suraski, Andrei Zmievski");
		CREDIT_LINE("UNIX Build and Modularization", "Stig Bakken, Sascha Schumann, Jani Taskinen, Peter Kokot");
		CREDIT_LINE("Windows Support", "Shane Caraveo, Zeev Suraski, Wez Furlong, Pierre-Alain Joye, Anatol Belski, Kalle Sommer Nielsen");
		CREDIT_LINE("Server API (SAPI) Abstraction Layer", "Andi Gutmans, Shane Caraveo, Zeev Suraski");
		CREDIT_LINE("Streams Abstraction Layer", "Wez Furlong, Sara Golemon");
		CREDIT_LINE("PHP Data Objects Layer", "Wez Furlong, Marcus Boerger, Sterling Hughes, George Schlossnagle, Ilia Alshanetsky");
		CREDIT_LINE("Output Handler", "Zeev Suraski, Thies C. Arntzen, Marcus Boerger, Michael Wallner");
		CREDIT_LINE("Consistent 64 bit support", "Anthony Ferrara, Anatol Belski");
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
		CREDIT_LINE("Authors", "Mehdi Achour, Friedhelm Betz, Antony Dovgal, Nuno Lopes, Hannes Magnusson, Philip Olson, Georg Richter, Damien Seguy, Jakub Vrana, Adam Harvey");
		CREDIT_LINE("Editor", "Peter Cowburn");
		CREDIT_LINE("User Note Maintainers", "Daniel P. Brown, Thiago Henrique Pojda");
		CREDIT_LINE("Other Contributors", "Previously active authors, editors and other contributors are listed in the manual.");
		php_info_print_table_end();
	}

	if (flag & PHP_CREDITS_QA) {
		php_info_print_table_start();
		php_info_print_table_header(1, "PHP Quality Assurance Team");
		php_info_print_table_row(1, "Ilia Alshanetsky, Joerg Behrens, Antony Dovgal, Stefan Esser, Moriyoshi Koizumi, Magnus Maatta, Sebastian Nohn, Derick Rethans, Melvyn Sopacua, Pierre-Alain Joye, Dmitry Stogov, Felipe Pena, David Soria Parra, Stanislav Malyshev, Julien Pauli, Stephen Zarkos, Anatol Belski, Remi Collet, Ferenc Kovacs");
		php_info_print_table_end();
	}

	if (flag & PHP_CREDITS_WEB) {
		/* Websites and infrastructure */

		php_info_print_table_start();
		php_info_print_table_colspan_header(2, "Websites and Infrastructure team");
		/* www., wiki., windows., master., and others, I guess pecl. too? */
		CREDIT_LINE("PHP Websites Team", "Rasmus Lerdorf, Hannes Magnusson, Philip Olson, Lukas Kahwe Smith, Pierre-Alain Joye, Kalle Sommer Nielsen, Peter Cowburn, Adam Harvey, Ferenc Kovacs, Levi Morrison");
		CREDIT_LINE("Event Maintainers", "Damien Seguy, Daniel P. Brown");
		/* Mirroring */
		CREDIT_LINE("Network Infrastructure", "Daniel P. Brown");
		/* Windows build boxes and such things */
		CREDIT_LINE("Windows Infrastructure", "Alex Schoenmaker");
		php_info_print_table_end();
	}

	if (!sapi_module.phpinfo_as_text && flag & PHP_CREDITS_FULLPAGE) {
		PUTS("</div></body></html>\n");
	}
}
/* }}} */
