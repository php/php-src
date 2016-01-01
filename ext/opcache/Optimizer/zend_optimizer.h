/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Stanislav Malyshev <stas@zend.com>                          |
   |          Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_OPTIMIZER_H
#define ZEND_OPTIMIZER_H

#include "zend.h"
#include "zend_compile.h"

#define ZEND_OPTIMIZER_PASS_1		(1<<0)   /* CSE, STRING construction     */
#define ZEND_OPTIMIZER_PASS_2		(1<<1)   /* Constant conversion and jumps */
#define ZEND_OPTIMIZER_PASS_3		(1<<2)   /* ++, +=, series of jumps      */
#define ZEND_OPTIMIZER_PASS_4		(1<<3)   /* INIT_FCALL_BY_NAME -> DO_FCALL */
#define ZEND_OPTIMIZER_PASS_5		(1<<4)   /* CFG based optimization       */
#define ZEND_OPTIMIZER_PASS_6		(1<<5)
#define ZEND_OPTIMIZER_PASS_7		(1<<6)
#define ZEND_OPTIMIZER_PASS_8		(1<<7)   
#define ZEND_OPTIMIZER_PASS_9		(1<<8)   /* TMP VAR usage                */
#define ZEND_OPTIMIZER_PASS_10		(1<<9)   /* NOP removal                 */
#define ZEND_OPTIMIZER_PASS_11		(1<<10)  /* Merge equal constants       */
#define ZEND_OPTIMIZER_PASS_12		(1<<11)
#define ZEND_OPTIMIZER_PASS_13		(1<<12)
#define ZEND_OPTIMIZER_PASS_14		(1<<13)
#define ZEND_OPTIMIZER_PASS_15		(1<<14)  /* Collect constants */

#define ZEND_OPTIMIZER_ALL_PASSES	0x7FFFFFFF

#define DEFAULT_OPTIMIZATION_LEVEL  "0x7FFFBFFF"

#endif
