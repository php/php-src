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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#include "uint32.h"

/* {{{ uint32_pack */
void uint32_pack(char *out, uint32 in)
{
	out[0] = in&0xff; in>>=8;
	out[1] = in&0xff; in>>=8;
	out[2] = in&0xff; in>>=8;
	out[3] = in&0xff;
}
/* }}} */

/* {{{ uint32_unpack */
void uint32_unpack(const char *in, uint32 *out)
{
	*out = (((uint32)(unsigned char)in[3])<<24) |
	       (((uint32)(unsigned char)in[2])<<16) |
	       (((uint32)(unsigned char)in[1])<<8) |
	       (((uint32)(unsigned char)in[0]));
}
/* }}} */
