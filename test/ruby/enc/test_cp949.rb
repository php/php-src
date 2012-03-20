require "test/unit"

class TestCP949 < Test::Unit::TestCase
  def s(s)
    s.force_encoding("cp949")
  end

  def test_mbc_enc_len
    assert_equal(1, s("\xa1\xa1").size)
  end

  def test_mbc_to_code
    assert_equal(0xa1a1, s("\xa1\xa1").ord)
  end

  def test_code_to_mbc
    assert_equal(s("\xa1\xa1"), 0xa1a1.chr("cp949"))
  end

  def test_mbc_case_fold
    r = Regexp.new(s("(\xa1\xa1)\\1"), "i")
    assert_match(r, s("\xa1\xa1\xa1\xa1"))
  end

  def test_left_adjust_char_head
    assert_equal(s("\xa1\xa1"), s("\xa1\xa1\xa1\xa1").chop)
  end
end
