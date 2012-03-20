require 'psych/helper'
require 'date'

module Psych
  class TestDateTime < TestCase
    def test_string_tag
      dt = DateTime.now
      yaml = Psych.dump dt
      assert_match(/DateTime/, yaml)
    end

    def test_round_trip
      dt = DateTime.now
      assert_cycle dt
    end
  end
end
