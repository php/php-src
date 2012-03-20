require_relative 'utils'

if defined?(OpenSSL)

class OpenSSL::TestNSSPI < Test::Unit::TestCase
  def setup
    # This request data is adopt from the specification of
    # "Netscape Extensions for User Key Generation".
    # -- http://wp.netscape.com/eng/security/comm4-keygen.html
    @b64  = "MIHFMHEwXDANBgkqhkiG9w0BAQEFAANLADBIAkEAnX0TILJrOMUue+PtwBRE6XfV"
    @b64 << "WtKQbsshxk5ZhcUwcwyvcnIq9b82QhJdoACdD34rqfCAIND46fXKQUnb0mvKzQID"
    @b64 << "AQABFhFNb3ppbGxhSXNNeUZyaWVuZDANBgkqhkiG9w0BAQQFAANBAAKv2Eex2n/S"
    @b64 << "r/7iJNroWlSzSMtTiQTEB+ADWHGj9u1xrUrOilq/o2cuQxIfZcNZkYAkWP4DubqW"
    @b64 << "i0//rgBvmco="
  end

  def test_build_data
    key1 = OpenSSL::TestUtils::TEST_KEY_RSA1024
    key2 = OpenSSL::TestUtils::TEST_KEY_RSA2048
    spki = OpenSSL::Netscape::SPKI.new
    spki.challenge = "RandomString"
    spki.public_key = key1.public_key
    spki.sign(key1, OpenSSL::Digest::SHA1.new)
    assert(spki.verify(spki.public_key))
    assert(spki.verify(key1.public_key))
    assert(!spki.verify(key2.public_key))

    der = spki.to_der
    spki = OpenSSL::Netscape::SPKI.new(der)
    assert_equal("RandomString", spki.challenge)
    assert_equal(key1.public_key.to_der, spki.public_key.to_der)
    assert(spki.verify(spki.public_key))
    assert_not_nil(spki.to_text)
  end

  def test_decode_data
    spki = OpenSSL::Netscape::SPKI.new(@b64)
    assert_equal(@b64, spki.to_pem)
    assert_equal(@b64.unpack("m").first, spki.to_der)
    assert_equal("MozillaIsMyFriend", spki.challenge)
    assert_equal(OpenSSL::PKey::RSA, spki.public_key.class)

    spki = OpenSSL::Netscape::SPKI.new(@b64.unpack("m").first)
    assert_equal(@b64, spki.to_pem)
    assert_equal(@b64.unpack("m").first, spki.to_der)
    assert_equal("MozillaIsMyFriend", spki.challenge)
    assert_equal(OpenSSL::PKey::RSA, spki.public_key.class)
  end
end

end
