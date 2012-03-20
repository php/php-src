require 'test/unit'

class TestBignum < Test::Unit::TestCase
  def setup
    @verbose = $VERBOSE
    $VERBOSE = nil
    @fmax = Float::MAX.to_i
    @fmax2 = @fmax * 2
    @big = (1 << 63) - 1
  end

  def teardown
    $VERBOSE = @verbose
  end

  def fact(n)
    return 1 if n == 0
    f = 1
    while n>0
      f *= n
      n -= 1
    end
    return f
  end

  def test_bignum
    $x = fact(40)
    assert_equal($x, $x)
    assert_equal($x, fact(40))
    assert_operator($x, :<, $x+2)
    assert_operator($x, :>, $x-2)
    assert_equal(815915283247897734345611269596115894272000000000, $x)
    assert_not_equal(815915283247897734345611269596115894272000000001, $x)
    assert_equal(815915283247897734345611269596115894272000000001, $x+1)
    assert_equal(335367096786357081410764800000, $x/fact(20))
    $x = -$x
    assert_equal(-815915283247897734345611269596115894272000000000, $x)
    assert_equal(2-(2**32), -(2**32-2))
    assert_equal(2**32 - 5, (2**32-3)-2)

    for i in 1000..1014
      assert_equal(2 ** i, 1 << i)
    end

    n1 = 1 << 1000
    for i in 1000..1014
      assert_equal(n1, 1 << i)
      n1 *= 2
    end

    n2=n1
    for i in 1..10
      n1 = n1 / 2
      n2 = n2 >> 1
      assert_equal(n1, n2)
    end

    for i in 4000..4096
      n1 = 1 << i;
      assert_equal(n1-1, (n1**2-1) / (n1+1))
    end
  end

  def test_calc
    b = 10**80
    a = b * 9 + 7
    assert_equal(7, a.modulo(b))
    assert_equal(-b + 7, a.modulo(-b))
    assert_equal(b + -7, (-a).modulo(b))
    assert_equal(-7, (-a).modulo(-b))
    assert_equal(7, a.remainder(b))
    assert_equal(7, a.remainder(-b))
    assert_equal(-7, (-a).remainder(b))
    assert_equal(-7, (-a).remainder(-b))

    assert_equal(10000000000000000000100000000000000000000, 10**40+10**20)
    assert_equal(100000000000000000000, 10**40/10**20)

    a = 677330545177305025495135714080
    b = 14269972710765292560
    assert_equal(0, a % b)
    assert_equal(0, -a % b)
  end

  def shift_test(a)
    b = a / (2 ** 32)
    c = a >> 32
    assert_equal(b, c)

    b = a * (2 ** 32)
    c = a << 32
    assert_equal(b, c)
  end

  def test_shift
    shift_test(-4518325415524767873)
    shift_test(-0xfffffffffffffffff)
  end

  def test_to_s
    assert_equal("fvvvvvvvvvvvv" ,18446744073709551615.to_s(32), "[ruby-core:10686]")
    assert_equal("g000000000000" ,18446744073709551616.to_s(32), "[ruby-core:10686]")
    assert_equal("3w5e11264sgsf" ,18446744073709551615.to_s(36), "[ruby-core:10686]")
    assert_equal("3w5e11264sgsg" ,18446744073709551616.to_s(36), "[ruby-core:10686]")
    assert_equal("nd075ib45k86f" ,18446744073709551615.to_s(31), "[ruby-core:10686]")
    assert_equal("nd075ib45k86g" ,18446744073709551616.to_s(31), "[ruby-core:10686]")
    assert_equal("1777777777777777777777" ,18446744073709551615.to_s(8))
    assert_equal("-1777777777777777777777" ,-18446744073709551615.to_s(8))
  end

  b = 2**64
  b *= b until Bignum === b

  T_ZERO = b.coerce(0).first
  T_ONE  = b.coerce(1).first
  T_MONE = b.coerce(-1).first
  T31  = b.coerce(2**31).first   # 2147483648
  T31P = b.coerce(T31 - 1).first # 2147483647
  T32  = b.coerce(2**32).first   # 4294967296
  T32P = b.coerce(T32 - 1).first # 4294967295
  T64  = b.coerce(2**64).first   # 18446744073709551616
  T64P = b.coerce(T64 - 1).first # 18446744073709551615
  T1024  = b.coerce(2**1024).first
  T1024P = b.coerce(T1024 - 1).first

  def test_prepare
    assert_instance_of(Bignum, T_ZERO)
    assert_instance_of(Bignum, T_ONE)
    assert_instance_of(Bignum, T_MONE)
    assert_instance_of(Bignum, T31)
    assert_instance_of(Bignum, T31P)
    assert_instance_of(Bignum, T32)
    assert_instance_of(Bignum, T32P)
    assert_instance_of(Bignum, T64)
    assert_instance_of(Bignum, T64P)
    assert_instance_of(Bignum, T1024)
    assert_instance_of(Bignum, T1024P)
  end

  def test_big_2comp
    assert_equal("-4294967296", (~T32P).to_s)
    assert_equal("..f00000000", "%x" % -T32)
  end

  def test_int2inum
    assert_equal([T31P], [T31P].pack("I").unpack("I"))
    assert_equal([T31P], [T31P].pack("i").unpack("i"))
  end

  def test_quad_pack
    assert_equal([    1], [    1].pack("q").unpack("q"))
    assert_equal([-   1], [-   1].pack("q").unpack("q"))
    assert_equal([ T31P], [ T31P].pack("q").unpack("q"))
    assert_equal([-T31P], [-T31P].pack("q").unpack("q"))
    assert_equal([ T64P], [ T64P].pack("Q").unpack("Q"))
    assert_equal([    0], [ T64 ].pack("Q").unpack("Q"))
  end

  def test_str_to_inum
    assert_equal(1, " +1".to_i)
    assert_equal(-1, " -1".to_i)
    assert_equal(0, "++1".to_i)
    assert_equal(73, "111".oct)
    assert_equal(273, "0x111".oct)
    assert_equal(7, "0b111".oct)
    assert_equal(73, "0o111".oct)
    assert_equal(111, "0d111".oct)
    assert_equal(73, "0111".oct)
    assert_equal(111, Integer("111"))
    assert_equal(13, "111".to_i(3))
    assert_raise(ArgumentError) { "111".to_i(37) }
    assert_equal(1333, "111".to_i(36))
    assert_equal(1057, "111".to_i(32))
    assert_equal(0, "00a".to_i)
    assert_equal(1, Integer("1 "))
    assert_raise(ArgumentError) { Integer("1_") }
    assert_raise(ArgumentError) { Integer("1__") }
    assert_raise(ArgumentError) { Integer("1_0 x") }
    assert_equal(T31P, "1111111111111111111111111111111".to_i(2))
    assert_equal(0_2, '0_2'.to_i)
    assert_equal(00_2, '00_2'.to_i)
    assert_equal(00_02, '00_02'.to_i)
  end

  def test_to_s2
    assert_raise(ArgumentError) { T31P.to_s(37) }
    assert_equal("9" * 32768, (10**32768-1).to_s)
    assert_raise(RangeError) { Process.wait(1, T64P) }
    assert_equal("0", T_ZERO.to_s)
    assert_equal("1", T_ONE.to_s)
  end

  def test_to_f
    assert_nothing_raised { T31P.to_f.to_i }
    assert_raise(FloatDomainError) { (1024**1024).to_f.to_i }
    assert_equal(1, (2**50000).to_f.infinite?)
    assert_equal(-1, (-(2**50000)).to_f.infinite?)
  end

  def test_cmp
    assert_operator(T31P, :>, 1)
    assert_operator(T31P, :<, 2147483648.0)
    assert_operator(T31P, :<, T64P)
    assert_operator(T64P, :>, T31P)
    assert_raise(ArgumentError) { T31P < "foo" }
    assert_operator(T64, :<, (1.0/0.0))
    assert_not_operator(T64, :>, (1.0/0.0))
  end

  def test_eq
    assert_not_equal(T31P, 1)
    assert_equal(T31P, 2147483647.0)
    assert_not_equal(T31P, "foo")
    assert_not_equal(2**77889, (1.0/0.0), '[ruby-core:31603]')
  end

  def test_eql
    assert_send([T31P, :eql?, T31P])
  end

  def test_convert
    assert_equal([255], [T_MONE].pack("C").unpack("C"))
    assert_equal([0], [T32].pack("C").unpack("C"))
    assert_raise(RangeError) { 0.to_s(T32) }
  end

  def test_sub
    assert_equal(-T31, T32 - (T32 + T31))
    x = 2**100
    assert_equal(1, (x+2) - (x+1))
    assert_equal(-1, (x+1) - (x+2))
    assert_equal(0, (2**100) - (2.0**100))
    o = Object.new
    def o.coerce(x); [x, 2**100+2]; end
    assert_equal(-1, (2**100+1) - o)
    assert_equal(-1, T_ONE - 2)
  end

  def test_plus
    assert_equal(T32.to_f, T32P + 1.0)
    assert_raise(TypeError) { T32 + "foo" }
    assert_equal(1267651809154049016125877911552, (2**100) + (2**80))
    assert_equal(1267651809154049016125877911552, (2**80) + (2**100))
    assert_equal(2**101, (2**100) + (2.0**100))
    o = Object.new
    def o.coerce(x); [x, 2**80]; end
    assert_equal(1267651809154049016125877911552, (2**100) + o)
  end

  def test_minus
    assert_equal(T32P.to_f, T32 - 1.0)
    assert_raise(TypeError) { T32 - "foo" }
  end

  def test_mul
    assert_equal(T32.to_f, T32 * 1.0)
    assert_raise(TypeError) { T32 * "foo" }
    o = Object.new
    def o.coerce(x); [x, 2**100]; end
    assert_equal(2**180, (2**80) * o)
  end

  def test_mul_balance
    assert_equal(3**7000, (3**5000) * (3**2000))
  end

  def test_divrem
    assert_equal(0, T32 / T64)
  end

  def test_divide
    bug5490 = '[ruby-core:40429]'
    assert_raise(ZeroDivisionError, bug5490) {T1024./(0)}
    assert_equal(Float::INFINITY, T1024./(0.0), bug5490)
  end

  def test_div
    assert_equal(T32.to_f, T32 / 1.0)
    assert_raise(TypeError) { T32 / "foo" }
    assert_equal(0x20000000, 0x40000001.div(2.0), "[ruby-dev:34553]")
    bug5490 = '[ruby-core:40429]'
    assert_raise(ZeroDivisionError, bug5490) {T1024.div(0)}
    assert_raise(ZeroDivisionError, bug5490) {T1024.div(0.0)}
  end

  def test_idiv
    assert_equal(715827882, 1073741824.div(Rational(3,2)), ' [ruby-dev:34066]')
  end

  def test_modulo
    assert_raise(TypeError) { T32 % "foo" }
  end

  def test_remainder
    assert_equal(0, T32.remainder(1))
    assert_raise(TypeError) { T32.remainder("foo") }
  end

  def test_divmod
    assert_equal([T32, 0], T32.divmod(1))
    assert_equal([2, 0], T32.divmod(T31))
    assert_raise(TypeError) { T32.divmod("foo") }
  end

  def test_quo
    assert_equal(T32.to_f, T32.quo(1))
    assert_equal(T32.to_f, T32.quo(1.0))
    assert_equal(T32.to_f, T32.quo(T_ONE))

    assert_raise(TypeError) { T32.quo("foo") }

    assert_equal(1024**1024, (1024**1024).quo(1))
    assert_equal(1024**1024, (1024**1024).quo(1.0))
    assert_equal(1024**1024*2, (1024**1024*2).quo(1))
    inf = 1 / 0.0; nan = inf / inf

    assert_send([(1024**1024*2).quo(nan), :nan?])
  end

  def test_pow
    assert_equal(1.0, T32 ** 0.0)
    assert_equal(1.0 / T32, T32 ** -1)
    assert_equal(1, (T32 ** T32).infinite?)
    assert_equal(1, (T32 ** (2**30-1)).infinite?)

    ### rational changes the behavior of Bignum#**
    #assert_raise(TypeError) { T32**"foo" }
    assert_raise(TypeError, ArgumentError) { T32**"foo" }

    feature3429 = '[ruby-core:30735]'
    assert_instance_of(Bignum, (2 ** 7830457), feature3429)
  end

  def test_and
    assert_equal(0, T32 & 1)
    assert_equal(-T32, (-T32) & (-T31))
    assert_equal(0, T32 & T64)
  end

  def test_or
    assert_equal(T32 + 1, T32 | 1)
    assert_equal(T32 + T31, T32 | T31)
    assert_equal(-T31, (-T32) | (-T31))
    assert_equal(T64 + T32, T32 | T64)
  end

  def test_xor
    assert_equal(T32 + 1, T32 ^ 1)
    assert_equal(T32 + T31, T32 ^ T31)
    assert_equal(T31, (-T32) ^ (-T31))
    assert_equal(T64 + T32, T32 ^ T64)
  end

  class DummyNumeric < Numeric
    def to_int
      1
    end
  end

  def test_and_with_float
    assert_raise(TypeError) { T1024 & 1.5 }
  end

  def test_and_with_rational
    assert_raise(TypeError, "#1792") { T1024 & Rational(3, 2) }
  end

  def test_and_with_nonintegral_numeric
    assert_raise(TypeError, "#1792") { T1024 & DummyNumeric.new }
  end

  def test_or_with_float
    assert_raise(TypeError) { T1024 | 1.5 }
  end

  def test_or_with_rational
    assert_raise(TypeError, "#1792") { T1024 | Rational(3, 2) }
  end

  def test_or_with_nonintegral_numeric
    assert_raise(TypeError, "#1792") { T1024 | DummyNumeric.new }
  end

  def test_xor_with_float
    assert_raise(TypeError) { T1024 ^ 1.5 }
  end

  def test_xor_with_rational
    assert_raise(TypeError, "#1792") { T1024 ^ Rational(3, 2) }
  end

  def test_xor_with_nonintegral_numeric
    assert_raise(TypeError, "#1792") { T1024 ^ DummyNumeric.new }
  end

  def test_shift2
    assert_equal(2**33, (2**32) <<  1)
    assert_equal(2**31, (2**32) << -1)
    assert_equal(2**33, (2**32) <<  1.0)
    assert_equal(2**31, (2**32) << -1.0)
    assert_equal(2**33, (2**32) << T_ONE)
    assert_equal(2**31, (2**32) << T_MONE)
    assert_equal(2**31, (2**32) >>  1)
    assert_equal(2**33, (2**32) >> -1)
    assert_equal(2**31, (2**32) >>  1.0)
    assert_equal(2**33, (2**32) >> -1.0)
    assert_equal(2**31, (2**32) >> T_ONE)
    assert_equal(2**33, (2**32) >> T_MONE)
    assert_equal( 0,  (2**32) >> (2**32))
    assert_equal(-1, -(2**32) >> (2**32))
    assert_equal( 0,  (2**32) >> 128)
    assert_equal(-1, -(2**32) >> 128)
    assert_equal( 0,  (2**31) >> 32)
    assert_equal(-1, -(2**31) >> 32)
  end

  def test_aref
    assert_equal(0, (2**32)[0])
    assert_equal(0, (2**32)[2**32])
    assert_equal(0, (2**32)[-(2**32)])
    assert_equal(0, (2**32)[T_ZERO])
    assert_equal(0, (-(2**64))[0])
    assert_equal(1, (-2**256)[256])
  end

  def test_hash
    assert_nothing_raised { T31P.hash }
  end

  def test_coerce
    assert_equal([T64P, T31P], T31P.coerce(T64P))
    assert_raise(TypeError) { T31P.coerce(nil) }
  end

  def test_abs
    assert_equal(T31P, (-T31P).abs)
  end

  def test_size
    assert_kind_of(Integer, T31P.size)
  end

  def test_odd
    assert_equal(true, (2**32+1).odd?)
    assert_equal(false, (2**32).odd?)
  end

  def test_even
    assert_equal(false, (2**32+1).even?)
    assert_equal(true, (2**32).even?)
  end

  def assert_interrupt
    time = Time.now
    start_flag = false
    end_flag = false
    thread = Thread.new do
      start_flag = true
      yield
      end_flag = true
    end
    Thread.pass until start_flag
    thread.raise
    thread.join rescue nil
    time = Time.now - time
    assert_equal([true, false], [start_flag, end_flag])
    assert_operator(time, :<, 10)
  end

  def test_interrupt
    assert_interrupt {(65536 ** 65536).to_s}
  end

  def test_too_big_to_s
    if (big = 2**31-1).is_a?(Fixnum)
      return
    end
    e = assert_raise(RangeError) {(1 << big).to_s}
    assert_match(/too big to convert/, e.message)
  end

  def test_fix_fdiv
    assert_not_equal(0, 1.fdiv(@fmax2))
    assert_in_delta(0.5, 1.fdiv(@fmax2) * @fmax, 0.01)
  end

  def test_big_fdiv
    assert_equal(1, @big.fdiv(@big))
    assert_not_equal(0, @big.fdiv(@fmax2))
    assert_not_equal(0, @fmax2.fdiv(@big))
    assert_not_equal(0, @fmax2.fdiv(@fmax2))
    assert_in_delta(0.5, @fmax.fdiv(@fmax2), 0.01)
    assert_in_delta(1.0, @fmax2.fdiv(@fmax2), 0.01)
  end

  def test_float_fdiv
    b = 1E+300.to_i
    assert_equal(b, (b ** 2).fdiv(b))
    assert_send([@big.fdiv(0.0 / 0.0), :nan?])
    assert_in_delta(1E+300, (10**500).fdiv(1E+200), 1E+285)
  end

  def test_obj_fdiv
    o = Object.new
    def o.coerce(x); [x, 2**100]; end
    assert_equal((2**200).to_f, (2**300).fdiv(o))
  end
end
