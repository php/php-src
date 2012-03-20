/**********************************************************************

  enum.c -

  $Author$
  created at: Fri Oct  1 15:15:19 JST 1993

  Copyright (C) 1993-2007 Yukihiro Matsumoto

**********************************************************************/

#include "ruby/ruby.h"
#include "ruby/util.h"
#include "node.h"
#include "id.h"
#include "internal.h"

#define STATIC_ASSERT(name, expr) typedef int static_assert_##name##_check[1 - 2*!(expr)]

VALUE rb_mEnumerable;
static ID id_next;
#define id_each idEach
#define id_eqq  idEqq
#define id_cmp  idCmp
#define id_lshift idLTLT

VALUE
rb_enum_values_pack(int argc, VALUE *argv)
{
    if (argc == 0) return Qnil;
    if (argc == 1) return argv[0];
    return rb_ary_new4(argc, argv);
}

#define ENUM_WANT_SVALUE() do { \
    i = rb_enum_values_pack(argc, argv); \
} while (0)

#define enum_yield rb_yield_values2

static VALUE
grep_i(VALUE i, VALUE args, int argc, VALUE *argv)
{
    NODE *memo = RNODE(args);
    ENUM_WANT_SVALUE();

    if (RTEST(rb_funcall(memo->u1.value, id_eqq, 1, i))) {
	rb_ary_push(memo->u2.value, i);
    }
    return Qnil;
}

static VALUE
grep_iter_i(VALUE i, VALUE args, int argc, VALUE *argv)
{
    NODE *memo = RNODE(args);
    ENUM_WANT_SVALUE();

    if (RTEST(rb_funcall(memo->u1.value, id_eqq, 1, i))) {
	rb_ary_push(memo->u2.value, rb_yield(i));
    }
    return Qnil;
}

/*
 *  call-seq:
 *     enum.grep(pattern)                  -> array
 *     enum.grep(pattern) { |obj| block }  -> array
 *
 *  Returns an array of every element in <i>enum</i> for which
 *  <code>Pattern === element</code>. If the optional <em>block</em> is
 *  supplied, each matching element is passed to it, and the block's
 *  result is stored in the output array.
 *
 *     (1..100).grep 38..44   #=> [38, 39, 40, 41, 42, 43, 44]
 *     c = IO.constants
 *     c.grep(/SEEK/)         #=> [:SEEK_SET, :SEEK_CUR, :SEEK_END]
 *     res = c.grep(/SEEK/) { |v| IO.const_get(v) }
 *     res                    #=> [0, 1, 2]
 *
 */

static VALUE
enum_grep(VALUE obj, VALUE pat)
{
    VALUE ary = rb_ary_new();
    NODE *memo = NEW_MEMO(pat, ary, 0);

    rb_block_call(obj, id_each, 0, 0, rb_block_given_p() ? grep_iter_i : grep_i, (VALUE)memo);

    return ary;
}

static VALUE
count_i(VALUE i, VALUE memop, int argc, VALUE *argv)
{
    NODE *memo = RNODE(memop);

    ENUM_WANT_SVALUE();

    if (rb_equal(i, memo->u1.value)) {
	memo->u3.cnt++;
    }
    return Qnil;
}

static VALUE
count_iter_i(VALUE i, VALUE memop, int argc, VALUE *argv)
{
    NODE *memo = RNODE(memop);

    if (RTEST(enum_yield(argc, argv))) {
	memo->u3.cnt++;
    }
    return Qnil;
}

static VALUE
count_all_i(VALUE i, VALUE memop, int argc, VALUE *argv)
{
    NODE *memo = RNODE(memop);

    memo->u3.cnt++;
    return Qnil;
}

/*
 *  call-seq:
 *     enum.count                 -> int
 *     enum.count(item)           -> int
 *     enum.count { |obj| block } -> int
 *
 *  Returns the number of items in <i>enum</i> if it responds to a #size call,
 *  otherwise the items are counted through enumeration.  If an argument is
 *  given the number of items in <i>enum</i> that are equal to <i>item</i> are
 *  counted.  If a block is given, it counts the number of elements yielding a
 *  true value.
 *
 *     ary = [1, 2, 4, 2]
 *     ary.count               #=> 4
 *     ary.count(2)            #=> 2
 *     ary.count{ |x| x%2==0 } #=> 3
 *
 */

static VALUE
enum_count(int argc, VALUE *argv, VALUE obj)
{
    VALUE item = Qnil;
    NODE *memo;
    rb_block_call_func *func;

    if (argc == 0) {
	if (rb_block_given_p()) {
	    func = count_iter_i;
	}
	else {
	    func = count_all_i;
	}
    }
    else {
	rb_scan_args(argc, argv, "1", &item);
	if (rb_block_given_p()) {
	    rb_warn("given block not used");
	}
        func = count_i;
    }

    memo = NEW_MEMO(item, 0, 0);
    rb_block_call(obj, id_each, 0, 0, func, (VALUE)memo);
    return INT2NUM(memo->u3.cnt);
}

static VALUE
find_i(VALUE i, VALUE memop, int argc, VALUE *argv)
{
    ENUM_WANT_SVALUE();

    if (RTEST(rb_yield(i))) {
	NODE *memo = RNODE(memop);
	memo->u1.value = i;
	memo->u3.cnt = 1;
	rb_iter_break();
    }
    return Qnil;
}

/*
 *  call-seq:
 *     enum.detect(ifnone = nil) { |obj| block } -> obj or nil
 *     enum.find(ifnone = nil)   { |obj| block } -> obj or nil
 *     enum.detect(ifnone = nil)                 -> an_enumerator
 *     enum.find(ifnone = nil)                   -> an_enumerator
 *
 *  Passes each entry in <i>enum</i> to <em>block</em>. Returns the
 *  first for which <em>block</em> is not false.  If no
 *  object matches, calls <i>ifnone</i> and returns its result when it
 *  is specified, or returns <code>nil</code> otherwise.
 *
 *  If no block is given, an enumerator is returned instead.
 *
 *     (1..10).detect  { |i| i % 5 == 0 and i % 7 == 0 }   #=> nil
 *     (1..100).detect { |i| i % 5 == 0 and i % 7 == 0 }   #=> 35
 *
 */

static VALUE
enum_find(int argc, VALUE *argv, VALUE obj)
{
    NODE *memo;
    VALUE if_none;

    rb_scan_args(argc, argv, "01", &if_none);
    RETURN_ENUMERATOR(obj, argc, argv);
    memo = NEW_MEMO(Qundef, 0, 0);
    rb_block_call(obj, id_each, 0, 0, find_i, (VALUE)memo);
    if (memo->u3.cnt) {
	return memo->u1.value;
    }
    if (!NIL_P(if_none)) {
	return rb_funcall(if_none, rb_intern("call"), 0, 0);
    }
    return Qnil;
}

static VALUE
find_index_i(VALUE i, VALUE memop, int argc, VALUE *argv)
{
    NODE *memo = RNODE(memop);

    ENUM_WANT_SVALUE();

    if (rb_equal(i, memo->u2.value)) {
	memo->u1.value = UINT2NUM(memo->u3.cnt);
	rb_iter_break();
    }
    memo->u3.cnt++;
    return Qnil;
}

static VALUE
find_index_iter_i(VALUE i, VALUE memop, int argc, VALUE *argv)
{
    NODE *memo = RNODE(memop);

    if (RTEST(enum_yield(argc, argv))) {
	memo->u1.value = UINT2NUM(memo->u3.cnt);
	rb_iter_break();
    }
    memo->u3.cnt++;
    return Qnil;
}

/*
 *  call-seq:
 *     enum.find_index(value)          -> int or nil
 *     enum.find_index { |obj| block } -> int or nil
 *     enum.find_index                 -> an_enumerator
 *
 *  Compares each entry in <i>enum</i> with <em>value</em> or passes
 *  to <em>block</em>.  Returns the index for the first for which the
 *  evaluated value is non-false.  If no object matches, returns
 *  <code>nil</code>
 *
 *  If neither block nor argument is given, an enumerator is returned instead.
 *
 *     (1..10).find_index  { |i| i % 5 == 0 and i % 7 == 0 }  #=> nil
 *     (1..100).find_index { |i| i % 5 == 0 and i % 7 == 0 }  #=> 34
 *     (1..100).find_index(50)                                #=> 49
 *
 */

static VALUE
enum_find_index(int argc, VALUE *argv, VALUE obj)
{
    NODE *memo;	/* [return value, current index, ] */
    VALUE condition_value = Qnil;
    rb_block_call_func *func;

    if (argc == 0) {
        RETURN_ENUMERATOR(obj, 0, 0);
        func = find_index_iter_i;
    }
    else {
	rb_scan_args(argc, argv, "1", &condition_value);
	if (rb_block_given_p()) {
	    rb_warn("given block not used");
	}
        func = find_index_i;
    }

    memo = NEW_MEMO(Qnil, condition_value, 0);
    rb_block_call(obj, id_each, 0, 0, func, (VALUE)memo);
    return memo->u1.value;
}

static VALUE
find_all_i(VALUE i, VALUE ary, int argc, VALUE *argv)
{
    ENUM_WANT_SVALUE();

    if (RTEST(rb_yield(i))) {
	rb_ary_push(ary, i);
    }
    return Qnil;
}

/*
 *  call-seq:
 *     enum.find_all { |obj| block } -> array
 *     enum.select   { |obj| block } -> array
 *     enum.find_all                 -> an_enumerator
 *     enum.select                   -> an_enumerator
 *
 *  Returns an array containing all elements of <i>enum</i> for which
 *  <em>block</em> is not <code>false</code> (see also
 *  <code>Enumerable#reject</code>).
 *
 *  If no block is given, an enumerator is returned instead.
 *
 *
 *     (1..10).find_all { |i|  i % 3 == 0 }   #=> [3, 6, 9]
 *
 */

static VALUE
enum_find_all(VALUE obj)
{
    VALUE ary;

    RETURN_ENUMERATOR(obj, 0, 0);

    ary = rb_ary_new();
    rb_block_call(obj, id_each, 0, 0, find_all_i, ary);

    return ary;
}

static VALUE
reject_i(VALUE i, VALUE ary, int argc, VALUE *argv)
{
    ENUM_WANT_SVALUE();

    if (!RTEST(rb_yield(i))) {
	rb_ary_push(ary, i);
    }
    return Qnil;
}

/*
 *  call-seq:
 *     enum.reject { |obj| block } -> array
 *     enum.reject                 -> an_enumerator
 *
 *  Returns an array for all elements of <i>enum</i> for which
 *  <em>block</em> is false (see also <code>Enumerable#find_all</code>).
 *
 *  If no block is given, an enumerator is returned instead.
 *
 *     (1..10).reject { |i|  i % 3 == 0 }   #=> [1, 2, 4, 5, 7, 8, 10]
 *
 */

static VALUE
enum_reject(VALUE obj)
{
    VALUE ary;

    RETURN_ENUMERATOR(obj, 0, 0);

    ary = rb_ary_new();
    rb_block_call(obj, id_each, 0, 0, reject_i, ary);

    return ary;
}

static VALUE
collect_i(VALUE i, VALUE ary, int argc, VALUE *argv)
{
    rb_ary_push(ary, enum_yield(argc, argv));

    return Qnil;
}

static VALUE
collect_all(VALUE i, VALUE ary, int argc, VALUE *argv)
{
    rb_thread_check_ints();
    rb_ary_push(ary, rb_enum_values_pack(argc, argv));

    return Qnil;
}

/*
 *  call-seq:
 *     enum.collect { |obj| block } -> array
 *     enum.map     { |obj| block } -> array
 *     enum.collect                 -> an_enumerator
 *     enum.map                     -> an_enumerator
 *
 *  Returns a new array with the results of running <em>block</em> once
 *  for every element in <i>enum</i>.
 *
 *  If no block is given, an enumerator is returned instead.
 *
 *     (1..4).collect { |i| i*i }  #=> [1, 4, 9, 16]
 *     (1..4).collect { "cat"  }   #=> ["cat", "cat", "cat", "cat"]
 *
 */

static VALUE
enum_collect(VALUE obj)
{
    VALUE ary;

    RETURN_ENUMERATOR(obj, 0, 0);

    ary = rb_ary_new();
    rb_block_call(obj, id_each, 0, 0, collect_i, ary);

    return ary;
}

static VALUE
flat_map_i(VALUE i, VALUE ary, int argc, VALUE *argv)
{
    VALUE tmp;

    i = enum_yield(argc, argv);
    tmp = rb_check_array_type(i);

    if (NIL_P(tmp)) {
	rb_ary_push(ary, i);
    }
    else {
	rb_ary_concat(ary, tmp);
    }
    return Qnil;
}

/*
 *  call-seq:
 *     enum.flat_map       { |obj| block } -> array
 *     enum.collect_concat { |obj| block } -> array
 *     enum.flat_map                       -> an_enumerator
 *     enum.collect_concat                 -> an_enumerator
 *
 *  Returns a new array with the concatenated results of running
 *  <em>block</em> once for every element in <i>enum</i>.
 *
 *  If no block is given, an enumerator is returned instead.
 *
 *     [1, 2, 3, 4].flat_map { |e| [e, -e] } #=> [1, -1, 2, -2, 3, -3, 4, -4]
 *     [[1, 2], [3, 4]].flat_map { |e| e + [100] } #=> [1, 2, 100, 3, 4, 100]
 *
 */

static VALUE
enum_flat_map(VALUE obj)
{
    VALUE ary;

    RETURN_ENUMERATOR(obj, 0, 0);

    ary = rb_ary_new();
    rb_block_call(obj, id_each, 0, 0, flat_map_i, ary);

    return ary;
}

/*
 *  call-seq:
 *     enum.to_a      -> array
 *     enum.entries   -> array
 *
 *  Returns an array containing the items in <i>enum</i>.
 *
 *     (1..7).to_a                       #=> [1, 2, 3, 4, 5, 6, 7]
 *     { 'a'=>1, 'b'=>2, 'c'=>3 }.to_a   #=> [["a", 1], ["b", 2], ["c", 3]]
 */
static VALUE
enum_to_a(int argc, VALUE *argv, VALUE obj)
{
    VALUE ary = rb_ary_new();

    rb_block_call(obj, id_each, argc, argv, collect_all, ary);
    OBJ_INFECT(ary, obj);

    return ary;
}

static VALUE
inject_i(VALUE i, VALUE p, int argc, VALUE *argv)
{
    NODE *memo = RNODE(p);

    ENUM_WANT_SVALUE();

    if (memo->u2.argc == 0) {
	memo->u2.argc = 1;
	memo->u1.value = i;
    }
    else {
	memo->u1.value = rb_yield_values(2, memo->u1.value, i);
    }
    return Qnil;
}

static VALUE
inject_op_i(VALUE i, VALUE p, int argc, VALUE *argv)
{
    NODE *memo = RNODE(p);

    ENUM_WANT_SVALUE();

    if (memo->u2.argc == 0) {
	memo->u2.argc = 1;
	memo->u1.value = i;
    }
    else {
	memo->u1.value = rb_funcall(memo->u1.value, memo->u3.id, 1, i);
    }
    return Qnil;
}

/*
 *  call-seq:
 *     enum.inject(initial, sym) -> obj
 *     enum.inject(sym)          -> obj
 *     enum.inject(initial) { |memo, obj| block }  -> obj
 *     enum.inject          { |memo, obj| block }  -> obj
 *     enum.reduce(initial, sym) -> obj
 *     enum.reduce(sym)          -> obj
 *     enum.reduce(initial) { |memo, obj| block }  -> obj
 *     enum.reduce          { |memo, obj| block }  -> obj
 *
 *  Combines all elements of <i>enum</i> by applying a binary
 *  operation, specified by a block or a symbol that names a
 *  method or operator.
 *
 *  If you specify a block, then for each element in <i>enum</i>
 *  the block is passed an accumulator value (<i>memo</i>) and the element.
 *  If you specify a symbol instead, then each element in the collection
 *  will be passed to the named method of <i>memo</i>.
 *  In either case, the result becomes the new value for <i>memo</i>.
 *  At the end of the iteration, the final value of <i>memo</i> is the
 *  return value for the method.
 *
 *  If you do not explicitly specify an <i>initial</i> value for <i>memo</i>,
 *  then the first element of collection is used as the initial value
 *  of <i>memo</i>.
 *
 *
 *     # Sum some numbers
 *     (5..10).reduce(:+)                             #=> 45
 *     # Same using a block and inject
 *     (5..10).inject { |sum, n| sum + n }            #=> 45
 *     # Multiply some numbers
 *     (5..10).reduce(1, :*)                          #=> 151200
 *     # Same using a block
 *     (5..10).inject(1) { |product, n| product * n } #=> 151200
 *     # find the longest word
 *     longest = %w{ cat sheep bear }.inject do |memo, word|
 *        memo.length > word.length ? memo : word
 *     end
 *     longest                                        #=> "sheep"
 *
 */
static VALUE
enum_inject(int argc, VALUE *argv, VALUE obj)
{
    NODE *memo;
    VALUE init, op;
    VALUE (*iter)(VALUE, VALUE, int, VALUE*) = inject_i;

    switch (rb_scan_args(argc, argv, "02", &init, &op)) {
      case 0:
	break;
      case 1:
	if (rb_block_given_p()) {
	    break;
	}
	op = (VALUE)rb_to_id(init);
	argc = 0;
	init = Qnil;
	iter = inject_op_i;
	break;
      case 2:
	if (rb_block_given_p()) {
	    rb_warning("given block not used");
	}
	op = (VALUE)rb_to_id(op);
	iter = inject_op_i;
	break;
    }
    memo = NEW_MEMO(init, argc, op);
    rb_block_call(obj, id_each, 0, 0, iter, (VALUE)memo);
    return memo->u1.value;
}

static VALUE
partition_i(VALUE i, VALUE arys, int argc, VALUE *argv)
{
    NODE *memo = RNODE(arys);
    VALUE ary;
    ENUM_WANT_SVALUE();

    if (RTEST(rb_yield(i))) {
	ary = memo->u1.value;
    }
    else {
	ary = memo->u2.value;
    }
    rb_ary_push(ary, i);
    return Qnil;
}

/*
 *  call-seq:
 *     enum.partition { |obj| block } -> [ true_array, false_array ]
 *     enum.partition                 -> an_enumerator
 *
 *  Returns two arrays, the first containing the elements of
 *  <i>enum</i> for which the block evaluates to true, the second
 *  containing the rest.
 *
 *  If no block is given, an enumerator is returned instead.
 *
 *     (1..6).partition { |v| v.even? }  #=> [[2, 4, 6], [1, 3, 5]]
 *
 */

static VALUE
enum_partition(VALUE obj)
{
    NODE *memo;

    RETURN_ENUMERATOR(obj, 0, 0);

    memo = NEW_MEMO(rb_ary_new(), rb_ary_new(), 0);
    rb_block_call(obj, id_each, 0, 0, partition_i, (VALUE)memo);

    return rb_assoc_new(memo->u1.value, memo->u2.value);
}

static VALUE
group_by_i(VALUE i, VALUE hash, int argc, VALUE *argv)
{
    VALUE group;
    VALUE values;

    ENUM_WANT_SVALUE();

    group = rb_yield(i);
    values = rb_hash_aref(hash, group);
    if (!RB_TYPE_P(values, T_ARRAY)) {
	values = rb_ary_new3(1, i);
	rb_hash_aset(hash, group, values);
    }
    else {
	rb_ary_push(values, i);
    }
    return Qnil;
}

/*
 *  call-seq:
 *     enum.group_by { |obj| block } -> a_hash
 *     enum.group_by                 -> an_enumerator
 *
 *  Groups the collection by result of the block.  Returns a hash where the
 *  keys are the evaluated result from the block and the values are
 *  arrays of elements in the collection that correspond to the key.
 *
 *  If no block is given an enumerator is returned.
 *
 *     (1..6).group_by { |i| i%3 }   #=> {0=>[3, 6], 1=>[1, 4], 2=>[2, 5]}
 *
 */

static VALUE
enum_group_by(VALUE obj)
{
    VALUE hash;

    RETURN_ENUMERATOR(obj, 0, 0);

    hash = rb_hash_new();
    rb_block_call(obj, id_each, 0, 0, group_by_i, hash);
    OBJ_INFECT(hash, obj);

    return hash;
}

static VALUE
first_i(VALUE i, VALUE params, int argc, VALUE *argv)
{
    NODE *memo = RNODE(params);
    ENUM_WANT_SVALUE();

    memo->u1.value = i;
    rb_iter_break();
    return Qnil;		/* not reached */
}

static VALUE enum_take(VALUE obj, VALUE n);

/*
 *  call-seq:
 *     enum.first       ->  obj or nil
 *     enum.first(n)    ->  an_array
 *
 *  Returns the first element, or the first +n+ elements, of the enumerable.
 *  If the enumerable is empty, the first form returns <code>nil</code>, and the
 *  second form returns an empty array.
 *
 *    %w[foo bar baz].first     #=> "foo"
 *    %w[foo bar baz].first(2)  #=> ["foo", "bar"]
 *    %w[foo bar baz].first(10) #=> ["foo", "bar", "baz"]
 *    [].first                  #=> nil
 *
 */

static VALUE
enum_first(int argc, VALUE *argv, VALUE obj)
{
    NODE *memo;
    rb_check_arity(argc, 0, 1);
    if (argc > 0) {
	return enum_take(obj, argv[0]);
    }
    else {
	memo = NEW_MEMO(Qnil, 0, 0);
	rb_block_call(obj, id_each, 0, 0, first_i, (VALUE)memo);
	return memo->u1.value;
    }
}


/*
 *  call-seq:
 *     enum.sort                  -> array
 *     enum.sort { |a, b| block } -> array
 *
 *  Returns an array containing the items in <i>enum</i> sorted,
 *  either according to their own <code><=></code> method, or by using
 *  the results of the supplied block. The block should return -1, 0, or
 *  +1 depending on the comparison between <i>a</i> and <i>b</i>. As of
 *  Ruby 1.8, the method <code>Enumerable#sort_by</code> implements a
 *  built-in Schwartzian Transform, useful when key computation or
 *  comparison is expensive.
 *
 *     %w(rhea kea flea).sort          #=> ["flea", "kea", "rhea"]
 *     (1..10).sort { |a, b| b <=> a }  #=> [10, 9, 8, 7, 6, 5, 4, 3, 2, 1]
 */

static VALUE
enum_sort(VALUE obj)
{
    return rb_ary_sort(enum_to_a(0, 0, obj));
}

#define SORT_BY_BUFSIZE 16
struct sort_by_data {
    VALUE ary;
    VALUE buf;
    long n;
};

static VALUE
sort_by_i(VALUE i, VALUE _data, int argc, VALUE *argv)
{
    struct sort_by_data *data = (struct sort_by_data *)&RNODE(_data)->u1;
    VALUE ary = data->ary;
    VALUE v;

    ENUM_WANT_SVALUE();

    v = rb_yield(i);

    if (RBASIC(ary)->klass) {
	rb_raise(rb_eRuntimeError, "sort_by reentered");
    }
    if (RARRAY_LEN(data->buf) != SORT_BY_BUFSIZE*2) {
	rb_raise(rb_eRuntimeError, "sort_by reentered");
    }

    RARRAY_PTR(data->buf)[data->n*2] = v;
    RARRAY_PTR(data->buf)[data->n*2+1] = i;
    data->n++;
    if (data->n == SORT_BY_BUFSIZE) {
	rb_ary_concat(ary, data->buf);
	data->n = 0;
    }
    return Qnil;
}

static int
sort_by_cmp(const void *ap, const void *bp, void *data)
{
    VALUE a;
    VALUE b;
    VALUE ary = (VALUE)data;

    if (RBASIC(ary)->klass) {
	rb_raise(rb_eRuntimeError, "sort_by reentered");
    }

    a = *(VALUE *)ap;
    b = *(VALUE *)bp;

    return rb_cmpint(rb_funcall(a, id_cmp, 1, b), a, b);
}

/*
 *  call-seq:
 *     enum.sort_by { |obj| block }   -> array
 *     enum.sort_by                   -> an_enumerator
 *
 *  Sorts <i>enum</i> using a set of keys generated by mapping the
 *  values in <i>enum</i> through the given block.
 *
 *  If no block is given, an enumerator is returned instead.
 *
 *     %w{apple pear fig}.sort_by { |word| word.length}
 *                   #=> ["fig", "pear", "apple"]
 *
 *  The current implementation of <code>sort_by</code> generates an
 *  array of tuples containing the original collection element and the
 *  mapped value. This makes <code>sort_by</code> fairly expensive when
 *  the keysets are simple.
 *
 *     require 'benchmark'
 *
 *     a = (1..100000).map { rand(100000) }
 *
 *     Benchmark.bm(10) do |b|
 *       b.report("Sort")    { a.sort }
 *       b.report("Sort by") { a.sort_by { |a| a } }
 *     end
 *
 *  <em>produces:</em>
 *
 *     user     system      total        real
 *     Sort        0.180000   0.000000   0.180000 (  0.175469)
 *     Sort by     1.980000   0.040000   2.020000 (  2.013586)
 *
 *  However, consider the case where comparing the keys is a non-trivial
 *  operation. The following code sorts some files on modification time
 *  using the basic <code>sort</code> method.
 *
 *     files = Dir["*"]
 *     sorted = files.sort { |a, b| File.new(a).mtime <=> File.new(b).mtime }
 *     sorted   #=> ["mon", "tues", "wed", "thurs"]
 *
 *  This sort is inefficient: it generates two new <code>File</code>
 *  objects during every comparison. A slightly better technique is to
 *  use the <code>Kernel#test</code> method to generate the modification
 *  times directly.
 *
 *     files = Dir["*"]
 *     sorted = files.sort { |a, b|
 *       test(?M, a) <=> test(?M, b)
 *     }
 *     sorted   #=> ["mon", "tues", "wed", "thurs"]
 *
 *  This still generates many unnecessary <code>Time</code> objects. A
 *  more efficient technique is to cache the sort keys (modification
 *  times in this case) before the sort. Perl users often call this
 *  approach a Schwartzian Transform, after Randal Schwartz. We
 *  construct a temporary array, where each element is an array
 *  containing our sort key along with the filename. We sort this array,
 *  and then extract the filename from the result.
 *
 *     sorted = Dir["*"].collect { |f|
 *        [test(?M, f), f]
 *     }.sort.collect { |f| f[1] }
 *     sorted   #=> ["mon", "tues", "wed", "thurs"]
 *
 *  This is exactly what <code>sort_by</code> does internally.
 *
 *     sorted = Dir["*"].sort_by { |f| test(?M, f) }
 *     sorted   #=> ["mon", "tues", "wed", "thurs"]
 */

static VALUE
enum_sort_by(VALUE obj)
{
    VALUE ary, buf;
    NODE *memo;
    long i;
    struct sort_by_data *data;

    RETURN_ENUMERATOR(obj, 0, 0);

    if (RB_TYPE_P(obj, T_ARRAY) && RARRAY_LEN(obj) <= LONG_MAX/2) {
	ary = rb_ary_new2(RARRAY_LEN(obj)*2);
    }
    else {
	ary = rb_ary_new();
    }
    RBASIC(ary)->klass = 0;
    buf = rb_ary_tmp_new(SORT_BY_BUFSIZE*2);
    rb_ary_store(buf, SORT_BY_BUFSIZE*2-1, Qnil);
    memo = NEW_MEMO(0, 0, 0);
    OBJ_INFECT(memo, obj);
    data = (struct sort_by_data *)&memo->u1;
    data->ary = ary;
    data->buf = buf;
    data->n = 0;
    rb_block_call(obj, id_each, 0, 0, sort_by_i, (VALUE)memo);
    ary = data->ary;
    buf = data->buf;
    if (data->n) {
	rb_ary_resize(buf, data->n*2);
	rb_ary_concat(ary, buf);
    }
    if (RARRAY_LEN(ary) > 2) {
	ruby_qsort(RARRAY_PTR(ary), RARRAY_LEN(ary)/2, 2*sizeof(VALUE),
		   sort_by_cmp, (void *)ary);
    }
    if (RBASIC(ary)->klass) {
	rb_raise(rb_eRuntimeError, "sort_by reentered");
    }
    for (i=1; i<RARRAY_LEN(ary); i+=2) {
	RARRAY_PTR(ary)[i/2] = RARRAY_PTR(ary)[i];
    }
    rb_ary_resize(ary, RARRAY_LEN(ary)/2);
    RBASIC(ary)->klass = rb_cArray;
    OBJ_INFECT(ary, memo);

    return ary;
}

#define ENUMFUNC(name) rb_block_given_p() ? name##_iter_i : name##_i

#define DEFINE_ENUMFUNCS(name) \
static VALUE enum_##name##_func(VALUE result, NODE *memo); \
\
static VALUE \
name##_i(VALUE i, VALUE memo, int argc, VALUE *argv) \
{ \
    return enum_##name##_func(rb_enum_values_pack(argc, argv), RNODE(memo)); \
} \
\
static VALUE \
name##_iter_i(VALUE i, VALUE memo, int argc, VALUE *argv) \
{ \
    return enum_##name##_func(enum_yield(argc, argv), RNODE(memo));	\
} \
\
static VALUE \
enum_##name##_func(VALUE result, NODE *memo)

DEFINE_ENUMFUNCS(all)
{
    if (!RTEST(result)) {
	memo->u1.value = Qfalse;
	rb_iter_break();
    }
    return Qnil;
}

/*
 *  call-seq:
 *     enum.all? [{ |obj| block } ]   -> true or false
 *
 *  Passes each element of the collection to the given block. The method
 *  returns <code>true</code> if the block never returns
 *  <code>false</code> or <code>nil</code>. If the block is not given,
 *  Ruby adds an implicit block of <code>{ |obj| obj }</code> which will
 *  cause #all? to return +true+ when none of the collection members are
 *  +false+ or +nil+.
 *
 *     %w[ant bear cat].all? { |word| word.length >= 3 } #=> true
 *     %w[ant bear cat].all? { |word| word.length >= 4 } #=> false
 *     [nil, true, 99].all?                              #=> false
 *
 */

static VALUE
enum_all(VALUE obj)
{
    NODE *memo = NEW_MEMO(Qtrue, 0, 0);
    rb_block_call(obj, id_each, 0, 0, ENUMFUNC(all), (VALUE)memo);
    return memo->u1.value;
}

DEFINE_ENUMFUNCS(any)
{
    if (RTEST(result)) {
	memo->u1.value = Qtrue;
	rb_iter_break();
    }
    return Qnil;
}

/*
 *  call-seq:
 *     enum.any? [{ |obj| block }]   -> true or false
 *
 *  Passes each element of the collection to the given block. The method
 *  returns <code>true</code> if the block ever returns a value other
 *  than <code>false</code> or <code>nil</code>. If the block is not
 *  given, Ruby adds an implicit block of <code>{ |obj| obj }</code> that
 *  will cause #any? to return +true+ if at least one of the collection
 *  members is not +false+ or +nil+.
 *
 *     %w[ant bear cat].any? { |word| word.length >= 3 } #=> true
 *     %w[ant bear cat].any? { |word| word.length >= 4 } #=> true
 *     [nil, true, 99].any?                              #=> true
 *
 */

static VALUE
enum_any(VALUE obj)
{
    NODE *memo = NEW_MEMO(Qfalse, 0, 0);
    rb_block_call(obj, id_each, 0, 0, ENUMFUNC(any), (VALUE)memo);
    return memo->u1.value;
}

DEFINE_ENUMFUNCS(one)
{
    if (RTEST(result)) {
	if (memo->u1.value == Qundef) {
	    memo->u1.value = Qtrue;
	}
	else if (memo->u1.value == Qtrue) {
	    memo->u1.value = Qfalse;
	    rb_iter_break();
	}
    }
    return Qnil;
}

/*
 *  call-seq:
 *     enum.one? [{ |obj| block }]   -> true or false
 *
 *  Passes each element of the collection to the given block. The method
 *  returns <code>true</code> if the block returns <code>true</code>
 *  exactly once. If the block is not given, <code>one?</code> will return
 *  <code>true</code> only if exactly one of the collection members is
 *  true.
 *
 *     %w{ant bear cat}.one? { |word| word.length == 4 }  #=> true
 *     %w{ant bear cat}.one? { |word| word.length > 4 }   #=> false
 *     %w{ant bear cat}.one? { |word| word.length < 4 }   #=> false
 *     [ nil, true, 99 ].one?                             #=> false
 *     [ nil, true, false ].one?                          #=> true
 *
 */

static VALUE
enum_one(VALUE obj)
{
    NODE *memo = NEW_MEMO(Qundef, 0, 0);
    VALUE result;

    rb_block_call(obj, id_each, 0, 0, ENUMFUNC(one), (VALUE)memo);
    result = memo->u1.value;
    if (result == Qundef) return Qfalse;
    return result;
}

DEFINE_ENUMFUNCS(none)
{
    if (RTEST(result)) {
	memo->u1.value = Qfalse;
	rb_iter_break();
    }
    return Qnil;
}

/*
 *  call-seq:
 *     enum.none? [{ |obj| block }]   -> true or false
 *
 *  Passes each element of the collection to the given block. The method
 *  returns <code>true</code> if the block never returns <code>true</code>
 *  for all elements. If the block is not given, <code>none?</code> will return
 *  <code>true</code> only if none of the collection members is true.
 *
 *     %w{ant bear cat}.none? { |word| word.length == 5 } #=> true
 *     %w{ant bear cat}.none? { |word| word.length >= 4 } #=> false
 *     [].none?                                           #=> true
 *     [nil].none?                                        #=> true
 *     [nil, false].none?                                 #=> true
 */
static VALUE
enum_none(VALUE obj)
{
    NODE *memo = NEW_MEMO(Qtrue, 0, 0);
    rb_block_call(obj, id_each, 0, 0, ENUMFUNC(none), (VALUE)memo);
    return memo->u1.value;
}

static VALUE
min_i(VALUE i, VALUE args, int argc, VALUE *argv)
{
    VALUE cmp;
    NODE *memo = RNODE(args);

    ENUM_WANT_SVALUE();

    if (memo->u1.value == Qundef) {
	memo->u1.value = i;
    }
    else {
	cmp = rb_funcall(i, id_cmp, 1, memo->u1.value);
	if (rb_cmpint(cmp, i, memo->u1.value) < 0) {
	    memo->u1.value = i;
	}
    }
    return Qnil;
}

static VALUE
min_ii(VALUE i, VALUE args, int argc, VALUE *argv)
{
    VALUE cmp;
    NODE *memo = RNODE(args);

    ENUM_WANT_SVALUE();

    if (memo->u1.value == Qundef) {
	memo->u1.value = i;
    }
    else {
	cmp = rb_yield_values(2, i, memo->u1.value);
	if (rb_cmpint(cmp, i, memo->u1.value) < 0) {
	    memo->u1.value = i;
	}
    }
    return Qnil;
}


/*
 *  call-seq:
 *     enum.min                 -> obj
 *     enum.min { |a, b| block } -> obj
 *
 *  Returns the object in <i>enum</i> with the minimum value. The
 *  first form assumes all objects implement <code>Comparable</code>;
 *  the second uses the block to return <em>a <=> b</em>.
 *
 *     a = %w(albatross dog horse)
 *     a.min                                   #=> "albatross"
 *     a.min { |a, b| a.length <=> b.length }  #=> "dog"
 */

static VALUE
enum_min(VALUE obj)
{
    NODE *memo = NEW_MEMO(Qundef, 0, 0);
    VALUE result;

    if (rb_block_given_p()) {
	rb_block_call(obj, id_each, 0, 0, min_ii, (VALUE)memo);
    }
    else {
	rb_block_call(obj, id_each, 0, 0, min_i, (VALUE)memo);
    }
    result = memo->u1.value;
    if (result == Qundef) return Qnil;
    return result;
}

static VALUE
max_i(VALUE i, VALUE args, int argc, VALUE *argv)
{
    NODE *memo = RNODE(args);
    VALUE cmp;

    ENUM_WANT_SVALUE();

    if (memo->u1.value == Qundef) {
	memo->u1.value = i;
    }
    else {
	cmp = rb_funcall(i, id_cmp, 1, memo->u1.value);
	if (rb_cmpint(cmp, i, memo->u1.value) > 0) {
	    memo->u1.value = i;
	}
    }
    return Qnil;
}

static VALUE
max_ii(VALUE i, VALUE args, int argc, VALUE *argv)
{
    NODE *memo = RNODE(args);
    VALUE cmp;

    ENUM_WANT_SVALUE();

    if (memo->u1.value == Qundef) {
	memo->u1.value = i;
    }
    else {
	cmp = rb_yield_values(2, i, memo->u1.value);
	if (rb_cmpint(cmp, i, memo->u1.value) > 0) {
	    memo->u1.value = i;
	}
    }
    return Qnil;
}

/*
 *  call-seq:
 *     enum.max                  -> obj
 *     enum.max { |a, b| block } -> obj
 *
 *  Returns the object in _enum_ with the maximum value. The
 *  first form assumes all objects implement <code>Comparable</code>;
 *  the second uses the block to return <em>a <=> b</em>.
 *
 *     a = %w(albatross dog horse)
 *     a.max                                   #=> "horse"
 *     a.max { |a, b| a.length <=> b.length }  #=> "albatross"
 */

static VALUE
enum_max(VALUE obj)
{
    NODE *memo = NEW_MEMO(Qundef, 0, 0);
    VALUE result;

    if (rb_block_given_p()) {
	rb_block_call(obj, id_each, 0, 0, max_ii, (VALUE)memo);
    }
    else {
	rb_block_call(obj, id_each, 0, 0, max_i, (VALUE)memo);
    }
    result = memo->u1.value;
    if (result == Qundef) return Qnil;
    return result;
}

struct minmax_t {
    VALUE min;
    VALUE max;
    VALUE last;
};

STATIC_ASSERT(minmax_t, sizeof(struct minmax_t) <= sizeof(NODE) - offsetof(NODE, u1));

static void
minmax_i_update(VALUE i, VALUE j, struct minmax_t *memo)
{
    int n;

    if (memo->min == Qundef) {
	memo->min = i;
	memo->max = j;
    }
    else {
	n = rb_cmpint(rb_funcall(i, id_cmp, 1, memo->min), i, memo->min);
	if (n < 0) {
	    memo->min = i;
	}
	n = rb_cmpint(rb_funcall(j, id_cmp, 1, memo->max), j, memo->max);
	if (n > 0) {
	    memo->max = j;
	}
    }
}

static VALUE
minmax_i(VALUE i, VALUE _memo, int argc, VALUE *argv)
{
    struct minmax_t *memo = (struct minmax_t *)&RNODE(_memo)->u1.value;
    int n;
    VALUE j;

    ENUM_WANT_SVALUE();

    if (memo->last == Qundef) {
        memo->last = i;
        return Qnil;
    }
    j = memo->last;
    memo->last = Qundef;

    n = rb_cmpint(rb_funcall(j, id_cmp, 1, i), j, i);
    if (n == 0)
        i = j;
    else if (n < 0) {
        VALUE tmp;
        tmp = i;
        i = j;
        j = tmp;
    }

    minmax_i_update(i, j, memo);

    return Qnil;
}

static void
minmax_ii_update(VALUE i, VALUE j, struct minmax_t *memo)
{
    int n;

    if (memo->min == Qundef) {
	memo->min = i;
	memo->max = j;
    }
    else {
	n = rb_cmpint(rb_yield_values(2, i, memo->min), i, memo->min);
	if (n < 0) {
	    memo->min = i;
	}
	n = rb_cmpint(rb_yield_values(2, j, memo->max), j, memo->max);
	if (n > 0) {
	    memo->max = j;
	}
    }
}

static VALUE
minmax_ii(VALUE i, VALUE _memo, int argc, VALUE *argv)
{
    struct minmax_t *memo = (struct minmax_t *)&RNODE(_memo)->u1.value;
    int n;
    VALUE j;

    ENUM_WANT_SVALUE();

    if (memo->last == Qundef) {
        memo->last = i;
        return Qnil;
    }
    j = memo->last;
    memo->last = Qundef;

    n = rb_cmpint(rb_yield_values(2, j, i), j, i);
    if (n == 0)
        i = j;
    else if (n < 0) {
        VALUE tmp;
        tmp = i;
        i = j;
        j = tmp;
    }

    minmax_ii_update(i, j, memo);

    return Qnil;
}

/*
 *  call-seq:
 *     enum.minmax                  -> [min, max]
 *     enum.minmax { |a, b| block } -> [min, max]
 *
 *  Returns two elements array which contains the minimum and the
 *  maximum value in the enumerable.  The first form assumes all
 *  objects implement <code>Comparable</code>; the second uses the
 *  block to return <em>a <=> b</em>.
 *
 *     a = %w(albatross dog horse)
 *     a.minmax                                  #=> ["albatross", "horse"]
 *     a.minmax { |a, b| a.length <=> b.length } #=> ["dog", "albatross"]
 */

static VALUE
enum_minmax(VALUE obj)
{
    NODE *memo = NEW_MEMO(Qundef, Qundef, Qundef);
    struct minmax_t *m = (struct minmax_t *)&memo->u1.value;
    VALUE ary = rb_ary_new3(2, Qnil, Qnil);

    m->min = Qundef;
    m->last = Qundef;
    if (rb_block_given_p()) {
	rb_block_call(obj, id_each, 0, 0, minmax_ii, (VALUE)memo);
	if (m->last != Qundef)
	    minmax_ii_update(m->last, m->last, m);
    }
    else {
	rb_block_call(obj, id_each, 0, 0, minmax_i, (VALUE)memo);
	if (m->last != Qundef)
	    minmax_i_update(m->last, m->last, m);
    }
    if (m->min != Qundef) {
	rb_ary_store(ary, 0, m->min);
	rb_ary_store(ary, 1, m->max);
    }
    return ary;
}

static VALUE
min_by_i(VALUE i, VALUE args, int argc, VALUE *argv)
{
    NODE *memo = RNODE(args);
    VALUE v;

    ENUM_WANT_SVALUE();

    v = rb_yield(i);
    if (memo->u1.value == Qundef) {
	memo->u1.value = v;
	memo->u2.value = i;
    }
    else if (rb_cmpint(rb_funcall(v, id_cmp, 1, memo->u1.value), v, memo->u1.value) < 0) {
	memo->u1.value = v;
	memo->u2.value = i;
    }
    return Qnil;
}

/*
 *  call-seq:
 *     enum.min_by { |obj| block } -> obj
 *     enum.min_by                 -> an_enumerator
 *
 *  Returns the object in <i>enum</i> that gives the minimum
 *  value from the given block.
 *
 *  If no block is given, an enumerator is returned instead.
 *
 *     a = %w(albatross dog horse)
 *     a.min_by { |x| x.length }   #=> "dog"
 */

static VALUE
enum_min_by(VALUE obj)
{
    NODE *memo;

    RETURN_ENUMERATOR(obj, 0, 0);

    memo = NEW_MEMO(Qundef, Qnil, 0);
    rb_block_call(obj, id_each, 0, 0, min_by_i, (VALUE)memo);
    return memo->u2.value;
}

static VALUE
max_by_i(VALUE i, VALUE args, int argc, VALUE *argv)
{
    NODE *memo = RNODE(args);
    VALUE v;

    ENUM_WANT_SVALUE();

    v = rb_yield(i);
    if (memo->u1.value == Qundef) {
	memo->u1.value = v;
	memo->u2.value = i;
    }
    else if (rb_cmpint(rb_funcall(v, id_cmp, 1, memo->u1.value), v, memo->u1.value) > 0) {
	memo->u1.value = v;
	memo->u2.value = i;
    }
    return Qnil;
}

/*
 *  call-seq:
 *     enum.max_by { |obj| block } -> obj
 *     enum.max_by                 -> an_enumerator
 *
 *  Returns the object in <i>enum</i> that gives the maximum
 *  value from the given block.
 *
 *  If no block is given, an enumerator is returned instead.
 *
 *     a = %w(albatross dog horse)
 *     a.max_by { |x| x.length }   #=> "albatross"
 */

static VALUE
enum_max_by(VALUE obj)
{
    NODE *memo;

    RETURN_ENUMERATOR(obj, 0, 0);

    memo = NEW_MEMO(Qundef, Qnil, 0);
    rb_block_call(obj, id_each, 0, 0, max_by_i, (VALUE)memo);
    return memo->u2.value;
}

struct minmax_by_t {
    VALUE min_bv;
    VALUE max_bv;
    VALUE min;
    VALUE max;
    VALUE last_bv;
    VALUE last;
};

static void
minmax_by_i_update(VALUE v1, VALUE v2, VALUE i1, VALUE i2, struct minmax_by_t *memo)
{
    if (memo->min_bv == Qundef) {
	memo->min_bv = v1;
	memo->max_bv = v2;
	memo->min = i1;
	memo->max = i2;
    }
    else {
	if (rb_cmpint(rb_funcall(v1, id_cmp, 1, memo->min_bv), v1, memo->min_bv) < 0) {
	    memo->min_bv = v1;
	    memo->min = i1;
	}
	if (rb_cmpint(rb_funcall(v2, id_cmp, 1, memo->max_bv), v2, memo->max_bv) > 0) {
	    memo->max_bv = v2;
	    memo->max = i2;
	}
    }
}

static VALUE
minmax_by_i(VALUE i, VALUE _memo, int argc, VALUE *argv)
{
    struct minmax_by_t *memo = MEMO_FOR(struct minmax_by_t, _memo);
    VALUE vi, vj, j;
    int n;

    ENUM_WANT_SVALUE();

    vi = rb_yield(i);

    if (memo->last_bv == Qundef) {
        memo->last_bv = vi;
        memo->last = i;
        return Qnil;
    }
    vj = memo->last_bv;
    j = memo->last;
    memo->last_bv = Qundef;

    n = rb_cmpint(rb_funcall(vj, id_cmp, 1, vi), vj, vi);
    if (n == 0) {
        i = j;
        vi = vj;
    }
    else if (n < 0) {
        VALUE tmp;
        tmp = i;
        i = j;
        j = tmp;
        tmp = vi;
        vi = vj;
        vj = tmp;
    }

    minmax_by_i_update(vi, vj, i, j, memo);

    return Qnil;
}

/*
 *  call-seq:
 *     enum.minmax_by { |obj| block } -> [min, max]
 *     enum.minmax_by                 -> an_enumerator
 *
 *  Returns a two element array containing the objects in
 *  <i>enum</i> that correspond to the minimum and maximum values respectively
 *  from the given block.
 *
 *  If no block is given, an enumerator is returned instead.
 *
 *     a = %w(albatross dog horse)
 *     a.minmax_by { |x| x.length }   #=> ["dog", "albatross"]
 */

static VALUE
enum_minmax_by(VALUE obj)
{
    VALUE memo;
    struct minmax_by_t *m = NEW_MEMO_FOR(struct minmax_by_t, memo);

    RETURN_ENUMERATOR(obj, 0, 0);

    m->min_bv = Qundef;
    m->max_bv = Qundef;
    m->min = Qnil;
    m->max = Qnil;
    m->last_bv = Qundef;
    m->last = Qundef;
    rb_block_call(obj, id_each, 0, 0, minmax_by_i, memo);
    if (m->last_bv != Qundef)
        minmax_by_i_update(m->last_bv, m->last_bv, m->last, m->last, m);
    m = MEMO_FOR(struct minmax_by_t, memo);
    return rb_assoc_new(m->min, m->max);
}

static VALUE
member_i(VALUE iter, VALUE args, int argc, VALUE *argv)
{
    NODE *memo = RNODE(args);

    if (rb_equal(rb_enum_values_pack(argc, argv), memo->u1.value)) {
	memo->u2.value = Qtrue;
	rb_iter_break();
    }
    return Qnil;
}

/*
 *  call-seq:
 *     enum.include?(obj)     -> true or false
 *     enum.member?(obj)      -> true or false
 *
 *  Returns <code>true</code> if any member of <i>enum</i> equals
 *  <i>obj</i>. Equality is tested using <code>==</code>.
 *
 *     IO.constants.include? :SEEK_SET          #=> true
 *     IO.constants.include? :SEEK_NO_FURTHER   #=> false
 *
 */

static VALUE
enum_member(VALUE obj, VALUE val)
{
    NODE *memo = NEW_MEMO(val, Qfalse, 0);

    rb_block_call(obj, id_each, 0, 0, member_i, (VALUE)memo);
    return memo->u2.value;
}

static VALUE
each_with_index_i(VALUE i, VALUE memo, int argc, VALUE *argv)
{
    long n = RNODE(memo)->u3.cnt++;

    return rb_yield_values(2, rb_enum_values_pack(argc, argv), INT2NUM(n));
}

/*
 *  call-seq:
 *     enum.each_with_index(*args) { |obj, i| block } ->  enum
 *     enum.each_with_index(*args)                    ->  an_enumerator
 *
 *  Calls <em>block</em> with two arguments, the item and its index,
 *  for each item in <i>enum</i>.  Given arguments are passed through
 *  to #each().
 *
 *  If no block is given, an enumerator is returned instead.
 *
 *     hash = Hash.new
 *     %w(cat dog wombat).each_with_index { |item, index|
 *       hash[item] = index
 *     }
 *     hash   #=> {"cat"=>0, "dog"=>1, "wombat"=>2}
 *
 */

static VALUE
enum_each_with_index(int argc, VALUE *argv, VALUE obj)
{
    NODE *memo;

    RETURN_ENUMERATOR(obj, argc, argv);

    memo = NEW_MEMO(0, 0, 0);
    rb_block_call(obj, id_each, argc, argv, each_with_index_i, (VALUE)memo);
    return obj;
}


/*
 *  call-seq:
 *     enum.reverse_each(*args) { |item| block } ->  enum
 *     enum.reverse_each(*args)                  ->  an_enumerator
 *
 *  Builds a temporary array and traverses that array in reverse order.
 *
 *  If no block is given, an enumerator is returned instead.
 *
 *      (1..3).reverse_each { |v| p v }
 *
 *    produces:
 *
 *      3
 *      2
 *      1
 */

static VALUE
enum_reverse_each(int argc, VALUE *argv, VALUE obj)
{
    VALUE ary;
    long i;

    RETURN_ENUMERATOR(obj, argc, argv);

    ary = enum_to_a(argc, argv, obj);

    for (i = RARRAY_LEN(ary); --i >= 0; ) {
	rb_yield(RARRAY_PTR(ary)[i]);
    }

    return obj;
}


static VALUE
each_val_i(VALUE i, VALUE p, int argc, VALUE *argv)
{
    ENUM_WANT_SVALUE();
    rb_yield(i);
    return Qnil;
}

/*
 *  call-seq:
 *     enum.each_entry { |obj| block }  -> enum
 *     enum.each_entry                  -> an_enumerator
 *
 *  Calls <i>block</i> once for each element in +self+, passing that
 *  element as a parameter, converting multiple values from yield to an
 *  array.
 *
 *  If no block is given, an enumerator is returned instead.
 *
 *     class Foo
 *       include Enumerable
 *       def each
 *         yield 1
 *         yield 1, 2
 *         yield
 *       end
 *     end
 *     Foo.new.each_entry{ |o| p o }
 *
 *  produces:
 *
 *     1
 *     [1, 2]
 *     nil
 *
 */

static VALUE
enum_each_entry(int argc, VALUE *argv, VALUE obj)
{
    RETURN_ENUMERATOR(obj, argc, argv);
    rb_block_call(obj, id_each, argc, argv, each_val_i, 0);
    return obj;
}

static VALUE
each_slice_i(VALUE i, VALUE m, int argc, VALUE *argv)
{
    NODE *memo = RNODE(m);
    VALUE ary = memo->u1.value;
    VALUE v = Qnil;
    long size = memo->u3.cnt;
    ENUM_WANT_SVALUE();

    rb_ary_push(ary, i);

    if (RARRAY_LEN(ary) == size) {
	v = rb_yield(ary);
	memo->u1.value = rb_ary_new2(size);
    }

    return v;
}

/*
 *  call-seq:
 *    enum.each_slice(n) { ... }  ->  nil
 *    enum.each_slice(n)          ->  an_enumerator
 *
 *  Iterates the given block for each slice of <n> elements.  If no
 *  block is given, returns an enumerator.
 *
 *      (1..10).each_slice(3) { |a| p a }
 *      # outputs below
 *      [1, 2, 3]
 *      [4, 5, 6]
 *      [7, 8, 9]
 *      [10]
 *
 */
static VALUE
enum_each_slice(VALUE obj, VALUE n)
{
    long size = NUM2LONG(n);
    VALUE ary;
    NODE *memo;

    if (size <= 0) rb_raise(rb_eArgError, "invalid slice size");
    RETURN_ENUMERATOR(obj, 1, &n);
    ary = rb_ary_new2(size);
    memo = NEW_MEMO(ary, 0, size);
    rb_block_call(obj, id_each, 0, 0, each_slice_i, (VALUE)memo);
    ary = memo->u1.value;
    if (RARRAY_LEN(ary) > 0) rb_yield(ary);

    return Qnil;
}

static VALUE
each_cons_i(VALUE i, VALUE args, int argc, VALUE *argv)
{
    NODE *memo = RNODE(args);
    VALUE ary = memo->u1.value;
    VALUE v = Qnil;
    long size = memo->u3.cnt;
    ENUM_WANT_SVALUE();

    if (RARRAY_LEN(ary) == size) {
	rb_ary_shift(ary);
    }
    rb_ary_push(ary, i);
    if (RARRAY_LEN(ary) == size) {
	v = rb_yield(rb_ary_dup(ary));
    }
    return v;
}

/*
 *  call-seq:
 *    enum.each_cons(n) { ... } ->  nil
 *    enum.each_cons(n)         ->  an_enumerator
 *
 *  Iterates the given block for each array of consecutive <n>
 *  elements.  If no block is given, returns an enumerator.
 *
 *  e.g.:
 *      (1..10).each_cons(3) { |a| p a }
 *      # outputs below
 *      [1, 2, 3]
 *      [2, 3, 4]
 *      [3, 4, 5]
 *      [4, 5, 6]
 *      [5, 6, 7]
 *      [6, 7, 8]
 *      [7, 8, 9]
 *      [8, 9, 10]
 *
 */
static VALUE
enum_each_cons(VALUE obj, VALUE n)
{
    long size = NUM2LONG(n);
    NODE *memo;

    if (size <= 0) rb_raise(rb_eArgError, "invalid size");
    RETURN_ENUMERATOR(obj, 1, &n);
    memo = NEW_MEMO(rb_ary_new2(size), 0, size);
    rb_block_call(obj, id_each, 0, 0, each_cons_i, (VALUE)memo);

    return Qnil;
}

static VALUE
each_with_object_i(VALUE i, VALUE memo, int argc, VALUE *argv)
{
    ENUM_WANT_SVALUE();
    return rb_yield_values(2, i, memo);
}

/*
 *  call-seq:
 *    enum.each_with_object(obj) { |(*args), memo_obj| ... }  ->  obj
 *    enum.each_with_object(obj)                              ->  an_enumerator
 *
 *  Iterates the given block for each element with an arbitrary
 *  object given, and returns the initially given object.
 *
 *  If no block is given, returns an enumerator.
 *
 *      evens = (1..10).each_with_object([]) { |i, a| a << i*2 }
 *      #=> [2, 4, 6, 8, 10, 12, 14, 16, 18, 20]
 *
 */
static VALUE
enum_each_with_object(VALUE obj, VALUE memo)
{
    RETURN_ENUMERATOR(obj, 1, &memo);

    rb_block_call(obj, id_each, 0, 0, each_with_object_i, memo);

    return memo;
}

static VALUE
zip_ary(VALUE val, NODE *memo, int argc, VALUE *argv)
{
    volatile VALUE result = memo->u1.value;
    volatile VALUE args = memo->u2.value;
    long n = memo->u3.cnt++;
    volatile VALUE tmp;
    int i;

    tmp = rb_ary_new2(RARRAY_LEN(args) + 1);
    rb_ary_store(tmp, 0, rb_enum_values_pack(argc, argv));
    for (i=0; i<RARRAY_LEN(args); i++) {
	VALUE e = RARRAY_PTR(args)[i];

	if (RARRAY_LEN(e) <= n) {
	    rb_ary_push(tmp, Qnil);
	}
	else {
	    rb_ary_push(tmp, RARRAY_PTR(e)[n]);
	}
    }
    if (NIL_P(result)) {
	rb_yield(tmp);
    }
    else {
	rb_ary_push(result, tmp);
    }
    return Qnil;
}

static VALUE
call_next(VALUE *v)
{
    return v[0] = rb_funcall(v[1], id_next, 0, 0);
}

static VALUE
call_stop(VALUE *v)
{
    return v[0] = Qundef;
}

static VALUE
zip_i(VALUE val, NODE *memo, int argc, VALUE *argv)
{
    volatile VALUE result = memo->u1.value;
    volatile VALUE args = memo->u2.value;
    volatile VALUE tmp;
    int i;

    tmp = rb_ary_new2(RARRAY_LEN(args) + 1);
    rb_ary_store(tmp, 0, rb_enum_values_pack(argc, argv));
    for (i=0; i<RARRAY_LEN(args); i++) {
	if (NIL_P(RARRAY_PTR(args)[i])) {
	    rb_ary_push(tmp, Qnil);
	}
	else {
	    VALUE v[2];

	    v[1] = RARRAY_PTR(args)[i];
	    rb_rescue2(call_next, (VALUE)v, call_stop, (VALUE)v, rb_eStopIteration, (VALUE)0);
	    if (v[0] == Qundef) {
		RARRAY_PTR(args)[i] = Qnil;
		v[0] = Qnil;
	    }
	    rb_ary_push(tmp, v[0]);
	}
    }
    if (NIL_P(result)) {
	rb_yield(tmp);
    }
    else {
	rb_ary_push(result, tmp);
    }
    return Qnil;
}

/*
 *  call-seq:
 *     enum.zip(arg, ...)                  -> an_array_of_array
 *     enum.zip(arg, ...) { |arr| block }  -> nil
 *
 *  Takes one element from <i>enum</i> and merges corresponding
 *  elements from each <i>args</i>.  This generates a sequence of
 *  <em>n</em>-element arrays, where <em>n</em> is one more than the
 *  count of arguments.  The length of the resulting sequence will be
 *  <code>enum#size</code>.  If the size of any argument is less than
 *  <code>enum#size</code>, <code>nil</code> values are supplied. If
 *  a block is given, it is invoked for each output array, otherwise
 *  an array of arrays is returned.
 *
 *     a = [ 4, 5, 6 ]
 *     b = [ 7, 8, 9 ]
 *
 *     [1, 2, 3].zip(a, b)      #=> [[1, 4, 7], [2, 5, 8], [3, 6, 9]]
 *     [1, 2].zip(a, b)         #=> [[1, 4, 7], [2, 5, 8]]
 *     a.zip([1, 2], [8])       #=> [[4, 1, 8], [5, 2, nil], [6, nil, nil]]
 *
 */

static VALUE
enum_zip(int argc, VALUE *argv, VALUE obj)
{
    int i;
    ID conv;
    NODE *memo;
    VALUE result = Qnil;
    VALUE args = rb_ary_new4(argc, argv);
    int allary = TRUE;

    argv = RARRAY_PTR(args);
    for (i=0; i<argc; i++) {
	VALUE ary = rb_check_array_type(argv[i]);
	if (NIL_P(ary)) {
	    allary = FALSE;
	    break;
	}
	argv[i] = ary;
    }
    if (!allary) {
	CONST_ID(conv, "to_enum");
	for (i=0; i<argc; i++) {
	    argv[i] = rb_funcall(argv[i], conv, 1, ID2SYM(id_each));
	}
    }
    if (!rb_block_given_p()) {
	result = rb_ary_new();
    }
    /* use NODE_DOT2 as memo(v, v, -) */
    memo = rb_node_newnode(NODE_DOT2, result, args, 0);
    rb_block_call(obj, id_each, 0, 0, allary ? zip_ary : zip_i, (VALUE)memo);

    return result;
}

static VALUE
take_i(VALUE i, VALUE args, int argc, VALUE *argv)
{
    NODE *memo = RNODE(args);
    rb_ary_push(memo->u1.value, rb_enum_values_pack(argc, argv));
    if (--memo->u3.cnt == 0) rb_iter_break();
    return Qnil;
}

/*
 *  call-seq:
 *     enum.take(n)               -> array
 *
 *  Returns first n elements from <i>enum</i>.
 *
 *     a = [1, 2, 3, 4, 5, 0]
 *     a.take(3)             #=> [1, 2, 3]
 *
 */

static VALUE
enum_take(VALUE obj, VALUE n)
{
    NODE *memo;
    VALUE result;
    long len = NUM2LONG(n);

    if (len < 0) {
	rb_raise(rb_eArgError, "attempt to take negative size");
    }

    if (len == 0) return rb_ary_new2(0);
    result = rb_ary_new2(len);
    memo = NEW_MEMO(result, 0, len);
    rb_block_call(obj, id_each, 0, 0, take_i, (VALUE)memo);
    return result;
}


static VALUE
take_while_i(VALUE i, VALUE ary, int argc, VALUE *argv)
{
    if (!RTEST(enum_yield(argc, argv))) rb_iter_break();
    rb_ary_push(ary, rb_enum_values_pack(argc, argv));
    return Qnil;
}

/*
 *  call-seq:
 *     enum.take_while { |arr| block } -> array
 *     enum.take_while                 -> an_enumerator
 *
 *  Passes elements to the block until the block returns +nil+ or +false+,
 *  then stops iterating and returns an array of all prior elements.
 *
 *  If no block is given, an enumerator is returned instead.
 *
 *     a = [1, 2, 3, 4, 5, 0]
 *     a.take_while { |i| i < 3 }   #=> [1, 2]
 *
 */

static VALUE
enum_take_while(VALUE obj)
{
    VALUE ary;

    RETURN_ENUMERATOR(obj, 0, 0);
    ary = rb_ary_new();
    rb_block_call(obj, id_each, 0, 0, take_while_i, ary);
    return ary;
}

static VALUE
drop_i(VALUE i, VALUE args, int argc, VALUE *argv)
{
    NODE *memo = RNODE(args);
    if (memo->u3.cnt == 0) {
	rb_ary_push(memo->u1.value, rb_enum_values_pack(argc, argv));
    }
    else {
	memo->u3.cnt--;
    }
    return Qnil;
}

/*
 *  call-seq:
 *     enum.drop(n)               -> array
 *
 *  Drops first n elements from <i>enum</i>, and returns rest elements
 *  in an array.
 *
 *     a = [1, 2, 3, 4, 5, 0]
 *     a.drop(3)             #=> [4, 5, 0]
 *
 */

static VALUE
enum_drop(VALUE obj, VALUE n)
{
    VALUE result;
    NODE *memo;
    long len = NUM2LONG(n);

    if (len < 0) {
	rb_raise(rb_eArgError, "attempt to drop negative size");
    }

    result = rb_ary_new();
    memo = NEW_MEMO(result, 0, len);
    rb_block_call(obj, id_each, 0, 0, drop_i, (VALUE)memo);
    return result;
}


static VALUE
drop_while_i(VALUE i, VALUE args, int argc, VALUE *argv)
{
    NODE *memo = RNODE(args);
    ENUM_WANT_SVALUE();

    if (!memo->u3.state && !RTEST(rb_yield(i))) {
	memo->u3.state = TRUE;
    }
    if (memo->u3.state) {
	rb_ary_push(memo->u1.value, i);
    }
    return Qnil;
}

/*
 *  call-seq:
 *     enum.drop_while { |arr| block }  -> array
 *     enum.drop_while                  -> an_enumerator
 *
 *  Drops elements up to, but not including, the first element for
 *  which the block returns +nil+ or +false+ and returns an array
 *  containing the remaining elements.
 *
 *  If no block is given, an enumerator is returned instead.
 *
 *     a = [1, 2, 3, 4, 5, 0]
 *     a.drop_while { |i| i < 3 }   #=> [3, 4, 5, 0]
 *
 */

static VALUE
enum_drop_while(VALUE obj)
{
    VALUE result;
    NODE *memo;

    RETURN_ENUMERATOR(obj, 0, 0);
    result = rb_ary_new();
    memo = NEW_MEMO(result, 0, FALSE);
    rb_block_call(obj, id_each, 0, 0, drop_while_i, (VALUE)memo);
    return result;
}

static VALUE
cycle_i(VALUE i, VALUE ary, int argc, VALUE *argv)
{
    ENUM_WANT_SVALUE();

    rb_ary_push(ary, i);
    rb_yield(i);
    return Qnil;
}

/*
 *  call-seq:
 *     enum.cycle(n=nil) { |obj| block }  ->  nil
 *     enum.cycle(n=nil)                  ->  an_enumerator
 *
 *  Calls <i>block</i> for each element of <i>enum</i> repeatedly _n_
 *  times or forever if none or +nil+ is given.  If a non-positive
 *  number is given or the collection is empty, does nothing.  Returns
 *  +nil+ if the loop has finished without getting interrupted.
 *
 *  Enumerable#cycle saves elements in an internal array so changes
 *  to <i>enum</i> after the first pass have no effect.
 *
 *  If no block is given, an enumerator is returned instead.
 *
 *     a = ["a", "b", "c"]
 *     a.cycle { |x| puts x }  # print, a, b, c, a, b, c,.. forever.
 *     a.cycle(2) { |x| puts x }  # print, a, b, c, a, b, c.
 *
 */

static VALUE
enum_cycle(int argc, VALUE *argv, VALUE obj)
{
    VALUE ary;
    VALUE nv = Qnil;
    long n, i, len;

    rb_scan_args(argc, argv, "01", &nv);

    RETURN_ENUMERATOR(obj, argc, argv);
    if (NIL_P(nv)) {
        n = -1;
    }
    else {
        n = NUM2LONG(nv);
        if (n <= 0) return Qnil;
    }
    ary = rb_ary_new();
    RBASIC(ary)->klass = 0;
    rb_block_call(obj, id_each, 0, 0, cycle_i, ary);
    len = RARRAY_LEN(ary);
    if (len == 0) return Qnil;
    while (n < 0 || 0 < --n) {
        for (i=0; i<len; i++) {
            rb_yield(RARRAY_PTR(ary)[i]);
        }
    }
    return Qnil;
}

struct chunk_arg {
    VALUE categorize;
    VALUE state;
    VALUE prev_value;
    VALUE prev_elts;
    VALUE yielder;
};

static VALUE
chunk_ii(VALUE i, VALUE _argp, int argc, VALUE *argv)
{
    struct chunk_arg *argp = MEMO_FOR(struct chunk_arg, _argp);
    VALUE v;
    VALUE alone = ID2SYM(rb_intern("_alone"));
    VALUE separator = ID2SYM(rb_intern("_separator"));

    ENUM_WANT_SVALUE();

    if (NIL_P(argp->state))
        v = rb_funcall(argp->categorize, rb_intern("call"), 1, i);
    else
        v = rb_funcall(argp->categorize, rb_intern("call"), 2, i, argp->state);

    if (v == alone) {
        if (!NIL_P(argp->prev_value)) {
            rb_funcall(argp->yielder, id_lshift, 1, rb_assoc_new(argp->prev_value, argp->prev_elts));
            argp->prev_value = argp->prev_elts = Qnil;
        }
        rb_funcall(argp->yielder, id_lshift, 1, rb_assoc_new(v, rb_ary_new3(1, i)));
    }
    else if (NIL_P(v) || v == separator) {
        if (!NIL_P(argp->prev_value)) {
            rb_funcall(argp->yielder, id_lshift, 1, rb_assoc_new(argp->prev_value, argp->prev_elts));
            argp->prev_value = argp->prev_elts = Qnil;
        }
    }
    else if (SYMBOL_P(v) && rb_id2name(SYM2ID(v))[0] == '_') {
	rb_raise(rb_eRuntimeError, "symbol begins with an underscore is reserved");
    }
    else {
        if (NIL_P(argp->prev_value)) {
            argp->prev_value = v;
            argp->prev_elts = rb_ary_new3(1, i);
        }
        else {
            if (rb_equal(argp->prev_value, v)) {
                rb_ary_push(argp->prev_elts, i);
            }
            else {
                rb_funcall(argp->yielder, id_lshift, 1, rb_assoc_new(argp->prev_value, argp->prev_elts));
                argp->prev_value = v;
                argp->prev_elts = rb_ary_new3(1, i);
            }
        }
    }
    return Qnil;
}

static VALUE
chunk_i(VALUE yielder, VALUE enumerator, int argc, VALUE *argv)
{
    VALUE enumerable;
    VALUE arg;
    struct chunk_arg *memo = NEW_MEMO_FOR(struct chunk_arg, arg);

    enumerable = rb_ivar_get(enumerator, rb_intern("chunk_enumerable"));
    memo->categorize = rb_ivar_get(enumerator, rb_intern("chunk_categorize"));
    memo->state = rb_ivar_get(enumerator, rb_intern("chunk_initial_state"));
    memo->prev_value = Qnil;
    memo->prev_elts = Qnil;
    memo->yielder = yielder;

    if (!NIL_P(memo->state))
	memo->state = rb_obj_dup(memo->state);

    rb_block_call(enumerable, id_each, 0, 0, chunk_ii, arg);
    memo = MEMO_FOR(struct chunk_arg, arg);
    if (!NIL_P(memo->prev_elts))
	rb_funcall(memo->yielder, id_lshift, 1, rb_assoc_new(memo->prev_value, memo->prev_elts));
    return Qnil;
}

/*
 *  call-seq:
 *     enum.chunk { |elt| ... }                       -> an_enumerator
 *     enum.chunk(initial_state) { |elt, state| ... } -> an_enumerator
 *
 *  Creates an enumerator for each chunked elements.
 *  The consecutive elements which have same block value are chunked.
 *
 *  The result enumerator yields the block value and an array of chunked elements.
 *  So "each" method can be called as follows.
 *
 *    enum.chunk { |elt| key }.each { |key, ary| ... }
 *    enum.chunk(initial_state) { |elt, state| key }.each { |key, ary| ... }
 *
 *  For example, consecutive even numbers and odd numbers can be
 *  splitted as follows.
 *
 *    [3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5].chunk { |n|
 *      n.even?
 *    }.each { |even, ary|
 *      p [even, ary]
 *    }
 *    #=> [false, [3, 1]]
 *    #   [true, [4]]
 *    #   [false, [1, 5, 9]]
 *    #   [true, [2, 6]]
 *    #   [false, [5, 3, 5]]
 *
 *  This method is especially useful for sorted series of elements.
 *  The following example counts words for each initial letter.
 *
 *    open("/usr/share/dict/words", "r:iso-8859-1") { |f|
 *      f.chunk { |line| line.ord }.each { |ch, lines| p [ch.chr, lines.length] }
 *    }
 *    #=> ["\n", 1]
 *    #   ["A", 1327]
 *    #   ["B", 1372]
 *    #   ["C", 1507]
 *    #   ["D", 791]
 *    #   ...
 *
 *  The following key values has special meaning:
 *  - nil and :_separator specifies that the elements are dropped.
 *  - :_alone specifies that the element should be chunked as a singleton.
 *  Other symbols which begins an underscore are reserved.
 *
 *  nil and :_separator can be used to ignore some elements.
 *  For example, the sequence of hyphens in svn log can be eliminated as follows.
 *
 *    sep = "-"*72 + "\n"
 *    IO.popen("svn log README") { |f|
 *      f.chunk { |line|
 *        line != sep || nil
 *      }.each { |_, lines|
 *        pp lines
 *      }
 *    }
 *    #=> ["r20018 | knu | 2008-10-29 13:20:42 +0900 (Wed, 29 Oct 2008) | 2 lines\n",
 *    #    "\n",
 *    #    "* README, README.ja: Update the portability section.\n",
 *    #    "\n"]
 *    #   ["r16725 | knu | 2008-05-31 23:34:23 +0900 (Sat, 31 May 2008) | 2 lines\n",
 *    #    "\n",
 *    #    "* README, README.ja: Add a note about default C flags.\n",
 *    #    "\n"]
 *    #   ...
 *
 *  paragraphs separated by empty lines can be parsed as follows.
 *
 *    File.foreach("README").chunk { |line|
 *      /\A\s*\z/ !~ line || nil
 *    }.each { |_, lines|
 *      pp lines
 *    }
 *
 *  :_alone can be used to pass through bunch of elements.
 *  For example, sort consecutive lines formed as Foo#bar and
 *  pass other lines, chunk can be used as follows.
 *
 *    pat = /\A[A-Z][A-Za-z0-9_]+\#/
 *    open(filename) { |f|
 *      f.chunk { |line| pat =~ line ? $& : :_alone }.each { |key, lines|
 *        if key != :_alone
 *          print lines.sort.join('')
 *        else
 *          print lines.join('')
 *        end
 *      }
 *    }
 *
 *  If the block needs to maintain state over multiple elements,
 *  _initial_state_ argument can be used.
 *  If non-nil value is given,
 *  it is duplicated for each "each" method invocation of the enumerator.
 *  The duplicated object is passed to 2nd argument of the block for "chunk" method.
 *
 */
static VALUE
enum_chunk(int argc, VALUE *argv, VALUE enumerable)
{
    VALUE initial_state;
    VALUE enumerator;

    if(!rb_block_given_p())
	rb_raise(rb_eArgError, "no block given");
    rb_scan_args(argc, argv, "01", &initial_state);

    enumerator = rb_obj_alloc(rb_cEnumerator);
    rb_ivar_set(enumerator, rb_intern("chunk_enumerable"), enumerable);
    rb_ivar_set(enumerator, rb_intern("chunk_categorize"), rb_block_proc());
    rb_ivar_set(enumerator, rb_intern("chunk_initial_state"), initial_state);
    rb_block_call(enumerator, rb_intern("initialize"), 0, 0, chunk_i, enumerator);
    return enumerator;
}


struct slicebefore_arg {
    VALUE sep_pred;
    VALUE sep_pat;
    VALUE state;
    VALUE prev_elts;
    VALUE yielder;
};

static VALUE
slicebefore_ii(VALUE i, VALUE _argp, int argc, VALUE *argv)
{
    struct slicebefore_arg *argp = MEMO_FOR(struct slicebefore_arg, _argp);
    VALUE header_p;

    ENUM_WANT_SVALUE();

    if (!NIL_P(argp->sep_pat))
        header_p = rb_funcall(argp->sep_pat, id_eqq, 1, i);
    else if (NIL_P(argp->state))
        header_p = rb_funcall(argp->sep_pred, rb_intern("call"), 1, i);
    else
        header_p = rb_funcall(argp->sep_pred, rb_intern("call"), 2, i, argp->state);
    if (RTEST(header_p)) {
        if (!NIL_P(argp->prev_elts))
            rb_funcall(argp->yielder, id_lshift, 1, argp->prev_elts);
        argp->prev_elts = rb_ary_new3(1, i);
    }
    else {
        if (NIL_P(argp->prev_elts))
            argp->prev_elts = rb_ary_new3(1, i);
        else
            rb_ary_push(argp->prev_elts, i);
    }

    return Qnil;
}

static VALUE
slicebefore_i(VALUE yielder, VALUE enumerator, int argc, VALUE *argv)
{
    VALUE enumerable;
    VALUE arg;
    struct slicebefore_arg *memo = NEW_MEMO_FOR(struct slicebefore_arg, arg);

    enumerable = rb_ivar_get(enumerator, rb_intern("slicebefore_enumerable"));
    memo->sep_pred = rb_attr_get(enumerator, rb_intern("slicebefore_sep_pred"));
    memo->sep_pat = NIL_P(memo->sep_pred) ? rb_ivar_get(enumerator, rb_intern("slicebefore_sep_pat")) : Qnil;
    memo->state = rb_attr_get(enumerator, rb_intern("slicebefore_initial_state"));
    memo->prev_elts = Qnil;
    memo->yielder = yielder;

    if (!NIL_P(memo->state))
        memo->state = rb_obj_dup(memo->state);

    rb_block_call(enumerable, id_each, 0, 0, slicebefore_ii, arg);
    memo = MEMO_FOR(struct slicebefore_arg, arg);
    if (!NIL_P(memo->prev_elts))
        rb_funcall(memo->yielder, id_lshift, 1, memo->prev_elts);
    return Qnil;
}

/*
 *  call-seq:
 *     enum.slice_before(pattern)                             -> an_enumerator
 *     enum.slice_before { |elt| bool }                       -> an_enumerator
 *     enum.slice_before(initial_state) { |elt, state| bool } -> an_enumerator
 *
 *  Creates an enumerator for each chunked elements.
 *  The beginnings of chunks are defined by _pattern_ and the block.
 *  If _pattern_ === _elt_ returns true or
 *  the block returns true for the element,
 *  the element is beginning of a chunk.
 *
 *  The === and block is called from the first element to the last element
 *  of _enum_.
 *  The result for the first element is ignored.
 *
 *  The result enumerator yields the chunked elements as an array for +each+
 *  method.
 *  +each+ method can be called as follows.
 *
 *    enum.slice_before(pattern).each { |ary| ... }
 *    enum.slice_before { |elt| bool }.each { |ary| ... }
 *    enum.slice_before(initial_state) { |elt, state| bool }.each { |ary| ... }
 *
 *  Other methods of Enumerator class and Enumerable module,
 *  such as map, etc., are also usable.
 *
 *  For example, iteration over ChangeLog entries can be implemented as
 *  follows.
 *
 *    # iterate over ChangeLog entries.
 *    open("ChangeLog") { |f|
 *      f.slice_before(/\A\S/).each { |e| pp e }
 *    }
 *
 *    # same as above.  block is used instead of pattern argument.
 *    open("ChangeLog") { |f|
 *      f.slice_before { |line| /\A\S/ === line }.each { |e| pp e }
 *    }
 *
 * "svn proplist -R" produces multiline output for each file.
 * They can be chunked as follows:
 *
 *    IO.popen([{"LC_ALL"=>"C"}, "svn", "proplist", "-R"]) { |f|
 *      f.lines.slice_before(/\AProp/).each { |lines| p lines }
 *    }
 *    #=> ["Properties on '.':\n", "  svn:ignore\n", "  svk:merge\n"]
 *    #   ["Properties on 'goruby.c':\n", "  svn:eol-style\n"]
 *    #   ["Properties on 'complex.c':\n", "  svn:mime-type\n", "  svn:eol-style\n"]
 *    #   ["Properties on 'regparse.c':\n", "  svn:eol-style\n"]
 *    #   ...
 *
 *  If the block needs to maintain state over multiple elements,
 *  local variables can be used.
 *  For example, three or more consecutive increasing numbers can be squashed
 *  as follows:
 *
 *    a = [0, 2, 3, 4, 6, 7, 9]
 *    prev = a[0]
 *    p a.slice_before { |e|
 *      prev, prev2 = e, prev
 *      prev2 + 1 != e
 *    }.map { |es|
 *      es.length <= 2 ? es.join(",") : "#{es.first}-#{es.last}"
 *    }.join(",")
 *    #=> "0,2-4,6,7,9"
 *
 *  However local variables are not appropriate to maintain state
 *  if the result enumerator is used twice or more.
 *  In such case, the last state of the 1st +each+ is used in 2nd +each+.
 *  _initial_state_ argument can be used to avoid this problem.
 *  If non-nil value is given as _initial_state_,
 *  it is duplicated for each "each" method invocation of the enumerator.
 *  The duplicated object is passed to 2nd argument of the block for
 *  +slice_before+ method.
 *
 *    # word wrapping.
 *    # this assumes all characters have same width.
 *    def wordwrap(words, maxwidth)
 *      # if cols is a local variable, 2nd "each" may start with non-zero cols.
 *      words.slice_before(cols: 0) { |w, h|
 *        h[:cols] += 1 if h[:cols] != 0
 *        h[:cols] += w.length
 *        if maxwidth < h[:cols]
 *          h[:cols] = w.length
 *          true
 *        else
 *          false
 *        end
 *      }
 *    end
 *    text = (1..20).to_a.join(" ")
 *    enum = wordwrap(text.split(/\s+/), 10)
 *    puts "-"*10
 *    enum.each { |ws| puts ws.join(" ") }
 *    puts "-"*10
 *    #=> ----------
 *    #   1 2 3 4 5
 *    #   6 7 8 9 10
 *    #   11 12 13
 *    #   14 15 16
 *    #   17 18 19
 *    #   20
 *    #   ----------
 *
 * mbox contains series of mails which start with Unix From line.
 * So each mail can be extracted by slice before Unix From line.
 *
 *    # parse mbox
 *    open("mbox") { |f|
 *      f.slice_before { |line|
 *        line.start_with? "From "
 *      }.each { |mail|
 *        unix_from = mail.shift
 *        i = mail.index("\n")
 *        header = mail[0...i]
 *        body = mail[(i+1)..-1]
 *        body.pop if body.last == "\n"
 *        fields = header.slice_before { |line| !" \t".include?(line[0]) }.to_a
 *        p unix_from
 *        pp fields
 *        pp body
 *      }
 *    }
 *
 *    # split mails in mbox (slice before Unix From line after an empty line)
 *    open("mbox") { |f|
 *      f.slice_before(emp: true) { |line, h|
 *        prevemp = h[:emp]
 *        h[:emp] = line == "\n"
 *        prevemp && line.start_with?("From ")
 *      }.each { |mail|
 *        mail.pop if mail.last == "\n"
 *        pp mail
 *      }
 *    }
 *
 */
static VALUE
enum_slice_before(int argc, VALUE *argv, VALUE enumerable)
{
    VALUE enumerator;

    if (rb_block_given_p()) {
        VALUE initial_state;
        rb_scan_args(argc, argv, "01", &initial_state);
        enumerator = rb_obj_alloc(rb_cEnumerator);
        rb_ivar_set(enumerator, rb_intern("slicebefore_sep_pred"), rb_block_proc());
        rb_ivar_set(enumerator, rb_intern("slicebefore_initial_state"), initial_state);
    }
    else {
        VALUE sep_pat;
        rb_scan_args(argc, argv, "1", &sep_pat);
        enumerator = rb_obj_alloc(rb_cEnumerator);
        rb_ivar_set(enumerator, rb_intern("slicebefore_sep_pat"), sep_pat);
    }
    rb_ivar_set(enumerator, rb_intern("slicebefore_enumerable"), enumerable);
    rb_block_call(enumerator, rb_intern("initialize"), 0, 0, slicebefore_i, enumerator);
    return enumerator;
}

/*
 *  The <code>Enumerable</code> mixin provides collection classes with
 *  several traversal and searching methods, and with the ability to
 *  sort. The class must provide a method <code>each</code>, which
 *  yields successive members of the collection. If
 *  <code>Enumerable#max</code>, <code>#min</code>, or
 *  <code>#sort</code> is used, the objects in the collection must also
 *  implement a meaningful <code><=></code> operator, as these methods
 *  rely on an ordering between members of the collection.
 */

void
Init_Enumerable(void)
{
#undef rb_intern
#define rb_intern(str) rb_intern_const(str)

    rb_mEnumerable = rb_define_module("Enumerable");

    rb_define_method(rb_mEnumerable, "to_a", enum_to_a, -1);
    rb_define_method(rb_mEnumerable, "entries", enum_to_a, -1);

    rb_define_method(rb_mEnumerable, "sort", enum_sort, 0);
    rb_define_method(rb_mEnumerable, "sort_by", enum_sort_by, 0);
    rb_define_method(rb_mEnumerable, "grep", enum_grep, 1);
    rb_define_method(rb_mEnumerable, "count", enum_count, -1);
    rb_define_method(rb_mEnumerable, "find", enum_find, -1);
    rb_define_method(rb_mEnumerable, "detect", enum_find, -1);
    rb_define_method(rb_mEnumerable, "find_index", enum_find_index, -1);
    rb_define_method(rb_mEnumerable, "find_all", enum_find_all, 0);
    rb_define_method(rb_mEnumerable, "select", enum_find_all, 0);
    rb_define_method(rb_mEnumerable, "reject", enum_reject, 0);
    rb_define_method(rb_mEnumerable, "collect", enum_collect, 0);
    rb_define_method(rb_mEnumerable, "map", enum_collect, 0);
    rb_define_method(rb_mEnumerable, "flat_map", enum_flat_map, 0);
    rb_define_method(rb_mEnumerable, "collect_concat", enum_flat_map, 0);
    rb_define_method(rb_mEnumerable, "inject", enum_inject, -1);
    rb_define_method(rb_mEnumerable, "reduce", enum_inject, -1);
    rb_define_method(rb_mEnumerable, "partition", enum_partition, 0);
    rb_define_method(rb_mEnumerable, "group_by", enum_group_by, 0);
    rb_define_method(rb_mEnumerable, "first", enum_first, -1);
    rb_define_method(rb_mEnumerable, "all?", enum_all, 0);
    rb_define_method(rb_mEnumerable, "any?", enum_any, 0);
    rb_define_method(rb_mEnumerable, "one?", enum_one, 0);
    rb_define_method(rb_mEnumerable, "none?", enum_none, 0);
    rb_define_method(rb_mEnumerable, "min", enum_min, 0);
    rb_define_method(rb_mEnumerable, "max", enum_max, 0);
    rb_define_method(rb_mEnumerable, "minmax", enum_minmax, 0);
    rb_define_method(rb_mEnumerable, "min_by", enum_min_by, 0);
    rb_define_method(rb_mEnumerable, "max_by", enum_max_by, 0);
    rb_define_method(rb_mEnumerable, "minmax_by", enum_minmax_by, 0);
    rb_define_method(rb_mEnumerable, "member?", enum_member, 1);
    rb_define_method(rb_mEnumerable, "include?", enum_member, 1);
    rb_define_method(rb_mEnumerable, "each_with_index", enum_each_with_index, -1);
    rb_define_method(rb_mEnumerable, "reverse_each", enum_reverse_each, -1);
    rb_define_method(rb_mEnumerable, "each_entry", enum_each_entry, -1);
    rb_define_method(rb_mEnumerable, "each_slice", enum_each_slice, 1);
    rb_define_method(rb_mEnumerable, "each_cons", enum_each_cons, 1);
    rb_define_method(rb_mEnumerable, "each_with_object", enum_each_with_object, 1);
    rb_define_method(rb_mEnumerable, "zip", enum_zip, -1);
    rb_define_method(rb_mEnumerable, "take", enum_take, 1);
    rb_define_method(rb_mEnumerable, "take_while", enum_take_while, 0);
    rb_define_method(rb_mEnumerable, "drop", enum_drop, 1);
    rb_define_method(rb_mEnumerable, "drop_while", enum_drop_while, 0);
    rb_define_method(rb_mEnumerable, "cycle", enum_cycle, -1);
    rb_define_method(rb_mEnumerable, "chunk", enum_chunk, -1);
    rb_define_method(rb_mEnumerable, "slice_before", enum_slice_before, -1);

    id_next = rb_intern("next");
}
