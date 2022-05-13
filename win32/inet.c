/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Pierre Joye <pierre@php.net>                                 |
   +----------------------------------------------------------------------+
 */

#include "inet.h"

int inet_aton(const char *cp, struct in_addr *inp) {
	inp->s_addr = inet_addr(cp);

	if (inp->s_addr == INADDR_NONE) {
		  return 0;
	}

	return 1;
}
