require_relative "utils"

if defined?(OpenSSL)

class OpenSSL::TestX509Store < Test::Unit::TestCase
  def setup
    @rsa1024 = OpenSSL::TestUtils::TEST_KEY_RSA1024
    @rsa2048 = OpenSSL::TestUtils::TEST_KEY_RSA2048
    @dsa256  = OpenSSL::TestUtils::TEST_KEY_DSA256
    @dsa512  = OpenSSL::TestUtils::TEST_KEY_DSA512
    @ca1 = OpenSSL::X509::Name.parse("/DC=org/DC=ruby-lang/CN=CA1")
    @ca2 = OpenSSL::X509::Name.parse("/DC=org/DC=ruby-lang/CN=CA2")
    @ee1 = OpenSSL::X509::Name.parse("/DC=org/DC=ruby-lang/CN=EE1")
    @ee2 = OpenSSL::X509::Name.parse("/DC=org/DC=ruby-lang/CN=EE2")
  end

  def teardown
  end

  def test_nosegv_on_cleanup
    cert  = OpenSSL::X509::Certificate.new
    store = OpenSSL::X509::Store.new
    ctx   = OpenSSL::X509::StoreContext.new(store, cert, [])
    ctx.cleanup
    ctx.verify
  end

  def issue_cert(*args)
    OpenSSL::TestUtils.issue_cert(*args)
  end

  def issue_crl(*args)
    OpenSSL::TestUtils.issue_crl(*args)
  end

  def test_verify
    now = Time.at(Time.now.to_i)
    ca_exts = [
      ["basicConstraints","CA:TRUE",true],
      ["keyUsage","cRLSign,keyCertSign",true],
    ]
    ee_exts = [
      ["keyUsage","keyEncipherment,digitalSignature",true],
    ]
    ca1_cert = issue_cert(@ca1, @rsa2048, 1, now, now+3600, ca_exts,
                          nil, nil, OpenSSL::Digest::SHA1.new)
    ca2_cert = issue_cert(@ca2, @rsa1024, 2, now, now+1800, ca_exts,
                          ca1_cert, @rsa2048, OpenSSL::Digest::SHA1.new)
    ee1_cert = issue_cert(@ee1, @dsa256, 10, now, now+1800, ee_exts,
                          ca2_cert, @rsa1024, OpenSSL::Digest::SHA1.new)
    ee2_cert = issue_cert(@ee2, @dsa512, 20, now, now+1800, ee_exts,
                          ca2_cert, @rsa1024, OpenSSL::Digest::SHA1.new)
    ee3_cert = issue_cert(@ee2, @dsa512, 30, now-100, now-1, ee_exts,
                          ca2_cert, @rsa1024, OpenSSL::Digest::SHA1.new)
    ee4_cert = issue_cert(@ee2, @dsa512, 40, now+1000, now+2000, ee_exts,
                          ca2_cert, @rsa1024, OpenSSL::Digest::SHA1.new)

    revoke_info = []
    crl1   = issue_crl(revoke_info, 1, now, now+1800, [],
                       ca1_cert, @rsa2048, OpenSSL::Digest::SHA1.new)
    revoke_info = [ [2, now, 1], ]
    crl1_2 = issue_crl(revoke_info, 2, now, now+1800, [],
                       ca1_cert, @rsa2048, OpenSSL::Digest::SHA1.new)
    revoke_info = [ [20, now, 1], ]
    crl2   = issue_crl(revoke_info, 1, now, now+1800, [],
                       ca2_cert, @rsa1024, OpenSSL::Digest::SHA1.new)
    revoke_info = []
    crl2_2 = issue_crl(revoke_info, 2, now-100, now-1, [],
                       ca2_cert, @rsa1024, OpenSSL::Digest::SHA1.new)

    assert_equal(true, ca1_cert.verify(ca1_cert.public_key))   # self signed
    assert_equal(true, ca2_cert.verify(ca1_cert.public_key))   # issued by ca1
    assert_equal(true, ee1_cert.verify(ca2_cert.public_key))   # issued by ca2
    assert_equal(true, ee2_cert.verify(ca2_cert.public_key))   # issued by ca2
    assert_equal(true, ee3_cert.verify(ca2_cert.public_key))   # issued by ca2
    assert_equal(true, crl1.verify(ca1_cert.public_key))       # issued by ca1
    assert_equal(true, crl1_2.verify(ca1_cert.public_key))     # issued by ca1
    assert_equal(true, crl2.verify(ca2_cert.public_key))       # issued by ca2
    assert_equal(true, crl2_2.verify(ca2_cert.public_key))     # issued by ca2

    store = OpenSSL::X509::Store.new
    assert_equal(false, store.verify(ca1_cert))
    assert_not_equal(OpenSSL::X509::V_OK, store.error)

    assert_equal(false, store.verify(ca2_cert))
    assert_not_equal(OpenSSL::X509::V_OK, store.error)

    store.add_cert(ca1_cert)
    assert_equal(true, store.verify(ca2_cert))
    assert_equal(OpenSSL::X509::V_OK, store.error)
    assert_equal("ok", store.error_string)
    chain = store.chain
    assert_equal(2, chain.size)
    assert_equal(@ca2.to_der, chain[0].subject.to_der)
    assert_equal(@ca1.to_der, chain[1].subject.to_der)

    store.purpose = OpenSSL::X509::PURPOSE_SSL_CLIENT
    assert_equal(false, store.verify(ca2_cert))
    assert_not_equal(OpenSSL::X509::V_OK, store.error)

    store.purpose = OpenSSL::X509::PURPOSE_CRL_SIGN
    assert_equal(true, store.verify(ca2_cert))
    assert_equal(OpenSSL::X509::V_OK, store.error)

    store.add_cert(ca2_cert)
    store.purpose = OpenSSL::X509::PURPOSE_SSL_CLIENT
    assert_equal(true, store.verify(ee1_cert))
    assert_equal(true, store.verify(ee2_cert))
    assert_equal(OpenSSL::X509::V_OK, store.error)
    assert_equal("ok", store.error_string)
    chain = store.chain
    assert_equal(3, chain.size)
    assert_equal(@ee2.to_der, chain[0].subject.to_der)
    assert_equal(@ca2.to_der, chain[1].subject.to_der)
    assert_equal(@ca1.to_der, chain[2].subject.to_der)
    assert_equal(false, store.verify(ee3_cert))
    assert_equal(OpenSSL::X509::V_ERR_CERT_HAS_EXPIRED, store.error)
    assert_match(/expire/i, store.error_string)
    assert_equal(false, store.verify(ee4_cert))
    assert_equal(OpenSSL::X509::V_ERR_CERT_NOT_YET_VALID, store.error)
    assert_match(/not yet valid/i, store.error_string)

    store = OpenSSL::X509::Store.new
    store.add_cert(ca1_cert)
    store.add_cert(ca2_cert)
    store.time = now + 1500
    assert_equal(true, store.verify(ca1_cert))
    assert_equal(true, store.verify(ca2_cert))
    assert_equal(true, store.verify(ee4_cert))
    store.time = now + 1900
    assert_equal(true, store.verify(ca1_cert))
    assert_equal(false, store.verify(ca2_cert))
    assert_equal(OpenSSL::X509::V_ERR_CERT_HAS_EXPIRED, store.error)
    assert_equal(false, store.verify(ee4_cert))
    assert_equal(OpenSSL::X509::V_ERR_CERT_HAS_EXPIRED, store.error)
    store.time = now + 4000
    assert_equal(false, store.verify(ee1_cert))
    assert_equal(OpenSSL::X509::V_ERR_CERT_HAS_EXPIRED, store.error)
    assert_equal(false, store.verify(ee4_cert))
    assert_equal(OpenSSL::X509::V_ERR_CERT_HAS_EXPIRED, store.error)

    # the underlying X509 struct caches the result of the last
    # verification for signature and not-before. so the following code
    # rebuilds new objects to avoid site effect.
    store.time = Time.now - 4000
    assert_equal(false, store.verify(OpenSSL::X509::Certificate.new(ca2_cert)))
    assert_equal(OpenSSL::X509::V_ERR_CERT_NOT_YET_VALID, store.error)
    assert_equal(false, store.verify(OpenSSL::X509::Certificate.new(ee1_cert)))
    assert_equal(OpenSSL::X509::V_ERR_CERT_NOT_YET_VALID, store.error)

    return unless defined?(OpenSSL::X509::V_FLAG_CRL_CHECK)

    store = OpenSSL::X509::Store.new
    store.purpose = OpenSSL::X509::PURPOSE_ANY
    store.flags = OpenSSL::X509::V_FLAG_CRL_CHECK
    store.add_cert(ca1_cert)
    store.add_crl(crl1)   # revoke no cert
    store.add_crl(crl2)   # revoke ee2_cert
    assert_equal(true,  store.verify(ca1_cert))
    assert_equal(true,  store.verify(ca2_cert))
    assert_equal(true,  store.verify(ee1_cert, [ca2_cert]))
    assert_equal(false, store.verify(ee2_cert, [ca2_cert]))

    store = OpenSSL::X509::Store.new
    store.purpose = OpenSSL::X509::PURPOSE_ANY
    store.flags = OpenSSL::X509::V_FLAG_CRL_CHECK
    store.add_cert(ca1_cert)
    store.add_crl(crl1_2) # revoke ca2_cert
    store.add_crl(crl2)   # revoke ee2_cert
    assert_equal(true,  store.verify(ca1_cert))
    assert_equal(false, store.verify(ca2_cert))
    assert_equal(true,  store.verify(ee1_cert, [ca2_cert]),
      "This test is expected to be success with OpenSSL 0.9.7c or later.")
    assert_equal(false, store.verify(ee2_cert, [ca2_cert]))

    store.flags =
      OpenSSL::X509::V_FLAG_CRL_CHECK|OpenSSL::X509::V_FLAG_CRL_CHECK_ALL
    assert_equal(true,  store.verify(ca1_cert))
    assert_equal(false, store.verify(ca2_cert))
    assert_equal(false, store.verify(ee1_cert, [ca2_cert]))
    assert_equal(false, store.verify(ee2_cert, [ca2_cert]))

    store = OpenSSL::X509::Store.new
    store.purpose = OpenSSL::X509::PURPOSE_ANY
    store.flags =
      OpenSSL::X509::V_FLAG_CRL_CHECK|OpenSSL::X509::V_FLAG_CRL_CHECK_ALL
    store.add_cert(ca1_cert)
    store.add_cert(ca2_cert)
    store.add_crl(crl1)
    store.add_crl(crl2_2) # issued by ca2 but expired.
    assert_equal(true, store.verify(ca1_cert))
    assert_equal(true, store.verify(ca2_cert))
    assert_equal(false, store.verify(ee1_cert))
    assert_equal(OpenSSL::X509::V_ERR_CRL_HAS_EXPIRED, store.error)
    assert_equal(false, store.verify(ee2_cert))
  end

  def test_set_errors
    now = Time.now
    ca1_cert = issue_cert(@ca1, @rsa2048, 1, now, now+3600, [],
                          nil, nil, OpenSSL::Digest::SHA1.new)
    store = OpenSSL::X509::Store.new
    store.add_cert(ca1_cert)
    assert_raise(OpenSSL::X509::StoreError){
      store.add_cert(ca1_cert)  # add same certificate twice
    }

    revoke_info = []
    crl1 = issue_crl(revoke_info, 1, now, now+1800, [],
                     ca1_cert, @rsa2048, OpenSSL::Digest::SHA1.new)
    revoke_info = [ [2, now, 1], ]
    crl2 = issue_crl(revoke_info, 2, now+1800, now+3600, [],
                     ca1_cert, @rsa2048, OpenSSL::Digest::SHA1.new)
    store.add_crl(crl1)
    if /0\.9\.8.*-rhel/ =~ OpenSSL::OPENSSL_VERSION
      # RedHat is distributing a patched version of OpenSSL that allows
      # multiple CRL for a key (multi-crl.patch)
      assert_nothing_raised do
        store.add_crl(crl2) # add CRL issued by same CA twice.
      end
    else
      assert_raise(OpenSSL::X509::StoreError){
        store.add_crl(crl2) # add CRL issued by same CA twice.
      }
    end
  end
end

end
