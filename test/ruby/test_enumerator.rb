require 'test/unit'

class TestEnumerator < Test::Unit::TestCase
  def setup
    @obj = Object.new
    class << @obj
      include Enumerable
      def foo(*a)
        a.each {|x| yield x }
      end
    end
  end

  def enum_test obj
    i = 0
    obj.map{|e|
      e
    }.sort
  end

  def test_iterators
    assert_equal [0, 1, 2], enum_test(3.times)
    assert_equal [:x, :y, :z], enum_test([:x, :y, :z].each)
    assert_equal [[:x, 1], [:y, 2]], enum_test({:x=>1, :y=>2})
  end

  ## Enumerator as Iterator

  def test_next
    e = 3.times
    3.times{|i|
      assert_equal i, e.next
    }
    assert_raise(StopIteration){e.next}
  end

  def test_loop
    e = 3.times
    i = 0
    loop{
      assert_equal(i, e.next)
      i += 1
    }
  end

  def test_nested_iteration
    def (o = Object.new).each
      yield :ok1
      yield [:ok2, :x].each.next
    end
    e = o.to_enum
    assert_equal :ok1, e.next
    assert_equal :ok2, e.next
    assert_raise(StopIteration){e.next}
  end


  def test_initialize
    assert_equal([1, 2, 3], @obj.to_enum(:foo, 1, 2, 3).to_a)
    assert_equal([1, 2, 3], Enumerator.new(@obj, :foo, 1, 2, 3).to_a)
    assert_equal([1, 2, 3], Enumerator.new { |y| i = 0; loop { y << (i += 1) } }.take(3))
    assert_raise(ArgumentError) { Enumerator.new }
  end

  def test_initialize_copy
    assert_equal([1, 2, 3], @obj.to_enum(:foo, 1, 2, 3).dup.to_a)
    e = @obj.to_enum(:foo, 1, 2, 3)
    assert_nothing_raised { assert_equal(1, e.next) }
    assert_raise(TypeError) { e.dup }

    e = Enumerator.new { |y| i = 0; loop { y << (i += 1) } }.dup
    assert_nothing_raised { assert_equal(1, e.next) }
    assert_raise(TypeError) { e.dup }
  end

  def test_gc
    assert_nothing_raised do
      1.times do
        foo = [1,2,3].to_enum
        GC.start
      end
      GC.start
    end
  end

  def test_slice
    assert_equal([[1,2,3],[4,5,6],[7,8,9],[10]], (1..10).each_slice(3).to_a)
  end

  def test_cons
    a = [[1,2,3], [2,3,4], [3,4,5], [4,5,6], [5,6,7], [6,7,8], [7,8,9], [8,9,10]]
    assert_equal(a, (1..10).each_cons(3).to_a)
  end

  def test_with_index
    assert_equal([[1,0],[2,1],[3,2]], @obj.to_enum(:foo, 1, 2, 3).with_index.to_a)
    assert_equal([[1,5],[2,6],[3,7]], @obj.to_enum(:foo, 1, 2, 3).with_index(5).to_a)
  end

  def test_with_object
    obj = [0, 1]
    ret = (1..10).each.with_object(obj) {|i, memo|
      memo[0] += i
      memo[1] *= i
    }
    assert_same(obj, ret)
    assert_equal([55, 3628800], ret)

    a = [2,5,2,1,5,3,4,2,1,0]
    obj = {}
    ret = a.delete_if.with_object(obj) {|i, seen|
      if seen.key?(i)
        true
      else
        seen[i] = true
        false
      end
    }
    assert_same(obj, ret)
    assert_equal([2, 5, 1, 3, 4, 0], a)
  end

  def test_next_rewind
    e = @obj.to_enum(:foo, 1, 2, 3)
    assert_equal(1, e.next)
    assert_equal(2, e.next)
    e.rewind
    assert_equal(1, e.next)
    assert_equal(2, e.next)
    assert_equal(3, e.next)
    assert_raise(StopIteration) { e.next }
  end

  def test_peek
    a = [1]
    e = a.each
    assert_equal(1, e.peek)
    assert_equal(1, e.peek)
    assert_equal(1, e.next)
    assert_raise(StopIteration) { e.peek }
    assert_raise(StopIteration) { e.peek }
  end

  def test_peek_modify
    o = Object.new
    def o.each
      yield 1,2
    end
    e = o.to_enum
    a = e.peek
    a << 3
    assert_equal([1,2], e.peek)
  end

  def test_peek_values_modify
    o = Object.new
    def o.each
      yield 1,2
    end
    e = o.to_enum
    a = e.peek_values
    a << 3
    assert_equal([1,2], e.peek)
  end

  def test_next_after_stopiteration
    a = [1]
    e = a.each
    assert_equal(1, e.next)
    assert_raise(StopIteration) { e.next }
    assert_raise(StopIteration) { e.next }
    e.rewind
    assert_equal(1, e.next)
    assert_raise(StopIteration) { e.next }
    assert_raise(StopIteration) { e.next }
  end

  def test_stop_result
    a = [1]
    res = a.each {}
    e = a.each
    assert_equal(1, e.next)
    exc = assert_raise(StopIteration) { e.next }
    assert_equal(res, exc.result)
  end

  def test_next_values
    o = Object.new
    def o.each
      yield
      yield 1
      yield 1, 2
    end
    e = o.to_enum
    assert_equal(nil, e.next)
    assert_equal(1, e.next)
    assert_equal([1,2], e.next)
    e = o.to_enum
    assert_equal([], e.next_values)
    assert_equal([1], e.next_values)
    assert_equal([1,2], e.next_values)
  end

  def test_peek_values
    o = Object.new
    def o.each
      yield
      yield 1
      yield 1, 2
    end
    e = o.to_enum
    assert_equal(nil, e.peek)
    assert_equal(nil, e.next)
    assert_equal(1, e.peek)
    assert_equal(1, e.next)
    assert_equal([1,2], e.peek)
    assert_equal([1,2], e.next)
    e = o.to_enum
    assert_equal([], e.peek_values)
    assert_equal([], e.next_values)
    assert_equal([1], e.peek_values)
    assert_equal([1], e.next_values)
    assert_equal([1,2], e.peek_values)
    assert_equal([1,2], e.next_values)
    e = o.to_enum
    assert_equal([], e.peek_values)
    assert_equal(nil, e.next)
    assert_equal([1], e.peek_values)
    assert_equal(1, e.next)
    assert_equal([1,2], e.peek_values)
    assert_equal([1,2], e.next)
    e = o.to_enum
    assert_equal(nil, e.peek)
    assert_equal([], e.next_values)
    assert_equal(1, e.peek)
    assert_equal([1], e.next_values)
    assert_equal([1,2], e.peek)
    assert_equal([1,2], e.next_values)
  end

  def test_each_arg
    o = Object.new
    def o.each(ary)
      ary << 1
      yield
    end
    ary = []
    e = o.to_enum.each(ary)
    e.next
    assert_equal([1], ary)
  end

  def test_feed
    o = Object.new
    def o.each(ary)
      ary << yield
      ary << yield
      ary << yield
    end
    ary = []
    e = o.to_enum(:each, ary)
    e.next
    e.feed 1
    e.next
    e.feed 2
    e.next
    e.feed 3
    assert_raise(StopIteration) { e.next }
    assert_equal([1,2,3], ary)
  end

  def test_feed_mixed
    o = Object.new
    def o.each(ary)
      ary << yield
      ary << yield
      ary << yield
    end
    ary = []
    e = o.to_enum(:each, ary)
    e.next
    e.feed 1
    e.next
    e.next
    e.feed 3
    assert_raise(StopIteration) { e.next }
    assert_equal([1,nil,3], ary)
  end

  def test_feed_twice
    o = Object.new
    def o.each(ary)
      ary << yield
      ary << yield
      ary << yield
    end
    ary = []
    e = o.to_enum(:each, ary)
    e.feed 1
    assert_raise(TypeError) { e.feed 2 }
  end

  def test_feed_before_first_next
    o = Object.new
    def o.each(ary)
      ary << yield
      ary << yield
      ary << yield
    end
    ary = []
    e = o.to_enum(:each, ary)
    e.feed 1
    e.next
    e.next
    assert_equal([1], ary)
  end

  def test_rewind_clear_feed
    o = Object.new
    def o.each(ary)
      ary << yield
      ary << yield
      ary << yield
    end
    ary = []
    e = o.to_enum(:each, ary)
    e.next
    e.feed 1
    e.next
    e.feed 2
    e.rewind
    e.next
    e.next
    assert_equal([1,nil], ary)
  end

  def test_feed_yielder
    x = nil
    e = Enumerator.new {|y| x = y.yield; 10 }
    e.next
    e.feed 100
    exc = assert_raise(StopIteration) { e.next }
    assert_equal(100, x)
    assert_equal(10, exc.result)
  end

  def test_inspect
    e = (0..10).each_cons(2)
    assert_equal("#<Enumerator: 0..10:each_cons(2)>", e.inspect)

    e = Enumerator.new {|y| x = y.yield; 10 }
    assert_match(/\A#<Enumerator: .*:each>/, e.inspect)

    a = []
    e = a.each_with_object(a)
    a << e
    assert_equal("#<Enumerator: [#<Enumerator: ...>]:each_with_object([#<Enumerator: ...>])>",
		e.inspect)
  end

  def test_generator
    # note: Enumerator::Generator is a class just for internal
    g = Enumerator::Generator.new {|y| y << 1 << 2 << 3; :foo }
    g2 = g.dup
    a = []
    assert_equal(:foo, g.each {|x| a << x })
    assert_equal([1, 2, 3], a)
    a = []
    assert_equal(:foo, g2.each {|x| a << x })
    assert_equal([1, 2, 3], a)
  end

  def test_generator_args
    g = Enumerator::Generator.new {|y, x| y << 1 << 2 << 3; x }
    a = []
    assert_equal(:bar, g.each(:bar) {|x| a << x })
    assert_equal([1, 2, 3], a)
  end

  def test_yielder
    # note: Enumerator::Yielder is a class just for internal
    a = []
    y = Enumerator::Yielder.new {|x| a << x }
    assert_equal(y, y << 1 << 2 << 3)
    assert_equal([1, 2, 3], a)

    a = []
    y = Enumerator::Yielder.new {|x| a << x }
    assert_equal([1], y.yield(1))
    assert_equal([1, 2], y.yield(2))
    assert_equal([1, 2, 3], y.yield(3))

    assert_raise(LocalJumpError) { Enumerator::Yielder.new }
  end
end

