require 'test/unit'
require 'date'

class TestDateStrptime < Test::Unit::TestCase

  STRFTIME_2001_02_03 = {
    '%A'=>['Saturday',{:wday=>6}],
    '%a'=>['Sat',{:wday=>6}],
    '%B'=>['February',{:mon=>2}],
    '%b'=>['Feb',{:mon=>2}],
    '%c'=>['Sat Feb  3 00:00:00 2001',
      {:wday=>6,:mon=>2,:mday=>3,:hour=>0,:min=>0,:sec=>0,:year=>2001}],
    '%d'=>['03',{:mday=>3}],
    '%e'=>[' 3',{:mday=>3}],
    '%H'=>['00',{:hour=>0}],
    '%I'=>['12',{:hour=>0}],
    '%j'=>['034',{:yday=>34}],
    '%M'=>['00',{:min=>0}],
    '%m'=>['02',{:mon=>2}],
    '%p'=>['AM',{}],
    '%S'=>['00',{:sec=>0}],
    '%U'=>['04',{:wnum0=>4}],
    '%W'=>['05',{:wnum1=>5}],
    '%X'=>['00:00:00',{:hour=>0,:min=>0,:sec=>0}],
    '%x'=>['02/03/01',{:mon=>2,:mday=>3,:year=>2001}],
    '%Y'=>['2001',{:year=>2001}],
    '%y'=>['01',{:year=>2001}],
    '%Z'=>['+00:00',{:zone=>'+00:00',:offset=>0}],
    '%%'=>['%',{}],
    '%C'=>['20',{}],
    '%D'=>['02/03/01',{:mon=>2,:mday=>3,:year=>2001}],
    '%F'=>['2001-02-03',{:year=>2001,:mon=>2,:mday=>3}],
    '%G'=>['2001',{:cwyear=>2001}],
    '%g'=>['01',{:cwyear=>2001}],
    '%h'=>['Feb',{:mon=>2}],
    '%k'=>[' 0',{:hour=>0}],
    '%L'=>['000',{:sec_fraction=>0}],
    '%l'=>['12',{:hour=>0}],
    '%N'=>['000000000',{:sec_fraction=>0}],
    '%n'=>["\n",{}],
    '%P'=>['am',{}],
    '%Q'=>['981158400000',{:seconds=>981158400.to_r}],
    '%R'=>['00:00',{:hour=>0,:min=>0}],
    '%r'=>['12:00:00 AM',{:hour=>0,:min=>0,:sec=>0}],
    '%s'=>['981158400',{:seconds=>981158400}],
    '%T'=>['00:00:00',{:hour=>0,:min=>0,:sec=>0}],
    '%t'=>["\t",{}],
    '%u'=>['6',{:cwday=>6}],
    '%V'=>['05',{:cweek=>5}],
    '%v'=>[' 3-Feb-2001',{:mday=>3,:mon=>2,:year=>2001}],
    '%z'=>['+0000',{:zone=>'+0000',:offset=>0}],
    '%+'=>['Sat Feb  3 00:00:00 +00:00 2001',
      {:wday=>6,:mon=>2,:mday=>3,
	:hour=>0,:min=>0,:sec=>0,:zone=>'+00:00',:offset=>0,:year=>2001}],
  }

  STRFTIME_2001_02_03_CVS19 = {
  }

  STRFTIME_2001_02_03_GNUext = {
    '%:z'=>['+00:00',{:zone=>'+00:00',:offset=>0}],
    '%::z'=>['+00:00:00',{:zone=>'+00:00:00',:offset=>0}],
    '%:::z'=>['+00',{:zone=>'+00',:offset=>0}],
  }

  STRFTIME_2001_02_03.update(STRFTIME_2001_02_03_CVS19)
  STRFTIME_2001_02_03.update(STRFTIME_2001_02_03_GNUext)

  def test__strptime
    STRFTIME_2001_02_03.each do |f, s|
      if (f == '%I' and s[0] == '12') or
	 (f == '%l' and s[0] == '12') # hour w/o merid
	s[1][:hour] = 12
      end
      assert_equal(s[1], Date._strptime(s[0], f), [f, s].inspect)
      case f[-1,1]
      when 'c', 'C', 'x', 'X', 'y', 'Y'
	f2 = f.sub(/\A%/, '%E')
	assert_equal(s[1], Date._strptime(s[0], f2), [f2, s].inspect)
      else
	f2 = f.sub(/\A%/, '%E')
	assert_equal(nil, Date._strptime(s[0], f2), [f2, s].inspect)
	assert_equal({}, Date._strptime(f2, f2), [f2, s].inspect)
      end
      case f[-1,1]
      when 'd', 'e', 'H', 'I', 'm', 'M', 'S', 'u', 'U', 'V', 'w', 'W', 'y'
	f2 = f.sub(/\A%/, '%O')
	assert_equal(s[1], Date._strptime(s[0], f2), [f2, s].inspect)
      else
	f2 = f.sub(/\A%/, '%O')
	assert_equal(nil, Date._strptime(s[0], f2), [f2, s].inspect)
	assert_equal({}, Date._strptime(f2, f2), [f2, s].inspect)
      end
    end
  end

  def test__strptime__2
    h = Date._strptime('2001-02-03')
    assert_equal([2001,2,3], h.values_at(:year,:mon,:mday))

    h = DateTime._strptime('2001-02-03T12:13:14Z')
    assert_equal([2001,2,3,12,13,14],
		 h.values_at(:year,:mon,:mday,:hour,:min,:sec))

    assert_equal({}, Date._strptime('', ''))
    assert_equal({:leftover=>"\s"*3}, Date._strptime("\s"*3, ''))
    assert_equal({:leftover=>'x'}, Date._strptime("\nx", "\n"))
    assert_equal({}, Date._strptime('', "\s"*3))
    assert_equal({}, Date._strptime("\s"*3, "\s"*3))
    assert_equal({}, Date._strptime("\tfoo\n\000\r", "\tfoo\n\000\r"))
    assert_equal({}, Date._strptime("foo\n\nbar", "foo\sbar"))
    assert_equal({}, Date._strptime("%\n", "%\n")) # gnu
    assert_equal({}, Date._strptime('%%', '%%%'))
    assert_equal({:wday=>6}, Date._strptime('Saturday'*1024 + ',', '%A'*1024 + ','))
    assert_equal({:wday=>6}, Date._strptime('Saturday'*1024 + ',', '%a'*1024 + ','))
    assert_equal({}, Date._strptime('Anton von Webern', 'Anton von Webern'))
  end

  def test__strptime__3
    [
     # iso8601
     [['2001-02-03', '%Y-%m-%d'], [2001,2,3,nil,nil,nil,nil,nil,nil]],
     [['2001-02-03T23:59:60', '%Y-%m-%dT%H:%M:%S'], [2001,2,3,23,59,60,nil,nil,nil]],
     [['2001-02-03T23:59:60+09:00', '%Y-%m-%dT%H:%M:%S%Z'], [2001,2,3,23,59,60,'+09:00',9*3600,nil]],
     [['-2001-02-03T23:59:60+09:00', '%Y-%m-%dT%H:%M:%S%Z'], [-2001,2,3,23,59,60,'+09:00',9*3600,nil]],
     [['+012345-02-03T23:59:60+09:00', '%Y-%m-%dT%H:%M:%S%Z'], [12345,2,3,23,59,60,'+09:00',9*3600,nil]],
     [['-012345-02-03T23:59:60+09:00', '%Y-%m-%dT%H:%M:%S%Z'], [-12345,2,3,23,59,60,'+09:00',9*3600,nil]],

     # ctime(3), asctime(3)
     [['Thu Jul 29 14:47:19 1999', '%c'], [1999,7,29,14,47,19,nil,nil,4]],
     [['Thu Jul 29 14:47:19 -1999', '%c'], [-1999,7,29,14,47,19,nil,nil,4]],

     # date(1)
     [['Thu Jul 29 16:39:41 EST 1999', '%a %b %d %H:%M:%S %Z %Y'], [1999,7,29,16,39,41,'EST',-5*3600,4]],
     [['Thu Jul 29 16:39:41 MET DST 1999', '%a %b %d %H:%M:%S %Z %Y'], [1999,7,29,16,39,41,'MET DST',2*3600,4]],
     [['Thu Jul 29 16:39:41 AMT 1999', '%a %b %d %H:%M:%S %Z %Y'], [1999,7,29,16,39,41,'AMT',nil,4]],
     [['Thu Jul 29 16:39:41 AMT -1999', '%a %b %d %H:%M:%S %Z %Y'], [-1999,7,29,16,39,41,'AMT',nil,4]],
     [['Thu Jul 29 16:39:41 GMT+09 1999', '%a %b %d %H:%M:%S %Z %Y'], [1999,7,29,16,39,41,'GMT+09',9*3600,4]],
     [['Thu Jul 29 16:39:41 GMT+0908 1999', '%a %b %d %H:%M:%S %Z %Y'], [1999,7,29,16,39,41,'GMT+0908',9*3600+8*60,4]],
     [['Thu Jul 29 16:39:41 GMT+090807 1999', '%a %b %d %H:%M:%S %Z %Y'], [1999,7,29,16,39,41,'GMT+090807',9*3600+8*60+7,4]],
     [['Thu Jul 29 16:39:41 GMT-09 1999', '%a %b %d %H:%M:%S %Z %Y'], [1999,7,29,16,39,41,'GMT-09',-9*3600,4]],
     [['Thu Jul 29 16:39:41 GMT-09:08 1999', '%a %b %d %H:%M:%S %Z %Y'], [1999,7,29,16,39,41,'GMT-09:08',-9*3600-8*60,4]],
     [['Thu Jul 29 16:39:41 GMT-09:08:07 1999', '%a %b %d %H:%M:%S %Z %Y'], [1999,7,29,16,39,41,'GMT-09:08:07',-9*3600-8*60-7,4]],
     [['Thu Jul 29 16:39:41 GMT-3.5 1999', '%a %b %d %H:%M:%S %Z %Y'], [1999,7,29,16,39,41,'GMT-3.5',-3*3600-30*60,4]],
     [['Thu Jul 29 16:39:41 GMT-3,5 1999', '%a %b %d %H:%M:%S %Z %Y'], [1999,7,29,16,39,41,'GMT-3,5',-3*3600-30*60,4]],
     [['Thu Jul 29 16:39:41 Mountain Daylight Time 1999', '%a %b %d %H:%M:%S %Z %Y'], [1999,7,29,16,39,41,'Mountain Daylight Time',-6*3600,4]],
     [['Thu Jul 29 16:39:41 E. Australia Standard Time 1999', '%a %b %d %H:%M:%S %Z %Y'], [1999,7,29,16,39,41,'E. Australia Standard Time',10*3600,4]],

     # rfc822
     [['Thu, 29 Jul 1999 09:54:21 UT', '%a, %d %b %Y %H:%M:%S %Z'], [1999,7,29,9,54,21,'UT',0,4]],
     [['Thu, 29 Jul 1999 09:54:21 GMT', '%a, %d %b %Y %H:%M:%S %Z'], [1999,7,29,9,54,21,'GMT',0,4]],
     [['Thu, 29 Jul 1999 09:54:21 PDT', '%a, %d %b %Y %H:%M:%S %Z'], [1999,7,29,9,54,21,'PDT',-7*3600,4]],
     [['Thu, 29 Jul 1999 09:54:21 z', '%a, %d %b %Y %H:%M:%S %Z'], [1999,7,29,9,54,21,'z',0,4]],
     [['Thu, 29 Jul 1999 09:54:21 +0900', '%a, %d %b %Y %H:%M:%S %Z'], [1999,7,29,9,54,21,'+0900',9*3600,4]],
     [['Thu, 29 Jul 1999 09:54:21 +0430', '%a, %d %b %Y %H:%M:%S %Z'], [1999,7,29,9,54,21,'+0430',4*3600+30*60,4]],
     [['Thu, 29 Jul 1999 09:54:21 -0430', '%a, %d %b %Y %H:%M:%S %Z'], [1999,7,29,9,54,21,'-0430',-4*3600-30*60,4]],
     [['Thu, 29 Jul -1999 09:54:21 -0430', '%a, %d %b %Y %H:%M:%S %Z'], [-1999,7,29,9,54,21,'-0430',-4*3600-30*60,4]],

     # etc
     [['06-DEC-99', '%d-%b-%y'], [1999,12,6,nil,nil,nil,nil,nil,nil]],
     [['sUnDay oCtoBer 31 01', '%A %B %d %y'], [2001,10,31,nil,nil,nil,nil,nil,0]],
     [["October\t\n\v\f\r 15,\t\n\v\f\r99", '%B %d, %y'], [1999,10,15,nil,nil,nil,nil,nil,nil]],
     [["October\t\n\v\f\r 15,\t\n\v\f\r99", '%B%t%d,%n%y'], [1999,10,15,nil,nil,nil,nil,nil,nil]],

     [['09:02:11 AM', '%I:%M:%S %p'], [nil,nil,nil,9,2,11,nil,nil,nil]],
     [['09:02:11 A.M.', '%I:%M:%S %p'], [nil,nil,nil,9,2,11,nil,nil,nil]],
     [['09:02:11 PM', '%I:%M:%S %p'], [nil,nil,nil,21,2,11,nil,nil,nil]],
     [['09:02:11 P.M.', '%I:%M:%S %p'], [nil,nil,nil,21,2,11,nil,nil,nil]],

     [['12:33:44 AM', '%r'], [nil,nil,nil,0,33,44,nil,nil,nil]],
     [['01:33:44 AM', '%r'], [nil,nil,nil,1,33,44,nil,nil,nil]],
     [['11:33:44 AM', '%r'], [nil,nil,nil,11,33,44,nil,nil,nil]],
     [['12:33:44 PM', '%r'], [nil,nil,nil,12,33,44,nil,nil,nil]],
     [['01:33:44 PM', '%r'], [nil,nil,nil,13,33,44,nil,nil,nil]],
     [['11:33:44 PM', '%r'], [nil,nil,nil,23,33,44,nil,nil,nil]],

     [['11:33:44 PM AMT', '%I:%M:%S %p %Z'], [nil,nil,nil,23,33,44,'AMT',nil,nil]],
     [['11:33:44 P.M. AMT', '%I:%M:%S %p %Z'], [nil,nil,nil,23,33,44,'AMT',nil,nil]],

     [['fri1feb034pm+5', '%a%d%b%y%H%p%Z'], [2003,2,1,16,nil,nil,'+5',5*3600,5]]
    ].each do |x, y|
      h = Date._strptime(*x)
      a = h.values_at(:year,:mon,:mday,:hour,:min,:sec,:zone,:offset,:wday)
      if y[1] == -1
	a[1] = -1
	a[2] = h[:yday]
      end
      assert_equal(y, a, [x, y, a].inspect)
    end
  end

  def test__strptime__width
    [
     [['99', '%y'], [1999,nil,nil,nil,nil,nil,nil,nil,nil]],
     [['01', '%y'], [2001,nil,nil,nil,nil,nil,nil,nil,nil]],
     [['19 99', '%C %y'], [1999,nil,nil,nil,nil,nil,nil,nil,nil]],
     [['20 01', '%C %y'], [2001,nil,nil,nil,nil,nil,nil,nil,nil]],
     [['1999', '%C%y'], [1999,nil,nil,nil,nil,nil,nil,nil,nil]],
     [['2001', '%C%y'], [2001,nil,nil,nil,nil,nil,nil,nil,nil]],

     [['20060806', '%Y'], [20060806,nil,nil,nil,nil,nil,nil,nil,nil]],
     [['20060806', "%Y\s"], [20060806,nil,nil,nil,nil,nil,nil,nil,nil]],
     [['20060806', '%Y%m%d'], [2006,8,6,nil,nil,nil,nil,nil,nil]],
     [['2006908906', '%Y9%m9%d'], [2006,8,6,nil,nil,nil,nil,nil,nil]],
     [['12006 08 06', '%Y %m %d'], [12006,8,6,nil,nil,nil,nil,nil,nil]],
     [['12006-08-06', '%Y-%m-%d'], [12006,8,6,nil,nil,nil,nil,nil,nil]],
     [['200608 6', '%Y%m%e'], [2006,8,6,nil,nil,nil,nil,nil,nil]],

     [['2006333', '%Y%j'], [2006,-1,333,nil,nil,nil,nil,nil,nil]],
     [['20069333', '%Y9%j'], [2006,-1,333,nil,nil,nil,nil,nil,nil]],
     [['12006 333', '%Y %j'], [12006,-1,333,nil,nil,nil,nil,nil,nil]],
     [['12006-333', '%Y-%j'], [12006,-1,333,nil,nil,nil,nil,nil,nil]],

     [['232425', '%H%M%S'], [nil,nil,nil,23,24,25,nil,nil,nil]],
     [['23924925', '%H9%M9%S'], [nil,nil,nil,23,24,25,nil,nil,nil]],
     [['23 24 25', '%H %M %S'], [nil,nil,nil,23,24,25,nil,nil,nil]],
     [['23:24:25', '%H:%M:%S'], [nil,nil,nil,23,24,25,nil,nil,nil]],
     [[' 32425', '%k%M%S'], [nil,nil,nil,3,24,25,nil,nil,nil]],
     [[' 32425', '%l%M%S'], [nil,nil,nil,3,24,25,nil,nil,nil]],

     [['FriAug', '%a%b'], [nil,8,nil,nil,nil,nil,nil,nil,5]],
     [['FriAug', '%A%B'], [nil,8,nil,nil,nil,nil,nil,nil,5]],
     [['FridayAugust', '%A%B'], [nil,8,nil,nil,nil,nil,nil,nil,5]],
     [['FridayAugust', '%a%b'], [nil,8,nil,nil,nil,nil,nil,nil,5]]
    ].each do |x, y|
      h = Date._strptime(*x)
      a = h.values_at(:year,:mon,:mday,:hour,:min,:sec,:zone,:offset,:wday)
      if y[1] == -1
	a[1] = -1
	a[2] = h[:yday]
      end
      assert_equal(y, a, [x, y, a].inspect)
    end
  end

  def test__strptime__fail
    assert_not_nil(Date._strptime('2001.', '%Y.'))
    assert_not_nil(Date._strptime("2001.\s", '%Y.'))
    assert_not_nil(Date._strptime('2001.', "%Y.\s"))
    assert_not_nil(Date._strptime("2001.\s", "%Y.\s"))

    assert_nil(Date._strptime('2001', '%Y.'))
    assert_nil(Date._strptime("2001\s", '%Y.'))
    assert_nil(Date._strptime('2001', "%Y.\s"))
    assert_nil(Date._strptime("2001\s", "%Y.\s"))

    assert_nil(Date._strptime('2001-13-31', '%Y-%m-%d'))
    assert_nil(Date._strptime('2001-12-00', '%Y-%m-%d'))
    assert_nil(Date._strptime('2001-12-32', '%Y-%m-%d'))
    assert_nil(Date._strptime('2001-12-00', '%Y-%m-%e'))
    assert_nil(Date._strptime('2001-12-32', '%Y-%m-%e'))
    assert_nil(Date._strptime('2001-12-31', '%y-%m-%d'))

    assert_nil(Date._strptime('2004-000', '%Y-%j'))
    assert_nil(Date._strptime('2004-367', '%Y-%j'))
    assert_nil(Date._strptime('2004-366', '%y-%j'))

    assert_not_nil(Date._strptime('24:59:59', '%H:%M:%S'))
    assert_not_nil(Date._strptime('24:59:59', '%k:%M:%S'))
    assert_not_nil(Date._strptime('24:59:60', '%H:%M:%S'))
    assert_not_nil(Date._strptime('24:59:60', '%k:%M:%S'))

    assert_nil(Date._strptime('24:60:59', '%H:%M:%S'))
    assert_nil(Date._strptime('24:60:59', '%k:%M:%S'))
    assert_nil(Date._strptime('24:59:61', '%H:%M:%S'))
    assert_nil(Date._strptime('24:59:61', '%k:%M:%S'))
    assert_nil(Date._strptime('00:59:59', '%I:%M:%S'))
    assert_nil(Date._strptime('13:59:59', '%I:%M:%S'))
    assert_nil(Date._strptime('00:59:59', '%l:%M:%S'))
    assert_nil(Date._strptime('13:59:59', '%l:%M:%S'))

    assert_not_nil(Date._strptime('0', '%U'))
    assert_nil(Date._strptime('54', '%U'))
    assert_not_nil(Date._strptime('0', '%W'))
    assert_nil(Date._strptime('54', '%W'))
    assert_nil(Date._strptime('0', '%V'))
    assert_nil(Date._strptime('54', '%V'))
    assert_nil(Date._strptime('0', '%u'))
    assert_not_nil(Date._strptime('7', '%u'))
    assert_not_nil(Date._strptime('0', '%w'))
    assert_nil(Date._strptime('7', '%w'))

    assert_nil(Date._strptime('Sanday', '%A'))
    assert_nil(Date._strptime('Jenuary', '%B'))
    assert_not_nil(Date._strptime('Sundai', '%A'))
    assert_not_nil(Date._strptime('Januari', '%B'))
    assert_nil(Date._strptime('Sundai,', '%A,'))
    assert_nil(Date._strptime('Januari,', '%B,'))
  end

  def test_strptime
    assert_equal(Date.new, Date.strptime)
    d = Date.new(2002,3,14)
    assert_equal(d, Date.strptime(d.to_s))
    assert_equal(Date.new(2002,3,14), Date.strptime('2002-03-14'))

    d = DateTime.new(2002,3,14,11,22,33, 0)
    assert_equal(d, DateTime.strptime(d.to_s))
    assert_equal(DateTime.new(2002,3,14,11,22,33, 0),
		 DateTime.strptime('2002-03-14T11:22:33Z'))
    assert_equal(DateTime.new(2002,3,14,11,22,33, 0),
		 DateTime.strptime('2002-03-14T11:22:33Z', '%Y-%m-%dT%H:%M:%S%Z'))
    assert_equal(DateTime.new(2002,3,14,11,22,33, 9.to_r/24),
		 DateTime.strptime('2002-03-14T11:22:33+09:00', '%Y-%m-%dT%H:%M:%S%Z'))
    assert_equal(DateTime.new(2002,3,14,11,22,33, -9.to_r/24),
		 DateTime.strptime('2002-03-14T11:22:33-09:00', '%FT%T%Z'))
    assert_equal(DateTime.new(2002,3,14,11,22,33, -9.to_r/24) + 123456789.to_r/1000000000/86400,
		 DateTime.strptime('2002-03-14T11:22:33.123456789-09:00', '%FT%T.%N%Z'))
  end

  def test_strptime__2
    n = 10**9
    (Date.new(2006,6,1)..Date.new(2007,6,1)).each do |d|
      [
       '%Y %m %d',
       '%C %y %m %d',

       '%Y %j',
       '%C %y %j',

       '%G %V %w',
       '%G %V %u',
       '%C %g %V %w',
       '%C %g %V %u',

       '%Y %U %w',
       '%Y %U %u',
       '%Y %W %w',
       '%Y %W %u',
       '%C %y %U %w',
       '%C %y %U %u',
       '%C %y %W %w',
       '%C %y %W %u',
       ].each do |fmt|
	s = d.strftime(fmt)
	d2 = Date.strptime(s, fmt)
	assert_equal(d, d2, [fmt, d.to_s, d2.to_s].inspect)
      end

      [
       '%Y %m %d %H %M %S',
       '%Y %m %d %H %M %S %N',
       '%C %y %m %d %H %M %S',
       '%C %y %m %d %H %M %S %N',

       '%Y %j %H %M %S',
       '%Y %j %H %M %S %N',
       '%C %y %j %H %M %S',
       '%C %y %j %H %M %S %N',

       '%s',
       '%s %N',
       '%Q',
       '%Q %N',
      ].each do |fmt|
	s = d.strftime(fmt)
	d2 = DateTime.strptime(s, fmt)
	assert_equal(d, d2, [fmt, d.to_s, d2.to_s].inspect)
      end
    end
  end

  def test_strptime__minus
    d = DateTime.strptime('-1', '%s')
    assert_equal([1969, 12, 31, 23, 59, 59],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec])
    d = DateTime.strptime('-86400', '%s')
    assert_equal([1969, 12, 31, 0, 0, 0],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec])

    d = DateTime.strptime('-999', '%Q')
    assert_equal([1969, 12, 31, 23, 59, 59, 1.to_r/10**3],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec, d.sec_fraction])
    d = DateTime.strptime('-1000', '%Q')
    assert_equal([1969, 12, 31, 23, 59, 59, 0],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec, d.sec_fraction])
  end

  def test_strptime__comp
    n = DateTime.now

    d = DateTime.strptime('073', '%j')
    assert_equal([n.year, 73, 0, 0, 0],
		 [d.year, d.yday, d.hour, d.min, d.sec])
    d = DateTime.strptime('13', '%d')
    assert_equal([n.year, n.mon, 13, 0, 0, 0],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec])

    d = DateTime.strptime('Mar', '%b')
    assert_equal([n.year, 3, 1, 0, 0, 0],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec])
    d = DateTime.strptime('2004', '%Y')
    assert_equal([2004, 1, 1, 0, 0, 0],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec])

    d = DateTime.strptime('Mar 13', '%b %d')
    assert_equal([n.year, 3, 13, 0, 0, 0],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec])
    d = DateTime.strptime('Mar 2004', '%b %Y')
    assert_equal([2004, 3, 1, 0, 0, 0],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec])
    d = DateTime.strptime('23:55', '%H:%M')
    assert_equal([n.year, n.mon, n.mday, 23, 55, 0],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec])
    d = DateTime.strptime('23:55:30', '%H:%M:%S')
    assert_equal([n.year, n.mon, n.mday, 23, 55, 30],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec])

    d = DateTime.strptime('Sun 23:55', '%a %H:%M')
    d2 = d - d.wday
    assert_equal([d2.year, d2.mon, d2.mday, 23, 55, 0],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec])
    d = DateTime.strptime('Aug 23:55', '%b %H:%M')
    assert_equal([n.year, 8, 1, 23, 55, 0],
		 [d.year, d.mon, d.mday, d.hour, d.min, d.sec])

    d = DateTime.strptime('2004', '%G')
    assert_equal([2004, 1, 1, 0, 0, 0],
		 [d.cwyear, d.cweek, d.cwday, d.hour, d.min, d.sec])
    d = DateTime.strptime('11', '%V')
    assert_equal([n.cwyear, 11, 1, 0, 0, 0],
		 [d.cwyear, d.cweek, d.cwday, d.hour, d.min, d.sec])
    d = DateTime.strptime('6', '%u')
    assert_equal([n.cwyear, n.cweek, 6, 0, 0, 0],
		 [d.cwyear, d.cweek, d.cwday, d.hour, d.min, d.sec])

    d = DateTime.strptime('11-6', '%V-%u')
    assert_equal([n.cwyear, 11, 6, 0, 0, 0],
		 [d.cwyear, d.cweek, d.cwday, d.hour, d.min, d.sec])
    d = DateTime.strptime('2004-11', '%G-%V')
    assert_equal([2004, 11, 1, 0, 0, 0],
		 [d.cwyear, d.cweek, d.cwday, d.hour, d.min, d.sec])

    d = DateTime.strptime('11-6', '%U-%w')
    assert_equal([n.year, 11, 6, 0, 0, 0],
		 [d.year, d.strftime('%U').to_i, d.wday, d.hour, d.min, d.sec])
    d = DateTime.strptime('2004-11', '%Y-%U')
    assert_equal([2004, 11, 0, 0, 0, 0],
		 [d.year, d.strftime('%U').to_i, d.wday, d.hour, d.min, d.sec])

    d = DateTime.strptime('11-6', '%W-%w')
    assert_equal([n.year, 11, 6, 0, 0, 0],
		 [d.year, d.strftime('%W').to_i, d.wday, d.hour, d.min, d.sec])
    d = DateTime.strptime('2004-11', '%Y-%W')
    assert_equal([2004, 11, 1, 0, 0, 0],
		 [d.year, d.strftime('%W').to_i, d.wday, d.hour, d.min, d.sec])
  end

  def test_strptime__d_to_s
    d = Date.new(2002,3,14)
    assert_equal(d, Date.strptime(d.to_s))

    d = DateTime.new(2002,3,14,11,22,33, 9.to_r/24)
    assert_equal(d, DateTime.strptime(d.to_s))
  end

  def test_strptime__ex
    assert_raise(ArgumentError) do
      Date.strptime('')
    end
    assert_raise(ArgumentError) do
      DateTime.strptime('')
    end
    assert_raise(ArgumentError) do
      Date.strptime('2001-02-29', '%F')
    end
    assert_raise(ArgumentError) do
      DateTime.strptime('2001-02-29T23:59:60', '%FT%T')
    end
    assert_raise(ArgumentError) do
      Date.strptime('23:55', '%H:%M')
    end
    assert_raise(ArgumentError) do
      Date.strptime('01-31-2011', '%m/%d/%Y')
    end
  end

  def test_given_string
    s = '2001-02-03T04:05:06Z'
    s0 = s.dup

    assert_not_equal({}, Date._strptime(s, '%FT%T%Z'))
    assert_equal(s0, s)
  end

end
