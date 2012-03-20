require 'psych/helper'

module Psych
  class TestNil < TestCase
    def test_nil
      yml = Psych.dump nil
      assert_match(/--- \n(?:\.\.\.\n)?/, yml)
      assert_nil Psych.load(yml)
    end

    def test_array_nil
      yml = Psych.dump [nil]
      assert_equal "---\n- \n", yml
      assert_equal [nil], Psych.load(yml)
    end

  end
end
