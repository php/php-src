require_relative 'utils'

if defined?(OpenSSL)

class OpenSSL::TestBN < Test::Unit::TestCase
  def test_integer_to_bn
    assert_equal(999.to_bn, OpenSSL::BN.new(999.to_s(16), 16))
    assert_equal((2 ** 107 - 1).to_bn, OpenSSL::BN.new((2 ** 107 - 1).to_s(16), 16))
  end

  def test_prime_p
    assert_equal(true, OpenSSL::BN.new((2 ** 107 - 1).to_s(16), 16).prime?)
    assert_equal(true, OpenSSL::BN.new((2 ** 127 - 1).to_s(16), 16).prime?(1))
  end

  def test_cmp_nil
    bn = OpenSSL::BN.new('1')
    assert_equal(false, bn == nil)
    assert_equal(true,  bn != nil)
  end
end

end
