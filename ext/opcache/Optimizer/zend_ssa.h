/*
   +----------------------------------------------------------------------+
   | Zend Engine, SSA - Static Single Assignment Form                     |
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

#ifndef ZEND_SSA_H
#define ZEND_SSA_H

#include "zend_optimizer.h"
#include "zend_cfg.h"

typedef struct _zend_ssa_range {
	zend_long              min;
	zend_long              max;
	zend_bool              underflow;
	zend_bool              overflow;
} zend_ssa_range;

typedef enum _zend_ssa_negative_lat {
	NEG_NONE      = 0,
	NEG_INIT      = 1,
	NEG_INVARIANT = 2,
	NEG_USE_LT    = 3,
	NEG_USE_GT    = 4,
	NEG_UNKNOWN   = 5
} zend_ssa_negative_lat;

/* Special kind of SSA Phi function used in eSSA */
typedef struct _zend_ssa_range_constraint {
	zend_ssa_range         range;       /* simple range constraint */
	int                    min_var;
	int                    max_var;
	int                    min_ssa_var; /* ((min_var>0) ? MIN(ssa_var) : 0) + range.min */
	int                    max_ssa_var; /* ((max_var>0) ? MAX(ssa_var) : 0) + range.max */
	zend_ssa_negative_lat  negative;
} zend_ssa_range_constraint;

typedef struct _zend_ssa_type_constraint {
	uint32_t               type_mask;   /* Type mask to intersect with */
	zend_class_entry      *ce;          /* Class entry for instanceof constraints */
} zend_ssa_type_constraint;

typedef union _zend_ssa_pi_constraint {
	zend_ssa_range_constraint range;
	zend_ssa_type_constraint type;
} zend_ssa_pi_constraint;

/* SSA Phi - ssa_var = Phi(source0, source1, ...sourceN) */
typedef struct _zend_ssa_phi zend_ssa_phi;
struct _zend_ssa_phi {
	zend_ssa_phi          *next;          /* next Phi in the same BB */
	int                    pi;            /* if >= 0 this is actually a e-SSA Pi */
	zend_ssa_pi_constraint constraint;    /* e-SSA Pi constraint */
	int                    var;           /* Original CV, VAR or TMP variable index */
	int                    ssa_var;       /* SSA variable index */
	int                    block;         /* current BB index */
	int                    visited : 1;   /* flag to avoid recursive processing */
	int                    has_range_constraint : 1;
	zend_ssa_phi         **use_chains;
	zend_ssa_phi          *sym_use_chain;
	int                   *sources;       /* Array of SSA IDs that produce this var.
									         As many as this block has
									         predecessors.  */
};

typedef struct _zend_ssa_block {
	zend_ssa_phi          *phis;
} zend_ssa_block;

typedef struct _zend_ssa_op {
	int                    op1_use;
	int                    op2_use;
	int                    result_use;
	int                    op1_def;
	int                    op2_def;
	int                    result_def;
	int                    op1_use_chain;
	int                    op2_use_chain;
	int                    res_use_chain;
} zend_ssa_op;

typedef enum _zend_ssa_alias_kind {
	NO_ALIAS,
	SYMTABLE_ALIAS,
	PHP_ERRORMSG_ALIAS,
	HTTP_RESPONSE_HEADER_ALIAS
} zend_ssa_alias_kind;

typedef enum _zend_ssa_escape_state {
	ESCAPE_STATE_UNKNOWN,
	ESCAPE_STATE_NO_ESCAPE,
	ESCAPE_STATE_FUNCTION_ESCAPE,
	ESCAPE_STATE_GLOBAL_ESCAPE
} zend_ssa_escape_state;

typedef struct _zend_ssa_var {
	int                    var;            /* original var number; op.var for CVs and following numbers for VARs and TMP_VARs */
	int                    scc;            /* strongly connected component */
	int                    definition;     /* opcode that defines this value */
	zend_ssa_phi          *definition_phi; /* phi that defines this value */
	int                    use_chain;      /* uses of this value, linked through opN_use_chain */
	zend_ssa_phi          *phi_use_chain;  /* uses of this value in Phi, linked through use_chain */
	zend_ssa_phi          *sym_use_chain;  /* uses of this value in Pi constraints */
	unsigned int           no_val : 1;     /* value doesn't mater (used as op1 in ZEND_ASSIGN) */
	unsigned int           scc_entry : 1;
	unsigned int           alias : 2;  /* value may be changed indirectly */
	unsigned int           escape_state : 2;
} zend_ssa_var;

typedef struct _zend_ssa_var_info {
	uint32_t               type; /* inferred type (see zend_inference.h) */
	zend_ssa_range         range;
	zend_class_entry      *ce;
	unsigned int           has_range : 1;
	unsigned int           is_instanceof : 1; /* 0 - class == "ce", 1 - may be child of "ce" */
	unsigned int           recursive : 1;
	unsigned int           use_as_double : 1;
} zend_ssa_var_info;

typedef struct _zend_ssa {
	zend_cfg               cfg;            /* control flow graph             */
	int                    rt_constants;   /* run-time or compile-time       */
	int                    vars_count;     /* number of SSA variables        */
	zend_ssa_block        *blocks;         /* array of SSA blocks            */
	zend_ssa_op           *ops;            /* array of SSA instructions      */
	zend_ssa_var          *vars;           /* use/def chain of SSA variables */
	int                    sccs;           /* number of SCCs                 */
	zend_ssa_var_info     *var_info;
} zend_ssa;

BEGIN_EXTERN_C()

int zend_build_ssa(zend_arena **arena, const zend_script *script, const zend_op_array *op_array, uint32_t build_flags, zend_ssa *ssa);
int zend_ssa_compute_use_def_chains(zend_arena **arena, const zend_op_array *op_array, zend_ssa *ssa);
int zend_ssa_unlink_use_chain(zend_ssa *ssa, int op, int var);

void zend_ssa_remove_predecessor(zend_ssa *ssa, int from, int to);
void zend_ssa_remove_instr(zend_ssa *ssa, zend_op *opline, zend_ssa_op *ssa_op);
void zend_ssa_remove_phi(zend_ssa *ssa, zend_ssa_phi *phi);
void zend_ssa_remove_uses_of_var(zend_ssa *ssa, int var_num);
void zend_ssa_remove_block(zend_op_array *op_array, zend_ssa *ssa, int b);
void zend_ssa_rename_var_uses(zend_ssa *ssa, int old_var, int new_var, zend_bool update_types);

static zend_always_inline void _zend_ssa_remove_def(zend_ssa_var *var)
{
	ZEND_ASSERT(var->definition >= 0);
	ZEND_ASSERT(var->use_chain < 0);
	ZEND_ASSERT(!var->phi_use_chain);
	var->definition = -1;
}

static zend_always_inline void zend_ssa_remove_result_def(zend_ssa *ssa, zend_ssa_op *ssa_op)
{
	zend_ssa_var *var = &ssa->vars[ssa_op->result_def];
	_zend_ssa_remove_def(var);
	ssa_op->result_def = -1;
}

static zend_always_inline void zend_ssa_remove_op1_def(zend_ssa *ssa, zend_ssa_op *ssa_op)
{
	zend_ssa_var *var = &ssa->vars[ssa_op->op1_def];
	_zend_ssa_remove_def(var);
	ssa_op->op1_def = -1;
}

static zend_always_inline void zend_ssa_remove_op2_def(zend_ssa *ssa, zend_ssa_op *ssa_op)
{
	zend_ssa_var *var = &ssa->vars[ssa_op->op2_def];
	_zend_ssa_remove_def(var);
	ssa_op->op2_def = -1;
}

END_EXTERN_C()

static zend_always_inline int zend_ssa_next_use(const zend_ssa_op *ssa_op, int var, int use)
{
	ssa_op += use;
	if (ssa_op->op1_use == var) {
		return ssa_op->op1_use_chain;
	} else if (ssa_op->op2_use == var) {
		return ssa_op->op2_use_chain;
	} else {
		return ssa_op->res_use_chain;
	}
}

static zend_always_inline zend_ssa_phi* zend_ssa_next_use_phi(const zend_ssa *ssa, int var, const zend_ssa_phi *p)
{
	if (p->pi >= 0) {
		return p->use_chains[0];
	} else {
		int j;
		for (j = 0; j < ssa->cfg.blocks[p->block].predecessors_count; j++) {
			if (p->sources[j] == var) {
				return p->use_chains[j];
			}
		}
	}
	return NULL;
}

static zend_always_inline zend_bool zend_ssa_is_no_val_use(const zend_op *opline, const zend_ssa_op *ssa_op, int var)
{
	if (opline->opcode == ZEND_ASSIGN || opline->opcode == ZEND_UNSET_CV) {
		return ssa_op->op1_use == var && ssa_op->op2_use != var;
	}
	// TODO: Reenable this after changing the SSA structure.
	/*if (opline->opcode == ZEND_FE_FETCH_R) {
		return ssa_op->op2_use == var && ssa_op->op1_use != var;
	}*/
	if (ssa_op->result_use == var
			&& opline->opcode != ZEND_ADD_ARRAY_ELEMENT
			&& opline->opcode != ZEND_ADD_ARRAY_UNPACK) {
		return ssa_op->op1_use != var && ssa_op->op2_use != var;
	}
	return 0;
}

static zend_always_inline void zend_ssa_rename_defs_of_instr(zend_ssa *ssa, zend_ssa_op *ssa_op) {
	/* Rename def to use if possible. Mark variable as not defined otherwise. */
	if (ssa_op->op1_def >= 0) {
		if (ssa_op->op1_use >= 0) {
			zend_ssa_rename_var_uses(ssa, ssa_op->op1_def, ssa_op->op1_use, 1);
		}
		ssa->vars[ssa_op->op1_def].definition = -1;
		ssa_op->op1_def = -1;
	}
	if (ssa_op->op2_def >= 0) {
		if (ssa_op->op2_use >= 0) {
			zend_ssa_rename_var_uses(ssa, ssa_op->op2_def, ssa_op->op2_use, 1);
		}
		ssa->vars[ssa_op->op2_def].definition = -1;
		ssa_op->op2_def = -1;
	}
	if (ssa_op->result_def >= 0) {
		if (ssa_op->result_use >= 0) {
			zend_ssa_rename_var_uses(ssa, ssa_op->result_def, ssa_op->result_use, 1);
		}
		ssa->vars[ssa_op->result_def].definition = -1;
		ssa_op->result_def = -1;
	}
}

#define NUM_PHI_SOURCES(phi) \
	((phi)->pi >= 0 ? 1 : (ssa->cfg.blocks[(phi)->block].predecessors_count))

/* FOREACH_USE and FOREACH_PHI_USE explicitly support "continue"
 * and changing the use chain of the current element */
#define FOREACH_USE(var, use) do { \
	int _var_num = (var) - ssa->vars, next; \
	for (use = (var)->use_chain; use >= 0; use = next) { \
		next = zend_ssa_next_use(ssa->ops, _var_num, use);
#define FOREACH_USE_END() \
	} \
} while (0)

#define FOREACH_PHI_USE(var, phi) do { \
	int _var_num = (var) - ssa->vars; \
	zend_ssa_phi *next_phi; \
	for (phi = (var)->phi_use_chain; phi; phi = next_phi) { \
		next_phi = zend_ssa_next_use_phi(ssa, _var_num, phi);
#define FOREACH_PHI_USE_END() \
	} \
} while (0)

#define FOREACH_PHI_SOURCE(phi, source) do { \
	zend_ssa_phi *_phi = (phi); \
	int _i, _end = NUM_PHI_SOURCES(phi); \
	for (_i = 0; _i < _end; _i++) { \
		ZEND_ASSERT(_phi->sources[_i] >= 0); \
		source = _phi->sources[_i];
#define FOREACH_PHI_SOURCE_END() \
	} \
} while (0)

#define FOREACH_PHI(phi) do { \
	int _i; \
	for (_i = 0; _i < ssa->cfg.blocks_count; _i++) { \
		phi = ssa->blocks[_i].phis; \
		for (; phi; phi = phi->next) {
#define FOREACH_PHI_END() \
		} \
	} \
} while (0)

#define FOREACH_BLOCK(block) do { \
	int _i; \
	for (_i = 0; _i < ssa->cfg.blocks_count; _i++) { \
		(block) = &ssa->cfg.blocks[_i]; \
		if (!((block)->flags & ZEND_BB_REACHABLE)) { \
			continue; \
		}
#define FOREACH_BLOCK_END() \
	} \
} while (0)

/* Does not support "break" */
#define FOREACH_INSTR_NUM(i) do { \
	zend_basic_block *_block; \
	FOREACH_BLOCK(_block) { \
		uint32_t _end = _block->start + _block->len; \
		for ((i) = _block->start; (i) < _end; (i)++) {
#define FOREACH_INSTR_NUM_END() \
		} \
	} FOREACH_BLOCK_END(); \
} while (0)

#endif /* ZEND_SSA_H */
