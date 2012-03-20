require 'test/unit'
require 'date'

class TestDateCompat < Test::Unit::TestCase

  def test_compat
    assert_equal(DateTime.new, Date.new)
    assert_equal(DateTime.new(2002,3,19), Date.new(2002,3,19))
    assert_equal(DateTime.new(2002,3,19, 0,0,0), Date.new(2002,3,19))
    assert_equal(DateTime.new(2002,3,19, 0,0,0, 0), Date.new(2002,3,19))
    assert_equal(DateTime.new(2002,3,19, 0,0,0, 0.to_r), Date.new(2002,3,19))
    assert_equal(DateTime.new(2002,3,19, 0,0,0, 0, Date::GREGORIAN), Date.new(2002,3,19, Date::GREGORIAN))
    assert_equal(DateTime.new(2002,3,19, 0,0,0, 0, Date::JULIAN), Date.new(2002,3,19, Date::JULIAN))

    assert(Date.new(2002,3,19) != DateTime.new(2002,3,19, 12,0,0))
    assert(Date.new(2002,3,19) != DateTime.new(2002,3,19, 0,0,1))
    assert(Date.new(2002,3,19) === DateTime.new(2002,3,19, 12,0,0))
    assert(Date.new(2002,3,19) === DateTime.new(2002,3,19, 0,0,1))
  end

end
