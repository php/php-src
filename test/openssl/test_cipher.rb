require_relative 'utils'

if defined?(OpenSSL)

class OpenSSL::TestCipher < Test::Unit::TestCase
  def setup
    @c1 = OpenSSL::Cipher::Cipher.new("DES-EDE3-CBC")
    @c2 = OpenSSL::Cipher::DES.new(:EDE3, "CBC")
    @key = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    @iv = "\0\0\0\0\0\0\0\0"
    @hexkey = "0000000000000000000000000000000000000000000000"
    @hexiv = "0000000000000000"
    @data = "DATA"
  end

  def teardown
    @c1 = @c2 = nil
  end

  def test_crypt
    @c1.encrypt.pkcs5_keyivgen(@key, @iv)
    @c2.encrypt.pkcs5_keyivgen(@key, @iv)
    s1 = @c1.update(@data) + @c1.final
    s2 = @c2.update(@data) + @c2.final
    assert_equal(s1, s2, "encrypt")

    @c1.decrypt.pkcs5_keyivgen(@key, @iv)
    @c2.decrypt.pkcs5_keyivgen(@key, @iv)
    assert_equal(@data, @c1.update(s1)+@c1.final, "decrypt")
    assert_equal(@data, @c2.update(s2)+@c2.final, "decrypt")
  end

  def test_info
    assert_equal("DES-EDE3-CBC", @c1.name, "name")
    assert_equal("DES-EDE3-CBC", @c2.name, "name")
    assert_kind_of(Fixnum, @c1.key_len, "key_len")
    assert_kind_of(Fixnum, @c1.iv_len, "iv_len")
  end

  def test_dup
    assert_equal(@c1.name, @c1.dup.name, "dup")
    assert_equal(@c1.name, @c1.clone.name, "clone")
    @c1.encrypt
    @c1.key = @key
    @c1.iv = @iv
    tmpc = @c1.dup
    s1 = @c1.update(@data) + @c1.final
    s2 = tmpc.update(@data) + tmpc.final
    assert_equal(s1, s2, "encrypt dup")
  end

  def test_reset
    @c1.encrypt
    @c1.key = @key
    @c1.iv = @iv
    s1 = @c1.update(@data) + @c1.final
    @c1.reset
    s2 = @c1.update(@data) + @c1.final
    assert_equal(s1, s2, "encrypt reset")
  end

  def test_empty_data
    @c1.encrypt
    assert_raise(ArgumentError){ @c1.update("") }
  end

  def test_initialize
    assert_raise(RuntimeError) {@c1.__send__(:initialize, "DES-EDE3-CBC")}
    assert_raise(RuntimeError) {OpenSSL::Cipher.allocate.final}
  end

  if OpenSSL::OPENSSL_VERSION_NUMBER > 0x00907000
    def test_ciphers
      OpenSSL::Cipher.ciphers.each{|name|
        next if /netbsd/ =~ RUBY_PLATFORM && /idea|rc5/i =~ name
        assert(OpenSSL::Cipher::Cipher.new(name).is_a?(OpenSSL::Cipher::Cipher))
      }
    end

    def test_AES
      pt = File.read(__FILE__)
      %w(ECB CBC CFB OFB).each{|mode|
        c1 = OpenSSL::Cipher::AES256.new(mode)
        c1.encrypt
        c1.pkcs5_keyivgen("passwd")
        ct = c1.update(pt) + c1.final

        c2 = OpenSSL::Cipher::AES256.new(mode)
        c2.decrypt
        c2.pkcs5_keyivgen("passwd")
        assert_equal(pt, c2.update(ct) + c2.final)
      }
    end

    def test_AES_crush
      500.times do
        assert_nothing_raised("[Bug #2768]") do
          # it caused OpenSSL SEGV by uninitialized key
          OpenSSL::Cipher::AES128.new("ECB").update "." * 17
        end
      end
    end
  end
end

end
