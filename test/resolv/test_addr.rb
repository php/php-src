require 'test/unit'
require 'resolv'
require 'socket'

class TestResolvAddr < Test::Unit::TestCase
  def test_invalid_ipv4_address
    assert_not_match(Resolv::IPv4::Regex, "1.2.3.256", "[ruby-core:29501]")
    1000.times {|i|
      if i < 256
        assert_match(Resolv::IPv4::Regex, "#{i}.#{i}.#{i}.#{i}")
      else
        assert_not_match(Resolv::IPv4::Regex, "#{i}.#{i}.#{i}.#{i}")
      end
    }
  end
end
