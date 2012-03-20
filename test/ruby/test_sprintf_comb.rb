require 'test/unit'
require_relative 'allpairs'

class TestSprintfComb < Test::Unit::TestCase
  VS = [
    #-0x1000000000000000000000000000000000000000000000002,
    #-0x1000000000000000000000000000000000000000000000001,
    #-0x1000000000000000000000000000000000000000000000000,
    #-0xffffffffffffffffffffffffffffffffffffffffffffffff,
    #-0x1000000000000000000000002,
    #-0x1000000000000000000000001,
    #-0x1000000000000000000000000,
    #-0xffffffffffffffffffffffff,
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
    #-0xc717a08d, # 0xc717a08d * 0x524b2245 = 0x4000000000000001
    -0x80000002,
    -0x80000001,
    -0x80000000,
    -0x7fffffff,
    #-0x524b2245,
    -0x40000002,
    -0x40000001,
    -0x40000000,
    -0x3fffffff,
    #-0x10002,
    #-0x10001,
    #-0x10000,
    #-0xffff,
    #-0x8101, # 0x8101 * 0x7f01 = 0x40000001
    #-0x8002,
    #-0x8001,
    #-0x8000,
    #-0x7fff,
    #-0x7f01,
    #-65,
    #-64,
    #-63,
    #-62,
    #-33,
    #-32,
    #-31,
    #-30,
    -3,
    -2,
    -1,
    0,
    1,
    2,
    3,
    #30,
    #31,
    #32,
    #33,
    #62,
    #63,
    #64,
    #65,
    #0x7f01,
    #0x7ffe,
    #0x7fff,
    #0x8000,
    #0x8001,
    #0x8101,
    #0xfffe,
    #0xffff,
    #0x10000,
    #0x10001,
    0x3ffffffe,
    0x3fffffff,
    0x40000000,
    0x40000001,
    #0x524b2245,
    0x7ffffffe,
    0x7fffffff,
    0x80000000,
    0x80000001,
    #0xc717a08d,
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
    #0xffffffffffffffffffffffff,
    #0x1000000000000000000000000,
    #0x1000000000000000000000001,
    #0xffffffffffffffffffffffffffffffffffffffffffffffff,
    #0x1000000000000000000000000000000000000000000000000,
    #0x1000000000000000000000000000000000000000000000001
  ]
  VS.reverse!

  def combination(*args, &b)
    #AllPairs.exhaustive_each(*args, &b)
    AllPairs.each(*args, &b)
  end

  def emu_int(format, v)
    /\A%( )?(\#)?(\+)?(-)?(0)?(\d+)?(?:\.(\d*))?(.)\z/ =~ format
    sp = $1
    hs = $2
    pl = $3
    mi = $4
    zr = $5
    width = $6
    precision = $7
    type = $8
    width = width.to_i if width
    precision = precision.to_i if precision
    prefix = ''

    zr = nil if precision

    zr = nil if mi && zr

    case type
    when 'B'
      radix = 2
      digitmap = {0 => '0', 1 => '1'}
      complement = !pl && !sp
      prefix = '0B' if hs && v != 0
    when 'b'
      radix = 2
      digitmap = {0 => '0', 1 => '1'}
      complement = !pl && !sp
      prefix = '0b' if hs && v != 0
    when 'd'
      radix = 10
      digitmap = {}
      10.times {|i| digitmap[i] = i.to_s }
      complement = false
    when 'o'
      radix = 8
      digitmap = {}
      8.times {|i| digitmap[i] = i.to_s }
      complement = !pl && !sp
    when 'X'
      radix = 16
      digitmap = {}
      16.times {|i| digitmap[i] = i.to_s(16).upcase }
      complement = !pl && !sp
      prefix = '0X' if hs && v != 0
    when 'x'
      radix = 16
      digitmap = {}
      16.times {|i| digitmap[i] = i.to_s(16) }
      complement = !pl && !sp
      prefix = '0x' if hs && v != 0
    else
      raise "unexpected type: #{type.inspect}"
    end

    digits = []
    abs = v.abs
    sign = ''
    while 0 < abs
      digits << (abs % radix)
      abs /= radix
    end

    if v < 0
      if complement
        digits.map! {|d| radix-1 - d }
        carry = 1
        digits.each_index {|i|
          digits[i] += carry
          carry = 0
          if radix <= digits[i]
            digits[i] -= radix
            carry = 1
          end
        }
        if digits.last != radix-1
          digits << (radix-1)
        end
        sign = '..'
      else
        sign = '-'
      end
    else
      if pl
        sign = '+'
      elsif sp
        sign = ' '
      end
    end

    dlen = digits.length
    dlen += 2 if sign == '..'

    if v < 0 && complement
      d = radix - 1
    else
      d = 0
    end
    if precision
      if dlen < precision
        (precision - dlen).times {
          digits << d
        }
      end
    else
      if dlen == 0
        digits << d
      end
    end
    if type == 'o' && hs
      if digits.empty? || digits.last != d
        digits << d
      end
    end

    digits.reverse!

    str = digits.map {|digit| digitmap[digit] }.join

    pad = ''
    nlen = prefix.length + sign.length + str.length
    if width && nlen < width
      len = width - nlen
      if zr
        if complement && v < 0
          pad = digitmap[radix-1] * len
        else
          pad = '0' * len
        end
      else
        pad = ' ' * len
      end
    end

    if / / =~ pad
      if sign == '..'
        str = prefix + sign + str
      else
        str = sign + prefix + str
      end
      if mi
        str = str + pad
      else
        str = pad + str
      end
    else
      if sign == '..'
        str = prefix + sign + pad + str
      else
        str = sign + prefix + pad + str
      end
    end

    str
  end

  def test_format_integer
    combination(
        %w[B b d o X x],
        [nil, 0, 5, 20],
        ["", ".", ".0", ".8", ".20"],
        ['', ' '],
        ['', '#'],
        ['', '+'],
        ['', '-'],
        ['', '0']) {|type, width, precision, sp, hs, pl, mi, zr|
      format = "%#{sp}#{hs}#{pl}#{mi}#{zr}#{width}#{precision}#{type}"
      VS.each {|v|
        r = sprintf format, v
        e = emu_int format, v
        if true
          assert_equal(e, r, "sprintf(#{format.dump}, #{v})")
        else
          if e != r
            puts "#{e.dump}\t#{r.dump}\tsprintf(#{format.dump}, #{v})"
          end
        end
      }
    }
  end

  FLOAT_VALUES = [
    -1e100,
    -123456789.0,
    -1.0,
    -0.0,
    0.0,
    0.01,
    1/3.0,
    2/3.0,
    1.0,
    2.0,
    9.99999999,
    123456789.0,
    1e100,
    Float::MAX,
    Float::MIN,
    Float::EPSILON,
    1+Float::EPSILON,
    #1-Float::EPSILON/2,
    10 + Float::EPSILON*10,
    10 - Float::EPSILON*5,
    1.0/0.0,
    -1.0/0.0,
    0.0/0.0,
  ]

  def split_float10(v)
    if v == 0
      if 1/v < 0
        sign = -1
        v = -v
      else
        sign = 1
      end
    else
      if v < 0
        sign = -1
        v = -v
      else
        sign = 1
      end
    end
    exp = 0
    int = v.floor
    v -= int
    while v != 0
      v *= 2
      int *= 2
      i = v.floor
      v -= i
      int += i
      exp -= 1
    end
    int *= 5 ** (-exp)
    [sign, int, exp]
  end

  def emu_e(sp, hs, pl, mi, zr, width, precision, type, v, sign, int, exp)
    precision = 6 unless precision
    if int == 0
      if precision == 0 && !hs
        result = "0#{type}+00"
      else
        result = "0." + "0" * precision + "#{type}+00"
      end
    else
      if int < 10**precision
        int *= 10**precision
        exp -= precision
      end
      digits = int.to_s.length
      discard = digits - (precision+1)
      if discard != 0
        q, r = int.divmod(10**discard)
        if r < 10**discard / 2
          int = q
          exp += discard
        elsif (q+1).to_s.length == q.to_s.length
          int = q+1
          exp += discard
        else
          discard += 1
          q, r = int.divmod(10**discard)
          int = q+1
          exp += discard
        end
      end
      ints = int.to_s
      frac = ints[1..-1]
      result = ints[0,1]
      e = exp + frac.length
      if precision != 0 || hs
        result << "."
        if precision != 0
          result << frac
        end
      end
      result << type
      if e == 0
        if v.abs < 1
          result << '-00' # glibc 2.7 causes '+00'
        else
          result << '+00'
        end
      else
        result << sprintf("%+03d", e)
      end
      result
    end
    result
  end

  def emu_f(sp, hs, pl, mi, zr, width, precision, type, sign, int, exp)
    precision = 6 unless precision
    if int == 0
      if precision == 0 && !hs
        result = '0'
      else
        result = '0.' + '0' * precision
      end
    else
      if -precision < exp
        int *= 10 ** (precision+exp)
        exp = -precision
      end
      if exp < -precision
        discard = -exp - precision
        q, r = int.divmod(10**discard)
        if 10**discard / 2 <= r
          q += 1
        end
        int = q
        exp += discard
      end
      result = int.to_s
      if result.length <= precision
        result = '0' * (precision+1 - result.length) + result
      end
      if precision != 0 || hs
        if precision == 0
          result << '.'
        else
          result[-precision,0] = '.'
        end
      end
    end
    result
  end

  def emu_float(format, v)
    /\A%( )?(\#)?(\+)?(-)?(0)?(\d+)?(?:\.(\d*))?(.)\z/ =~ format
    sp = $1
    hs = $2
    pl = $3
    mi = $4
    zr = $5
    width = $6
    precision = $7
    type = $8
    width = width.to_i if width
    precision = precision.to_i if precision

    zr = nil if mi && zr

    if v.infinite?
      sign = v < 0 ? -1 : 1
      int = :inf
      hs = zr = nil
    elsif v.nan?
      sign = 1
      int = :nan
      hs = zr = nil
    else
      sign, int, exp = split_float10(v)
    end

    if sign < 0
      sign = '-'
    elsif sign == 0
      sign = ''
    elsif pl
      sign = '+'
    elsif sp
      sign = ' '
    else
      sign = ''
    end

    if v.nan?
      result = 'NaN'
    elsif v.infinite?
      result = 'Inf'
    else
      case type
      when /[eE]/
        result = emu_e(sp, hs, pl, mi, zr, width, precision, type, v, sign, int, exp)
      when /f/
        result = emu_f(sp, hs, pl, mi, zr, width, precision, type, sign, int, exp)
      when /[gG]/
        precision = 6 unless precision
        precision = 1 if precision == 0
        r = emu_e(sp, hs, pl, mi, zr, width, precision-1, type.tr('gG', 'eE'), v, sign, int, exp)
        /[eE]([+-]\d+)/ =~ r
        e = $1.to_i
        if e < -4 || precision <= e
          result = r
        else
          result = emu_f(sp, hs, pl, mi, zr, width, precision-1-e, type, sign, int, exp)
        end
        result.sub!(/\.[0-9]*/) { $&.sub(/\.?0*\z/, '') } if !hs
      else
        raise "unexpected type: #{type}"
      end
    end

    pad = ''
    if width && sign.length + result.length < width
      if zr
        pad = '0' * (width - sign.length - result.length)
      else
        pad = ' ' * (width - sign.length - result.length)
      end
    end
    if mi
      sign + result + pad
    elsif zr
      sign + pad + result
    else
      pad + sign + result
    end

  end

  def test_format_float
    combination(
        %w[e E f g G],
        [nil, 0, 5, 20],
        ["", ".", ".0", ".8", ".20", ".200"],
        ['', ' '],
        ['', '#'],
        ['', '+'],
        ['', '-'],
        ['', '0']) {|type, width, precision, sp, hs, pl, mi, zr|
      format = "%#{sp}#{hs}#{pl}#{mi}#{zr}#{width}#{precision}#{type}"
      FLOAT_VALUES.each {|v|
        r = sprintf format, v
        e = emu_float format, v
        if true
          assert_equal(e, r, "sprintf(#{format.dump}, #{'%.20g' % v})")
        else
          if e != r
            puts "#{e.dump}\t#{r.dump}\tsprintf(#{format.dump}, #{'%.20g' % v})"
          end
        end
      }
    }
  end
end
