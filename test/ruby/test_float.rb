require 'test/unit'
require_relative 'envutil'

class TestFloat < Test::Unit::TestCase
  include EnvUtil

  def test_float
    assert_equal(2, 2.6.floor)
    assert_equal(-3, (-2.6).floor)
    assert_equal(3, 2.6.ceil)
    assert_equal(-2, (-2.6).ceil)
    assert_equal(2, 2.6.truncate)
    assert_equal(-2, (-2.6).truncate)
    assert_equal(3, 2.6.round)
    assert_equal(-2, (-2.4).truncate)
    assert((13.4 % 1 - 0.4).abs < 0.0001)
    assert_equal(36893488147419111424,
                 36893488147419107329.0.to_i)
  end

  def nan_test(x,y)
    extend Test::Unit::Assertions
    assert(x != y)
    assert_equal(false, (x < y))
    assert_equal(false, (x > y))
    assert_equal(false, (x <= y))
    assert_equal(false, (x >= y))
  end
  def test_nan
    nan = Float::NAN
    nan_test(nan, nan)
    nan_test(nan, 0)
    nan_test(nan, 1)
    nan_test(nan, -1)
    nan_test(nan, 1000)
    nan_test(nan, -1000)
    nan_test(nan, 1_000_000_000_000)
    nan_test(nan, -1_000_000_000_000)
    nan_test(nan, 100.0);
    nan_test(nan, -100.0);
    nan_test(nan, 0.001);
    nan_test(nan, -0.001);
    nan_test(nan, 1.0/0);
    nan_test(nan, -1.0/0);
  end

  def test_precision
    u = 3.7517675036461267e+17
    v = sprintf("%.16e", u).to_f
    assert_in_delta(u, v, u.abs * Float::EPSILON)
    assert_in_delta(u, v, v.abs * Float::EPSILON)
  end

  def test_symmetry_bignum # [ruby-bugs-ja:118]
    a = 100000000000000000000000
    b = 100000000000000000000000.0
    assert_equal(a == b, b == a)
  end

  def test_strtod
    a = Float("0")
    assert(a.abs < Float::EPSILON)
    a = Float("0.0")
    assert(a.abs < Float::EPSILON)
    a = Float("+0.0")
    assert(a.abs < Float::EPSILON)
    a = Float("-0.0")
    assert(a.abs < Float::EPSILON)
    a = Float("0.0000000000000000001")
    assert(a != 0.0)
    a = Float("+0.0000000000000000001")
    assert(a != 0.0)
    a = Float("-0.0000000000000000001")
    assert(a != 0.0)
    a = Float(".0")
    assert(a.abs < Float::EPSILON)
    a = Float("+.0")
    assert(a.abs < Float::EPSILON)
    a = Float("-.0")
    assert(a.abs < Float::EPSILON)
    assert_raise(ArgumentError){Float("0.")}
    assert_raise(ArgumentError){Float("+0.")}
    assert_raise(ArgumentError){Float("-0.")}
    assert_raise(ArgumentError){Float(".")}
    assert_raise(ArgumentError){Float("+")}
    assert_raise(ArgumentError){Float("+.")}
    assert_raise(ArgumentError){Float("-")}
    assert_raise(ArgumentError){Float("-.")}
    assert_raise(ArgumentError){Float("1e")}
    assert_raise(ArgumentError){Float("1__1")}
    assert_raise(ArgumentError){Float("1.")}
    assert_raise(ArgumentError){Float("1.e+00")}
    assert_raise(ArgumentError){Float("0x1.p+0")}
    # add expected behaviour here.
    assert_equal(10, Float("1_0"))

    assert_equal([ 0.0].pack('G'), [Float(" 0x0p+0").to_f].pack('G'))
    assert_equal([-0.0].pack('G'), [Float("-0x0p+0").to_f].pack('G'))
    assert_equal(255.0,     Float("0Xff"))
    assert_equal(1024.0,    Float("0x1p10"))
    assert_equal(1024.0,    Float("0x1p+10"))
    assert_equal(0.0009765625, Float("0x1p-10"))
    assert_equal(2.6881171418161356e+43, Float("0x1.3494a9b171bf5p+144"))
    assert_equal(-3.720075976020836e-44, Float("-0x1.a8c1f14e2af5dp-145"))
    assert_equal(31.0*2**1019, Float("0x0."+("0"*268)+"1fp2099"))
    assert_equal(31.0*2**1019, Float("0x0."+("0"*600)+"1fp3427"))
    assert_equal(-31.0*2**1019, Float("-0x0."+("0"*268)+"1fp2099"))
    assert_equal(-31.0*2**1019, Float("-0x0."+("0"*600)+"1fp3427"))
    assert_equal(31.0*2**-1027, Float("0x1f"+("0"*268)+".0p-2099"))
    assert_equal(31.0*2**-1027, Float("0x1f"+("0"*600)+".0p-3427"))
    assert_equal(-31.0*2**-1027, Float("-0x1f"+("0"*268)+".0p-2099"))
    assert_equal(-31.0*2**-1027, Float("-0x1f"+("0"*600)+".0p-3427"))
  end

  def test_divmod
    assert_equal([2, 3.5], 11.5.divmod(4))
    assert_equal([-3, -0.5], 11.5.divmod(-4))
    assert_equal([-3, 0.5], (-11.5).divmod(4))
    assert_equal([2, -3.5], (-11.5).divmod(-4))
  end

  def test_div
    assert_equal(2, 11.5.div(4))
    assert_equal(-3, 11.5.div(-4))
    assert_equal(-3, (-11.5).div(4))
    assert_equal(2, (-11.5).div(-4))
  end

  def test_modulo
    assert_equal(3.5, 11.5.modulo(4))
    assert_equal(-0.5, 11.5.modulo(-4))
    assert_equal(0.5, (-11.5).modulo(4))
    assert_equal(-3.5, (-11.5).modulo(-4))
  end

  def test_remainder
    assert_equal(3.5, 11.5.remainder(4))
    assert_equal(3.5, 11.5.remainder(-4))
    assert_equal(-3.5, (-11.5).remainder(4))
    assert_equal(-3.5, (-11.5).remainder(-4))
  end

  def test_to_s
    inf = Float::INFINITY
    assert_equal("Infinity", inf.to_s)
    assert_equal("-Infinity", (-inf).to_s)
    assert_equal("NaN", (inf / inf).to_s)

    assert_equal("1.0e+18", 1000_00000_00000_00000.0.to_s)

    bug3273 = '[ruby-core:30145]'
    [0.21611564636388508, 0.56].each do |f|
      s = f.to_s
      assert_equal(f, s.to_f, bug3273)
      assert_not_equal(f, s.chop.to_f, bug3273)
    end
  end

  def test_coerce
    assert_equal(Float, 1.0.coerce(1).first.class)
  end

  def test_plus
    assert_equal(4.0, 2.0.send(:+, 2))
    assert_equal(4.0, 2.0.send(:+, (2**32).coerce(2).first))
    assert_equal(4.0, 2.0.send(:+, 2.0))
    assert_raise(TypeError) { 2.0.send(:+, nil) }
  end

  def test_minus
    assert_equal(0.0, 2.0.send(:-, 2))
    assert_equal(0.0, 2.0.send(:-, (2**32).coerce(2).first))
    assert_equal(0.0, 2.0.send(:-, 2.0))
    assert_raise(TypeError) { 2.0.send(:-, nil) }
  end

  def test_mul
    assert_equal(4.0, 2.0.send(:*, 2))
    assert_equal(4.0, 2.0.send(:*, (2**32).coerce(2).first))
    assert_equal(4.0, 2.0.send(:*, 2.0))
    assert_raise(TypeError) { 2.0.send(:*, nil) }
  end

  def test_div2
    assert_equal(1.0, 2.0.send(:/, 2))
    assert_equal(1.0, 2.0.send(:/, (2**32).coerce(2).first))
    assert_equal(1.0, 2.0.send(:/, 2.0))
    assert_raise(TypeError) { 2.0.send(:/, nil) }
  end

  def test_modulo2
    assert_equal(0.0, 2.0.send(:%, 2))
    assert_equal(0.0, 2.0.send(:%, (2**32).coerce(2).first))
    assert_equal(0.0, 2.0.send(:%, 2.0))
    assert_raise(TypeError) { 2.0.send(:%, nil) }
  end

  def test_modulo3
    bug6048 = '[ruby-core:42726]'
    assert_equal(4.2, 4.2.send(:%, Float::INFINITY))
    assert_equal(4.2, 4.2 % Float::INFINITY)
    assert_is_minus_zero(-0.0 % 4.2)
    assert_is_minus_zero(-0.0.send :%, 4.2)
    assert_raise(ZeroDivisionError) { 4.2.send(:%, 0.0) }
    assert_raise(ZeroDivisionError) { 4.2 % 0.0 }
    assert_raise(ZeroDivisionError) { 42.send(:%, 0) }
    assert_raise(ZeroDivisionError) { 42 % 0 }
  end

  def test_divmod2
    assert_equal([1.0, 0.0], 2.0.divmod(2))
    assert_equal([1.0, 0.0], 2.0.divmod((2**32).coerce(2).first))
    assert_equal([1.0, 0.0], 2.0.divmod(2.0))
    assert_raise(TypeError) { 2.0.divmod(nil) }

    inf = Float::INFINITY
    assert_raise(ZeroDivisionError) {inf.divmod(0)}

    a, b = (2.0**32).divmod(1.0)
    assert_equal(2**32, a)
    assert_equal(0, b)
  end

  def test_pow
    assert_equal(1.0, 1.0 ** (2**32))
    assert_equal(1.0, 1.0 ** 1.0)
    assert_raise(TypeError) { 1.0 ** nil }
  end

  def test_eql
    inf = Float::INFINITY
    nan = Float::NAN
    assert(1.0.eql?(1.0))
    assert(inf.eql?(inf))
    assert(!(nan.eql?(nan)))
    assert(!(1.0.eql?(nil)))

    assert(1.0 == 1)
    assert(1.0 != 2**32)
    assert(1.0 != nan)
    assert(1.0 != nil)
  end

  def test_cmp
    inf = Float::INFINITY
    nan = Float::NAN
    assert_equal(0, 1.0 <=> 1.0)
    assert_equal(1, 1.0 <=> 0.0)
    assert_equal(-1, 1.0 <=> 2.0)
    assert_nil(1.0 <=> nil)
    assert_nil(1.0 <=> nan)
    assert_nil(nan <=> 1.0)

    assert_equal(0, 1.0 <=> 1)
    assert_equal(1, 1.0 <=> 0)
    assert_equal(-1, 1.0 <=> 2)

    assert_equal(-1, 1.0 <=> 2**32)

    assert_equal(1, inf <=> (Float::MAX.to_i*2))
    assert_equal(-1, -inf <=> (-Float::MAX.to_i*2))
    assert_equal(-1, (Float::MAX.to_i*2) <=> inf)
    assert_equal(1, (-Float::MAX.to_i*2) <=> -inf)

    bug3609 = '[ruby-core:31470]'
    def (pinf = Object.new).infinite?; +1 end
    def (ninf = Object.new).infinite?; -1 end
    def (fin = Object.new).infinite?; nil end
    nonum = Object.new
    assert_equal(0, inf <=> pinf, bug3609)
    assert_equal(1, inf <=> fin, bug3609)
    assert_equal(1, inf <=> ninf, bug3609)
    assert_nil(inf <=> nonum, bug3609)
    assert_equal(-1, -inf <=> pinf, bug3609)
    assert_equal(-1, -inf <=> fin, bug3609)
    assert_equal(0, -inf <=> ninf, bug3609)
    assert_nil(-inf <=> nonum, bug3609)

    assert_raise(ArgumentError) { 1.0 > nil }
    assert_raise(ArgumentError) { 1.0 >= nil }
    assert_raise(ArgumentError) { 1.0 < nil }
    assert_raise(ArgumentError) { 1.0 <= nil }
  end

  def test_zero_p
    assert(0.0.zero?)
    assert(!(1.0.zero?))
  end

  def test_infinite_p
    inf = Float::INFINITY
    assert_equal(1, inf.infinite?)
    assert_equal(-1, (-inf).infinite?)
    assert_nil(1.0.infinite?)
  end

  def test_finite_p
    inf = Float::INFINITY
    assert(!(inf.finite?))
    assert(!((-inf).finite?))
    assert(1.0.finite?)
  end

  def test_floor_ceil_round_truncate
    assert_equal(1, 1.5.floor)
    assert_equal(2, 1.5.ceil)
    assert_equal(2, 1.5.round)
    assert_equal(1, 1.5.truncate)

    assert_equal(2, 2.0.floor)
    assert_equal(2, 2.0.ceil)
    assert_equal(2, 2.0.round)
    assert_equal(2, 2.0.truncate)

    assert_equal(-2, (-1.5).floor)
    assert_equal(-1, (-1.5).ceil)
    assert_equal(-2, (-1.5).round)
    assert_equal(-1, (-1.5).truncate)

    assert_equal(-2, (-2.0).floor)
    assert_equal(-2, (-2.0).ceil)
    assert_equal(-2, (-2.0).round)
    assert_equal(-2, (-2.0).truncate)

    inf = Float::INFINITY
    assert_raise(FloatDomainError) { inf.floor }
    assert_raise(FloatDomainError) { inf.ceil }
    assert_raise(FloatDomainError) { inf.round }
    assert_raise(FloatDomainError) { inf.truncate }
  end

  def test_round_with_precision
    assert_equal(1.100, 1.111.round(1))
    assert_equal(1.110, 1.111.round(2))
    assert_equal(11110.0, 11111.1.round(-1))
    assert_equal(11100.0, 11111.1.round(-2))

    assert_equal(10**300, 1.1e300.round(-300))
    assert_equal(-10**300, -1.1e300.round(-300))
    assert_equal(1.0e-300, 1.1e-300.round(300))
    assert_equal(-1.0e-300, -1.1e-300.round(300))

    bug5227 = '[ruby-core:39093]'
    assert_equal(42.0, 42.0.round(308), bug5227)
    assert_equal(1.0e307, 1.0e307.round(2), bug5227)

    assert_raise(TypeError) {1.0.round("4")}
    assert_raise(TypeError) {1.0.round(nil)}
    def (prec = Object.new).to_int; 2; end
    assert_equal(1.0, 0.998.round(prec))
  end

  VS = [
    18446744073709551617.0,
    18446744073709551616.0,
    18446744073709551615.8,
    18446744073709551615.5,
    18446744073709551615.2,
    18446744073709551615.0,
    18446744073709551614.0,

    4611686018427387905.0,
    4611686018427387904.0,
    4611686018427387903.8,
    4611686018427387903.5,
    4611686018427387903.2,
    4611686018427387903.0,
    4611686018427387902.0,

    4294967297.0,
    4294967296.0,
    4294967295.8,
    4294967295.5,
    4294967295.2,
    4294967295.0,
    4294967294.0,

    1073741825.0,
    1073741824.0,
    1073741823.8,
    1073741823.5,
    1073741823.2,
    1073741823.0,
    1073741822.0,

    -1073741823.0,
    -1073741824.0,
    -1073741824.2,
    -1073741824.5,
    -1073741824.8,
    -1073741825.0,
    -1073741826.0,

    -4294967295.0,
    -4294967296.0,
    -4294967296.2,
    -4294967296.5,
    -4294967296.8,
    -4294967297.0,
    -4294967298.0,

    -4611686018427387903.0,
    -4611686018427387904.0,
    -4611686018427387904.2,
    -4611686018427387904.5,
    -4611686018427387904.8,
    -4611686018427387905.0,
    -4611686018427387906.0,

    -18446744073709551615.0,
    -18446744073709551616.0,
    -18446744073709551616.2,
    -18446744073709551616.5,
    -18446744073709551616.8,
    -18446744073709551617.0,
    -18446744073709551618.0,
  ]

  def test_truncate
    VS.each {|f|
      i = f.truncate
      assert_equal(i, f.to_i)
      if f < 0
        assert_operator(i, :<, 0)
      else
        assert_operator(i, :>, 0)
      end
      assert_operator(i.abs, :<=, f.abs)
      d = f.abs - i.abs
      assert_operator(0, :<=, d)
      assert_operator(d, :<, 1)
    }
  end

  def test_ceil
    VS.each {|f|
      i = f.ceil
      if f < 0
        assert_operator(i, :<, 0)
      else
        assert_operator(i, :>, 0)
      end
      assert_operator(i, :>=, f)
      d = f - i
      assert_operator(-1, :<, d)
      assert_operator(d, :<=, 0)
    }
  end

  def test_floor
    VS.each {|f|
      i = f.floor
      if f < 0
        assert_operator(i, :<, 0)
      else
        assert_operator(i, :>, 0)
      end
      assert_operator(i, :<=, f)
      d = f - i
      assert_operator(0, :<=, d)
      assert_operator(d, :<, 1)
    }
  end

  def test_round
    VS.each {|f|
      msg = "round(#{f})"
      i = f.round
      if f < 0
        assert_operator(i, :<, 0, msg)
      else
        assert_operator(i, :>, 0, msg)
      end
      d = f - i
      assert_operator(-0.5, :<=, d, msg)
      assert_operator(d, :<=, 0.5, msg)
    }
  end

  def test_Float
    assert_in_delta(0.125, Float("0.1_2_5"), 0.00001)
    assert_in_delta(0.125, "0.1_2_5__".to_f, 0.00001)
    assert_equal(1, suppress_warning {Float(([1] * 10000).join)}.infinite?)
    assert(!Float(([1] * 10000).join("_")).infinite?) # is it really OK?
    assert_raise(ArgumentError) { Float("1.0\x001") }
    assert_equal(15.9375, Float('0xf.fp0'))
    assert_raise(ArgumentError) { Float('0x') }
    assert_equal(15, Float('0xf'))
    assert_equal(15, Float('0xfp0'))
    assert_raise(ArgumentError) { Float('0xfp') }
    assert_raise(ArgumentError) { Float('0xf.') }
    assert_raise(ArgumentError) { Float('0xf.p') }
    assert_raise(ArgumentError) { Float('0xf.p0') }
    assert_raise(ArgumentError) { Float('0xf.f') }
    assert_raise(ArgumentError) { Float('0xf.fp') }
    assert_equal(Float::INFINITY, Float('0xf.fp1000000000000000'))
    assert_equal(1, suppress_warning {Float("1e10_00")}.infinite?)
    assert_raise(TypeError) { Float(nil) }
    o = Object.new
    def o.to_f; inf = Float::INFINITY; inf/inf; end
    assert(Float(o).nan?)
  end

  def test_invalid_str
    bug4310 = '[ruby-core:34820]'
    assert_raise(ArgumentError, bug4310) {under_gc_stress {Float('a'*10000)}}
  end

  def test_num2dbl
    assert_raise(TypeError) do
      1.0.step(2.0, "0.5") {}
    end
    assert_raise(TypeError) do
      1.0.step(2.0, nil) {}
    end
  end

  def test_sleep_with_Float
    assert_nothing_raised("[ruby-core:23282]") do
      sleep(0.1+0.1+0.1+0.1+0.1+0.1+0.1+0.1+0.1+0.1)
    end
  end

  def test_step
    1000.times do
      a = rand
      b = a+rand*1000
      s = (b - a) / 10
      assert_equal(11, (a..b).step(s).to_a.length)
    end

    (1.0..12.7).step(1.3).each do |n|
      assert_operator(n, :<=, 12.7)
    end

    assert_equal([5.0, 4.0, 3.0, 2.0], 5.0.step(1.5, -1).to_a)
  end

  def test_step_excl
    1000.times do
      a = rand
      b = a+rand*1000
      s = (b - a) / 10
      assert_equal(10, (a...b).step(s).to_a.length)
    end

    assert_equal([1.0, 2.9, 4.8, 6.699999999999999], (1.0...6.8).step(1.9).to_a)

    e = 1+1E-12
    (1.0 ... e).step(1E-16) do |n|
      assert_operator(n, :<=, e)
    end
  end
end
