require 'test/unit'
require 'date'

class TestDateStrftime < Test::Unit::TestCase

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

  def test_strftime
    d = Date.new(2001,2,3)
    STRFTIME_2001_02_03.each do |f, s|
      assert_equal(s[0], d.strftime(f), [f, s].inspect)
      case f[-1,1]
      when 'c', 'C', 'x', 'X', 'y', 'Y'
	f2 = f.sub(/\A%/, '%E')
	assert_equal(s[0], d.strftime(f2), [f2, s].inspect)
      else
	f2 = f.sub(/\A%/, '%E')
	assert_equal(f2, d.strftime(f2), [f2, s].inspect)
      end
      case f[-1,1]
      when 'd', 'e', 'H', 'I', 'm', 'M', 'S', 'u', 'U', 'V', 'w', 'W', 'y'
	f2 = f.sub(/\A%/, '%O')
	assert_equal(s[0], d.strftime(f2), [f2, s].inspect)
      else
	f2 = f.sub(/\A%/, '%O')
	assert_equal(f2, d.strftime(f2), [f2, s].inspect)
      end
    end
  end

  def test_strftime__2
    d = Date.new(2001,2,3)
    assert_equal('2001-02-03', d.strftime)

    d = DateTime.new(2001,2,3)
    assert_equal('2001-02-03T00:00:00+00:00', d.strftime)

    assert_equal('', d.strftime(''))
    assert_equal("\s"*3, d.strftime("\s"*3))
    assert_equal("\tfoo\n\000\r", d.strftime("\tfoo\n\000\r"))
    assert_equal("%\n", d.strftime("%\n")) # gnu
    assert_equal('Saturday'*1024 + ',', d.strftime('%A'*1024 + ','))
    assert_equal('%%', d.strftime('%%%'))
    assert_equal('Anton von Webern', d.strftime('Anton von Webern'))

    d = DateTime.new(2001,2,3, 1,2,3)
    assert_equal('2001-02-03T01:02:03+00:00', d.strftime)
    assert_equal('AM', d.strftime('%p'))
    assert_equal('am', d.strftime('%P'))
    d = DateTime.new(2001,2,3, 13,14,15)
    assert_equal('2001-02-03T13:14:15+00:00', d.strftime)
    assert_equal('PM', d.strftime('%p'))
    assert_equal('pm', d.strftime('%P'))
  end

  def test_strftime__3_1
    (Date.new(1970,1,1)..Date.new(2037,12,31)).each do |d|
      t = Time.utc(d.year,d.mon,d.mday)
      assert_equal(t.strftime('%U'), d.strftime('%U'))
      assert_equal(t.strftime('%W'), d.strftime('%W'))
    end
  end

  def test_strftime__3_2
    s = Time.now.strftime('%G')
    skip if s.empty? || s == '%G'
    (Date.new(1970,1,1)..Date.new(2037,12,31)).each do |d|
      t = Time.utc(d.year,d.mon,d.mday)
      assert_equal(t.strftime('%G'), d.strftime('%G'))
      assert_equal(t.strftime('%g'), d.strftime('%g'))
      assert_equal(t.strftime('%V'), d.strftime('%V'))
      assert_equal(t.strftime('%u'), d.strftime('%u'))
    end
  end

  def test_strftime__4
    s = '2006-08-08T23:15:33.123456789'
    f = '%FT%T.%N'
    d = DateTime.parse(s)
    assert_equal(s, d.strftime(f))
    d = DateTime.strptime(s, f)
    assert_equal(s, d.strftime(f))

    s = '2006-08-08T23:15:33.123456789'
    f = '%FT%T.%N'
    d = DateTime.parse(s + '123456789')
    assert_equal(s, d.strftime(f))
    d = DateTime.strptime(s + '123456789', f)
    assert_equal(s, d.strftime(f))

    si = '2006-08-08T23:15:33.9'
    so = '2006-08-08T23:15:33.900000000'
    f = '%FT%T.%N'
    d = DateTime.parse(si)
    assert_equal(so, d.strftime(f))
    d = DateTime.strptime(si, f)
    assert_equal(so, d.strftime(f))

    s = '2006-08-08T23:15:33.123'
    f = '%FT%T.%L'
    d = DateTime.parse(s)
    assert_equal(s, d.strftime(f))
    d = DateTime.strptime(s, f)
    assert_equal(s, d.strftime(f))

    s = '2006-08-08T23:15:33.123'
    f = '%FT%T.%L'
    d = DateTime.parse(s + '123')
    assert_equal(s, d.strftime(f))
    d = DateTime.strptime(s + '123', f)
    assert_equal(s, d.strftime(f))

    si = '2006-08-08T23:15:33.9'
    so = '2006-08-08T23:15:33.900'
    f = '%FT%T.%L'
    d = DateTime.parse(si)
    assert_equal(so, d.strftime(f))
    d = DateTime.strptime(si, f)
    assert_equal(so, d.strftime(f))
  end

  def test_strftime__offset
    s = '2006-08-08T23:15:33'
    (-24..24).collect{|x| '%+.2d' % x}.each do |hh|
      %w(00 30).each do |mm|
	r = hh + mm
	if r[-4,4] == '2430'
	  r = '+0000'
	end
	d = DateTime.parse(s + hh + mm)
	assert_equal(r, d.strftime('%z'))
      end
    end
  end

  def test_strftime_milli
    s = '1970-01-01T00:00:00.123456789'
    d = DateTime.parse(s)
    assert_equal('123', d.strftime('%Q'))
    s = '1970-01-02T02:03:04.123456789'
    d = DateTime.parse(s)
    assert_equal('93784123', d.strftime('%Q'))
  end

  def test_strftime__minus
    d = DateTime.new(1969, 12, 31, 23, 59, 59)
    assert_equal('-1', d.strftime('%s'))
    assert_equal('-1000', d.strftime('%Q'))
  end

  def test_strftime__gnuext # coreutils
    d = DateTime.new(2006,8,8,23,15,33,9.to_r/24)

    assert_equal('2006', d.strftime('%-Y'))
    assert_equal('2006', d.strftime('%-5Y'))
    assert_equal('02006', d.strftime('%5Y'))
    assert_equal('2006', d.strftime('%_Y'))
    assert_equal(' 2006', d.strftime('%_5Y'))
    assert_equal('02006', d.strftime('%05Y'))

    assert_equal('8', d.strftime('%-d'))
    assert_equal('8', d.strftime('%-3d'))
    assert_equal('008', d.strftime('%3d'))
    assert_equal(' 8', d.strftime('%_d'))
    assert_equal('  8', d.strftime('%_3d'))
    assert_equal('008', d.strftime('%03d'))

    assert_equal('8', d.strftime('%-e'))
    assert_equal('8', d.strftime('%-3e'))
    assert_equal('  8', d.strftime('%3e'))
    assert_equal(' 8', d.strftime('%_e'))
    assert_equal('  8', d.strftime('%_3e'))
    assert_equal('008', d.strftime('%03e'))

    assert_equal('Tuesday', d.strftime('%-10A'))
    assert_equal('   Tuesday', d.strftime('%10A'))
    assert_equal('   Tuesday', d.strftime('%_10A'))
    assert_equal('000Tuesday', d.strftime('%010A'))
    assert_equal('TUESDAY', d.strftime('%^A'))
    assert_equal('TUESDAY', d.strftime('%#A'))

    assert_equal('Tue', d.strftime('%-6a'))
    assert_equal('   Tue', d.strftime('%6a'))
    assert_equal('   Tue', d.strftime('%_6a'))
    assert_equal('000Tue', d.strftime('%06a'))
    assert_equal('TUE', d.strftime('%^a'))
    assert_equal('TUE', d.strftime('%#a'))
    assert_equal('   TUE', d.strftime('%#6a'))

    assert_equal('August', d.strftime('%-10B'))
    assert_equal('    August', d.strftime('%10B'))
    assert_equal('    August', d.strftime('%_10B'))
    assert_equal('0000August', d.strftime('%010B'))
    assert_equal('AUGUST', d.strftime('%^B'))
    assert_equal('AUGUST', d.strftime('%#B'))

    assert_equal('Aug', d.strftime('%-6b'))
    assert_equal('   Aug', d.strftime('%6b'))
    assert_equal('   Aug', d.strftime('%_6b'))
    assert_equal('000Aug', d.strftime('%06b'))
    assert_equal('AUG', d.strftime('%^b'))
    assert_equal('AUG', d.strftime('%#b'))
    assert_equal('   AUG', d.strftime('%#6b'))

    assert_equal('Aug', d.strftime('%-6h'))
    assert_equal('   Aug', d.strftime('%6h'))
    assert_equal('   Aug', d.strftime('%_6h'))
    assert_equal('000Aug', d.strftime('%06h'))
    assert_equal('AUG', d.strftime('%^h'))
    assert_equal('AUG', d.strftime('%#h'))
    assert_equal('   AUG', d.strftime('%#6h'))

    assert_equal('PM', d.strftime('%^p'))
    assert_equal('pm', d.strftime('%#p'))
    assert_equal('PM', d.strftime('%^P'))
    assert_equal('PM', d.strftime('%#P'))

    assert_equal('+000900', d.strftime('%7z'))
    assert_equal('   +900', d.strftime('%_7z'))
    assert_equal('+09:00', d.strftime('%:z'))
    assert_equal('+0009:00', d.strftime('%8:z'))
    assert_equal('   +9:00', d.strftime('%_8:z'))
    assert_equal('+09:00:00', d.strftime('%::z'))
    assert_equal('+0009:00:00', d.strftime('%11::z'))
    assert_equal('   +9:00:00', d.strftime('%_11::z'))
    assert_equal('+09', d.strftime('%:::z'))
    assert_equal('+0009', d.strftime('%5:::z'))
    assert_equal('   +9', d.strftime('%_5:::z'))
    assert_equal('+9', d.strftime('%-:::z'))

    d = DateTime.new(-200,8,8,23,15,33,9.to_r/24)

    assert_equal('-0200', d.strftime('%Y'))
    assert_equal('-200', d.strftime('%-Y'))
    assert_equal('-200', d.strftime('%-5Y'))
    assert_equal('-0200', d.strftime('%5Y'))
    assert_equal(' -200', d.strftime('%_Y'))
    assert_equal(' -200', d.strftime('%_5Y'))
    assert_equal('-0200', d.strftime('%05Y'))

    d = DateTime.new(-2000,8,8,23,15,33,9.to_r/24)

    assert_equal('-2000', d.strftime('%Y'))
    assert_equal('-2000', d.strftime('%-Y'))
    assert_equal('-2000', d.strftime('%-5Y'))
    assert_equal('-2000', d.strftime('%5Y'))
    assert_equal('-2000', d.strftime('%_Y'))
    assert_equal('-2000', d.strftime('%_5Y'))
    assert_equal('-2000', d.strftime('%05Y'))
  end

  def test_strftime__gnuext_LN # coreutils
    d = DateTime.parse('2008-11-25T00:11:22.0123456789')
    assert_equal('012', d.strftime('%L'))
    assert_equal('012', d.strftime('%0L'))
    assert_equal('0', d.strftime('%1L'))
    assert_equal('01', d.strftime('%2L'))
    assert_equal('01234567890', d.strftime('%11L'))
    assert_equal('01234567890', d.strftime('%011L'))
    assert_equal('01234567890', d.strftime('%_11L'))
    assert_equal('012345678', d.strftime('%N'))
    assert_equal('012345678', d.strftime('%0N'))
    assert_equal('0', d.strftime('%1N'))
    assert_equal('01', d.strftime('%2N'))
    assert_equal('01234567890', d.strftime('%11N'))
    assert_equal('01234567890', d.strftime('%011N'))
    assert_equal('01234567890', d.strftime('%_11N'))
  end

  def test_strftime__gnuext_z # coreutils
    d = DateTime.parse('2006-08-08T23:15:33+09:08:07')
    assert_equal('+0908', d.strftime('%z'))
    assert_equal('+09:08', d.strftime('%:z'))
    assert_equal('+09:08:07', d.strftime('%::z'))
    assert_equal('+09:08:07', d.strftime('%:::z'))
  end

  def test__different_format
    d = Date.new(2001,2,3)

    assert_equal('Sat Feb  3 00:00:00 2001', d.ctime)
    assert_equal(d.ctime, d.asctime)

    assert_equal('2001-02-03', d.iso8601)
    assert_equal(d.xmlschema, d.iso8601)
    assert_equal('2001-02-03T00:00:00+00:00', d.rfc3339)
    assert_equal('Sat, 3 Feb 2001 00:00:00 +0000', d.rfc2822)
    assert_equal(d.rfc822, d.rfc2822)
    assert_equal('Sat, 03 Feb 2001 00:00:00 GMT', d.httpdate)
    assert_equal('H13.02.03', d.jisx0301)

    d = DateTime.new(2001,2,3)

    assert_equal('Sat Feb  3 00:00:00 2001', d.ctime)
    assert_equal(d.ctime, d.asctime)

    assert_equal('2001-02-03T00:00:00+00:00', d.iso8601)
    assert_equal(d.rfc3339, d.iso8601)
    assert_equal(d.xmlschema, d.iso8601)
    assert_equal('Sat, 3 Feb 2001 00:00:00 +0000', d.rfc2822)
    assert_equal(d.rfc822, d.rfc2822)
    assert_equal('Sat, 03 Feb 2001 00:00:00 GMT', d.httpdate)
    assert_equal('H13.02.03T00:00:00+00:00', d.jisx0301)

    d2 = DateTime.parse('2001-02-03T04:05:06.123456')
    assert_equal('2001-02-03T04:05:06.123+00:00', d2.iso8601(3))
    assert_equal('2001-02-03T04:05:06.123+00:00', d2.rfc3339(3))
    assert_equal('H13.02.03T04:05:06.123+00:00', d2.jisx0301(3))
    assert_equal('2001-02-03T04:05:06.123+00:00', d2.iso8601(3.5))
    assert_equal('2001-02-03T04:05:06.123+00:00', d2.rfc3339(3.5))
    assert_equal('H13.02.03T04:05:06.123+00:00', d2.jisx0301(3.5))
    assert_equal('2001-02-03T04:05:06.123456000+00:00', d2.iso8601(9))
    assert_equal('2001-02-03T04:05:06.123456000+00:00', d2.rfc3339(9))
    assert_equal('H13.02.03T04:05:06.123456000+00:00', d2.jisx0301(9))
    assert_equal('2001-02-03T04:05:06.123456000+00:00', d2.iso8601(9.9))
    assert_equal('2001-02-03T04:05:06.123456000+00:00', d2.rfc3339(9.9))
    assert_equal('H13.02.03T04:05:06.123456000+00:00', d2.jisx0301(9.9))

    assert_equal('1800-01-01T00:00:00+00:00', DateTime.new(1800).jisx0301)

    assert_equal('1868-01-25', Date.parse('1868-01-25').jisx0301)
    assert_equal('1872-12-31', Date.parse('1872-12-31').jisx0301)

    assert_equal('M06.01.01', Date.parse('1873-01-01').jisx0301)
    assert_equal('M45.07.29', Date.parse('1912-07-29').jisx0301)
    assert_equal('T01.07.30', Date.parse('1912-07-30').jisx0301)
    assert_equal('T15.12.24', Date.parse('1926-12-24').jisx0301)
    assert_equal('S01.12.25', Date.parse('1926-12-25').jisx0301)
    assert_equal('S64.01.07', Date.parse('1989-01-07').jisx0301)
    assert_equal('H01.01.08', Date.parse('1989-01-08').jisx0301)
    assert_equal('H18.09.01', Date.parse('2006-09-01').jisx0301)

    %w(M06.01.01
       M45.07.29
       T01.07.30
       T15.12.24
       S01.12.25
       S64.01.07
       H01.01.08
       H18.09.01).each do |s|
      assert_equal(s, Date.parse(s).jisx0301)
    end

  end

end
