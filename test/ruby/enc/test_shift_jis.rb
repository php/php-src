# vim: set fileencoding=shift_jis

require "test/unit"

class TestShiftJIS < Test::Unit::TestCase
  def test_mbc_case_fold
    assert_match(/(ａ)(a)\1\2/i, "ａaａA")
    assert_match(/(ａ)(a)\1\2/i, "ａaＡA")
  end

  def test_property
    assert_match(/あ{0}\p{Hiragana}{4}/, "ひらがな")
    assert_no_match(/あ{0}\p{Hiragana}{4}/, "カタカナ")
    assert_no_match(/あ{0}\p{Hiragana}{4}/, "漢字漢字")
    assert_no_match(/あ{0}\p{Katakana}{4}/, "ひらがな")
    assert_match(/あ{0}\p{Katakana}{4}/, "カタカナ")
    assert_no_match(/あ{0}\p{Katakana}{4}/, "漢字漢字")
    assert_raise(RegexpError) { Regexp.new('あ{0}\p{foobarbaz}') }
  end

  def test_code_to_mbclen
    s = "あいうえお"
    s << 0x82a9
    assert_equal("あいうえおか", s)
    assert_raise(RangeError) { s << 0x82 }
  end
end
