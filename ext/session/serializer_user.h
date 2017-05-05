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
   | Author: Yasuo Ohgaki <yohgaki@ohgaki.net/php.net>                    |
   +----------------------------------------------------------------------+
 */

#ifndef MOD_SERIALIZER_USER_H
#define MOD_SERIALIZER_USER_H

extern ps_serializer ps_serializer_user;
#define ps_serializer_user_ptr &ps_serializer_user

PS_SERIALIZER_FUNCS(user);

#endif
