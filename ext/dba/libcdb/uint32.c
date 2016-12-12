/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Marcus Boerger <helly@php.net>                               |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

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
