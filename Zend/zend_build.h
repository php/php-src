/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Stanislav Malyshev <stas@zend.com>                          |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_BUILD_H
#define ZEND_BUILD_H

#define ZEND_TOSTR_(x) #x
#define ZEND_TOSTR(x) ZEND_TOSTR_(x)

#ifdef ZTS
#define ZEND_BUILD_TS ",TS"
#else
#define ZEND_BUILD_TS ",NTS"
#endif

#if ZEND_DEBUG
#define ZEND_BUILD_DEBUG ",debug"
#else
#define ZEND_BUILD_DEBUG
#endif

#if defined(ZEND_WIN32) && defined(PHP_COMPILER_ID)
#define ZEND_BUILD_SYSTEM "," PHP_COMPILER_ID
#else
#define ZEND_BUILD_SYSTEM
#endif

/* for private applications */
#define ZEND_BUILD_EXTRA 

#endif
