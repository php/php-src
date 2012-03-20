require 'test/unit'

class TestISO8859 < Test::Unit::TestCase
  ASSERTS = %q(
    assert_match(/^(\xdf)\1$/i, "\xdf\xdf")
    assert_match(/^(\xdf)\1$/i, "ssss")
    # assert_match(/^(\xdf)\1$/i, "\xdfss") # this must be bug...
    assert_match(/^[\xdfz]+$/i, "sszzsszz")
    assert_match(/^SS$/i, "\xdf")
    assert_match(/^Ss$/i, "\xdf")
    ((0xc0..0xde).to_a - [0xd7]).each do |c|
      c1 = c.chr("ENCODING")
      c2 = (c + 0x20).chr("ENCODING")
      assert_match(/^(#{ c1 })\1$/i, c2 + c1)
      assert_match(/^(#{ c2 })\1$/i, c1 + c2)
      assert_match(/^[#{ c1 }]+$/i, c2 + c1)
      assert_match(/^[#{ c2 }]+$/i, c1 + c2)
    end
    assert_match(/^\xff$/i, "\xff")
  )

  def test_iso_8859_1
    eval("# encoding: iso8859-1\n" + ASSERTS.gsub(/ENCODING/m, "iso8859-1"))
  end

  def test_iso_8859_2
    eval("# encoding: iso8859-2\n" + ASSERTS.gsub(/ENCODING/m, "iso8859-2"))
  end

  def test_iso_8859_3
    eval(%q(# encoding: iso8859-3
      assert_match(/^(\xdf)\1$/i, "\xdf\xdf")
      assert_match(/^(\xdf)\1$/i, "ssss")
      assert_match(/^[\xdfz]+$/i, "sszzsszz")
      assert_match(/^SS$/i, "\xdf")
      assert_match(/^Ss$/i, "\xdf")
      [0xa1, 0xa6, *(0xa9..0xac), 0xaf].each do |c|
        c1 = c.chr("iso8859-3")
        c2 = (c + 0x10).chr("iso8859-3")
        assert_match(/^(#{ c1 })\1$/i, c2 + c1)
        assert_match(/^(#{ c2 })\1$/i, c1 + c2)
        assert_match(/^[#{ c1 }]+$/i, c2 + c1)
        assert_match(/^[#{ c2 }]+$/i, c1 + c2)
      end
      ([*(0xc0..0xde)] - [0xc3, 0xd0, 0xd7]).each do |c|
        c1 = c.chr("iso8859-3")
        c2 = (c + 0x20).chr("iso8859-3")
        assert_match(/^(#{ c1 })\1$/i, c2 + c1)
        assert_match(/^(#{ c2 })\1$/i, c1 + c2)
        assert_match(/^[#{ c1 }]+$/i, c2 + c1)
        assert_match(/^[#{ c2 }]+$/i, c1 + c2)
      end
    ))
  end

  def test_iso_8859_4
    eval("# encoding: iso8859-4\n" + ASSERTS.gsub(/ENCODING/m, "iso8859-4"))
  end

  def test_iso_8859_5
    eval(%q(# encoding: iso8859-5
      (0xb0..0xcf).each do |c|
        c1 = c.chr("iso8859-5")
        c2 = (c + 0x20).chr("iso8859-5")
        assert_match(/^(#{ c1 })\1$/i, c2 + c1)
        assert_match(/^(#{ c2 })\1$/i, c1 + c2)
        assert_match(/^[#{ c1 }]+$/i, c2 + c1)
        assert_match(/^[#{ c2 }]+$/i, c1 + c2)
      end
      ((0xa1..0xaf).to_a - [0xad]).each do |c|
        c1 = c.chr("iso8859-5")
        c2 = (c + 0x50).chr("iso8859-5")
        assert_match(/^(#{ c1 })\1$/i, c2 + c1)
        assert_match(/^(#{ c2 })\1$/i, c1 + c2)
        assert_match(/^[#{ c1 }]+$/i, c2 + c1)
        assert_match(/^[#{ c2 }]+$/i, c1 + c2)
      end
    ))
  end

  def test_iso_8859_6
    eval(%q(# encoding: iso8859-6
      [0xa4, 0xac, 0xbb, 0xbf, *(0xc1..0xda), *(0xe0..0xf2)].each do |c|
        c1 = c.chr("iso8859-6")
        assert_match(/^(#{ c1 })\1$/i, c1 * 2)
      end
    ))
  end

  def test_iso_8859_7
    eval(%q(# encoding: iso8859-7
      ((0xa0..0xfe).to_a - [0xae, 0xd2]).each do |c|
        c1 = c.chr("iso8859-7")
        assert_match(/^(#{ c1 })\1$/i, c1 * 2)
      end
      ((0xc1..0xd9).to_a - [0xd2]).each do |c|
        c1 = c.chr("iso8859-7")
        c2 = (c + 0x20).chr("iso8859-7")
        assert_match(/^(#{ c1 })\1$/i, c2 + c1)
        assert_match(/^(#{ c2 })\1$/i, c1 + c2)
        assert_match(/^[#{ c1 }]+$/i, c2 + c1)
        assert_match(/^[#{ c2 }]+$/i, c1 + c2)
      end
    ))
  end

  def test_iso_8859_8
    eval(%q(# encoding: iso8859-8
      [0xa0, *(0xa2..0xbe), *(0xdf..0xfa), 0xfc, 0xfd].each do |c|
        c1 = c.chr("iso8859-8")
        assert_match(/^(#{ c1 })\1$/i, c1 * 2)
      end
    ))
  end

  def test_iso_8859_9
    eval(%q(# encoding: iso8859-9
      assert_match(/^(\xdf)\1$/i, "\xdf\xdf")
      assert_match(/^(\xdf)\1$/i, "ssss")
      assert_match(/^[\xdfz]+$/i, "sszzsszz")
      assert_match(/^SS$/i, "\xdf")
      assert_match(/^Ss$/i, "\xdf")
      ([*(0xc0..0xdc)] - [0xd7]).each do |c|
        c1 = c.chr("iso8859-9")
        c2 = (c + 0x20).chr("iso8859-9")
        assert_match(/^(#{ c1 })\1$/i, c2 + c1)
        assert_match(/^(#{ c2 })\1$/i, c1 + c2)
        assert_match(/^[#{ c1 }]+$/i, c2 + c1)
        assert_match(/^[#{ c2 }]+$/i, c1 + c2)
      end
    ))
  end

  def test_iso_8859_10
    eval("# encoding: iso8859-10\n" + ASSERTS.gsub(/ENCODING/m, "iso8859-10"))
  end

  def test_iso_8859_11
    eval(%q(# encoding: iso8859-11
      [*(0xa0..0xda), *(0xdf..0xfb)].each do |c|
        c1 = c.chr("iso8859-11")
        assert_match(/^(#{ c1 })\1$/i, c1 * 2)
      end
    ))
  end

  def test_iso_8859_13
    eval("# encoding: iso8859-13\n" + ASSERTS.gsub(/ENCODING/m, "iso8859-13"))
  end

  def test_iso_8859_14
    eval("# encoding: iso8859-14\n" + ASSERTS.gsub(/ENCODING/m, "iso8859-14"))
  end

  def test_iso_8859_15
    eval("# encoding: iso8859-15\n" + ASSERTS.gsub(/ENCODING/m, "iso8859-15"))
  end

  def test_iso_8859_16
    eval("# encoding: iso8859-16\n" + ASSERTS.gsub(/ENCODING/m, "iso8859-16"))
  end
end

