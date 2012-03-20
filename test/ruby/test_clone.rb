require 'test/unit'

class TestClone < Test::Unit::TestCase
  module M001; end
  module M002; end
  module M003; include M002; end
  module M002; include M001; end
  module M003; include M002; end

  def test_clone
    foo = Object.new
    def foo.test
      "test"
    end
    bar = foo.clone
    def bar.test2
      "test2"
    end

    assert_equal("test2", bar.test2)
    assert_equal("test", bar.test)
    assert_equal("test", foo.test)

    assert_raise(NoMethodError) {foo.test2}

    assert_equal([M003, M002, M001], M003.ancestors)
  end
end
