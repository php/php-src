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
   | Authors: Gustavo Lopes <cataphract@php.net>                          |
   +----------------------------------------------------------------------+
 */

#ifndef RULEBASEDBREAKITERATOR_METHODS_H
#define RULEBASEDBREAKITERATOR_METHODS_H

#include <php.h>

PHP_METHOD(IntlRuleBasedBreakIterator, __construct);

PHP_FUNCTION(rbbi_get_rules);

PHP_FUNCTION(rbbi_get_rule_status);

PHP_FUNCTION(rbbi_get_rule_status_vec);

PHP_FUNCTION(rbbi_get_binary_rules);

#endif
