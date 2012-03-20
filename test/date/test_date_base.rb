require 'test/unit'
require 'date'

begin
  require 'calendar'
  include  Calendar
rescue LoadError
end

class TestDateBase < Test::Unit::TestCase

  def setup
    if defined?(Calendar)
      @from ||= julian_day_number_from_absolute(absolute_from_julian(1, 1, 1601))
      @to ||= julian_day_number_from_absolute(absolute_from_julian(12, 31, 2400))
      @from4t ||= julian_day_number_from_absolute(absolute_from_julian(1, 1, 1970))
      @to4t ||= julian_day_number_from_absolute(absolute_from_julian(12, 31, 2037))
    end
  end

  def test__inf
    assert_equal(0, Date::Infinity.new(-1) <=> Date::Infinity.new(-1))
    assert_equal(-1, Date::Infinity.new(-1) <=> Date::Infinity.new(+1))
    assert_equal(-1, Date::Infinity.new(-1) <=> 0)

    assert_equal(1, Date::Infinity.new(+1) <=> Date::Infinity.new(-1))
    assert_equal(0, Date::Infinity.new(+1) <=> Date::Infinity.new(+1))
    assert_equal(1, Date::Infinity.new(+1) <=> 0)

    assert_equal(1, 0 <=> Date::Infinity.new(-1))
    assert_equal(-1, 0 <=> Date::Infinity.new(+1))
    assert_equal(0, 0 <=> 0)

    assert_equal(0, Date::ITALY <=> Date::ITALY)
    assert_equal(-1, Date::ITALY <=> Date::ENGLAND)
    assert_equal(-1, Date::ITALY <=> Date::JULIAN)
    assert_equal(1, Date::ITALY <=> Date::GREGORIAN)

    assert_equal(1, Date::ENGLAND <=> Date::ITALY)
    assert_equal(0, Date::ENGLAND <=> Date::ENGLAND)
    assert_equal(-1, Date::ENGLAND <=> Date::JULIAN)
    assert_equal(1, Date::ENGLAND <=> Date::GREGORIAN)

    assert_equal(1, Date::JULIAN <=> Date::ITALY)
    assert_equal(1, Date::JULIAN <=> Date::ENGLAND)
    assert_equal(0, Date::JULIAN <=> Date::JULIAN)
    assert_equal(1, Date::JULIAN <=> Date::GREGORIAN)

    assert_equal(-1, Date::GREGORIAN <=> Date::ITALY)
    assert_equal(-1, Date::GREGORIAN <=> Date::ENGLAND)
    assert_equal(-1, Date::GREGORIAN <=> Date::JULIAN)
    assert_equal(0, Date::GREGORIAN <=> Date::GREGORIAN)
  end

  def test_ordinal__julian
    skip unless defined?(Calendar)
    for j in @from..@to
      m, d, y = julian_from_absolute(absolute_from_julian_day_number(j))
      j0 = julian_day_number_from_absolute(absolute_from_julian(12, 31, y - 1))
      j2 = julian_day_number_from_absolute(absolute_from_julian(m, d, y))
      assert_equal(j, j2)
      oy, od = Date.__send__(:jd_to_ordinal, j, Date::JULIAN)
      assert_equal(y, oy)
      assert_equal(j2 - j0, od)
      oj = Date.__send__(:ordinal_to_jd, oy, od, Date::JULIAN)
      assert_equal(j, oj)
    end
  end

  def test_ordinal__gregorian
    skip unless defined?(Calendar)
    for j in @from..@to
      m, d, y = gregorian_from_absolute(absolute_from_julian_day_number(j))
      j0 =
	julian_day_number_from_absolute(absolute_from_gregorian(12, 31, y - 1))
      j2 = julian_day_number_from_absolute(absolute_from_gregorian(m, d, y))
      assert_equal(j, j2)
      oy, od = Date.__send__(:jd_to_ordinal, j, Date::GREGORIAN)
      assert_equal(y, oy)
      assert_equal(j2 - j0, od)
      oj = Date.__send__(:ordinal_to_jd, oy, od, Date::GREGORIAN)
      assert_equal(j, oj)
    end
  end

  def test_civil__julian
    skip unless defined?(Calendar)
    for j in @from..@to
      m, d, y = julian_from_absolute(absolute_from_julian_day_number(j))
      j2 = julian_day_number_from_absolute(absolute_from_julian(m, d, y))
      assert_equal(j2, j)
      cy, cm, cd = Date.__send__(:jd_to_civil, j, Date::JULIAN)
      assert_equal(y, cy)
      assert_equal(m, cm)
      assert_equal(d, cd)
      cj = Date.__send__(:civil_to_jd, cy, cm, cd, Date::JULIAN)
      assert_equal(j, cj)
    end
  end

  def test_civil__gregorian
    skip unless defined?(Calendar)
    for j in @from..@to
      m, d, y = gregorian_from_absolute(absolute_from_julian_day_number(j))
      j2 = julian_day_number_from_absolute(absolute_from_gregorian(m, d, y))
      assert_equal(j2, j)
      cy, cm, cd = Date.__send__(:jd_to_civil, j, Date::GREGORIAN)
      assert_equal(y, cy)
      assert_equal(m, cm)
      assert_equal(d, cd)
      cj = Date.__send__(:civil_to_jd, cy, cm, cd, Date::GREGORIAN)
      assert_equal(j, cj)
    end
  end

  def test_commercial__gregorian
    skip unless defined?(Calendar)
    for j in @from..@to
      w, d, y = iso_from_absolute(absolute_from_julian_day_number(j))
      j2 = julian_day_number_from_absolute(absolute_from_iso(w, d, y))
      assert_equal(j2, j)
      cy, cw, cd = Date.__send__(:jd_to_commercial, j, Date::GREGORIAN)
      assert_equal(y, cy)
      assert_equal(w, cw)
      assert_equal(d, cd)
      cj = Date.__send__(:commercial_to_jd, cy, cw, cd, Date::GREGORIAN)
      assert_equal(j, cj)
    end
  end

  def test_weeknum
    skip unless defined?(Calendar)
    for j in @from..@to
      for k in 0..1
	wy, ww, wd = Date.__send__(:jd_to_weeknum, j, k, Date::GREGORIAN)
	wj = Date.__send__(:weeknum_to_jd, wy, ww, wd, k, Date::GREGORIAN)
	assert_equal(j, wj)
      end
    end
  end

  def test_weeknum__2
    skip unless defined?(Calendar)
    for j in @from4t..@to4t
      d = Date.jd(j)
      t = Time.mktime(d.year, d.mon, d.mday)
      [
       '%Y %U %w',
       '%Y %U %u',
       '%Y %W %w',
       '%Y %W %u'
      ].each do |fmt|
	s = t.strftime(fmt)
	d2 = Date.strptime(s, fmt)
	assert_equal(j, d2.jd)
      end
    end
  end

  def test_nth_kday
    skip unless defined?(Calendar)
    skip unless (Date.respond_to?(:nth_kday_to_jd, true) &&
		 Date.respond_to?(:jd_to_nth_kday, true))
    for y in 1601..2401
      for m in 1..12
	for n in -5..5
	  next if n == 0
	  for k in 0..6
	    j = julian_day_number_from_absolute(Nth_Kday(n, k, m, y))
	    j2 =  Date.__send__(:nth_kday_to_jd, y, m, n, k, Date::GREGORIAN)
	    assert_equal(j, j2)

	    d1 =  Date.__send__(:jd_to_nth_kday, j2, Date::GREGORIAN)
	    j3 =  Date.__send__(:nth_kday_to_jd, *d1)
	    assert_equal(j, j3)
	  end
	end
      end
    end
  end

  def test_jd
    assert_equal(1 << 33, Date.jd(1 << 33).jd)
  end

  def test_mjd
    skip unless Date.respond_to?(:mjd_to_jd, true)
    jd = Date.__send__(:mjd_to_jd, 51321)
    mjd = Date.__send__(:jd_to_mjd, jd)
    assert_equal(51321, mjd)
  end

  def test_ld
    skip unless Date.respond_to?(:ld_to_jd, true)
    jd = Date.__send__(:ld_to_jd, 152162)
    ld = Date.__send__(:jd_to_ld, jd)
    assert_equal(152162, ld)
  end

  def test_wday
    skip unless Date.respond_to?(:jd_to_wday, true)
    assert_equal(4, Date.__send__(:jd_to_wday, 3))
    assert_equal(3, Date.__send__(:jd_to_wday, 2))
    assert_equal(2, Date.__send__(:jd_to_wday, 1))
    assert_equal(1, Date.__send__(:jd_to_wday, 0))
    assert_equal(0, Date.__send__(:jd_to_wday, -1))
    assert_equal(6, Date.__send__(:jd_to_wday, -2))
    assert_equal(5, Date.__send__(:jd_to_wday, -3))
  end

  def test_leap?
    assert_equal(true, Date.julian_leap?(1900))
    assert_equal(false, Date.julian_leap?(1999))
    assert_equal(true, Date.julian_leap?(2000))

    assert_equal(false, Date.gregorian_leap?(1900))
    assert_equal(false, Date.gregorian_leap?(1999))
    assert_equal(true, Date.gregorian_leap?(2000))

    assert_equal(Date.leap?(1990), Date.gregorian_leap?(1900))
    assert_equal(Date.leap?(1999), Date.gregorian_leap?(1999))
    assert_equal(Date.leap?(2000), Date.gregorian_leap?(2000))
  end

  def test_valid_jd
    valid_jd_p =  :_valid_jd?
    skip unless Date.respond_to?(valid_jd_p, true)
    assert_equal(-1, Date.__send__(valid_jd_p, -1))
    assert_equal(0, Date.__send__(valid_jd_p, 0))
    assert_equal(1, Date.__send__(valid_jd_p, 1))
    assert_equal(2452348, Date.__send__(valid_jd_p, 2452348))
  end

  def test_valid_ordinal
    valid_ordinal_p = :_valid_ordinal?
    skip unless Date.respond_to?(valid_ordinal_p, true)
    assert_nil(Date.__send__(valid_ordinal_p, 1999,366))
    assert_equal(2451910, Date.__send__(valid_ordinal_p, 2000,366))
    assert_nil(Date.__send__(valid_ordinal_p, 1999,-366))
    assert_equal(2451545, Date.__send__(valid_ordinal_p, 2000,-366))
    assert_equal(2452275, Date.__send__(valid_ordinal_p, 2001,365))
    assert_nil(Date.__send__(valid_ordinal_p, 2001,366))
    assert_equal(Date.__send__(valid_ordinal_p, 2001,1),
		 Date.__send__(valid_ordinal_p, 2001,-365))
    assert_nil(Date.__send__(valid_ordinal_p, 2001,-366))
    assert_equal(2452348, Date.__send__(valid_ordinal_p, 2002,73))
  end

  def test_valid_ordinal__edge
    valid_ordinal_p = :_valid_ordinal?
    skip unless Date.respond_to?(valid_ordinal_p, true)
    (1601..2400).each do |y|
      d = if Date.leap?(y) then 366 else 365 end
      assert_not_nil(Date.__send__(valid_ordinal_p, y,d))
      assert_nil(Date.__send__(valid_ordinal_p, y,d + 1))
      assert_not_nil(Date.__send__(valid_ordinal_p, y,-d))
      assert_nil(Date.__send__(valid_ordinal_p, y,-(d + 1)))
    end
  end

  #        October 1582
  #   S   M  Tu   W  Th   F   S
  #     274 275 276 277 288 289
  # 290 291 292 293 294 295 296
  # 297 298 299 300 301 302 303
  # 304

  #        October 1582
  #   S   M  Tu   W  Th   F   S
  #     -92 -91 -90 -89 -78 -77
  # -76 -75 -74 -73 -72 -71 -70
  # -69 -68 -67 -66 -65 -64 -63
  # -62

  def test_valid_ordinal__italy
    valid_ordinal_p =  :_valid_ordinal?
    skip unless Date.respond_to?(valid_ordinal_p, true)
    (1..355).each do |d|
      assert_not_nil(Date.__send__(valid_ordinal_p, 1582,d,Date::ITALY))
    end
    (356..365).each do |d|
      assert_nil(Date.__send__(valid_ordinal_p, 1582,d,Date::ITALY))
    end
  end

  #       September 1752
  #   S   M  Tu   W  Th   F   S
  #         245 246 258 259 260
  # 261 262 263 264 265 266 267
  # 268 269 270 271 272 273 274

  def test_valid_ordinal__england
    valid_ordinal_p =  :_valid_ordinal?
    skip unless Date.respond_to?(valid_ordinal_p, true)
    (1..355).each do |d|
      assert_not_nil(Date.__send__(valid_ordinal_p, 1752,d,Date::ENGLAND))
    end
    (356..366).each do |d|
      assert_nil(Date.__send__(valid_ordinal_p, 1752,d,Date::ENGLAND))
    end
  end

  def test_valid_civil
    valid_civil_p = :_valid_civil?
    skip unless Date.respond_to?(valid_civil_p, true)
    assert_nil(Date.__send__(valid_civil_p, 1999,2,29))
    assert_equal(2451604, Date.__send__(valid_civil_p, 2000,2,29))
    assert_nil(Date.__send__(valid_civil_p, 1999,2,-29))
    assert_equal(2451576, Date.__send__(valid_civil_p, 2000,2,-29))
    assert_equal(2451941, Date.__send__(valid_civil_p, 2001,1,31))
    assert_nil(Date.__send__(valid_civil_p, 2001,1,32))
    assert_equal(Date.__send__(valid_civil_p, 2001,1,1),
		 Date.__send__(valid_civil_p, 2001,1,-31))
    assert_nil(Date.__send__(valid_civil_p, 2001,1,-32))
    assert_equal(2452348, Date.__send__(valid_civil_p, 2002,3,14))
    assert_nil(Date.__send__(valid_civil_p, 2010,-13,-1))
  end

  def test_valid_civil__edge
    valid_civil_p = :_valid_civil?
    skip unless Date.respond_to?(valid_civil_p, true)
    (1601..2400).each do |y|
      d = if Date.leap?(y) then 29 else 28 end
      assert_not_nil(Date.__send__(valid_civil_p, y,2,d))
      assert_nil(Date.__send__(valid_civil_p, y,2,d + 1))
      assert_not_nil(Date.__send__(valid_civil_p, y,2,-d))
      assert_nil(Date.__send__(valid_civil_p, y,2,-(d + 1)))
    end
  end

  #     October 1582
  #  S  M Tu  W Th  F  S
  #     1  2  3  4 15 16
  # 17 18 19 20 21 22 23
  # 24 25 26 27 28 29 30
  # 31

  def test_valid_civil__italy
    valid_civil_p = :_valid_civil?
    skip unless Date.respond_to?(valid_civil_p, true)
    (1..4).each do |d|
      assert_not_nil(Date.__send__(valid_civil_p, 1582,10,d,Date::ITALY))
    end
    (5..14).each do |d|
      assert_nil(Date.__send__(valid_civil_p, 1582,10,d,Date::ITALY))
    end
    (15..31).each do |d|
      assert_not_nil(Date.__send__(valid_civil_p, 1582,10,d,Date::ITALY))
    end
    (32..100).each do |d|
      assert_nil(Date.__send__(valid_civil_p, 1582,10,d,Date::ITALY))
    end
    (-31..-22).each do |d|
      assert_nil(Date.__send__(valid_civil_p, 1582,10,d,Date::ITALY))
    end
    (-21..-1).each do |d|
      assert_not_nil(Date.__send__(valid_civil_p, 1582,10,d,Date::ITALY))
    end
  end

  #    September 1752
  #  S  M Tu  W Th  F  S
  #        1  2 14 15 16
  # 17 18 19 20 21 22 23
  # 24 25 26 27 28 29 30

  def test_valid_civil__england
    valid_civil_p = :_valid_civil?
    skip unless Date.respond_to?(valid_civil_p, true)
    (1..2).each do |d|
      assert_not_nil(Date.__send__(valid_civil_p, 1752,9,d,Date::ENGLAND))
    end
    (3..13).each do |d|
      assert_nil(Date.__send__(valid_civil_p, 1752,9,d,Date::ENGLAND))
    end
    (14..30).each do |d|
      assert_not_nil(Date.__send__(valid_civil_p, 1752,9,d,Date::ENGLAND))
    end
    (31..100).each do |d|
      assert_nil(Date.__send__(valid_civil_p, 1752,9,d,Date::ENGLAND))
    end
    (-31..-20).each do |d|
      assert_nil(Date.__send__(valid_civil_p, 1752,9,d,Date::ENGLAND))
    end
    (-19..-1).each do |d|
      assert_not_nil(Date.__send__(valid_civil_p, 1752,9,d,Date::ENGLAND))
    end
  end

  def test_valid_commercial
    valid_commercial_p = :_valid_commercial?
    skip unless Date.respond_to?(valid_commercial_p, true)
    assert_nil(Date.__send__(valid_commercial_p, 1999,53,1))
    assert_equal(2453367, Date.__send__(valid_commercial_p, 2004,53,1))
    assert_nil(Date.__send__(valid_commercial_p, 1999,-53,-1))
    assert_equal(2453009, Date.__send__(valid_commercial_p, 2004,-53,-1))
    assert_equal(2452348, Date.__send__(valid_commercial_p, 2002,11,4))
  end

  def test_valid_weeknum
    valid_weeknum_p = :_valid_weeknum?
    skip unless Date.respond_to?(valid_weeknum_p, true)
    assert_nil(Date.__send__(valid_weeknum_p, 1999,53,0, 0))
    assert_equal(2454101, Date.__send__(valid_weeknum_p, 2006,53,0, 0))
    assert_nil(Date.__send__(valid_weeknum_p, 1999,-53,-1, 0))
    assert_equal(2453743, Date.__send__(valid_weeknum_p, 2006,-53,-1, 0))
    assert_equal(2452355, Date.__send__(valid_weeknum_p, 2002,11,4, 0))
    assert_nil(Date.__send__(valid_weeknum_p, 1999,53,0, 1))
    assert_equal(2454101, Date.__send__(valid_weeknum_p, 2006,52,6, 1))
    assert_nil(Date.__send__(valid_weeknum_p, 1999,-53,-1, 1))
    assert_equal(2453743, Date.__send__(valid_weeknum_p, 2006,-52,-2, 1))
    assert_equal(2452355, Date.__send__(valid_weeknum_p, 2002,11,3, 1))
  end

  def test_valid_nth_kday
    valid_nth_kday_p = :_valid_nth_kday?
    skip unless Date.respond_to?(valid_nth_kday_p, true)
    assert_nil(Date.__send__(valid_nth_kday_p, 1992,2, 5,0))
    assert_equal(2448682, Date.__send__(valid_nth_kday_p, 1992,2, 5,6))
    assert_equal(2448682, Date.__send__(valid_nth_kday_p, 1992,2, 5,-1))
    assert_equal(2448682, Date.__send__(valid_nth_kday_p, 1992,2, -1,6))
    assert_equal(2448682, Date.__send__(valid_nth_kday_p, 1992,2, -1,-1))
  end

  def test_valid_time
    valid_time_p = :_valid_time?
    skip unless Date.respond_to?(valid_time_p, true)
    assert_equal(Rational(0), DateTime.__send__(valid_time_p, 0,0,0))
    assert_nil(DateTime.__send__(valid_time_p, 25,59,59))
    assert_nil(DateTime.__send__(valid_time_p, 23,60,59))
    assert_nil(DateTime.__send__(valid_time_p, 23,59,60))
    assert_equal(Rational(86399, 86400),
		 DateTime.__send__(valid_time_p, 23,59,59))
    assert_equal(Rational(86399, 86400),
		 DateTime.__send__(valid_time_p, -1,-1,-1))
    assert_equal(Rational(1), DateTime.__send__(valid_time_p, 24,0,0))
    assert_nil(DateTime.__send__(valid_time_p, 24,0,1))
    assert_nil(DateTime.__send__(valid_time_p, 24,1,0))
    assert_nil(DateTime.__send__(valid_time_p, 24,1,1))
  end

end
