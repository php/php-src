require 'test/unit'

class TestNumeric < Test::Unit::TestCase
  class DummyNumeric < Numeric
  end

  def test_coerce
    a, b = 1.coerce(2)
    assert_equal(Fixnum, a.class)
    assert_equal(Fixnum, b.class)

    a, b = 1.coerce(2.0)
    assert_equal(Float, a.class)
    assert_equal(Float, b.class)

    assert_raise(TypeError) { -Numeric.new }
  end

  def test_dummynumeric
    a = DummyNumeric.new

    DummyNumeric.class_eval do
      def coerce(x); nil; end
    end
    assert_raise(TypeError) { -a }
    assert_nil(1 <=> a)
    assert_raise(ArgumentError) { 1 <= a }

    DummyNumeric.class_eval do
      remove_method :coerce
      def coerce(x); 1.coerce(x); end
    end
    assert_equal(2, 1 + a)
    assert_equal(0, 1 <=> a)
    assert(1 <= a)

    DummyNumeric.class_eval do
      remove_method :coerce
      def coerce(x); [x, 1]; end
    end
    assert_equal(-1, -a)

  ensure
    DummyNumeric.class_eval do
      remove_method :coerce
    end
  end

  def test_numeric
    a = Numeric.new
    assert_raise(TypeError) { def a.foo; end }
    assert_raise(TypeError) { eval("def a.\u3042; end") }
    assert_raise(TypeError) { a.dup }
  end

  def test_quo
    assert_raise(ArgumentError) {DummyNumeric.new.quo(1)}
  end

  def test_divmod
=begin
    DummyNumeric.class_eval do
      def /(x); 42.0; end
      def %(x); :mod; end
    end

    assert_equal(42, DummyNumeric.new.div(1))
    assert_equal(:mod, DummyNumeric.new.modulo(1))
    assert_equal([42, :mod], DummyNumeric.new.divmod(1))
=end

    assert_kind_of(Integer, 11.divmod(3.5).first, '[ruby-dev:34006]')

=begin
  ensure
    DummyNumeric.class_eval do
      remove_method :/, :%
    end
=end
  end

  def test_real_p
    assert(Numeric.new.real?)
  end

  def test_integer_p
    assert(!Numeric.new.integer?)
  end

  def test_abs
    a = DummyNumeric.new
    DummyNumeric.class_eval do
      def -@; :ok; end
      def <(x); true; end
    end

    assert_equal(:ok, a.abs)

    DummyNumeric.class_eval do
      remove_method :<
      def <(x); false; end
    end

    assert_equal(a, a.abs)

  ensure
    DummyNumeric.class_eval do
      remove_method :-@, :<
    end
  end

  def test_zero_p
    DummyNumeric.class_eval do
      def ==(x); true; end
    end

    assert(DummyNumeric.new.zero?)

  ensure
    DummyNumeric.class_eval do
      remove_method :==
    end
  end

  def test_to_int
    DummyNumeric.class_eval do
      def to_i; :ok; end
    end

    assert_equal(:ok, DummyNumeric.new.to_int)

  ensure
    DummyNumeric.class_eval do
      remove_method :to_i
    end
  end

  def test_cmp
    a = Numeric.new
    assert_equal(0, a <=> a)
    assert_nil(a <=> :foo)
  end

  def test_floor_ceil_round_truncate
    DummyNumeric.class_eval do
      def to_f; 1.5; end
    end

    a = DummyNumeric.new
    assert_equal(1, a.floor)
    assert_equal(2, a.ceil)
    assert_equal(2, a.round)
    assert_equal(1, a.truncate)

    DummyNumeric.class_eval do
      remove_method :to_f
      def to_f; 1.4; end
    end

    a = DummyNumeric.new
    assert_equal(1, a.floor)
    assert_equal(2, a.ceil)
    assert_equal(1, a.round)
    assert_equal(1, a.truncate)

    DummyNumeric.class_eval do
      remove_method :to_f
      def to_f; -1.5; end
    end

    a = DummyNumeric.new
    assert_equal(-2, a.floor)
    assert_equal(-1, a.ceil)
    assert_equal(-2, a.round)
    assert_equal(-1, a.truncate)

  ensure
    DummyNumeric.class_eval do
      remove_method :to_f
    end
  end

  def test_step
    a = []
    1.step(10) {|x| a << x }
    assert_equal([1, 2, 3, 4, 5, 6, 7, 8, 9, 10], a)

    a = []
    1.step(10, 2) {|x| a << x }
    assert_equal([1, 3, 5, 7, 9], a)

    assert_raise(ArgumentError) { 1.step(10, 1, 0) { } }
    assert_raise(ArgumentError) { 1.step(10, 0) { } }

    a = []
    10.step(1, -2) {|x| a << x }
    assert_equal([10, 8, 6, 4, 2], a)

    a = []
    1.0.step(10.0, 2.0) {|x| a << x }
    assert_equal([1.0, 3.0, 5.0, 7.0, 9.0], a)

    a = []
    1.step(10, 2**32) {|x| a << x }
    assert_equal([1], a)

    a = []
    10.step(1, -(2**32)) {|x| a << x }
    assert_equal([10], a)

    a = []
    1.step(0, Float::INFINITY) {|x| a << x }
    assert_equal([], a)

    a = []
    0.step(1, -Float::INFINITY) {|x| a << x }
    assert_equal([], a)
  end

  def test_num2long
    assert_raise(TypeError) { 1 & nil }
    assert_raise(TypeError) { 1 & 1.0 }
    assert_raise(TypeError) { 1 & 2147483648.0 }
    assert_raise(TypeError) { 1 & 9223372036854777856.0 }
    o = Object.new
    def o.to_int; 1; end
    assert_raise(TypeError) { assert_equal(1, 1 & o) }
  end

  def test_eql
    assert(1 == 1.0)
    assert(!(1.eql?(1.0)))
    assert(!(1.eql?(2)))
  end
end
