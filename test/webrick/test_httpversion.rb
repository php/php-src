require "test/unit"
require "webrick/httpversion"

class TestWEBrickHTTPVersion < Test::Unit::TestCase
  def setup
    @v09 = WEBrick::HTTPVersion.new("0.9")
    @v10 = WEBrick::HTTPVersion.new("1.0")
    @v11 = WEBrick::HTTPVersion.new("1.001")
  end

  def test_to_s()
    assert_equal("0.9", @v09.to_s)
    assert_equal("1.0", @v10.to_s)
    assert_equal("1.1", @v11.to_s)
  end

  def test_major()
    assert_equal(0, @v09.major)
    assert_equal(1, @v10.major)
    assert_equal(1, @v11.major)
  end

  def test_minor()
    assert_equal(9, @v09.minor)
    assert_equal(0, @v10.minor)
    assert_equal(1, @v11.minor)
  end

  def test_compar()
    assert_equal(0, @v09 <=> "0.9")
    assert_equal(0, @v09 <=> "0.09")

    assert_equal(-1, @v09 <=> @v10)
    assert_equal(-1, @v09 <=> "1.00")

    assert_equal(1, @v11 <=> @v09)
    assert_equal(1, @v11 <=> "1.0")
    assert_equal(1, @v11 <=> "0.9")
  end
end
