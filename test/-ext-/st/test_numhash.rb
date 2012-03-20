require 'test/unit'
require "-test-/st/numhash"

class Bug::StNumHash
  class Test_NumHash < Test::Unit::TestCase
    def setup
      @tbl = Bug::StNumHash.new
      5.times {|i| @tbl[i] = i}
    end

    def test_check
      keys = []
      @tbl.each do |k, v, t|
        keys << k
        t[5] = 5 if k == 3
        true
      end
      assert_equal([*0..5], keys)
    end

    def test_update
      assert_equal(true, @tbl.update(0) {@tbl[5] = :x})
      assert_equal(:x, @tbl[0])
      assert_equal(:x, @tbl[5])
    end

    def test_size_after_delete_safe
      10.downto(1) do |up|
        tbl = Bug::StNumHash.new
        1.upto(up){|i| tbl[i] = i}
        assert_equal(1, tbl.delete_safe(1))
        assert_equal(up - 1, tbl.size, "delete_safe doesn't change size from #{up} to #{up-1}")
      end
    end

    def test_delete_safe_on_iteration
      10.downto(1) do |up|
        tbl = Bug::StNumHash.new
        1.upto(up){|i| tbl[i] = i}
        assert_nothing_raised("delete_safe forces iteration to fail with size #{up}") do
          tbl.each do |k, v, t|
            assert_equal k, t.delete_safe(k)
            true
          end
        end
      end
    end
  end
end
