/**********************************************************************

  iseq.c -

  $Author$
  created at: 2006-07-11(Tue) 09:00:03 +0900

  Copyright (C) 2006 Koichi Sasada

**********************************************************************/

#include "ruby/ruby.h"
#include "internal.h"

/* #define RUBY_MARK_FREE_DEBUG 1 */
#include "gc.h"
#include "vm_core.h"
#include "iseq.h"

#include "insns.inc"
#include "insns_info.inc"

#define ISEQ_MAJOR_VERSION 1
#define ISEQ_MINOR_VERSION 2

VALUE rb_cISeq;

#define hidden_obj_p(obj) (!SPECIAL_CONST_P(obj) && !RBASIC(obj)->klass)

static inline VALUE
obj_resurrect(VALUE obj)
{
    if (hidden_obj_p(obj)) {
	switch (BUILTIN_TYPE(obj)) {
	  case T_STRING:
	    obj = rb_str_resurrect(obj);
	    break;
	  case T_ARRAY:
	    obj = rb_ary_resurrect(obj);
	    break;
	}
    }
    return obj;
}

static void
compile_data_free(struct iseq_compile_data *compile_data)
{
    if (compile_data) {
	struct iseq_compile_data_storage *cur, *next;
	cur = compile_data->storage_head;
	while (cur) {
	    next = cur->next;
	    ruby_xfree(cur);
	    cur = next;
	}
	ruby_xfree(compile_data);
    }
}

static void
iseq_free(void *ptr)
{
    rb_iseq_t *iseq;
    RUBY_FREE_ENTER("iseq");

    if (ptr) {
	iseq = ptr;
	if (!iseq->orig) {
	    /* It's possible that strings are freed */
	    if (0) {
		RUBY_GC_INFO("%s @ %s\n", RSTRING_PTR(iseq->name),
					  RSTRING_PTR(iseq->filename));
	    }

	    if (iseq->iseq != iseq->iseq_encoded) {
		RUBY_FREE_UNLESS_NULL(iseq->iseq_encoded);
	    }

	    RUBY_FREE_UNLESS_NULL(iseq->iseq);
	    RUBY_FREE_UNLESS_NULL(iseq->line_info_table);
	    RUBY_FREE_UNLESS_NULL(iseq->local_table);
	    RUBY_FREE_UNLESS_NULL(iseq->ic_entries);
	    RUBY_FREE_UNLESS_NULL(iseq->catch_table);
	    RUBY_FREE_UNLESS_NULL(iseq->arg_opt_table);
	    RUBY_FREE_UNLESS_NULL(iseq->arg_keyword_table);
	    compile_data_free(iseq->compile_data);
	}
	ruby_xfree(ptr);
    }
    RUBY_FREE_LEAVE("iseq");
}

static void
iseq_mark(void *ptr)
{
    RUBY_MARK_ENTER("iseq");

    if (ptr) {
	rb_iseq_t *iseq = ptr;

	RUBY_GC_INFO("%s @ %s\n", RSTRING_PTR(iseq->name), RSTRING_PTR(iseq->filename));
	RUBY_MARK_UNLESS_NULL(iseq->mark_ary);
	RUBY_MARK_UNLESS_NULL(iseq->name);
	RUBY_MARK_UNLESS_NULL(iseq->filename);
	RUBY_MARK_UNLESS_NULL(iseq->filepath);
	RUBY_MARK_UNLESS_NULL((VALUE)iseq->cref_stack);
	RUBY_MARK_UNLESS_NULL(iseq->klass);
	RUBY_MARK_UNLESS_NULL(iseq->coverage);
#if 0
	RUBY_MARK_UNLESS_NULL((VALUE)iseq->node);
	RUBY_MARK_UNLESS_NULL(iseq->cached_special_block);
#endif
	RUBY_MARK_UNLESS_NULL(iseq->orig);

	if (iseq->compile_data != 0) {
	    struct iseq_compile_data *const compile_data = iseq->compile_data;
	    RUBY_MARK_UNLESS_NULL(compile_data->mark_ary);
	    RUBY_MARK_UNLESS_NULL(compile_data->err_info);
	    RUBY_MARK_UNLESS_NULL(compile_data->catch_table_ary);
	}
    }
    RUBY_MARK_LEAVE("iseq");
}

static size_t
iseq_memsize(const void *ptr)
{
    size_t size = sizeof(rb_iseq_t);
    const rb_iseq_t *iseq;

    if (ptr) {
	iseq = ptr;
	if (!iseq->orig) {
	    if (iseq->iseq != iseq->iseq_encoded) {
		size += iseq->iseq_size * sizeof(VALUE);
	    }

	    size += iseq->iseq_size * sizeof(VALUE);
	    size += iseq->line_info_size * sizeof(struct iseq_line_info_entry);
	    size += iseq->local_table_size * sizeof(ID);
	    size += iseq->catch_table_size * sizeof(struct iseq_catch_table_entry);
	    size += iseq->arg_opts * sizeof(VALUE);
	    size += iseq->ic_size * sizeof(struct iseq_inline_cache_entry);

	    if (iseq->compile_data) {
		struct iseq_compile_data_storage *cur;

		cur = iseq->compile_data->storage_head;
		while (cur) {
		    size += cur->size + sizeof(struct iseq_compile_data_storage);
		    cur = cur->next;
		}
		size += sizeof(struct iseq_compile_data);
	    }
	}
    }

    return size;
}

static const rb_data_type_t iseq_data_type = {
    "iseq",
    {
	iseq_mark,
	iseq_free,
	iseq_memsize,
    },
};

static VALUE
iseq_alloc(VALUE klass)
{
    rb_iseq_t *iseq;
    return TypedData_Make_Struct(klass, rb_iseq_t, &iseq_data_type, iseq);
}

static void
set_relation(rb_iseq_t *iseq, const VALUE parent)
{
    const VALUE type = iseq->type;
    rb_thread_t *th = GET_THREAD();

    /* set class nest stack */
    if (type == ISEQ_TYPE_TOP) {
	/* toplevel is private */
	iseq->cref_stack = NEW_BLOCK(rb_cObject);
	iseq->cref_stack->nd_visi = NOEX_PRIVATE;
	if (th->top_wrapper) {
	    NODE *cref = NEW_BLOCK(th->top_wrapper);
	    cref->nd_visi = NOEX_PRIVATE;
	    cref->nd_next = iseq->cref_stack;
	    iseq->cref_stack = cref;
	}
    }
    else if (type == ISEQ_TYPE_METHOD || type == ISEQ_TYPE_CLASS) {
	iseq->cref_stack = NEW_BLOCK(0); /* place holder */
    }
    else if (RTEST(parent)) {
	rb_iseq_t *piseq;
	GetISeqPtr(parent, piseq);
	iseq->cref_stack = piseq->cref_stack;
    }

    if (type == ISEQ_TYPE_TOP ||
	type == ISEQ_TYPE_METHOD || type == ISEQ_TYPE_CLASS) {
	iseq->local_iseq = iseq;
    }
    else if (RTEST(parent)) {
	rb_iseq_t *piseq;
	GetISeqPtr(parent, piseq);
	iseq->local_iseq = piseq->local_iseq;
    }

    if (RTEST(parent)) {
	rb_iseq_t *piseq;
	GetISeqPtr(parent, piseq);
	iseq->parent_iseq = piseq;
    }
}

static VALUE
prepare_iseq_build(rb_iseq_t *iseq,
		   VALUE name, VALUE filename, VALUE filepath, VALUE line_no,
		   VALUE parent, enum iseq_type type, VALUE block_opt,
		   const rb_compile_option_t *option)
{
    OBJ_FREEZE(name);
    OBJ_FREEZE(filename);

    iseq->name = name;
    iseq->filename = filename;
    iseq->filepath = filepath;
    iseq->line_no = (unsigned short)line_no; /* TODO: really enough? */
    iseq->defined_method_id = 0;
    iseq->mark_ary = rb_ary_tmp_new(3);
    OBJ_UNTRUST(iseq->mark_ary);
    RBASIC(iseq->mark_ary)->klass = 0;

    iseq->type = type;
    iseq->arg_rest = -1;
    iseq->arg_block = -1;
    iseq->arg_keyword = -1;
    iseq->klass = 0;

    /*
     * iseq->special_block_builder = GC_GUARDED_PTR_REF(block_opt);
     * iseq->cached_special_block_builder = 0;
     * iseq->cached_special_block = 0;
     */

    iseq->compile_data = ALLOC(struct iseq_compile_data);
    MEMZERO(iseq->compile_data, struct iseq_compile_data, 1);
    iseq->compile_data->err_info = Qnil;
    iseq->compile_data->mark_ary = rb_ary_tmp_new(3);

    iseq->compile_data->storage_head = iseq->compile_data->storage_current =
      (struct iseq_compile_data_storage *)
	ALLOC_N(char, INITIAL_ISEQ_COMPILE_DATA_STORAGE_BUFF_SIZE +
		sizeof(struct iseq_compile_data_storage));

    iseq->compile_data->catch_table_ary = rb_ary_new();
    iseq->compile_data->storage_head->pos = 0;
    iseq->compile_data->storage_head->next = 0;
    iseq->compile_data->storage_head->size =
      INITIAL_ISEQ_COMPILE_DATA_STORAGE_BUFF_SIZE;
    iseq->compile_data->storage_head->buff =
      (char *)(&iseq->compile_data->storage_head->buff + 1);
    iseq->compile_data->option = option;
    iseq->compile_data->last_coverable_line = -1;

    set_relation(iseq, parent);

    iseq->coverage = Qfalse;
    if (!GET_THREAD()->parse_in_eval) {
	VALUE coverages = rb_get_coverages();
	if (RTEST(coverages)) {
	    iseq->coverage = rb_hash_lookup(coverages, filename);
	    if (NIL_P(iseq->coverage)) iseq->coverage = Qfalse;
	}
    }

    return Qtrue;
}

static VALUE
cleanup_iseq_build(rb_iseq_t *iseq)
{
    struct iseq_compile_data *data = iseq->compile_data;
    VALUE err = data->err_info;
    iseq->compile_data = 0;
    compile_data_free(data);

    if (RTEST(err)) {
	rb_funcall2(err, rb_intern("set_backtrace"), 1, &iseq->filename);
	rb_exc_raise(err);
    }
    return Qtrue;
}

static rb_compile_option_t COMPILE_OPTION_DEFAULT = {
    OPT_INLINE_CONST_CACHE, /* int inline_const_cache; */
    OPT_PEEPHOLE_OPTIMIZATION, /* int peephole_optimization; */
    OPT_TAILCALL_OPTIMIZATION, /* int tailcall_optimization */
    OPT_SPECIALISED_INSTRUCTION, /* int specialized_instruction; */
    OPT_OPERANDS_UNIFICATION, /* int operands_unification; */
    OPT_INSTRUCTIONS_UNIFICATION, /* int instructions_unification; */
    OPT_STACK_CACHING, /* int stack_caching; */
    OPT_TRACE_INSTRUCTION, /* int trace_instruction */
};
static const rb_compile_option_t COMPILE_OPTION_FALSE = {0};

static void
make_compile_option(rb_compile_option_t *option, VALUE opt)
{
    if (opt == Qnil) {
	*option = COMPILE_OPTION_DEFAULT;
    }
    else if (opt == Qfalse) {
	*option = COMPILE_OPTION_FALSE;
    }
    else if (opt == Qtrue) {
	memset(option, 1, sizeof(rb_compile_option_t));
    }
    else if (CLASS_OF(opt) == rb_cHash) {
	*option = COMPILE_OPTION_DEFAULT;

#define SET_COMPILE_OPTION(o, h, mem) \
  { VALUE flag = rb_hash_aref((h), ID2SYM(rb_intern(#mem))); \
      if (flag == Qtrue)  { (o)->mem = 1; } \
      else if (flag == Qfalse)  { (o)->mem = 0; } \
  }
#define SET_COMPILE_OPTION_NUM(o, h, mem) \
  { VALUE num = rb_hash_aref(opt, ID2SYM(rb_intern(#mem))); \
      if (!NIL_P(num)) (o)->mem = NUM2INT(num); \
  }
	SET_COMPILE_OPTION(option, opt, inline_const_cache);
	SET_COMPILE_OPTION(option, opt, peephole_optimization);
	SET_COMPILE_OPTION(option, opt, tailcall_optimization);
	SET_COMPILE_OPTION(option, opt, specialized_instruction);
	SET_COMPILE_OPTION(option, opt, operands_unification);
	SET_COMPILE_OPTION(option, opt, instructions_unification);
	SET_COMPILE_OPTION(option, opt, stack_caching);
	SET_COMPILE_OPTION(option, opt, trace_instruction);
	SET_COMPILE_OPTION_NUM(option, opt, debug_level);
#undef SET_COMPILE_OPTION
#undef SET_COMPILE_OPTION_NUM
    }
    else {
	rb_raise(rb_eTypeError, "Compile option must be Hash/true/false/nil");
    }
}

static VALUE
make_compile_option_value(rb_compile_option_t *option)
{
    VALUE opt = rb_hash_new();
#define SET_COMPILE_OPTION(o, h, mem) \
  rb_hash_aset((h), ID2SYM(rb_intern(#mem)), (o)->mem ? Qtrue : Qfalse)
#define SET_COMPILE_OPTION_NUM(o, h, mem) \
  rb_hash_aset((h), ID2SYM(rb_intern(#mem)), INT2NUM((o)->mem))
    {
	SET_COMPILE_OPTION(option, opt, inline_const_cache);
	SET_COMPILE_OPTION(option, opt, peephole_optimization);
	SET_COMPILE_OPTION(option, opt, tailcall_optimization);
	SET_COMPILE_OPTION(option, opt, specialized_instruction);
	SET_COMPILE_OPTION(option, opt, operands_unification);
	SET_COMPILE_OPTION(option, opt, instructions_unification);
	SET_COMPILE_OPTION(option, opt, stack_caching);
	SET_COMPILE_OPTION_NUM(option, opt, debug_level);
    }
#undef SET_COMPILE_OPTION
#undef SET_COMPILE_OPTION_NUM
    return opt;
}

VALUE
rb_iseq_new(NODE *node, VALUE name, VALUE filename, VALUE filepath,
	    VALUE parent, enum iseq_type type)
{
    return rb_iseq_new_with_opt(node, name, filename, filepath, INT2FIX(0), parent, type,
				&COMPILE_OPTION_DEFAULT);
}

VALUE
rb_iseq_new_top(NODE *node, VALUE name, VALUE filename, VALUE filepath, VALUE parent)
{
    return rb_iseq_new_with_opt(node, name, filename, filepath, INT2FIX(0), parent, ISEQ_TYPE_TOP,
				&COMPILE_OPTION_DEFAULT);
}

VALUE
rb_iseq_new_main(NODE *node, VALUE filename, VALUE filepath)
{
    rb_thread_t *th = GET_THREAD();
    VALUE parent = th->base_block->iseq->self;
    return rb_iseq_new_with_opt(node, rb_str_new2("<main>"), filename, filepath, INT2FIX(0),
				parent, ISEQ_TYPE_MAIN, &COMPILE_OPTION_DEFAULT);
}

static VALUE
rb_iseq_new_with_bopt_and_opt(NODE *node, VALUE name, VALUE filename, VALUE filepath, VALUE line_no,
				VALUE parent, enum iseq_type type, VALUE bopt,
				const rb_compile_option_t *option)
{
    rb_iseq_t *iseq;
    VALUE self = iseq_alloc(rb_cISeq);

    GetISeqPtr(self, iseq);
    iseq->self = self;

    prepare_iseq_build(iseq, name, filename, filepath, line_no, parent, type, bopt, option);
    rb_iseq_compile_node(self, node);
    cleanup_iseq_build(iseq);
    return self;
}

VALUE
rb_iseq_new_with_opt(NODE *node, VALUE name, VALUE filename, VALUE filepath, VALUE line_no,
		     VALUE parent, enum iseq_type type,
		     const rb_compile_option_t *option)
{
    /* TODO: argument check */
    return rb_iseq_new_with_bopt_and_opt(node, name, filename, filepath, line_no, parent, type,
					   Qfalse, option);
}

VALUE
rb_iseq_new_with_bopt(NODE *node, VALUE name, VALUE filename, VALUE filepath, VALUE line_no,
		       VALUE parent, enum iseq_type type, VALUE bopt)
{
    /* TODO: argument check */
    return rb_iseq_new_with_bopt_and_opt(node, name, filename, filepath, line_no, parent, type,
					   bopt, &COMPILE_OPTION_DEFAULT);
}

#define CHECK_ARRAY(v)   rb_convert_type((v), T_ARRAY, "Array", "to_ary")
#define CHECK_STRING(v)  rb_convert_type((v), T_STRING, "String", "to_str")
#define CHECK_SYMBOL(v)  rb_convert_type((v), T_SYMBOL, "Symbol", "to_sym")
static inline VALUE CHECK_INTEGER(VALUE v) {(void)NUM2LONG(v); return v;}
static VALUE
iseq_load(VALUE self, VALUE data, VALUE parent, VALUE opt)
{
    VALUE iseqval = iseq_alloc(self);

    VALUE magic, version1, version2, format_type, misc;
    VALUE name, filename, filepath, line_no;
    VALUE type, body, locals, args, exception;

    st_data_t iseq_type;
    struct st_table *type_map = 0;
    rb_iseq_t *iseq;
    rb_compile_option_t option;
    int i = 0;

    /* [magic, major_version, minor_version, format_type, misc,
     *  name, filename, line_no,
     *  type, locals, args, exception_table, body]
     */

    data        = CHECK_ARRAY(data);

    magic       = CHECK_STRING(rb_ary_entry(data, i++));
    version1    = CHECK_INTEGER(rb_ary_entry(data, i++));
    version2    = CHECK_INTEGER(rb_ary_entry(data, i++));
    format_type = CHECK_INTEGER(rb_ary_entry(data, i++));
    misc        = rb_ary_entry(data, i++); /* TODO */
    ((void)magic, (void)version1, (void)version2, (void)format_type, (void)misc);

    name        = CHECK_STRING(rb_ary_entry(data, i++));
    filename    = CHECK_STRING(rb_ary_entry(data, i++));
    filepath    = rb_ary_entry(data, i++);
    filepath    = NIL_P(filepath) ? Qnil : CHECK_STRING(filepath);
    line_no     = CHECK_INTEGER(rb_ary_entry(data, i++));

    type        = CHECK_SYMBOL(rb_ary_entry(data, i++));
    locals      = CHECK_ARRAY(rb_ary_entry(data, i++));

    args        = rb_ary_entry(data, i++);
    if (FIXNUM_P(args) || (args = CHECK_ARRAY(args))) {
	/* */
    }

    exception   = CHECK_ARRAY(rb_ary_entry(data, i++));
    body        = CHECK_ARRAY(rb_ary_entry(data, i++));

    GetISeqPtr(iseqval, iseq);
    iseq->self = iseqval;

    if (type_map == 0) {
	type_map = st_init_numtable();
	st_insert(type_map, ID2SYM(rb_intern("top")), ISEQ_TYPE_TOP);
	st_insert(type_map, ID2SYM(rb_intern("method")), ISEQ_TYPE_METHOD);
	st_insert(type_map, ID2SYM(rb_intern("block")), ISEQ_TYPE_BLOCK);
	st_insert(type_map, ID2SYM(rb_intern("class")), ISEQ_TYPE_CLASS);
	st_insert(type_map, ID2SYM(rb_intern("rescue")), ISEQ_TYPE_RESCUE);
	st_insert(type_map, ID2SYM(rb_intern("ensure")), ISEQ_TYPE_ENSURE);
	st_insert(type_map, ID2SYM(rb_intern("eval")), ISEQ_TYPE_EVAL);
	st_insert(type_map, ID2SYM(rb_intern("main")), ISEQ_TYPE_MAIN);
	st_insert(type_map, ID2SYM(rb_intern("defined_guard")), ISEQ_TYPE_DEFINED_GUARD);
    }

    if (st_lookup(type_map, type, &iseq_type) == 0) {
	const char *typename = rb_id2name(type);
	if (typename)
	    rb_raise(rb_eTypeError, "unsupport type: :%s", typename);
	else
	    rb_raise(rb_eTypeError, "unsupport type: %p", (void *)type);
    }

    if (parent == Qnil) {
	parent = 0;
    }

    make_compile_option(&option, opt);
    prepare_iseq_build(iseq, name, filename, filepath, line_no,
		       parent, (enum iseq_type)iseq_type, 0, &option);

    rb_iseq_build_from_ary(iseq, locals, args, exception, body);

    cleanup_iseq_build(iseq);
    return iseqval;
}

static VALUE
iseq_s_load(int argc, VALUE *argv, VALUE self)
{
    VALUE data, opt=Qnil;
    rb_scan_args(argc, argv, "11", &data, &opt);

    return iseq_load(self, data, 0, opt);
}

VALUE
rb_iseq_load(VALUE data, VALUE parent, VALUE opt)
{
    return iseq_load(rb_cISeq, data, parent, opt);
}

static NODE *
parse_string(VALUE str, const char *file, int line)
{
    VALUE parser = rb_parser_new();
    NODE *node = rb_parser_compile_string(parser, file, str, line);

    if (!node) {
	rb_exc_raise(GET_THREAD()->errinfo);	/* TODO: check err */
    }
    return node;
}

VALUE
rb_iseq_compile_with_option(VALUE src, VALUE file, VALUE filepath, VALUE line, VALUE opt)
{
    rb_compile_option_t option;
    const char *fn = StringValueCStr(file);
    int ln = NUM2INT(line);
    NODE *node = parse_string(StringValue(src), fn, ln);
    rb_thread_t *th = GET_THREAD();
    make_compile_option(&option, opt);

    if (th->base_block && th->base_block->iseq) {
	return rb_iseq_new_with_opt(node, th->base_block->iseq->name,
				    file, filepath, line, th->base_block->iseq->self,
				    ISEQ_TYPE_EVAL, &option);
    }
    else {
	return rb_iseq_new_with_opt(node, rb_str_new2("<compiled>"), file, filepath, line, Qfalse,
				    ISEQ_TYPE_TOP, &option);
    }
}

VALUE
rb_iseq_compile(VALUE src, VALUE file, VALUE line)
{
    return rb_iseq_compile_with_option(src, file, Qnil, line, Qnil);
}

static VALUE
iseq_s_compile(int argc, VALUE *argv, VALUE self)
{
    VALUE src, file = Qnil, path = Qnil, line = INT2FIX(1), opt = Qnil;

    rb_secure(1);

    rb_scan_args(argc, argv, "14", &src, &file, &path, &line, &opt);
    if (NIL_P(file)) file = rb_str_new2("<compiled>");
    if (NIL_P(line)) line = INT2FIX(1);

    return rb_iseq_compile_with_option(src, file, path, line, opt);
}

static VALUE
iseq_s_compile_file(int argc, VALUE *argv, VALUE self)
{
    VALUE file, line = INT2FIX(1), opt = Qnil;
    VALUE parser;
    VALUE f;
    NODE *node;
    const char *fname;
    rb_compile_option_t option;

    rb_secure(1);
    rb_scan_args(argc, argv, "11", &file, &opt);
    FilePathValue(file);
    fname = StringValueCStr(file);

    f = rb_file_open_str(file, "r");

    parser = rb_parser_new();
    node = rb_parser_compile_file(parser, fname, f, NUM2INT(line));
    make_compile_option(&option, opt);
    return rb_iseq_new_with_opt(node, rb_str_new2("<main>"), file,
				rb_realpath_internal(Qnil, file, 1), line, Qfalse,
				ISEQ_TYPE_TOP, &option);
}

static VALUE
iseq_s_compile_option_set(VALUE self, VALUE opt)
{
    rb_compile_option_t option;
    rb_secure(1);
    make_compile_option(&option, opt);
    COMPILE_OPTION_DEFAULT = option;
    return opt;
}

static VALUE
iseq_s_compile_option_get(VALUE self)
{
    return make_compile_option_value(&COMPILE_OPTION_DEFAULT);
}

static rb_iseq_t *
iseq_check(VALUE val)
{
    rb_iseq_t *iseq;
    GetISeqPtr(val, iseq);
    if (!iseq->name) {
	rb_raise(rb_eTypeError, "uninitialized InstructionSequence");
    }
    return iseq;
}

static VALUE
iseq_eval(VALUE self)
{
    rb_secure(1);
    return rb_iseq_eval(self);
}

static VALUE
iseq_inspect(VALUE self)
{
    rb_iseq_t *iseq;
    GetISeqPtr(self, iseq);
    if (!iseq->name) {
        return rb_sprintf("#<%s: uninitialized>", rb_obj_classname(self));
    }

    return rb_sprintf("<%s:%s@%s>",
                      rb_obj_classname(self),
		      RSTRING_PTR(iseq->name), RSTRING_PTR(iseq->filename));
}

static
VALUE iseq_data_to_ary(rb_iseq_t *iseq);

static VALUE
iseq_to_a(VALUE self)
{
    rb_iseq_t *iseq = iseq_check(self);
    rb_secure(1);
    return iseq_data_to_ary(iseq);
}

int
rb_iseq_first_lineno(const rb_iseq_t *iseq)
{
    return FIX2INT(iseq->line_no);
}

/* TODO: search algorithm is brute force.
         this should be binary search or so. */

static struct iseq_line_info_entry *
get_line_info(const rb_iseq_t *iseq, size_t pos)
{
    size_t i = 0, size = iseq->line_info_size;
    struct iseq_line_info_entry *table = iseq->line_info_table;
    const int debug = 0;

    if (debug) {
	printf("size: %"PRIdSIZE"\n", size);
	printf("table[%"PRIdSIZE"]: position: %d, line: %d, pos: %"PRIdSIZE"\n",
	       i, table[i].position, table[i].line_no, pos);
    }

    if (size == 0) {
	return 0;
    }
    else if (size == 1) {
	return &table[0];
    }
    else {
	for (i=1; i<size; i++) {
	    if (debug) printf("table[%"PRIdSIZE"]: position: %d, line: %d, pos: %"PRIdSIZE"\n",
			      i, table[i].position, table[i].line_no, pos);

	    if (table[i].position == pos) {
		return &table[i];
	    }
	    if (table[i].position > pos) {
		return &table[i-1];
	    }
	}
    }
    return &table[i-1];
}

static unsigned int
find_line_no(const rb_iseq_t *iseq, size_t pos)
{
    struct iseq_line_info_entry *entry = get_line_info(iseq, pos);
    if (entry) {
	return entry->line_no;
    }
    else {
	return 0;
    }
}

unsigned int
rb_iseq_line_no(const rb_iseq_t *iseq, size_t pos)
{
    if (pos == 0) {
	return find_line_no(iseq, pos);
    }
    else {
	return find_line_no(iseq, pos - 1);
    }
}

static VALUE
insn_operand_intern(rb_iseq_t *iseq,
		    VALUE insn, int op_no, VALUE op,
		    int len, size_t pos, VALUE *pnop, VALUE child)
{
    const char *types = insn_op_types(insn);
    char type = types[op_no];
    VALUE ret;

    switch (type) {
      case TS_OFFSET:		/* LONG */
	ret = rb_sprintf("%"PRIdVALUE, (VALUE)(pos + len + op));
	break;

      case TS_NUM:		/* ULONG */
	ret = rb_sprintf("%"PRIuVALUE, op);
	break;

      case TS_LINDEX:
	{
	    rb_iseq_t *liseq = iseq->local_iseq;
	    int lidx = liseq->local_size - (int)op;
	    const char *name = rb_id2name(liseq->local_table[lidx]);

	    if (name) {
		ret = rb_str_new2(name);
	    }
	    else {
		ret = rb_str_new2("*");
	    }
	    break;
	}
      case TS_DINDEX:{
	if (insn == BIN(getdynamic) || insn == BIN(setdynamic)) {
	    rb_iseq_t *diseq = iseq;
	    VALUE level = *pnop, i;
	    const char *name;
	    for (i = 0; i < level; i++) {
		diseq = diseq->parent_iseq;
	    }
	    name = rb_id2name(diseq->local_table[diseq->local_size - op]);

	    if (!name) {
		name = "*";
	    }
	    ret = rb_str_new2(name);
	}
	else {
	    ret = rb_inspect(INT2FIX(op));
	}
	break;
      }
      case TS_ID:		/* ID (symbol) */
	op = ID2SYM(op);

      case TS_VALUE:		/* VALUE */
	op = obj_resurrect(op);
	ret = rb_inspect(op);
	if (CLASS_OF(op) == rb_cISeq) {
	    rb_ary_push(child, op);
	}
	break;

      case TS_ISEQ:		/* iseq */
	{
	    rb_iseq_t *iseq = (rb_iseq_t *)op;
	    if (iseq) {
		ret = iseq->name;
		if (child) {
		    rb_ary_push(child, iseq->self);
		}
	    }
	    else {
		ret = rb_str_new2("nil");
	    }
	    break;
	}
      case TS_GENTRY:
	{
	    struct rb_global_entry *entry = (struct rb_global_entry *)op;
	    ret = rb_str_dup(rb_id2str(entry->id));
	}
	break;

      case TS_IC:
	ret = rb_sprintf("<ic:%"PRIdPTRDIFF">", (struct iseq_inline_cache_entry *)op - iseq->ic_entries);
	break;

      case TS_CDHASH:
	ret = rb_str_new2("<cdhash>");
	break;

      case TS_FUNCPTR:
	ret = rb_str_new2("<funcptr>");
	break;

      default:
	rb_bug("rb_iseq_disasm: unknown operand type: %c", type);
    }
    return ret;
}

/**
 * Disassemble a instruction
 * Iseq -> Iseq inspect object
 */
int
rb_iseq_disasm_insn(VALUE ret, VALUE *iseq, size_t pos,
		    rb_iseq_t *iseqdat, VALUE child)
{
    VALUE insn = iseq[pos];
    int len = insn_len(insn);
    int j;
    const char *types = insn_op_types(insn);
    VALUE str = rb_str_new(0, 0);
    const char *insn_name_buff;

    insn_name_buff = insn_name(insn);
    if (1) {
	rb_str_catf(str, "%04"PRIdSIZE" %-16s ", pos, insn_name_buff);
    }
    else {
	rb_str_catf(str, "%04"PRIdSIZE" %-16.*s ", pos,
		    (int)strcspn(insn_name_buff, "_"), insn_name_buff);
    }

    for (j = 0; types[j]; j++) {
	const char *types = insn_op_types(insn);
	VALUE opstr = insn_operand_intern(iseqdat, insn, j, iseq[pos + j + 1],
					  len, pos, &iseq[pos + j + 2],
					  child);
	rb_str_concat(str, opstr);

	if (types[j + 1]) {
	    rb_str_cat2(str, ", ");
	}
    }

    {
	unsigned int line_no = find_line_no(iseqdat, pos);
	unsigned int prev = pos == 0 ? 0 : find_line_no(iseqdat, pos - 1);
	if (line_no && line_no != prev) {
	    long slen = RSTRING_LEN(str);
	    slen = (slen > 70) ? 0 : (70 - slen);
	    str = rb_str_catf(str, "%*s(%4d)", (int)slen, "", line_no);
	}
    }

    if (ret) {
	rb_str_cat2(str, "\n");
	rb_str_concat(ret, str);
    }
    else {
	printf("%s\n", RSTRING_PTR(str));
    }
    return len;
}

static const char *
catch_type(int type)
{
    switch (type) {
      case CATCH_TYPE_RESCUE:
	return "rescue";
      case CATCH_TYPE_ENSURE:
	return "ensure";
      case CATCH_TYPE_RETRY:
	return "retry";
      case CATCH_TYPE_BREAK:
	return "break";
      case CATCH_TYPE_REDO:
	return "redo";
      case CATCH_TYPE_NEXT:
	return "next";
      default:
	rb_bug("unknown catch type (%d)", type);
	return 0;
    }
}

VALUE
rb_iseq_disasm(VALUE self)
{
    rb_iseq_t *iseqdat = iseq_check(self);
    VALUE *iseq;
    VALUE str = rb_str_new(0, 0);
    VALUE child = rb_ary_new();
    unsigned long size;
    int i;
    long l;
    ID *tbl;
    size_t n;
    enum {header_minlen = 72};

    rb_secure(1);

    iseq = iseqdat->iseq;
    size = iseqdat->iseq_size;

    rb_str_cat2(str, "== disasm: ");

    rb_str_concat(str, iseq_inspect(iseqdat->self));
    if ((l = RSTRING_LEN(str)) < header_minlen) {
	rb_str_resize(str, header_minlen);
	memset(RSTRING_PTR(str) + l, '=', header_minlen - l);
    }
    rb_str_cat2(str, "\n");

    /* show catch table information */
    if (iseqdat->catch_table_size != 0) {
	rb_str_cat2(str, "== catch table\n");
    }
    for (i = 0; i < iseqdat->catch_table_size; i++) {
	struct iseq_catch_table_entry *entry = &iseqdat->catch_table[i];
	rb_str_catf(str,
		    "| catch type: %-6s st: %04d ed: %04d sp: %04d cont: %04d\n",
		    catch_type((int)entry->type), (int)entry->start,
		    (int)entry->end, (int)entry->sp, (int)entry->cont);
	if (entry->iseq) {
	    rb_str_concat(str, rb_iseq_disasm(entry->iseq));
	}
    }
    if (iseqdat->catch_table_size != 0) {
	rb_str_cat2(str, "|-------------------------------------"
		    "-----------------------------------\n");
    }

    /* show local table information */
    tbl = iseqdat->local_table;

    if (tbl) {
	rb_str_catf(str,
		    "local table (size: %d, argc: %d "
		    "[opts: %d, rest: %d, post: %d, block: %d] s%d)\n",
		    iseqdat->local_size, iseqdat->argc,
		    iseqdat->arg_opts, iseqdat->arg_rest,
		    iseqdat->arg_post_len, iseqdat->arg_block,
		    iseqdat->arg_simple);

	for (i = 0; i < iseqdat->local_table_size; i++) {
	    const char *name = rb_id2name(tbl[i]);
	    char info[0x100];
	    char argi[0x100] = "";
	    char opti[0x100] = "";

	    if (iseqdat->arg_opts) {
		int argc = iseqdat->argc;
		int opts = iseqdat->arg_opts;
		if (i >= argc && i < argc + opts - 1) {
		    snprintf(opti, sizeof(opti), "Opt=%"PRIdVALUE,
			     iseqdat->arg_opt_table[i - argc]);
		}
	    }

	    snprintf(argi, sizeof(argi), "%s%s%s%s%s",	/* arg, opts, rest, post  block */
		     iseqdat->argc > i ? "Arg" : "",
		     opti,
		     iseqdat->arg_rest == i ? "Rest" : "",
		     (iseqdat->arg_post_start <= i &&
		      i < iseqdat->arg_post_start + iseqdat->arg_post_len) ? "Post" : "",
		     iseqdat->arg_block == i ? "Block" : "");

	    snprintf(info, sizeof(info), "%s%s%s%s", name ? name : "?",
		     *argi ? "<" : "", argi, *argi ? ">" : "");

	    rb_str_catf(str, "[%2d] %-11s", iseqdat->local_size - i, info);
	}
	rb_str_cat2(str, "\n");
    }

    /* show each line */
    for (n = 0; n < size;) {
	n += rb_iseq_disasm_insn(str, iseq, n, iseqdat, child);
    }

    for (i = 0; i < RARRAY_LEN(child); i++) {
	VALUE isv = rb_ary_entry(child, i);
	rb_str_concat(str, rb_iseq_disasm(isv));
    }

    return str;
}

static VALUE
iseq_s_disasm(VALUE klass, VALUE body)
{
    VALUE ret = Qnil;
    rb_iseq_t *iseq;

    rb_secure(1);

    if (rb_obj_is_proc(body)) {
	rb_proc_t *proc;
	GetProcPtr(body, proc);
	iseq = proc->block.iseq;
	if (RUBY_VM_NORMAL_ISEQ_P(iseq)) {
	    ret = rb_iseq_disasm(iseq->self);
	}
    }
    else if ((iseq = rb_method_get_iseq(body)) != 0) {
	ret = rb_iseq_disasm(iseq->self);
    }

    return ret;
}

const char *
ruby_node_name(int node)
{
    switch (node) {
#include "node_name.inc"
      default:
	rb_bug("unknown node (%d)", node);
	return 0;
    }
}

#define DECL_SYMBOL(name) \
  static VALUE sym_##name

#define INIT_SYMBOL(name) \
  sym_##name = ID2SYM(rb_intern(#name))

static VALUE
register_label(struct st_table *table, unsigned long idx)
{
    VALUE sym;
    char buff[8 + (sizeof(idx) * CHAR_BIT * 32 / 100)];

    snprintf(buff, sizeof(buff), "label_%lu", idx);
    sym = ID2SYM(rb_intern(buff));
    st_insert(table, idx, sym);
    return sym;
}

static VALUE
exception_type2symbol(VALUE type)
{
    ID id;
    switch(type) {
      case CATCH_TYPE_RESCUE: CONST_ID(id, "rescue"); break;
      case CATCH_TYPE_ENSURE: CONST_ID(id, "ensure"); break;
      case CATCH_TYPE_RETRY:  CONST_ID(id, "retry");  break;
      case CATCH_TYPE_BREAK:  CONST_ID(id, "break");  break;
      case CATCH_TYPE_REDO:   CONST_ID(id, "redo");   break;
      case CATCH_TYPE_NEXT:   CONST_ID(id, "next");   break;
      default:
	rb_bug("...");
    }
    return ID2SYM(id);
}

static int
cdhash_each(VALUE key, VALUE value, VALUE ary)
{
    rb_ary_push(ary, obj_resurrect(key));
    rb_ary_push(ary, value);
    return ST_CONTINUE;
}

static VALUE
iseq_data_to_ary(rb_iseq_t *iseq)
{
    long i;
    size_t ti;
    unsigned int pos;
    unsigned int line = 0;
    VALUE *seq;

    VALUE val = rb_ary_new();
    VALUE type; /* Symbol */
    VALUE locals = rb_ary_new();
    VALUE args = rb_ary_new();
    VALUE body = rb_ary_new(); /* [[:insn1, ...], ...] */
    VALUE nbody;
    VALUE exception = rb_ary_new(); /* [[....]] */
    VALUE misc = rb_hash_new();

    static VALUE insn_syms[VM_INSTRUCTION_SIZE];
    struct st_table *labels_table = st_init_numtable();

    DECL_SYMBOL(top);
    DECL_SYMBOL(method);
    DECL_SYMBOL(block);
    DECL_SYMBOL(class);
    DECL_SYMBOL(rescue);
    DECL_SYMBOL(ensure);
    DECL_SYMBOL(eval);
    DECL_SYMBOL(main);
    DECL_SYMBOL(defined_guard);

    if (sym_top == 0) {
	int i;
	for (i=0; i<VM_INSTRUCTION_SIZE; i++) {
	    insn_syms[i] = ID2SYM(rb_intern(insn_name(i)));
	}
	INIT_SYMBOL(top);
	INIT_SYMBOL(method);
	INIT_SYMBOL(block);
	INIT_SYMBOL(class);
	INIT_SYMBOL(rescue);
	INIT_SYMBOL(ensure);
	INIT_SYMBOL(eval);
	INIT_SYMBOL(main);
	INIT_SYMBOL(defined_guard);
    }

    /* type */
    switch(iseq->type) {
      case ISEQ_TYPE_TOP:    type = sym_top;    break;
      case ISEQ_TYPE_METHOD: type = sym_method; break;
      case ISEQ_TYPE_BLOCK:  type = sym_block;  break;
      case ISEQ_TYPE_CLASS:  type = sym_class;  break;
      case ISEQ_TYPE_RESCUE: type = sym_rescue; break;
      case ISEQ_TYPE_ENSURE: type = sym_ensure; break;
      case ISEQ_TYPE_EVAL:   type = sym_eval;   break;
      case ISEQ_TYPE_MAIN:   type = sym_main;   break;
      case ISEQ_TYPE_DEFINED_GUARD: type = sym_defined_guard; break;
      default: rb_bug("unsupported iseq type");
    };

    /* locals */
    for (i=0; i<iseq->local_table_size; i++) {
	ID lid = iseq->local_table[i];
	if (lid) {
	    if (rb_id2str(lid)) rb_ary_push(locals, ID2SYM(lid));
	}
	else {
	    rb_ary_push(locals, ID2SYM(rb_intern("#arg_rest")));
	}
    }

    /* args */
    {
	/*
	 * [argc,                 # argc
	 *  [label1, label2, ...] # opts
	 *  rest index,
	 *  post_len
	 *  post_start
	 *  block index,
	 *  simple,
         * ]
	 */
	VALUE arg_opt_labels = rb_ary_new();
	int j;

	for (j=0; j<iseq->arg_opts; j++) {
	    rb_ary_push(arg_opt_labels,
			register_label(labels_table, iseq->arg_opt_table[j]));
	}

	/* commit */
	if (iseq->arg_simple == 1) {
	    args = INT2FIX(iseq->argc);
	}
	else {
	    rb_ary_push(args, INT2FIX(iseq->argc));
	    rb_ary_push(args, arg_opt_labels);
	    rb_ary_push(args, INT2FIX(iseq->arg_post_len));
	    rb_ary_push(args, INT2FIX(iseq->arg_post_start));
	    rb_ary_push(args, INT2FIX(iseq->arg_rest));
	    rb_ary_push(args, INT2FIX(iseq->arg_block));
	    rb_ary_push(args, INT2FIX(iseq->arg_simple));
	}
    }

    /* body */
    for (seq = iseq->iseq; seq < iseq->iseq + iseq->iseq_size; ) {
	VALUE insn = *seq++;
	int j, len = insn_len(insn);
	VALUE *nseq = seq + len - 1;
	VALUE ary = rb_ary_new2(len);

	rb_ary_push(ary, insn_syms[insn]);
	for (j=0; j<len-1; j++, seq++) {
	    switch (insn_op_type(insn, j)) {
	      case TS_OFFSET: {
		unsigned long idx = nseq - iseq->iseq + *seq;
		rb_ary_push(ary, register_label(labels_table, idx));
		break;
	      }
	      case TS_LINDEX:
	      case TS_DINDEX:
	      case TS_NUM:
		rb_ary_push(ary, INT2FIX(*seq));
		break;
	      case TS_VALUE:
		rb_ary_push(ary, obj_resurrect(*seq));
		break;
	      case TS_ISEQ:
		{
		    rb_iseq_t *iseq = (rb_iseq_t *)*seq;
		    if (iseq) {
			VALUE val = iseq_data_to_ary(iseq);
			rb_ary_push(ary, val);
		    }
		    else {
			rb_ary_push(ary, Qnil);
		    }
		}
		break;
	      case TS_GENTRY:
		{
		    struct rb_global_entry *entry = (struct rb_global_entry *)*seq;
		    rb_ary_push(ary, ID2SYM(entry->id));
		}
		break;
	      case TS_IC: {
		  struct iseq_inline_cache_entry *ic = (struct iseq_inline_cache_entry *)*seq;
		    rb_ary_push(ary, INT2FIX(ic - iseq->ic_entries));
	        }
		break;
	      case TS_ID:
		rb_ary_push(ary, ID2SYM(*seq));
		break;
	      case TS_CDHASH:
		{
		    VALUE hash = *seq;
		    VALUE val = rb_ary_new();
		    int i;

		    rb_hash_foreach(hash, cdhash_each, val);

		    for (i=0; i<RARRAY_LEN(val); i+=2) {
			VALUE pos = FIX2INT(rb_ary_entry(val, i+1));
			unsigned long idx = nseq - iseq->iseq + pos;

			rb_ary_store(val, i+1,
				     register_label(labels_table, idx));
		    }
		    rb_ary_push(ary, val);
		}
		break;
	      default:
		rb_bug("unknown operand: %c", insn_op_type(insn, j));
	    }
	}
	rb_ary_push(body, ary);
    }

    nbody = body;

    /* exception */
    for (i=0; i<iseq->catch_table_size; i++) {
	VALUE ary = rb_ary_new();
	struct iseq_catch_table_entry *entry = &iseq->catch_table[i];
	rb_ary_push(ary, exception_type2symbol(entry->type));
	if (entry->iseq) {
	    rb_iseq_t *eiseq;
	    GetISeqPtr(entry->iseq, eiseq);
	    rb_ary_push(ary, iseq_data_to_ary(eiseq));
	}
	else {
	    rb_ary_push(ary, Qnil);
	}
	rb_ary_push(ary, register_label(labels_table, entry->start));
	rb_ary_push(ary, register_label(labels_table, entry->end));
	rb_ary_push(ary, register_label(labels_table, entry->cont));
	rb_ary_push(ary, INT2FIX(entry->sp));
	rb_ary_push(exception, ary);
    }

    /* make body with labels and insert line number */
    body = rb_ary_new();
    ti = 0;

    for (i=0, pos=0; i<RARRAY_LEN(nbody); i++) {
	VALUE ary = RARRAY_PTR(nbody)[i];
	st_data_t label;

	if (st_lookup(labels_table, pos, &label)) {
	    rb_ary_push(body, (VALUE)label);
	}

	if (iseq->line_info_size < ti && iseq->line_info_table[ti].position == pos) {
	    line = iseq->line_info_table[ti].line_no;
	    rb_ary_push(body, INT2FIX(line));
	    ti++;
	}

	rb_ary_push(body, ary);
	pos += RARRAY_LENINT(ary); /* reject too huge data */
    }

    st_free_table(labels_table);

    rb_hash_aset(misc, ID2SYM(rb_intern("arg_size")), INT2FIX(iseq->arg_size));
    rb_hash_aset(misc, ID2SYM(rb_intern("local_size")), INT2FIX(iseq->local_size));
    rb_hash_aset(misc, ID2SYM(rb_intern("stack_max")), INT2FIX(iseq->stack_max));

    /*
     * [:magic, :major_version, :minor_version, :format_type, :misc,
     *  :name, :filename, :filepath, :line_no, :type, :locals, :args,
     *  :catch_table, :bytecode]
     */
    rb_ary_push(val, rb_str_new2("YARVInstructionSequence/SimpleDataFormat"));
    rb_ary_push(val, INT2FIX(ISEQ_MAJOR_VERSION)); /* major */
    rb_ary_push(val, INT2FIX(ISEQ_MINOR_VERSION)); /* minor */
    rb_ary_push(val, INT2FIX(1));
    rb_ary_push(val, misc);
    rb_ary_push(val, iseq->name);
    rb_ary_push(val, iseq->filename);
    rb_ary_push(val, iseq->filepath);
    rb_ary_push(val, iseq->line_no);
    rb_ary_push(val, type);
    rb_ary_push(val, locals);
    rb_ary_push(val, args);
    rb_ary_push(val, exception);
    rb_ary_push(val, body);
    return val;
}

VALUE
rb_iseq_clone(VALUE iseqval, VALUE newcbase)
{
    VALUE newiseq = iseq_alloc(rb_cISeq);
    rb_iseq_t *iseq0, *iseq1;

    GetISeqPtr(iseqval, iseq0);
    GetISeqPtr(newiseq, iseq1);

    *iseq1 = *iseq0;
    iseq1->self = newiseq;
    if (!iseq1->orig) {
	iseq1->orig = iseqval;
    }
    if (iseq0->local_iseq == iseq0) {
	iseq1->local_iseq = iseq1;
    }
    if (newcbase) {
	iseq1->cref_stack = NEW_BLOCK(newcbase);
	if (iseq0->cref_stack->nd_next) {
	    iseq1->cref_stack->nd_next = iseq0->cref_stack->nd_next;
	}
	iseq1->klass = newcbase;
    }

    return newiseq;
}

VALUE
rb_iseq_parameters(const rb_iseq_t *iseq, int is_proc)
{
    int i, r;
    VALUE a, args = rb_ary_new2(iseq->arg_size);
    ID req, opt, rest, block, key, keyrest;
#define PARAM_TYPE(type) rb_ary_push(a = rb_ary_new2(2), ID2SYM(type))
#define PARAM_ID(i) iseq->local_table[(i)]
#define PARAM(i, type) (		      \
	PARAM_TYPE(type),		      \
	rb_id2name(PARAM_ID(i)) ?	      \
	rb_ary_push(a, ID2SYM(PARAM_ID(i))) : \
	a)

    CONST_ID(req, "req");
    CONST_ID(opt, "opt");
    if (is_proc) {
	for (i = 0; i < iseq->argc; i++) {
	    PARAM_TYPE(opt);
	    rb_ary_push(a, rb_id2name(PARAM_ID(i)) ? ID2SYM(PARAM_ID(i)) : Qnil);
	    rb_ary_push(args, a);
	}
    }
    else {
	for (i = 0; i < iseq->argc; i++) {
	    rb_ary_push(args, PARAM(i, req));
	}
    }
    r = iseq->arg_rest != -1 ? iseq->arg_rest :
	iseq->arg_post_len > 0 ? iseq->arg_post_start :
	iseq->arg_block != -1 ? iseq->arg_block :
	iseq->arg_keyword != -1 ? iseq->arg_keyword :
	iseq->arg_size;
    if (iseq->arg_keyword != -1) r -= iseq->arg_keywords;
    for (; i < r; i++) {
	PARAM_TYPE(opt);
	if (rb_id2name(PARAM_ID(i))) {
	    rb_ary_push(a, ID2SYM(PARAM_ID(i)));
	}
	rb_ary_push(args, a);
    }
    if (iseq->arg_rest != -1) {
	CONST_ID(rest, "rest");
	rb_ary_push(args, PARAM(iseq->arg_rest, rest));
    }
    r = iseq->arg_post_start + iseq->arg_post_len;
    if (is_proc) {
	for (i = iseq->arg_post_start; i < r; i++) {
	    PARAM_TYPE(opt);
	    rb_ary_push(a, rb_id2name(PARAM_ID(i)) ? ID2SYM(PARAM_ID(i)) : Qnil);
	    rb_ary_push(args, a);
	}
    }
    else {
	for (i = iseq->arg_post_start; i < r; i++) {
	    rb_ary_push(args, PARAM(i, req));
	}
    }
    if (iseq->arg_keyword != -1) {
	CONST_ID(key, "key");
	for (i = 0; i < iseq->arg_keywords; i++) {
	    PARAM_TYPE(key);
	    if (rb_id2name(iseq->arg_keyword_table[i])) {
		rb_ary_push(a, ID2SYM(iseq->arg_keyword_table[i]));
	    }
	    rb_ary_push(args, a);
	}
	if (rb_id2name(iseq->local_table[iseq->arg_keyword])) {
	    CONST_ID(keyrest, "keyrest");
	    rb_ary_push(args, PARAM(iseq->arg_keyword, keyrest));
	}
    }
    if (iseq->arg_block != -1) {
	CONST_ID(block, "block");
	rb_ary_push(args, PARAM(iseq->arg_block, block));
    }
    return args;
}

/* ruby2cext */

VALUE
rb_iseq_build_for_ruby2cext(
    const rb_iseq_t *iseq_template,
    const rb_insn_func_t *func,
    const struct iseq_line_info_entry *line_info_table,
    const char **local_table,
    const VALUE *arg_opt_table,
    const struct iseq_catch_table_entry *catch_table,
    const char *name,
    const char *filename,
    const unsigned short line_no)
{
    unsigned long i;
    VALUE iseqval = iseq_alloc(rb_cISeq);
    rb_iseq_t *iseq;
    GetISeqPtr(iseqval, iseq);

    /* copy iseq */
    *iseq = *iseq_template;
    iseq->name = rb_str_new2(name);
    iseq->filename = rb_str_new2(filename);
    iseq->line_no = line_no;
    iseq->mark_ary = rb_ary_tmp_new(3);
    OBJ_UNTRUST(iseq->mark_ary);
    iseq->self = iseqval;

    iseq->iseq = ALLOC_N(VALUE, iseq->iseq_size);

    for (i=0; i<iseq->iseq_size; i+=2) {
	iseq->iseq[i] = BIN(opt_call_c_function);
	iseq->iseq[i+1] = (VALUE)func;
    }

    rb_iseq_translate_threaded_code(iseq);

#define ALLOC_AND_COPY(dst, src, type, size) do { \
  if (size) { \
      (dst) = ALLOC_N(type, (size)); \
      MEMCPY((dst), (src), type, (size)); \
  } \
} while (0)

    ALLOC_AND_COPY(iseq->line_info_table, line_info_table,
		   struct iseq_line_info_entry, iseq->line_info_size);

    ALLOC_AND_COPY(iseq->catch_table, catch_table,
		   struct iseq_catch_table_entry, iseq->catch_table_size);

    ALLOC_AND_COPY(iseq->arg_opt_table, arg_opt_table,
		   VALUE, iseq->arg_opts);

    set_relation(iseq, 0);

    return iseqval;
}

void
Init_ISeq(void)
{
    /* declare ::RubyVM::InstructionSequence */
    rb_cISeq = rb_define_class_under(rb_cRubyVM, "InstructionSequence", rb_cObject);
    rb_define_alloc_func(rb_cISeq, iseq_alloc);
    rb_define_method(rb_cISeq, "inspect", iseq_inspect, 0);
    rb_define_method(rb_cISeq, "disasm", rb_iseq_disasm, 0);
    rb_define_method(rb_cISeq, "disassemble", rb_iseq_disasm, 0);
    rb_define_method(rb_cISeq, "to_a", iseq_to_a, 0);
    rb_define_method(rb_cISeq, "eval", iseq_eval, 0);

#if 0 /* TBD */
    rb_define_method(rb_cISeq, "marshal_dump", iseq_marshal_dump, 0);
    rb_define_method(rb_cISeq, "marshal_load", iseq_marshal_load, 1);
#endif

    /* disable this feature because there is no verifier. */
    /* rb_define_singleton_method(rb_cISeq, "load", iseq_s_load, -1); */
    (void)iseq_s_load;

    rb_define_singleton_method(rb_cISeq, "compile", iseq_s_compile, -1);
    rb_define_singleton_method(rb_cISeq, "new", iseq_s_compile, -1);
    rb_define_singleton_method(rb_cISeq, "compile_file", iseq_s_compile_file, -1);
    rb_define_singleton_method(rb_cISeq, "compile_option", iseq_s_compile_option_get, 0);
    rb_define_singleton_method(rb_cISeq, "compile_option=", iseq_s_compile_option_set, 1);
    rb_define_singleton_method(rb_cISeq, "disasm", iseq_s_disasm, 1);
    rb_define_singleton_method(rb_cISeq, "disassemble", iseq_s_disasm, 1);
}

