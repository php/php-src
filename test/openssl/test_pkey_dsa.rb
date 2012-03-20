require_relative 'utils'
require 'base64'

if defined?(OpenSSL)

class OpenSSL::TestPKeyDSA < Test::Unit::TestCase
  def test_private
    key = OpenSSL::PKey::DSA.new(256)
    assert(key.private?)
    key2 = OpenSSL::PKey::DSA.new(key.to_der)
    assert(key2.private?)
    key3 = key.public_key
    assert(!key3.private?)
    key4 = OpenSSL::PKey::DSA.new(key3.to_der)
    assert(!key4.private?)
  end

  def test_new
    key = OpenSSL::PKey::DSA.new 256
    pem  = key.public_key.to_pem
    OpenSSL::PKey::DSA.new pem
    assert_equal([], OpenSSL.errors)
  end

  def test_new_break
    assert_nil(OpenSSL::PKey::DSA.new(512) { break })
    assert_raise(RuntimeError) do
      OpenSSL::PKey::DSA.new(512) { raise }
    end
  end

  def test_sys_sign_verify
    key = OpenSSL::TestUtils::TEST_KEY_DSA256
    data = 'Sign me!'
    digest = OpenSSL::Digest::SHA1.digest(data)
    sig = key.syssign(digest)
    assert(key.sysverify(digest, sig))
  end

  def test_sign_verify
    check_sign_verify(OpenSSL::Digest::DSS1.new)
  end

if (OpenSSL::OPENSSL_VERSION_NUMBER > 0x10000000)
  def test_sign_verify_sha1
    check_sign_verify(OpenSSL::Digest::SHA1.new)
  end

  def test_sign_verify_sha256
    check_sign_verify(OpenSSL::Digest::SHA256.new)
  end
end

  def test_digest_state_irrelevant_verify
    key = OpenSSL::TestUtils::TEST_KEY_DSA256
    digest1 = OpenSSL::Digest::DSS1.new
    digest2 = OpenSSL::Digest::DSS1.new
    data = 'Sign me!'
    sig = key.sign(digest1, data)
    digest1.reset
    digest1 << 'Change state of digest1'
    assert(key.verify(digest1, sig, data))
    assert(key.verify(digest2, sig, data))
  end

  def test_read_DSA_PUBKEY
    p = 7188211954100152441468596248707152960171255279130004340103875772401008316444412091945435731597638374542374929457672178957081124632837356913990200866056699
    q = 957032439192465935099784319494405376402293318491
    g = 122928973717064636255205666162891733518376475981809749897454444301389338825906076467196186192907631719698166056821519884939865041993585844526937010746285
    y = 1235756183583465414789073313502727057075641172514181938731172021825149551960029708596057102104063395063907739571546165975727369183495540798749742124846271
    algo = OpenSSL::ASN1::ObjectId.new('DSA')
    params = OpenSSL::ASN1::Sequence.new([OpenSSL::ASN1::Integer.new(p),
                                          OpenSSL::ASN1::Integer.new(q),
                                          OpenSSL::ASN1::Integer.new(g)])
    algo_id = OpenSSL::ASN1::Sequence.new ([algo, params])
    pub_key = OpenSSL::ASN1::Integer.new(y)
    seq = OpenSSL::ASN1::Sequence.new([algo_id, OpenSSL::ASN1::BitString.new(pub_key.to_der)])
    key = OpenSSL::PKey::DSA.new(seq.to_der)
    assert(key.public?)
    assert(!key.private?)
    assert_equal(p, key.p)
    assert_equal(q, key.q)
    assert_equal(g, key.g)
    assert_equal(y, key.pub_key)
    assert_equal(nil, key.priv_key)
    assert_equal([], OpenSSL.errors)
  end

  def test_read_DSAPublicKey_pem
    p = 12260055936871293565827712385212529106400444521449663325576634579961635627321079536132296996623400607469624537382977152381984332395192110731059176842635699
    q = 979494906553787301107832405790107343409973851677
    g = 3731695366899846297271147240305742456317979984190506040697507048095553842519347835107669437969086119948785140453492839427038591924536131566350847469993845
    y = 10505239074982761504240823422422813362721498896040719759460296306305851824586095328615844661273887569281276387605297130014564808567159023649684010036304695
    pem = <<-EOF
-----BEGIN DSA PUBLIC KEY-----
MIHfAkEAyJSJ+g+P/knVcgDwwTzC7Pwg/pWs2EMd/r+lYlXhNfzg0biuXRul8VR4
VUC/phySExY0PdcqItkR/xYAYNMbNwJBAOoV57X0FxKO/PrNa/MkoWzkCKV/hzhE
p0zbFdsicw+hIjJ7S6Sd/FlDlo89HQZ2FuvWJ6wGLM1j00r39+F2qbMCFQCrkhIX
SG+is37hz1IaBeEudjB2HQJAR0AloavBvtsng8obsjLb7EKnB+pSeHr/BdIQ3VH7
fWLOqqkzFeRrYMDzUpl36XktY6Yq8EJYlW9pCMmBVNy/dQ==
-----END DSA PUBLIC KEY-----
    EOF
    key = OpenSSL::PKey::DSA.new(pem)
    assert(key.public?)
    assert(!key.private?)
    assert_equal(p, key.p)
    assert_equal(q, key.q)
    assert_equal(g, key.g)
    assert_equal(y, key.pub_key)
    assert_equal(nil, key.priv_key)
    assert_equal([], OpenSSL.errors)
  end

  def test_read_DSA_PUBKEY_pem
    p = 12260055936871293565827712385212529106400444521449663325576634579961635627321079536132296996623400607469624537382977152381984332395192110731059176842635699
    q = 979494906553787301107832405790107343409973851677
    g = 3731695366899846297271147240305742456317979984190506040697507048095553842519347835107669437969086119948785140453492839427038591924536131566350847469993845
    y = 10505239074982761504240823422422813362721498896040719759460296306305851824586095328615844661273887569281276387605297130014564808567159023649684010036304695
    pem = <<-EOF
-----BEGIN PUBLIC KEY-----
MIHxMIGoBgcqhkjOOAQBMIGcAkEA6hXntfQXEo78+s1r8yShbOQIpX+HOESnTNsV
2yJzD6EiMntLpJ38WUOWjz0dBnYW69YnrAYszWPTSvf34XapswIVAKuSEhdIb6Kz
fuHPUhoF4S52MHYdAkBHQCWhq8G+2yeDyhuyMtvsQqcH6lJ4ev8F0hDdUft9Ys6q
qTMV5GtgwPNSmXfpeS1jpirwQliVb2kIyYFU3L91A0QAAkEAyJSJ+g+P/knVcgDw
wTzC7Pwg/pWs2EMd/r+lYlXhNfzg0biuXRul8VR4VUC/phySExY0PdcqItkR/xYA
YNMbNw==
-----END PUBLIC KEY-----
    EOF
    key = OpenSSL::PKey::DSA.new(pem)
    assert(key.public?)
    assert(!key.private?)
    assert_equal(p, key.p)
    assert_equal(q, key.q)
    assert_equal(g, key.g)
    assert_equal(y, key.pub_key)
    assert_equal(nil, key.priv_key)
    assert_equal([], OpenSSL.errors)
  end

  def test_export_format_is_DSA_PUBKEY_pem
    key = OpenSSL::TestUtils::TEST_KEY_DSA256
    pem = key.public_key.to_pem
    pem.gsub!(/^-+(\w|\s)+-+$/, "") # eliminate --------BEGIN...-------
    asn1 = OpenSSL::ASN1.decode(Base64.decode64(pem))
    assert_equal(OpenSSL::ASN1::SEQUENCE, asn1.tag)
    assert_equal(2, asn1.value.size)
    seq = asn1.value
    assert_equal(OpenSSL::ASN1::SEQUENCE, seq[0].tag)
    assert_equal(2, seq[0].value.size)
    algo_id = seq[0].value
    assert_equal(OpenSSL::ASN1::OBJECT, algo_id[0].tag)
    assert_equal('DSA', algo_id[0].value)
    assert_equal(OpenSSL::ASN1::SEQUENCE, algo_id[1].tag)
    assert_equal(3, algo_id[1].value.size)
    params = algo_id[1].value
    assert_equal(OpenSSL::ASN1::INTEGER, params[0].tag)
    assert_equal(key.p, params[0].value)
    assert_equal(OpenSSL::ASN1::INTEGER, params[1].tag)
    assert_equal(key.q, params[1].value)
    assert_equal(OpenSSL::ASN1::INTEGER, params[2].tag)
    assert_equal(key.g, params[2].value)
    assert_equal(OpenSSL::ASN1::BIT_STRING, seq[1].tag)
    assert_equal(0, seq[1].unused_bits)
    pub_key = OpenSSL::ASN1.decode(seq[1].value)
    assert_equal(OpenSSL::ASN1::INTEGER, pub_key.tag)
    assert_equal(key.pub_key, pub_key.value)
    assert_equal([], OpenSSL.errors)
  end

  def test_read_private_key_der
    key = OpenSSL::TestUtils::TEST_KEY_DSA256
    der = key.to_der
    key2 = OpenSSL::PKey.read(der)
    assert(key2.private?)
    assert_equal(der, key2.to_der)
    assert_equal([], OpenSSL.errors)
  end

  def test_read_private_key_pem
    key = OpenSSL::TestUtils::TEST_KEY_DSA256
    pem = key.to_pem
    key2 = OpenSSL::PKey.read(pem)
    assert(key2.private?)
    assert_equal(pem, key2.to_pem)
    assert_equal([], OpenSSL.errors)
  end

  def test_read_public_key_der
    key = OpenSSL::TestUtils::TEST_KEY_DSA256.public_key
    der = key.to_der
    key2 = OpenSSL::PKey.read(der)
    assert(!key2.private?)
    assert_equal(der, key2.to_der)
    assert_equal([], OpenSSL.errors)
  end

  def test_read_public_key_pem
    key = OpenSSL::TestUtils::TEST_KEY_DSA256.public_key
    pem = key.to_pem
    key2 = OpenSSL::PKey.read(pem)
    assert(!key2.private?)
    assert_equal(pem, key2.to_pem)
    assert_equal([], OpenSSL.errors)
  end

  def test_read_private_key_pem_pw
    key = OpenSSL::TestUtils::TEST_KEY_DSA256
    pem = key.to_pem(OpenSSL::Cipher.new('AES-128-CBC'), 'secret')
    #callback form for password
    key2 = OpenSSL::PKey.read(pem) do
      'secret'
    end
    assert(key2.private?)
    # pass password directly
    key2 = OpenSSL::PKey.read(pem, 'secret')
    assert(key2.private?)
    #omit pem equality check, will be different due to cipher iv
    assert_equal([], OpenSSL.errors)
  end

  private

  def check_sign_verify(digest)
    key = OpenSSL::TestUtils::TEST_KEY_DSA256
    data = 'Sign me!'
    sig = key.sign(digest, data)
    assert(key.verify(digest, sig, data))
  end
end

end
