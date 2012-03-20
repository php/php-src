require 'test/unit'
require 'date'

class TestDateAttr < Test::Unit::TestCase

  def test__attr
    date = Date.new(1965, 5, 23)
    datetime = DateTime.new(1965, 5, 23, 22, 31, 59)

    [date, datetime].each_with_index do |d, i|
=begin
      if i == 0
	assert_match(/\#<Date\d?: 1965-05-23 \(4877807\/2,0,2299161\)>/,
		     d.inspect)
      else
	assert_match(/\#<DateTime\d?: 1965-05-23T22:31:59\+00:00 \(210721343519\/86400,0,2299161\)>/,
		     d.inspect)
      end
=end

      if i == 0
	assert_equal('1965-05-23', d.to_s)
      else
	assert_equal('1965-05-23T22:31:59+00:00', d.to_s)
      end

      assert_equal('', d.inspect.gsub!(/./,''))
      assert_equal('', d.to_s.gsub!(/./,''))

      assert_equal(2438904, d.jd)

      if i == 0
	assert_equal(0, d.day_fraction)
      else
	assert_equal(22.to_r/24 + 31.to_r/1440 + 59.to_r/86400, d.day_fraction)
      end

      assert_equal(38903, d.mjd)
      assert_equal(139744, d.ld)

      assert_equal(1965, d.year)
      assert_equal(143, d.yday)
      assert_equal(5, d.mon)
      assert_equal(d.mon, d.month)
      assert_equal(23, d.mday)
      assert_equal(d.mday, d.day)

      if i == 0
	assert_equal(false, d.respond_to?(:hour))
	assert_equal(false, d.respond_to?(:min))
	assert_equal(false, d.respond_to?(:sec))
	assert_equal(false, d.respond_to?(:sec_fraction))
	assert_equal(false, d.respond_to?(:zone))
	assert_equal(false, d.respond_to?(:offset))
      else
	assert_equal(22, d.hour)
	assert_equal(31, d.min)
	assert_equal(59, d.sec)
	assert_equal(0, d.sec_fraction)
	assert_equal('+00:00', d.zone)
	assert_equal(0, d.offset)
      end

      assert_equal(1965, d.cwyear)
      assert_equal(20, d.cweek)
      assert_equal(7, d.cwday)

      assert_equal(0, d.wday)
      assert_equal(false, d.leap?)
      assert_equal(false, d.julian?)
      assert_equal(true, d.gregorian?)

      assert_equal(Date::ITALY, d.start)
      assert_equal(d.start, d.start)
    end

    d = DateTime.new(1965, 5, 23, 22, 31, 59) + 1.to_r/(86400*2)
    assert_equal(1.to_r/2, d.sec_fraction)
  end

  def test__wday_predicate
    d = Date.new(2005, 10, 23)
    assert_equal(true, d.sunday?)
    assert_equal(false, d.monday?)
    assert_equal(false, d.tuesday?)
    assert_equal(false, d.wednesday?)
    assert_equal(false, d.thursday?)
    assert_equal(false, d.friday?)
    assert_equal(false, d.saturday?)

    d = Date.new(2005, 10, 30)
    14.times do |i|
      assert((d + i).__send__(%w(sunday? monday? tuesday? wednesday?
				 thursday? friday? saturday?)[i % 7]))
    end
  end

  def test_nth_kday
    skip unless Date.new.respond_to?(:nth_kday?, true)
    assert_equal(false, Date.new(2001,1,14).__send__(:nth_kday?, 1,0))
    assert_equal(true, Date.new(2001,1,14).__send__(:nth_kday?, 2,0))
    assert_equal(false, Date.new(2001,1,14).__send__(:nth_kday?, 3,0))
    assert_equal(false, Date.new(2001,1,14).__send__(:nth_kday?, 4,0))
    assert_equal(false, Date.new(2001,1,14).__send__(:nth_kday?, 5,0))
    assert_equal(false, Date.new(2001,1,14).__send__(:nth_kday?, -1,0))
    assert_equal(false, Date.new(2001,1,14).__send__(:nth_kday?, -2,0))
    assert_equal(true, Date.new(2001,1,14).__send__(:nth_kday?, -3,0))
    assert_equal(false, Date.new(2001,1,14).__send__(:nth_kday?, -4,0))
    assert_equal(false, Date.new(2001,1,14).__send__(:nth_kday?, -5,0))
  end

end
