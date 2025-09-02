/*
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Ed Batutis <ed@batutis.com>                                 |
   +----------------------------------------------------------------------+
 */

#ifndef GRAPHEME_GRAPHEME_H
#define GRAPHEME_GRAPHEME_H

#include <php.h>
#include <unicode/utypes.h>

void grapheme_close_global_iterator( void );

#define GRAPHEME_EXTRACT_TYPE_COUNT		0
#define GRAPHEME_EXTRACT_TYPE_MAXBYTES	1
#define GRAPHEME_EXTRACT_TYPE_MAXCHARS	2
#define GRAPHEME_EXTRACT_TYPE_MIN	GRAPHEME_EXTRACT_TYPE_COUNT
#define GRAPHEME_EXTRACT_TYPE_MAX	GRAPHEME_EXTRACT_TYPE_MAXCHARS

#endif // GRAPHEME_GRAPHEME_H
