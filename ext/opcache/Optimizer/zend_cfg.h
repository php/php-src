/*
   +----------------------------------------------------------------------+
   | Zend Engine, CFG - Control Flow Graph                                |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_CFG_H
#define ZEND_CFG_H

/* func flags */
#define ZEND_FUNC_TOO_DYNAMIC    (1<<0)
#define ZEND_FUNC_HAS_CALLS      (1<<1)
#define ZEND_FUNC_VARARG         (1<<2)

/* zend_basic_bloc.flags */
#define ZEND_BB_START            (1<<0)  /* fist block             */
#define ZEND_BB_FOLLOW           (1<<1)  /* follows the next block */
#define ZEND_BB_TARGET           (1<<2)  /* jump taget             */
#define ZEND_BB_EXIT             (1<<3)  /* without successors     */
#define ZEND_BB_ENTRY            (1<<4)  /* stackless entry        */
#define ZEND_BB_TRY              (1<<5)  /* start of try block     */
#define ZEND_BB_CATCH            (1<<6)  /* start of catch block   */
#define ZEND_BB_FINALLY          (1<<7)  /* start of finally block */
#define ZEND_BB_FINALLY_END      (1<<8)  /* end of finally block   */
#define ZEND_BB_GEN_VAR          (1<<9)  /* start of live range    */
#define ZEND_BB_KILL_VAR         (1<<10) /* end of live range      */
#define ZEND_BB_EMPTY            (1<<11)

#define ZEND_BB_LOOP_HEADER      (1<<16)
#define ZEND_BB_IRREDUCIBLE_LOOP (1<<17)

#define ZEND_BB_REACHABLE        (1<<31)

#define ZEND_BB_PROTECTED        (ZEND_BB_ENTRY|ZEND_BB_TRY|ZEND_BB_CATCH|ZEND_BB_FINALLY|ZEND_BB_FINALLY_END|ZEND_BB_GEN_VAR|ZEND_BB_KILL_VAR)

typedef struct _zend_basic_block {
	uint32_t          flags;
	uint32_t          start;              /* first opcode number         */
	uint32_t          end;                /* last opcode number          */
	int               successors[2];      /* up to 2 successor blocks    */
	int               predecessors_count; /* number of predecessors      */
	int               predecessor_offset; /* offset of 1-st predecessor  */
	int               idom;               /* immediate dominator block   */
	int               loop_header;        /* closest loop header, or -1  */
	int               level;              /* steps away from the entry in the dom. tree */
	int               children;           /* list of dominated blocks    */
	int               next_child;         /* next dominated block        */
} zend_basic_block;

/*
+------------+---+---+---+---+---+
|            |OP1|OP2|EXT| 0 | 1 |
+------------+---+---+---+---+---+
|JMP         |ADR|   |   |OP1| - |
|JMPZ        |   |ADR|   |OP2|FOL|
|JMPNZ       |   |ADR|   |OP2|FOL|
|JMPZNZ      |   |ADR|ADR|OP2|EXT|
|JMPZ_EX     |   |ADR|   |OP2|FOL|
|JMPNZ_EX    |   |ADR|   |OP2|FOL|
|JMP_SET     |   |ADR|   |OP2|FOL|
|COALESCE    |   |ADR|   |OP2|FOL|
|ASSERT_CHK  |   |ADR|   |OP2|FOL|
|NEW         |   |ADR|   |OP2|FOL|
|DCL_ANON*   |ADR|   |   |OP1|FOL|
|FE_RESET_*  |   |ADR|   |OP2|FOL|
|FE_FETCH_*  |   |   |ADR|EXT|FOL|
|CATCH       |   |   |ADR|EXT|FOL|
|FAST_CALL   |ADR|   |   |OP1|FOL|
|FAST_RET    |   |   |   | - | - |
|RETURN*     |   |   |   | - | - |
|EXIT        |   |   |   | - | - |
|THROW       |   |   |   | - | - |
|*           |   |   |   |FOL| - |
+------------+---+---+---+---+---+
*/

typedef struct _zend_cfg {
	int               blocks_count;       /* number of basic blocks      */
	zend_basic_block *blocks;             /* array of basic blocks       */
	int              *predecessors;
	uint32_t         *map;
} zend_cfg;

BEGIN_EXTERN_C()

int zend_build_cfg(zend_arena **arena, zend_op_array *op_array, int rt_constants, int stackless, zend_cfg *cfg, uint32_t *func_flags);
void zend_remark_reachable_blocks(zend_op_array *op_array, zend_cfg *cfg);
int zend_cfg_build_predecessors(zend_arena **arena, zend_cfg *cfg);

END_EXTERN_C()

#endif /* ZEND_CFG_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
