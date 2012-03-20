require 'test/unit'
require '-test-/array/resize'

class TestArray < Test::Unit::TestCase
  class TestResize < Test::Unit::TestCase
    def test_expand
      feature = '[ruby-dev:42912]'
      ary = [*1..10]
      ary.__resize__(10)
      assert_equal(10, ary.size, feature)
      assert_equal([*1..10], ary, feature)
      ary.__resize__(100)
      assert_equal(100, ary.size, feature)
      assert_equal([*1..10]+[nil]*90, ary, feature)
      ary.__resize__(20)
      assert_equal(20, ary.size, feature)
      assert_equal([*1..10]+[nil]*10, ary, feature)
      ary.__resize__(2)
      assert_equal(2, ary.size, feature)
      assert_equal([1,2], ary, feature)
      ary.__resize__(3)
      assert_equal(3, ary.size, feature)
      assert_equal([1,2,nil], ary, feature)
      ary.__resize__(10)
      assert_equal(10, ary.size, feature)
      assert_equal([1,2]+[nil]*8, ary, feature)
    end
  end
end
