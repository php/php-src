require 'test/unit'

class TestPack < Test::Unit::TestCase
  def test_pack
    $format = "c2x5CCxsdils_l_a6";
    # Need the expression in here to force ary[5] to be numeric.  This avoids
    # test2 failing because ary2 goes str->numeric->str and ary does not.
    ary = [1,-100,127,128,32767,987.654321098 / 100.0,12345,123456,-32767,-123456,"abcdef"]
    $x = ary.pack($format)
    ary2 = $x.unpack($format)

    assert_equal(ary.length, ary2.length)
    assert_equal(ary.join(':'), ary2.join(':'))
    assert_match(/def/, $x)

    $x = [-1073741825]
    assert_equal($x, $x.pack("q").unpack("q"))

    $x = [-1]
    assert_equal($x, $x.pack("l").unpack("l"))
  end

  def test_pack_n
    assert_equal "\000\000", [0].pack('n')
    assert_equal "\000\001", [1].pack('n')
    assert_equal "\000\002", [2].pack('n')
    assert_equal "\000\003", [3].pack('n')
    assert_equal "\377\376", [65534].pack('n')
    assert_equal "\377\377", [65535].pack('n')

    assert_equal "\200\000", [2**15].pack('n')
    assert_equal "\177\377", [-2**15-1].pack('n')
    assert_equal "\377\377", [-1].pack('n')

    assert_equal "\000\001\000\001", [1,1].pack('n*')
    assert_equal "\000\001\000\001\000\001", [1,1,1].pack('n*')
  end

  def test_unpack_n
    assert_equal 1, "\000\001".unpack('n')[0]
    assert_equal 2, "\000\002".unpack('n')[0]
    assert_equal 3, "\000\003".unpack('n')[0]
    assert_equal 65535, "\377\377".unpack('n')[0]
    assert_equal [1,1], "\000\001\000\001".unpack('n*')
    assert_equal [1,1,1], "\000\001\000\001\000\001".unpack('n*')
  end

  def test_pack_N
    assert_equal "\000\000\000\000", [0].pack('N')
    assert_equal "\000\000\000\001", [1].pack('N')
    assert_equal "\000\000\000\002", [2].pack('N')
    assert_equal "\000\000\000\003", [3].pack('N')
    assert_equal "\377\377\377\376", [4294967294].pack('N')
    assert_equal "\377\377\377\377", [4294967295].pack('N')

    assert_equal "\200\000\000\000", [2**31].pack('N')
    assert_equal "\177\377\377\377", [-2**31-1].pack('N')
    assert_equal "\377\377\377\377", [-1].pack('N')

    assert_equal "\000\000\000\001\000\000\000\001", [1,1].pack('N*')
    assert_equal "\000\000\000\001\000\000\000\001\000\000\000\001", [1,1,1].pack('N*')
  end

  def test_unpack_N
    assert_equal 1, "\000\000\000\001".unpack('N')[0]
    assert_equal 2, "\000\000\000\002".unpack('N')[0]
    assert_equal 3, "\000\000\000\003".unpack('N')[0]
    assert_equal 4294967295, "\377\377\377\377".unpack('N')[0]
    assert_equal [1,1], "\000\000\000\001\000\000\000\001".unpack('N*')
    assert_equal [1,1,1], "\000\000\000\001\000\000\000\001\000\000\000\001".unpack('N*')
  end

  def _integer_big_endian(mod='')
    assert_equal("\x01\x02", [0x0102].pack("s"+mod))
    assert_equal("\x01\x02", [0x0102].pack("S"+mod))
    assert_equal("\x01\x02\x03\x04", [0x01020304].pack("l"+mod))
    assert_equal("\x01\x02\x03\x04", [0x01020304].pack("L"+mod))
    assert_equal("\x01\x02\x03\x04\x05\x06\x07\x08", [0x0102030405060708].pack("q"+mod))
    assert_equal("\x01\x02\x03\x04\x05\x06\x07\x08", [0x0102030405060708].pack("Q"+mod))
    assert_match(/\A\x00*\x01\x02\z/, [0x0102].pack("s!"+mod))
    assert_match(/\A\x00*\x01\x02\z/, [0x0102].pack("S!"+mod))
    assert_match(/\A\x00*\x01\x02\x03\x04\z/, [0x01020304].pack("i"+mod))
    assert_match(/\A\x00*\x01\x02\x03\x04\z/, [0x01020304].pack("I"+mod))
    assert_match(/\A\x00*\x01\x02\x03\x04\z/, [0x01020304].pack("i!"+mod))
    assert_match(/\A\x00*\x01\x02\x03\x04\z/, [0x01020304].pack("I!"+mod))
    assert_match(/\A\x00*\x01\x02\x03\x04\z/, [0x01020304].pack("l!"+mod))
    assert_match(/\A\x00*\x01\x02\x03\x04\z/, [0x01020304].pack("L!"+mod))
    %w[s S l L q Q s! S! i I i! I! l! L!].each {|fmt|
      fmt += mod
      nuls = [0].pack(fmt)
      v = 0
      s = "".force_encoding("ascii-8bit")
      nuls.bytesize.times {|i|
        j = i + 40
        v = v * 256 + j
        s << [j].pack("C")
      }
      assert_equal(s, [v].pack(fmt), "[#{v}].pack(#{fmt.dump})")
      assert_equal([v], s.unpack(fmt), "#{s.dump}.unpack(#{fmt.dump})")
      s2 = s+s
      fmt2 = fmt+"*"
      assert_equal([v,v], s2.unpack(fmt2), "#{s2.dump}.unpack(#{fmt2.dump})")
    }
  end

  def _integer_little_endian(mod='')
    assert_equal("\x02\x01", [0x0102].pack("s"+mod))
    assert_equal("\x02\x01", [0x0102].pack("S"+mod))
    assert_equal("\x04\x03\x02\x01", [0x01020304].pack("l"+mod))
    assert_equal("\x04\x03\x02\x01", [0x01020304].pack("L"+mod))
    assert_equal("\x08\x07\x06\x05\x04\x03\x02\x01", [0x0102030405060708].pack("q"+mod))
    assert_equal("\x08\x07\x06\x05\x04\x03\x02\x01", [0x0102030405060708].pack("Q"+mod))
    assert_match(/\A\x02\x01\x00*\z/, [0x0102].pack("s!"+mod))
    assert_match(/\A\x02\x01\x00*\z/, [0x0102].pack("S!"+mod))
    assert_match(/\A\x04\x03\x02\x01\x00*\z/, [0x01020304].pack("i"+mod))
    assert_match(/\A\x04\x03\x02\x01\x00*\z/, [0x01020304].pack("I"+mod))
    assert_match(/\A\x04\x03\x02\x01\x00*\z/, [0x01020304].pack("i!"+mod))
    assert_match(/\A\x04\x03\x02\x01\x00*\z/, [0x01020304].pack("I!"+mod))
    assert_match(/\A\x04\x03\x02\x01\x00*\z/, [0x01020304].pack("l!"+mod))
    assert_match(/\A\x04\x03\x02\x01\x00*\z/, [0x01020304].pack("L!"+mod))
    %w[s S l L q Q s! S! i I i! I! l! L!].each {|fmt|
      fmt += mod
      nuls = [0].pack(fmt)
      v = 0
      s = "".force_encoding("ascii-8bit")
      nuls.bytesize.times {|i|
        j = i+40
        v = v * 256 + j
        s << [j].pack("C")
      }
      s.reverse!
      assert_equal(s, [v].pack(fmt), "[#{v}].pack(#{fmt.dump})")
      assert_equal([v], s.unpack(fmt), "#{s.dump}.unpack(#{fmt.dump})")
      s2 = s+s
      fmt2 = fmt+"*"
      assert_equal([v,v], s2.unpack(fmt2), "#{s2.dump}.unpack(#{fmt2.dump})")
    }
  end

  def test_integer_endian
    s = [1].pack("s")
    assert_include(["\0\1", "\1\0"], s)
    if s == "\0\1"
      _integer_big_endian()
    else
      _integer_little_endian()
    end
    assert_equal("\x01\x02\x02\x01", [0x0102,0x0102].pack("s>s<"))
    assert_equal([0x0102,0x0102], "\x01\x02\x02\x01".unpack("s>s<"))
  end

  def test_integer_endian_explicit
      _integer_big_endian('>')
      _integer_little_endian('<')
  end

  def test_pack_U
    assert_raise(RangeError) { [-0x40000001].pack("U") }
    assert_raise(RangeError) { [-0x40000000].pack("U") }
    assert_raise(RangeError) { [-1].pack("U") }
    assert_equal "\000", [0].pack("U")
    assert_equal "\374\277\277\277\277\277".force_encoding(Encoding::UTF_8), [0x3fffffff].pack("U")
    assert_equal "\375\200\200\200\200\200".force_encoding(Encoding::UTF_8), [0x40000000].pack("U")
    assert_equal "\375\277\277\277\277\277".force_encoding(Encoding::UTF_8), [0x7fffffff].pack("U")
    assert_raise(RangeError) { [0x80000000].pack("U") }
    assert_raise(RangeError) { [0x100000000].pack("U") }
  end

  def test_pack_P
    a = ["abc"]
    assert_equal a, a.pack("P").unpack("P*")
    assert_equal "a", a.pack("P").unpack("P")[0]
    assert_equal a, a.pack("P").freeze.unpack("P*")
    assert_raise(ArgumentError) { (a.pack("P") + "").unpack("P*") }
  end

  def test_pack_p
    a = ["abc"]
    assert_equal a, a.pack("p").unpack("p*")
    assert_equal a[0], a.pack("p").unpack("p")[0]
    assert_equal a, a.pack("p").freeze.unpack("p*")
    assert_raise(ArgumentError) { (a.pack("p") + "").unpack("p*") }
  end

  def test_format_string_modified
    fmt = "CC"
    o = Object.new
    class << o; self; end.class_eval do
      define_method(:to_int) { fmt.clear; 0 }
    end
    assert_raise(RuntimeError) do
      [o, o].pack(fmt)
    end
  end

  def test_comment
    assert_equal("\0\1", [0,1].pack("  C  #foo \n  C  "))
    assert_equal([0,1], "\0\1".unpack("  C  #foo \n  C  "))
  end

  def test_illegal_bang
    assert_raise(ArgumentError) { [].pack("a!") }
    assert_raise(ArgumentError) { "".unpack("a!") }
  end

  def test_pack_unpack_aA
    assert_equal("f", ["foo"].pack("A"))
    assert_equal("f", ["foo"].pack("a"))
    assert_equal("foo", ["foo"].pack("A*"))
    assert_equal("foo", ["foo"].pack("a*"))
    assert_equal("fo", ["foo"].pack("A2"))
    assert_equal("fo", ["foo"].pack("a2"))
    assert_equal("foo ", ["foo"].pack("A4"))
    assert_equal("foo\0", ["foo"].pack("a4"))
    assert_equal(" ", [nil].pack("A"))
    assert_equal("\0", [nil].pack("a"))
    assert_equal("", [nil].pack("A*"))
    assert_equal("", [nil].pack("a*"))
    assert_equal("  ", [nil].pack("A2"))
    assert_equal("\0\0", [nil].pack("a2"))

    assert_equal("foo" + "\0" * 27, ["foo"].pack("a30"))

    assert_equal(["f"], "foo\0".unpack("A"))
    assert_equal(["f"], "foo\0".unpack("a"))
    assert_equal(["foo"], "foo\0".unpack("A4"))
    assert_equal(["foo\0"], "foo\0".unpack("a4"))
    assert_equal(["foo"], "foo ".unpack("A4"))
    assert_equal(["foo "], "foo ".unpack("a4"))
    assert_equal(["foo"], "foo".unpack("A4"))
    assert_equal(["foo"], "foo".unpack("a4"))
  end

  def test_pack_unpack_Z
    assert_equal("f", ["foo"].pack("Z"))
    assert_equal("foo\0", ["foo"].pack("Z*"))
    assert_equal("fo", ["foo"].pack("Z2"))
    assert_equal("foo\0\0", ["foo"].pack("Z5"))
    assert_equal("\0", [nil].pack("Z"))
    assert_equal("\0", [nil].pack("Z*"))
    assert_equal("\0\0", [nil].pack("Z2"))

    assert_equal(["f"], "foo\0".unpack("Z"))
    assert_equal(["foo"], "foo".unpack("Z*"))
    assert_equal(["foo"], "foo\0".unpack("Z*"))
    assert_equal(["foo"], "foo".unpack("Z5"))
  end

  def test_pack_unpack_bB
    assert_equal("\xff\x00", ["1111111100000000"].pack("b*"))
    assert_equal("\x01\x02", ["1000000001000000"].pack("b*"))
    assert_equal("", ["1"].pack("b0"))
    assert_equal("\x01", ["1"].pack("b1"))
    assert_equal("\x01\x00", ["1"].pack("b2"))
    assert_equal("\x01\x00", ["1"].pack("b3"))
    assert_equal("\x01\x00\x00", ["1"].pack("b4"))
    assert_equal("\x01\x00\x00", ["1"].pack("b5"))
    assert_equal("\x01\x00\x00\x00", ["1"].pack("b6"))

    assert_equal("\xff\x00", ["1111111100000000"].pack("B*"))
    assert_equal("\x01\x02", ["0000000100000010"].pack("B*"))
    assert_equal("", ["1"].pack("B0"))
    assert_equal("\x80", ["1"].pack("B1"))
    assert_equal("\x80\x00", ["1"].pack("B2"))
    assert_equal("\x80\x00", ["1"].pack("B3"))
    assert_equal("\x80\x00\x00", ["1"].pack("B4"))
    assert_equal("\x80\x00\x00", ["1"].pack("B5"))
    assert_equal("\x80\x00\x00\x00", ["1"].pack("B6"))

    assert_equal(["1111111100000000"], "\xff\x00".unpack("b*"))
    assert_equal(["1000000001000000"], "\x01\x02".unpack("b*"))
    assert_equal([""], "".unpack("b0"))
    assert_equal(["1"], "\x01".unpack("b1"))
    assert_equal(["10"], "\x01".unpack("b2"))
    assert_equal(["100"], "\x01".unpack("b3"))

    assert_equal(["1111111100000000"], "\xff\x00".unpack("B*"))
    assert_equal(["0000000100000010"], "\x01\x02".unpack("B*"))
    assert_equal([""], "".unpack("B0"))
    assert_equal(["1"], "\x80".unpack("B1"))
    assert_equal(["10"], "\x80".unpack("B2"))
    assert_equal(["100"], "\x80".unpack("B3"))
  end

  def test_pack_unpack_hH
    assert_equal("\x01\xfe", ["10ef"].pack("h*"))
    assert_equal("", ["10ef"].pack("h0"))
    assert_equal("\x01\x0e", ["10ef"].pack("h3"))
    assert_equal("\x01\xfe\x0", ["10ef"].pack("h5"))
    assert_equal("\xff\x0f", ["fff"].pack("h3"))
    assert_equal("\xff\x0f", ["fff"].pack("h4"))
    assert_equal("\xff\x0f\0", ["fff"].pack("h5"))
    assert_equal("\xff\x0f\0", ["fff"].pack("h6"))
    assert_equal("\xff\x0f\0\0", ["fff"].pack("h7"))
    assert_equal("\xff\x0f\0\0", ["fff"].pack("h8"))

    assert_equal("\x10\xef", ["10ef"].pack("H*"))
    assert_equal("", ["10ef"].pack("H0"))
    assert_equal("\x10\xe0", ["10ef"].pack("H3"))
    assert_equal("\x10\xef\x0", ["10ef"].pack("H5"))
    assert_equal("\xff\xf0", ["fff"].pack("H3"))
    assert_equal("\xff\xf0", ["fff"].pack("H4"))
    assert_equal("\xff\xf0\0", ["fff"].pack("H5"))
    assert_equal("\xff\xf0\0", ["fff"].pack("H6"))
    assert_equal("\xff\xf0\0\0", ["fff"].pack("H7"))
    assert_equal("\xff\xf0\0\0", ["fff"].pack("H8"))

    assert_equal(["10ef"], "\x01\xfe".unpack("h*"))
    assert_equal([""], "\x01\xfe".unpack("h0"))
    assert_equal(["1"], "\x01\xfe".unpack("h1"))
    assert_equal(["10"], "\x01\xfe".unpack("h2"))
    assert_equal(["10e"], "\x01\xfe".unpack("h3"))
    assert_equal(["10ef"], "\x01\xfe".unpack("h4"))
    assert_equal(["10ef"], "\x01\xfe".unpack("h5"))

    assert_equal(["10ef"], "\x10\xef".unpack("H*"))
    assert_equal([""], "\x10\xef".unpack("H0"))
    assert_equal(["1"], "\x10\xef".unpack("H1"))
    assert_equal(["10"], "\x10\xef".unpack("H2"))
    assert_equal(["10e"], "\x10\xef".unpack("H3"))
    assert_equal(["10ef"], "\x10\xef".unpack("H4"))
    assert_equal(["10ef"], "\x10\xef".unpack("H5"))
  end

  def test_pack_unpack_cC
    assert_equal("\0\1\xff", [0, 1, 255].pack("c*"))
    assert_equal("\0\1\xff", [0, 1, -1].pack("c*"))

    assert_equal("\0\1\xff", [0, 1, 255].pack("C*"))
    assert_equal("\0\1\xff", [0, 1, -1].pack("C*"))

    assert_equal([0, 1, -1], "\0\1\xff".unpack("c*"))

    assert_equal([0, 1, 255], "\0\1\xff".unpack("C*"))
  end

  def test_pack_unpack_sS
    s1 = [513, -514].pack("s*")
    s2 = [513, 65022].pack("S*")
    assert_equal(s1, s2)
    assert_equal([513, -514], s2.unpack("s*"))
    assert_equal([513, 65022], s1.unpack("S*"))

    s1 = [513, -514].pack("s!*")
    s2 = [513, 65022].pack("S!*")
    assert_equal(s1, s2)
    assert_equal([513, -514], s2.unpack("s!*"))
    assert_equal([513, 65022], s1.unpack("S!*"))

    assert_equal(2, [1].pack("s").bytesize)
    assert_equal(2, [1].pack("S").bytesize)
    assert_operator(2, :<=, [1].pack("s!").bytesize)
    assert_operator(2, :<=, [1].pack("S!").bytesize)
  end

  def test_pack_unpack_iI
    s1 = [67305985, -50462977].pack("i*")
    s2 = [67305985, 4244504319].pack("I*")
    assert_equal(s1, s2)
    assert_equal([67305985, -50462977], s2.unpack("i*"))
    assert_equal([67305985, 4244504319], s1.unpack("I*"))

    s1 = [67305985, -50462977].pack("i!*")
    s2 = [67305985, 4244504319].pack("I!*")
    assert_equal([67305985, -50462977], s1.unpack("i!*"))
    assert_equal([67305985, 4244504319], s2.unpack("I!*"))

    assert_operator(4, :<=, [1].pack("i").bytesize)
    assert_operator(4, :<=, [1].pack("I").bytesize)
    assert_operator(4, :<=, [1].pack("i!").bytesize)
    assert_operator(4, :<=, [1].pack("I!").bytesize)
  end

  def test_pack_unpack_lL
    s1 = [67305985, -50462977].pack("l*")
    s2 = [67305985, 4244504319].pack("L*")
    assert_equal(s1, s2)
    assert_equal([67305985, -50462977], s2.unpack("l*"))
    assert_equal([67305985, 4244504319], s1.unpack("L*"))

    s1 = [67305985, -50462977].pack("l!*")
    s2 = [67305985, 4244504319].pack("L!*")
    assert_equal([67305985, -50462977], s1.unpack("l!*"))
    assert_equal([67305985, 4244504319], s2.unpack("L!*"))

    assert_equal(4, [1].pack("l").bytesize)
    assert_equal(4, [1].pack("L").bytesize)
    assert_operator(4, :<=, [1].pack("l!").bytesize)
    assert_operator(4, :<=, [1].pack("L!").bytesize)
  end

  def test_pack_unpack_qQ
    s1 = [578437695752307201, -506097522914230529].pack("q*")
    s2 = [578437695752307201, 17940646550795321087].pack("Q*")
    assert_equal(s1, s2)
    assert_equal([578437695752307201, -506097522914230529], s2.unpack("q*"))
    assert_equal([578437695752307201, 17940646550795321087], s1.unpack("Q*"))

    assert_equal(8, [1].pack("q").bytesize)
    assert_equal(8, [1].pack("Q").bytesize)
  end

  def test_pack_unpack_nN
    assert_equal("\000\000\000\001\377\377\177\377\200\000\377\377", [0,1,-1,32767,-32768,65535].pack("n*"))
    assert_equal("\000\000\000\000\000\000\000\001\377\377\377\377", [0,1,-1].pack("N*"))

    assert_equal([0,1,65535,32767,32768,65535], "\000\000\000\001\377\377\177\377\200\000\377\377".unpack("n*"))
    assert_equal([0,1,4294967295], "\000\000\000\000\000\000\000\001\377\377\377\377".unpack("N*"))

    assert_equal(2, [1].pack("n").bytesize)
    assert_equal(4, [1].pack("N").bytesize)
  end

  def test_pack_unpack_vV
    assert_equal("\000\000\001\000\377\377\377\177\000\200\377\377", [0,1,-1,32767,-32768,65535].pack("v*"))
    assert_equal("\000\000\000\000\001\000\000\000\377\377\377\377", [0,1,-1].pack("V*"))

    assert_equal([0,1,65535,32767,32768,65535], "\000\000\001\000\377\377\377\177\000\200\377\377".unpack("v*"))
    assert_equal([0,1,4294967295], "\000\000\000\000\001\000\000\000\377\377\377\377".unpack("V*"))

    assert_equal(2, [1].pack("v").bytesize)
    assert_equal(4, [1].pack("V").bytesize)
  end

  def test_pack_unpack_fdeEgG
    inf = 1.0/0.0
    nan = inf/inf
    [0.0, 1.0, 3.0, inf, -inf, nan].each do |x|
      %w(f d e E g G).each do |f|
        v = [x].pack(f).unpack(f)
        if x.nan?
          assert(v.first.nan?)
        else
          assert_equal([x], v)
        end
      end
    end
  end

  def test_pack_unpack_x
    assert_equal("", [].pack("x0"))
    assert_equal("\0", [].pack("x"))
    assert_equal("\0" * 30, [].pack("x30"))

    assert_equal([0, 2], "\x00\x00\x02".unpack("CxC"))
    assert_raise(ArgumentError) { "".unpack("x") }
  end

  def test_pack_unpack_X
    assert_equal("\x00\x02", [0, 1, 2].pack("CCXC"))
    assert_equal("\x02", [0, 1, 2].pack("CCX2C"))
    assert_raise(ArgumentError) { [].pack("X") }

    assert_equal([0, 2, 2], "\x00\x02".unpack("CCXC"))
    assert_raise(ArgumentError) { "".unpack("X") }
  end

  def test_pack_unpack_atmark
    assert_equal("\x01\x00\x00\x02", [1, 2].pack("C@3C"))
    assert_equal("\x02", [1, 2].pack("C@0C"))
    assert_equal("\x01\x02", [1, 2].pack("C@1C"))

    assert_equal([1, 2], "\x01\x00\x00\x02".unpack("C@3C"))
    assert_equal([nil], "\x00".unpack("@1C")) # is it OK?
    assert_raise(ArgumentError) { "\x00".unpack("@2C") }
  end

  def test_pack_unpack_percent
    assert_raise(ArgumentError) { [].pack("%") }
    assert_raise(ArgumentError) { "".unpack("%") }
  end

  def test_pack_unpack_U
    assert_equal([0], [0].pack("U").unpack("U"))
    assert_equal([0x80], [0x80].pack("U").unpack("U"))
    assert_equal([0x800], [0x800].pack("U").unpack("U"))
    assert_equal([0x10000], [0x10000].pack("U").unpack("U"))
    assert_equal([0x400000], [0x400000].pack("U").unpack("U"))

    assert_raise(ArgumentError) { "\x80".unpack("U") }
    assert_raise(ArgumentError) { "\xff".unpack("U") }
    assert_raise(ArgumentError) { "\xfc\x00".unpack("U") }
    assert_raise(ArgumentError) { "\xc0\xc0".unpack("U") }
    assert_raise(ArgumentError) { "\xe0\x80\x80".unpack("U") }
  end

  def test_pack_unpack_u
    assert_equal("", [""].pack("u"))
    assert_equal("!80``\n", ["a"].pack("u"))
    assert_equal("#86)C\n", ["abc"].pack("u"))
    assert_equal("$86)C9```\n", ["abcd"].pack("u"))
    assert_equal("M86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A\n", ["a"*45].pack("u"))
    assert_equal("M86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A\n!80``\n", ["a"*46].pack("u"))
    assert_equal("&86)C9&5F\n#9VAI\n", ["abcdefghi"].pack("u6"))

    assert_equal("M86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A\n!80``\n", ["a"*46].pack("u0"))
    assert_equal("M86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A\n!80``\n", ["a"*46].pack("u1"))
    assert_equal("M86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A\n!80``\n", ["a"*46].pack("u2"))

    assert_equal([""], "".unpack("u"))
    assert_equal(["a"], "!80``\n".unpack("u"))
    assert_equal(["abc"], "#86)C\n".unpack("u"))
    assert_equal(["abcd"], "$86)C9```\n".unpack("u"))
    assert_equal(["a"*45], "M86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A\n".unpack("u"))
    assert_equal(["a"*46], "M86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A\n!80``\n".unpack("u"))
    assert_equal(["abcdefghi"], "&86)C9&5F\n#9VAI\n".unpack("u"))

    assert_equal(["\x00"], "\"\n".unpack("u"))
    assert_equal(["\x00"], "! \r \n".unpack("u"))
  end

  def test_pack_unpack_m
    assert_equal("", [""].pack("m"))
    assert_equal("AA==\n", ["\0"].pack("m"))
    assert_equal("AAA=\n", ["\0\0"].pack("m"))
    assert_equal("AAAA\n", ["\0\0\0"].pack("m"))
    assert_equal("/w==\n", ["\377"].pack("m"))
    assert_equal("//8=\n", ["\377\377"].pack("m"))
    assert_equal("////\n", ["\377\377\377"].pack("m"))

    assert_equal([""], "".unpack("m"))
    assert_equal(["\0"], "AA==\n".unpack("m"))
    assert_equal(["\0\0"], "AAA=\n".unpack("m"))
    assert_equal(["\0\0\0"], "AAAA\n".unpack("m"))
    assert_equal(["\377"], "/w==\n".unpack("m"))
    assert_equal(["\377\377"], "//8=\n".unpack("m"))
    assert_equal(["\377\377\377"], "////\n".unpack("m"))
  end

  def test_pack_unpack_m0
    assert_equal("", [""].pack("m0"))
    assert_equal("AA==", ["\0"].pack("m0"))
    assert_equal("AAA=", ["\0\0"].pack("m0"))
    assert_equal("AAAA", ["\0\0\0"].pack("m0"))
    assert_equal("/w==", ["\377"].pack("m0"))
    assert_equal("//8=", ["\377\377"].pack("m0"))
    assert_equal("////", ["\377\377\377"].pack("m0"))

    assert_equal([""], "".unpack("m0"))
    assert_equal(["\0"], "AA==".unpack("m0"))
    assert_equal(["\0\0"], "AAA=".unpack("m0"))
    assert_equal(["\0\0\0"], "AAAA".unpack("m0"))
    assert_equal(["\377"], "/w==".unpack("m0"))
    assert_equal(["\377\377"], "//8=".unpack("m0"))
    assert_equal(["\377\377\377"], "////".unpack("m0"))

    assert_raise(ArgumentError) { "^".unpack("m0") }
    assert_raise(ArgumentError) { "A".unpack("m0") }
    assert_raise(ArgumentError) { "A^".unpack("m0") }
    assert_raise(ArgumentError) { "AA".unpack("m0") }
    assert_raise(ArgumentError) { "AA=".unpack("m0") }
    assert_raise(ArgumentError) { "AA===".unpack("m0") }
    assert_raise(ArgumentError) { "AA=x".unpack("m0") }
    assert_raise(ArgumentError) { "AAA".unpack("m0") }
    assert_raise(ArgumentError) { "AAA^".unpack("m0") }
    assert_raise(ArgumentError) { "AB==".unpack("m0") }
    assert_raise(ArgumentError) { "AAB=".unpack("m0") }
  end

  def test_pack_unpack_M
    assert_equal("a b c\td =\n\ne=\n", ["a b c\td \ne"].pack("M"))
    assert_equal(["a b c\td \ne"], "a b c\td =\n\ne=\n".unpack("M"))
    assert_equal("=00=\n", ["\0"].pack("M"))
    assert_equal("a"*73+"=\na=\n", ["a"*74].pack("M"))
    assert_equal(("a"*73+"=\n")*14+"a=\n", ["a"*1023].pack("M"))
    assert_equal(["\0"], "=00=\n".unpack("M"))
    assert_equal(["a"*74], ("a"*73+"=\na=\n").unpack("M"))
    assert_equal(["a"*1023], (("a"*73+"=\n")*14+"a=\n").unpack("M"))
    assert_equal(["\x0a"], "=0a=\n".unpack("M"))
    assert_equal(["\x0a"], "=0A=\n".unpack("M"))
    assert_equal([""], "=0Z=\n".unpack("M"))
    assert_equal([""], "=\r\n".unpack("M"))
    assert_equal([""], "=\r\n".unpack("M"))
    assert_equal(["\xC6\xF7"], "=C6=F7".unpack('M*'))
  end

  def test_pack_unpack_P2
    assert_raise(ArgumentError) { ["abc"].pack("P4") }
    assert_raise(ArgumentError) { [""].pack("P") }

    assert_equal([], ".".unpack("P"))
    assert_equal([], ".".unpack("p"))
    assert_equal([nil], ("\0" * 1024).unpack("P"))
  end

  def test_pack_p2
    assert_match(/\A\0*\Z/, [nil].pack("p"))
  end

  def test_pack_unpack_w
    assert_equal("\000", [0].pack("w"))
    assert_equal("\001", [1].pack("w"))
    assert_equal("\177", [127].pack("w"))
    assert_equal("\201\000", [128].pack("w"))
    assert_equal("\377\177", [0x3fff].pack("w"))
    assert_equal("\201\200\000", [0x4000].pack("w"))
    assert_equal("\203\377\377\377\177", [0x3fffffff].pack("w"))
    assert_equal("\204\200\200\200\000", [0x40000000].pack("w"))
    assert_equal("\217\377\377\377\177", [0xffffffff].pack("w"))
    assert_equal("\220\200\200\200\000", [0x100000000].pack("w"))
    assert_raise(ArgumentError) { [-1].pack("w") }

    assert_equal([0], "\000".unpack("w"))
    assert_equal([1], "\001".unpack("w"), [1])
    assert_equal([127], "\177".unpack("w"), [127])
    assert_equal([128], "\201\000".unpack("w"), [128])
    assert_equal([0x3fff], "\377\177".unpack("w"), [0x3fff])
    assert_equal([0x4000], "\201\200\000".unpack("w"), [0x4000])
    assert_equal([0x3fffffff], "\203\377\377\377\177".unpack("w"), [0x3fffffff])
    assert_equal([0x40000000], "\204\200\200\200\000".unpack("w"), [0x40000000])
    assert_equal([0xffffffff], "\217\377\377\377\177".unpack("w"), [0xffffffff])
    assert_equal([0x100000000], "\220\200\200\200\000".unpack("w"), [0x100000000])
  end


  def test_pack_unpack_M
    assert_equal(["pre123after"], "pre=31=32=33after".unpack("M"))
    assert_equal(["preafter"], "pre=\nafter".unpack("M"))
    assert_equal(["preafter"], "pre=\r\nafter".unpack("M"))
    assert_equal(["pre="], "pre=".unpack("M"))
    assert_equal(["pre=\r"], "pre=\r".unpack("M"))
    assert_equal(["pre=hoge"], "pre=hoge".unpack("M"))
    assert_equal(["pre==31after"], "pre==31after".unpack("M"))
    assert_equal(["pre===31after"], "pre===31after".unpack("M"))
  end

  def test_modify_under_safe4
    s = "foo"
    assert_raise(SecurityError) do
      Thread.new do
        $SAFE = 4
        s.clear
      end.join
    end
  end

  def test_length_too_big
    assert_raise(RangeError) { [].pack("C100000000000000000000") }
  end

  def test_short_string
    %w[n N v V s S i I l L q Q s! S! i! I! l! l!].each {|fmt|
      str = [1].pack(fmt)
      assert_equal([1,nil], str.unpack("#{fmt}2"))
    }
  end

  def test_short_with_block
    bug4059 = '[ruby-core:33193]'
    result = :ok
    assert_nil("".unpack("i") {|x| result = x}, bug4059)
    assert_equal(:ok, result)
  end
end
