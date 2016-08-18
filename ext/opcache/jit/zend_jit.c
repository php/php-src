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
   | Authors: Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

#include <ZendAccelerator.h>
#include "jit/zend_jit.h"

#ifdef HAVE_JIT

#include "Optimizer/zend_ssa.h"

// TODO: define DASM_M_GROW and DASM_M_FREE to use CG(arena) ???

#include "dynasm/dasm_proto.h"
#include "dynasm/dasm_x86.h"
#include "jit/zend_jit_x86.c"

#if _WIN32
# include <Windows.h>
#else
# include <sys/mman.h>
# if !defined(MAP_ANONYMOUS) && defined(MAP_ANON)
#   define MAP_ANONYMOUS MAP_ANON
# endif
#endif

#define DASM_ALIGNMENT 16

static void *dasm_buf = NULL;
static void *dasm_ptr = NULL;
static void *dasm_end = NULL;

static void *dasm_link_and_encode(dasm_State **dasm_state)
{
	size_t size;
	int ret;
	void *entry;

	if (dasm_link(dasm_state, &size) != DASM_S_OK) {
		// TODO: dasm_link() failed ???
		return NULL;
	}
	
	size = ZEND_MM_ALIGNED_SIZE_EX(size, DASM_ALIGNMENT);

	if ((void*)((char*)dasm_ptr + size) > dasm_end) {
		// TODO: jit_buffer_size overflow ???
		return NULL;
	}

#ifdef HAVE_MPROTECT
//	mprotect(dasm_ptr, size, PROT_READ | PROT_WRITE);
//???	mprotect(dasm_buf, ((char*)dasm_end) - ((char*)dasm_buf), PROT_READ | PROT_WRITE);
#endif

	ret = dasm_encode(dasm_state, dasm_ptr);

#ifdef HAVE_MPROTECT
//	mprotect(dasm_ptr, size, PROT_READ | PROT_EXEC);
//???	mprotect(dasm_buf, ((char*)dasm_end) - ((char*)dasm_buf), PROT_READ | PROT_EXEC);
#endif

	if (ret != DASM_S_OK) {
		// TODO: dasm_encode() failed ???
		return NULL;
	}
	
	entry = dasm_ptr;
	dasm_ptr = (void*)((char*)dasm_ptr + size);
	
	return entry;
}

static size_t jit_page_size(void)
{
#ifdef _WIN32
	SYSTEM_INFO system_info;
	GetSystemInfo(&system_info);
	return system_info.dwPageSize;
#else
	return getpagesize();
#endif
}

static void *jit_alloc(size_t size, int shared) 
{
#ifdef _WIN32
	return VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
#else
	void *p;

# ifdef MAP_HUGETLB
	p = mmap(NULL, size,
//???#  ifdef HAVE_MPROTECT
//???			PROT_NONE,
//???#  else
			PROT_EXEC | PROT_READ | PROT_WRITE,
//???#  endif
			(shared ? MAP_SHARED : MAP_PRIVATE) | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
    if (p != MAP_FAILED) {
    	return (void*)p;
	}
# endif
	p = mmap(NULL, size,
//???#  ifdef HAVE_MPROTECT
//???			PROT_NONE,
//???#  else
			PROT_EXEC | PROT_READ | PROT_WRITE,
//???#  endif
			(shared ? MAP_SHARED : MAP_PRIVATE) | MAP_ANONYMOUS, -1, 0);
    if (p == MAP_FAILED) {
    	return NULL;
	}

    return (void*)p;
#endif
}

static void jit_free(void *p, size_t size)
{
#ifdef _WIN32
    VirtualFree(p, 0, MEM_RELEASE); 
#else
	munmap(p, size);
#endif
}
  
ZEND_API int zend_jit(zend_op_array *op_array, zend_persistent_script* main_persistent_script)
{	
	uint32_t flags;
	zend_ssa ssa;
	void *checkpoint;
	int b, i, end;
	zend_op *opline;
	dasm_State* dasm_state = NULL;
	void *handler;

	if (!dasm_buf) {
		return FAILURE;
	}

	checkpoint = zend_arena_checkpoint(CG(arena));

    /* Build SSA */
	memset(&ssa, 0, sizeof(zend_ssa));

	if (zend_build_cfg(&CG(arena), op_array, ZEND_RT_CONSTANTS | ZEND_SSA_RC_INFERENCE, &ssa.cfg, &flags) != SUCCESS) {
		goto jit_failure;
	}

	if (zend_cfg_build_predecessors(&CG(arena), &ssa.cfg) != SUCCESS) {
		goto jit_failure;
	}

	/* Compute Dominators Tree */
	if (zend_cfg_compute_dominators_tree(op_array, &ssa.cfg) != SUCCESS) {
		goto jit_failure;
	}

	/* Identify reducible and irreducible loops */
	if (zend_cfg_identify_loops(op_array, &ssa.cfg, &flags) != SUCCESS) {
		goto jit_failure;
	}

	dasm_init(&dasm_state, DASM_MAXSECTION);
	dasm_setupglobal(&dasm_state, dasm_labels, lbl__MAX);
	dasm_setup(&dasm_state, dasm_actions);

	dasm_growpc(&dasm_state, ssa.cfg.blocks_count * 2);

	zend_jit_align_func(&dasm_state);
	for (b = 0; b < ssa.cfg.blocks_count; b++) {
		if ((ssa.cfg.blocks[b].flags & ZEND_BB_REACHABLE) == 0) {
			continue;
		}
		if (ssa.cfg.blocks[b].flags & (ZEND_BB_START|ZEND_BB_ENTRY)) {
			zend_jit_label(&dasm_state, ssa.cfg.blocks_count + b);
			zend_jit_prologue(&dasm_state);
		}
		zend_jit_label(&dasm_state, b);
		if (ssa.cfg.blocks[b].flags & ZEND_BB_LOOP_HEADER) {
			if (!zend_jit_check_timeout(&dasm_state)) {
				goto jit_failure;
			}
		}
		if (!ssa.cfg.blocks[b].len) {
			continue;
		}
		end = ssa.cfg.blocks[b].start + ssa.cfg.blocks[b].len - 1;
		for (i = ssa.cfg.blocks[b].start; i <= end; i++) {
			opline = op_array->opcodes + i;
			switch (opline->opcode) {
				case ZEND_RECV:
					/* RECV may be skipped */
					if (!zend_jit_recv(&dasm_state, opline)) {
						goto jit_failure;
					}
					break;
				case ZEND_RECV_INIT:
				case ZEND_BIND_GLOBAL:
					if (opline->opcode != op_array->opcodes[i+1].opcode) {
						/* repeatable opcodes */
						if (!zend_jit_handler(&dasm_state, opline)) {
							goto jit_failure;
						}
					}
					break;
				case ZEND_NOP:
					if (!zend_jit_skip_handler(&dasm_state)) {
						goto jit_failure;
					}
					break;
				case ZEND_OP_DATA:
					break;
				case ZEND_JMP:
					if (!zend_jit_set_opline(&dasm_state, OP_JMP_ADDR(opline, opline->op1)) ||
					    !zend_jit_jmp(&dasm_state, ssa.cfg.blocks[b].successors[0])) {
						goto jit_failure;
					}
					break;
				case ZEND_CATCH:
				case ZEND_FAST_CALL:
				case ZEND_FAST_RET:
				case ZEND_GENERATOR_CREATE:
				case ZEND_GENERATOR_RETURN:
				case ZEND_RETURN_BY_REF:
				case ZEND_RETURN:
				case ZEND_EXIT:
				/* switch through trampoline */
				case ZEND_YIELD:
				case ZEND_YIELD_FROM:
					if (!zend_jit_tail_handler(&dasm_state, opline)) {
						goto jit_failure;
					}
					break;
				/* stackless execution */
				case ZEND_INCLUDE_OR_EVAL:
				case ZEND_DO_FCALL:
				case ZEND_DO_UCALL:
				case ZEND_DO_FCALL_BY_NAME:
					if (!zend_jit_call(&dasm_state, opline)) {
						goto jit_failure;
					}
					break;
				case ZEND_JMPZ:
				case ZEND_JMPNZ:
					if (opline != 0) {
						if ((opline-1)->opcode == ZEND_IS_EQUAL ||
						    (opline-1)->opcode == ZEND_IS_NOT_EQUAL ||
						    (opline-1)->opcode == ZEND_IS_SMALLER ||
						    (opline-1)->opcode == ZEND_IS_SMALLER_OR_EQUAL ||
						    (opline-1)->opcode == ZEND_CASE) {
							/* might be smart branch */
							if (!zend_jit_smart_branch(&dasm_state, opline, (b + 1), ssa.cfg.blocks[b].successors[0])) {
								goto jit_failure;
							}
							/* break missing intentionally */
						} else if ((opline-1)->opcode == ZEND_IS_IDENTICAL ||
						           (opline-1)->opcode == ZEND_IS_NOT_IDENTICAL ||
						           (opline-1)->opcode == ZEND_ISSET_ISEMPTY_VAR ||
						           (opline-1)->opcode == ZEND_ISSET_ISEMPTY_STATIC_PROP ||
						           (opline-1)->opcode == ZEND_ISSET_ISEMPTY_DIM_OBJ ||
						           (opline-1)->opcode == ZEND_ISSET_ISEMPTY_PROP_OBJ ||
						           (opline-1)->opcode == ZEND_INSTANCEOF ||
						           (opline-1)->opcode == ZEND_TYPE_CHECK ||
						           (opline-1)->opcode == ZEND_DEFINED) {
						    /* smart branch */
							if (!zend_jit_cond_jmp(&dasm_state, opline + 1, ssa.cfg.blocks[b].successors[0])) {
								goto jit_failure;
			            	}
			            	break;
						}
					}
					/* break missing intentionally */
				case ZEND_JMPZ_EX:
				case ZEND_JMPNZ_EX:
				case ZEND_JMP_SET:
				case ZEND_COALESCE:
				case ZEND_FE_RESET_R:
				case ZEND_FE_RESET_RW:
				case ZEND_ASSERT_CHECK:
					if (!zend_jit_handler(&dasm_state, opline) ||
					    !zend_jit_cond_jmp(&dasm_state, opline + 1, ssa.cfg.blocks[b].successors[0])) {
						goto jit_failure;
					}
					break;
				case ZEND_NEW:
					if (!zend_jit_new(&dasm_state, opline)) {
						goto jit_failure;
					}
					if (EXPECTED(opline->extended_value == 0 && (opline+1)->opcode == ZEND_DO_FCALL)) {
						i++;
					}
					break;
				case ZEND_JMPZNZ:
					if (!zend_jit_handler(&dasm_state, opline) ||
					    !zend_jit_cond_jmp(&dasm_state, OP_JMP_ADDR(opline, opline->op2), ssa.cfg.blocks[b].successors[1]) ||
					    !zend_jit_jmp(&dasm_state, ssa.cfg.blocks[b].successors[0])) {
						goto jit_failure;
					}
					break;
				case ZEND_FE_FETCH_R:
				case ZEND_FE_FETCH_RW:
				case ZEND_DECLARE_ANON_CLASS:
				case ZEND_DECLARE_ANON_INHERITED_CLASS:
				if (!zend_jit_handler(&dasm_state, opline) ||
					    !zend_jit_cond_jmp(&dasm_state, opline + 1, ssa.cfg.blocks[b].successors[0])) {
						goto jit_failure;
					}
					break;
				default:
					if (!zend_jit_handler(&dasm_state, opline)) {
						goto jit_failure;
					}
			}
		}
	}

	handler = dasm_link_and_encode(&dasm_state);
	if (!handler) {
		goto jit_failure;
	}
	for (b = 0; b < ssa.cfg.blocks_count; b++) {
		if (ssa.cfg.blocks[b].flags & (ZEND_BB_START|ZEND_BB_ENTRY)) {
			opline = op_array->opcodes + ssa.cfg.blocks[b].start;
			opline->handler = (void*)(((char*)handler) +
				dasm_getpclabel(&dasm_state, ssa.cfg.blocks_count + b));
		}
	}
	dasm_free(&dasm_state);	
	zend_arena_release(&CG(arena), checkpoint);
	return SUCCESS;

jit_failure:
    if (dasm_state) {
		dasm_free(&dasm_state);
    }
	zend_arena_release(&CG(arena), checkpoint);
	return FAILURE;
}

ZEND_API int zend_jit_startup(size_t size)
{
	size_t page_size = jit_page_size();
	int shared = 1;
	void *buf;

	/* Round up to the page size, which should be a power of two.  */
	page_size = jit_page_size();
	if (!page_size || (page_size & (page_size - 1))) {
		abort();
	}
	size = (size + page_size - 1) & ~(page_size - 1);

	buf = jit_alloc(size, shared);

	if (!buf) {
		return FAILURE;
	}

	dasm_buf = dasm_ptr = buf;
	dasm_end = (void*)(((char*)dasm_buf)+size);

	return SUCCESS;
}

ZEND_API void zend_jit_shutdown(void)
{
	if (dasm_buf) {
		jit_free(dasm_buf, ((char*)dasm_end) - ((char*)dasm_buf));
	}
}

#else /* HAVE_JIT */

ZEND_API int zend_jit(zend_op_array *op_array, zend_persistent_script* main_persistent_script)
{
	return FAILURE;
}

ZEND_API int zend_jit_startup(size_t size)
{
	return FAILURE;
}

ZEND_API void zend_jit_shutdown(void)
{
}

#endif /* HAVE_JIT */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
