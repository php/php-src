require 'test/unit'
require 'yaml'

module Syck
  class TestString < Test::Unit::TestCase
    def test_binary_string_null
      string = "\x00"
      yml = YAML.dump string
      assert_match(/binary/, yml)
      assert_equal string, YAML.load(yml)
    end

    def test_binary_string
      string = binary_string
      yml = YAML.dump string
      assert_match(/binary/, yml)
      assert_equal string, YAML.load(yml)
    end

    def test_non_binary_string
      string = binary_string(0.29)
      yml = YAML.dump string
      assert_not_match(/binary/, yml)
      assert_equal string, YAML.load(yml)
    end

    def test_string_with_ivars
      food = "is delicious"
      ivar = "on rock and roll"
      food.instance_variable_set(:@we_built_this_city, ivar)

      str = YAML.load YAML.dump food
      assert_equal ivar, food.instance_variable_get(:@we_built_this_city)
    end

    def binary_string percentage = 0.31, length = 100
      string = ''
      (percentage * length).to_i.times do |i|
        string << "\b"
      end
      string << 'a' * (length - string.length)
      string
    end
  end
end
