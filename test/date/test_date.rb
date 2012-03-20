require 'test/unit'
require 'date'

class DateSub < Date; end
class DateTimeSub < DateTime; end

class TestDate < Test::Unit::TestCase

  def test__const
    assert_nil(Date::MONTHNAMES[0])
    assert_equal('January', Date::MONTHNAMES[1])
    assert_equal(13, Date::MONTHNAMES.size)
    assert_equal('Sunday', Date::DAYNAMES[0])
    assert_equal(7, Date::DAYNAMES.size)

    assert_nil(Date::ABBR_MONTHNAMES[0])
    assert_equal('Jan', Date::ABBR_MONTHNAMES[1])
    assert_equal(13, Date::ABBR_MONTHNAMES.size)
    assert_equal('Sun', Date::ABBR_DAYNAMES[0])
    assert_equal(7, Date::ABBR_DAYNAMES.size)

    assert(Date::MONTHNAMES.frozen?)
    assert(!Date::MONTHNAMES[0].frozen?)
    assert(Date::MONTHNAMES[1].frozen?)
    assert(Date::DAYNAMES.frozen?)
    assert(Date::DAYNAMES[0].frozen?)

    assert(Date::ABBR_MONTHNAMES.frozen?)
    assert(Date::ABBR_MONTHNAMES[1].frozen?)
    assert(Date::ABBR_DAYNAMES.frozen?)
    assert(Date::ABBR_DAYNAMES[0].frozen?)
  end

  def test_sub
    d = DateSub.new
    dt = DateTimeSub.new

    assert_instance_of(DateSub, d)
    assert_instance_of(DateTimeSub, dt)

    assert_instance_of(DateSub, DateSub.today)
    assert_instance_of(DateTimeSub, DateTimeSub.now)

#    assert_equal('#<DateSub: -4712-01-01 (-1/2,0,2299161)>', d.inspect)
    assert_equal('-4712-01-01', d.to_s)
#    assert_equal('#<DateTimeSub: -4712-01-01T00:00:00+00:00 (-1/2,0,2299161)>', dt.inspect)
    assert_equal('-4712-01-01T00:00:00+00:00', dt.to_s)

    d2 = d + 1
    assert_instance_of(DateSub, d2)
    d2 = d - 1
    assert_instance_of(DateSub, d2)
    d2 = d >> 1
    assert_instance_of(DateSub, d2)
    d2 = d << 1
    assert_instance_of(DateSub, d2)
    d2 = d.succ
    assert_instance_of(DateSub, d2)
    d2 = d.next
    assert_instance_of(DateSub, d2)
    d2 = d.italy
    assert_instance_of(DateSub, d2)
    d2 = d.england
    assert_instance_of(DateSub, d2)
    d2 = d.julian
    assert_instance_of(DateSub, d2)
    d2 = d.gregorian
    assert_instance_of(DateSub, d2)
    s = Marshal.dump(d)
    d2 = Marshal.load(s)
    assert_equal(d2, d)
    assert_instance_of(DateSub, d2)

    dt2 = dt + 1
    assert_instance_of(DateTimeSub, dt2)
    dt2 = dt - 1
    assert_instance_of(DateTimeSub, dt2)
    dt2 = dt >> 1
    assert_instance_of(DateTimeSub, dt2)
    dt2 = dt << 1
    assert_instance_of(DateTimeSub, dt2)
    dt2 = dt.succ
    assert_instance_of(DateTimeSub, dt2)
    dt2 = dt.next
    assert_instance_of(DateTimeSub, dt2)
    dt2 = dt.italy
    assert_instance_of(DateTimeSub, dt2)
    dt2 = dt.england
    assert_instance_of(DateTimeSub, dt2)
    dt2 = dt.julian
    assert_instance_of(DateTimeSub, dt2)
    dt2 = dt.gregorian
    assert_instance_of(DateTimeSub, dt2)
    s = Marshal.dump(dt)
    dt2 = Marshal.load(s)
    assert_equal(dt2, dt)
    assert_instance_of(DateTimeSub, dt2)
  end

  def test_eql_p
    d = Date.jd(0)
    d2 = Date.jd(0)
    dt = DateTime.jd(0)
    dt2 = DateTime.jd(0)

    assert_equal(d, d2)
    assert_not_equal(d, 0)

    assert_equal(dt, dt2)
    assert_not_equal(dt, 0)

    assert_equal(d, dt)
    assert_equal(d2, dt2)
  end

  def test_hash
    h = {}
    h[Date.new(1999,5,23)] = 0
    h[Date.new(1999,5,24)] = 1
    h[Date.new(1999,5,25)] = 2
    h[Date.new(1999,5,25)] = 9
    assert_equal(3, h.size)
    assert_equal(9, h[Date.new(1999,5,25)])
    assert_equal(9, h[DateTime.new(1999,5,25)])

    h = {}
    h[DateTime.new(1999,5,23)] = 0
    h[DateTime.new(1999,5,24)] = 1
    h[DateTime.new(1999,5,25)] = 2
    h[DateTime.new(1999,5,25)] = 9
    assert_equal(3, h.size)
    assert_equal(9, h[Date.new(1999,5,25)])
    assert_equal(9, h[DateTime.new(1999,5,25)])
  end

  def test_freeze
    d = Date.new
    d.freeze
    assert_equal(true, d.frozen?)
    assert_instance_of(Fixnum, d.yday)
    assert_instance_of(String, d.to_s)
  end

end
