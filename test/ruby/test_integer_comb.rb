require 'test/unit'

class TestIntegerComb < Test::Unit::TestCase
  VS = [
    -0x1000000000000000000000000000000000000000000000002,
    -0x1000000000000000000000000000000000000000000000001,
    -0x1000000000000000000000000000000000000000000000000,
    -0xffffffffffffffffffffffffffffffffffffffffffffffff,
    -0x1000000000000000000000002,
    -0x1000000000000000000000001,
    -0x1000000000000000000000000,
    -0xffffffffffffffffffffffff,
    -0x10000000000000002,
    -0x10000000000000001,
    -0x10000000000000000,
    -0xffffffffffffffff,
    -0x4000000000000002,
    -0x4000000000000001,
    -0x4000000000000000,
    -0x3fffffffffffffff,
    -0x100000002,
    -0x100000001,
    -0x100000000,
    -0xffffffff,
    -0xc717a08d, # 0xc717a08d * 0x524b2245 = 0x4000000000000001
    -0x80000002,
    -0x80000001,
    -0x80000000,
    -0x7fffffff,
    -0x524b2245,
    -0x40000002,
    -0x40000001,
    -0x40000000,
    -0x3fffffff,
    -0x10002,
    -0x10001,
    -0x10000,
    -0xffff,
    -0x8101, # 0x8101 * 0x7f01 = 0x40000001
    -0x8002,
    -0x8001,
    -0x8000,
    -0x7fff,
    -0x7f01,
    -65,
    -64,
    -63,
    -62,
    -33,
    -32,
    -31,
    -30,
    -3,
    -2,
    -1,
    0,
    1,
    2,
    3,
    30,
    31,
    32,
    33,
    62,
    63,
    64,
    65,
    0x7f01,
    0x7ffe,
    0x7fff,
    0x8000,
    0x8001,
    0x8101,
    0xfffe,
    0xffff,
    0x10000,
    0x10001,
    0x3ffffffe,
    0x3fffffff,
    0x40000000,
    0x40000001,
    0x524b2245,
    0x7ffffffe,
    0x7fffffff,
    0x80000000,
    0x80000001,
    0xc717a08d,
    0xfffffffe,
    0xffffffff,
    0x100000000,
    0x100000001,
    0x3ffffffffffffffe,
    0x3fffffffffffffff,
    0x4000000000000000,
    0x4000000000000001,
    0xfffffffffffffffe,
    0xffffffffffffffff,
    0x10000000000000000,
    0x10000000000000001,
    0xffffffffffffffffffffffff,
    0x1000000000000000000000000,
    0x1000000000000000000000001,
    0xffffffffffffffffffffffffffffffffffffffffffffffff,
    0x1000000000000000000000000000000000000000000000000,
    0x1000000000000000000000000000000000000000000000001
  ]

  #VS.map! {|v| 0x4000000000000000.coerce(v)[0] }

  min = -1
  min *= 2 while min.class == Fixnum
  FIXNUM_MIN = min/2
  max = 1
  max *= 2 while (max-1).class == Fixnum
  FIXNUM_MAX = max/2-1

  def test_fixnum_range
    assert_instance_of(Bignum, FIXNUM_MIN-1)
    assert_instance_of(Fixnum, FIXNUM_MIN)
    assert_instance_of(Fixnum, FIXNUM_MAX)
    assert_instance_of(Bignum, FIXNUM_MAX+1)
  end

  def check_class(n)
    if FIXNUM_MIN <= n && n <= FIXNUM_MAX
      assert_instance_of(Fixnum, n)
    else
      assert_instance_of(Bignum, n)
    end
  end

  def test_aref
    VS.each {|a|
      100.times {|i|
        assert_equal((a >> i).odd? ? 1 : 0, a[i], "(#{a})[#{i}]")
      }
    }
    VS.each {|a|
      VS.each {|b|
        c = nil
        assert_nothing_raised("(#{a})[#{b}]") { c = a[b] }
        check_class(c)
        if b < 0
          assert_equal(0, c, "(#{a})[#{b}]")
        else
          assert_equal((a >> b).odd? ? 1 : 0, c, "(#{a})[#{b}]")
        end
      }
    }
  end

  def test_plus
    VS.each {|a|
      VS.each {|b|
        c = a + b
        check_class(c)
        assert_equal(b + a, c, "#{a} + #{b}")
        assert_equal(a, c - b, "(#{a} + #{b}) - #{b}")
        assert_equal(a-~b-1, c, "#{a} + #{b}") # Hacker's Delight
        assert_equal((a^b)+2*(a&b), c, "#{a} + #{b}") # Hacker's Delight
        assert_equal((a|b)+(a&b), c, "#{a} + #{b}") # Hacker's Delight
        assert_equal(2*(a|b)-(a^b), c, "#{a} + #{b}") # Hacker's Delight
      }
    }
  end

  def test_minus
    VS.each {|a|
      VS.each {|b|
        c = a - b
        check_class(c)
        assert_equal(a, c + b, "(#{a} - #{b}) + #{b}")
        assert_equal(-b, c - a, "(#{a} - #{b}) - #{a}")
        assert_equal(a+~b+1, c, "#{a} - #{b}") # Hacker's Delight
        assert_equal((a^b)-2*(b&~a), c, "#{a} - #{b}") # Hacker's Delight
        assert_equal((a&~b)-(b&~a), c, "#{a} - #{b}") # Hacker's Delight
        assert_equal(2*(a&~b)-(a^b), c, "#{a} - #{b}") # Hacker's Delight
      }
    }
  end

  def test_mult
    VS.each {|a|
      VS.each {|b|
        c = a * b
        check_class(c)
        assert_equal(b * a, c, "#{a} * #{b}")
        assert_equal(b, c / a, "(#{a} * #{b}) / #{a}") if a != 0
        assert_equal(a.abs * b.abs, (a * b).abs, "(#{a} * #{b}).abs")
        assert_equal((a-100)*(b-100)+(a-100)*100+(b-100)*100+10000, c, "#{a} * #{b}")
        assert_equal((a+100)*(b+100)-(a+100)*100-(b+100)*100+10000, c, "#{a} * #{b}")
      }
    }
  end

  def test_divmod
    VS.each {|a|
      VS.each {|b|
        if b == 0
          assert_raise(ZeroDivisionError) { a.divmod(b) }
        else
          q, r = a.divmod(b)
          check_class(q)
          check_class(r)
          assert_equal(a, b*q+r)
          assert(r.abs < b.abs)
          assert(0 < b ? (0 <= r && r < b) : (b < r && r <= 0))
          assert_equal(q, a/b)
          assert_equal(q, a.div(b))
          assert_equal(r, a%b)
          assert_equal(r, a.modulo(b))
        end
      }
    }
  end

  def test_pow
    small_values = VS.find_all {|v| 0 <= v && v < 1000 }
    VS.each {|a|
      small_values.each {|b|
        c = a ** b
        check_class(c)
        d = 1
        b.times { d *= a }
        assert_equal(d, c, "(#{a}) ** #{b}")
        if a != 0
          d = c
          b.times { d /= a }
          assert_equal(1, d, "((#{a}) ** #{b}) / #{a} / ...(#{b} times)...")
        end
      }
    }
  end

  def test_not
    VS.each {|a|
      b = ~a
      check_class(b)
      assert_equal(-1 ^ a, b, "~#{a}")
      assert_equal(-a-1, b, "~#{a}") # Hacker's Delight
      assert_equal(0, a & b, "#{a} & ~#{a}")
      assert_equal(-1, a | b, "#{a} | ~#{a}")
    }
  end

  def test_or
    VS.each {|a|
      VS.each {|b|
        c = a | b
        check_class(c)
        assert_equal(b | a, c, "#{a} | #{b}")
        assert_equal(a + b - (a&b), c, "#{a} | #{b}")
        assert_equal((a & ~b) + b, c, "#{a} | #{b}") # Hacker's Delight
        assert_equal(-1, c | ~a, "(#{a} | #{b}) | ~#{a})")
      }
    }
  end

  def test_and
    VS.each {|a|
      VS.each {|b|
        c = a & b
        check_class(c)
        assert_equal(b & a, c, "#{a} & #{b}")
        assert_equal(a + b - (a|b), c, "#{a} & #{b}")
        assert_equal((~a | b) - ~a, c, "#{a} & #{b}") # Hacker's Delight
        assert_equal(0, c & ~a, "(#{a} & #{b}) & ~#{a}")
      }
    }
  end

  def test_xor
    VS.each {|a|
      VS.each {|b|
        c = a ^ b
        check_class(c)
        assert_equal(b ^ a, c, "#{a} ^ #{b}")
        assert_equal((a|b)-(a&b), c, "#{a} ^ #{b}") # Hacker's Delight
        assert_equal(b, c ^ a, "(#{a} ^ #{b}) ^ #{a}")
      }
    }
  end

  def test_lshift
    small_values = VS.find_all {|v| v < 8000 }
    VS.each {|a|
      small_values.each {|b|
        c = a << b
        check_class(c)
        if 0 <= b
          assert_equal(a, c >> b, "(#{a} << #{b}) >> #{b}")
          assert_equal(a * 2**b, c, "#{a} << #{b}")
        end
        0.upto(c.size*8+10) {|nth|
          assert_equal(a[nth-b], c[nth], "(#{a} << #{b})[#{nth}]")
        }
      }
    }
  end

  def test_rshift
    small_values = VS.find_all {|v| -8000 < v }
    VS.each {|a|
      small_values.each {|b|
        c = a >> b
        check_class(c)
        if b <= 0
          assert_equal(a, c << b, "(#{a} >> #{b}) << #{b}")
          assert_equal(a * 2**(-b), c, "#{a} >> #{b}")
        end
        0.upto(c.size*8+10) {|nth|
          assert_equal(a[nth+b], c[nth], "(#{a} >> #{b})[#{nth}]")
        }
      }
    }
  end

  def test_succ
    VS.each {|a|
      b = a.succ
      check_class(b)
      assert_equal(a+1, b, "(#{a}).succ")
      assert_equal(a, b.pred, "(#{a}).succ.pred")
      assert_equal(a, b-1, "(#{a}).succ - 1")
    }
  end

  def test_pred
    VS.each {|a|
      b = a.pred
      check_class(b)
      assert_equal(a-1, b, "(#{a}).pred")
      assert_equal(a, b.succ, "(#{a}).pred.succ")
      assert_equal(a, b + 1, "(#{a}).pred + 1")
    }
  end

  def test_unary_plus
    VS.each {|a|
      b = +a
      check_class(b)
      assert_equal(a, b, "+(#{a})")
    }
  end

  def test_unary_minus
    VS.each {|a|
      b = -a
      check_class(b)
      assert_equal(0-a, b, "-(#{a})")
      assert_equal(~a+1, b, "-(#{a})")
      assert_equal(0, a+b, "#{a}+(-(#{a}))")
    }
  end

  def test_cmp
    VS.each_with_index {|a, i|
      VS.each_with_index {|b, j|
        assert_equal(i <=> j, a <=> b, "#{a} <=> #{b}")
        assert_equal(i < j, a < b, "#{a} < #{b}")
        assert_equal(i <= j, a <= b, "#{a} <= #{b}")
        assert_equal(i > j, a > b, "#{a} > #{b}")
        assert_equal(i >= j, a >= b, "#{a} >= #{b}")
      }
    }
  end

  def test_eq
    VS.each_with_index {|a, i|
      VS.each_with_index {|b, j|
        c = a == b
        assert_equal(b == a, c, "#{a} == #{b}")
        assert_equal(i == j, c, "#{a} == #{b}")
      }
    }
  end

  def test_abs
    VS.each {|a|
      b = a.abs
      check_class(b)
      if a < 0
        assert_equal(-a, b, "(#{a}).abs")
      else
        assert_equal(a, b, "(#{a}).abs")
      end
    }
  end

  def test_ceil
    VS.each {|a|
      b = a.ceil
      check_class(b)
      assert_equal(a, b, "(#{a}).ceil")
    }
  end

  def test_floor
    VS.each {|a|
      b = a.floor
      check_class(b)
      assert_equal(a, b, "(#{a}).floor")
    }
  end

  def test_round
    VS.each {|a|
      b = a.round
      check_class(b)
      assert_equal(a, b, "(#{a}).round")
    }
  end

  def test_truncate
    VS.each {|a|
      b = a.truncate
      check_class(b)
      assert_equal(a, b, "(#{a}).truncate")
    }
  end

  def test_remainder
    VS.each {|a|
      VS.each {|b|
        if b == 0
          assert_raise(ZeroDivisionError) { a.divmod(b) }
        else
          r = a.remainder(b)
          check_class(r)
          if a < 0
            assert_operator(-b.abs, :<, r, "#{a}.remainder(#{b})")
            assert_operator(0, :>=, r, "#{a}.remainder(#{b})")
          elsif 0 < a
            assert_operator(0, :<=, r, "#{a}.remainder(#{b})")
            assert_operator(b.abs, :>, r, "#{a}.remainder(#{b})")
          else
            assert_equal(0, r, "#{a}.remainder(#{b})")
          end
        end
      }
    }
  end

  def test_zero_nonzero
    VS.each {|a|
      z = a.zero?
      n = a.nonzero?
      if a == 0
        assert_equal(true, z, "(#{a}).zero?")
        assert_equal(nil, n, "(#{a}).nonzero?")
      else
        assert_equal(false, z, "(#{a}).zero?")
        assert_equal(a, n, "(#{a}).nonzero?")
        check_class(n)
      end
      assert(z ^ n, "(#{a}).zero? ^ (#{a}).nonzero?")
    }
  end

  def test_even_odd
    VS.each {|a|
      e = a.even?
      o = a.odd?
      assert_equal((a % 2) == 0, e, "(#{a}).even?")
      assert_equal((a % 2) == 1, o, "(#{a}).odd")
      assert_equal((a & 1) == 0, e, "(#{a}).even?")
      assert_equal((a & 1) == 1, o, "(#{a}).odd")
      assert(e ^ o, "(#{a}).even? ^ (#{a}).odd?")
    }
  end

  def test_to_s
    2.upto(36) {|radix|
      VS.each {|a|
        s = a.to_s(radix)
        b = s.to_i(radix)
        assert_equal(a, b, "(#{a}).to_s(#{radix}).to_i(#{radix})")
      }
    }
  end

  def test_printf_x
    VS.reverse_each {|a|
      s = sprintf("%x", a)
      if /\A\.\./ =~ s
        b = -($'.tr('0123456789abcdef', 'fedcba9876543210').to_i(16) + 1)
      else
        b = s.to_i(16)
      end
      assert_equal(a, b, "sprintf('%x', #{a}) = #{s.inspect}")
    }
  end

  def test_printf_x_sign
    VS.reverse_each {|a|
      s = sprintf("%+x", a)
      b = s.to_i(16)
      assert_equal(a, b, "sprintf('%+x', #{a}) = #{s.inspect}")
      s = sprintf("% x", a)
      b = s.to_i(16)
      assert_equal(a, b, "sprintf('% x', #{a}) = #{s.inspect}")
    }
  end

  def test_printf_o
    VS.reverse_each {|a|
      s = sprintf("%o", a)
      if /\A\.\./ =~ s
        b = -($'.tr('01234567', '76543210').to_i(8) + 1)
      else
        b = s.to_i(8)
      end
      assert_equal(a, b, "sprintf('%o', #{a}) = #{s.inspect}")
    }
  end

  def test_printf_o_sign
    VS.reverse_each {|a|
      s = sprintf("%+o", a)
      b = s.to_i(8)
      assert_equal(a, b, "sprintf('%+o', #{a}) = #{s.inspect}")
      s = sprintf("% o", a)
      b = s.to_i(8)
      assert_equal(a, b, "sprintf('% o', #{a}) = #{s.inspect}")
    }
  end

  def test_printf_b
    VS.reverse_each {|a|
      s = sprintf("%b", a)
      if /\A\.\./ =~ s
        b = -($'.tr('01', '10').to_i(2) + 1)
      else
        b = s.to_i(2)
      end
      assert_equal(a, b, "sprintf('%b', #{a}) = #{s.inspect}")
    }
  end

  def test_printf_b_sign
    VS.reverse_each {|a|
      s = sprintf("%+b", a)
      b = s.to_i(2)
      assert_equal(a, b, "sprintf('%+b', #{a}) = #{s.inspect}")
      s = sprintf("% b", a)
      b = s.to_i(2)
      assert_equal(a, b, "sprintf('% b', #{a}) = #{s.inspect}")
    }
  end

  def test_printf_diu
    VS.reverse_each {|a|
      s = sprintf("%d", a)
      b = s.to_i
      assert_equal(a, b, "sprintf('%d', #{a}) = #{s.inspect}")
      s = sprintf("%i", a)
      b = s.to_i
      assert_equal(a, b, "sprintf('%i', #{a}) = #{s.inspect}")
      s = sprintf("%u", a)
      b = s.to_i
      assert_equal(a, b, "sprintf('%u', #{a}) = #{s.inspect}")
    }
  end

  def test_marshal
    VS.reverse_each {|a|
      s = Marshal.dump(a)
      b = Marshal.load(s)
      assert_equal(a, b, "Marshal.load(Marshal.dump(#{a}))")
    }
  end

  def test_pack
    %w[c C s S s! S! i I i! I! l L l! L! q Q n N v V].each {|template|
      size = [0].pack(template).size
      mask = (1 << (size * 8)) - 1
      if /[A-Znv]/ =~ template
        min = 0
        max = (1 << (size * 8))-1
      else
        min = -(1 << (size * 8 - 1))
        max = (1 << (size * 8 - 1)) - 1
      end
      VS.reverse_each {|a|
        s = [a].pack(template)
        b = s.unpack(template)[0]
        if min <= a && a <= max
          assert_equal(a, b, "[#{a}].pack(#{template.dump}).unpack(#{template.dump})[0]")
        end
        assert_operator(min, :<=, b)
        assert_operator(b, :<=, max)
        assert_equal(a & mask, b & mask, "[#{a}].pack(#{template.dump}).unpack(#{template.dump})[0] & #{mask}")
      }
    }
  end

  def test_pack_ber
    template = "w"
    VS.reverse_each {|a|
      if a < 0
        assert_raise(ArgumentError) { [a].pack(template) }
      else
        s = [a].pack(template)
        b = s.unpack(template)[0]
        assert_equal(a, b, "[#{a}].pack(#{template.dump}).unpack(#{template.dump})")
      end
    }
  end

  def test_pack_utf8
    template = "U"
    VS.reverse_each {|a|
      if a < 0 || 0x7fffffff < a
        assert_raise(RangeError) { [a].pack(template) }
      else
        s = [a].pack(template)
        b = s.unpack(template)[0]
        assert_equal(a, b, "[#{a}].pack(#{template.dump}).unpack(#{template.dump})")
      end
    }
  end
end
