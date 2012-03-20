require 'rubygems/test_case'
require 'rubygems/security'
require 'rubygems/fix_openssl_warnings' if RUBY_VERSION < "1.9"

class TestGemSecurity < Gem::TestCase

  def test_class_build_cert
    name = OpenSSL::X509::Name.parse "CN=nobody/DC=example"
    key = OpenSSL::PKey::RSA.new 512
    opt = { :cert_age => 60 }

    cert = Gem::Security.build_cert name, key, opt

    assert_kind_of OpenSSL::X509::Certificate, cert

    assert_equal    2,                     cert.version
    assert_equal    0,                     cert.serial
    assert_equal    key.public_key.to_pem, cert.public_key.to_pem
    assert_in_delta Time.now,              cert.not_before, 10
    assert_in_delta Time.now + 60,         cert.not_after, 10
    assert_equal    name.to_s,             cert.subject.to_s

    assert_equal 3, cert.extensions.length

    constraints = cert.extensions.find { |ext| ext.oid == 'basicConstraints' }
    assert_equal 'CA:FALSE', constraints.value

    key_usage = cert.extensions.find { |ext| ext.oid == 'keyUsage' }
    assert_equal 'Digital Signature, Key Encipherment, Data Encipherment',
                 key_usage.value

    key_ident = cert.extensions.find { |ext| ext.oid == 'subjectKeyIdentifier' }
    assert_equal 59, key_ident.value.length

    assert_equal name.to_s, cert.issuer.to_s
    assert_equal name.to_s, cert.subject.to_s
  end

  def test_class_build_self_signed_cert
    email = 'nobody@example'
    opt = {
      :cert_age  => 60,
      :key_size  => 512,
      :save_cert => false,
      :save_key  => false,
      :trust_dir => File.join(Gem.user_home, '.gem', 'trust'),
    }

    result = Gem::Security.build_self_signed_cert email, opt

    key = result[:key]

    assert_kind_of OpenSSL::PKey::RSA, key
    # assert_equal 512, key.something_here

    cert = result[:cert]

    assert_equal '/CN=nobody/DC=example', cert.issuer.to_s
  end

  def test_class_sign_cert
    name = OpenSSL::X509::Name.parse "CN=nobody/DC=example"
    key  = OpenSSL::PKey::RSA.new 512
    cert = OpenSSL::X509::Certificate.new

    cert.subject    = name
    cert.public_key = key.public_key

    signed = Gem::Security.sign_cert cert, key, cert

    assert cert.verify key
    assert_equal name.to_s, signed.subject.to_s
  end

  def test_class_email_to_name
    munger = Gem::Security::OPT[:munge_re]

    assert_equal '/CN=nobody/DC=example',
                 Gem::Security.email_to_name('nobody@example', munger).to_s

    assert_equal '/CN=nobody/DC=example/DC=com',
                 Gem::Security.email_to_name('nobody@example.com', munger).to_s

    assert_equal '/CN=no.body/DC=example',
                 Gem::Security.email_to_name('no.body@example', munger).to_s

    assert_equal '/CN=no_body/DC=example',
                 Gem::Security.email_to_name('no+body@example', munger).to_s
  end

end if defined?(OpenSSL)
