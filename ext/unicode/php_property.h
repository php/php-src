/*
   +----------------------------------------------------------------------+
   | PHP Version 6                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andrei Zmievski <andrei@php.net>                            |
   +----------------------------------------------------------------------+
*/

/* $Id$ */ 

#ifndef PHP_PROPERTY_H
#define PHP_PROPERTY_H

/*
 * C/POSIX migration functions
 */
PHP_FUNCTION(unicode_is_lower);
PHP_FUNCTION(unicode_is_upper);
PHP_FUNCTION(unicode_is_digit);
PHP_FUNCTION(unicode_is_alpha);
PHP_FUNCTION(unicode_is_alnum);
PHP_FUNCTION(unicode_is_xdigit);
PHP_FUNCTION(unicode_is_punct);
PHP_FUNCTION(unicode_is_graph);
PHP_FUNCTION(unicode_is_blank);
PHP_FUNCTION(unicode_is_space);
PHP_FUNCTION(unicode_is_cntrl);
PHP_FUNCTION(unicode_is_print);

/*
 * Additional binary property functions
 */

PHP_FUNCTION(unicode_is_title);
PHP_FUNCTION(unicode_is_defined);
PHP_FUNCTION(unicode_is_id_start);
PHP_FUNCTION(unicode_is_id_part);
PHP_FUNCTION(unicode_is_id_ignorable);
PHP_FUNCTION(unicode_is_iso_control);
PHP_FUNCTION(unicode_is_mirrored);
PHP_FUNCTION(unicode_is_base);
PHP_FUNCTION(unicode_is_whitespace);
PHP_FUNCTION(unicode_is_u_whitespace);
PHP_FUNCTION(unicode_is_u_alphabetic);
PHP_FUNCTION(unicode_is_u_uppercase);
PHP_FUNCTION(unicode_is_u_lowercase);

/*
 * Single character property functions.
 */

PHP_FUNCTION(unicode_get_numeric_value);
PHP_FUNCTION(unicode_get_combining_class);
PHP_FUNCTION(unicode_get_digit_value);
PHP_FUNCTION(unicode_get_mirror);
PHP_FUNCTION(unicode_get_direction);
PHP_FUNCTION(unicode_get_char_type);
PHP_FUNCTION(unicode_is_char_valid);

#endif /* PHP_PROPERTY_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
