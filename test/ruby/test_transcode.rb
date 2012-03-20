# encoding: ASCII-8BIT   # make sure this runs in binary mode
# some of the comments are in UTF-8

require 'test/unit'
class TestTranscode < Test::Unit::TestCase
  def test_errors
    assert_raise(Encoding::ConverterNotFoundError) { 'abc'.encode('foo', 'bar') }
    assert_raise(Encoding::ConverterNotFoundError) { 'abc'.encode!('foo', 'bar') }
    assert_raise(Encoding::ConverterNotFoundError) { 'abc'.force_encoding('utf-8').encode('foo') }
    assert_raise(Encoding::ConverterNotFoundError) { 'abc'.force_encoding('utf-8').encode!('foo') }
    assert_raise(Encoding::UndefinedConversionError) { "\x80".encode('utf-8','ASCII-8BIT') }
    assert_raise(Encoding::InvalidByteSequenceError) { "\x80".encode('utf-8','US-ASCII') }
    assert_raise(Encoding::UndefinedConversionError) { "\xA5".encode('utf-8','iso-8859-3') }
    assert_raise(RuntimeError) { 'hello'.freeze.encode!('iso-8859-1') }
    assert_raise(RuntimeError) { '\u3053\u3093\u306b\u3061\u306f'.freeze.encode!('iso-8859-1') } # こんにちは
  end

  def test_arguments
    assert_equal('abc', 'abc'.force_encoding('utf-8').encode('iso-8859-1'))
    # check that encoding is kept when no conversion is done
    assert_equal('abc'.force_encoding('Shift_JIS'), 'abc'.force_encoding('Shift_JIS').encode('Shift_JIS'))
    assert_equal('abc'.force_encoding('Shift_JIS'), 'abc'.force_encoding('Shift_JIS').encode!('Shift_JIS'))
    # assert that encoding is correctly set
    assert_equal("D\u00FCrst".encoding, "D\xFCrst".force_encoding('iso-8859-1').encode('utf-8').encoding)
    # check that Encoding can be used as parameter
    assert_equal("D\u00FCrst", "D\xFCrst".encode('utf-8', Encoding.find('ISO-8859-1')))
    assert_equal("D\u00FCrst", "D\xFCrst".encode(Encoding.find('utf-8'), 'ISO-8859-1'))
    assert_equal("D\u00FCrst", "D\xFCrst".encode(Encoding.find('utf-8'), Encoding.find('ISO-8859-1')))
  end

  def test_noargument
    default_default_internal = Encoding.default_internal
    Encoding.default_internal = nil
    assert_equal("\u3042".encode, "\u3042")
    assert_equal("\xE3\x81\x82\x81".force_encoding("utf-8").encode,
                 "\xE3\x81\x82\x81".force_encoding("utf-8"))
    Encoding.default_internal = 'EUC-JP'
    assert_equal("\u3042".encode, "\xA4\xA2".force_encoding('EUC-JP'))
    assert_equal("\xE3\x81\x82\x81".force_encoding("utf-8").encode,
                 "\xA4\xA2?".force_encoding('EUC-JP'))
    Encoding.default_internal = default_default_internal
  end

  def test_length
    assert_equal("\u20AC"*20, ("\xA4"*20).encode('utf-8', 'iso-8859-15'))
    assert_equal("\u20AC"*20, ("\xA4"*20).encode!('utf-8', 'iso-8859-15'))
    assert_equal("\u20AC"*2000, ("\xA4"*2000).encode('utf-8', 'iso-8859-15'))
    assert_equal("\u20AC"*2000, ("\xA4"*2000).encode!('utf-8', 'iso-8859-15'))
    assert_equal("\u20AC"*200000, ("\xA4"*200000).encode('utf-8', 'iso-8859-15'))
    assert_equal("\u20AC"*200000, ("\xA4"*200000).encode!('utf-8', 'iso-8859-15'))
  end

  def check_both_ways(utf8, raw, encoding)
    assert_equal(utf8.force_encoding('utf-8'), raw.encode('utf-8', encoding),utf8.dump)
    assert_equal(raw.force_encoding(encoding), utf8.encode(encoding, 'utf-8'))
  end

  def check_both_ways2(str1, enc1, str2, enc2)
    assert_equal(str1.force_encoding(enc1), str2.encode(enc1, enc2))
    assert_equal(str2.force_encoding(enc2), str1.encode(enc2, enc1))
  end

  def test_encodings
    check_both_ways("\u307E\u3064\u3082\u3068 \u3086\u304D\u3072\u308D",
        "\x82\xdc\x82\xc2\x82\xe0\x82\xc6 \x82\xe4\x82\xab\x82\xd0\x82\xeb", 'shift_jis') # まつもと ゆきひろ
    check_both_ways("\u307E\u3064\u3082\u3068 \u3086\u304D\u3072\u308D",
        "\xa4\xde\xa4\xc4\xa4\xe2\xa4\xc8 \xa4\xe6\xa4\xad\xa4\xd2\xa4\xed", 'euc-jp')
    check_both_ways("\u677E\u672C\u884C\u5F18", "\x8f\xbc\x96\x7b\x8d\x73\x8d\x4f", 'shift_jis') # 松本行弘
    check_both_ways("\u677E\u672C\u884C\u5F18", "\xbe\xbe\xcb\xdc\xb9\xd4\xb9\xb0", 'euc-jp')
    check_both_ways("D\u00FCrst", "D\xFCrst", 'iso-8859-1') # Dürst
    check_both_ways("D\u00FCrst", "D\xFCrst", 'iso-8859-2')
    check_both_ways("D\u00FCrst", "D\xFCrst", 'iso-8859-3')
    check_both_ways("D\u00FCrst", "D\xFCrst", 'iso-8859-4')
    check_both_ways("D\u00FCrst", "D\xFCrst", 'iso-8859-9')
    check_both_ways("D\u00FCrst", "D\xFCrst", 'iso-8859-10')
    check_both_ways("D\u00FCrst", "D\xFCrst", 'iso-8859-13')
    check_both_ways("D\u00FCrst", "D\xFCrst", 'iso-8859-14')
    check_both_ways("D\u00FCrst", "D\xFCrst", 'iso-8859-15')
    check_both_ways("r\u00E9sum\u00E9", "r\xE9sum\xE9", 'iso-8859-1') # résumé
    check_both_ways("\u0065\u006C\u0151\u00ED\u0072\u00E1\u0073", "el\xF5\xEDr\xE1s", 'iso-8859-2') # előírás
    check_both_ways("\u043F\u0435\u0440\u0435\u0432\u043E\u0434",
         "\xDF\xD5\xE0\xD5\xD2\xDE\xD4", 'iso-8859-5') # перевод
    check_both_ways("\u0643\u062A\u0628", "\xE3\xCA\xC8", 'iso-8859-6') # كتب
    check_both_ways("\u65E5\u8A18", "\x93\xFA\x8BL", 'shift_jis') # 日記
    check_both_ways("\u65E5\u8A18", "\xC6\xFC\xB5\xAD", 'euc-jp')
    check_both_ways("\uC560\uC778\uAD6C\uD568\u0020\u6734\uC9C0\uC778",
         "\xBE\xD6\xC0\xCE\xB1\xB8\xC7\xD4\x20\xDA\xD3\xC1\xF6\xC0\xCE", 'euc-kr') # 애인구함 朴지인
    check_both_ways("\uC544\uD58F\uD58F\u0020\uB620\uBC29\uD6BD\uB2D8\u0020\uC0AC\uB791\uD716",
         "\xBE\xC6\xC1\x64\xC1\x64\x20\x8C\x63\xB9\xE6\xC4\x4F\xB4\xD4\x20\xBB\xE7\xB6\xFB\xC5\x42", 'cp949') # 아햏햏 똠방횽님 사랑휖
    assert_equal(Encoding::ISO_8859_1, "D\xFCrst".force_encoding('iso-8859-2').encode('iso-8859-1', 'iso-8859-1').encoding)
  end

  def test_twostep
    assert_equal("D\xFCrst".force_encoding('iso-8859-2'), "D\xFCrst".encode('iso-8859-2', 'iso-8859-1'))
  end

  def test_ascii_range
    encodings = [
      'US-ASCII', 'ASCII-8BIT',
      'ISO-8859-1', 'ISO-8859-2', 'ISO-8859-3',
      'ISO-8859-4', 'ISO-8859-5', 'ISO-8859-6',
      'ISO-8859-7', 'ISO-8859-8', 'ISO-8859-9',
      'ISO-8859-10', 'ISO-8859-11', 'ISO-8859-13',
      'ISO-8859-14', 'ISO-8859-15',
      'EUC-JP', 'SHIFT_JIS', 'EUC-KR'
    ]
    all_ascii = (0..127).to_a.pack 'C*'
    encodings.each do |enc|
      test_start = all_ascii
      assert_equal(test_start, test_start.encode('UTF-8',enc).encode(enc).force_encoding('ASCII-8BIT'))
    end
  end

  def test_all_bytes
    encodings_8859 = [
      'ISO-8859-1', 'ISO-8859-2',
      #'ISO-8859-3', # not all bytes used
      'ISO-8859-4', 'ISO-8859-5',
      #'ISO-8859-6', # not all bytes used
      #'ISO-8859-7', # not all bytes used
      #'ISO-8859-8', # not all bytes used
      'ISO-8859-9', 'ISO-8859-10',
      #'ISO-8859-11', # not all bytes used
      #'ISO-8859-12', # not available
      'ISO-8859-13','ISO-8859-14','ISO-8859-15',
      #'ISO-8859-16', # not available
    ]
    all_bytes = (0..255).to_a.pack 'C*'
    encodings_8859.each do |enc|
      test_start = all_bytes
      assert_equal(test_start, test_start.encode('UTF-8',enc).encode(enc).force_encoding('ASCII-8BIT'))
    end
  end

  def test_windows_874
    check_both_ways("\u20AC", "\x80", 'windows-874') # €
    assert_raise(Encoding::UndefinedConversionError) { "\x81".encode("utf-8", 'windows-874') }
    assert_raise(Encoding::UndefinedConversionError) { "\x84".encode("utf-8", 'windows-874') }
    check_both_ways("\u2026", "\x85", 'windows-874') # …
    assert_raise(Encoding::UndefinedConversionError) { "\x86".encode("utf-8", 'windows-874') }
    assert_raise(Encoding::UndefinedConversionError) { "\x8F".encode("utf-8", 'windows-874') }
    assert_raise(Encoding::UndefinedConversionError) { "\x90".encode("utf-8", 'windows-874') }
    check_both_ways("\u2018", "\x91", 'windows-874') # ‘
    check_both_ways("\u2014", "\x97", 'windows-874') # —
    assert_raise(Encoding::UndefinedConversionError) { "\x98".encode("utf-8", 'windows-874') }
    assert_raise(Encoding::UndefinedConversionError) { "\x9F".encode("utf-8", 'windows-874') }
    check_both_ways("\u00A0", "\xA0", 'windows-874') # non-breaking space
    check_both_ways("\u0E0F", "\xAF", 'windows-874') # ฏ
    check_both_ways("\u0E10", "\xB0", 'windows-874') # ฐ
    check_both_ways("\u0E1F", "\xBF", 'windows-874') # ฟ
    check_both_ways("\u0E20", "\xC0", 'windows-874') # ภ
    check_both_ways("\u0E2F", "\xCF", 'windows-874') # ฯ
    check_both_ways("\u0E30", "\xD0", 'windows-874') # ะ
    check_both_ways("\u0E3A", "\xDA", 'windows-874') # ฺ
    assert_raise(Encoding::UndefinedConversionError) { "\xDB".encode("utf-8", 'windows-874') }
    assert_raise(Encoding::UndefinedConversionError) { "\xDE".encode("utf-8", 'windows-874') }
    check_both_ways("\u0E3F", "\xDF", 'windows-874') # ฿
    check_both_ways("\u0E40", "\xE0", 'windows-874') # เ
    check_both_ways("\u0E4F", "\xEF", 'windows-874') # ๏
    check_both_ways("\u0E50", "\xF0", 'windows-874') # ๐
    check_both_ways("\u0E5B", "\xFB", 'windows-874') # ๛
    assert_raise(Encoding::UndefinedConversionError) { "\xFC".encode("utf-8", 'windows-874') }
    assert_raise(Encoding::UndefinedConversionError) { "\xFF".encode("utf-8", 'windows-874') }
  end

  def test_windows_1250
    check_both_ways("\u20AC", "\x80", 'windows-1250') # €
    assert_raise(Encoding::UndefinedConversionError) { "\x81".encode("utf-8", 'windows-1250') }
    check_both_ways("\u201A", "\x82", 'windows-1250') # ‚
    assert_raise(Encoding::UndefinedConversionError) { "\x83".encode("utf-8", 'windows-1250') }
    check_both_ways("\u201E", "\x84", 'windows-1250') # „
    check_both_ways("\u2021", "\x87", 'windows-1250') # ‡
    assert_raise(Encoding::UndefinedConversionError) { "\x88".encode("utf-8", 'windows-1250') }
    check_both_ways("\u2030", "\x89", 'windows-1250') # ‰
    check_both_ways("\u0179", "\x8F", 'windows-1250') # Ź
    assert_raise(Encoding::UndefinedConversionError) { "\x90".encode("utf-8", 'windows-1250') }
    check_both_ways("\u2018", "\x91", 'windows-1250') # ‘
    check_both_ways("\u2014", "\x97", 'windows-1250') # —
    assert_raise(Encoding::UndefinedConversionError) { "\x98".encode("utf-8", 'windows-1250') }
    check_both_ways("\u2122", "\x99", 'windows-1250') # ™
    check_both_ways("\u00A0", "\xA0", 'windows-1250') # non-breaking space
    check_both_ways("\u017B", "\xAF", 'windows-1250') # Ż
    check_both_ways("\u00B0", "\xB0", 'windows-1250') # °
    check_both_ways("\u017C", "\xBF", 'windows-1250') # ż
    check_both_ways("\u0154", "\xC0", 'windows-1250') # Ŕ
    check_both_ways("\u010E", "\xCF", 'windows-1250') # Ď
    check_both_ways("\u0110", "\xD0", 'windows-1250') # Đ
    check_both_ways("\u00DF", "\xDF", 'windows-1250') # ß
    check_both_ways("\u0155", "\xE0", 'windows-1250') # ŕ
    check_both_ways("\u010F", "\xEF", 'windows-1250') # ď
    check_both_ways("\u0111", "\xF0", 'windows-1250') # đ
    check_both_ways("\u02D9", "\xFF", 'windows-1250') # ˙
  end

  def test_windows_1251
    check_both_ways("\u0402", "\x80", 'windows-1251') # Ђ
    check_both_ways("\u20AC", "\x88", 'windows-1251') # €
    check_both_ways("\u040F", "\x8F", 'windows-1251') # Џ
    check_both_ways("\u0452", "\x90", 'windows-1251') # ђ
    assert_raise(Encoding::UndefinedConversionError) { "\x98".encode("utf-8", 'windows-1251') }
    check_both_ways("\u045F", "\x9F", 'windows-1251') # џ
    check_both_ways("\u00A0", "\xA0", 'windows-1251') # non-breaking space
    check_both_ways("\u0407", "\xAF", 'windows-1251') # Ї
    check_both_ways("\u00B0", "\xB0", 'windows-1251') # °
    check_both_ways("\u0457", "\xBF", 'windows-1251') # ї
    check_both_ways("\u0410", "\xC0", 'windows-1251') # А
    check_both_ways("\u041F", "\xCF", 'windows-1251') # П
    check_both_ways("\u0420", "\xD0", 'windows-1251') # Р
    check_both_ways("\u042F", "\xDF", 'windows-1251') # Я
    check_both_ways("\u0430", "\xE0", 'windows-1251') # а
    check_both_ways("\u043F", "\xEF", 'windows-1251') # п
    check_both_ways("\u0440", "\xF0", 'windows-1251') # р
    check_both_ways("\u044F", "\xFF", 'windows-1251') # я
  end

  def test_windows_1252
    check_both_ways("\u20AC", "\x80", 'windows-1252') # €
    assert_raise(Encoding::UndefinedConversionError) { "\x81".encode("utf-8", 'windows-1252') }
    check_both_ways("\u201A", "\x82", 'windows-1252') # ‚
    check_both_ways("\u0152", "\x8C", 'windows-1252') # >Œ
    assert_raise(Encoding::UndefinedConversionError) { "\x8D".encode("utf-8", 'windows-1252') }
    check_both_ways("\u017D", "\x8E", 'windows-1252') # Ž
    assert_raise(Encoding::UndefinedConversionError) { "\x8F".encode("utf-8", 'windows-1252') }
    assert_raise(Encoding::UndefinedConversionError) { "\x90".encode("utf-8", 'windows-1252') }
    check_both_ways("\u2018", "\x91", 'windows-1252') #‘
    check_both_ways("\u0153", "\x9C", 'windows-1252') # œ
    assert_raise(Encoding::UndefinedConversionError) { "\x9D".encode("utf-8", 'windows-1252') }
    check_both_ways("\u017E", "\x9E", 'windows-1252') # ž
    check_both_ways("\u00A0", "\xA0", 'windows-1252') # non-breaking space
    check_both_ways("\u00AF", "\xAF", 'windows-1252') # ¯
    check_both_ways("\u00B0", "\xB0", 'windows-1252') # °
    check_both_ways("\u00BF", "\xBF", 'windows-1252') # ¿
    check_both_ways("\u00C0", "\xC0", 'windows-1252') # À
    check_both_ways("\u00CF", "\xCF", 'windows-1252') # Ï
    check_both_ways("\u00D0", "\xD0", 'windows-1252') # Ð
    check_both_ways("\u00DF", "\xDF", 'windows-1252') # ß
    check_both_ways("\u00E0", "\xE0", 'windows-1252') # à
    check_both_ways("\u00EF", "\xEF", 'windows-1252') # ï
    check_both_ways("\u00F0", "\xF0", 'windows-1252') # ð
    check_both_ways("\u00FF", "\xFF", 'windows-1252') # ÿ
  end

  def test_windows_1253
    check_both_ways("\u20AC", "\x80", 'windows-1253') # €
    assert_raise(Encoding::UndefinedConversionError) { "\x81".encode("utf-8", 'windows-1253') }
    check_both_ways("\u201A", "\x82", 'windows-1253') # ‚
    check_both_ways("\u2021", "\x87", 'windows-1253') # ‡
    assert_raise(Encoding::UndefinedConversionError) { "\x88".encode("utf-8", 'windows-1253') }
    check_both_ways("\u2030", "\x89", 'windows-1253') # ‰
    assert_raise(Encoding::UndefinedConversionError) { "\x8A".encode("utf-8", 'windows-1253') }
    check_both_ways("\u2039", "\x8B", 'windows-1253') # ‹
    assert_raise(Encoding::UndefinedConversionError) { "\x8C".encode("utf-8", 'windows-1253') }
    assert_raise(Encoding::UndefinedConversionError) { "\x8F".encode("utf-8", 'windows-1253') }
    assert_raise(Encoding::UndefinedConversionError) { "\x90".encode("utf-8", 'windows-1253') }
    check_both_ways("\u2018", "\x91", 'windows-1253') # ‘
    check_both_ways("\u2014", "\x97", 'windows-1253') # —
    assert_raise(Encoding::UndefinedConversionError) { "\x98".encode("utf-8", 'windows-1253') }
    check_both_ways("\u2122", "\x99", 'windows-1253') # ™
    assert_raise(Encoding::UndefinedConversionError) { "\x9A".encode("utf-8", 'windows-1253') }
    check_both_ways("\u203A", "\x9B", 'windows-1253') # ›
    assert_raise(Encoding::UndefinedConversionError) { "\x9C".encode("utf-8", 'windows-1253') }
    assert_raise(Encoding::UndefinedConversionError) { "\x9F".encode("utf-8", 'windows-1253') }
    check_both_ways("\u00A0", "\xA0", 'windows-1253') # non-breaking space
    check_both_ways("\u2015", "\xAF", 'windows-1253') # ―
    check_both_ways("\u00B0", "\xB0", 'windows-1253') # °
    check_both_ways("\u038F", "\xBF", 'windows-1253') # Ώ
    check_both_ways("\u0390", "\xC0", 'windows-1253') # ΐ
    check_both_ways("\u039F", "\xCF", 'windows-1253') # Ο
    check_both_ways("\u03A0", "\xD0", 'windows-1253') # Π
    check_both_ways("\u03A1", "\xD1", 'windows-1253') # Ρ
    assert_raise(Encoding::UndefinedConversionError) { "\xD2".encode("utf-8", 'windows-1253') }
    check_both_ways("\u03A3", "\xD3", 'windows-1253') # Σ
    check_both_ways("\u03AF", "\xDF", 'windows-1253') # ί
    check_both_ways("\u03B0", "\xE0", 'windows-1253') # ΰ
    check_both_ways("\u03BF", "\xEF", 'windows-1253') # ο
    check_both_ways("\u03C0", "\xF0", 'windows-1253') # π
    check_both_ways("\u03CE", "\xFE", 'windows-1253') # ώ
    assert_raise(Encoding::UndefinedConversionError) { "\xFF".encode("utf-8", 'windows-1253') }
  end

  def test_windows_1254
    check_both_ways("\u20AC", "\x80", 'windows-1254') # €
    assert_raise(Encoding::UndefinedConversionError) { "\x81".encode("utf-8", 'windows-1254') }
    check_both_ways("\u201A", "\x82", 'windows-1254') # ‚
    check_both_ways("\u0152", "\x8C", 'windows-1254') # Œ
    assert_raise(Encoding::UndefinedConversionError) { "\x8D".encode("utf-8", 'windows-1254') }
    assert_raise(Encoding::UndefinedConversionError) { "\x8F".encode("utf-8", 'windows-1254') }
    assert_raise(Encoding::UndefinedConversionError) { "\x90".encode("utf-8", 'windows-1254') }
    check_both_ways("\u2018", "\x91", 'windows-1254') # ‘
    check_both_ways("\u0153", "\x9C", 'windows-1254') # œ
    assert_raise(Encoding::UndefinedConversionError) { "\x9D".encode("utf-8", 'windows-1254') }
    assert_raise(Encoding::UndefinedConversionError) { "\x9E".encode("utf-8", 'windows-1254') }
    check_both_ways("\u0178", "\x9F", 'windows-1254') # Ÿ
    check_both_ways("\u00A0", "\xA0", 'windows-1254') # non-breaking space
    check_both_ways("\u00AF", "\xAF", 'windows-1254') # ¯
    check_both_ways("\u00B0", "\xB0", 'windows-1254') # °
    check_both_ways("\u00BF", "\xBF", 'windows-1254') # ¿
    check_both_ways("\u00C0", "\xC0", 'windows-1254') # À
    check_both_ways("\u00CF", "\xCF", 'windows-1254') # Ï
    check_both_ways("\u011E", "\xD0", 'windows-1254') # Ğ
    check_both_ways("\u00DF", "\xDF", 'windows-1254') # ß
    check_both_ways("\u00E0", "\xE0", 'windows-1254') # à
    check_both_ways("\u00EF", "\xEF", 'windows-1254') # ï
    check_both_ways("\u011F", "\xF0", 'windows-1254') # ğ
    check_both_ways("\u00FF", "\xFF", 'windows-1254') # ÿ
  end

  def test_windows_1255
    check_both_ways("\u20AC", "\x80", 'windows-1255') # €
    assert_raise(Encoding::UndefinedConversionError) { "\x81".encode("utf-8", 'windows-1255') }
    check_both_ways("\u201A", "\x82", 'windows-1255') # ‚
    check_both_ways("\u2030", "\x89", 'windows-1255') # ‰
    assert_raise(Encoding::UndefinedConversionError) { "\x8A".encode("utf-8", 'windows-1255') }
    check_both_ways("\u2039", "\x8B", 'windows-1255') # ‹
    assert_raise(Encoding::UndefinedConversionError) { "\x8C".encode("utf-8", 'windows-1255') }
    assert_raise(Encoding::UndefinedConversionError) { "\x8F".encode("utf-8", 'windows-1255') }
    assert_raise(Encoding::UndefinedConversionError) { "\x90".encode("utf-8", 'windows-1255') }
    check_both_ways("\u2018", "\x91", 'windows-1255') # ‘
    check_both_ways("\u2122", "\x99", 'windows-1255') # ™
    assert_raise(Encoding::UndefinedConversionError) { "\x9A".encode("utf-8", 'windows-1255') }
    check_both_ways("\u203A", "\x9B", 'windows-1255') # ›
    assert_raise(Encoding::UndefinedConversionError) { "\x9C".encode("utf-8", 'windows-1255') }
    assert_raise(Encoding::UndefinedConversionError) { "\x9F".encode("utf-8", 'windows-1255') }
    check_both_ways("\u00A0", "\xA0", 'windows-1255') # non-breaking space
    check_both_ways("\u00A1", "\xA1", 'windows-1255') # ¡
    check_both_ways("\u00D7", "\xAA", 'windows-1255') # ×
    check_both_ways("\u00AF", "\xAF", 'windows-1255') # ¯
    check_both_ways("\u00B0", "\xB0", 'windows-1255') # °
    check_both_ways("\u00B8", "\xB8", 'windows-1255') # ¸
    check_both_ways("\u00F7", "\xBA", 'windows-1255') # ÷
    check_both_ways("\u00BF", "\xBF", 'windows-1255') # ¿
    check_both_ways("\u05B0", "\xC0", 'windows-1255') # ְ
    check_both_ways("\u05B9", "\xC9", 'windows-1255') # ֹ
    assert_raise(Encoding::UndefinedConversionError) { "\xCA".encode("utf-8", 'windows-1255') }
    check_both_ways("\u05BB", "\xCB", 'windows-1255') # ֻ
    check_both_ways("\u05BF", "\xCF", 'windows-1255') # ֿ
    check_both_ways("\u05C0", "\xD0", 'windows-1255') # ׀
    check_both_ways("\u05F3", "\xD7", 'windows-1255') # ׳
    check_both_ways("\u05F4", "\xD8", 'windows-1255') # ״
    assert_raise(Encoding::UndefinedConversionError) { "\xD9".encode("utf-8", 'windows-1255') }
    assert_raise(Encoding::UndefinedConversionError) { "\xDF".encode("utf-8", 'windows-1255') }
    check_both_ways("\u05D0", "\xE0", 'windows-1255') # א
    check_both_ways("\u05DF", "\xEF", 'windows-1255') # ן
    check_both_ways("\u05E0", "\xF0", 'windows-1255') # נ
    check_both_ways("\u05EA", "\xFA", 'windows-1255') # ת
    assert_raise(Encoding::UndefinedConversionError) { "\xFB".encode("utf-8", 'windows-1255') }
    assert_raise(Encoding::UndefinedConversionError) { "\xFC".encode("utf-8", 'windows-1255') }
    check_both_ways("\u200E", "\xFD", 'windows-1255') # left-to-right mark
    check_both_ways("\u200F", "\xFE", 'windows-1255') # right-to-left mark
    assert_raise(Encoding::UndefinedConversionError) { "\xFF".encode("utf-8", 'windows-1255') }
  end

  def test_windows_1256
    check_both_ways("\u20AC", "\x80", 'windows-1256') # €
    check_both_ways("\u0679", "\x8A", 'windows-1256') # ٹ
    check_both_ways("\u0688", "\x8F", 'windows-1256') # ڈ
    check_both_ways("\u06AF", "\x90", 'windows-1256') # گ
    check_both_ways("\u06A9", "\x98", 'windows-1256') # ک
    check_both_ways("\u0691", "\x9A", 'windows-1256') # ڑ
    check_both_ways("\u06BA", "\x9F", 'windows-1256') # ں
    check_both_ways("\u00A0", "\xA0", 'windows-1256') # non-breaking space
    check_both_ways("\u06BE", "\xAA", 'windows-1256') # ھ
    check_both_ways("\u00AF", "\xAF", 'windows-1256') # ¯
    check_both_ways("\u00B0", "\xB0", 'windows-1256') # °
    check_both_ways("\u061F", "\xBF", 'windows-1256') # ؟
    check_both_ways("\u06C1", "\xC0", 'windows-1256') # ہ
    check_both_ways("\u062F", "\xCF", 'windows-1256') # د
    check_both_ways("\u0630", "\xD0", 'windows-1256') # ذ
    check_both_ways("\u0643", "\xDF", 'windows-1256') # ك
    check_both_ways("\u00E0", "\xE0", 'windows-1256') # à
    check_both_ways("\u00EF", "\xEF", 'windows-1256') # ï
    check_both_ways("\u064B", "\xF0", 'windows-1256') #  ًً
    check_both_ways("\u06D2", "\xFF", 'windows-1256') # ے
  end

  def test_windows_1257
    check_both_ways("\u20AC", "\x80", 'windows-1257') # €
    assert_raise(Encoding::UndefinedConversionError) { "\x81".encode("utf-8", 'windows-1257') }
    check_both_ways("\u201A", "\x82", 'windows-1257') # ‚
    assert_raise(Encoding::UndefinedConversionError) { "\x83".encode("utf-8", 'windows-1257') }
    check_both_ways("\u201E", "\x84", 'windows-1257') # „
    check_both_ways("\u2021", "\x87", 'windows-1257') # ‡
    assert_raise(Encoding::UndefinedConversionError) { "\x88".encode("utf-8", 'windows-1257') }
    check_both_ways("\u2030", "\x89", 'windows-1257') # ‰
    assert_raise(Encoding::UndefinedConversionError) { "\x8A".encode("utf-8", 'windows-1257') }
    check_both_ways("\u2039", "\x8B", 'windows-1257') # ‹
    assert_raise(Encoding::UndefinedConversionError) { "\x8C".encode("utf-8", 'windows-1257') }
    check_both_ways("\u00A8", "\x8D", 'windows-1257') # ¨
    check_both_ways("\u02C7", "\x8E", 'windows-1257') # ˇ
    check_both_ways("\u00B8", "\x8F", 'windows-1257') # ¸
    assert_raise(Encoding::UndefinedConversionError) { "\x90".encode("utf-8", 'windows-1257') }
    check_both_ways("\u2018", "\x91", 'windows-1257') # ‘
    check_both_ways("\u2014", "\x97", 'windows-1257') # —
    assert_raise(Encoding::UndefinedConversionError) { "\x98".encode("utf-8", 'windows-1257') }
    check_both_ways("\u2122", "\x99", 'windows-1257') # ™
    assert_raise(Encoding::UndefinedConversionError) { "\x9A".encode("utf-8", 'windows-1257') }
    check_both_ways("\u203A", "\x9B", 'windows-1257') # ›
    assert_raise(Encoding::UndefinedConversionError) { "\x9C".encode("utf-8", 'windows-1257') }
    check_both_ways("\u00AF", "\x9D", 'windows-1257') # ¯
    check_both_ways("\u02DB", "\x9E", 'windows-1257') # ˛
    assert_raise(Encoding::UndefinedConversionError) { "\x9F".encode("utf-8", 'windows-1257') }
    check_both_ways("\u00A0", "\xA0", 'windows-1257') # non-breaking space
    assert_raise(Encoding::UndefinedConversionError) { "\xA1".encode("utf-8", 'windows-1257') }
    check_both_ways("\u00A2", "\xA2", 'windows-1257') # ¢
    check_both_ways("\u00A4", "\xA4", 'windows-1257') # ¤
    assert_raise(Encoding::UndefinedConversionError) { "\xA5".encode("utf-8", 'windows-1257') }
    check_both_ways("\u00A6", "\xA6", 'windows-1257') # ¦
    check_both_ways("\u00C6", "\xAF", 'windows-1257') # Æ
    check_both_ways("\u00B0", "\xB0", 'windows-1257') # °
    check_both_ways("\u00E6", "\xBF", 'windows-1257') # æ
    check_both_ways("\u0104", "\xC0", 'windows-1257') # Ą
    check_both_ways("\u013B", "\xCF", 'windows-1257') # Ļ
    check_both_ways("\u0160", "\xD0", 'windows-1257') # Š
    check_both_ways("\u00DF", "\xDF", 'windows-1257') # ß
    check_both_ways("\u0105", "\xE0", 'windows-1257') # ą
    check_both_ways("\u013C", "\xEF", 'windows-1257') # ļ
    check_both_ways("\u0161", "\xF0", 'windows-1257') # š
    check_both_ways("\u02D9", "\xFF", 'windows-1257') # ˙
  end

  def test_IBM437
    check_both_ways("\u00C7", "\x80", 'IBM437') # Ç
    check_both_ways("\u00C5", "\x8F", 'IBM437') # Å
    check_both_ways("\u00C9", "\x90", 'IBM437') # É
    check_both_ways("\u0192", "\x9F", 'IBM437') # ƒ
    check_both_ways("\u00E1", "\xA0", 'IBM437') # á
    check_both_ways("\u00BB", "\xAF", 'IBM437') # »
    check_both_ways("\u2591", "\xB0", 'IBM437') # ░
    check_both_ways("\u2510", "\xBF", 'IBM437') # ┐
    check_both_ways("\u2514", "\xC0", 'IBM437') # └
    check_both_ways("\u2567", "\xCF", 'IBM437') # ╧
    check_both_ways("\u2568", "\xD0", 'IBM437') # ╨
    check_both_ways("\u2580", "\xDF", 'IBM437') # ▀
    check_both_ways("\u03B1", "\xE0", 'IBM437') # α
    check_both_ways("\u2229", "\xEF", 'IBM437') # ∩
    check_both_ways("\u2261", "\xF0", 'IBM437') # ≡
    check_both_ways("\u00A0", "\xFF", 'IBM437') # non-breaking space
  end

  def test_IBM775
    check_both_ways("\u0106", "\x80", 'IBM775') # Ć
    check_both_ways("\u00C5", "\x8F", 'IBM775') # Å
    check_both_ways("\u00C9", "\x90", 'IBM775') # É
    check_both_ways("\u00A4", "\x9F", 'IBM775') # ¤
    check_both_ways("\u0100", "\xA0", 'IBM775') # Ā
    check_both_ways("\u00BB", "\xAF", 'IBM775') # »
    check_both_ways("\u2591", "\xB0", 'IBM775') # ░
    check_both_ways("\u2510", "\xBF", 'IBM775') # ┐
    check_both_ways("\u2514", "\xC0", 'IBM775') # └
    check_both_ways("\u017D", "\xCF", 'IBM775') # Ž
    check_both_ways("\u0105", "\xD0", 'IBM775') # ą
    check_both_ways("\u2580", "\xDF", 'IBM775') # ▀
    check_both_ways("\u00D3", "\xE0", 'IBM775') # Ó
    check_both_ways("\u2019", "\xEF", 'IBM775') # ’
    check_both_ways("\u00AD", "\xF0", 'IBM775') # osft hyphen
    check_both_ways("\u00A0", "\xFF", 'IBM775') # non-breaking space
  end

  def test_IBM852
    check_both_ways("\u00C7", "\x80", 'IBM852') # Ç
    check_both_ways("\u0106", "\x8F", 'IBM852') # Ć
    check_both_ways("\u00C9", "\x90", 'IBM852') # É
    check_both_ways("\u010D", "\x9F", 'IBM852') # č
    check_both_ways("\u00E1", "\xA0", 'IBM852') # á
    check_both_ways("\u00BB", "\xAF", 'IBM852') # »
    check_both_ways("\u2591", "\xB0", 'IBM852') # ░
    check_both_ways("\u2510", "\xBF", 'IBM852') # ┐
    check_both_ways("\u2514", "\xC0", 'IBM852') # └
    check_both_ways("\u00A4", "\xCF", 'IBM852') # ¤
    check_both_ways("\u0111", "\xD0", 'IBM852') # đ
    check_both_ways("\u2580", "\xDF", 'IBM852') # ▀
    check_both_ways("\u00D3", "\xE0", 'IBM852') # Ó
    check_both_ways("\u00B4", "\xEF", 'IBM852') # ´
    check_both_ways("\u00AD", "\xF0", 'IBM852') # osft hyphen
    check_both_ways("\u00A0", "\xFF", 'IBM852') # non-breaking space
  end

  def test_IBM855
    check_both_ways("\u0452", "\x80", 'IBM855') # ђ
    check_both_ways("\u0408", "\x8F", 'IBM855') # Ј
    check_both_ways("\u0459", "\x90", 'IBM855') # љ
    check_both_ways("\u042A", "\x9F", 'IBM855') # Ъ
    check_both_ways("\u0430", "\xA0", 'IBM855') # а
    check_both_ways("\u00BB", "\xAF", 'IBM855') # »
    check_both_ways("\u2591", "\xB0", 'IBM855') # ░
    check_both_ways("\u2510", "\xBF", 'IBM855') # ┐
    check_both_ways("\u2514", "\xC0", 'IBM855') # └
    check_both_ways("\u00A4", "\xCF", 'IBM855') # ¤
    check_both_ways("\u043B", "\xD0", 'IBM855') # л
    check_both_ways("\u2580", "\xDF", 'IBM855') # ▀
    check_both_ways("\u042F", "\xE0", 'IBM855') # Я
    check_both_ways("\u2116", "\xEF", 'IBM855') # №
    check_both_ways("\u00AD", "\xF0", 'IBM855') # osft hyphen
    check_both_ways("\u00A0", "\xFF", 'IBM855') # non-breaking space
  end

  def test_IBM857
    check_both_ways("\u00C7", "\x80", 'IBM857') # Ç
    check_both_ways("\u00C5", "\x8F", 'IBM857') # Å
    check_both_ways("\u00C9", "\x90", 'IBM857') # É
    check_both_ways("\u015F", "\x9F", 'IBM857') # ş
    check_both_ways("\u00E1", "\xA0", 'IBM857') # á
    check_both_ways("\u00BB", "\xAF", 'IBM857') # »
    check_both_ways("\u2591", "\xB0", 'IBM857') # ░
    check_both_ways("\u2510", "\xBF", 'IBM857') # ┐
    check_both_ways("\u2514", "\xC0", 'IBM857') # └
    check_both_ways("\u00A4", "\xCF", 'IBM857') # ¤
    check_both_ways("\u00BA", "\xD0", 'IBM857') # º
    check_both_ways("\u00C8", "\xD4", 'IBM857') # È
    assert_raise(Encoding::UndefinedConversionError) { "\xD5".encode("utf-8", 'IBM857') }
    check_both_ways("\u00CD", "\xD6", 'IBM857') # Í
    check_both_ways("\u2580", "\xDF", 'IBM857') # ▀
    check_both_ways("\u00D3", "\xE0", 'IBM857') # Ó
    check_both_ways("\u00B5", "\xE6", 'IBM857') # µ
    assert_raise(Encoding::UndefinedConversionError) { "\xE7".encode("utf-8", 'IBM857') }
    check_both_ways("\u00D7", "\xE8", 'IBM857') # ×
    check_both_ways("\u00B4", "\xEF", 'IBM857') # ´
    check_both_ways("\u00AD", "\xF0", 'IBM857') # soft hyphen
    check_both_ways("\u00B1", "\xF1", 'IBM857') # ±
    assert_raise(Encoding::UndefinedConversionError) { "\xF2".encode("utf-8", 'IBM857') }
    check_both_ways("\u00BE", "\xF3", 'IBM857') # ¾
    check_both_ways("\u00A0", "\xFF", 'IBM857') # non-breaking space
  end

  def test_IBM860
    check_both_ways("\u00C7", "\x80", 'IBM860') # Ç
    check_both_ways("\u00C2", "\x8F", 'IBM860') # Â
    check_both_ways("\u00C9", "\x90", 'IBM860') # É
    check_both_ways("\u00D3", "\x9F", 'IBM860') # Ó
    check_both_ways("\u00E1", "\xA0", 'IBM860') # á
    check_both_ways("\u00BB", "\xAF", 'IBM860') # »
    check_both_ways("\u2591", "\xB0", 'IBM860') # ░
    check_both_ways("\u2510", "\xBF", 'IBM860') # ┐
    check_both_ways("\u2514", "\xC0", 'IBM860') # └
    check_both_ways("\u2567", "\xCF", 'IBM860') # ╧
    check_both_ways("\u2568", "\xD0", 'IBM860') # ╨
    check_both_ways("\u2580", "\xDF", 'IBM860') # ▀
    check_both_ways("\u03B1", "\xE0", 'IBM860') # α
    check_both_ways("\u2229", "\xEF", 'IBM860') # ∩
    check_both_ways("\u2261", "\xF0", 'IBM860') # ≡
    check_both_ways("\u00A0", "\xFF", 'IBM860') # non-breaking space
  end

  def test_IBM861
    check_both_ways("\u00C7", "\x80", 'IBM861') # Ç
    check_both_ways("\u00C5", "\x8F", 'IBM861') # Å
    check_both_ways("\u00C9", "\x90", 'IBM861') # É
    check_both_ways("\u0192", "\x9F", 'IBM861') # ƒ
    check_both_ways("\u00E1", "\xA0", 'IBM861') # á
    check_both_ways("\u00BB", "\xAF", 'IBM861') # »
    check_both_ways("\u2591", "\xB0", 'IBM861') # ░
    check_both_ways("\u2510", "\xBF", 'IBM861') # ┐
    check_both_ways("\u2514", "\xC0", 'IBM861') # └
    check_both_ways("\u2567", "\xCF", 'IBM861') # ╧
    check_both_ways("\u2568", "\xD0", 'IBM861') # ╨
    check_both_ways("\u2580", "\xDF", 'IBM861') # ▀
    check_both_ways("\u03B1", "\xE0", 'IBM861') # α
    check_both_ways("\u2229", "\xEF", 'IBM861') # ∩
    check_both_ways("\u2261", "\xF0", 'IBM861') # ≡
    check_both_ways("\u00A0", "\xFF", 'IBM861') # non-breaking space
  end

  def test_IBM862
    check_both_ways("\u05D0", "\x80", 'IBM862') # א
    check_both_ways("\u05DF", "\x8F", 'IBM862') # ן
    check_both_ways("\u05E0", "\x90", 'IBM862') # נ
    check_both_ways("\u0192", "\x9F", 'IBM862') # ƒ
    check_both_ways("\u00E1", "\xA0", 'IBM862') # á
    check_both_ways("\u00BB", "\xAF", 'IBM862') # »
    check_both_ways("\u2591", "\xB0", 'IBM862') # ░
    check_both_ways("\u2510", "\xBF", 'IBM862') # ┐
    check_both_ways("\u2514", "\xC0", 'IBM862') # └
    check_both_ways("\u2567", "\xCF", 'IBM862') # ╧
    check_both_ways("\u2568", "\xD0", 'IBM862') # ╨
    check_both_ways("\u2580", "\xDF", 'IBM862') # ▀
    check_both_ways("\u03B1", "\xE0", 'IBM862') # α
    check_both_ways("\u2229", "\xEF", 'IBM862') # ∩
    check_both_ways("\u2261", "\xF0", 'IBM862') # ≡
    check_both_ways("\u00A0", "\xFF", 'IBM862') # non-breaking space
  end

  def test_IBM863
    check_both_ways("\u00C7", "\x80", 'IBM863') # Ç
    check_both_ways("\u00A7", "\x8F", 'IBM863') # §
    check_both_ways("\u00C9", "\x90", 'IBM863') # É
    check_both_ways("\u0192", "\x9F", 'IBM863') # ƒ
    check_both_ways("\u00A6", "\xA0", 'IBM863') # ¦
    check_both_ways("\u00BB", "\xAF", 'IBM863') # »
    check_both_ways("\u2591", "\xB0", 'IBM863') # ░
    check_both_ways("\u2510", "\xBF", 'IBM863') # ┐
    check_both_ways("\u2514", "\xC0", 'IBM863') # └
    check_both_ways("\u2567", "\xCF", 'IBM863') # ╧
    check_both_ways("\u2568", "\xD0", 'IBM863') # ╨
    check_both_ways("\u2580", "\xDF", 'IBM863') # ▀
    check_both_ways("\u03B1", "\xE0", 'IBM863') # α
    check_both_ways("\u2229", "\xEF", 'IBM863') # ∩
    check_both_ways("\u2261", "\xF0", 'IBM863') # ≡
    check_both_ways("\u00A0", "\xFF", 'IBM863') # non-breaking space
  end

  def test_IBM865
    check_both_ways("\u00C7", "\x80", 'IBM865') # Ç
    check_both_ways("\u00C5", "\x8F", 'IBM865') # Å
    check_both_ways("\u00C9", "\x90", 'IBM865') # É
    check_both_ways("\u0192", "\x9F", 'IBM865') # ƒ
    check_both_ways("\u00E1", "\xA0", 'IBM865') # á
    check_both_ways("\u00A4", "\xAF", 'IBM865') # ¤
    check_both_ways("\u2591", "\xB0", 'IBM865') # ░
    check_both_ways("\u2510", "\xBF", 'IBM865') # ┐
    check_both_ways("\u2514", "\xC0", 'IBM865') # └
    check_both_ways("\u2567", "\xCF", 'IBM865') # ╧
    check_both_ways("\u2568", "\xD0", 'IBM865') # ╨
    check_both_ways("\u2580", "\xDF", 'IBM865') # ▀
    check_both_ways("\u03B1", "\xE0", 'IBM865') # α
    check_both_ways("\u2229", "\xEF", 'IBM865') # ∩
    check_both_ways("\u2261", "\xF0", 'IBM865') # ≡
    check_both_ways("\u00A0", "\xFF", 'IBM865') # non-breaking space
  end

  def test_IBM866
    check_both_ways("\u0410", "\x80", 'IBM866') # А
    check_both_ways("\u041F", "\x8F", 'IBM866') # П
    check_both_ways("\u0420", "\x90", 'IBM866') # Р
    check_both_ways("\u042F", "\x9F", 'IBM866') # Я
    check_both_ways("\u0430", "\xA0", 'IBM866') # а
    check_both_ways("\u043F", "\xAF", 'IBM866') # п
    check_both_ways("\u2591", "\xB0", 'IBM866') # ░
    check_both_ways("\u2510", "\xBF", 'IBM866') # ┐
    check_both_ways("\u2514", "\xC0", 'IBM866') # └
    check_both_ways("\u2567", "\xCF", 'IBM866') # ╧
    check_both_ways("\u2568", "\xD0", 'IBM866') # ╨
    check_both_ways("\u2580", "\xDF", 'IBM866') # ▀
    check_both_ways("\u0440", "\xE0", 'IBM866') # р
    check_both_ways("\u044F", "\xEF", 'IBM866') # я
    check_both_ways("\u0401", "\xF0", 'IBM866') # Ё
    check_both_ways("\u00A0", "\xFF", 'IBM866') # non-breaking space
  end

  def test_IBM869
    assert_raise(Encoding::UndefinedConversionError) { "\x80".encode("utf-8", 'IBM869') }
    assert_raise(Encoding::UndefinedConversionError) { "\x85".encode("utf-8", 'IBM869') }
    check_both_ways("\u0386", "\x86", 'IBM869') # Ά
    assert_raise(Encoding::UndefinedConversionError) { "\x87".encode("utf-8", 'IBM869') }
    check_both_ways("\u00B7", "\x88", 'IBM869') # ·
    check_both_ways("\u0389", "\x8F", 'IBM869') # Ή
    check_both_ways("\u038A", "\x90", 'IBM869') # Ί
    check_both_ways("\u038C", "\x92", 'IBM869') # Ό
    assert_raise(Encoding::UndefinedConversionError) { "\x93".encode("utf-8", 'IBM869') }
    assert_raise(Encoding::UndefinedConversionError) { "\x94".encode("utf-8", 'IBM869') }
    check_both_ways("\u038E", "\x95", 'IBM869') # Ύ
    check_both_ways("\u03AF", "\x9F", 'IBM869') # ί
    check_both_ways("\u03CA", "\xA0", 'IBM869') # ϊ
    check_both_ways("\u00BB", "\xAF", 'IBM869') # »
    check_both_ways("\u2591", "\xB0", 'IBM869') # ░
    check_both_ways("\u2510", "\xBF", 'IBM869') # ┐
    check_both_ways("\u2514", "\xC0", 'IBM869') # └
    check_both_ways("\u03A3", "\xCF", 'IBM869') # Σ
    check_both_ways("\u03A4", "\xD0", 'IBM869') # Τ
    check_both_ways("\u2580", "\xDF", 'IBM869') # ▀
    check_both_ways("\u03B6", "\xE0", 'IBM869') # ζ
    check_both_ways("\u0384", "\xEF", 'IBM869') # ΄
    check_both_ways("\u00AD", "\xF0", 'IBM869') # soft hyphen
    check_both_ways("\u00A0", "\xFF", 'IBM869') # non-breaking space
  end

  def test_macCroatian
    check_both_ways("\u00C4", "\x80", 'macCroatian') # Ä
    check_both_ways("\u00E8", "\x8F", 'macCroatian') # è
    check_both_ways("\u00EA", "\x90", 'macCroatian') # ê
    check_both_ways("\u00FC", "\x9F", 'macCroatian') # ü
    check_both_ways("\u2020", "\xA0", 'macCroatian') # †
    check_both_ways("\u00D8", "\xAF", 'macCroatian') # Ø
    check_both_ways("\u221E", "\xB0", 'macCroatian') # ∞
    check_both_ways("\u00F8", "\xBF", 'macCroatian') # ø
    check_both_ways("\u00BF", "\xC0", 'macCroatian') # ¿
    check_both_ways("\u0153", "\xCF", 'macCroatian') # œ
    check_both_ways("\u0110", "\xD0", 'macCroatian') # Đ
    check_both_ways("\u00A9", "\xD9", 'macCroatian') # ©
    check_both_ways("\u2044", "\xDA", 'macCroatian') # ⁄
    check_both_ways("\u203A", "\xDD", 'macCroatian') # ›
    check_both_ways("\u00C6", "\xDE", 'macCroatian') # Æ
    check_both_ways("\u00BB", "\xDF", 'macCroatian') # »
    check_both_ways("\u2013", "\xE0", 'macCroatian') # –
    check_both_ways("\u00B7", "\xE1", 'macCroatian') # ·
    check_both_ways("\u00C2", "\xE5", 'macCroatian') # Â
    check_both_ways("\u0107", "\xE6", 'macCroatian') # ć
    check_both_ways("\u00C1", "\xE7", 'macCroatian') # Á
    check_both_ways("\u010D", "\xE8", 'macCroatian') # č
    check_both_ways("\u00C8", "\xE9", 'macCroatian') # È
    check_both_ways("\u00D4", "\xEF", 'macCroatian') # Ô
    check_both_ways("\u0111", "\xF0", 'macCroatian') # đ
    check_both_ways("\u00D2", "\xF1", 'macCroatian') # Ò
    check_both_ways("\u00AF", "\xF8", 'macCroatian') # ¯
    check_both_ways("\u03C0", "\xF9", 'macCroatian') # π
    check_both_ways("\u00CB", "\xFA", 'macCroatian') # Ë
    check_both_ways("\u00CA", "\xFD", 'macCroatian') # Ê
    check_both_ways("\u00E6", "\xFE", 'macCroatian') # æ
    check_both_ways("\u02C7", "\xFF", 'macCroatian') # ˇ
  end

  def test_macCyrillic
    check_both_ways("\u0410", "\x80", 'macCyrillic') # А
    check_both_ways("\u041F", "\x8F", 'macCyrillic') # П
    check_both_ways("\u0420", "\x90", 'macCyrillic') # Р
    check_both_ways("\u042F", "\x9F", 'macCyrillic') # Я
    check_both_ways("\u2020", "\xA0", 'macCyrillic') # †
    check_both_ways("\u0453", "\xAF", 'macCyrillic') # ѓ
    check_both_ways("\u221E", "\xB0", 'macCyrillic') # ∞
    check_both_ways("\u045A", "\xBF", 'macCyrillic') # њ
    check_both_ways("\u0458", "\xC0", 'macCyrillic') # ј
    check_both_ways("\u0455", "\xCF", 'macCyrillic') # ѕ
    check_both_ways("\u2013", "\xD0", 'macCyrillic') # –
    check_both_ways("\u044F", "\xDF", 'macCyrillic') # я
    check_both_ways("\u0430", "\xE0", 'macCyrillic') # а
    check_both_ways("\u043F", "\xEF", 'macCyrillic') # п
    check_both_ways("\u0440", "\xF0", 'macCyrillic') # р
    check_both_ways("\u00A4", "\xFF", 'macCyrillic') # ¤
  end

  def test_macGreek
    check_both_ways("\u00C4", "\x80", 'macGreek') # Ä
    check_both_ways("\u00E8", "\x8F", 'macGreek') # è
    check_both_ways("\u00EA", "\x90", 'macGreek') # ê
    check_both_ways("\u00FC", "\x9F", 'macGreek') # ü
    check_both_ways("\u2020", "\xA0", 'macGreek') # †
    check_both_ways("\u0393", "\xA1", 'macGreek') # Γ
    check_both_ways("\u0387", "\xAF", 'macGreek') # ·
    check_both_ways("\u0391", "\xB0", 'macGreek') # Α
    check_both_ways("\u03A9", "\xBF", 'macGreek') # Ω
    check_both_ways("\u03AC", "\xC0", 'macGreek') # ά
    check_both_ways("\u0153", "\xCF", 'macGreek') # œ
    check_both_ways("\u2013", "\xD0", 'macGreek') # –
    check_both_ways("\u038F", "\xDF", 'macGreek') # Ώ
    check_both_ways("\u03CD", "\xE0", 'macGreek') # ύ
    check_both_ways("\u03BF", "\xEF", 'macGreek') # ο
    check_both_ways("\u03C0", "\xF0", 'macGreek') # π
    check_both_ways("\u03B0", "\xFE", 'macGreek') # ΰ
    assert_raise(Encoding::UndefinedConversionError) { "\xFF".encode("utf-8", 'macGreek') }
  end

  def test_macIceland
    check_both_ways("\u00C4", "\x80", 'macIceland') # Ä
    check_both_ways("\u00E8", "\x8F", 'macIceland') # è
    check_both_ways("\u00EA", "\x90", 'macIceland') # ê
    check_both_ways("\u00FC", "\x9F", 'macIceland') # ü
    check_both_ways("\u00DD", "\xA0", 'macIceland') # Ý
    check_both_ways("\u00D8", "\xAF", 'macIceland') # Ø
    check_both_ways("\u221E", "\xB0", 'macIceland') # ∞
    check_both_ways("\u00F8", "\xBF", 'macIceland') # ø
    check_both_ways("\u00BF", "\xC0", 'macIceland') # ¿
    check_both_ways("\u0153", "\xCF", 'macIceland') # œ
    check_both_ways("\u2013", "\xD0", 'macIceland') # –
    check_both_ways("\u00FE", "\xDF", 'macIceland') # þ
    check_both_ways("\u00FD", "\xE0", 'macIceland') # ý
    check_both_ways("\u00D4", "\xEF", 'macIceland') # Ô
    #check_both_ways("\uF8FF", "\xF0", 'macIceland') # Apple logo
    check_both_ways("\u02C7", "\xFF", 'macIceland') # ˇ
  end

  def test_macRoman
    check_both_ways("\u00C4", "\x80", 'macRoman') # Ä
    check_both_ways("\u00E8", "\x8F", 'macRoman') # è
    check_both_ways("\u00EA", "\x90", 'macRoman') # ê
    check_both_ways("\u00FC", "\x9F", 'macRoman') # ü
    check_both_ways("\u2020", "\xA0", 'macRoman') # †
    #check_both_ways("\u00DB", "\xAF", 'macRoman') # Ø
    check_both_ways("\u221E", "\xB0", 'macRoman') # ∞
    check_both_ways("\u00F8", "\xBF", 'macRoman') # ø
    check_both_ways("\u00BF", "\xC0", 'macRoman') # ¿
    check_both_ways("\u0153", "\xCF", 'macRoman') # œ
    check_both_ways("\u2013", "\xD0", 'macRoman') # –
    check_both_ways("\u00A4", "\xDB", 'macRoman') # ¤
    check_both_ways("\uFB02", "\xDF", 'macRoman') # ﬂ
    check_both_ways("\u2021", "\xE0", 'macRoman') # ‡
    check_both_ways("\u00D4", "\xEF", 'macRoman') # Ô
    #check_both_ways("\uF8FF", "\xF0", 'macRoman') # Apple logo
    check_both_ways("\u02C7", "\xFF", 'macRoman') # ˇ
  end

  def test_macRomania
    check_both_ways("\u00C4", "\x80", 'macRomania') # Ä
    check_both_ways("\u00E8", "\x8F", 'macRomania') # è
    check_both_ways("\u00EA", "\x90", 'macRomania') # ê
    check_both_ways("\u00FC", "\x9F", 'macRomania') # ü
    check_both_ways("\u2020", "\xA0", 'macRomania') # †
    check_both_ways("\u015E", "\xAF", 'macRomania') # Ş
    check_both_ways("\u221E", "\xB0", 'macRomania') # ∞
    check_both_ways("\u015F", "\xBF", 'macRomania') # ş
    check_both_ways("\u00BF", "\xC0", 'macRomania') # ¿
    check_both_ways("\u0153", "\xCF", 'macRomania') # œ
    check_both_ways("\u2013", "\xD0", 'macRomania') # –
    check_both_ways("\u00A4", "\xDB", 'macRomania') # €
    check_both_ways("\u0163", "\xDF", 'macRomania') # ţ
    check_both_ways("\u2021", "\xE0", 'macRomania') # ‡
    check_both_ways("\u00D4", "\xEF", 'macRomania') # Ô
    #check_both_ways("\uF8FF", "\xF0", 'macRomania') # Apple logo
    check_both_ways("\u02C7", "\xFF", 'macRomania') # ˇ
  end

  def test_macTurkish
    check_both_ways("\u00C4", "\x80", 'macTurkish') # Ä
    check_both_ways("\u00E8", "\x8F", 'macTurkish') # è
    check_both_ways("\u00EA", "\x90", 'macTurkish') # ê
    check_both_ways("\u00FC", "\x9F", 'macTurkish') # ü
    check_both_ways("\u2020", "\xA0", 'macTurkish') # †
    check_both_ways("\u00D8", "\xAF", 'macTurkish') # Ø
    check_both_ways("\u221E", "\xB0", 'macTurkish') # ∞
    check_both_ways("\u00F8", "\xBF", 'macTurkish') # ø
    check_both_ways("\u00BF", "\xC0", 'macTurkish') # ¿
    check_both_ways("\u0153", "\xCF", 'macTurkish') # œ
    check_both_ways("\u2013", "\xD0", 'macTurkish') # –
    check_both_ways("\u015F", "\xDF", 'macTurkish') # ş
    check_both_ways("\u2021", "\xE0", 'macTurkish') # ‡
    check_both_ways("\u00D4", "\xEF", 'macTurkish') # Ô
    #check_both_ways("\uF8FF", "\xF0", 'macTurkish') # Apple logo
    check_both_ways("\u00D9", "\xF4", 'macTurkish') # Ù
    assert_raise(Encoding::UndefinedConversionError) { "\xF5".encode("utf-8", 'macTurkish') }
    check_both_ways("\u02C6", "\xF6", 'macTurkish') # ˆ
    check_both_ways("\u02C7", "\xFF", 'macTurkish') # ˇ
  end

  def test_macUkraine
    check_both_ways("\u0410", "\x80", 'macUkraine') # А
    check_both_ways("\u041F", "\x8F", 'macUkraine') # П
    check_both_ways("\u0420", "\x90", 'macUkraine') # Р
    check_both_ways("\u042F", "\x9F", 'macUkraine') # Я
    check_both_ways("\u2020", "\xA0", 'macUkraine') # †
    check_both_ways("\u0453", "\xAF", 'macUkraine') # ѓ
    check_both_ways("\u221E", "\xB0", 'macUkraine') # ∞
    check_both_ways("\u045A", "\xBF", 'macUkraine') # њ
    check_both_ways("\u0458", "\xC0", 'macUkraine') # ј
    check_both_ways("\u0455", "\xCF", 'macUkraine') # ѕ
    check_both_ways("\u2013", "\xD0", 'macUkraine') # –
    check_both_ways("\u044F", "\xDF", 'macUkraine') # я
    check_both_ways("\u0430", "\xE0", 'macUkraine') # а
    check_both_ways("\u043F", "\xEF", 'macUkraine') # п
    check_both_ways("\u0440", "\xF0", 'macUkraine') # р
    check_both_ways("\u00A4", "\xFF", 'macUkraine') # ¤
  end

  def test_koi8_u
    check_both_ways("\u2500", "\x80", 'KOI8-U') # ─
    check_both_ways("\u2590", "\x8F", 'KOI8-U') # ▐
    check_both_ways("\u2591", "\x90", 'KOI8-U') # ░
    check_both_ways("\u00F7", "\x9F", 'KOI8-U') # ÷
    check_both_ways("\u2550", "\xA0", 'KOI8-U') # ═
    check_both_ways("\u0454", "\xA4", 'KOI8-U') # є
    check_both_ways("\u0456", "\xA6", 'KOI8-U') # і
    check_both_ways("\u0457", "\xA7", 'KOI8-U') # ї
    check_both_ways("\u0491", "\xAD", 'KOI8-U') # ґ
    check_both_ways("\u255E", "\xAF", 'KOI8-U') # ╞
    check_both_ways("\u255F", "\xB0", 'KOI8-U') # ╟
    check_both_ways("\u0404", "\xB4", 'KOI8-U') # Є
    check_both_ways("\u0406", "\xB6", 'KOI8-U') # І
    check_both_ways("\u0407", "\xB7", 'KOI8-U') # Ї
    check_both_ways("\u0490", "\xBD", 'KOI8-U') # Ґ
    check_both_ways("\u00A9", "\xBF", 'KOI8-U') # ©
    check_both_ways("\u044E", "\xC0", 'KOI8-U') # ю
    check_both_ways("\u043E", "\xCF", 'KOI8-U') # о
    check_both_ways("\u043F", "\xD0", 'KOI8-U') # п
    check_both_ways("\u044A", "\xDF", 'KOI8-U') # ъ
    check_both_ways("\u042E", "\xE0", 'KOI8-U') # Ю
    check_both_ways("\u041E", "\xEF", 'KOI8-U') # О
    check_both_ways("\u041F", "\xF0", 'KOI8-U') # П
    check_both_ways("\u042A", "\xFF", 'KOI8-U') # Ъ
  end

  def test_koi8_r
    check_both_ways("\u2500", "\x80", 'KOI8-R') # ─
    check_both_ways("\u2590", "\x8F", 'KOI8-R') # ▐
    check_both_ways("\u2591", "\x90", 'KOI8-R') # ░
    check_both_ways("\u00F7", "\x9F", 'KOI8-R') # ÷
    check_both_ways("\u2550", "\xA0", 'KOI8-R') # ═
    check_both_ways("\u255E", "\xAF", 'KOI8-R') # ╞
    check_both_ways("\u255F", "\xB0", 'KOI8-R') # ╟
    check_both_ways("\u00A9", "\xBF", 'KOI8-R') # ©
    check_both_ways("\u044E", "\xC0", 'KOI8-R') # ю
    check_both_ways("\u043E", "\xCF", 'KOI8-R') # о
    check_both_ways("\u043F", "\xD0", 'KOI8-R') # п
    check_both_ways("\u044A", "\xDF", 'KOI8-R') # ъ
    check_both_ways("\u042E", "\xE0", 'KOI8-R') # Ю
    check_both_ways("\u041E", "\xEF", 'KOI8-R') # О
    check_both_ways("\u041F", "\xF0", 'KOI8-R') # П
    check_both_ways("\u042A", "\xFF", 'KOI8-R') # Ъ
  end

  def test_TIS_620
    assert_raise(Encoding::UndefinedConversionError) { "\x80".encode("utf-8", 'TIS-620') }
    assert_raise(Encoding::UndefinedConversionError) { "\x8F".encode("utf-8", 'TIS-620') }
    assert_raise(Encoding::UndefinedConversionError) { "\x90".encode("utf-8", 'TIS-620') }
    assert_raise(Encoding::UndefinedConversionError) { "\x9F".encode("utf-8", 'TIS-620') }
    assert_raise(Encoding::UndefinedConversionError) { "\xA0".encode("utf-8", 'TIS-620') }
    check_both_ways("\u0E01", "\xA1", 'TIS-620') # ก
    check_both_ways("\u0E0F", "\xAF", 'TIS-620') # ฏ
    check_both_ways("\u0E10", "\xB0", 'TIS-620') # ฐ
    check_both_ways("\u0E1F", "\xBF", 'TIS-620') # ฟ
    check_both_ways("\u0E20", "\xC0", 'TIS-620') # ภ
    check_both_ways("\u0E2F", "\xCF", 'TIS-620') # ฯ
    check_both_ways("\u0E30", "\xD0", 'TIS-620') # ะ
    check_both_ways("\u0E3A", "\xDA", 'TIS-620') #  ฺ
    assert_raise(Encoding::UndefinedConversionError) { "\xDB".encode("utf-8", 'TIS-620') }
    assert_raise(Encoding::UndefinedConversionError) { "\xDE".encode("utf-8", 'TIS-620') }
    check_both_ways("\u0E3F", "\xDF", 'TIS-620') # ฿
    check_both_ways("\u0E40", "\xE0", 'TIS-620') # เ
    check_both_ways("\u0E4F", "\xEF", 'TIS-620') # ๏
    check_both_ways("\u0E50", "\xF0", 'TIS-620') # ๐
    check_both_ways("\u0E5B", "\xFB", 'TIS-620') # ๛
    assert_raise(Encoding::UndefinedConversionError) { "\xFC".encode("utf-8", 'TIS-620') }
    assert_raise(Encoding::UndefinedConversionError) { "\xFF".encode("utf-8", 'TIS-620') }
  end

  def test_CP850
    check_both_ways("\u00C7", "\x80", 'CP850') # Ç
    check_both_ways("\u00C5", "\x8F", 'CP850') # Å
    check_both_ways("\u00C9", "\x90", 'CP850') # É
    check_both_ways("\u0192", "\x9F", 'CP850') # ƒ
    check_both_ways("\u00E1", "\xA0", 'CP850') # á
    check_both_ways("\u00BB", "\xAF", 'CP850') # »
    check_both_ways("\u2591", "\xB0", 'CP850') # ░
    check_both_ways("\u2510", "\xBF", 'CP850') # ┐
    check_both_ways("\u2514", "\xC0", 'CP850') # └
    check_both_ways("\u00A4", "\xCF", 'CP850') # ¤
    check_both_ways("\u00F0", "\xD0", 'CP850') # ð
    check_both_ways("\u2580", "\xDF", 'CP850') # ▀
    check_both_ways("\u00D3", "\xE0", 'CP850') # Ó
    check_both_ways("\u00B4", "\xEF", 'CP850') # ´
    check_both_ways("\u00AD", "\xF0", 'CP850') # soft hyphen
    check_both_ways("\u00A0", "\xFF", 'CP850') # non-breaking space
  end

  def test_CP852
    check_both_ways("\u00C7", "\x80", 'CP852') # Ç
    check_both_ways("\u0106", "\x8F", 'CP852') # Ć
    check_both_ways("\u00C9", "\x90", 'CP852') # É
    check_both_ways("\u010D", "\x9F", 'CP852') # č
    check_both_ways("\u00E1", "\xA0", 'CP852') # á
    check_both_ways("\u00BB", "\xAF", 'CP852') # »
    check_both_ways("\u2591", "\xB0", 'CP852') # ░
    check_both_ways("\u2510", "\xBF", 'CP852') # ┐
    check_both_ways("\u2514", "\xC0", 'CP852') # └
    check_both_ways("\u00A4", "\xCF", 'CP852') # ¤
    check_both_ways("\u0111", "\xD0", 'CP852') # đ
    check_both_ways("\u2580", "\xDF", 'CP852') # ▀
    check_both_ways("\u00D3", "\xE0", 'CP852') # Ó
    check_both_ways("\u00B4", "\xEF", 'CP852') # ´
    check_both_ways("\u00AD", "\xF0", 'CP852') # soft hyphen
    check_both_ways("\u00A0", "\xFF", 'CP852') # non-breaking space
  end

  def test_CP855
    check_both_ways("\u0452", "\x80", 'CP855') # ђ
    check_both_ways("\u0408", "\x8F", 'CP855') # Ј
    check_both_ways("\u0459", "\x90", 'CP855') # љ
    check_both_ways("\u042A", "\x9F", 'CP855') # Ъ
    check_both_ways("\u0430", "\xA0", 'CP855') # а
    check_both_ways("\u00BB", "\xAF", 'CP855') # »
    check_both_ways("\u2591", "\xB0", 'CP855') # ░
    check_both_ways("\u2510", "\xBF", 'CP855') # ┐
    check_both_ways("\u2514", "\xC0", 'CP855') # └
    check_both_ways("\u00A4", "\xCF", 'CP855') # ¤
    check_both_ways("\u043B", "\xD0", 'CP855') # л
    check_both_ways("\u2580", "\xDF", 'CP855') # ▀
    check_both_ways("\u042F", "\xE0", 'CP855') # Я
    check_both_ways("\u2116", "\xEF", 'CP855') # №
    check_both_ways("\u00AD", "\xF0", 'CP855') # soft hyphen
    check_both_ways("\u00A0", "\xFF", 'CP855') # non-breaking space
  end

  def check_utf_16_both_ways(utf8, raw)
    copy = raw.dup
    0.step(copy.length-1, 2) { |i| copy[i+1], copy[i] = copy[i], copy[i+1] }
    check_both_ways(utf8, raw, 'utf-16be')
    check_both_ways(utf8, copy, 'utf-16le')
  end

  def test_utf_16
    check_utf_16_both_ways("abc", "\x00a\x00b\x00c")
    check_utf_16_both_ways("\u00E9", "\x00\xE9");
    check_utf_16_both_ways("\u00E9\u0070\u00E9\u0065", "\x00\xE9\x00\x70\x00\xE9\x00\x65") # épée
    check_utf_16_both_ways("\u677E\u672C\u884C\u5F18", "\x67\x7E\x67\x2C\x88\x4C\x5F\x18") # 松本行弘
    check_utf_16_both_ways("\u9752\u5C71\u5B66\u9662\u5927\u5B66", "\x97\x52\x5C\x71\x5B\x66\x96\x62\x59\x27\x5B\x66") # 青山学院大学
    check_utf_16_both_ways("Martin D\u00FCrst", "\x00M\x00a\x00r\x00t\x00i\x00n\x00 \x00D\x00\xFC\x00r\x00s\x00t") # Martin Dürst
    # BMP
    check_utf_16_both_ways("\u0000", "\x00\x00")
    check_utf_16_both_ways("\u007F", "\x00\x7F")
    check_utf_16_both_ways("\u0080", "\x00\x80")
    check_utf_16_both_ways("\u0555", "\x05\x55")
    check_utf_16_both_ways("\u04AA", "\x04\xAA")
    check_utf_16_both_ways("\u0333", "\x03\x33")
    check_utf_16_both_ways("\u04CC", "\x04\xCC")
    check_utf_16_both_ways("\u00F0", "\x00\xF0")
    check_utf_16_both_ways("\u070F", "\x07\x0F")
    check_utf_16_both_ways("\u07FF", "\x07\xFF")
    check_utf_16_both_ways("\u0800", "\x08\x00")
    check_utf_16_both_ways("\uD7FF", "\xD7\xFF")
    check_utf_16_both_ways("\uE000", "\xE0\x00")
    check_utf_16_both_ways("\uFFFF", "\xFF\xFF")
    check_utf_16_both_ways("\u5555", "\x55\x55")
    check_utf_16_both_ways("\uAAAA", "\xAA\xAA")
    check_utf_16_both_ways("\u3333", "\x33\x33")
    check_utf_16_both_ways("\uCCCC", "\xCC\xCC")
    check_utf_16_both_ways("\uF0F0", "\xF0\xF0")
    check_utf_16_both_ways("\u0F0F", "\x0F\x0F")
    check_utf_16_both_ways("\uFF00", "\xFF\x00")
    check_utf_16_both_ways("\u00FF", "\x00\xFF")
    # outer planes
    check_utf_16_both_ways("\u{10000}", "\xD8\x00\xDC\x00")
    check_utf_16_both_ways("\u{FFFFF}", "\xDB\xBF\xDF\xFF")
    check_utf_16_both_ways("\u{100000}", "\xDB\xC0\xDC\x00")
    check_utf_16_both_ways("\u{10FFFF}", "\xDB\xFF\xDF\xFF")
    check_utf_16_both_ways("\u{105555}", "\xDB\xD5\xDD\x55")
    check_utf_16_both_ways("\u{55555}", "\xD9\x15\xDD\x55")
    check_utf_16_both_ways("\u{AAAAA}", "\xDA\x6A\xDE\xAA")
    check_utf_16_both_ways("\u{33333}", "\xD8\x8C\xDF\x33")
    check_utf_16_both_ways("\u{CCCCC}", "\xDA\xF3\xDC\xCC")
    check_utf_16_both_ways("\u{8F0F0}", "\xD9\xFC\xDC\xF0")
    check_utf_16_both_ways("\u{F0F0F}", "\xDB\x83\xDF\x0F")
    check_utf_16_both_ways("\u{8FF00}", "\xD9\xFF\xDF\x00")
    check_utf_16_both_ways("\u{F00FF}", "\xDB\x80\xDC\xFF")
  end

  def test_utf_16_bom
    expected = "\u{3042}\u{3044}\u{20bb7}"
    assert_equal(expected, %w/fffe4230443042d8b7df/.pack("H*").encode("UTF-8","UTF-16"))
    check_both_ways(expected, %w/feff30423044d842dfb7/.pack("H*"), "UTF-16")
    assert_raise(Encoding::InvalidByteSequenceError){%w/feffdfb7/.pack("H*").encode("UTF-8","UTF-16")}
    assert_raise(Encoding::InvalidByteSequenceError){%w/fffeb7df/.pack("H*").encode("UTF-8","UTF-16")}
  end

  def test_utf_32_bom
    expected = "\u{3042}\u{3044}\u{20bb7}"
    assert_equal(expected, %w/fffe00004230000044300000b70b0200/.pack("H*").encode("UTF-8","UTF-32"))
    check_both_ways(expected, %w/0000feff000030420000304400020bb7/.pack("H*"), "UTF-32")
    assert_raise(Encoding::InvalidByteSequenceError){%w/0000feff00110000/.pack("H*").encode("UTF-8","UTF-32")}
  end

  def check_utf_32_both_ways(utf8, raw)
    copy = raw.dup
    0.step(copy.length-1, 4) do |i|
      copy[i+3], copy[i+2], copy[i+1], copy[i] = copy[i], copy[i+1], copy[i+2], copy[i+3]
    end
    check_both_ways(utf8, raw, 'utf-32be')
    #check_both_ways(utf8, copy, 'utf-32le')
  end

  def test_utf_32
    check_utf_32_both_ways("abc", "\x00\x00\x00a\x00\x00\x00b\x00\x00\x00c")
    check_utf_32_both_ways("\u00E9", "\x00\x00\x00\xE9");
    check_utf_32_both_ways("\u00E9\u0070\u00E9\u0065",
      "\x00\x00\x00\xE9\x00\x00\x00\x70\x00\x00\x00\xE9\x00\x00\x00\x65") # épée
    check_utf_32_both_ways("\u677E\u672C\u884C\u5F18",
      "\x00\x00\x67\x7E\x00\x00\x67\x2C\x00\x00\x88\x4C\x00\x00\x5F\x18") # 松本行弘
    check_utf_32_both_ways("\u9752\u5C71\u5B66\u9662\u5927\u5B66",
      "\x00\x00\x97\x52\x00\x00\x5C\x71\x00\x00\x5B\x66\x00\x00\x96\x62\x00\x00\x59\x27\x00\x00\x5B\x66") # 青山学院大学
    check_utf_32_both_ways("Martin D\u00FCrst",
      "\x00\x00\x00M\x00\x00\x00a\x00\x00\x00r\x00\x00\x00t\x00\x00\x00i\x00\x00\x00n\x00\x00\x00 \x00\x00\x00D\x00\x00\x00\xFC\x00\x00\x00r\x00\x00\x00s\x00\x00\x00t") # Martin Dürst
    # BMP
    check_utf_32_both_ways("\u0000", "\x00\x00\x00\x00")
    check_utf_32_both_ways("\u007F", "\x00\x00\x00\x7F")
    check_utf_32_both_ways("\u0080", "\x00\x00\x00\x80")
    check_utf_32_both_ways("\u0555", "\x00\x00\x05\x55")
    check_utf_32_both_ways("\u04AA", "\x00\x00\x04\xAA")
    check_utf_32_both_ways("\u0333", "\x00\x00\x03\x33")
    check_utf_32_both_ways("\u04CC", "\x00\x00\x04\xCC")
    check_utf_32_both_ways("\u00F0", "\x00\x00\x00\xF0")
    check_utf_32_both_ways("\u070F", "\x00\x00\x07\x0F")
    check_utf_32_both_ways("\u07FF", "\x00\x00\x07\xFF")
    check_utf_32_both_ways("\u0800", "\x00\x00\x08\x00")
    check_utf_32_both_ways("\uD7FF", "\x00\x00\xD7\xFF")
    check_utf_32_both_ways("\uE000", "\x00\x00\xE0\x00")
    check_utf_32_both_ways("\uFFFF", "\x00\x00\xFF\xFF")
    check_utf_32_both_ways("\u5555", "\x00\x00\x55\x55")
    check_utf_32_both_ways("\uAAAA", "\x00\x00\xAA\xAA")
    check_utf_32_both_ways("\u3333", "\x00\x00\x33\x33")
    check_utf_32_both_ways("\uCCCC", "\x00\x00\xCC\xCC")
    check_utf_32_both_ways("\uF0F0", "\x00\x00\xF0\xF0")
    check_utf_32_both_ways("\u0F0F", "\x00\x00\x0F\x0F")
    check_utf_32_both_ways("\uFF00", "\x00\x00\xFF\x00")
    check_utf_32_both_ways("\u00FF", "\x00\x00\x00\xFF")
    # outer planes
    check_utf_32_both_ways("\u{10000}", "\x00\x01\x00\x00")
    check_utf_32_both_ways("\u{FFFFF}", "\x00\x0F\xFF\xFF")
    check_utf_32_both_ways("\u{100000}","\x00\x10\x00\x00")
    check_utf_32_both_ways("\u{10FFFF}","\x00\x10\xFF\xFF")
    check_utf_32_both_ways("\u{105555}","\x00\x10\x55\x55")
    check_utf_32_both_ways("\u{55555}", "\x00\x05\x55\x55")
    check_utf_32_both_ways("\u{AAAAA}", "\x00\x0A\xAA\xAA")
    check_utf_32_both_ways("\u{33333}", "\x00\x03\x33\x33")
    check_utf_32_both_ways("\u{CCCCC}", "\x00\x0C\xCC\xCC")
    check_utf_32_both_ways("\u{8F0F0}", "\x00\x08\xF0\xF0")
    check_utf_32_both_ways("\u{F0F0F}", "\x00\x0F\x0F\x0F")
    check_utf_32_both_ways("\u{8FF00}", "\x00\x08\xFF\x00")
    check_utf_32_both_ways("\u{F00FF}", "\x00\x0F\x00\xFF")
  end

  def test_invalid_ignore
    # arguments only
    assert_nothing_raised { 'abc'.encode('utf-8', invalid: :replace, replace: "") }
    # check handling of UTF-8 ill-formed subsequences
    assert_equal("\x00\x41\x00\x3E\x00\x42".force_encoding('UTF-16BE'),
      "\x41\xC2\x3E\x42".encode('UTF-16BE', 'UTF-8', invalid: :replace, replace: ""))
    assert_equal("\x00\x41\x00\xF1\x00\x42".force_encoding('UTF-16BE'),
      "\x41\xC2\xC3\xB1\x42".encode('UTF-16BE', 'UTF-8', invalid: :replace, replace: ""))
    assert_equal("\x00\x42".force_encoding('UTF-16BE'),
      "\xF0\x80\x80\x42".encode('UTF-16BE', 'UTF-8', invalid: :replace, replace: ""))
    assert_equal(''.force_encoding('UTF-16BE'),
      "\x82\xAB".encode('UTF-16BE', 'UTF-8', invalid: :replace, replace: ""))

    assert_equal("\e$B!!\e(B".force_encoding("ISO-2022-JP"),
      "\xA1\xA1\xFF".encode("ISO-2022-JP", "EUC-JP", invalid: :replace, replace: ""))
    assert_equal("\e$B\x24\x22\x24\x24\e(B".force_encoding("ISO-2022-JP"),
      "\xA4\xA2\xFF\xA4\xA4".encode("ISO-2022-JP", "EUC-JP", invalid: :replace, replace: ""))
    assert_equal("\e$B\x24\x22\x24\x24\e(B".force_encoding("ISO-2022-JP"),
      "\xA4\xA2\xFF\xFF\xA4\xA4".encode("ISO-2022-JP", "EUC-JP", invalid: :replace, replace: ""))
  end

  def test_invalid_replace
    # arguments only
    assert_nothing_raised { 'abc'.encode('UTF-8', invalid: :replace) }
    assert_equal("\xEF\xBF\xBD".force_encoding("UTF-8"),
      "\x80".encode("UTF-8", "UTF-16BE", invalid: :replace))
    assert_equal("\xFF\xFD".force_encoding("UTF-16BE"),
      "\x80".encode("UTF-16BE", "UTF-8", invalid: :replace))
    assert_equal("\xFD\xFF".force_encoding("UTF-16LE"),
      "\x80".encode("UTF-16LE", "UTF-8", invalid: :replace))
    assert_equal("\x00\x00\xFF\xFD".force_encoding("UTF-32BE"),
      "\x80".encode("UTF-32BE", "UTF-8", invalid: :replace))
    assert_equal("\xFD\xFF\x00\x00".force_encoding("UTF-32LE"),
      "\x80".encode("UTF-32LE", "UTF-8", invalid: :replace))

    assert_equal("\uFFFD!",
      "\xdc\x00\x00!".encode("utf-8", "utf-16be", :invalid=>:replace))
    assert_equal("\uFFFD!",
      "\xd8\x00\x00!".encode("utf-8", "utf-16be", :invalid=>:replace))

    assert_equal("\uFFFD!",
      "\x00\xdc!\x00".encode("utf-8", "utf-16le", :invalid=>:replace))
    assert_equal("\uFFFD!",
      "\x00\xd8!\x00".encode("utf-8", "utf-16le", :invalid=>:replace))

    assert_equal("\uFFFD!",
      "\x01\x00\x00\x00\x00\x00\x00!".encode("utf-8", "utf-32be", :invalid=>:replace), "[ruby-dev:35726]")
    assert_equal("\uFFFD!",
      "\x00\xff\x00\x00\x00\x00\x00!".encode("utf-8", "utf-32be", :invalid=>:replace))
    assert_equal("\uFFFD!",
      "\x00\x00\xd8\x00\x00\x00\x00!".encode("utf-8", "utf-32be", :invalid=>:replace))

    assert_equal("\uFFFD!",
      "\x00\x00\x00\xff!\x00\x00\x00".encode("utf-8", "utf-32le", :invalid=>:replace))
    assert_equal("\uFFFD!",
      "\x00\x00\xff\x00!\x00\x00\x00".encode("utf-8", "utf-32le", :invalid=>:replace))
    assert_equal("\uFFFD!",
      "\x00\xd8\x00\x00!\x00\x00\x00".encode("utf-8", "utf-32le", :invalid=>:replace))

    assert_equal("\uFFFD!",
      "\xff!".encode("utf-8", "euc-jp", :invalid=>:replace))
    assert_equal("\uFFFD!",
      "\xa1!".encode("utf-8", "euc-jp", :invalid=>:replace))
    assert_equal("\uFFFD!",
      "\x8f\xa1!".encode("utf-8", "euc-jp", :invalid=>:replace))

    assert_equal("?",
      "\xdc\x00".encode("EUC-JP", "UTF-16BE", :invalid=>:replace), "[ruby-dev:35776]")
    assert_equal("ab?cd?ef",
      "\0a\0b\xdc\x00\0c\0d\xdf\x00\0e\0f".encode("EUC-JP", "UTF-16BE", :invalid=>:replace))

    assert_equal("\e$B!!\e(B?".force_encoding("ISO-2022-JP"),
      "\xA1\xA1\xFF".encode("ISO-2022-JP", "EUC-JP", invalid: :replace))
    assert_equal("\e$B\x24\x22\e(B?\e$B\x24\x24\e(B".force_encoding("ISO-2022-JP"),
      "\xA4\xA2\xFF\xA4\xA4".encode("ISO-2022-JP", "EUC-JP", invalid: :replace))
    assert_equal("\e$B\x24\x22\e(B??\e$B\x24\x24\e(B".force_encoding("ISO-2022-JP"),
      "\xA4\xA2\xFF\xFF\xA4\xA4".encode("ISO-2022-JP", "EUC-JP", invalid: :replace))
  end

  def test_invalid_replace_string
    assert_equal("a<x>A", "a\x80A".encode("us-ascii", "euc-jp", :invalid=>:replace, :replace=>"<x>"))
  end

  def test_undef_replace
    assert_equal("?", "\u20AC".encode("EUC-JP", :undef=>:replace), "[ruby-dev:35709]")
  end

  def test_undef_replace_string
    assert_equal("a<x>A", "a\u3042A".encode("us-ascii", :undef=>:replace, :replace=>"<x>"))
  end

  def test_shift_jis
    check_both_ways("\u3000", "\x81\x40", 'shift_jis') # full-width space
    check_both_ways("\u00D7", "\x81\x7E", 'shift_jis') # ×
    check_both_ways("\u00F7", "\x81\x80", 'shift_jis') # ÷
    check_both_ways("\u25C7", "\x81\x9E", 'shift_jis') # ◇
    check_both_ways("\u25C6", "\x81\x9F", 'shift_jis') # ◆
    check_both_ways("\u25EF", "\x81\xFC", 'shift_jis') # ◯
    check_both_ways("\u6A97", "\x9F\x40", 'shift_jis') # 檗
    check_both_ways("\u6BEF", "\x9F\x7E", 'shift_jis') # 毯
    check_both_ways("\u9EBE", "\x9F\x80", 'shift_jis') # 麾
    check_both_ways("\u6CBE", "\x9F\x9E", 'shift_jis') # 沾
    check_both_ways("\u6CBA", "\x9F\x9F", 'shift_jis') # 沺
    check_both_ways("\u6ECC", "\x9F\xFC", 'shift_jis') # 滌
    check_both_ways("\u6F3E", "\xE0\x40", 'shift_jis') # 漾
    check_both_ways("\u70DD", "\xE0\x7E", 'shift_jis') # 烝
    check_both_ways("\u70D9", "\xE0\x80", 'shift_jis') # 烙
    check_both_ways("\u71FC", "\xE0\x9E", 'shift_jis') # 燼
    check_both_ways("\u71F9", "\xE0\x9F", 'shift_jis') # 燹
    check_both_ways("\u73F1", "\xE0\xFC", 'shift_jis') # 珱
    assert_raise(Encoding::UndefinedConversionError) { "\xEF\x40".encode("utf-8", 'shift_jis') }
    assert_raise(Encoding::UndefinedConversionError) { "\xEF\x7E".encode("utf-8", 'shift_jis') }
    assert_raise(Encoding::UndefinedConversionError) { "\xEF\x80".encode("utf-8", 'shift_jis') }
    assert_raise(Encoding::UndefinedConversionError) { "\xEF\x9E".encode("utf-8", 'shift_jis') }
    assert_raise(Encoding::UndefinedConversionError) { "\xEF\x9F".encode("utf-8", 'shift_jis') }
    assert_raise(Encoding::UndefinedConversionError) { "\xEF\xFC".encode("utf-8", 'shift_jis') }
    assert_raise(Encoding::UndefinedConversionError) { "\xF0\x40".encode("utf-8", 'shift_jis') }
    assert_raise(Encoding::UndefinedConversionError) { "\xF0\x7E".encode("utf-8", 'shift_jis') }
    assert_raise(Encoding::UndefinedConversionError) { "\xF0\x80".encode("utf-8", 'shift_jis') }
    assert_raise(Encoding::UndefinedConversionError) { "\xF0\x9E".encode("utf-8", 'shift_jis') }
    assert_raise(Encoding::UndefinedConversionError) { "\xF0\x9F".encode("utf-8", 'shift_jis') }
    assert_raise(Encoding::UndefinedConversionError) { "\xF0\xFC".encode("utf-8", 'shift_jis') }
    #check_both_ways("\u9ADC", "\xFC\x40", 'shift_jis') # 髜 (IBM extended)
    assert_raise(Encoding::UndefinedConversionError) { "\xFC\x7E".encode("utf-8", 'shift_jis') }
    assert_raise(Encoding::UndefinedConversionError) { "\xFC\x80".encode("utf-8", 'shift_jis') }
    assert_raise(Encoding::UndefinedConversionError) { "\xFC\x9E".encode("utf-8", 'shift_jis') }
    assert_raise(Encoding::UndefinedConversionError) { "\xFC\x9F".encode("utf-8", 'shift_jis') }
    assert_raise(Encoding::UndefinedConversionError) { "\xFC\xFC".encode("utf-8", 'shift_jis') }
    check_both_ways("\u677E\u672C\u884C\u5F18", "\x8f\xbc\x96\x7b\x8d\x73\x8d\x4f", 'shift_jis') # 松本行弘
    check_both_ways("\u9752\u5C71\u5B66\u9662\u5927\u5B66", "\x90\xC2\x8E\x52\x8A\x77\x89\x40\x91\xE5\x8A\x77", 'shift_jis') # 青山学院大学
    check_both_ways("\u795E\u6797\u7FA9\u535A", "\x90\x5F\x97\xD1\x8B\x60\x94\x8E", 'shift_jis') # 神林義博
  end

  def test_windows_31j
    check_both_ways("\u222A", "\x81\xBE", 'Windows-31J') # Union
    check_both_ways("\uFFE2", "\x81\xCA", 'Windows-31J') # Fullwidth Not Sign
    check_both_ways("\u2235", "\x81\xE6", 'Windows-31J') # Because
    check_both_ways("\u2160", "\x87\x54", 'Windows-31J') # Roman Numeral One
    check_both_ways("\u2170", "\xFA\x40", 'Windows-31J') # Small Roman Numeral One
  end

  def test_euc_jp
    check_both_ways("\u3000", "\xA1\xA1", 'euc-jp') # full-width space
    check_both_ways("\u00D7", "\xA1\xDF", 'euc-jp') # ×
    check_both_ways("\u00F7", "\xA1\xE0", 'euc-jp') # ÷
    check_both_ways("\u25C7", "\xA1\xFE", 'euc-jp') # ◇
    check_both_ways("\u25C6", "\xA2\xA1", 'euc-jp') # ◆
    assert_raise(Encoding::UndefinedConversionError) { "\xA2\xAF".encode("utf-8", 'euc-jp') }
    assert_raise(Encoding::UndefinedConversionError) { "\xA2\xB9".encode("utf-8", 'euc-jp') }
    assert_raise(Encoding::UndefinedConversionError) { "\xA2\xC2".encode("utf-8", 'euc-jp') }
    assert_raise(Encoding::UndefinedConversionError) { "\xA2\xC9".encode("utf-8", 'euc-jp') }
    assert_raise(Encoding::UndefinedConversionError) { "\xA2\xD1".encode("utf-8", 'euc-jp') }
    assert_raise(Encoding::UndefinedConversionError) { "\xA2\xDB".encode("utf-8", 'euc-jp') }
    assert_raise(Encoding::UndefinedConversionError) { "\xA2\xEB".encode("utf-8", 'euc-jp') }
    assert_raise(Encoding::UndefinedConversionError) { "\xA2\xF1".encode("utf-8", 'euc-jp') }
    assert_raise(Encoding::UndefinedConversionError) { "\xA2\xFA".encode("utf-8", 'euc-jp') }
    assert_raise(Encoding::UndefinedConversionError) { "\xA2\xFD".encode("utf-8", 'euc-jp') }
    check_both_ways("\u25EF", "\xA2\xFE", 'euc-jp') # ◯
    assert_raise(Encoding::UndefinedConversionError) { "\xA3\xAF".encode("utf-8", 'euc-jp') }
    assert_raise(Encoding::UndefinedConversionError) { "\xA3\xBA".encode("utf-8", 'euc-jp') }
    assert_raise(Encoding::UndefinedConversionError) { "\xA3\xC0".encode("utf-8", 'euc-jp') }
    assert_raise(Encoding::UndefinedConversionError) { "\xA3\xDB".encode("utf-8", 'euc-jp') }
    assert_raise(Encoding::UndefinedConversionError) { "\xA3\xE0".encode("utf-8", 'euc-jp') }
    assert_raise(Encoding::UndefinedConversionError) { "\xA3\xFB".encode("utf-8", 'euc-jp') }
    assert_raise(Encoding::UndefinedConversionError) { "\xA4\xF4".encode("utf-8", 'euc-jp') }
    assert_raise(Encoding::UndefinedConversionError) { "\xA5\xF7".encode("utf-8", 'euc-jp') }
    assert_raise(Encoding::UndefinedConversionError) { "\xA6\xB9".encode("utf-8", 'euc-jp') }
    assert_raise(Encoding::UndefinedConversionError) { "\xA6\xC0".encode("utf-8", 'euc-jp') }
    assert_raise(Encoding::UndefinedConversionError) { "\xA6\xD9".encode("utf-8", 'euc-jp') }
    assert_raise(Encoding::UndefinedConversionError) { "\xA7\xC2".encode("utf-8", 'euc-jp') }
    assert_raise(Encoding::UndefinedConversionError) { "\xA7\xD0".encode("utf-8", 'euc-jp') }
    assert_raise(Encoding::UndefinedConversionError) { "\xA7\xF2".encode("utf-8", 'euc-jp') }
    assert_raise(Encoding::UndefinedConversionError) { "\xA8\xC1".encode("utf-8", 'euc-jp') }
    assert_raise(Encoding::UndefinedConversionError) { "\xCF\xD4".encode("utf-8", 'euc-jp') }
    assert_raise(Encoding::UndefinedConversionError) { "\xCF\xFE".encode("utf-8", 'euc-jp') }
    check_both_ways("\u6A97", "\xDD\xA1", 'euc-jp') # 檗
    check_both_ways("\u6BEF", "\xDD\xDF", 'euc-jp') # 毯
    check_both_ways("\u9EBE", "\xDD\xE0", 'euc-jp') # 麾
    check_both_ways("\u6CBE", "\xDD\xFE", 'euc-jp') # 沾
    check_both_ways("\u6CBA", "\xDE\xA1", 'euc-jp') # 沺
    check_both_ways("\u6ECC", "\xDE\xFE", 'euc-jp') # 滌
    check_both_ways("\u6F3E", "\xDF\xA1", 'euc-jp') # 漾
    check_both_ways("\u70DD", "\xDF\xDF", 'euc-jp') # 烝
    check_both_ways("\u70D9", "\xDF\xE0", 'euc-jp') # 烙
    check_both_ways("\u71FC", "\xDF\xFE", 'euc-jp') # 燼
    check_both_ways("\u71F9", "\xE0\xA1", 'euc-jp') # 燹
    check_both_ways("\u73F1", "\xE0\xFE", 'euc-jp') # 珱
    assert_raise(Encoding::UndefinedConversionError) { "\xF4\xA7".encode("utf-8", 'euc-jp') }
    #check_both_ways("\u9ADC", "\xFC\xE3", 'euc-jp') # 髜 (IBM extended)

    check_both_ways("\u677E\u672C\u884C\u5F18", "\xBE\xBE\xCB\xDC\xB9\xD4\xB9\xB0", 'euc-jp') # 松本行弘
    check_both_ways("\u9752\u5C71\u5B66\u9662\u5927\u5B66", "\xC0\xC4\xBB\xB3\xB3\xD8\xB1\xA1\xC2\xE7\xB3\xD8", 'euc-jp') # 青山学院大学
    check_both_ways("\u795E\u6797\u7FA9\u535A", "\xBF\xC0\xCE\xD3\xB5\xC1\xC7\xEE", 'euc-jp') # 神林義博
  end

  def test_eucjp_ms
    check_both_ways("\u2116", "\xAD\xE2", 'eucJP-ms') # NUMERO SIGN
    check_both_ways("\u221A", "\xA2\xE5", 'eucJP-ms') # SQUARE ROOT
    check_both_ways("\u3231", "\xAD\xEA", 'eucJP-ms') # PARENTHESIZED IDEOGRAPH STOCK
    check_both_ways("\uFF5E", "\xA1\xC1", 'eucJP-ms') # WAVE DASH
  end

  def test_eucjp_sjis
    check_both_ways2("\xa1\xa1", "EUC-JP", "\x81\x40", "Shift_JIS")
    check_both_ways2("\xa1\xdf", "EUC-JP", "\x81\x7e", "Shift_JIS")
    check_both_ways2("\xa1\xe0", "EUC-JP", "\x81\x80", "Shift_JIS")
    check_both_ways2("\xa1\xfe", "EUC-JP", "\x81\x9e", "Shift_JIS")
    check_both_ways2("\xa2\xa1", "EUC-JP", "\x81\x9f", "Shift_JIS")
    check_both_ways2("\xa2\xfe", "EUC-JP", "\x81\xfc", "Shift_JIS")

    check_both_ways2("\xdd\xa1", "EUC-JP", "\x9f\x40", "Shift_JIS")
    check_both_ways2("\xdd\xdf", "EUC-JP", "\x9f\x7e", "Shift_JIS")
    check_both_ways2("\xdd\xe0", "EUC-JP", "\x9f\x80", "Shift_JIS")
    check_both_ways2("\xdd\xfe", "EUC-JP", "\x9f\x9e", "Shift_JIS")
    check_both_ways2("\xde\xa1", "EUC-JP", "\x9f\x9f", "Shift_JIS")
    check_both_ways2("\xde\xfe", "EUC-JP", "\x9f\xfc", "Shift_JIS")

    check_both_ways2("\xdf\xa1", "EUC-JP", "\xe0\x40", "Shift_JIS")
    check_both_ways2("\xdf\xdf", "EUC-JP", "\xe0\x7e", "Shift_JIS")
    check_both_ways2("\xdf\xe0", "EUC-JP", "\xe0\x80", "Shift_JIS")
    check_both_ways2("\xdf\xfe", "EUC-JP", "\xe0\x9e", "Shift_JIS")
    check_both_ways2("\xe0\xa1", "EUC-JP", "\xe0\x9f", "Shift_JIS")
    check_both_ways2("\xe0\xfe", "EUC-JP", "\xe0\xfc", "Shift_JIS")

    check_both_ways2("\xf4\xa1", "EUC-JP", "\xea\x9f", "Shift_JIS")
    check_both_ways2("\xf4\xa2", "EUC-JP", "\xea\xa0", "Shift_JIS")
    check_both_ways2("\xf4\xa3", "EUC-JP", "\xea\xa1", "Shift_JIS")
    check_both_ways2("\xf4\xa4", "EUC-JP", "\xea\xa2", "Shift_JIS") # end of JIS X 0208 1983
    check_both_ways2("\xf4\xa5", "EUC-JP", "\xea\xa3", "Shift_JIS")
    check_both_ways2("\xf4\xa6", "EUC-JP", "\xea\xa4", "Shift_JIS") # end of JIS X 0208 1990

    check_both_ways2("\x8e\xa1", "EUC-JP", "\xa1", "Shift_JIS")
    check_both_ways2("\x8e\xdf", "EUC-JP", "\xdf", "Shift_JIS")
  end

  def test_eucjp_sjis_unassigned
    check_both_ways2("\xfd\xa1", "EUC-JP", "\xef\x40", "Shift_JIS")
    check_both_ways2("\xfd\xa1", "EUC-JP", "\xef\x40", "Shift_JIS")
    check_both_ways2("\xfd\xdf", "EUC-JP", "\xef\x7e", "Shift_JIS")
    check_both_ways2("\xfd\xe0", "EUC-JP", "\xef\x80", "Shift_JIS")
    check_both_ways2("\xfd\xfe", "EUC-JP", "\xef\x9e", "Shift_JIS")
    check_both_ways2("\xfe\xa1", "EUC-JP", "\xef\x9f", "Shift_JIS")
    check_both_ways2("\xfe\xfe", "EUC-JP", "\xef\xfc", "Shift_JIS")
  end

  def test_eucjp_sjis_undef
    assert_raise(Encoding::UndefinedConversionError) { "\x8e\xe0".encode("Shift_JIS", "EUC-JP") }
    assert_raise(Encoding::UndefinedConversionError) { "\x8e\xfe".encode("Shift_JIS", "EUC-JP") }
    assert_raise(Encoding::UndefinedConversionError) { "\x8f\xa1\xa1".encode("Shift_JIS", "EUC-JP") }
    assert_raise(Encoding::UndefinedConversionError) { "\x8f\xa1\xfe".encode("Shift_JIS", "EUC-JP") }
    assert_raise(Encoding::UndefinedConversionError) { "\x8f\xfe\xa1".encode("Shift_JIS", "EUC-JP") }
    assert_raise(Encoding::UndefinedConversionError) { "\x8f\xfe\xfe".encode("Shift_JIS", "EUC-JP") }

    assert_raise(Encoding::UndefinedConversionError) { "\xf0\x40".encode("EUC-JP", "Shift_JIS") }
    assert_raise(Encoding::UndefinedConversionError) { "\xf0\x7e".encode("EUC-JP", "Shift_JIS") }
    assert_raise(Encoding::UndefinedConversionError) { "\xf0\x80".encode("EUC-JP", "Shift_JIS") }
    assert_raise(Encoding::UndefinedConversionError) { "\xf0\xfc".encode("EUC-JP", "Shift_JIS") }
    assert_raise(Encoding::UndefinedConversionError) { "\xfc\x40".encode("EUC-JP", "Shift_JIS") }
    assert_raise(Encoding::UndefinedConversionError) { "\xfc\x7e".encode("EUC-JP", "Shift_JIS") }
    assert_raise(Encoding::UndefinedConversionError) { "\xfc\x80".encode("EUC-JP", "Shift_JIS") }
    assert_raise(Encoding::UndefinedConversionError) { "\xfc\xfc".encode("EUC-JP", "Shift_JIS") }
  end

  def test_iso_2022_jp
    assert_raise(Encoding::InvalidByteSequenceError) { "\x1b(A".encode("utf-8", "iso-2022-jp") }
    assert_raise(Encoding::InvalidByteSequenceError) { "\x1b$(A".encode("utf-8", "iso-2022-jp") }
    assert_raise(Encoding::InvalidByteSequenceError) { "\x1b$C".encode("utf-8", "iso-2022-jp") }
    assert_raise(Encoding::InvalidByteSequenceError) { "\x0e".encode("utf-8", "iso-2022-jp") }
    assert_raise(Encoding::InvalidByteSequenceError) { "\x80".encode("utf-8", "iso-2022-jp") }
    assert_raise(Encoding::InvalidByteSequenceError) { "\x1b$(Dd!\x1b(B".encode("utf-8", "iso-2022-jp") }
    assert_raise(Encoding::UndefinedConversionError) { "\u9299".encode("iso-2022-jp") }
    assert_raise(Encoding::UndefinedConversionError) { "\uff71\uff72\uff73\uff74\uff75".encode("iso-2022-jp") }
    assert_raise(Encoding::InvalidByteSequenceError) { "\x1b(I12345\x1b(B".encode("utf-8", "iso-2022-jp") }
    assert_equal("\xA1\xA1".force_encoding("euc-jp"),
                 "\e$B!!\e(B".encode("EUC-JP", "ISO-2022-JP"))
    assert_equal("\e$B!!\e(B".force_encoding("ISO-2022-JP"),
                 "\xA1\xA1".encode("ISO-2022-JP", "EUC-JP"))
  end

  def test_from_cp50221
    assert_equal("!", "\e(B\x21".encode("utf-8", "cp50221"))
    assert_equal("!", "\e(J\x21".encode("utf-8", "cp50221"))
    assert_equal("\uFF71",     "\xB1".encode("utf-8", "cp50221"))
    assert_equal("\uFF71", "\e(B\xB1".encode("utf-8", "cp50221"))
    assert_equal("\uFF71", "\e(J\xB1".encode("utf-8", "cp50221"))
    assert_equal("\uFF71", "\e(I\xB1".encode("utf-8", "cp50221"))
    assert_equal("\uFF71", "\e(I\x31".encode("utf-8", "cp50221"))
    assert_equal("\uFF71", "\x0E\xB1".encode("utf-8", "cp50221"))
    assert_equal("\u3000", "\e$@\x21\x21".encode("utf-8", "cp50221"))
    assert_equal("\u3000", "\e$B\x21\x21".encode("utf-8", "cp50221"))
    assert_equal("\u2460", "\e$B\x2D\x21".encode("utf-8", "cp50221"))
    assert_equal("\u7e8a", "\e$B\x79\x21".encode("utf-8", "cp50221"))
    assert_equal("\u5fde", "\e$B\x7A\x21".encode("utf-8", "cp50221"))
    assert_equal("\u72be", "\e$B\x7B\x21".encode("utf-8", "cp50221"))
    assert_equal("\u91d7", "\e$B\x7C\x21".encode("utf-8", "cp50221"))
    assert_equal("\xA1\xDF".force_encoding("sjis"),
                 "\e(I!_\e(B".encode("sjis","cp50220"))
  end

  def test_to_cp50221
    assert_equal("\e$B!#!,\e(B".force_encoding("cp50220"),
                 "\xA1\xDF".encode("cp50220","sjis"))
    assert_equal("\e$B%*!+%,%I%J!+%N!+%P%\\%^!+%Q%]%\"\e(B".force_encoding("cp50220"),
        "\xB5\xDE\xB6\xDE\xC4\xDE\xC5\xDE\xC9\xDE\xCA\xDE\xCE\xDE\xCF\xDE\xCA\xDF\xCE\xDF\xB1".
                 encode("cp50220", "sjis"))
  end

  def test_iso_2022_jp_1
    # check_both_ways("\u9299", "\x1b$(Dd!\x1b(B", "iso-2022-jp-1") # JIS X 0212 区68 点01 銙
  end

  def test_unicode_public_review_issue_121 # see http://www.unicode.org/review/pr-121.html
    assert_equal("\x00\x61\xFF\xFD\xFF\xFD\xFF\xFD\x00\x62".force_encoding('UTF-16BE'),
      "\x61\xF1\x80\x80\xE1\x80\xC2\x62".encode('UTF-16BE', 'UTF-8', invalid: :replace)) # option 2
    assert_equal("\x61\x00\xFD\xFF\xFD\xFF\xFD\xFF\x62\x00".force_encoding('UTF-16LE'),
      "\x61\xF1\x80\x80\xE1\x80\xC2\x62".encode('UTF-16LE', 'UTF-8', invalid: :replace)) # option 2

    # additional clarification
    assert_equal("\xFF\xFD\xFF\xFD\xFF\xFD\xFF\xFD".force_encoding('UTF-16BE'),
      "\xF0\x80\x80\x80".encode('UTF-16BE', 'UTF-8', invalid: :replace))
    assert_equal("\xFD\xFF\xFD\xFF\xFD\xFF\xFD\xFF".force_encoding('UTF-16LE'),
      "\xF0\x80\x80\x80".encode('UTF-16LE', 'UTF-8', invalid: :replace))
  end

  def test_yen_sign
    check_both_ways("\u005C", "\x5C", "Shift_JIS")
    check_both_ways("\u005C", "\x5C", "Windows-31J")
    check_both_ways("\u005C", "\x5C", "EUC-JP")
    check_both_ways("\u005C", "\x5C", "eucJP-ms")
    check_both_ways("\u005C", "\x5C", "CP51932")
    check_both_ways("\u005C", "\x5C", "ISO-2022-JP")
    assert_equal("\u005C", "\e(B\x5C\e(B".encode("UTF-8", "ISO-2022-JP"))
    assert_equal("\u005C", "\e(J\x5C\e(B".encode("UTF-8", "ISO-2022-JP"))
    assert_equal("\u005C", "\x5C".encode("stateless-ISO-2022-JP", "ISO-2022-JP"))
    assert_equal("\u005C", "\e(J\x5C\e(B".encode("stateless-ISO-2022-JP", "ISO-2022-JP"))
    assert_raise(Encoding::UndefinedConversionError) { "\u00A5".encode("Shift_JIS") }
    assert_raise(Encoding::UndefinedConversionError) { "\u00A5".encode("Windows-31J") }
    assert_raise(Encoding::UndefinedConversionError) { "\u00A5".encode("EUC-JP") }
    assert_raise(Encoding::UndefinedConversionError) { "\u00A5".encode("eucJP-ms") }
    assert_raise(Encoding::UndefinedConversionError) { "\u00A5".encode("CP51932") }

    # FULLWIDTH REVERSE SOLIDUS
    check_both_ways("\uFF3C", "\x81\x5F", "Shift_JIS")
    check_both_ways("\uFF3C", "\x81\x5F", "Windows-31J")
    check_both_ways("\uFF3C", "\xA1\xC0", "EUC-JP")
    check_both_ways("\uFF3C", "\xA1\xC0", "eucJP-ms")
    check_both_ways("\uFF3C", "\xA1\xC0", "CP51932")
  end

  def test_tilde_overline
    check_both_ways("\u007E", "\x7E", "Shift_JIS")
    check_both_ways("\u007E", "\x7E", "Windows-31J")
    check_both_ways("\u007E", "\x7E", "EUC-JP")
    check_both_ways("\u007E", "\x7E", "eucJP-ms")
    check_both_ways("\u007E", "\x7E", "CP51932")
    check_both_ways("\u007E", "\x7E", "ISO-2022-JP")
    assert_equal("\u007E", "\e(B\x7E\e(B".encode("UTF-8", "ISO-2022-JP"))
    assert_equal("\u007E", "\e(J\x7E\e(B".encode("UTF-8", "ISO-2022-JP"))
    assert_equal("\u007E", "\x7E".encode("stateless-ISO-2022-JP", "ISO-2022-JP"))
    assert_equal("\u007E", "\e(J\x7E\e(B".encode("stateless-ISO-2022-JP", "ISO-2022-JP"))
    assert_raise(Encoding::UndefinedConversionError) { "\u203E".encode("Shift_JIS") }
    assert_raise(Encoding::UndefinedConversionError) { "\u203E".encode("Windows-31J") }
    assert_raise(Encoding::UndefinedConversionError) { "\u203E".encode("EUC-JP") }
    assert_raise(Encoding::UndefinedConversionError) { "\u203E".encode("eucJP-ms") }
    assert_raise(Encoding::UndefinedConversionError) { "\u203E".encode("CP51932") }
  end

  def test_gb2312
    check_both_ways("\u3000", "\xA1\xA1", 'GB2312') # full-width space
    check_both_ways("\u3013", "\xA1\xFE", 'GB2312') # 〓
    assert_raise(Encoding::UndefinedConversionError) { "\xA2\xB0".encode("utf-8", 'GB2312') }
    check_both_ways("\u2488", "\xA2\xB1", 'GB2312') # ⒈
    assert_raise(Encoding::UndefinedConversionError) { "\xA2\xE4".encode("utf-8", 'GB2312') }
    check_both_ways("\u3220", "\xA2\xE5", 'GB2312') # ㈠
    assert_raise(Encoding::UndefinedConversionError) { "\xA2\xF0".encode("utf-8", 'GB2312') }
    check_both_ways("\u2160", "\xA2\xF1", 'GB2312') # Ⅰ
    check_both_ways("\uFF01", "\xA3\xA1", 'GB2312') # ！
    check_both_ways("\uFFE3", "\xA3\xFE", 'GB2312') # ￣
    check_both_ways("\u3041", "\xA4\xA1", 'GB2312') # ぁ
    check_both_ways("\u30A1", "\xA5\xA1", 'GB2312') # ァ
    check_both_ways("\u0391", "\xA6\xA1", 'GB2312') # Α
    check_both_ways("\u03B1", "\xA6\xC1", 'GB2312') # α
    check_both_ways("\u0410", "\xA7\xA1", 'GB2312') # А
    check_both_ways("\u0430", "\xA7\xD1", 'GB2312') # а
    check_both_ways("\u0101", "\xA8\xA1", 'GB2312') # ā
    assert_raise(Encoding::UndefinedConversionError) { "\xA8\xC4".encode("utf-8", 'GB2312') }
    check_both_ways("\u3105", "\xA8\xC5", 'GB2312') # ㄅ
    assert_raise(Encoding::UndefinedConversionError) { "\xA9\xA3".encode("utf-8", 'GB2312') }
    check_both_ways("\u2500", "\xA9\xA4", 'GB2312') # ─
    check_both_ways("\u554A", "\xB0\xA1", 'GB2312') # 啊
    check_both_ways("\u5265", "\xB0\xFE", 'GB2312') # 剥
    check_both_ways("\u4FCA", "\xBF\xA1", 'GB2312') # 俊
    check_both_ways("\u5080", "\xBF\xFE", 'GB2312') # 傀
    check_both_ways("\u9988", "\xC0\xA1", 'GB2312') # 馈
    check_both_ways("\u4FD0", "\xC0\xFE", 'GB2312') # 俐
    check_both_ways("\u7A00", "\xCF\xA1", 'GB2312') # 稀
    check_both_ways("\u6653", "\xCF\xFE", 'GB2312') # 晓
    check_both_ways("\u5C0F", "\xD0\xA1", 'GB2312') # 小
    check_both_ways("\u7384", "\xD0\xFE", 'GB2312') # 玄
    check_both_ways("\u4F4F", "\xD7\xA1", 'GB2312') # 住
    check_both_ways("\u5EA7", "\xD7\xF9", 'GB2312') # 座
    assert_raise(Encoding::UndefinedConversionError) { "\xD7\xFA".encode("utf-8", 'GB2312') }
    check_both_ways("\u647A", "\xDF\xA1", 'GB2312') # 摺
    check_both_ways("\u553C", "\xDF\xFE", 'GB2312') # 唼
    check_both_ways("\u5537", "\xE0\xA1", 'GB2312') # 唷
    check_both_ways("\u5E3C", "\xE0\xFE", 'GB2312') # 帼
    check_both_ways("\u94E9", "\xEF\xA1", 'GB2312') # 铩
    check_both_ways("\u7A14", "\xEF\xFE", 'GB2312') # 稔
    check_both_ways("\u7A39", "\xF0\xA1", 'GB2312') # 稹
    check_both_ways("\u7619", "\xF0\xFE", 'GB2312') # 瘙
    check_both_ways("\u9CCC", "\xF7\xA1", 'GB2312') # 鳌
    check_both_ways("\u9F44", "\xF7\xFE", 'GB2312') # 齄
    check_both_ways("\u9752\u5C71\u5B66\u9662\u5927\u5B66", "\xC7\xE0\xC9\xBD\xD1\xA7\xD4\xBA\xB4\xF3\xD1\xA7", 'GB2312') # 青山学院大学
  end

  def test_gbk
    check_both_ways("\u4E02", "\x81\x40", 'GBK') # 丂
    check_both_ways("\u4E8A", "\x81\x7E", 'GBK') # 亊
    check_both_ways("\u4E90", "\x81\x80", 'GBK') # 亐
    check_both_ways("\u4FA2", "\x81\xFE", 'GBK') # 侢
    check_both_ways("\u5EC6", "\x8F\x40", 'GBK') # 廆
    check_both_ways("\u5F24", "\x8F\x7E", 'GBK') # 弤
    check_both_ways("\u5F28", "\x8F\x80", 'GBK') # 弨
    check_both_ways("\u6007", "\x8F\xFE", 'GBK') # 怇
    check_both_ways("\u6008", "\x90\x40", 'GBK') # 怈
    check_both_ways("\u6080", "\x90\x7E", 'GBK') # 悀
    check_both_ways("\u6081", "\x90\x80", 'GBK') # 悁
    check_both_ways("\u6146", "\x90\xFE", 'GBK') # 慆
    check_both_ways("\u70DC", "\x9F\x40", 'GBK') # 烜
    check_both_ways("\u7134", "\x9F\x7E", 'GBK') # 焴
    check_both_ways("\u7135", "\x9F\x80", 'GBK') # 焵
    check_both_ways("\u71D3", "\x9F\xFE", 'GBK') # 燓
    check_both_ways("\u71D6", "\xA0\x40", 'GBK') # 燖
    check_both_ways("\u721A", "\xA0\x7E", 'GBK') # 爚
    check_both_ways("\u721B", "\xA0\x80", 'GBK') # 爛
    check_both_ways("\u72DB", "\xA0\xFE", 'GBK') # 狛
    check_both_ways("\u3000", "\xA1\xA1", 'GBK') # full-width space
    check_both_ways("\u3001", "\xA1\xA2", 'GBK') # 、
    check_both_ways("\u3013", "\xA1\xFE", 'GBK') # 〓
    assert_raise(Encoding::UndefinedConversionError) { "\xA2\xA0".encode("utf-8", 'GBK') }
    check_both_ways("\u2170", "\xA2\xA1", 'GBK') # ⅰ
    assert_raise(Encoding::UndefinedConversionError) { "\xA2\xB0".encode("utf-8", 'GBK') }
    check_both_ways("\u2488", "\xA2\xB1", 'GBK') # ⒈
    assert_raise(Encoding::UndefinedConversionError) { "\xA2\xE4".encode("utf-8", 'GBK') }
    check_both_ways("\u3220", "\xA2\xE5", 'GBK') # ㈠
    assert_raise(Encoding::UndefinedConversionError) { "\xA2\xF0".encode("utf-8", 'GBK') }
    check_both_ways("\u2160", "\xA2\xF1", 'GBK') # Ⅰ
    assert_raise(Encoding::UndefinedConversionError) { "\xA3\xA0".encode("utf-8", 'GBK') }
    check_both_ways("\uFF01", "\xA3\xA1", 'GBK') # ！
    check_both_ways("\uFFE3", "\xA3\xFE", 'GBK') # ￣
    assert_raise(Encoding::UndefinedConversionError) { "\xA4\xA0".encode("utf-8", 'GBK') }
    check_both_ways("\u3041", "\xA4\xA1", 'GBK') # ぁ
    assert_raise(Encoding::UndefinedConversionError) { "\xA5\xA0".encode("utf-8", 'GBK') }
    check_both_ways("\u30A1", "\xA5\xA1", 'GBK') # ァ
    check_both_ways("\u0391", "\xA6\xA1", 'GBK') # Α
    check_both_ways("\u03B1", "\xA6\xC1", 'GBK') # α
    assert_raise(Encoding::UndefinedConversionError) { "\xA6\xED".encode("utf-8", 'GBK') }
    check_both_ways("\uFE3B", "\xA6\xEE", 'GBK') # ︻
    check_both_ways("\u0410", "\xA7\xA1", 'GBK') # А
    check_both_ways("\u0430", "\xA7\xD1", 'GBK') # а
    check_both_ways("\u02CA", "\xA8\x40", 'GBK') # ˊ
    check_both_ways("\u2587", "\xA8\x7E", 'GBK') # ▇
    assert_raise(Encoding::UndefinedConversionError) { "\xA8\x96".encode("utf-8", 'GBK') }
    check_both_ways("\u0101", "\xA8\xA1", 'GBK') # ā
    assert_raise(Encoding::UndefinedConversionError) { "\xA8\xBC".encode("utf-8", 'GBK') }
    assert_raise(Encoding::UndefinedConversionError) { "\xA8\xBF".encode("utf-8", 'GBK') }
    assert_raise(Encoding::UndefinedConversionError) { "\xA8\xC4".encode("utf-8", 'GBK') }
    check_both_ways("\u3105", "\xA8\xC5", 'GBK') # ㄅ
    check_both_ways("\u3021", "\xA9\x40", 'GBK') # 〡
    assert_raise(Encoding::UndefinedConversionError) { "\xA9\x58".encode("utf-8", 'GBK') }
    assert_raise(Encoding::UndefinedConversionError) { "\xA9\x5B".encode("utf-8", 'GBK') }
    assert_raise(Encoding::UndefinedConversionError) { "\xA9\x5D".encode("utf-8", 'GBK') }
    check_both_ways("\u3007", "\xA9\x96", 'GBK') # 〇
    assert_raise(Encoding::UndefinedConversionError) { "\xA9\xA3".encode("utf-8", 'GBK') }
    check_both_ways("\u2500", "\xA9\xA4", 'GBK') # ─
    assert_raise(Encoding::UndefinedConversionError) { "\xA9\xF0".encode("utf-8", 'GBK') }
    check_both_ways("\u7588", "\xAF\x40", 'GBK') # 疈
    check_both_ways("\u7607", "\xAF\x7E", 'GBK') # 瘇
    check_both_ways("\u7608", "\xAF\x80", 'GBK') # 瘈
    check_both_ways("\u7644", "\xAF\xA0", 'GBK') # 癄
    assert_raise(Encoding::UndefinedConversionError) { "\xAF\xA1".encode("utf-8", 'GBK') }
    check_both_ways("\u7645", "\xB0\x40", 'GBK') # 癅
    check_both_ways("\u769B", "\xB0\x7E", 'GBK') # 皛
    check_both_ways("\u769C", "\xB0\x80", 'GBK') # 皜
    check_both_ways("\u5265", "\xB0\xFE", 'GBK') # 剥
    check_both_ways("\u7DFB", "\xBF\x40", 'GBK') # 緻
    check_both_ways("\u7E39", "\xBF\x7E", 'GBK') # 縹
    check_both_ways("\u7E3A", "\xBF\x80", 'GBK') # 縺
    check_both_ways("\u5080", "\xBF\xFE", 'GBK') # 傀
    check_both_ways("\u7E5E", "\xC0\x40", 'GBK') # 繞
    check_both_ways("\u7E9E", "\xC0\x7E", 'GBK') # 纞
    check_both_ways("\u7EAE", "\xC0\x80", 'GBK') # 纮
    check_both_ways("\u4FD0", "\xC0\xFE", 'GBK') # 俐
    check_both_ways("\u87A5", "\xCF\x40", 'GBK') # 螥
    check_both_ways("\u87F8", "\xCF\x7E", 'GBK') # 蟸
    check_both_ways("\u87FA", "\xCF\x80", 'GBK') # 蟺
    check_both_ways("\u6653", "\xCF\xFE", 'GBK') # 晓
    check_both_ways("\u8824", "\xD0\x40", 'GBK') # 蠤
    check_both_ways("\u887A", "\xD0\x7E", 'GBK') # 衺
    check_both_ways("\u887B", "\xD0\x80", 'GBK') # 衻
    check_both_ways("\u7384", "\xD0\xFE", 'GBK') # 玄
    check_both_ways("\u9019", "\xDF\x40", 'GBK') # 這
    check_both_ways("\u9081", "\xDF\x7E", 'GBK') # 邁
    check_both_ways("\u9084", "\xDF\x80", 'GBK') # 還
    check_both_ways("\u553C", "\xDF\xFE", 'GBK') # 唼
    check_both_ways("\u90C2", "\xE0\x40", 'GBK') # 郂
    check_both_ways("\u911C", "\xE0\x7E", 'GBK') # 鄜
    check_both_ways("\u911D", "\xE0\x80", 'GBK') # 鄝
    check_both_ways("\u5E3C", "\xE0\xFE", 'GBK') # 帼
    check_both_ways("\u986F", "\xEF\x40", 'GBK') # 顯
    check_both_ways("\u98E4", "\xEF\x7E", 'GBK') # 飤
    check_both_ways("\u98E5", "\xEF\x80", 'GBK') # 飥
    check_both_ways("\u7A14", "\xEF\xFE", 'GBK') # 稔
    check_both_ways("\u9908", "\xF0\x40", 'GBK') # 餈
    check_both_ways("\u9949", "\xF0\x7E", 'GBK') # 饉
    check_both_ways("\u994A", "\xF0\x80", 'GBK') # 饊
    check_both_ways("\u7619", "\xF0\xFE", 'GBK') # 瘙
    check_both_ways("\u9F32", "\xFD\x40", 'GBK') # 鼲
    check_both_ways("\u9F78", "\xFD\x7E", 'GBK') # 齸
    check_both_ways("\u9F79", "\xFD\x80", 'GBK') # 齹
    check_both_ways("\uF9F1", "\xFD\xA0", 'GBK') # 隣
    assert_raise(Encoding::UndefinedConversionError) { "\xFD\xA1".encode("utf-8", 'GBK') }
    check_both_ways("\uFA0C", "\xFE\x40", 'GBK') # 兀
    check_both_ways("\uFA29", "\xFE\x4F", 'GBK') # 﨩
    assert_raise(Encoding::UndefinedConversionError) { "\xFE\x50".encode("utf-8", 'GBK') }
    check_both_ways("\u9752\u5C71\u5B66\u9662\u5927\u5B66", "\xC7\xE0\xC9\xBD\xD1\xA7\xD4\xBA\xB4\xF3\xD1\xA7", 'GBK') # 青山学院大学
    check_both_ways("\u795E\u6797\u7FA9\u535A", "\xC9\xF1\xC1\xD6\xC1\x78\xB2\xA9", 'GBK') # 神林義博
  end

  def test_gb18030
    # overall roundtrip test
    all_unicode = (0x0..0xD7FF).to_a.pack 'U*' #追加
    all_unicode << (0xE000..0xFFFF).to_a.pack("U*") #追加

    assert_equal(all_unicode, all_unicode.encode("gb18030").encode("UTF-8")) #追加

    # tests from GBK
    check_both_ways("\u4E02", "\x81\x40", 'GB18030') #
    check_both_ways("\u4E8A", "\x81\x7E", 'GB18030') #
    check_both_ways("\u4E90", "\x81\x80", 'GB18030') #
    check_both_ways("\u4FA2", "\x81\xFE", 'GB18030') # 侢
    check_both_ways("\u5EC6", "\x8F\x40", 'GB18030') #
    check_both_ways("\u5F24", "\x8F\x7E", 'GB18030') # 弤
    check_both_ways("\u5F28", "\x8F\x80", 'GB18030') # 弨
    check_both_ways("\u6007", "\x8F\xFE", 'GB18030') #
    check_both_ways("\u6008", "\x90\x40", 'GB18030') #
    check_both_ways("\u6080", "\x90\x7E", 'GB18030') # 悀
    check_both_ways("\u6081", "\x90\x80", 'GB18030') #
    check_both_ways("\u6146", "\x90\xFE", 'GB18030') #
    check_both_ways("\u70DC", "\x9F\x40", 'GB18030') #
    check_both_ways("\u7134", "\x9F\x7E", 'GB18030') # 焴
    check_both_ways("\u7135", "\x9F\x80", 'GB18030') # 焵
    check_both_ways("\u71D3", "\x9F\xFE", 'GB18030') #
    check_both_ways("\u71D6", "\xA0\x40", 'GB18030') #
    check_both_ways("\u721A", "\xA0\x7E", 'GB18030') #
    check_both_ways("\u721B", "\xA0\x80", 'GB18030') #
    check_both_ways("\u72DB", "\xA0\xFE", 'GB18030') #
    check_both_ways("\u3000", "\xA1\xA1", 'GB18030') # full-width space
    check_both_ways("\u3001", "\xA1\xA2", 'GB18030') #
    check_both_ways("\u3013", "\xA1\xFE", 'GB18030') #
    #assert_raise(Encoding::UndefinedConversionError) { "\xA2\xA0".encode("utf-8", 'GB18030') }
    check_both_ways("\u2170", "\xA2\xA1", 'GB18030') # ⅰ
    #assert_raise(Encoding::UndefinedConversionError) { "\xA2\xB0".encode("utf-8", 'GB18030') }
    check_both_ways("\u2488", "\xA2\xB1", 'GB18030') #
    #assert_raise(Encoding::UndefinedConversionError) { "\xA2\xE4".encode("utf-8", 'GB18030') }
    check_both_ways("\u3220", "\xA2\xE5", 'GB18030') # ㈠
    #assert_raise(Encoding::UndefinedConversionError) { "\xA2\xF0".encode("utf-8", 'GB18030') }
    check_both_ways("\u2160", "\xA2\xF1", 'GB18030') # Ⅰ
    #assert_raise(Encoding::UndefinedConversionError) { "\xA3\xA0".encode("utf-8", 'GB18030') }
    check_both_ways("\uFF01", "\xA3\xA1", 'GB18030') # E
    check_both_ways("\uFFE3", "\xA3\xFE", 'GB18030') # E
    #assert_raise(Encoding::UndefinedConversionError) { "\xA4\xA0".encode("utf-8", 'GB18030') }
    check_both_ways("\u3041", "\xA4\xA1", 'GB18030') #
    #assert_raise(Encoding::UndefinedConversionError) { "\xA5\xA0".encode("utf-8", 'GB18030') }
    check_both_ways("\u30A1", "\xA5\xA1", 'GB18030') # ァ
    check_both_ways("\u0391", "\xA6\xA1", 'GB18030') #
    check_both_ways("\u03B1", "\xA6\xC1", 'GB18030') # α
    #assert_raise(Encoding::UndefinedConversionError) { "\xA6\xED".encode("utf-8", 'GB18030') }
    check_both_ways("\uFE3B", "\xA6\xEE", 'GB18030') # E
    check_both_ways("\u0410", "\xA7\xA1", 'GB18030') #
    check_both_ways("\u0430", "\xA7\xD1", 'GB18030') # а
    check_both_ways("\u02CA", "\xA8\x40", 'GB18030') #
    check_both_ways("\u2587", "\xA8\x7E", 'GB18030') #
    #assert_raise(Encoding::UndefinedConversionError) { "\xA8\x96".encode("utf-8", 'GB18030') }
    check_both_ways("\u0101", "\xA8\xA1", 'GB18030') #
    #assert_raise(Encoding::UndefinedConversionError) { "\xA8\xBC".encode("utf-8", 'GB18030') }
    #assert_raise(Encoding::UndefinedConversionError) { "\xA8\xBF".encode("utf-8", 'GB18030') }
    #assert_raise(Encoding::UndefinedConversionError) { "\xA8\xC4".encode("utf-8", 'GB18030') }
    check_both_ways("\u3105", "\xA8\xC5", 'GB18030') #
    check_both_ways("\u3021", "\xA9\x40", 'GB18030') # 〡
    #assert_raise(Encoding::UndefinedConversionError) { "\xA9\x58".encode("utf-8", 'GB18030') }
    #assert_raise(Encoding::UndefinedConversionError) { "\xA9\x5B".encode("utf-8", 'GB18030') }
    #assert_raise(Encoding::UndefinedConversionError) { "\xA9\x5D".encode("utf-8", 'GB18030') }
    check_both_ways("\u3007", "\xA9\x96", 'GB18030') #
    #assert_raise(Encoding::UndefinedConversionError) { "\xA9\xA3".encode("utf-8", 'GB18030') }
    check_both_ways("\u2500", "\xA9\xA4", 'GB18030') # ─
    #assert_raise(Encoding::UndefinedConversionError) { "\xA9\xF0".encode("utf-8", 'GB18030') }
    check_both_ways("\u7588", "\xAF\x40", 'GB18030') #
    check_both_ways("\u7607", "\xAF\x7E", 'GB18030') #
    check_both_ways("\u7608", "\xAF\x80", 'GB18030') #
    check_both_ways("\u7644", "\xAF\xA0", 'GB18030') #
    #assert_raise(Encoding::UndefinedConversionError) { "\xAF\xA1".encode("utf-8", 'GB18030') }
    check_both_ways("\u7645", "\xB0\x40", 'GB18030') #
    check_both_ways("\u769B", "\xB0\x7E", 'GB18030') #
    check_both_ways("\u769C", "\xB0\x80", 'GB18030') #
    check_both_ways("\u5265", "\xB0\xFE", 'GB18030') # 剥
    check_both_ways("\u7DFB", "\xBF\x40", 'GB18030') # 緻
    check_both_ways("\u7E39", "\xBF\x7E", 'GB18030') # 縹
    check_both_ways("\u7E3A", "\xBF\x80", 'GB18030') # 縺
    check_both_ways("\u5080", "\xBF\xFE", 'GB18030') # 傀
    check_both_ways("\u7E5E", "\xC0\x40", 'GB18030') #
    check_both_ways("\u7E9E", "\xC0\x7E", 'GB18030') #
    check_both_ways("\u7EAE", "\xC0\x80", 'GB18030') # 纮
    check_both_ways("\u4FD0", "\xC0\xFE", 'GB18030') #
    check_both_ways("\u87A5", "\xCF\x40", 'GB18030') # 螥
    check_both_ways("\u87F8", "\xCF\x7E", 'GB18030') # 蟸
    check_both_ways("\u87FA", "\xCF\x80", 'GB18030') # 蟺
    check_both_ways("\u6653", "\xCF\xFE", 'GB18030') #
    check_both_ways("\u8824", "\xD0\x40", 'GB18030') # 蠤
    check_both_ways("\u887A", "\xD0\x7E", 'GB18030') # 衺
    check_both_ways("\u887B", "\xD0\x80", 'GB18030') # 衻
    check_both_ways("\u7384", "\xD0\xFE", 'GB18030') #
    check_both_ways("\u9019", "\xDF\x40", 'GB18030') #
    check_both_ways("\u9081", "\xDF\x7E", 'GB18030') #
    check_both_ways("\u9084", "\xDF\x80", 'GB18030') #
    check_both_ways("\u553C", "\xDF\xFE", 'GB18030') # 唼
    check_both_ways("\u90C2", "\xE0\x40", 'GB18030') #
    check_both_ways("\u911C", "\xE0\x7E", 'GB18030') #
    check_both_ways("\u911D", "\xE0\x80", 'GB18030') #
    check_both_ways("\u5E3C", "\xE0\xFE", 'GB18030') # 帼
    check_both_ways("\u986F", "\xEF\x40", 'GB18030') # 顯
    check_both_ways("\u98E4", "\xEF\x7E", 'GB18030') # 飤
    check_both_ways("\u98E5", "\xEF\x80", 'GB18030') # 飥
    check_both_ways("\u7A14", "\xEF\xFE", 'GB18030') #
    check_both_ways("\u9908", "\xF0\x40", 'GB18030') #
    check_both_ways("\u9949", "\xF0\x7E", 'GB18030') #
    check_both_ways("\u994A", "\xF0\x80", 'GB18030') #
    check_both_ways("\u7619", "\xF0\xFE", 'GB18030') #
    check_both_ways("\u9F32", "\xFD\x40", 'GB18030') # 鼲
    check_both_ways("\u9F78", "\xFD\x7E", 'GB18030') # 齸
    check_both_ways("\u9F79", "\xFD\x80", 'GB18030') # 齹
    check_both_ways("\uF9F1", "\xFD\xA0", 'GB18030') # E
    #assert_raise(Encoding::UndefinedConversionError) { "\xFD\xA1".encode("utf-8", 'GB18030') }
    check_both_ways("\uFA0C", "\xFE\x40", 'GB18030') # E
    check_both_ways("\uFA29", "\xFE\x4F", 'GB18030') # E
    #assert_raise(Encoding::UndefinedConversionError) { "\xFE\x50".encode("utf-8", 'GB18030') }
    check_both_ways("\u9752\u5C71\u5B66\u9662\u5927\u5B66", "\xC7\xE0\xC9\xBD\xD1\xA7\xD4\xBA\xB4\xF3\xD1\xA7", 'GB18030') # 青山学院大学
    check_both_ways("\u795E\u6797\u7FA9\u535A", "\xC9\xF1\xC1\xD6\xC1\x78\xB2\xA9", 'GB18030') # 神林義

    # new tests for GB18030
    check_both_ways("\u9FA6", "\x82\x35\x8F\x33", 'GB18030') # 龦
    check_both_ways("\uD7FF", "\x83\x36\xC7\x38", 'GB18030') # No name ()

    check_both_ways("\u0452", "\x81\x30\xD3\x30", 'GB18030') #
    check_both_ways("\u200F", "\x81\x36\xA5\x31", 'GB18030') # RIGHT-TO-LEFT MARK

    check_both_ways("\uE865", "\x83\x36\xD0\x30", 'GB18030') # No name (Private Use Area)
    check_both_ways("\uF92B", "\x84\x30\x85\x34", 'GB18030') # E

    check_both_ways("\u2643", "\x81\x37\xA8\x39", 'GB18030') #
    check_both_ways("\u2E80", "\x81\x38\xFD\x38", 'GB18030') # ⺀

    check_both_ways("\uFA2A", "\x84\x30\x9C\x38", 'GB18030') # E
    check_both_ways("\uFE2F", "\x84\x31\x85\x37", 'GB18030') # No name (Combining Half Marks)

    check_both_ways("\u3CE1", "\x82\x31\xD4\x38", 'GB18030') # 㳡
    check_both_ways("\u4055", "\x82\x32\xAF\x32", 'GB18030') #

    check_both_ways("\u361B", "\x82\x30\xA6\x33", 'GB18030') #
    check_both_ways("\u3917", "\x82\x30\xF2\x37", 'GB18030') #

    check_both_ways("\u49B8", "\x82\x34\xA1\x31", 'GB18030') # 䦸
    check_both_ways("\u4C76", "\x82\x34\xE7\x33", 'GB18030') # 䱶

    check_both_ways("\u4160", "\x82\x32\xC9\x37", 'GB18030') # 䅠
    check_both_ways("\u4336", "\x82\x32\xF8\x37", 'GB18030') # 䌶

    check_both_ways("\u478E", "\x82\x33\xE8\x38", 'GB18030') #
    check_both_ways("\u4946", "\x82\x34\x96\x38", 'GB18030') #

    check_both_ways("\u44D7", "\x82\x33\xA3\x39", 'GB18030') #
    check_both_ways("\u464B", "\x82\x33\xC9\x31", 'GB18030') #

    check_both_ways("\uFFE6", "\x84\x31\xA2\x34", 'GB18030') # E
    check_both_ways("\uFFFF", "\x84\x31\xA4\x39", 'GB18030') # not a character

    check_both_ways("\u{10000}", "\x90\x30\x81\x30", 'GB18030') # 𐀀
    check_both_ways("\u{10FFFE}", "\xE3\x32\x9A\x34", 'GB18030') # No name (Not a character)
    check_both_ways("\u{10FFFF}", "\xE3\x32\x9A\x35", 'GB18030') # No name (Not a character)
  end

  def test_Big5
    check_both_ways("\u3000", "\xA1\x40", 'Big5') # full-width space
    check_both_ways("\uFE5A", "\xA1\x7E", 'Big5') # ﹚
    check_both_ways("\uFE5B", "\xA1\xA1", 'Big5') # ﹛
    #check_both_ways("\uFF0F", "\xA1\xFE", 'Big5') # ／
    check_both_ways("\uFF57", "\xA3\x40", 'Big5') # ｗ
    check_both_ways("\u310F", "\xA3\x7E", 'Big5') # ㄏ
    check_both_ways("\u3110", "\xA3\xA1", 'Big5') # ㄐ
    check_both_ways("\u02CB", "\xA3\xBF", 'Big5') # ˋ
    assert_raise(Encoding::UndefinedConversionError) { "\xA3\xC0".encode("utf-8", 'Big5') }
    check_both_ways("\u6D6C", "\xAF\x40", 'Big5') # 浬
    check_both_ways("\u7837", "\xAF\x7E", 'Big5') # 砷
    check_both_ways("\u7825", "\xAF\xA1", 'Big5') # 砥
    check_both_ways("\u8343", "\xAF\xFE", 'Big5') # 荃
    check_both_ways("\u8654", "\xB0\x40", 'Big5') # 虔
    check_both_ways("\u9661", "\xB0\x7E", 'Big5') # 陡
    check_both_ways("\u965B", "\xB0\xA1", 'Big5') # 陛
    check_both_ways("\u5A40", "\xB0\xFE", 'Big5') # 婀
    check_both_ways("\u6FC3", "\xBF\x40", 'Big5') # 濃
    check_both_ways("\u7E0A", "\xBF\x7E", 'Big5') # 縊
    check_both_ways("\u7E11", "\xBF\xA1", 'Big5') # 縑
    check_both_ways("\u931A", "\xBF\xFE", 'Big5') # 錚
    check_both_ways("\u9310", "\xC0\x40", 'Big5') # 錐
    check_both_ways("\u5687", "\xC0\x7E", 'Big5') # 嚇
    check_both_ways("\u568F", "\xC0\xA1", 'Big5') # 嚏
    check_both_ways("\u77AC", "\xC0\xFE", 'Big5') # 瞬
    check_both_ways("\u8B96", "\xC6\x40", 'Big5') # 讖
    check_both_ways("\u7C72", "\xC6\x7E", 'Big5') # 籲
    #assert_raise(Encoding::UndefinedConversionError) { "\xC6\xA1".encode("utf-8", 'Big5') }
    #assert_raise(Encoding::UndefinedConversionError) { "\xC7\x40".encode("utf-8", 'Big5') }
    #assert_raise(Encoding::UndefinedConversionError) { "\xC8\x40".encode("utf-8", 'Big5') }
    check_both_ways("\u4E42", "\xC9\x40", 'Big5') # 乂
    check_both_ways("\u6C15", "\xC9\x7E", 'Big5') # 氕
    check_both_ways("\u6C36", "\xC9\xA1", 'Big5') # 氶
    check_both_ways("\u6C4B", "\xC9\xFE", 'Big5') # 汋
    check_both_ways("\u67DC", "\xCF\x40", 'Big5') # 柜
    check_both_ways("\u6D42", "\xCF\x7E", 'Big5') # 浂
    check_both_ways("\u6D01", "\xCF\xA1", 'Big5') # 洁
    check_both_ways("\u7A80", "\xCF\xFE", 'Big5') # 窀
    check_both_ways("\u7A7E", "\xD0\x40", 'Big5') # 穾
    check_both_ways("\u82EA", "\xD0\x7E", 'Big5') # 苪
    check_both_ways("\u82E4", "\xD0\xA1", 'Big5') # 苤
    check_both_ways("\u54F1", "\xD0\xFE", 'Big5') # 哱
    check_both_ways("\u7A1B", "\xDF\x40", 'Big5') # 稛
    check_both_ways("\u816F", "\xDF\x7E", 'Big5') # 腯
    check_both_ways("\u8144", "\xDF\xA1", 'Big5') # 腄
    check_both_ways("\u89E4", "\xDF\xFE", 'Big5') # 觤
    check_both_ways("\u89E1", "\xE0\x40", 'Big5') # 觡
    check_both_ways("\u903F", "\xE0\x7E", 'Big5') # 逿
    check_both_ways("\u9044", "\xE0\xA1", 'Big5') # 遄
    check_both_ways("\u50E0", "\xE0\xFE", 'Big5') # 僠
    check_both_ways("\u979E", "\xEF\x40", 'Big5') # 鞞
    check_both_ways("\u9D30", "\xEF\x7E", 'Big5') # 鴰
    check_both_ways("\u9D45", "\xEF\xA1", 'Big5') # 鵅
    check_both_ways("\u7376", "\xEF\xFE", 'Big5') # 獶
    check_both_ways("\u74B8", "\xF0\x40", 'Big5') # 璸
    check_both_ways("\u81D2", "\xF0\x7E", 'Big5') # 臒
    check_both_ways("\u81D0", "\xF0\xA1", 'Big5') # 臐
    check_both_ways("\u8E67", "\xF0\xFE", 'Big5') # 蹧
    check_both_ways("\u7E98", "\xF9\x40", 'Big5') # 纘
    check_both_ways("\u9F0A", "\xF9\x7E", 'Big5') # 鼊
    check_both_ways("\u9FA4", "\xF9\xA1", 'Big5') # 龤
    check_both_ways("\u9F98", "\xF9\xD5", 'Big5') # 龘
    #assert_raise(Encoding::UndefinedConversionError) { "\xF9\xD6".encode("utf-8", 'Big5') }
    check_both_ways("\u795E\u6797\u7FA9\u535A", "\xAF\xAB\xAA\x4C\xB8\x71\xB3\xD5", 'Big5') # 神林義博
  end

  def test_Big5_Hkscs
    check_both_ways("\u3000", "\xA1\x40", 'Big5-HKSCS') # full-width space
    check_both_ways("\uFE5A", "\xA1\x7E", 'Big5-HKSCS') # ﹚
    check_both_ways("\uFE5B", "\xA1\xA1", 'Big5-HKSCS') # ﹛
    #check_both_ways("\uFF0F", "\xA1\xFE", 'Big5-HKSCS') # ／
    check_both_ways("\uFF57", "\xA3\x40", 'Big5-HKSCS') # ｗ
    check_both_ways("\u310F", "\xA3\x7E", 'Big5-HKSCS') # ㄏ
    check_both_ways("\u3110", "\xA3\xA1", 'Big5-HKSCS') # ㄐ
    check_both_ways("\u02CB", "\xA3\xBF", 'Big5-HKSCS') # ˋ
    #assert_raise(Encoding::UndefinedConversionError) { "\xA3\xC0".encode("utf-8", 'Big5-HKSCS') }
    check_both_ways("\u6D6C", "\xAF\x40", 'Big5-HKSCS') # 浬
    check_both_ways("\u7837", "\xAF\x7E", 'Big5-HKSCS') # 砷
    check_both_ways("\u7825", "\xAF\xA1", 'Big5-HKSCS') # 砥
    check_both_ways("\u8343", "\xAF\xFE", 'Big5-HKSCS') # 荃
    check_both_ways("\u8654", "\xB0\x40", 'Big5-HKSCS') # 虔
    check_both_ways("\u9661", "\xB0\x7E", 'Big5-HKSCS') # 陡
    check_both_ways("\u965B", "\xB0\xA1", 'Big5-HKSCS') # 陛
    check_both_ways("\u5A40", "\xB0\xFE", 'Big5-HKSCS') # 婀
    check_both_ways("\u6FC3", "\xBF\x40", 'Big5-HKSCS') # 濃
    check_both_ways("\u7E0A", "\xBF\x7E", 'Big5-HKSCS') # 縊
    check_both_ways("\u7E11", "\xBF\xA1", 'Big5-HKSCS') # 縑
    check_both_ways("\u931A", "\xBF\xFE", 'Big5-HKSCS') # 錚
    check_both_ways("\u9310", "\xC0\x40", 'Big5-HKSCS') # 錐
    check_both_ways("\u5687", "\xC0\x7E", 'Big5-HKSCS') # 嚇
    check_both_ways("\u568F", "\xC0\xA1", 'Big5-HKSCS') # 嚏
    check_both_ways("\u77AC", "\xC0\xFE", 'Big5-HKSCS') # 瞬
    check_both_ways("\u8B96", "\xC6\x40", 'Big5-HKSCS') # 讖
    check_both_ways("\u7C72", "\xC6\x7E", 'Big5-HKSCS') # 籲
    #assert_raise(Encoding::UndefinedConversionError) { "\xC6\xA1".encode("utf-8", 'Big5-HKSCS') }
    #assert_raise(Encoding::UndefinedConversionError) { "\xC7\x40".encode("utf-8", 'Big5-HKSCS') }
    #assert_raise(Encoding::UndefinedConversionError) { "\xC8\x40".encode("utf-8", 'Big5-HKSCS') }
    check_both_ways("\u4E42", "\xC9\x40", 'Big5-HKSCS') # 乂
    check_both_ways("\u6C15", "\xC9\x7E", 'Big5-HKSCS') # 氕
    check_both_ways("\u6C36", "\xC9\xA1", 'Big5-HKSCS') # 氶
    check_both_ways("\u6C4B", "\xC9\xFE", 'Big5-HKSCS') # 汋
    check_both_ways("\u67DC", "\xCF\x40", 'Big5-HKSCS') # 柜
    check_both_ways("\u6D42", "\xCF\x7E", 'Big5-HKSCS') # 浂
    check_both_ways("\u6D01", "\xCF\xA1", 'Big5-HKSCS') # 洁
    check_both_ways("\u7A80", "\xCF\xFE", 'Big5-HKSCS') # 窀
    check_both_ways("\u7A7E", "\xD0\x40", 'Big5-HKSCS') # 穾
    check_both_ways("\u82EA", "\xD0\x7E", 'Big5-HKSCS') # 苪
    check_both_ways("\u82E4", "\xD0\xA1", 'Big5-HKSCS') # 苤
    check_both_ways("\u54F1", "\xD0\xFE", 'Big5-HKSCS') # 哱
    check_both_ways("\u7A1B", "\xDF\x40", 'Big5-HKSCS') # 稛
    check_both_ways("\u816F", "\xDF\x7E", 'Big5-HKSCS') # 腯
    check_both_ways("\u8144", "\xDF\xA1", 'Big5-HKSCS') # 腄
    check_both_ways("\u89E4", "\xDF\xFE", 'Big5-HKSCS') # 觤
    check_both_ways("\u89E1", "\xE0\x40", 'Big5-HKSCS') # 觡
    check_both_ways("\u903F", "\xE0\x7E", 'Big5-HKSCS') # 逿
    check_both_ways("\u9044", "\xE0\xA1", 'Big5-HKSCS') # 遄
    check_both_ways("\u50E0", "\xE0\xFE", 'Big5-HKSCS') # 僠
    check_both_ways("\u979E", "\xEF\x40", 'Big5-HKSCS') # 鞞
    check_both_ways("\u9D30", "\xEF\x7E", 'Big5-HKSCS') # 鴰
    check_both_ways("\u9D45", "\xEF\xA1", 'Big5-HKSCS') # 鵅
    check_both_ways("\u7376", "\xEF\xFE", 'Big5-HKSCS') # 獶
    check_both_ways("\u74B8", "\xF0\x40", 'Big5-HKSCS') # 璸
    check_both_ways("\u81D2", "\xF0\x7E", 'Big5-HKSCS') # 臒
    check_both_ways("\u81D0", "\xF0\xA1", 'Big5-HKSCS') # 臐
    check_both_ways("\u8E67", "\xF0\xFE", 'Big5-HKSCS') # 蹧
    check_both_ways("\u7E98", "\xF9\x40", 'Big5-HKSCS') # 纘
    check_both_ways("\u9F0A", "\xF9\x7E", 'Big5-HKSCS') # 鼊
    check_both_ways("\u9FA4", "\xF9\xA1", 'Big5-HKSCS') # 龤
    check_both_ways("\u9F98", "\xF9\xD5", 'Big5-HKSCS') # 龘
    #check_both_ways("\u{23ED7}", "\x8E\x40", 'Big5-HKSCS') # 𣻗
    #assert_raise(Encoding::UndefinedConversionError) { "\xF9\xD6".encode("utf-8", 'Big5-HKSCS') }
    check_both_ways("\u795E\u6797\u7FA9\u535A", "\xAF\xAB\xAA\x4C\xB8\x71\xB3\xD5", 'Big5-HKSCS') # 神林義博
  end

  def test_Big5_UAO
    check_both_ways("\u4e17", "\x81\x40", 'Big5-UAO') # 丗
  end

  def test_nothing_changed
    a = "James".force_encoding("US-ASCII")
    b = a.encode("Shift_JIS")
    assert_equal(Encoding::US_ASCII, a.encoding)
    assert_equal(Encoding::Shift_JIS, b.encoding)
  end

  def test_utf8_mac
    assert_equal("\u{fb4d}", "\u05DB\u05BF".encode("UTF-8", "UTF8-MAC"))
    assert_equal("\u{1ff7}", "\u03C9\u0345\u0342".encode("UTF-8", "UTF8-MAC"))

    assert_equal("\u05DB\u05BF", "\u{fb4d}".encode("UTF8-MAC").force_encoding("UTF-8"))
    assert_equal("\u03C9\u0345\u0342", "\u{1ff7}".encode("UTF8-MAC").force_encoding("UTF-8"))

    check_both_ways("\u{e9 74 e8}", "e\u0301te\u0300", 'UTF8-MAC')
  end

  def test_fallback
    assert_equal("\u3042".encode("EUC-JP"), "\u{20000}".encode("EUC-JP",
        fallback: {"\u{20000}" => "\u3042".encode("EUC-JP")}))
    assert_equal("\u3042".encode("EUC-JP"), "\u{20000}".encode("EUC-JP",
        fallback: {"\u{20000}" => "\u3042"}))
    assert_equal("[ISU]", "\u{1F4BA}".encode("SJIS-KDDI",
        fallback: {"\u{1F4BA}" => "[ISU]"}))
  end

  def test_fallback_hash_default
    fallback = Hash.new {|h, x| "U+%.4X" % x.unpack("U")}
    assert_equal("U+3042", "\u{3042}".encode("US-ASCII", fallback: fallback))
  end

  def test_fallback_proc
    fallback = proc {|x| "U+%.4X" % x.unpack("U")}
    assert_equal("U+3042", "\u{3042}".encode("US-ASCII", fallback: fallback))
  end

  def test_fallback_method
    def (fallback = "U+%.4X").escape(x)
      self % x.unpack("U")
    end
    assert_equal("U+3042", "\u{3042}".encode("US-ASCII", fallback: fallback.method(:escape)))
  end
end
