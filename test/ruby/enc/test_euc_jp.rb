# vim: set fileencoding=euc-jp

require "test/unit"

class TestEUC_JP < Test::Unit::TestCase
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

  def test_charboundary
    assert_nil(/\xA2\xA2/ =~ "\xA1\xA2\xA2\xA3")
  end
end
