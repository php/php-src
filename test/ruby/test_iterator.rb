require 'test/unit'

class Array
  def iter_test1
    collect{|e| [e, yield(e)]}.sort{|a,b|a[1]<=>b[1]}
  end
  def iter_test2
    ary = collect{|e| [e, yield(e)]}
    ary.sort{|a,b|a[1]<=>b[1]}
  end
end

class TestIterator < Test::Unit::TestCase
  def ttt
    assert(iterator?)
  end

  def test_iterator
    assert(!iterator?)

    ttt{}

    # yield at top level	!! here's not toplevel
    assert(!defined?(yield))
  end

  def test_array
    $x = [1, 2, 3, 4]
    $y = []

    # iterator over array
    for i in $x
      $y.push i
    end
    assert_equal($x, $y)
  end

  def tt
    1.upto(10) {|i|
      yield i
    }
  end

  def tt2(dummy)
    yield 1
  end

  def tt3(&block)
    tt2(raise(ArgumentError,""),&block)
  end

  def test_nested_iterator
    i = 0
    tt{|j| break if j == 5}
    assert_equal(0, i)

    assert_raise(ArgumentError) do
      tt3{}
    end
  end

  def tt4 &block
    tt2(raise(ArgumentError,""),&block)
  end

  def test_block_argument_without_paren
    assert_raise(ArgumentError) do
      tt4{}
    end
  end

  # iterator break/redo/next
  def test_break
    done = true
    loop{
      break
      done = false			# should not reach here
    }
    assert(done)

    done = false
    $bad = false
    loop {
      break if done
      done = true
      next
      $bad = true			# should not reach here
    }
    assert(!$bad)

    done = false
    $bad = false
    loop {
      break if done
      done = true
      redo
      $bad = true			# should not reach here
    }
    assert(!$bad)

    $x = []
    for i in 1 .. 7
      $x.push i
    end
    assert_equal(7, $x.size)
    assert_equal([1, 2, 3, 4, 5, 6, 7], $x)
  end

  def test_append_method_to_built_in_class
    $x = [[1,2],[3,4],[5,6]]
    assert_equal($x.iter_test1{|x|x}, $x.iter_test2{|x|x})
  end

  class IterTest
    def initialize(e); @body = e; end

    def each0(&block); @body.each(&block); end
    def each1(&block); @body.each {|*x| block.call(*x) } end
    def each2(&block); @body.each {|*x| block.call(x) } end
    def each3(&block); @body.each {|x| block.call(*x) } end
    def each4(&block); @body.each {|x| block.call(x) } end
    def each5; @body.each {|*x| yield(*x) } end
    def each6; @body.each {|*x| yield(x) } end
    def each7; @body.each {|x| yield(*x) } end
    def each8; @body.each {|x| yield(x) } end

    def f(a)
      a
    end
  end

  def test_itertest
    assert_equal([1], IterTest.new(nil).method(:f).to_proc.call([1]))
    m = /\w+/.match("abc")
    assert_equal([m], IterTest.new(nil).method(:f).to_proc.call([m]))

    IterTest.new([0]).each0 {|x| assert_equal(0, x)}
    IterTest.new([1]).each1 {|x| assert_equal(1, x)}
    IterTest.new([2]).each2 {|x| assert_equal([2], x)}
    IterTest.new([4]).each4 {|x| assert_equal(4, x)}
    IterTest.new([5]).each5 {|x| assert_equal(5, x)}
    IterTest.new([6]).each6 {|x| assert_equal([6], x)}
    IterTest.new([8]).each8 {|x| assert_equal(8, x)}

    IterTest.new([[0]]).each0 {|x| assert_equal([0], x)}
    IterTest.new([[1]]).each1 {|x| assert_equal([1], x)}
    IterTest.new([[2]]).each2 {|x| assert_equal([[2]], x)}
    IterTest.new([[3]]).each3 {|x| assert_equal(3, x)}
    IterTest.new([[4]]).each4 {|x| assert_equal([4], x)}
    IterTest.new([[5]]).each5 {|x| assert_equal([5], x)}
    IterTest.new([[6]]).each6 {|x| assert_equal([[6]], x)}
    IterTest.new([[7]]).each7 {|x| assert_equal(7, x)}
    IterTest.new([[8]]).each8 {|x| assert_equal([8], x)}

    IterTest.new([[0,0]]).each0 {|*x| assert_equal([[0,0]], x)}
    IterTest.new([[8,8]]).each8 {|*x| assert_equal([[8,8]], x)}
  end

  def m(var)
    var
  end

  def m1
    m(block_given?)
  end

  def m2
    m(block_given?,&proc{})
  end

  def test_block_given
    assert(m1{p 'test'})
    assert(m2{p 'test'})
    assert(!m1())
    assert(!m2())
  end

  def m3(var, &block)
    m(yield(var), &block)
  end

  def m4(&block)
    m(m1(), &block)
  end

  def test_block_passing
    assert(!m4())
    assert(!m4 {})
    assert_equal(100, m3(10) {|x|x*x})
  end

  class C
    include Enumerable
    def initialize
      @a = [1,2,3]
    end
    def each(&block)
      @a.each(&block)
    end
  end

  def test_collect
    assert_equal([1,2,3], C.new.collect{|n| n})
  end

  def test_proc
    assert_instance_of(Proc, lambda{})
    assert_instance_of(Proc, Proc.new{})
    lambda{|a|assert_equal(a, 1)}.call(1)
  end

  def test_block
    assert_instance_of(NilClass, get_block)
    assert_instance_of(Proc, get_block{})
  end

  def test_argument
    assert_nothing_raised {lambda{||}.call}
    assert_raise(ArgumentError) {lambda{||}.call(1)}
    assert_nothing_raised {lambda{|a,|}.call(1)}
    assert_raise(ArgumentError) {lambda{|a,|}.call()}
    assert_raise(ArgumentError) {lambda{|a,|}.call(1,2)}
  end

  def get_block(&block)
    block
  end

  def test_get_block
    assert_instance_of(Proc, get_block{})
    assert_nothing_raised {get_block{||}.call()}
    assert_nothing_raised {get_block{||}.call(1)}
    assert_nothing_raised {get_block{|a,|}.call(1)}
    assert_nothing_raised {get_block{|a,|}.call()}
    assert_nothing_raised {get_block{|a,|}.call(1,2)}

    assert_nothing_raised {get_block(&lambda{||}).call()}
    assert_raise(ArgumentError) {get_block(&lambda{||}).call(1)}
    assert_nothing_raised {get_block(&lambda{|a,|}).call(1)}
    assert_raise(ArgumentError) {get_block(&lambda{|a,|}).call(1,2)}

    block = get_block{11}
    assert_instance_of(Proc, block)
    assert_instance_of(Proc, block.to_proc)
    assert_equal(block.clone.call, 11)
    assert_instance_of(Proc, get_block(&block))

    lmd = lambda{44}
    assert_instance_of(Proc, lmd)
    assert_instance_of(Proc, lmd.to_proc)
    assert_equal(lmd.clone.call, 44)
    assert_instance_of(Proc, get_block(&lmd))

    assert_equal(1, Proc.new{|a,| a}.call(1,2,3))
    assert_nothing_raised {Proc.new{|a,|}.call(1,2)}
  end

  def return1_test
    Proc.new {
      return 55
    }.call + 5
  end

  def test_return1
    assert_equal(55, return1_test())
  end

  def return2_test
    lambda {
      return 55
    }.call + 5
  end

  def test_return2
    assert_equal(60, return2_test())
  end

  def proc_call(&b)
    b.call
  end
  def proc_yield()
    yield
  end
  def proc_return1
    proc_call{return 42}+1
  end

  def test_proc_return1
    assert_equal(42, proc_return1())
  end

  def proc_return2
    proc_yield{return 42}+1
  end

  def test_proc_return2
    assert_equal(42, proc_return2())
  end

  def test_ljump
    assert_raise(LocalJumpError) {get_block{break}.call}

    # cannot use assert_nothing_raised due to passing block.
    begin
      val = lambda{break 11}.call
    rescue LocalJumpError
      assert(false, "LocalJumpError occurred from break in lambda")
    else
      assert_equal(11, val)
    end

    block = get_block{11}
    lmd = lambda{44}
    assert_equal(0, block.arity)
    assert_equal(0, lmd.arity)
    assert_equal(0, lambda{||}.arity)
    assert_equal(1, lambda{|a|}.arity)
    assert_equal(1, lambda{|a,|}.arity)
    assert_equal(2, lambda{|a,b|}.arity)
  end

  def marity_test(m)
    mobj = method(m)
    assert_equal(mobj.arity, mobj.to_proc.arity)
  end

  def test_marity
    marity_test(:assert)
    marity_test(:marity_test)
    marity_test(:p)

    lambda(&method(:assert)).call(true)
    lambda(&get_block{|a,n| assert(a,n)}).call(true, "marity")
  end

  def foo
    yield(:key, :value)
  end
  def bar(&blk)
    blk.call(:key, :value)
  end

  def test_yield_vs_call
    foo{|k,v| assert_equal([:key, :value], [k,v])}
    bar{|k,v| assert_equal([:key, :value], [k,v])}
  end

  class H
    def each
      yield [:key, :value]
    end
    alias each_pair each
  end

  def test_assoc_yield
     [{:key=>:value}, H.new].each {|h|
       h.each{|a| assert_equal([:key, :value], a)}
       h.each{|a,| assert_equal(:key, a)}
       h.each{|*a| assert_equal([[:key, :value]], a)}
       h.each{|k,v| assert_equal([:key, :value], [k,v])}
       h.each_pair{|a| assert_equal([:key, :value], a)}
       h.each_pair{|a,| assert_equal(:key, a)}
       h.each_pair{|*a| assert_equal([[:key, :value]], a)}
       h.each_pair{|k,v| assert_equal([:key, :value], [k,v])}
    }
  end

  class ITER_TEST1
    def a
      block_given?
    end
  end

  class ITER_TEST2 < ITER_TEST1
    include Test::Unit::Assertions
    def a
      assert(super)
      super
    end
  end

  def test_iter_test2
    assert(ITER_TEST2.new.a {})
  end

  class ITER_TEST3
    def foo x
      return yield if block_given?
      x
    end
  end

  class ITER_TEST4 < ITER_TEST3
    include Test::Unit::Assertions
    def foo x
      assert_equal(super, yield)
      assert_equal(x, super(x, &nil))
    end
  end

  def test_iter4
    ITER_TEST4.new.foo(44){55}
  end

  def test_break__nested_loop1
    _test_break__nested_loop1 do
      break
    end
  end

  def _test_break__nested_loop1
    while true
      yield
    end
    assert(false, "must not reach here")
  end

  def test_break__nested_loop2
    _test_break__nested_loop2 do
      break
    end
  end

  def _test_break__nested_loop2
    until false
      yield
    end
    assert(false, "must not reach here")
  end

  def test_break__nested_loop3
    _test_break__nested_loop3 do
      break
    end
  end

  def _test_break__nested_loop3
    loop do
      yield
    end
    assert(false, "must not reach here")
  end

  def test_break_from_enum
    result = ["a"].inject("ng") {|x,y| break "ok"}
    assert_equal("ok", result)
  end

  def _test_return_trace_func(x)
    set_trace_func(proc {})
    [].fetch(2) {return x}
  ensure
    set_trace_func(nil)
  end

  def test_return_trace_func
    ok = "returned gracefully"
    result = "skipped"
    result = _test_return_trace_func(ok)
  ensure
    assert_equal(ok, result)
    return
  end

  class IterString < ::String
    def ===(other)
      super if !block_given?
    end
  end

  # Check that the block passed to an iterator
  # does not get propagated inappropriately
  def test_block_given_within_iterator
    assert_equal(["b"], ["a", "b", "c"].grep(IterString.new("b")) {|s| s})
  end

  def test_enumerator
    [1,2,3].each.with_index {|x,i|
      assert_equal(x, i+1)
    }

    e = [1,2,3].each
    assert_equal(1, e.next)
    assert_equal(2, e.next)
    assert_equal(3, e.next)
    assert_raise(StopIteration){e.next}
    e.rewind
    assert_equal(1, e.next)
    e.rewind
    a = []
    loop{a.push e.next}
    assert_equal([1,2,3], a)

    assert_equal([[8, 1, 10], [6, 2, 11], [4, 3, 12]],
                 [8,6,4].zip((1..10),(10..100)).to_a)
  end
end
