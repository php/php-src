/**********************************************************************

  iseq.h -

  $Author$
  created at: 04/01/01 23:36:57 JST

  Copyright (C) 2004-2008 Koichi Sasada

**********************************************************************/

#ifndef RUBY_COMPILE_H
#define RUBY_COMPILE_H

#if defined __GNUC__ && __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

/* compile.c */
VALUE rb_iseq_compile_node(VALUE self, NODE *node);
int rb_iseq_translate_threaded_code(rb_iseq_t *iseq);
VALUE rb_iseq_build_from_ary(rb_iseq_t *iseq, VALUE locals, VALUE args,
			     VALUE exception, VALUE body);

/* iseq.c */
VALUE rb_iseq_load(VALUE data, VALUE parent, VALUE opt);
VALUE rb_iseq_parameters(const rb_iseq_t *iseq, int is_proc);
struct st_table *ruby_insn_make_insn_table(void);
unsigned int rb_iseq_line_no(const rb_iseq_t *iseq, size_t pos);

/* proc.c */
rb_iseq_t *rb_method_get_iseq(VALUE body);
rb_iseq_t *rb_proc_get_iseq(VALUE proc, int *is_proc);

struct rb_compile_option_struct {
    int inline_const_cache;
    int peephole_optimization;
    int tailcall_optimization;
    int specialized_instruction;
    int operands_unification;
    int instructions_unification;
    int stack_caching;
    int trace_instruction;
    int debug_level;
};

struct iseq_line_info_entry {
    unsigned int position;
    unsigned int line_no;
};

struct iseq_catch_table_entry {
    enum catch_type {
	CATCH_TYPE_RESCUE,
	CATCH_TYPE_ENSURE,
	CATCH_TYPE_RETRY,
	CATCH_TYPE_BREAK,
	CATCH_TYPE_REDO,
	CATCH_TYPE_NEXT
    } type;
    VALUE iseq;
    unsigned long start;
    unsigned long end;
    unsigned long cont;
    unsigned long sp;
};

#define INITIAL_ISEQ_COMPILE_DATA_STORAGE_BUFF_SIZE (512)

struct iseq_compile_data_storage {
    struct iseq_compile_data_storage *next;
    unsigned long pos;
    unsigned long size;
    char *buff;
};

struct iseq_compile_data {
    /* GC is needed */
    VALUE err_info;
    VALUE mark_ary;
    VALUE catch_table_ary;	/* Array */

    /* GC is not needed */
    struct iseq_label_data *start_label;
    struct iseq_label_data *end_label;
    struct iseq_label_data *redo_label;
    VALUE current_block;
    VALUE ensure_node;
    VALUE for_iseq;
    struct iseq_compile_data_ensure_node_stack *ensure_node_stack;
    int loopval_popped;	/* used by NODE_BREAK */
    int cached_const;
    struct iseq_compile_data_storage *storage_head;
    struct iseq_compile_data_storage *storage_current;
    int last_line;
    int last_coverable_line;
    int flip_cnt;
    int label_no;
    int node_level;
    const rb_compile_option_t *option;
#if SUPPORT_JOKE
    st_table *labels_table;
#endif
};

/* defined? */

enum defined_type {
    DEFINED_IVAR = 1,
    DEFINED_IVAR2,
    DEFINED_GVAR,
    DEFINED_CVAR,
    DEFINED_CONST,
    DEFINED_METHOD,
    DEFINED_YIELD,
    DEFINED_REF,
    DEFINED_ZSUPER,
    DEFINED_FUNC
};

#if defined __GNUC__ && __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* RUBY_COMPILE_H */
