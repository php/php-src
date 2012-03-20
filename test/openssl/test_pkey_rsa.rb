require_relative 'utils'
require 'base64'

if defined?(OpenSSL)

class OpenSSL::TestPKeyRSA < Test::Unit::TestCase
  def test_padding
    key = OpenSSL::PKey::RSA.new(512, 3)

    # Need right size for raw mode
    plain0 = "x" * (512/8)
    cipher = key.private_encrypt(plain0, OpenSSL::PKey::RSA::NO_PADDING)
    plain1 = key.public_decrypt(cipher, OpenSSL::PKey::RSA::NO_PADDING)
    assert_equal(plain0, plain1)

    # Need smaller size for pkcs1 mode
    plain0 = "x" * (512/8 - 11)
    cipher1 = key.private_encrypt(plain0, OpenSSL::PKey::RSA::PKCS1_PADDING)
    plain1 = key.public_decrypt(cipher1, OpenSSL::PKey::RSA::PKCS1_PADDING)
    assert_equal(plain0, plain1)

    cipherdef = key.private_encrypt(plain0) # PKCS1_PADDING is default
    plain1 = key.public_decrypt(cipherdef)
    assert_equal(plain0, plain1)
    assert_equal(cipher1, cipherdef)

    # Failure cases
    assert_raise(ArgumentError){ key.private_encrypt() }
    assert_raise(ArgumentError){ key.private_encrypt("hi", 1, nil) }
    assert_raise(OpenSSL::PKey::RSAError){ key.private_encrypt(plain0, 666) }
  end

  def test_private
    key = OpenSSL::PKey::RSA.new(512, 3)
    assert(key.private?)
    key2 = OpenSSL::PKey::RSA.new(key.to_der)
    assert(key2.private?)
    key3 = key.public_key
    assert(!key3.private?)
    key4 = OpenSSL::PKey::RSA.new(key3.to_der)
    assert(!key4.private?)
  end

  def test_new
    key = OpenSSL::PKey::RSA.new 512
    pem  = key.public_key.to_pem
    OpenSSL::PKey::RSA.new pem
    assert_equal([], OpenSSL.errors)
  end

  def test_new_exponent_default
    assert_equal(65537, OpenSSL::PKey::RSA.new(512).e)
  end

  def test_new_with_exponent
    1.upto(30) do |idx|
      e = (2 ** idx) + 1
      key = OpenSSL::PKey::RSA.new(512, e)
      assert_equal(e, key.e)
    end
  end

  def test_new_break
    assert_nil(OpenSSL::PKey::RSA.new(1024) { break })
    assert_raise(RuntimeError) do
      OpenSSL::PKey::RSA.new(1024) { raise }
    end
  end

  def test_sign_verify
    key = OpenSSL::TestUtils::TEST_KEY_RSA1024
    digest = OpenSSL::Digest::SHA1.new
    data = 'Sign me!'
    sig = key.sign(digest, data)
    assert(key.verify(digest, sig, data))
  end

  def test_digest_state_irrelevant_sign
    key = OpenSSL::TestUtils::TEST_KEY_RSA1024
    digest1 = OpenSSL::Digest::SHA1.new
    digest2 = OpenSSL::Digest::SHA1.new
    data = 'Sign me!'
    digest1 << 'Change state of digest1'
    sig1 = key.sign(digest1, data)
    sig2 = key.sign(digest2, data)
    assert_equal(sig1, sig2)
  end

  def test_digest_state_irrelevant_verify
    key = OpenSSL::TestUtils::TEST_KEY_RSA1024
    digest1 = OpenSSL::Digest::SHA1.new
    digest2 = OpenSSL::Digest::SHA1.new
    data = 'Sign me!'
    sig = key.sign(digest1, data)
    digest1.reset
    digest1 << 'Change state of digest1'
    assert(key.verify(digest1, sig, data))
    assert(key.verify(digest2, sig, data))
  end

  def test_read_RSAPublicKey
    modulus = 10664264882656732240315063514678024569492171560814833397008094754351396057398262071307709191731289492697968568138092052265293364132872019762410446076526351
    exponent = 65537
    seq = OpenSSL::ASN1::Sequence.new([OpenSSL::ASN1::Integer.new(modulus), OpenSSL::ASN1::Integer.new(exponent)])
    key = OpenSSL::PKey::RSA.new(seq.to_der)
    assert(key.public?)
    assert(!key.private?)
    assert_equal(modulus, key.n)
    assert_equal(exponent, key.e)
    assert_equal(nil, key.d)
    assert_equal(nil, key.p)
    assert_equal(nil, key.q)
    assert_equal([], OpenSSL.errors)
  end

  def test_read_RSA_PUBKEY
    modulus = 10664264882656732240315063514678024569492171560814833397008094754351396057398262071307709191731289492697968568138092052265293364132872019762410446076526351
    exponent = 65537
    algo = OpenSSL::ASN1::ObjectId.new('rsaEncryption')
    null_params = OpenSSL::ASN1::Null.new(nil)
    algo_id = OpenSSL::ASN1::Sequence.new ([algo, null_params])
    pub_key = OpenSSL::ASN1::Sequence.new([OpenSSL::ASN1::Integer.new(modulus), OpenSSL::ASN1::Integer.new(exponent)])
    seq = OpenSSL::ASN1::Sequence.new([algo_id, OpenSSL::ASN1::BitString.new(pub_key.to_der)])
    key = OpenSSL::PKey::RSA.new(seq.to_der)
    assert(key.public?)
    assert(!key.private?)
    assert_equal(modulus, key.n)
    assert_equal(exponent, key.e)
    assert_equal(nil, key.d)
    assert_equal(nil, key.p)
    assert_equal(nil, key.q)
    assert_equal([], OpenSSL.errors)
  end

  def test_read_RSAPublicKey_pem
    modulus = 9416340886363418692990906464787534854462163316648195510702927337693641649864839352187127240942127674615733815606532506566068276485089353644309497938966061
    exponent = 65537
    pem = <<-EOF
-----BEGIN RSA PUBLIC KEY-----
MEgCQQCzyh2RIZK62E2PbTWqUljD+K23XR9AGBKNtXjal6WD2yRGcLqzPJLNCa60
AudJR1JobbIbDJrQu6AXnWh5k/YtAgMBAAE=
-----END RSA PUBLIC KEY-----
    EOF
    key = OpenSSL::PKey::RSA.new(pem)
    assert(key.public?)
    assert(!key.private?)
    assert_equal(modulus, key.n)
    assert_equal(exponent, key.e)
    assert_equal(nil, key.d)
    assert_equal(nil, key.p)
    assert_equal(nil, key.q)
    assert_equal([], OpenSSL.errors)
  end

  def test_read_RSA_PUBKEY_pem
    modulus = 9416340886363418692990906464787534854462163316648195510702927337693641649864839352187127240942127674615733815606532506566068276485089353644309497938966061
    exponent = 65537
    pem = <<-EOF
-----BEGIN PUBLIC KEY-----
MFwwDQYJKoZIhvcNAQEBBQADSwAwSAJBALPKHZEhkrrYTY9tNapSWMP4rbdd
H0AYEo21eNqXpYPbJEZwurM8ks0JrrQC50lHUmhtshsMmtC7oBedaHmT9i0C
AwEAAQ==
-----END PUBLIC KEY-----
    EOF
    key = OpenSSL::PKey::RSA.new(pem)
    assert(key.public?)
    assert(!key.private?)
    assert_equal(modulus, key.n)
    assert_equal(exponent, key.e)
    assert_equal(nil, key.d)
    assert_equal(nil, key.p)
    assert_equal(nil, key.q)
    assert_equal([], OpenSSL.errors)
  end

  def test_export_format_is_RSA_PUBKEY
    key = OpenSSL::PKey::RSA.new(512)
    asn1 = OpenSSL::ASN1.decode(key.public_key.to_der)
    check_PUBKEY(asn1, key)
  end

  def test_export_format_is_RSA_PUBKEY_pem
    key = OpenSSL::PKey::RSA.new(512)
    pem = key.public_key.to_pem
    pem.gsub!(/^-+(\w|\s)+-+$/, "") # eliminate --------BEGIN...-------
    asn1 = OpenSSL::ASN1.decode(Base64.decode64(pem))
    check_PUBKEY(asn1, key)
  end

  def test_read_private_key_der
    der = OpenSSL::TestUtils::TEST_KEY_RSA1024.to_der
    key = OpenSSL::PKey.read(der)
    assert(key.private?)
    assert_equal(der, key.to_der)
    assert_equal([], OpenSSL.errors)
  end

  def test_read_private_key_pem
    pem = OpenSSL::TestUtils::TEST_KEY_RSA1024.to_pem
    key = OpenSSL::PKey.read(pem)
    assert(key.private?)
    assert_equal(pem, key.to_pem)
    assert_equal([], OpenSSL.errors)
  end

  def test_read_public_key_der
    der = OpenSSL::TestUtils::TEST_KEY_RSA1024.public_key.to_der
    key = OpenSSL::PKey.read(der)
    assert(!key.private?)
    assert_equal(der, key.to_der)
    assert_equal([], OpenSSL.errors)
  end

  def test_read_public_key_pem
    pem = OpenSSL::TestUtils::TEST_KEY_RSA1024.public_key.to_pem
    key = OpenSSL::PKey.read(pem)
    assert(!key.private?)
    assert_equal(pem, key.to_pem)
    assert_equal([], OpenSSL.errors)
  end

  def test_read_private_key_pem_pw
    pem = OpenSSL::TestUtils::TEST_KEY_RSA1024.to_pem(OpenSSL::Cipher.new('AES-128-CBC'), 'secret')
    #callback form for password
    key = OpenSSL::PKey.read(pem) do
      'secret'
    end
    assert(key.private?)
    # pass password directly
    key = OpenSSL::PKey.read(pem, 'secret')
    assert(key.private?)
    #omit pem equality check, will be different due to cipher iv
    assert_equal([], OpenSSL.errors)
  end

  def test_read_private_key_pem_pw_exception
    pem = OpenSSL::TestUtils::TEST_KEY_RSA1024.to_pem(OpenSSL::Cipher.new('AES-128-CBC'), 'secret')
    # it raises an ArgumentError from PEM reading. The exception raised inside are ignored for now.
    assert_raise(ArgumentError) do
      OpenSSL::PKey.read(pem) do
        raise RuntimeError
      end
    end
    assert_equal([], OpenSSL.errors)
  end

  private

  def check_PUBKEY(asn1, key)
    assert_equal(OpenSSL::ASN1::SEQUENCE, asn1.tag)
    assert_equal(2, asn1.value.size)
    seq = asn1.value
    assert_equal(OpenSSL::ASN1::SEQUENCE, seq[0].tag)
    assert_equal(2, seq[0].value.size)
    algo_id = seq[0].value
    assert_equal(OpenSSL::ASN1::OBJECT, algo_id[0].tag)
    assert_equal('rsaEncryption', algo_id[0].value)
    assert_equal(OpenSSL::ASN1::NULL, algo_id[1].tag)
    assert_equal(nil, algo_id[1].value)
    assert_equal(OpenSSL::ASN1::BIT_STRING, seq[1].tag)
    assert_equal(0, seq[1].unused_bits)
    pub_key = OpenSSL::ASN1.decode(seq[1].value)
    assert_equal(OpenSSL::ASN1::SEQUENCE, pub_key.tag)
    assert_equal(2, pub_key.value.size)
    assert_equal(OpenSSL::ASN1::INTEGER, pub_key.value[0].tag)
    assert_equal(key.n, pub_key.value[0].value)
    assert_equal(OpenSSL::ASN1::INTEGER, pub_key.value[1].tag)
    assert_equal(key.e, pub_key.value[1].value)
    assert_equal([], OpenSSL.errors)
  end

end

end
