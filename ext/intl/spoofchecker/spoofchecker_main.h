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
   | Authors: Scott MacVicar <scottmac@php.net>                           |
   +----------------------------------------------------------------------+
 */

#ifndef SPOOFCHECKER_MAIN_H
#define SPOOFCHECKER_MAIN_H

#include <php.h>

PHP_METHOD(Spoofchecker, isSuspicious);
PHP_METHOD(Spoofchecker, areConfusable);
PHP_METHOD(Spoofchecker, setAllowedLocales);
PHP_METHOD(Spoofchecker, setChecks);

#endif // SPOOFCHECKER_MAIN_H
