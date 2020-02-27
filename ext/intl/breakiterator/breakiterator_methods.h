/*
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

PHP_METHOD(BreakIterator, createWordInstance);

PHP_METHOD(BreakIterator, createLineInstance);

PHP_METHOD(BreakIterator, createCharacterInstance);

PHP_METHOD(BreakIterator, createSentenceInstance);

PHP_METHOD(BreakIterator, createTitleInstance);

PHP_METHOD(BreakIterator, createCodePointInstance);

PHP_METHOD(BreakIterator, getText);

PHP_METHOD(BreakIterator, setText);

PHP_METHOD(BreakIterator, first);

PHP_METHOD(BreakIterator, last);

PHP_METHOD(BreakIterator, previous);

PHP_METHOD(BreakIterator, next);

PHP_METHOD(BreakIterator, current);

PHP_METHOD(BreakIterator, following);

PHP_METHOD(BreakIterator, preceding);

PHP_METHOD(BreakIterator, isBoundary);

PHP_METHOD(BreakIterator, getLocale);

PHP_METHOD(BreakIterator, getPartsIterator);

PHP_METHOD(BreakIterator, getErrorCode);

PHP_METHOD(BreakIterator, getErrorMessage);

#endif
