require 'test/unit'
require 'envutil'

class TestRegexp < Test::Unit::TestCase
  def setup
    @verbose = $VERBOSE
    $VERBOSE = nil
  end

  def teardown
    $VERBOSE = @verbose
  end

  def test_has_NOENCODING
    assert Regexp::NOENCODING
    re = //n
    assert_equal Regexp::NOENCODING, re.options
  end

  def test_ruby_dev_999
    assert_match(/(?<=a).*b/, "aab")
    assert_match(/(?<=\u3042).*b/, "\u3042ab")
  end

  def test_ruby_core_27247
    assert_match(/(a){2}z/, "aaz")
  end

  def test_ruby_dev_24643
    assert_nothing_raised("[ruby-dev:24643]") {
      /(?:(?:[a]*[a])?b)*a*$/ =~ "aabaaca"
    }
  end

  def test_ruby_talk_116455
    assert_match(/^(\w{2,}).* ([A-Za-z\xa2\xc0-\xff]{2,}?)$/n, "Hallo Welt")
  end

  def test_ruby_dev_24887
    assert_equal("a".gsub(/a\Z/, ""), "")
  end

  def test_yoshidam_net_20041111_1
    s = "[\xC2\xA0-\xC3\xBE]"
    assert_match(Regexp.new(s, nil, "u"), "\xC3\xBE")
  end

  def test_yoshidam_net_20041111_2
    assert_raise(RegexpError) do
      s = "[\xFF-\xFF]".force_encoding("utf-8")
      Regexp.new(s, nil, "u")
    end
  end

  def test_ruby_dev_31309
    assert_equal('Ruby', 'Ruby'.sub(/[^a-z]/i, '-'))
  end

  def test_assert_normal_exit
    # moved from knownbug.  It caused core.
    Regexp.union("a", "a")
  end

  def test_to_s
    assert_equal '(?-mix:\x00)', Regexp.new("\0").to_s
  end

  def test_union
    assert_equal :ok, begin
      Regexp.union(
        "a",
        Regexp.new("\xc2\xa1".force_encoding("euc-jp")),
        Regexp.new("\xc2\xa1".force_encoding("utf-8")))
      :ng
    rescue ArgumentError
      :ok
    end
  end

  def test_word_boundary
    assert_match(/\u3042\b /, "\u3042 ")
    assert_not_match(/\u3042\ba/, "\u3042a")
  end

  def test_named_capture
    m = /&(?<foo>.*?);/.match("aaa &amp; yyy")
    assert_equal("amp", m["foo"])
    assert_equal("amp", m[:foo])
    assert_equal(5, m.begin(:foo))
    assert_equal(8, m.end(:foo))
    assert_equal([5,8], m.offset(:foo))

    assert_equal("aaa [amp] yyy",
      "aaa &amp; yyy".sub(/&(?<foo>.*?);/, '[\k<foo>]'))

    assert_equal('#<MatchData "&amp; y" foo:"amp">',
      /&(?<foo>.*?); (y)/.match("aaa &amp; yyy").inspect)
    assert_equal('#<MatchData "&amp; y" 1:"amp" 2:"y">',
      /&(.*?); (y)/.match("aaa &amp; yyy").inspect)
    assert_equal('#<MatchData "&amp; y" foo:"amp" bar:"y">',
      /&(?<foo>.*?); (?<bar>y)/.match("aaa &amp; yyy").inspect)
    assert_equal('#<MatchData "&amp; y" foo:"amp" foo:"y">',
      /&(?<foo>.*?); (?<foo>y)/.match("aaa &amp; yyy").inspect)

    /(?<id>[A-Za-z_]+)/ =~ "!abc"
    assert_equal("abc", Regexp.last_match(:id))

    /a/ =~ "b" # doesn't match.
    assert_equal(nil, Regexp.last_match)
    assert_equal(nil, Regexp.last_match(1))
    assert_equal(nil, Regexp.last_match(:foo))

    assert_equal(["foo", "bar"], /(?<foo>.)(?<bar>.)/.names)
    assert_equal(["foo"], /(?<foo>.)(?<foo>.)/.names)
    assert_equal([], /(.)(.)/.names)

    assert_equal(["foo", "bar"], /(?<foo>.)(?<bar>.)/.match("ab").names)
    assert_equal(["foo"], /(?<foo>.)(?<foo>.)/.match("ab").names)
    assert_equal([], /(.)(.)/.match("ab").names)

    assert_equal({"foo"=>[1], "bar"=>[2]},
                 /(?<foo>.)(?<bar>.)/.named_captures)
    assert_equal({"foo"=>[1, 2]},
                 /(?<foo>.)(?<foo>.)/.named_captures)
    assert_equal({}, /(.)(.)/.named_captures)

    assert_equal("a[b]c", "abc".sub(/(?<x>[bc])/, "[\\k<x>]"))

    assert_equal("o", "foo"[/(?<bar>o)/, "bar"])

    s = "foo"
    s[/(?<bar>o)/, "bar"] = "baz"
    assert_equal("fbazo", s)
  end

  def test_assign_named_capture
    assert_equal("a", eval('/(?<foo>.)/ =~ "a"; foo'))
    assert_equal("a", eval('foo = 1; /(?<foo>.)/ =~ "a"; foo'))
    assert_equal("a", eval('1.times {|foo| /(?<foo>.)/ =~ "a"; break foo }'))
    assert_nothing_raised { eval('/(?<Foo>.)/ =~ "a"') }
    assert_nil(eval('/(?<Foo>.)/ =~ "a"; defined? Foo'))
  end

  def test_assign_named_capture_to_reserved_word
    /(?<nil>.)/ =~ "a"
    assert(!local_variables.include?(:nil), "[ruby-dev:32675]")
  end

  def test_match_regexp
    r = /./
    m = r.match("a")
    assert_equal(r, m.regexp)
    re = /foo/
    assert_equal(re, re.match("foo").regexp)
  end

  def test_source
    bug5484 = '[ruby-core:40364]'
    assert_equal('', //.source)
    assert_equal('\:', /\:/.source, bug5484)
    assert_equal(':', %r:\::.source, bug5484)
  end

  def test_source_escaped
    expected, result = "$*+.?^|".each_char.map {|c|
      [
        ["\\#{c}", "\\#{c}", 1],
        begin
          re = eval("%r#{c}\\#{c}#{c}", nil, __FILE__, __LINE__)
          t = eval("/\\#{c}/", nil, __FILE__, __LINE__).source
        rescue SyntaxError => e
          [e, t, nil]
        else
          [re.source, t, re =~ "a#{c}a"]
        end
      ]
    }.transpose
    assert_equal(expected, result)
  end

  def test_source_unescaped
    expected, result = "!\"#%&',-/:;=@_`~".each_char.map {|c|
      [
        ["#{c}", "\\#{c}", 1],
        begin
          re = eval("%r#{c}\\#{c}#{c}", nil, __FILE__, __LINE__)
          t = eval("%r{\\#{c}}", nil, __FILE__, __LINE__).source
        rescue SyntaxError => e
          [e, t, nil]
        else
          [re.source, t, re =~ "a#{c}a"]
        end
      ]
    }.transpose
    assert_equal(expected, result)
  end

  def test_inspect
    assert_equal('//', //.inspect)
    assert_equal('//i', //i.inspect)
    assert_equal('/\//i', /\//i.inspect)
    assert_equal('/\//i', %r"#{'/'}"i.inspect)
    assert_equal('/\/x/i', /\/x/i.inspect)
    assert_equal('/\x00/i', /#{"\0"}/i.inspect)
    assert_equal("/\n/i", /#{"\n"}/i.inspect)
    s = [0xf1, 0xf2, 0xf3].pack("C*")
    assert_equal('/\/\xF1\xF2\xF3/i', /\/#{s}/i.inspect)
  end

  def test_char_to_option
    assert_equal("BAR", "FOOBARBAZ"[/b../i])
    assert_equal("bar", "foobarbaz"[/  b  .  .  /x])
    assert_equal("bar\n", "foo\nbar\nbaz"[/b.../m])
    assert_raise(SyntaxError) { eval('//z') }
  end

  def test_char_to_option_kcode
    assert_equal("bar", "foobarbaz"[/b../s])
    assert_equal("bar", "foobarbaz"[/b../e])
    assert_equal("bar", "foobarbaz"[/b../u])
  end

  def test_to_s2
    assert_equal('(?-mix:foo)', /(?:foo)/.to_s)
    assert_equal('(?m-ix:foo)', /(?:foo)/m.to_s)
    assert_equal('(?mi-x:foo)', /(?:foo)/mi.to_s)
    assert_equal('(?mix:foo)', /(?:foo)/mix.to_s)
    assert_equal('(?m-ix:foo)', /(?m-ix:foo)/.to_s)
    assert_equal('(?mi-x:foo)', /(?mi-x:foo)/.to_s)
    assert_equal('(?mix:foo)', /(?mix:foo)/.to_s)
    assert_equal('(?mix:)', /(?mix)/.to_s)
    assert_equal('(?-mix:(?mix:foo) )', /(?mix:foo) /.to_s)
  end

  def test_casefold_p
    assert_equal(false, /a/.casefold?)
    assert_equal(true, /a/i.casefold?)
    assert_equal(false, /(?i:a)/.casefold?)
  end

  def test_options
    assert_equal(Regexp::IGNORECASE, /a/i.options)
    assert_equal(Regexp::EXTENDED, /a/x.options)
    assert_equal(Regexp::MULTILINE, /a/m.options)
  end

  def test_match_init_copy
    m = /foo/.match("foo")
    assert_equal(/foo/, m.dup.regexp)
    assert_raise(TypeError) do
      m.instance_eval { initialize_copy(nil) }
    end
    assert_equal([0, 3], m.offset(0))
    assert_equal(/foo/, m.dup.regexp)
  end

  def test_match_size
    m = /(.)(.)(\d+)(\d)/.match("THX1138.")
    assert_equal(5, m.size)
  end

  def test_match_offset_begin_end
    m = /(?<x>b..)/.match("foobarbaz")
    assert_equal([3, 6], m.offset("x"))
    assert_equal(3, m.begin("x"))
    assert_equal(6, m.end("x"))
    assert_raise(IndexError) { m.offset("y") }
    assert_raise(IndexError) { m.offset(2) }
    assert_raise(IndexError) { m.begin(2) }
    assert_raise(IndexError) { m.end(2) }

    m = /(?<x>q..)?/.match("foobarbaz")
    assert_equal([nil, nil], m.offset("x"))
    assert_equal(nil, m.begin("x"))
    assert_equal(nil, m.end("x"))

    m = /\A\u3042(.)(.)?(.)\z/.match("\u3042\u3043\u3044")
    assert_equal([1, 2], m.offset(1))
    assert_equal([nil, nil], m.offset(2))
    assert_equal([2, 3], m.offset(3))
  end

  def test_match_to_s
    m = /(?<x>b..)/.match("foobarbaz")
    assert_equal("bar", m.to_s)
  end

  def test_match_pre_post
    m = /(?<x>b..)/.match("foobarbaz")
    assert_equal("foo", m.pre_match)
    assert_equal("baz", m.post_match)
  end

  def test_match_array
    m = /(...)(...)(...)(...)?/.match("foobarbaz")
    assert_equal(["foobarbaz", "foo", "bar", "baz", nil], m.to_a)
  end

  def test_match_captures
    m = /(...)(...)(...)(...)?/.match("foobarbaz")
    assert_equal(["foo", "bar", "baz", nil], m.captures)
  end

  def test_match_aref
    m = /(...)(...)(...)(...)?/.match("foobarbaz")
    assert_equal("foo", m[1])
    assert_equal(["foo", "bar", "baz"], m[1..3])
    assert_nil(m[5])
    assert_raise(IndexError) { m[:foo] }
  end

  def test_match_values_at
    m = /(...)(...)(...)(...)?/.match("foobarbaz")
    assert_equal(["foo", "bar", "baz"], m.values_at(1, 2, 3))
  end

  def test_match_string
    m = /(?<x>b..)/.match("foobarbaz")
    assert_equal("foobarbaz", m.string)
  end

  def test_match_inspect
    m = /(...)(...)(...)(...)?/.match("foobarbaz")
    assert_equal('#<MatchData "foobarbaz" 1:"foo" 2:"bar" 3:"baz" 4:nil>', m.inspect)
  end

  def test_initialize
    assert_raise(ArgumentError) { Regexp.new }
    assert_equal(/foo/, Regexp.new(/foo/, Regexp::IGNORECASE))
    re = /foo/
    assert_raise(SecurityError) do
      Thread.new { $SAFE = 4; re.instance_eval { initialize(re) } }.join
    end
    re.taint
    assert_raise(SecurityError) do
      Thread.new { $SAFE = 4; re.instance_eval { initialize(re) } }.join
    end

    assert_equal(Encoding.find("US-ASCII"), Regexp.new("b..", nil, "n").encoding)
    assert_equal("bar", "foobarbaz"[Regexp.new("b..", nil, "n")])
    assert_equal(//n, Regexp.new("", nil, "n"))

    arg_encoding_none = 32 # ARG_ENCODING_NONE is implementation defined value
    assert_equal(arg_encoding_none, Regexp.new("", nil, "n").options)
    assert_equal(arg_encoding_none, Regexp.new("", nil, "N").options)

    assert_raise(RegexpError) { Regexp.new(")(") }
  end

  def test_unescape
    assert_raise(ArgumentError) { s = '\\'; /#{ s }/ }
    assert_equal(/\xFF/n, /#{ s="\\xFF" }/n)
    assert_equal(/\177/, (s = '\177'; /#{ s }/))
    assert_raise(ArgumentError) { s = '\u'; /#{ s }/ }
    assert_raise(ArgumentError) { s = '\u{ ffffffff }'; /#{ s }/ }
    assert_raise(ArgumentError) { s = '\u{ ffffff }'; /#{ s }/ }
    assert_raise(ArgumentError) { s = '\u{ ffff X }'; /#{ s }/ }
    assert_raise(ArgumentError) { s = '\u{ }'; /#{ s }/ }
    assert_equal("b", "abc"[(s = '\u{0062}'; /#{ s }/)])
    assert_equal("b", "abc"[(s = '\u0062'; /#{ s }/)])
    assert_raise(ArgumentError) { s = '\u0'; /#{ s }/ }
    assert_raise(ArgumentError) { s = '\u000X'; /#{ s }/ }
    assert_raise(ArgumentError) { s = "\xff" + '\u3042'; /#{ s }/ }
    assert_raise(ArgumentError) { s = '\u3042' + [0xff].pack("C"); /#{ s }/ }
    assert_raise(SyntaxError) { s = ''; eval(%q(/\u#{ s }/)) }

    assert_equal(/a/, eval(%q(s="\u0061";/#{s}/n)))
    assert_raise(RegexpError) { s = "\u3042"; eval(%q(/#{s}/n)) }
    assert_raise(RegexpError) { s = "\u0061"; eval(%q(/\u3042#{s}/n)) }
    assert_raise(RegexpError) { s1=[0xff].pack("C"); s2="\u3042"; eval(%q(/#{s1}#{s2}/)) }

    assert_raise(ArgumentError) { s = '\x'; /#{ s }/ }

    assert_equal("\xe1", [0x00, 0xe1, 0xff].pack("C*")[/\M-a/])
    assert_equal("\xdc", [0x00, 0xdc, 0xff].pack("C*")[/\M-\\/])
    assert_equal("\x8a", [0x00, 0x8a, 0xff].pack("C*")[/\M-\n/])
    assert_equal("\x89", [0x00, 0x89, 0xff].pack("C*")[/\M-\t/])
    assert_equal("\x8d", [0x00, 0x8d, 0xff].pack("C*")[/\M-\r/])
    assert_equal("\x8c", [0x00, 0x8c, 0xff].pack("C*")[/\M-\f/])
    assert_equal("\x8b", [0x00, 0x8b, 0xff].pack("C*")[/\M-\v/])
    assert_equal("\x87", [0x00, 0x87, 0xff].pack("C*")[/\M-\a/])
    assert_equal("\x9b", [0x00, 0x9b, 0xff].pack("C*")[/\M-\e/])
    assert_equal("\x01", [0x00, 0x01, 0xff].pack("C*")[/\C-a/])

    assert_raise(ArgumentError) { s = '\M'; /#{ s }/ }
    assert_raise(ArgumentError) { s = '\M-\M-a'; /#{ s }/ }
    assert_raise(ArgumentError) { s = '\M-\\'; /#{ s }/ }

    assert_raise(ArgumentError) { s = '\C'; /#{ s }/ }
    assert_raise(ArgumentError) { s = '\c'; /#{ s }/ }
    assert_raise(ArgumentError) { s = '\C-\C-a'; /#{ s }/ }

    assert_raise(ArgumentError) { s = '\M-\z'; /#{ s }/ }
    assert_raise(ArgumentError) { s = '\M-\777'; /#{ s }/ }

    assert_equal("\u3042\u3042", "\u3042\u3042"[(s = "\u3042" + %q(\xe3\x81\x82); /#{s}/)])
    assert_raise(ArgumentError) { s = "\u3042" + %q(\xe3); /#{s}/ }
    assert_raise(ArgumentError) { s = "\u3042" + %q(\xe3\xe3); /#{s}/ }
    assert_raise(ArgumentError) { s = '\u3042' + [0xff].pack("C"); /#{s}/ }

    assert_raise(SyntaxError) { eval("/\u3042/n") }

    s = ".........."
    5.times { s.sub!(".", "") }
    assert_equal(".....", s)
  end

  def test_equal
    bug5484 = '[ruby-core:40364]'
    assert_equal(/abc/, /abc/)
    assert_not_equal(/abc/, /abc/m)
    assert_not_equal(/abc/, /abd/)
    assert_equal(/\/foo/, Regexp.new('/foo'), bug5484)
  end

  def test_match
    assert_nil(//.match(nil))
    assert_equal("abc", /.../.match(:abc)[0])
    assert_raise(TypeError) { /.../.match(Object.new)[0] }
    assert_equal("bc", /../.match('abc', 1)[0])
    assert_equal("bc", /../.match('abc', -2)[0])
    assert_nil(/../.match("abc", -4))
    assert_nil(/../.match("abc", 4))
    assert_equal('\x', /../n.match("\u3042" + '\x', 1)[0])

    r = nil
    /.../.match("abc") {|m| r = m[0] }
    assert_equal("abc", r)

    $_ = "abc"; assert_equal(1, ~/bc/)
    $_ = "abc"; assert_nil(~/d/)
    $_ = nil; assert_nil(~/./)
  end

  def test_eqq
    assert_equal(false, /../ === nil)
  end

  def test_quote
    assert_equal("\xff", Regexp.quote([0xff].pack("C")))
    assert_equal("\\ ", Regexp.quote("\ "))
    assert_equal("\\t", Regexp.quote("\t"))
    assert_equal("\\n", Regexp.quote("\n"))
    assert_equal("\\r", Regexp.quote("\r"))
    assert_equal("\\f", Regexp.quote("\f"))
    assert_equal("\\v", Regexp.quote("\v"))
    assert_equal("\u3042\\t", Regexp.quote("\u3042\t"))
    assert_equal("\\t\xff", Regexp.quote("\t" + [0xff].pack("C")))
  end

  def test_try_convert
    assert_equal(/re/, Regexp.try_convert(/re/))
    assert_nil(Regexp.try_convert("re"))

    o = Object.new
    assert_nil(Regexp.try_convert(o))
    def o.to_regexp() /foo/ end
    assert_equal(/foo/, Regexp.try_convert(o))
  end

  def test_union2
    assert_equal(/(?!)/, Regexp.union)
    assert_equal(/foo/, Regexp.union(/foo/))
    assert_equal(/foo/, Regexp.union([/foo/]))
    assert_equal(/\t/, Regexp.union("\t"))
    assert_equal(/(?-mix:\u3042)|(?-mix:\u3042)/, Regexp.union(/\u3042/, /\u3042/))
    assert_equal("\u3041", "\u3041"[Regexp.union(/\u3042/, "\u3041")])
  end

  def test_dup
    assert_equal(//, //.dup)
    assert_raise(TypeError) { //.instance_eval { initialize_copy(nil) } }
  end

  def test_regsub
    assert_equal("fooXXXbaz", "foobarbaz".sub!(/bar/, "XXX"))
    s = [0xff].pack("C")
    assert_equal(s, "X".sub!(/./, s))
    assert_equal('\\' + s, "X".sub!(/./, '\\' + s))
    assert_equal('\k', "foo".sub!(/.../, '\k'))
    assert_raise(RuntimeError) { "foo".sub!(/(?<x>o)/, '\k<x') }
    assert_equal('foo[bar]baz', "foobarbaz".sub!(/(b..)/, '[\0]'))
    assert_equal('foo[foo]baz', "foobarbaz".sub!(/(b..)/, '[\`]'))
    assert_equal('foo[baz]baz', "foobarbaz".sub!(/(b..)/, '[\\\']'))
    assert_equal('foo[r]baz', "foobarbaz".sub!(/(b)(.)(.)/, '[\+]'))
    assert_equal('foo[\\]baz', "foobarbaz".sub!(/(b..)/, '[\\\\]'))
    assert_equal('foo[\z]baz', "foobarbaz".sub!(/(b..)/, '[\z]'))
  end

  def test_KCODE
    assert_nil($KCODE)
    assert_nothing_raised { $KCODE = nil }
    assert_equal(false, $=)
    assert_nothing_raised { $= = nil }
  end

  def test_match_setter
    /foo/ =~ "foo"
    m = $~
    /bar/ =~ "bar"
    $~ = m
    assert_equal("foo", $&)
  end

  def test_last_match
    /(...)(...)(...)(...)?/.match("foobarbaz")
    assert_equal("foobarbaz", Regexp.last_match(0))
    assert_equal("foo", Regexp.last_match(1))
    assert_nil(Regexp.last_match(5))
    assert_nil(Regexp.last_match(-1))
  end

  def test_getter
    alias $__REGEXP_TEST_LASTMATCH__ $&
    alias $__REGEXP_TEST_PREMATCH__ $`
    alias $__REGEXP_TEST_POSTMATCH__ $'
    alias $__REGEXP_TEST_LASTPARENMATCH__ $+
    /(b)(.)(.)/.match("foobarbaz")
    assert_equal("bar", $__REGEXP_TEST_LASTMATCH__)
    assert_equal("foo", $__REGEXP_TEST_PREMATCH__)
    assert_equal("baz", $__REGEXP_TEST_POSTMATCH__)
    assert_equal("r", $__REGEXP_TEST_LASTPARENMATCH__)

    /(...)(...)(...)/.match("foobarbaz")
    assert_equal("baz", $+)
  end

  def test_rindex_regexp
    assert_equal(3, "foobarbaz\u3042".rindex(/b../n, 5))
  end

  def test_taint
    m = Thread.new do
      "foo"[/foo/]
      $SAFE = 4
      /foo/.match("foo")
    end.value
    assert(m.tainted?)
    assert_nothing_raised('[ruby-core:26137]') {
      m = proc {$SAFE = 4; %r"#{ }"o}.call
    }
    assert(m.tainted?)
  end

  def check(re, ss, fs = [])
    re = Regexp.new(re) unless re.is_a?(Regexp)
    ss = [ss] unless ss.is_a?(Array)
    ss.each do |e, s|
      s ||= e
      assert_match(re, s)
      m = re.match(s)
      assert_equal(e, m[0])
    end
    fs = [fs] unless fs.is_a?(Array)
    fs.each {|s| assert_no_match(re, s) }
  end

  def failcheck(re)
    assert_raise(RegexpError) { %r"#{ re }" }
  end

  def test_parse
    check(/\*\+\?\{\}\|\(\)\<\>\`\'/, "*+?{}|()<>`'")
    check(/\A\w\W\z/, %w(a. b!), %w(.. ab))
    check(/\A.\b.\b.\B.\B.\z/, %w(a.aaa .a...), %w(aaaaa .....))
    check(/\A\s\S\z/, [' a', "\n."], ['  ', "\n\n", 'a '])
    check(/\A\d\D\z/, '0a', %w(00 aa))
    check(/\A\h\H\z/, %w(0g ag BH), %w(a0 af GG))
    check(/\Afoo\Z\s\z/, "foo\n", ["foo", "foo\nbar"])
    assert_equal(%w(a b c), "abc def".scan(/\G\w/))
    check(/\A\u3042\z/, "\u3042", ["", "\u3043", "a"])
    check(/\A(..)\1\z/, %w(abab ....), %w(abba aba))
    failcheck('\1')
    check(/\A\80\z/, "80", ["\100", ""])
    check(/\A\77\z/, "?")
    check(/\A\78\z/, "\7" + '8', ["\100", ""])
    check(/\A\Qfoo\E\z/, "QfooE")
    check(/\Aa++\z/, "aaa")
    check('\Ax]\z', "x]")
    check(/x#foo/x, "x", "#foo")
    check(/\Ax#foo#{ "\n" }x\z/x, "xx", ["x", "x#foo\nx"])
    check(/\A\p{Alpha}\z/, ["a", "z"], [".", "", ".."])
    check(/\A\p{^Alpha}\z/, [".", "!"], ["!a", ""])
    check(/\A\n\z/, "\n")
    check(/\A\t\z/, "\t")
    check(/\A\r\z/, "\r")
    check(/\A\f\z/, "\f")
    check(/\A\a\z/, "\007")
    check(/\A\e\z/, "\033")
    check(/\A\v\z/, "\v")
    failcheck('(')
    failcheck('(?foo)')
    failcheck('/\p{foobarbazqux}/')
    failcheck('/\p{foobarbazqux' + 'a' * 1000 + '}/')
    failcheck('/[1-\w]/')
  end

  def test_exec
    check(/A*B/, %w(B AB AAB AAAB), %w(A))
    check(/\w*!/, %w(! a! ab! abc!), %w(abc))
    check(/\w*\W/, %w(! a" ab# abc$), %w(abc))
    check(/\w*\w/, %w(z az abz abcz), %w(!))
    check(/[a-z]*\w/, %w(z az abz abcz), %w(!))
    check(/[a-z]*\W/, %w(! a" ab# abc$), %w(A))
    check(/((a|bb|ccc|dddd)(1|22|333|4444))/i, %w(a1 bb1 a22), %w(a2 b1))
    check(/\u0080/, (1..4).map {|i| ["\u0080", "\u0080" * i] }, ["\u0081"])
    check(/\u0080\u0080/, (2..4).map {|i| ["\u0080" * 2, "\u0080" * i] }, ["\u0081"])
    check(/\u0080\u0080\u0080/, (3..4).map {|i| ["\u0080" * 3, "\u0080" * i] }, ["\u0081"])
    check(/\u0080\u0080\u0080\u0080/, (4..4).map {|i| ["\u0080" * 4, "\u0080" * i] }, ["\u0081"])
    check(/[^\u3042\u3043\u3044]/, %W(a b \u0080 \u3041 \u3045), %W(\u3042 \u3043 \u3044))
    check(/a.+/m, %W(a\u0080 a\u0080\u0080 a\u0080\u0080\u0080), %W(a))
    check(/a.+z/m, %W(a\u0080z a\u0080\u0080z a\u0080\u0080\u0080z), %W(az))
    check(/abc\B.\Bxyz/, %w(abcXxyz abc0xyz), %w(abc|xyz abc-xyz))
    check(/\Bxyz/, [%w(xyz abcXxyz), %w(xyz abc0xyz)], %w(abc xyz abc-xyz))
    check(/abc\B/, [%w(abc abcXxyz), %w(abc abc0xyz)], %w(abc xyz abc-xyz))
    failcheck('(?<foo>abc)\1')
    check(/^(A+|B+)(?>\g<1>)*[BC]$/, %w(AC BC ABC BAC AABBC), %w(AABB))
    check(/^(A+|B(?>\g<1>)*)[AC]$/, %w(AAAC BBBAAAAC), %w(BBBAAA))
    check(/^()(?>\g<1>)*$/, "", "a")
    check(/^(?>(?=a)(#{ "a" * 1000 }|))++$/, ["a" * 1000, "a" * 2000, "a" * 3000], ["", "a" * 500, "b" * 1000])
    check(eval('/^(?:a?)?$/'), ["", "a"], ["aa"])
    check(eval('/^(?:a+)?$/'), ["", "a", "aa"], ["ab"])
    check(/^(?:a?)+?$/, ["", "a", "aa"], ["ab"])
    check(/^a??[ab]/, [["a", "a"], ["a", "aa"], ["b", "b"], ["a", "ab"]], ["c"])
    check(/^(?:a*){3,5}$/, ["", "a", "aa", "aaa", "aaaa", "aaaaa", "aaaaaa"], ["b"])
    check(/^(?:a+){3,5}$/, ["aaa", "aaaa", "aaaaa", "aaaaaa"], ["", "a", "aa", "b"])
  end

  def test_parse_look_behind
    check(/(?<=A)B(?=C)/, [%w(B ABC)], %w(aBC ABc aBc))
    check(/(?<!A)B(?!C)/, [%w(B aBc)], %w(ABC aBC ABc))
    failcheck('(?<=.*)')
    failcheck('(?<!.*)')
    check(/(?<=A|B.)C/, [%w(C AC), %w(C BXC)], %w(C BC))
    check(/(?<!A|B.)C/, [%w(C C), %w(C BC)], %w(AC BXC))

    assert_not_match(/(?<!aa|b)c/i, "Aac")
    assert_not_match(/(?<!b|aa)c/i, "Aac")
  end

  def test_parse_kg
    check(/\A(.)(.)\k<1>(.)\z/, %w(abac abab ....), %w(abcd aaba xxx))
    check(/\A(.)(.)\k<-1>(.)\z/, %w(abbc abba ....), %w(abcd aaba xxx))
    check(/\A(?<n>.)(?<x>\g<n>){0}(?<y>\k<n+0>){0}\g<x>\g<y>\z/, "aba", "abb")
    check(/\A(?<n>.)(?<x>\g<n>){0}(?<y>\k<n+1>){0}\g<x>\g<y>\z/, "abb", "aba")
    check(/\A(?<x>..)\k<x>\z/, %w(abab ....), %w(abac abba xxx))
    check(/\A(.)(..)\g<-1>\z/, "abcde", %w(.... ......))
    failcheck('\k<x>')
    failcheck('\k<')
    failcheck('\k<>')
    failcheck('\k<.>')
    failcheck('\k<x.>')
    failcheck('\k<1.>')
    failcheck('\k<x')
    failcheck('\k<x+')
    failcheck('()\k<-2>')
    failcheck('()\g<-2>')
    check(/\A(?<x>.)(?<x>.)\k<x>\z/, %w(aba abb), %w(abc .. ....))
    check(/\A(?<x>.)(?<x>.)\k<x>\z/i, %w(aba ABa abb ABb), %w(abc .. ....))
    check('\k\g', "kg")
    failcheck('(.\g<1>)')
    failcheck('(.\g<2>)')
    failcheck('(?=\g<1>)')
    failcheck('((?=\g<1>))')
    failcheck('(\g<1>|.)')
    failcheck('(.|\g<1>)')
    check(/(!)(?<=(a)|\g<1>)/, ["!"], %w(a))
    check(/^(a|b\g<1>c)$/, %w(a bac bbacc bbbaccc), %w(bbac bacc))
    check(/^(a|b\g<2>c)(B\g<1>C){0}$/, %w(a bBaCc bBbBaCcCc bBbBbBaCcCcCc), %w(bBbBaCcC BbBaCcCc))
    check(/\A(?<n>.|X\g<n>)(?<x>\g<n>){0}(?<y>\k<n+0>){0}\g<x>\g<y>\z/, "XXaXbXXa", %w(XXabXa abb))
    check(/\A(?<n>.|X\g<n>)(?<x>\g<n>){0}(?<y>\k<n+1>){0}\g<x>\g<y>\z/, "XaXXbXXb", %w(aXXbXb aba))
    failcheck('(?<x>)(?<x>)(\g<x>)')
    check(/^(?<x>foo)(bar)\k<x>/, %w(foobarfoo), %w(foobar barfoo))
    check(/^(?<a>f)(?<a>o)(?<a>o)(?<a>b)(?<a>a)(?<a>r)(?<a>b)(?<a>a)(?<a>z)\k<a>{9}$/, %w(foobarbazfoobarbaz foobarbazbazbarfoo foobarbazzabraboof), %w(foobar barfoo))
  end

  def test_parse_curly_brace
    check(/\A{/, ["{", ["{", "{x"]])
    check(/\A{ /, ["{ ", ["{ ", "{ x"]])
    check(/\A{,}\z/, "{,}")
    check(/\A{}\z/, "{}")
    check(/\Aa{0}+\z/, "", %w(a aa aab))
    check(/\Aa{1}+\z/, %w(a aa), ["", "aab"])
    check(/\Aa{1,2}b{1,2}\z/, %w(ab aab abb aabb), ["", "aaabb", "abbb"])
    check(/(?!x){0,1}/, [ ['', 'ab'], ['', ''] ])
    check(/c\z{0,1}/, [ ['c', 'abc'], ['c', 'cab']], ['abd'])
    check(/\A{0,1}a/, [ ['a', 'abc'], ['a', '____abc']], ['bcd'])
    failcheck('.{100001}')
    failcheck('.{0,100001}')
    failcheck('.{1,0}')
    failcheck('{0}')
  end

  def test_parse_comment
    check(/\A(?#foo\)bar)\z/, "", "a")
    failcheck('(?#')
  end

  def test_char_type
    check(/\u3042\d/, ["\u30421", "\u30422"])

    # CClassTable cache test
    assert_match(/\u3042\d/, "\u30421")
    assert_match(/\u3042\d/, "\u30422")
  end

  def test_char_class
    failcheck('[]')
    failcheck('[x')
    check('\A[]]\z', "]", "")
    check('\A[]\.]+\z', %w(] . ]..]), ["", "["])
    check(/\A[\u3042]\z/, "\u3042", "\u3042aa")
    check(/\A[\u3042\x61]+\z/, ["aa\u3042aa", "\u3042\u3042", "a"], ["", "b"])
    check(/\A[\u3042\x61\x62]+\z/, "abab\u3042abab\u3042")
    check(/\A[abc]+\z/, "abcba", ["", "ada"])
    check(/\A[\w][\W]\z/, %w(a. b!), %w(.. ab))
    check(/\A[\s][\S]\z/, [' a', "\n."], ['  ', "\n\n", 'a '])
    check(/\A[\d][\D]\z/, '0a', %w(00 aa))
    check(/\A[\h][\H]\z/, %w(0g ag BH), %w(a0 af GG))
    check(/\A[\p{Alpha}]\z/, ["a", "z"], [".", "", ".."])
    check(/\A[\p{^Alpha}]\z/, [".", "!"], ["!a", ""])
    check(/\A[\xff]\z/, "\xff", ["", "\xfe"])
    check(/\A[\80]+\z/, "8008", ["\\80", "\100", "\1000"])
    check(/\A[\77]+\z/, "???")
    check(/\A[\78]+\z/, "\788\7")
    check(/\A[\0]\z/, "\0")
    check(/\A[[:0]]\z/, [":", "0"], ["", ":0"])
    check(/\A[0-]\z/, ["0", "-"], "0-")
    check('\A[a-&&\w]\z', "a", "-")
    check('\A[--0]\z', ["-", "/", "0"], ["", "1"])
    check('\A[\'--0]\z', %w(* + \( \) 0 ,), ["", ".", "1"])
    check(/\A[a-b-]\z/, %w(a b -), ["", "c"])
    check('\A[a-b-&&\w]\z', %w(a b), ["", "-"])
    check('\A[a-b-&&\W]\z', "-", ["", "a", "b"])
    check('\A[a-c-e]\z', %w(a b c e), %w(- d)) # is it OK?
    check(/\A[a-f&&[^b-c]&&[^e]]\z/, %w(a d f), %w(b c e g 0))
    check(/\A[[^b-c]&&[^e]&&a-f]\z/, %w(a d f), %w(b c e g 0))
    check(/\A[\n\r\t]\z/, ["\n", "\r", "\t"])
    failcheck('[9-1]')

    assert_match(/\A\d+\z/, "0123456789")
    assert_no_match(/\d/, "\uff10\uff11\uff12\uff13\uff14\uff15\uff16\uff17\uff18\uff19")
    assert_match(/\A\w+\z/, "09azAZ_")
    assert_no_match(/\w/, "\uff10\uff19\uff41\uff5a\uff21\uff3a")
    assert_match(/\A\s+\z/, "\r\n\v\f\r\s")
    assert_no_match(/\s/, "\u0085")
  end

  def test_posix_bracket
    check(/\A[[:alpha:]0]\z/, %w(0 a), %w(1 .))
    check(/\A[[:^alpha:]0]\z/, %w(0 1 .), "a")
    check(/\A[[:alpha\:]]\z/, %w(a l p h a :), %w(b 0 1 .))
    check(/\A[[:alpha:foo]0]\z/, %w(0 a), %w(1 .))
    check(/\A[[:xdigit:]&&[:alpha:]]\z/, "a", %w(g 0))
    check('\A[[:abcdefghijklmnopqrstu:]]+\z', "[]")
    failcheck('[[:alpha')
    failcheck('[[:alpha:')
    failcheck('[[:alp:]]')

    assert_match(/\A[[:digit:]]+\z/, "\uff10\uff11\uff12\uff13\uff14\uff15\uff16\uff17\uff18\uff19")
    assert_match(/\A[[:alnum:]]+\z/, "\uff10\uff19\uff41\uff5a\uff21\uff3a")
    assert_match(/\A[[:space:]]+\z/, "\r\n\v\f\r\s\u0085")
    assert_match(/\A[[:ascii:]]+\z/, "\x00\x7F")
    assert_no_match(/[[:ascii:]]/, "\x80\xFF")
  end

  def test_backward
    assert_equal(3, "foobar".rindex(/b.r/i))
    assert_equal(nil, "foovar".rindex(/b.r/i))
    assert_equal(3, ("foo" + "bar" * 1000).rindex(/#{"bar"*1000}/))
    assert_equal(4, ("foo\nbar\nbaz\n").rindex(/bar/i))
  end

  def test_uninitialized
    assert_raise(TypeError) { Regexp.allocate.hash }
    assert_raise(TypeError) { Regexp.allocate.eql? Regexp.allocate }
    assert_raise(TypeError) { Regexp.allocate == Regexp.allocate }
    assert_raise(TypeError) { Regexp.allocate =~ "" }
    assert_equal(false, Regexp.allocate === Regexp.allocate)
    assert_nil(~Regexp.allocate)
    assert_raise(TypeError) { Regexp.allocate.match("") }
    assert_raise(TypeError) { Regexp.allocate.to_s }
    assert_match(/^#<Regexp:.*>$/, Regexp.allocate.inspect)
    assert_raise(TypeError) { Regexp.allocate.source }
    assert_raise(TypeError) { Regexp.allocate.casefold? }
    assert_raise(TypeError) { Regexp.allocate.options }
    assert_equal(Encoding.find("ASCII-8BIT"), Regexp.allocate.encoding)
    assert_equal(false, Regexp.allocate.fixed_encoding?)
    assert_raise(TypeError) { Regexp.allocate.names }
    assert_raise(TypeError) { Regexp.allocate.named_captures }

    assert_raise(TypeError) { MatchData.allocate.regexp }
    assert_raise(TypeError) { MatchData.allocate.names }
    assert_raise(TypeError) { MatchData.allocate.size }
    assert_raise(TypeError) { MatchData.allocate.length }
    assert_raise(TypeError) { MatchData.allocate.offset(0) }
    assert_raise(TypeError) { MatchData.allocate.begin(0) }
    assert_raise(TypeError) { MatchData.allocate.end(0) }
    assert_raise(TypeError) { MatchData.allocate.to_a }
    assert_raise(TypeError) { MatchData.allocate[:foo] }
    assert_raise(TypeError) { MatchData.allocate.captures }
    assert_raise(TypeError) { MatchData.allocate.values_at }
    assert_raise(TypeError) { MatchData.allocate.pre_match }
    assert_raise(TypeError) { MatchData.allocate.post_match }
    assert_raise(TypeError) { MatchData.allocate.to_s }
    assert_match(/^#<MatchData:.*>$/, MatchData.allocate.inspect)
    assert_raise(TypeError) { MatchData.allocate.string }
    $~ = MatchData.allocate
    assert_raise(TypeError) { $& }
    assert_raise(TypeError) { $` }
    assert_raise(TypeError) { $' }
    assert_raise(TypeError) { $+ }
  end

  def test_unicode
    assert_match(/^\u3042{0}\p{Any}$/, "a")
    assert_match(/^\u3042{0}\p{Any}$/, "\u3041")
    assert_match(/^\u3042{0}\p{Any}$/, "\0")
    assert_match(/^\p{Lo}{4}$/u, "\u3401\u4E01\u{20001}\u{2A701}")
    assert_no_match(/^\u3042{0}\p{Any}$/, "\0\0")
    assert_no_match(/^\u3042{0}\p{Any}$/, "")
    assert_raise(SyntaxError) { eval('/^\u3042{0}\p{' + "\u3042" + '}$/') }
    assert_raise(SyntaxError) { eval('/^\u3042{0}\p{' + 'a' * 1000 + '}$/') }
    assert_raise(SyntaxError) { eval('/^\u3042{0}\p{foobarbazqux}$/') }
    assert_match(/^(\uff21)(a)\1\2$/i, "\uff21A\uff41a")
    assert_no_match(/^(\uff21)\1$/i, "\uff21A")
    assert_no_match(/^(\uff41)\1$/i, "\uff41a")
    assert_match(/^\u00df$/i, "\u00df")
    assert_match(/^\u00df$/i, "ss")
    #assert_match(/^(\u00df)\1$/i, "\u00dfss") # this must be bug...
    assert_match(/^\u00df{2}$/i, "\u00dfss")
    assert_match(/^\u00c5$/i, "\u00c5")
    assert_match(/^\u00c5$/i, "\u00e5")
    assert_match(/^\u00c5$/i, "\u212b")
    assert_match(/^(\u00c5)\1\1$/i, "\u00c5\u00e5\u212b")
    assert_match(/^\u0149$/i, "\u0149")
    assert_match(/^\u0149$/i, "\u02bcn")
    #assert_match(/^(\u0149)\1$/i, "\u0149\u02bcn") # this must be bug...
    assert_match(/^\u0149{2}$/i, "\u0149\u02bcn")
    assert_match(/^\u0390$/i, "\u0390")
    assert_match(/^\u0390$/i, "\u03b9\u0308\u0301")
    #assert_match(/^(\u0390)\1$/i, "\u0390\u03b9\u0308\u0301") # this must be bug...
    assert_match(/^\u0390{2}$/i, "\u0390\u03b9\u0308\u0301")
    assert_match(/^\ufb05$/i, "\ufb05")
    assert_match(/^\ufb05$/i, "\ufb06")
    assert_match(/^\ufb05$/i, "st")
    #assert_match(/^(\ufb05)\1\1$/i, "\ufb05\ufb06st") # this must be bug...
    assert_match(/^\ufb05{3}$/i, "\ufb05\ufb06st")
    assert_match(/^\u03b9\u0308\u0301$/i, "\u0390")
  end

  def test_unicode_age
    assert_match(/^\p{Age=6.0}$/u, "\u261c")
    assert_match(/^\p{Age=1.1}$/u, "\u261c")
    assert_no_match(/^\P{age=6.0}$/u, "\u261c")

    assert_match(/^\p{age=6.0}$/u, "\u31f6")
    assert_match(/^\p{age=3.2}$/u, "\u31f6")
    assert_no_match(/^\p{age=3.1}$/u, "\u31f6")
    assert_no_match(/^\p{age=3.0}$/u, "\u31f6")
    assert_no_match(/^\p{age=1.1}$/u, "\u31f6")

    assert_match(/^\p{age=6.0}$/u, "\u2754")
    assert_no_match(/^\p{age=5.0}$/u, "\u2754")
    assert_no_match(/^\p{age=4.0}$/u, "\u2754")
    assert_no_match(/^\p{age=3.0}$/u, "\u2754")
    assert_no_match(/^\p{age=2.0}$/u, "\u2754")
    assert_no_match(/^\p{age=1.1}$/u, "\u2754")
  end

  def test_matchdata
    a = "haystack".match(/hay/)
    b = "haystack".match(/hay/)
    assert_equal(a, b, '[ruby-core:24748]')
    h = {a => 42}
    assert_equal(42, h[b], '[ruby-core:24748]')
  end

  def test_regexp_poped
    assert_nothing_raised { eval("a = 1; /\#{ a }/; a") }
    assert_nothing_raised { eval("a = 1; /\#{ a }/o; a") }
  end

  def test_invalid_fragment
    bug2547 = '[ruby-core:27374]'
    assert_raise(SyntaxError, bug2547) {eval('/#{"\\\\"}y/')}
  end

  def test_dup_warn
    assert_warn(/duplicated/) { Regexp.new('[\u3042\u3043\u3042]') }
    assert_warn(/duplicated/) { Regexp.new('[\u3042\u3043\u3043]') }
    assert_warn(/\A\z/) { Regexp.new('[\u3042\u3044\u3043]') }
    assert_warn(/\A\z/) { Regexp.new('[\u3042\u3045\u3043]') }
    assert_warn(/\A\z/) { Regexp.new('[\u3042\u3045\u3044]') }
    assert_warn(/\A\z/) { Regexp.new('[\u3042\u3045\u3043-\u3044]') }
    assert_warn(/duplicated/) { Regexp.new('[\u3042\u3045\u3042-\u3043]') }
    assert_warn(/duplicated/) { Regexp.new('[\u3042\u3045\u3044-\u3045]') }
    assert_warn(/\A\z/) { Regexp.new('[\u3042\u3046\u3044]') }
    assert_warn(/duplicated/) { Regexp.new('[\u1000-\u2000\u3042-\u3046\u3044]') }
    assert_warn(/duplicated/) { Regexp.new('[\u3044\u3041-\u3047]') }
    assert_warn(/duplicated/) { Regexp.new('[\u3042\u3044\u3046\u3041-\u3047]') }
  end

  def test_property_warn
    assert_in_out_err('-w', 'x=/\p%s/', [], %r"warning: invalid Unicode Property \\p: /\\p%s/")
  end

  def test_invalid_escape_error
    bug3539 = '[ruby-core:31048]'
    error = assert_raise(SyntaxError) {eval('/\x/', nil, bug3539)}
    assert_match(/invalid hex escape/, error.message)
    assert_equal(1, error.message.scan(/.*invalid .*escape.*/i).size, bug3539)
  end
end
