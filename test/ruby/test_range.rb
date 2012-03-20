require 'test/unit'
require 'delegate'
require 'timeout'

class TestRange < Test::Unit::TestCase
  def test_range_string
    # XXX: Is this really the test of Range?
    assert_equal([], ("a" ... "a").to_a)
    assert_equal(["a"], ("a" .. "a").to_a)
    assert_equal(["a"], ("a" ... "b").to_a)
    assert_equal(["a", "b"], ("a" .. "b").to_a)
  end

  def test_range_numeric_string
    assert_equal(["6", "7", "8"], ("6".."8").to_a, "[ruby-talk:343187]")
    assert_equal(["6", "7"], ("6"..."8").to_a)
    assert_equal(["9", "10"], ("9".."10").to_a)
    assert_equal(["09", "10"], ("09".."10").to_a, "[ruby-dev:39361]")
    assert_equal(["9", "10"], (SimpleDelegator.new("9").."10").to_a)
    assert_equal(["9", "10"], ("9"..SimpleDelegator.new("10")).to_a)
  end

  def test_range_symbol
    assert_equal([:a, :b], (:a .. :b).to_a)
  end

  def test_evaluation_order
    arr = [1,2]
    r = (arr.shift)..(arr.shift)
    assert_equal(1..2, r, "[ruby-dev:26383]")
  end

  class DuckRange
    def initialize(b,e,excl=false)
      @begin = b
      @end = e
      @excl = excl
    end
    attr_reader :begin, :end

    def exclude_end?
      @excl
    end
  end

  def test_duckrange
    assert_equal("bc", "abcd"[DuckRange.new(1,2)])
  end

  def test_min
    assert_equal(1, (1..2).min)
    assert_equal(nil, (2..1).min)
    assert_equal(1, (1...2).min)

    assert_equal(1.0, (1.0..2.0).min)
    assert_equal(nil, (2.0..1.0).min)
    assert_equal(1, (1.0...2.0).min)

    assert_equal(0, (0..0).min)
    assert_equal(nil, (0...0).min)
  end

  def test_max
    assert_equal(2, (1..2).max)
    assert_equal(nil, (2..1).max)
    assert_equal(1, (1...2).max)

    assert_equal(2.0, (1.0..2.0).max)
    assert_equal(nil, (2.0..1.0).max)
    assert_raise(TypeError) { (1.0...2.0).max }
    assert_raise(TypeError) { (1...1.5).max }
    assert_raise(TypeError) { (1.5...2).max }

    assert_equal(-0x80000002, ((-0x80000002)...(-0x80000001)).max)

    assert_equal(0, (0..0).max)
    assert_equal(nil, (0...0).max)
  end

  def test_initialize_twice
    r = eval("1..2")
    assert_raise(NameError) { r.instance_eval { initialize 3, 4 } }
  end

  def test_uninitialized_range
    r = Range.allocate
    s = Marshal.dump(r)
    r = Marshal.load(s)
    assert_nothing_raised { r.instance_eval { initialize 5, 6} }
  end

  def test_bad_value
    assert_raise(ArgumentError) { (1 .. :a) }
  end

  def test_exclude_end
    assert(!((0..1).exclude_end?))
    assert((0...1).exclude_end?)
  end

  def test_eq
    r = (0..1)
    assert(r == r)
    assert(r == (0..1))
    assert(r != 0)
    assert(r != (1..2))
    assert(r != (0..2))
    assert(r != (0...1))
    subclass = Class.new(Range)
    assert(r == subclass.new(0,1))
  end

  def test_eql
    r = (0..1)
    assert(r.eql?(r))
    assert(r.eql?(0..1))
    assert(!r.eql?(0))
    assert(!r.eql?(1..2))
    assert(!r.eql?(0..2))
    assert(!r.eql?(0...1))
    subclass = Class.new(Range)
    assert(r.eql?(subclass.new(0,1)))
  end

  def test_hash
    assert((0..1).hash.is_a?(Fixnum))
  end

  def test_step
    a = []
    (0..10).step {|x| a << x }
    assert_equal([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10], a)

    a = []
    (0..10).step(2) {|x| a << x }
    assert_equal([0, 2, 4, 6, 8, 10], a)

    assert_raise(ArgumentError) { (0..10).step(-1) { } }
    assert_raise(ArgumentError) { (0..10).step(0) { } }

    a = []
    ("a" .. "z").step(2) {|x| a << x }
    assert_equal(%w(a c e g i k m o q s u w y), a)

    a = []
    ("a" .. "z").step(2**32) {|x| a << x }
    assert_equal(["a"], a)

    a = []
    (2**32-1 .. 2**32+1).step(2) {|x| a << x }
    assert_equal([4294967295, 4294967297], a)
    zero = (2**32).coerce(0).first
    assert_raise(ArgumentError) { (2**32-1 .. 2**32+1).step(zero) { } }

    o1 = Object.new
    o2 = Object.new
    def o1.<=>(x); -1; end
    def o2.<=>(x); 0; end
    assert_raise(TypeError) { (o1..o2).step(1) { } }

    class << o1; self; end.class_eval do
      define_method(:succ) { o2 }
    end
    a = []
    (o1..o2).step(1) {|x| a << x }
    assert_equal([o1, o2], a)

    a = []
    (o1...o2).step(1) {|x| a << x }
    assert_equal([o1], a)

    assert_nothing_raised("[ruby-dev:34557]") { (0..2).step(0.5) {|x| } }

    a = []
    (0..2).step(0.5) {|x| a << x }
    assert_equal([0, 0.5, 1.0, 1.5, 2.0], a)

    a = []
    (0x40000000..0x40000002).step(0.5) {|x| a << x }
    assert_equal([1073741824, 1073741824.5, 1073741825.0, 1073741825.5, 1073741826], a)

    o = Object.new
    def o.to_int() 1 end
    assert_nothing_raised("[ruby-dev:34558]") { (0..2).step(o) {|x| } }
  end

  def test_step_ruby_core_35753
    assert_equal(6, (1...6.3).step.to_a.size)
    assert_equal(5, (1.1...6).step.to_a.size)
    assert_equal(5, (1...6).step(1.1).to_a.size)
    assert_equal(3, (1.0...5.4).step(1.5).to_a.size)
    assert_equal(3, (1.0...5.5).step(1.5).to_a.size)
    assert_equal(4, (1.0...5.6).step(1.5).to_a.size)
  end

  def test_each
    a = []
    (0..10).each {|x| a << x }
    assert_equal([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10], a)

    o1 = Object.new
    o2 = Object.new
    def o1.setcmp(v) @cmpresult = v end
    o1.setcmp(-1)
    def o1.<=>(x); @cmpresult; end
    def o2.setcmp(v) @cmpresult = v end
    o2.setcmp(0)
    def o2.<=>(x); @cmpresult; end
    class << o1; self; end.class_eval do
      define_method(:succ) { o2 }
    end

    r1 = (o1..o2)
    r2 = (o1...o2)

    a = []
    r1.each {|x| a << x }
    assert_equal([o1, o2], a)

    a = []
    r2.each {|x| a << x }
    assert_equal([o1], a)

    o2.setcmp(1)

    a = []
    r1.each {|x| a << x }
    assert_equal([o1], a)

    o2.setcmp(nil)

    a = []
    r1.each {|x| a << x }
    assert_equal([o1], a)

    o1.setcmp(nil)

    a = []
    r2.each {|x| a << x }
    assert_equal([], a)
  end

  def test_begin_end
    assert_equal(0, (0..1).begin)
    assert_equal(1, (0..1).end)
  end

  def test_first_last
    assert_equal([0, 1, 2], (0..10).first(3))
    assert_equal([8, 9, 10], (0..10).last(3))
  end

  def test_to_s
    assert_equal("0..1", (0..1).to_s)
    assert_equal("0...1", (0...1).to_s)
  end

  def test_inspect
    assert_equal("0..1", (0..1).inspect)
    assert_equal("0...1", (0...1).inspect)
  end

  def test_eqq
    assert((0..10) === 5)
    assert(!((0..10) === 11))
  end

  def test_include
    assert(("a".."z").include?("c"))
    assert(!(("a".."z").include?("5")))
    assert(("a"..."z").include?("y"))
    assert(!(("a"..."z").include?("z")))
    assert(!(("a".."z").include?("cc")))
    assert((0...10).include?(5))
  end

  def test_cover
    assert(("a".."z").cover?("c"))
    assert(!(("a".."z").cover?("5")))
    assert(("a"..."z").cover?("y"))
    assert(!(("a"..."z").cover?("z")))
    assert(("a".."z").cover?("cc"))
  end

  def test_beg_len
    o = Object.new
    assert_raise(TypeError) { [][o] }
    class << o; attr_accessor :begin end
    o.begin = -10
    assert_raise(TypeError) { [][o] }
    class << o; attr_accessor :end end
    o.end = 0
    assert_raise(NoMethodError) { [][o] }
    def o.exclude_end=(v) @exclude_end = v end
    def o.exclude_end?() @exclude_end end
    o.exclude_end = false
    assert_nil([0][o])
    assert_raise(RangeError) { [0][o] = 1 }
    o.begin = 10
    o.end = 10
    assert_nil([0][o])
    o.begin = 0
    assert_equal([0], [0][o])
    o.begin = 2
    o.end = 0
    assert_equal([], [0, 1, 2][o])
  end

  class CyclicRange < Range
    def <=>(other); true; end
  end
  def test_cyclic_range_inspect
    o = CyclicRange.allocate
    o.instance_eval { initialize(o, 1) }
    assert_equal("(... .. ...)..1", o.inspect)
  end

  def test_comparison_when_recursive
    x = CyclicRange.allocate; x.send(:initialize, x, 1)
    y = CyclicRange.allocate; y.send(:initialize, y, 1)
    Timeout.timeout(1) {
      assert x == y
      assert x.eql? y
    }

    z = CyclicRange.allocate; z.send(:initialize, z, :another)
    Timeout.timeout(1) {
      assert x != z
      assert !x.eql?(z)
    }

    x = CyclicRange.allocate
    y = CyclicRange.allocate
    x.send(:initialize, y, 1)
    y.send(:initialize, x, 1)
    Timeout.timeout(1) {
      assert x == y
      assert x.eql?(y)
    }

    x = CyclicRange.allocate
    z = CyclicRange.allocate
    x.send(:initialize, z, 1)
    z.send(:initialize, x, :other)
    Timeout.timeout(1) {
      assert x != z
      assert !x.eql?(z)
    }
  end
end
