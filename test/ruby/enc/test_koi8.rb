require "test/unit"

class TestKOI8 < Test::Unit::TestCase
  ASSERTS = %q(
    (0xc0..0xdf).each do |c|
      c1 = c.chr("ENCODING")
      c2 = (c + 0x20).chr("ENCODING")
      assert_match(/^(#{ c1 })\1$/i, c2 + c1)
      assert_match(/^(#{ c2 })\1$/i, c1 + c2)
      assert_match(/^[#{ c1 }]+$/i, c2 + c1)
      assert_match(/^[#{ c2 }]+$/i, c1 + c2)
    end
  )

  def test_koi8_r
    eval("# encoding: koi8-r\n" + ASSERTS.gsub("ENCODING", "koi8-r"))
  end

  def test_koi8_u
    eval("# encoding: koi8-u\n" + ASSERTS.gsub("ENCODING", "koi8-u"))
  end
end
