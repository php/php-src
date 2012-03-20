require 'test/unit'

class TestMath < Test::Unit::TestCase
  def assert_infinity(a, *rest)
    rest = ["not infinity: #{a.inspect}"] if rest.empty?
    assert(!a.finite?, *rest)
  end

  def assert_nan(a, *rest)
    rest = ["not nan: #{a.inspect}"] if rest.empty?
    assert(a.nan?, *rest)
  end

  def check(a, b)
    err = [Float::EPSILON * 4, [a.abs, b.abs].max * Float::EPSILON * 256].max
    assert_in_delta(a, b, err)
  end

  def test_atan2
    check(+0.0, Math.atan2(+0.0, +0.0))
    check(-0.0, Math.atan2(-0.0, +0.0))
    check(+Math::PI, Math.atan2(+0.0, -0.0))
    check(-Math::PI, Math.atan2(-0.0, -0.0))
    assert_raise(Math::DomainError) { Math.atan2(Float::INFINITY, Float::INFINITY) }
    assert_raise(Math::DomainError) { Math.atan2(Float::INFINITY, -Float::INFINITY) }
    assert_raise(Math::DomainError) { Math.atan2(-Float::INFINITY, Float::INFINITY) }
    assert_raise(Math::DomainError) { Math.atan2(-Float::INFINITY, -Float::INFINITY) }
    check(0, Math.atan2(0, 1))
    check(Math::PI / 4, Math.atan2(1, 1))
    check(Math::PI / 2, Math.atan2(1, 0))
  end

  def test_cos
    check(1.0,  Math.cos(0 * Math::PI / 4))
    check(1.0 / Math.sqrt(2), Math.cos(1 * Math::PI / 4))
    check(0.0,  Math.cos(2 * Math::PI / 4))
    check(-1.0, Math.cos(4 * Math::PI / 4))
    check(0.0,  Math.cos(6 * Math::PI / 4))
  end

  def test_sin
    check(0.0,  Math.sin(0 * Math::PI / 4))
    check(1.0 / Math.sqrt(2), Math.sin(1 * Math::PI / 4))
    check(1.0,  Math.sin(2 * Math::PI / 4))
    check(0.0,  Math.sin(4 * Math::PI / 4))
    check(-1.0, Math.sin(6 * Math::PI / 4))
  end

  def test_tan
    check(0.0, Math.tan(0 * Math::PI / 4))
    check(1.0, Math.tan(1 * Math::PI / 4))
    assert(Math.tan(2 * Math::PI / 4).abs > 1024)
    check(0.0, Math.tan(4 * Math::PI / 4))
    assert(Math.tan(6 * Math::PI / 4).abs > 1024)
  end

  def test_acos
    check(0 * Math::PI / 4, Math.acos( 1.0))
    check(1 * Math::PI / 4, Math.acos( 1.0 / Math.sqrt(2)))
    check(2 * Math::PI / 4, Math.acos( 0.0))
    check(4 * Math::PI / 4, Math.acos(-1.0))
    assert_raise(Math::DomainError) { Math.acos(+1.0 + Float::EPSILON) }
    assert_raise(Math::DomainError) { Math.acos(-1.0 - Float::EPSILON) }
    assert_raise(Math::DomainError) { Math.acos(2.0) }
  end

  def test_asin
    check( 0 * Math::PI / 4, Math.asin( 0.0))
    check( 1 * Math::PI / 4, Math.asin( 1.0 / Math.sqrt(2)))
    check( 2 * Math::PI / 4, Math.asin( 1.0))
    check(-2 * Math::PI / 4, Math.asin(-1.0))
    assert_raise(Math::DomainError) { Math.asin(+1.0 + Float::EPSILON) }
    assert_raise(Math::DomainError) { Math.asin(-1.0 - Float::EPSILON) }
    assert_raise(Math::DomainError) { Math.asin(2.0) }
  end

  def test_atan
    check( 0 * Math::PI / 4, Math.atan( 0.0))
    check( 1 * Math::PI / 4, Math.atan( 1.0))
    check( 2 * Math::PI / 4, Math.atan(1.0 / 0.0))
    check(-1 * Math::PI / 4, Math.atan(-1.0))
  end

  def test_cosh
    check(1, Math.cosh(0))
    check((Math::E ** 1 + Math::E ** -1) / 2, Math.cosh(1))
    check((Math::E ** 2 + Math::E ** -2) / 2, Math.cosh(2))
  end

  def test_sinh
    check(0, Math.sinh(0))
    check((Math::E ** 1 - Math::E ** -1) / 2, Math.sinh(1))
    check((Math::E ** 2 - Math::E ** -2) / 2, Math.sinh(2))
  end

  def test_tanh
    check(Math.sinh(0) / Math.cosh(0), Math.tanh(0))
    check(Math.sinh(1) / Math.cosh(1), Math.tanh(1))
    check(Math.sinh(2) / Math.cosh(2), Math.tanh(2))
  end

  def test_acosh
    check(0, Math.acosh(1))
    check(1, Math.acosh((Math::E ** 1 + Math::E ** -1) / 2))
    check(2, Math.acosh((Math::E ** 2 + Math::E ** -2) / 2))
    assert_raise(Math::DomainError) { Math.acosh(1.0 - Float::EPSILON) }
    assert_raise(Math::DomainError) { Math.acosh(0) }
  end

  def test_asinh
    check(0, Math.asinh(0))
    check(1, Math.asinh((Math::E ** 1 - Math::E ** -1) / 2))
    check(2, Math.asinh((Math::E ** 2 - Math::E ** -2) / 2))
  end

  def test_atanh
    check(0, Math.atanh(Math.sinh(0) / Math.cosh(0)))
    check(1, Math.atanh(Math.sinh(1) / Math.cosh(1)))
    check(2, Math.atanh(Math.sinh(2) / Math.cosh(2)))
    assert_nothing_raised { assert_infinity(Math.atanh(1)) }
    assert_nothing_raised { assert_infinity(-Math.atanh(-1)) }
    assert_raise(Math::DomainError) { Math.atanh(+1.0 + Float::EPSILON) }
    assert_raise(Math::DomainError) { Math.atanh(-1.0 - Float::EPSILON) }
  end

  def test_exp
    check(1, Math.exp(0))
    check(Math.sqrt(Math::E), Math.exp(0.5))
    check(Math::E, Math.exp(1))
    check(Math::E ** 2, Math.exp(2))
  end

  def test_log
    check(0, Math.log(1))
    check(1, Math.log(Math::E))
    check(0, Math.log(1, 10))
    check(1, Math.log(10, 10))
    check(2, Math.log(100, 10))
    assert_equal(1.0/0, Math.log(1.0/0))
    assert_nothing_raised { assert_infinity(-Math.log(+0.0)) }
    assert_nothing_raised { assert_infinity(-Math.log(-0.0)) }
    assert_raise(Math::DomainError) { Math.log(-1.0) }
    assert_raise(TypeError) { Math.log(1,nil) }
  end

  def test_log2
    check(0, Math.log2(1))
    check(1, Math.log2(2))
    check(2, Math.log2(4))
    assert_equal(1.0/0, Math.log2(1.0/0))
    assert_nothing_raised { assert_infinity(-Math.log2(+0.0)) }
    assert_nothing_raised { assert_infinity(-Math.log2(-0.0)) }
    assert_raise(Math::DomainError) { Math.log2(-1.0) }
  end

  def test_log10
    check(0, Math.log10(1))
    check(1, Math.log10(10))
    check(2, Math.log10(100))
    assert_equal(1.0/0, Math.log10(1.0/0))
    assert_nothing_raised { assert_infinity(-Math.log10(+0.0)) }
    assert_nothing_raised { assert_infinity(-Math.log10(-0.0)) }
    assert_raise(Math::DomainError) { Math.log10(-1.0) }
  end

  def test_sqrt
    check(0, Math.sqrt(0))
    check(1, Math.sqrt(1))
    check(2, Math.sqrt(4))
    assert_equal(1.0/0, Math.sqrt(1.0/0))
    assert_equal("0.0", Math.sqrt(-0.0).to_s) # insure it is +0.0, not -0.0
    assert_raise(Math::DomainError) { Math.sqrt(-1.0) }
  end

  def test_frexp
    check(0.0, Math.frexp(0.0).first)
    assert_equal(0, Math.frexp(0).last)
    check(0.5, Math.frexp(0.5).first)
    assert_equal(0, Math.frexp(0.5).last)
    check(0.5, Math.frexp(1.0).first)
    assert_equal(1, Math.frexp(1.0).last)
    check(0.5, Math.frexp(2.0).first)
    assert_equal(2, Math.frexp(2.0).last)
    check(0.75, Math.frexp(3.0).first)
    assert_equal(2, Math.frexp(3.0).last)
  end

  def test_ldexp
    check(0.0, Math.ldexp(0.0, 0.0))
    check(0.5, Math.ldexp(0.5, 0.0))
    check(1.0, Math.ldexp(0.5, 1.0))
    check(2.0, Math.ldexp(0.5, 2.0))
    check(3.0, Math.ldexp(0.75, 2.0))
  end

  def test_hypot
    check(5, Math.hypot(3, 4))
  end

  def test_erf
    check(0, Math.erf(0))
    check(1, Math.erf(1.0 / 0.0))
  end

  def test_erfc
    check(1, Math.erfc(0))
    check(0, Math.erfc(1.0 / 0.0))
  end

  def test_gamma
    sqrt_pi = Math.sqrt(Math::PI)
    check(4 * sqrt_pi / 3, Math.gamma(-1.5))
    check(-2 * sqrt_pi, Math.gamma(-0.5))
    check(sqrt_pi, Math.gamma(0.5))
    check(1, Math.gamma(1))
    check(sqrt_pi / 2, Math.gamma(1.5))
    check(1, Math.gamma(2))
    check(3 * sqrt_pi / 4, Math.gamma(2.5))
    check(2, Math.gamma(3))
    check(15 * sqrt_pi / 8, Math.gamma(3.5))
    check(6, Math.gamma(4))

    # no SEGV [ruby-core:25257]
    31.upto(65) do |i|
      i = 1 << i
      assert_infinity(Math.gamma(i), "Math.gamma(#{i}) should be INF")
      assert_infinity(Math.gamma(i-1), "Math.gamma(#{i-1}) should be INF")
    end

    assert_raise(Math::DomainError) { Math.gamma(-Float::INFINITY) }
  end

  def test_lgamma
    sqrt_pi = Math.sqrt(Math::PI)

    g, s = Math.lgamma(-1.5)
    check(Math.log(4 * sqrt_pi / 3), g)
    assert_equal(s, 1)

    g, s = Math.lgamma(-0.5)
    check(Math.log(2 * sqrt_pi), g)
    assert_equal(s, -1)

    g, s = Math.lgamma(0.5)
    check(Math.log(sqrt_pi), g)
    assert_equal(s, 1)

    assert_equal([0, 1], Math.lgamma(1))

    g, s = Math.lgamma(1.5)
    check(Math.log(sqrt_pi / 2), g)
    assert_equal(s, 1)

    assert_equal([0, 1], Math.lgamma(2))

    g, s = Math.lgamma(2.5)
    check(Math.log(3 * sqrt_pi / 4), g)
    assert_equal(s, 1)

    g, s = Math.lgamma(3)
    check(Math.log(2), g)
    assert_equal(s, 1)

    g, s = Math.lgamma(3.5)
    check(Math.log(15 * sqrt_pi / 8), g)
    assert_equal(s, 1)

    g, s = Math.lgamma(4)
    check(Math.log(6), g)
    assert_equal(s, 1)

    assert_raise(Math::DomainError) { Math.lgamma(-Float::INFINITY) }
  end

  def test_cbrt
    check(1, Math.cbrt(1))
    check(-2, Math.cbrt(-8))
    check(3, Math.cbrt(27))
    check(-0.1, Math.cbrt(-0.001))
  end
end
