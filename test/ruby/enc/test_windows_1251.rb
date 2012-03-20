# encoding:windows-1251

require "test/unit"

class TestWindows1251 < Test::Unit::TestCase
  def test_windows_1251
    (0xc0..0xdf).each do |c|
      c1 = c.chr("windows-1251")
      c2 = (c + 0x20).chr("windows-1251")
      assert_match(/^(#{ c1 })\1$/i, c2 + c1)
      assert_match(/^(#{ c2 })\1$/i, c1 + c2)
      assert_match(/^[#{ c1 }]+$/i, c2 + c1)
      assert_match(/^[#{ c2 }]+$/i, c1 + c2)
    end
  end
end
