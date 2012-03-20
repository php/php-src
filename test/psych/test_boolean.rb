require 'psych/helper'

module Psych
  ###
  # Test booleans from YAML spec:
  # http://yaml.org/type/bool.html
  class TestBoolean < TestCase
    %w{ yes Yes YES true True TRUE on On ON }.each do |truth|
      define_method(:"test_#{truth}") do
        assert_equal true, Psych.load("--- #{truth}")
      end
    end

    %w{ no No NO false False FALSE off Off OFF }.each do |truth|
      define_method(:"test_#{truth}") do
        assert_equal false, Psych.load("--- #{truth}")
      end
    end

    ###
    # YAML spec says "y" and "Y" may be used as true, but Syck treats them
    # as literal strings
    def test_y
      assert_equal "y", Psych.load("--- y")
      assert_equal "Y", Psych.load("--- Y")
    end

    ###
    # YAML spec says "n" and "N" may be used as false, but Syck treats them
    # as literal strings
    def test_n
      assert_equal "n", Psych.load("--- n")
      assert_equal "N", Psych.load("--- N")
    end
  end
end
