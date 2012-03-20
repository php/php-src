require 'test/unit'

class TestComparable < Test::Unit::TestCase
  def setup
    @o = Object.new
    @o.extend(Comparable)
  end
  def cmp(b)
    class << @o; self; end.class_eval {
      undef :<=>
      define_method(:<=>, b)
    }
  end

  def test_equal
    cmp->(x) do 0; end
    assert_equal(true, @o == nil)
    cmp->(x) do 1; end
    assert_equal(false, @o == nil)
    cmp->(x) do raise; end
    assert_equal(false, @o == nil)
  end

  def test_gt
    cmp->(x) do 1; end
    assert_equal(true, @o > nil)
    cmp->(x) do 0; end
    assert_equal(false, @o > nil)
    cmp->(x) do -1; end
    assert_equal(false, @o > nil)
  end

  def test_ge
    cmp->(x) do 1; end
    assert_equal(true, @o >= nil)
    cmp->(x) do 0; end
    assert_equal(true, @o >= nil)
    cmp->(x) do -1; end
    assert_equal(false, @o >= nil)
  end

  def test_lt
    cmp->(x) do 1; end
    assert_equal(false, @o < nil)
    cmp->(x) do 0; end
    assert_equal(false, @o < nil)
    cmp->(x) do -1; end
    assert_equal(true, @o < nil)
  end

  def test_le
    cmp->(x) do 1; end
    assert_equal(false, @o <= nil)
    cmp->(x) do 0; end
    assert_equal(true, @o <= nil)
    cmp->(x) do -1; end
    assert_equal(true, @o <= nil)
  end

  def test_between
    cmp->(x) do 0 <=> x end
    assert_equal(false, @o.between?(1, 2))
    assert_equal(false, @o.between?(-2, -1))
    assert_equal(true, @o.between?(-1, 1))
    assert_equal(true, @o.between?(0, 0))
  end

  def test_err
    assert_raise(ArgumentError) { 1.0 < nil }
    assert_raise(ArgumentError) { 1.0 < Object.new }
  end
end
