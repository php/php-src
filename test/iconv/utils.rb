begin
  verbose, $VERBOSE = $VERBOSE, nil
  require 'iconv'
rescue LoadError
else
  require 'test/unit'
ensure
  $VERBOSE = verbose
end

class TestIconv < ::Test::Unit::TestCase
  if defined?(::Encoding) and String.method_defined?(:force_encoding)
    def self.encode(str, enc)
      str.force_encoding(enc)
    end
  else
    def self.encode(str, enc)
      str
    end
  end

  def default_test
    self.class == TestIconv or super
  end

  ASCII = "ascii"
  EUCJ_STR = encode("\xa4\xa2\xa4\xa4\xa4\xa6\xa4\xa8\xa4\xaa", "EUC-JP").freeze
  SJIS_STR = encode("\x82\xa0\x82\xa2\x82\xa4\x82\xa6\x82\xa8", "Shift_JIS").freeze
end if defined?(::Iconv)
