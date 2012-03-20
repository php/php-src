require 'test/unit'
require 'kconv'

class TestKconv < Test::Unit::TestCase
  def setup
    @euc_str = "\
\xa5\xaa\xa5\xd6\xa5\xb8\xa5\xa7\xa5\xaf\xa5\xc8\xbb\xd8\xb8\xfe\
\xa5\xd7\xa5\xed\xa5\xb0\xa5\xe9\xa5\xdf\xa5\xf3\xa5\xb0\xb8\xc0\xb8\xec
\x52\x75\x62\x79".force_encoding('EUC-JP')
    @utf8_str = "\
\xe3\x82\xaa\xe3\x83\x96\xe3\x82\xb8\xe3\x82\xa7\
\xe3\x82\xaf\xe3\x83\x88\xe6\x8c\x87\xe5\x90\x91\
\xe3\x83\x97\xe3\x83\xad\xe3\x82\xb0\xe3\x83\xa9\xe3\x83\x9f\
\xe3\x83\xb3\xe3\x82\xb0\xe8\xa8\x80\xe8\xaa\x9e
\x52\x75\x62\x79".force_encoding('UTF-8')
    @sjis_str = "\
\x83\x49\x83\x75\x83\x57\x83\x46\x83\x4e\x83\x67\x8e\x77\x8c\xfc\
\x83\x76\x83\x8d\x83\x4f\x83\x89\x83\x7e\x83\x93\x83\x4f\x8c\xbe\x8c\xea
\x52\x75\x62\x79".force_encoding('Shift_JIS')
    @jis_str = "\
\x1b\x24\x42\x25\x2a\x25\x56\x25\x38\x25\x27\x25\x2f\x25\x48\x3b\x58\x38\x7e\
\x25\x57\x25\x6d\x25\x30\x25\x69\x25\x5f\x25\x73\x25\x30\x38\x40\x38\x6c\x1b\x28\x42
\x52\x75\x62\x79".force_encoding('ISO-2022-JP')
  end


  def test_eucjp
    assert(@euc_str.iseuc)
    assert_equal(::Kconv::EUC, Kconv.guess(@euc_str))
    assert_equal(@euc_str, @euc_str.toeuc)
    assert_equal(@euc_str, @sjis_str.toeuc)
    assert_equal(@euc_str, @utf8_str.toeuc)
    assert_equal(@euc_str, @jis_str.toeuc)
    assert_equal(@euc_str, @euc_str.kconv(::NKF::EUC))
    assert_equal(@euc_str, @sjis_str.kconv(::NKF::EUC))
    assert_equal(@euc_str, @utf8_str.kconv(::NKF::EUC))
    assert_equal(@euc_str, @jis_str.kconv(::NKF::EUC))
  end
  def test_shiftjis
    assert(@sjis_str.issjis)
    assert_equal(::Kconv::SJIS, Kconv.guess(@sjis_str))
    assert_equal(@sjis_str, @euc_str.tosjis)
    assert_equal(@sjis_str, @sjis_str.tosjis)
    assert_equal(@sjis_str, @utf8_str.tosjis)
    assert_equal(@sjis_str, @jis_str.tosjis)
    assert_equal(@sjis_str, @euc_str.kconv(::NKF::SJIS))
    assert_equal(@sjis_str, @sjis_str.kconv(::NKF::SJIS))
    assert_equal(@sjis_str, @utf8_str.kconv(::NKF::SJIS))
    assert_equal(@sjis_str, @jis_str.kconv(::NKF::SJIS))
  end
  def test_utf8
    assert(@utf8_str.isutf8)
    assert_equal(::Kconv::UTF8, Kconv.guess(@utf8_str))
    assert_equal(@utf8_str, @euc_str.toutf8)
    assert_equal(@utf8_str, @sjis_str.toutf8)
    assert_equal(@utf8_str, @utf8_str.toutf8)
    assert_equal(@utf8_str, @jis_str.toutf8)
    assert_equal(@utf8_str, @euc_str.kconv(::NKF::UTF8))
    assert_equal(@utf8_str, @sjis_str.kconv(::NKF::UTF8))
    assert_equal(@utf8_str, @utf8_str.kconv(::NKF::UTF8))
    assert_equal(@utf8_str, @jis_str.kconv(::NKF::UTF8))
  end
  def test_jis
    assert_equal(::Kconv::JIS, Kconv.guess(@jis_str))
    assert_equal(@jis_str, @euc_str.tojis)
    assert_equal(@jis_str, @sjis_str.tojis)
    assert_equal(@jis_str, @utf8_str.tojis)
    assert_equal(@jis_str, @jis_str.tojis)
    assert_equal(@jis_str, @euc_str.kconv(::NKF::JIS))
    assert_equal(@jis_str, @sjis_str.kconv(::NKF::JIS))
    assert_equal(@jis_str, @utf8_str.kconv(::NKF::JIS))
    assert_equal(@jis_str, @jis_str.kconv(::NKF::JIS))
  end
  def test_kconv
    str = "\xc2\xa1"
    %w/UTF-8 EUC-JP/.each do |enc|
      s = str.dup.force_encoding(enc)
      assert_equal(s, s.kconv(enc))
    end
  end
end
