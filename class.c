/**********************************************************************

  class.c -

  $Author$
  created at: Tue Aug 10 15:05:44 JST 1993

  Copyright (C) 1993-2007 Yukihiro Matsumoto

**********************************************************************/

/*!
 * \defgroup class Classes and their hierarchy.
 * \par Terminology
 * - class: same as in Ruby.
 * - singleton class: class for a particular object
 * - eigenclass: = singleton class
 * - metaclass: class of a class. metaclass is a kind of singleton class.
 * - metametaclass: class of a metaclass.
 * - meta^(n)-class: class of a meta^(n-1)-class.
 * - attached object: A singleton class knows its unique instance.
 *   The instance is called the attached object for the singleton class.
 * \{
 */

#include "ruby/ruby.h"
#include "ruby/st.h"
#include "method.h"
#include "constant.h"
#include "vm_core.h"
#include "internal.h"
#include <ctype.h>

extern st_table *rb_class_tbl;
static ID id_attached;

/**
 * Allocates a struct RClass for a new class.
 *
 * \param flags     initial value for basic.flags of the returned class.
 * \param klass     the class of the returned class.
 * \return          an uninitialized Class object.
 * \pre  \p klass must refer \c Class class or an ancestor of Class.
 * \pre  \code (flags | T_CLASS) != 0  \endcode
 * \post the returned class can safely be \c #initialize 'd.
 *
 * \note this function is not Class#allocate.
 */
static VALUE
class_alloc(VALUE flags, VALUE klass)
{
    NEWOBJ(obj, struct RClass);
    OBJSETUP(obj, klass, flags);
    obj->ptr = ALLOC(rb_classext_t);
    RCLASS_IV_TBL(obj) = 0;
    RCLASS_CONST_TBL(obj) = 0;
    RCLASS_M_TBL(obj) = 0;
    RCLASS_SUPER(obj) = 0;
    RCLASS_IV_INDEX_TBL(obj) = 0;
    return (VALUE)obj;
}


/*!
 * A utility function that wraps class_alloc.
 *
 * allocates a class and initializes safely.
 * \param super     a class from which the new class derives.
 * \return          a class object.
 * \pre  \a super must be a class.
 * \post the metaclass of the new class is Class.
 */
VALUE
rb_class_boot(VALUE super)
{
    VALUE klass = class_alloc(T_CLASS, rb_cClass);

    RCLASS_SUPER(klass) = super;
    RCLASS_M_TBL(klass) = st_init_numtable();

    OBJ_INFECT(klass, super);
    return (VALUE)klass;
}


/*!
 * Ensures a class can be derived from super.
 *
 * \param super a reference to an object.
 * \exception TypeError if \a super is not a Class or \a super is a singleton class.
 */
void
rb_check_inheritable(VALUE super)
{
    if (!RB_TYPE_P(super, T_CLASS)) {
	rb_raise(rb_eTypeError, "superclass must be a Class (%s given)",
		 rb_obj_classname(super));
    }
    if (RBASIC(super)->flags & FL_SINGLETON) {
	rb_raise(rb_eTypeError, "can't make subclass of singleton class");
    }
    if (super == rb_cClass) {
	rb_raise(rb_eTypeError, "can't make subclass of Class");
    }
}


/*!
 * Creates a new class.
 * \param super     a class from which the new class derives.
 * \exception TypeError \a super is not inheritable.
 * \exception TypeError \a super is the Class class.
 */
VALUE
rb_class_new(VALUE super)
{
    Check_Type(super, T_CLASS);
    rb_check_inheritable(super);
    return rb_class_boot(super);
}

static void
rb_mod_clone_method(VALUE klass, ID mid, const rb_method_entry_t *me)
{
    VALUE newiseqval;
    if (me->def && me->def->type == VM_METHOD_TYPE_ISEQ) {
	rb_iseq_t *iseq;
	newiseqval = rb_iseq_clone(me->def->body.iseq->self, klass);
	GetISeqPtr(newiseqval, iseq);
	rb_add_method(klass, mid, VM_METHOD_TYPE_ISEQ, iseq, me->flag);
	RB_GC_GUARD(newiseqval);
    }
    else {
	rb_method_entry_set(klass, mid, me, me->flag);
    }
}

static int
clone_method_i(st_data_t key, st_data_t value, st_data_t data)
{
    rb_mod_clone_method((VALUE)data, (ID)key, (const rb_method_entry_t *)value);
    return ST_CONTINUE;
}

static int
clone_const(ID key, const rb_const_entry_t *ce, st_table *tbl)
{
    rb_const_entry_t *nce = ALLOC(rb_const_entry_t);
    *nce = *ce;
    st_insert(tbl, key, (st_data_t)nce);
    return ST_CONTINUE;
}

static int
clone_const_i(st_data_t key, st_data_t value, st_data_t data)
{
    return clone_const((ID)key, (const rb_const_entry_t *)value, (st_table *)data);
}

/* :nodoc: */
VALUE
rb_mod_init_copy(VALUE clone, VALUE orig)
{
    rb_obj_init_copy(clone, orig);
    if (!FL_TEST(CLASS_OF(clone), FL_SINGLETON)) {
	RBASIC(clone)->klass = rb_singleton_class_clone(orig);
	rb_singleton_class_attached(RBASIC(clone)->klass, (VALUE)clone);
    }
    RCLASS_SUPER(clone) = RCLASS_SUPER(orig);
    if (RCLASS_IV_TBL(orig)) {
	st_data_t id;

	if (RCLASS_IV_TBL(clone)) {
	    st_free_table(RCLASS_IV_TBL(clone));
	}
	RCLASS_IV_TBL(clone) = st_copy(RCLASS_IV_TBL(orig));
	CONST_ID(id, "__classpath__");
	st_delete(RCLASS_IV_TBL(clone), &id, 0);
	CONST_ID(id, "__classid__");
	st_delete(RCLASS_IV_TBL(clone), &id, 0);
    }
    if (RCLASS_CONST_TBL(orig)) {
	if (RCLASS_CONST_TBL(clone)) {
	    rb_free_const_table(RCLASS_CONST_TBL(clone));
	}
	RCLASS_CONST_TBL(clone) = st_init_numtable();
	st_foreach(RCLASS_CONST_TBL(orig), clone_const_i, (st_data_t)RCLASS_CONST_TBL(clone));
    }
    if (RCLASS_M_TBL(orig)) {
	if (RCLASS_M_TBL(clone)) {
	    rb_free_m_table(RCLASS_M_TBL(clone));
	}
	RCLASS_M_TBL(clone) = st_init_numtable();
	st_foreach(RCLASS_M_TBL(orig), clone_method_i, (st_data_t)clone);
    }

    return clone;
}

/* :nodoc: */
VALUE
rb_class_init_copy(VALUE clone, VALUE orig)
{
    if (orig == rb_cBasicObject) {
	rb_raise(rb_eTypeError, "can't copy the root class");
    }
    if (RCLASS_SUPER(clone) != 0 || clone == rb_cBasicObject) {
	rb_raise(rb_eTypeError, "already initialized class");
    }
    if (FL_TEST(orig, FL_SINGLETON)) {
	rb_raise(rb_eTypeError, "can't copy singleton class");
    }
    return rb_mod_init_copy(clone, orig);
}

VALUE
rb_singleton_class_clone(VALUE obj)
{
    VALUE klass = RBASIC(obj)->klass;

    if (!FL_TEST(klass, FL_SINGLETON))
	return klass;
    else {
	/* copy singleton(unnamed) class */
	VALUE clone = class_alloc(RBASIC(klass)->flags, 0);

	if (BUILTIN_TYPE(obj) == T_CLASS) {
	    RBASIC(clone)->klass = clone;
	}
	else {
	    RBASIC(clone)->klass = rb_singleton_class_clone(klass);
	}

	RCLASS_SUPER(clone) = RCLASS_SUPER(klass);
	if (RCLASS_IV_TBL(klass)) {
	    RCLASS_IV_TBL(clone) = st_copy(RCLASS_IV_TBL(klass));
	}
	if (RCLASS_CONST_TBL(klass)) {
	    RCLASS_CONST_TBL(clone) = st_init_numtable();
	    st_foreach(RCLASS_CONST_TBL(klass), clone_const_i, (st_data_t)RCLASS_CONST_TBL(clone));
	}
	RCLASS_M_TBL(clone) = st_init_numtable();
	st_foreach(RCLASS_M_TBL(klass), clone_method_i, (st_data_t)clone);
	rb_singleton_class_attached(RBASIC(clone)->klass, clone);
	FL_SET(clone, FL_SINGLETON);
	return clone;
    }
}

/*!
 * Attach a object to a singleton class.
 * @pre \a klass is the singleton class of \a obj.
 */
void
rb_singleton_class_attached(VALUE klass, VALUE obj)
{
    if (FL_TEST(klass, FL_SINGLETON)) {
	if (!RCLASS_IV_TBL(klass)) {
	    RCLASS_IV_TBL(klass) = st_init_numtable();
	}
	st_insert(RCLASS_IV_TBL(klass), id_attached, obj);
    }
}



#define METACLASS_OF(k) RBASIC(k)->klass

/*!
 * whether k is a meta^(n)-class of Class class
 * @retval 1 if \a k is a meta^(n)-class of Class class (n >= 0)
 * @retval 0 otherwise
 */
#define META_CLASS_OF_CLASS_CLASS_P(k)  (METACLASS_OF(k) == (k))


/*!
 * ensures \a klass belongs to its own eigenclass.
 * @return the eigenclass of \a klass
 * @post \a klass belongs to the returned eigenclass.
 *       i.e. the attached object of the eigenclass is \a klass.
 * @note this macro creates a new eigenclass if necessary.
 */
#define ENSURE_EIGENCLASS(klass) \
 (rb_ivar_get(METACLASS_OF(klass), id_attached) == (klass) ? METACLASS_OF(klass) : make_metaclass(klass))


/*!
 * Creates a metaclass of \a klass
 * \param klass     a class
 * \return          created metaclass for the class
 * \pre \a klass is a Class object
 * \pre \a klass has no singleton class.
 * \post the class of \a klass is the returned class.
 * \post the returned class is meta^(n+1)-class when \a klass is a meta^(n)-klass for n >= 0
 */
static inline VALUE
make_metaclass(VALUE klass)
{
    VALUE super;
    VALUE metaclass = rb_class_boot(Qundef);

    FL_SET(metaclass, FL_SINGLETON);
    rb_singleton_class_attached(metaclass, klass);

    if (META_CLASS_OF_CLASS_CLASS_P(klass)) {
	METACLASS_OF(klass) = METACLASS_OF(metaclass) = metaclass;
    }
    else {
	VALUE tmp = METACLASS_OF(klass); /* for a meta^(n)-class klass, tmp is meta^(n)-class of Class class */
	METACLASS_OF(klass) = metaclass;
	METACLASS_OF(metaclass) = ENSURE_EIGENCLASS(tmp);
    }

    super = RCLASS_SUPER(klass);
    while (RB_TYPE_P(super, T_ICLASS)) super = RCLASS_SUPER(super);
    RCLASS_SUPER(metaclass) = super ? ENSURE_EIGENCLASS(super) : rb_cClass;

    OBJ_INFECT(metaclass, RCLASS_SUPER(metaclass));

    return metaclass;
}

/*!
 * Creates a singleton class for \a obj.
 * \pre \a obj must not a immediate nor a special const.
 * \pre \a obj must not a Class object.
 * \pre \a obj has no singleton class.
 */
static inline VALUE
make_singleton_class(VALUE obj)
{
    VALUE orig_class = RBASIC(obj)->klass;
    VALUE klass = rb_class_boot(orig_class);

    FL_SET(klass, FL_SINGLETON);
    RBASIC(obj)->klass = klass;
    rb_singleton_class_attached(klass, obj);

    METACLASS_OF(klass) = METACLASS_OF(rb_class_real(orig_class));
    return klass;
}


static VALUE
boot_defclass(const char *name, VALUE super)
{
    extern st_table *rb_class_tbl;
    VALUE obj = rb_class_boot(super);
    ID id = rb_intern(name);

    rb_name_class(obj, id);
    st_add_direct(rb_class_tbl, id, obj);
    rb_const_set((rb_cObject ? rb_cObject : obj), id, obj);
    return obj;
}

void
Init_class_hierarchy(void)
{
    id_attached = rb_intern("__attached__");

    rb_cBasicObject = boot_defclass("BasicObject", 0);
    rb_cObject = boot_defclass("Object", rb_cBasicObject);
    rb_cModule = boot_defclass("Module", rb_cObject);
    rb_cClass =  boot_defclass("Class",  rb_cModule);

    rb_const_set(rb_cObject, rb_intern("BasicObject"), rb_cBasicObject);
    RBASIC(rb_cClass)->klass
	= RBASIC(rb_cModule)->klass
	= RBASIC(rb_cObject)->klass
	= RBASIC(rb_cBasicObject)->klass
	= rb_cClass;
}


/*!
 * \internal
 * Creates a new *singleton class* for an object.
 *
 * \pre \a obj has no singleton class.
 * \note DO NOT USE the function in an extension libraries. Use \ref rb_singleton_class.
 * \param obj     An object.
 * \param unused  ignored.
 * \return        The singleton class of the object.
 */
VALUE
rb_make_metaclass(VALUE obj, VALUE unused)
{
    if (BUILTIN_TYPE(obj) == T_CLASS) {
	return make_metaclass(obj);
    }
    else {
	return make_singleton_class(obj);
    }
}


/*!
 * Defines a new class.
 * \param id     ignored
 * \param super  A class from which the new class will derive. NULL means \c Object class.
 * \return       the created class
 * \throw TypeError if super is not a \c Class object.
 *
 * \note the returned class will not be associated with \a id.
 *       You must explicitly set a class name if necessary.
 */
VALUE
rb_define_class_id(ID id, VALUE super)
{
    VALUE klass;

    if (!super) super = rb_cObject;
    klass = rb_class_new(super);
    rb_make_metaclass(klass, RBASIC(super)->klass);

    return klass;
}


/*!
 * Calls Class#inherited.
 * \param super  A class which will be called #inherited.
 *               NULL means Object class.
 * \param klass  A Class object which derived from \a super
 * \return the value \c Class#inherited's returns
 * \pre Each of \a super and \a klass must be a \c Class object.
 */
VALUE
rb_class_inherited(VALUE super, VALUE klass)
{
    ID inherited;
    if (!super) super = rb_cObject;
    CONST_ID(inherited, "inherited");
    return rb_funcall(super, inherited, 1, klass);
}



/*!
 * Defines a top-level class.
 * \param name   name of the class
 * \param super  a class from which the new class will derive.
 *               NULL means \c Object class.
 * \return the created class
 * \throw TypeError if the constant name \a name is already taken but
 *                  the constant is not a \c Class.
 * \throw NameError if the class is already defined but the class can not
 *                  be reopened because its superclass is not \a super.
 * \post top-level constant named \a name refers the returned class.
 *
 * \note if a class named \a name is already defined and its superclass is
 *       \a super, the function just returns the defined class.
 */
VALUE
rb_define_class(const char *name, VALUE super)
{
    VALUE klass;
    ID id;

    id = rb_intern(name);
    if (rb_const_defined(rb_cObject, id)) {
	klass = rb_const_get(rb_cObject, id);
	if (!RB_TYPE_P(klass, T_CLASS)) {
	    rb_raise(rb_eTypeError, "%s is not a class", name);
	}
	if (rb_class_real(RCLASS_SUPER(klass)) != super) {
	    rb_raise(rb_eTypeError, "superclass mismatch for class %s", name);
	}
	return klass;
    }
    if (!super) {
	rb_warn("no super class for `%s', Object assumed", name);
    }
    klass = rb_define_class_id(id, super);
    st_add_direct(rb_class_tbl, id, klass);
    rb_name_class(klass, id);
    rb_const_set(rb_cObject, id, klass);
    rb_class_inherited(super, klass);

    return klass;
}


/*!
 * Defines a class under the namespace of \a outer.
 * \param outer  a class which contains the new class.
 * \param name   name of the new class
 * \param super  a class from which the new class will derive.
 *               NULL means \c Object class.
 * \return the created class
 * \throw TypeError if the constant name \a name is already taken but
 *                  the constant is not a \c Class.
 * \throw NameError if the class is already defined but the class can not
 *                  be reopened because its superclass is not \a super.
 * \post top-level constant named \a name refers the returned class.
 *
 * \note if a class named \a name is already defined and its superclass is
 *       \a super, the function just returns the defined class.
 */
VALUE
rb_define_class_under(VALUE outer, const char *name, VALUE super)
{
    return rb_define_class_id_under(outer, rb_intern(name), super);
}


/*!
 * Defines a class under the namespace of \a outer.
 * \param outer  a class which contains the new class.
 * \param id     name of the new class
 * \param super  a class from which the new class will derive.
 *               NULL means \c Object class.
 * \return the created class
 * \throw TypeError if the constant name \a name is already taken but
 *                  the constant is not a \c Class.
 * \throw NameError if the class is already defined but the class can not
 *                  be reopened because its superclass is not \a super.
 * \post top-level constant named \a name refers the returned class.
 *
 * \note if a class named \a name is already defined and its superclass is
 *       \a super, the function just returns the defined class.
 */
VALUE
rb_define_class_id_under(VALUE outer, ID id, VALUE super)
{
    VALUE klass;

    if (rb_const_defined_at(outer, id)) {
	klass = rb_const_get_at(outer, id);
	if (!RB_TYPE_P(klass, T_CLASS)) {
	    rb_raise(rb_eTypeError, "%s is not a class", rb_id2name(id));
	}
	if (rb_class_real(RCLASS_SUPER(klass)) != super) {
	    rb_name_error(id, "%s is already defined", rb_id2name(id));
	}
	return klass;
    }
    if (!super) {
	rb_warn("no super class for `%s::%s', Object assumed",
		rb_class2name(outer), rb_id2name(id));
    }
    klass = rb_define_class_id(id, super);
    rb_set_class_path_string(klass, outer, rb_id2str(id));
    rb_const_set(outer, id, klass);
    rb_class_inherited(super, klass);
    rb_gc_register_mark_object(klass);

    return klass;
}

VALUE
rb_module_new(void)
{
    VALUE mdl = class_alloc(T_MODULE, rb_cModule);

    RCLASS_M_TBL(mdl) = st_init_numtable();

    return (VALUE)mdl;
}

VALUE
rb_define_module_id(ID id)
{
    VALUE mdl;

    mdl = rb_module_new();
    rb_name_class(mdl, id);

    return mdl;
}

VALUE
rb_define_module(const char *name)
{
    VALUE module;
    ID id;

    id = rb_intern(name);
    if (rb_const_defined(rb_cObject, id)) {
	module = rb_const_get(rb_cObject, id);
	if (RB_TYPE_P(module, T_MODULE))
	    return module;
	rb_raise(rb_eTypeError, "%s is not a module", rb_obj_classname(module));
    }
    module = rb_define_module_id(id);
    st_add_direct(rb_class_tbl, id, module);
    rb_const_set(rb_cObject, id, module);

    return module;
}

VALUE
rb_define_module_under(VALUE outer, const char *name)
{
    return rb_define_module_id_under(outer, rb_intern(name));
}

VALUE
rb_define_module_id_under(VALUE outer, ID id)
{
    VALUE module;

    if (rb_const_defined_at(outer, id)) {
	module = rb_const_get_at(outer, id);
	if (RB_TYPE_P(module, T_MODULE))
	    return module;
	rb_raise(rb_eTypeError, "%s::%s is not a module",
		 rb_class2name(outer), rb_obj_classname(module));
    }
    module = rb_define_module_id(id);
    rb_const_set(outer, id, module);
    rb_set_class_path_string(module, outer, rb_id2str(id));
    rb_gc_register_mark_object(module);

    return module;
}

static VALUE
include_class_new(VALUE module, VALUE super)
{
    VALUE klass = class_alloc(T_ICLASS, rb_cClass);

    if (BUILTIN_TYPE(module) == T_ICLASS) {
	module = RBASIC(module)->klass;
    }
    if (!RCLASS_IV_TBL(module)) {
	RCLASS_IV_TBL(module) = st_init_numtable();
    }
    if (!RCLASS_CONST_TBL(module)) {
	RCLASS_CONST_TBL(module) = st_init_numtable();
    }
    RCLASS_IV_TBL(klass) = RCLASS_IV_TBL(module);
    RCLASS_CONST_TBL(klass) = RCLASS_CONST_TBL(module);
    RCLASS_M_TBL(klass) = RCLASS_M_TBL(module);
    RCLASS_SUPER(klass) = super;
    if (RB_TYPE_P(module, T_ICLASS)) {
	RBASIC(klass)->klass = RBASIC(module)->klass;
    }
    else {
	RBASIC(klass)->klass = module;
    }
    OBJ_INFECT(klass, module);
    OBJ_INFECT(klass, super);

    return (VALUE)klass;
}

void
rb_include_module(VALUE klass, VALUE module)
{
    VALUE p, c;
    int changed = 0;

    rb_frozen_class_p(klass);
    if (!OBJ_UNTRUSTED(klass)) {
	rb_secure(4);
    }

    if (!RB_TYPE_P(module, T_MODULE)) {
	Check_Type(module, T_MODULE);
    }

    OBJ_INFECT(klass, module);
    c = klass;
    while (module) {
	int superclass_seen = FALSE;

	if (RCLASS_M_TBL(klass) == RCLASS_M_TBL(module))
	    rb_raise(rb_eArgError, "cyclic include detected");
	/* ignore if the module included already in superclasses */
	for (p = RCLASS_SUPER(klass); p; p = RCLASS_SUPER(p)) {
	    switch (BUILTIN_TYPE(p)) {
	      case T_ICLASS:
		if (RCLASS_M_TBL(p) == RCLASS_M_TBL(module)) {
		    if (!superclass_seen) {
			c = p;  /* move insertion point */
		    }
		    goto skip;
		}
		break;
	      case T_CLASS:
		superclass_seen = TRUE;
		break;
	    }
	}
	c = RCLASS_SUPER(c) = include_class_new(module, RCLASS_SUPER(c));
	if (RMODULE_M_TBL(module) && RMODULE_M_TBL(module)->num_entries)
	    changed = 1;
      skip:
	module = RCLASS_SUPER(module);
    }
    if (changed) rb_clear_cache();
}

struct mixing_arg {
    st_table *mtbl;
    ID id;
    st_table *aliasing;
    VALUE klass;
};

static int
check_mix_const_i(st_data_t key, st_data_t value, st_data_t arg)
{
    struct mixing_arg *argp = (struct mixing_arg *)arg;
    ID id = (ID)key;
    st_table *aliasing = argp->aliasing;
    st_data_t alias;

    if (!rb_is_const_id(id)) return ST_CONTINUE;
    if (aliasing && st_lookup(aliasing, ID2SYM(id), &alias)) {
	id = rb_to_id(alias);
    }
    if (st_lookup(argp->mtbl, id, NULL)) {
	argp->id = id;
	return ST_STOP;
    }
    return ST_CONTINUE;
}

static int
do_mix_const_i(st_data_t key, st_data_t value, st_data_t arg)
{
    struct mixing_arg *argp = (struct mixing_arg *)arg;
    ID id = (ID)key;
    st_table *aliasing = argp->aliasing;
    st_data_t old, alias;

    if (!rb_is_const_id(id)) return ST_CONTINUE;
    if (aliasing && st_lookup(aliasing, ID2SYM(id), &alias)) {
	id = rb_to_id(alias);
    }
    if (st_lookup(argp->mtbl, id, &old)) {
	argp->id = id;
	return ST_STOP;
    }
    st_insert(argp->mtbl, id, value);
    return ST_CONTINUE;
}

static int
check_mix_method_i(st_data_t key, st_data_t value, st_data_t arg)
{
    struct mixing_arg *argp = (struct mixing_arg *)arg;
    ID id = (ID)key;
    st_table *aliasing = argp->aliasing;
    st_data_t alias;

    if (aliasing && st_lookup(aliasing, ID2SYM(id), &alias)) {
	if (NIL_P(alias)) return ST_CONTINUE;
	id = rb_to_id(alias);
    }
    if (st_lookup(argp->mtbl, id, NULL)) {
	argp->id = id;
	return ST_STOP;
    }
    return ST_CONTINUE;
}

static int
do_mix_method_i(st_data_t key, st_data_t value, st_data_t arg)
{
    struct mixing_arg *argp = (struct mixing_arg *)arg;
    ID id = (ID)key;
    st_table *aliasing = argp->aliasing;
    st_data_t old, alias;

    if (aliasing && st_lookup(aliasing, ID2SYM(id), &alias)) {
	if (NIL_P(alias)) return ST_CONTINUE;
	id = rb_to_id(alias);
    }
    if (st_lookup(argp->mtbl, id, &old)) {
	argp->id = id;
	return ST_STOP;
    }
    rb_mod_clone_method(argp->klass, id, (rb_method_entry_t *)value);
    return ST_CONTINUE;
}

void
rb_mix_module(VALUE klass, VALUE module, st_table *constants, st_table *methods)
{
    st_table *mtbl_from;
    struct mixing_arg methodarg, constarg;

    rb_frozen_class_p(klass);
    if (!OBJ_UNTRUSTED(klass)) {
	rb_secure(4);
    }

    if (!RB_TYPE_P(module, T_MODULE)) {
	Check_Type(module, T_MODULE);
    }

    OBJ_INFECT(klass, module);

    mtbl_from = RMODULE_M_TBL(module);
    methodarg.mtbl = RMODULE_M_TBL(klass);
    methodarg.id = 0;
    methodarg.aliasing = methods;
    methodarg.klass = klass;
    constarg.mtbl = RMODULE_IV_TBL(klass);
    constarg.id = 0;
    constarg.aliasing = constants;

    st_foreach(mtbl_from, check_mix_method_i, (st_data_t)&methodarg);
    if (methodarg.id) {
	rb_raise(rb_eArgError, "method would conflict - %s", rb_id2name(methodarg.id));
    }
    st_foreach(mtbl_from, check_mix_const_i, (st_data_t)&constarg);
    if (constarg.id) {
	rb_raise(rb_eArgError, "constant would conflict - %s", rb_id2name(constarg.id));
    }
    st_foreach(mtbl_from, do_mix_method_i, (st_data_t)&methodarg);
    if (methodarg.id) {
	rb_raise(rb_eArgError, "method would conflict - %s", rb_id2name(methodarg.id));
    }
    st_foreach(mtbl_from, do_mix_const_i, (st_data_t)&constarg);
    if (constarg.id) {
	rb_raise(rb_eArgError, "constant would conflict - %s", rb_id2name(constarg.id));
    }
    rb_vm_inc_const_missing_count();
}

/*
 *  call-seq:
 *     mod.included_modules -> array
 *
 *  Returns the list of modules included in <i>mod</i>.
 *
 *     module Mixin
 *     end
 *
 *     module Outer
 *       include Mixin
 *     end
 *
 *     Mixin.included_modules   #=> []
 *     Outer.included_modules   #=> [Mixin]
 */

VALUE
rb_mod_included_modules(VALUE mod)
{
    VALUE ary = rb_ary_new();
    VALUE p;

    for (p = RCLASS_SUPER(mod); p; p = RCLASS_SUPER(p)) {
	if (BUILTIN_TYPE(p) == T_ICLASS) {
	    rb_ary_push(ary, RBASIC(p)->klass);
	}
    }
    return ary;
}

/*
 *  call-seq:
 *     mod.include?(module)    -> true or false
 *
 *  Returns <code>true</code> if <i>module</i> is included in
 *  <i>mod</i> or one of <i>mod</i>'s ancestors.
 *
 *     module A
 *     end
 *     class B
 *       include A
 *     end
 *     class C < B
 *     end
 *     B.include?(A)   #=> true
 *     C.include?(A)   #=> true
 *     A.include?(A)   #=> false
 */

VALUE
rb_mod_include_p(VALUE mod, VALUE mod2)
{
    VALUE p;

    Check_Type(mod2, T_MODULE);
    for (p = RCLASS_SUPER(mod); p; p = RCLASS_SUPER(p)) {
	if (BUILTIN_TYPE(p) == T_ICLASS) {
	    if (RBASIC(p)->klass == mod2) return Qtrue;
	}
    }
    return Qfalse;
}

/*
 *  call-seq:
 *     mod.ancestors -> array
 *
 *  Returns a list of modules included in <i>mod</i> (including
 *  <i>mod</i> itself).
 *
 *     module Mod
 *       include Math
 *       include Comparable
 *     end
 *
 *     Mod.ancestors    #=> [Mod, Comparable, Math]
 *     Math.ancestors   #=> [Math]
 */

VALUE
rb_mod_ancestors(VALUE mod)
{
    VALUE p, ary = rb_ary_new();

    for (p = mod; p; p = RCLASS_SUPER(p)) {
	if (FL_TEST(p, FL_SINGLETON))
	    continue;
	if (BUILTIN_TYPE(p) == T_ICLASS) {
	    rb_ary_push(ary, RBASIC(p)->klass);
	}
	else {
	    rb_ary_push(ary, p);
	}
    }
    return ary;
}

#define VISI(x) ((x)&NOEX_MASK)
#define VISI_CHECK(x,f) (VISI(x) == (f))

static int
ins_methods_push(ID name, long type, VALUE ary, long visi)
{
    if (type == -1) return ST_CONTINUE;

    switch (visi) {
      case NOEX_PRIVATE:
      case NOEX_PROTECTED:
      case NOEX_PUBLIC:
	visi = (type == visi);
	break;
      default:
	visi = (type != NOEX_PRIVATE);
	break;
    }
    if (visi) {
	rb_ary_push(ary, ID2SYM(name));
    }
    return ST_CONTINUE;
}

static int
ins_methods_i(st_data_t name, st_data_t type, st_data_t ary)
{
    return ins_methods_push((ID)name, (long)type, (VALUE)ary, -1); /* everything but private */
}

static int
ins_methods_prot_i(st_data_t name, st_data_t type, st_data_t ary)
{
    return ins_methods_push((ID)name, (long)type, (VALUE)ary, NOEX_PROTECTED);
}

static int
ins_methods_priv_i(st_data_t name, st_data_t type, st_data_t ary)
{
    return ins_methods_push((ID)name, (long)type, (VALUE)ary, NOEX_PRIVATE);
}

static int
ins_methods_pub_i(st_data_t name, st_data_t type, st_data_t ary)
{
    return ins_methods_push((ID)name, (long)type, (VALUE)ary, NOEX_PUBLIC);
}

static int
method_entry_i(st_data_t key, st_data_t value, st_data_t data)
{
    const rb_method_entry_t *me = (const rb_method_entry_t *)value;
    st_table *list = (st_table *)data;
    long type;

    if ((ID)key == ID_ALLOCATOR) {
	return ST_CONTINUE;
    }

    if (!st_lookup(list, key, 0)) {
	if (UNDEFINED_METHOD_ENTRY_P(me)) {
	    type = -1; /* none */
	}
	else {
	    type = VISI(me->flag);
	}
	st_add_direct(list, key, type);
    }
    return ST_CONTINUE;
}

static VALUE
class_instance_method_list(int argc, VALUE *argv, VALUE mod, int obj, int (*func) (st_data_t, st_data_t, st_data_t))
{
    VALUE ary;
    int recur;
    st_table *list;

    if (argc == 0) {
	recur = TRUE;
    }
    else {
	VALUE r;
	rb_scan_args(argc, argv, "01", &r);
	recur = RTEST(r);
    }

    list = st_init_numtable();
    for (; mod; mod = RCLASS_SUPER(mod)) {
	st_foreach(RCLASS_M_TBL(mod), method_entry_i, (st_data_t)list);
	if (BUILTIN_TYPE(mod) == T_ICLASS) continue;
	if (obj && FL_TEST(mod, FL_SINGLETON)) continue;
	if (!recur) break;
    }
    ary = rb_ary_new();
    st_foreach(list, func, ary);
    st_free_table(list);

    return ary;
}

/*
 *  call-seq:
 *     mod.instance_methods(include_super=true)   -> array
 *
 *  Returns an array containing the names of the public and protected instance
 *  methods in the receiver. For a module, these are the public and protected methods;
 *  for a class, they are the instance (not singleton) methods. With no
 *  argument, or with an argument that is <code>false</code>, the
 *  instance methods in <i>mod</i> are returned, otherwise the methods
 *  in <i>mod</i> and <i>mod</i>'s superclasses are returned.
 *
 *     module A
 *       def method1()  end
 *     end
 *     class B
 *       def method2()  end
 *     end
 *     class C < B
 *       def method3()  end
 *     end
 *
 *     A.instance_methods                #=> [:method1]
 *     B.instance_methods(false)         #=> [:method2]
 *     C.instance_methods(false)         #=> [:method3]
 *     C.instance_methods(true).length   #=> 43
 */

VALUE
rb_class_instance_methods(int argc, VALUE *argv, VALUE mod)
{
    return class_instance_method_list(argc, argv, mod, 0, ins_methods_i);
}

/*
 *  call-seq:
 *     mod.protected_instance_methods(include_super=true)   -> array
 *
 *  Returns a list of the protected instance methods defined in
 *  <i>mod</i>. If the optional parameter is not <code>false</code>, the
 *  methods of any ancestors are included.
 */

VALUE
rb_class_protected_instance_methods(int argc, VALUE *argv, VALUE mod)
{
    return class_instance_method_list(argc, argv, mod, 0, ins_methods_prot_i);
}

/*
 *  call-seq:
 *     mod.private_instance_methods(include_super=true)    -> array
 *
 *  Returns a list of the private instance methods defined in
 *  <i>mod</i>. If the optional parameter is not <code>false</code>, the
 *  methods of any ancestors are included.
 *
 *     module Mod
 *       def method1()  end
 *       private :method1
 *       def method2()  end
 *     end
 *     Mod.instance_methods           #=> [:method2]
 *     Mod.private_instance_methods   #=> [:method1]
 */

VALUE
rb_class_private_instance_methods(int argc, VALUE *argv, VALUE mod)
{
    return class_instance_method_list(argc, argv, mod, 0, ins_methods_priv_i);
}

/*
 *  call-seq:
 *     mod.public_instance_methods(include_super=true)   -> array
 *
 *  Returns a list of the public instance methods defined in <i>mod</i>.
 *  If the optional parameter is not <code>false</code>, the methods of
 *  any ancestors are included.
 */

VALUE
rb_class_public_instance_methods(int argc, VALUE *argv, VALUE mod)
{
    return class_instance_method_list(argc, argv, mod, 0, ins_methods_pub_i);
}

/*
 *  call-seq:
 *     obj.methods(all=true)    -> array
 *
 *  Returns a list of the names of public and protected methods of
 *  <i>obj</i>. This will include all the methods accessible in
 *  <i>obj</i>'s ancestors.
 *  If the <i>all</i> parameter is set to <code>false</code>, only those methods
 *  in the receiver will be listed.
 *
 *     class Klass
 *       def klass_method()
 *       end
 *     end
 *     k = Klass.new
 *     k.methods[0..9]    #=> [:klass_method, :nil?, :===,
 *                        #    :==~, :!, :eql?
 *                        #    :hash, :<=>, :class, :singleton_class]
 *     k.methods.length   #=> 57
 */

VALUE
rb_obj_methods(int argc, VALUE *argv, VALUE obj)
{
  retry:
    if (argc == 0) {
	return class_instance_method_list(argc, argv, CLASS_OF(obj), 1, ins_methods_i);
    }
    else {
	VALUE recur;

	rb_scan_args(argc, argv, "1", &recur);
	if (RTEST(recur)) {
	    argc = 0;
	    goto retry;
	}
	return rb_obj_singleton_methods(argc, argv, obj);
    }
}

/*
 *  call-seq:
 *     obj.protected_methods(all=true)   -> array
 *
 *  Returns the list of protected methods accessible to <i>obj</i>. If
 *  the <i>all</i> parameter is set to <code>false</code>, only those methods
 *  in the receiver will be listed.
 */

VALUE
rb_obj_protected_methods(int argc, VALUE *argv, VALUE obj)
{
    return class_instance_method_list(argc, argv, CLASS_OF(obj), 1, ins_methods_prot_i);
}

/*
 *  call-seq:
 *     obj.private_methods(all=true)   -> array
 *
 *  Returns the list of private methods accessible to <i>obj</i>. If
 *  the <i>all</i> parameter is set to <code>false</code>, only those methods
 *  in the receiver will be listed.
 */

VALUE
rb_obj_private_methods(int argc, VALUE *argv, VALUE obj)
{
    return class_instance_method_list(argc, argv, CLASS_OF(obj), 1, ins_methods_priv_i);
}

/*
 *  call-seq:
 *     obj.public_methods(all=true)   -> array
 *
 *  Returns the list of public methods accessible to <i>obj</i>. If
 *  the <i>all</i> parameter is set to <code>false</code>, only those methods
 *  in the receiver will be listed.
 */

VALUE
rb_obj_public_methods(int argc, VALUE *argv, VALUE obj)
{
    return class_instance_method_list(argc, argv, CLASS_OF(obj), 1, ins_methods_pub_i);
}

/*
 *  call-seq:
 *     obj.singleton_methods(all=true)    -> array
 *
 *  Returns an array of the names of singleton methods for <i>obj</i>.
 *  If the optional <i>all</i> parameter is true, the list will include
 *  methods in modules included in <i>obj</i>.
 *  Only public and protected singleton methods are returned.
 *
 *     module Other
 *       def three() end
 *     end
 *
 *     class Single
 *       def Single.four() end
 *     end
 *
 *     a = Single.new
 *
 *     def a.one()
 *     end
 *
 *     class << a
 *       include Other
 *       def two()
 *       end
 *     end
 *
 *     Single.singleton_methods    #=> [:four]
 *     a.singleton_methods(false)  #=> [:two, :one]
 *     a.singleton_methods         #=> [:two, :one, :three]
 */

VALUE
rb_obj_singleton_methods(int argc, VALUE *argv, VALUE obj)
{
    VALUE recur, ary, klass;
    st_table *list;

    if (argc == 0) {
	recur = Qtrue;
    }
    else {
	rb_scan_args(argc, argv, "01", &recur);
    }
    klass = CLASS_OF(obj);
    list = st_init_numtable();
    if (klass && FL_TEST(klass, FL_SINGLETON)) {
	st_foreach(RCLASS_M_TBL(klass), method_entry_i, (st_data_t)list);
	klass = RCLASS_SUPER(klass);
    }
    if (RTEST(recur)) {
	while (klass && (FL_TEST(klass, FL_SINGLETON) || RB_TYPE_P(klass, T_ICLASS))) {
	    st_foreach(RCLASS_M_TBL(klass), method_entry_i, (st_data_t)list);
	    klass = RCLASS_SUPER(klass);
	}
    }
    ary = rb_ary_new();
    st_foreach(list, ins_methods_i, ary);
    st_free_table(list);

    return ary;
}

/*!
 * \}
 */
/*!
 * \defgroup defmethod Defining methods
 * There are some APIs to define a method from C.
 * These API takes a C function as a method body.
 *
 * \par Method body functions
 * Method body functions must return a VALUE and
 * can be one of the following form:
 * <dl>
 * <dt>Fixed number of parameters</dt>
 * <dd>
 *     This form is a normal C function, excepting it takes
 *     a receiver object as the first argument.
 *
 *     \code
 *     static VALUE my_method(VALUE self, VALUE x, VALUE y);
 *     \endcode
 * </dd>
 * <dt>argc and argv style</dt>
 * <dd>
 *     This form takes three parameters: \a argc, \a argv and \a self.
 *     \a self is the receiver. \a argc is the number of arguments.
 *     \a argv is a pointer to an array of the arguments.
 *
 *     \code
 *     static VALUE my_method(int argc, VALUE *argv, VALUE self);
 *     \endcode
 * </dd>
 * <dt>Ruby array style</dt>
 * <dd>
 *     This form takes two parameters: self and args.
 *     \a self is the receiver. \a args is an Array object which
 *     contains the arguments.
 *
 *     \code
 *     static VALUE my_method(VALUE self, VALUE args);
 *     \endcode
 * </dd>
 *
 * \par Number of parameters
 * Method defining APIs takes the number of parameters which the
 * method will takes. This number is called \a argc.
 * \a argc can be:
 * <dl>
 * <dt>zero or positive number</dt>
 * <dd>This means the method body function takes a fixed number of parameters</dd>
 * <dt>-1</dt>
 * <dd>This means the method body function is "argc and argv" style.</dd>
 * <dt>-2</dt>
 * <dd>This means the method body function is "self and args" style.</dd>
 * </dl>
 * \{
 */

void
rb_define_method_id(VALUE klass, ID mid, VALUE (*func)(ANYARGS), int argc)
{
    rb_add_method_cfunc(klass, mid, func, argc, NOEX_PUBLIC);
}

void
rb_define_method(VALUE klass, const char *name, VALUE (*func)(ANYARGS), int argc)
{
    rb_add_method_cfunc(klass, rb_intern(name), func, argc, NOEX_PUBLIC);
}

void
rb_define_protected_method(VALUE klass, const char *name, VALUE (*func)(ANYARGS), int argc)
{
    rb_add_method_cfunc(klass, rb_intern(name), func, argc, NOEX_PROTECTED);
}

void
rb_define_private_method(VALUE klass, const char *name, VALUE (*func)(ANYARGS), int argc)
{
    rb_add_method_cfunc(klass, rb_intern(name), func, argc, NOEX_PRIVATE);
}

void
rb_undef_method(VALUE klass, const char *name)
{
    rb_add_method(klass, rb_intern(name), VM_METHOD_TYPE_UNDEF, 0, NOEX_UNDEF);
}

/*!
 * \}
 */
/*!
 * \addtogroup class
 * \{
 */

#define SPECIAL_SINGLETON(x,c) do {\
    if (obj == (x)) {\
	return (c);\
    }\
} while (0)


/*!
 * \internal
 * Returns the singleton class of \a obj. Creates it if necessary.
 *
 * \note DO NOT expose the returned singleton class to
 *       outside of class.c.
 *       Use \ref rb_singleton_class instead for
 *       consistency of the metaclass hierarchy.
 */
static VALUE
singleton_class_of(VALUE obj)
{
    VALUE klass;

    if (FIXNUM_P(obj) || SYMBOL_P(obj)) {
	rb_raise(rb_eTypeError, "can't define singleton");
    }
    if (rb_special_const_p(obj)) {
	SPECIAL_SINGLETON(Qnil, rb_cNilClass);
	SPECIAL_SINGLETON(Qfalse, rb_cFalseClass);
	SPECIAL_SINGLETON(Qtrue, rb_cTrueClass);
	rb_bug("unknown immediate %p", (void *)obj);
    }

    if (FL_TEST(RBASIC(obj)->klass, FL_SINGLETON) &&
	rb_ivar_get(RBASIC(obj)->klass, id_attached) == obj) {
	klass = RBASIC(obj)->klass;
    }
    else {
	klass = rb_make_metaclass(obj, RBASIC(obj)->klass);
    }

    if (OBJ_TAINTED(obj)) {
	OBJ_TAINT(klass);
    }
    else {
	FL_UNSET(klass, FL_TAINT);
    }
    if (OBJ_UNTRUSTED(obj)) {
	OBJ_UNTRUST(klass);
    }
    else {
	FL_UNSET(klass, FL_UNTRUSTED);
    }
    if (OBJ_FROZEN(obj)) OBJ_FREEZE(klass);

    return klass;
}


/*!
 * Returns the singleton class of \a obj. Creates it if necessary.
 *
 * \param obj an arbitrary object.
 * \throw TypeError if \a obj is a Fixnum or a Symbol.
 * \return the singleton class.
 *
 * \post \a obj has its own singleton class.
 * \post if \a obj is a class,
 *       the returned singleton class also has its own
 *       singleton class in order to keep consistency of the
 *       inheritance structure of metaclasses.
 * \note a new singleton class will be created
 *       if \a obj does not have it.
 * \note the singleton classes for nil, true and false are:
 *       NilClass, TrueClass and FalseClass.
 */
VALUE
rb_singleton_class(VALUE obj)
{
    VALUE klass = singleton_class_of(obj);

    /* ensures an exposed class belongs to its own eigenclass */
    if (RB_TYPE_P(obj, T_CLASS)) (void)ENSURE_EIGENCLASS(klass);

    return klass;
}

/*!
 * \}
 */

/*!
 * \addtogroup defmethod
 * \{
 */

/*!
 * Defines a singleton method for \a obj.
 * \param obj    an arbitrary object
 * \param name   name of the singleton method
 * \param func   the method body
 * \param argc   the number of parameters, or -1 or -2. see \ref defmethod.
 */
void
rb_define_singleton_method(VALUE obj, const char *name, VALUE (*func)(ANYARGS), int argc)
{
    rb_define_method(singleton_class_of(obj), name, func, argc);
}



/*!
 * Defines a module function for \a module.
 * \param module  an module or a class.
 * \param name    name of the function
 * \param func    the method body
 * \param argc    the number of parameters, or -1 or -2. see \ref defmethod.
 */
void
rb_define_module_function(VALUE module, const char *name, VALUE (*func)(ANYARGS), int argc)
{
    rb_define_private_method(module, name, func, argc);
    rb_define_singleton_method(module, name, func, argc);
}


/*!
 * Defines a global function
 * \param name    name of the function
 * \param func    the method body
 * \param argc    the number of parameters, or -1 or -2. see \ref defmethod.
 */
void
rb_define_global_function(const char *name, VALUE (*func)(ANYARGS), int argc)
{
    rb_define_module_function(rb_mKernel, name, func, argc);
}


/*!
 * Defines an alias of a method.
 * \param klass  the class which the original method belongs to
 * \param name1  a new name for the method
 * \param name2  the original name of the method
 */
void
rb_define_alias(VALUE klass, const char *name1, const char *name2)
{
    rb_alias(klass, rb_intern(name1), rb_intern(name2));
}

/*!
 * Defines (a) public accessor method(s) for an attribute.
 * \param klass  the class which the attribute will belongs to
 * \param name   name of the attribute
 * \param read   a getter method for the attribute will be defined if \a read is non-zero.
 * \param write  a setter method for the attribute will be defined if \a write is non-zero.
 */
void
rb_define_attr(VALUE klass, const char *name, int read, int write)
{
    rb_attr(klass, rb_intern(name), read, write, FALSE);
}

int
rb_obj_basic_to_s_p(VALUE obj)
{
    const rb_method_entry_t *me = rb_method_entry(CLASS_OF(obj), rb_intern("to_s"));
    if (me && me->def && me->def->type == VM_METHOD_TYPE_CFUNC &&
	me->def->body.cfunc.func == rb_any_to_s)
	return 1;
    return 0;
}

#include <stdarg.h>

int
rb_scan_args(int argc, const VALUE *argv, const char *fmt, ...)
{
    int i;
    const char *p = fmt;
    VALUE *var;
    va_list vargs;
    int f_var = 0, f_hash = 0, f_block = 0;
    int n_lead = 0, n_opt = 0, n_trail = 0, n_mand;
    int argi = 0;
    VALUE hash = Qnil;

    if (ISDIGIT(*p)) {
	n_lead = *p - '0';
	p++;
	if (ISDIGIT(*p)) {
	    n_opt = *p - '0';
	    p++;
	    if (ISDIGIT(*p)) {
		n_trail = *p - '0';
		p++;
		goto block_arg;
	    }
	}
    }
    if (*p == '*') {
	f_var = 1;
	p++;
	if (ISDIGIT(*p)) {
	    n_trail = *p - '0';
	    p++;
	}
    }
  block_arg:
    if (*p == ':') {
	f_hash = 1;
	p++;
    }
    if (*p == '&') {
	f_block = 1;
	p++;
    }
    if (*p != '\0') {
	rb_fatal("bad scan arg format: %s", fmt);
    }
    n_mand = n_lead + n_trail;

    if (argc < n_mand)
	goto argc_error;

    va_start(vargs, fmt);

    /* capture an option hash - phase 1: pop */
    if (f_hash && n_mand < argc) {
	VALUE last = argv[argc - 1];

	if (NIL_P(last)) {
	    /* nil is taken as an empty option hash only if it is not
	       ambiguous; i.e. '*' is not specified and arguments are
	       given more than sufficient */
	    if (!f_var && n_mand + n_opt < argc)
		argc--;
	}
	else {
	    hash = rb_check_convert_type(last, T_HASH, "Hash", "to_hash");
	    if (!NIL_P(hash))
		argc--;
	}
    }
    /* capture leading mandatory arguments */
    for (i = n_lead; i-- > 0; ) {
	var = va_arg(vargs, VALUE *);
	if (var) *var = argv[argi];
	argi++;
    }
    /* capture optional arguments */
    for (i = n_opt; i-- > 0; ) {
	var = va_arg(vargs, VALUE *);
	if (argi < argc - n_trail) {
	    if (var) *var = argv[argi];
	    argi++;
	}
	else {
	    if (var) *var = Qnil;
	}
    }
    /* capture variable length arguments */
    if (f_var) {
	int n_var = argc - argi - n_trail;

	var = va_arg(vargs, VALUE *);
	if (0 < n_var) {
	    if (var) *var = rb_ary_new4(n_var, &argv[argi]);
	    argi += n_var;
	}
	else {
	    if (var) *var = rb_ary_new();
	}
    }
    /* capture trailing mandatory arguments */
    for (i = n_trail; i-- > 0; ) {
	var = va_arg(vargs, VALUE *);
	if (var) *var = argv[argi];
	argi++;
    }
    /* capture an option hash - phase 2: assignment */
    if (f_hash) {
	var = va_arg(vargs, VALUE *);
	if (var) *var = hash;
    }
    /* capture iterator block */
    if (f_block) {
	var = va_arg(vargs, VALUE *);
	if (rb_block_given_p()) {
	    *var = rb_block_proc();
	}
	else {
	    *var = Qnil;
	}
    }
    va_end(vargs);

    if (argi < argc)
	goto argc_error;

    return argc;

  argc_error:
    rb_error_arity(argc, n_mand, f_var ? UNLIMITED_ARGUMENTS : n_mand + n_opt);
}

/*!
 * \}
 */
