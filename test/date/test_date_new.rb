require 'test/unit'
require 'date'

class TestDateNew < Test::Unit::TestCase

  def test_jd
    d = Date.jd
    dt = DateTime.jd
    assert_equal([-4712, 1, 1], [d.year, d.mon, d.mday])
    assert_equal([-4712, 1, 1], [dt.year, dt.mon, dt.mday])
    assert_equal([0, 0, 0], [dt.hour, dt.min, dt.sec])

    d2 = Date.jd
    dt2 = DateTime.jd
    assert_equal(d, d2)
    assert_equal(dt, dt2)

    d = Date.jd(0)
    assert_equal([-4712, 1, 1], [d.year, d.mon, d.mday])
    d = DateTime.jd(0, 0,0,0, 0)
    assert_equal([-4712, 1, 1, 0, 0, 0, 0],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec, d.offset])
    d = DateTime.jd(0, 0,0,0, '+0900')
    assert_equal([-4712, 1, 1, 0, 0, 0, 9.to_r/24],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec, d.offset])
  end

  def test_jd__ex
    assert_raise(ArgumentError) do
      DateTime.jd(0, 23,59,60,0)
    end
  end

  def test_ordinal
    d = Date.ordinal
    dt = DateTime.ordinal
    assert_equal([-4712, 1, 1], [d.year, d.mon, d.mday])
    assert_equal([-4712, 1, 1], [dt.year, dt.mon, dt.mday])
    assert_equal([0, 0, 0], [dt.hour, dt.min, dt.sec])

    d2 = Date.ordinal
    dt2 = DateTime.ordinal
    assert_equal(d, d2)
    assert_equal(dt, dt2)

    d = Date.ordinal(-4712,1)
    assert_equal([-4712, 1, 1], [d.year, d.mon, d.mday])

    d = Date.ordinal(-4712,1.0)
    assert_equal([-4712, 1, 1], [d.year, d.mon, d.mday])

    d = DateTime.ordinal(-4712,1, 0,0,0, 0)
    assert_equal([-4712, 1, 1, 0, 0, 0, 0],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec, d.offset])
    d = DateTime.ordinal(-4712,1, 0,0,0, '+0900')
    assert_equal([-4712, 1, 1, 0, 0, 0, 9.to_r/24],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec, d.offset])
  end

  def test_ordinal__neg
    d = Date.ordinal(-1,-1)
    assert_equal([-1, 365], [d.year, d.yday])

    d = DateTime.ordinal(-1,-1, -1,-1,-1, 0)
    assert_equal([-1, 365, 23, 59, 59, 0],
		 [d.year, d.yday, d.hour, d.min, d.sec, d.offset])
  end

  def test_ordinal__ex
    assert_raise(ArgumentError) do
      Date.ordinal(2001,366)
    end
    assert_raise(ArgumentError) do
      DateTime.ordinal(2001,365, 23,59,60, 0)
    end
  end

  def test_civil
    d = Date.civil
    dt = DateTime.civil
    assert_equal([-4712, 1, 1], [d.year, d.mon, d.mday])
    assert_equal([-4712, 1, 1], [dt.year, dt.mon, dt.mday])
    assert_equal([0, 0, 0], [dt.hour, dt.min, dt.sec])

    d2 = Date.civil
    dt2 = DateTime.civil
    assert_equal(d, d2)
    assert_equal(dt, dt2)

    d = Date.civil(-4712,1,1)
    assert_equal([-4712, 1, 1], [d.year, d.mon, d.mday])

    d = Date.civil(-4712,1,1.0)
    assert_equal([-4712, 1, 1], [d.year, d.mon, d.mday])

    d = DateTime.civil(-4712,1,1, 0,0,0, 0)
    assert_equal([-4712, 1, 1, 0, 0, 0, 0],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec, d.offset])
    d = DateTime.civil(-4712,1,1, 0,0,0, '+0900')
    assert_equal([-4712, 1, 1, 0, 0, 0, 9.to_r/24],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec, d.offset])


    d = DateTime.civil(2001,2,3 + 1.to_r/2)
    assert_equal([2001, 2, 3, 12, 0, 0, 0],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec, d.offset])
    d = DateTime.civil(2001,2,3, 4 + 1.to_r/2)
    assert_equal([2001, 2, 3, 4, 30, 0, 0],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec, d.offset])
    d = DateTime.civil(2001,2,3, 4,5 + 1.to_r/2)
    assert_equal([2001, 2, 3, 4, 5, 30, 0],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec, d.offset])
    d = DateTime.civil(2001,2,3, 4,5,6 + 1.to_r/2)
    assert_equal([2001, 2, 3, 4, 5, 6, 0],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec, d.offset])
    assert_equal(1.to_r/2, d.sec_fraction)
  end

  def test_civil__neg
    d = Date.civil(-1,-1,-1)
    assert_equal([-1, 12, 31], [d.year, d.mon, d.mday])

    d = DateTime.civil(-1,-1,-1, -1,-1,-1, 0)
    assert_equal([-1, 12, 31, 23, 59, 59, 0],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec, d.offset])
  end

  def test_civil__ex
    assert_raise(ArgumentError) do
      Date.civil(2001,2,29)
    end
    assert_raise(ArgumentError) do
      DateTime.civil(2001,2,28, 23,59,60, 0)
    end
    assert_raise(ArgumentError) do
      DateTime.civil(2001,2,28, 24,59,59, 0)
    end
  end

  def test_civil__reform
    d = Date.jd(Date::ENGLAND, Date::ENGLAND)
    dt = DateTime.jd(Date::ENGLAND, 0,0,0,0, Date::ENGLAND)
    assert_equal([1752, 9, 14], [d.year, d.mon, d.mday])
    assert_equal([1752, 9, 14], [dt.year, dt.mon, dt.mday])
    d -= 1
    dt -= 1
    assert_equal([1752, 9, 2], [d.year, d.mon, d.mday])
    assert_equal([1752, 9, 2], [dt.year, dt.mon, dt.mday])

    d = Date.jd(Date::ITALY, Date::ITALY)
    dt = DateTime.jd(Date::ITALY, 0,0,0,0, Date::ITALY)
    assert_equal([1582, 10, 15], [d.year, d.mon, d.mday])
    assert_equal([1582, 10, 15], [dt.year, dt.mon, dt.mday])
    d -= 1
    dt -= 1
    assert_equal([1582, 10, 4], [d.year, d.mon, d.mday])
    assert_equal([1582, 10, 4], [dt.year, dt.mon, dt.mday])
  end

  def test_commercial
    d = Date.commercial
    dt = DateTime.commercial
    assert_equal([-4712, 1, 1], [d.year, d.mon, d.mday])
    assert_equal([-4712, 1, 1], [dt.year, dt.mon, dt.mday])
    assert_equal([0, 0, 0], [dt.hour, dt.min, dt.sec])

    d2 = Date.commercial
    dt2 = DateTime.commercial
    assert_equal(d, d2)
    assert_equal(dt, dt2)

    d = Date.commercial(1582,40,5)
    assert_equal([1582, 10, 15], [d.year, d.mon, d.mday])

    d = Date.commercial(1582,40,5.0)
    assert_equal([1582, 10, 15], [d.year, d.mon, d.mday])

    d = DateTime.commercial(1582,40,5, 0,0,0, 0)
    assert_equal([1582, 10, 15, 0, 0, 0, 0],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec, d.offset])
    d = DateTime.commercial(1582,40,5, 0,0,0, '+0900')
    assert_equal([1582, 10, 15, 0, 0, 0, 9.to_r/24],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec, d.offset])
  end

  def test_commercial__neg
    d = Date.commercial(1998,-1,-1)
    assert_equal([1999, 1, 3], [d.year, d.mon, d.mday])

    d = DateTime.commercial(1998,-1,-1, -1,-1,-1, 0)
    assert_equal([1999, 1, 3, 23, 59, 59, 0],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec, d.offset])
  end

  def test_commercial__ex
    assert_raise(ArgumentError) do
      Date.commercial(1997,53,1)
    end
    assert_raise(ArgumentError) do
      DateTime.commercial(1997,52,1, 23,59,60, 0)
    end
  end

  def test_weeknum
    skip unless Date.respond_to?(:weeknum, true)
    d = Date.__send__(:weeknum)
    dt = DateTime.__send__(:weeknum)
    assert_equal([-4712, 1, 1], [d.year, d.mon, d.mday])
    assert_equal([-4712, 1, 1], [dt.year, dt.mon, dt.mday])
    assert_equal([0, 0, 0], [dt.hour, dt.min, dt.sec])

    d = Date.__send__(:weeknum, 2002,11,4, 0)
    assert_equal(2452355, d.jd)

    d = DateTime.__send__(:weeknum, 2002,11,4, 0, 11,22,33)
    assert_equal(2452355, d.jd)
    assert_equal([11,22,33], [d.hour, d.min, d.sec])

    assert_raise(ArgumentError) do
      Date.__send__(:weeknum, 1999,53,0, 0)
    end
    assert_raise(ArgumentError) do
      Date.__send__(:weeknum, 1999,-53,-1, 0)
    end
  end

  def test_nth_kday
    skip unless Date.respond_to?(:nth_kday, true)
    d = Date.__send__(:nth_kday)
    dt = DateTime.__send__(:nth_kday)
    assert_equal([-4712, 1, 1], [d.year, d.mon, d.mday])
    assert_equal([-4712, 1, 1], [dt.year, dt.mon, dt.mday])
    assert_equal([0, 0, 0], [dt.hour, dt.min, dt.sec])

    d = Date.__send__(:nth_kday, 1992,2, 5,6)
    assert_equal(2448682, d.jd)

    d = DateTime.__send__(:nth_kday, 1992,2, 5,6, 11,22,33)
    assert_equal(2448682, d.jd)
    assert_equal([11,22,33], [d.hour, d.min, d.sec])

    assert_raise(ArgumentError) do
      Date.__send__(:nth_kday, 2006,5, 5,0)
    end
    assert_raise(ArgumentError) do
      Date.__send__(:nth_kday, 2006,5, -5,0)
    end
  end

  def test_today
    z = Time.now
    d = Date.today
    t = Time.now
    t2 = Time.utc(t.year, t.mon, t.mday)
    t3 = Time.utc(d.year, d.mon, d.mday)
    assert_in_delta(t2, t3, t - z + 2)

    assert_equal(false, DateTime.respond_to?(:today))
  end

  def test_now
    assert_equal(false, Date.respond_to?(:now))

    z = Time.now
    d = DateTime.now
    t = Time.now
    t2 = Time.local(d.year, d.mon, d.mday, d.hour, d.min, d.sec)
    assert_in_delta(t, t2, t - z + 2)
  end

end
