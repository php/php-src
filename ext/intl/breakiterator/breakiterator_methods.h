/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Gustavo Lopes <cataphract@php.net>                          |
   +----------------------------------------------------------------------+
 */

#ifndef BREAKITERATOR_METHODS_H
#define BREAKITERATOR_METHODS_H

#include <php.h>

PHP_METHOD(BreakIterator, __construct);

PHP_FUNCTION(breakiter_create_word_instance);

PHP_FUNCTION(breakiter_create_line_instance);

PHP_FUNCTION(breakiter_create_character_instance);

PHP_FUNCTION(breakiter_create_sentence_instance);

PHP_FUNCTION(breakiter_create_title_instance);

PHP_FUNCTION(breakiter_create_code_point_instance);

PHP_FUNCTION(breakiter_get_text);

PHP_FUNCTION(breakiter_set_text);

PHP_FUNCTION(breakiter_first);

PHP_FUNCTION(breakiter_last);

PHP_FUNCTION(breakiter_previous);

PHP_FUNCTION(breakiter_next);

PHP_FUNCTION(breakiter_current);

PHP_FUNCTION(breakiter_following);

PHP_FUNCTION(breakiter_preceding);

PHP_FUNCTION(breakiter_is_boundary);

PHP_FUNCTION(breakiter_get_locale);

PHP_FUNCTION(breakiter_get_parts_iterator);

PHP_FUNCTION(breakiter_get_error_code);

PHP_FUNCTION(breakiter_get_error_message);

#endif
