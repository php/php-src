require 'test/unit'

class TestD < Test::Unit::TestCase
  def test_fail_at_worker
    #if /test\/unit\/parallel\.rb/ =~ $0
    if on_parallel_worker?
      assert_equal(0,1)
    end
  end
end
