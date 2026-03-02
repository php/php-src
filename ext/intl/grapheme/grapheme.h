/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Ed Batutis <ed@batutis.com>                                 |
   +----------------------------------------------------------------------+
 */

#ifndef GRAPHEME_GRAPHEME_H
#define GRAPHEME_GRAPHEME_H

#include <php.h>
#include <unicode/utypes.h>

#ifdef __cplusplus
extern "C" {
#endif
void grapheme_close_global_iterator( void );
#ifdef __cplusplus
}
#endif

#define GRAPHEME_EXTRACT_TYPE_COUNT		0
#define GRAPHEME_EXTRACT_TYPE_MAXBYTES	1
#define GRAPHEME_EXTRACT_TYPE_MAXCHARS	2
#define GRAPHEME_EXTRACT_TYPE_MIN	GRAPHEME_EXTRACT_TYPE_COUNT
#define GRAPHEME_EXTRACT_TYPE_MAX	GRAPHEME_EXTRACT_TYPE_MAXCHARS

#endif // GRAPHEME_GRAPHEME_H
