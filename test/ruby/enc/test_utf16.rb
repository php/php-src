require 'test/unit'

class TestUTF16 < Test::Unit::TestCase
  def encdump(obj)
    case obj
    when String
      d = obj.dump
      if /\.force_encoding\("[A-Za-z0-9.:_+-]*"\)\z/ =~ d
        d
      else
        "#{d}.force_encoding(#{obj.encoding.name.dump})"
      end
    when Regexp
      "Regexp.new(#{encdump(obj.source)}, #{obj.options})"
    else
      raise Argument, "unexpected: #{obj.inspect}"
    end
  end

  def enccall(recv, meth, *args)
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
    result = nil
    assert_nothing_raised(desc) {
      result = recv.send(meth, *args)
    }
    result
  end

  def assert_str_equal(expected, actual, message=nil)
    full_message = build_message(message, <<EOT)
#{encdump expected} expected but not equal to
#{encdump actual}.
EOT
    assert_block(full_message) { expected == actual }
  end

  # tests start

  def test_utf16be_valid_encoding
    [
      "\x00\x00",
      "\xd7\xff",
      "\xd8\x00\xdc\x00",
      "\xdb\xff\xdf\xff",
      "\xe0\x00",
      "\xff\xff",
    ].each {|s|
      s.force_encoding("utf-16be")
      assert_equal(true, s.valid_encoding?, "#{encdump s}.valid_encoding?")
    }
    [
      "\x00",
      "\xd7",
      "\xd8\x00",
      "\xd8\x00\xd8\x00",
      "\xdc\x00",
      "\xdc\x00\xd8\x00",
      "\xdc\x00\xdc\x00",
      "\xe0",
      "\xff",
    ].each {|s|
      s.force_encoding("utf-16be")
      assert_equal(false, s.valid_encoding?, "#{encdump s}.valid_encoding?")
    }
  end

  def test_utf16le_valid_encoding
    [
      "\x00\x00",
      "\xff\xd7",
      "\x00\xd8\x00\xdc",
      "\xff\xdb\xff\xdf",
      "\x00\xe0",
      "\xff\xff",
    ].each {|s|
      s.force_encoding("utf-16le")
      assert_equal(true, s.valid_encoding?, "#{encdump s}.valid_encoding?")
    }
    [
      "\x00",
      "\xd7",
      "\x00\xd8",
      "\x00\xd8\x00\xd8",
      "\x00\xdc",
      "\x00\xdc\x00\xd8",
      "\x00\xdc\x00\xdc",
      "\xe0",
      "\xff",
    ].each {|s|
      s.force_encoding("utf-16le")
      assert_equal(false, s.valid_encoding?, "#{encdump s}.valid_encoding?")
    }
  end

  def test_strftime
    s = "aa".force_encoding("utf-16be")
    assert_raise(ArgumentError, "Time.now.strftime(#{encdump s})") { Time.now.strftime(s) }
  end

  def test_intern
    s = "aaaa".force_encoding("utf-16be")
    assert_equal(s.encoding, s.intern.to_s.encoding, "#{encdump s}.intern.to_s.encoding")
  end

  def test_sym_eq
    s = "aa".force_encoding("utf-16le")
    assert(s.intern != :aa, "#{encdump s}.intern != :aa")
  end

  def test_compatible
    s1 = "aa".force_encoding("utf-16be")
    s2 = "z".force_encoding("us-ascii")
    assert_nil(Encoding.compatible?(s1, s2), "Encoding.compatible?(#{encdump s1}, #{encdump s2})")
  end

  def test_casecmp
    s1 = "aa".force_encoding("utf-16be")
    s2 = "AA"
    assert_not_equal(0, s1.casecmp(s2), "#{encdump s1}.casecmp(#{encdump s2})")
  end

  def test_end_with
    s1 = "ab".force_encoding("utf-16be")
    s2 = "b".force_encoding("utf-16be")
    assert_equal(false, s1.end_with?(s2), "#{encdump s1}.end_with?(#{encdump s2})")
  end

  def test_hex
    assert_raise(Encoding::CompatibilityError) {
      "ff".encode("utf-16le").hex
    }
    assert_raise(Encoding::CompatibilityError) {
      "ff".encode("utf-16be").hex
    }
  end

  def test_oct
    assert_raise(Encoding::CompatibilityError) {
      "77".encode("utf-16le").oct
    }
    assert_raise(Encoding::CompatibilityError) {
      "77".encode("utf-16be").oct
    }
  end

  def test_count
    s1 = "aa".force_encoding("utf-16be")
    s2 = "aa"
    assert_raise(Encoding::CompatibilityError, "#{encdump s1}.count(#{encdump s2})") {
      s1.count(s2)
    }
  end

  def test_plus
    s1 = "a".force_encoding("us-ascii")
    s2 = "aa".force_encoding("utf-16be")
    assert_raise(Encoding::CompatibilityError, "#{encdump s1} + #{encdump s2}") {
      s1 + s2
    }
  end

  def test_encoding_find
    assert_raise(ArgumentError) {
      Encoding.find("utf-8".force_encoding("utf-16be"))
    }
  end

  def test_interpolation
    s = "aa".force_encoding("utf-16be")
    assert_raise(Encoding::CompatibilityError, "\"a\#{#{encdump s}}\"") {
      "a#{s}"
    }
  end

  def test_slice!
    enccall("aa".force_encoding("UTF-16BE"), :slice!, -1)
  end

  def test_plus_empty1
    s1 = ""
    s2 = "aa".force_encoding("utf-16be")
    assert_nothing_raised("#{encdump s1} << #{encdump s2}") {
      s1 + s2
    }
  end

  def test_plus_empty2
    s1 = "aa"
    s2 = "".force_encoding("utf-16be")
    assert_nothing_raised("#{encdump s1} << #{encdump s2}") {
      s1 + s2
    }
  end

  def test_plus_nonempty
    s1 = "aa"
    s2 = "bb".force_encoding("utf-16be")
    assert_raise(Encoding::CompatibilityError, "#{encdump s1} << #{encdump s2}") {
      s1 + s2
    }
  end

  def test_concat_empty1
    s1 = ""
    s2 = "aa".force_encoding("utf-16be")
    assert_nothing_raised("#{encdump s1} << #{encdump s2}") {
      s1 << s2
    }
  end

  def test_concat_empty2
    s1 = "aa"
    s2 = "".force_encoding("utf-16be")
    assert_nothing_raised("#{encdump s1} << #{encdump s2}") {
      s1 << s2
    }
  end

  def test_concat_nonempty
    s1 = "aa"
    s2 = "bb".force_encoding("utf-16be")
    assert_raise(Encoding::CompatibilityError, "#{encdump s1} << #{encdump s2}") {
      s1 << s2
    }
  end

  def test_chomp
    s = "\1\n".force_encoding("utf-16be")
    assert_equal(s, s.chomp, "#{encdump s}.chomp")
    s = "\0\n".force_encoding("utf-16be")
    assert_equal("", s.chomp, "#{encdump s}.chomp")
    s = "\0\r\0\n".force_encoding("utf-16be")
    assert_equal("", s.chomp, "#{encdump s}.chomp")
  end

  def test_succ
    s = "\xff\xff".force_encoding("utf-16be")
    assert(s.succ.valid_encoding?, "#{encdump s}.succ.valid_encoding?")

    s = "\xdb\xff\xdf\xff".force_encoding("utf-16be")
    assert(s.succ.valid_encoding?, "#{encdump s}.succ.valid_encoding?")
  end

  def test_regexp_union
    enccall(Regexp, :union, "aa".force_encoding("utf-16be"), "bb".force_encoding("utf-16be"))
  end

  def test_empty_regexp
    s = "".force_encoding("utf-16be")
    assert_equal(Encoding.find("utf-16be"), Regexp.new(s).encoding,
                "Regexp.new(#{encdump s}).encoding")
  end

  def test_regexp_match
    assert_raise(Encoding::CompatibilityError) { Regexp.new("aa".force_encoding("utf-16be")) =~ "aa" }
  end

  def test_gsub
    s = "abcd".force_encoding("utf-16be")
    assert_nothing_raised {
      s.gsub(Regexp.new(".".encode("utf-16be")), "xy")
    }
    s = "ab\0\ncd".force_encoding("utf-16be")
    assert_raise(Encoding::CompatibilityError) {
      s.gsub(Regexp.new(".".encode("utf-16be")), "xy")
    }
  end

  def test_split_awk
    s = " ab cd ".encode("utf-16be")
    r = s.split(" ".encode("utf-16be"))
    assert_equal(2, r.length)
    assert_str_equal("ab".encode("utf-16be"), r[0])
    assert_str_equal("cd".encode("utf-16be"), r[1])
  end

  def test_count2
    e = "abc".count("^b")
    assert_equal(e, "abc".encode("utf-16be").count("^b".encode("utf-16be")))
    assert_equal(e, "abc".encode("utf-16le").count("^b".encode("utf-16le")))
  end

  def test_header
    assert_raise(ArgumentError) { eval("# encoding:utf-16le\nfoo") }
    assert_raise(ArgumentError) { eval("# encoding:utf-16be\nfoo") }
  end


  def test_is_mbc_newline
    sl = "f\0o\0o\0\n\0b\0a\0r\0\n\0b\0a\0z\0\n\0".force_encoding("utf-16le")
    sb = "\0f\0o\0o\0\n\0b\0a\0r\0\n\0b\0a\0z\0\n".force_encoding("utf-16be")
    al = sl.lines.to_a
    ab = sb.lines.to_a
    assert_equal("f\0o\0o\0\n\0".force_encoding("utf-16le"), al.shift)
    assert_equal("b\0a\0r\0\n\0".force_encoding("utf-16le"), al.shift)
    assert_equal("b\0a\0z\0\n\0".force_encoding("utf-16le"), al.shift)
    assert_equal("\0f\0o\0o\0\n".force_encoding("utf-16be"), ab.shift)
    assert_equal("\0b\0a\0r\0\n".force_encoding("utf-16be"), ab.shift)
    assert_equal("\0b\0a\0z\0\n".force_encoding("utf-16be"), ab.shift)

    sl = "f\0o\0o\0\n\0".force_encoding("utf-16le")
    sb = "\0f\0o\0o\0\n".force_encoding("utf-16be")
    sl2 = "f\0o\0o\0".force_encoding("utf-16le")
    sb2 = "\0f\0o\0o".force_encoding("utf-16be")
    assert_equal(sl2, sl.chomp)
    assert_equal(sl2, sl.chomp.chomp)
    assert_equal(sb2, sb.chomp)
    assert_equal(sb2, sb.chomp.chomp)

    sl = "f\0o\0o\0\n".force_encoding("utf-16le")
    sb = "\0f\0o\0o\n".force_encoding("utf-16be")
    assert_equal(sl, sl.chomp)
    assert_equal(sb, sb.chomp)
  end

  def test_code_to_mbc
    assert_equal("a\0".force_encoding("utf-16le"), "a".ord.chr("utf-16le"))
    assert_equal("\0a".force_encoding("utf-16be"), "a".ord.chr("utf-16be"))
  end

  def utf8_to_utf16(s, e)
    s.chars.map {|c| c.ord.chr(e) }.join
  end

  def test_mbc_case_fold
    rl = Regexp.new(utf8_to_utf16("^(\u3042)(a)\\1\\2$", "utf-16le"), "i")
    rb = Regexp.new(utf8_to_utf16("^(\u3042)(a)\\1\\2$", "utf-16be"), "i")
    assert_equal(Encoding.find("utf-16le"), rl.encoding)
    assert_equal(Encoding.find("utf-16be"), rb.encoding)
    assert_match(rl, utf8_to_utf16("\u3042a\u3042a", "utf-16le"))
    assert_match(rb, utf8_to_utf16("\u3042a\u3042a", "utf-16be"))
  end

  def test_surrogate_pair
    sl = "\x42\xd8\xb7\xdf".force_encoding("utf-16le")
    sb = "\xd8\x42\xdf\xb7".force_encoding("utf-16be")

    assert_equal(1, sl.size)
    assert_equal(1, sb.size)
    assert_equal(0x20bb7, sl.ord)
    assert_equal(0x20bb7, sb.ord)
    assert_equal(sl, 0x20bb7.chr("utf-16le"))
    assert_equal(sb, 0x20bb7.chr("utf-16be"))
    assert_equal("", sl.chop)
    assert_equal("", sb.chop)
  end

  def test_regexp_escape
    s = "\0*".force_encoding("UTF-16BE")
    r = Regexp.new(Regexp.escape(s))
    assert(r =~ s, "#{encdump(r)} =~ #{encdump(s)}")
  end

  def test_casecmp2
    assert_equal(0, "\0A".force_encoding("UTF-16BE").casecmp("\0a".force_encoding("UTF-16BE")))
    assert_not_equal(0, "\0A".force_encoding("UTF-16LE").casecmp("\0a".force_encoding("UTF-16LE")))
    assert_not_equal(0, "A\0".force_encoding("UTF-16BE").casecmp("a\0".force_encoding("UTF-16BE")))
    assert_equal(0, "A\0".force_encoding("UTF-16LE").casecmp("a\0".force_encoding("UTF-16LE")))

    ary = ["01".force_encoding("UTF-16LE"),
           "10".force_encoding("UTF-16LE")]
    e = ary.sort {|x,y| x <=> y }
    a = ary.sort {|x,y| x.casecmp(y) }
    assert_equal(e, a)
  end
end
