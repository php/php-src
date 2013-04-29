/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Kirti Velankar <kirtig@yahoo-inc.com>                        |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef LOCALE_METHODS_H
#define LOCALE_METHODS_H

#include <php.h>

PHP_FUNCTION( locale_get_primary_language );
PHP_FUNCTION( locale_get_script );
PHP_FUNCTION( locale_get_region );
PHP_FUNCTION( locale_get_all_variants);

PHP_NAMED_FUNCTION( zif_locale_get_default );
PHP_NAMED_FUNCTION( zif_locale_set_default );

PHP_FUNCTION( locale_get_display_name );
PHP_FUNCTION( locale_get_display_language );
PHP_FUNCTION( locale_get_display_script );
PHP_FUNCTION( locale_get_display_region );
PHP_FUNCTION( locale_get_display_variant );

PHP_FUNCTION( locale_get_keywords );
PHP_FUNCTION( locale_canonicalize);

PHP_FUNCTION( locale_compose);
PHP_FUNCTION( locale_parse);

PHP_FUNCTION( locale_filter_matches);
PHP_FUNCTION( locale_lookup);
PHP_FUNCTION( locale_canonicalize);
PHP_FUNCTION( locale_accept_from_http);

#endif // LOCALE_METHODS_H
