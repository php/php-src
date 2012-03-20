require 'test/unit'

class TestNum2int < Test::Unit::TestCase
  module Num2int
  end
  require '-test-/num2int/num2int'

  SHRT_MIN = -32768
  SHRT_MAX = 32767
  USHRT_MAX = 65535

  INT_MIN = -2147483648
  INT_MAX = 2147483647
  UINT_MAX = 4294967295

  case [0].pack('L!').size
  when 4
    LONG_MAX = 2147483647
    LONG_MIN = -2147483648
    ULONG_MAX = 4294967295
  when 8
    LONG_MAX = 9223372036854775807
    LONG_MIN = -9223372036854775808
    ULONG_MAX = 18446744073709551615
  end

  LLONG_MAX = 9223372036854775807
  LLONG_MIN = -9223372036854775808
  ULLONG_MAX = 18446744073709551615

  FIXNUM_MAX = LONG_MAX/2
  FIXNUM_MIN = LONG_MIN/2

  def test_num2short
    assert_output(SHRT_MIN.to_s) do
      Num2int.print_num2short(SHRT_MIN)
    end
    assert_output(SHRT_MAX.to_s) do
      Num2int.print_num2short(SHRT_MAX)
    end
    assert_raise(RangeError) do
      Num2int.print_num2short(SHRT_MIN-1)
    end
    assert_raise(RangeError) do
      Num2int.print_num2short(SHRT_MAX+1)
    end
  end

  def test_num2ushort
    assert_output("0") do
      Num2int.print_num2ushort(0)
    end
    assert_output(USHRT_MAX.to_s) do
      Num2int.print_num2ushort(USHRT_MAX)
    end
    assert_output(USHRT_MAX.to_s) do
      Num2int.print_num2ushort(-1)
    end
    assert_output((SHRT_MAX+1).to_s) do
      Num2int.print_num2ushort(SHRT_MIN)
    end
    assert_raise(RangeError) do
      Num2int.print_num2ushort(SHRT_MIN-1)
    end
    assert_raise(RangeError) do
      Num2int.print_num2ushort(USHRT_MAX+1)
    end
  end

  def test_num2int
    assert_output(INT_MIN.to_s) do
      Num2int.print_num2int(INT_MIN)
    end
    assert_output(INT_MAX.to_s) do
      Num2int.print_num2int(INT_MAX)
    end
    assert_raise(RangeError) do
      Num2int.print_num2int(INT_MIN-1)
    end
    assert_raise(RangeError) do
      Num2int.print_num2int(INT_MAX+1)
    end
  end

  def test_num2uint
    assert_output("0") do
      Num2int.print_num2uint(0)
    end
    assert_output(UINT_MAX.to_s) do
      Num2int.print_num2uint(UINT_MAX)
    end
    assert_output(UINT_MAX.to_s) do
      Num2int.print_num2uint(-1)
    end
    assert_output((INT_MAX+1).to_s) do
      Num2int.print_num2uint(INT_MIN)
    end
    assert_raise(RangeError) do
      Num2int.print_num2uint(INT_MIN-1)
    end
    assert_raise(RangeError) do
      Num2int.print_num2uint(UINT_MAX+1)
    end
  end

  def test_num2long
    assert_output(LONG_MIN.to_s) do
      Num2int.print_num2long(LONG_MIN)
    end
    assert_output(LONG_MAX.to_s) do
      Num2int.print_num2long(LONG_MAX)
    end
    assert_raise(RangeError) do
      Num2int.print_num2long(LONG_MIN-1)
    end
    assert_raise(RangeError) do
      Num2int.print_num2long(LONG_MAX+1)
    end
    assert_output(FIXNUM_MIN.to_s) do
      Num2int.print_num2long(FIXNUM_MIN)
    end
    assert_output((FIXNUM_MIN-1).to_s) do
      Num2int.print_num2long(FIXNUM_MIN-1)
    end
    assert_output(FIXNUM_MAX.to_s) do
      Num2int.print_num2long(FIXNUM_MAX)
    end
    assert_output((FIXNUM_MAX+1).to_s) do
      Num2int.print_num2long(FIXNUM_MAX+1)
    end
  end

  def test_num2ulong
    assert_output("0") do
      Num2int.print_num2ulong(0)
    end
    assert_output(ULONG_MAX.to_s) do
      Num2int.print_num2ulong(ULONG_MAX)
    end
    assert_output(ULONG_MAX.to_s) do
      Num2int.print_num2ulong(-1)
    end
    assert_output((LONG_MAX+1).to_s) do
      Num2int.print_num2ulong(LONG_MIN)
    end
    assert_raise(RangeError) do
      Num2int.print_num2ulong(LONG_MIN-1)
    end
    assert_raise(RangeError) do
      Num2int.print_num2ulong(ULONG_MAX+1)
    end
    assert_output((ULONG_MAX-FIXNUM_MAX).to_s) do
      Num2int.print_num2ulong(FIXNUM_MIN)
    end
    assert_output((ULONG_MAX-FIXNUM_MAX-1).to_s) do
      Num2int.print_num2ulong(FIXNUM_MIN-1)
    end
    assert_output(FIXNUM_MAX.to_s) do
      Num2int.print_num2ulong(FIXNUM_MAX)
    end
    assert_output((FIXNUM_MAX+1).to_s) do
      Num2int.print_num2ulong(FIXNUM_MAX+1)
    end
  end

  def test_num2ll
    assert_output(LONG_MIN.to_s) do
      Num2int.print_num2ll(LONG_MIN)
    end
    assert_output(LLONG_MAX.to_s) do
      Num2int.print_num2ll(LLONG_MAX)
    end
    assert_raise(RangeError) do
      Num2int.print_num2ll(LLONG_MIN-1)
    end
    assert_raise(RangeError) do
      Num2int.print_num2ll(LLONG_MAX+1)
    end
    assert_output(FIXNUM_MIN.to_s) do
      Num2int.print_num2ll(FIXNUM_MIN)
    end
    assert_output((FIXNUM_MIN-1).to_s) do
      Num2int.print_num2ll(FIXNUM_MIN-1)
    end
    assert_output(FIXNUM_MAX.to_s) do
      Num2int.print_num2ll(FIXNUM_MAX)
    end
    assert_output((FIXNUM_MAX+1).to_s) do
      Num2int.print_num2ll(FIXNUM_MAX+1)
    end
  end if defined?(Num2int.print_num2ll)

  def test_num2ull
    assert_output("0") do
      Num2int.print_num2ull(0)
    end
    assert_output(ULLONG_MAX.to_s) do
      Num2int.print_num2ull(ULLONG_MAX)
    end
    assert_output(ULLONG_MAX.to_s) do
      Num2int.print_num2ull(-1)
    end
    assert_output((LLONG_MAX+1).to_s) do
      Num2int.print_num2ull(LLONG_MIN)
    end
    assert_raise(RangeError) do
      Num2int.print_num2ull(LLONG_MIN-1)
    end
    assert_raise(RangeError) do
      Num2int.print_num2ull(ULLONG_MAX+1)
    end
    assert_output((ULLONG_MAX-FIXNUM_MAX).to_s) do
      Num2int.print_num2ull(FIXNUM_MIN)
    end
    assert_output((ULLONG_MAX-FIXNUM_MAX-1).to_s) do
      Num2int.print_num2ull(FIXNUM_MIN-1)
    end
    assert_output(FIXNUM_MAX.to_s) do
      Num2int.print_num2ull(FIXNUM_MAX)
    end
    assert_output((FIXNUM_MAX+1).to_s) do
      Num2int.print_num2ull(FIXNUM_MAX+1)
    end
  end if defined?(Num2int.print_num2ull)
end


