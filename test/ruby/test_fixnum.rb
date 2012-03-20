require 'test/unit'

class TestFixnum < Test::Unit::TestCase
  def setup
    @verbose = $VERBOSE
    $VERBOSE = nil
  end

  def teardown
    $VERBOSE = @verbose
  end

  def test_pow
    [1, 2, 2**64, 2**63*3, 2**64*3].each do |y|
      [-1, 0, 1].each do |x|
        z1 = x**y
        z2 = (-x)**y
        if y % 2 == 1
          assert_equal(z2, -z1)
        else
          assert_equal(z2, z1)
        end
      end
    end
  end

  def test_succ
    assert_equal(0x40000000, 0x3fffffff.succ, "[ruby-dev:31189]")
    assert_equal(0x4000000000000000, 0x3fffffffffffffff.succ, "[ruby-dev:31190]")
  end

  def test_pred
    assert_equal(-0x40000001, (-0x40000000).pred)
    assert_equal(-0x4000000000000001, (-0x4000000000000000).pred)
  end

  def test_plus
    assert_equal(0x40000000, 0x3fffffff+1)
    assert_equal(0x4000000000000000, 0x3fffffffffffffff+1)
    assert_equal(-0x40000001, (-0x40000000)+(-1))
    assert_equal(-0x4000000000000001, (-0x4000000000000000)+(-1))
    assert_equal(-0x80000000, (-0x40000000)+(-0x40000000))
  end

  def test_sub
    assert_equal(0x40000000, 0x3fffffff-(-1))
    assert_equal(0x4000000000000000, 0x3fffffffffffffff-(-1))
    assert_equal(-0x40000001, (-0x40000000)-1)
    assert_equal(-0x4000000000000001, (-0x4000000000000000)-1)
    assert_equal(-0x80000000, (-0x40000000)-0x40000000)
  end

  def test_mult
    assert_equal(0x40000000, 0x20000000*2)
    assert_equal(0x4000000000000000, 0x2000000000000000*2)
    assert_equal(-0x40000001, 33025*(-32513))
    assert_equal(-0x4000000000000001, 1380655685*(-3340214413))
    assert_equal(0x40000000, (-0x40000000)*(-1))
  end

  def test_div
    assert_equal(2, 5/2)
    assert_equal(0, 1/2)
    assert_equal(-1, -1/2)
    assert_equal(0, -(1/2))
    assert_equal(-1, (-1)/2)
    assert_equal(0, (-1)/(-2))
    assert_equal(-1, 1/(-2))
    assert_equal(1, -(1/(-2)))
    assert_equal(0x3fffffff, 0xbffffffd/3)
    assert_equal(0x40000000, 0xc0000000/3)
    assert_equal(0x4000000000000000, 0xc000000000000000/3)
    assert_equal(-0x40000001, 0xc0000003/(-3))
    assert_equal(-0x4000000000000001, 0xc000000000000003/(-3))
    assert_equal(0x40000000, (-0x40000000)/(-1), "[ruby-dev:31210]")
    assert_equal(0x4000000000000000, (-0x4000000000000000)/(-1))
  end

  def test_mod
    assert_equal(2, (-0x40000000) % 3)
    assert_equal(0, (-0x40000000) % (-1))
  end

  def test_divmod
    (-5).upto(5) {|a|
      (-5).upto(5) {|b|
        next if b == 0
        q, r = a.divmod(b)
        assert_equal(a, b*q+r)
        assert(r.abs < b.abs)
        assert(0 < b ? (0 <= r && r < b) : (b < r && r <= 0))
        assert_equal(q, a/b)
        assert_equal(q, a.div(b))
        assert_equal(r, a%b)
        assert_equal(r, a.modulo(b))
      }
    }
  end

  def test_not
    assert_equal(-0x40000000, ~0x3fffffff)
    assert_equal(0x3fffffff, ~-0x40000000)
  end

  def test_lshift
    assert_equal(0x40000000, 0x20000000 << 1)
    assert_equal(-0x40000000, (-0x20000000) << 1)
    assert_equal(-0x80000000, (-0x40000000) << 1)
  end

  def test_rshift
    assert_equal(0x20000000, 0x40000000 >> 1)
    assert_equal(-0x20000000, (-0x40000000) >> 1)
    assert_equal(-0x40000000, (-0x80000000) >> 1)
  end

  def test_abs
    assert_equal(0x40000000, (-0x40000000).abs)
    assert_equal(0x4000000000000000, (-0x4000000000000000).abs)
  end

  def test_to_s
    assert_equal("1010", 10.to_s(2))
    assert_equal("a", 10.to_s(36))
    assert_raise(ArgumentError) { 10.to_s(1) }
  end

  def test_plus2
    assert_equal(2, 1 + 1)
    assert_equal(4294967297, 1 + 2**32)
    assert_equal(2.0, 1 + 1.0)
    assert_raise(TypeError) { 1 + nil }
  end

  def test_minus
    assert_equal(0, 1 - 1)
    assert_equal(-4294967295, 1 - 2**32)
    assert_equal(0.0, 1 - 1.0)
    assert_raise(TypeError) { 1 - nil }
  end

  def test_mul
    assert_equal(6, 2.send(:*, 3))
    a = 2**30-1
    assert_equal(1152921502459363329, a.send(:*, a))

    assert_equal(6.0, 2 * 3.0)
    assert_raise(TypeError) { 2 * nil }
  end

  def test_divide
    assert_equal(2.0, 4.quo(2))
    assert_equal(2.0, 4 / 2)
    assert_equal(2.0, 4.div(2))

    assert_equal(0.5**32, 1.quo(2**32))
    assert_equal(0, 1 / (2**32))
    assert_equal(0, 1.div(2**32))

    assert_equal(0.5, 1.quo(2.0))
    assert_equal(0.5, 1 / 2.0)
    assert_equal(0, 1.div(2.0))

    ### rational changes the behavior of Fixnum#quo
    #assert_raise(TypeError) { 2.quo(nil) }
    assert_raise(TypeError, NoMethodError) { 2.quo(nil) }
    assert_raise(TypeError) { 2 / nil }
    assert_raise(TypeError) { 2.div(nil) }

    assert_equal(0, 4.modulo(2))
    assert_equal(1, 1.modulo(2**32))
    assert_equal(1, 1.modulo(2.0))
    assert_raise(TypeError) { 2.modulo(nil) }

    assert_equal([2, 0], 4.divmod(2))
    assert_equal([0, 1], 1.divmod(2**32))
    assert_equal([0, 1], 1.divmod(2.0))
    assert_raise(TypeError) { 2.divmod(nil) }
  end

  def test_pow2
    assert_equal(65536, 2**16)
    assert_equal(4294967296, 2**32)
    assert_equal(0.5**16, 2**-16)
    assert_equal(1, (-1)**4294967296)
    assert_equal(-1, (-1)**4294967295)
    assert_equal(4, 2**((2**32).coerce(2).first))
    assert_equal(2, 4**0.5)
    assert_equal(0, 0**0.5)
    assert_equal(1, (0**-1.0).infinite?)
    ### rational changes the behavior of Fixnum#**
    #assert_raise(TypeError) { 1 ** nil }
    assert_raise(TypeError, NoMethodError) { 1 ** nil }
  end

  def test_cmp
    assert(1 != nil)

    assert_equal(0, 1 <=> 1)
    assert_equal(-1, 1 <=> 4294967296)
    assert_equal(0, 1 <=> 1.0)
    assert_nil(1 <=> nil)

    assert(1.send(:>, 0))
    assert(!(1.send(:>, 1)))
    assert(!(1.send(:>, 2)))
    assert(!(1.send(:>, 4294967296)))
    assert(1.send(:>, 0.0))
    assert_raise(ArgumentError) { 1.send(:>, nil) }

    assert(1.send(:>=, 0))
    assert(1.send(:>=, 1))
    assert(!(1.send(:>=, 2)))
    assert(!(1.send(:>=, 4294967296)))
    assert(1.send(:>=, 0.0))
    assert_raise(ArgumentError) { 1.send(:>=, nil) }

    assert(!(1.send(:<, 0)))
    assert(!(1.send(:<, 1)))
    assert(1.send(:<, 2))
    assert(1.send(:<, 4294967296))
    assert(!(1.send(:<, 0.0)))
    assert_raise(ArgumentError) { 1.send(:<, nil) }

    assert(!(1.send(:<=, 0)))
    assert(1.send(:<=, 1))
    assert(1.send(:<=, 2))
    assert(1.send(:<=, 4294967296))
    assert(!(1.send(:<=, 0.0)))
    assert_raise(ArgumentError) { 1.send(:<=, nil) }
  end

  class DummyNumeric < Numeric
    def to_int
      1
    end
  end

  def test_and_with_float
    assert_raise(TypeError) { 1 & 1.5 }
  end

  def test_and_with_rational
    assert_raise(TypeError, "#1792") { 1 & Rational(3, 2) }
  end

  def test_and_with_nonintegral_numeric
    assert_raise(TypeError, "#1792") { 1 & DummyNumeric.new }
  end

  def test_or_with_float
    assert_raise(TypeError) { 1 | 1.5 }
  end

  def test_or_with_rational
    assert_raise(TypeError, "#1792") { 1 | Rational(3, 2) }
  end

  def test_or_with_nonintegral_numeric
    assert_raise(TypeError, "#1792") { 1 | DummyNumeric.new }
  end

  def test_xor_with_float
    assert_raise(TypeError) { 1 ^ 1.5 }
  end

  def test_xor_with_rational
    assert_raise(TypeError, "#1792") { 1 ^ Rational(3, 2) }
  end

  def test_xor_with_nonintegral_numeric
    assert_raise(TypeError, "#1792") { 1 ^ DummyNumeric.new }
  end
end
