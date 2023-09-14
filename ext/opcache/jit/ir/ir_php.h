/*
 * IR - Lightweight JIT Compilation Framework
 * (IR/PHP integration)
 * Copyright (C) 2022 Zend by Perforce.
 * Authors: Dmitry Stogov <dmitry@php.net>
 */

#ifndef IR_PHP_H
#define IR_PHP_H

#define IR_PHP_OPS(_)


#define IR_SNAPSHOT_HANDLER_DCL() \
	void *zend_jit_snapshot_handler(ir_ctx *ctx, ir_ref snapshot_ref, ir_insn *snapshot, void *addr)

#define IR_SNAPSHOT_HANDLER(ctx, ref, insn, addr) \
	zend_jit_snapshot_handler(ctx, ref, insn, addr)

#ifndef IR_PHP_MM
# define IR_PHP_MM 1
#endif

#if IR_PHP_MM
# include "zend.h"

# define ir_mem_malloc  emalloc
# define ir_mem_calloc  ecalloc
# define ir_mem_realloc erealloc
# define ir_mem_free    efree
#endif

#if defined(IR_TARGET_AARCH64)
# define IR_EXTERNAL_GDB_ENTRY
#endif

#endif /* IR_PHP_H */
