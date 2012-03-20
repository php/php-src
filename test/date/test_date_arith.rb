require 'test/unit'
require 'date'

class TestDateArith < Test::Unit::TestCase

  def new_offset
    d = DateTime.new(2002, 3, 14)
    assert_equal(Rational(9, 24), d.new_offset(Rational(9, 24)).offset)
    assert_equal(Rational(9, 24), d.new_offset('+0900').offset)
  end

  def test__plus
    d = Date.new(2000,2,29) + -1
    assert_equal([2000, 2, 28], [d.year, d.mon, d.mday])
    d = Date.new(2000,2,29) + 0
    assert_equal([2000, 2, 29], [d.year, d.mon, d.mday])
    d = Date.new(2000,2,29) + 1
    assert_equal([2000, 3, 1], [d.year, d.mon, d.mday])

    d = DateTime.new(2000,2,29) + 1.to_r/2
    assert_equal([2000, 2, 29, 12, 0, 0],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec])
  end

  def test__plus__ex
    e = TypeError
    assert_raise(e) do
      Date.new(2000,2,29) + 'foo'
    end
    assert_raise(e) do
      DateTime.new(2000,2,29) + 'foo'
    end
    assert_raise(e) do
      Date.new(2000,2,29) + Time.mktime(2000,2,29)
    end
    assert_raise(e) do
      DateTime.new(2000,2,29) + Time.mktime(2000,2,29)
    end
  end

  def test__minus
    d = Date.new(2000,3,1) - -1
    assert_equal([2000, 3, 2], [d.year, d.mon, d.mday])
    d = Date.new(2000,3,1) - 0
    assert_equal([2000, 3, 1], [d.year, d.mon, d.mday])
    d = Date.new(2000,3,1) - 1
    assert_equal([2000, 2, 29], [d.year, d.mon, d.mday])

    d = Date.new(2000,3,1) - Date.new(2000,2,29)
    assert_equal(1, d)
    d = Date.new(2000,2,29) - Date.new(2000,3,1)
    assert_equal(-1, d)

    d = DateTime.new(2000,3,1) - 1.to_r/2
    assert_equal([2000, 2, 29, 12, 0, 0],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec])
  end

  def test__minus__ex
    e = TypeError
    assert_raise(e) do
      Date.new(2000,2,29) - 'foo'
    end
    assert_raise(e) do
      DateTime.new(2000,2,29) - 'foo'
    end
    assert_raise(e) do
      Date.new(2000,2,29) - Time.mktime(2000,2,29)
    end
    assert_raise(e) do
      DateTime.new(2000,2,29) - Time.mktime(2000,2,29)
    end
  end

  def test__compare
    assert_equal(0, (Date.new(2000,1,1) <=> Date.new(2000,1,1)))
    assert_equal(-1, (Date.new(2000,1,1) <=> Date.new(2000,1,2)))
    assert_equal(1, (Date.new(2000,1,2) <=> Date.new(2000,1,1)))
    assert_equal(0, (Date.new(2001,1,4,Date::JULIAN) <=>
		     Date.new(2001,1,17, Date::GREGORIAN)))
    assert_equal(0, (DateTime.new(2001,1,4,0,0,0,0,Date::JULIAN) <=>
		     DateTime.new(2001,1,17,0,0,0,0,Date::GREGORIAN)))
  end

  def test_prev
    d = Date.new(2000,1,1)
    assert_raise(NoMethodError) do
      d.prev
    end
  end

  def test_prev_day
    d = Date.new(2001,1,1).prev_day
    assert_equal([2000, 12, 31], [d.year, d.mon, d.mday])
    d = Date.new(2001,1,1).prev_day(2)
    assert_equal([2000, 12, 30], [d.year, d.mon, d.mday])
    d = Date.new(2000,12,31).prev_day(-2)
    assert_equal([2001, 1, 2], [d.year, d.mon, d.mday])

    d = DateTime.new(2000,3,1).prev_day(1.to_r/2)
    assert_equal([2000, 2, 29, 12, 0, 0], [d.year, d.mon, d.mday, d.hour, d.min, d.sec])
  end

  def test_prev_month
    d = Date.new(2000,1,31) << -1
    assert_equal([2000, 2, 29], [d.year, d.mon, d.mday])
    d = Date.new(2000,1,31) << 1
    assert_equal([1999, 12, 31], [d.year, d.mon, d.mday])
    d = Date.new(2000,1,31) << 12
    assert_equal([1999, 1, 31], [d.year, d.mon, d.mday])
    d = Date.new(2000,1,31) << 14
    assert_equal([1998, 11, 30], [d.year, d.mon, d.mday])

  end

  def test_prev_month__2
    d = Date.new(2000,1,31).prev_month(-1)
    assert_equal([2000, 2, 29], [d.year, d.mon, d.mday])
    d = Date.new(2000,1,31).prev_month
    assert_equal([1999, 12, 31], [d.year, d.mon, d.mday])
    d = Date.new(2000,1,31).prev_month(12)
    assert_equal([1999, 1, 31], [d.year, d.mon, d.mday])
    d = Date.new(2000,1,31).prev_month(14)
    assert_equal([1998, 11, 30], [d.year, d.mon, d.mday])
  end

  def test_prev_year
    d = Date.new(2000,1,31).prev_year(-1)
    assert_equal([2001, 1, 31], [d.year, d.mon, d.mday])
    d = Date.new(2000,1,31).prev_year
    assert_equal([1999, 1, 31], [d.year, d.mon, d.mday])
    d = Date.new(2000,1,31).prev_year(10)
    assert_equal([1990, 1, 31], [d.year, d.mon, d.mday])
    d = Date.new(2000,1,31).prev_year(100)
    assert_equal([1900, 1, 31], [d.year, d.mon, d.mday])
  end

  def test_next
    d = Date.new(2000,12,31).next
    assert_equal([2001, 1, 1], [d.year, d.mon, d.mday])
    d = Date.new(2000,12,31).succ
    assert_equal([2001, 1, 1], [d.year, d.mon, d.mday])

    d = Date.today
    d2 = d.next
    assert_equal(d, (d2 - 1))
    d = Date.today
    d2 = d.succ
    assert_equal(d, (d2 - 1))

    d = DateTime.now
    d2 = d.next
    assert_equal(d, (d2 - 1))
    d = DateTime.now
    d2 = d.succ
    assert_equal(d, (d2 - 1))
  end

  def test_next_day
    d = Date.new(2000,12,31).next_day
    assert_equal([2001, 1, 1], [d.year, d.mon, d.mday])
    d = Date.new(2000,12,31).next_day(2)
    assert_equal([2001, 1, 2], [d.year, d.mon, d.mday])
    d = Date.new(2001,1,1).next_day(-2)
    assert_equal([2000, 12, 30], [d.year, d.mon, d.mday])

    d = DateTime.new(2000,2,29).next_day(1.to_r/2)
    assert_equal([2000, 2, 29, 12, 0, 0], [d.year, d.mon, d.mday, d.hour, d.min, d.sec])
  end

  def test_next_month
    d = Date.new(2000,1,31) >> -1
    assert_equal([1999, 12, 31], [d.year, d.mon, d.mday])
    d = Date.new(2000,1,31) >> 1
    assert_equal([2000, 2, 29], [d.year, d.mon, d.mday])
    d = Date.new(2000,1,31) >> 12
    assert_equal([2001, 1, 31], [d.year, d.mon, d.mday])
    d = Date.new(2000,1,31) >> 13
    assert_equal([2001, 2, 28], [d.year, d.mon, d.mday])
  end

  def test_next_month__2
    d = Date.new(2000,1,31).next_month(-1)
    assert_equal([1999, 12, 31], [d.year, d.mon, d.mday])
    d = Date.new(2000,1,31).next_month
    assert_equal([2000, 2, 29], [d.year, d.mon, d.mday])
    d = Date.new(2000,1,31).next_month(12)
    assert_equal([2001, 1, 31], [d.year, d.mon, d.mday])
    d = Date.new(2000,1,31).next_month(13)
    assert_equal([2001, 2, 28], [d.year, d.mon, d.mday])
  end

  def test_next_year
    d = Date.new(2000,1,31).next_year(-1)
    assert_equal([1999, 1, 31], [d.year, d.mon, d.mday])
    d = Date.new(2000,1,31).next_year
    assert_equal([2001, 1, 31], [d.year, d.mon, d.mday])
    d = Date.new(2000,1,31).next_year(10)
    assert_equal([2010, 1, 31], [d.year, d.mon, d.mday])
    d = Date.new(2000,1,31).next_year(100)
    assert_equal([2100, 1, 31], [d.year, d.mon, d.mday])
  end

  def test_downto
    p = Date.new(2001,1,14)
    q = Date.new(2001,1,7)
    i = 0
    p.downto(q) do |d|
      i += 1
    end
    assert_equal(8, i)
  end

  def test_downto__noblock
    p = Date.new(2001,1,14)
    q = Date.new(2001,1,7)
    e = p.downto(q)
    assert_equal(8, e.to_a.size)
  end

  def test_upto
    p = Date.new(2001,1,14)
    q = Date.new(2001,1,21)
    i = 0
    p.upto(q) do |d|
      i += 1
    end
    assert_equal(8, i)
  end

  def test_upto__noblock
    p = Date.new(2001,1,14)
    q = Date.new(2001,1,21)
    e = p.upto(q)
    assert_equal(8, e.to_a.size)
  end

  def test_step
    p = Date.new(2001,1,14)
    q = Date.new(2001,1,21)
    i = 0
    p.step(q, 2) do |d|
      i += 1
    end
    assert_equal(4, i)

    i = 0
    p.step(q) do |d|
      i += 1
    end
    assert_equal(8, i)
  end

  def test_step__noblock
    p = Date.new(2001,1,14)
    q = Date.new(2001,1,21)
    e = p.step(q, 2)
    assert_equal(4, e.to_a.size)

    e = p.step(q)
    assert_equal(8, e.to_a.size)
  end

=begin
  def test_step__inf
    p = Date.new(2001,1,14)
    q = Date.new(2001,1,21)
    inf = 1.0/0

    if inf.infinite?
      [p, q].each do |a|
	[p, q].each do |b|
	  [inf, -inf].each do |c|
	    i = 0
	    a.step(b, c) do |d|
	      i += 1
	    end
	    assert_equal(0, i)
	  end
	end
      end
    end
  end
=end

end
