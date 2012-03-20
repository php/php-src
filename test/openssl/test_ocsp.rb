require_relative "utils"

if defined?(OpenSSL)

class OpenSSL::TestOCSP < Test::Unit::TestCase
  def setup
    ca_subj = OpenSSL::X509::Name.parse("/DC=org/DC=ruby-lang/CN=TestCA")
    ca_key = OpenSSL::TestUtils::TEST_KEY_RSA1024
    ca_serial = 0xabcabcabcabc

    subj = OpenSSL::X509::Name.parse("/DC=org/DC=ruby-lang/CN=TestCert")
    @key = OpenSSL::TestUtils::TEST_KEY_RSA1024
    serial = 0xabcabcabcabd

    now = Time.at(Time.now.to_i) # suppress usec
    dgst = OpenSSL::Digest::SHA1.new

    @ca_cert = OpenSSL::TestUtils.issue_cert(
       ca_subj, ca_key, ca_serial, now, now+3600, [], nil, nil, dgst)
    @cert = OpenSSL::TestUtils.issue_cert(
       subj, @key, serial, now, now+3600, [], @ca_cert, nil, dgst)
  end

  def test_new_certificate_id
    cid = OpenSSL::OCSP::CertificateId.new(@cert, @ca_cert)
    assert_kind_of OpenSSL::OCSP::CertificateId, cid
    assert_equal @cert.serial, cid.serial
  end

  def test_new_certificate_id_with_digest
    cid = OpenSSL::OCSP::CertificateId.new(@cert, @ca_cert, OpenSSL::Digest::SHA256.new)
    assert_kind_of OpenSSL::OCSP::CertificateId, cid
    assert_equal @cert.serial, cid.serial
  end if defined?(OpenSSL::Digest::SHA256)

  def test_new_ocsp_request
    request = OpenSSL::OCSP::Request.new
    cid = OpenSSL::OCSP::CertificateId.new(@cert, @ca_cert, OpenSSL::Digest::SHA1.new)
    request.add_certid(cid)
    request.sign(@cert, @key, [@cert])
    assert_kind_of OpenSSL::OCSP::Request, request
    # in current implementation not same instance of certificate id, but should contain same data
    assert_equal cid.serial, request.certid.first.serial
  end
end

end
