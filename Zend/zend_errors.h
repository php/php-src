/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_ERRORS_H
#define ZEND_ERRORS_H

#define E_ERROR				(1<<0L)
#define E_WARNING			(1<<1L)
#define E_PARSE				(1<<2L)
#define E_NOTICE			(1<<3L)
#define E_CORE_ERROR		(1<<4L)
#define E_CORE_WARNING		(1<<5L)
#define E_COMPILE_ERROR		(1<<6L)
#define E_COMPILE_WARNING	(1<<7L)
#define E_USER_ERROR		(1<<8L)
#define E_USER_WARNING		(1<<9L)
#define E_USER_NOTICE		(1<<10L)
#define E_STRICT			(1<<11L)
#define E_RECOVERABLE_ERROR	(1<<12L)
#define E_DEPRECATED		(1<<13L)
#define E_USER_DEPRECATED	(1<<14L)

/* Indicates that this usually fatal error should not result in a bailout */
#define E_DONT_BAIL			(1<<15L)

#define E_ALL (E_ERROR | E_WARNING | E_PARSE | E_NOTICE | E_CORE_ERROR | E_CORE_WARNING | E_COMPILE_ERROR | E_COMPILE_WARNING | E_USER_ERROR | E_USER_WARNING | E_USER_NOTICE | E_RECOVERABLE_ERROR | E_DEPRECATED | E_USER_DEPRECATED | E_STRICT)
#define E_CORE (E_CORE_ERROR | E_CORE_WARNING)

/* Fatal errors that are ignored by the silence operator */
#define E_FATAL_ERRORS (E_ERROR | E_CORE_ERROR | E_COMPILE_ERROR | E_USER_ERROR | E_RECOVERABLE_ERROR | E_PARSE)

#define E_HAS_ONLY_FATAL_ERRORS(mask) !((mask) & ~E_FATAL_ERRORS)

#endif /* ZEND_ERRORS_H */
