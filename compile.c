/**********************************************************************

  compile.c - ruby node tree -> VM instruction sequence

  $Author$
  created at: 04/01/01 03:42:15 JST

  Copyright (C) 2004-2007 Koichi Sasada

**********************************************************************/

#include "ruby/ruby.h"
#include "internal.h"
#include <math.h>

#define USE_INSN_STACK_INCREASE 1
#include "vm_core.h"
#include "iseq.h"
#include "insns.inc"
#include "insns_info.inc"

#define numberof(array) (int)(sizeof(array) / sizeof((array)[0]))
#define FIXNUM_INC(n, i) ((n)+(INT2FIX(i)&~FIXNUM_FLAG))
#define FIXNUM_OR(n, i) ((n)|INT2FIX(i))

typedef struct iseq_link_element {
    enum {
	ISEQ_ELEMENT_NONE,
	ISEQ_ELEMENT_LABEL,
	ISEQ_ELEMENT_INSN,
	ISEQ_ELEMENT_ADJUST
    } type;
    struct iseq_link_element *next;
    struct iseq_link_element *prev;
} LINK_ELEMENT;

typedef struct iseq_link_anchor {
    LINK_ELEMENT anchor;
    LINK_ELEMENT *last;
} LINK_ANCHOR;

typedef struct iseq_label_data {
    LINK_ELEMENT link;
    int label_no;
    int position;
    int sc_state;
    int set;
    int sp;
} LABEL;

typedef struct iseq_insn_data {
    LINK_ELEMENT link;
    enum ruby_vminsn_type insn_id;
    unsigned int line_no;
    int operand_size;
    int sc_state;
    VALUE *operands;
} INSN;

typedef struct iseq_adjust_data {
    LINK_ELEMENT link;
    LABEL *label;
    int line_no;
} ADJUST;

struct ensure_range {
    LABEL *begin;
    LABEL *end;
    struct ensure_range *next;
};

struct iseq_compile_data_ensure_node_stack {
    NODE *ensure_node;
    struct iseq_compile_data_ensure_node_stack *prev;
    struct ensure_range *erange;
};

/**
 * debug function(macro) interface depend on CPDEBUG
 * if it is less than 0, runtime option is in effect.
 *
 * debug level:
 *  0: no debug output
 *  1: show node type
 *  2: show node important parameters
 *  ...
 *  5: show other parameters
 * 10: show every AST array
 */

#ifndef CPDEBUG
#define CPDEBUG 0
#endif

#if CPDEBUG >= 0
#define compile_debug CPDEBUG
#else
#define compile_debug iseq->compile_data->option->debug_level
#endif

#if CPDEBUG

#define compile_debug_print_indent(level) \
    ruby_debug_print_indent((level), compile_debug, gl_node_level * 2)

#define debugp(header, value) (void) \
  (compile_debug_print_indent(1) && \
   ruby_debug_print_value(1, compile_debug, (header), (value)))

#define debugi(header, id)  (void) \
  (compile_debug_print_indent(1) && \
   ruby_debug_print_id(1, compile_debug, (header), (id)))

#define debugp_param(header, value)  (void) \
  (compile_debug_print_indent(1) && \
   ruby_debug_print_value(1, compile_debug, (header), (value)))

#define debugp_verbose(header, value)  (void) \
  (compile_debug_print_indent(2) && \
   ruby_debug_print_value(2, compile_debug, (header), (value)))

#define debugp_verbose_node(header, value)  (void) \
  (compile_debug_print_indent(10) && \
   ruby_debug_print_value(10, compile_debug, (header), (value)))

#define debug_node_start(node)  ((void) \
  (compile_debug_print_indent(1) && \
   (ruby_debug_print_node(1, CPDEBUG, "", (NODE *)(node)), gl_node_level)), \
   gl_node_level++)

#define debug_node_end()  gl_node_level --;

#else

static inline ID
r_id(ID id)
{
    return id;
}

static inline VALUE
r_value(VALUE value)
{
    return value;
}

#define debugi(header, id)                 r_id(id)
#define debugp(header, value)              r_value(value)
#define debugp_verbose(header, value)      r_value(value)
#define debugp_verbose_node(header, value) r_value(value)
#define debugp_param(header, value)        r_value(value)
#define debug_node_start(node)             ((void)0)
#define debug_node_end()                   ((void)0)
#endif

#if CPDEBUG > 1 || CPDEBUG < 0
#define debugs if (compile_debug_print_indent(1)) ruby_debug_printf
#define debug_compile(msg, v) ((void)(compile_debug_print_indent(1) && fputs((msg), stderr)), (v))
#else
#define debugs                             if(0)printf
#define debug_compile(msg, v) (v)
#endif


/* create new label */
#define NEW_LABEL(l) new_label_body(iseq, (l))

#define iseq_filename(iseq) \
  (((rb_iseq_t*)DATA_PTR(iseq))->filename)

#define iseq_filepath(iseq) \
  (((rb_iseq_t*)DATA_PTR(iseq))->filepath)

#define NEW_ISEQVAL(node, name, type, line_no)       \
  new_child_iseq(iseq, (node), (name), 0, (type), (line_no))

#define NEW_CHILD_ISEQVAL(node, name, type, line_no)       \
  new_child_iseq(iseq, (node), (name), iseq->self, (type), (line_no))

/* add instructions */
#define ADD_SEQ(seq1, seq2) \
  APPEND_LIST((seq1), (seq2))

/* add an instruction */
#define ADD_INSN(seq, line, insn) \
  ADD_ELEM((seq), (LINK_ELEMENT *) new_insn_body(iseq, (line), BIN(insn), 0))

/* add an instruction with label operand */
#define ADD_INSNL(seq, line, insn, label) \
  ADD_ELEM((seq), (LINK_ELEMENT *) \
           new_insn_body(iseq, (line), BIN(insn), 1, (VALUE)(label)))

/* add an instruction with some operands (1, 2, 3, 5) */
#define ADD_INSN1(seq, line, insn, op1) \
  ADD_ELEM((seq), (LINK_ELEMENT *) \
           new_insn_body(iseq, (line), BIN(insn), 1, (VALUE)(op1)))

#define ADD_INSN2(seq, line, insn, op1, op2) \
  ADD_ELEM((seq), (LINK_ELEMENT *) \
           new_insn_body(iseq, (line), BIN(insn), 2, (VALUE)(op1), (VALUE)(op2)))

#define ADD_INSN3(seq, line, insn, op1, op2, op3) \
  ADD_ELEM((seq), (LINK_ELEMENT *) \
           new_insn_body(iseq, (line), BIN(insn), 3, (VALUE)(op1), (VALUE)(op2), (VALUE)(op3)))

/* Specific Insn factory */
#define ADD_SEND(seq, line, id, argc) \
  ADD_SEND_R((seq), (line), (id), (argc), (VALUE)Qfalse, (VALUE)INT2FIX(0))

#define ADD_CALL_RECEIVER(seq, line) \
  ADD_INSN((seq), (line), putself)

#define ADD_CALL(seq, line, id, argc) \
  ADD_SEND_R((seq), (line), (id), (argc), (VALUE)Qfalse, (VALUE)INT2FIX(VM_CALL_FCALL_BIT))

#define ADD_CALL_WITH_BLOCK(seq, line, id, argc, block) \
  ADD_SEND_R((seq), (line), (id), (argc), (block), (VALUE)INT2FIX(VM_CALL_FCALL_BIT))

#define ADD_SEND_R(seq, line, id, argc, block, flag) \
  ADD_ELEM((seq), (LINK_ELEMENT *) \
           new_insn_send(iseq, (line), \
                         (VALUE)(id), (VALUE)(argc), (VALUE)(block), (VALUE)(flag)))

#define ADD_TRACE(seq, line, event) \
  do { \
      if ((event) == RUBY_EVENT_LINE && iseq->coverage && \
	  (line) != iseq->compile_data->last_coverable_line) { \
	  RARRAY_PTR(iseq->coverage)[(line) - 1] = INT2FIX(0); \
	  iseq->compile_data->last_coverable_line = (line); \
	  ADD_INSN1((seq), (line), trace, INT2FIX(RUBY_EVENT_COVERAGE)); \
      } \
      if (iseq->compile_data->option->trace_instruction) { \
	  ADD_INSN1((seq), (line), trace, INT2FIX(event)); \
      } \
  }while(0);

/* add label */
#define ADD_LABEL(seq, label) \
  ADD_ELEM((seq), (LINK_ELEMENT *) (label))

#define APPEND_LABEL(seq, before, label) \
  APPEND_ELEM((seq), (before), (LINK_ELEMENT *) (label))

#define ADD_ADJUST(seq, line, label) \
  ADD_ELEM((seq), (LINK_ELEMENT *) new_adjust_body(iseq, (label), (line)))

#define ADD_ADJUST_RESTORE(seq, label) \
  ADD_ELEM((seq), (LINK_ELEMENT *) new_adjust_body(iseq, (label), -1))

#define ADD_CATCH_ENTRY(type, ls, le, iseqv, lc)		\
    (rb_ary_push(iseq->compile_data->catch_table_ary,		\
		 rb_ary_new3(5, (type),				\
			     (VALUE)(ls) | 1, (VALUE)(le) | 1,	\
			     (iseqv), (VALUE)(lc) | 1)))

/* compile node */
#define COMPILE(anchor, desc, node) \
  (debug_compile("== " desc "\n", \
                 iseq_compile_each(iseq, (anchor), (node), 0)))

/* compile node, this node's value will be popped */
#define COMPILE_POPED(anchor, desc, node)    \
  (debug_compile("== " desc "\n", \
                 iseq_compile_each(iseq, (anchor), (node), 1)))

/* compile node, which is popped when 'poped' is true */
#define COMPILE_(anchor, desc, node, poped)  \
  (debug_compile("== " desc "\n", \
                 iseq_compile_each(iseq, (anchor), (node), (poped))))

#define OPERAND_AT(insn, idx) \
  (((INSN*)(insn))->operands[(idx)])

#define INSN_OF(insn) \
  (((INSN*)(insn))->insn_id)

/* error */
#define COMPILE_ERROR(strs)                        \
{                                                  \
  VALUE tmp = GET_THREAD()->errinfo;               \
  if (compile_debug) rb_compile_bug strs;          \
  GET_THREAD()->errinfo = iseq->compile_data->err_info;  \
  rb_compile_error strs;                           \
  iseq->compile_data->err_info = GET_THREAD()->errinfo; \
  GET_THREAD()->errinfo = tmp;                     \
  ret = 0;                                         \
  break;                                           \
}

#define ERROR_ARGS ruby_sourcefile, nd_line(node),


#define COMPILE_OK 1
#define COMPILE_NG 0


/* leave name uninitialized so that compiler warn if INIT_ANCHOR is
 * missing */
#define DECL_ANCHOR(name) \
  LINK_ANCHOR *name, name##_body__ = {{0,},}
#define INIT_ANCHOR(name) \
  (name##_body__.last = &name##_body__.anchor, name = &name##_body__)

#define hide_obj(obj) do {OBJ_FREEZE(obj); RBASIC(obj)->klass = 0;} while (0)

#include "optinsn.inc"
#if OPT_INSTRUCTIONS_UNIFICATION
#include "optunifs.inc"
#endif

/* for debug */
#if CPDEBUG < 0
#define ISEQ_ARG iseq,
#define ISEQ_ARG_DECLARE rb_iseq_t *iseq,
#else
#define ISEQ_ARG
#define ISEQ_ARG_DECLARE
#endif

#if CPDEBUG
#define gl_node_level iseq->compile_data->node_level
#if 0
static void debug_list(ISEQ_ARG_DECLARE LINK_ANCHOR *anchor);
#endif
#endif

static void dump_disasm_list(LINK_ELEMENT *elem);

static int insn_data_length(INSN *iobj);
static int insn_data_line_no(INSN *iobj);
static int calc_sp_depth(int depth, INSN *iobj);

static INSN *new_insn_body(rb_iseq_t *iseq, int line_no, int insn_id, int argc, ...);
static LABEL *new_label_body(rb_iseq_t *iseq, long line);
static ADJUST *new_adjust_body(rb_iseq_t *iseq, LABEL *label, int line);

static int iseq_compile_each(rb_iseq_t *iseq, LINK_ANCHOR *anchor, NODE * n, int);
static int iseq_setup(rb_iseq_t *iseq, LINK_ANCHOR *anchor);
static int iseq_optimize(rb_iseq_t *iseq, LINK_ANCHOR *anchor);
static int iseq_insns_unification(rb_iseq_t *iseq, LINK_ANCHOR *anchor);

static int iseq_set_local_table(rb_iseq_t *iseq, ID *tbl);
static int iseq_set_exception_local_table(rb_iseq_t *iseq);
static int iseq_set_arguments(rb_iseq_t *iseq, LINK_ANCHOR *anchor, NODE * node);

static int iseq_set_sequence_stackcaching(rb_iseq_t *iseq, LINK_ANCHOR *anchor);
static int iseq_set_sequence(rb_iseq_t *iseq, LINK_ANCHOR *anchor);
static int iseq_set_exception_table(rb_iseq_t *iseq);
static int iseq_set_optargs_table(rb_iseq_t *iseq);

/*
 * To make Array to LinkedList, use link_anchor
 */

static void
verify_list(ISEQ_ARG_DECLARE const char *info, LINK_ANCHOR *anchor)
{
#if CPDEBUG
    int flag = 0;
    LINK_ELEMENT *list, *plist;

    if (!compile_debug) return;

    list = anchor->anchor.next;
    plist = &anchor->anchor;
    while (list) {
	if (plist != list->prev) {
	    flag += 1;
	}
	plist = list;
	list = list->next;
    }

    if (anchor->last != plist && anchor->last != 0) {
	flag |= 0x70000;
    }

    if (flag != 0) {
	rb_bug("list verify error: %08x (%s)", flag, info);
    }
#endif
}
#if CPDEBUG < 0
#define verify_list(info, anchor) verify_list(iseq, (info), (anchor))
#endif

/*
 * elem1, elem2 => elem1, elem2, elem
 */
static void
ADD_ELEM(ISEQ_ARG_DECLARE LINK_ANCHOR *anchor, LINK_ELEMENT *elem)
{
    elem->prev = anchor->last;
    anchor->last->next = elem;
    anchor->last = elem;
    verify_list("add", anchor);
}

/*
 * elem1, before, elem2 => elem1, before, elem, elem2
 */
static void
APPEND_ELEM(ISEQ_ARG_DECLARE LINK_ANCHOR *anchor, LINK_ELEMENT *before, LINK_ELEMENT *elem)
{
    elem->prev = before;
    elem->next = before->next;
    elem->next->prev = elem;
    before->next = elem;
    if (before == anchor->last) anchor->last = elem;
    verify_list("add", anchor);
}
#if CPDEBUG < 0
#define ADD_ELEM(anchor, elem) ADD_ELEM(iseq, (anchor), (elem))
#define APPEND_ELEM(anchor, before, elem) ADD_ELEM(iseq, (anchor), (before), (elem))
#endif

static int
iseq_add_mark_object(rb_iseq_t *iseq, VALUE v)
{
    if (!SPECIAL_CONST_P(v)) {
	rb_ary_push(iseq->mark_ary, v);
    }
    return COMPILE_OK;
}

#define ruby_sourcefile		RSTRING_PTR(iseq->filename)

static int
iseq_add_mark_object_compile_time(rb_iseq_t *iseq, VALUE v)
{
    if (!SPECIAL_CONST_P(v)) {
	rb_ary_push(iseq->compile_data->mark_ary, v);
    }
    return COMPILE_OK;
}

static int
validate_label(st_data_t name, st_data_t label, st_data_t arg)
{
    rb_iseq_t *iseq = (rb_iseq_t *)arg;
    LABEL *lobj = (LABEL *)label;
    if (!lobj->link.next) {
	do {
	    int ret;
	    COMPILE_ERROR((ruby_sourcefile, lobj->position,
			   "%s: undefined label", rb_id2name((ID)name)));
	    if (ret) break;
	} while (0);
    }
    return ST_CONTINUE;
}

static void
validate_labels(rb_iseq_t *iseq, st_table *labels_table)
{
    st_foreach(labels_table, validate_label, (st_data_t)iseq);
    if (!NIL_P(iseq->compile_data->err_info)) {
	rb_exc_raise(iseq->compile_data->err_info);
    }
}

VALUE
rb_iseq_compile_node(VALUE self, NODE *node)
{
    DECL_ANCHOR(ret);
    rb_iseq_t *iseq;
    INIT_ANCHOR(ret);
    GetISeqPtr(self, iseq);

    if (node == 0) {
	COMPILE(ret, "nil", node);
	iseq_set_local_table(iseq, 0);
    }
    else if (nd_type(node) == NODE_SCOPE) {
	/* iseq type of top, method, class, block */
	iseq_set_local_table(iseq, node->nd_tbl);
	iseq_set_arguments(iseq, ret, node->nd_args);

	switch (iseq->type) {
	  case ISEQ_TYPE_BLOCK: {
	    LABEL *start = iseq->compile_data->start_label = NEW_LABEL(0);
	    LABEL *end = iseq->compile_data->end_label = NEW_LABEL(0);

	    ADD_LABEL(ret, start);
	    COMPILE(ret, "block body", node->nd_body);
	    ADD_LABEL(ret, end);

	    /* wide range catch handler must put at last */
	    ADD_CATCH_ENTRY(CATCH_TYPE_REDO, start, end, 0, start);
	    ADD_CATCH_ENTRY(CATCH_TYPE_NEXT, start, end, 0, end);
	    break;
	  }
	  case ISEQ_TYPE_CLASS: {
	    ADD_TRACE(ret, FIX2INT(iseq->line_no), RUBY_EVENT_CLASS);
	    COMPILE(ret, "scoped node", node->nd_body);
	    ADD_TRACE(ret, nd_line(node), RUBY_EVENT_END);
	    break;
	  }
	  case ISEQ_TYPE_METHOD: {
	    ADD_TRACE(ret, FIX2INT(iseq->line_no), RUBY_EVENT_CALL);
	    COMPILE(ret, "scoped node", node->nd_body);
	    ADD_TRACE(ret, nd_line(node), RUBY_EVENT_RETURN);
	    break;
	  }
	  default: {
	    COMPILE(ret, "scoped node", node->nd_body);
	    break;
	  }
	}
    }
    else {
	switch (iseq->type) {
	  case ISEQ_TYPE_METHOD:
	  case ISEQ_TYPE_CLASS:
	  case ISEQ_TYPE_BLOCK:
	  case ISEQ_TYPE_EVAL:
	  case ISEQ_TYPE_MAIN:
	  case ISEQ_TYPE_TOP:
	    rb_compile_error(ERROR_ARGS "compile/should not be reached: %s:%d",
			     __FILE__, __LINE__);
	    break;
	  case ISEQ_TYPE_RESCUE:
	    iseq_set_exception_local_table(iseq);
	    COMPILE(ret, "rescue", node);
	    break;
	  case ISEQ_TYPE_ENSURE:
	    iseq_set_exception_local_table(iseq);
	    COMPILE_POPED(ret, "ensure", node);
	    break;
	  case ISEQ_TYPE_DEFINED_GUARD:
	    iseq_set_local_table(iseq, 0);
	    COMPILE(ret, "defined guard", node);
	    break;
	  default:
	    rb_bug("unknown scope");
	}
    }

    if (iseq->type == ISEQ_TYPE_RESCUE || iseq->type == ISEQ_TYPE_ENSURE) {
	ADD_INSN2(ret, 0, getdynamic, INT2FIX(2), INT2FIX(0));
	ADD_INSN1(ret, 0, throw, INT2FIX(0) /* continue throw */ );
    }
    else {
	ADD_INSN(ret, iseq->compile_data->last_line, leave);
    }

#if SUPPORT_JOKE
    if (iseq->compile_data->labels_table) {
	validate_labels(iseq, iseq->compile_data->labels_table);
    }
#endif
    return iseq_setup(iseq, ret);
}

int
rb_iseq_translate_threaded_code(rb_iseq_t *iseq)
{
#if OPT_DIRECT_THREADED_CODE || OPT_CALL_THREADED_CODE
    const void * const *table = rb_vm_get_insns_address_table();
    unsigned long i;

    iseq->iseq_encoded = ALLOC_N(VALUE, iseq->iseq_size);
    MEMCPY(iseq->iseq_encoded, iseq->iseq, VALUE, iseq->iseq_size);

    for (i = 0; i < iseq->iseq_size; /* */ ) {
	int insn = (int)iseq->iseq_encoded[i];
	int len = insn_len(insn);
	iseq->iseq_encoded[i] = (VALUE)table[insn];
	i += len;
    }
#else
    iseq->iseq_encoded = iseq->iseq;
#endif
    return COMPILE_OK;
}

/*********************************************/
/* definition of data structure for compiler */
/*********************************************/

static void *
compile_data_alloc(rb_iseq_t *iseq, size_t size)
{
    void *ptr = 0;
    struct iseq_compile_data_storage *storage =
	iseq->compile_data->storage_current;

    if (storage->pos + size > storage->size) {
	unsigned long alloc_size = storage->size * 2;

      retry:
	if (alloc_size < size) {
	    alloc_size *= 2;
	    goto retry;
	}
	storage->next = (void *)ALLOC_N(char, alloc_size +
					sizeof(struct
					       iseq_compile_data_storage));
	storage = iseq->compile_data->storage_current = storage->next;
	storage->next = 0;
	storage->pos = 0;
	storage->size = alloc_size;
	storage->buff = (char *)(&storage->buff + 1);
    }

    ptr = (void *)&storage->buff[storage->pos];
    storage->pos += size;
    return ptr;
}

static INSN *
compile_data_alloc_insn(rb_iseq_t *iseq)
{
    return (INSN *)compile_data_alloc(iseq, sizeof(INSN));
}

static LABEL *
compile_data_alloc_label(rb_iseq_t *iseq)
{
    return (LABEL *)compile_data_alloc(iseq, sizeof(LABEL));
}

static ADJUST *
compile_data_alloc_adjust(rb_iseq_t *iseq)
{
    return (ADJUST *)compile_data_alloc(iseq, sizeof(ADJUST));
}

/*
 * elem1, elemX => elem1, elem2, elemX
 */
static void
INSERT_ELEM_NEXT(LINK_ELEMENT *elem1, LINK_ELEMENT *elem2)
{
    elem2->next = elem1->next;
    elem2->prev = elem1;
    elem1->next = elem2;
    if (elem2->next) {
	elem2->next->prev = elem2;
    }
}

#if 0 /* unused */
/*
 * elemX, elem1 => elemX, elem2, elem1
 */
static void
INSERT_ELEM_PREV(LINK_ELEMENT *elem1, LINK_ELEMENT *elem2)
{
    elem2->prev = elem1->prev;
    elem2->next = elem1;
    elem1->prev = elem2;
    if (elem2->prev) {
	elem2->prev->next = elem2;
    }
}
#endif

/*
 * elemX, elem1, elemY => elemX, elem2, elemY
 */
static void
REPLACE_ELEM(LINK_ELEMENT *elem1, LINK_ELEMENT *elem2)
{
    elem2->prev = elem1->prev;
    elem2->next = elem1->next;
    if (elem1->prev) {
	elem1->prev->next = elem2;
    }
    if (elem1->next) {
	elem1->next->prev = elem2;
    }
}

static void
REMOVE_ELEM(LINK_ELEMENT *elem)
{
    elem->prev->next = elem->next;
    if (elem->next) {
	elem->next->prev = elem->prev;
    }
}

static LINK_ELEMENT *
FIRST_ELEMENT(LINK_ANCHOR *anchor)
{
    return anchor->anchor.next;
}

#if 0 /* unused */
static LINK_ELEMENT *
LAST_ELEMENT(LINK_ANCHOR *anchor)
{
  return anchor->last;
}
#endif

static LINK_ELEMENT *
POP_ELEMENT(ISEQ_ARG_DECLARE LINK_ANCHOR *anchor)
{
    LINK_ELEMENT *elem = anchor->last;
    anchor->last = anchor->last->prev;
    anchor->last->next = 0;
    verify_list("pop", anchor);
    return elem;
}
#if CPDEBUG < 0
#define POP_ELEMENT(anchor) POP_ELEMENT(iseq, (anchor))
#endif

#if 0 /* unused */
static LINK_ELEMENT *
SHIFT_ELEMENT(LINK_ANCHOR *anchor)
{
    LINK_ELEMENT *elem = anchor->anchor.next;
    if (elem) {
	anchor->anchor.next = elem->next;
    }
    return elem;
}
#endif

#if 0 /* unused */
static int
LIST_SIZE(LINK_ANCHOR *anchor)
{
    LINK_ELEMENT *elem = anchor->anchor.next;
    int size = 0;
    while (elem) {
	size += 1;
	elem = elem->next;
    }
    return size;
}
#endif

static int
LIST_SIZE_ZERO(LINK_ANCHOR *anchor)
{
    if (anchor->anchor.next == 0) {
	return 1;
    }
    else {
	return 0;
    }
}

/*
 * anc1: e1, e2, e3
 * anc2: e4, e5
 *#=>
 * anc1: e1, e2, e3, e4, e5
 * anc2: e4, e5 (broken)
 */
static void
APPEND_LIST(ISEQ_ARG_DECLARE LINK_ANCHOR *anc1, LINK_ANCHOR *anc2)
{
    if (anc2->anchor.next) {
	anc1->last->next = anc2->anchor.next;
	anc2->anchor.next->prev = anc1->last;
	anc1->last = anc2->last;
    }
    verify_list("append", anc1);
}
#if CPDEBUG < 0
#define APPEND_LIST(anc1, anc2) APPEND_LIST(iseq, (anc1), (anc2))
#endif

/*
 * anc1: e1, e2, e3
 * anc2: e4, e5
 *#=>
 * anc1: e4, e5, e1, e2, e3
 * anc2: e4, e5 (broken)
 */
static void
INSERT_LIST(ISEQ_ARG_DECLARE LINK_ANCHOR *anc1, LINK_ANCHOR *anc2)
{
    if (anc2->anchor.next) {
	LINK_ELEMENT *first = anc1->anchor.next;
	anc1->anchor.next = anc2->anchor.next;
	anc1->anchor.next->prev = &anc1->anchor;
	anc2->last->next = first;
	if (first) {
	    first->prev = anc2->last;
	}
	else {
	    anc1->last = anc2->last;
	}
    }

    verify_list("append", anc1);
}
#if CPDEBUG < 0
#define INSERT_LIST(anc1, anc2) INSERT_LIST(iseq, (anc1), (anc2))
#endif

#if 0 /* unused */
/*
 * anc1: e1, e2, e3
 * anc2: e4, e5
 *#=>
 * anc1: e4, e5
 * anc2: e1, e2, e3
 */
static void
SWAP_LIST(ISEQ_ARG_DECLARE LINK_ANCHOR *anc1, LINK_ANCHOR *anc2)
{
    LINK_ANCHOR tmp = *anc2;

    /* it has bug */
    *anc2 = *anc1;
    *anc1 = tmp;

    verify_list("swap1", anc1);
    verify_list("swap2", anc2);
}
#if CPDEBUG < 0
#define SWAP_LIST(anc1, anc2) SWAP_LIST(iseq, (anc1), (anc2))
#endif

static LINK_ANCHOR *
REVERSE_LIST(ISEQ_ARG_DECLARE LINK_ANCHOR *anc)
{
    LINK_ELEMENT *first, *last, *elem, *e;
    first = &anc->anchor;
    elem = first->next;
    last = anc->last;

    if (elem != 0) {
	anc->anchor.next = last;
	anc->last = elem;
    }
    else {
	/* null list */
	return anc;
    }
    while (elem) {
	e = elem->next;
	elem->next = elem->prev;
	elem->prev = e;
	elem = e;
    }

    first->next = last;
    last->prev = first;
    anc->last->next = 0;

    verify_list("reverse", anc);
    return anc;
}
#if CPDEBUG < 0
#define REVERSE_LIST(anc) REVERSE_LIST(iseq, (anc))
#endif
#endif

#if CPDEBUG && 0
static void
debug_list(ISEQ_ARG_DECLARE LINK_ANCHOR *anchor)
{
    LINK_ELEMENT *list = FIRST_ELEMENT(anchor);
    printf("----\n");
    printf("anch: %p, frst: %p, last: %p\n", &anchor->anchor,
	   anchor->anchor.next, anchor->last);
    while (list) {
	printf("curr: %p, next: %p, prev: %p, type: %d\n", list, list->next,
	       list->prev, FIX2INT(list->type));
	list = list->next;
    }
    printf("----\n");

    dump_disasm_list(anchor->anchor.next);
    verify_list("debug list", anchor);
}
#if CPDEBUG < 0
#define debug_list(anc) debug_list(iseq, (anc))
#endif
#endif

static LABEL *
new_label_body(rb_iseq_t *iseq, long line)
{
    LABEL *labelobj = compile_data_alloc_label(iseq);

    labelobj->link.type = ISEQ_ELEMENT_LABEL;
    labelobj->link.next = 0;

    labelobj->label_no = iseq->compile_data->label_no++;
    labelobj->sc_state = 0;
    labelobj->sp = -1;
    return labelobj;
}

static ADJUST *
new_adjust_body(rb_iseq_t *iseq, LABEL *label, int line)
{
    ADJUST *adjust = compile_data_alloc_adjust(iseq);
    adjust->link.type = ISEQ_ELEMENT_ADJUST;
    adjust->link.next = 0;
    adjust->label = label;
    adjust->line_no = line;
    return adjust;
}

static INSN *
new_insn_core(rb_iseq_t *iseq, int line_no,
	      int insn_id, int argc, VALUE *argv)
{
    INSN *iobj = compile_data_alloc_insn(iseq);
    /* printf("insn_id: %d, line: %d\n", insn_id, line_no); */

    iobj->link.type = ISEQ_ELEMENT_INSN;
    iobj->link.next = 0;
    iobj->insn_id = insn_id;
    iobj->line_no = line_no;
    iobj->operands = argv;
    iobj->operand_size = argc;
    iobj->sc_state = 0;
    return iobj;
}

static INSN *
new_insn_body(rb_iseq_t *iseq, int line_no, int insn_id, int argc, ...)
{
    VALUE *operands = 0;
    va_list argv;
    if (argc > 0) {
	int i;
	va_init_list(argv, argc);
	operands = (VALUE *)compile_data_alloc(iseq, sizeof(VALUE) * argc);
	for (i = 0; i < argc; i++) {
	    VALUE v = va_arg(argv, VALUE);
	    operands[i] = v;
	}
	va_end(argv);
    }
    return new_insn_core(iseq, line_no, insn_id, argc, operands);
}

static INSN *
new_insn_send(rb_iseq_t *iseq, int line_no,
	      VALUE id, VALUE argc, VALUE block, VALUE flag)
{
    INSN *iobj = 0;
    VALUE *operands =
      (VALUE *)compile_data_alloc(iseq, sizeof(VALUE) * 5);
    operands[0] = id;
    operands[1] = argc;
    operands[2] = block;
    operands[3] = flag;
    operands[4] = INT2FIX(iseq->ic_size++);
    iobj = new_insn_core(iseq, line_no, BIN(send), 5, operands);
    return iobj;
}

static VALUE
new_child_iseq(rb_iseq_t *iseq, NODE *node,
	       VALUE name, VALUE parent, enum iseq_type type, int line_no)
{
    VALUE ret;

    debugs("[new_child_iseq]> ---------------------------------------\n");
    ret = rb_iseq_new_with_opt(node, name, iseq_filename(iseq->self), iseq_filepath(iseq->self), INT2FIX(line_no),
			       parent, type, iseq->compile_data->option);
    debugs("[new_child_iseq]< ---------------------------------------\n");
    iseq_add_mark_object(iseq, ret);
    return ret;
}

static int
iseq_setup(rb_iseq_t *iseq, LINK_ANCHOR *anchor)
{
    /* debugs("[compile step 2] (iseq_array_to_linkedlist)\n"); */

    if (compile_debug > 5)
	dump_disasm_list(FIRST_ELEMENT(anchor));

    debugs("[compile step 3.1 (iseq_optimize)]\n");
    iseq_optimize(iseq, anchor);

    if (compile_debug > 5)
	dump_disasm_list(FIRST_ELEMENT(anchor));

    if (iseq->compile_data->option->instructions_unification) {
	debugs("[compile step 3.2 (iseq_insns_unification)]\n");
	iseq_insns_unification(iseq, anchor);
	if (compile_debug > 5)
	    dump_disasm_list(FIRST_ELEMENT(anchor));
    }

    if (iseq->compile_data->option->stack_caching) {
	debugs("[compile step 3.3 (iseq_set_sequence_stackcaching)]\n");
	iseq_set_sequence_stackcaching(iseq, anchor);
	if (compile_debug > 5)
	    dump_disasm_list(FIRST_ELEMENT(anchor));
    }

    debugs("[compile step 4.1 (iseq_set_sequence)]\n");
    iseq_set_sequence(iseq, anchor);
    if (compile_debug > 5)
	dump_disasm_list(FIRST_ELEMENT(anchor));

    debugs("[compile step 4.2 (iseq_set_exception_table)]\n");
    iseq_set_exception_table(iseq);

    debugs("[compile step 4.3 (set_optargs_table)] \n");
    iseq_set_optargs_table(iseq);

    debugs("[compile step 5 (iseq_translate_threaded_code)] \n");
    rb_iseq_translate_threaded_code(iseq);

    if (compile_debug > 1) {
	VALUE str = rb_iseq_disasm(iseq->self);
	printf("%s\n", StringValueCStr(str));
	fflush(stdout);
    }
    debugs("[compile step: finish]\n");

    return 0;
}

static int
iseq_set_exception_local_table(rb_iseq_t *iseq)
{
    ID id_dollar_bang;

    CONST_ID(id_dollar_bang, "#$!");
    iseq->local_table = (ID *)ALLOC_N(ID, 1);
    iseq->local_table_size = 1;
    iseq->local_size = iseq->local_table_size + 1;
    iseq->local_table[0] = id_dollar_bang;
    return COMPILE_OK;
}

static int
get_dyna_var_idx_at_raw(rb_iseq_t *iseq, ID id)
{
    int i;

    for (i = 0; i < iseq->local_table_size; i++) {
	if (iseq->local_table[i] == id) {
	    return i;
	}
    }
    return -1;
}

static int
get_local_var_idx(rb_iseq_t *iseq, ID id)
{
    int idx = get_dyna_var_idx_at_raw(iseq->local_iseq, id);

    if (idx < 0) {
	rb_bug("get_local_var_idx: %d", idx);
    }

    return idx;
}

static int
get_dyna_var_idx(rb_iseq_t *iseq, ID id, int *level, int *ls)
{
    int lv = 0, idx = -1;

    while (iseq) {
	idx = get_dyna_var_idx_at_raw(iseq, id);
	if (idx >= 0) {
	    break;
	}
	iseq = iseq->parent_iseq;
	lv++;
    }

    if (idx < 0) {
	rb_bug("get_dyna_var_idx: -1");
    }

    *level = lv;
    *ls = iseq->local_size;
    return idx;
}

static int
iseq_set_arguments(rb_iseq_t *iseq, LINK_ANCHOR *optargs, NODE *node_args)
{
    debugs("iseq_set_arguments: %s\n", node_args ? "" : "0");

    if (node_args) {
	struct rb_args_info *args = node_args->nd_ainfo;
	ID rest_id = 0;
	int last_comma = 0;
	ID block_id = 0;

	if (nd_type(node_args) != NODE_ARGS) {
	    rb_bug("iseq_set_arguments: NODE_ARGS is expected, but %s",
		   ruby_node_name(nd_type(node_args)));
	}


	iseq->argc = (int)args->pre_args_num;
	debugs("  - argc: %d\n", iseq->argc);

	rest_id = args->rest_arg;
	if (rest_id == 1) {
	    last_comma = 1;
	    rest_id = 0;
	}
	block_id = args->block_arg;

	if (args->first_post_arg) {
	    iseq->arg_post_start = get_dyna_var_idx_at_raw(iseq, args->first_post_arg);
	    iseq->arg_post_len = args->post_args_num;
	}

	if (args->opt_args) {
	    NODE *node = args->opt_args;
	    LABEL *label;
	    VALUE labels = rb_ary_tmp_new(1);
	    int i = 0, j;

	    while (node) {
		label = NEW_LABEL(nd_line(node));
		rb_ary_push(labels, (VALUE)label | 1);
		ADD_LABEL(optargs, label);
		COMPILE_POPED(optargs, "optarg", node->nd_body);
		node = node->nd_next;
		i += 1;
	    }

	    /* last label */
	    label = NEW_LABEL(nd_line(node_args));
	    rb_ary_push(labels, (VALUE)label | 1);
	    ADD_LABEL(optargs, label);
	    i += 1;

	    iseq->arg_opts = i;
	    iseq->arg_opt_table = ALLOC_N(VALUE, i);
	    MEMCPY(iseq->arg_opt_table, RARRAY_PTR(labels), VALUE, i);
	    for (j = 0; j < i; j++) {
		iseq->arg_opt_table[j] &= ~1;
	    }
	    rb_ary_clear(labels);
	}
	else {
	    iseq->arg_opts = 0;
	}

	if (args->kw_args) {
	    NODE *node = args->kw_args;
	    VALUE keywords = rb_ary_tmp_new(1);
	    int i = 0, j;

	    iseq->arg_keyword = get_dyna_var_idx_at_raw(iseq, args->kw_rest_arg->nd_vid);
	    COMPILE(optargs, "kwarg", args->kw_rest_arg);
	    while (node) {
		rb_ary_push(keywords, INT2FIX(node->nd_body->nd_vid));
		COMPILE_POPED(optargs, "kwarg", node); /* nd_type(node) == NODE_KW_ARG */
		node = node->nd_next;
		i += 1;
	    }
	    iseq->arg_keyword_check = (args->kw_rest_arg->nd_vid & ID_SCOPE_MASK) == ID_JUNK;
	    iseq->arg_keywords = i;
	    iseq->arg_keyword_table = ALLOC_N(ID, i);
	    for (j = 0; j < i; j++) {
		iseq->arg_keyword_table[j] = FIX2INT(RARRAY_PTR(keywords)[j]);
	    }
	    ADD_INSN(optargs, nd_line(args->kw_args), pop);
	}
	else if (args->kw_rest_arg) {
	    iseq->arg_keyword = get_dyna_var_idx_at_raw(iseq, args->kw_rest_arg->nd_vid);
	    COMPILE(optargs, "kwarg", args->kw_rest_arg);
	    iseq->arg_keyword_check = (args->kw_rest_arg->nd_vid & ID_SCOPE_MASK) == ID_JUNK;
	    ADD_INSN(optargs, nd_line(args->kw_rest_arg), pop);
	}
	else {
	    iseq->arg_keyword = -1;
	}

	if (args->pre_init) { /* m_init */
	    COMPILE_POPED(optargs, "init arguments (m)", args->pre_init);
	}
	if (args->post_init) { /* p_init */
	    COMPILE_POPED(optargs, "init arguments (p)", args->post_init);
	}

	if (rest_id) {
	    iseq->arg_rest = get_dyna_var_idx_at_raw(iseq, rest_id);

	    if (iseq->arg_rest == -1) {
		rb_bug("arg_rest: -1");
	    }

	    if (iseq->arg_post_start == 0) {
		iseq->arg_post_start = iseq->arg_rest + 1;
	    }
	}

	if (block_id) {
	    iseq->arg_block = get_dyna_var_idx_at_raw(iseq, block_id);
	}

	if (iseq->arg_opts != 0 || iseq->arg_post_len != 0 ||
	    iseq->arg_rest != -1 || iseq->arg_block != -1 ||
	    iseq->arg_keyword != -1) {
	    iseq->arg_simple = 0;

	    /* set arg_size: size of arguments */
	    if (iseq->arg_keyword != -1) {
		iseq->arg_size = iseq->arg_keyword + 1;
	    }
	    else if (iseq->arg_block != -1) {
		iseq->arg_size = iseq->arg_block + 1;
	    }
	    else if (iseq->arg_post_len) {
		iseq->arg_size = iseq->arg_post_start + iseq->arg_post_len;
	    }
	    else if (iseq->arg_rest != -1) {
		iseq->arg_size = iseq->arg_rest + 1;
	    }
	    else if (iseq->arg_opts) {
		iseq->arg_size = iseq->argc + iseq->arg_opts - 1;
	    }
	    else {
		iseq->arg_size = iseq->argc;
	    }
	}
	else {
	    iseq->arg_simple = 1;
	    iseq->arg_size = iseq->argc;
	}

	if (iseq->type == ISEQ_TYPE_BLOCK) {
	    if (iseq->arg_opts == 0 && iseq->arg_post_len == 0 && iseq->arg_rest == -1) {
		if (iseq->argc == 1 && last_comma == 0) {
		    /* {|a|} */
		    iseq->arg_simple |= 0x02;
		}
	    }
	}
    }
    else {
	iseq->arg_simple = 1;
    }

    return COMPILE_OK;
}

static int
iseq_set_local_table(rb_iseq_t *iseq, ID *tbl)
{
    int size;

    if (tbl) {
	size = (int)*tbl;
	tbl++;
    }
    else {
	size = 0;
    }

    if (size > 0) {
	iseq->local_table = (ID *)ALLOC_N(ID, size);
	MEMCPY(iseq->local_table, tbl, ID, size);
    }

    iseq->local_size = iseq->local_table_size = size;
    iseq->local_size += 1;
    /*
      if (lfp == dfp ) { // top, class, method
	  dfp[-1]: svar
      else {             // block
          dfp[-1]: cref
      }
     */

    debugs("iseq_set_local_table: %d, %d\n", iseq->local_size, iseq->local_table_size);
    return COMPILE_OK;
}

static int
cdhash_cmp(VALUE val, VALUE lit)
{
    if (val == lit) return 0;
    if (SPECIAL_CONST_P(lit)) {
	return val != lit;
    }
    if (SPECIAL_CONST_P(val) || BUILTIN_TYPE(val) != BUILTIN_TYPE(lit)) {
	return -1;
    }
    if (BUILTIN_TYPE(lit) == T_STRING) {
	return rb_str_hash_cmp(lit, val);
    }
    return !rb_eql(lit, val);
}

static st_index_t
cdhash_hash(VALUE a)
{
    if (SPECIAL_CONST_P(a)) return (st_index_t)a;
    if (RB_TYPE_P(a, T_STRING)) return rb_str_hash(a);
    {
	VALUE hval = rb_hash(a);
	return (st_index_t)FIX2LONG(hval);
    }
}

static const struct st_hash_type cdhash_type = {
    cdhash_cmp,
    cdhash_hash,
};

/**
  ruby insn object list -> raw instruction sequence
 */
static int
iseq_set_sequence(rb_iseq_t *iseq, LINK_ANCHOR *anchor)
{
    LABEL *lobj;
    INSN *iobj;
    struct iseq_line_info_entry *line_info_table;
    unsigned int last_line = 0;
    LINK_ELEMENT *list;
    VALUE *generated_iseq;

    int k, pos, sp, stack_max = 0, line = 0;

    /* set label position */
    list = FIRST_ELEMENT(anchor);
    k = pos = 0;
    while (list) {
	switch (list->type) {
	  case ISEQ_ELEMENT_INSN:
	    {
		iobj = (INSN *)list;
		line = iobj->line_no;
		pos += insn_data_length(iobj);
		k++;
		break;
	    }
	  case ISEQ_ELEMENT_LABEL:
	    {
		lobj = (LABEL *)list;
		lobj->position = pos;
		lobj->set = TRUE;
		break;
	    }
	  case ISEQ_ELEMENT_NONE:
	    {
		/* ignore */
		break;
	    }
	  case ISEQ_ELEMENT_ADJUST:
	    {
		ADJUST *adjust = (ADJUST *)list;
		if (adjust->line_no != -1) {
		    pos += 2 /* insn + 1 operand */;
		    k++;
		}
		break;
	    }
	  default:
	    dump_disasm_list(FIRST_ELEMENT(anchor));
	    dump_disasm_list(list);
	    rb_compile_error(RSTRING_PTR(iseq->filename), line,
			     "error: set_sequence");
	    break;
	}
	list = list->next;
    }

    /* make instruction sequence */
    generated_iseq = ALLOC_N(VALUE, pos);
    line_info_table = ALLOC_N(struct iseq_line_info_entry, k);
    iseq->ic_entries = ALLOC_N(struct iseq_inline_cache_entry, iseq->ic_size);
    MEMZERO(iseq->ic_entries, struct iseq_inline_cache_entry, iseq->ic_size);

    list = FIRST_ELEMENT(anchor);
    k = pos = sp = 0;

    while (list) {
	switch (list->type) {
	  case ISEQ_ELEMENT_INSN:
	    {
		int j, len, insn;
		const char *types;
		VALUE *operands;

		iobj = (INSN *)list;

		/* update sp */
		sp = calc_sp_depth(sp, iobj);
		if (sp > stack_max) {
		    stack_max = sp;
		}

		/* fprintf(stderr, "insn: %-16s, sp: %d\n", insn_name(iobj->insn_id), sp); */
		operands = iobj->operands;
		insn = iobj->insn_id;
		generated_iseq[pos] = insn;
		types = insn_op_types(insn);
		len = insn_len(insn);

		/* operand check */
		if (iobj->operand_size != len - 1) {
		    /* printf("operand size miss! (%d, %d)\n", iobj->operand_size, len); */
		    dump_disasm_list(list);
		    rb_compile_error(RSTRING_PTR(iseq->filename), iobj->line_no,
				     "operand size miss! (%d for %d)",
				     iobj->operand_size, len - 1);
		    xfree(generated_iseq);
		    xfree(line_info_table);
		    return 0;
		}

		for (j = 0; types[j]; j++) {
		    char type = types[j];
		    /* printf("--> [%c - (%d-%d)]\n", type, k, j); */
		    switch (type) {
		      case TS_OFFSET:
			{
			    /* label(destination position) */
			    lobj = (LABEL *)operands[j];
			    if (!lobj->set) {
				rb_compile_error(RSTRING_PTR(iseq->filename), iobj->line_no,
						 "unknown label");
			    }
			    if (lobj->sp == -1) {
				lobj->sp = sp;
			    }
			    generated_iseq[pos + 1 + j] =
				lobj->position - (pos + len);
			    break;
			}
		      case TS_CDHASH:
			{
			    /*
			     * [obj, label, ...]
			     */
			    int i;
			    VALUE lits = operands[j];
			    VALUE map = rb_hash_new();
			    RHASH_TBL(map)->type = &cdhash_type;

			    for (i=0; i < RARRAY_LEN(lits); i+=2) {
				VALUE obj = rb_ary_entry(lits, i);
				VALUE lv  = rb_ary_entry(lits, i+1);
				lobj = (LABEL *)(lv & ~1);

				if (!lobj->set) {
				    rb_compile_error(RSTRING_PTR(iseq->filename), iobj->line_no,
						     "unknown label");
				}
				if (!st_lookup(rb_hash_tbl(map), obj, 0)) {
				    rb_hash_aset(map, obj, INT2FIX(lobj->position - (pos+len)));
				}
				else {
				    rb_compile_warning(RSTRING_PTR(iseq->filename), iobj->line_no,
						       "duplicated when clause is ignored");
				}
			    }
			    hide_obj(map);
			    generated_iseq[pos + 1 + j] = map;
			    iseq_add_mark_object(iseq, map);
			    break;
			}
		      case TS_LINDEX:
		      case TS_DINDEX:
		      case TS_NUM:	/* ulong */
			generated_iseq[pos + 1 + j] = FIX2INT(operands[j]);
			break;
		      case TS_ISEQ:	/* iseq */
			{
			    VALUE v = operands[j];
			    rb_iseq_t *block = 0;
			    if (v) {
				GetISeqPtr(v, block);
			    }
			    generated_iseq[pos + 1 + j] = (VALUE)block;
			    break;
			}
		      case TS_VALUE:	/* VALUE */
			{
			    VALUE v = operands[j];
			    generated_iseq[pos + 1 + j] = v;
			    /* to mark ruby object */
			    iseq_add_mark_object(iseq, v);
			    break;
			}
		      case TS_IC: /* inline cache */
			{
			    int ic_index = FIX2INT(operands[j]);
			    IC ic = &iseq->ic_entries[ic_index];
			    if (UNLIKELY(ic_index >= iseq->ic_size)) {
				rb_bug("iseq_set_sequence: ic_index overflow: index: %d, size: %d",
				       ic_index, iseq->ic_size);
			    }
			    generated_iseq[pos + 1 + j] = (VALUE)ic;
			    break;
			}
		      case TS_ID: /* ID */
			generated_iseq[pos + 1 + j] = SYM2ID(operands[j]);
			break;
		      case TS_GENTRY:
			{
			    struct rb_global_entry *entry =
				(struct rb_global_entry *)(operands[j] & (~1));
			    generated_iseq[pos + 1 + j] = (VALUE)entry;
			}
			break;
		      default:
			rb_compile_error(RSTRING_PTR(iseq->filename), iobj->line_no,
					 "unknown operand type: %c", type);
			xfree(generated_iseq);
			xfree(line_info_table);
			return 0;
		    }
		}
		if (last_line != iobj->line_no) {
		    line_info_table[k].line_no = last_line = iobj->line_no;
		    line_info_table[k].position = pos;
		    k++;
		}
		pos += len;
		break;
	    }
	  case ISEQ_ELEMENT_LABEL:
	    {
		lobj = (LABEL *)list;
		if (lobj->sp == -1) {
		    lobj->sp = sp;
		}
		else {
		    sp = lobj->sp;
		}
		break;
	    }
	  case ISEQ_ELEMENT_ADJUST:
	    {
		ADJUST *adjust = (ADJUST *)list;
		int orig_sp = sp;

		if (adjust->label) {
		    sp = adjust->label->sp;
		}
		else {
		    sp = 0;
		}

		if (adjust->line_no != -1) {
		    if (orig_sp - sp > 0) {
			if (last_line != (unsigned int)adjust->line_no) {
			    line_info_table[k].line_no = last_line = adjust->line_no;
			    line_info_table[k].position = pos;
			    k++;
			}
			generated_iseq[pos++] = BIN(adjuststack);
			generated_iseq[pos++] = orig_sp - sp;
		    }
		    else if (orig_sp - sp == 0) {
			/* jump to next insn */
			if (last_line != (unsigned int)adjust->line_no) {
			    line_info_table[k].line_no = last_line = adjust->line_no;
			    line_info_table[k].position = pos;
			    k++;
			}
			generated_iseq[pos++] = BIN(jump);
			generated_iseq[pos++] = 0;
		    }
		    else {
			rb_bug("iseq_set_sequence: adjust bug");
		    }
		}
		break;
	    }
	  default:
	    /* ignore */
	    break;
	}
	list = list->next;
    }

#if 0 /* XXX */
    /* this check need dead code elimination */
    if (sp != 1) {
	rb_bug("SP is not 0 on %s (%d)\n", RSTRING_PTR(iseq->name), sp);
    }
#endif

    iseq->iseq = (void *)generated_iseq;
    iseq->iseq_size = pos;
    iseq->stack_max = stack_max;

    line_info_table = ruby_xrealloc(line_info_table, k * sizeof(struct iseq_line_info_entry));
    iseq->line_info_table = line_info_table;
    iseq->line_info_size = k;

    return COMPILE_OK;
}

static int
label_get_position(LABEL *lobj)
{
    return lobj->position;
}

static int
label_get_sp(LABEL *lobj)
{
    return lobj->sp;
}

static int
iseq_set_exception_table(rb_iseq_t *iseq)
{
    VALUE *tptr, *ptr;
    int tlen, i;
    struct iseq_catch_table_entry *entry;

    tlen = (int)RARRAY_LEN(iseq->compile_data->catch_table_ary);
    tptr = RARRAY_PTR(iseq->compile_data->catch_table_ary);

    iseq->catch_table = tlen ? ALLOC_N(struct iseq_catch_table_entry, tlen) : 0;
    iseq->catch_table_size = tlen;

    for (i = 0; i < tlen; i++) {
	ptr = RARRAY_PTR(tptr[i]);
	entry = &iseq->catch_table[i];
	entry->type = (enum catch_type)(ptr[0] & 0xffff);
	entry->start = label_get_position((LABEL *)(ptr[1] & ~1));
	entry->end = label_get_position((LABEL *)(ptr[2] & ~1));
	entry->iseq = ptr[3];

	/* register iseq as mark object */
	if (entry->iseq != 0) {
	    iseq_add_mark_object(iseq, entry->iseq);
	}

	/* stack depth */
	if (ptr[4]) {
	    LABEL *lobj = (LABEL *)(ptr[4] & ~1);
	    entry->cont = label_get_position(lobj);
	    entry->sp = label_get_sp(lobj);

	    /* TODO: Dirty Hack!  Fix me */
	    if (entry->type == CATCH_TYPE_RESCUE ||
		entry->type == CATCH_TYPE_BREAK ||
		entry->type == CATCH_TYPE_NEXT) {
		entry->sp--;
	    }
	}
	else {
	    entry->cont = 0;
	}
    }

    iseq->compile_data->catch_table_ary = 0;	/* free */
    return COMPILE_OK;
}

/*
 * set optional argument table
 *   def foo(a, b=expr1, c=expr2)
 *   =>
 *    b:
 *      expr1
 *    c:
 *      expr2
 */
static int
iseq_set_optargs_table(rb_iseq_t *iseq)
{
    int i;

    if (iseq->arg_opts != 0) {
	for (i = 0; i < iseq->arg_opts; i++) {
	    iseq->arg_opt_table[i] =
		label_get_position((LABEL *)iseq->arg_opt_table[i]);
	}
    }
    return COMPILE_OK;
}

static LINK_ELEMENT *
get_destination_insn(INSN *iobj)
{
    LABEL *lobj = (LABEL *)OPERAND_AT(iobj, 0);
    LINK_ELEMENT *list;

    list = lobj->link.next;
    while (list) {
	if (list->type == ISEQ_ELEMENT_INSN || list->type == ISEQ_ELEMENT_ADJUST) {
	    break;
	}
	list = list->next;
    }
    return list;
}

static LINK_ELEMENT *
get_next_insn(INSN *iobj)
{
    LINK_ELEMENT *list = iobj->link.next;

    while (list) {
	if (list->type == ISEQ_ELEMENT_INSN || list->type == ISEQ_ELEMENT_ADJUST) {
	    return list;
	}
	list = list->next;
    }
    return 0;
}

static LINK_ELEMENT *
get_prev_insn(INSN *iobj)
{
    LINK_ELEMENT *list = iobj->link.prev;

    while (list) {
	if (list->type == ISEQ_ELEMENT_INSN || list->type == ISEQ_ELEMENT_ADJUST) {
	    return list;
	}
	list = list->prev;
    }
    return 0;
}

static int
iseq_peephole_optimize(rb_iseq_t *iseq, LINK_ELEMENT *list, const int do_tailcallopt)
{
    INSN *iobj = (INSN *)list;
  again:
    if (iobj->insn_id == BIN(jump)) {
	INSN *niobj, *diobj, *piobj;
	/*
	 *  useless jump elimination:
	 *     jump LABEL1
	 *     ...
	 *   LABEL1:
	 *     jump LABEL2
	 *
	 *   => in this case, first jump instruction should jump to
	 *      LABEL2 directly
	 */
	diobj = (INSN *)get_destination_insn(iobj);
	niobj = (INSN *)get_next_insn(iobj);

	if (diobj == niobj) {
	    /*
	     *   jump LABEL
	     *  LABEL:
	     * =>
	     *   LABEL:
	     */
	    REMOVE_ELEM(&iobj->link);
	}
	else if (iobj != diobj && diobj->insn_id == BIN(jump)) {
	    if (OPERAND_AT(iobj, 0) != OPERAND_AT(diobj, 0)) {
		OPERAND_AT(iobj, 0) = OPERAND_AT(diobj, 0);
		goto again;
	    }
	}
	else if (diobj->insn_id == BIN(leave)) {
	    /*
	     *  jump LABEL
	     *  ...
	     * LABEL:
	     *  leave
	     * =>
	     *  leave
	     *  ...
	     * LABEL:
	     *  leave
	     */
	    INSN *eiobj = new_insn_core(iseq, iobj->line_no, BIN(leave),
					diobj->operand_size, diobj->operands);
	    INSN *popiobj = new_insn_core(iseq, iobj->line_no,
					  BIN(pop), 0, 0);
	    /* replace */
	    REPLACE_ELEM((LINK_ELEMENT *)iobj, (LINK_ELEMENT *)eiobj);
	    INSERT_ELEM_NEXT((LINK_ELEMENT *)eiobj, (LINK_ELEMENT *)popiobj);
	    iobj = popiobj;
	}
	/*
	 * useless jump elimination (if/unless destination):
	 *   if   L1
	 *   jump L2
	 * L1:
	 *   ...
	 * L2:
	 *
	 * ==>
	 *   unless L2
	 * L1:
	 *   ...
	 * L2:
	 */
	else if ((piobj = (INSN *)get_prev_insn(iobj)) != 0 &&
		 (piobj->insn_id == BIN(branchif) ||
		  piobj->insn_id == BIN(branchunless))) {
	    if (niobj == (INSN *)get_destination_insn(piobj)) {
		piobj->insn_id = (piobj->insn_id == BIN(branchif))
		  ? BIN(branchunless) : BIN(branchif);
		OPERAND_AT(piobj, 0) = OPERAND_AT(iobj, 0);
		REMOVE_ELEM(&iobj->link);
	    }
	}
    }

    if (iobj->insn_id == BIN(branchif) ||
	iobj->insn_id == BIN(branchunless)) {
	/*
	 *   if L1
	 *   ...
	 * L1:
	 *   jump L2
	 * =>
	 *   if L2
	 */
	INSN *nobj = (INSN *)get_destination_insn(iobj);
	if (nobj->insn_id == BIN(jump)) {
	    OPERAND_AT(iobj, 0) = OPERAND_AT(nobj, 0);
	}
    }

    if (do_tailcallopt && iobj->insn_id == BIN(leave)) {
	/*
	 *  send ...
	 *  leave
	 * =>
	 *  send ..., ... | VM_CALL_TAILCALL_BIT, ...
	 *  leave # unreachable
	 */
	INSN *piobj = (INSN *)get_prev_insn((INSN *)list);

	if (piobj->insn_id == BIN(send) &&
	    piobj->operands[2] == 0 /* block */
	    ) {
	    piobj->operands[3] = FIXNUM_OR(piobj->operands[3], VM_CALL_TAILCALL_BIT);
	}
    }
    return COMPILE_OK;
}

static int
insn_set_specialized_instruction(rb_iseq_t *iseq, INSN *iobj, int insn_id)
{
    int i, old_opsize = iobj->operand_size;

    iobj->insn_id = insn_id;
    iobj->operand_size = insn_len(insn_id) - 1;
    /* printf("iobj->operand_size: %d\n", iobj->operand_size); */

    if (iobj->operand_size > old_opsize) {
	iobj->operands = (VALUE *)compile_data_alloc(iseq, iobj->operand_size);
    }

    for (i=0; i<iobj->operand_size; i++) {
	iobj->operands[i] = INT2FIX(iseq->ic_size++);
    }

    return COMPILE_OK;
}

static int
iseq_specialized_instruction(rb_iseq_t *iseq, INSN *iobj)
{
    if (iobj->insn_id == BIN(send)) {
	ID mid = SYM2ID(OPERAND_AT(iobj, 0));
	int argc = FIX2INT(OPERAND_AT(iobj, 1));
	VALUE block = OPERAND_AT(iobj, 2);
	VALUE flag = OPERAND_AT(iobj, 3);

#define SP_INSN(opt) insn_set_specialized_instruction(iseq, iobj, BIN(opt_##opt))

	if (block == 0 && flag == INT2FIX(0)) {
	    switch (argc) {
	      case 0:
		switch (mid) {
		  case idLength: SP_INSN(length); break;
		  case idSize:	 SP_INSN(size);	  break;
		  case idSucc:	 SP_INSN(succ);	  break;
		  case idNot:	 SP_INSN(not);	  break;
		}
		break;
	      case 1:
		switch (mid) {
		  case idPLUS:	 SP_INSN(plus);	  break;
		  case idMINUS:	 SP_INSN(minus);  break;
		  case idMULT:	 SP_INSN(mult);	  break;
		  case idDIV:	 SP_INSN(div);	  break;
		  case idMOD:	 SP_INSN(mod);	  break;
		  case idEq:	 SP_INSN(eq);	  break;
		  case idNeq:	 SP_INSN(neq);	  break;
		  case idLT:	 SP_INSN(lt);	  break;
		  case idLE:	 SP_INSN(le);	  break;
		  case idGT:	 SP_INSN(gt);	  break;
		  case idGE:	 SP_INSN(ge);	  break;
		  case idLTLT:	 SP_INSN(ltlt);	  break;
		  case idAREF:	 SP_INSN(aref);	  break;
		}
		break;
	    }
	}
    }
    return COMPILE_OK;
#undef SP_INSN
}

static int
iseq_optimize(rb_iseq_t *iseq, LINK_ANCHOR *anchor)
{
    LINK_ELEMENT *list;
    const int do_peepholeopt = iseq->compile_data->option->peephole_optimization;
    const int do_tailcallopt = iseq->compile_data->option->tailcall_optimization;
    const int do_si = iseq->compile_data->option->specialized_instruction;
    const int do_ou = iseq->compile_data->option->operands_unification;
    list = FIRST_ELEMENT(anchor);

    while (list) {
	if (list->type == ISEQ_ELEMENT_INSN) {
	    if (do_peepholeopt) {
		iseq_peephole_optimize(iseq, list, do_tailcallopt);
	    }
	    if (do_si) {
		iseq_specialized_instruction(iseq, (INSN *)list);
	    }
	    if (do_ou) {
		insn_operands_unification((INSN *)list);
	    }
	}
	list = list->next;
    }
    return COMPILE_OK;
}

#if OPT_INSTRUCTIONS_UNIFICATION
static INSN *
new_unified_insn(rb_iseq_t *iseq,
		 int insn_id, int size, LINK_ELEMENT *seq_list)
{
    INSN *iobj = 0;
    LINK_ELEMENT *list = seq_list;
    int i, argc = 0;
    VALUE *operands = 0, *ptr = 0;


    /* count argc */
    for (i = 0; i < size; i++) {
	iobj = (INSN *)list;
	argc += iobj->operand_size;
	list = list->next;
    }

    if (argc > 0) {
	ptr = operands =
	    (VALUE *)compile_data_alloc(iseq, sizeof(VALUE) * argc);
    }

    /* copy operands */
    list = seq_list;
    for (i = 0; i < size; i++) {
	iobj = (INSN *)list;
	MEMCPY(ptr, iobj->operands, VALUE, iobj->operand_size);
	ptr += iobj->operand_size;
	list = list->next;
    }

    return new_insn_core(iseq, iobj->line_no, insn_id, argc, operands);
}
#endif

/*
 * This scheme can get more performance if do this optimize with
 * label address resolving.
 * It's future work (if compile time was bottle neck).
 */
static int
iseq_insns_unification(rb_iseq_t *iseq, LINK_ANCHOR *anchor)
{
#if OPT_INSTRUCTIONS_UNIFICATION
    LINK_ELEMENT *list;
    INSN *iobj, *niobj;
    int id, k;
    intptr_t j;

    list = FIRST_ELEMENT(anchor);
    while (list) {
	if (list->type == ISEQ_ELEMENT_INSN) {
	    iobj = (INSN *)list;
	    id = iobj->insn_id;
	    if (unified_insns_data[id] != 0) {
		const int *const *entry = unified_insns_data[id];
		for (j = 1; j < (intptr_t)entry[0]; j++) {
		    const int *unified = entry[j];
		    LINK_ELEMENT *li = list->next;
		    for (k = 2; k < unified[1]; k++) {
			if (li->type != ISEQ_ELEMENT_INSN ||
			    ((INSN *)li)->insn_id != unified[k]) {
			    goto miss;
			}
			li = li->next;
		    }
		    /* matched */
		    niobj =
			new_unified_insn(iseq, unified[0], unified[1] - 1,
					 list);

		    /* insert to list */
		    niobj->link.prev = (LINK_ELEMENT *)iobj->link.prev;
		    niobj->link.next = li;
		    if (li) {
			li->prev = (LINK_ELEMENT *)niobj;
		    }

		    list->prev->next = (LINK_ELEMENT *)niobj;
		    list = (LINK_ELEMENT *)niobj;
		    break;
		  miss:;
		}
	    }
	}
	list = list->next;
    }
#endif
    return COMPILE_OK;
}

#if OPT_STACK_CACHING

#define SC_INSN(insn, stat) sc_insn_info[(insn)][(stat)]
#define SC_NEXT(insn)       sc_insn_next[(insn)]

#include "opt_sc.inc"

static int
insn_set_sc_state(rb_iseq_t *iseq, INSN *iobj, int state)
{
    int nstate;
    int insn_id;

    insn_id = iobj->insn_id;
    iobj->insn_id = SC_INSN(insn_id, state);
    nstate = SC_NEXT(iobj->insn_id);

    if (insn_id == BIN(jump) ||
	insn_id == BIN(branchif) || insn_id == BIN(branchunless)) {
	LABEL *lobj = (LABEL *)OPERAND_AT(iobj, 0);

	if (lobj->sc_state != 0) {
	    if (lobj->sc_state != nstate) {
		dump_disasm_list((LINK_ELEMENT *)iobj);
		dump_disasm_list((LINK_ELEMENT *)lobj);
		printf("\n-- %d, %d\n", lobj->sc_state, nstate);
		rb_compile_error(RSTRING_PTR(iseq->filename), iobj->line_no,
				 "insn_set_sc_state error\n");
		return 0;
	    }
	}
	else {
	    lobj->sc_state = nstate;
	}
	if (insn_id == BIN(jump)) {
	    nstate = SCS_XX;
	}
    }
    else if (insn_id == BIN(leave)) {
	nstate = SCS_XX;
    }

    return nstate;
}

static int
label_set_sc_state(LABEL *lobj, int state)
{
    if (lobj->sc_state != 0) {
	if (lobj->sc_state != state) {
	    state = lobj->sc_state;
	}
    }
    else {
	lobj->sc_state = state;
    }

    return state;
}


#endif

static int
iseq_set_sequence_stackcaching(rb_iseq_t *iseq, LINK_ANCHOR *anchor)
{
#if OPT_STACK_CACHING
    LINK_ELEMENT *list;
    int state, insn_id;

    /* initialize */
    state = SCS_XX;
    list = FIRST_ELEMENT(anchor);
    /* dump_disasm_list(list); */

    /* for each list element */
    while (list) {
      redo_point:
	switch (list->type) {
	  case ISEQ_ELEMENT_INSN:
	    {
		INSN *iobj = (INSN *)list;
		insn_id = iobj->insn_id;

		/* dump_disasm_list(list); */

		switch (insn_id) {
		  case BIN(nop):
		    {
			/* exception merge point */
			if (state != SCS_AX) {
			    INSN *rpobj =
				new_insn_body(iseq, 0, BIN(reput), 0);

			    /* replace this insn */
			    REPLACE_ELEM(list, (LINK_ELEMENT *)rpobj);
			    list = (LINK_ELEMENT *)rpobj;
			    goto redo_point;
			}
			break;
		    }
		  case BIN(swap):
		    {
			if (state == SCS_AB || state == SCS_BA) {
			    state = (state == SCS_AB ? SCS_BA : SCS_AB);

			    REMOVE_ELEM(list);
			    list = list->next;
			    goto redo_point;
			}
			break;
		    }
		  case BIN(pop):
		    {
			switch (state) {
			  case SCS_AX:
			  case SCS_BX:
			    state = SCS_XX;
			    break;
			  case SCS_AB:
			    state = SCS_AX;
			    break;
			  case SCS_BA:
			    state = SCS_BX;
			    break;
			  case SCS_XX:
			    goto normal_insn;
			  default:
			    rb_compile_error(RSTRING_PTR(iseq->filename), iobj->line_no,
					     "unreachable");
			}
			/* remove useless pop */
			REMOVE_ELEM(list);
			list = list->next;
			goto redo_point;
		    }
		  default:;
		    /* none */
		}		/* end of switch */
	      normal_insn:
		state = insn_set_sc_state(iseq, iobj, state);
		break;
	    }
	  case ISEQ_ELEMENT_LABEL:
	    {
		LABEL *lobj;
		lobj = (LABEL *)list;

		state = label_set_sc_state(lobj, state);
	    }
	  default:
	    break;
	}
	list = list->next;
    }
#endif
    return COMPILE_OK;
}



static int
compile_dstr_fragments(rb_iseq_t *iseq, LINK_ANCHOR *ret, NODE * node, int *cntp)
{
    NODE *list = node->nd_next;
    VALUE lit = node->nd_lit;
    int cnt = 0;

    debugp_param("nd_lit", lit);
    if (!NIL_P(lit)) {
	hide_obj(lit);
	cnt++;
	ADD_INSN1(ret, nd_line(node), putobject, lit);
    }

    while (list) {
	COMPILE(ret, "each string", list->nd_head);
	cnt++;
	list = list->nd_next;
    }
    *cntp = cnt;

    return COMPILE_OK;
}

static int
compile_dstr(rb_iseq_t *iseq, LINK_ANCHOR *ret, NODE * node)
{
    int cnt;
    compile_dstr_fragments(iseq, ret, node, &cnt);
    ADD_INSN1(ret, nd_line(node), concatstrings, INT2FIX(cnt));
    return COMPILE_OK;
}

static int
compile_dregx(rb_iseq_t *iseq, LINK_ANCHOR *ret, NODE * node)
{
    int cnt;
    compile_dstr_fragments(iseq, ret, node, &cnt);
    ADD_INSN2(ret, nd_line(node), toregexp, INT2FIX(node->nd_cflag), INT2FIX(cnt));
    return COMPILE_OK;
}

static int
compile_branch_condition(rb_iseq_t *iseq, LINK_ANCHOR *ret, NODE * cond,
			 LABEL *then_label, LABEL *else_label)
{
    switch (nd_type(cond)) {
      case NODE_AND:
	{
	    LABEL *label = NEW_LABEL(nd_line(cond));
	    compile_branch_condition(iseq, ret, cond->nd_1st, label,
				     else_label);
	    ADD_LABEL(ret, label);
	    compile_branch_condition(iseq, ret, cond->nd_2nd, then_label,
				     else_label);
	    break;
	}
      case NODE_OR:
	{
	    LABEL *label = NEW_LABEL(nd_line(cond));
	    compile_branch_condition(iseq, ret, cond->nd_1st, then_label,
				     label);
	    ADD_LABEL(ret, label);
	    compile_branch_condition(iseq, ret, cond->nd_2nd, then_label,
				     else_label);
	    break;
	}
      case NODE_LIT:		/* NODE_LIT is always not true */
      case NODE_TRUE:
      case NODE_STR:
	/* printf("useless condition eliminate (%s)\n",  ruby_node_name(nd_type(cond))); */
	ADD_INSNL(ret, nd_line(cond), jump, then_label);
	break;
      case NODE_FALSE:
      case NODE_NIL:
	/* printf("useless condition eliminate (%s)\n", ruby_node_name(nd_type(cond))); */
	ADD_INSNL(ret, nd_line(cond), jump, else_label);
	break;
      default:
	COMPILE(ret, "branch condition", cond);
	ADD_INSNL(ret, nd_line(cond), branchunless, else_label);
	ADD_INSNL(ret, nd_line(cond), jump, then_label);
	break;
    }
    return COMPILE_OK;
}

static int
compile_array_(rb_iseq_t *iseq, LINK_ANCHOR *ret, NODE* node_root,
	       VALUE opt_p, int poped)
{
    NODE *node = node_root;
    int len = (int)node->nd_alen, line = (int)nd_line(node), i=0;
    DECL_ANCHOR(anchor);

    INIT_ANCHOR(anchor);
    if (nd_type(node) != NODE_ZARRAY) {
	while (node) {
	    if (nd_type(node) != NODE_ARRAY) {
		rb_bug("compile_array: This node is not NODE_ARRAY, but %s",
		       ruby_node_name(nd_type(node)));
	    }

	    i++;
	    if (opt_p && nd_type(node->nd_head) != NODE_LIT) {
		opt_p = Qfalse;
	    }
	    COMPILE_(anchor, "array element", node->nd_head, poped);
	    node = node->nd_next;
	}
    }

    if (len != i) {
	if (0) {
	    rb_bug("node error: compile_array (%d: %d-%d)",
		   (int)nd_line(node_root), len, i);
	}
	len = i;
    }

    if (opt_p == Qtrue) {
	if (!poped) {
	    VALUE ary = rb_ary_tmp_new(len);
	    node = node_root;
	    while (node) {
		rb_ary_push(ary, node->nd_head->nd_lit);
		node = node->nd_next;
	    }
	    OBJ_FREEZE(ary);
	    iseq_add_mark_object_compile_time(iseq, ary);
	    ADD_INSN1(ret, nd_line(node_root), duparray, ary);
	}
    }
    else {
	if (!poped) {
	    ADD_INSN1(anchor, line, newarray, INT2FIX(len));
	}
	APPEND_LIST(ret, anchor);
    }
    return len;
}

static VALUE
compile_array(rb_iseq_t *iseq, LINK_ANCHOR *ret, NODE* node_root, VALUE opt_p)
{
    return compile_array_(iseq, ret, node_root, opt_p, 0);
}

static VALUE
case_when_optimizable_literal(NODE * node)
{
    switch (nd_type(node)) {
      case NODE_LIT: {
	VALUE v = node->nd_lit;
	double ival;
	if (RB_TYPE_P(v, T_FLOAT) &&
	    modf(RFLOAT_VALUE(v), &ival) == 0.0) {
	    return FIXABLE(ival) ? LONG2FIX((long)ival) : rb_dbl2big(ival);
	}
	if (SYMBOL_P(v) || rb_obj_is_kind_of(v, rb_cNumeric)) {
	    return v;
	}
	break;
      }
      case NODE_STR:
	return node->nd_lit;
    }
    return Qfalse;
}

static VALUE
when_vals(rb_iseq_t *iseq, LINK_ANCHOR *cond_seq, NODE *vals, LABEL *l1, VALUE special_literals)
{
    while (vals) {
	VALUE lit;
	NODE* val;

	val = vals->nd_head;

	if (special_literals &&
	    (lit = case_when_optimizable_literal(val)) != Qfalse) {
	    rb_ary_push(special_literals, lit);
	    rb_ary_push(special_literals, (VALUE)(l1) | 1);
	}
	else {
	    special_literals = Qfalse;
	}

	if (nd_type(val) == NODE_STR) {
	    debugp_param("nd_lit", val->nd_lit);
	    OBJ_FREEZE(val->nd_lit);
	    ADD_INSN1(cond_seq, nd_line(val), putobject, val->nd_lit);
	}
	else {
	    COMPILE(cond_seq, "when cond", val);
	}
	ADD_INSN1(cond_seq, nd_line(val), topn, INT2FIX(1));
	ADD_SEND(cond_seq, nd_line(val), ID2SYM(idEqq), INT2FIX(1));
	ADD_INSNL(cond_seq, nd_line(val), branchif, l1);
	vals = vals->nd_next;
    }
    return special_literals;
}

static int
compile_massign_lhs(rb_iseq_t *iseq, LINK_ANCHOR *ret, NODE *node)
{
    switch (nd_type(node)) {
      case NODE_ATTRASGN: {
	INSN *iobj;
	VALUE dupidx;

	COMPILE_POPED(ret, "masgn lhs (NODE_ATTRASGN)", node);
	POP_ELEMENT(ret);        /* pop pop insn */
	iobj = (INSN *)POP_ELEMENT(ret); /* pop send insn */

	dupidx = iobj->operands[1];
	dupidx = FIXNUM_INC(dupidx, 1);
	iobj->operands[1] = dupidx;

	ADD_INSN1(ret, nd_line(node), topn, dupidx);
	ADD_ELEM(ret, (LINK_ELEMENT *)iobj);
	ADD_INSN(ret, nd_line(node), pop);	/* result */
	ADD_INSN(ret, nd_line(node), pop);	/* rhs    */
	break;
      }
      case NODE_MASGN: {
	DECL_ANCHOR(anchor);
	INIT_ANCHOR(anchor);
	COMPILE_POPED(anchor, "nest masgn lhs", node);
	REMOVE_ELEM(FIRST_ELEMENT(anchor));
	ADD_SEQ(ret, anchor);
	break;
      }
      default: {
	DECL_ANCHOR(anchor);
	INIT_ANCHOR(anchor);
	COMPILE_POPED(anchor, "masgn lhs", node);
	REMOVE_ELEM(FIRST_ELEMENT(anchor));
	ADD_SEQ(ret, anchor);
      }
    }

    return COMPILE_OK;
}

static void
compile_massign_opt_lhs(rb_iseq_t *iseq, LINK_ANCHOR *ret, NODE *lhsn)
{
    if (lhsn) {
	compile_massign_opt_lhs(iseq, ret, lhsn->nd_next);
	compile_massign_lhs(iseq, ret, lhsn->nd_head);
    }
}

static int
compile_massign_opt(rb_iseq_t *iseq, LINK_ANCHOR *ret,
		    NODE *rhsn, NODE *orig_lhsn)
{
    VALUE mem[64];
    const int memsize = numberof(mem);
    int memindex = 0;
    int llen = 0, rlen = 0;
    int i;
    NODE *lhsn = orig_lhsn;

#define MEMORY(v) { \
    int i; \
    if (memindex == memsize) return 0; \
    for (i=0; i<memindex; i++) { \
	if (mem[i] == (v)) return 0; \
    } \
    mem[memindex++] = (v); \
}

    if (rhsn == 0 || nd_type(rhsn) != NODE_ARRAY) {
	return 0;
    }

    while (lhsn) {
	NODE *ln = lhsn->nd_head;
	switch (nd_type(ln)) {
	  case NODE_LASGN:
	    MEMORY(ln->nd_vid);
	    break;
	  case NODE_DASGN:
	  case NODE_DASGN_CURR:
	  case NODE_IASGN:
	  case NODE_IASGN2:
	  case NODE_CVASGN:
	    MEMORY(ln->nd_vid);
	    break;
	  default:
	    return 0;
	}
	lhsn = lhsn->nd_next;
	llen++;
    }

    while (rhsn) {
	if (llen <= rlen) {
	    COMPILE_POPED(ret, "masgn val (popped)", rhsn->nd_head);
	}
	else {
	    COMPILE(ret, "masgn val", rhsn->nd_head);
	}
	rhsn = rhsn->nd_next;
	rlen++;
    }

    if (llen > rlen) {
	for (i=0; i<llen-rlen; i++) {
	    ADD_INSN(ret, nd_line(orig_lhsn), putnil);
	}
    }

    compile_massign_opt_lhs(iseq, ret, orig_lhsn);
    return 1;
}

static int
compile_massign(rb_iseq_t *iseq, LINK_ANCHOR *ret, NODE *node, int poped)
{
    NODE *rhsn = node->nd_value;
    NODE *splatn = node->nd_args;
    NODE *lhsn = node->nd_head;
    int lhs_splat = (splatn && (VALUE)splatn != (VALUE)-1) ? 1 : 0;

    if (!poped || splatn || !compile_massign_opt(iseq, ret, rhsn, lhsn)) {
	int llen = 0;
	DECL_ANCHOR(lhsseq);

	INIT_ANCHOR(lhsseq);

	while (lhsn) {
	    compile_massign_lhs(iseq, lhsseq, lhsn->nd_head);
	    llen += 1;
	    lhsn = lhsn->nd_next;
	}

	COMPILE(ret, "normal masgn rhs", rhsn);

	if (!poped) {
	    ADD_INSN(ret, nd_line(node), dup);
	}

	ADD_INSN2(ret, nd_line(node), expandarray,
		  INT2FIX(llen), INT2FIX(lhs_splat));
	ADD_SEQ(ret, lhsseq);

	if (lhs_splat) {
	    if (nd_type(splatn) == NODE_POSTARG) {
		/*a, b, *r, p1, p2 */
		NODE *postn = splatn->nd_2nd;
		NODE *restn = splatn->nd_1st;
		int num = (int)postn->nd_alen;
		int flag = 0x02 | (((VALUE)restn == (VALUE)-1) ? 0x00 : 0x01);

		ADD_INSN2(ret, nd_line(splatn), expandarray,
			  INT2FIX(num), INT2FIX(flag));

		if ((VALUE)restn != (VALUE)-1) {
		    compile_massign_lhs(iseq, ret, restn);
		}
		while (postn) {
		    compile_massign_lhs(iseq, ret, postn->nd_head);
		    postn = postn->nd_next;
		}
	    }
	    else {
		/* a, b, *r */
		compile_massign_lhs(iseq, ret, splatn);
	    }
	}
    }
    return COMPILE_OK;
}

static int
compile_colon2(rb_iseq_t *iseq, NODE * node,
	       LINK_ANCHOR *pref, LINK_ANCHOR *body)
{
    switch (nd_type(node)) {
      case NODE_CONST:
	debugi("compile_colon2 - colon", node->nd_vid);
	ADD_INSN1(body, nd_line(node), getconstant, ID2SYM(node->nd_vid));
	break;
      case NODE_COLON3:
	debugi("compile_colon2 - colon3", node->nd_mid);
	ADD_INSN(body, nd_line(node), pop);
	ADD_INSN1(body, nd_line(node), putobject, rb_cObject);
	ADD_INSN1(body, nd_line(node), getconstant, ID2SYM(node->nd_mid));
	break;
      case NODE_COLON2:
	compile_colon2(iseq, node->nd_head, pref, body);
	debugi("compile_colon2 - colon2", node->nd_mid);
	ADD_INSN1(body, nd_line(node), getconstant, ID2SYM(node->nd_mid));
	break;
      default:
	COMPILE(pref, "const colon2 prefix", node);
	break;
    }
    return COMPILE_OK;
}

static VALUE
compile_cpath(LINK_ANCHOR *ret, rb_iseq_t *iseq, NODE *cpath)
{
    if (nd_type(cpath) == NODE_COLON3) {
	/* toplevel class ::Foo */
	ADD_INSN1(ret, nd_line(cpath), putobject, rb_cObject);
	return Qfalse;
    }
    else if (cpath->nd_head) {
	/* Bar::Foo */
	COMPILE(ret, "nd_else->nd_head", cpath->nd_head);
	return Qfalse;
    }
    else {
	/* class at cbase Foo */
	ADD_INSN1(ret, nd_line(cpath), putspecialobject,
		  INT2FIX(VM_SPECIAL_OBJECT_CONST_BASE));
	return Qtrue;
    }
}

#define defined_expr defined_expr0
static int
defined_expr(rb_iseq_t *iseq, LINK_ANCHOR *ret,
	     NODE *node, LABEL **lfinish, VALUE needstr)
{
    const char *estr = 0;
    enum node_type type;

    switch (type = nd_type(node)) {

	/* easy literals */
      case NODE_NIL:
	estr = "nil";
	break;
      case NODE_SELF:
	estr = "self";
	break;
      case NODE_TRUE:
	estr = "true";
	break;
      case NODE_FALSE:
	estr = "false";
	break;

      case NODE_ARRAY:{
	NODE *vals = node;

	do {
	    defined_expr(iseq, ret, vals->nd_head, lfinish, Qfalse);

	    if (!lfinish[1]) {
		lfinish[1] = NEW_LABEL(nd_line(node));
	    }
	    ADD_INSNL(ret, nd_line(node), branchunless, lfinish[1]);
	} while ((vals = vals->nd_next) != NULL);
      }
      case NODE_STR:
      case NODE_LIT:
      case NODE_ZARRAY:
      case NODE_AND:
      case NODE_OR:
      default:
	estr = "expression";
	break;

	/* variables */
      case NODE_LVAR:
      case NODE_DVAR:
	estr = "local-variable";
	break;

      case NODE_IVAR:
	ADD_INSN(ret, nd_line(node), putnil);
	ADD_INSN3(ret, nd_line(node), defined, INT2FIX(DEFINED_IVAR),
		  ID2SYM(node->nd_vid), needstr);
	return 1;

      case NODE_GVAR:
	ADD_INSN(ret, nd_line(node), putnil);
	ADD_INSN3(ret, nd_line(node), defined, INT2FIX(DEFINED_GVAR),
		  ID2SYM(node->nd_entry->id), needstr);
	return 1;

      case NODE_CVAR:
	ADD_INSN(ret, nd_line(node), putnil);
	ADD_INSN3(ret, nd_line(node), defined, INT2FIX(DEFINED_CVAR),
		  ID2SYM(node->nd_vid), needstr);
	return 1;

      case NODE_CONST:
	ADD_INSN(ret, nd_line(node), putnil);
	ADD_INSN3(ret, nd_line(node), defined, INT2FIX(DEFINED_CONST),
		  ID2SYM(node->nd_vid), needstr);
	return 1;
      case NODE_COLON2:
	if (!lfinish[1]) {
	    lfinish[1] = NEW_LABEL(nd_line(node));
	}
	defined_expr(iseq, ret, node->nd_head, lfinish, Qfalse);
	ADD_INSNL(ret, nd_line(node), branchunless, lfinish[1]);

	if (rb_is_const_id(node->nd_mid)) {
	    COMPILE(ret, "defined/colon2#nd_head", node->nd_head);
	    ADD_INSN3(ret, nd_line(node), defined, INT2FIX(DEFINED_CONST),
		      ID2SYM(node->nd_mid), needstr);
	}
	else {
	    COMPILE(ret, "defined/colon2#nd_head", node->nd_head);
	    ADD_INSN3(ret, nd_line(node), defined, INT2FIX(DEFINED_METHOD),
		      ID2SYM(node->nd_mid), needstr);
	}
	return 1;
      case NODE_COLON3:
	ADD_INSN1(ret, nd_line(node), putobject, rb_cObject);
	ADD_INSN3(ret, nd_line(node), defined,
		  INT2FIX(DEFINED_CONST), ID2SYM(node->nd_mid), needstr);
	return 1;

	/* method dispatch */
      case NODE_CALL:
      case NODE_VCALL:
      case NODE_FCALL:
      case NODE_ATTRASGN:{
	int self = TRUE;

	switch (type) {
	  case NODE_ATTRASGN:
	    if (node->nd_recv == (NODE *)1) break;
	  case NODE_CALL:
	    self = FALSE;
	    break;
	  default:
	    /* through */;
	}
	if (!lfinish[1]) {
	    lfinish[1] = NEW_LABEL(nd_line(node));
	}
	if (node->nd_args) {
	    defined_expr(iseq, ret, node->nd_args, lfinish, Qfalse);
	    ADD_INSNL(ret, nd_line(node), branchunless, lfinish[1]);
	}
	if (!self) {
	    defined_expr(iseq, ret, node->nd_recv, lfinish, Qfalse);
	    ADD_INSNL(ret, nd_line(node), branchunless, lfinish[1]);
	    COMPILE(ret, "defined/recv", node->nd_recv);
	    ADD_INSN3(ret, nd_line(node), defined, INT2FIX(DEFINED_METHOD),
		      ID2SYM(node->nd_mid), needstr);
	}
	else {
	    ADD_INSN(ret, nd_line(node), putself);
	    ADD_INSN3(ret, nd_line(node), defined, INT2FIX(DEFINED_FUNC),
		      ID2SYM(node->nd_mid), needstr);
	}
	return 1;
      }

      case NODE_YIELD:
	ADD_INSN(ret, nd_line(node), putnil);
	ADD_INSN3(ret, nd_line(node), defined, INT2FIX(DEFINED_YIELD), 0,
		  needstr);
	return 1;

      case NODE_BACK_REF:
      case NODE_NTH_REF:
	ADD_INSN(ret, nd_line(node), putnil);
	ADD_INSN3(ret, nd_line(node), defined, INT2FIX(DEFINED_REF),
		  INT2FIX((node->nd_nth << 1) | (type == NODE_BACK_REF)),
		  needstr);
	return 1;

      case NODE_SUPER:
      case NODE_ZSUPER:
	ADD_INSN(ret, nd_line(node), putnil);
	ADD_INSN3(ret, nd_line(node), defined, INT2FIX(DEFINED_ZSUPER), 0,
		  needstr);
	return 1;

      case NODE_OP_ASGN1:
      case NODE_OP_ASGN2:
      case NODE_OP_ASGN_OR:
      case NODE_OP_ASGN_AND:
      case NODE_MASGN:
      case NODE_LASGN:
      case NODE_DASGN:
      case NODE_DASGN_CURR:
      case NODE_GASGN:
      case NODE_IASGN:
      case NODE_CDECL:
      case NODE_CVDECL:
      case NODE_CVASGN:
	estr = "assignment";
	break;
    }

    if (estr != 0) {
	if (needstr != Qfalse) {
	    VALUE str = rb_str_new2(estr);
	    hide_obj(str);
	    ADD_INSN1(ret, nd_line(node), putstring, str);
	    iseq_add_mark_object_compile_time(iseq, str);
	}
	else {
	    ADD_INSN1(ret, nd_line(node), putobject, Qtrue);
	}
	return 1;
    }
    return 0;
}
#undef defined_expr

static int
defined_expr(rb_iseq_t *iseq, LINK_ANCHOR *ret,
	     NODE *node, LABEL **lfinish, VALUE needstr)
{
    LINK_ELEMENT *lcur = ret->last;
    int done = defined_expr0(iseq, ret, node, lfinish, needstr);
    if (lfinish[1]) {
	int line = nd_line(node);
	LABEL *lstart = NEW_LABEL(line);
	LABEL *lend = NEW_LABEL(line);
	VALUE rescue = NEW_CHILD_ISEQVAL(NEW_NIL(),
					 rb_str_concat(rb_str_new2
						       ("defined guard in "),
						       iseq->name),
					 ISEQ_TYPE_DEFINED_GUARD, 0);
	APPEND_LABEL(ret, lcur, lstart);
	ADD_LABEL(ret, lend);
	ADD_CATCH_ENTRY(CATCH_TYPE_RESCUE, lstart, lend, rescue, lfinish[1]);
    }
    return done;
}

#define BUFSIZE 0x100

static VALUE
make_name_for_block(rb_iseq_t *iseq)
{
    int level = 1;
    rb_iseq_t *ip = iseq;

    if (iseq->parent_iseq != 0) {
	while (ip->local_iseq != ip) {
	    if (ip->type == ISEQ_TYPE_BLOCK) {
		level++;
	    }
	    ip = ip->parent_iseq;
	}
    }

    if (level == 1) {
	return rb_sprintf("block in %s", RSTRING_PTR(ip->name));
    }
    else {
	return rb_sprintf("block (%d levels) in %s", level, RSTRING_PTR(ip->name));
    }
}

static void
push_ensure_entry(rb_iseq_t *iseq,
		  struct iseq_compile_data_ensure_node_stack *enl,
		  struct ensure_range *er, NODE *node)
{
    enl->ensure_node = node;
    enl->prev = iseq->compile_data->ensure_node_stack;	/* prev */
    enl->erange = er;
    iseq->compile_data->ensure_node_stack = enl;
}

static void
add_ensure_range(rb_iseq_t *iseq, struct ensure_range *erange,
		 LABEL *lstart, LABEL *lend)
{
    struct ensure_range *ne =
	compile_data_alloc(iseq, sizeof(struct ensure_range));

    while (erange->next != 0) {
	erange = erange->next;
    }
    ne->next = 0;
    ne->begin = lend;
    ne->end = erange->end;
    erange->end = lstart;

    erange->next = ne;
}

static void
add_ensure_iseq(LINK_ANCHOR *ret, rb_iseq_t *iseq, int is_return)
{
    struct iseq_compile_data_ensure_node_stack *enlp =
	iseq->compile_data->ensure_node_stack;
    struct iseq_compile_data_ensure_node_stack *prev_enlp = enlp;
    DECL_ANCHOR(ensure);

    INIT_ANCHOR(ensure);
    while (enlp) {
	if (enlp->erange != 0) {
	    DECL_ANCHOR(ensure_part);
	    LABEL *lstart = NEW_LABEL(0);
	    LABEL *lend = NEW_LABEL(0);
	    INIT_ANCHOR(ensure_part);

	    add_ensure_range(iseq, enlp->erange, lstart, lend);

	    iseq->compile_data->ensure_node_stack = enlp->prev;
	    ADD_LABEL(ensure_part, lstart);
	    COMPILE_POPED(ensure_part, "ensure part", enlp->ensure_node);
	    ADD_LABEL(ensure_part, lend);
	    ADD_SEQ(ensure, ensure_part);
	}
	else {
	    if (!is_return) {
		break;
	    }
	}
	enlp = enlp->prev;
    }
    iseq->compile_data->ensure_node_stack = prev_enlp;
    ADD_SEQ(ret, ensure);
}

static VALUE
setup_args(rb_iseq_t *iseq, LINK_ANCHOR *args, NODE *argn, VALUE *flag)
{
    VALUE argc = INT2FIX(0);
    int nsplat = 0;
    DECL_ANCHOR(arg_block);
    DECL_ANCHOR(args_splat);

    INIT_ANCHOR(arg_block);
    INIT_ANCHOR(args_splat);
    if (argn && nd_type(argn) == NODE_BLOCK_PASS) {
	COMPILE(arg_block, "block", argn->nd_body);
	*flag |= VM_CALL_ARGS_BLOCKARG_BIT;
	argn = argn->nd_head;
    }

  setup_argn:
    if (argn) {
	switch (nd_type(argn)) {
	  case NODE_SPLAT: {
	    COMPILE(args, "args (splat)", argn->nd_head);
	    argc = INT2FIX(1);
	    nsplat++;
	    *flag |= VM_CALL_ARGS_SPLAT_BIT;
	    break;
	  }
	  case NODE_ARGSCAT:
	  case NODE_ARGSPUSH: {
	    int next_is_array = (nd_type(argn->nd_head) == NODE_ARRAY);
	    DECL_ANCHOR(tmp);

	    INIT_ANCHOR(tmp);
	    COMPILE(tmp, "args (cat: splat)", argn->nd_body);
	    if (next_is_array && nsplat == 0) {
		/* none */
	    }
	    else {
		if (nd_type(argn) == NODE_ARGSCAT) {
		    ADD_INSN1(tmp, nd_line(argn), splatarray, Qfalse);
		}
		else {
		    ADD_INSN1(tmp, nd_line(argn), newarray, INT2FIX(1));
		}
	    }
	    INSERT_LIST(args_splat, tmp);
	    nsplat++;
	    *flag |= VM_CALL_ARGS_SPLAT_BIT;

	    if (next_is_array) {
		argc = INT2FIX(compile_array(iseq, args, argn->nd_head, Qfalse) + 1);
		POP_ELEMENT(args);
	    }
	    else {
		argn = argn->nd_head;
		goto setup_argn;
	    }
	    break;
	  }
	  case NODE_ARRAY: {
	    argc = INT2FIX(compile_array(iseq, args, argn, Qfalse));
	    POP_ELEMENT(args);
	    break;
	  }
	  default: {
	    rb_bug("setup_arg: unknown node: %s\n", ruby_node_name(nd_type(argn)));
	  }
	}
    }

    if (nsplat > 1) {
	int i;
	for (i=1; i<nsplat; i++) {
	    ADD_INSN(args_splat, nd_line(args), concatarray);
	}
    }

    if (!LIST_SIZE_ZERO(args_splat)) {
	ADD_SEQ(args, args_splat);
    }

    if (*flag & VM_CALL_ARGS_BLOCKARG_BIT) {
	ADD_SEQ(args, arg_block);
    }
    return argc;
}


/**
  compile each node

  self:  InstructionSequence
  node:  Ruby compiled node
  poped: This node will be poped
 */
static int
iseq_compile_each(rb_iseq_t *iseq, LINK_ANCHOR *ret, NODE * node, int poped)
{
    enum node_type type;

    if (node == 0) {
	if (!poped) {
	    debugs("node: NODE_NIL(implicit)\n");
	    ADD_INSN(ret, iseq->compile_data->last_line, putnil);
	}
	return COMPILE_OK;
    }

    iseq->compile_data->last_line = (int)nd_line(node);
    debug_node_start(node);

    type = nd_type(node);

    if (node->flags & NODE_FL_NEWLINE) {
	ADD_TRACE(ret, nd_line(node), RUBY_EVENT_LINE);
    }

    switch (type) {
      case NODE_BLOCK:{
	while (node && nd_type(node) == NODE_BLOCK) {
	    COMPILE_(ret, "BLOCK body", node->nd_head,
		     (node->nd_next == 0 && poped == 0) ? 0 : 1);
	    node = node->nd_next;
	}
	if (node) {
	    COMPILE_(ret, "BLOCK next", node->nd_next, poped);
	}
	break;
      }
      case NODE_IF:{
	DECL_ANCHOR(cond_seq);
	DECL_ANCHOR(then_seq);
	DECL_ANCHOR(else_seq);
	LABEL *then_label, *else_label, *end_label;

	INIT_ANCHOR(cond_seq);
	INIT_ANCHOR(then_seq);
	INIT_ANCHOR(else_seq);
	then_label = NEW_LABEL(nd_line(node));
	else_label = NEW_LABEL(nd_line(node));
	end_label = NEW_LABEL(nd_line(node));

	compile_branch_condition(iseq, cond_seq, node->nd_cond,
				 then_label, else_label);
	COMPILE_(then_seq, "then", node->nd_body, poped);
	COMPILE_(else_seq, "else", node->nd_else, poped);

	ADD_SEQ(ret, cond_seq);

	ADD_LABEL(ret, then_label);
	ADD_SEQ(ret, then_seq);
	ADD_INSNL(ret, nd_line(node), jump, end_label);

	ADD_LABEL(ret, else_label);
	ADD_SEQ(ret, else_seq);

	ADD_LABEL(ret, end_label);

	break;
      }
      case NODE_CASE:{
	NODE *vals;
	NODE *tempnode = node;
	LABEL *endlabel, *elselabel;
	DECL_ANCHOR(head);
	DECL_ANCHOR(body_seq);
	DECL_ANCHOR(cond_seq);
	VALUE special_literals = rb_ary_tmp_new(1);

	INIT_ANCHOR(head);
	INIT_ANCHOR(body_seq);
	INIT_ANCHOR(cond_seq);
	if (node->nd_head == 0) {
	    COMPILE_(ret, "when", node->nd_body, poped);
	    break;
	}
	COMPILE(head, "case base", node->nd_head);

	node = node->nd_body;
	type = nd_type(node);

	if (type != NODE_WHEN) {
	    COMPILE_ERROR((ERROR_ARGS "NODE_CASE: unexpected node. must be NODE_WHEN, but %s", ruby_node_name(type)));
	}

	endlabel = NEW_LABEL(nd_line(node));
	elselabel = NEW_LABEL(nd_line(node));

	ADD_SEQ(ret, head);	/* case VAL */

	while (type == NODE_WHEN) {
	    LABEL *l1;

	    l1 = NEW_LABEL(nd_line(node));
	    ADD_LABEL(body_seq, l1);
	    ADD_INSN(body_seq, nd_line(node), pop);
	    COMPILE_(body_seq, "when body", node->nd_body, poped);
	    ADD_INSNL(body_seq, nd_line(node), jump, endlabel);

	    vals = node->nd_head;
	    if (vals) {
		switch (nd_type(vals)) {
		  case NODE_ARRAY:
		    special_literals = when_vals(iseq, cond_seq, vals, l1, special_literals);
		    break;
		  case NODE_SPLAT:
		  case NODE_ARGSCAT:
		  case NODE_ARGSPUSH:
		    special_literals = 0;
		    COMPILE(cond_seq, "when/cond splat", vals);
		    ADD_INSN1(cond_seq, nd_line(vals), checkincludearray, Qtrue);
		    ADD_INSNL(cond_seq, nd_line(vals), branchif, l1);
		    break;
		  default:
		    rb_bug("NODE_CASE: unknown node (%s)",
			   ruby_node_name(nd_type(vals)));
		}
	    }
	    else {
		rb_bug("NODE_CASE: must be NODE_ARRAY, but 0");
	    }

	    node = node->nd_next;
	    if (!node) {
		break;
	    }
	    type = nd_type(node);
	}
	/* else */
	if (node) {
	    ADD_LABEL(cond_seq, elselabel);
	    ADD_INSN(cond_seq, nd_line(node), pop);
	    COMPILE_(cond_seq, "else", node, poped);
	    ADD_INSNL(cond_seq, nd_line(node), jump, endlabel);
	}
	else {
	    debugs("== else (implicit)\n");
	    ADD_LABEL(cond_seq, elselabel);
	    ADD_INSN(cond_seq, nd_line(tempnode), pop);
	    if (!poped) {
		ADD_INSN(cond_seq, nd_line(tempnode), putnil);
	    }
	    ADD_INSNL(cond_seq, nd_line(tempnode), jump, endlabel);
	}

	if (special_literals) {
	    ADD_INSN(ret, nd_line(tempnode), dup);
	    ADD_INSN2(ret, nd_line(tempnode), opt_case_dispatch,
		      special_literals, elselabel);
	    iseq_add_mark_object_compile_time(iseq, special_literals);
	}

	ADD_SEQ(ret, cond_seq);
	ADD_SEQ(ret, body_seq);
	ADD_LABEL(ret, endlabel);
	break;
      }
      case NODE_WHEN:{
	NODE *vals;
	NODE *val;
	NODE *orig_node = node;
	LABEL *endlabel;
	DECL_ANCHOR(body_seq);

	INIT_ANCHOR(body_seq);
	endlabel = NEW_LABEL(nd_line(node));

	while (node && nd_type(node) == NODE_WHEN) {
	    LABEL *l1 = NEW_LABEL(nd_line(node));
	    ADD_LABEL(body_seq, l1);
	    COMPILE_(body_seq, "when", node->nd_body, poped);
	    ADD_INSNL(body_seq, nd_line(node), jump, endlabel);

	    vals = node->nd_head;
	    if (!vals) {
		rb_bug("NODE_WHEN: must be NODE_ARRAY, but 0");
	    }
	    switch (nd_type(vals)) {
	      case NODE_ARRAY:
		while (vals) {
		    val = vals->nd_head;
		    COMPILE(ret, "when2", val);
		    ADD_INSNL(ret, nd_line(val), branchif, l1);
		    vals = vals->nd_next;
		}
		break;
	      case NODE_SPLAT:
	      case NODE_ARGSCAT:
	      case NODE_ARGSPUSH:
		ADD_INSN(ret, nd_line(vals), putnil);
		COMPILE(ret, "when2/cond splat", vals);
		ADD_INSN1(ret, nd_line(vals), checkincludearray, Qfalse);
		ADD_INSN(ret, nd_line(vals), pop);
		ADD_INSNL(ret, nd_line(vals), branchif, l1);
		break;
	      default:
		rb_bug("NODE_WHEN: unknown node (%s)",
		       ruby_node_name(nd_type(vals)));
	    }
	    node = node->nd_next;
	}
	/* else */
	COMPILE_(ret, "else", node, poped);
	ADD_INSNL(ret, nd_line(orig_node), jump, endlabel);

	ADD_SEQ(ret, body_seq);
	ADD_LABEL(ret, endlabel);

	break;
      }
      case NODE_OPT_N:
      case NODE_WHILE:
      case NODE_UNTIL:{
	LABEL *prev_start_label = iseq->compile_data->start_label;
	LABEL *prev_end_label = iseq->compile_data->end_label;
	LABEL *prev_redo_label = iseq->compile_data->redo_label;
	int prev_loopval_popped = iseq->compile_data->loopval_popped;

	struct iseq_compile_data_ensure_node_stack enl;

	LABEL *next_label = iseq->compile_data->start_label = NEW_LABEL(nd_line(node));	/* next  */
	LABEL *redo_label = iseq->compile_data->redo_label = NEW_LABEL(nd_line(node));	/* redo  */
	LABEL *break_label = iseq->compile_data->end_label = NEW_LABEL(nd_line(node));	/* break */
	LABEL *end_label = NEW_LABEL(nd_line(node));

	LABEL *next_catch_label = NEW_LABEL(nd_line(node));
	LABEL *tmp_label = NULL;

	iseq->compile_data->loopval_popped = 0;
	push_ensure_entry(iseq, &enl, 0, 0);

	if (type == NODE_OPT_N || node->nd_state == 1) {
	    ADD_INSNL(ret, nd_line(node), jump, next_label);
	}
	else {
	    tmp_label = NEW_LABEL(nd_line(node));
	    ADD_INSNL(ret, nd_line(node), jump, tmp_label);
	}
	ADD_INSN(ret, nd_line(node), putnil);
	ADD_LABEL(ret, next_catch_label);
	ADD_INSN(ret, nd_line(node), pop);
	ADD_INSNL(ret, nd_line(node), jump, next_label);
	if (tmp_label) ADD_LABEL(ret, tmp_label);

	ADD_LABEL(ret, redo_label);
	COMPILE_POPED(ret, "while body", node->nd_body);
	ADD_LABEL(ret, next_label);	/* next */

	if (type == NODE_WHILE) {
	    compile_branch_condition(iseq, ret, node->nd_cond,
				     redo_label, end_label);
	}
	else if (type == NODE_UNTIL) {
	    /* untile */
	    compile_branch_condition(iseq, ret, node->nd_cond,
				     end_label, redo_label);
	}
	else {
	    ADD_CALL_RECEIVER(ret, nd_line(node));
	    ADD_CALL(ret, nd_line(node), ID2SYM(idGets), INT2FIX(0));
	    ADD_INSNL(ret, nd_line(node), branchif, redo_label);
	    /* opt_n */
	}

	ADD_LABEL(ret, end_label);

	if (node->nd_state == Qundef) {
	    /* ADD_INSN(ret, nd_line(node), putundef); */
	    rb_bug("unsupported: putundef");
	}
	else {
	    ADD_INSN(ret, nd_line(node), putnil);
	}

	ADD_LABEL(ret, break_label);	/* break */

	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}

	ADD_CATCH_ENTRY(CATCH_TYPE_BREAK, redo_label, break_label,
			0, break_label);
	ADD_CATCH_ENTRY(CATCH_TYPE_NEXT, redo_label, break_label, 0,
			next_catch_label);
	ADD_CATCH_ENTRY(CATCH_TYPE_REDO, redo_label, break_label, 0,
			iseq->compile_data->redo_label);

	iseq->compile_data->start_label = prev_start_label;
	iseq->compile_data->end_label = prev_end_label;
	iseq->compile_data->redo_label = prev_redo_label;
	iseq->compile_data->loopval_popped = prev_loopval_popped;
	iseq->compile_data->ensure_node_stack = iseq->compile_data->ensure_node_stack->prev;
	break;
      }
      case NODE_ITER:
      case NODE_FOR:{
	VALUE prevblock = iseq->compile_data->current_block;
	LABEL *retry_label = NEW_LABEL(nd_line(node));
	LABEL *retry_end_l = NEW_LABEL(nd_line(node));

	ADD_LABEL(ret, retry_label);
	if (nd_type(node) == NODE_FOR) {
	    COMPILE(ret, "iter caller (for)", node->nd_iter);

	    iseq->compile_data->current_block =
		NEW_CHILD_ISEQVAL(node->nd_body, make_name_for_block(iseq),
				  ISEQ_TYPE_BLOCK, nd_line(node));

	    ADD_SEND_R(ret, nd_line(node), ID2SYM(idEach), INT2FIX(0),
		       iseq->compile_data->current_block, INT2FIX(0));
	}
	else {
	    iseq->compile_data->current_block =
		NEW_CHILD_ISEQVAL(node->nd_body, make_name_for_block(iseq),
				  ISEQ_TYPE_BLOCK, nd_line(node));
	    COMPILE(ret, "iter caller", node->nd_iter);
	}
	ADD_LABEL(ret, retry_end_l);

	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}

	iseq->compile_data->current_block = prevblock;

	ADD_CATCH_ENTRY(CATCH_TYPE_BREAK, retry_label, retry_end_l, 0, retry_end_l);

	break;
      }
      case NODE_BREAK:{
	unsigned long level = 0;

	if (iseq->compile_data->redo_label != 0) {
	    /* while/until */
	    LABEL *splabel = NEW_LABEL(0);
	    ADD_LABEL(ret, splabel);
	    ADD_ADJUST(ret, nd_line(node), iseq->compile_data->redo_label);
	    COMPILE_(ret, "break val (while/until)", node->nd_stts, iseq->compile_data->loopval_popped);
	    add_ensure_iseq(ret, iseq, 0);
	    ADD_INSNL(ret, nd_line(node), jump, iseq->compile_data->end_label);
	    ADD_ADJUST_RESTORE(ret, splabel);

	    if (!poped) {
		ADD_INSN(ret, nd_line(node), putnil);
	    }
	}
	else if (iseq->type == ISEQ_TYPE_BLOCK) {
	  break_by_insn:
	    /* escape from block */
	    COMPILE(ret, "break val (block)", node->nd_stts);
	    ADD_INSN1(ret, nd_line(node), throw, INT2FIX(level | 0x02) /* TAG_BREAK */ );
	    if (poped) {
		ADD_INSN(ret, nd_line(node), pop);
	    }
	}
	else if (iseq->type == ISEQ_TYPE_EVAL) {
	  break_in_eval:
	    COMPILE_ERROR((ERROR_ARGS "Can't escape from eval with break"));
	}
	else {
	    rb_iseq_t *ip = iseq->parent_iseq;
	    while (ip) {
		if (!ip->compile_data) {
		    ip = 0;
		    break;
		}

		level++;
		if (ip->compile_data->redo_label != 0) {
		    level = 0x8000;
		    if (ip->compile_data->loopval_popped == 0) {
			/* need value */
			level |= 0x4000;
		    }
		    goto break_by_insn;
		}
		else if (ip->type == ISEQ_TYPE_BLOCK) {
		    level <<= 16;
		    goto break_by_insn;
		}
		else if (ip->type == ISEQ_TYPE_EVAL) {
		    goto break_in_eval;
		}

		ip = ip->parent_iseq;
	    }
	    COMPILE_ERROR((ERROR_ARGS "Invalid break"));
	}
	break;
      }
      case NODE_NEXT:{
	unsigned long level = 0;

	if (iseq->compile_data->redo_label != 0) {
	    LABEL *splabel = NEW_LABEL(0);
	    debugs("next in while loop\n");
	    ADD_LABEL(ret, splabel);
	    COMPILE(ret, "next val/valid syntax?", node->nd_stts);
	    add_ensure_iseq(ret, iseq, 0);
	    ADD_ADJUST(ret, nd_line(node), iseq->compile_data->redo_label);
	    ADD_INSNL(ret, nd_line(node), jump, iseq->compile_data->start_label);
	    ADD_ADJUST_RESTORE(ret, splabel);
	    if (!poped) {
		ADD_INSN(ret, nd_line(node), putnil);
	    }
	}
	else if (iseq->compile_data->end_label) {
	    LABEL *splabel = NEW_LABEL(0);
	    debugs("next in block\n");
	    ADD_LABEL(ret, splabel);
	    ADD_ADJUST(ret, nd_line(node), iseq->compile_data->start_label);
	    COMPILE(ret, "next val", node->nd_stts);
	    add_ensure_iseq(ret, iseq, 0);
	    ADD_INSNL(ret, nd_line(node), jump, iseq->compile_data->end_label);
	    ADD_ADJUST_RESTORE(ret, splabel);

	    if (!poped) {
		ADD_INSN(ret, nd_line(node), putnil);
	    }
	}
	else if (iseq->type == ISEQ_TYPE_EVAL) {
	  next_in_eval:
	    COMPILE_ERROR((ERROR_ARGS "Can't escape from eval with next"));
	}
	else {
	    rb_iseq_t *ip;
	    ip = iseq;
	    while (ip) {
		if (!ip->compile_data) {
		    ip = 0;
		    break;
		}

		level = 0x8000 | 0x4000;
		if (ip->compile_data->redo_label != 0) {
		    /* while loop */
		    break;
		}
		else if (ip->type == ISEQ_TYPE_BLOCK) {
		    break;
		}
		else if (ip->type == ISEQ_TYPE_EVAL) {
		    goto next_in_eval;
		}

		ip = ip->parent_iseq;
	    }
	    if (ip != 0) {
		COMPILE(ret, "next val", node->nd_stts);
		ADD_INSN1(ret, nd_line(node), throw, INT2FIX(level | 0x03) /* TAG_NEXT */ );

		if (poped) {
		    ADD_INSN(ret, nd_line(node), pop);
		}
	    }
	    else {
		COMPILE_ERROR((ERROR_ARGS "Invalid next"));
	    }
	}
	break;
      }
      case NODE_REDO:{
	if (iseq->compile_data->redo_label) {
	    LABEL *splabel = NEW_LABEL(0);
	    debugs("redo in while");
	    ADD_LABEL(ret, splabel);
	    ADD_ADJUST(ret, nd_line(node), iseq->compile_data->redo_label);
	    add_ensure_iseq(ret, iseq, 0);
	    ADD_INSNL(ret, nd_line(node), jump, iseq->compile_data->redo_label);
	    ADD_ADJUST_RESTORE(ret, splabel);
	    if (!poped) {
		ADD_INSN(ret, nd_line(node), putnil);
	    }
	}
	else if (iseq->type == ISEQ_TYPE_EVAL) {
	  redo_in_eval:
	    COMPILE_ERROR((ERROR_ARGS "Can't escape from eval with redo"));
	}
	else if (iseq->compile_data->start_label) {
	    LABEL *splabel = NEW_LABEL(0);

	    debugs("redo in block");
	    ADD_LABEL(ret, splabel);
	    add_ensure_iseq(ret, iseq, 0);
	    ADD_ADJUST(ret, nd_line(node), iseq->compile_data->start_label);
	    ADD_INSNL(ret, nd_line(node), jump, iseq->compile_data->start_label);
	    ADD_ADJUST_RESTORE(ret, splabel);

	    if (!poped) {
		ADD_INSN(ret, nd_line(node), putnil);
	    }
	}
	else {
	    rb_iseq_t *ip;
	    unsigned long level;
	    level = 0x8000 | 0x4000;
	    ip = iseq;
	    while (ip) {
		if (!ip->compile_data) {
		    ip = 0;
		    break;
		}

		if (ip->compile_data->redo_label != 0) {
		    break;
		}
		else if (ip->type == ISEQ_TYPE_BLOCK) {
		    break;
		}
		else if (ip->type == ISEQ_TYPE_EVAL) {
		    goto redo_in_eval;
		}

		ip = ip->parent_iseq;
	    }
	    if (ip != 0) {
		ADD_INSN(ret, nd_line(node), putnil);
		ADD_INSN1(ret, nd_line(node), throw, INT2FIX(level | 0x05) /* TAG_REDO */ );

		if (poped) {
		    ADD_INSN(ret, nd_line(node), pop);
		}
	    }
	    else {
		COMPILE_ERROR((ERROR_ARGS "Invalid redo"));
	    }
	}
	break;
      }
      case NODE_RETRY:{
	if (iseq->type == ISEQ_TYPE_RESCUE) {
	    ADD_INSN(ret, nd_line(node), putnil);
	    ADD_INSN1(ret, nd_line(node), throw, INT2FIX(0x04) /* TAG_RETRY */ );

	    if (poped) {
		ADD_INSN(ret, nd_line(node), pop);
	    }
	}
	else {
	    COMPILE_ERROR((ERROR_ARGS "Invalid retry"));
	}
	break;
      }
      case NODE_BEGIN:{
	COMPILE_(ret, "NODE_BEGIN", node->nd_body, poped);
	break;
      }
      case NODE_RESCUE:{
	LABEL *lstart = NEW_LABEL(nd_line(node));
	LABEL *lend = NEW_LABEL(nd_line(node));
	LABEL *lcont = NEW_LABEL(nd_line(node));
	VALUE rescue = NEW_CHILD_ISEQVAL(
	    node->nd_resq,
	    rb_str_concat(rb_str_new2("rescue in "), iseq->name),
	    ISEQ_TYPE_RESCUE, nd_line(node));

	ADD_LABEL(ret, lstart);
	COMPILE(ret, "rescue head", node->nd_head);
	ADD_LABEL(ret, lend);
	if (node->nd_else) {
	    ADD_INSN(ret, nd_line(node), pop);
	    COMPILE(ret, "rescue else", node->nd_else);
	}
	ADD_INSN(ret, nd_line(node), nop);
	ADD_LABEL(ret, lcont);

	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}

	/* register catch entry */
	ADD_CATCH_ENTRY(CATCH_TYPE_RESCUE, lstart, lend, rescue, lcont);
	ADD_CATCH_ENTRY(CATCH_TYPE_RETRY, lend, lcont, 0, lstart);
	break;
      }
      case NODE_RESBODY:{
	NODE *resq = node;
	NODE *narg;
	LABEL *label_miss, *label_hit;

	while (resq) {
	    label_miss = NEW_LABEL(nd_line(node));
	    label_hit = NEW_LABEL(nd_line(node));

	    narg = resq->nd_args;
	    if (narg) {
		switch (nd_type(narg)) {
		  case NODE_ARRAY:
		    while (narg) {
			COMPILE(ret, "rescue arg", narg->nd_head);
			ADD_INSN2(ret, nd_line(node), getdynamic, INT2FIX(2), INT2FIX(0));
			ADD_SEND(ret, nd_line(node), ID2SYM(idEqq), INT2FIX(1));
			ADD_INSNL(ret, nd_line(node), branchif, label_hit);
			narg = narg->nd_next;
		    }
		    break;
		  case NODE_SPLAT:
		  case NODE_ARGSCAT:
		  case NODE_ARGSPUSH:
		    ADD_INSN2(ret, nd_line(node), getdynamic, INT2FIX(2), INT2FIX(0));
		    COMPILE(ret, "rescue/cond splat", narg);
		    ADD_INSN1(ret, nd_line(node), checkincludearray, Qtrue);
		    ADD_INSN(ret, nd_line(node), swap);
		    ADD_INSN(ret, nd_line(node), pop);
		    ADD_INSNL(ret, nd_line(node), branchif, label_hit);
		    break;
		  default:
		    rb_bug("NODE_RESBODY: unknown node (%s)",
			   ruby_node_name(nd_type(narg)));
		}
	    }
	    else {
		ADD_INSN1(ret, nd_line(node), putobject,
			  rb_eStandardError);
		ADD_INSN2(ret, nd_line(node), getdynamic, INT2FIX(2), INT2FIX(0));
		ADD_SEND(ret, nd_line(node), ID2SYM(idEqq), INT2FIX(1));
		ADD_INSNL(ret, nd_line(node), branchif, label_hit);
	    }
	    ADD_INSNL(ret, nd_line(node), jump, label_miss);
	    ADD_LABEL(ret, label_hit);
	    COMPILE(ret, "resbody body", resq->nd_body);
	    if (iseq->compile_data->option->tailcall_optimization) {
		ADD_INSN(ret, nd_line(node), nop);
	    }
	    ADD_INSN(ret, nd_line(node), leave);
	    ADD_LABEL(ret, label_miss);
	    resq = resq->nd_head;
	}
	break;
      }
      case NODE_ENSURE:{
	DECL_ANCHOR(ensr);
	VALUE ensure = NEW_CHILD_ISEQVAL(node->nd_ensr,
					 rb_str_concat(rb_str_new2
						       ("ensure in "),
						       iseq->name),
					 ISEQ_TYPE_ENSURE, nd_line(node));
	LABEL *lstart = NEW_LABEL(nd_line(node));
	LABEL *lend = NEW_LABEL(nd_line(node));
	LABEL *lcont = NEW_LABEL(nd_line(node));
	struct ensure_range er;
	struct iseq_compile_data_ensure_node_stack enl;
	struct ensure_range *erange;

	INIT_ANCHOR(ensr);
	COMPILE_POPED(ensr, "ensure ensr", node->nd_ensr);

	er.begin = lstart;
	er.end = lend;
	er.next = 0;
	push_ensure_entry(iseq, &enl, &er, node->nd_ensr);

	ADD_LABEL(ret, lstart);
	COMPILE_(ret, "ensure head", node->nd_head, poped);
	ADD_LABEL(ret, lend);
	if (ensr->anchor.next == 0) {
	    ADD_INSN(ret, nd_line(node), nop);
	}
	else {
	    ADD_SEQ(ret, ensr);
	}
	ADD_LABEL(ret, lcont);

	erange = iseq->compile_data->ensure_node_stack->erange;
	while (erange) {
	    ADD_CATCH_ENTRY(CATCH_TYPE_ENSURE, erange->begin, erange->end,
			    ensure, lcont);
	    erange = erange->next;
	}

	iseq->compile_data->ensure_node_stack = enl.prev;
	break;
      }

      case NODE_AND:
      case NODE_OR:{
	LABEL *end_label = NEW_LABEL(nd_line(node));
	COMPILE(ret, "nd_1st", node->nd_1st);
	if (!poped) {
	    ADD_INSN(ret, nd_line(node), dup);
	}
	if (type == NODE_AND) {
	    ADD_INSNL(ret, nd_line(node), branchunless, end_label);
	}
	else {
	    ADD_INSNL(ret, nd_line(node), branchif, end_label);
	}
	if (!poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}
	COMPILE_(ret, "nd_2nd", node->nd_2nd, poped);
	ADD_LABEL(ret, end_label);
	break;
      }

      case NODE_MASGN:{
	compile_massign(iseq, ret, node, poped);
	break;
      }

      case NODE_LASGN:{
	ID id = node->nd_vid;
	int idx = iseq->local_iseq->local_size - get_local_var_idx(iseq, id);

	debugs("lvar: %s idx: %d\n", rb_id2name(id), idx);
	COMPILE(ret, "rvalue", node->nd_value);

	if (!poped) {
	    ADD_INSN(ret, nd_line(node), dup);
	}
	ADD_INSN1(ret, nd_line(node), setlocal, INT2FIX(idx));

	break;
      }
      case NODE_DASGN:
      case NODE_DASGN_CURR:{
	int idx, lv, ls;
	COMPILE(ret, "dvalue", node->nd_value);
	debugp_param("dassn id", rb_str_new2(rb_id2name(node->nd_vid) ? rb_id2name(node->nd_vid) : "*"));

	if (!poped) {
	    ADD_INSN(ret, nd_line(node), dup);
	}

	idx = get_dyna_var_idx(iseq, node->nd_vid, &lv, &ls);

	if (idx < 0) {
	    rb_bug("NODE_DASGN(_CURR): unknown id (%s)", rb_id2name(node->nd_vid));
	}

	ADD_INSN2(ret, nd_line(node), setdynamic,
		  INT2FIX(ls - idx), INT2FIX(lv));
	break;
      }
      case NODE_GASGN:{
	COMPILE(ret, "lvalue", node->nd_value);

	if (!poped) {
	    ADD_INSN(ret, nd_line(node), dup);
	}
	ADD_INSN1(ret, nd_line(node), setglobal,
		  ((VALUE)node->nd_entry | 1));
	break;
      }
      case NODE_IASGN:
      case NODE_IASGN2:{
	COMPILE(ret, "lvalue", node->nd_value);
	if (!poped) {
	    ADD_INSN(ret, nd_line(node), dup);
	}
	ADD_INSN2(ret, nd_line(node), setinstancevariable,
		  ID2SYM(node->nd_vid), INT2FIX(iseq->ic_size++));
	break;
      }
      case NODE_CDECL:{
	COMPILE(ret, "lvalue", node->nd_value);

	if (!poped) {
	    ADD_INSN(ret, nd_line(node), dup);
	}

	if (node->nd_vid) {
	    ADD_INSN1(ret, nd_line(node), putspecialobject,
		      INT2FIX(VM_SPECIAL_OBJECT_CONST_BASE));
	    ADD_INSN1(ret, nd_line(node), setconstant, ID2SYM(node->nd_vid));
	}
	else {
	    compile_cpath(ret, iseq, node->nd_else);
	    ADD_INSN1(ret, nd_line(node), setconstant, ID2SYM(node->nd_else->nd_mid));
	}
	break;
      }
      case NODE_CVASGN:{
	COMPILE(ret, "cvasgn val", node->nd_value);
	if (!poped) {
	    ADD_INSN(ret, nd_line(node), dup);
	}
	ADD_INSN1(ret, nd_line(node), setclassvariable,
		  ID2SYM(node->nd_vid));
	break;
      }
      case NODE_OP_ASGN1: {
	DECL_ANCHOR(args);
	VALUE argc;
	VALUE flag = 0;
	ID id = node->nd_mid;
	int boff = 0;

	/*
	 * a[x] (op)= y
	 *
	 * nil       # nil
	 * eval a    # nil a
	 * eval x    # nil a x
	 * dupn 2    # nil a x a x
	 * send :[]  # nil a x a[x]
	 * eval y    # nil a x a[x] y
	 * send op   # nil a x ret
	 * setn 3    # ret a x ret
	 * send []=  # ret ?
	 * pop       # ret
	 */

	/*
	 * nd_recv[nd_args->nd_body] (nd_mid)= nd_args->nd_head;
	 * NODE_OP_ASGN nd_recv
	 *              nd_args->nd_head
	 *              nd_args->nd_body
	 *              nd_mid
	 */

	if (!poped) {
	    ADD_INSN(ret, nd_line(node), putnil);
	}
	COMPILE(ret, "NODE_OP_ASGN1 recv", node->nd_recv);
	switch (nd_type(node->nd_args->nd_head)) {
	  case NODE_ZARRAY:
	    argc = INT2FIX(0);
	    break;
	  case NODE_BLOCK_PASS:
	    boff = 1;
	  default:
	    INIT_ANCHOR(args);
	    argc = setup_args(iseq, args, node->nd_args->nd_head, &flag);
	    ADD_SEQ(ret, args);
	}
	ADD_INSN1(ret, nd_line(node), dupn, FIXNUM_INC(argc, 1 + boff));
	ADD_SEND_R(ret, nd_line(node), ID2SYM(idAREF), argc, Qfalse, LONG2FIX(flag));

	if (id == 0 || id == 1) {
	    /* 0: or, 1: and
	       a[x] ||= y

	       unless/if a[x]
	       a[x]= y
	       else
	       nil
	       end
	    */
	    LABEL *label = NEW_LABEL(nd_line(node));
	    LABEL *lfin = NEW_LABEL(nd_line(node));

	    ADD_INSN(ret, nd_line(node), dup);
	    if (id == 0) {
		/* or */
		ADD_INSNL(ret, nd_line(node), branchif, label);
	    }
	    else {
		/* and */
		ADD_INSNL(ret, nd_line(node), branchunless, label);
	    }
	    ADD_INSN(ret, nd_line(node), pop);

	    COMPILE(ret, "NODE_OP_ASGN1 args->body: ", node->nd_args->nd_body);
	    if (!poped) {
		ADD_INSN1(ret, nd_line(node), setn, FIXNUM_INC(argc, 2+boff));
	    }
	    if (flag & VM_CALL_ARGS_SPLAT_BIT) {
		ADD_INSN1(ret, nd_line(node), newarray, INT2FIX(1));
		if (boff > 0) {
		    ADD_INSN1(ret, nd_line(node), dupn, INT2FIX(3));
		    ADD_INSN(ret, nd_line(node), swap);
		    ADD_INSN(ret, nd_line(node), pop);
		}
		ADD_INSN(ret, nd_line(node), concatarray);
		if (boff > 0) {
		    ADD_INSN1(ret, nd_line(node), setn, INT2FIX(3));
		    ADD_INSN(ret, nd_line(node), pop);
		    ADD_INSN(ret, nd_line(node), pop);
		}
		ADD_SEND_R(ret, nd_line(node), ID2SYM(idASET),
			   argc, Qfalse, LONG2FIX(flag));
	    }
	    else {
		if (boff > 0)
		    ADD_INSN(ret, nd_line(node), swap);
		ADD_SEND_R(ret, nd_line(node), ID2SYM(idASET),
			   FIXNUM_INC(argc, 1), Qfalse, LONG2FIX(flag));
	    }
	    ADD_INSN(ret, nd_line(node), pop);
	    ADD_INSNL(ret, nd_line(node), jump, lfin);
	    ADD_LABEL(ret, label);
	    if (!poped) {
		ADD_INSN1(ret, nd_line(node), setn, FIXNUM_INC(argc, 2+boff));
	    }
	    ADD_INSN1(ret, nd_line(node), adjuststack, FIXNUM_INC(argc, 2+boff));
	    ADD_LABEL(ret, lfin);
	}
	else {
	    COMPILE(ret, "NODE_OP_ASGN1 args->body: ", node->nd_args->nd_body);
	    ADD_SEND(ret, nd_line(node), ID2SYM(id), INT2FIX(1));
	    if (!poped) {
		ADD_INSN1(ret, nd_line(node), setn, FIXNUM_INC(argc, 2+boff));
	    }
	    if (flag & VM_CALL_ARGS_SPLAT_BIT) {
		ADD_INSN1(ret, nd_line(node), newarray, INT2FIX(1));
		if (boff > 0) {
		    ADD_INSN1(ret, nd_line(node), dupn, INT2FIX(3));
		    ADD_INSN(ret, nd_line(node), swap);
		    ADD_INSN(ret, nd_line(node), pop);
		}
		ADD_INSN(ret, nd_line(node), concatarray);
		if (boff > 0) {
		    ADD_INSN1(ret, nd_line(node), setn, INT2FIX(3));
		    ADD_INSN(ret, nd_line(node), pop);
		    ADD_INSN(ret, nd_line(node), pop);
		}
		ADD_SEND_R(ret, nd_line(node), ID2SYM(idASET),
			   argc, Qfalse, LONG2FIX(flag));
	    }
	    else {
		if (boff > 0)
		    ADD_INSN(ret, nd_line(node), swap);
		ADD_SEND_R(ret, nd_line(node), ID2SYM(idASET),
			   FIXNUM_INC(argc, 1), Qfalse, LONG2FIX(flag));
	    }
	    ADD_INSN(ret, nd_line(node), pop);
	}

	break;
      }
      case NODE_OP_ASGN2:{
	ID atype = node->nd_next->nd_mid;
	LABEL *lfin = NEW_LABEL(nd_line(node));
	LABEL *lcfin = NEW_LABEL(nd_line(node));
	/*
	  class C; attr_accessor :c; end
	  r = C.new
	  r.a &&= v # asgn2

	  eval r    # r
	  dup       # r r
	  eval r.a  # r o

	  # or
	  dup       # r o o
	  if lcfin  # r o
	  pop       # r
	  eval v    # r v
	  swap      # v r
	  topn 1    # v r v
	  send a=   # v ?
	  jump lfin # v ?

	  lcfin:      # r o
	  swap      # o r

	  lfin:       # o ?
	  pop       # o

	  # and
	  dup       # r o o
	  unless lcfin
	  pop       # r
	  eval v    # r v
	  swap      # v r
	  topn 1    # v r v
	  send a=   # v ?
	  jump lfin # v ?

	  # others
	  eval v    # r o v
	  send ??   # r w
	  send a=   # w

	*/

	COMPILE(ret, "NODE_OP_ASGN2#recv", node->nd_recv);
	ADD_INSN(ret, nd_line(node), dup);
	ADD_SEND(ret, nd_line(node), ID2SYM(node->nd_next->nd_vid),
		 INT2FIX(0));

	if (atype == 0 || atype == 1) {	/* 0: OR or 1: AND */
	    ADD_INSN(ret, nd_line(node), dup);
	    if (atype == 0) {
		ADD_INSNL(ret, nd_line(node), branchif, lcfin);
	    }
	    else {
		ADD_INSNL(ret, nd_line(node), branchunless, lcfin);
	    }
	    ADD_INSN(ret, nd_line(node), pop);
	    COMPILE(ret, "NODE_OP_ASGN2 val", node->nd_value);
	    ADD_INSN(ret, nd_line(node), swap);
	    ADD_INSN1(ret, nd_line(node), topn, INT2FIX(1));
	    ADD_SEND(ret, nd_line(node), ID2SYM(node->nd_next->nd_aid),
		     INT2FIX(1));
	    ADD_INSNL(ret, nd_line(node), jump, lfin);

	    ADD_LABEL(ret, lcfin);
	    ADD_INSN(ret, nd_line(node), swap);

	    ADD_LABEL(ret, lfin);
	    ADD_INSN(ret, nd_line(node), pop);
	    if (poped) {
		/* we can apply more optimize */
		ADD_INSN(ret, nd_line(node), pop);
	    }
	}
	else {
	    COMPILE(ret, "NODE_OP_ASGN2 val", node->nd_value);
	    ADD_SEND(ret, nd_line(node), ID2SYM(node->nd_next->nd_mid),
		     INT2FIX(1));
	    if (!poped) {
		ADD_INSN(ret, nd_line(node), swap);
		ADD_INSN1(ret, nd_line(node), topn, INT2FIX(1));
	    }
	    ADD_SEND(ret, nd_line(node), ID2SYM(node->nd_next->nd_aid),
		     INT2FIX(1));
	    ADD_INSN(ret, nd_line(node), pop);
	}
	break;
      }
      case NODE_OP_ASGN_AND:
      case NODE_OP_ASGN_OR:{
	LABEL *lfin = NEW_LABEL(nd_line(node));
	LABEL *lassign;

	if (nd_type(node) == NODE_OP_ASGN_OR) {
	    LABEL *lfinish[2];
	    lfinish[0] = lfin;
	    lfinish[1] = 0;
	    defined_expr(iseq, ret, node->nd_head, lfinish, Qfalse);
	    lassign = lfinish[1];
	    if (!lassign) {
		lassign = NEW_LABEL(nd_line(node));
	    }
	    ADD_INSNL(ret, nd_line(node), branchunless, lassign);
	}
	else {
	    lassign = NEW_LABEL(nd_line(node));
	}

	COMPILE(ret, "NODE_OP_ASGN_AND/OR#nd_head", node->nd_head);
	ADD_INSN(ret, nd_line(node), dup);

	if (nd_type(node) == NODE_OP_ASGN_AND) {
	    ADD_INSNL(ret, nd_line(node), branchunless, lfin);
	}
	else {
	    ADD_INSNL(ret, nd_line(node), branchif, lfin);
	}

	ADD_INSN(ret, nd_line(node), pop);
	ADD_LABEL(ret, lassign);
	COMPILE(ret, "NODE_OP_ASGN_AND/OR#nd_value", node->nd_value);
	ADD_LABEL(ret, lfin);

	if (poped) {
	    /* we can apply more optimize */
	    ADD_INSN(ret, nd_line(node), pop);
	}
	break;
      }
      case NODE_CALL:
      case NODE_FCALL:
      case NODE_VCALL:{		/* VCALL: variable or call */
	/*
	  call:  obj.method(...)
	  fcall: func(...)
	  vcall: func
	*/
	DECL_ANCHOR(recv);
	DECL_ANCHOR(args);
	ID mid = node->nd_mid;
	VALUE argc;
	VALUE flag = 0;
	VALUE parent_block = iseq->compile_data->current_block;
	iseq->compile_data->current_block = Qfalse;

	INIT_ANCHOR(recv);
	INIT_ANCHOR(args);
#if SUPPORT_JOKE
	if (nd_type(node) == NODE_VCALL) {
	    if (mid == idBitblt) {
		ADD_INSN(ret, nd_line(node), bitblt);
		break;
	    }
	    else if (mid == idAnswer) {
		ADD_INSN(ret, nd_line(node), answer);
		break;
	    }
	}
	/* only joke */
	{
	    ID goto_id;
	    ID label_id;

	    CONST_ID(goto_id, "__goto__");
	    CONST_ID(label_id, "__label__");

	    if (nd_type(node) == NODE_FCALL &&
		(mid == goto_id || mid == label_id)) {
		LABEL *label;
		st_data_t data;
		st_table *labels_table = iseq->compile_data->labels_table;
		ID label_name;

		if (!labels_table) {
		    labels_table = st_init_numtable();
		    iseq->compile_data->labels_table = labels_table;
		}
		if (nd_type(node->nd_args->nd_head) == NODE_LIT &&
		    SYMBOL_P(node->nd_args->nd_head->nd_lit)) {

		    label_name = SYM2ID(node->nd_args->nd_head->nd_lit);
		    if (!st_lookup(labels_table, (st_data_t)label_name, &data)) {
			label = NEW_LABEL(nd_line(node));
			label->position = nd_line(node);
			st_insert(labels_table, (st_data_t)label_name, (st_data_t)label);
		    }
		    else {
			label = (LABEL *)data;
		    }
		}
		else {
		    COMPILE_ERROR((ERROR_ARGS "invalid goto/label format"));
		}


		if (mid == goto_id) {
		    ADD_INSNL(ret, nd_line(node), jump, label);
		}
		else {
		    ADD_LABEL(ret, label);
		}
		break;
	    }
	}
#endif
	/* receiver */
	if (type == NODE_CALL) {
	    COMPILE(recv, "recv", node->nd_recv);
	}
	else if (type == NODE_FCALL || type == NODE_VCALL) {
	    ADD_CALL_RECEIVER(recv, nd_line(node));
	}

	/* args */
	if (nd_type(node) != NODE_VCALL) {
	    argc = setup_args(iseq, args, node->nd_args, &flag);
	}
	else {
	    argc = INT2FIX(0);
	}

	ADD_SEQ(ret, recv);
	ADD_SEQ(ret, args);

	debugp_param("call args argc", argc);
	debugp_param("call method", ID2SYM(mid));

	switch (nd_type(node)) {
	  case NODE_VCALL:
	    flag |= VM_CALL_VCALL_BIT;
	    /* VCALL is funcall, so fall through */
	  case NODE_FCALL:
	    flag |= VM_CALL_FCALL_BIT;
	}

	ADD_SEND_R(ret, nd_line(node), ID2SYM(mid),
		   argc, parent_block, LONG2FIX(flag));

	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}
	break;
      }
      case NODE_SUPER:
      case NODE_ZSUPER:{
	DECL_ANCHOR(args);
	VALUE argc;
	VALUE flag = 0;
	VALUE parent_block = iseq->compile_data->current_block;

	INIT_ANCHOR(args);
	iseq->compile_data->current_block = Qfalse;
	if (nd_type(node) == NODE_SUPER) {
	    argc = setup_args(iseq, args, node->nd_args, &flag);
	}
	else {
	    /* NODE_ZSUPER */
	    int i;
	    rb_iseq_t *liseq = iseq->local_iseq;

	    argc = INT2FIX(liseq->argc);

	    /* normal arguments */
	    for (i = 0; i < liseq->argc; i++) {
		int idx = liseq->local_size - i;
		ADD_INSN1(args, nd_line(node), getlocal, INT2FIX(idx));
	    }

	    if (!liseq->arg_simple) {
		if (liseq->arg_opts) {
		    /* optional arguments */
		    int j;
		    for (j = 0; j < liseq->arg_opts - 1; j++) {
			int idx = liseq->local_size - (i + j);
			ADD_INSN1(args, nd_line(node), getlocal, INT2FIX(idx));
		    }
		    i += j;
		    argc = INT2FIX(i);
		}

		if (liseq->arg_rest != -1) {
		    /* rest argument */
		    int idx = liseq->local_size - liseq->arg_rest;
		    ADD_INSN1(args, nd_line(node), getlocal, INT2FIX(idx));
		    argc = INT2FIX(liseq->arg_rest + 1);
		    flag |= VM_CALL_ARGS_SPLAT_BIT;
		}

		if (liseq->arg_post_len) {
		    /* post arguments */
		    int post_len = liseq->arg_post_len;
		    int post_start = liseq->arg_post_start;

		    if (liseq->arg_rest != -1) {
			int j;
			for (j=0; j<post_len; j++) {
			    int idx = liseq->local_size - (post_start + j);
			    ADD_INSN1(args, nd_line(node), getlocal, INT2FIX(idx));
			}
			ADD_INSN1(args, nd_line(node), newarray, INT2FIX(j));
			ADD_INSN (args, nd_line(node), concatarray);
			/* argc is setteled at above */
		    }
		    else {
			int j;
			for (j=0; j<post_len; j++) {
			    int idx = liseq->local_size - (post_start + j);
			    ADD_INSN1(args, nd_line(node), getlocal, INT2FIX(idx));
			}
			argc = INT2FIX(post_len + post_start);
		    }
		}
	    }
	}

	/* dummy receiver */
	ADD_INSN1(ret, nd_line(node), putobject,
		  nd_type(node) == NODE_ZSUPER ? Qfalse : Qtrue);
	ADD_SEQ(ret, args);
	ADD_INSN3(ret, nd_line(node), invokesuper,
		  argc, parent_block, LONG2FIX(flag));

	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}
	break;
      }
      case NODE_ARRAY:{
	compile_array_(iseq, ret, node, Qtrue, poped);
	break;
      }
      case NODE_ZARRAY:{
	if (!poped) {
	    ADD_INSN1(ret, nd_line(node), newarray, INT2FIX(0));
	}
	break;
      }
      case NODE_VALUES:{
	NODE *n = node;
	while (n) {
	    COMPILE(ret, "values item", n->nd_head);
	    n = n->nd_next;
	}
	ADD_INSN1(ret, nd_line(node), newarray, INT2FIX(node->nd_alen));
	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}
	break;
      }
      case NODE_HASH:{
	DECL_ANCHOR(list);
	VALUE size = 0;
	int type = node->nd_head ? nd_type(node->nd_head) : NODE_ZARRAY;

	INIT_ANCHOR(list);
	switch (type) {
	  case NODE_ARRAY:{
	    compile_array(iseq, list, node->nd_head, Qfalse);
	    size = OPERAND_AT(POP_ELEMENT(list), 0);
	    ADD_SEQ(ret, list);
	    break;
	  }
	  case NODE_ZARRAY:
	    size = INT2FIX(0);
	    break;

	  default:
	    rb_bug("can't make hash with this node: %s", ruby_node_name(type));
	}

	ADD_INSN1(ret, nd_line(node), newhash, size);

	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}
	break;
      }
      case NODE_RETURN:{
	rb_iseq_t *is = iseq;

	if (is) {
	    if (is->type == ISEQ_TYPE_TOP) {
		COMPILE_ERROR((ERROR_ARGS "Invalid return"));
	    }
	    else {
		LABEL *splabel = 0;

		if (is->type == ISEQ_TYPE_METHOD) {
		    splabel = NEW_LABEL(0);
		    ADD_LABEL(ret, splabel);
		    ADD_ADJUST(ret, nd_line(node), 0);
		}

		COMPILE(ret, "return nd_stts (return val)", node->nd_stts);

		if (is->type == ISEQ_TYPE_METHOD) {
		    add_ensure_iseq(ret, iseq, 1);
		    ADD_TRACE(ret, nd_line(node), RUBY_EVENT_RETURN);
		    ADD_INSN(ret, nd_line(node), leave);
		    ADD_ADJUST_RESTORE(ret, splabel);

		    if (!poped) {
			ADD_INSN(ret, nd_line(node), putnil);
		    }
		}
		else {
		    ADD_INSN1(ret, nd_line(node), throw, INT2FIX(0x01) /* TAG_RETURN */ );
		    if (poped) {
			ADD_INSN(ret, nd_line(node), pop);
		    }
		}
	    }
	}
	break;
      }
      case NODE_YIELD:{
	DECL_ANCHOR(args);
	VALUE argc;
	VALUE flag = 0;

	INIT_ANCHOR(args);
	if (iseq->type == ISEQ_TYPE_TOP) {
	    COMPILE_ERROR((ERROR_ARGS "Invalid yield"));
	}

	if (node->nd_head) {
	    argc = setup_args(iseq, args, node->nd_head, &flag);
	}
	else {
	    argc = INT2FIX(0);
	}

	ADD_SEQ(ret, args);
	ADD_INSN2(ret, nd_line(node), invokeblock, argc, LONG2FIX(flag));

	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}
	break;
      }
      case NODE_LVAR:{
	if (!poped) {
	    ID id = node->nd_vid;
	    int idx = iseq->local_iseq->local_size - get_local_var_idx(iseq, id);

	    debugs("id: %s idx: %d\n", rb_id2name(id), idx);
	    ADD_INSN1(ret, nd_line(node), getlocal, INT2FIX(idx));
	}
	break;
      }
      case NODE_DVAR:{
	int lv, idx, ls;
	debugi("nd_vid", node->nd_vid);
	if (!poped) {
	    idx = get_dyna_var_idx(iseq, node->nd_vid, &lv, &ls);
	    if (idx < 0) {
		rb_bug("unknown dvar (%s)", rb_id2name(node->nd_vid));
	    }
	    ADD_INSN2(ret, nd_line(node), getdynamic, INT2FIX(ls - idx), INT2FIX(lv));
	}
	break;
      }
      case NODE_GVAR:{
	ADD_INSN1(ret, nd_line(node), getglobal,
		  ((VALUE)node->nd_entry | 1));
	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}
	break;
      }
      case NODE_IVAR:{
	debugi("nd_vid", node->nd_vid);
	if (!poped) {
	    ADD_INSN2(ret, nd_line(node), getinstancevariable,
		      ID2SYM(node->nd_vid), INT2FIX(iseq->ic_size++));
	}
	break;
      }
      case NODE_CONST:{
	debugi("nd_vid", node->nd_vid);

	if (iseq->compile_data->option->inline_const_cache) {
	    LABEL *lend = NEW_LABEL(nd_line(node));
	    int ic_index = iseq->ic_size++;

	    ADD_INSN2(ret, nd_line(node), getinlinecache, lend, INT2FIX(ic_index));
	    ADD_INSN1(ret, nd_line(node), getconstant, ID2SYM(node->nd_vid));
	    ADD_INSN1(ret, nd_line(node), setinlinecache, INT2FIX(ic_index));
	    ADD_LABEL(ret, lend);
	}
	else {
	    ADD_INSN(ret, nd_line(node), putnil);
	    ADD_INSN1(ret, nd_line(node), getconstant, ID2SYM(node->nd_vid));
	}

	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}
	break;
      }
      case NODE_CVAR:{
	if (!poped) {
	    ADD_INSN1(ret, nd_line(node), getclassvariable,
		      ID2SYM(node->nd_vid));
	}
	break;
      }
      case NODE_NTH_REF:{
        if (!poped) {
	    ADD_INSN2(ret, nd_line(node), getspecial, INT2FIX(1) /* '~'  */,
		      INT2FIX(node->nd_nth << 1));
	}
	break;
      }
      case NODE_BACK_REF:{
	if (!poped) {
	    ADD_INSN2(ret, nd_line(node), getspecial, INT2FIX(1) /* '~' */,
		      INT2FIX(0x01 | (node->nd_nth << 1)));
	}
	break;
      }
      case NODE_MATCH:
      case NODE_MATCH2:
      case NODE_MATCH3:{
	DECL_ANCHOR(recv);
	DECL_ANCHOR(val);

	INIT_ANCHOR(recv);
	INIT_ANCHOR(val);
	switch(nd_type(node)) {
	  case NODE_MATCH:
	    ADD_INSN1(recv, nd_line(node), putobject, node->nd_lit);
	    ADD_INSN2(val, nd_line(node), getspecial, INT2FIX(0),
		      INT2FIX(0));
	    break;
	  case NODE_MATCH2:
	    COMPILE(recv, "receiver", node->nd_recv);
	    COMPILE(val, "value", node->nd_value);
	    break;
	  case NODE_MATCH3:
	    COMPILE(recv, "receiver", node->nd_value);
	    COMPILE(val, "value", node->nd_recv);
	    break;
	}

	if (iseq->compile_data->option->specialized_instruction) {
	    /* TODO: detect by node */
	    if (recv->last == recv->anchor.next &&
		INSN_OF(recv->last) == BIN(putobject) &&
		nd_type(node) == NODE_MATCH2) {
		ADD_SEQ(ret, val);
		ADD_INSN1(ret, nd_line(node), opt_regexpmatch1,
			  OPERAND_AT(recv->last, 0));
	    }
	    else {
		ADD_SEQ(ret, recv);
		ADD_SEQ(ret, val);
		ADD_INSN(ret, nd_line(node), opt_regexpmatch2);
	    }
	}
	else {
	    ADD_SEQ(ret, recv);
	    ADD_SEQ(ret, val);
	    ADD_SEND(ret, nd_line(node), ID2SYM(idEqTilde), INT2FIX(1));
	}

	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}
	break;
      }
      case NODE_LIT:{
	debugp_param("lit", node->nd_lit);
	if (!poped) {
	    ADD_INSN1(ret, nd_line(node), putobject, node->nd_lit);
	}
	break;
      }
      case NODE_STR:{
	debugp_param("nd_lit", node->nd_lit);
	if (!poped) {
	    OBJ_FREEZE(node->nd_lit);
	    ADD_INSN1(ret, nd_line(node), putstring, node->nd_lit);
	}
	break;
      }
      case NODE_DSTR:{
	compile_dstr(iseq, ret, node);

	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}
	break;
      }
      case NODE_XSTR:{
	OBJ_FREEZE(node->nd_lit);
	ADD_CALL_RECEIVER(ret, nd_line(node));
	ADD_INSN1(ret, nd_line(node), putobject, node->nd_lit);
	ADD_CALL(ret, nd_line(node), ID2SYM(idBackquote), INT2FIX(1));

	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}
	break;
      }
      case NODE_DXSTR:{
	ADD_CALL_RECEIVER(ret, nd_line(node));
	compile_dstr(iseq, ret, node);
	ADD_CALL(ret, nd_line(node), ID2SYM(idBackquote), INT2FIX(1));

	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}
	break;
      }
      case NODE_EVSTR:{
	COMPILE(ret, "nd_body", node->nd_body);

	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}
	else {
	    ADD_INSN(ret, nd_line(node), tostring);
	}
	break;
      }
      case NODE_DREGX:{
	compile_dregx(iseq, ret, node);

	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}
	break;
      }
      case NODE_DREGX_ONCE:{
	/* TODO: once? */
	LABEL *lend = NEW_LABEL(nd_line(node));
	int ic_index = iseq->ic_size++;

	ADD_INSN2(ret, nd_line(node), onceinlinecache, lend, INT2FIX(ic_index));
	ADD_INSN(ret, nd_line(node), pop);

	compile_dregx(iseq, ret, node);

	ADD_INSN1(ret, nd_line(node), setinlinecache, INT2FIX(ic_index));
	ADD_LABEL(ret, lend);

	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}
	break;
      }
      case NODE_ARGSCAT:{
	if (poped) {
	    COMPILE(ret, "argscat head", node->nd_head);
	    ADD_INSN1(ret, nd_line(node), splatarray, Qfalse);
	    ADD_INSN(ret, nd_line(node), pop);
	    COMPILE(ret, "argscat body", node->nd_body);
	    ADD_INSN1(ret, nd_line(node), splatarray, Qfalse);
	    ADD_INSN(ret, nd_line(node), pop);
	}
	else {
	    COMPILE(ret, "argscat head", node->nd_head);
	    COMPILE(ret, "argscat body", node->nd_body);
	    ADD_INSN(ret, nd_line(node), concatarray);
	}
	break;
      }
      case NODE_ARGSPUSH:{
	if (poped) {
	    COMPILE(ret, "arsgpush head", node->nd_head);
	    ADD_INSN1(ret, nd_line(node), splatarray, Qfalse);
	    ADD_INSN(ret, nd_line(node), pop);
	    COMPILE_(ret, "argspush body", node->nd_body, poped);
	}
	else {
	    COMPILE(ret, "arsgpush head", node->nd_head);
	    COMPILE_(ret, "argspush body", node->nd_body, poped);
	    ADD_INSN1(ret, nd_line(node), newarray, INT2FIX(1));
	    ADD_INSN(ret, nd_line(node), concatarray);
	}
	break;
      }
      case NODE_SPLAT:{
	COMPILE(ret, "splat", node->nd_head);
	ADD_INSN1(ret, nd_line(node), splatarray, Qtrue);

	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}
	break;
      }
      case NODE_DEFN:{
	VALUE iseqval = NEW_ISEQVAL(node->nd_defn,
				    rb_str_dup(rb_id2str(node->nd_mid)),
				    ISEQ_TYPE_METHOD, nd_line(node));

	debugp_param("defn/iseq", iseqval);

	ADD_INSN1(ret, nd_line(node), putspecialobject, INT2FIX(VM_SPECIAL_OBJECT_VMCORE));
	ADD_INSN1(ret, nd_line(node), putspecialobject, INT2FIX(VM_SPECIAL_OBJECT_CBASE));
	ADD_INSN1(ret, nd_line(node), putobject, ID2SYM(node->nd_mid));
	ADD_INSN1(ret, nd_line(node), putiseq, iseqval);
	ADD_SEND (ret, nd_line(node), ID2SYM(id_core_define_method), INT2FIX(3));

	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}

	debugp_param("defn", iseqval);
	break;
      }
      case NODE_DEFS:{
	VALUE iseqval = NEW_ISEQVAL(node->nd_defn,
				    rb_str_dup(rb_id2str(node->nd_mid)),
				    ISEQ_TYPE_METHOD, nd_line(node));

	debugp_param("defs/iseq", iseqval);

	ADD_INSN1(ret, nd_line(node), putspecialobject, INT2FIX(VM_SPECIAL_OBJECT_VMCORE));
	COMPILE(ret, "defs: recv", node->nd_recv);
	ADD_INSN1(ret, nd_line(node), putobject, ID2SYM(node->nd_mid));
	ADD_INSN1(ret, nd_line(node), putiseq, iseqval);
	ADD_SEND (ret, nd_line(node), ID2SYM(id_core_define_singleton_method), INT2FIX(3));

	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}
	break;
      }
      case NODE_ALIAS:{
	ADD_INSN1(ret, nd_line(node), putspecialobject, INT2FIX(VM_SPECIAL_OBJECT_VMCORE));
	ADD_INSN1(ret, nd_line(node), putspecialobject, INT2FIX(VM_SPECIAL_OBJECT_CBASE));
	COMPILE(ret, "alias arg1", node->u1.node);
	COMPILE(ret, "alias arg2", node->u2.node);
	ADD_SEND(ret, nd_line(node), ID2SYM(id_core_set_method_alias), INT2FIX(3));

	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}
	break;
      }
      case NODE_VALIAS:{
	ADD_INSN1(ret, nd_line(node), putspecialobject, INT2FIX(VM_SPECIAL_OBJECT_VMCORE));
	ADD_INSN1(ret, nd_line(node), putobject, ID2SYM(node->u1.id));
	ADD_INSN1(ret, nd_line(node), putobject, ID2SYM(node->u2.id));
	ADD_SEND(ret, nd_line(node), ID2SYM(id_core_set_variable_alias), INT2FIX(2));

	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}
	break;
      }
      case NODE_UNDEF:{
	ADD_INSN1(ret, nd_line(node), putspecialobject, INT2FIX(VM_SPECIAL_OBJECT_VMCORE));
	ADD_INSN1(ret, nd_line(node), putspecialobject, INT2FIX(VM_SPECIAL_OBJECT_CBASE));
	COMPILE(ret, "undef arg", node->u2.node);
	ADD_SEND(ret, nd_line(node), ID2SYM(id_core_undef_method), INT2FIX(2));

	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}
	break;
      }
      case NODE_CLASS:{
	VALUE iseqval =
	    NEW_CHILD_ISEQVAL(
		node->nd_body,
		rb_sprintf("<class:%s>", rb_id2name(node->nd_cpath->nd_mid)),
		ISEQ_TYPE_CLASS, nd_line(node));
	VALUE noscope = compile_cpath(ret, iseq, node->nd_cpath);
	COMPILE(ret, "super", node->nd_super);
	ADD_INSN3(ret, nd_line(node), defineclass,
		  ID2SYM(node->nd_cpath->nd_mid), iseqval, INT2FIX(noscope ? 3 : 0));

	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}
	break;
      }
      case NODE_MODULE:{
	VALUE iseqval = NEW_CHILD_ISEQVAL(
	    node->nd_body,
	    rb_sprintf("<module:%s>", rb_id2name(node->nd_cpath->nd_mid)),
	    ISEQ_TYPE_CLASS, nd_line(node));

	VALUE noscope = compile_cpath(ret, iseq, node->nd_cpath);
	ADD_INSN (ret, nd_line(node), putnil); /* dummy */
	ADD_INSN3(ret, nd_line(node), defineclass,
		  ID2SYM(node->nd_cpath->nd_mid), iseqval, INT2FIX(noscope ? 5 : 2));
	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}
	break;
      }
      case NODE_SCLASS:{
	ID singletonclass;
	VALUE iseqval =
	    NEW_ISEQVAL(node->nd_body, rb_str_new2("singleton class"),
			ISEQ_TYPE_CLASS, nd_line(node));

	COMPILE(ret, "sclass#recv", node->nd_recv);
	ADD_INSN (ret, nd_line(node), putnil);
	CONST_ID(singletonclass, "singletonclass");
	ADD_INSN3(ret, nd_line(node), defineclass,
		  ID2SYM(singletonclass), iseqval, INT2FIX(1));

	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}
	break;
      }
      case NODE_COLON2:{
	if (rb_is_const_id(node->nd_mid)) {
	    /* constant */
	    LABEL *lend = NEW_LABEL(nd_line(node));
	    int ic_index = iseq->ic_size++;

	    DECL_ANCHOR(pref);
	    DECL_ANCHOR(body);

	    INIT_ANCHOR(pref);
	    INIT_ANCHOR(body);
	    compile_colon2(iseq, node, pref, body);
	    if (LIST_SIZE_ZERO(pref)) {
		if (iseq->compile_data->option->inline_const_cache) {
		    ADD_INSN2(ret, nd_line(node), getinlinecache, lend, INT2FIX(ic_index));
		}
		else {
		    ADD_INSN(ret, nd_line(node), putnil);
		}

		ADD_SEQ(ret, body);

		if (iseq->compile_data->option->inline_const_cache) {
		    ADD_INSN1(ret, nd_line(node), setinlinecache, INT2FIX(ic_index));
		    ADD_LABEL(ret, lend);
		}
	    }
	    else {
		ADD_SEQ(ret, pref);
		ADD_SEQ(ret, body);
	    }
	}
	else {
	    /* function call */
	    ADD_CALL_RECEIVER(ret, nd_line(node));
	    COMPILE(ret, "colon2#nd_head", node->nd_head);
	    ADD_CALL(ret, nd_line(node), ID2SYM(node->nd_mid),
		     INT2FIX(1));
	}
	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}
	break;
      }
      case NODE_COLON3:{
	LABEL *lend = NEW_LABEL(nd_line(node));
	int ic_index = iseq->ic_size++;

	  debugi("colon3#nd_mid", node->nd_mid);

	/* add cache insn */
	if (iseq->compile_data->option->inline_const_cache) {
	    ADD_INSN2(ret, nd_line(node), getinlinecache, lend, INT2FIX(ic_index));
	    ADD_INSN(ret, nd_line(node), pop);
	}

	ADD_INSN1(ret, nd_line(node), putobject, rb_cObject);
	ADD_INSN1(ret, nd_line(node), getconstant, ID2SYM(node->nd_mid));

	if (iseq->compile_data->option->inline_const_cache) {
	    ADD_INSN1(ret, nd_line(node), setinlinecache, INT2FIX(ic_index));
	    ADD_LABEL(ret, lend);
	}

	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}
	break;
      }
      case NODE_DOT2:
      case NODE_DOT3:{
	VALUE flag = type == NODE_DOT2 ? INT2FIX(0) : INT2FIX(1);
	COMPILE(ret, "min", (NODE *) node->nd_beg);
	COMPILE(ret, "max", (NODE *) node->nd_end);
	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	    ADD_INSN(ret, nd_line(node), pop);
	}
	else {
	    ADD_INSN1(ret, nd_line(node), newrange, flag);
	}
	break;
      }
      case NODE_FLIP2:
      case NODE_FLIP3:{
	LABEL *lend = NEW_LABEL(nd_line(node));
	LABEL *lfin = NEW_LABEL(nd_line(node));
	LABEL *ltrue = NEW_LABEL(nd_line(node));
	VALUE key = rb_sprintf("flipflag/%s-%p-%d",
			       RSTRING_PTR(iseq->name), (void *)iseq,
			       iseq->compile_data->flip_cnt++);

	hide_obj(key);
	iseq_add_mark_object_compile_time(iseq, key);
	ADD_INSN2(ret, nd_line(node), getspecial, key, INT2FIX(0));
	ADD_INSNL(ret, nd_line(node), branchif, lend);

	/* *flip == 0 */
	COMPILE(ret, "flip2 beg", node->nd_beg);
	ADD_INSN(ret, nd_line(node), dup);
	ADD_INSNL(ret, nd_line(node), branchunless, lfin);
	if (nd_type(node) == NODE_FLIP3) {
	    ADD_INSN(ret, nd_line(node), dup);
	    ADD_INSN1(ret, nd_line(node), setspecial, key);
	    ADD_INSNL(ret, nd_line(node), jump, lfin);
	}
	else {
	    ADD_INSN1(ret, nd_line(node), setspecial, key);
	}

	/* *flip == 1 */
	ADD_LABEL(ret, lend);
	COMPILE(ret, "flip2 end", node->nd_end);
	ADD_INSNL(ret, nd_line(node), branchunless, ltrue);
	ADD_INSN1(ret, nd_line(node), putobject, Qfalse);
	ADD_INSN1(ret, nd_line(node), setspecial, key);

	ADD_LABEL(ret, ltrue);
	ADD_INSN1(ret, nd_line(node), putobject, Qtrue);

	ADD_LABEL(ret, lfin);
	break;
      }
      case NODE_SELF:{
	if (!poped) {
	    ADD_INSN(ret, nd_line(node), putself);
	}
	break;
      }
      case NODE_NIL:{
	if (!poped) {
	    ADD_INSN(ret, nd_line(node), putnil);
	}
	break;
      }
      case NODE_TRUE:{
	if (!poped) {
	    ADD_INSN1(ret, nd_line(node), putobject, Qtrue);
	}
	break;
      }
      case NODE_FALSE:{
	if (!poped) {
	    ADD_INSN1(ret, nd_line(node), putobject, Qfalse);
	}
	break;
      }
      case NODE_ERRINFO:{
	if (!poped) {
	    if (iseq->type == ISEQ_TYPE_RESCUE) {
		ADD_INSN2(ret, nd_line(node), getdynamic, INT2FIX(2), INT2FIX(0));
	    }
	    else {
		rb_iseq_t *ip = iseq;
		int level = 0;
		while (ip) {
		    if (ip->type == ISEQ_TYPE_RESCUE) {
			break;
		    }
		    ip = ip->parent_iseq;
		    level++;
		}
		if (ip) {
		    ADD_INSN2(ret, nd_line(node), getdynamic, INT2FIX(2), INT2FIX(level));
		}
		else {
		    ADD_INSN(ret, nd_line(node), putnil);
		}
	    }
	}
	break;
      }
      case NODE_DEFINED:{
	if (!poped) {
	    LABEL *lfinish[2];
	    lfinish[0] = NEW_LABEL(nd_line(node));
	    lfinish[1] = 0;
	    ADD_INSN(ret, nd_line(node), putnil);
	    defined_expr(iseq, ret, node->nd_head, lfinish, Qtrue);
	    ADD_INSN(ret, nd_line(node), swap);
	    ADD_INSN(ret, nd_line(node), pop);
	    if (lfinish[1]) {
		ADD_LABEL(ret, lfinish[1]);
	    }
	    ADD_LABEL(ret, lfinish[0]);
	}
	break;
      }
      case NODE_POSTEXE:{
	LABEL *lend = NEW_LABEL(nd_line(node));
	VALUE block = NEW_CHILD_ISEQVAL(node->nd_body, make_name_for_block(iseq), ISEQ_TYPE_BLOCK, nd_line(node));
	int ic_index = iseq->ic_size++;

	ADD_INSN2(ret, nd_line(node), onceinlinecache, lend, INT2FIX(ic_index));
	ADD_INSN(ret, nd_line(node), pop);

	ADD_INSN1(ret, nd_line(node), putspecialobject, INT2FIX(VM_SPECIAL_OBJECT_VMCORE));
	ADD_INSN1(ret, nd_line(node), putiseq, block);
	ADD_SEND (ret, nd_line(node), ID2SYM(id_core_set_postexe), INT2FIX(1));

	ADD_INSN1(ret, nd_line(node), setinlinecache, INT2FIX(ic_index));
	ADD_LABEL(ret, lend);

	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}
	break;
      }
      case NODE_KW_ARG:{
	LABEL *default_label = NEW_LABEL(nd_line(node));
	LABEL *end_label = NEW_LABEL(nd_line(node));
	int idx, lv, ls;
	ID id = node->nd_body->nd_vid;

	ADD_INSN(ret, nd_line(node), dup);
	ADD_INSN1(ret, nd_line(node), putobject, ID2SYM(id));
	ADD_SEND(ret, nd_line(node), ID2SYM(rb_intern("key?")), INT2FIX(1));
	ADD_INSNL(ret, nd_line(node), branchunless, default_label);
	ADD_INSN(ret, nd_line(node), dup);
	ADD_INSN1(ret, nd_line(node), putobject, ID2SYM(id));
	ADD_SEND(ret, nd_line(node), ID2SYM(rb_intern("delete")), INT2FIX(1));
	switch (nd_type(node->nd_body)) {
	  case NODE_LASGN:
	    idx = iseq->local_iseq->local_size - get_local_var_idx(iseq, id);
	    ADD_INSN1(ret, nd_line(node), setlocal, INT2FIX(idx));
	    break;
	  case NODE_DASGN:
	  case NODE_DASGN_CURR:
	    idx = get_dyna_var_idx(iseq, id, &lv, &ls);
	    ADD_INSN2(ret, nd_line(node), setdynamic, INT2FIX(ls - idx), INT2FIX(lv));
	    break;
	  default:
	    rb_bug("iseq_compile_each (NODE_KW_ARG): unknown node: %s", ruby_node_name(nd_type(node->nd_body)));
	}
	ADD_INSNL(ret, nd_line(node), jump, end_label);
	ADD_LABEL(ret, default_label);
	COMPILE_POPED(ret, "keyword default argument", node->nd_body);
	ADD_LABEL(ret, end_label);
	break;
      }
      case NODE_DSYM:{
	compile_dstr(iseq, ret, node);
	if (!poped) {
	    ADD_SEND(ret, nd_line(node), ID2SYM(idIntern), INT2FIX(0));
	}
	else {
	    ADD_INSN(ret, nd_line(node), pop);
	}
	break;
      }
      case NODE_ATTRASGN:{
	DECL_ANCHOR(recv);
	DECL_ANCHOR(args);
	VALUE flag = 0;
	VALUE argc;

	INIT_ANCHOR(recv);
	INIT_ANCHOR(args);
	argc = setup_args(iseq, args, node->nd_args, &flag);

	if (node->nd_recv == (NODE *) 1) {
	    flag |= VM_CALL_FCALL_BIT;
	    ADD_INSN(recv, nd_line(node), putself);
	}
	else {
	    COMPILE(recv, "recv", node->nd_recv);
	}

	debugp_param("argc", argc);
	debugp_param("nd_mid", ID2SYM(node->nd_mid));

	if (!poped) {
	    ADD_INSN(ret, nd_line(node), putnil);
	    ADD_SEQ(ret, recv);
	    ADD_SEQ(ret, args);

	    if (flag & VM_CALL_ARGS_BLOCKARG_BIT) {
		ADD_INSN1(ret, nd_line(node), topn, INT2FIX(1));
		if (flag & VM_CALL_ARGS_SPLAT_BIT) {
		    ADD_INSN1(ret, nd_line(node), putobject, INT2FIX(-1));
		    ADD_SEND(ret, nd_line(node), ID2SYM(idAREF), INT2FIX(1));
		}
		ADD_INSN1(ret, nd_line(node), setn, FIXNUM_INC(argc, 3));
		ADD_INSN (ret, nd_line(node), pop);
	    }
	    else if (flag & VM_CALL_ARGS_SPLAT_BIT) {
		ADD_INSN(ret, nd_line(node), dup);
		ADD_INSN1(ret, nd_line(node), putobject, INT2FIX(-1));
		ADD_SEND(ret, nd_line(node), ID2SYM(idAREF), INT2FIX(1));
		ADD_INSN1(ret, nd_line(node), setn, FIXNUM_INC(argc, 2));
		ADD_INSN (ret, nd_line(node), pop);
	    }
	    else {
		ADD_INSN1(ret, nd_line(node), setn, FIXNUM_INC(argc, 1));
	    }
	}
	else {
	    ADD_SEQ(ret, recv);
	    ADD_SEQ(ret, args);
	}
	ADD_SEND_R(ret, nd_line(node), ID2SYM(node->nd_mid), argc, 0, LONG2FIX(flag));
	ADD_INSN(ret, nd_line(node), pop);

	break;
      }
      case NODE_OPTBLOCK:{
	/* for optimize */
	LABEL *redo_label = NEW_LABEL(0);
	LABEL *next_label = NEW_LABEL(0);

	iseq->compile_data->start_label = next_label;
	iseq->compile_data->redo_label = redo_label;

	ADD_LABEL(ret, redo_label);
	COMPILE_(ret, "optblock body", node->nd_head, 1 /* pop */ );
	ADD_LABEL(ret, next_label);
	ADD_INSN(ret, 0, opt_checkenv);
	break;
      }
      case NODE_PRELUDE:{
	COMPILE_POPED(ret, "prelude", node->nd_head);
	COMPILE_(ret, "body", node->nd_body, poped);
	break;
      }
      case NODE_LAMBDA:{
	/* compile same as lambda{...} */
	VALUE block = NEW_CHILD_ISEQVAL(node->nd_body, make_name_for_block(iseq), ISEQ_TYPE_BLOCK, nd_line(node));
	VALUE argc = INT2FIX(0);
	ADD_INSN1(ret, nd_line(node), putspecialobject, INT2FIX(VM_SPECIAL_OBJECT_VMCORE));
	ADD_CALL_WITH_BLOCK(ret, nd_line(node), ID2SYM(idLambda), argc, block);

	if (poped) {
	    ADD_INSN(ret, nd_line(node), pop);
	}
	break;
      }
      default:
	rb_bug("iseq_compile_each: unknown node: %s", ruby_node_name(type));
	return COMPILE_NG;
    }

    debug_node_end();
    return COMPILE_OK;
}

/***************************/
/* instruction information */
/***************************/

static int
insn_data_length(INSN *iobj)
{
    return insn_len(iobj->insn_id);
}

static int
calc_sp_depth(int depth, INSN *insn)
{
    return insn_stack_increase(depth, insn->insn_id, insn->operands);
}

static int
insn_data_line_no(INSN *iobj)
{
    return insn_len(iobj->line_no);
}

static VALUE
insn_data_to_s_detail(INSN *iobj)
{
    VALUE str = rb_sprintf("%-16s", insn_name(iobj->insn_id));

    if (iobj->operands) {
	const char *types = insn_op_types(iobj->insn_id);
	int j;

	for (j = 0; types[j]; j++) {
	    char type = types[j];
	    printf("str: %"PRIxVALUE", type: %c\n", str, type);

	    switch (type) {
	      case TS_OFFSET:	/* label(destination position) */
		{
		    LABEL *lobj = (LABEL *)OPERAND_AT(iobj, j);
		    rb_str_catf(str, "<L%03d>", lobj->label_no);
		    break;
		}
		break;
	      case TS_ISEQ:	/* iseq */
		{
		    rb_iseq_t *iseq = (rb_iseq_t *)OPERAND_AT(iobj, j);
		    VALUE val = Qnil;
		    if (iseq) {
			val = iseq->self;
		    }
		    rb_str_concat(str, rb_inspect(val));
		}
		break;
	      case TS_LINDEX:
	      case TS_DINDEX:
	      case TS_NUM:	/* ulong */
	      case TS_VALUE:	/* VALUE */
		rb_str_concat(str, rb_inspect(OPERAND_AT(iobj, j)));
		break;
	      case TS_ID:	/* ID */
		rb_str_concat(str, rb_inspect(OPERAND_AT(iobj, j)));
		break;
	      case TS_GENTRY:
		{
		    struct rb_global_entry *entry = (struct rb_global_entry *)
		      (OPERAND_AT(iobj, j) & (~1));
		    rb_str_cat2(str, rb_id2name(entry->id));
		}
	      case TS_IC:	/* method cache */
		rb_str_catf(str, "<ic:%d>", FIX2INT(OPERAND_AT(iobj, j)));
		break;
	      case TS_CDHASH:	/* case/when condition cache */
		rb_str_cat2(str, "<ch>");
		break;
	      default:{
		rb_raise(rb_eSyntaxError, "unknown operand type: %c", type);
	      }
	    }
	    if (types[j + 1]) {
		rb_str_cat2(str, ", ");
	    }
	}
    }
    return str;
}

static void
dump_disasm_list(struct iseq_link_element *link)
{
    int pos = 0;
    INSN *iobj;
    LABEL *lobj;
    VALUE str;

    printf("-- raw disasm--------\n");

    while (link) {
	switch (link->type) {
	  case ISEQ_ELEMENT_INSN:
	    {
		iobj = (INSN *)link;
		str = insn_data_to_s_detail(iobj);
		printf("%04d %-65s(%4d)\n", pos, StringValueCStr(str),
		       insn_data_line_no(iobj));
		pos += insn_data_length(iobj);
		break;
	    }
	  case ISEQ_ELEMENT_LABEL:
	    {
		lobj = (LABEL *)link;
		printf("<L%03d>\n", lobj->label_no);
		break;
	    }
	  case ISEQ_ELEMENT_NONE:
	    {
		printf("[none]\n");
		break;
	    }
	  case ISEQ_ELEMENT_ADJUST:
	    {
		ADJUST *adjust = (ADJUST *)link;
		printf("adjust: [label: %d]\n", adjust->label->label_no);
		break;
	    }
	  default:
	    /* ignore */
	    rb_raise(rb_eSyntaxError, "dump_disasm_list error: %ld\n", FIX2LONG(link->type));
	}
	link = link->next;
    }
    printf("---------------------\n");
}

VALUE
rb_insns_name_array(void)
{
    VALUE ary = rb_ary_new();
    int i;
    for (i = 0; i < numberof(insn_name_info); i++) {
	rb_ary_push(ary, rb_obj_freeze(rb_str_new2(insn_name_info[i])));
    }
    return rb_obj_freeze(ary);
}

static LABEL *
register_label(rb_iseq_t *iseq, struct st_table *labels_table, VALUE obj)
{
    LABEL *label = 0;
    st_data_t tmp;
    obj = rb_convert_type(obj, T_SYMBOL, "Symbol", "to_sym");

    if (st_lookup(labels_table, obj, &tmp) == 0) {
	label = NEW_LABEL(0);
	st_insert(labels_table, obj, (st_data_t)label);
    }
    else {
	label = (LABEL *)tmp;
    }
    return label;
}

static VALUE
get_exception_sym2type(VALUE sym)
{
#undef rb_intern
#define rb_intern(str) rb_intern_const(str)
    VALUE sym_inspect;
    static VALUE symRescue, symEnsure, symRetry;
    static VALUE symBreak, symRedo, symNext;

    if (symRescue == 0) {
	symRescue = ID2SYM(rb_intern("rescue"));
	symEnsure = ID2SYM(rb_intern("ensure"));
	symRetry  = ID2SYM(rb_intern("retry"));
	symBreak  = ID2SYM(rb_intern("break"));
	symRedo   = ID2SYM(rb_intern("redo"));
	symNext   = ID2SYM(rb_intern("next"));
    }

    if (sym == symRescue) return CATCH_TYPE_RESCUE;
    if (sym == symEnsure) return CATCH_TYPE_ENSURE;
    if (sym == symRetry)  return CATCH_TYPE_RETRY;
    if (sym == symBreak)  return CATCH_TYPE_BREAK;
    if (sym == symRedo)   return CATCH_TYPE_REDO;
    if (sym == symNext)   return CATCH_TYPE_NEXT;
    sym_inspect = rb_inspect(sym);
    rb_raise(rb_eSyntaxError, "invalid exception symbol: %s",
	     StringValuePtr(sym_inspect));
    return 0;
}

static int
iseq_build_from_ary_exception(rb_iseq_t *iseq, struct st_table *labels_table,
		     VALUE exception)
{
    int i;

    for (i=0; i<RARRAY_LEN(exception); i++) {
	VALUE v, type, *ptr, eiseqval;
	LABEL *lstart, *lend, *lcont;
	int sp;

	RB_GC_GUARD(v) = rb_convert_type(RARRAY_PTR(exception)[i], T_ARRAY,
					 "Array", "to_ary");
	if (RARRAY_LEN(v) != 6) {
	    rb_raise(rb_eSyntaxError, "wrong exception entry");
	}
	ptr  = RARRAY_PTR(v);
	type = get_exception_sym2type(ptr[0]);
	if (ptr[1] == Qnil) {
	    eiseqval = 0;
	}
	else {
	    eiseqval = rb_iseq_load(ptr[1], iseq->self, Qnil);
	}

	lstart = register_label(iseq, labels_table, ptr[2]);
	lend   = register_label(iseq, labels_table, ptr[3]);
	lcont  = register_label(iseq, labels_table, ptr[4]);
	sp     = NUM2INT(ptr[5]);

	(void)sp;

	ADD_CATCH_ENTRY(type, lstart, lend, eiseqval, lcont);
    }
    return COMPILE_OK;
}

static struct st_table *
insn_make_insn_table(void)
{
    struct st_table *table;
    int i;
    table = st_init_numtable();

    for (i=0; i<VM_INSTRUCTION_SIZE; i++) {
	st_insert(table, ID2SYM(rb_intern(insn_name(i))), i);
    }

    return table;
}

static int
iseq_build_from_ary_body(rb_iseq_t *iseq, LINK_ANCHOR *anchor,
		VALUE body, struct st_table *labels_table)
{
    /* TODO: body should be frozen */
    VALUE *ptr = RARRAY_PTR(body);
    long i, len = RARRAY_LEN(body);
    int j;
    int line_no = 0;

    /*
     * index -> LABEL *label
     */
    static struct st_table *insn_table;

    if (insn_table == 0) {
	insn_table = insn_make_insn_table();
    }

    for (i=0; i<len; i++) {
	VALUE obj = ptr[i];

	if (SYMBOL_P(obj)) {
	    LABEL *label = register_label(iseq, labels_table, obj);
	    ADD_LABEL(anchor, label);
	}
	else if (FIXNUM_P(obj)) {
	    line_no = NUM2INT(obj);
	}
	else if (RB_TYPE_P(obj, T_ARRAY)) {
	    VALUE *argv = 0;
	    int argc = RARRAY_LENINT(obj) - 1;
	    st_data_t insn_id;
	    VALUE insn;

	    insn = (argc < 0) ? Qnil : RARRAY_PTR(obj)[0];
	    if (st_lookup(insn_table, (st_data_t)insn, &insn_id) == 0) {
		/* TODO: exception */
		RB_GC_GUARD(insn) = rb_inspect(insn);
		rb_compile_error(RSTRING_PTR(iseq->filename), line_no,
				 "unknown instruction: %s", RSTRING_PTR(insn));
	    }

	    if (argc != insn_len((VALUE)insn_id)-1) {
		rb_compile_error(RSTRING_PTR(iseq->filename), line_no,
				 "operand size mismatch");
	    }

	    if (argc > 0) {
		argv = compile_data_alloc(iseq, sizeof(VALUE) * argc);
		for (j=0; j<argc; j++) {
		    VALUE op = rb_ary_entry(obj, j+1);
		    switch (insn_op_type((VALUE)insn_id, j)) {
		      case TS_OFFSET: {
			LABEL *label = register_label(iseq, labels_table, op);
			argv[j] = (VALUE)label;
			break;
		      }
		      case TS_LINDEX:
		      case TS_DINDEX:
		      case TS_NUM:
			(void)NUM2INT(op);
			argv[j] = op;
			break;
		      case TS_VALUE:
			argv[j] = op;
			iseq_add_mark_object(iseq, op);
			break;
		      case TS_ISEQ:
			{
			    if (op != Qnil) {
				if (RB_TYPE_P(op, T_ARRAY)) {
				    argv[j] = rb_iseq_load(op, iseq->self, Qnil);
				}
				else if (CLASS_OF(op) == rb_cISeq) {
				    argv[j] = op;
				}
				else {
				    rb_raise(rb_eSyntaxError, "ISEQ is required");
				}
				iseq_add_mark_object(iseq, argv[j]);
			    }
			    else {
				argv[j] = 0;
			    }
			}
			break;
		      case TS_GENTRY:
			op = rb_convert_type(op, T_SYMBOL, "Symbol", "to_sym");
			argv[j] = (VALUE)rb_global_entry(SYM2ID(op));
			break;
		      case TS_IC:
			argv[j] = op;
			if (NUM2INT(op) >= iseq->ic_size)
			    iseq->ic_size = NUM2INT(op) + 1;
			break;
		      case TS_ID:
			argv[j] = rb_convert_type(op, T_SYMBOL,
						  "Symbol", "to_sym");
			break;
		      case TS_CDHASH:
			{
			    int i;
			    op = rb_convert_type(op, T_ARRAY, "Array", "to_ary");
			    op = rb_ary_dup(op);
			    for (i=0; i<RARRAY_LEN(op); i+=2) {
				VALUE sym = rb_ary_entry(op, i+1);
				LABEL *label =
				  register_label(iseq, labels_table, sym);
				rb_ary_store(op, i+1, (VALUE)label | 1);
			    }
			    argv[j] = op;
			    iseq_add_mark_object_compile_time(iseq, op);
			}
			break;
		      default:
			rb_raise(rb_eSyntaxError, "unknown operand: %c", insn_op_type((VALUE)insn_id, j));
		    }
		}
	    }
	    ADD_ELEM(anchor,
		     (LINK_ELEMENT*)new_insn_core(iseq, line_no,
						  (enum ruby_vminsn_type)insn_id, argc, argv));
	}
	else {
	    rb_raise(rb_eTypeError, "unexpected object for instruction");
	}
    }
    validate_labels(iseq, labels_table);
    st_free_table(labels_table);
    iseq_setup(iseq, anchor);
    return COMPILE_OK;
}

#define CHECK_ARRAY(v)   rb_convert_type((v), T_ARRAY, "Array", "to_ary")
#define CHECK_STRING(v)  rb_convert_type((v), T_STRING, "String", "to_str")
#define CHECK_SYMBOL(v)  rb_convert_type((v), T_SYMBOL, "Symbol", "to_sym")
static inline VALUE CHECK_INTEGER(VALUE v) {(void)NUM2LONG(v); return v;}

VALUE
rb_iseq_build_from_ary(rb_iseq_t *iseq, VALUE locals, VALUE args,
			 VALUE exception, VALUE body)
{
    int i;
    ID *tbl;
    struct st_table *labels_table = st_init_numtable();
    DECL_ANCHOR(anchor);
    INIT_ANCHOR(anchor);

    iseq->local_table_size = RARRAY_LENINT(locals);
    iseq->local_table = tbl = (ID *)ALLOC_N(ID, iseq->local_table_size);
    iseq->local_size = iseq->local_table_size + 1;

    for (i=0; i<RARRAY_LEN(locals); i++) {
	VALUE lv = RARRAY_PTR(locals)[i];
	tbl[i] = FIXNUM_P(lv) ? (ID)FIX2LONG(lv) : SYM2ID(CHECK_SYMBOL(lv));
    }

    /* args */
    if (FIXNUM_P(args)) {
	iseq->arg_size = iseq->argc = FIX2INT(args);
	iseq->arg_simple = 1;
    }
    else {
	int i = 0;
	VALUE argc = CHECK_INTEGER(rb_ary_entry(args, i++));
	VALUE arg_opt_labels = CHECK_ARRAY(rb_ary_entry(args, i++));
	VALUE arg_post_len = CHECK_INTEGER(rb_ary_entry(args, i++));
	VALUE arg_post_start = CHECK_INTEGER(rb_ary_entry(args, i++));
	VALUE arg_rest = CHECK_INTEGER(rb_ary_entry(args, i++));
	VALUE arg_block = CHECK_INTEGER(rb_ary_entry(args, i++));
	VALUE arg_simple = CHECK_INTEGER(rb_ary_entry(args, i++));

	iseq->argc = FIX2INT(argc);
	iseq->arg_rest = FIX2INT(arg_rest);
	iseq->arg_post_len = FIX2INT(arg_post_len);
	iseq->arg_post_start = FIX2INT(arg_post_start);
	iseq->arg_block = FIX2INT(arg_block);
	iseq->arg_opts = RARRAY_LENINT(arg_opt_labels);
	iseq->arg_opt_table = (VALUE *)ALLOC_N(VALUE, iseq->arg_opts);

	if (iseq->arg_block != -1) {
	    iseq->arg_size = iseq->arg_block + 1;
	}
	else if (iseq->arg_post_len) {
	    iseq->arg_size = iseq->arg_post_start + iseq->arg_post_len;
	}
	else if (iseq->arg_rest != -1) {
	    iseq->arg_size = iseq->arg_rest + 1;
	}
	else {
	    iseq->arg_size = iseq->argc + (iseq->arg_opts ? iseq->arg_opts - 1 : 0);
	}

	for (i=0; i<RARRAY_LEN(arg_opt_labels); i++) {
	    iseq->arg_opt_table[i] =
	      (VALUE)register_label(iseq, labels_table,
				    rb_ary_entry(arg_opt_labels, i));
	}

	iseq->arg_simple = NUM2INT(arg_simple);
    }

    /* exception */
    iseq_build_from_ary_exception(iseq, labels_table, exception);

    /* body */
    iseq_build_from_ary_body(iseq, anchor, body, labels_table);
    return iseq->self;
}

/* for parser */

int
rb_dvar_defined(ID id)
{
    rb_thread_t *th = GET_THREAD();
    rb_iseq_t *iseq;
    if (th->base_block && (iseq = th->base_block->iseq)) {
	while (iseq->type == ISEQ_TYPE_BLOCK ||
	       iseq->type == ISEQ_TYPE_RESCUE ||
	       iseq->type == ISEQ_TYPE_ENSURE ||
	       iseq->type == ISEQ_TYPE_EVAL ||
	       iseq->type == ISEQ_TYPE_MAIN
	       ) {
	    int i;

	    for (i = 0; i < iseq->local_table_size; i++) {
		if (iseq->local_table[i] == id) {
		    return 1;
		}
	    }
	    iseq = iseq->parent_iseq;
	}
    }
    return 0;
}

int
rb_local_defined(ID id)
{
    rb_thread_t *th = GET_THREAD();
    rb_iseq_t *iseq;

    if (th->base_block && th->base_block->iseq) {
	int i;
	iseq = th->base_block->iseq->local_iseq;

	for (i=0; i<iseq->local_table_size; i++) {
	    if (iseq->local_table[i] == id) {
		return 1;
	    }
	}
    }
    return 0;
}

int
rb_parse_in_eval(void)
{
    return GET_THREAD()->parse_in_eval > 0;
}

int
rb_parse_in_main(void)
{
    return GET_THREAD()->parse_in_eval < 0;
}
