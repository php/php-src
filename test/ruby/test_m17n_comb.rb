require 'test/unit'
require_relative 'allpairs'

class TestM17NComb < Test::Unit::TestCase
  def assert_encoding(encname, actual, message=nil)
    assert_equal(Encoding.find(encname), actual, message)
  end

  module AESU
    def a(str) str.dup.force_encoding("ASCII-8BIT") end
    def e(str) str.dup.force_encoding("EUC-JP") end
    def s(str) str.dup.force_encoding("Shift_JIS") end
    def u(str) str.dup.force_encoding("UTF-8") end
  end
  include AESU
  extend AESU

  def assert_strenc(bytes, enc, actual, message=nil)
    assert_instance_of(String, actual, message)
    enc = Encoding.find(enc) if String === enc
    assert_equal(enc, actual.encoding, message)
    assert_equal(a(bytes), a(actual), message)
  end

  STRINGS = [
    a(""), e(""), s(""), u(""),
    a("a"), e("a"), s("a"), u("a"),
    a("."), e("."), s("."), u("."),

    # single character
    a("\x80"), a("\xff"),
    e("\xa1\xa1"), e("\xfe\xfe"),
    e("\x8e\xa1"), e("\x8e\xfe"),
    e("\x8f\xa1\xa1"), e("\x8f\xfe\xfe"),
    s("\x81\x40"), s("\xfc\xfc"),
    s("\xa1"), s("\xdf"),
    u("\xc2\x80"), u("\xf4\x8f\xbf\xbf"),

    # same byte sequence
    a("\xc2\xa1"), e("\xc2\xa1"), s("\xc2\xa1"), u("\xc2\xa1"),

    s("\x81A"), # mutibyte character which contains "A"
    s("\x81a"), # mutibyte character which contains "a"

    # invalid
    e("\xa1"), e("\x80"),
    s("\x81"), s("\x80"),
    u("\xc2"), u("\x80"),

    # for transitivity test
    u("\xe0\xa0\xa1"), e("\xe0\xa0\xa1"), s("\xe0\xa0\xa1"), # [ruby-dev:32693]
    e("\xa1\xa1"), a("\xa1\xa1"), s("\xa1\xa1"), # [ruby-dev:36484]

    #"aa".force_encoding("utf-16be"),
    #"aaaa".force_encoding("utf-32be"),
    #"aaa".force_encoding("utf-32be"),
  ]

  def combination(*args, &b)
    AllPairs.each(*args, &b)
    #AllPairs.exhaustive_each(*args, &b)
  end

  def encdump(str)
    d = str.dump
    if /\.force_encoding\("[A-Za-z0-9.:_+-]*"\)\z/ =~ d
      d
    else
      "#{d}.force_encoding(#{str.encoding.name.dump})"
    end
  end

  def encdumpargs(args)
    r = '('
    args.each_with_index {|a, i|
      r << ',' if 0 < i
      if String === a
        r << encdump(a)
      else
        r << a.inspect
      end
    }
    r << ')'
    r
  end

  def enccall(recv, meth, *args, &block)
    desc = ''
    if String === recv
      desc << encdump(recv)
    else
      desc << recv.inspect
    end
    desc << '.' << meth.to_s
    if !args.empty?
      desc << '('
      args.each_with_index {|a, i|
        desc << ',' if 0 < i
        if String === a
          desc << encdump(a)
        else
          desc << a.inspect
        end
      }
      desc << ')'
    end
    if block
      desc << ' {}'
    end
    result = nil
    assert_nothing_raised(desc) {
      result = recv.send(meth, *args, &block)
    }
    result
  end

  def assert_str_enc_propagation(t, s1, s2)
    if !s1.ascii_only?
      assert_equal(s1.encoding, t.encoding)
    elsif !s2.ascii_only?
      assert_equal(s2.encoding, t.encoding)
    else
      assert([s1.encoding, s2.encoding].include?(t.encoding))
    end
  end

  def assert_same_result(expected_proc, actual_proc)
    e = nil
    begin
      t = expected_proc.call
    rescue
      e = $!
    end
    if e
      assert_raise(e.class) { actual_proc.call }
    else
      assert_equal(t, actual_proc.call)
    end
  end

  def each_slice_call
    combination(STRINGS, -2..2) {|s, nth|
      yield s, nth
    }
    combination(STRINGS, -2..2, 0..2) {|s, nth, len|
      yield s, nth, len
    }
    combination(STRINGS, STRINGS) {|s, substr|
      yield s, substr
    }
    combination(STRINGS, -2..2, 0..2) {|s, first, last|
      yield s, first..last
      yield s, first...last
    }
    combination(STRINGS, STRINGS) {|s1, s2|
      if !s2.valid_encoding?
        next
      end
      yield s1, Regexp.new(Regexp.escape(s2))
    }
    combination(STRINGS, STRINGS, 0..2) {|s1, s2, nth|
      if !s2.valid_encoding?
        next
      end
      yield s1, Regexp.new(Regexp.escape(s2)), nth
    }
  end

  ASCII_INCOMPATIBLE_ENCODINGS = %w[
    UTF-16BE
    UTF-16LE
    UTF-32BE
    UTF-32LE
  ]
  def str_enc_compatible?(*strs)
    encs = []
    ascii_incompatible_encodings = {}
    has_ascii_compatible = false
    strs.each {|s|
      encs << s.encoding if !s.ascii_only?
      if /\A#{Regexp.union ASCII_INCOMPATIBLE_ENCODINGS}\z/o =~ s.encoding.name
        ascii_incompatible_encodings[s.encoding] = true
      else
        has_ascii_compatible = true
      end
    }
    if ascii_incompatible_encodings.empty?
      encs.uniq!
      encs.length <= 1
    else
      !has_ascii_compatible && ascii_incompatible_encodings.size == 1
    end
  end

  # tests start

  def test_str_new
    STRINGS.each {|s|
      t = String.new(s)
      assert_strenc(a(s), s.encoding, t)
    }
  end

  def test_str_plus
    combination(STRINGS, STRINGS) {|s1, s2|
      if s1.encoding != s2.encoding && !s1.ascii_only? && !s2.ascii_only?
        assert_raise(Encoding::CompatibilityError) { s1 + s2 }
      else
        t = enccall(s1, :+, s2)
        assert(t.valid_encoding?) if s1.valid_encoding? && s2.valid_encoding?
        assert_equal(a(s1) + a(s2), a(t))
        assert_str_enc_propagation(t, s1, s2)
      end
    }
  end

  def test_str_times
    STRINGS.each {|s|
      [0,1,2].each {|n|
        t = s * n
        assert(t.valid_encoding?) if s.valid_encoding?
        assert_strenc(a(s) * n, s.encoding, t)
      }
    }
  end

  def test_sprintf_s
    STRINGS.each {|s|
      assert_strenc(a(s), s.encoding, "%s".force_encoding(s.encoding) % s)
      if !s.empty? # xxx
        t = enccall(a("%s"), :%, s)
        assert_strenc(a(s), (a('')+s).encoding, t)
      end
    }
  end

  def test_str_eq_reflexive
    STRINGS.each {|s|
      assert(s == s, "#{encdump s} == #{encdump s}")
    }
  end

  def test_str_eq_symmetric
    combination(STRINGS, STRINGS) {|s1, s2|
      if s1 == s2
        assert(s2 == s1, "#{encdump s2} == #{encdump s1}")
      else
        assert(!(s2 == s1), "!(#{encdump s2} == #{encdump s1})")
      end
    }
  end

  def test_str_eq_transitive
    combination(STRINGS, STRINGS, STRINGS) {|s1, s2, s3|
      if s1 == s2 && s2 == s3
        assert(s1 == s3, "transitive: #{encdump s1} == #{encdump s2} == #{encdump s3}")
      end
    }
  end

  def test_str_eq
    combination(STRINGS, STRINGS) {|s1, s2|
      desc_eq = "#{encdump s1} == #{encdump s2}"
      if a(s1) == a(s2) and
          (s1.ascii_only? && s2.ascii_only? or
           s1.encoding == s2.encoding) then
        assert(s1 == s2, desc_eq)
        assert(!(s1 != s2))
        assert_equal(0, s1 <=> s2)
        assert(s1.eql?(s2), desc_eq)
      else
        assert(!(s1 == s2), "!(#{desc_eq})")
        assert(s1 != s2)
        assert_not_equal(0, s1 <=> s2)
        assert(!s1.eql?(s2))
      end
    }
  end

  def test_str_concat
    combination(STRINGS, STRINGS) {|s1, s2|
      s = s1.dup
      if s1.ascii_only? || s2.ascii_only? || s1.encoding == s2.encoding
        s << s2
        assert(s.valid_encoding?) if s1.valid_encoding? && s2.valid_encoding?
        assert_equal(a(s), a(s1) + a(s2))
        assert_str_enc_propagation(s, s1, s2)
      else
        assert_raise(Encoding::CompatibilityError) { s << s2 }
      end
    }
  end

  def test_str_aref
    STRINGS.each {|s|
      t = ''.force_encoding(s.encoding)
      0.upto(s.length-1) {|i|
        u = s[i]
        assert(u.valid_encoding?) if s.valid_encoding?
        t << u
      }
      assert_equal(t, s)
    }
  end

  def test_str_aref_len
    STRINGS.each {|s|
      t = ''.force_encoding(s.encoding)
      0.upto(s.length-1) {|i|
        u = s[i,1]
        assert(u.valid_encoding?) if s.valid_encoding?
        t << u
      }
      assert_equal(t, s)
    }

    STRINGS.each {|s|
      t = ''.force_encoding(s.encoding)
      0.step(s.length-1, 2) {|i|
        u = s[i,2]
        assert(u.valid_encoding?) if s.valid_encoding?
        t << u
      }
      assert_equal(t, s)
    }
  end

  def test_str_aref_substr
    combination(STRINGS, STRINGS) {|s1, s2|
      if s1.ascii_only? || s2.ascii_only? || s1.encoding == s2.encoding
        t = enccall(s1, :[], s2)
        if t != nil
          assert(t.valid_encoding?) if s1.valid_encoding? && s2.valid_encoding?
          assert_equal(s2, t)
          assert_match(/#{Regexp.escape(a(s2))}/, a(s1))
          if s1.valid_encoding?
            assert_match(/#{Regexp.escape(s2)}/, s1)
          end
        end
      else
        assert_raise(Encoding::CompatibilityError) { s1[s2] }
      end
    }
  end

  def test_str_aref_range2
    combination(STRINGS, -2..2, -2..2) {|s, first, last|
      desc = "#{encdump s}[#{first}..#{last}]"
      t = s[first..last]
      if first < 0
        first += s.length
        if first < 0
          assert_nil(t, desc)
          next
        end
      end
      if s.length < first
        assert_nil(t, desc)
        next
      end
      assert(t.valid_encoding?) if s.valid_encoding?
      if last < 0
        last += s.length
      end
      t2 = ''
      first.upto(last) {|i|
        c = s[i]
        t2 << c if c
      }
      assert_equal(t2, t, desc)
    }
  end

  def test_str_aref_range3
    combination(STRINGS, -2..2, -2..2) {|s, first, last|
      desc = "#{encdump s}[#{first}..#{last}]"
      t = s[first...last]
      if first < 0
        first += s.length
        if first < 0
          assert_nil(t, desc)
          next
        end
      end
      if s.length < first
        assert_nil(t, desc)
        next
      end
      if last < 0
        last += s.length
      end
      assert(t.valid_encoding?) if s.valid_encoding?
      t2 = ''
      first.upto(last-1) {|i|
        c = s[i]
        t2 << c if c
      }
      assert_equal(t2, t, desc)
    }
  end

  def test_str_assign
    combination(STRINGS, STRINGS) {|s1, s2|
      (-2).upto(2) {|i|
        t = s1.dup
        if s1.ascii_only? || s2.ascii_only? || s1.encoding == s2.encoding
          if i < -s1.length || s1.length < i
            assert_raise(IndexError) { t[i] = s2 }
          else
            t[i] = s2
            assert(t.valid_encoding?) if s1.valid_encoding? && s2.valid_encoding?
            assert(a(t).index(a(s2)))
            if s1.valid_encoding? && s2.valid_encoding?
              if i == s1.length && s2.empty?
                assert_nil(t[i])
              elsif i < 0
                assert_equal(s2, t[i-s2.length+1,s2.length],
                  "t = #{encdump(s1)}; t[#{i}] = #{encdump(s2)}; t[#{i-s2.length+1},#{s2.length}]")
              else
                assert_equal(s2, t[i,s2.length],
                  "t = #{encdump(s1)}; t[#{i}] = #{encdump(s2)}; t[#{i},#{s2.length}]")
              end
            end
          end
        else
          assert_raise(Encoding::CompatibilityError) { t[i] = s2 }
        end
      }
    }
  end

  def test_str_assign_len
    combination(STRINGS, -2..2, 0..2, STRINGS) {|s1, i, len, s2|
      t = s1.dup
      if s1.ascii_only? || s2.ascii_only? || s1.encoding == s2.encoding
        if i < -s1.length || s1.length < i
          assert_raise(IndexError) { t[i,len] = s2 }
        else
          assert(t.valid_encoding?) if s1.valid_encoding? && s2.valid_encoding?
          t[i,len] = s2
          assert(a(t).index(a(s2)))
          if s1.valid_encoding? && s2.valid_encoding?
            if i == s1.length && s2.empty?
              assert_nil(t[i])
            elsif i < 0
              if -i < len
                len = -i
              end
              assert_equal(s2, t[i-s2.length+len,s2.length],
                "t = #{encdump(s1)}; t[#{i},#{len}] = #{encdump(s2)}; t[#{i-s2.length+len},#{s2.length}]")
            else
              assert_equal(s2, t[i,s2.length],
                "t = #{encdump(s1)}; t[#{i},#{len}] = #{encdump(s2)}; t[#{i},#{s2.length}]")
            end
          end
        end
      else
        assert_raise(Encoding::CompatibilityError) { t[i,len] = s2 }
      end
    }
  end

  def test_str_assign_substr
    combination(STRINGS, STRINGS, STRINGS) {|s1, s2, s3|
      t = s1.dup
      encs = [
        !s1.ascii_only? ? s1.encoding : nil,
        !s2.ascii_only? ? s2.encoding : nil,
        !s3.ascii_only? ? s3.encoding : nil].uniq.compact
      if 1 < encs.length
        assert_raise(Encoding::CompatibilityError, IndexError) { t[s2] = s3 }
      else
        if encs.empty?
          encs = [
            s1.encoding,
            s2.encoding,
            s3.encoding].uniq.reject {|e| e == Encoding.find("ASCII-8BIT") }
          if encs.empty?
            encs = [Encoding.find("ASCII-8BIT")]
          end
        end
        if !t[s2]
        else
          enccall(t, :[]=, s2, s3)
          assert(t.valid_encoding?) if s1.valid_encoding? && s2.valid_encoding? && s3.valid_encoding?
        end
      end
    }
  end

  def test_str_assign_range2
    combination(STRINGS, -2..2, -2..2, STRINGS) {|s1, first, last, s2|
      t = s1.dup
      if s1.ascii_only? || s2.ascii_only? || s1.encoding == s2.encoding
        if first < -s1.length || s1.length < first
          assert_raise(RangeError) { t[first..last] = s2 }
        else
          enccall(t, :[]=, first..last, s2)
          assert(t.valid_encoding?) if s1.valid_encoding? && s2.valid_encoding?
          assert(a(t).index(a(s2)))
          if s1.valid_encoding? && s2.valid_encoding?
            if first < 0
              assert_equal(s2, t[s1.length+first, s2.length])
            else
              assert_equal(s2, t[first, s2.length])
            end
          end
        end
      else
        assert_raise(Encoding::CompatibilityError, RangeError,
                     "t=#{encdump(s1)};t[#{first}..#{last}]=#{encdump(s2)}") {
          t[first..last] = s2
        }
      end
    }
  end

  def test_str_assign_range3
    combination(STRINGS, -2..2, -2..2, STRINGS) {|s1, first, last, s2|
      t = s1.dup
      if s1.ascii_only? || s2.ascii_only? || s1.encoding == s2.encoding
        if first < -s1.length || s1.length < first
          assert_raise(RangeError) { t[first...last] = s2 }
        else
          enccall(t, :[]=, first...last, s2)
          assert(t.valid_encoding?) if s1.valid_encoding? && s2.valid_encoding?
          assert(a(t).index(a(s2)))
          if s1.valid_encoding? && s2.valid_encoding?
            if first < 0
              assert_equal(s2, t[s1.length+first, s2.length])
            else
              assert_equal(s2, t[first, s2.length])
            end
          end
        end
      else
        assert_raise(Encoding::CompatibilityError, RangeError,
                     "t=#{encdump(s1)};t[#{first}...#{last}]=#{encdump(s2)}") {
          t[first...last] = s2
        }
      end
    }
  end

  def test_str_cmp
    combination(STRINGS, STRINGS) {|s1, s2|
      desc = "#{encdump s1} <=> #{encdump s2}"
      r = s1 <=> s2
      if s1 == s2
        assert_equal(0, r, desc)
      else
        assert_not_equal(0, r, desc)
      end
    }
  end

  def test_str_capitalize
    STRINGS.each {|s|
      begin
        t1 = s.capitalize
      rescue ArgumentError
        assert(!s.valid_encoding?)
        next
      end
      assert(t1.valid_encoding?) if s.valid_encoding?
      assert(t1.casecmp(s))
      t2 = s.dup
      t2.capitalize!
      assert_equal(t1, t2)
      r = s.downcase
      r = enccall(r, :sub, /\A[a-z]/) {|ch| a(ch).upcase }
      assert_equal(r, t1)
    }
  end

  def test_str_casecmp
    combination(STRINGS, STRINGS) {|s1, s2|
      #puts "#{encdump(s1)}.casecmp(#{encdump(s2)})"
      next unless s1.valid_encoding? && s2.valid_encoding? && Encoding.compatible?(s1, s2)
      r = s1.casecmp(s2)
      assert_equal(s1.upcase <=> s2.upcase, r)
    }
  end

  def test_str_center
    combination(STRINGS, [0,1,2,3,10]) {|s1, width|
      t = s1.center(width)
      assert(a(t).index(a(s1)))
    }
    combination(STRINGS, [0,1,2,3,10], STRINGS) {|s1, width, s2|
      if s2.empty?
        assert_raise(ArgumentError) { s1.center(width, s2) }
        next
      end
      if !s1.ascii_only? && !s2.ascii_only? && s1.encoding != s2.encoding
        assert_raise(Encoding::CompatibilityError) { s1.center(width, s2) }
        next
      end
      t = enccall(s1, :center, width, s2)
      assert(t.valid_encoding?) if s1.valid_encoding? && s2.valid_encoding?
      assert(a(t).index(a(s1)))
      assert_str_enc_propagation(t, s1, s2) if (t != s1)
    }
  end

  def test_str_ljust
    combination(STRINGS, [0,1,2,3,10]) {|s1, width|
      t = s1.ljust(width)
      assert(a(t).index(a(s1)))
    }
    combination(STRINGS, [0,1,2,3,10], STRINGS) {|s1, width, s2|
      if s2.empty?
        assert_raise(ArgumentError) { s1.ljust(width, s2) }
        next
      end
      if !s1.ascii_only? && !s2.ascii_only? && s1.encoding != s2.encoding
        assert_raise(Encoding::CompatibilityError) { s1.ljust(width, s2) }
        next
      end
      t = enccall(s1, :ljust, width, s2)
      assert(t.valid_encoding?) if s1.valid_encoding? && s2.valid_encoding?
      assert(a(t).index(a(s1)))
      assert_str_enc_propagation(t, s1, s2) if (t != s1)
    }
  end

  def test_str_rjust
    combination(STRINGS, [0,1,2,3,10]) {|s1, width|
      t = s1.rjust(width)
      assert(a(t).index(a(s1)))
    }
    combination(STRINGS, [0,1,2,3,10], STRINGS) {|s1, width, s2|
      if s2.empty?
        assert_raise(ArgumentError) { s1.rjust(width, s2) }
        next
      end
      if !s1.ascii_only? && !s2.ascii_only? && s1.encoding != s2.encoding
        assert_raise(Encoding::CompatibilityError) { s1.rjust(width, s2) }
        next
      end
      t = enccall(s1, :rjust, width, s2)
      assert(t.valid_encoding?) if s1.valid_encoding? && s2.valid_encoding?
      assert(a(t).index(a(s1)))
      assert_str_enc_propagation(t, s1, s2) if (t != s1)
    }
  end

  def test_str_chomp
    combination(STRINGS, STRINGS) {|s1, s2|
      if !s1.ascii_only? && !s2.ascii_only? && !Encoding.compatible?(s1,s2)
        if s1.bytesize > s2.bytesize
          assert_raise(Encoding::CompatibilityError) { s1.chomp(s2) }
        end
        next
      end
      t = enccall(s1, :chomp, s2)
      assert(t.valid_encoding?, "#{encdump(s1)}.chomp(#{encdump(s2)})") if s1.valid_encoding? && s2.valid_encoding?
      assert_equal(s1.encoding, t.encoding)
      t2 = s1.dup
      t2.chomp!(s2)
      assert_equal(t, t2)
    }
  end

  def test_str_chop
    STRINGS.each {|s|
      s = s.dup
      desc = "#{encdump s}.chop"
      t = nil
      assert_nothing_raised(desc) { t = s.chop }
      assert(t.valid_encoding?) if s.valid_encoding?
      assert(a(s).index(a(t)))
      t2 = s.dup
      t2.chop!
      assert_equal(t, t2)
    }
  end

  def test_str_clear
    STRINGS.each {|s|
      t = s.dup
      t.clear
      assert(t.valid_encoding?)
      assert(t.empty?)
    }
  end

  def test_str_clone
    STRINGS.each {|s|
      t = s.clone
      assert_equal(s, t)
      assert_equal(s.encoding, t.encoding)
      assert_equal(a(s), a(t))
    }
  end

  def test_str_dup
    STRINGS.each {|s|
      t = s.dup
      assert_equal(s, t)
      assert_equal(s.encoding, t.encoding)
      assert_equal(a(s), a(t))
    }
  end

  def test_str_count
    combination(STRINGS, STRINGS) {|s1, s2|
      if !s1.valid_encoding? || !s2.valid_encoding?
        assert_raise(ArgumentError, Encoding::CompatibilityError) { s1.count(s2) }
        next
      end
      if !s1.ascii_only? && !s2.ascii_only? && s1.encoding != s2.encoding
        assert_raise(Encoding::CompatibilityError) { s1.count(s2) }
        next
      end
      n = enccall(s1, :count, s2)
      n0 = a(s1).count(a(s2))
      assert_operator(n, :<=, n0)
    }
  end

  def test_str_crypt
    combination(STRINGS, STRINGS) {|str, salt|
      if a(salt).length < 2
        assert_raise(ArgumentError) { str.crypt(salt) }
        next
      end
      t = str.crypt(salt)
      assert_equal(a(str).crypt(a(salt)), t, "#{encdump(str)}.crypt(#{encdump(salt)})")
      assert_encoding('ASCII-8BIT', t.encoding)
    }
  end

  def test_str_delete
    combination(STRINGS, STRINGS) {|s1, s2|
      if s1.empty?
        assert_equal(s1, s1.delete(s2))
        next
      end
      if !s1.valid_encoding? || !s2.valid_encoding?
        assert_raise(ArgumentError, Encoding::CompatibilityError) { s1.delete(s2) }
        next
      end
      if !s1.ascii_only? && !s2.ascii_only? && s1.encoding != s2.encoding
        assert_raise(Encoding::CompatibilityError) { s1.delete(s2) }
        next
      end
      t = enccall(s1, :delete, s2)
      assert(t.valid_encoding?)
      assert_equal(t.encoding, s1.encoding)
      assert_operator(t.length, :<=, s1.length)
      t2 = s1.dup
      t2.delete!(s2)
      assert_equal(t, t2)
    }
  end

  def test_str_downcase
    STRINGS.each {|s|
      if !s.valid_encoding?
        assert_raise(ArgumentError) { s.downcase }
        next
      end
      t = s.downcase
      assert(t.valid_encoding?)
      assert_equal(t.encoding, s.encoding)
      assert(t.casecmp(s))
      t2 = s.dup
      t2.downcase!
      assert_equal(t, t2)
    }
  end

  def test_str_dump
    STRINGS.each {|s|
      t = s.dump
      assert(t.valid_encoding?)
      assert(t.ascii_only?)
      u = eval(t)
      assert_equal(a(s), a(u))
    }
  end

  def test_str_each_line
    combination(STRINGS, STRINGS) {|s1, s2|
      if !s1.valid_encoding? || !s2.valid_encoding?
        assert_raise(ArgumentError, Encoding::CompatibilityError) { s1.each_line(s2) {} }
        next
      end
      if !s1.ascii_only? && !s2.ascii_only? && s1.encoding != s2.encoding
        assert_raise(Encoding::CompatibilityError) { s1.each_line(s2) {} }
        next
      end
      lines = []
      enccall(s1, :each_line, s2) {|line|
        assert(line.valid_encoding?)
        assert_equal(s1.encoding, line.encoding)
        lines << line
      }
      next if lines.size == 0
      s2 = lines.join('')
      assert_equal(s1.encoding, s2.encoding)
      assert_equal(s1, s2)
    }
  end

  def test_str_each_byte
    STRINGS.each {|s|
      bytes = []
      s.each_byte {|b|
        bytes << b
      }
      a(s).split(//).each_with_index {|ch, i|
        assert_equal(ch.ord, bytes[i])
      }
    }
  end

  def test_str_empty?
    STRINGS.each {|s|
      if s.length == 0
        assert(s.empty?)
      else
        assert(!s.empty?)
      end
    }
  end

  def test_str_hex
    STRINGS.each {|s|
      t = s.hex
      t2 = a(s)[/\A[0-9a-fA-Fx]*/].hex
      assert_equal(t2, t)
    }
  end

  def test_str_include?
    combination(STRINGS, STRINGS) {|s1, s2|
      if !s1.ascii_only? && !s2.ascii_only? && s1.encoding != s2.encoding
        assert_raise(Encoding::CompatibilityError) { s1.include?(s2) }
        assert_raise(Encoding::CompatibilityError) { s1.index(s2) }
        assert_raise(Encoding::CompatibilityError) { s1.rindex(s2) }
        next
      end
      t = enccall(s1, :include?, s2)
      if t
        assert(a(s1).include?(a(s2)))
        assert(s1.index(s2))
        assert(s1.rindex(s2))
      else
        assert(!s1.index(s2))
        assert(!s1.rindex(s2), "!#{encdump(s1)}.rindex(#{encdump(s2)})")
      end
      if s2.empty?
        assert_equal(true, t)
        next
      end
      if !s1.valid_encoding? || !s2.valid_encoding?
        assert_equal(false, t, "#{encdump s1}.include?(#{encdump s2})")
        next
      end
      if t && s1.valid_encoding? && s2.valid_encoding?
        assert_match(/#{Regexp.escape(s2)}/, s1)
      else
        assert_no_match(/#{Regexp.escape(s2)}/, s1)
      end
    }
  end

  def test_str_index
    combination(STRINGS, STRINGS, -2..2) {|s1, s2, pos|
      if !s1.ascii_only? && !s2.ascii_only? && s1.encoding != s2.encoding
        assert_raise(Encoding::CompatibilityError) { s1.index(s2) }
        next
      end
      t = enccall(s1, :index, s2, pos)
      if s2.empty?
        if pos < 0 && pos+s1.length < 0
          assert_equal(nil, t, "#{encdump s1}.index(#{encdump s2}, #{pos})");
        elsif pos < 0
          assert_equal(s1.length+pos, t, "#{encdump s1}.index(#{encdump s2}, #{pos})");
        elsif s1.length < pos
          assert_equal(nil, t, "#{encdump s1}.index(#{encdump s2}, #{pos})");
        else
          assert_equal(pos, t, "#{encdump s1}.index(#{encdump s2}, #{pos})");
        end
        next
      end
      if !s1.valid_encoding? || !s2.valid_encoding?
        assert_equal(nil, t, "#{encdump s1}.index(#{encdump s2}, #{pos})");
        next
      end
      if t
        re = /#{Regexp.escape(s2)}/
        assert(re.match(s1, pos))
        assert_equal($`.length, t, "#{encdump s1}.index(#{encdump s2}, #{pos})")
      else
        assert_no_match(/#{Regexp.escape(s2)}/, s1[pos..-1])
      end
    }
  end

  def test_str_rindex
    combination(STRINGS, STRINGS, -2..2) {|s1, s2, pos|
      if !s1.ascii_only? && !s2.ascii_only? && s1.encoding != s2.encoding
        assert_raise(Encoding::CompatibilityError) { s1.rindex(s2) }
        next
      end
      t = enccall(s1, :rindex, s2, pos)
      if s2.empty?
        if pos < 0 && pos+s1.length < 0
          assert_equal(nil, t, "#{encdump s1}.rindex(#{encdump s2}, #{pos})")
        elsif pos < 0
          assert_equal(s1.length+pos, t, "#{encdump s1}.rindex(#{encdump s2}, #{pos})")
        elsif s1.length < pos
          assert_equal(s1.length, t, "#{encdump s1}.rindex(#{encdump s2}, #{pos})")
        else
          assert_equal(pos, t, "#{encdump s1}.rindex(#{encdump s2}, #{pos})")
        end
        next
      end
      if !s1.valid_encoding? || !s2.valid_encoding?
        assert_equal(nil, t, "#{encdump s1}.rindex(#{encdump s2}, #{pos})")
        next
      end
      if t
        #puts "#{encdump s1}.rindex(#{encdump s2}, #{pos}) => #{t}"
        assert(a(s1).index(a(s2)))
        pos2 = pos
        pos2 += s1.length if pos < 0
        re = /\A(.{0,#{pos2}})#{Regexp.escape(s2)}/m
        m = enccall(re, :match, s1)
        assert(m, "#{re.inspect}.match(#{encdump(s1)})")
        assert_equal(m[1].length, t, "#{encdump s1}.rindex(#{encdump s2}, #{pos})")
      else
        re = /#{Regexp.escape(s2)}/
        n = re =~ s1
        if n
          if pos < 0
            assert_operator(n, :>, s1.length+pos)
          else
            assert_operator(n, :>, pos)
          end
        end
      end
    }
  end

  def test_str_insert
    combination(STRINGS, 0..2, STRINGS) {|s1, nth, s2|
      t1 = s1.dup
      t2 = s1.dup
      begin
        t1[nth, 0] = s2
      rescue Encoding::CompatibilityError, IndexError => e1
      end
      begin
        t2.insert(nth, s2)
      rescue Encoding::CompatibilityError, IndexError => e2
      end
      assert_equal(t1, t2, "t=#{encdump s1}; t.insert(#{nth},#{encdump s2}); t")
      assert_equal(e1.class, e2.class, "begin #{encdump s1}.insert(#{nth},#{encdump s2}); rescue ArgumentError, IndexError => e; e end")
    }
    combination(STRINGS, -2..-1, STRINGS) {|s1, nth, s2|
      next if s1.length + nth < 0
      next unless s1.valid_encoding?
      next unless s2.valid_encoding?
      t1 = s1.dup
      begin
        t1.insert(nth, s2)
        slen = s2.length
        assert_equal(t1[nth-slen+1,slen], s2, "t=#{encdump s1}; t.insert(#{nth},#{encdump s2}); t")
      rescue Encoding::CompatibilityError, IndexError
      end
    }
  end

  def test_str_intern
    STRINGS.each {|s|
      if /\0/ =~ a(s)
        assert_raise(ArgumentError) { s.intern }
      elsif s.valid_encoding?
        sym = s.intern
        assert_equal(s, sym.to_s, "#{encdump s}.intern.to_s")
        assert_equal(sym, s.to_sym)
      else
        assert_raise(EncodingError) { s.intern }
      end
    }
  end

  def test_str_length
    STRINGS.each {|s|
      assert_operator(s.length, :<=, s.bytesize)
    }
  end

  def test_str_oct
    STRINGS.each {|s|
      t = s.oct
      t2 = a(s)[/\A[0-9a-fA-FxX]*/].oct
      assert_equal(t2, t)
    }
  end

  def test_str_replace
    combination(STRINGS, STRINGS) {|s1, s2|
      t = s1.dup
      t.replace s2
      assert_equal(s2, t)
      assert_equal(s2.encoding, t.encoding)
    }
  end

  def test_str_reverse
    STRINGS.each {|s|
      t = s.reverse
      assert_equal(s.bytesize, t.bytesize)
      if !s.valid_encoding?
        assert_operator(t.length, :<=, s.length)
        next
      end
      assert_equal(s, t.reverse)
    }
  end

  def test_str_scan
    combination(STRINGS, STRINGS) {|s1, s2|
      if !s2.valid_encoding?
        assert_raise(RegexpError) { s1.scan(s2) }
        next
      end
      if !s1.ascii_only? && !s2.ascii_only? && s1.encoding != s2.encoding
        if s1.valid_encoding?
          assert_raise(Encoding::CompatibilityError) { s1.scan(s2) }
        else
          assert_match(/invalid byte sequence/, assert_raise(ArgumentError) { s1.scan(s2) }.message)
        end
        next
      end
      if !s1.valid_encoding?
        assert_raise(ArgumentError) { s1.scan(s2) }
        next
      end
      r = enccall(s1, :scan, s2)
      r.each {|t|
        assert_equal(s2, t)
      }
    }
  end

  def test_str_slice
    each_slice_call {|obj, *args|
      assert_same_result(lambda { obj[*args] }, lambda { obj.slice(*args) })
    }
  end

  def test_str_slice!
    each_slice_call {|s, *args|
      desc_slice = "#{encdump s}.slice#{encdumpargs args}"
      desc_slice_bang = "#{encdump s}.slice!#{encdumpargs args}"
      t = s.dup
      begin
        r = t.slice!(*args)
      rescue
        e = $!
      end
      if e
        assert_raise(e.class, desc_slice) { s.slice(*args) }
        next
      end
      if !r
        assert_nil(s.slice(*args), desc_slice)
        next
      end
      assert_equal(s.slice(*args), r, desc_slice_bang)
      assert_equal(s.bytesize, r.bytesize + t.bytesize)
      if args.length == 1 && String === args[0]
        assert_equal(args[0].encoding, r.encoding,
                    "#{encdump s}.slice!#{encdumpargs args}.encoding")
      else
        assert_equal(s.encoding, r.encoding,
                    "#{encdump s}.slice!#{encdumpargs args}.encoding")
      end
      if [s, *args].all? {|o| !(String === o) || o.valid_encoding? }
        assert(r.valid_encoding?)
        assert(t.valid_encoding?)
        assert_equal(s.length, r.length + t.length)
      end
    }
  end

  def test_str_split
    combination(STRINGS, STRINGS) {|s1, s2|
      if !s2.valid_encoding?
        assert_raise(ArgumentError, RegexpError) { s1.split(s2) }
        next
      end
      if !s1.ascii_only? && !s2.ascii_only? && s1.encoding != s2.encoding
        assert_raise(ArgumentError, Encoding::CompatibilityError) { s1.split(s2) }
        next
      end
      if !s1.valid_encoding?
        assert_raise(ArgumentError) { s1.split(s2) }
        next
      end
      t = enccall(s1, :split, s2)
      t.each {|r|
        assert(a(s1).include?(a(r)))
        assert_equal(s1.encoding, r.encoding)
      }
      assert(a(s1).include?(t.map {|u| a(u) }.join(a(s2))))
      if s1.valid_encoding? && s2.valid_encoding?
        t.each {|r|
          assert(r.valid_encoding?)
        }
      end
    }
  end

  def test_str_squeeze
    combination(STRINGS, STRINGS) {|s1, s2|
      if !s1.valid_encoding? || !s2.valid_encoding?
        assert_raise(ArgumentError, Encoding::CompatibilityError, "#{encdump s1}.squeeze(#{encdump s2})") { s1.squeeze(s2) }
        next
      end
      if !s1.ascii_only? && !s2.ascii_only? && s1.encoding != s2.encoding
        assert_raise(Encoding::CompatibilityError) { s1.squeeze(s2) }
        next
      end
      t = enccall(s1, :squeeze, s2)
      assert_operator(t.length, :<=, s1.length)
      t2 = s1.dup
      t2.squeeze!(s2)
      assert_equal(t, t2)
    }
  end

  def test_str_strip
    STRINGS.each {|s|
      if !s.valid_encoding?
        assert_raise(ArgumentError, "#{encdump s}.strip") { s.strip }
        next
      end
      t = s.strip
      l = s.lstrip
      r = s.rstrip
      assert_operator(l.length, :<=, s.length)
      assert_operator(r.length, :<=, s.length)
      assert_operator(t.length, :<=, l.length)
      assert_operator(t.length, :<=, r.length)
      t2 = s.dup
      t2.strip!
      assert_equal(t, t2)
      l2 = s.dup
      l2.lstrip!
      assert_equal(l, l2)
      r2 = s.dup
      r2.rstrip!
      assert_equal(r, r2)
    }
  end

  def test_str_sum
    STRINGS.each {|s|
      assert_equal(a(s).sum, s.sum)
    }
  end

  def test_str_swapcase
    STRINGS.each {|s|
      if !s.valid_encoding?
        assert_raise(ArgumentError, "#{encdump s}.swapcase") { s.swapcase }
        next
      end
      t1 = s.swapcase
      assert(t1.valid_encoding?) if s.valid_encoding?
      assert(t1.casecmp(s))
      t2 = s.dup
      t2.swapcase!
      assert_equal(t1, t2)
      t3 = t1.swapcase
      assert_equal(s, t3);
    }
  end


  def test_str_to_f
    STRINGS.each {|s|
      assert_nothing_raised { s.to_f }
    }
  end

  def test_str_to_i
    STRINGS.each {|s|
      assert_nothing_raised { s.to_i }
      2.upto(36) {|radix|
        assert_nothing_raised { s.to_i(radix) }
      }
    }
  end

  def test_str_to_s
    STRINGS.each {|s|
      assert_same(s, s.to_s)
      assert_same(s, s.to_str)
    }
  end

  def test_tr
    combination(STRINGS, STRINGS, STRINGS) {|s1, s2, s3|
      desc = "#{encdump s1}.tr(#{encdump s2}, #{encdump s3})"
      if s1.empty?
        assert_equal(s1, s1.tr(s2, s3), desc)
        next
      end
      if !str_enc_compatible?(s1, s2, s3)
        assert_raise(Encoding::CompatibilityError, desc) { s1.tr(s2, s3) }
        next
      end
      if !s1.valid_encoding?
        assert_raise(ArgumentError, desc) { s1.tr(s2, s3) }
        next
      end
      if s2.empty?
        t = enccall(s1, :tr, s2, s3)
        assert_equal(s1, t, desc)
        next
      end
      if !s2.valid_encoding? || !s3.valid_encoding?
        assert_raise(ArgumentError, desc) { s1.tr(s2, s3) }
        next
      end
      t = enccall(s1, :tr, s2, s3)
      assert_operator(s1.length, :>=, t.length, desc)
    }
  end

  def test_tr_s
    combination(STRINGS, STRINGS, STRINGS) {|s1, s2, s3|
      desc = "#{encdump s1}.tr_s(#{encdump s2}, #{encdump s3})"
      if s1.empty?
        assert_equal(s1, s1.tr_s(s2, s3), desc)
        next
      end
      if !s1.valid_encoding?
        assert_raise(ArgumentError, Encoding::CompatibilityError, desc) { s1.tr_s(s2, s3) }
        next
      end
      if !str_enc_compatible?(s1, s2, s3)
        assert_raise(Encoding::CompatibilityError, desc) { s1.tr(s2, s3) }
        next
      end
      if s2.empty?
        t = enccall(s1, :tr_s, s2, s3)
        assert_equal(s1, t, desc)
        next
      end
      if !s2.valid_encoding? || !s3.valid_encoding?
        assert_raise(ArgumentError, desc) { s1.tr_s(s2, s3) }
        next
      end

      t = enccall(s1, :tr_s, s2, s3)
      assert_operator(s1.length, :>=, t.length, desc)
    }
  end

  def test_str_upcase
    STRINGS.each {|s|
      desc = "#{encdump s}.upcase"
      if !s.valid_encoding?
        assert_raise(ArgumentError, desc) { s.upcase }
        next
      end
      t1 = s.upcase
      assert(t1.valid_encoding?)
      assert(t1.casecmp(s))
      t2 = s.dup
      t2.upcase!
      assert_equal(t1, t2)
    }
  end

  def test_str_succ
    STRINGS.each {|s0|
      next if s0.empty?
      s = s0.dup
      n = 300
      h = {}
      n.times {|i|
        if h[s]
          assert(false, "#{encdump s} cycle with succ #{i-h[s]} times")
        end
        h[s] = i
        assert_operator(s.length, :<=, s0.length + Math.log2(i+1) + 1, "#{encdump s0} succ #{i} times => #{encdump s}")
        #puts encdump(s)
        t = s.succ
        if s.valid_encoding?
          assert(t.valid_encoding?, "#{encdump s}.succ.valid_encoding?")
        end
        s = t
      }
    }
  end

  def test_str_hash
    combination(STRINGS, STRINGS) {|s1, s2|
      if s1.eql?(s2)
        assert_equal(s1.hash, s2.hash, "#{encdump s1}.hash == #{encdump s2}.dump")
      end
    }
  end

  def test_marshal
    STRINGS.each {|s|
      m = Marshal.dump(s)
      t = Marshal.load(m)
      assert_equal(s, t)
    }
  end

  def test_str_sub
    combination(STRINGS, STRINGS, STRINGS) {|s1, s2, s3|
      if !s2.valid_encoding?
        assert_raise(RegexpError) { Regexp.new(Regexp.escape(s2)) }
        next
      end
      r2 = Regexp.new(Regexp.escape(s2))
      [
        [
          "#{encdump s1}.sub(Regexp.new(#{encdump s2}), #{encdump s3})",
          lambda { s1.sub(r2, s3) },
          false
        ],
        [
          "#{encdump s1}.sub(Regexp.new(#{encdump s2}), #{encdump s3})",
          lambda { s1.sub(r2) { s3 } },
          false
        ],
        [
          "#{encdump s1}.gsub(Regexp.new(#{encdump s2}), #{encdump s3})",
          lambda { s1.gsub(r2, s3) },
          true
        ],
        [
          "#{encdump s1}.gsub(Regexp.new(#{encdump s2}), #{encdump s3})",
          lambda { s1.gsub(r2) { s3 } },
          true
        ]
      ].each {|desc, doit, g|
        if !s1.valid_encoding?
          assert_raise(ArgumentError, desc) { doit.call }
          next
        end
        if !str_enc_compatible?(s1, s2)
          assert_raise(Encoding::CompatibilityError, desc) { doit.call }
          next
        end
        if !enccall(s1, :include?, s2)
          assert_equal(s1, doit.call)
          next
        end
        if !str_enc_compatible?(g ? s1.gsub(r2, '') : s1.sub(r2, ''), s3)
          assert_raise(Encoding::CompatibilityError, desc) { doit.call }
          next
        end
        t = nil
        assert_nothing_raised(desc) {
          t = doit.call
        }
        if s2 == s3
          assert_equal(s1, t, desc)
        else
          assert_not_equal(s1, t, desc)
        end
      }
    }
  end

  def test_str_sub!
    combination(STRINGS, STRINGS, STRINGS) {|s1, s2, s3|
      if !s2.valid_encoding?
        assert_raise(RegexpError) { Regexp.new(Regexp.escape(s2)) }
        next
      end
      r2 = Regexp.new(Regexp.escape(s2))
      [
        [
          "t=#{encdump s1}.dup;t.sub!(Regexp.new(#{encdump s2}), #{encdump s3})",
          lambda { t=s1.dup; [t, t.sub!(r2, s3)] },
          false
        ],
        [
          "t=#{encdump s1}.dup;t.sub!(Regexp.new(#{encdump s2}), #{encdump s3})",
          lambda { t=s1.dup; [t, t.sub!(r2) { s3 }] },
          false
        ],
        [
          "t=#{encdump s1}.dup;t.gsub!(Regexp.new(#{encdump s2}), #{encdump s3})",
          lambda { t=s1.dup; [t, t.gsub!(r2, s3)] },
          true
        ],
        [
          "t=#{encdump s1}.dup;t.gsub!(Regexp.new(#{encdump s2}), #{encdump s3})",
          lambda { t=s1.dup; [t, t.gsub!(r2) { s3 }] },
          true
        ]
      ].each {|desc, doit, g|
        if !s1.valid_encoding?
          assert_raise(ArgumentError, desc) { doit.call }
          next
        end
        if !str_enc_compatible?(s1, s2)
          assert_raise(Encoding::CompatibilityError, desc) { doit.call }
          next
        end
        if !enccall(s1, :include?, s2)
          assert_equal([s1, nil], doit.call)
          next
        end
        if !str_enc_compatible?(g ? s1.gsub(r2, '') : s1.sub(r2, ''), s3)
          assert_raise(Encoding::CompatibilityError, desc) { doit.call }
          next
        end
        t = ret = nil
        assert_nothing_raised(desc) {
          t, ret = doit.call
        }
        assert(ret)
        if s2 == s3
          assert_equal(s1, t, desc)
        else
          assert_not_equal(s1, t, desc)
        end
      }
    }
  end

  def test_str_bytes
    STRINGS.each {|s1|
      ary = []
      s1.bytes.each {|b|
        ary << b
      }
      assert_equal(s1.unpack("C*"), ary)
    }
  end

  def test_str_bytesize
    STRINGS.each {|s1|
      assert_equal(s1.unpack("C*").length, s1.bytesize)
    }
  end

  def test_str_chars
    STRINGS.each {|s1|
      ary = []
      s1.chars.each {|c|
        ary << c
      }
      expected = []
      s1.length.times {|i|
        expected << s1[i]
      }
      assert_equal(expected, ary)
    }
  end

  def test_str_chr
    STRINGS.each {|s1|
      if s1.empty?
        assert_equal("", s1.chr)
        next
      end
      assert_equal(s1[0], s1.chr)
    }
  end

  def test_str_end_with?
    combination(STRINGS, STRINGS) {|s1, s2|
      desc = "#{encdump s1}.end_with?(#{encdump s2})"
      if !str_enc_compatible?(s1, s2)
        assert_raise(Encoding::CompatibilityError, desc) { s1.end_with?(s2) }
        next
      end
      if s1.length < s2.length
        assert_equal(false, enccall(s1, :end_with?, s2), desc)
        next
      end
      if s1[s1.length-s2.length, s2.length] == s2
        assert_equal(true, enccall(s1, :end_with?, s2), desc)
        next
      end
      assert_equal(false, enccall(s1, :end_with?, s2), desc)
    }
  end

  def test_str_start_with?
    combination(STRINGS, STRINGS) {|s1, s2|
      desc = "#{encdump s1}.start_with?(#{encdump s2})"
      if !str_enc_compatible?(s1, s2)
        assert_raise(Encoding::CompatibilityError, desc) { s1.start_with?(s2) }
        next
      end
      s1 = s1.dup.force_encoding("ASCII-8BIT")
      s2 = s2.dup.force_encoding("ASCII-8BIT")
      if s1.length < s2.length
        assert_equal(false, enccall(s1, :start_with?, s2), desc)
        next
      end
      if s1[0, s2.length] == s2
        assert_equal(true, enccall(s1, :start_with?, s2), desc)
        next
      end
      assert_equal(false, enccall(s1, :start_with?, s2), desc)
    }
  end

  def test_str_ord
    STRINGS.each {|s1|
      if s1.empty?
        assert_raise(ArgumentError) { s1.ord }
        next
      end
      if !s1.valid_encoding?
        assert_raise(ArgumentError) { s1.ord }
        next
      end
      assert_equal(s1[0].ord, s1.ord)
    }
  end

  def test_str_partition
    combination(STRINGS, STRINGS) {|s1, s2|
      desc = "#{encdump s1}.partition(#{encdump s2})"
      if !str_enc_compatible?(s1, s2)
        assert_raise(Encoding::CompatibilityError, desc) { s1.partition(s2) }
        next
      end
      i = enccall(s1, :index, s2)
      if !i
        assert_equal([s1, "", ""], s1.partition(s2), desc)
        next
      end
      assert_equal([s1[0,i], s2, s1[(i+s2.length)..-1]], s1.partition(s2), desc)
    }
  end

  def test_str_rpartition
    combination(STRINGS, STRINGS) {|s1, s2|
      desc = "#{encdump s1}.rpartition(#{encdump s2})"
      if !str_enc_compatible?(s1, s2)
        assert_raise(Encoding::CompatibilityError, desc) { s1.rpartition(s2) }
        next
      end
      i = enccall(s1, :rindex, s2)
      if !i
        assert_equal(["", "", s1], s1.rpartition(s2), desc)
        next
      end
      assert_equal([s1[0,i], s2, s1[(i+s2.length)..-1]], s1.rpartition(s2), desc)
    }
  end

end
