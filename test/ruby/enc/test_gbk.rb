require "test/unit"

class TestGBK < Test::Unit::TestCase
  def s(s)
    s.force_encoding("gbk")
  end

  def test_mbc_enc_len
    assert_equal(1, s("\x81\x40").size)
  end

  def test_mbc_to_code
    assert_equal(0x8140, s("\x81\x40").ord)
  end

  def test_code_to_mbc
    assert_equal(s("\x81\x40"), 0x8140.chr("gbk"))
  end

  def test_mbc_case_fold
    r = Regexp.new(s("(\x81\x40)\\1"), "i")
    assert_match(r, s("\x81\x40\x81\x40"))
  end

  def test_left_adjust_char_head
    assert_equal(s("\x81\x40"), s("\x81\x40\x81\x40").chop)
  end
end
