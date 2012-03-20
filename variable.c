/**********************************************************************

  variable.c -

  $Author$
  created at: Tue Apr 19 23:55:15 JST 1994

  Copyright (C) 1993-2007 Yukihiro Matsumoto
  Copyright (C) 2000  Network Applied Communication Laboratory, Inc.
  Copyright (C) 2000  Information-technology Promotion Agency, Japan

**********************************************************************/

#include "ruby/ruby.h"
#include "ruby/st.h"
#include "ruby/util.h"
#include "ruby/encoding.h"
#include "node.h"
#include "constant.h"
#include "internal.h"

st_table *rb_global_tbl;
st_table *rb_class_tbl;
static ID autoload, classpath, tmp_classpath, classid;

void
Init_var_tables(void)
{
    rb_global_tbl = st_init_numtable();
    rb_class_tbl = st_init_numtable();
    CONST_ID(autoload, "__autoload__");
    CONST_ID(classpath, "__classpath__");
    CONST_ID(tmp_classpath, "__tmp_classpath__");
    CONST_ID(classid, "__classid__");
}

struct fc_result {
    ID name;
    VALUE klass;
    VALUE path;
    VALUE track;
    struct fc_result *prev;
};

static VALUE
fc_path(struct fc_result *fc, ID name)
{
    VALUE path, tmp;

    path = rb_str_dup(rb_id2str(name));
    while (fc) {
	st_data_t n;
	if (fc->track == rb_cObject) break;
	if (RCLASS_IV_TBL(fc->track) &&
	    st_lookup(RCLASS_IV_TBL(fc->track), (st_data_t)classpath, &n)) {
	    tmp = rb_str_dup((VALUE)n);
	    rb_str_cat2(tmp, "::");
	    rb_str_append(tmp, path);
	    path = tmp;
	    break;
	}
	tmp = rb_str_dup(rb_id2str(fc->name));
	rb_str_cat2(tmp, "::");
	rb_str_append(tmp, path);
	path = tmp;
	fc = fc->prev;
    }
    OBJ_FREEZE(path);
    return path;
}

static int
fc_i(ID key, rb_const_entry_t *ce, struct fc_result *res)
{
    VALUE value = ce->value;
    if (!rb_is_const_id(key)) return ST_CONTINUE;

    if (value == res->klass) {
	res->path = fc_path(res, key);
	return ST_STOP;
    }
    switch (TYPE(value)) {
      case T_MODULE:
      case T_CLASS:
	if (!RCLASS_CONST_TBL(value)) return ST_CONTINUE;
	else {
	    struct fc_result arg;
	    struct fc_result *list;

	    list = res;
	    while (list) {
		if (list->track == value) return ST_CONTINUE;
		list = list->prev;
	    }

	    arg.name = key;
	    arg.path = 0;
	    arg.klass = res->klass;
	    arg.track = value;
	    arg.prev = res;
	    st_foreach(RCLASS_CONST_TBL(value), fc_i, (st_data_t)&arg);
	    if (arg.path) {
		res->path = arg.path;
		return ST_STOP;
	    }
	}
	break;

      default:
	break;
    }
    return ST_CONTINUE;
}

static VALUE
find_class_path(VALUE klass)
{
    struct fc_result arg;

    arg.name = 0;
    arg.path = 0;
    arg.klass = klass;
    arg.track = rb_cObject;
    arg.prev = 0;
    if (RCLASS_CONST_TBL(rb_cObject)) {
	st_foreach_safe(RCLASS_CONST_TBL(rb_cObject), fc_i, (st_data_t)&arg);
    }
    if (arg.path == 0) {
	st_foreach_safe(rb_class_tbl, fc_i, (st_data_t)&arg);
    }
    if (arg.path) {
	st_data_t tmp = tmp_classpath;
	if (!RCLASS_IV_TBL(klass)) {
	    RCLASS_IV_TBL(klass) = st_init_numtable();
	}
	st_insert(RCLASS_IV_TBL(klass), (st_data_t)classpath, arg.path);
	st_delete(RCLASS_IV_TBL(klass), &tmp, 0);
	return arg.path;
    }
    return Qnil;
}

static VALUE
classname(VALUE klass)
{
    VALUE path = Qnil;
    st_data_t n;

    if (!klass) klass = rb_cObject;
    if (RCLASS_IV_TBL(klass)) {
	if (!st_lookup(RCLASS_IV_TBL(klass), (st_data_t)classpath, &n)) {
	    if (!st_lookup(RCLASS_IV_TBL(klass), (st_data_t)classid, &n)) {
		return find_class_path(klass);
	    }
	    path = rb_str_dup(rb_id2str(SYM2ID((VALUE)n)));
	    OBJ_FREEZE(path);
	    st_insert(RCLASS_IV_TBL(klass), (st_data_t)classpath, (st_data_t)path);
	    n = classid;
	    st_delete(RCLASS_IV_TBL(klass), &n, 0);
	}
	else {
	    path = (VALUE)n;
	}
	if (!RB_TYPE_P(path, T_STRING)) {
	    rb_bug("class path is not set properly");
	}
	return path;
    }
    return find_class_path(klass);
}

/*
 *  call-seq:
 *     mod.name    -> string
 *
 *  Returns the name of the module <i>mod</i>.  Returns nil for anonymous modules.
 */

VALUE
rb_mod_name(VALUE mod)
{
    VALUE path = classname(mod);

    if (!NIL_P(path)) return rb_str_dup(path);
    return path;
}

VALUE
rb_class_path(VALUE klass)
{
    VALUE path = classname(klass);
    st_data_t n = (st_data_t)path;

    if (!NIL_P(path)) return path;
    if (RCLASS_IV_TBL(klass) && st_lookup(RCLASS_IV_TBL(klass),
					  (st_data_t)tmp_classpath, &n)) {
	return (VALUE)n;
    }
    else {
	const char *s = "Class";

	if (RB_TYPE_P(klass, T_MODULE)) {
	    if (rb_obj_class(klass) == rb_cModule) {
		s = "Module";
	    }
	    else {
		s = rb_class2name(RBASIC(klass)->klass);
	    }
	}
	path = rb_sprintf("#<%s:%p>", s, (void*)klass);
	OBJ_FREEZE(path);
	rb_ivar_set(klass, tmp_classpath, path);

	return path;
    }
}

void
rb_set_class_path_string(VALUE klass, VALUE under, VALUE name)
{
    VALUE str;

    if (under == rb_cObject) {
	str = rb_str_new_frozen(name);
    }
    else {
	str = rb_str_dup(rb_class_path(under));
	rb_str_cat2(str, "::");
	rb_str_append(str, name);
	OBJ_FREEZE(str);
    }
    rb_ivar_set(klass, classpath, str);
}

void
rb_set_class_path(VALUE klass, VALUE under, const char *name)
{
    VALUE str;

    if (under == rb_cObject) {
	str = rb_str_new2(name);
    }
    else {
	str = rb_str_dup(rb_class_path(under));
	rb_str_cat2(str, "::");
	rb_str_cat2(str, name);
    }
    OBJ_FREEZE(str);
    rb_ivar_set(klass, classpath, str);
}

VALUE
rb_path_to_class(VALUE pathname)
{
    rb_encoding *enc = rb_enc_get(pathname);
    const char *pbeg, *p, *path = RSTRING_PTR(pathname);
    ID id;
    VALUE c = rb_cObject;

    if (!rb_enc_asciicompat(enc)) {
	rb_raise(rb_eArgError, "invalid class path encoding (non ASCII)");
    }
    pbeg = p = path;
    if (path[0] == '#') {
	rb_raise(rb_eArgError, "can't retrieve anonymous class %s", path);
    }
    while (*p) {
	while (*p && *p != ':') p++;
	id = rb_intern3(pbeg, p-pbeg, enc);
	if (p[0] == ':') {
	    if (p[1] != ':') goto undefined_class;
	    p += 2;
	    pbeg = p;
	}
	if (!rb_const_defined_at(c, id)) {
	  undefined_class:
	    rb_raise(rb_eArgError, "undefined class/module %.*s", (int)(p-path), path);
	}
	c = rb_const_get_at(c, id);
	switch (TYPE(c)) {
	  case T_MODULE:
	  case T_CLASS:
	    break;
	  default:
	    rb_raise(rb_eTypeError, "%s does not refer to class/module", path);
	}
    }

    return c;
}

VALUE
rb_path2class(const char *path)
{
    return rb_path_to_class(rb_str_new_cstr(path));
}

void
rb_name_class(VALUE klass, ID id)
{
    rb_ivar_set(klass, classid, ID2SYM(id));
}

VALUE
rb_class_name(VALUE klass)
{
    return rb_class_path(rb_class_real(klass));
}

const char *
rb_class2name(VALUE klass)
{
    VALUE name = rb_class_name(klass);
    return RSTRING_PTR(name);
}

const char *
rb_obj_classname(VALUE obj)
{
    return rb_class2name(CLASS_OF(obj));
}

#define global_variable rb_global_variable
#define global_entry rb_global_entry

#define gvar_getter_t rb_gvar_getter_t
#define gvar_setter_t rb_gvar_setter_t
#define gvar_marker_t rb_gvar_marker_t

struct trace_var {
    int removed;
    void (*func)(VALUE arg, VALUE val);
    VALUE data;
    struct trace_var *next;
};

struct global_variable {
    int   counter;
    void *data;
    gvar_getter_t *getter;
    gvar_setter_t *setter;
    gvar_marker_t *marker;
    int block_trace;
    struct trace_var *trace;
};

#define undef_getter	rb_gvar_undef_getter
#define undef_setter	rb_gvar_undef_setter
#define undef_marker	rb_gvar_undef_marker

#define val_getter	rb_gvar_val_getter
#define val_setter	rb_gvar_val_setter
#define val_marker	rb_gvar_val_marker

#define var_getter	rb_gvar_var_getter
#define var_setter	rb_gvar_var_setter
#define var_marker	rb_gvar_var_marker

#define readonly_setter rb_gvar_readonly_setter

struct global_entry*
rb_global_entry(ID id)
{
    struct global_entry *entry;
    st_data_t data;

    if (!st_lookup(rb_global_tbl, (st_data_t)id, &data)) {
	struct global_variable *var;
	entry = ALLOC(struct global_entry);
	var = ALLOC(struct global_variable);
	entry->id = id;
	entry->var = var;
	var->counter = 1;
	var->data = 0;
	var->getter = undef_getter;
	var->setter = undef_setter;
	var->marker = undef_marker;

	var->block_trace = 0;
	var->trace = 0;
	st_add_direct(rb_global_tbl, id, (st_data_t)entry);
    }
    else {
	entry = (struct global_entry *)data;
    }
    return entry;
}

VALUE
undef_getter(ID id, void *data, struct global_variable *var)
{
    rb_warning("global variable `%s' not initialized", rb_id2name(id));

    return Qnil;
}

void
undef_setter(VALUE val, ID id, void *data, struct global_variable *var)
{
    var->getter = val_getter;
    var->setter = val_setter;
    var->marker = val_marker;

    var->data = (void*)val;
}

void
undef_marker(VALUE *var)
{
}

VALUE
val_getter(ID id, void *data, struct global_variable *var)
{
    return (VALUE)data;
}

void
val_setter(VALUE val, ID id, void *data, struct global_variable *var)
{
    var->data = (void*)val;
}

void
val_marker(VALUE *var)
{
    VALUE data = (VALUE)var;
    if (data) rb_gc_mark_maybe(data);
}

VALUE
var_getter(ID id, void *data, struct global_variable *gvar)
{
    VALUE *var = data;
    if (!var) return Qnil;
    return *var;
}

void
var_setter(VALUE val, ID id, void *data, struct global_variable *gvar)
{
    *(VALUE *)data = val;
}

void
var_marker(VALUE *var)
{
    if (var) rb_gc_mark_maybe(*var);
}

void
readonly_setter(VALUE val, ID id, void *data, struct global_variable *gvar)
{
    rb_name_error(id, "%s is a read-only variable", rb_id2name(id));
}

static int
mark_global_entry(ID key, struct global_entry *entry)
{
    struct trace_var *trace;
    struct global_variable *var = entry->var;

    (*var->marker)(var->data);
    trace = var->trace;
    while (trace) {
	if (trace->data) rb_gc_mark_maybe(trace->data);
	trace = trace->next;
    }
    return ST_CONTINUE;
}

void
rb_gc_mark_global_tbl(void)
{
    if (rb_global_tbl)
        st_foreach_safe(rb_global_tbl, mark_global_entry, 0);
}

static ID
global_id(const char *name)
{
    ID id;

    if (name[0] == '$') id = rb_intern(name);
    else {
	size_t len = strlen(name);
	char *buf = ALLOCA_N(char, len+1);
	buf[0] = '$';
	memcpy(buf+1, name, len);
	id = rb_intern2(buf, len+1);
    }
    return id;
}

void
rb_define_hooked_variable(
    const char *name,
    VALUE *var,
    VALUE (*getter)(ANYARGS),
    void  (*setter)(ANYARGS))
{
    volatile VALUE tmp = var ? *var : Qnil;
    ID id = global_id(name);
    struct global_variable *gvar = rb_global_entry(id)->var;

    gvar->data = (void*)var;
    gvar->getter = getter?(gvar_getter_t *)getter:var_getter;
    gvar->setter = setter?(gvar_setter_t *)setter:var_setter;
    gvar->marker = var_marker;

    RB_GC_GUARD(tmp);
}

void
rb_define_variable(const char *name, VALUE *var)
{
    rb_define_hooked_variable(name, var, 0, 0);
}

void
rb_define_readonly_variable(const char *name, VALUE *var)
{
    rb_define_hooked_variable(name, var, 0, readonly_setter);
}

void
rb_define_virtual_variable(
    const char *name,
    VALUE (*getter)(ANYARGS),
    void  (*setter)(ANYARGS))
{
    if (!getter) getter = val_getter;
    if (!setter) setter = readonly_setter;
    rb_define_hooked_variable(name, 0, getter, setter);
}

static void
rb_trace_eval(VALUE cmd, VALUE val)
{
    rb_eval_cmd(cmd, rb_ary_new3(1, val), 0);
}

/*
 *  call-seq:
 *     trace_var(symbol, cmd )             -> nil
 *     trace_var(symbol) {|val| block }    -> nil
 *
 *  Controls tracing of assignments to global variables. The parameter
 *  +symbol_ identifies the variable (as either a string name or a
 *  symbol identifier). _cmd_ (which may be a string or a
 *  +Proc+ object) or block is executed whenever the variable
 *  is assigned. The block or +Proc+ object receives the
 *  variable's new value as a parameter. Also see
 *  <code>Kernel::untrace_var</code>.
 *
 *     trace_var :$_, proc {|v| puts "$_ is now '#{v}'" }
 *     $_ = "hello"
 *     $_ = ' there'
 *
 *  <em>produces:</em>
 *
 *     $_ is now 'hello'
 *     $_ is now ' there'
 */

VALUE
rb_f_trace_var(int argc, VALUE *argv)
{
    VALUE var, cmd;
    struct global_entry *entry;
    struct trace_var *trace;

    rb_secure(4);
    if (rb_scan_args(argc, argv, "11", &var, &cmd) == 1) {
	cmd = rb_block_proc();
    }
    if (NIL_P(cmd)) {
	return rb_f_untrace_var(argc, argv);
    }
    entry = rb_global_entry(rb_to_id(var));
    if (OBJ_TAINTED(cmd)) {
	rb_raise(rb_eSecurityError, "Insecure: tainted variable trace");
    }
    trace = ALLOC(struct trace_var);
    trace->next = entry->var->trace;
    trace->func = rb_trace_eval;
    trace->data = cmd;
    trace->removed = 0;
    entry->var->trace = trace;

    return Qnil;
}

static void
remove_trace(struct global_variable *var)
{
    struct trace_var *trace = var->trace;
    struct trace_var t;
    struct trace_var *next;

    t.next = trace;
    trace = &t;
    while (trace->next) {
	next = trace->next;
	if (next->removed) {
	    trace->next = next->next;
	    xfree(next);
	}
	else {
	    trace = next;
	}
    }
    var->trace = t.next;
}

/*
 *  call-seq:
 *     untrace_var(symbol [, cmd] )   -> array or nil
 *
 *  Removes tracing for the specified command on the given global
 *  variable and returns +nil+. If no command is specified,
 *  removes all tracing for that variable and returns an array
 *  containing the commands actually removed.
 */

VALUE
rb_f_untrace_var(int argc, VALUE *argv)
{
    VALUE var, cmd;
    ID id;
    struct global_entry *entry;
    struct trace_var *trace;
    st_data_t data;

    rb_secure(4);
    rb_scan_args(argc, argv, "11", &var, &cmd);
    id = rb_check_id(&var);
    if (!id) {
	rb_name_error_str(var, "undefined global variable %s", RSTRING_PTR(var));
    }
    if (!st_lookup(rb_global_tbl, (st_data_t)id, &data)) {
	rb_name_error(id, "undefined global variable %s", rb_id2name(id));
    }

    trace = (entry = (struct global_entry *)data)->var->trace;
    if (NIL_P(cmd)) {
	VALUE ary = rb_ary_new();

	while (trace) {
	    struct trace_var *next = trace->next;
	    rb_ary_push(ary, (VALUE)trace->data);
	    trace->removed = 1;
	    trace = next;
	}

	if (!entry->var->block_trace) remove_trace(entry->var);
	return ary;
    }
    else {
	while (trace) {
	    if (trace->data == cmd) {
		trace->removed = 1;
		if (!entry->var->block_trace) remove_trace(entry->var);
		return rb_ary_new3(1, cmd);
	    }
	    trace = trace->next;
	}
    }
    return Qnil;
}

VALUE
rb_gvar_get(struct global_entry *entry)
{
    struct global_variable *var = entry->var;
    return (*var->getter)(entry->id, var->data, var);
}

struct trace_data {
    struct trace_var *trace;
    VALUE val;
};

static VALUE
trace_ev(struct trace_data *data)
{
    struct trace_var *trace = data->trace;

    while (trace) {
	(*trace->func)(trace->data, data->val);
	trace = trace->next;
    }
    return Qnil;		/* not reached */
}

static VALUE
trace_en(struct global_variable *var)
{
    var->block_trace = 0;
    remove_trace(var);
    return Qnil;		/* not reached */
}

VALUE
rb_gvar_set(struct global_entry *entry, VALUE val)
{
    struct trace_data trace;
    struct global_variable *var = entry->var;

    if (rb_safe_level() >= 4)
	rb_raise(rb_eSecurityError, "Insecure: can't change global variable value");
    (*var->setter)(val, entry->id, var->data, var);

    if (var->trace && !var->block_trace) {
	var->block_trace = 1;
	trace.trace = var->trace;
	trace.val = val;
	rb_ensure(trace_ev, (VALUE)&trace, trace_en, (VALUE)var);
    }
    return val;
}

VALUE
rb_gv_set(const char *name, VALUE val)
{
    struct global_entry *entry;

    entry = rb_global_entry(global_id(name));
    return rb_gvar_set(entry, val);
}

VALUE
rb_gv_get(const char *name)
{
    struct global_entry *entry;

    entry = rb_global_entry(global_id(name));
    return rb_gvar_get(entry);
}

VALUE
rb_gvar_defined(struct global_entry *entry)
{
    if (entry->var->getter == undef_getter) return Qfalse;
    return Qtrue;
}

static int
gvar_i(ID key, struct global_entry *entry, VALUE ary)
{
    rb_ary_push(ary, ID2SYM(key));
    return ST_CONTINUE;
}

/*
 *  call-seq:
 *     global_variables    -> array
 *
 *  Returns an array of the names of global variables.
 *
 *     global_variables.grep /std/   #=> [:$stdin, :$stdout, :$stderr]
 */

VALUE
rb_f_global_variables(void)
{
    VALUE ary = rb_ary_new();
    char buf[2];
    int i;

    st_foreach_safe(rb_global_tbl, gvar_i, ary);
    buf[0] = '$';
    for (i = 1; i <= 9; ++i) {
	buf[1] = (char)(i + '0');
	rb_ary_push(ary, ID2SYM(rb_intern2(buf, 2)));
    }
    return ary;
}

void
rb_alias_variable(ID name1, ID name2)
{
    struct global_entry *entry1, *entry2;
    st_data_t data1;

    if (rb_safe_level() >= 4)
	rb_raise(rb_eSecurityError, "Insecure: can't alias global variable");

    entry2 = rb_global_entry(name2);
    if (!st_lookup(rb_global_tbl, (st_data_t)name1, &data1)) {
	entry1 = ALLOC(struct global_entry);
	entry1->id = name1;
	st_add_direct(rb_global_tbl, name1, (st_data_t)entry1);
    }
    else if ((entry1 = (struct global_entry *)data1)->var != entry2->var) {
	struct global_variable *var = entry1->var;
	if (var->block_trace) {
	    rb_raise(rb_eRuntimeError, "can't alias in tracer");
	}
	var->counter--;
	if (var->counter == 0) {
	    struct trace_var *trace = var->trace;
	    while (trace) {
		struct trace_var *next = trace->next;
		xfree(trace);
		trace = next;
	    }
	    xfree(var);
	}
    }
    else {
	return;
    }
    entry2->var->counter++;
    entry1->var = entry2->var;
}

static int special_generic_ivar = 0;
static st_table *generic_iv_tbl;

st_table*
rb_generic_ivar_table(VALUE obj)
{
    st_data_t tbl;

    if (!FL_TEST(obj, FL_EXIVAR)) return 0;
    if (!generic_iv_tbl) return 0;
    if (!st_lookup(generic_iv_tbl, (st_data_t)obj, &tbl)) return 0;
    return (st_table *)tbl;
}

static VALUE
generic_ivar_get(VALUE obj, ID id, int warn)
{
    st_data_t tbl, val;

    if (generic_iv_tbl) {
	if (st_lookup(generic_iv_tbl, (st_data_t)obj, &tbl)) {
	    if (st_lookup((st_table *)tbl, (st_data_t)id, &val)) {
		return (VALUE)val;
	    }
	}
    }
    if (warn) {
	rb_warning("instance variable %s not initialized", rb_id2name(id));
    }
    return Qnil;
}

static void
generic_ivar_set(VALUE obj, ID id, VALUE val)
{
    st_table *tbl;
    st_data_t data;

    if (rb_special_const_p(obj)) {
	if (rb_obj_frozen_p(obj)) rb_error_frozen("object");
	special_generic_ivar = 1;
    }
    if (!generic_iv_tbl) {
	generic_iv_tbl = st_init_numtable();
    }
    if (!st_lookup(generic_iv_tbl, (st_data_t)obj, &data)) {
	FL_SET(obj, FL_EXIVAR);
	tbl = st_init_numtable();
	st_add_direct(generic_iv_tbl, (st_data_t)obj, (st_data_t)tbl);
	st_add_direct(tbl, (st_data_t)id, (st_data_t)val);
	return;
    }
    st_insert((st_table *)data, (st_data_t)id, (st_data_t)val);
}

static VALUE
generic_ivar_defined(VALUE obj, ID id)
{
    st_table *tbl;
    st_data_t data;

    if (!generic_iv_tbl) return Qfalse;
    if (!st_lookup(generic_iv_tbl, (st_data_t)obj, &data)) return Qfalse;
    tbl = (st_table *)data;
    if (st_lookup(tbl, (st_data_t)id, &data)) {
	return Qtrue;
    }
    return Qfalse;
}

static int
generic_ivar_remove(VALUE obj, ID id, st_data_t *valp)
{
    st_table *tbl;
    st_data_t data, key = (st_data_t)id;
    int status;

    if (!generic_iv_tbl) return 0;
    if (!st_lookup(generic_iv_tbl, (st_data_t)obj, &data)) return 0;
    tbl = (st_table *)data;
    status = st_delete(tbl, &key, valp);
    if (tbl->num_entries == 0) {
	key = (st_data_t)obj;
	st_delete(generic_iv_tbl, &key, &data);
	st_free_table((st_table *)data);
    }
    return status;
}

void
rb_mark_generic_ivar(VALUE obj)
{
    st_data_t tbl;

    if (!generic_iv_tbl) return;
    if (st_lookup(generic_iv_tbl, (st_data_t)obj, &tbl)) {
	rb_mark_tbl((st_table *)tbl);
    }
}

static int
givar_mark_i(ID key, VALUE value)
{
    rb_gc_mark(value);
    return ST_CONTINUE;
}

static int
givar_i(VALUE obj, st_table *tbl)
{
    if (rb_special_const_p(obj)) {
	st_foreach_safe(tbl, givar_mark_i, 0);
    }
    return ST_CONTINUE;
}

void
rb_mark_generic_ivar_tbl(void)
{
    if (!generic_iv_tbl) return;
    if (special_generic_ivar == 0) return;
    st_foreach_safe(generic_iv_tbl, givar_i, 0);
}

void
rb_free_generic_ivar(VALUE obj)
{
    st_data_t key = (st_data_t)obj, tbl;

    if (!generic_iv_tbl) return;
    if (st_delete(generic_iv_tbl, &key, &tbl))
	st_free_table((st_table *)tbl);
}

RUBY_FUNC_EXPORTED size_t
rb_generic_ivar_memsize(VALUE obj)
{
    st_data_t tbl;
    if (st_lookup(generic_iv_tbl, (st_data_t)obj, &tbl))
	return st_memsize((st_table *)tbl);
    return 0;
}

void
rb_copy_generic_ivar(VALUE clone, VALUE obj)
{
    st_data_t data;

    if (!generic_iv_tbl) return;
    if (!FL_TEST(obj, FL_EXIVAR)) {
      clear:
        if (FL_TEST(clone, FL_EXIVAR)) {
            rb_free_generic_ivar(clone);
            FL_UNSET(clone, FL_EXIVAR);
        }
        return;
    }
    if (st_lookup(generic_iv_tbl, (st_data_t)obj, &data)) {
	st_table *tbl = (st_table *)data;

        if (tbl->num_entries == 0)
            goto clear;

	if (st_lookup(generic_iv_tbl, (st_data_t)clone, &data)) {
	    st_free_table((st_table *)data);
	    st_insert(generic_iv_tbl, (st_data_t)clone, (st_data_t)st_copy(tbl));
	}
	else {
	    st_add_direct(generic_iv_tbl, (st_data_t)clone, (st_data_t)st_copy(tbl));
	    FL_SET(clone, FL_EXIVAR);
	}
    }
}

static VALUE
ivar_get(VALUE obj, ID id, int warn)
{
    VALUE val, *ptr;
    struct st_table *iv_index_tbl;
    long len;
    st_data_t index;

    switch (TYPE(obj)) {
      case T_OBJECT:
        len = ROBJECT_NUMIV(obj);
        ptr = ROBJECT_IVPTR(obj);
        iv_index_tbl = ROBJECT_IV_INDEX_TBL(obj);
        if (!iv_index_tbl) break;
        if (!st_lookup(iv_index_tbl, (st_data_t)id, &index)) break;
        if (len <= (long)index) break;
        val = ptr[index];
        if (val != Qundef)
            return val;
	break;
      case T_CLASS:
      case T_MODULE:
	if (RCLASS_IV_TBL(obj) && st_lookup(RCLASS_IV_TBL(obj), (st_data_t)id, &index))
	    return (VALUE)index;
	break;
      default:
	if (FL_TEST(obj, FL_EXIVAR) || rb_special_const_p(obj))
	    return generic_ivar_get(obj, id, warn);
	break;
    }
    if (warn) {
	rb_warning("instance variable %s not initialized", rb_id2name(id));
    }
    return Qnil;
}

VALUE
rb_ivar_get(VALUE obj, ID id)
{
    return ivar_get(obj, id, TRUE);
}

VALUE
rb_attr_get(VALUE obj, ID id)
{
    return ivar_get(obj, id, FALSE);
}

VALUE
rb_ivar_set(VALUE obj, ID id, VALUE val)
{
    struct st_table *iv_index_tbl;
    st_data_t index;
    long i, len;
    int ivar_extended;

    if (!OBJ_UNTRUSTED(obj) && rb_safe_level() >= 4)
	rb_raise(rb_eSecurityError, "Insecure: can't modify instance variable");
    rb_check_frozen(obj);
    switch (TYPE(obj)) {
      case T_OBJECT:
        iv_index_tbl = ROBJECT_IV_INDEX_TBL(obj);
        if (!iv_index_tbl) {
            VALUE klass = rb_obj_class(obj);
            iv_index_tbl = RCLASS_IV_INDEX_TBL(klass);
            if (!iv_index_tbl) {
                iv_index_tbl = RCLASS_IV_INDEX_TBL(klass) = st_init_numtable();
            }
        }
        ivar_extended = 0;
        if (!st_lookup(iv_index_tbl, (st_data_t)id, &index)) {
            index = iv_index_tbl->num_entries;
            st_add_direct(iv_index_tbl, (st_data_t)id, index);
            ivar_extended = 1;
        }
        len = ROBJECT_NUMIV(obj);
        if (len <= (long)index) {
            VALUE *ptr = ROBJECT_IVPTR(obj);
            if (index < ROBJECT_EMBED_LEN_MAX) {
                RBASIC(obj)->flags |= ROBJECT_EMBED;
                ptr = ROBJECT(obj)->as.ary;
                for (i = 0; i < ROBJECT_EMBED_LEN_MAX; i++) {
                    ptr[i] = Qundef;
                }
            }
            else {
                VALUE *newptr;
                long newsize = (index+1) + (index+1)/4; /* (index+1)*1.25 */
                if (!ivar_extended &&
                    iv_index_tbl->num_entries < (st_index_t)newsize) {
                    newsize = iv_index_tbl->num_entries;
                }
                if (RBASIC(obj)->flags & ROBJECT_EMBED) {
                    newptr = ALLOC_N(VALUE, newsize);
                    MEMCPY(newptr, ptr, VALUE, len);
                    RBASIC(obj)->flags &= ~ROBJECT_EMBED;
                    ROBJECT(obj)->as.heap.ivptr = newptr;
                }
                else {
                    REALLOC_N(ROBJECT(obj)->as.heap.ivptr, VALUE, newsize);
                    newptr = ROBJECT(obj)->as.heap.ivptr;
                }
                for (; len < newsize; len++)
                    newptr[len] = Qundef;
                ROBJECT(obj)->as.heap.numiv = newsize;
                ROBJECT(obj)->as.heap.iv_index_tbl = iv_index_tbl;
            }
        }
        ROBJECT_IVPTR(obj)[index] = val;
	break;
      case T_CLASS:
      case T_MODULE:
	if (!RCLASS_IV_TBL(obj)) RCLASS_IV_TBL(obj) = st_init_numtable();
	st_insert(RCLASS_IV_TBL(obj), (st_data_t)id, val);
        break;
      default:
	generic_ivar_set(obj, id, val);
	break;
    }
    return val;
}

VALUE
rb_ivar_defined(VALUE obj, ID id)
{
    VALUE val;
    struct st_table *iv_index_tbl;
    st_data_t index;
    switch (TYPE(obj)) {
      case T_OBJECT:
        iv_index_tbl = ROBJECT_IV_INDEX_TBL(obj);
        if (!iv_index_tbl) break;
        if (!st_lookup(iv_index_tbl, (st_data_t)id, &index)) break;
        if (ROBJECT_NUMIV(obj) <= (long)index) break;
        val = ROBJECT_IVPTR(obj)[index];
        if (val != Qundef)
            return Qtrue;
	break;
      case T_CLASS:
      case T_MODULE:
	if (RCLASS_IV_TBL(obj) && st_lookup(RCLASS_IV_TBL(obj), (st_data_t)id, 0))
	    return Qtrue;
	break;
      default:
	if (FL_TEST(obj, FL_EXIVAR) || rb_special_const_p(obj))
	    return generic_ivar_defined(obj, id);
	break;
    }
    return Qfalse;
}

struct obj_ivar_tag {
    VALUE obj;
    int (*func)(ID key, VALUE val, st_data_t arg);
    st_data_t arg;
};

static int
obj_ivar_i(st_data_t key, st_data_t index, st_data_t arg)
{
    struct obj_ivar_tag *data = (struct obj_ivar_tag *)arg;
    if ((long)index < ROBJECT_NUMIV(data->obj)) {
        VALUE val = ROBJECT_IVPTR(data->obj)[(long)index];
        if (val != Qundef) {
            return (data->func)((ID)key, val, data->arg);
        }
    }
    return ST_CONTINUE;
}

static void
obj_ivar_each(VALUE obj, int (*func)(ANYARGS), st_data_t arg)
{
    st_table *tbl;
    struct obj_ivar_tag data;

    tbl = ROBJECT_IV_INDEX_TBL(obj);
    if (!tbl)
        return;

    data.obj = obj;
    data.func = (int (*)(ID key, VALUE val, st_data_t arg))func;
    data.arg = arg;

    st_foreach_safe(tbl, obj_ivar_i, (st_data_t)&data);
}

void
rb_ivar_foreach(VALUE obj, int (*func)(ANYARGS), st_data_t arg)
{
    switch (TYPE(obj)) {
      case T_OBJECT:
        obj_ivar_each(obj, func, arg);
	break;
      case T_CLASS:
      case T_MODULE:
	if (RCLASS_IV_TBL(obj)) {
	    st_foreach_safe(RCLASS_IV_TBL(obj), func, arg);
	}
	break;
      default:
	if (!generic_iv_tbl) break;
	if (FL_TEST(obj, FL_EXIVAR) || rb_special_const_p(obj)) {
	    st_data_t tbl;

	    if (st_lookup(generic_iv_tbl, (st_data_t)obj, &tbl)) {
		st_foreach_safe((st_table *)tbl, func, arg);
	    }
	}
	break;
    }
}

st_index_t
rb_ivar_count(VALUE obj)
{
    st_table *tbl;
    switch (TYPE(obj)) {
      case T_OBJECT:
	if ((tbl = ROBJECT_IV_INDEX_TBL(obj)) != 0) {
	    st_index_t i, count, num = tbl->num_entries;
	    const VALUE *const ivptr = ROBJECT_IVPTR(obj);
	    for (i = count = 0; i < num; ++i) {
		if (ivptr[i] != Qundef) {
		    count++;
		}
	    }
	    return count;
        }
	break;
      case T_CLASS:
      case T_MODULE:
	if ((tbl = RCLASS_IV_TBL(obj)) != 0) {
	    return tbl->num_entries;
	}
	break;
      default:
	if (!generic_iv_tbl) break;
	if (FL_TEST(obj, FL_EXIVAR) || rb_special_const_p(obj)) {
	    st_data_t data;

	    if (st_lookup(generic_iv_tbl, (st_data_t)obj, &data) &&
		(tbl = (st_table *)data) != 0) {
		return tbl->num_entries;
	    }
	}
	break;
    }
    return 0;
}

static int
ivar_i(ID key, VALUE val, VALUE ary)
{
    if (rb_is_instance_id(key)) {
	rb_ary_push(ary, ID2SYM(key));
    }
    return ST_CONTINUE;
}

/*
 *  call-seq:
 *     obj.instance_variables    -> array
 *
 *  Returns an array of instance variable names for the receiver. Note
 *  that simply defining an accessor does not create the corresponding
 *  instance variable.
 *
 *     class Fred
 *       attr_accessor :a1
 *       def initialize
 *         @iv = 3
 *       end
 *     end
 *     Fred.new.instance_variables   #=> [:@iv]
 */

VALUE
rb_obj_instance_variables(VALUE obj)
{
    VALUE ary;

    ary = rb_ary_new();
    rb_ivar_foreach(obj, ivar_i, ary);
    return ary;
}

/*
 *  call-seq:
 *     obj.remove_instance_variable(symbol)    -> obj
 *
 *  Removes the named instance variable from <i>obj</i>, returning that
 *  variable's value.
 *
 *     class Dummy
 *       attr_reader :var
 *       def initialize
 *         @var = 99
 *       end
 *       def remove
 *         remove_instance_variable(:@var)
 *       end
 *     end
 *     d = Dummy.new
 *     d.var      #=> 99
 *     d.remove   #=> 99
 *     d.var      #=> nil
 */

VALUE
rb_obj_remove_instance_variable(VALUE obj, VALUE name)
{
    VALUE val = Qnil;
    const ID id = rb_check_id(&name);
    st_data_t n, v;
    struct st_table *iv_index_tbl;
    st_data_t index;

    if (!OBJ_UNTRUSTED(obj) && rb_safe_level() >= 4)
	rb_raise(rb_eSecurityError, "Insecure: can't modify instance variable");
    rb_check_frozen(obj);
    if (!id) {
	if (rb_is_instance_name(name)) {
	    rb_name_error_str(name, "instance variable %s not defined", RSTRING_PTR(name));
	}
	else {
	    rb_name_error_str(name, "`%s' is not allowed as an instance variable name", RSTRING_PTR(name));
	}
    }
    if (!rb_is_instance_id(id)) {
	rb_name_error(id, "`%s' is not allowed as an instance variable name", rb_id2name(id));
    }

    switch (TYPE(obj)) {
      case T_OBJECT:
        iv_index_tbl = ROBJECT_IV_INDEX_TBL(obj);
        if (!iv_index_tbl) break;
        if (!st_lookup(iv_index_tbl, (st_data_t)id, &index)) break;
        if (ROBJECT_NUMIV(obj) <= (long)index) break;
        val = ROBJECT_IVPTR(obj)[index];
        if (val != Qundef) {
            ROBJECT_IVPTR(obj)[index] = Qundef;
            return val;
        }
	break;
      case T_CLASS:
      case T_MODULE:
	n = id;
	if (RCLASS_IV_TBL(obj) && st_delete(RCLASS_IV_TBL(obj), &n, &v)) {
	    return (VALUE)v;
	}
	break;
      default:
	if (FL_TEST(obj, FL_EXIVAR) || rb_special_const_p(obj)) {
	    v = val;
	    if (generic_ivar_remove(obj, (st_data_t)id, &v)) {
		return (VALUE)v;
	    }
	}
	break;
    }
    rb_name_error(id, "instance variable %s not defined", rb_id2name(id));
    return Qnil;		/* not reached */
}

NORETURN(static void uninitialized_constant(VALUE, ID));
static void
uninitialized_constant(VALUE klass, ID id)
{
    if (klass && rb_class_real(klass) != rb_cObject)
	rb_name_error(id, "uninitialized constant %s::%s",
		      rb_class2name(klass),
		      rb_id2name(id));
    else {
	rb_name_error(id, "uninitialized constant %s", rb_id2name(id));
    }
}

static VALUE
const_missing(VALUE klass, ID id)
{
    return rb_funcall(klass, rb_intern("const_missing"), 1, ID2SYM(id));
}


/*
 * call-seq:
 *    mod.const_missing(sym)    -> obj
 *
 * Invoked when a reference is made to an undefined constant in
 * <i>mod</i>. It is passed a symbol for the undefined constant, and
 * returns a value to be used for that constant. The
 * following code is an example of the same:
 *
 *   def Foo.const_missing(name)
 *     name # return the constant name as Symbol
 *   end
 *
 *   Foo::UNDEFINED_CONST    #=> :UNDEFINED_CONST: symbol returned
 *
 * In the next example when a reference is made to an undefined constant,
 * it attempts to load a file whose name is the lowercase version of the
 * constant (thus class <code>Fred</code> is assumed to be in file
 * <code>fred.rb</code>).  If found, it returns the loaded class. It
 * therefore implements an autoload feature similar to Kernel#autoload and
 * Module#autoload.
 *
 *   def Object.const_missing(name)
 *     @looked_for ||= {}
 *     str_name = name.to_s
 *     raise "Class not found: #{name}" if @looked_for[str_name]
 *     @looked_for[str_name] = 1
 *     file = str_name.downcase
 *     require file
 *     klass = const_get(name)
 *     return klass if klass
 *     raise "Class not found: #{name}"
 *   end
 *
 */

VALUE
rb_mod_const_missing(VALUE klass, VALUE name)
{
    rb_frame_pop(); /* pop frame for "const_missing" */
    uninitialized_constant(klass, rb_to_id(name));
    return Qnil;		/* not reached */
}

static void
autoload_mark(void *ptr)
{
    rb_mark_tbl((st_table *)ptr);
}

static void
autoload_free(void *ptr)
{
    st_free_table((st_table *)ptr);
}

static size_t
autoload_memsize(const void *ptr)
{
    const st_table *tbl = ptr;
    return st_memsize(tbl);
}

static const rb_data_type_t autoload_data_type = {
    "autoload",
    {autoload_mark, autoload_free, autoload_memsize,},
};

#define check_autoload_table(av) \
    (struct st_table *)rb_check_typeddata((av), &autoload_data_type)

static VALUE
autoload_data(VALUE mod, ID id)
{
    struct st_table *tbl;
    st_data_t val;

    if (!st_lookup(RCLASS_IV_TBL(mod), autoload, &val) ||
	    !(tbl = check_autoload_table((VALUE)val)) || !st_lookup(tbl, (st_data_t)id, &val)) {
	return 0;
    }
    return (VALUE)val;
}

struct autoload_data_i {
    VALUE feature;
    int safe_level;
    VALUE thread;
    VALUE value;
};

static void
autoload_i_mark(void *ptr)
{
    struct autoload_data_i *p = ptr;
    rb_gc_mark(p->feature);
    rb_gc_mark(p->thread);
    rb_gc_mark(p->value);
}

static void
autoload_i_free(void *ptr)
{
    struct autoload_data_i *p = ptr;
    xfree(p);
}

static size_t
autoload_i_memsize(const void *ptr)
{
    return sizeof(struct autoload_data_i);
}

static const rb_data_type_t autoload_data_i_type = {
    "autoload_i",
    {autoload_i_mark, autoload_i_free, autoload_i_memsize,},
};

#define check_autoload_data(av) \
    (struct autoload_data_i *)rb_check_typeddata((av), &autoload_data_i_type)

void
rb_autoload(VALUE mod, ID id, const char *file)
{
    st_data_t av;
    VALUE ad, fn;
    struct st_table *tbl;
    struct autoload_data_i *ele;

    if (!rb_is_const_id(id)) {
	rb_raise(rb_eNameError, "autoload must be constant name: %s", rb_id2name(id));
    }
    if (!file || !*file) {
	rb_raise(rb_eArgError, "empty file name");
    }

    if ((tbl = RCLASS_CONST_TBL(mod)) && st_lookup(tbl, (st_data_t)id, &av) && ((rb_const_entry_t*)av)->value != Qundef)
	return;

    rb_const_set(mod, id, Qundef);
    tbl = RCLASS_IV_TBL(mod);
    if (tbl && st_lookup(tbl, (st_data_t)autoload, &av)) {
	tbl = check_autoload_table((VALUE)av);
    }
    else {
	if (!tbl) tbl = RCLASS_IV_TBL(mod) = st_init_numtable();
	av = (st_data_t)TypedData_Wrap_Struct(0, &autoload_data_type, 0);
	st_add_direct(tbl, (st_data_t)autoload, av);
	DATA_PTR(av) = tbl = st_init_numtable();
    }
    fn = rb_str_new2(file);
    FL_UNSET(fn, FL_TAINT);
    OBJ_FREEZE(fn);

    ele = ALLOC(struct autoload_data_i);
    ele->feature = fn;
    ele->safe_level = rb_safe_level();
    ele->thread = Qnil;
    ele->value = Qundef;
    ad = TypedData_Wrap_Struct(0, &autoload_data_i_type, ele);
    st_insert(tbl, (st_data_t)id, (st_data_t)ad);
}

static void
autoload_delete(VALUE mod, ID id)
{
    st_data_t val, load = 0, n = id;
    rb_const_entry_t *ce;

    st_delete(RCLASS_CONST_TBL(mod), &n, &val);
    ce = (rb_const_entry_t*)val;
    if (ce) xfree(ce);
    if (st_lookup(RCLASS_IV_TBL(mod), (st_data_t)autoload, &val)) {
	struct st_table *tbl = check_autoload_table((VALUE)val);

	st_delete(tbl, &n, &load);

	if (tbl->num_entries == 0) {
	    n = autoload;
	    st_delete(RCLASS_IV_TBL(mod), &n, &val);
	}
    }
}

static VALUE
autoload_provided(VALUE arg)
{
    const char **p = (const char **)arg;
    return rb_feature_provided(*p, p);
}

static VALUE
reset_safe(VALUE safe)
{
    rb_set_safe_level_force((int)safe);
    return safe;
}

static VALUE
check_autoload_required(VALUE mod, ID id, const char **loadingpath)
{
    VALUE file, load;
    struct autoload_data_i *ele;
    const char *loading;
    int safe;

    if (!(load = autoload_data(mod, id)) || !(ele = check_autoload_data(load))) {
	return 0;
    }
    file = ele->feature;
    Check_Type(file, T_STRING);
    if (!RSTRING_PTR(file) || !*RSTRING_PTR(file)) {
	rb_raise(rb_eArgError, "empty file name");
    }
    loading = RSTRING_PTR(file);
    safe = rb_safe_level();
    rb_set_safe_level_force(0);
    if (!rb_ensure(autoload_provided, (VALUE)&loading, reset_safe, (VALUE)safe)) {
	return load;
    }
    if (loadingpath && loading) {
	*loadingpath = loading;
	return load;
    }
    return 0;
}

int
rb_autoloading_value(VALUE mod, ID id, VALUE* value)
{
    VALUE load;
    struct autoload_data_i *ele;

    if (!(load = autoload_data(mod, id)) || !(ele = check_autoload_data(load))) {
	return 0;
    }
    if (ele->thread == rb_thread_current()) {
	if (ele->value != Qundef) {
    	    if (value) {
    		*value = ele->value;
    	    }
	    return 1;
	}
    }
    return 0;
}

static int
autoload_defined_p(VALUE mod, ID id)
{
    struct st_table *tbl = RCLASS_CONST_TBL(mod);
    st_data_t val;

    if (!tbl || !st_lookup(tbl, (st_data_t)id, &val) || ((rb_const_entry_t*)val)->value != Qundef) {
	return 0;
    }
    return !rb_autoloading_value(mod, id, NULL);
}

struct autoload_const_set_args {
    VALUE mod;
    ID id;
    VALUE value;
};

static VALUE
autoload_const_set(VALUE arg)
{
    struct autoload_const_set_args* args = (struct autoload_const_set_args *)arg;
    autoload_delete(args->mod, args->id);
    rb_const_set(args->mod, args->id, args->value);
    return 0;			/* ignored */
}

static VALUE
autoload_require(VALUE arg)
{
    struct autoload_data_i *ele = (struct autoload_data_i *)arg;
    return rb_require_safe(ele->feature, ele->safe_level);
}

VALUE
rb_autoload_load(VALUE mod, ID id)
{
    VALUE load, result;
    const char *loading = 0, *src;
    struct autoload_data_i *ele;
    int state = 0;

    if (!autoload_defined_p(mod, id)) return Qfalse;
    load = check_autoload_required(mod, id, &loading);
    if (!load) return Qfalse;
    src = rb_sourcefile();
    if (src && loading && strcmp(src, loading) == 0) return Qfalse;

    /* set ele->thread for a marker of autoloading thread */
    if (!(ele = check_autoload_data(load))) {
	return Qfalse;
    }
    if (ele->thread == Qnil) {
	ele->thread = rb_thread_current();
    }
    /* autoload_data_i can be deleted by another thread while require */
    result = rb_protect(autoload_require, (VALUE)ele, &state);
    if (ele->thread == rb_thread_current()) {
	ele->thread = Qnil;
    }
    if (state) rb_jump_tag(state);

    if (RTEST(result)) {
	/* At the last, move a value defined in autoload to constant table */
	if (ele->value != Qundef) {
	    int safe_backup;
	    struct autoload_const_set_args args;
	    args.mod = mod;
	    args.id = id;
	    args.value = ele->value;
	    safe_backup = rb_safe_level();
	    rb_set_safe_level_force(ele->safe_level);
	    rb_ensure(autoload_const_set, (VALUE)&args, reset_safe, (VALUE)safe_backup);
	}
    }
    RB_GC_GUARD(load);
    return result;
}

VALUE
rb_autoload_p(VALUE mod, ID id)
{
    VALUE load;
    struct autoload_data_i *ele;

    while (!autoload_defined_p(mod, id)) {
	mod = RCLASS_SUPER(mod);
	if (!mod) return Qnil;
    }
    load = check_autoload_required(mod, id, 0);
    if (!load) return Qnil;
    return (ele = check_autoload_data(load)) ? ele->feature : Qnil;
}

static VALUE
rb_const_get_0(VALUE klass, ID id, int exclude, int recurse, int visibility)
{
    VALUE value, tmp, av;
    int mod_retry = 0;

    tmp = klass;
  retry:
    while (RTEST(tmp)) {
	VALUE am = 0;
	st_data_t data;
	while (RCLASS_CONST_TBL(tmp) && st_lookup(RCLASS_CONST_TBL(tmp), (st_data_t)id, &data)) {
	    rb_const_entry_t *ce = (rb_const_entry_t *)data;
	    if (visibility && ce->flag == CONST_PRIVATE) {
		rb_name_error(id, "private constant %s::%s referenced", rb_class2name(klass), rb_id2name(id));
	    }
	    value = ce->value;
	    if (value == Qundef) {
		if (am == tmp) break;
		am = tmp;
		if (rb_autoloading_value(tmp, id, &av)) return av;
		rb_autoload_load(tmp, id);
		continue;
	    }
	    if (exclude && tmp == rb_cObject && klass != rb_cObject) {
		rb_warn("toplevel constant %s referenced by %s::%s",
			rb_id2name(id), rb_class2name(klass), rb_id2name(id));
	    }
	    return value;
	}
	if (!recurse) break;
	tmp = RCLASS_SUPER(tmp);
    }
    if (!exclude && !mod_retry && BUILTIN_TYPE(klass) == T_MODULE) {
	mod_retry = 1;
	tmp = rb_cObject;
	goto retry;
    }

    value = const_missing(klass, id);
    rb_vm_inc_const_missing_count();
    return value;
}

VALUE
rb_const_get_from(VALUE klass, ID id)
{
    return rb_const_get_0(klass, id, TRUE, TRUE, FALSE);
}

VALUE
rb_const_get(VALUE klass, ID id)
{
    return rb_const_get_0(klass, id, FALSE, TRUE, FALSE);
}

VALUE
rb_const_get_at(VALUE klass, ID id)
{
    return rb_const_get_0(klass, id, TRUE, FALSE, FALSE);
}

VALUE
rb_public_const_get_from(VALUE klass, ID id)
{
    return rb_const_get_0(klass, id, TRUE, TRUE, TRUE);
}

VALUE
rb_public_const_get(VALUE klass, ID id)
{
    return rb_const_get_0(klass, id, FALSE, TRUE, TRUE);
}

VALUE
rb_public_const_get_at(VALUE klass, ID id)
{
    return rb_const_get_0(klass, id, TRUE, FALSE, TRUE);
}

/*
 *  call-seq:
 *     remove_const(sym)   -> obj
 *
 *  Removes the definition of the given constant, returning that
 *  constant's previous value.  If that constant referred to
 *  a module, this will not change that module's name and can lead
 *  to confusion.
 */

VALUE
rb_mod_remove_const(VALUE mod, VALUE name)
{
    const ID id = rb_check_id(&name);

    if (!id) {
	if (rb_is_const_name(name)) {
	    rb_name_error_str(name, "constant %s::%s not defined",
			      rb_class2name(mod), RSTRING_PTR(name));
	}
	else {
	    rb_name_error_str(name, "`%s' is not allowed as a constant name", RSTRING_PTR(name));
	}
    }
    if (!rb_is_const_id(id)) {
	rb_name_error(id, "`%s' is not allowed as a constant name", rb_id2name(id));
    }
    return rb_const_remove(mod, id);
}

VALUE
rb_const_remove(VALUE mod, ID id)
{
    VALUE val;
    st_data_t v, n = id;

    if (!OBJ_UNTRUSTED(mod) && rb_safe_level() >= 4)
	rb_raise(rb_eSecurityError, "Insecure: can't remove constant");
    rb_check_frozen(mod);
    if (!RCLASS_CONST_TBL(mod) || !st_delete(RCLASS_CONST_TBL(mod), &n, &v)) {
	if (rb_const_defined_at(mod, id)) {
	    rb_name_error(id, "cannot remove %s::%s",
			  rb_class2name(mod), rb_id2name(id));
	}
	rb_name_error(id, "constant %s::%s not defined",
		      rb_class2name(mod), rb_id2name(id));
    }

    rb_vm_change_state();

    val = ((rb_const_entry_t*)v)->value;
    if (val == Qundef) {
	autoload_delete(mod, id);
	val = Qnil;
    }
    xfree((rb_const_entry_t*)v);
    return val;
}

static int
sv_i(ID key, rb_const_entry_t *ce, st_table *tbl)
{
    if (rb_is_const_id(key)) {
	if (!st_lookup(tbl, (st_data_t)key, 0)) {
	    st_insert(tbl, (st_data_t)key, (st_data_t)ce);
	}
    }
    return ST_CONTINUE;
}

void*
rb_mod_const_at(VALUE mod, void *data)
{
    st_table *tbl = data;
    if (!tbl) {
	tbl = st_init_numtable();
    }
    if (RCLASS_CONST_TBL(mod)) {
	st_foreach_safe(RCLASS_CONST_TBL(mod), sv_i, (st_data_t)tbl);
    }
    return tbl;
}

void*
rb_mod_const_of(VALUE mod, void *data)
{
    VALUE tmp = mod;
    for (;;) {
	data = rb_mod_const_at(tmp, data);
	tmp = RCLASS_SUPER(tmp);
	if (!tmp) break;
	if (tmp == rb_cObject && mod != rb_cObject) break;
    }
    return data;
}

static int
list_i(st_data_t key, st_data_t value, VALUE ary)
{
    ID sym = (ID)key;
    rb_const_entry_t *ce = (rb_const_entry_t *)value;
    if (ce->flag != CONST_PRIVATE) rb_ary_push(ary, ID2SYM(sym));
    return ST_CONTINUE;
}

VALUE
rb_const_list(void *data)
{
    st_table *tbl = data;
    VALUE ary;

    if (!tbl) return rb_ary_new2(0);
    ary = rb_ary_new2(tbl->num_entries);
    st_foreach_safe(tbl, list_i, ary);
    st_free_table(tbl);

    return ary;
}

/*
 *  call-seq:
 *     mod.constants(inherit=true)    -> array
 *
 *  Returns an array of the names of the constants accessible in
 *  <i>mod</i>. This includes the names of constants in any included
 *  modules (example at start of section), unless the <i>all</i>
 *  parameter is set to <code>false</code>.
 *
 *    IO.constants.include?(:SYNC)        #=> true
 *    IO.constants(false).include?(:SYNC) #=> false
 *
 *  Also see <code>Module::const_defined?</code>.
 */

VALUE
rb_mod_constants(int argc, VALUE *argv, VALUE mod)
{
    VALUE inherit;
    st_table *tbl;

    if (argc == 0) {
	inherit = Qtrue;
    }
    else {
	rb_scan_args(argc, argv, "01", &inherit);
    }
    if (RTEST(inherit)) {
	tbl = rb_mod_const_of(mod, 0);
    }
    else {
	tbl = rb_mod_const_at(mod, 0);
    }
    return rb_const_list(tbl);
}

static int
rb_const_defined_0(VALUE klass, ID id, int exclude, int recurse, int visibility)
{
    st_data_t value;
    VALUE tmp;
    int mod_retry = 0;

    tmp = klass;
  retry:
    while (tmp) {
	if (RCLASS_CONST_TBL(tmp) && st_lookup(RCLASS_CONST_TBL(tmp), (st_data_t)id, &value)) {
	    rb_const_entry_t *ce = (rb_const_entry_t *)value;
	    if (visibility && ce->flag == CONST_PRIVATE) {
		return (int)Qfalse;
	    }
	    if (ce->value == Qundef && !check_autoload_required(tmp, id, 0) && !rb_autoloading_value(tmp, id, 0))
		return (int)Qfalse;
	    return (int)Qtrue;
	}
	if (!recurse) break;
	tmp = RCLASS_SUPER(tmp);
    }
    if (!exclude && !mod_retry && BUILTIN_TYPE(klass) == T_MODULE) {
	mod_retry = 1;
	tmp = rb_cObject;
	goto retry;
    }
    return (int)Qfalse;
}

int
rb_const_defined_from(VALUE klass, ID id)
{
    return rb_const_defined_0(klass, id, TRUE, TRUE, FALSE);
}

int
rb_const_defined(VALUE klass, ID id)
{
    return rb_const_defined_0(klass, id, FALSE, TRUE, FALSE);
}

int
rb_const_defined_at(VALUE klass, ID id)
{
    return rb_const_defined_0(klass, id, TRUE, FALSE, FALSE);
}

int
rb_public_const_defined_from(VALUE klass, ID id)
{
    return rb_const_defined_0(klass, id, TRUE, TRUE, TRUE);
}

int
rb_public_const_defined(VALUE klass, ID id)
{
    return rb_const_defined_0(klass, id, FALSE, TRUE, TRUE);
}

int
rb_public_const_defined_at(VALUE klass, ID id)
{
    return rb_const_defined_0(klass, id, TRUE, FALSE, TRUE);
}

static void
check_before_mod_set(VALUE klass, ID id, VALUE val, const char *dest)
{
    if (!OBJ_UNTRUSTED(klass) && rb_safe_level() >= 4)
	rb_raise(rb_eSecurityError, "Insecure: can't set %s", dest);
    rb_check_frozen(klass);
}

void
rb_const_set(VALUE klass, ID id, VALUE val)
{
    rb_const_entry_t *ce;
    VALUE visibility = CONST_PUBLIC;

    if (NIL_P(klass)) {
	rb_raise(rb_eTypeError, "no class/module to define constant %s",
		 rb_id2name(id));
    }

    check_before_mod_set(klass, id, val, "constant");
    if (!RCLASS_CONST_TBL(klass)) {
	RCLASS_CONST_TBL(klass) = st_init_numtable();
    }
    else {
	st_data_t value;

	if (st_lookup(RCLASS_CONST_TBL(klass), (st_data_t)id, &value)) {
	    rb_const_entry_t *ce = (rb_const_entry_t*)value;
	    if (ce->value == Qundef) {
		VALUE load;
		struct autoload_data_i *ele;

		load = autoload_data(klass, id);
		/* for autoloading thread, keep the defined value to autoloading storage */
		if (load && (ele = check_autoload_data(load)) && (ele->thread == rb_thread_current())) {
		    rb_vm_change_state();
		    ele->value = val;
		    return;
		}
		/* otherwise, allow to override */
		autoload_delete(klass, id);
	    }
	    else {
		const char *name = rb_id2name(id);
		visibility = ce->flag;
		rb_warn("already initialized constant %s", name);
		if (!NIL_P(ce->file) && ce->line) {
		    rb_compile_warn(RSTRING_PTR(ce->file), ce->line,
				    "previous definition of %s was here", name);
		}
	    }
	}
    }

    rb_vm_change_state();

    ce = ALLOC(rb_const_entry_t);
    ce->flag = (rb_const_flag_t)visibility;
    ce->value = val;
    ce->file = rb_sourcefilename();
    ce->line = rb_sourceline();

    st_insert(RCLASS_CONST_TBL(klass), (st_data_t)id, (st_data_t)ce);
}

void
rb_define_const(VALUE klass, const char *name, VALUE val)
{
    ID id = rb_intern(name);

    if (!rb_is_const_id(id)) {
	rb_warn("rb_define_const: invalid name `%s' for constant", name);
    }
    if (klass == rb_cObject) {
	rb_secure(4);
    }
    rb_const_set(klass, id, val);
}

void
rb_define_global_const(const char *name, VALUE val)
{
    rb_define_const(rb_cObject, name, val);
}

static void
set_const_visibility(VALUE mod, int argc, VALUE *argv, rb_const_flag_t flag)
{
    int i;
    st_data_t v;
    ID id;

    if (rb_safe_level() >= 4 && !OBJ_UNTRUSTED(mod)) {
	rb_raise(rb_eSecurityError,
		 "Insecure: can't change constant visibility");
    }

    if (argc == 0) {
	rb_warning("%s with no argument is just ignored", rb_id2name(rb_frame_callee()));
    }

    for (i = 0; i < argc; i++) {
	VALUE val = argv[i];
	id = rb_check_id(&val);
	if (!id) {
	    if ( i > 0 )
		rb_clear_cache_by_class(mod);
	    rb_name_error_str(val, "constant %s::%s not defined", rb_class2name(mod), RSTRING_PTR(val));
	}
	if (RCLASS_CONST_TBL(mod) &&
	    st_lookup(RCLASS_CONST_TBL(mod), (st_data_t)id, &v)) {
	    ((rb_const_entry_t*)v)->flag = flag;
	}
	else {
	    if ( i > 0 )
		rb_clear_cache_by_class(mod);
	    rb_name_error(id, "constant %s::%s not defined", rb_class2name(mod), rb_id2name(id));
	}
    }
    rb_clear_cache_by_class(mod);
}

/*
 *  call-seq:
 *     mod.private_constant(symbol, ...)    => mod
 *
 *  Makes a list of existing constants private.
 */

VALUE
rb_mod_private_constant(int argc, VALUE *argv, VALUE obj)
{
    set_const_visibility(obj, argc, argv, CONST_PRIVATE);
    return obj;
}

/*
 *  call-seq:
 *     mod.public_constant(symbol, ...)    => mod
 *
 *  Makes a list of existing constants public.
 */

VALUE
rb_mod_public_constant(int argc, VALUE *argv, VALUE obj)
{
    set_const_visibility(obj, argc, argv, CONST_PUBLIC);
    return obj;
}

static VALUE
original_module(VALUE c)
{
    if (RB_TYPE_P(c, T_ICLASS))
	return RBASIC(c)->klass;
    return c;
}

#define CVAR_LOOKUP(v,r) do {\
    if (RCLASS_IV_TBL(klass) && st_lookup(RCLASS_IV_TBL(klass),(st_data_t)id,(v))) {\
	r;\
    }\
    if (FL_TEST(klass, FL_SINGLETON) ) {\
	VALUE obj = rb_iv_get(klass, "__attached__");\
	switch (TYPE(obj)) {\
	  case T_MODULE:\
	  case T_CLASS:\
	    klass = obj;\
	    break;\
	  default:\
	    klass = RCLASS_SUPER(klass);\
	    break;\
	}\
    }\
    else {\
	klass = RCLASS_SUPER(klass);\
    }\
    while (klass) {\
	if (RCLASS_IV_TBL(klass) && st_lookup(RCLASS_IV_TBL(klass),(st_data_t)id,(v))) {\
	    r;\
	}\
	klass = RCLASS_SUPER(klass);\
    }\
} while(0)

void
rb_cvar_set(VALUE klass, ID id, VALUE val)
{
    VALUE tmp, front = 0, target = 0;

    tmp = klass;
    CVAR_LOOKUP(0, {if (!front) front = klass; target = klass;});
    if (target) {
	if (front && target != front) {
	    st_data_t did = id;

	    if (RTEST(ruby_verbose)) {
		rb_warning("class variable %s of %s is overtaken by %s",
			   rb_id2name(id), rb_class2name(original_module(front)),
			   rb_class2name(original_module(target)));
	    }
	    if (BUILTIN_TYPE(front) == T_CLASS) {
		st_delete(RCLASS_IV_TBL(front),&did,0);
	    }
	}
    }
    else {
	target = tmp;
    }

    check_before_mod_set(target, id, val, "class variable");
    if (!RCLASS_IV_TBL(target)) {
	RCLASS_IV_TBL(target) = st_init_numtable();
    }

    st_insert(RCLASS_IV_TBL(target), (st_data_t)id, (st_data_t)val);
}

VALUE
rb_cvar_get(VALUE klass, ID id)
{
    VALUE tmp, front = 0, target = 0;
    st_data_t value;

    tmp = klass;
    CVAR_LOOKUP(&value, {if (!front) front = klass; target = klass;});
    if (!target) {
	rb_name_error(id,"uninitialized class variable %s in %s",
		      rb_id2name(id), rb_class2name(tmp));
    }
    if (front && target != front) {
	st_data_t did = id;

	if (RTEST(ruby_verbose)) {
	    rb_warning("class variable %s of %s is overtaken by %s",
		       rb_id2name(id), rb_class2name(original_module(front)),
		       rb_class2name(original_module(target)));
	}
	if (BUILTIN_TYPE(front) == T_CLASS) {
	    st_delete(RCLASS_IV_TBL(front),&did,0);
	}
    }
    return (VALUE)value;
}

VALUE
rb_cvar_defined(VALUE klass, ID id)
{
    if (!klass) return Qfalse;
    CVAR_LOOKUP(0,return Qtrue);
    return Qfalse;
}

void
rb_cv_set(VALUE klass, const char *name, VALUE val)
{
    ID id = rb_intern(name);
    if (!rb_is_class_id(id)) {
	rb_name_error(id, "wrong class variable name %s", name);
    }
    rb_cvar_set(klass, id, val);
}

VALUE
rb_cv_get(VALUE klass, const char *name)
{
    ID id = rb_intern(name);
    if (!rb_is_class_id(id)) {
	rb_name_error(id, "wrong class variable name %s", name);
    }
    return rb_cvar_get(klass, id);
}

void
rb_define_class_variable(VALUE klass, const char *name, VALUE val)
{
    ID id = rb_intern(name);

    if (!rb_is_class_id(id)) {
	rb_name_error(id, "wrong class variable name %s", name);
    }
    rb_cvar_set(klass, id, val);
}

static int
cv_i(ID key, VALUE value, VALUE ary)
{
    if (rb_is_class_id(key)) {
	VALUE kval = ID2SYM(key);
	if (!rb_ary_includes(ary, kval)) {
	    rb_ary_push(ary, kval);
	}
    }
    return ST_CONTINUE;
}

/*
 *  call-seq:
 *     mod.class_variables   -> array
 *
 *  Returns an array of the names of class variables in <i>mod</i>.
 *
 *     class One
 *       @@var1 = 1
 *     end
 *     class Two < One
 *       @@var2 = 2
 *     end
 *     One.class_variables   #=> [:@@var1]
 *     Two.class_variables   #=> [:@@var2]
 */

VALUE
rb_mod_class_variables(VALUE obj)
{
    VALUE ary = rb_ary_new();

    if (RCLASS_IV_TBL(obj)) {
	st_foreach_safe(RCLASS_IV_TBL(obj), cv_i, ary);
    }
    return ary;
}

/*
 *  call-seq:
 *     remove_class_variable(sym)    -> obj
 *
 *  Removes the definition of the <i>sym</i>, returning that
 *  constant's value.
 *
 *     class Dummy
 *       @@var = 99
 *       puts @@var
 *       remove_class_variable(:@@var)
 *       p(defined? @@var)
 *     end
 *
 *  <em>produces:</em>
 *
 *     99
 *     nil
 */

VALUE
rb_mod_remove_cvar(VALUE mod, VALUE name)
{
    const ID id = rb_check_id(&name);
    st_data_t val, n = id;

    if (!id) {
	if (rb_is_class_name(name)) {
	    rb_name_error_str(name, "class variable %s not defined for %s",
			      RSTRING_PTR(name), rb_class2name(mod));
	}
	else {
	    rb_name_error_str(name, "wrong class variable name %s", RSTRING_PTR(name));
	}
    }
    if (!rb_is_class_id(id)) {
	rb_name_error(id, "wrong class variable name %s", rb_id2name(id));
    }
    if (!OBJ_UNTRUSTED(mod) && rb_safe_level() >= 4)
	rb_raise(rb_eSecurityError, "Insecure: can't remove class variable");
    rb_check_frozen(mod);
    if (RCLASS_IV_TBL(mod) && st_delete(RCLASS_IV_TBL(mod), &n, &val)) {
	return (VALUE)val;
    }
    if (rb_cvar_defined(mod, id)) {
	rb_name_error(id, "cannot remove %s for %s",
		 rb_id2name(id), rb_class2name(mod));
    }
    rb_name_error(id, "class variable %s not defined for %s",
		  rb_id2name(id), rb_class2name(mod));
    return Qnil;		/* not reached */
}

VALUE
rb_iv_get(VALUE obj, const char *name)
{
    ID id = rb_intern(name);

    return rb_ivar_get(obj, id);
}

VALUE
rb_iv_set(VALUE obj, const char *name, VALUE val)
{
    ID id = rb_intern(name);

    return rb_ivar_set(obj, id, val);
}
