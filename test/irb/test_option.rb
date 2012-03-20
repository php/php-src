require 'test/unit'
require_relative '../ruby/envutil'

module TestIRB
  class TestOption < Test::Unit::TestCase
    def test_end_of_option
      bug4117 = '[ruby-core:33574]'
      status = assert_in_out_err(%w[-rirb -e IRB.start(__FILE__) -- -f --], "", //, [], bug4117)
      assert(status.success?, bug4117)
    end
  end
end
