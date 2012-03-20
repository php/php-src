require File.expand_path("../utils.rb", __FILE__)

class TestIconv::Partial < TestIconv
  def test_partial_ascii
    c = Iconv.open(ASCII, ASCII)
    ref = '[ruby-core:17092]'
  rescue
    return
  else
    assert_equal("abc", c.iconv("abc"))
    assert_equal("c",   c.iconv("abc", 2),     "#{ref}: with start")
    assert_equal("c",   c.iconv("abc", 2, 1),  "#{ref}: with start, length")
    assert_equal("c",   c.iconv("abc", 2, 5),  "#{ref}: with start, longer length")
    assert_equal("bc",  c.iconv("abc", -2),    "#{ref}: with nagative start")
    assert_equal("b",   c.iconv("abc", -2, 1), "#{ref}: with nagative start, length")
    assert_equal("bc",  c.iconv("abc", -2, 5), "#{ref}: with nagative start, longer length")
    assert_equal("",    c.iconv("abc", 5),     "#{ref}: with OOB")
    assert_equal("",    c.iconv("abc", 5, 2),  "#{ref}: with OOB, length")
  ensure
    c.close if c
  end

  def test_partial_euc2sjis
    c = Iconv.open('SHIFT_JIS', 'EUC-JP')
  rescue
    return
  else
    assert_equal(SJIS_STR[0, 2],   c.iconv(EUCJ_STR, 0, 2))
    assert_equal(SJIS_STR,         c.iconv(EUCJ_STR, 0, 20))
    assert_equal(SJIS_STR[2..-1],  c.iconv(EUCJ_STR, 2))
    assert_equal(SJIS_STR[2, 2],   c.iconv(EUCJ_STR, 2, 2))
    assert_equal(SJIS_STR[2..-1],  c.iconv(EUCJ_STR, 2, 20))
    assert_equal(SJIS_STR[-4..-1], c.iconv(EUCJ_STR, -4))
    assert_equal(SJIS_STR[-4, 2],  c.iconv(EUCJ_STR, -4, 2))
    assert_equal(SJIS_STR[-4..-1], c.iconv(EUCJ_STR, -4, 20))
    assert_equal("",               c.iconv(EUCJ_STR, 20))
    assert_equal("",               c.iconv(EUCJ_STR, 20, 2))
  ensure
    c.close
  end
end if defined?(TestIconv)
