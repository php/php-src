require "test/unit"
require "base64"

class TestBase64 < Test::Unit::TestCase
  def test_sample
    assert_equal("U2VuZCByZWluZm9yY2VtZW50cw==\n", Base64.encode64('Send reinforcements'))
    assert_equal('Send reinforcements', Base64.decode64("U2VuZCByZWluZm9yY2VtZW50cw==\n"))
    assert_equal(
      "Tm93IGlzIHRoZSB0aW1lIGZvciBhbGwgZ29vZCBjb2RlcnMKdG8gbGVhcm4g\nUnVieQ==\n",
      Base64.encode64("Now is the time for all good coders\nto learn Ruby"))
    assert_equal(
      "Now is the time for all good coders\nto learn Ruby",
      Base64.decode64("Tm93IGlzIHRoZSB0aW1lIGZvciBhbGwgZ29vZCBjb2RlcnMKdG8gbGVhcm4g\nUnVieQ==\n"))
    assert_equal(
      "VGhpcyBpcyBsaW5lIG9uZQpUaGlzIGlzIGxpbmUgdHdvClRoaXMgaXMgbGlu\nZSB0aHJlZQpBbmQgc28gb24uLi4K\n",
      Base64.encode64("This is line one\nThis is line two\nThis is line three\nAnd so on...\n"))
    assert_equal(
      "This is line one\nThis is line two\nThis is line three\nAnd so on...\n",
      Base64.decode64("VGhpcyBpcyBsaW5lIG9uZQpUaGlzIGlzIGxpbmUgdHdvClRoaXMgaXMgbGluZSB0aHJlZQpBbmQgc28gb24uLi4K"))
  end

  def test_encode64
    assert_equal("", Base64.encode64(""))
    assert_equal("AA==\n", Base64.encode64("\0"))
    assert_equal("AAA=\n", Base64.encode64("\0\0"))
    assert_equal("AAAA\n", Base64.encode64("\0\0\0"))
    assert_equal("/w==\n", Base64.encode64("\377"))
    assert_equal("//8=\n", Base64.encode64("\377\377"))
    assert_equal("////\n", Base64.encode64("\377\377\377"))
    assert_equal("/+8=\n", Base64.encode64("\xff\xef"))
  end

  def test_decode64
    assert_equal("", Base64.decode64(""))
    assert_equal("\0", Base64.decode64("AA==\n"))
    assert_equal("\0\0", Base64.decode64("AAA=\n"))
    assert_equal("\0\0\0", Base64.decode64("AAAA\n"))
    assert_equal("\377", Base64.decode64("/w==\n"))
    assert_equal("\377\377", Base64.decode64("//8=\n"))
    assert_equal("\377\377\377", Base64.decode64("////\n"))
    assert_equal("\xff\xef", Base64.decode64("/+8=\n"))
  end

  def test_strict_encode64
    assert_equal("", Base64.strict_encode64(""))
    assert_equal("AA==", Base64.strict_encode64("\0"))
    assert_equal("AAA=", Base64.strict_encode64("\0\0"))
    assert_equal("AAAA", Base64.strict_encode64("\0\0\0"))
    assert_equal("/w==", Base64.strict_encode64("\377"))
    assert_equal("//8=", Base64.strict_encode64("\377\377"))
    assert_equal("////", Base64.strict_encode64("\377\377\377"))
    assert_equal("/+8=", Base64.strict_encode64("\xff\xef"))
  end

  def test_strict_decode64
    assert_equal("", Base64.strict_decode64(""))
    assert_equal("\0", Base64.strict_decode64("AA=="))
    assert_equal("\0\0", Base64.strict_decode64("AAA="))
    assert_equal("\0\0\0", Base64.strict_decode64("AAAA"))
    assert_equal("\377", Base64.strict_decode64("/w=="))
    assert_equal("\377\377", Base64.strict_decode64("//8="))
    assert_equal("\377\377\377", Base64.strict_decode64("////"))
    assert_equal("\xff\xef", Base64.strict_decode64("/+8="))

    assert_raise(ArgumentError) { Base64.strict_decode64("^") }
    assert_raise(ArgumentError) { Base64.strict_decode64("A") }
    assert_raise(ArgumentError) { Base64.strict_decode64("A^") }
    assert_raise(ArgumentError) { Base64.strict_decode64("AA") }
    assert_raise(ArgumentError) { Base64.strict_decode64("AA=") }
    assert_raise(ArgumentError) { Base64.strict_decode64("AA===") }
    assert_raise(ArgumentError) { Base64.strict_decode64("AA=x") }
    assert_raise(ArgumentError) { Base64.strict_decode64("AAA") }
    assert_raise(ArgumentError) { Base64.strict_decode64("AAA^") }
    assert_raise(ArgumentError) { Base64.strict_decode64("AB==") }
    assert_raise(ArgumentError) { Base64.strict_decode64("AAB=") }
  end

  def test_urlsafe_encode64
    assert_equal("", Base64.urlsafe_encode64(""))
    assert_equal("AA==", Base64.urlsafe_encode64("\0"))
    assert_equal("AAA=", Base64.urlsafe_encode64("\0\0"))
    assert_equal("AAAA", Base64.urlsafe_encode64("\0\0\0"))
    assert_equal("_w==", Base64.urlsafe_encode64("\377"))
    assert_equal("__8=", Base64.urlsafe_encode64("\377\377"))
    assert_equal("____", Base64.urlsafe_encode64("\377\377\377"))
    assert_equal("_-8=", Base64.urlsafe_encode64("\xff\xef"))
  end

  def test_urlsafe_decode64
    assert_equal("", Base64.urlsafe_decode64(""))
    assert_equal("\0", Base64.urlsafe_decode64("AA=="))
    assert_equal("\0\0", Base64.urlsafe_decode64("AAA="))
    assert_equal("\0\0\0", Base64.urlsafe_decode64("AAAA"))
    assert_equal("\377", Base64.urlsafe_decode64("_w=="))
    assert_equal("\377\377", Base64.urlsafe_decode64("__8="))
    assert_equal("\377\377\377", Base64.urlsafe_decode64("____"))
    assert_equal("\xff\xef", Base64.urlsafe_decode64("_+8="))
  end
end
