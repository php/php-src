require 'test/unit'
require "-test-/string/string"

class Test_StringQSort < Test::Unit::TestCase
  def test_qsort
    s = Bug::String.new("xxozfxx")
    s.qsort!
    assert_equal("foxxxxz", s)
  end

  def test_qsort_slice
    s = Bug::String.new("xxofzx1")
    s.qsort!(nil, nil, 3)
    assert_equal("fzxxxo1", s)
    s = Bug::String.new("xxofzx231")
    s.qsort!(nil, nil, 3)
    assert_equal("231fzxxxo", s)
  end
end
