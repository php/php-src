require 'psych/helper'

module Psych
  class TestSymbol < TestCase
    def test_cycle
      assert_cycle :a
    end

    def test_stringy
      assert_cycle :"1"
    end

    def test_load_quoted
      assert_equal :"1", Psych.load("--- :'1'\n")
    end
  end
end
