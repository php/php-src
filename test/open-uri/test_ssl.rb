require 'test/unit'
require 'open-uri'
require 'openssl'
require 'stringio'
require 'webrick'
require 'webrick/https'
require 'webrick/httpproxy'

class TestOpenURISSL < Test::Unit::TestCase

  NullLog = Object.new
  def NullLog.<<(arg)
  end

  def with_https
    Dir.mktmpdir {|dr|
      srv = WEBrick::HTTPServer.new({
        :DocumentRoot => dr,
        :ServerType => Thread,
        :Logger => WEBrick::Log.new(NullLog),
        :AccessLog => [[NullLog, ""]],
        :SSLEnable => true,
        :SSLCertificate => OpenSSL::X509::Certificate.new(SERVER_CERT),
        :SSLPrivateKey => OpenSSL::PKey::RSA.new(SERVER_KEY),
        :BindAddress => '127.0.0.1',
        :Port => 0})
      _, port, _, host = srv.listeners[0].addr
      begin
        th = srv.start
        yield srv, dr, "https://#{host}:#{port}"
      ensure
        srv.shutdown
      end
    }
  end

  def setup
    @proxies = %w[http_proxy HTTP_PROXY https_proxy HTTPS_PROXY ftp_proxy FTP_PROXY no_proxy]
    @old_proxies = @proxies.map {|k| ENV[k] }
    @proxies.each {|k| ENV[k] = nil }
  end

  def teardown
    @proxies.each_with_index {|k, i| ENV[k] = @old_proxies[i] }
  end

  def test_validation
    with_https {|srv, dr, url|
      cacert_filename = "#{dr}/cacert.pem"
      open(cacert_filename, "w") {|f| f << CA_CERT }
      open("#{dr}/data", "w") {|f| f << "ddd" }
      open("#{url}/data", :ssl_ca_cert => cacert_filename) {|f|
        assert_equal("200", f.status[0])
        assert_equal("ddd", f.read)
      }
      open("#{url}/data", :ssl_verify_mode => OpenSSL::SSL::VERIFY_NONE) {|f|
        assert_equal("200", f.status[0])
        assert_equal("ddd", f.read)
      }
      assert_raise(OpenSSL::SSL::SSLError) { open("#{url}/data") {} }
    }
  end

  def test_proxy
    with_https {|srv, dr, url|
      cacert_filename = "#{dr}/cacert.pem"
      open(cacert_filename, "w") {|f| f << CA_CERT }
      cacert_directory = "#{dr}/certs"
      Dir.mkdir cacert_directory
      hashed_name = "%08x.0" % OpenSSL::X509::Certificate.new(CA_CERT).subject.hash
      open("#{cacert_directory}/#{hashed_name}", "w") {|f| f << CA_CERT }
      prxy = WEBrick::HTTPProxyServer.new({
        :ServerType => Thread,
        :Logger => WEBrick::Log.new(NullLog),
        :AccessLog => [[sio=StringIO.new, WEBrick::AccessLog::COMMON_LOG_FORMAT]],
        :BindAddress => '127.0.0.1',
        :Port => 0})
      _, p_port, _, p_host = prxy.listeners[0].addr
      begin
        th = prxy.start
        open("#{dr}/proxy", "w") {|f| f << "proxy" }
        open("#{url}/proxy", :proxy=>"http://#{p_host}:#{p_port}/", :ssl_ca_cert => cacert_filename) {|f|
          assert_equal("200", f.status[0])
          assert_equal("proxy", f.read)
        }
        assert_match(%r[CONNECT #{url.sub(%r{\Ahttps://}, '')} ], sio.string)
        sio.truncate(0); sio.rewind
        open("#{url}/proxy", :proxy=>"http://#{p_host}:#{p_port}/", :ssl_ca_cert => cacert_directory) {|f|
          assert_equal("200", f.status[0])
          assert_equal("proxy", f.read)
        }
        assert_match(%r[CONNECT #{url.sub(%r{\Ahttps://}, '')} ], sio.string)
        sio.truncate(0); sio.rewind
      ensure
        prxy.shutdown
      end
    }
  end

end

# mkdir demoCA demoCA/private demoCA/newcerts
# touch demoCA/index.txt
# echo 00 > demoCA/serial
# openssl req -new -keyout demoCA/private/cakey.pem -out demoCA/careq.pem
# openssl ca -out demoCA/cacert.pem -startdate 090101000000Z -enddate 491231235959Z -batch -keyfile demoCA/private/cakey.pem -selfsign -infiles demoCA/careq.pem

# cp /etc/ssl/openssl.cnf openssl-server.cnf # Debian
# vi openssl-server.cnf # enable "nsCertType = server"
# mkdir server
# openssl genrsa -des3 -out server/server.key 1024
# openssl rsa -in server/server.key -out server/servernopass.key
# openssl req -new -days 365 -key server/servernopass.key -out server/csr.pem
# openssl ca -config openssl-server.cnf -startdate 090101000000Z -enddate 491231235959Z -in server/csr.pem -keyfile demoCA/private/cakey.pem -cert demoCA/cacert.pem -out server/cert.pem

# demoCA/cacert.pem => TestOpenURISSL::CA_CERT
# server/cert.pem => TestOpenURISSL::SERVER_CERT
# server/servernopass.key => TestOpenURISSL::SERVER_KEY

TestOpenURISSL::CA_CERT = <<'End'
Certificate:
    Data:
        Version: 3 (0x2)
        Serial Number: 0 (0x0)
        Signature Algorithm: sha1WithRSAEncryption
        Issuer: C=JP, ST=Tokyo, O=RubyTest, CN=Ruby Test CA
        Validity
            Not Before: Jan  1 00:00:00 2009 GMT
            Not After : Dec 31 23:59:59 2049 GMT
        Subject: C=JP, ST=Tokyo, O=RubyTest, CN=Ruby Test CA
        Subject Public Key Info:
            Public Key Algorithm: rsaEncryption
            RSA Public Key: (1024 bit)
                Modulus (1024 bit):
                    00:9f:58:19:39:bc:ea:0c:b8:c3:5d:12:a7:d8:20:
                    6c:53:ac:91:34:c8:b4:db:3f:56:f6:75:b6:6c:23:
                    80:23:6a:5f:b3:f6:9a:3e:00:b4:16:19:1c:9c:2c:
                    8d:e8:53:d5:0b:f1:52:3f:7b:60:93:86:ae:89:ab:
                    20:82:9a:b6:72:14:3c:4d:a9:0b:6c:34:79:9e:d3:
                    14:82:6d:c9:3b:90:d9:5e:68:6f:8c:b5:d8:09:f4:
                    6f:3b:22:9f:5e:81:9c:37:df:cf:90:36:65:57:dc:
                    ad:31:ca:8b:48:92:a7:3c:1e:42:e9:1c:4e:1e:cb:
                    36:c1:44:4e:ab:9a:b2:73:6d
                Exponent: 65537 (0x10001)
        X509v3 extensions:
            X509v3 Basic Constraints:
                CA:FALSE
            Netscape Comment:
                OpenSSL Generated Certificate
            X509v3 Subject Key Identifier:
                24:6F:03:A3:EE:06:51:75:B2:BA:FC:3A:38:59:BF:ED:87:CD:E8:7F
            X509v3 Authority Key Identifier:
                keyid:24:6F:03:A3:EE:06:51:75:B2:BA:FC:3A:38:59:BF:ED:87:CD:E8:7F

    Signature Algorithm: sha1WithRSAEncryption
        13:eb:db:ca:cd:90:f2:09:9e:d9:72:70:5e:42:5d:11:84:ce:
        00:1d:c4:2f:41:d2:3e:16:e5:d4:97:1f:43:a9:a7:9c:fa:60:
        c4:35:96:f2:f6:0d:13:6d:0f:36:dd:59:03:08:ee:2e:a6:df:
        9e:d8:6d:ca:72:8f:02:c2:2b:53:7b:12:7f:55:81:6c:9e:7d:
        e7:40:7e:f8:f5:75:0d:4b:a0:8d:ee:a4:d9:e8:5f:06:c9:86:
        66:71:70:6c:41:81:6a:dd:a4:4f:a3:c1:ac:70:d4:78:1b:23:
        30:2f:a5:ef:98:ee:d4:62:80:fd:bf:d4:7a:9b:8e:2d:18:e5:
        00:46
-----BEGIN CERTIFICATE-----
MIICfzCCAeigAwIBAgIBADANBgkqhkiG9w0BAQUFADBHMQswCQYDVQQGEwJKUDEO
MAwGA1UECBMFVG9reW8xETAPBgNVBAoTCFJ1YnlUZXN0MRUwEwYDVQQDEwxSdWJ5
IFRlc3QgQ0EwHhcNMDkwMTAxMDAwMDAwWhcNNDkxMjMxMjM1OTU5WjBHMQswCQYD
VQQGEwJKUDEOMAwGA1UECBMFVG9reW8xETAPBgNVBAoTCFJ1YnlUZXN0MRUwEwYD
VQQDEwxSdWJ5IFRlc3QgQ0EwgZ8wDQYJKoZIhvcNAQEBBQADgY0AMIGJAoGBAJ9Y
GTm86gy4w10Sp9ggbFOskTTItNs/VvZ1tmwjgCNqX7P2mj4AtBYZHJwsjehT1Qvx
Uj97YJOGromrIIKatnIUPE2pC2w0eZ7TFIJtyTuQ2V5ob4y12An0bzsin16BnDff
z5A2ZVfcrTHKi0iSpzweQukcTh7LNsFETquasnNtAgMBAAGjezB5MAkGA1UdEwQC
MAAwLAYJYIZIAYb4QgENBB8WHU9wZW5TU0wgR2VuZXJhdGVkIENlcnRpZmljYXRl
MB0GA1UdDgQWBBQkbwOj7gZRdbK6/Do4Wb/th83ofzAfBgNVHSMEGDAWgBQkbwOj
7gZRdbK6/Do4Wb/th83ofzANBgkqhkiG9w0BAQUFAAOBgQAT69vKzZDyCZ7ZcnBe
Ql0RhM4AHcQvQdI+FuXUlx9Dqaec+mDENZby9g0TbQ823VkDCO4upt+e2G3Kco8C
witTexJ/VYFsnn3nQH749XUNS6CN7qTZ6F8GyYZmcXBsQYFq3aRPo8GscNR4GyMw
L6XvmO7UYoD9v9R6m44tGOUARg==
-----END CERTIFICATE-----
End

TestOpenURISSL::SERVER_CERT = <<'End'
Certificate:
    Data:
        Version: 3 (0x2)
        Serial Number: 1 (0x1)
        Signature Algorithm: sha1WithRSAEncryption
        Issuer: C=JP, ST=Tokyo, O=RubyTest, CN=Ruby Test CA
        Validity
            Not Before: Jan  1 00:00:00 2009 GMT
            Not After : Dec 31 23:59:59 2049 GMT
        Subject: C=JP, ST=Tokyo, O=RubyTest, CN=127.0.0.1
        Subject Public Key Info:
            Public Key Algorithm: rsaEncryption
            RSA Public Key: (1024 bit)
                Modulus (1024 bit):
                    00:bb:bd:74:69:53:58:50:24:79:f2:eb:db:8b:97:
                    e4:69:a4:dd:48:0c:40:35:62:42:b3:35:8c:96:2a:
                    62:76:98:b5:2a:e0:f8:78:33:b6:ff:f8:55:bf:44:
                    69:21:d7:b5:0e:bd:8a:dd:31:1b:88:d5:b4:5e:7a:
                    82:e0:ba:99:6c:04:76:e9:ff:e6:f8:f5:06:8e:7e:
                    a4:db:db:eb:43:44:12:a7:ca:ca:2b:aa:5f:83:10:
                    e2:9e:35:55:e8:e8:af:be:c8:7d:bb:c2:d4:aa:c1:
                    1c:57:0b:c0:0c:3a:1d:6e:23:a9:03:26:7c:ea:8c:
                    f0:86:61:ce:f1:ff:42:c7:23
                Exponent: 65537 (0x10001)
        X509v3 extensions:
            X509v3 Basic Constraints:
                CA:FALSE
            Netscape Cert Type:
                SSL Server
            Netscape Comment:
                OpenSSL Generated Certificate
            X509v3 Subject Key Identifier:
                7F:17:5A:58:88:96:E1:1F:44:EA:FF:AD:C6:2E:90:E2:95:32:DD:F0
            X509v3 Authority Key Identifier:
                keyid:24:6F:03:A3:EE:06:51:75:B2:BA:FC:3A:38:59:BF:ED:87:CD:E8:7F

    Signature Algorithm: sha1WithRSAEncryption
        9a:34:99:ea:76:a2:ed:f0:f7:a7:75:3b:81:fb:75:57:93:c1:
        27:b6:1e:7a:38:67:95:be:58:42:9a:0a:dd:2b:23:fb:85:42:
        80:34:bf:b9:0e:9c:5e:5a:dc:2d:25:8c:68:02:a2:c7:7f:c0:
        eb:f3:e0:61:e2:05:e5:7e:c1:e0:33:1c:76:65:23:2c:25:08:
        f6:5a:11:b9:d4:f7:e3:80:bb:b0:ce:76:1a:56:22:af:e2:4a:
        e1:7e:a4:60:f3:fd:9c:53:46:51:57:32:6b:05:53:80:5c:a5:
        61:93:87:ae:06:a8:a2:ba:4d:a1:b7:1b:0f:8f:82:0a:e8:b3:
        ea:63
-----BEGIN CERTIFICATE-----
MIICkTCCAfqgAwIBAgIBATANBgkqhkiG9w0BAQUFADBHMQswCQYDVQQGEwJKUDEO
MAwGA1UECBMFVG9reW8xETAPBgNVBAoTCFJ1YnlUZXN0MRUwEwYDVQQDEwxSdWJ5
IFRlc3QgQ0EwHhcNMDkwMTAxMDAwMDAwWhcNNDkxMjMxMjM1OTU5WjBEMQswCQYD
VQQGEwJKUDEOMAwGA1UECBMFVG9reW8xETAPBgNVBAoTCFJ1YnlUZXN0MRIwEAYD
VQQDEwkxMjcuMC4wLjEwgZ8wDQYJKoZIhvcNAQEBBQADgY0AMIGJAoGBALu9dGlT
WFAkefLr24uX5Gmk3UgMQDViQrM1jJYqYnaYtSrg+Hgztv/4Vb9EaSHXtQ69it0x
G4jVtF56guC6mWwEdun/5vj1Bo5+pNvb60NEEqfKyiuqX4MQ4p41Vejor77IfbvC
1KrBHFcLwAw6HW4jqQMmfOqM8IZhzvH/QscjAgMBAAGjgY8wgYwwCQYDVR0TBAIw
ADARBglghkgBhvhCAQEEBAMCBkAwLAYJYIZIAYb4QgENBB8WHU9wZW5TU0wgR2Vu
ZXJhdGVkIENlcnRpZmljYXRlMB0GA1UdDgQWBBR/F1pYiJbhH0Tq/63GLpDilTLd
8DAfBgNVHSMEGDAWgBQkbwOj7gZRdbK6/Do4Wb/th83ofzANBgkqhkiG9w0BAQUF
AAOBgQCaNJnqdqLt8PendTuB+3VXk8Enth56OGeVvlhCmgrdKyP7hUKANL+5Dpxe
WtwtJYxoAqLHf8Dr8+Bh4gXlfsHgMxx2ZSMsJQj2WhG51PfjgLuwznYaViKv4krh
fqRg8/2cU0ZRVzJrBVOAXKVhk4euBqiiuk2htxsPj4IK6LPqYw==
-----END CERTIFICATE-----
End

TestOpenURISSL::SERVER_KEY = <<'End'
Private-Key: (1024 bit)
modulus:
    00:bb:bd:74:69:53:58:50:24:79:f2:eb:db:8b:97:
    e4:69:a4:dd:48:0c:40:35:62:42:b3:35:8c:96:2a:
    62:76:98:b5:2a:e0:f8:78:33:b6:ff:f8:55:bf:44:
    69:21:d7:b5:0e:bd:8a:dd:31:1b:88:d5:b4:5e:7a:
    82:e0:ba:99:6c:04:76:e9:ff:e6:f8:f5:06:8e:7e:
    a4:db:db:eb:43:44:12:a7:ca:ca:2b:aa:5f:83:10:
    e2:9e:35:55:e8:e8:af:be:c8:7d:bb:c2:d4:aa:c1:
    1c:57:0b:c0:0c:3a:1d:6e:23:a9:03:26:7c:ea:8c:
    f0:86:61:ce:f1:ff:42:c7:23
publicExponent: 65537 (0x10001)
privateExponent:
    00:af:3a:ec:17:0a:f5:d9:07:d2:d3:4c:15:c5:3b:
    66:b4:bc:6e:d5:ba:a9:8b:aa:45:3b:63:f5:ee:8b:
    6d:0f:e9:04:e0:1a:cf:8f:d2:25:32:d1:a5:a7:3a:
    c1:2e:17:5a:25:82:00:c4:e7:fb:1d:42:ea:71:6c:
    c4:0f:e1:db:23:ff:1e:d6:c8:d6:60:ca:2d:06:fc:
    54:3c:03:d4:09:96:bb:38:7a:22:a1:61:2c:f7:d0:
    d0:90:6c:9f:61:ba:61:30:5a:aa:64:ad:43:3a:53:
    38:e8:ba:cc:8c:51:3e:68:3e:3a:6a:0f:5d:5d:e0:
    d6:df:f2:54:93:d3:14:22:a1
prime1:
    00:e8:ec:11:fe:e6:2b:23:21:29:d5:40:a6:11:ec:
    4c:ae:4d:08:2a:71:18:ac:d1:3e:40:2f:12:41:59:
    12:09:e2:f7:c2:d7:6b:0a:96:0a:06:e3:90:6a:4e:
    b2:eb:25:b7:09:68:e9:13:ab:d0:5a:29:7a:e4:72:
    1a:ee:46:a0:8b
prime2:
    00:ce:57:5e:31:e9:c9:a8:5b:1f:55:af:67:e2:49:
    2a:af:90:b6:02:c0:32:2f:ca:ae:1e:de:47:81:73:
    a8:f8:37:53:70:93:24:62:77:d4:b8:80:30:9f:65:
    26:20:46:ae:5a:65:6e:6d:af:68:4c:8d:e8:3c:f3:
    d1:d1:d9:6e:c9
exponent1:
    03:f1:02:b8:f2:82:26:5d:08:4d:30:83:de:e7:c5:
    c0:69:53:4b:0c:90:e3:53:c3:1e:e8:ed:01:28:15:
    b3:0f:21:2c:2d:e3:04:d1:d7:27:98:b0:37:ec:4f:
    00:c5:a9:9c:42:27:37:8a:ff:c2:96:d3:1a:8c:87:
    c2:22:75:d3
exponent2:
    6f:17:32:ab:84:c7:01:51:2d:e9:9f:ea:3a:36:52:
    38:fb:9c:42:96:df:6e:43:9c:c3:19:c1:3d:bc:db:
    77:e7:b1:90:a6:67:ac:6b:ff:a6:e5:bd:47:d3:d9:
    56:ff:36:d7:8c:4c:8b:d9:28:3a:2f:1c:9d:d4:57:
    5e:b7:c5:a1
coefficient:
    45:50:47:66:56:e9:21:d9:40:0e:af:3f:f2:05:77:
    ab:e7:08:40:97:88:2a:51:b3:7e:86:b0:b2:03:2e:
    6d:36:3f:46:42:97:7d:5a:a2:93:6c:05:c2:8b:8b:
    2d:af:d5:7d:75:e9:70:f0:2d:21:e3:b9:cf:4d:9a:
    c4:97:e2:79
-----BEGIN RSA PRIVATE KEY-----
MIICXAIBAAKBgQC7vXRpU1hQJHny69uLl+RppN1IDEA1YkKzNYyWKmJ2mLUq4Ph4
M7b/+FW/RGkh17UOvYrdMRuI1bReeoLguplsBHbp/+b49QaOfqTb2+tDRBKnysor
ql+DEOKeNVXo6K++yH27wtSqwRxXC8AMOh1uI6kDJnzqjPCGYc7x/0LHIwIDAQAB
AoGBAK867BcK9dkH0tNMFcU7ZrS8btW6qYuqRTtj9e6LbQ/pBOAaz4/SJTLRpac6
wS4XWiWCAMTn+x1C6nFsxA/h2yP/HtbI1mDKLQb8VDwD1AmWuzh6IqFhLPfQ0JBs
n2G6YTBaqmStQzpTOOi6zIxRPmg+OmoPXV3g1t/yVJPTFCKhAkEA6OwR/uYrIyEp
1UCmEexMrk0IKnEYrNE+QC8SQVkSCeL3wtdrCpYKBuOQak6y6yW3CWjpE6vQWil6
5HIa7kagiwJBAM5XXjHpyahbH1WvZ+JJKq+QtgLAMi/Krh7eR4FzqPg3U3CTJGJ3
1LiAMJ9lJiBGrlplbm2vaEyN6Dzz0dHZbskCQAPxArjygiZdCE0wg97nxcBpU0sM
kONTwx7o7QEoFbMPISwt4wTR1yeYsDfsTwDFqZxCJzeK/8KW0xqMh8IiddMCQG8X
MquExwFRLemf6jo2Ujj7nEKW325DnMMZwT2823fnsZCmZ6xr/6blvUfT2Vb/NteM
TIvZKDovHJ3UV163xaECQEVQR2ZW6SHZQA6vP/IFd6vnCECXiCpRs36GsLIDLm02
P0ZCl31aopNsBcKLiy2v1X116XDwLSHjuc9NmsSX4nk=
-----END RSA PRIVATE KEY-----
End
