require 'test/unit'
require_relative 'envutil'

# use of $= is deprecated after 1.7.1
def pre_1_7_1
end

class TestString < Test::Unit::TestCase

  def initialize(*args)
    @cls = String
    @aref_re_nth = true
    @aref_re_silent = false
    @aref_slicebang_silent = true
    super
  end

  def S(str)
    @cls.new(str)
  end

  def test_s_new
    assert_equal("RUBY", S("RUBY"))
  end

  def test_AREF # '[]'
    assert_equal("A",  S("AooBar")[0])
    assert_equal("B",  S("FooBaB")[-1])
    assert_equal(nil, S("FooBar")[6])
    assert_equal(nil, S("FooBar")[-7])

    assert_equal(S("Foo"), S("FooBar")[0,3])
    assert_equal(S("Bar"), S("FooBar")[-3,3])
    assert_equal(S(""),    S("FooBar")[6,2])
    assert_equal(nil,      S("FooBar")[-7,10])

    assert_equal(S("Foo"), S("FooBar")[0..2])
    assert_equal(S("Foo"), S("FooBar")[0...3])
    assert_equal(S("Bar"), S("FooBar")[-3..-1])
    assert_equal(S(""),    S("FooBar")[6..2])
    assert_equal(nil,      S("FooBar")[-10..-7])

    assert_equal(S("Foo"), S("FooBar")[/^F../])
    assert_equal(S("Bar"), S("FooBar")[/..r$/])
    assert_equal(nil,      S("FooBar")[/xyzzy/])
    assert_equal(nil,      S("FooBar")[/plugh/])

    assert_equal(S("Foo"), S("FooBar")[S("Foo")])
    assert_equal(S("Bar"), S("FooBar")[S("Bar")])
    assert_equal(nil,      S("FooBar")[S("xyzzy")])
    assert_equal(nil,      S("FooBar")[S("plugh")])

    if @aref_re_nth
      assert_equal(S("Foo"), S("FooBar")[/([A-Z]..)([A-Z]..)/, 1])
      assert_equal(S("Bar"), S("FooBar")[/([A-Z]..)([A-Z]..)/, 2])
      assert_equal(nil,      S("FooBar")[/([A-Z]..)([A-Z]..)/, 3])
      assert_equal(S("Bar"), S("FooBar")[/([A-Z]..)([A-Z]..)/, -1])
      assert_equal(S("Foo"), S("FooBar")[/([A-Z]..)([A-Z]..)/, -2])
      assert_equal(nil,      S("FooBar")[/([A-Z]..)([A-Z]..)/, -3])
    end

    o = Object.new
    def o.to_int; 2; end
    assert_equal("o", "foo"[o])

    assert_raise(ArgumentError) { "foo"[] }
  end

  def test_ASET # '[]='
    s = S("FooBar")
    s[0] = S('A')
    assert_equal(S("AooBar"), s)

    s[-1]= S('B')
    assert_equal(S("AooBaB"), s)
    assert_raise(IndexError) { s[-7] = S("xyz") }
    assert_equal(S("AooBaB"), s)
    s[0] = S("ABC")
    assert_equal(S("ABCooBaB"), s)

    s = S("FooBar")
    s[0,3] = S("A")
    assert_equal(S("ABar"),s)
    s[0] = S("Foo")
    assert_equal(S("FooBar"), s)
    s[-3,3] = S("Foo")
    assert_equal(S("FooFoo"), s)
    assert_raise(IndexError) { s[7,3] =  S("Bar") }
    assert_raise(IndexError) { s[-7,3] = S("Bar") }

    s = S("FooBar")
    s[0..2] = S("A")
    assert_equal(S("ABar"), s)
    s[1..3] = S("Foo")
    assert_equal(S("AFoo"), s)
    s[-4..-4] = S("Foo")
    assert_equal(S("FooFoo"), s)
    assert_raise(RangeError) { s[7..10]   = S("Bar") }
    assert_raise(RangeError) { s[-7..-10] = S("Bar") }

    s = S("FooBar")
    s[/^F../]= S("Bar")
    assert_equal(S("BarBar"), s)
    s[/..r$/] = S("Foo")
    assert_equal(S("BarFoo"), s)
    if @aref_re_silent
      s[/xyzzy/] = S("None")
      assert_equal(S("BarFoo"), s)
    else
      assert_raise(IndexError) { s[/xyzzy/] = S("None") }
    end
    if @aref_re_nth
      s[/([A-Z]..)([A-Z]..)/, 1] = S("Foo")
      assert_equal(S("FooFoo"), s)
      s[/([A-Z]..)([A-Z]..)/, 2] = S("Bar")
      assert_equal(S("FooBar"), s)
      assert_raise(IndexError) { s[/([A-Z]..)([A-Z]..)/, 3] = "None" }
      s[/([A-Z]..)([A-Z]..)/, -1] = S("Foo")
      assert_equal(S("FooFoo"), s)
      s[/([A-Z]..)([A-Z]..)/, -2] = S("Bar")
      assert_equal(S("BarFoo"), s)
      assert_raise(IndexError) { s[/([A-Z]..)([A-Z]..)/, -3] = "None" }
    end

    s = S("FooBar")
    s[S("Foo")] = S("Bar")
    assert_equal(S("BarBar"), s)

    pre_1_7_1 do
      s = S("FooBar")
      s[S("Foo")] = S("xyz")
      assert_equal(S("xyzBar"), s)

      $= = true
      s = S("FooBar")
      s[S("FOO")] = S("Bar")
      assert_equal(S("BarBar"), s)
      s[S("FOO")] = S("xyz")
      assert_equal(S("BarBar"), s)
      $= = false
    end

    s = S("a string")
    s[0..s.size] = S("another string")
    assert_equal(S("another string"), s)

    o = Object.new
    def o.to_int; 2; end
    s = "foo"
    s[o] = "bar"
    assert_equal("fobar", s)

    assert_raise(ArgumentError) { "foo"[1, 2, 3] = "" }
  end

  def test_CMP # '<=>'
    assert_equal(1, S("abcdef") <=> S("abcde"))
    assert_equal(0, S("abcdef") <=> S("abcdef"))
    assert_equal(-1, S("abcde") <=> S("abcdef"))

    assert_equal(-1, S("ABCDEF") <=> S("abcdef"))

    pre_1_7_1 do
      $= = true
      assert_equal(0, S("ABCDEF") <=> S("abcdef"))
      $= = false
    end

    assert_nil("foo" <=> Object.new)

    o = Object.new
    def o.to_str; "bar"; end
    assert_nil("foo" <=> o)

    def o.<=>(x); nil; end
    assert_nil("foo" <=> o)

    class << o;remove_method :<=>;end
    def o.<=>(x); 1; end
    assert_equal(-1, "foo" <=> o)

    class << o;remove_method :<=>;end
    def o.<=>(x); 2**100; end
    assert_equal(-(2**100), "foo" <=> o)
  end

  def test_EQUAL # '=='
    assert_equal(false, S("foo") == :foo)
    assert(S("abcdef") == S("abcdef"))

    pre_1_7_1 do
      $= = true
      assert(S("CAT") == S('cat'))
      assert(S("CaT") == S('cAt'))
      $= = false
    end

    assert(S("CAT") != S('cat'))
    assert(S("CaT") != S('cAt'))

    o = Object.new
    def o.to_str; end
    def o.==(x); false; end
    assert_equal(false, "foo" == o)
    class << o;remove_method :==;end
    def o.==(x); true; end
    assert_equal(true, "foo" == o)
  end

  def test_LSHIFT # '<<'
    assert_equal(S("world!"), S("world") << 33)
    assert_equal(S("world!"), S("world") << S('!'))

    s = "a"
    10.times {|i|
      s << s
      assert_equal("a" * (2 << i), s)
    }

    s = ["foo"].pack("p")
    l = s.size
    s << "bar"
    assert_equal(l + 3, s.size)

    bug = '[ruby-core:27583]'
    assert_raise(RangeError, bug) {S("a".force_encoding(Encoding::UTF_8)) << -3}
    assert_raise(RangeError, bug) {S("a".force_encoding(Encoding::UTF_8)) << -2}
    assert_raise(RangeError, bug) {S("a".force_encoding(Encoding::UTF_8)) << -1}
    assert_raise(RangeError, bug) {S("a".force_encoding(Encoding::UTF_8)) << 0x81308130}
    assert_nothing_raised {S("a".force_encoding(Encoding::GB18030)) << 0x81308130}
  end

  def test_MATCH # '=~'
    assert_equal(10,  S("FeeFieFoo-Fum") =~ /Fum$/)
    assert_equal(nil, S("FeeFieFoo-Fum") =~ /FUM$/)

    pre_1_7_1 do
      $= = true
      assert_equal(10,  S("FeeFieFoo-Fum") =~ /FUM$/)
      $= = false
    end

    o = Object.new
    def o.=~(x); x + "bar"; end
    assert_equal("foobar", S("foo") =~ o)

    assert_raise(TypeError) { S("foo") =~ "foo" }
  end

  def test_MOD # '%'
    assert_equal(S("00123"), S("%05d") % 123)
    assert_equal(S("123  |00000001"), S("%-5s|%08x") % [123, 1])
    x = S("%3s %-4s%%foo %.0s%5d %#x%c%3.1f %b %x %X %#b %#x %#X") %
    [S("hi"),
      123,
      S("never seen"),
      456,
      0,
      ?A,
      3.0999,
      11,
      171,
      171,
      11,
      171,
      171]

    assert_equal(S(' hi 123 %foo   456 0A3.1 1011 ab AB 0b1011 0xab 0XAB'), x)
  end

  def test_MUL # '*'
    assert_equal(S("XXX"),  S("X") * 3)
    assert_equal(S("HOHO"), S("HO") * 2)
  end

  def test_PLUS # '+'
    assert_equal(S("Yodel"), S("Yo") + S("del"))
  end

  def casetest(a, b, rev=false)
    case a
      when b
        assert(!rev)
      else
        assert(rev)
    end
  end

  def test_VERY_EQUAL # '==='
    # assert_equal(true, S("foo") === :foo)
    casetest(S("abcdef"), S("abcdef"))

    pre_1_7_1 do
      $= = true
      casetest(S("CAT"), S('cat'))
      casetest(S("CaT"), S('cAt'))
      $= = false
    end

    casetest(S("CAT"), S('cat'), true) # Reverse the test - we don't want to
    casetest(S("CaT"), S('cAt'), true) # find these in the case.
  end

  def test_capitalize
    assert_equal(S("Hello"),  S("hello").capitalize)
    assert_equal(S("Hello"),  S("hELLO").capitalize)
    assert_equal(S("123abc"), S("123ABC").capitalize)
  end

  def test_capitalize!
    a = S("hello"); a.capitalize!
    assert_equal(S("Hello"), a)

    a = S("hELLO"); a.capitalize!
    assert_equal(S("Hello"), a)

    a = S("123ABC"); a.capitalize!
    assert_equal(S("123abc"), a)

    assert_equal(nil,         S("123abc").capitalize!)
    assert_equal(S("123abc"), S("123ABC").capitalize!)
    assert_equal(S("Abc"),    S("ABC").capitalize!)
    assert_equal(S("Abc"),    S("abc").capitalize!)
    assert_equal(nil,         S("Abc").capitalize!)

    a = S("hello")
    b = a.dup
    assert_equal(S("Hello"), a.capitalize!)
    assert_equal(S("hello"), b)

  end

  Bug2463 = '[ruby-dev:39856]'
  def test_center
    assert_equal(S("hello"),       S("hello").center(4))
    assert_equal(S("   hello   "), S("hello").center(11))
    assert_equal(S("ababaababa"), S("").center(10, "ab"), Bug2463)
    assert_equal(S("ababaababab"), S("").center(11, "ab"), Bug2463)
  end

  def test_chomp
    assert_equal(S("hello"), S("hello").chomp("\n"))
    assert_equal(S("hello"), S("hello\n").chomp("\n"))
    save = $/

    $/ = "\n"

    assert_equal(S("hello"), S("hello").chomp)
    assert_equal(S("hello"), S("hello\n").chomp)

    $/ = "!"
    assert_equal(S("hello"), S("hello").chomp)
    assert_equal(S("hello"), S("hello!").chomp)
    $/ = save

    assert_equal(S("a").hash, S("a\u0101").chomp(S("\u0101")).hash, '[ruby-core:22414]')
  end

  def test_chomp!
    a = S("hello")
    a.chomp!(S("\n"))

    assert_equal(S("hello"), a)
    assert_equal(nil, a.chomp!(S("\n")))

    a = S("hello\n")
    a.chomp!(S("\n"))
    assert_equal(S("hello"), a)
    save = $/

    $/ = "\n"
    a = S("hello")
    a.chomp!
    assert_equal(S("hello"), a)

    a = S("hello\n")
    a.chomp!
    assert_equal(S("hello"), a)

    $/ = "!"
    a = S("hello")
    a.chomp!
    assert_equal(S("hello"), a)

    a="hello!"
    a.chomp!
    assert_equal(S("hello"), a)

    $/ = save

    a = S("hello\n")
    b = a.dup
    assert_equal(S("hello"), a.chomp!)
    assert_equal(S("hello\n"), b)

    s = "foo\r\n"
    s.chomp!
    assert_equal("foo", s)

    s = "foo\r"
    s.chomp!
    assert_equal("foo", s)

    s = "foo\r\n"
    s.chomp!("")
    assert_equal("foo", s)

    s = "foo\r"
    s.chomp!("")
    assert_equal("foo\r", s)

    assert_equal(S("a").hash, S("a\u0101").chomp!(S("\u0101")).hash, '[ruby-core:22414]')
  end

  def test_chop
    assert_equal(S("hell"),    S("hello").chop)
    assert_equal(S("hello"),   S("hello\r\n").chop)
    assert_equal(S("hello\n"), S("hello\n\r").chop)
    assert_equal(S(""),        S("\r\n").chop)
    assert_equal(S(""),        S("").chop)
    assert_equal(S("a").hash,  S("a\u00d8").chop.hash)
  end

  def test_chop!
    a = S("hello").chop!
    assert_equal(S("hell"), a)

    a = S("hello\r\n").chop!
    assert_equal(S("hello"), a)

    a = S("hello\n\r").chop!
    assert_equal(S("hello\n"), a)

    a = S("\r\n").chop!
    assert_equal(S(""), a)

    a = S("").chop!
    assert_nil(a)

    a = S("a\u00d8")
    a.chop!
    assert_equal(S("a").hash, a.hash)

    a = S("hello\n")
    b = a.dup
    assert_equal(S("hello"),   a.chop!)
    assert_equal(S("hello\n"), b)
  end

  def test_clone
    for taint in [ false, true ]
      for untrust in [ false, true ]
        for frozen in [ false, true ]
          a = S("Cool")
          a.taint  if taint
          a.untrust  if untrust
          a.freeze if frozen
          b = a.clone

          assert_equal(a, b)
          assert(a.__id__ != b.__id__)
          assert_equal(a.frozen?, b.frozen?)
          assert_equal(a.untrusted?, b.untrusted?)
          assert_equal(a.tainted?, b.tainted?)
        end
      end
    end

    null = File.exist?("/dev/null") ? "/dev/null" : "NUL" # maybe DOSISH
    assert_equal("", File.read(null).clone, '[ruby-dev:32819] reported by Kazuhiro NISHIYAMA')
  end

  def test_concat
    assert_equal(S("world!"), S("world").concat(33))
    assert_equal(S("world!"), S("world").concat(S('!')))
  end

  def test_count
    a = S("hello world")
    assert_equal(5, a.count(S("lo")))
    assert_equal(2, a.count(S("lo"), S("o")))
    assert_equal(4, a.count(S("hello"), S("^l")))
    assert_equal(4, a.count(S("ej-m")))
    assert_equal(0, S("y").count(S("a\\-z")))
    assert_equal(5, "abc\u{3042 3044 3046}".count("^a"))
    assert_equal(5, "abc\u{3042 3044 3046}".count("^\u3042"))
    assert_equal(2, "abc\u{3042 3044 3046}".count("a-z", "^a"))

    assert_raise(ArgumentError) { "foo".count }
  end

  def test_crypt
    assert_equal(S('aaGUC/JkO9/Sc'), S("mypassword").crypt(S("aa")))
    assert(S('aaGUC/JkO9/Sc') != S("mypassword").crypt(S("ab")))
  end

  def test_delete
    assert_equal(S("heo"),  S("hello").delete(S("l"), S("lo")))
    assert_equal(S("he"),   S("hello").delete(S("lo")))
    assert_equal(S("hell"), S("hello").delete(S("aeiou"), S("^e")))
    assert_equal(S("ho"),   S("hello").delete(S("ej-m")))

    assert_equal("a".hash, "a\u0101".delete("\u0101").hash, '[ruby-talk:329267]')
    assert_equal(true, "a\u0101".delete("\u0101").ascii_only?)
    assert_equal(true, "a\u3041".delete("\u3041").ascii_only?)
    assert_equal(false, "a\u3041\u3042".tr("\u3041", "a").ascii_only?)

    assert_equal("a", "abc\u{3042 3044 3046}".delete("^a"))
    assert_equal("bc\u{3042 3044 3046}", "abc\u{3042 3044 3046}".delete("a"))
    assert_equal("\u3042", "abc\u{3042 3044 3046}".delete("^\u3042"))

    bug6160 = '[ruby-dev:45374]'
    assert_equal("", '\\'.delete('\\'), bug6160)
  end

  def test_delete!
    a = S("hello")
    a.delete!(S("l"), S("lo"))
    assert_equal(S("heo"), a)

    a = S("hello")
    a.delete!(S("lo"))
    assert_equal(S("he"), a)

    a = S("hello")
    a.delete!(S("aeiou"), S("^e"))
    assert_equal(S("hell"), a)

    a = S("hello")
    a.delete!(S("ej-m"))
    assert_equal(S("ho"), a)

    a = S("hello")
    assert_nil(a.delete!(S("z")))

    a = S("hello")
    b = a.dup
    a.delete!(S("lo"))
    assert_equal(S("he"), a)
    assert_equal(S("hello"), b)

    a = S("hello")
    a.delete!(S("^el"))
    assert_equal(S("ell"), a)

    assert_raise(ArgumentError) { S("foo").delete! }
  end


  def test_downcase
    assert_equal(S("hello"), S("helLO").downcase)
    assert_equal(S("hello"), S("hello").downcase)
    assert_equal(S("hello"), S("HELLO").downcase)
    assert_equal(S("abc hello 123"), S("abc HELLO 123").downcase)
  end

  def test_downcase!
    a = S("helLO")
    b = a.dup
    assert_equal(S("hello"), a.downcase!)
    assert_equal(S("hello"), a)
    assert_equal(S("helLO"), b)

    a=S("hello")
    assert_nil(a.downcase!)
    assert_equal(S("hello"), a)
  end

  def test_dump
    a= S("Test") << 1 << 2 << 3 << 9 << 13 << 10
    assert_equal(S('"Test\\x01\\x02\\x03\\t\\r\\n"'), a.dump)
  end

  def test_dup
    for taint in [ false, true ]
      for untrust in [ false, true ]
        for frozen in [ false, true ]
          a = S("hello")
          a.taint  if taint
          a.untrust  if untrust
          a.freeze if frozen
          b = a.dup

          assert_equal(a, b)
          assert(a.__id__ != b.__id__)
          assert(!b.frozen?)
          assert_equal(a.tainted?, b.tainted?)
          assert_equal(a.untrusted?, b.untrusted?)
        end
      end
    end
  end

  def test_each
    save = $/
    $/ = "\n"
    res=[]
    S("hello\nworld").lines.each {|x| res << x}
    assert_equal(S("hello\n"), res[0])
    assert_equal(S("world"),   res[1])

    res=[]
    S("hello\n\n\nworld").lines(S('')).each {|x| res << x}
    assert_equal(S("hello\n\n\n"), res[0])
    assert_equal(S("world"),       res[1])

    $/ = "!"
    res=[]
    S("hello!world").lines.each {|x| res << x}
    assert_equal(S("hello!"), res[0])
    assert_equal(S("world"),  res[1])
    $/ = save
  end

  def test_each_byte
    res = []
    S("ABC").each_byte {|x| res << x }
    assert_equal(65, res[0])
    assert_equal(66, res[1])
    assert_equal(67, res[2])
  end

  def test_each_line
    save = $/
    $/ = "\n"
    res=[]
    S("hello\nworld").lines.each {|x| res << x}
    assert_equal(S("hello\n"), res[0])
    assert_equal(S("world"),   res[1])

    res=[]
    S("hello\n\n\nworld").lines(S('')).each {|x| res << x}
    assert_equal(S("hello\n\n\n"), res[0])
    assert_equal(S("world"),       res[1])

    $/ = "!"

    res=[]
    S("hello!world").lines.each {|x| res << x}
    assert_equal(S("hello!"), res[0])
    assert_equal(S("world"),  res[1])

    $/ = save

    s = nil
    "foo\nbar".each_line(nil) {|s2| s = s2 }
    assert_equal("foo\nbar", s)
  end

  def test_empty?
    assert(S("").empty?)
    assert(!S("not").empty?)
  end

  def test_eql?
    a = S("hello")
    assert(a.eql?(S("hello")))
    assert(a.eql?(a))
  end

  def test_gsub
    assert_equal(S("h*ll*"),     S("hello").gsub(/[aeiou]/, S('*')))
    assert_equal(S("h<e>ll<o>"), S("hello").gsub(/([aeiou])/, S('<\1>')))
    assert_equal(S("h e l l o "),
                 S("hello").gsub(/./) { |s| s[0].to_s + S(' ')})
    assert_equal(S("HELL-o"),
                 S("hello").gsub(/(hell)(.)/) { |s| $1.upcase + S('-') + $2 })

    a = S("hello")
    a.taint
    a.untrust
    assert(a.gsub(/./, S('X')).tainted?)
    assert(a.gsub(/./, S('X')).untrusted?)

    assert_equal("z", "abc".gsub(/./, "a" => "z"), "moved from btest/knownbug")

    assert_raise(ArgumentError) { "foo".gsub }
  end

  def test_gsub_encoding
    a = S("hello world")
    a.force_encoding Encoding::UTF_8

    b = S("hi")
    b.force_encoding Encoding::US_ASCII

    assert_equal Encoding::UTF_8, a.gsub(/hello/, b).encoding

    c = S("everybody")
    c.force_encoding Encoding::US_ASCII

    assert_equal Encoding::UTF_8, a.gsub(/world/, c).encoding
  end

  def test_gsub!
    a = S("hello")
    b = a.dup
    a.gsub!(/[aeiou]/, S('*'))
    assert_equal(S("h*ll*"), a)
    assert_equal(S("hello"), b)

    a = S("hello")
    a.gsub!(/([aeiou])/, S('<\1>'))
    assert_equal(S("h<e>ll<o>"), a)

    a = S("hello")
    a.gsub!(/./) { |s| s[0].to_s + S(' ')}
    assert_equal(S("h e l l o "), a)

    a = S("hello")
    a.gsub!(/(hell)(.)/) { |s| $1.upcase + S('-') + $2 }
    assert_equal(S("HELL-o"), a)

    r = S('X')
    r.taint
    r.untrust
    a.gsub!(/./, r)
    assert(a.tainted?)
    assert(a.untrusted?)

    a = S("hello")
    assert_nil(a.sub!(S('X'), S('Y')))
  end

  def test_sub_hash
    assert_equal('azc', 'abc'.sub(/b/, "b" => "z"))
    assert_equal('ac', 'abc'.sub(/b/, {}))
    assert_equal('a1c', 'abc'.sub(/b/, "b" => 1))
    assert_equal('aBc', 'abc'.sub(/b/, Hash.new {|h, k| k.upcase }))
    assert_equal('a[\&]c', 'abc'.sub(/b/, "b" => '[\&]'))
    assert_equal('aBcabc', 'abcabc'.sub(/b/, Hash.new {|h, k| h[k] = k.upcase }))
    assert_equal('aBcdef', 'abcdef'.sub(/de|b/, "b" => "B", "de" => "DE"))
  end

  def test_gsub_hash
    assert_equal('azc', 'abc'.gsub(/b/, "b" => "z"))
    assert_equal('ac', 'abc'.gsub(/b/, {}))
    assert_equal('a1c', 'abc'.gsub(/b/, "b" => 1))
    assert_equal('aBc', 'abc'.gsub(/b/, Hash.new {|h, k| k.upcase }))
    assert_equal('a[\&]c', 'abc'.gsub(/b/, "b" => '[\&]'))
    assert_equal('aBcaBc', 'abcabc'.gsub(/b/, Hash.new {|h, k| h[k] = k.upcase }))
    assert_equal('aBcDEf', 'abcdef'.gsub(/de|b/, "b" => "B", "de" => "DE"))
  end

  def test_hash
    assert_equal(S("hello").hash, S("hello").hash)
    assert(S("hello").hash != S("helLO").hash)
    bug4104 = '[ruby-core:33500]'
    assert_not_equal(S("a").hash, S("a\0").hash, bug4104)
  end

  def test_hash_random
    str = 'abc'
    a = [str.hash.to_s]
    3.times {
      assert_in_out_err(["-e", "print #{str.dump}.hash"], "") do |r, e|
        a += r
        assert_equal([], e)
      end
    }
    assert_not_equal([str.hash.to_s], a.uniq)
  end

  def test_hex
    assert_equal(255,  S("0xff").hex)
    assert_equal(-255, S("-0xff").hex)
    assert_equal(255,  S("ff").hex)
    assert_equal(-255, S("-ff").hex)
    assert_equal(0,    S("-ralph").hex)
    assert_equal(-15,  S("-fred").hex)
    assert_equal(15,   S("fred").hex)
  end

  def test_include?
    assert( S("foobar").include?(?f))
    assert( S("foobar").include?(S("foo")))
    assert(!S("foobar").include?(S("baz")))
    assert(!S("foobar").include?(?z))
  end

  def test_index
    assert_equal(0, S("hello").index(?h))
    assert_equal(1, S("hello").index(S("ell")))
    assert_equal(2, S("hello").index(/ll./))

    assert_equal(3, S("hello").index(?l, 3))
    assert_equal(3, S("hello").index(S("l"), 3))
    assert_equal(3, S("hello").index(/l./, 3))

    assert_nil(S("hello").index(?z, 3))
    assert_nil(S("hello").index(S("z"), 3))
    assert_nil(S("hello").index(/z./, 3))

    assert_nil(S("hello").index(?z))
    assert_nil(S("hello").index(S("z")))
    assert_nil(S("hello").index(/z./))

    o = Object.new
    def o.to_str; "bar"; end
    assert_equal(3, "foobarbarbaz".index(o))
    assert_raise(TypeError) { "foo".index(Object.new) }

    assert_nil("foo".index(//, -100))
    assert_nil($~)
  end

  def test_intern
    assert_equal(:koala, S("koala").intern)
    assert(:koala !=     S("Koala").intern)
  end

  def test_length
    assert_equal(0, S("").length)
    assert_equal(4, S("1234").length)
    assert_equal(6, S("1234\r\n").length)
    assert_equal(7, S("\0011234\r\n").length)
  end

  def test_ljust
    assert_equal(S("hello"),       S("hello").ljust(4))
    assert_equal(S("hello      "), S("hello").ljust(11))
    assert_equal(S("ababababab"), S("").ljust(10, "ab"), Bug2463)
    assert_equal(S("abababababa"), S("").ljust(11, "ab"), Bug2463)
  end

  def test_next
    assert_equal(S("abd"), S("abc").next)
    assert_equal(S("z"),   S("y").next)
    assert_equal(S("aaa"), S("zz").next)

    assert_equal(S("124"),  S("123").next)
    assert_equal(S("1000"), S("999").next)

    assert_equal(S("2000aaa"),  S("1999zzz").next)
    assert_equal(S("AAAAA000"), S("ZZZZ999").next)

    assert_equal(S("*+"), S("**").next)
  end

  def test_next!
    a = S("abc")
    b = a.dup
    assert_equal(S("abd"), a.next!)
    assert_equal(S("abd"), a)
    assert_equal(S("abc"), b)

    a = S("y")
    assert_equal(S("z"), a.next!)
    assert_equal(S("z"), a)

    a = S("zz")
    assert_equal(S("aaa"), a.next!)
    assert_equal(S("aaa"), a)

    a = S("123")
    assert_equal(S("124"), a.next!)
    assert_equal(S("124"), a)

    a = S("999")
    assert_equal(S("1000"), a.next!)
    assert_equal(S("1000"), a)

    a = S("1999zzz")
    assert_equal(S("2000aaa"), a.next!)
    assert_equal(S("2000aaa"), a)

    a = S("ZZZZ999")
    assert_equal(S("AAAAA000"), a.next!)
    assert_equal(S("AAAAA000"), a)

    a = S("**")
    assert_equal(S("*+"), a.next!)
    assert_equal(S("*+"), a)
  end

  def test_oct
    assert_equal(255,  S("0377").oct)
    assert_equal(255,  S("377").oct)
    assert_equal(-255, S("-0377").oct)
    assert_equal(-255, S("-377").oct)
    assert_equal(0,    S("OO").oct)
    assert_equal(24,   S("030OO").oct)
  end

  def test_replace
    a = S("foo")
    assert_equal(S("f"), a.replace(S("f")))

    a = S("foo")
    assert_equal(S("foobar"), a.replace(S("foobar")))

    a = S("foo")
    a.taint
    a.untrust
    b = a.replace(S("xyz"))
    assert_equal(S("xyz"), b)
    assert(b.tainted?)
    assert(b.untrusted?)

    s = "foo" * 100
    s2 = ("bar" * 100).dup
    s.replace(s2)
    assert_equal(s2, s)

    s2 = ["foo"].pack("p")
    s.replace(s2)
    assert_equal(s2, s)

    fs = "".freeze
    assert_raise(RuntimeError) { fs.replace("a") }
    assert_raise(RuntimeError) { fs.replace(fs) }
    assert_raise(ArgumentError) { fs.replace() }
    assert_raise(RuntimeError) { fs.replace(42) }
  end

  def test_reverse
    assert_equal(S("beta"), S("ateb").reverse)
    assert_equal(S("madamImadam"), S("madamImadam").reverse)

    a=S("beta")
    assert_equal(S("ateb"), a.reverse)
    assert_equal(S("beta"), a)
  end

  def test_reverse!
    a = S("beta")
    b = a.dup
    assert_equal(S("ateb"), a.reverse!)
    assert_equal(S("ateb"), a)
    assert_equal(S("beta"), b)

    assert_equal(S("madamImadam"), S("madamImadam").reverse!)

    a = S("madamImadam")
    assert_equal(S("madamImadam"), a.reverse!)  # ??
    assert_equal(S("madamImadam"), a)
  end

  def test_rindex
    assert_equal(3, S("hello").rindex(?l))
    assert_equal(6, S("ell, hello").rindex(S("ell")))
    assert_equal(7, S("ell, hello").rindex(/ll./))

    assert_equal(3, S("hello,lo").rindex(?l, 3))
    assert_equal(3, S("hello,lo").rindex(S("l"), 3))
    assert_equal(3, S("hello,lo").rindex(/l./, 3))

    assert_nil(S("hello").rindex(?z,     3))
    assert_nil(S("hello").rindex(S("z"), 3))
    assert_nil(S("hello").rindex(/z./,   3))

    assert_nil(S("hello").rindex(?z))
    assert_nil(S("hello").rindex(S("z")))
    assert_nil(S("hello").rindex(/z./))

    o = Object.new
    def o.to_str; "bar"; end
    assert_equal(6, "foobarbarbaz".rindex(o))
    assert_raise(TypeError) { "foo".rindex(Object.new) }

    assert_nil("foo".rindex(//, -100))
    assert_nil($~)
  end

  def test_rjust
    assert_equal(S("hello"), S("hello").rjust(4))
    assert_equal(S("      hello"), S("hello").rjust(11))
    assert_equal(S("ababababab"), S("").rjust(10, "ab"), Bug2463)
    assert_equal(S("abababababa"), S("").rjust(11, "ab"), Bug2463)
  end

  def test_scan
    a = S("cruel world")
    assert_equal([S("cruel"), S("world")],a.scan(/\w+/))
    assert_equal([S("cru"), S("el "), S("wor")],a.scan(/.../))
    assert_equal([[S("cru")], [S("el ")], [S("wor")]],a.scan(/(...)/))

    res = []
    a.scan(/\w+/) { |w| res << w }
    assert_equal([S("cruel"), S("world") ],res)

    res = []
    a.scan(/.../) { |w| res << w }
    assert_equal([S("cru"), S("el "), S("wor")],res)

    res = []
    a.scan(/(...)/) { |w| res << w }
    assert_equal([[S("cru")], [S("el ")], [S("wor")]],res)

    a = S("hello")
    a.taint
    a.untrust
    res = []
    a.scan(/./) { |w| res << w }
    assert(res[0].tainted?, '[ruby-core:33338] #4087')
    assert(res[0].untrusted?, '[ruby-core:33338] #4087')
  end

  def test_size
    assert_equal(0, S("").size)
    assert_equal(4, S("1234").size)
    assert_equal(6, S("1234\r\n").size)
    assert_equal(7, S("\0011234\r\n").size)
  end

  def test_slice
    assert_equal(?A, S("AooBar").slice(0))
    assert_equal(?B, S("FooBaB").slice(-1))
    assert_nil(S("FooBar").slice(6))
    assert_nil(S("FooBar").slice(-7))

    assert_equal(S("Foo"), S("FooBar").slice(0,3))
    assert_equal(S(S("Bar")), S("FooBar").slice(-3,3))
    assert_nil(S("FooBar").slice(7,2))     # Maybe should be six?
    assert_nil(S("FooBar").slice(-7,10))

    assert_equal(S("Foo"), S("FooBar").slice(0..2))
    assert_equal(S("Bar"), S("FooBar").slice(-3..-1))
    assert_equal(S(""), S("FooBar").slice(6..2))
    assert_nil(S("FooBar").slice(-10..-7))

    assert_equal(S("Foo"), S("FooBar").slice(/^F../))
    assert_equal(S("Bar"), S("FooBar").slice(/..r$/))
    assert_nil(S("FooBar").slice(/xyzzy/))
    assert_nil(S("FooBar").slice(/plugh/))

    assert_equal(S("Foo"), S("FooBar").slice(S("Foo")))
    assert_equal(S("Bar"), S("FooBar").slice(S("Bar")))
    assert_nil(S("FooBar").slice(S("xyzzy")))
    assert_nil(S("FooBar").slice(S("plugh")))
  end

  def test_slice!
    a = S("AooBar")
    b = a.dup
    assert_equal(?A, a.slice!(0))
    assert_equal(S("ooBar"), a)
    assert_equal(S("AooBar"), b)

    a = S("FooBar")
    assert_equal(?r,a.slice!(-1))
    assert_equal(S("FooBa"), a)

    a = S("FooBar")
    if @aref_slicebang_silent
      assert_nil( a.slice!(6) )
    else
      assert_raise(IndexError) { a.slice!(6) }
    end
    assert_equal(S("FooBar"), a)

    if @aref_slicebang_silent
      assert_nil( a.slice!(-7) )
    else
      assert_raise(IndexError) { a.slice!(-7) }
    end
    assert_equal(S("FooBar"), a)

    a = S("FooBar")
    assert_equal(S("Foo"), a.slice!(0,3))
    assert_equal(S("Bar"), a)

    a = S("FooBar")
    assert_equal(S("Bar"), a.slice!(-3,3))
    assert_equal(S("Foo"), a)

    a=S("FooBar")
    if @aref_slicebang_silent
    assert_nil(a.slice!(7,2))      # Maybe should be six?
    else
    assert_raise(IndexError) {a.slice!(7,2)}     # Maybe should be six?
    end
    assert_equal(S("FooBar"), a)
    if @aref_slicebang_silent
    assert_nil(a.slice!(-7,10))
    else
    assert_raise(IndexError) {a.slice!(-7,10)}
    end
    assert_equal(S("FooBar"), a)

    a=S("FooBar")
    assert_equal(S("Foo"), a.slice!(0..2))
    assert_equal(S("Bar"), a)

    a=S("FooBar")
    assert_equal(S("Bar"), a.slice!(-3..-1))
    assert_equal(S("Foo"), a)

    a=S("FooBar")
    if @aref_slicebang_silent
    assert_equal(S(""), a.slice!(6..2))
    else
    assert_raise(RangeError) {a.slice!(6..2)}
    end
    assert_equal(S("FooBar"), a)
    if @aref_slicebang_silent
    assert_nil(a.slice!(-10..-7))
    else
    assert_raise(RangeError) {a.slice!(-10..-7)}
    end
    assert_equal(S("FooBar"), a)

    a=S("FooBar")
    assert_equal(S("Foo"), a.slice!(/^F../))
    assert_equal(S("Bar"), a)

    a=S("FooBar")
    assert_equal(S("Bar"), a.slice!(/..r$/))
    assert_equal(S("Foo"), a)

    a=S("FooBar")
    if @aref_slicebang_silent
      assert_nil(a.slice!(/xyzzy/))
    else
      assert_raise(IndexError) {a.slice!(/xyzzy/)}
    end
    assert_equal(S("FooBar"), a)
    if @aref_slicebang_silent
      assert_nil(a.slice!(/plugh/))
    else
      assert_raise(IndexError) {a.slice!(/plugh/)}
    end
    assert_equal(S("FooBar"), a)

    a=S("FooBar")
    assert_equal(S("Foo"), a.slice!(S("Foo")))
    assert_equal(S("Bar"), a)

    a=S("FooBar")
    assert_equal(S("Bar"), a.slice!(S("Bar")))
    assert_equal(S("Foo"), a)

    pre_1_7_1 do
      a=S("FooBar")
      assert_nil(a.slice!(S("xyzzy")))
      assert_equal(S("FooBar"), a)
      assert_nil(a.slice!(S("plugh")))
      assert_equal(S("FooBar"), a)
    end

    assert_raise(ArgumentError) { "foo".slice! }
  end

  def test_split
    assert_nil($;)
    assert_equal([S("a"), S("b"), S("c")], S(" a   b\t c ").split)
    assert_equal([S("a"), S("b"), S("c")], S(" a   b\t c ").split(S(" ")))

    assert_equal([S(" a "), S(" b "), S(" c ")], S(" a | b | c ").split(S("|")))

    assert_equal([S("a"), S("b"), S("c")], S("aXXbXXcXX").split(/X./))

    assert_equal([S("a"), S("b"), S("c")], S("abc").split(//))

    assert_equal([S("a|b|c")], S("a|b|c").split(S('|'), 1))

    assert_equal([S("a"), S("b|c")], S("a|b|c").split(S('|'), 2))
    assert_equal([S("a"), S("b"), S("c")], S("a|b|c").split(S('|'), 3))

    assert_equal([S("a"), S("b"), S("c"), S("")], S("a|b|c|").split(S('|'), -1))
    assert_equal([S("a"), S("b"), S("c"), S(""), S("")], S("a|b|c||").split(S('|'), -1))

    assert_equal([S("a"), S(""), S("b"), S("c")], S("a||b|c|").split(S('|')))
    assert_equal([S("a"), S(""), S("b"), S("c"), S("")], S("a||b|c|").split(S('|'), -1))

    assert_equal([], "".split(//, 1))

    assert_equal("[2, 3]", [1,2,3].slice!(1,10000).inspect, "moved from btest/knownbug")

  end

  def test_squeeze
    assert_equal(S("abc"), S("aaabbbbccc").squeeze)
    assert_equal(S("aa bb cc"), S("aa   bb      cc").squeeze(S(" ")))
    assert_equal(S("BxTyWz"), S("BxxxTyyyWzzzzz").squeeze(S("a-z")))
  end

  def test_squeeze!
    a = S("aaabbbbccc")
    b = a.dup
    assert_equal(S("abc"), a.squeeze!)
    assert_equal(S("abc"), a)
    assert_equal(S("aaabbbbccc"), b)

    a = S("aa   bb      cc")
    assert_equal(S("aa bb cc"), a.squeeze!(S(" ")))
    assert_equal(S("aa bb cc"), a)

    a = S("BxxxTyyyWzzzzz")
    assert_equal(S("BxTyWz"), a.squeeze!(S("a-z")))
    assert_equal(S("BxTyWz"), a)

    a=S("The quick brown fox")
    assert_nil(a.squeeze!)
  end

  def test_strip
    assert_equal(S("x"), S("      x        ").strip)
    assert_equal(S("x"), S(" \n\r\t     x  \t\r\n\n      ").strip)

    assert_equal("0b0 ".force_encoding("UTF-16BE"),
                 "\x00 0b0 ".force_encoding("UTF-16BE").strip)
    assert_equal("0\x000b0 ".force_encoding("UTF-16BE"),
                 "0\x000b0 ".force_encoding("UTF-16BE").strip)
  end

  def test_strip!
    a = S("      x        ")
    b = a.dup
    assert_equal(S("x") ,a.strip!)
    assert_equal(S("x") ,a)
    assert_equal(S("      x        "), b)

    a = S(" \n\r\t     x  \t\r\n\n      ")
    assert_equal(S("x"), a.strip!)
    assert_equal(S("x"), a)

    a = S("x")
    assert_nil(a.strip!)
    assert_equal(S("x") ,a)
  end

  def test_sub
    assert_equal(S("h*llo"),    S("hello").sub(/[aeiou]/, S('*')))
    assert_equal(S("h<e>llo"),  S("hello").sub(/([aeiou])/, S('<\1>')))
    assert_equal(S("h ello"), S("hello").sub(/./) {
                   |s| s[0].to_s + S(' ')})
    assert_equal(S("HELL-o"),   S("hello").sub(/(hell)(.)/) {
                   |s| $1.upcase + S('-') + $2
                   })

    assert_equal(S("a\\aba"), S("ababa").sub(/b/, '\\'))
    assert_equal(S("ab\\aba"), S("ababa").sub(/(b)/, '\1\\'))
    assert_equal(S("ababa"), S("ababa").sub(/(b)/, '\1'))
    assert_equal(S("ababa"), S("ababa").sub(/(b)/, '\\1'))
    assert_equal(S("a\\1aba"), S("ababa").sub(/(b)/, '\\\1'))
    assert_equal(S("a\\1aba"), S("ababa").sub(/(b)/, '\\\\1'))
    assert_equal(S("a\\baba"), S("ababa").sub(/(b)/, '\\\\\1'))

    assert_equal(S("a--ababababababababab"),
		 S("abababababababababab").sub(/(b)/, '-\9-'))
    assert_equal(S("1-b-0"),
		 S("1b2b3b4b5b6b7b8b9b0").
		 sub(/(b).(b).(b).(b).(b).(b).(b).(b).(b)/, '-\9-'))
    assert_equal(S("1-b-0"),
		 S("1b2b3b4b5b6b7b8b9b0").
		 sub(/(b).(b).(b).(b).(b).(b).(b).(b).(b)/, '-\\9-'))
    assert_equal(S("1-\\9-0"),
		 S("1b2b3b4b5b6b7b8b9b0").
		 sub(/(b).(b).(b).(b).(b).(b).(b).(b).(b)/, '-\\\9-'))
    assert_equal(S("k"),
		 S("1a2b3c4d5e6f7g8h9iAjBk").
		 sub(/.(.).(.).(.).(.).(.).(.).(.).(.).(.).(.).(.)/, '\+'))

    assert_equal(S("ab\\aba"), S("ababa").sub(/b/, '\&\\'))
    assert_equal(S("ababa"), S("ababa").sub(/b/, '\&'))
    assert_equal(S("ababa"), S("ababa").sub(/b/, '\\&'))
    assert_equal(S("a\\&aba"), S("ababa").sub(/b/, '\\\&'))
    assert_equal(S("a\\&aba"), S("ababa").sub(/b/, '\\\\&'))
    assert_equal(S("a\\baba"), S("ababa").sub(/b/, '\\\\\&'))

    a = S("hello")
    a.taint
    a.untrust
    x = a.sub(/./, S('X'))
    assert(x.tainted?)
    assert(x.untrusted?)

    o = Object.new
    def o.to_str; "bar"; end
    assert_equal("fooBARbaz", "foobarbaz".sub(o, "BAR"))

    assert_raise(TypeError) { "foo".sub(Object.new, "") }

    assert_raise(ArgumentError) { "foo".sub }

    assert_raise(IndexError) { "foo"[/(?:(o$)|(x))/, 2] = 'bar' }

    o = Object.new
    def o.to_s; self; end
    assert_match(/^foo#<Object:0x.*>baz$/, "foobarbaz".sub("bar") { o })
  end

  def test_sub!
    a = S("hello")
    b = a.dup
    a.sub!(/[aeiou]/, S('*'))
    assert_equal(S("h*llo"), a)
    assert_equal(S("hello"), b)

    a = S("hello")
    a.sub!(/([aeiou])/, S('<\1>'))
    assert_equal(S("h<e>llo"), a)

    a = S("hello")
    a.sub!(/./) { |s| s[0].to_s + S(' ')}
    assert_equal(S("h ello"), a)

    a = S("hello")
    a.sub!(/(hell)(.)/) { |s| $1.upcase + S('-') + $2 }
    assert_equal(S("HELL-o"), a)

    a=S("hello")
    assert_nil(a.sub!(/X/, S('Y')))

    r = S('X')
    r.taint
    r.untrust
    a.sub!(/./, r)
    assert(a.tainted?)
    assert(a.untrusted?)
  end

  def test_succ
    assert_equal(S("abd"), S("abc").succ)
    assert_equal(S("z"),   S("y").succ)
    assert_equal(S("aaa"), S("zz").succ)

    assert_equal(S("124"),  S("123").succ)
    assert_equal(S("1000"), S("999").succ)
    assert_equal(S("2.000"), S("1.999").succ)

    assert_equal(S("No.10"), S("No.9").succ)
    assert_equal(S("2000aaa"),  S("1999zzz").succ)
    assert_equal(S("AAAAA000"), S("ZZZZ999").succ)
    assert_equal(S("*+"), S("**").succ)

    assert_equal("abce", "abcd".succ)
    assert_equal("THX1139", "THX1138".succ)
    assert_equal("<<koalb>>", "<<koala>>".succ)
    assert_equal("2000aaa", "1999zzz".succ)
    assert_equal("AAAA0000", "ZZZ9999".succ)
    assert_equal("**+", "***".succ)
  end

  def test_succ!
    a = S("abc")
    b = a.dup
    assert_equal(S("abd"), a.succ!)
    assert_equal(S("abd"), a)
    assert_equal(S("abc"), b)

    a = S("y")
    assert_equal(S("z"), a.succ!)
    assert_equal(S("z"), a)

    a = S("zz")
    assert_equal(S("aaa"), a.succ!)
    assert_equal(S("aaa"), a)

    a = S("123")
    assert_equal(S("124"), a.succ!)
    assert_equal(S("124"), a)

    a = S("999")
    assert_equal(S("1000"), a.succ!)
    assert_equal(S("1000"), a)

    a = S("1999zzz")
    assert_equal(S("2000aaa"), a.succ!)
    assert_equal(S("2000aaa"), a)

    a = S("ZZZZ999")
    assert_equal(S("AAAAA000"), a.succ!)
    assert_equal(S("AAAAA000"), a)

    a = S("**")
    assert_equal(S("*+"), a.succ!)
    assert_equal(S("*+"), a)

    a = S("No.9")
    assert_equal(S("No.10"), a.succ!)
    assert_equal(S("No.10"), a)

    assert_equal("aaaaaaaaaaaa", "zzzzzzzzzzz".succ!)
    assert_equal("aaaaaaaaaaaaaaaaaaaaaaaa", "zzzzzzzzzzzzzzzzzzzzzzz".succ!)
  end

  def test_sum
    n = S("\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001")
    assert_equal(15, n.sum)
    n += S("\001")
    assert_equal(16, n.sum(17))
    n[0] = 2.chr
    assert(15 != n.sum)
  end

  def check_sum(str, bits=16)
    sum = 0
    str.each_byte {|c| sum += c}
    sum = sum & ((1 << bits) - 1) if bits != 0
    assert_equal(sum, str.sum(bits))
  end

  def test_sum_2
    assert_equal(0, "".sum)
    assert_equal(294, "abc".sum)
    check_sum("abc")
    check_sum("\x80")
    0.upto(70) {|bits|
      check_sum("xyz", bits)
    }
  end

  def test_sum_long
    s8421505 = "\xff" * 8421505
    assert_equal(127, s8421505.sum(31))
    assert_equal(2147483775, s8421505.sum(0))
    s16843010 = ("\xff" * 16843010)
    assert_equal(254, s16843010.sum(32))
    assert_equal(4294967550, s16843010.sum(0))
  end

  def test_swapcase
    assert_equal(S("hi&LOW"), S("HI&low").swapcase)
  end

  def test_swapcase!
    a = S("hi&LOW")
    b = a.dup
    assert_equal(S("HI&low"), a.swapcase!)
    assert_equal(S("HI&low"), a)
    assert_equal(S("hi&LOW"), b)

    a = S("$^#^%$#!!")
    assert_nil(a.swapcase!)
    assert_equal(S("$^#^%$#!!"), a)
  end

  def test_to_f
    assert_equal(344.3,     S("344.3").to_f)
    assert_equal(5.9742e24, S("5.9742e24").to_f)
    assert_equal(98.6,      S("98.6 degrees").to_f)
    assert_equal(0.0,       S("degrees 100.0").to_f)
    assert_equal([ 0.0].pack('G'), [S(" 0.0").to_f].pack('G'))
    assert_equal([-0.0].pack('G'), [S("-0.0").to_f].pack('G'))
  end

  def test_to_i
    assert_equal(1480, S("1480ft/sec").to_i)
    assert_equal(0,    S("speed of sound in water @20C = 1480ft/sec)").to_i)
    assert_equal(0, " 0".to_i)
    assert_equal(0, "+0".to_i)
    assert_equal(0, "-0".to_i)
    assert_equal(0, "--0".to_i)
    assert_equal(16, "0x10".to_i(0))
    assert_equal(16, "0X10".to_i(0))
    assert_equal(2, "0b10".to_i(0))
    assert_equal(2, "0B10".to_i(0))
    assert_equal(8, "0o10".to_i(0))
    assert_equal(8, "0O10".to_i(0))
    assert_equal(10, "0d10".to_i(0))
    assert_equal(10, "0D10".to_i(0))
    assert_equal(8, "010".to_i(0))
    assert_raise(ArgumentError) { "010".to_i(-10) }
    2.upto(36) {|radix|
      assert_equal(radix, "10".to_i(radix))
      assert_equal(radix**2, "100".to_i(radix))
    }
    assert_raise(ArgumentError) { "0".to_i(1) }
    assert_raise(ArgumentError) { "0".to_i(37) }
    assert_equal(0, "z".to_i(10))
    assert_equal(12, "1_2".to_i(10))
    assert_equal(0x40000000, "1073741824".to_i(10))
    assert_equal(0x4000000000000000, "4611686018427387904".to_i(10))
    assert_equal(1, "1__2".to_i(10))
    assert_equal(1, "1_z".to_i(10))
  end

  def test_to_s
    a = S("me")
    assert_equal("me", a.to_s)
    assert_equal(a.__id__, a.to_s.__id__) if @cls == String
  end

  def test_to_str
    a = S("me")
    assert_equal("me", a.to_s)
    assert_equal(a.__id__, a.to_s.__id__) if @cls == String

    o = Object.new
    def o.to_str
      "at"
    end
    assert_equal("meat", a.concat(o))

    o = Object.new
    def o.to_str
      foo_bar()
    end
    assert_match(/foo_bar/, assert_raise(NoMethodError) {a.concat(o)}.message)
  end

  def test_tr
    assert_equal(S("hippo"), S("hello").tr(S("el"), S("ip")))
    assert_equal(S("*e**o"), S("hello").tr(S("^aeiou"), S("*")))
    assert_equal(S("hal"),   S("ibm").tr(S("b-z"), S("a-z")))

    a = "abc".force_encoding(Encoding::US_ASCII)
    assert_equal(Encoding::US_ASCII, a.tr(S("z"), S("\u0101")).encoding, '[ruby-core:22326]')

    assert_equal("a".hash, "a".tr("a", "\u0101").tr("\u0101", "a").hash, '[ruby-core:22328]')
    assert_equal(true, "\u0101".tr("\u0101", "a").ascii_only?)
    assert_equal(true, "\u3041".tr("\u3041", "a").ascii_only?)
    assert_equal(false, "\u3041\u3042".tr("\u3041", "a").ascii_only?)

    bug6156 = '[ruby-core:43335]'
    str, range, star = %w[b a-z *].map{|s|s.encode("utf-16le")}
    assert_equal(star, str.tr(range, star), bug6156)
  end

  def test_tr!
    a = S("hello")
    b = a.dup
    assert_equal(S("hippo"), a.tr!(S("el"), S("ip")))
    assert_equal(S("hippo"), a)
    assert_equal(S("hello"),b)

    a = S("hello")
    assert_equal(S("*e**o"), a.tr!(S("^aeiou"), S("*")))
    assert_equal(S("*e**o"), a)

    a = S("IBM")
    assert_equal(S("HAL"), a.tr!(S("B-Z"), S("A-Z")))
    assert_equal(S("HAL"), a)

    a = S("ibm")
    assert_nil(a.tr!(S("B-Z"), S("A-Z")))
    assert_equal(S("ibm"), a)

    a = "abc".force_encoding(Encoding::US_ASCII)
    assert_nil(a.tr!(S("z"), S("\u0101")), '[ruby-core:22326]')
    assert_equal(Encoding::US_ASCII, a.encoding, '[ruby-core:22326]')
  end

  def test_tr_s
    assert_equal(S("hypo"), S("hello").tr_s(S("el"), S("yp")))
    assert_equal(S("h*o"),  S("hello").tr_s(S("el"), S("*")))
    assert_equal("a".hash, "\u0101\u0101".tr_s("\u0101", "a").hash)
    assert_equal(true, "\u3041\u3041".tr("\u3041", "a").ascii_only?)
  end

  def test_tr_s!
    a = S("hello")
    b = a.dup
    assert_equal(S("hypo"),  a.tr_s!(S("el"), S("yp")))
    assert_equal(S("hypo"),  a)
    assert_equal(S("hello"), b)

    a = S("hello")
    assert_equal(S("h*o"), a.tr_s!(S("el"), S("*")))
    assert_equal(S("h*o"), a)
  end

  def test_unpack
    a = [S("cat"),  S("wom"), S("x"), S("yy")]
    assert_equal(a, S("catwomx  yy ").unpack(S("A3A3A3A3")))

    assert_equal([S("cat")], S("cat  \000\000").unpack(S("A*")))
    assert_equal([S("cwx"), S("wx"), S("x"), S("yy")],
                   S("cwx  yy ").unpack(S("A3@1A3@2A3A3")))
    assert_equal([S("cat"), S("wom"), S("x\000\000"), S("yy\000")],
                  S("catwomx\000\000yy\000").unpack(S("a3a3a3a3")))
    assert_equal([S("cat \000\000")], S("cat \000\000").unpack(S("a*")))
    assert_equal([S("ca")], S("catdog").unpack(S("a2")))

    assert_equal([S("cat\000\000")],
                  S("cat\000\000\000\000\000dog").unpack(S("a5")))

    assert_equal([S("01100001")], S("\x61").unpack(S("B8")))
    assert_equal([S("01100001")], S("\x61").unpack(S("B*")))
    assert_equal([S("0110000100110111")], S("\x61\x37").unpack(S("B16")))
    assert_equal([S("01100001"), S("00110111")], S("\x61\x37").unpack(S("B8B8")))
    assert_equal([S("0110")], S("\x60").unpack(S("B4")))

    assert_equal([S("01")], S("\x40").unpack(S("B2")))

    assert_equal([S("01100001")], S("\x86").unpack(S("b8")))
    assert_equal([S("01100001")], S("\x86").unpack(S("b*")))

    assert_equal([S("0110000100110111")], S("\x86\xec").unpack(S("b16")))
    assert_equal([S("01100001"), S("00110111")], S("\x86\xec").unpack(S("b8b8")))

    assert_equal([S("0110")], S("\x06").unpack(S("b4")))
    assert_equal([S("01")], S("\x02").unpack(S("b2")))

    assert_equal([ 65, 66, 67 ],  S("ABC").unpack(S("C3")))
    assert_equal([ 255, 66, 67 ], S("\377BC").unpack("C*"))
    assert_equal([ 65, 66, 67 ],  S("ABC").unpack("c3"))
    assert_equal([ -1, 66, 67 ],  S("\377BC").unpack("c*"))


    assert_equal([S("4142"), S("0a"), S("1")], S("AB\n\x10").unpack(S("H4H2H1")))
    assert_equal([S("1424"), S("a0"), S("2")], S("AB\n\x02").unpack(S("h4h2h1")))

    assert_equal([S("abc\002defcat\001"), S(""), S("")],
                 S("abc=02def=\ncat=\n=01=\n").unpack(S("M9M3M4")))

    assert_equal([S("hello\n")], S("aGVsbG8K\n").unpack(S("m")))

    assert_equal([S("hello\nhello\n")], S(",:&5L;&\\*:&5L;&\\*\n").unpack(S("u")))

    assert_equal([0xa9, 0x42, 0x2260], S("\xc2\xa9B\xe2\x89\xa0").unpack(S("U*")))

=begin
    skipping "Not tested:
        D,d & double-precision float, native format\\
        E & double-precision float, little-endian byte order\\
        e & single-precision float, little-endian byte order\\
        F,f & single-precision float, native format\\
        G & double-precision float, network (big-endian) byte order\\
        g & single-precision float, network (big-endian) byte order\\
        I & unsigned integer\\
        i & integer\\
        L & unsigned long\\
        l & long\\

        m & string encoded in base64 (uuencoded)\\
        N & long, network (big-endian) byte order\\
        n & short, network (big-endian) byte-order\\
        P & pointer to a structure (fixed-length string)\\
        p & pointer to a null-terminated string\\
        S & unsigned short\\
        s & short\\
        V & long, little-endian byte order\\
        v & short, little-endian byte order\\
        X & back up a byte\\
        x & null byte\\
        Z & ASCII string (null padded, count is width)\\
"
=end
  end

  def test_upcase
    assert_equal(S("HELLO"), S("hello").upcase)
    assert_equal(S("HELLO"), S("hello").upcase)
    assert_equal(S("HELLO"), S("HELLO").upcase)
    assert_equal(S("ABC HELLO 123"), S("abc HELLO 123").upcase)
  end

  def test_upcase!
    a = S("hello")
    b = a.dup
    assert_equal(S("HELLO"), a.upcase!)
    assert_equal(S("HELLO"), a)
    assert_equal(S("hello"), b)

    a = S("HELLO")
    assert_nil(a.upcase!)
    assert_equal(S("HELLO"), a)
  end

  def test_upto
    a     = S("aa")
    start = S("aa")
    count = 0
    assert_equal(S("aa"), a.upto(S("zz")) {|s|
                   assert_equal(start, s)
                   start.succ!
                   count += 1
                   })
    assert_equal(676, count)
  end

  def test_upto_numeric
    a     = S("00")
    start = S("00")
    count = 0
    assert_equal(S("00"), a.upto(S("23")) {|s|
                   assert_equal(start, s, "[ruby-dev:39361]")
                   assert_equal(Encoding::US_ASCII, s.encoding)
                   start.succ!
                   count += 1
                   })
    assert_equal(24, count, "[ruby-dev:39361]")
  end

  def test_upto_nonalnum
    first = S("\u3041")
    last  = S("\u3093")
    count = 0
    assert_equal(first, first.upto(last) {|s|
                   count += 1
                   s.replace(last)
                   })
    assert_equal(83, count, "[ruby-dev:39626]")
  end

  def test_mod_check
    assert_raise(RuntimeError) {
      s = ""
      s.sub!(/\A/) { s.replace "z" * 2000; "zzz" }
    }
  end

  def test_frozen_check
    assert_raise(RuntimeError) {
      s = ""
      s.sub!(/\A/) { s.freeze; "zzz" }
    }
  end

  class S2 < String
  end
  def test_str_new4
    s = (0..54).to_a.join # length = 100
    s2 = S2.new(s[10,90])
    s3 = s2[10,80]
    assert_equal((10..54).to_a.to_a.join, s2)
    assert_equal((15..54).to_a.to_a.join, s3)
  end

  def test_rb_str_new4
    s = "a" * 100
    s2 = s[10,90]
    assert_equal("a" * 90, s2)
    s3 = s2[10,80]
    assert_equal("a" * 80, s3)
  end

  class StringLike
    def initialize(str)
      @str = str
    end

    def to_str
      @str
    end
  end

  def test_rb_str_to_str
    assert_equal("ab", "a" + StringLike.new("b"))
  end

  def test_rb_str_shared_replace
    s = "a" * 100
    s.succ!
    assert_equal("a" * 99 + "b", s)
    s = ""
    s.succ!
    assert_equal("", s)
  end

  def test_times
    assert_raise(ArgumentError) { "a" * (-1) }
  end

  def test_splice!
    l = S("1234\n234\n34\n4\n")
    assert_equal(S("1234\n"), l.slice!(/\A.*\n/), "[ruby-dev:31665]")
    assert_equal(S("234\n"), l.slice!(/\A.*\n/), "[ruby-dev:31665]")
    assert_equal(S("34\n"), l.slice!(/\A.*\n/), "[ruby-dev:31665]")
    assert_equal(S("4\n"), l.slice!(/\A.*\n/), "[ruby-dev:31665]")
    assert_nil(l.slice!(/\A.*\n/), "[ruby-dev:31665]")
  end

  def test_end_with?
    assert("abc".end_with?("c"))
  end

  def test_times2
    s1 = ''
    100.times {|n|
      s2 = "a" * n
      assert_equal(s1, s2)
      s1 << 'a'
    }

    assert_raise(ArgumentError) { "foo" * (-1) }
  end

  def test_respond_to
    o = Object.new
    def o.respond_to?(arg) [:to_str].include?(arg) ? nil : super end
    def o.to_str() "" end
    def o.==(other) "" == other end
    assert_equal(false, "" == o)
  end

  def test_match_method
    assert_equal("bar", "foobarbaz".match(/bar/).to_s)

    o = /foo/
    def o.match(x, y, z); x + y + z; end
    assert_equal("foobarbaz", "foo".match(o, "bar", "baz"))
    x = nil
    "foo".match(o, "bar", "baz") {|y| x = y }
    assert_equal("foobarbaz", x)

    assert_raise(ArgumentError) { "foo".match }
  end

  def test_clear
    s = "foo" * 100
    s.clear
    assert_equal("", s)
  end

  def test_to_s_2
    c = Class.new(String)
    s = c.new
    s.replace("foo")
    assert_equal("foo", s.to_s)
    assert_instance_of(String, s.to_s)
  end

  def test_partition
    assert_equal(%w(he l lo), "hello".partition(/l/))
    assert_equal(%w(he l lo), "hello".partition("l"))
    assert_raise(TypeError) { "hello".partition(1) }
    def (hyphen = Object.new).to_str; "-"; end
    assert_equal(%w(foo - bar), "foo-bar".partition(hyphen), '[ruby-core:23540]')
  end

  def test_rpartition
    assert_equal(%w(hel l o), "hello".rpartition(/l/))
    assert_equal(%w(hel l o), "hello".rpartition("l"))
    assert_raise(TypeError) { "hello".rpartition(1) }
    def (hyphen = Object.new).to_str; "-"; end
    assert_equal(%w(foo - bar), "foo-bar".rpartition(hyphen), '[ruby-core:23540]')
  end

  def test_setter
    assert_raise(TypeError) { $/ = 1 }
  end

  def test_to_id
    c = Class.new
    c.class_eval do
      def initialize
        @foo = :foo
      end
    end

    assert_raise(TypeError) do
      c.class_eval { attr 1 }
    end

    o = Object.new
    def o.to_str; :foo; end
    assert_raise(TypeError) do
      c.class_eval { attr 1 }
    end

    class << o;remove_method :to_str;end
    def o.to_str; "foo"; end
    assert_nothing_raised do
      c.class_eval { attr o }
    end
    assert_equal(:foo, c.new.foo)
  end

  def test_gsub_enumerator
    assert_normal_exit %q{"abc".gsub(/./).next}, "[ruby-dev:34828]"
  end

  def test_clear_nonasciicompat
    assert_equal("", "\u3042".encode("ISO-2022-JP").clear)
  end

  def test_try_convert
    assert_equal(nil, String.try_convert(1))
    assert_equal("foo", String.try_convert("foo"))
  end

  def test_substr_negative_begin
    assert_equal("\u3042", ("\u3042" * 100)[-1])
  end

=begin
  def test_compare_different_encoding_string
    s1 = "\xff".force_encoding("UTF-8")
    s2 = "\xff".force_encoding("ISO-2022-JP")
    assert_equal([-1, 1], [s1 <=> s2, s2 <=> s1].sort)
  end
=end

  def test_casecmp
    assert_equal(1, "\u3042B".casecmp("\u3042a"))
  end

  def test_upcase2
    assert_equal("\u3042AB", "\u3042aB".upcase)
  end

  def test_downcase2
    assert_equal("\u3042ab", "\u3042aB".downcase)
  end

  def test_rstrip
    assert_equal("\u3042", "\u3042   ".rstrip)
    assert_raise(Encoding::CompatibilityError) { "\u3042".encode("ISO-2022-JP").rstrip }
  end

=begin
  def test_symbol_table_overflow
    assert_in_out_err([], <<-INPUT, [], /symbol table overflow \(symbol [a-z]{8}\) \(RuntimeError\)/)
      ("aaaaaaaa".."zzzzzzzz").each {|s| s.to_sym }
    INPUT
  end
=end

  def test_shared_force_encoding
    s = "\u{3066}\u{3059}\u{3068}".gsub(//, '')
    h = {}
    h[s] = nil
    k = h.keys[0]
    assert_equal(s, k, '[ruby-dev:39068]')
    assert_equal(Encoding::UTF_8, k.encoding, '[ruby-dev:39068]')
    s.dup.force_encoding(Encoding::ASCII_8BIT).gsub(//, '')
    k = h.keys[0]
    assert_equal(s, k, '[ruby-dev:39068]')
    assert_equal(Encoding::UTF_8, k.encoding, '[ruby-dev:39068]')
  end

  def test_ascii_incomat_inspect
    bug4081 = '[ruby-core:33283]'
    [Encoding::UTF_16LE, Encoding::UTF_16BE,
     Encoding::UTF_32LE, Encoding::UTF_32BE].each do |e|
      assert_equal('"abc"', "abc".encode(e).inspect)
      assert_equal('"\\u3042\\u3044\\u3046"', "\u3042\u3044\u3046".encode(e).inspect)
      assert_equal('"ab\\"c"', "ab\"c".encode(e).inspect, bug4081)
    end
    begin
      ext = Encoding.default_external
      Encoding.default_external = "us-ascii"
      i = "abc\"\\".force_encoding("utf-8").inspect
    ensure
      Encoding.default_external = ext
    end
    assert_equal('"abc\\"\\\\"', i, bug4081)
  end

  def test_dummy_inspect
    assert_equal('"\e\x24\x42\x22\x4C\x22\x68\e\x28\x42"',
                 "\u{ffe2}\u{2235}".encode("cp50220").inspect)
  end

  def test_prepend
    assert_equal(S("hello world!"), "world!".prepend("hello "))

    foo = Object.new
    def foo.to_str
      "b"
    end
    assert_equal(S("ba"), "a".prepend(foo))

    a = S("world")
    b = S("hello ")
    a.prepend(b)
    assert_equal(S("hello world"), a)
    assert_equal(S("hello "), b)
  end

  def u(str)
    str.force_encoding(Encoding::UTF_8)
  end

  def test_byteslice
    assert_equal("h", "hello".byteslice(0))
    assert_equal(nil, "hello".byteslice(5))
    assert_equal("o", "hello".byteslice(-1))
    assert_equal(nil, "hello".byteslice(-6))

    assert_equal("", "hello".byteslice(0, 0))
    assert_equal("hello", "hello".byteslice(0, 6))
    assert_equal("hello", "hello".byteslice(0, 6))
    assert_equal("", "hello".byteslice(5, 1))
    assert_equal("o", "hello".byteslice(-1, 6))
    assert_equal(nil, "hello".byteslice(-6, 1))
    assert_equal(nil, "hello".byteslice(0, -1))

    assert_equal("h", "hello".byteslice(0..0))
    assert_equal("", "hello".byteslice(5..0))
    assert_equal("o", "hello".byteslice(4..5))
    assert_equal(nil, "hello".byteslice(6..0))
    assert_equal("", "hello".byteslice(-1..0))
    assert_equal("llo", "hello".byteslice(-3..5))

    assert_equal(u("\x81"), "\u3042".byteslice(1))
    assert_equal(u("\x81\x82"), "\u3042".byteslice(1, 2))
    assert_equal(u("\x81\x82"), "\u3042".byteslice(1..2))

    assert_equal(u("\x82")+("\u3042"*9), ("\u3042"*10).byteslice(2, 28))
  end
end

class TestString2 < TestString
  def initialize(*args)
    super
    @cls = S2
  end
end
