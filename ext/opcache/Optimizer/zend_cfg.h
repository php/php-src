/*
   +----------------------------------------------------------------------+
   | Zend Engine, CFG - Control Flow Graph                                |
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
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_CFG_H
#define ZEND_CFG_H

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
#define ZEND_BB_UNREACHABLE_FREE (1<<11) /* unreachable loop free  */
#define ZEND_BB_RECV_ENTRY       (1<<12) /* RECV entry             */

#define ZEND_BB_LOOP_HEADER      (1<<16)
#define ZEND_BB_IRREDUCIBLE_LOOP (1<<17)

#define ZEND_BB_REACHABLE        (1U<<31)

#define ZEND_BB_PROTECTED        (ZEND_BB_ENTRY|ZEND_BB_RECV_ENTRY|ZEND_BB_TRY|ZEND_BB_CATCH|ZEND_BB_FINALLY|ZEND_BB_FINALLY_END|ZEND_BB_UNREACHABLE_FREE)

typedef struct _zend_basic_block {
	int              *successors;         /* successor block indices     */
	uint32_t          flags;
	uint32_t          start;              /* first opcode number         */
	uint32_t          len;                /* number of opcodes           */
	int               successors_count;   /* number of successors        */
	int               predecessors_count; /* number of predecessors      */
	int               predecessor_offset; /* offset of 1-st predecessor  */
	int               idom;               /* immediate dominator block   */
	int               loop_header;        /* closest loop header, or -1  */
	int               level;              /* steps away from the entry in the dom. tree */
	int               children;           /* list of dominated blocks    */
	int               next_child;         /* next dominated block        */
	int               successors_storage[2]; /* up to 2 successor blocks */
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
	int               edges_count;        /* number of edges             */
	zend_basic_block *blocks;             /* array of basic blocks       */
	int              *predecessors;
	uint32_t         *map;
	uint32_t          flags;
} zend_cfg;

/* Build Flags */
#define ZEND_RT_CONSTANTS              (1U<<31)
#define ZEND_CFG_STACKLESS             (1<<30)
#define ZEND_SSA_DEBUG_LIVENESS        (1<<29)
#define ZEND_SSA_DEBUG_PHI_PLACEMENT   (1<<28)
#define ZEND_SSA_RC_INFERENCE          (1<<27)
#define ZEND_CFG_NO_ENTRY_PREDECESSORS (1<<25)
#define ZEND_CFG_RECV_ENTRY            (1<<24)
#define ZEND_CALL_TREE                 (1<<23)
#define ZEND_SSA_USE_CV_RESULTS        (1<<22)

#define CRT_CONSTANT_EX(op_array, opline, node, rt_constants) \
	((rt_constants) ? \
		RT_CONSTANT(opline, (node)) \
	: \
		CT_CONSTANT_EX(op_array, (node).constant) \
	)

#define CRT_CONSTANT(node) \
	CRT_CONSTANT_EX(op_array, opline, node, (build_flags & ZEND_RT_CONSTANTS))

#define RETURN_VALUE_USED(opline) \
	((opline)->result_type != IS_UNUSED)

BEGIN_EXTERN_C()

int  zend_build_cfg(zend_arena **arena, const zend_op_array *op_array, uint32_t build_flags, zend_cfg *cfg);
void zend_cfg_remark_reachable_blocks(const zend_op_array *op_array, zend_cfg *cfg);
int  zend_cfg_build_predecessors(zend_arena **arena, zend_cfg *cfg);
int  zend_cfg_compute_dominators_tree(const zend_op_array *op_array, zend_cfg *cfg);
int  zend_cfg_identify_loops(const zend_op_array *op_array, zend_cfg *cfg);

END_EXTERN_C()

#endif /* ZEND_CFG_H */
