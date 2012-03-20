require_relative "utils"

if defined?(OpenSSL)

class OpenSSL::TestX509CRL < Test::Unit::TestCase
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

  def issue_crl(*args)
    OpenSSL::TestUtils.issue_crl(*args)
  end

  def issue_cert(*args)
    OpenSSL::TestUtils.issue_cert(*args)
  end

  def test_basic
    now = Time.at(Time.now.to_i)

    cert = issue_cert(@ca, @rsa2048, 1, now, now+3600, [],
                      nil, nil, OpenSSL::Digest::SHA1.new)
    crl = issue_crl([], 1, now, now+1600, [],
                    cert, @rsa2048, OpenSSL::Digest::SHA1.new)
    assert_equal(1, crl.version)
    assert_equal(cert.issuer.to_der, crl.issuer.to_der)
    assert_equal(now, crl.last_update)
    assert_equal(now+1600, crl.next_update)

    crl = OpenSSL::X509::CRL.new(crl.to_der)
    assert_equal(1, crl.version)
    assert_equal(cert.issuer.to_der, crl.issuer.to_der)
    assert_equal(now, crl.last_update)
    assert_equal(now+1600, crl.next_update)
  end

  def test_revoked

    # CRLReason ::= ENUMERATED {
    #      unspecified             (0),
    #      keyCompromise           (1),
    #      cACompromise            (2),
    #      affiliationChanged      (3),
    #      superseded              (4),
    #      cessationOfOperation    (5),
    #      certificateHold         (6),
    #      removeFromCRL           (8),
    #      privilegeWithdrawn      (9),
    #      aACompromise           (10) }

    now = Time.at(Time.now.to_i)
    revoke_info = [
      [1, Time.at(0),          1],
      [2, Time.at(0x7fffffff), 2],
      [3, now,                 3],
      [4, now,                 4],
      [5, now,                 5],
    ]
    cert = issue_cert(@ca, @rsa2048, 1, Time.now, Time.now+3600, [],
                      nil, nil, OpenSSL::Digest::SHA1.new)
    crl = issue_crl(revoke_info, 1, Time.now, Time.now+1600, [],
                    cert, @rsa2048, OpenSSL::Digest::SHA1.new)
    revoked = crl.revoked
    assert_equal(5, revoked.size)
    assert_equal(1, revoked[0].serial)
    assert_equal(2, revoked[1].serial)
    assert_equal(3, revoked[2].serial)
    assert_equal(4, revoked[3].serial)
    assert_equal(5, revoked[4].serial)

    assert_equal(Time.at(0), revoked[0].time)
    assert_equal(Time.at(0x7fffffff), revoked[1].time)
    assert_equal(now, revoked[2].time)
    assert_equal(now, revoked[3].time)
    assert_equal(now, revoked[4].time)

    assert_equal("CRLReason", revoked[0].extensions[0].oid)
    assert_equal("CRLReason", revoked[1].extensions[0].oid)
    assert_equal("CRLReason", revoked[2].extensions[0].oid)
    assert_equal("CRLReason", revoked[3].extensions[0].oid)
    assert_equal("CRLReason", revoked[4].extensions[0].oid)

    assert_equal("Key Compromise", revoked[0].extensions[0].value)
    assert_equal("CA Compromise", revoked[1].extensions[0].value)
    assert_equal("Affiliation Changed", revoked[2].extensions[0].value)
    assert_equal("Superseded", revoked[3].extensions[0].value)
    assert_equal("Cessation Of Operation", revoked[4].extensions[0].value)

    assert_equal(false, revoked[0].extensions[0].critical?)
    assert_equal(false, revoked[1].extensions[0].critical?)
    assert_equal(false, revoked[2].extensions[0].critical?)
    assert_equal(false, revoked[3].extensions[0].critical?)
    assert_equal(false, revoked[4].extensions[0].critical?)

    crl = OpenSSL::X509::CRL.new(crl.to_der)
    assert_equal("Key Compromise", revoked[0].extensions[0].value)
    assert_equal("CA Compromise", revoked[1].extensions[0].value)
    assert_equal("Affiliation Changed", revoked[2].extensions[0].value)
    assert_equal("Superseded", revoked[3].extensions[0].value)
    assert_equal("Cessation Of Operation", revoked[4].extensions[0].value)

    revoke_info = (1..1000).collect{|i| [i, now, 0] }
    crl = issue_crl(revoke_info, 1, Time.now, Time.now+1600, [],
                    cert, @rsa2048, OpenSSL::Digest::SHA1.new)
    revoked = crl.revoked
    assert_equal(1000, revoked.size)
    assert_equal(1, revoked[0].serial)
    assert_equal(1000, revoked[999].serial)
  end

  def test_extension
    cert_exts = [
      ["basicConstraints", "CA:TRUE", true],
      ["subjectKeyIdentifier", "hash", false],
      ["authorityKeyIdentifier", "keyid:always", false],
      ["subjectAltName", "email:xyzzy@ruby-lang.org", false],
      ["keyUsage", "cRLSign, keyCertSign", true],
    ]
    crl_exts = [
      ["authorityKeyIdentifier", "keyid:always", false],
      ["issuerAltName", "issuer:copy", false],
    ]

    cert = issue_cert(@ca, @rsa2048, 1, Time.now, Time.now+3600, cert_exts,
                      nil, nil, OpenSSL::Digest::SHA1.new)
    crl = issue_crl([], 1, Time.now, Time.now+1600, crl_exts,
                    cert, @rsa2048, OpenSSL::Digest::SHA1.new)
    exts = crl.extensions
    assert_equal(3, exts.size)
    assert_equal("1", exts[0].value)
    assert_equal("crlNumber", exts[0].oid)
    assert_equal(false, exts[0].critical?)

    assert_equal("authorityKeyIdentifier", exts[1].oid)
    keyid = OpenSSL::TestUtils.get_subject_key_id(cert)
    assert_match(/^keyid:#{keyid}/, exts[1].value)
    assert_equal(false, exts[1].critical?)

    assert_equal("issuerAltName", exts[2].oid)
    assert_equal("email:xyzzy@ruby-lang.org", exts[2].value)
    assert_equal(false, exts[2].critical?)

    crl = OpenSSL::X509::CRL.new(crl.to_der)
    exts = crl.extensions
    assert_equal(3, exts.size)
    assert_equal("1", exts[0].value)
    assert_equal("crlNumber", exts[0].oid)
    assert_equal(false, exts[0].critical?)

    assert_equal("authorityKeyIdentifier", exts[1].oid)
    keyid = OpenSSL::TestUtils.get_subject_key_id(cert)
    assert_match(/^keyid:#{keyid}/, exts[1].value)
    assert_equal(false, exts[1].critical?)

    assert_equal("issuerAltName", exts[2].oid)
    assert_equal("email:xyzzy@ruby-lang.org", exts[2].value)
    assert_equal(false, exts[2].critical?)
  end

  def test_crlnumber
    cert = issue_cert(@ca, @rsa2048, 1, Time.now, Time.now+3600, [],
                      nil, nil, OpenSSL::Digest::SHA1.new)
    crl = issue_crl([], 1, Time.now, Time.now+1600, [],
                    cert, @rsa2048, OpenSSL::Digest::SHA1.new)
    assert_match(1.to_s, crl.extensions[0].value)
    assert_match(/X509v3 CRL Number:\s+#{1}/m, crl.to_text)

    crl = issue_crl([], 2**32, Time.now, Time.now+1600, [],
                    cert, @rsa2048, OpenSSL::Digest::SHA1.new)
    assert_match((2**32).to_s, crl.extensions[0].value)
    assert_match(/X509v3 CRL Number:\s+#{2**32}/m, crl.to_text)

    crl = issue_crl([], 2**100, Time.now, Time.now+1600, [],
                    cert, @rsa2048, OpenSSL::Digest::SHA1.new)
    assert_match(/X509v3 CRL Number:\s+#{2**100}/m, crl.to_text)
    assert_match((2**100).to_s, crl.extensions[0].value)
  end

  def test_sign_and_verify
    cert = issue_cert(@ca, @rsa2048, 1, Time.now, Time.now+3600, [],
                      nil, nil, OpenSSL::Digest::SHA1.new)
    crl = issue_crl([], 1, Time.now, Time.now+1600, [],
                    cert, @rsa2048, OpenSSL::Digest::SHA1.new)
    assert_equal(false, crl.verify(@rsa1024))
    assert_equal(true,  crl.verify(@rsa2048))
    assert_equal(false, crl_error_returns_false { crl.verify(@dsa256) })
    assert_equal(false, crl_error_returns_false { crl.verify(@dsa512) })
    crl.version = 0
    assert_equal(false, crl.verify(@rsa2048))

    cert = issue_cert(@ca, @dsa512, 1, Time.now, Time.now+3600, [],
                      nil, nil, OpenSSL::Digest::DSS1.new)
    crl = issue_crl([], 1, Time.now, Time.now+1600, [],
                    cert, @dsa512, OpenSSL::Digest::DSS1.new)
    assert_equal(false, crl_error_returns_false { crl.verify(@rsa1024) })
    assert_equal(false, crl_error_returns_false { crl.verify(@rsa2048) })
    assert_equal(false, crl.verify(@dsa256))
    assert_equal(true,  crl.verify(@dsa512))
    crl.version = 0
    assert_equal(false, crl.verify(@dsa512))
  end

  private

  def crl_error_returns_false
    yield
  rescue OpenSSL::X509::CRLError
    false
  end
end

end
