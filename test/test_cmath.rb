require 'test/unit'
require 'cmath'

class TestCMath < Test::Unit::TestCase
  def test_sqrt
    assert_equal CMath.sqrt(1.0.i), CMath.sqrt(1.i), '[ruby-core:31672]'
  end

  def test_acos
    assert_in_delta CMath.acos(Complex(3, 4)), Complex(0.9368124611557199,-2.305509031243477)
  end

  def test_cbrt_returns_principal_value_of_cube_root
    assert_equal CMath.cbrt(-8), (-8)**(1.0/3), '#3676'
  end
end
