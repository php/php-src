/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Edin Kadribasic                                                             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */
#ifndef _MD5CRYPT_H_
#define _MD5CRYPT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define MD5_MAGIC	"$1$"
#define MD5_MAGIC_LEN	3

char	*md5_crypt(const char *pw, const char *salt);

#ifdef __cplusplus
}
#endif

#endif /* _MD5CRYPT_H_ */