/*
 * This file is included by vm.c
 */

#define CACHE_SIZE 0x800
#define CACHE_MASK 0x7ff
#define EXPR1(c,m) ((((c)>>3)^(m))&CACHE_MASK)

#define NOEX_NOREDEF NOEX_RESPONDS

static void rb_vm_check_redefinition_opt_method(const rb_method_entry_t *me, VALUE klass);

static ID object_id, respond_to_missing;
static ID removed, singleton_removed, undefined, singleton_undefined;
static ID added, singleton_added, attached;

struct cache_entry {		/* method hash table. */
    VALUE filled_version;        /* filled state version */
    ID mid;			/* method's id */
    VALUE klass;		/* receiver's class */
    rb_method_entry_t *me;
};

static struct cache_entry cache[CACHE_SIZE];
#define ruby_running (GET_VM()->running)
/* int ruby_running = 0; */

static void
vm_clear_global_method_cache(void)
{
    struct cache_entry *ent, *end;

    ent = cache;
    end = ent + CACHE_SIZE;
    while (ent < end) {
	ent->filled_version = 0;
	ent++;
    }
}

void
rb_clear_cache(void)
{
    rb_vm_change_state();
}

static void
rb_clear_cache_for_undef(VALUE klass, ID id)
{
    rb_vm_change_state();
}

static void
rb_clear_cache_by_id(ID id)
{
    rb_vm_change_state();
}

void
rb_clear_cache_by_class(VALUE klass)
{
    rb_vm_change_state();
}

VALUE
rb_f_notimplement(int argc, VALUE *argv, VALUE obj)
{
    rb_notimplement();
}

static void
rb_define_notimplement_method_id(VALUE mod, ID id, rb_method_flag_t noex)
{
    rb_add_method(mod, id, VM_METHOD_TYPE_NOTIMPLEMENTED, 0, noex);
}

void
rb_add_method_cfunc(VALUE klass, ID mid, VALUE (*func)(ANYARGS), int argc, rb_method_flag_t noex)
{
    if (func != rb_f_notimplement) {
	rb_method_cfunc_t opt;
	opt.func = func;
	opt.argc = argc;
	rb_add_method(klass, mid, VM_METHOD_TYPE_CFUNC, &opt, noex);
    }
    else {
	rb_define_notimplement_method_id(klass, mid, noex);
    }
}

void
rb_unlink_method_entry(rb_method_entry_t *me)
{
    struct unlinked_method_entry_list_entry *ume = ALLOC(struct unlinked_method_entry_list_entry);
    ume->me = me;
    ume->next = GET_VM()->unlinked_method_entry_list;
    GET_VM()->unlinked_method_entry_list = ume;
}

void
rb_gc_mark_unlinked_live_method_entries(void *pvm)
{
    rb_vm_t *vm = pvm;
    struct unlinked_method_entry_list_entry *ume = vm->unlinked_method_entry_list;

    while (ume) {
	if (ume->me->mark) {
	    rb_mark_method_entry(ume->me);
	}
	ume = ume->next;
    }
}

void
rb_sweep_method_entry(void *pvm)
{
    rb_vm_t *vm = pvm;
    struct unlinked_method_entry_list_entry *ume = vm->unlinked_method_entry_list, *prev_ume = 0, *curr_ume;

    while (ume) {
	if (ume->me->mark) {
	    ume->me->mark = 0;
	    prev_ume = ume;
	    ume = ume->next;
	}
	else {
	    rb_free_method_entry(ume->me);

	    if (prev_ume == 0) {
		vm->unlinked_method_entry_list = ume->next;
	    }
	    else {
		prev_ume->next = ume->next;
	    }

	    curr_ume = ume;
	    ume = ume->next;
	    xfree(curr_ume);
	}
    }
}

void
rb_free_method_entry(rb_method_entry_t *me)
{
    rb_method_definition_t *def = me->def;

    if (def) {
	if (def->alias_count == 0) {
	    xfree(def);
	}
	else if (def->alias_count > 0) {
	    def->alias_count--;
	}
	me->def = 0;
    }
    xfree(me);
}

static int rb_method_definition_eq(const rb_method_definition_t *d1, const rb_method_definition_t *d2);

static rb_method_entry_t *
rb_method_entry_make(VALUE klass, ID mid, rb_method_type_t type,
		     rb_method_definition_t *def, rb_method_flag_t noex)
{
    rb_method_entry_t *me;
    st_table *mtbl;
    st_data_t data;

    if (NIL_P(klass)) {
	klass = rb_cObject;
    }
    if (rb_safe_level() >= 4 &&
	(klass == rb_cObject || !OBJ_UNTRUSTED(klass))) {
	rb_raise(rb_eSecurityError, "Insecure: can't define method");
    }
    if (!FL_TEST(klass, FL_SINGLETON) &&
	type != VM_METHOD_TYPE_NOTIMPLEMENTED &&
	type != VM_METHOD_TYPE_ZSUPER &&
	(mid == rb_intern("initialize") || mid == rb_intern("initialize_copy"))) {
	noex = NOEX_PRIVATE | noex;
    }
    else if (FL_TEST(klass, FL_SINGLETON) &&
	     type == VM_METHOD_TYPE_CFUNC &&
	     mid == rb_intern("allocate")) {
	rb_warn("defining %s.allocate is deprecated; use rb_define_alloc_func()",
		rb_class2name(rb_ivar_get(klass, attached)));
	mid = ID_ALLOCATOR;
    }

    rb_check_frozen(klass);
    mtbl = RCLASS_M_TBL(klass);

    /* check re-definition */
    if (st_lookup(mtbl, mid, &data)) {
	rb_method_entry_t *old_me = (rb_method_entry_t *)data;
	rb_method_definition_t *old_def = old_me->def;

	if (rb_method_definition_eq(old_def, def)) return old_me;
#if 0
	if (old_me->flag & NOEX_NOREDEF) {
	    rb_raise(rb_eTypeError, "cannot redefine %s#%s", rb_class2name(klass), rb_id2name(mid));
	}
#endif
	rb_vm_check_redefinition_opt_method(old_me, klass);

	if (RTEST(ruby_verbose) &&
	    type != VM_METHOD_TYPE_UNDEF &&
	    old_def->alias_count == 0 &&
	    old_def->type != VM_METHOD_TYPE_UNDEF &&
	    old_def->type != VM_METHOD_TYPE_ZSUPER) {
	    rb_iseq_t *iseq = 0;

	    rb_warning("method redefined; discarding old %s", rb_id2name(mid));
	    switch (old_def->type) {
	      case VM_METHOD_TYPE_ISEQ:
		iseq = old_def->body.iseq;
		break;
	      case VM_METHOD_TYPE_BMETHOD:
		iseq = rb_proc_get_iseq(old_def->body.proc, 0);
		break;
	      default:
		break;
	    }
	    if (iseq && !NIL_P(iseq->filename)) {
		int line = iseq->line_info_table ? rb_iseq_first_lineno(iseq) : 0;
		rb_compile_warning(RSTRING_PTR(iseq->filename), line,
				   "previous definition of %s was here",
				   rb_id2name(old_def->original_id));
	    }
	}

	rb_unlink_method_entry(old_me);
    }

    me = ALLOC(rb_method_entry_t);

    rb_clear_cache_by_id(mid);

    me->flag = NOEX_WITH_SAFE(noex);
    me->mark = 0;
    me->called_id = mid;
    me->klass = klass;
    me->def = def;
    if (def) def->alias_count++;

    /* check mid */
    if (klass == rb_cObject && mid == idInitialize) {
	rb_warn("redefining Object#initialize may cause infinite loop");
    }
    /* check mid */
    if (mid == object_id || mid == id__send__) {
	if (type == VM_METHOD_TYPE_ISEQ) {
	    rb_warn("redefining `%s' may cause serious problems", rb_id2name(mid));
	}
    }

    st_insert(mtbl, mid, (st_data_t) me);

    return me;
}

#define CALL_METHOD_HOOK(klass, hook, mid) do {		\
	const VALUE arg = ID2SYM(mid);			\
	VALUE recv_class = (klass);			\
	ID hook_id = (hook);				\
	if (FL_TEST((klass), FL_SINGLETON)) {		\
	    recv_class = rb_ivar_get((klass), attached);	\
	    hook_id = singleton_##hook;			\
	}						\
	rb_funcall2(recv_class, hook_id, 1, &arg);	\
    } while (0)

static void
method_added(VALUE klass, ID mid)
{
    if (mid != ID_ALLOCATOR && ruby_running) {
	CALL_METHOD_HOOK(klass, added, mid);
    }
}

rb_method_entry_t *
rb_add_method(VALUE klass, ID mid, rb_method_type_t type, void *opts, rb_method_flag_t noex)
{
    rb_thread_t *th;
    rb_control_frame_t *cfp;
    int line;
    rb_method_entry_t *me = rb_method_entry_make(klass, mid, type, 0, noex);
    rb_method_definition_t *def = ALLOC(rb_method_definition_t);
    me->def = def;
    def->type = type;
    def->original_id = mid;
    def->alias_count = 0;
    switch (type) {
      case VM_METHOD_TYPE_ISEQ:
	def->body.iseq = (rb_iseq_t *)opts;
	break;
      case VM_METHOD_TYPE_CFUNC:
	def->body.cfunc = *(rb_method_cfunc_t *)opts;
	break;
      case VM_METHOD_TYPE_ATTRSET:
      case VM_METHOD_TYPE_IVAR:
	def->body.attr.id = (ID)opts;
	def->body.attr.location = Qfalse;
	th = GET_THREAD();
	cfp = rb_vm_get_ruby_level_next_cfp(th, th->cfp);
	if (cfp && (line = rb_vm_get_sourceline(cfp))) {
	    VALUE location = rb_ary_new3(2, cfp->iseq->filename, INT2FIX(line));
	    def->body.attr.location = rb_ary_freeze(location);
	}
	break;
      case VM_METHOD_TYPE_BMETHOD:
	def->body.proc = (VALUE)opts;
	break;
      case VM_METHOD_TYPE_NOTIMPLEMENTED:
	def->body.cfunc.func = rb_f_notimplement;
	def->body.cfunc.argc = -1;
	break;
      case VM_METHOD_TYPE_OPTIMIZED:
	def->body.optimize_type = (enum method_optimized_type)opts;
	break;
      case VM_METHOD_TYPE_ZSUPER:
      case VM_METHOD_TYPE_UNDEF:
	break;
      default:
	rb_bug("rb_add_method: unsupported method type (%d)\n", type);
    }
    if (type != VM_METHOD_TYPE_UNDEF) {
	method_added(klass, mid);
    }
    return me;
}

rb_method_entry_t *
rb_method_entry_set(VALUE klass, ID mid, const rb_method_entry_t *me, rb_method_flag_t noex)
{
    rb_method_type_t type = me->def ? me->def->type : VM_METHOD_TYPE_UNDEF;
    rb_method_entry_t *newme = rb_method_entry_make(klass, mid, type, me->def, noex);
    method_added(klass, mid);
    return newme;
}

void
rb_define_alloc_func(VALUE klass, VALUE (*func)(VALUE))
{
    Check_Type(klass, T_CLASS);
    rb_add_method_cfunc(rb_singleton_class(klass), ID_ALLOCATOR,
			func, 0, NOEX_PRIVATE);
}

void
rb_undef_alloc_func(VALUE klass)
{
    Check_Type(klass, T_CLASS);
    rb_add_method(rb_singleton_class(klass), ID_ALLOCATOR, VM_METHOD_TYPE_UNDEF, 0, NOEX_UNDEF);
}

rb_alloc_func_t
rb_get_alloc_func(VALUE klass)
{
    rb_method_entry_t *me;
    Check_Type(klass, T_CLASS);
    me = rb_method_entry(CLASS_OF(klass), ID_ALLOCATOR);

    if (me && me->def && me->def->type == VM_METHOD_TYPE_CFUNC) {
	return (rb_alloc_func_t)me->def->body.cfunc.func;
    }
    else {
	return 0;
    }
}

static rb_method_entry_t*
search_method(VALUE klass, ID id)
{
    st_data_t body;
    if (!klass) {
	return 0;
    }

    while (!st_lookup(RCLASS_M_TBL(klass), id, &body)) {
	klass = RCLASS_SUPER(klass);
	if (!klass) {
	    return 0;
	}
    }

    return (rb_method_entry_t *)body;
}

/*
 * search method entry without the method cache.
 *
 * if you need method entry with method cache (normal case), use
 * rb_method_entry() simply.
 */
rb_method_entry_t *
rb_method_entry_get_without_cache(VALUE klass, ID id)
{
    rb_method_entry_t *me = search_method(klass, id);

    if (ruby_running) {
	struct cache_entry *ent;
	ent = cache + EXPR1(klass, id);
	ent->filled_version = GET_VM_STATE_VERSION();
	ent->klass = klass;

	if (UNDEFINED_METHOD_ENTRY_P(me)) {
	    ent->mid = id;
	    ent->me = 0;
	    me = 0;
	}
	else {
	    ent->mid = id;
	    ent->me = me;
	}
    }

    return me;
}

rb_method_entry_t *
rb_method_entry(VALUE klass, ID id)
{
    struct cache_entry *ent;

    ent = cache + EXPR1(klass, id);
    if (ent->filled_version == GET_VM_STATE_VERSION() &&
	ent->mid == id && ent->klass == klass) {
	return ent->me;
    }

    return rb_method_entry_get_without_cache(klass, id);
}

static void
remove_method(VALUE klass, ID mid)
{
    st_data_t key, data;
    rb_method_entry_t *me = 0;

    if (klass == rb_cObject) {
	rb_secure(4);
    }
    if (rb_safe_level() >= 4 && !OBJ_UNTRUSTED(klass)) {
	rb_raise(rb_eSecurityError, "Insecure: can't remove method");
    }
    rb_check_frozen(klass);
    if (mid == object_id || mid == id__send__ || mid == idInitialize) {
	rb_warn("removing `%s' may cause serious problems", rb_id2name(mid));
    }

    if (!st_lookup(RCLASS_M_TBL(klass), mid, &data) ||
	!(me = (rb_method_entry_t *)data) ||
	(!me->def || me->def->type == VM_METHOD_TYPE_UNDEF)) {
	rb_name_error(mid, "method `%s' not defined in %s",
		      rb_id2name(mid), rb_class2name(klass));
    }
    key = (st_data_t)mid;
    st_delete(RCLASS_M_TBL(klass), &key, &data);

    rb_vm_check_redefinition_opt_method(me, klass);
    rb_clear_cache_for_undef(klass, mid);
    rb_unlink_method_entry(me);

    CALL_METHOD_HOOK(klass, removed, mid);
}

void
rb_remove_method_id(VALUE klass, ID mid)
{
    remove_method(klass, mid);
}

void
rb_remove_method(VALUE klass, const char *name)
{
    remove_method(klass, rb_intern(name));
}

/*
 *  call-seq:
 *     remove_method(symbol)   -> self
 *
 *  Removes the method identified by _symbol_ from the current
 *  class. For an example, see <code>Module.undef_method</code>.
 */

static VALUE
rb_mod_remove_method(int argc, VALUE *argv, VALUE mod)
{
    int i;

    for (i = 0; i < argc; i++) {
	VALUE v = argv[i];
	ID id = rb_check_id(&v);
	if (!id) {
	    rb_name_error_str(v, "method `%s' not defined in %s",
			      RSTRING_PTR(v), rb_class2name(mod));
	}
	remove_method(mod, id);
    }
    return mod;
}

#undef rb_disable_super
#undef rb_enable_super

void
rb_disable_super(VALUE klass, const char *name)
{
    /* obsolete - no use */
}

void
rb_enable_super(VALUE klass, const char *name)
{
    rb_warning("rb_enable_super() is obsolete");
}

static void
rb_export_method(VALUE klass, ID name, rb_method_flag_t noex)
{
    rb_method_entry_t *me;

    if (klass == rb_cObject) {
	rb_secure(4);
    }

    me = search_method(klass, name);
    if (!me && RB_TYPE_P(klass, T_MODULE)) {
	me = search_method(rb_cObject, name);
    }

    if (UNDEFINED_METHOD_ENTRY_P(me)) {
	rb_print_undef(klass, name, 0);
    }

    if (me->flag != noex) {
	rb_vm_check_redefinition_opt_method(me, klass);

	if (klass == me->klass) {
	    me->flag = noex;
	}
	else {
	    rb_add_method(klass, name, VM_METHOD_TYPE_ZSUPER, 0, noex);
	}
    }
}

int
rb_method_boundp(VALUE klass, ID id, int ex)
{
    rb_method_entry_t *me = rb_method_entry(klass, id);

    if (me != 0) {
	if ((ex & ~NOEX_RESPONDS) &&
	    ((me->flag & NOEX_PRIVATE) ||
	     ((ex & NOEX_RESPONDS) && (me->flag & NOEX_PROTECTED)))) {
	    return 0;
	}
	if (!me->def) return 0;
	if (me->def->type == VM_METHOD_TYPE_NOTIMPLEMENTED) {
	    if (ex & NOEX_RESPONDS) return 2;
	    return 0;
	}
	return 1;
    }
    return 0;
}

void
rb_attr(VALUE klass, ID id, int read, int write, int ex)
{
    const char *name;
    ID attriv;
    VALUE aname;
    rb_method_flag_t noex;

    if (!ex) {
	noex = NOEX_PUBLIC;
    }
    else {
	if (SCOPE_TEST(NOEX_PRIVATE)) {
	    noex = NOEX_PRIVATE;
	    rb_warning((SCOPE_CHECK(NOEX_MODFUNC)) ?
		       "attribute accessor as module_function" :
		       "private attribute?");
	}
	else if (SCOPE_TEST(NOEX_PROTECTED)) {
	    noex = NOEX_PROTECTED;
	}
	else {
	    noex = NOEX_PUBLIC;
	}
    }

    if (!rb_is_local_id(id) && !rb_is_const_id(id)) {
	rb_name_error(id, "invalid attribute name `%s'", rb_id2name(id));
    }
    name = rb_id2name(id);
    if (!name) {
	rb_raise(rb_eArgError, "argument needs to be symbol or string");
    }
    aname = rb_sprintf("@%s", name);
    rb_enc_copy(aname, rb_id2str(id));
    attriv = rb_intern_str(aname);
    if (read) {
	rb_add_method(klass, id, VM_METHOD_TYPE_IVAR, (void *)attriv, noex);
    }
    if (write) {
	rb_add_method(klass, rb_id_attrset(id), VM_METHOD_TYPE_ATTRSET, (void *)attriv, noex);
    }
}

void
rb_undef(VALUE klass, ID id)
{
    rb_method_entry_t *me;

    if (NIL_P(klass)) {
	rb_raise(rb_eTypeError, "no class to undef method");
    }
    if (rb_vm_cbase() == rb_cObject && klass == rb_cObject) {
	rb_secure(4);
    }
    if (rb_safe_level() >= 4 && !OBJ_UNTRUSTED(klass)) {
	rb_raise(rb_eSecurityError, "Insecure: can't undef `%s'", rb_id2name(id));
    }
    rb_frozen_class_p(klass);
    if (id == object_id || id == id__send__ || id == idInitialize) {
	rb_warn("undefining `%s' may cause serious problems", rb_id2name(id));
    }

    me = search_method(klass, id);

    if (UNDEFINED_METHOD_ENTRY_P(me)) {
	const char *s0 = " class";
	VALUE c = klass;

	if (FL_TEST(c, FL_SINGLETON)) {
	    VALUE obj = rb_ivar_get(klass, attached);

	    switch (TYPE(obj)) {
	      case T_MODULE:
	      case T_CLASS:
		c = obj;
		s0 = "";
	    }
	}
	else if (RB_TYPE_P(c, T_MODULE)) {
	    s0 = " module";
	}
	rb_name_error(id, "undefined method `%s' for%s `%s'",
		      rb_id2name(id), s0, rb_class2name(c));
    }

    rb_add_method(klass, id, VM_METHOD_TYPE_UNDEF, 0, NOEX_PUBLIC);

    CALL_METHOD_HOOK(klass, undefined, id);
}

/*
 *  call-seq:
 *     undef_method(symbol)    -> self
 *
 *  Prevents the current class from responding to calls to the named
 *  method. Contrast this with <code>remove_method</code>, which deletes
 *  the method from the particular class; Ruby will still search
 *  superclasses and mixed-in modules for a possible receiver.
 *
 *     class Parent
 *       def hello
 *         puts "In parent"
 *       end
 *     end
 *     class Child < Parent
 *       def hello
 *         puts "In child"
 *       end
 *     end
 *
 *
 *     c = Child.new
 *     c.hello
 *
 *
 *     class Child
 *       remove_method :hello  # remove from child, still in parent
 *     end
 *     c.hello
 *
 *
 *     class Child
 *       undef_method :hello   # prevent any calls to 'hello'
 *     end
 *     c.hello
 *
 *  <em>produces:</em>
 *
 *     In child
 *     In parent
 *     prog.rb:23: undefined method `hello' for #<Child:0x401b3bb4> (NoMethodError)
 */

static VALUE
rb_mod_undef_method(int argc, VALUE *argv, VALUE mod)
{
    int i;
    for (i = 0; i < argc; i++) {
	VALUE v = argv[i];
	ID id = rb_check_id(&v);
	if (!id) {
	    rb_method_name_error(mod, v);
	}
	rb_undef(mod, id);
    }
    return mod;
}

/*
 *  call-seq:
 *     mod.method_defined?(symbol)    -> true or false
 *
 *  Returns +true+ if the named method is defined by
 *  _mod_ (or its included modules and, if _mod_ is a class,
 *  its ancestors). Public and protected methods are matched.
 *
 *     module A
 *       def method1()  end
 *     end
 *     class B
 *       def method2()  end
 *     end
 *     class C < B
 *       include A
 *       def method3()  end
 *     end
 *
 *     A.method_defined? :method1    #=> true
 *     C.method_defined? "method1"   #=> true
 *     C.method_defined? "method2"   #=> true
 *     C.method_defined? "method3"   #=> true
 *     C.method_defined? "method4"   #=> false
 */

static VALUE
rb_mod_method_defined(VALUE mod, VALUE mid)
{
    ID id = rb_check_id(&mid);
    if (!id || !rb_method_boundp(mod, id, 1)) {
	return Qfalse;
    }
    return Qtrue;

}

#define VISI_CHECK(x,f) (((x)&NOEX_MASK) == (f))

static VALUE
check_definition(VALUE mod, ID mid, rb_method_flag_t noex)
{
    const rb_method_entry_t *me;
    me = rb_method_entry(mod, mid);
    if (me) {
	if (VISI_CHECK(me->flag, noex))
	    return Qtrue;
    }
    return Qfalse;
}

/*
 *  call-seq:
 *     mod.public_method_defined?(symbol)   -> true or false
 *
 *  Returns +true+ if the named public method is defined by
 *  _mod_ (or its included modules and, if _mod_ is a class,
 *  its ancestors).
 *
 *     module A
 *       def method1()  end
 *     end
 *     class B
 *       protected
 *       def method2()  end
 *     end
 *     class C < B
 *       include A
 *       def method3()  end
 *     end
 *
 *     A.method_defined? :method1           #=> true
 *     C.public_method_defined? "method1"   #=> true
 *     C.public_method_defined? "method2"   #=> false
 *     C.method_defined? "method2"          #=> true
 */

static VALUE
rb_mod_public_method_defined(VALUE mod, VALUE mid)
{
    ID id = rb_check_id(&mid);
    if (!id) return Qfalse;
    return check_definition(mod, id, NOEX_PUBLIC);
}

/*
 *  call-seq:
 *     mod.private_method_defined?(symbol)    -> true or false
 *
 *  Returns +true+ if the named private method is defined by
 *  _ mod_ (or its included modules and, if _mod_ is a class,
 *  its ancestors).
 *
 *     module A
 *       def method1()  end
 *     end
 *     class B
 *       private
 *       def method2()  end
 *     end
 *     class C < B
 *       include A
 *       def method3()  end
 *     end
 *
 *     A.method_defined? :method1            #=> true
 *     C.private_method_defined? "method1"   #=> false
 *     C.private_method_defined? "method2"   #=> true
 *     C.method_defined? "method2"           #=> false
 */

static VALUE
rb_mod_private_method_defined(VALUE mod, VALUE mid)
{
    ID id = rb_check_id(&mid);
    if (!id) return Qfalse;
    return check_definition(mod, id, NOEX_PRIVATE);
}

/*
 *  call-seq:
 *     mod.protected_method_defined?(symbol)   -> true or false
 *
 *  Returns +true+ if the named protected method is defined
 *  by _mod_ (or its included modules and, if _mod_ is a
 *  class, its ancestors).
 *
 *     module A
 *       def method1()  end
 *     end
 *     class B
 *       protected
 *       def method2()  end
 *     end
 *     class C < B
 *       include A
 *       def method3()  end
 *     end
 *
 *     A.method_defined? :method1              #=> true
 *     C.protected_method_defined? "method1"   #=> false
 *     C.protected_method_defined? "method2"   #=> true
 *     C.method_defined? "method2"             #=> true
 */

static VALUE
rb_mod_protected_method_defined(VALUE mod, VALUE mid)
{
    ID id = rb_check_id(&mid);
    if (!id) return Qfalse;
    return check_definition(mod, id, NOEX_PROTECTED);
}

int
rb_method_entry_eq(const rb_method_entry_t *m1, const rb_method_entry_t *m2)
{
    return rb_method_definition_eq(m1->def, m2->def);
}

static int
rb_method_definition_eq(const rb_method_definition_t *d1, const rb_method_definition_t *d2)
{
    if (d1 == d2) return 1;
    if (!d1 || !d2) return 0;
    if (d1->type != d2->type) {
	return 0;
    }
    switch (d1->type) {
      case VM_METHOD_TYPE_ISEQ:
	return d1->body.iseq == d2->body.iseq;
      case VM_METHOD_TYPE_CFUNC:
	return
	  d1->body.cfunc.func == d2->body.cfunc.func &&
	  d1->body.cfunc.argc == d2->body.cfunc.argc;
      case VM_METHOD_TYPE_ATTRSET:
      case VM_METHOD_TYPE_IVAR:
	return d1->body.attr.id == d2->body.attr.id;
      case VM_METHOD_TYPE_BMETHOD:
	return RTEST(rb_equal(d1->body.proc, d2->body.proc));
      case VM_METHOD_TYPE_MISSING:
	return d1->original_id == d2->original_id;
      case VM_METHOD_TYPE_ZSUPER:
      case VM_METHOD_TYPE_NOTIMPLEMENTED:
      case VM_METHOD_TYPE_UNDEF:
	return 1;
      case VM_METHOD_TYPE_OPTIMIZED:
	return d1->body.optimize_type == d2->body.optimize_type;
      default:
	rb_bug("rb_method_entry_eq: unsupported method type (%d)\n", d1->type);
	return 0;
    }
}

static st_index_t
rb_hash_method_definition(st_index_t hash, const rb_method_definition_t *def)
{
    hash = rb_hash_uint(hash, def->type);
    switch (def->type) {
      case VM_METHOD_TYPE_ISEQ:
	return rb_hash_uint(hash, (st_index_t)def->body.iseq);
      case VM_METHOD_TYPE_CFUNC:
	hash = rb_hash_uint(hash, (st_index_t)def->body.cfunc.func);
	return rb_hash_uint(hash, def->body.cfunc.argc);
      case VM_METHOD_TYPE_ATTRSET:
      case VM_METHOD_TYPE_IVAR:
	return rb_hash_uint(hash, def->body.attr.id);
      case VM_METHOD_TYPE_BMETHOD:
	return rb_hash_proc(hash, def->body.proc);
      case VM_METHOD_TYPE_MISSING:
	return rb_hash_uint(hash, def->original_id);
      case VM_METHOD_TYPE_ZSUPER:
      case VM_METHOD_TYPE_NOTIMPLEMENTED:
      case VM_METHOD_TYPE_UNDEF:
	return hash;
      case VM_METHOD_TYPE_OPTIMIZED:
	return rb_hash_uint(hash, def->body.optimize_type);
      default:
	rb_bug("rb_hash_method_definition: unsupported method type (%d)\n", def->type);
    }
    return hash;
}

st_index_t
rb_hash_method_entry(st_index_t hash, const rb_method_entry_t *me)
{
    return rb_hash_method_definition(hash, me->def);
}

void
rb_alias(VALUE klass, ID name, ID def)
{
    VALUE target_klass = klass;
    rb_method_entry_t *orig_me;
    rb_method_flag_t flag = NOEX_UNDEF;

    if (NIL_P(klass)) {
	rb_raise(rb_eTypeError, "no class to make alias");
    }

    rb_frozen_class_p(klass);
    if (klass == rb_cObject) {
	rb_secure(4);
    }

  again:
    orig_me = search_method(klass, def);

    if (UNDEFINED_METHOD_ENTRY_P(orig_me)) {
	if ((!RB_TYPE_P(klass, T_MODULE)) ||
	    (orig_me = search_method(rb_cObject, def), UNDEFINED_METHOD_ENTRY_P(orig_me))) {
	    rb_print_undef(klass, def, 0);
	}
    }
    if (orig_me->def->type == VM_METHOD_TYPE_ZSUPER) {
	klass = RCLASS_SUPER(klass);
	def = orig_me->def->original_id;
	flag = orig_me->flag;
	goto again;
    }

    if (flag == NOEX_UNDEF) flag = orig_me->flag;
    rb_method_entry_set(target_klass, name, orig_me, flag);
}

/*
 *  call-seq:
 *     alias_method(new_name, old_name)   -> self
 *
 *  Makes <i>new_name</i> a new copy of the method <i>old_name</i>. This can
 *  be used to retain access to methods that are overridden.
 *
 *     module Mod
 *       alias_method :orig_exit, :exit
 *       def exit(code=0)
 *         puts "Exiting with code #{code}"
 *         orig_exit(code)
 *       end
 *     end
 *     include Mod
 *     exit(99)
 *
 *  <em>produces:</em>
 *
 *     Exiting with code 99
 */

static VALUE
rb_mod_alias_method(VALUE mod, VALUE newname, VALUE oldname)
{
    ID oldid = rb_check_id(&oldname);
    if (!oldid) {
	rb_print_undef_str(mod, oldname);
    }
    rb_alias(mod, rb_to_id(newname), oldid);
    return mod;
}

static void
secure_visibility(VALUE self)
{
    if (rb_safe_level() >= 4 && !OBJ_UNTRUSTED(self)) {
	rb_raise(rb_eSecurityError,
		 "Insecure: can't change method visibility");
    }
}

static void
set_method_visibility(VALUE self, int argc, VALUE *argv, rb_method_flag_t ex)
{
    int i;
    secure_visibility(self);

    if (argc == 0) {
	rb_warning("%s with no argument is just ignored", rb_id2name(rb_frame_callee()));
    }

    for (i = 0; i < argc; i++) {
	VALUE v = argv[i];
	ID id = rb_check_id(&v);
	if (!id) {
	    rb_print_undef_str(self, v);
	}
	rb_export_method(self, id, ex);
    }
    rb_clear_cache_by_class(self);
}

/*
 *  call-seq:
 *     public                 -> self
 *     public(symbol, ...)    -> self
 *
 *  With no arguments, sets the default visibility for subsequently
 *  defined methods to public. With arguments, sets the named methods to
 *  have public visibility.
 */

static VALUE
rb_mod_public(int argc, VALUE *argv, VALUE module)
{
    secure_visibility(module);
    if (argc == 0) {
	SCOPE_SET(NOEX_PUBLIC);
    }
    else {
	set_method_visibility(module, argc, argv, NOEX_PUBLIC);
    }
    return module;
}

/*
 *  call-seq:
 *     protected                -> self
 *     protected(symbol, ...)   -> self
 *
 *  With no arguments, sets the default visibility for subsequently
 *  defined methods to protected. With arguments, sets the named methods
 *  to have protected visibility.
 */

static VALUE
rb_mod_protected(int argc, VALUE *argv, VALUE module)
{
    secure_visibility(module);
    if (argc == 0) {
	SCOPE_SET(NOEX_PROTECTED);
    }
    else {
	set_method_visibility(module, argc, argv, NOEX_PROTECTED);
    }
    return module;
}

/*
 *  call-seq:
 *     private                 -> self
 *     private(symbol, ...)    -> self
 *
 *  With no arguments, sets the default visibility for subsequently
 *  defined methods to private. With arguments, sets the named methods
 *  to have private visibility.
 *
 *     module Mod
 *       def a()  end
 *       def b()  end
 *       private
 *       def c()  end
 *       private :a
 *     end
 *     Mod.private_instance_methods   #=> [:a, :c]
 */

static VALUE
rb_mod_private(int argc, VALUE *argv, VALUE module)
{
    secure_visibility(module);
    if (argc == 0) {
	SCOPE_SET(NOEX_PRIVATE);
    }
    else {
	set_method_visibility(module, argc, argv, NOEX_PRIVATE);
    }
    return module;
}

/*
 *  call-seq:
 *     mod.public_class_method(symbol, ...)    -> mod
 *
 *  Makes a list of existing class methods public.
 */

static VALUE
rb_mod_public_method(int argc, VALUE *argv, VALUE obj)
{
    set_method_visibility(CLASS_OF(obj), argc, argv, NOEX_PUBLIC);
    return obj;
}

/*
 *  call-seq:
 *     mod.private_class_method(symbol, ...)   -> mod
 *
 *  Makes existing class methods private. Often used to hide the default
 *  constructor <code>new</code>.
 *
 *     class SimpleSingleton  # Not thread safe
 *       private_class_method :new
 *       def SimpleSingleton.create(*args, &block)
 *         @me = new(*args, &block) if ! @me
 *         @me
 *       end
 *     end
 */

static VALUE
rb_mod_private_method(int argc, VALUE *argv, VALUE obj)
{
    set_method_visibility(CLASS_OF(obj), argc, argv, NOEX_PRIVATE);
    return obj;
}

/*
 *  call-seq:
 *     public
 *     public(symbol, ...)
 *
 *  With no arguments, sets the default visibility for subsequently
 *  defined methods to public. With arguments, sets the named methods to
 *  have public visibility.
 */

static VALUE
top_public(int argc, VALUE *argv)
{
    return rb_mod_public(argc, argv, rb_cObject);
}

static VALUE
top_private(int argc, VALUE *argv)
{
    return rb_mod_private(argc, argv, rb_cObject);
}

/*
 *  call-seq:
 *     module_function(symbol, ...)    -> self
 *
 *  Creates module functions for the named methods. These functions may
 *  be called with the module as a receiver, and also become available
 *  as instance methods to classes that mix in the module. Module
 *  functions are copies of the original, and so may be changed
 *  independently. The instance-method versions are made private. If
 *  used with no arguments, subsequently defined methods become module
 *  functions.
 *
 *     module Mod
 *       def one
 *         "This is one"
 *       end
 *       module_function :one
 *     end
 *     class Cls
 *       include Mod
 *       def call_one
 *         one
 *       end
 *     end
 *     Mod.one     #=> "This is one"
 *     c = Cls.new
 *     c.call_one  #=> "This is one"
 *     module Mod
 *       def one
 *         "This is the new one"
 *       end
 *     end
 *     Mod.one     #=> "This is one"
 *     c.call_one  #=> "This is the new one"
 */

static VALUE
rb_mod_modfunc(int argc, VALUE *argv, VALUE module)
{
    int i;
    ID id;
    const rb_method_entry_t *me;

    if (!RB_TYPE_P(module, T_MODULE)) {
	rb_raise(rb_eTypeError, "module_function must be called for modules");
    }

    secure_visibility(module);
    if (argc == 0) {
	SCOPE_SET(NOEX_MODFUNC);
	return module;
    }

    set_method_visibility(module, argc, argv, NOEX_PRIVATE);

    for (i = 0; i < argc; i++) {
	VALUE m = module;

	id = rb_to_id(argv[i]);
	for (;;) {
	    me = search_method(m, id);
	    if (me == 0) {
		me = search_method(rb_cObject, id);
	    }
	    if (UNDEFINED_METHOD_ENTRY_P(me)) {
		rb_print_undef(module, id, 0);
	    }
	    if (me->def->type != VM_METHOD_TYPE_ZSUPER) {
		break; /* normal case: need not to follow 'super' link */
	    }
	    m = RCLASS_SUPER(m);
	    if (!m)
		break;
	}
	rb_method_entry_set(rb_singleton_class(module), id, me, NOEX_PUBLIC);
    }
    return module;
}

int
rb_method_basic_definition_p(VALUE klass, ID id)
{
    const rb_method_entry_t *me = rb_method_entry(klass, id);
    if (me && (me->flag & NOEX_BASIC))
	return 1;
    return 0;
}

static inline int
basic_obj_respond_to(VALUE obj, ID id, int pub)
{
    VALUE klass = CLASS_OF(obj);

    switch (rb_method_boundp(klass, id, pub|NOEX_RESPONDS)) {
      case 2:
	return FALSE;
      case 0:
	return RTEST(rb_funcall(obj, respond_to_missing, 2, ID2SYM(id), pub ? Qfalse : Qtrue));
      default:
	return TRUE;
    }
}

int
rb_obj_respond_to(VALUE obj, ID id, int priv)
{
    VALUE klass = CLASS_OF(obj);

    if (rb_method_basic_definition_p(klass, idRespond_to)) {
	return basic_obj_respond_to(obj, id, !RTEST(priv));
    }
    else {
	return RTEST(rb_funcall(obj, idRespond_to, priv ? 2 : 1, ID2SYM(id), Qtrue));
    }
}

int
rb_respond_to(VALUE obj, ID id)
{
    return rb_obj_respond_to(obj, id, FALSE);
}


/*
 *  call-seq:
 *     obj.respond_to?(symbol, include_all=false) -> true or false
 *
 *  Returns +true+ if _obj_ responds to the given method.  Private and
 *  protected methods are included in the search only if the optional
 *  second parameter evaluates to +true+.
 *
 *  If the method is not implemented,
 *  as Process.fork on Windows, File.lchmod on GNU/Linux, etc.,
 *  false is returned.
 *
 *  If the method is not defined, <code>respond_to_missing?</code>
 *  method is called and the result is returned.
 */

static VALUE
obj_respond_to(int argc, VALUE *argv, VALUE obj)
{
    VALUE mid, priv;
    ID id;

    rb_scan_args(argc, argv, "11", &mid, &priv);
    if (!(id = rb_check_id(&mid))) {
	if (!rb_method_basic_definition_p(CLASS_OF(obj), respond_to_missing)) {
	    VALUE args[2];
	    args[0] = ID2SYM(rb_to_id(mid));
	    args[1] = priv;
	    return rb_funcall2(obj, respond_to_missing, 2, args);
	}
	return Qfalse;
    }
    if (basic_obj_respond_to(obj, id, !RTEST(priv)))
	return Qtrue;
    return Qfalse;
}

/*
 *  call-seq:
 *     obj.respond_to_missing?(symbol, include_all) -> true or false
 *
 *  Hook method to return whether the _obj_ can respond to _id_ method
 *  or not.
 *
 *  See #respond_to?.
 */
static VALUE
obj_respond_to_missing(VALUE obj, VALUE mid, VALUE priv)
{
    return Qfalse;
}

void
Init_eval_method(void)
{
#undef rb_intern
#define rb_intern(str) rb_intern_const(str)

    rb_define_method(rb_mKernel, "respond_to?", obj_respond_to, -1);
    rb_define_method(rb_mKernel, "respond_to_missing?", obj_respond_to_missing, 2);

    rb_define_private_method(rb_cModule, "remove_method", rb_mod_remove_method, -1);
    rb_define_private_method(rb_cModule, "undef_method", rb_mod_undef_method, -1);
    rb_define_private_method(rb_cModule, "alias_method", rb_mod_alias_method, 2);
    rb_define_private_method(rb_cModule, "public", rb_mod_public, -1);
    rb_define_private_method(rb_cModule, "protected", rb_mod_protected, -1);
    rb_define_private_method(rb_cModule, "private", rb_mod_private, -1);
    rb_define_private_method(rb_cModule, "module_function", rb_mod_modfunc, -1);

    rb_define_method(rb_cModule, "method_defined?", rb_mod_method_defined, 1);
    rb_define_method(rb_cModule, "public_method_defined?", rb_mod_public_method_defined, 1);
    rb_define_method(rb_cModule, "private_method_defined?", rb_mod_private_method_defined, 1);
    rb_define_method(rb_cModule, "protected_method_defined?", rb_mod_protected_method_defined, 1);
    rb_define_method(rb_cModule, "public_class_method", rb_mod_public_method, -1);
    rb_define_method(rb_cModule, "private_class_method", rb_mod_private_method, -1);

    rb_define_singleton_method(rb_vm_top_self(), "public", top_public, -1);
    rb_define_singleton_method(rb_vm_top_self(), "private", top_private, -1);

    object_id = rb_intern("object_id");
    added = rb_intern("method_added");
    singleton_added = rb_intern("singleton_method_added");
    removed = rb_intern("method_removed");
    singleton_removed = rb_intern("singleton_method_removed");
    undefined = rb_intern("method_undefined");
    singleton_undefined = rb_intern("singleton_method_undefined");
    attached = rb_intern("__attached__");
    respond_to_missing = rb_intern("respond_to_missing?");

    {
#define REPLICATE_METHOD(klass, id, noex) \
	rb_method_entry_set((klass), (id), rb_method_entry((klass), (id)), \
			    (rb_method_flag_t)(noex | NOEX_BASIC | NOEX_NOREDEF))
	REPLICATE_METHOD(rb_eException, idMethodMissing, NOEX_PRIVATE);
	REPLICATE_METHOD(rb_eException, idRespond_to, NOEX_PUBLIC);
	REPLICATE_METHOD(rb_eException, respond_to_missing, NOEX_PUBLIC);
    }
}

