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
   | Author: Marcus Boerger <helly@php.net>                               |
   +----------------------------------------------------------------------+
 */

/* incorporated from D.J.Bernstein's cdb-0.75 (http://cr.yp.to/cdb.html)*/

#ifndef UINT32_H
#define UINT32_H

#if SIZEOF_INT == 4
/* Most 32-bit and 64-bit systems have 32-bit ints */
typedef unsigned int uint32;
#elif SIZEOF_LONG == 4
/* 16-bit systems? */
typedef unsigned long uint32;
#else
#error Need type which holds 32 bits
#endif

void uint32_pack(char *out, uint32 in);
void uint32_unpack(const char *in, uint32 *out);

#endif
