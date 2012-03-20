/**********************************************************************

  range.c -

  $Author$
  created at: Thu Aug 19 17:46:47 JST 1993

  Copyright (C) 1993-2007 Yukihiro Matsumoto

**********************************************************************/

#include "ruby/ruby.h"
#include "ruby/encoding.h"
#include "internal.h"

VALUE rb_cRange;
static ID id_cmp, id_succ, id_beg, id_end, id_excl;

#define RANGE_BEG(r) (RSTRUCT(r)->as.ary[0])
#define RANGE_END(r) (RSTRUCT(r)->as.ary[1])
#define RANGE_EXCL(r) (RSTRUCT(r)->as.ary[2])

#define EXCL(r) RTEST(RANGE_EXCL(r))
#define SET_EXCL(r,v) (RSTRUCT(r)->as.ary[2] = (v) ? Qtrue : Qfalse)

static VALUE
range_failed(void)
{
    rb_raise(rb_eArgError, "bad value for range");
    return Qnil;		/* dummy */
}

static VALUE
range_check(VALUE *args)
{
    return rb_funcall(args[0], id_cmp, 1, args[1]);
}

static void
range_init(VALUE range, VALUE beg, VALUE end, int exclude_end)
{
    VALUE args[2];

    args[0] = beg;
    args[1] = end;

    if (!FIXNUM_P(beg) || !FIXNUM_P(end)) {
	VALUE v;

	v = rb_rescue(range_check, (VALUE)args, range_failed, 0);
	if (NIL_P(v))
	    range_failed();
    }

    SET_EXCL(range, exclude_end);
    RSTRUCT(range)->as.ary[0] = beg;
    RSTRUCT(range)->as.ary[1] = end;
}

VALUE
rb_range_new(VALUE beg, VALUE end, int exclude_end)
{
    VALUE range = rb_obj_alloc(rb_cRange);

    range_init(range, beg, end, exclude_end);
    return range;
}

/*
 *  call-seq:
 *     Range.new(begin, end, exclude_end=false)    -> rng
 *
 *  Constructs a range using the given +begin+ and +end+. If the +exclude_end+
 *  parameter is omitted or is <code>false</code>, the +rng+ will include
 *  the end object; otherwise, it will be excluded.
 */

static VALUE
range_initialize(int argc, VALUE *argv, VALUE range)
{
    VALUE beg, end, flags;

    rb_scan_args(argc, argv, "21", &beg, &end, &flags);
    /* Ranges are immutable, so that they should be initialized only once. */
    if (RANGE_EXCL(range) != Qnil) {
	rb_name_error(rb_intern("initialize"), "`initialize' called twice");
    }
    range_init(range, beg, end, RTEST(flags));
    return Qnil;
}

#define range_initialize_copy rb_struct_init_copy /* :nodoc: */

/*
 *  call-seq:
 *     rng.exclude_end?    -> true or false
 *
 *  Returns <code>true</code> if the range excludes its end value.
 *
 *     (1..5).exclude_end?     #=> false
 *     (1...5).exclude_end?    #=> true
 */

static VALUE
range_exclude_end_p(VALUE range)
{
    return EXCL(range) ? Qtrue : Qfalse;
}

static VALUE
recursive_equal(VALUE range, VALUE obj, int recur)
{
    if (recur) return Qtrue; /* Subtle! */
    if (!rb_equal(RANGE_BEG(range), RANGE_BEG(obj)))
	return Qfalse;
    if (!rb_equal(RANGE_END(range), RANGE_END(obj)))
	return Qfalse;

    if (EXCL(range) != EXCL(obj))
	return Qfalse;
    return Qtrue;
}


/*
 *  call-seq:
 *     rng == obj    -> true or false
 *
 *  Returns <code>true</code> only if +obj+ is a Range, has equivalent
 *  begin and end items (by comparing them with <code>==</code>), and has
 *  the same #exclude_end? setting as the range.
 *
 *    (0..2) == (0..2)            #=> true
 *    (0..2) == Range.new(0,2)    #=> true
 *    (0..2) == (0...2)           #=> false
 *
 */

static VALUE
range_eq(VALUE range, VALUE obj)
{
    if (range == obj)
	return Qtrue;
    if (!rb_obj_is_kind_of(obj, rb_cRange))
	return Qfalse;

    return rb_exec_recursive_paired(recursive_equal, range, obj, obj);
}

static int
r_lt(VALUE a, VALUE b)
{
    VALUE r = rb_funcall(a, id_cmp, 1, b);

    if (NIL_P(r))
	return (int)Qfalse;
    if (rb_cmpint(r, a, b) < 0)
	return (int)Qtrue;
    return (int)Qfalse;
}

static int
r_le(VALUE a, VALUE b)
{
    int c;
    VALUE r = rb_funcall(a, id_cmp, 1, b);

    if (NIL_P(r))
	return (int)Qfalse;
    c = rb_cmpint(r, a, b);
    if (c == 0)
	return (int)INT2FIX(0);
    if (c < 0)
	return (int)Qtrue;
    return (int)Qfalse;
}


static VALUE
recursive_eql(VALUE range, VALUE obj, int recur)
{
    if (recur) return Qtrue; /* Subtle! */
    if (!rb_eql(RANGE_BEG(range), RANGE_BEG(obj)))
	return Qfalse;
    if (!rb_eql(RANGE_END(range), RANGE_END(obj)))
	return Qfalse;

    if (EXCL(range) != EXCL(obj))
	return Qfalse;
    return Qtrue;
}

/*
 *  call-seq:
 *     rng.eql?(obj)    -> true or false
 *
 *  Returns <code>true</code> only if +obj+ is a Range, has equivalent
 *  begin and end items (by comparing them with <code>eql?</code>),
 *  and has the same #exclude_end? setting as the range.
 *
 *    (0..2).eql?(0..2)            #=> true
 *    (0..2).eql?(Range.new(0,2))  #=> true
 *    (0..2).eql?(0...2)           #=> false
 *
 */

static VALUE
range_eql(VALUE range, VALUE obj)
{
    if (range == obj)
	return Qtrue;
    if (!rb_obj_is_kind_of(obj, rb_cRange))
	return Qfalse;
    return rb_exec_recursive_paired(recursive_eql, range, obj, obj);
}

static VALUE
recursive_hash(VALUE range, VALUE dummy, int recur)
{
    st_index_t hash = EXCL(range);
    VALUE v;

    hash = rb_hash_start(hash);
    if (!recur) {
	v = rb_hash(RANGE_BEG(range));
	hash = rb_hash_uint(hash, NUM2LONG(v));
	v = rb_hash(RANGE_END(range));
	hash = rb_hash_uint(hash, NUM2LONG(v));
    }
    hash = rb_hash_uint(hash, EXCL(range) << 24);
    hash = rb_hash_end(hash);

    return LONG2FIX(hash);
}

/*
 * call-seq:
 *   rng.hash    -> fixnum
 *
 * Compute a hash-code for this range. Two ranges with equal
 * begin and end points (using <code>eql?</code>), and the same
 * #exclude_end? value will generate the same hash-code.
 */

static VALUE
range_hash(VALUE range)
{
    return rb_exec_recursive_outer(recursive_hash, range, 0);
}

static void
range_each_func(VALUE range, VALUE (*func) (VALUE, void *), void *arg)
{
    int c;
    VALUE b = RANGE_BEG(range);
    VALUE e = RANGE_END(range);
    VALUE v = b;

    if (EXCL(range)) {
	while (r_lt(v, e)) {
	    (*func) (v, arg);
	    v = rb_funcall(v, id_succ, 0, 0);
	}
    }
    else {
	while ((c = r_le(v, e)) != Qfalse) {
	    (*func) (v, arg);
	    if (c == (int)INT2FIX(0))
		break;
	    v = rb_funcall(v, id_succ, 0, 0);
	}
    }
}

static VALUE
sym_step_i(VALUE i, void *arg)
{
    VALUE *iter = arg;

    if (FIXNUM_P(iter[0])) {
	iter[0] -= INT2FIX(1) & ~FIXNUM_FLAG;
    }
    else {
	iter[0] = rb_funcall(iter[0], '-', 1, INT2FIX(1));
    }
    if (iter[0] == INT2FIX(0)) {
	rb_yield(rb_str_intern(i));
	iter[0] = iter[1];
    }
    return Qnil;
}

static VALUE
step_i(VALUE i, void *arg)
{
    VALUE *iter = arg;

    if (FIXNUM_P(iter[0])) {
	iter[0] -= INT2FIX(1) & ~FIXNUM_FLAG;
    }
    else {
	iter[0] = rb_funcall(iter[0], '-', 1, INT2FIX(1));
    }
    if (iter[0] == INT2FIX(0)) {
	rb_yield(i);
	iter[0] = iter[1];
    }
    return Qnil;
}

static int
discrete_object_p(VALUE obj)
{
    if (rb_obj_is_kind_of(obj, rb_cTime)) return FALSE; /* until Time#succ removed */
    return rb_respond_to(obj, id_succ);
}


/*
 *  call-seq:
 *     rng.step(n=1) {| obj | block }    -> rng
 *     rng.step(n=1)                     -> an_enumerator
 *
 *  Iterates over the range, passing each <code>n</code>th element to the block.
 *  If begin and end are numeric, +n+ is added for each iteration.
 *  Otherwise <code>step</code> invokes <code>succ</code> to iterate through
 *  range elements.
 *
 *  If no block is given, an enumerator is returned instead.
 *
 *    range = Xs.new(1)..Xs.new(10)
 *    range.step(2) {|x| puts x}
 *    puts
 *    range.step(3) {|x| puts x}
 *
 *  <em>produces:</em>
 *
 *     1 x
 *     3 xxx
 *     5 xxxxx
 *     7 xxxxxxx
 *     9 xxxxxxxxx
 *
 *     1 x
 *     4 xxxx
 *     7 xxxxxxx
 *    10 xxxxxxxxxx
 *
 *  See Range for the definition of class Xs.
 */


static VALUE
range_step(int argc, VALUE *argv, VALUE range)
{
    VALUE b, e, step, tmp;

    RETURN_ENUMERATOR(range, argc, argv);

    b = RANGE_BEG(range);
    e = RANGE_END(range);
    if (argc == 0) {
	step = INT2FIX(1);
    }
    else {
	rb_scan_args(argc, argv, "01", &step);
	if (!rb_obj_is_kind_of(step, rb_cNumeric)) {
	    step = rb_to_int(step);
	}
	if (rb_funcall(step, '<', 1, INT2FIX(0))) {
	    rb_raise(rb_eArgError, "step can't be negative");
	}
	else if (!rb_funcall(step, '>', 1, INT2FIX(0))) {
	    rb_raise(rb_eArgError, "step can't be 0");
	}
    }

    if (FIXNUM_P(b) && FIXNUM_P(e) && FIXNUM_P(step)) { /* fixnums are special */
	long end = FIX2LONG(e);
	long i, unit = FIX2LONG(step);

	if (!EXCL(range))
	    end += 1;
	i = FIX2LONG(b);
	while (i < end) {
	    rb_yield(LONG2NUM(i));
	    if (i + unit < i) break;
	    i += unit;
	}

    }
    else if (SYMBOL_P(b) && SYMBOL_P(e)) { /* symbols are special */
	VALUE args[2], iter[2];

	args[0] = rb_sym_to_s(e);
	args[1] = EXCL(range) ? Qtrue : Qfalse;
	iter[0] = INT2FIX(1);
	iter[1] = step;
	rb_block_call(rb_sym_to_s(b), rb_intern("upto"), 2, args, sym_step_i, (VALUE)iter);
    }
    else if (ruby_float_step(b, e, step, EXCL(range))) {
	/* done */
    }
    else if (rb_obj_is_kind_of(b, rb_cNumeric) ||
	     !NIL_P(rb_check_to_integer(b, "to_int")) ||
	     !NIL_P(rb_check_to_integer(e, "to_int"))) {
	ID op = EXCL(range) ? '<' : rb_intern("<=");
	VALUE v = b;
	int i = 0;

	while (RTEST(rb_funcall(v, op, 1, e))) {
	    rb_yield(v);
	    i++;
	    v = rb_funcall(b, '+', 1, rb_funcall(INT2NUM(i), '*', 1, step));
	}
    }
    else {
	tmp = rb_check_string_type(b);

	if (!NIL_P(tmp)) {
	    VALUE args[2], iter[2];

	    b = tmp;
	    args[0] = e;
	    args[1] = EXCL(range) ? Qtrue : Qfalse;
	    iter[0] = INT2FIX(1);
	    iter[1] = step;
	    rb_block_call(b, rb_intern("upto"), 2, args, step_i, (VALUE)iter);
	}
	else {
	    VALUE args[2];

	    if (!discrete_object_p(b)) {
		rb_raise(rb_eTypeError, "can't iterate from %s",
			 rb_obj_classname(b));
	    }
	    args[0] = INT2FIX(1);
	    args[1] = step;
	    range_each_func(range, step_i, args);
	}
    }
    return range;
}

static VALUE
each_i(VALUE v, void *arg)
{
    rb_yield(v);
    return Qnil;
}

static VALUE
sym_each_i(VALUE v, void *arg)
{
    rb_yield(rb_str_intern(v));
    return Qnil;
}

/*
 *  call-seq:
 *     rng.each {| i | block } -> rng
 *     rng.each                -> an_enumerator
 *
 *  Iterates over the elements of range, passing each in turn to the
 *  block.
 *
 *  The +each+ method can only be used if the begin object of the range
 *  supports the +succ+ method.  A TypeError is raised if the object
 *  does not have +succ+ method defined (like Float).
 *
 *  If no block is given, an enumerator is returned instead.
 *
 *     (10..15).each {|n| print n, ' ' }
 *     # prints: 10 11 12 13 14 15
 *
 *     (2.5..5).each {|n| print n, ' ' }
 *     # raises: TypeError: can't iterate from Float
 */

static VALUE
range_each(VALUE range)
{
    VALUE beg, end;

    RETURN_ENUMERATOR(range, 0, 0);

    beg = RANGE_BEG(range);
    end = RANGE_END(range);

    if (FIXNUM_P(beg) && FIXNUM_P(end)) { /* fixnums are special */
	long lim = FIX2LONG(end);
	long i;

	if (!EXCL(range))
	    lim += 1;
	for (i = FIX2LONG(beg); i < lim; i++) {
	    rb_yield(LONG2FIX(i));
	}
    }
    else if (SYMBOL_P(beg) && SYMBOL_P(end)) { /* symbols are special */
	VALUE args[2];

	args[0] = rb_sym_to_s(end);
	args[1] = EXCL(range) ? Qtrue : Qfalse;
	rb_block_call(rb_sym_to_s(beg), rb_intern("upto"), 2, args, sym_each_i, 0);
    }
    else {
	VALUE tmp = rb_check_string_type(beg);

	if (!NIL_P(tmp)) {
	    VALUE args[2];

	    args[0] = end;
	    args[1] = EXCL(range) ? Qtrue : Qfalse;
	    rb_block_call(tmp, rb_intern("upto"), 2, args, rb_yield, 0);
	}
	else {
	    if (!discrete_object_p(beg)) {
		rb_raise(rb_eTypeError, "can't iterate from %s",
			 rb_obj_classname(beg));
	    }
	    range_each_func(range, each_i, NULL);
	}
    }
    return range;
}

/*
 *  call-seq:
 *     rng.begin    -> obj
 *
 *  Returns the object that defines the beginning of the range.
 *
 *      (1..10).begin   #=> 1
 */

static VALUE
range_begin(VALUE range)
{
    return RANGE_BEG(range);
}


/*
 *  call-seq:
 *     rng.end    -> obj
 *
 *  Returns the object that defines the end of the range.
 *
 *     (1..10).end    #=> 10
 *     (1...10).end   #=> 10
 */


static VALUE
range_end(VALUE range)
{
    return RANGE_END(range);
}


static VALUE
first_i(VALUE i, VALUE *ary)
{
    long n = NUM2LONG(ary[0]);

    if (n <= 0) {
	rb_iter_break();
    }
    rb_ary_push(ary[1], i);
    n--;
    ary[0] = INT2NUM(n);
    return Qnil;
}

/*
 *  call-seq:
 *     rng.first    -> obj
 *     rng.first(n) -> an_array
 *
 *  Returns the first object in the range, or an array of the first +n+
 *  elements.
 *
 *    (10..20).first     #=> 10
 *    (10..20).first(3)  #=> [10, 11, 12]
 */

static VALUE
range_first(int argc, VALUE *argv, VALUE range)
{
    VALUE n, ary[2];

    if (argc == 0) return RANGE_BEG(range);

    rb_scan_args(argc, argv, "1", &n);
    ary[0] = n;
    ary[1] = rb_ary_new2(NUM2LONG(n));
    rb_block_call(range, rb_intern("each"), 0, 0, first_i, (VALUE)ary);

    return ary[1];
}


/*
 *  call-seq:
 *     rng.last    -> obj
 *     rng.last(n) -> an_array
 *
 *  Returns the last object in the range,
 *  or an array of the last +n+ elements.
 *
 *  Note that with no arguments +last+ will return the object that defines
 *  the end of the range even if #exclude_end? is +true+.
 *
 *    (10..20).last      #=> 20
 *    (10...20).last     #=> 20
 *    (10..20).last(3)   #=> [18, 19, 20]
 *    (10...20).last(3)  #=> [17, 18, 19]
 */

static VALUE
range_last(int argc, VALUE *argv, VALUE range)
{
    if (argc == 0) return RANGE_END(range);
    return rb_ary_last(argc, argv, rb_Array(range));
}


/*
 *  call-seq:
 *     rng.min                    -> obj
 *     rng.min {| a,b | block }   -> obj
 *
 *  Returns the minimum value in the range. Returns +nil+ if the begin
 *  value of the range is larger than the end value.
 *
 *  Can be given an optional block to override the default comparison
 *  method <code>a <=> b</code>.
 *
 *    (10..20).min    #=> 10
 */


static VALUE
range_min(VALUE range)
{
    if (rb_block_given_p()) {
	return rb_call_super(0, 0);
    }
    else {
	VALUE b = RANGE_BEG(range);
	VALUE e = RANGE_END(range);
	int c = rb_cmpint(rb_funcall(b, id_cmp, 1, e), b, e);

	if (c > 0 || (c == 0 && EXCL(range)))
	    return Qnil;
	return b;
    }
}

/*
 *  call-seq:
 *     rng.max                    -> obj
 *     rng.max {| a,b | block }   -> obj
 *
 *  Returns the maximum value in the range. Returns +nil+ if the begin
 *  value of the range larger than the end value.
 *
 *  Can be given an optional block to override the default comparison
 *  method <code>a <=> b</code>.
 *
 *    (10..20).max    #=> 20
 */

static VALUE
range_max(VALUE range)
{
    VALUE e = RANGE_END(range);
    int nm = FIXNUM_P(e) || rb_obj_is_kind_of(e, rb_cNumeric);

    if (rb_block_given_p() || (EXCL(range) && !nm)) {
	return rb_call_super(0, 0);
    }
    else {
	VALUE b = RANGE_BEG(range);
	int c = rb_cmpint(rb_funcall(b, id_cmp, 1, e), b, e);

	if (c > 0)
	    return Qnil;
	if (EXCL(range)) {
	    if (!FIXNUM_P(e) && !rb_obj_is_kind_of(e, rb_cInteger)) {
		rb_raise(rb_eTypeError, "cannot exclude non Integer end value");
	    }
	    if (c == 0) return Qnil;
	    if (!FIXNUM_P(b) && !rb_obj_is_kind_of(b,rb_cInteger)) {
		rb_raise(rb_eTypeError, "cannot exclude end value with non Integer begin value");
	    }
	    if (FIXNUM_P(e)) {
		return LONG2NUM(FIX2LONG(e) - 1);
	    }
	    return rb_funcall(e, '-', 1, INT2FIX(1));
	}
	return e;
    }
}

int
rb_range_values(VALUE range, VALUE *begp, VALUE *endp, int *exclp)
{
    VALUE b, e;
    int excl;

    if (rb_obj_is_kind_of(range, rb_cRange)) {
	b = RANGE_BEG(range);
	e = RANGE_END(range);
	excl = EXCL(range);
    }
    else {
	if (!rb_respond_to(range, id_beg)) return (int)Qfalse;
	if (!rb_respond_to(range, id_end)) return (int)Qfalse;
	b = rb_funcall(range, id_beg, 0);
	e = rb_funcall(range, id_end, 0);
	excl = RTEST(rb_funcall(range, rb_intern("exclude_end?"), 0));
    }
    *begp = b;
    *endp = e;
    *exclp = excl;
    return (int)Qtrue;
}

VALUE
rb_range_beg_len(VALUE range, long *begp, long *lenp, long len, int err)
{
    long beg, end, origbeg, origend;
    VALUE b, e;
    int excl;

    if (!rb_range_values(range, &b, &e, &excl))
	return Qfalse;
    beg = NUM2LONG(b);
    end = NUM2LONG(e);
    origbeg = beg;
    origend = end;
    if (beg < 0) {
	beg += len;
	if (beg < 0)
	    goto out_of_range;
    }
    if (err == 0 || err == 2) {
	if (beg > len)
	    goto out_of_range;
	if (end > len)
	    end = len;
    }
    if (end < 0)
	end += len;
    if (!excl)
	end++;			/* include end point */
    len = end - beg;
    if (len < 0)
	len = 0;

    *begp = beg;
    *lenp = len;
    return Qtrue;

  out_of_range:
    if (err) {
	rb_raise(rb_eRangeError, "%ld..%s%ld out of range",
		 origbeg, excl ? "." : "", origend);
    }
    return Qnil;
}

/*
 * call-seq:
 *   rng.to_s   -> string
 *
 * Convert this range object to a printable form (using #to_s to convert the
 * begin and end objects).
 */

static VALUE
range_to_s(VALUE range)
{
    VALUE str, str2;

    str = rb_obj_as_string(RANGE_BEG(range));
    str2 = rb_obj_as_string(RANGE_END(range));
    str = rb_str_dup(str);
    rb_str_cat(str, "...", EXCL(range) ? 3 : 2);
    rb_str_append(str, str2);
    OBJ_INFECT(str, str2);

    return str;
}

static VALUE
inspect_range(VALUE range, VALUE dummy, int recur)
{
    VALUE str, str2;

    if (recur) {
	return rb_str_new2(EXCL(range) ? "(... ... ...)" : "(... .. ...)");
    }
    str = rb_inspect(RANGE_BEG(range));
    str2 = rb_inspect(RANGE_END(range));
    str = rb_str_dup(str);
    rb_str_cat(str, "...", EXCL(range) ? 3 : 2);
    rb_str_append(str, str2);
    OBJ_INFECT(str, str2);

    return str;
}

/*
 * call-seq:
 *   rng.inspect  -> string
 *
 * Convert this range object to a printable form (using
 * <code>inspect</code> to convert the begin and end
 * objects).
 */


static VALUE
range_inspect(VALUE range)
{
    return rb_exec_recursive(inspect_range, range, 0);
}

/*
 *  call-seq:
 *     rng === obj       ->  true or false
 *
 *  Returns <code>true</code> if +obj+ is an element of the range,
 *  <code>false</code> otherwise.  Conveniently, <code>===</code> is the
 *  comparison operator used by <code>case</code> statements.
 *
 *     case 79
 *     when 1..50   then   print "low\n"
 *     when 51..75  then   print "medium\n"
 *     when 76..100 then   print "high\n"
 *     end
 *
 *  <em>produces:</em>
 *
 *     high
 */

static VALUE
range_eqq(VALUE range, VALUE val)
{
    return rb_funcall(range, rb_intern("include?"), 1, val);
}


/*
 *  call-seq:
 *     rng.member?(obj)  ->  true or false
 *     rng.include?(obj) ->  true or false
 *
 *  Returns <code>true</code> if +obj+ is an element of
 *  the range, <code>false</code> otherwise.  If begin and end are
 *  numeric, comparison is done according to the magnitude of the values.
 *
 *     ("a".."z").include?("g")   #=> true
 *     ("a".."z").include?("A")   #=> false
 *     ("a".."z").include?("cc")  #=> false
 */

static VALUE
range_include(VALUE range, VALUE val)
{
    VALUE beg = RANGE_BEG(range);
    VALUE end = RANGE_END(range);
    int nv = FIXNUM_P(beg) || FIXNUM_P(end) ||
	     rb_obj_is_kind_of(beg, rb_cNumeric) ||
	     rb_obj_is_kind_of(end, rb_cNumeric);

    if (nv ||
	!NIL_P(rb_check_to_integer(beg, "to_int")) ||
	!NIL_P(rb_check_to_integer(end, "to_int"))) {
	if (r_le(beg, val)) {
	    if (EXCL(range)) {
		if (r_lt(val, end))
		    return Qtrue;
	    }
	    else {
		if (r_le(val, end))
		    return Qtrue;
	    }
	}
	return Qfalse;
    }
    else if (RB_TYPE_P(beg, T_STRING) && RB_TYPE_P(end, T_STRING) &&
	     RSTRING_LEN(beg) == 1 && RSTRING_LEN(end) == 1) {
	if (NIL_P(val)) return Qfalse;
	if (RB_TYPE_P(val, T_STRING)) {
	    if (RSTRING_LEN(val) == 0 || RSTRING_LEN(val) > 1)
		return Qfalse;
	    else {
		char b = RSTRING_PTR(beg)[0];
		char e = RSTRING_PTR(end)[0];
		char v = RSTRING_PTR(val)[0];

		if (ISASCII(b) && ISASCII(e) && ISASCII(v)) {
		    if (b <= v && v < e) return Qtrue;
		    if (!EXCL(range) && v == e) return Qtrue;
		    return Qfalse;
		}
	    }
	}
    }
    /* TODO: ruby_frame->this_func = rb_intern("include?"); */
    return rb_call_super(1, &val);
}


/*
 *  call-seq:
 *     rng.cover?(obj)  ->  true or false
 *
 *  Returns <code>true</code> if +obj+ is between the begin and end of
 *  the range.
 *
 *  This tests <code>begin <= obj <= end</code> when #exclude_end? is +false+
 *  and <code>begin <= obj < end</code> when #exclude_end? is +true+.
 *
 *     ("a".."z").cover?("c")    #=> true
 *     ("a".."z").cover?("5")    #=> false
 *     ("a".."z").cover?("cc")   #=> true
 */

static VALUE
range_cover(VALUE range, VALUE val)
{
    VALUE beg, end;

    beg = RANGE_BEG(range);
    end = RANGE_END(range);
    if (r_le(beg, val)) {
	if (EXCL(range)) {
	    if (r_lt(val, end))
		return Qtrue;
	}
	else {
	    if (r_le(val, end))
		return Qtrue;
	}
    }
    return Qfalse;
}

static VALUE
range_dumper(VALUE range)
{
    VALUE v;
    NEWOBJ(m, struct RObject);
    OBJSETUP(m, rb_cObject, T_OBJECT);

    v = (VALUE)m;

    rb_ivar_set(v, id_excl, RANGE_EXCL(range));
    rb_ivar_set(v, id_beg, RANGE_BEG(range));
    rb_ivar_set(v, id_end, RANGE_END(range));
    return v;
}

static VALUE
range_loader(VALUE range, VALUE obj)
{
    if (!RB_TYPE_P(obj, T_OBJECT) || RBASIC(obj)->klass != rb_cObject) {
        rb_raise(rb_eTypeError, "not a dumped range object");
    }

    RSTRUCT(range)->as.ary[0] = rb_ivar_get(obj, id_beg);
    RSTRUCT(range)->as.ary[1] = rb_ivar_get(obj, id_end);
    RSTRUCT(range)->as.ary[2] = rb_ivar_get(obj, id_excl);
    return range;
}

static VALUE
range_alloc(VALUE klass)
{
  /* rb_struct_alloc_noinit itself should not be used because
   * rb_marshal_define_compat uses equality of allocaiton function */
    return rb_struct_alloc_noinit(klass);
}

/*  A <code>Range</code> represents an interval---a set of values with a
 *  beginning and an end. Ranges may be constructed using the
 *  <em>s</em><code>..</code><em>e</em> and
 *  <em>s</em><code>...</code><em>e</em> literals, or with
 *  Range::new. Ranges constructed using <code>..</code>
 *  run from the beginning to the end inclusively. Those created using
 *  <code>...</code> exclude the end value. When used as an iterator,
 *  ranges return each value in the sequence.
 *
 *     (-1..-5).to_a      #=> []
 *     (-5..-1).to_a      #=> [-5, -4, -3, -2, -1]
 *     ('a'..'e').to_a    #=> ["a", "b", "c", "d", "e"]
 *     ('a'...'e').to_a   #=> ["a", "b", "c", "d"]
 *
 *  == Custom Objects in Ranges
 *
 *  Ranges can be constructed using any objects that can be compared
 *  using the <code><=></code> operator.
 *  Methods that treat the range as a sequence (#each and methods inherited
 *  from Enumerable) expect the begin object to implement a
 *  <code>succ</code> method to return the next object in sequence.
 *  The #step and #include? methods require the begin
 *  object to implement <code>succ</code> or to be numeric.
 *
 *  In the <code>Xs</code> class below both <code><=></code> and
 *  <code>succ</code> are implemented so <code>Xs</code> can be used
 *  to construct ranges. Note that the Comparable module is included
 *  so the <code>==</code> method is defined in terms of <code><=></code>.
 *
 *     class Xs                # represent a string of 'x's
 *       include Comparable
 *       attr :length
 *       def initialize(n)
 *         @length = n
 *       end
 *       def succ
 *         Xs.new(@length + 1)
 *       end
 *       def <=>(other)
 *         @length <=> other.length
 *       end
 *       def to_s
 *         sprintf "%2d #{inspect}", @length
 *       end
 *       def inspect
 *         'x' * @length
 *       end
 *     end
 *
 *  An example of using <code>Xs</code> to construct a range:
 *
 *     r = Xs.new(3)..Xs.new(6)   #=> xxx..xxxxxx
 *     r.to_a                     #=> [xxx, xxxx, xxxxx, xxxxxx]
 *     r.member?(Xs.new(5))       #=> true
 *
 */

void
Init_Range(void)
{
#undef rb_intern
#define rb_intern(str) rb_intern_const(str)

    id_cmp = rb_intern("<=>");
    id_succ = rb_intern("succ");
    id_beg = rb_intern("begin");
    id_end = rb_intern("end");
    id_excl = rb_intern("excl");

    rb_cRange = rb_struct_define_without_accessor(
        "Range", rb_cObject, range_alloc,
        "begin", "end", "excl", NULL);

    rb_include_module(rb_cRange, rb_mEnumerable);
    rb_marshal_define_compat(rb_cRange, rb_cObject, range_dumper, range_loader);
    rb_define_method(rb_cRange, "initialize", range_initialize, -1);
    rb_define_method(rb_cRange, "initialize_copy", range_initialize_copy, 1);
    rb_define_method(rb_cRange, "==", range_eq, 1);
    rb_define_method(rb_cRange, "===", range_eqq, 1);
    rb_define_method(rb_cRange, "eql?", range_eql, 1);
    rb_define_method(rb_cRange, "hash", range_hash, 0);
    rb_define_method(rb_cRange, "each", range_each, 0);
    rb_define_method(rb_cRange, "step", range_step, -1);
    rb_define_method(rb_cRange, "begin", range_begin, 0);
    rb_define_method(rb_cRange, "end", range_end, 0);
    rb_define_method(rb_cRange, "first", range_first, -1);
    rb_define_method(rb_cRange, "last", range_last, -1);
    rb_define_method(rb_cRange, "min", range_min, 0);
    rb_define_method(rb_cRange, "max", range_max, 0);
    rb_define_method(rb_cRange, "to_s", range_to_s, 0);
    rb_define_method(rb_cRange, "inspect", range_inspect, 0);

    rb_define_method(rb_cRange, "exclude_end?", range_exclude_end_p, 0);

    rb_define_method(rb_cRange, "member?", range_include, 1);
    rb_define_method(rb_cRange, "include?", range_include, 1);
    rb_define_method(rb_cRange, "cover?", range_cover, 1);
}
