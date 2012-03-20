require_relative "utils"

if defined?(OpenSSL)

class OpenSSL::TestX509Certificate < Test::Unit::TestCase
  def setup
    @rsa1024 = OpenSSL::TestUtils::TEST_KEY_RSA1024
    @rsa2048 = OpenSSL::TestUtils::TEST_KEY_RSA2048
    @dsa256  = OpenSSL::TestUtils::TEST_KEY_DSA256
    @dsa512  = OpenSSL::TestUtils::TEST_KEY_DSA512
    @ca = OpenSSL::X509::Name.parse("/DC=org/DC=ruby-lang/CN=CA")
    @ee1 = OpenSSL::X509::Name.parse("/DC=org/DC=ruby-lang/CN=EE1")
    @ee2 = OpenSSL::X509::Name.parse("/DC=org/DC=ruby-lang/CN=EE2")
  end

  def teardown
  end

  def issue_cert(*args)
    OpenSSL::TestUtils.issue_cert(*args)
  end

  def test_serial
    [1, 2**32, 2**100].each{|s|
      cert = issue_cert(@ca, @rsa2048, s, Time.now, Time.now+3600, [],
                        nil, nil, OpenSSL::Digest::SHA1.new)
      assert_equal(s, cert.serial)
      cert = OpenSSL::X509::Certificate.new(cert.to_der)
      assert_equal(s, cert.serial)
    }
  end

  def test_public_key
    exts = [
      ["basicConstraints","CA:TRUE",true],
      ["subjectKeyIdentifier","hash",false],
      ["authorityKeyIdentifier","keyid:always",false],
    ]

    sha1 = OpenSSL::Digest::SHA1.new
    dss1 = OpenSSL::Digest::DSS1.new
    [
      [@rsa1024, sha1], [@rsa2048, sha1], [@dsa256, dss1], [@dsa512, dss1],
    ].each{|pk, digest|
      cert = issue_cert(@ca, pk, 1, Time.now, Time.now+3600, exts,
                        nil, nil, digest)
      assert_equal(cert.extensions[1].value,
                   OpenSSL::TestUtils.get_subject_key_id(cert))
      cert = OpenSSL::X509::Certificate.new(cert.to_der)
      assert_equal(cert.extensions[1].value,
                   OpenSSL::TestUtils.get_subject_key_id(cert))
    }
  end

  def test_validity
    now = Time.now until now && now.usec != 0
    cert = issue_cert(@ca, @rsa2048, 1, now, now+3600, [],
                      nil, nil, OpenSSL::Digest::SHA1.new)
    assert_not_equal(now, cert.not_before)
    assert_not_equal(now+3600, cert.not_after)

    now = Time.at(now.to_i)
    cert = issue_cert(@ca, @rsa2048, 1, now, now+3600, [],
                      nil, nil, OpenSSL::Digest::SHA1.new)
    assert_equal(now.getutc, cert.not_before)
    assert_equal((now+3600).getutc, cert.not_after)

    now = Time.at(0)
    cert = issue_cert(@ca, @rsa2048, 1, now, now, [],
                      nil, nil, OpenSSL::Digest::SHA1.new)
    assert_equal(now.getutc, cert.not_before)
    assert_equal(now.getutc, cert.not_after)

    now = Time.at(0x7fffffff)
    cert = issue_cert(@ca, @rsa2048, 1, now, now, [],
                      nil, nil, OpenSSL::Digest::SHA1.new)
    assert_equal(now.getutc, cert.not_before)
    assert_equal(now.getutc, cert.not_after)
  end

  def test_extension
    ca_exts = [
      ["basicConstraints","CA:TRUE",true],
      ["keyUsage","keyCertSign, cRLSign",true],
      ["subjectKeyIdentifier","hash",false],
      ["authorityKeyIdentifier","keyid:always",false],
    ]
    ca_cert = issue_cert(@ca, @rsa2048, 1, Time.now, Time.now+3600, ca_exts,
                         nil, nil, OpenSSL::Digest::SHA1.new)
    ca_cert.extensions.each_with_index{|ext, i|
      assert_equal(ca_exts[i].first, ext.oid)
      assert_equal(ca_exts[i].last, ext.critical?)
    }

    ee1_exts = [
      ["keyUsage","Non Repudiation, Digital Signature, Key Encipherment",true],
      ["subjectKeyIdentifier","hash",false],
      ["authorityKeyIdentifier","keyid:always",false],
      ["extendedKeyUsage","clientAuth, emailProtection, codeSigning",false],
      ["subjectAltName","email:ee1@ruby-lang.org",false],
    ]
    ee1_cert = issue_cert(@ee1, @rsa1024, 2, Time.now, Time.now+1800, ee1_exts,
                          ca_cert, @rsa2048, OpenSSL::Digest::SHA1.new)
    assert_equal(ca_cert.subject.to_der, ee1_cert.issuer.to_der)
    ee1_cert.extensions.each_with_index{|ext, i|
      assert_equal(ee1_exts[i].first, ext.oid)
      assert_equal(ee1_exts[i].last, ext.critical?)
    }

    ee2_exts = [
      ["keyUsage","Non Repudiation, Digital Signature, Key Encipherment",true],
      ["subjectKeyIdentifier","hash",false],
      ["authorityKeyIdentifier","issuer:always",false],
      ["extendedKeyUsage","clientAuth, emailProtection, codeSigning",false],
      ["subjectAltName","email:ee2@ruby-lang.org",false],
    ]
    ee2_cert = issue_cert(@ee2, @rsa1024, 3, Time.now, Time.now+1800, ee2_exts,
                          ca_cert, @rsa2048, OpenSSL::Digest::MD5.new)
    assert_equal(ca_cert.subject.to_der, ee2_cert.issuer.to_der)
    ee2_cert.extensions.each_with_index{|ext, i|
      assert_equal(ee2_exts[i].first, ext.oid)
      assert_equal(ee2_exts[i].last, ext.critical?)
    }

  end

  def test_sign_and_verify
    cert = issue_cert(@ca, @rsa2048, 1, Time.now, Time.now+3600, [],
                      nil, nil, OpenSSL::Digest::SHA1.new)
    assert_equal(false, cert.verify(@rsa1024))
    assert_equal(true,  cert.verify(@rsa2048))
    assert_equal(false, certificate_error_returns_false { cert.verify(@dsa256) })
    assert_equal(false, certificate_error_returns_false { cert.verify(@dsa512) })
    cert.serial = 2
    assert_equal(false, cert.verify(@rsa2048))

    cert = issue_cert(@ca, @rsa2048, 1, Time.now, Time.now+3600, [],
                      nil, nil, OpenSSL::Digest::MD5.new)
    assert_equal(false, cert.verify(@rsa1024))
    assert_equal(true, cert.verify(@rsa2048))

    assert_equal(false, certificate_error_returns_false { cert.verify(@dsa256) })
    assert_equal(false, certificate_error_returns_false { cert.verify(@dsa512) })
    cert.subject = @ee1
    assert_equal(false, cert.verify(@rsa2048))

    cert = issue_cert(@ca, @dsa512, 1, Time.now, Time.now+3600, [],
                      nil, nil, OpenSSL::Digest::DSS1.new)
    assert_equal(false, certificate_error_returns_false { cert.verify(@rsa1024) })
    assert_equal(false, certificate_error_returns_false { cert.verify(@rsa2048) })
    assert_equal(false, cert.verify(@dsa256))
    assert_equal(true,  cert.verify(@dsa512))
    cert.not_after = Time.now
    assert_equal(false, cert.verify(@dsa512))

    begin
      cert = issue_cert(@ca, @rsa2048, 1, Time.now, Time.now+3600, [],
                        nil, nil, OpenSSL::Digest::DSS1.new)
      assert_equal(false, cert.verify(@rsa1024))
      assert_equal(true, cert.verify(@rsa2048))
      assert_equal(false, certificate_error_returns_false { cert.verify(@dsa256) })
      assert_equal(false, certificate_error_returns_false { cert.verify(@dsa512) })
      cert.subject = @ee1
      assert_equal(false, cert.verify(@rsa2048))
    rescue OpenSSL::X509::CertificateError
    end

    assert_raise(OpenSSL::X509::CertificateError){
      cert = issue_cert(@ca, @dsa512, 1, Time.now, Time.now+3600, [],
                        nil, nil, OpenSSL::Digest::MD5.new)
    }
  end

  def test_dsig_algorithm_mismatch
    assert_raise(OpenSSL::X509::CertificateError) do
      cert = issue_cert(@ca, @rsa2048, 1, Time.now, Time.now+3600, [],
                        nil, nil, OpenSSL::Digest::DSS1.new)
    end
    assert_raise(OpenSSL::X509::CertificateError) do
      cert = issue_cert(@ca, @dsa512, 1, Time.now, Time.now+3600, [],
                        nil, nil, OpenSSL::Digest::MD5.new)
    end
  end

  def test_dsa_with_sha2
    begin
      cert = issue_cert(@ca, @dsa256, 1, Time.now, Time.now+3600, [],
                        nil, nil, OpenSSL::Digest::SHA256.new)
      assert_equal("dsa_with_SHA256", cert.signature_algorithm)
    rescue OpenSSL::X509::CertificateError
      # dsa_with_sha2 not supported. skip following test.
      return
    end
    # TODO: need more tests for dsa + sha2

    # SHA1 is allowed from OpenSSL 1.0.0 (0.9.8 requires DSS1)
    cert = issue_cert(@ca, @dsa256, 1, Time.now, Time.now+3600, [],
                      nil, nil, OpenSSL::Digest::SHA1.new)
    assert_equal("dsaWithSHA1", cert.signature_algorithm)
  end if defined?(OpenSSL::Digest::SHA256)

  def test_check_private_key
    cert = issue_cert(@ca, @rsa2048, 1, Time.now, Time.now+3600, [],
                      nil, nil, OpenSSL::Digest::SHA1.new)
    assert_equal(true, cert.check_private_key(@rsa2048))
  end

  private

  def certificate_error_returns_false
    yield
  rescue OpenSSL::X509::CertificateError
    false
  end
end

end
