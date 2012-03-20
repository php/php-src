require 'net/smtp'
require 'minitest/autorun'

module Net
  class TestSMTP < MiniTest::Unit::TestCase
    def test_esmtp
      smtp = Net::SMTP.new 'localhost', 25
      assert smtp.esmtp
      assert smtp.esmtp?

      smtp.esmtp = 'omg'
      assert_equal 'omg', smtp.esmtp
      assert_equal 'omg', smtp.esmtp?
    end
  end
end
