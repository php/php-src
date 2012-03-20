require 'test/unit'

class Test_PathToClass < Test::Unit::TestCase
  require '-test-/path_to_class/path_to_class'

  def test_path_to_class
    bug5691 = '[ruby-core:41410]'
    assert_raise(ArgumentError, bug5691) {
      Test_PathToClass.path_to_class("Test_PathToClass::Object")
    }
  end
end
