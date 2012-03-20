require_relative "utils"

if defined?(OpenSSL)

module OpenSSL
  class TestPKCS12 < Test::Unit::TestCase
    include OpenSSL::TestUtils

    def setup
      ca = OpenSSL::X509::Name.parse("/DC=org/DC=ruby-lang/CN=CA")

      now = Time.now
      ca_exts = [
        ["basicConstraints","CA:TRUE",true],
        ["keyUsage","keyCertSign, cRLSign",true],
        ["subjectKeyIdentifier","hash",false],
        ["authorityKeyIdentifier","keyid:always",false],
      ]
      
      @cacert = issue_cert(ca, TEST_KEY_RSA2048, 1, now, now+3600, ca_exts,
                            nil, nil, OpenSSL::Digest::SHA1.new)

      inter_ca = OpenSSL::X509::Name.parse("/DC=org/DC=ruby-lang/CN=Intermediate CA")
      inter_ca_key = OpenSSL::PKey.read <<-_EOS_
-----BEGIN RSA PRIVATE KEY-----
MIICXAIBAAKBgQDp7hIG0SFMG/VWv1dBUWziAPrNmkMXJgTCAoB7jffzRtyyN04K
oq/89HAszTMStZoMigQURfokzKsjpUp8OYCAEsBtt9d5zPndWMz/gHN73GrXk3LT
ZsxEn7Xv5Da+Y9F/Hx2QZUHarV5cdZixq2NbzWGwrToogOQMh2pxN3Z/0wIDAQAB
AoGBAJysUyx3olpsGzv3OMRJeahASbmsSKTXVLZvoIefxOINosBFpCIhZccAG6UV
5c/xCvS89xBw8aD15uUfziw3AuT8QPEtHCgfSjeT7aWzBfYswEgOW4XPuWr7EeI9
iNHGD6z+hCN/IQr7FiEBgTp6A+i/hffcSdR83fHWKyb4M7TRAkEA+y4BNd668HmC
G5MPRx25n6LixuBxrNp1umfjEI6UZgEFVpYOg4agNuimN6NqM253kcTR94QNTUs5
Kj3EhG1YWwJBAO5rUjiOyCNVX2WUQrOMYK/c1lU7fvrkdygXkvIGkhsPoNRzLPeA
HGJszKtrKD8bNihWpWNIyqKRHfKVD7yXT+kCQGCAhVCIGTRoypcDghwljHqLnysf
ci0h5ZdPcIqc7ODfxYhFsJ/Rql5ONgYsT5Ig/+lOQAkjf+TRYM4c2xKx2/8CQBvG
jv6dy70qDgIUgqzONtlmHeYyFzn9cdBO5sShdVYHvRHjFSMEXsosqK9zvW2UqvuK
FJx7d3f29gkzynCLJDkCQGQZlEZJC4vWmWJGRKJ24P6MyQn3VsPfErSKOg4lvyM3
Li8JsX5yIiuVYaBg/6ha3tOg4TCa5K/3r3tVliRZ2Es=
-----END RSA PRIVATE KEY-----
      _EOS_

      @inter_cacert = issue_cert(inter_ca, inter_ca_key, 2, now, now+3600, ca_exts,
                                 @ca_cert, TEST_KEY_RSA2048, OpenSSL::Digest::SHA1.new)

      exts = [
        ["keyUsage","digitalSignature",true],
        ["subjectKeyIdentifier","hash",false],
      ]
      ee = OpenSSL::X509::Name.parse("/DC=org/DC=ruby-lang/CN=Ruby PKCS12 Test Certificate")
      @mycert = issue_cert(ee, TEST_KEY_RSA1024, 3, now, now+3600, exts,
                           @inter_cacert, inter_ca_key, OpenSSL::Digest::SHA1.new)
    end

    def test_create
      pkcs12 = OpenSSL::PKCS12.create(
        "omg",
        "hello",
        TEST_KEY_RSA1024,
        @mycert
      )
      assert_equal @mycert, pkcs12.certificate
      assert_equal TEST_KEY_RSA1024, pkcs12.key
      assert_nil pkcs12.ca_certs
    end

    def test_create_no_pass
      pkcs12 = OpenSSL::PKCS12.create(
        nil,
        "hello",
        TEST_KEY_RSA1024,
        @mycert
      )
      assert_equal @mycert, pkcs12.certificate
      assert_equal TEST_KEY_RSA1024, pkcs12.key
      assert_nil pkcs12.ca_certs

      decoded = OpenSSL::PKCS12.new(pkcs12.to_der)
      assert_cert @mycert, decoded.certificate
    end

    def test_create_with_chain
      chain = [@inter_cacert, @cacert]

      pkcs12 = OpenSSL::PKCS12.create(
        "omg",
        "hello",
        TEST_KEY_RSA1024,
        @mycert,
        chain
      )
      assert_equal chain, pkcs12.ca_certs
    end

    def test_create_with_chain_decode
      chain = [@cacert, @inter_cacert]

      passwd = "omg"

      pkcs12 = OpenSSL::PKCS12.create(
        passwd,
        "hello",
        TEST_KEY_RSA1024,
        @mycert,
        chain
      )

      decoded = OpenSSL::PKCS12.new(pkcs12.to_der, passwd)
      assert_equal chain.size, decoded.ca_certs.size
      assert_include_cert @cacert, decoded.ca_certs
      assert_include_cert @inter_cacert, decoded.ca_certs
      assert_cert @mycert, decoded.certificate 
      assert_equal TEST_KEY_RSA1024.to_der, decoded.key.to_der
    end

    def test_create_with_bad_nid
      assert_raises(ArgumentError) do
        OpenSSL::PKCS12.create(
          "omg",
          "hello",
          TEST_KEY_RSA1024,
          @mycert,
          [],
          "foo"
        )
      end
    end

    def test_create_with_itr
      OpenSSL::PKCS12.create(
        "omg",
        "hello",
        TEST_KEY_RSA1024,
        @mycert,
        [],
        nil,
        nil,
        2048
      )

      assert_raises(TypeError) do
        OpenSSL::PKCS12.create(
          "omg",
          "hello",
          TEST_KEY_RSA1024,
          @mycert,
          [],
          nil,
          nil,
          "omg"
        )
      end
    end

    def test_create_with_mac_itr
      OpenSSL::PKCS12.create(
        "omg",
        "hello",
        TEST_KEY_RSA1024,
        @mycert,
        [],
        nil,
        nil,
        nil,
        2048
      )

      assert_raises(TypeError) do
        OpenSSL::PKCS12.create(
          "omg",
          "hello",
          TEST_KEY_RSA1024,
          @mycert,
          [],
          nil,
          nil,
          nil,
          "omg"
        )
      end
    end

    private
    def assert_cert expected, actual
      [
        :subject,
        :issuer,
        :serial,
        :not_before,
        :not_after,
      ].each do |attribute|
        assert_equal expected.send(attribute), actual.send(attribute)
      end
      assert_equal expected.to_der, actual.to_der
    end

    def assert_include_cert cert, ary
      der = cert.to_der
      ary.each do |candidate|
        if candidate.to_der == der
          return true
        end
      end
      false
    end

  end
end

end
