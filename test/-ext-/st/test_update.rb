require 'test/unit'
require "-test-/st/update"

class Bug::StTable
  class Test_Update < Test::Unit::TestCase
    def setup
      @tbl = Bug::StTable.new
      @tbl[:a] = 1
      @tbl[:b] = 2
    end

    def test_notfound
      called = false
      assert_equal(false, @tbl.st_update(:c) {called = true})
      assert_equal(false, called)
    end

    def test_continue
      args = nil
      assert_equal(true, @tbl.st_update(:a) {|*x| args = x; false})
      assert_equal({a: 1, b: 2}, @tbl, :a)
      assert_equal([:a, 1], args)
    end

    def test_delete
      args = nil
      assert_equal(true, @tbl.st_update(:a) {|*x| args = x; nil})
      assert_equal({b: 2}, @tbl, :a)
      assert_equal([:a, 1], args)
    end

    def test_update
      args = nil
      assert_equal(true, @tbl.st_update(:a) {|*x| args = x; 3})
      assert_equal({a: 3, b: 2}, @tbl, :a)
      assert_equal([:a, 1], args)
    end
  end
end
