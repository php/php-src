require File.expand_path("../utils.rb", __FILE__)

class TestIconv::Basic < TestIconv
  def test_euc2sjis
    iconv = Iconv.open('SHIFT_JIS', 'EUC-JP')
    str = iconv.iconv(EUCJ_STR)
    str << iconv.iconv(nil)
    assert_equal(SJIS_STR, str)
    iconv.close
  end

  def test_close
    iconv = Iconv.new('Shift_JIS', 'EUC-JP')
    output = ""
    begin
      output += iconv.iconv(EUCJ_STR)
      output += iconv.iconv(nil)
    ensure
      assert_respond_to(iconv, :close)
      assert_equal("", iconv.close)
      assert_equal(SJIS_STR, output)
    end
  end

  def test_open_without_block
    assert_respond_to(Iconv, :open)
    iconv = Iconv.open('SHIFT_JIS', 'EUC-JP')
    str = iconv.iconv(EUCJ_STR)
    str << iconv.iconv(nil)
    assert_equal(SJIS_STR, str )
    iconv.close
  end

  def test_open_with_block
    input = "#{EUCJ_STR}\n"*2
    output = ""
    Iconv.open("Shift_JIS", "EUC-JP") do |cd|
      input.each_line do |s|
        output << cd.iconv(s)
      end
      output << cd.iconv(nil)
    end
    assert_equal("#{SJIS_STR}\n"*2, output)
  end

  def test_invalid_arguments
    assert_raise(TypeError) { Iconv.new(nil, 'Shift_JIS') }
    assert_raise(TypeError) { Iconv.new('Shift_JIS', nil) }
    assert_raise(TypeError) { Iconv.open(nil, 'Shift_JIS') }
    assert_raise(TypeError) { Iconv.open('Shift_JIS', nil) }
  end

  def test_unknown_encoding
    assert_raise(Iconv::InvalidEncoding) { Iconv.iconv("utf-8", "X-UKNOWN", "heh") }
    assert_raise(Iconv::InvalidEncoding, '[ruby-dev:39487]') {
      Iconv.iconv("X-UNKNOWN-1", "X-UNKNOWN-2") {break}
    }
  end
end if defined?(TestIconv)
