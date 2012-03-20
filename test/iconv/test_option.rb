require File.expand_path("../utils.rb", __FILE__)

class TestIconv::Option < TestIconv
  def test_ignore_option
    begin
      iconv = Iconv.new('SHIFT_JIS', 'EUC-JP')
      iconv.transliterate?
    rescue NotImplementedError
      return
    end
    iconv = Iconv.new('SHIFT_JIS', 'EUC-JP//ignore')
    str = iconv.iconv(EUCJ_STR)
    str << iconv.iconv(nil)
    assert_equal(SJIS_STR, str)
    iconv.close

    iconv = Iconv.new('SHIFT_JIS//IGNORE', 'EUC-JP//ignore')
    str = iconv.iconv(EUCJ_STR)
    str << iconv.iconv(nil)
    assert_equal(SJIS_STR, str)
    iconv.close
  end

  def test_translit_option
    begin
      iconv = Iconv.new('SHIFT_JIS', 'EUC-JP')
      iconv.transliterate?
    rescue NotImplementedError
      return
    end
    iconv = Iconv.new('SHIFT_JIS', 'EUC-JP//ignore')
    str = iconv.iconv(EUCJ_STR)
    str << iconv.iconv(nil)
    assert_equal(SJIS_STR, str)
    iconv.close

    iconv = Iconv.new('SHIFT_JIS//TRANSLIT', 'EUC-JP//translit//ignore')
    str = iconv.iconv(EUCJ_STR)
    str << iconv.iconv(nil)
    assert_equal(SJIS_STR, str)
    iconv.close
  end
end if defined?(TestIconv)
