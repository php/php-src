/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Stanislav Malyshev <stas@zend.com>                          |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_OPTIMIZER_H
#define ZEND_OPTIMIZER_H

#include "zend.h"
#include "zend_compile.h"

#define ZEND_OPTIMIZER_PASS_1		(1<<0)   /* Simple local optimizations   */
#define ZEND_OPTIMIZER_PASS_2		(1<<1)   /*                              */
#define ZEND_OPTIMIZER_PASS_3		(1<<2)   /* Jump optimization            */
#define ZEND_OPTIMIZER_PASS_4		(1<<3)   /* INIT_FCALL_BY_NAME -> DO_FCALL */
#define ZEND_OPTIMIZER_PASS_5		(1<<4)   /* CFG based optimization       */
#define ZEND_OPTIMIZER_PASS_6		(1<<5)   /* DFA based optimization       */
#define ZEND_OPTIMIZER_PASS_7		(1<<6)   /* CALL GRAPH optimization      */
#define ZEND_OPTIMIZER_PASS_8		(1<<7)   /* SCCP (constant propagation)  */
#define ZEND_OPTIMIZER_PASS_9		(1<<8)   /* TMP VAR usage                */
#define ZEND_OPTIMIZER_PASS_10		(1<<9)   /* NOP removal                 */
#define ZEND_OPTIMIZER_PASS_11		(1<<10)  /* Merge equal constants       */
#define ZEND_OPTIMIZER_PASS_12		(1<<11)  /* Adjust used stack           */
#define ZEND_OPTIMIZER_PASS_13		(1<<12)  /* Remove unused variables     */
#define ZEND_OPTIMIZER_PASS_14		(1<<13)  /* DCE (dead code elimination) */
#define ZEND_OPTIMIZER_PASS_15		(1<<14)  /* (unsafe) Collect constants */
#define ZEND_OPTIMIZER_PASS_16		(1<<15)  /* Inline functions */

#define ZEND_OPTIMIZER_IGNORE_OVERLOADING	(1<<16)  /* (unsafe) Ignore possibility of operator overloading */

#define ZEND_OPTIMIZER_NARROW_TO_DOUBLE		(1<<17)  /* try to narrow long constant assignments to double */

#define ZEND_OPTIMIZER_ALL_PASSES	0x7FFFFFFF

#define DEFAULT_OPTIMIZATION_LEVEL  "0x7FFEBFFF"


#define ZEND_DUMP_AFTER_PASS_1		ZEND_OPTIMIZER_PASS_1
#define ZEND_DUMP_AFTER_PASS_2		ZEND_OPTIMIZER_PASS_2
#define ZEND_DUMP_AFTER_PASS_3		ZEND_OPTIMIZER_PASS_3
#define ZEND_DUMP_AFTER_PASS_4		ZEND_OPTIMIZER_PASS_4
#define ZEND_DUMP_AFTER_PASS_5		ZEND_OPTIMIZER_PASS_5
#define ZEND_DUMP_AFTER_PASS_6		ZEND_OPTIMIZER_PASS_6
#define ZEND_DUMP_AFTER_PASS_7		ZEND_OPTIMIZER_PASS_7
#define ZEND_DUMP_AFTER_PASS_8		ZEND_OPTIMIZER_PASS_8
#define ZEND_DUMP_AFTER_PASS_9		ZEND_OPTIMIZER_PASS_9
#define ZEND_DUMP_AFTER_PASS_10		ZEND_OPTIMIZER_PASS_10
#define ZEND_DUMP_AFTER_PASS_11		ZEND_OPTIMIZER_PASS_11
#define ZEND_DUMP_AFTER_PASS_12		ZEND_OPTIMIZER_PASS_12
#define ZEND_DUMP_AFTER_PASS_13		ZEND_OPTIMIZER_PASS_13
#define ZEND_DUMP_AFTER_PASS_14		ZEND_OPTIMIZER_PASS_14

#define ZEND_DUMP_BEFORE_OPTIMIZER  (1<<16)
#define ZEND_DUMP_AFTER_OPTIMIZER   (1<<17)

#define ZEND_DUMP_BEFORE_BLOCK_PASS (1<<18)
#define ZEND_DUMP_AFTER_BLOCK_PASS  (1<<19)
#define ZEND_DUMP_BLOCK_PASS_VARS   (1<<20)

#define ZEND_DUMP_BEFORE_DFA_PASS   (1<<21)
#define ZEND_DUMP_AFTER_DFA_PASS    (1<<22)
#define ZEND_DUMP_DFA_CFG           (1<<23)
#define ZEND_DUMP_DFA_DOMINATORS    (1<<24)
#define ZEND_DUMP_DFA_LIVENESS      (1<<25)
#define ZEND_DUMP_DFA_PHI           (1<<26)
#define ZEND_DUMP_DFA_SSA           (1<<27)
#define ZEND_DUMP_DFA_SSA_VARS      (1<<28)
#define ZEND_DUMP_SCCP              (1<<29)

typedef struct _zend_script {
	zend_string   *filename;
	zend_op_array  main_op_array;
	HashTable      function_table;
	HashTable      class_table;
	uint32_t       first_early_binding_opline; /* the linked list of delayed declarations */
} zend_script;

int zend_optimize_script(zend_script *script, zend_long optimization_level, zend_long debug_level);
int zend_optimizer_startup(void);
int zend_optimizer_shutdown(void);

#endif
