require 'psych/helper'
require 'bigdecimal'

module Psych
  ###
  # Test numerics from YAML spec:
  # http://yaml.org/type/float.html
  # http://yaml.org/type/int.html
  class TestNumeric < TestCase
    def test_non_float_with_0
      str = Psych.load('--- 090')
      assert_equal '090', str
    end

    def test_big_decimal_tag
      decimal = BigDecimal("12.34")
      assert_match "!ruby/object:BigDecimal", Psych.dump(decimal)
    end

    def test_big_decimal_round_trip
      decimal = BigDecimal("12.34")
      assert_cycle decimal
    end
  end
end
