require_relative 'utils'

class OpenSSL::TestHMAC < Test::Unit::TestCase
  def setup
    @digest = OpenSSL::Digest::MD5
    @key = "KEY"
    @data = "DATA"
    @h1 = OpenSSL::HMAC.new(@key, @digest.new)
    @h2 = OpenSSL::HMAC.new(@key, "MD5")
  end

  def teardown
  end

  def test_hmac
    @h1.update(@data)
    @h2.update(@data)
    assert_equal(@h1.digest, @h2.digest)

    assert_equal(OpenSSL::HMAC.digest(@digest.new, @key, @data), @h1.digest, "digest")
    assert_equal(OpenSSL::HMAC.hexdigest(@digest.new, @key, @data), @h1.hexdigest, "hexdigest")

    assert_equal(OpenSSL::HMAC.digest("MD5", @key, @data), @h2.digest, "digest")
    assert_equal(OpenSSL::HMAC.hexdigest("MD5", @key, @data), @h2.hexdigest, "hexdigest")
  end

  def test_dup
    @h1.update(@data)
    h = @h1.dup
    assert_equal(@h1.digest, h.digest, "dup digest")
  end
end if defined?(OpenSSL)
