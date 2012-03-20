require 'test/unit'

class TestInteger < Test::Unit::TestCase
  BDSIZE = 0x4000000000000000.coerce(0)[0].size
  def self.bdsize(x)
    ((x + 1) / 8 + BDSIZE) / BDSIZE * BDSIZE
  end
  def bdsize(x)
    self.class.bdsize(x)
  end

  def test_aref
    # assert_equal(1, (1 << 0x40000000)[0x40000000], "[ruby-dev:31271]")
    # assert_equal(0, (-1 << 0x40000001)[0x40000000], "[ruby-dev:31271]")
    big_zero = 0x40000000.coerce(0)[0]
    assert_equal(0, (-0x40000002)[big_zero], "[ruby-dev:31271]")
    assert_equal(1, 0x400000001[big_zero], "[ruby-dev:31271]")
  end

  def test_pow
    assert_not_equal(0, begin
                          0**-1
                        rescue
                          nil
                        end, "[ruby-dev:32084] [ruby-dev:34547]")
  end

  def test_lshift
    assert_equal(0, 1 << -0x40000000)
    assert_equal(0, 1 << -0x40000001)
    assert_equal(0, 1 << -0x80000000)
    assert_equal(0, 1 << -0x80000001)
    # assert_equal(bdsize(0x80000000), (1 << 0x80000000).size)
  end

  def test_rshift
    # assert_equal(bdsize(0x40000001), (1 >> -0x40000001).size)
    assert((1 >> 0x80000000).zero?)
    assert((1 >> 0xffffffff).zero?)
    assert((1 >> 0x100000000).zero?)
    # assert_equal((1 << 0x40000000), (1 >> -0x40000000))
    # assert_equal((1 << 0x40000001), (1 >> -0x40000001))
  end

  def test_Integer
    assert_raise(ArgumentError) {Integer("0x-1")}
    assert_raise(ArgumentError) {Integer("-0x-1")}
    assert_raise(ArgumentError) {Integer("0x     123")}
    assert_raise(ArgumentError) {Integer("0x      123")}
    assert_raise(ArgumentError) {Integer("0x0x5")}
    assert_raise(ArgumentError) {Integer("0x0x000000005")}
    assert_nothing_raised(ArgumentError) {
      assert_equal(1540841, "0x0x5".to_i(36))
    }
    assert_raise(ArgumentError) { Integer("--0") }
    assert_raise(ArgumentError) { Integer("-+0") }
    assert_raise(ArgumentError) { Integer("++1") }
    assert_raise(ArgumentError) { Integer("") }
    assert_raise(ArgumentError) { Integer("10  x") }
    assert_raise(ArgumentError) { Integer("1__2") }
    assert_raise(ArgumentError) { Integer("1z") }
    assert_raise(ArgumentError) { Integer("46116860184273__87904") }
    assert_raise(ArgumentError) { Integer("4611686018427387904_") }
    assert_raise(ArgumentError) { Integer("4611686018427387904  :") }
    assert_equal(0x4000000000000000, Integer("46_11_686_0184273_87904"))
    assert_raise(ArgumentError) { Integer("\0") }
    assert_nothing_raised(ArgumentError, "[ruby-core:13873]") {
      assert_equal(0, Integer("0 "))
    }
    assert_nothing_raised(ArgumentError, "[ruby-core:14139]") {
      assert_equal(0377, Integer("0_3_7_7"))
    }
    assert_raise(ArgumentError, "[ruby-core:14139]") {Integer("0__3_7_7")}
    assert_equal(1234, Integer(1234))
    assert_equal(1, Integer(1.234))

    # base argument
    assert_equal(1234, Integer("1234", 10))
    assert_equal(668, Integer("1234", 8))
    assert_equal(4660, Integer("1234", 16))
    assert_equal(49360, Integer("1234", 36))
    # decimal, not octal
    assert_equal(1234, Integer("01234", 10))
    assert_raise(ArgumentError) { Integer("0x123", 10) }
    assert_raise(ArgumentError) { Integer(1234, 10) }
    assert_raise(ArgumentError) { Integer(12.34, 10) }
    assert_raise(ArgumentError) { Integer(Object.new, 1) }

    assert_raise(ArgumentError) { Integer(1, 1, 1) }

    assert_equal(2 ** 50, Integer(2.0 ** 50))
    assert_raise(TypeError) { Integer(nil) }
  end

  def test_int_p
    assert(!(1.0.integer?))
    assert(1.integer?)
  end

  def test_odd_p_even_p
    Fixnum.class_eval do
      alias odd_bak odd?
      alias even_bak even?
      remove_method :odd?, :even?
    end

    assert(1.odd?)
    assert(!(2.odd?))
    assert(!(1.even?))
    assert(2.even?)

  ensure
    Fixnum.class_eval do
      alias odd? odd_bak
      alias even? even_bak
      remove_method :odd_bak, :even_bak
    end
  end

  def test_succ
    assert_equal(2, 1.send(:succ))

    Fixnum.class_eval do
      alias succ_bak succ
      remove_method :succ
    end

    assert_equal(2, 1.succ)
    assert_equal(4294967297, 4294967296.succ)

  ensure
    Fixnum.class_eval do
      alias succ succ_bak
      remove_method :succ_bak
    end
  end

  def test_chr
    assert_equal("a", "a".ord.chr)
    assert_raise(RangeError) { (-1).chr }
    assert_raise(RangeError) { 0x100.chr }
  end

  def test_upto
    a = []
    1.upto(3) {|x| a << x }
    assert_equal([1, 2, 3], a)

    a = []
    1.upto(0) {|x| a << x }
    assert_equal([], a)

    y = 2**30 - 1
    a = []
    y.upto(y+2) {|x| a << x }
    assert_equal([y, y+1, y+2], a)
  end

  def test_downto
    a = []
    -1.downto(-3) {|x| a << x }
    assert_equal([-1, -2, -3], a)

    a = []
    1.downto(2) {|x| a << x }
    assert_equal([], a)

    y = -(2**30)
    a = []
    y.downto(y-2) {|x| a << x }
    assert_equal([y, y-1, y-2], a)
  end

  def test_times
    (2**32).times do |i|
      break if i == 2
    end
  end

  def test_round
    assert_equal(11111, 11111.round)
    assert_equal(Fixnum, 11111.round.class)
    assert_equal(11111, 11111.round(0))
    assert_equal(Fixnum, 11111.round(0).class)

    assert_equal(11111.0, 11111.round(1))
    assert_equal(Float, 11111.round(1).class)
    assert_equal(11111.0, 11111.round(2))
    assert_equal(Float, 11111.round(2).class)

    assert_equal(11110, 11111.round(-1))
    assert_equal(Fixnum, 11111.round(-1).class)
    assert_equal(11100, 11111.round(-2))
    assert_equal(Fixnum, 11111.round(-2).class)

    assert_equal(1111_1111_1111_1111_1111_1111_1111_1110, 1111_1111_1111_1111_1111_1111_1111_1111.round(-1))
    assert_equal(Bignum, 1111_1111_1111_1111_1111_1111_1111_1111.round(-1).class)
    assert_equal(-1111_1111_1111_1111_1111_1111_1111_1110, (-1111_1111_1111_1111_1111_1111_1111_1111).round(-1))
    assert_equal(Bignum, (-1111_1111_1111_1111_1111_1111_1111_1111).round(-1).class)
  end
end
