require 'test/unit'
require "xmlrpc/datetime"

class Test_DateTime < Test::Unit::TestCase

  def test_new
    dt = createDateTime()

    assert_instance_of(XMLRPC::DateTime, dt)
  end

  def test_new_exception
    assert_raise(ArgumentError) { XMLRPC::DateTime.new(4.5, 13, 32, 25, 60, 60) }
    assert_raise(ArgumentError) { XMLRPC::DateTime.new(2001, 12, 32, 25, 60, 60) }
    assert_raise(ArgumentError) { XMLRPC::DateTime.new(2001, 12, 31, 25, 60, 60) }
    assert_raise(ArgumentError) { XMLRPC::DateTime.new(2001, 12, 31, 24, 60, 60) }
    assert_raise(ArgumentError) { XMLRPC::DateTime.new(2001, 12, 31, 24, 59, 60) }
    assert_nothing_raised(ArgumentError) { XMLRPC::DateTime.new(2001, 12, 31, 24, 59, 59) }

    assert_raise(ArgumentError) { XMLRPC::DateTime.new(2001, 0, 0, -1, -1, -1) }
    assert_raise(ArgumentError) { XMLRPC::DateTime.new(2001, 1, 0, -1, -1, -1) }
    assert_raise(ArgumentError) { XMLRPC::DateTime.new(2001, 1, 1, -1, -1, -1) }
    assert_raise(ArgumentError) { XMLRPC::DateTime.new(2001, 1, 1, 0, -1, -1) }
    assert_raise(ArgumentError) { XMLRPC::DateTime.new(2001, 1, 1, 0, 0, -1) }
    assert_nothing_raised(ArgumentError) { XMLRPC::DateTime.new(2001, 1, 1, 0, 0, 0) }
  end


  def test_get_values
    y, m, d, h, mi, s = 1970, 3, 24, 12, 0, 5
    dt = XMLRPC::DateTime.new(y, m, d, h, mi, s)

    assert_equal(y, dt.year)
    assert_equal(m, dt.month)
    assert_equal(m, dt.mon)
    assert_equal(d, dt.day)

    assert_equal(h, dt.hour)
    assert_equal(mi,dt.min)
    assert_equal(s, dt.sec)
  end

  def test_set_values
    dt = createDateTime()
    y, m, d, h, mi, s = 1950, 12, 9, 8, 52, 30

    dt.year  = y
    dt.month = m
    dt.day   = d
    dt.hour  = h
    dt.min   = mi
    dt.sec   = s

    assert_equal(y, dt.year)
    assert_equal(m, dt.month)
    assert_equal(m, dt.mon)
    assert_equal(d, dt.day)

    assert_equal(h, dt.hour)
    assert_equal(mi,dt.min)
    assert_equal(s, dt.sec)

    dt.mon = 5
    assert_equal(5, dt.month)
    assert_equal(5, dt.mon)
  end

  def test_set_exception
    dt = createDateTime()

    assert_raise(ArgumentError)    { dt.year = 4.5 }
    assert_nothing_raised(ArgumentError) { dt.year = -2000 }

    assert_raise(ArgumentError) { dt.month = 0 }
    assert_raise(ArgumentError) { dt.month = 13 }
    assert_nothing_raised(ArgumentError) { dt.month = 7 }

    assert_raise(ArgumentError) { dt.mon = 0 }
    assert_raise(ArgumentError) { dt.mon = 13 }
    assert_nothing_raised(ArgumentError) { dt.mon = 7 }

    assert_raise(ArgumentError) { dt.day = 0 }
    assert_raise(ArgumentError) { dt.day = 32 }
    assert_nothing_raised(ArgumentError) { dt.day = 16 }

    assert_raise(ArgumentError) { dt.hour = -1 }
    assert_raise(ArgumentError) { dt.hour = 25 }
    assert_nothing_raised(ArgumentError) { dt.hour = 12 }

    assert_raise(ArgumentError) { dt.min = -1 }
    assert_raise(ArgumentError) { dt.min = 60 }
    assert_nothing_raised(ArgumentError) { dt.min = 30 }

    assert_raise(ArgumentError) { dt.sec = -1 }
    assert_raise(ArgumentError) { dt.sec = 60 }
    assert_nothing_raised(ArgumentError) { dt.sec = 30 }
  end

  def test_to_a
    y, m, d, h, mi, s = 1970, 3, 24, 12, 0, 5
    dt = XMLRPC::DateTime.new(y, m, d, h, mi, s)
    a = dt.to_a

    assert_instance_of(Array, a)
    assert_equal(6,  a.size, "Returned array has wrong size")

    assert_equal(y,  a[0])
    assert_equal(m,  a[1])
    assert_equal(d,  a[2])
    assert_equal(h,  a[3])
    assert_equal(mi, a[4])
    assert_equal(s,  a[5])
  end

  def test_to_time1
    y, m, d, h, mi, s = 1970, 3, 24, 12, 0, 5
    dt = XMLRPC::DateTime.new(y, m, d, h, mi, s)
    time = dt.to_time

    assert_not_nil(time)

    assert_equal(y,  time.year)
    assert_equal(m,  time.month)
    assert_equal(d,  time.day)
    assert_equal(h,  time.hour)
    assert_equal(mi, time.min)
    assert_equal(s,  time.sec)
  end

  def test_to_time2
    dt = createDateTime()
    dt.year = 1969

    assert_nil(dt.to_time)
  end

  def test_to_date1
    y, m, d, h, mi, s = 1970, 3, 24, 12, 0, 5
    dt = XMLRPC::DateTime.new(y, m, d, h, mi, s)
    date = dt.to_date

    assert_equal(y, date.year)
    assert_equal(m, date.month)
    assert_equal(d, date.day)
  end

  def test_to_date2
    dt = createDateTime()
    dt.year = 666

    assert_equal(666, dt.to_date.year)
  end


  def createDateTime
    XMLRPC::DateTime.new(1970, 3, 24, 12, 0, 5)
  end

end
