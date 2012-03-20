require_relative 'utils'

class OpenSSL::TestPKCS5 < Test::Unit::TestCase

  def test_pbkdf2_hmac_sha1_rfc6070_c_1_len_20
    p ="password"
    s = "salt"
    c = 1
    dk_len = 20
    raw = %w{ 0c 60 c8 0f 96 1f 0e 71
              f3 a9 b5 24 af 60 12 06
              2f e0 37 a6 }
    expected = [raw.join('')].pack('H*')
    value = OpenSSL::PKCS5.pbkdf2_hmac_sha1(p, s, c, dk_len)
    assert_equal(expected, value)
  end

  def test_pbkdf2_hmac_sha1_rfc6070_c_2_len_20
    p ="password"
    s = "salt"
    c = 2
    dk_len = 20
    raw = %w{ ea 6c 01 4d c7 2d 6f 8c
              cd 1e d9 2a ce 1d 41 f0
              d8 de 89 57 }
    expected = [raw.join('')].pack('H*')
    value = OpenSSL::PKCS5.pbkdf2_hmac_sha1(p, s, c, dk_len)
    assert_equal(expected, value)
  end

  def test_pbkdf2_hmac_sha1_rfc6070_c_4096_len_20
    p ="password"
    s = "salt"
    c = 4096
    dk_len = 20
    raw = %w{ 4b 00 79 01 b7 65 48 9a
              be ad 49 d9 26 f7 21 d0
              65 a4 29 c1 }
    expected = [raw.join('')].pack('H*')
    value = OpenSSL::PKCS5.pbkdf2_hmac_sha1(p, s, c, dk_len)
    assert_equal(expected, value)
  end

# takes too long!
#  def test_pbkdf2_hmac_sha1_rfc6070_c_16777216_len_20
#    p ="password"
#    s = "salt"
#    c = 16777216
#    dk_len = 20
#    raw = %w{ ee fe 3d 61 cd 4d a4 e4
#              e9 94 5b 3d 6b a2 15 8c
#              26 34 e9 84 }
#    expected = [raw.join('')].pack('H*')
#    value = OpenSSL::PKCS5.pbkdf2_hmac_sha1(p, s, c, dk_len)
#    assert_equal(expected, value)
#  end

  def test_pbkdf2_hmac_sha1_rfc6070_c_4096_len_25
    p ="passwordPASSWORDpassword"
    s = "saltSALTsaltSALTsaltSALTsaltSALTsalt"
    c = 4096
    dk_len = 25

    raw = %w{ 3d 2e ec 4f e4 1c 84 9b
              80 c8 d8 36 62 c0 e4 4a
              8b 29 1a 96 4c f2 f0 70
              38 }
    expected = [raw.join('')].pack('H*')
    value = OpenSSL::PKCS5.pbkdf2_hmac_sha1(p, s, c, dk_len)
    assert_equal(expected, value)
  end

  def test_pbkdf2_hmac_sha1_rfc6070_c_4096_len_16
    p ="pass\0word"
    s = "sa\0lt"
    c = 4096
    dk_len = 16
    raw = %w{ 56 fa 6a a7 55 48 09 9d
              cc 37 d7 f0 34 25 e0 c3 }
    expected = [raw.join('')].pack('H*')
    value = OpenSSL::PKCS5.pbkdf2_hmac_sha1(p, s, c, dk_len)
    assert_equal(expected, value)
  end

  def test_pbkdf2_hmac_sha256_c_20000_len_32
    #unfortunately no official test vectors available yet for SHA-2
    p ="password"
    s = OpenSSL::Random.random_bytes(16)
    c = 20000
    dk_len = 32
    digest = OpenSSL::Digest::SHA256.new
    value1 = OpenSSL::PKCS5.pbkdf2_hmac(p, s, c, dk_len, digest)
    value2 = OpenSSL::PKCS5.pbkdf2_hmac(p, s, c, dk_len, digest)
    assert_equal(value1, value2)
  end if OpenSSL::PKCS5.respond_to?(:pbkdf2_hmac)

end if defined?(OpenSSL)
