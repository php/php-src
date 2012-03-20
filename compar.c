/**********************************************************************

  compar.c -

  $Author$
  created at: Thu Aug 26 14:39:48 JST 1993

  Copyright (C) 1993-2007 Yukihiro Matsumoto

**********************************************************************/

#include "ruby/ruby.h"

VALUE rb_mComparable;

static ID cmp;

void
rb_cmperr(VALUE x, VALUE y)
{
    const char *classname;

    if (SPECIAL_CONST_P(y)) {
	y = rb_inspect(y);
	classname = StringValuePtr(y);
    }
    else {
	classname = rb_obj_classname(y);
    }
    rb_raise(rb_eArgError, "comparison of %s with %s failed",
	     rb_obj_classname(x), classname);
}

static VALUE
cmp_eq(VALUE *a)
{
    VALUE c = rb_funcall(a[0], cmp, 1, a[1]);

    if (NIL_P(c)) return Qfalse;
    if (rb_cmpint(c, a[0], a[1]) == 0) return Qtrue;
    return Qfalse;
}

static VALUE
cmp_failed(void)
{
    return Qfalse;
}

/*
 *  call-seq:
 *     obj == other    -> true or false
 *
 *  Compares two objects based on the receiver's <code><=></code>
 *  method, returning true if it returns 0. Also returns true if
 *  _obj_ and _other_ are the same object.
 */

static VALUE
cmp_equal(VALUE x, VALUE y)
{
    VALUE a[2];

    if (x == y) return Qtrue;

    a[0] = x; a[1] = y;
    return rb_rescue(cmp_eq, (VALUE)a, cmp_failed, 0);
}

/*
 *  call-seq:
 *     obj > other    -> true or false
 *
 *  Compares two objects based on the receiver's <code><=></code>
 *  method, returning true if it returns 1.
 */

static VALUE
cmp_gt(VALUE x, VALUE y)
{
    VALUE c = rb_funcall(x, cmp, 1, y);

    if (rb_cmpint(c, x, y) > 0) return Qtrue;
    return Qfalse;
}

/*
 *  call-seq:
 *     obj >= other    -> true or false
 *
 *  Compares two objects based on the receiver's <code><=></code>
 *  method, returning true if it returns 0 or 1.
 */

static VALUE
cmp_ge(VALUE x, VALUE y)
{
    VALUE c = rb_funcall(x, cmp, 1, y);

    if (rb_cmpint(c, x, y) >= 0) return Qtrue;
    return Qfalse;
}

/*
 *  call-seq:
 *     obj < other    -> true or false
 *
 *  Compares two objects based on the receiver's <code><=></code>
 *  method, returning true if it returns -1.
 */

static VALUE
cmp_lt(VALUE x, VALUE y)
{
    VALUE c = rb_funcall(x, cmp, 1, y);

    if (rb_cmpint(c, x, y) < 0) return Qtrue;
    return Qfalse;
}

/*
 *  call-seq:
 *     obj <= other    -> true or false
 *
 *  Compares two objects based on the receiver's <code><=></code>
 *  method, returning true if it returns -1 or 0.
 */

static VALUE
cmp_le(VALUE x, VALUE y)
{
    VALUE c = rb_funcall(x, cmp, 1, y);

    if (rb_cmpint(c, x, y) <= 0) return Qtrue;
    return Qfalse;
}

/*
 *  call-seq:
 *     obj.between?(min, max)    -> true or false
 *
 *  Returns <code>false</code> if <i>obj</i> <code><=></code>
 *  <i>min</i> is less than zero or if <i>anObject</i> <code><=></code>
 *  <i>max</i> is greater than zero, <code>true</code> otherwise.
 *
 *     3.between?(1, 5)               #=> true
 *     6.between?(1, 5)               #=> false
 *     'cat'.between?('ant', 'dog')   #=> true
 *     'gnu'.between?('ant', 'dog')   #=> false
 *
 */

static VALUE
cmp_between(VALUE x, VALUE min, VALUE max)
{
    if (RTEST(cmp_lt(x, min))) return Qfalse;
    if (RTEST(cmp_gt(x, max))) return Qfalse;
    return Qtrue;
}

/*
 *  The <code>Comparable</code> mixin is used by classes whose objects
 *  may be ordered. The class must define the <code><=></code> operator,
 *  which compares the receiver against another object, returning -1, 0,
 *  or +1 depending on whether the receiver is less than, equal to, or
 *  greater than the other object. If the other object is not comparable
 *  then the <code><=></code> operator should return nil.
 *  <code>Comparable</code> uses
 *  <code><=></code> to implement the conventional comparison operators
 *  (<code><</code>, <code><=</code>, <code>==</code>, <code>>=</code>,
 *  and <code>></code>) and the method <code>between?</code>.
 *
 *     class SizeMatters
 *       include Comparable
 *       attr :str
 *       def <=>(anOther)
 *         str.size <=> anOther.str.size
 *       end
 *       def initialize(str)
 *         @str = str
 *       end
 *       def inspect
 *         @str
 *       end
 *     end
 *
 *     s1 = SizeMatters.new("Z")
 *     s2 = SizeMatters.new("YY")
 *     s3 = SizeMatters.new("XXX")
 *     s4 = SizeMatters.new("WWWW")
 *     s5 = SizeMatters.new("VVVVV")
 *
 *     s1 < s2                       #=> true
 *     s4.between?(s1, s3)           #=> false
 *     s4.between?(s3, s5)           #=> true
 *     [ s3, s2, s5, s4, s1 ].sort   #=> [Z, YY, XXX, WWWW, VVVVV]
 *
 */

void
Init_Comparable(void)
{
#undef rb_intern
#define rb_intern(str) rb_intern_const(str)

    rb_mComparable = rb_define_module("Comparable");
    rb_define_method(rb_mComparable, "==", cmp_equal, 1);
    rb_define_method(rb_mComparable, ">", cmp_gt, 1);
    rb_define_method(rb_mComparable, ">=", cmp_ge, 1);
    rb_define_method(rb_mComparable, "<", cmp_lt, 1);
    rb_define_method(rb_mComparable, "<=", cmp_le, 1);
    rb_define_method(rb_mComparable, "between?", cmp_between, 2);

    cmp = rb_intern("<=>");
}
