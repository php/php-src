require 'time'
require 'test/unit'
require_relative 'ruby/envutil.rb'

class TestTimeExtension < Test::Unit::TestCase # :nodoc:
  def test_rfc822
    assert_equal(Time.utc(1976, 8, 26, 14, 30) + 4 * 3600,
                 Time.rfc2822("26 Aug 76 14:30 EDT"))
    assert_equal(Time.utc(1976, 8, 27, 9, 32) + 7 * 3600,
                 Time.rfc2822("27 Aug 76 09:32 PDT"))
  end

  def test_rfc2822
    assert_equal(Time.utc(1997, 11, 21, 9, 55, 6) + 6 * 3600,
                 Time.rfc2822("Fri, 21 Nov 1997 09:55:06 -0600"))
    assert_equal(Time.utc(2003, 7, 1, 10, 52, 37) - 2 * 3600,
                 Time.rfc2822("Tue, 1 Jul 2003 10:52:37 +0200"))
    assert_equal(Time.utc(1997, 11, 21, 10, 1, 10) + 6 * 3600,
                 Time.rfc2822("Fri, 21 Nov 1997 10:01:10 -0600"))
    assert_equal(Time.utc(1997, 11, 21, 11, 0, 0) + 6 * 3600,
                 Time.rfc2822("Fri, 21 Nov 1997 11:00:00 -0600"))
    assert_equal(Time.utc(1997, 11, 24, 14, 22, 1) + 8 * 3600,
                 Time.rfc2822("Mon, 24 Nov 1997 14:22:01 -0800"))
    begin
      Time.at(-1)
    rescue ArgumentError
      # ignore
    else
      assert_equal(Time.utc(1969, 2, 13, 23, 32, 54) + 3 * 3600 + 30 * 60,
                   Time.rfc2822("Thu, 13 Feb 1969 23:32:54 -0330"))
      assert_equal(Time.utc(1969, 2, 13, 23, 32, 0) + 3 * 3600 + 30 * 60,
                   Time.rfc2822(" Thu,
      13
        Feb
          1969
      23:32
               -0330 (Newfoundland Time)"))
    end
    assert_equal(Time.utc(1997, 11, 21, 9, 55, 6),
                 Time.rfc2822("21 Nov 97 09:55:06 GMT"))
    assert_equal(Time.utc(1997, 11, 21, 9, 55, 6) + 6 * 3600,
                 Time.rfc2822("Fri, 21 Nov 1997 09 :   55  :  06 -0600"))
    assert_raise(ArgumentError) {
      # inner comment is not supported.
      Time.rfc2822("Fri, 21 Nov 1997 09(comment):   55  :  06 -0600")
    }
  end

  def test_encode_rfc2822
    t = Time.utc(1)
    assert_equal("Mon, 01 Jan 0001 00:00:00 -0000", t.rfc2822)
  end

  def test_rfc2616
    t = Time.utc(1994, 11, 6, 8, 49, 37)
    assert_equal(t, Time.httpdate("Sun, 06 Nov 1994 08:49:37 GMT"))
    assert_equal(t, Time.httpdate("Sunday, 06-Nov-94 08:49:37 GMT"))
    assert_equal(t, Time.httpdate("Sun Nov  6 08:49:37 1994"))
    assert_equal(Time.utc(1995, 11, 15, 6, 25, 24),
                 Time.httpdate("Wed, 15 Nov 1995 06:25:24 GMT"))
    assert_equal(Time.utc(1995, 11, 15, 4, 58, 8),
                 Time.httpdate("Wed, 15 Nov 1995 04:58:08 GMT"))
    assert_equal(Time.utc(1994, 11, 15, 8, 12, 31),
                 Time.httpdate("Tue, 15 Nov 1994 08:12:31 GMT"))
    assert_equal(Time.utc(1994, 12, 1, 16, 0, 0),
                 Time.httpdate("Thu, 01 Dec 1994 16:00:00 GMT"))
    assert_equal(Time.utc(1994, 10, 29, 19, 43, 31),
                 Time.httpdate("Sat, 29 Oct 1994 19:43:31 GMT"))
    assert_equal(Time.utc(1994, 11, 15, 12, 45, 26),
                 Time.httpdate("Tue, 15 Nov 1994 12:45:26 GMT"))
    assert_equal(Time.utc(1999, 12, 31, 23, 59, 59),
                 Time.httpdate("Fri, 31 Dec 1999 23:59:59 GMT"))

    assert_equal(Time.utc(2007, 12, 23, 11, 22, 33),
                 Time.httpdate('Sunday, 23-Dec-07 11:22:33 GMT'))
  end

  def test_encode_httpdate
    t = Time.utc(1)
    assert_equal("Mon, 01 Jan 0001 00:00:00 GMT", t.httpdate)
  end

  def test_rfc3339
    t = Time.utc(1985, 4, 12, 23, 20, 50, 520000)
    s = "1985-04-12T23:20:50.52Z"
    assert_equal(t, Time.iso8601(s))
    assert_equal(s, t.iso8601(2))

    t = Time.utc(1996, 12, 20, 0, 39, 57)
    s = "1996-12-19T16:39:57-08:00"
    assert_equal(t, Time.iso8601(s))
    # There is no way to generate time string with arbitrary timezone.
    s = "1996-12-20T00:39:57Z"
    assert_equal(t, Time.iso8601(s))
    assert_equal(s, t.iso8601)

    t = Time.utc(1990, 12, 31, 23, 59, 60)
    s = "1990-12-31T23:59:60Z"
    assert_equal(t, Time.iso8601(s))
    # leap second is representable only if timezone file has it.
    s = "1990-12-31T15:59:60-08:00"
    assert_equal(t, Time.iso8601(s))

    begin
      Time.at(-1)
    rescue ArgumentError
      # ignore
    else
      t = Time.utc(1937, 1, 1, 11, 40, 27, 870000)
      s = "1937-01-01T12:00:27.87+00:20"
      assert_equal(t, Time.iso8601(s))
    end
  end

  # http://www.w3.org/TR/xmlschema-2/
  def test_xmlschema
    assert_equal(Time.utc(1999, 5, 31, 13, 20, 0) + 5 * 3600,
                 Time.xmlschema("1999-05-31T13:20:00-05:00"))
    assert_equal(Time.local(2000, 1, 20, 12, 0, 0),
                 Time.xmlschema("2000-01-20T12:00:00"))
    assert_equal(Time.utc(2000, 1, 20, 12, 0, 0),
                 Time.xmlschema("2000-01-20T12:00:00Z"))
    assert_equal(Time.utc(2000, 1, 20, 12, 0, 0) - 12 * 3600,
                 Time.xmlschema("2000-01-20T12:00:00+12:00"))
    assert_equal(Time.utc(2000, 1, 20, 12, 0, 0) + 13 * 3600,
                 Time.xmlschema("2000-01-20T12:00:00-13:00"))
    assert_equal(Time.utc(2000, 3, 4, 23, 0, 0) - 3 * 3600,
                 Time.xmlschema("2000-03-04T23:00:00+03:00"))
    assert_equal(Time.utc(2000, 3, 4, 20, 0, 0),
                 Time.xmlschema("2000-03-04T20:00:00Z"))
    assert_equal(Time.local(2000, 1, 15, 0, 0, 0),
                 Time.xmlschema("2000-01-15T00:00:00"))
    assert_equal(Time.local(2000, 2, 15, 0, 0, 0),
                 Time.xmlschema("2000-02-15T00:00:00"))
    assert_equal(Time.local(2000, 1, 15, 12, 0, 0),
                 Time.xmlschema("2000-01-15T12:00:00"))
    assert_equal(Time.utc(2000, 1, 16, 12, 0, 0),
                 Time.xmlschema("2000-01-16T12:00:00Z"))
    assert_equal(Time.local(2000, 1, 1, 12, 0, 0),
                 Time.xmlschema("2000-01-01T12:00:00"))
    assert_equal(Time.utc(1999, 12, 31, 23, 0, 0),
                 Time.xmlschema("1999-12-31T23:00:00Z"))
    assert_equal(Time.local(2000, 1, 16, 12, 0, 0),
                 Time.xmlschema("2000-01-16T12:00:00"))
    assert_equal(Time.local(2000, 1, 16, 0, 0, 0),
                 Time.xmlschema("2000-01-16T00:00:00"))
    assert_equal(Time.utc(2000, 1, 12, 12, 13, 14),
                 Time.xmlschema("2000-01-12T12:13:14Z"))
    assert_equal(Time.utc(2001, 4, 17, 19, 23, 17, 300000),
                 Time.xmlschema("2001-04-17T19:23:17.3Z"))
    assert_equal(Time.utc(2000, 1, 2, 0, 0, 0),
                 Time.xmlschema("2000-01-01T24:00:00Z"))
    assert_raise(ArgumentError) { Time.xmlschema("2000-01-01T00:00:00.+00:00") }
  end

  def test_encode_xmlschema
    bug6100 = '[ruby-core:42997]'

    t = Time.utc(2001, 4, 17, 19, 23, 17, 300000)
    assert_equal("2001-04-17T19:23:17Z", t.xmlschema)
    assert_equal("2001-04-17T19:23:17.3Z", t.xmlschema(1))
    assert_equal("2001-04-17T19:23:17.300000Z", t.xmlschema(6))
    assert_equal("2001-04-17T19:23:17.3000000Z", t.xmlschema(7))
    assert_equal("2001-04-17T19:23:17.3Z", t.xmlschema(1.9), bug6100)

    t = Time.utc(2001, 4, 17, 19, 23, 17, 123456)
    assert_equal("2001-04-17T19:23:17.1234560Z", t.xmlschema(7))
    assert_equal("2001-04-17T19:23:17.123456Z", t.xmlschema(6))
    assert_equal("2001-04-17T19:23:17.12345Z", t.xmlschema(5))
    assert_equal("2001-04-17T19:23:17.1Z", t.xmlschema(1))
    assert_equal("2001-04-17T19:23:17.1Z", t.xmlschema(1.9), bug6100)

    t = Time.at(2.quo(3)).getlocal("+09:00")
    assert_equal("1970-01-01T09:00:00.666+09:00", t.xmlschema(3))
    assert_equal("1970-01-01T09:00:00.6666666666+09:00", t.xmlschema(10))
    assert_equal("1970-01-01T09:00:00.66666666666666666666+09:00", t.xmlschema(20))
    assert_equal("1970-01-01T09:00:00.6+09:00", t.xmlschema(1.1), bug6100)
    assert_equal("1970-01-01T09:00:00.666+09:00", t.xmlschema(3.2), bug6100)

    t = Time.at(123456789.quo(9999999999)).getlocal("+09:00")
    assert_equal("1970-01-01T09:00:00.012+09:00", t.xmlschema(3))
    assert_equal("1970-01-01T09:00:00.012345678+09:00", t.xmlschema(9))
    assert_equal("1970-01-01T09:00:00.0123456789+09:00", t.xmlschema(10))
    assert_equal("1970-01-01T09:00:00.0123456789012345678+09:00", t.xmlschema(19))
    assert_equal("1970-01-01T09:00:00.01234567890123456789+09:00", t.xmlschema(20))
    assert_equal("1970-01-01T09:00:00.012+09:00", t.xmlschema(3.8), bug6100)

    t = Time.utc(1)
    assert_equal("0001-01-01T00:00:00Z", t.xmlschema)

    begin
      Time.at(-1)
    rescue ArgumentError
      # ignore
    else
      t = Time.utc(1960, 12, 31, 23, 0, 0, 123456)
      assert_equal("1960-12-31T23:00:00.123456Z", t.xmlschema(6))
    end

    assert_equal(249, Time.xmlschema("2008-06-05T23:49:23.000249+09:00").usec)

    assert_equal("10000-01-01T00:00:00Z", Time.utc(10000).xmlschema)
    assert_equal("9999-01-01T00:00:00Z", Time.utc(9999).xmlschema)
    assert_equal("0001-01-01T00:00:00Z", Time.utc(1).xmlschema) # 1 AD
    assert_equal("0000-01-01T00:00:00Z", Time.utc(0).xmlschema) # 1 BC
    assert_equal("-0001-01-01T00:00:00Z", Time.utc(-1).xmlschema) # 2 BC
    assert_equal("-0004-01-01T00:00:00Z", Time.utc(-4).xmlschema) # 5 BC
    assert_equal("-9999-01-01T00:00:00Z", Time.utc(-9999).xmlschema)
    assert_equal("-10000-01-01T00:00:00Z", Time.utc(-10000).xmlschema)
  end

  def test_completion
    now = Time.local(2001,11,29,21,26,35)
    assert_equal(Time.local( 2001,11,29,21,12),
                 Time.parse("2001/11/29 21:12", now))
    assert_equal(Time.local( 2001,11,29),
                 Time.parse("2001/11/29", now))
    assert_equal(Time.local( 2001,11,29),
                 Time.parse(     "11/29", now))
    #assert_equal(Time.local(2001,11,1), Time.parse("Nov", now))
    assert_equal(Time.local( 2001,11,29,10,22),
                 Time.parse(           "10:22", now))
    assert_raise(ArgumentError) { Time.parse("foo", now) }
  end

  def test_invalid
    # They were actually used in some web sites.
    assert_raise(ArgumentError) { Time.httpdate("1 Dec 2001 10:23:57 GMT") }
    assert_raise(ArgumentError) { Time.httpdate("Sat, 1 Dec 2001 10:25:42 GMT") }
    assert_raise(ArgumentError) { Time.httpdate("Sat,  1-Dec-2001 10:53:55 GMT") }
    assert_raise(ArgumentError) { Time.httpdate("Saturday, 01-Dec-2001 10:15:34 GMT") }
    assert_raise(ArgumentError) { Time.httpdate("Saturday, 01-Dec-101 11:10:07 GMT") }
    assert_raise(ArgumentError) { Time.httpdate("Fri, 30 Nov 2001 21:30:00 JST") }

    # They were actually used in some mails.
    assert_raise(ArgumentError) { Time.rfc2822("01-5-20") }
    assert_raise(ArgumentError) { Time.rfc2822("7/21/00") }
    assert_raise(ArgumentError) { Time.rfc2822("2001-8-28") }
    assert_raise(ArgumentError) { Time.rfc2822("00-5-6 1:13:06") }
    assert_raise(ArgumentError) { Time.rfc2822("2001-9-27 9:36:49") }
    assert_raise(ArgumentError) { Time.rfc2822("2000-12-13 11:01:11") }
    assert_raise(ArgumentError) { Time.rfc2822("2001/10/17 04:29:55") }
    assert_raise(ArgumentError) { Time.rfc2822("9/4/2001 9:23:19 PM") }
    assert_raise(ArgumentError) { Time.rfc2822("01 Nov 2001 09:04:31") }
    assert_raise(ArgumentError) { Time.rfc2822("13 Feb 2001 16:4 GMT") }
    assert_raise(ArgumentError) { Time.rfc2822("01 Oct 00 5:41:19 PM") }
    assert_raise(ArgumentError) { Time.rfc2822("2 Jul 00 00:51:37 JST") }
    assert_raise(ArgumentError) { Time.rfc2822("01 11 2001 06:55:57 -0500") }
    assert_raise(ArgumentError) { Time.rfc2822("18 \343\366\356\341\370 2000") }
    assert_raise(ArgumentError) { Time.rfc2822("Fri, Oct 2001  18:53:32") }
    assert_raise(ArgumentError) { Time.rfc2822("Fri, 2 Nov 2001 03:47:54") }
    assert_raise(ArgumentError) { Time.rfc2822("Fri, 27 Jul 2001 11.14.14 +0200") }
    assert_raise(ArgumentError) { Time.rfc2822("Thu, 2 Nov 2000 04:13:53 -600") }
    assert_raise(ArgumentError) { Time.rfc2822("Wed, 5 Apr 2000 22:57:09 JST") }
    assert_raise(ArgumentError) { Time.rfc2822("Mon, 11 Sep 2000 19:47:33 00000") }
    assert_raise(ArgumentError) { Time.rfc2822("Fri, 28 Apr 2000 20:40:47 +-900") }
    assert_raise(ArgumentError) { Time.rfc2822("Fri, 19 Jan 2001 8:15:36 AM -0500") }
    assert_raise(ArgumentError) { Time.rfc2822("Thursday, Sep 27 2001 7:42:35 AM EST") }
    assert_raise(ArgumentError) { Time.rfc2822("3/11/2001 1:31:57 PM Pacific Daylight Time") }
    assert_raise(ArgumentError) { Time.rfc2822("Mi, 28 Mrz 2001 11:51:36") }
    assert_raise(ArgumentError) { Time.rfc2822("P, 30 sept 2001 23:03:14") }
    assert_raise(ArgumentError) { Time.rfc2822("fr, 11 aug 2000 18:39:22") }
    assert_raise(ArgumentError) { Time.rfc2822("Fr, 21 Sep 2001 17:44:03 -1000") }
    assert_raise(ArgumentError) { Time.rfc2822("Mo, 18 Jun 2001 19:21:40 -1000") }
    assert_raise(ArgumentError) { Time.rfc2822("l\366, 12 aug 2000 18:53:20") }
    assert_raise(ArgumentError) { Time.rfc2822("l\366, 26 maj 2001 00:15:58") }
    assert_raise(ArgumentError) { Time.rfc2822("Dom, 30 Sep 2001 17:36:30") }
    assert_raise(ArgumentError) { Time.rfc2822("%&, 31 %2/ 2000 15:44:47 -0500") }
    assert_raise(ArgumentError) { Time.rfc2822("dom, 26 ago 2001 03:57:07 -0300") }
    assert_raise(ArgumentError) { Time.rfc2822("ter, 04 set 2001 16:27:58 -0300") }
    assert_raise(ArgumentError) { Time.rfc2822("Wen, 3 oct 2001 23:17:49 -0400") }
    assert_raise(ArgumentError) { Time.rfc2822("Wen, 3 oct 2001 23:17:49 -0400") }
    assert_raise(ArgumentError) { Time.rfc2822("ele, 11 h: 2000 12:42:15 -0500") }
    assert_raise(ArgumentError) { Time.rfc2822("Tue, 14 Aug 2001 3:55:3 +0200") }
    assert_raise(ArgumentError) { Time.rfc2822("Fri, 25 Aug 2000 9:3:48 +0800") }
    assert_raise(ArgumentError) { Time.rfc2822("Fri, 1 Dec 2000 0:57:50 EST") }
    assert_raise(ArgumentError) { Time.rfc2822("Mon, 7 May 2001 9:39:51 +0200") }
    assert_raise(ArgumentError) { Time.rfc2822("Wed, 1 Aug 2001 16:9:15 +0200") }
    assert_raise(ArgumentError) { Time.rfc2822("Wed, 23 Aug 2000 9:17:36 +0800") }
    assert_raise(ArgumentError) { Time.rfc2822("Fri, 11 Aug 2000 10:4:42 +0800") }
    assert_raise(ArgumentError) { Time.rfc2822("Sat, 15 Sep 2001 13:22:2 +0300") }
    assert_raise(ArgumentError) { Time.rfc2822("Wed,16 \276\305\324\302 2001 20:06:25 +0800") }
    assert_raise(ArgumentError) { Time.rfc2822("Wed,7 \312\256\322\273\324\302 2001 23:47:22 +0800") }
    assert_raise(ArgumentError) { Time.rfc2822("=?iso-8859-1?Q?(=C5=DA),?= 10   2 2001 23:32:26 +0900 (JST)") }
    assert_raise(ArgumentError) { Time.rfc2822("\307\341\314\343\332\311, 30 \344\346\335\343\310\321 2001 10:01:06") }
    assert_raise(ArgumentError) { Time.rfc2822("=?iso-8859-1?Q?(=BF=E5),?= 12  =?iso-8859-1?Q?9=B7=EE?= 2001 14:52:41\n+0900 (JST)") }
  end

  def test_zone_0000
    assert_equal(true, Time.parse("2000-01-01T00:00:00Z").utc?)
    assert_equal(true, Time.parse("2000-01-01T00:00:00-00:00").utc?)
    assert_equal(false, Time.parse("2000-01-01T00:00:00+00:00").utc?)
    assert_equal(false, Time.parse("Sat, 01 Jan 2000 00:00:00 GMT").utc?)
    assert_equal(true, Time.parse("Sat, 01 Jan 2000 00:00:00 -0000").utc?)
    assert_equal(false, Time.parse("Sat, 01 Jan 2000 00:00:00 +0000").utc?)
    assert_equal(false, Time.rfc2822("Sat, 01 Jan 2000 00:00:00 GMT").utc?)
    assert_equal(true, Time.rfc2822("Sat, 01 Jan 2000 00:00:00 -0000").utc?)
    assert_equal(false, Time.rfc2822("Sat, 01 Jan 2000 00:00:00 +0000").utc?)
    assert_equal(true, Time.rfc2822("Sat, 01 Jan 2000 00:00:00 UTC").utc?)
  end

  def test_rfc2822_utc_roundtrip_winter
    t1 = Time.local(2008,12,1)
    t2 = Time.rfc2822(t1.rfc2822)
    assert_equal(t1.utc?, t2.utc?, "[ruby-dev:37126]")
  end

  def test_rfc2822_utc_roundtrip_summer
    t1 = Time.local(2008,8,1)
    t2 = Time.rfc2822(t1.rfc2822)
    assert_equal(t1.utc?, t2.utc?)
  end

  def test_parse_leap_second
    t = Time.utc(1998,12,31,23,59,59)
    assert_equal(t, Time.parse("Thu Dec 31 23:59:59 UTC 1998"))
    assert_equal(t, Time.parse("Fri Dec 31 23:59:59 -0000 1998"));t.localtime
    assert_equal(t, Time.parse("Fri Jan  1 08:59:59 +0900 1999"))
    assert_equal(t, Time.parse("Fri Jan  1 00:59:59 +0100 1999"))
    assert_equal(t, Time.parse("Fri Dec 31 23:59:59 +0000 1998"))
    assert_equal(t, Time.parse("Fri Dec 31 22:59:59 -0100 1998"));t.utc
    t += 1
    assert_equal(t, Time.parse("Thu Dec 31 23:59:60 UTC 1998"))
    assert_equal(t, Time.parse("Fri Dec 31 23:59:60 -0000 1998"));t.localtime
    assert_equal(t, Time.parse("Fri Jan  1 08:59:60 +0900 1999"))
    assert_equal(t, Time.parse("Fri Jan  1 00:59:60 +0100 1999"))
    assert_equal(t, Time.parse("Fri Dec 31 23:59:60 +0000 1998"))
    assert_equal(t, Time.parse("Fri Dec 31 22:59:60 -0100 1998"));t.utc
    t += 1 if t.sec == 60
    assert_equal(t, Time.parse("Thu Jan  1 00:00:00 UTC 1999"))
    assert_equal(t, Time.parse("Fri Jan  1 00:00:00 -0000 1999"));t.localtime
    assert_equal(t, Time.parse("Fri Jan  1 09:00:00 +0900 1999"))
    assert_equal(t, Time.parse("Fri Jan  1 01:00:00 +0100 1999"))
    assert_equal(t, Time.parse("Fri Jan  1 00:00:00 +0000 1999"))
    assert_equal(t, Time.parse("Fri Dec 31 23:00:00 -0100 1998"))
  end

  def test_rfc2822_leap_second
    t = Time.utc(1998,12,31,23,59,59)
    assert_equal(t, Time.rfc2822("Thu, 31 Dec 1998 23:59:59 UTC"))
    assert_equal(t, Time.rfc2822("Fri, 31 Dec 1998 23:59:59 -0000"));t.localtime
    assert_equal(t, Time.rfc2822("Fri,  1 Jan 1999 08:59:59 +0900"))
    assert_equal(t, Time.rfc2822("Fri,  1 Jan 1999 00:59:59 +0100"))
    assert_equal(t, Time.rfc2822("Fri, 31 Dec 1998 23:59:59 +0000"))
    assert_equal(t, Time.rfc2822("Fri, 31 Dec 1998 22:59:59 -0100"));t.utc
    t += 1
    assert_equal(t, Time.rfc2822("Thu, 31 Dec 1998 23:59:60 UTC"))
    assert_equal(t, Time.rfc2822("Fri, 31 Dec 1998 23:59:60 -0000"));t.localtime
    assert_equal(t, Time.rfc2822("Fri,  1 Jan 1999 08:59:60 +0900"))
    assert_equal(t, Time.rfc2822("Fri,  1 Jan 1999 00:59:60 +0100"))
    assert_equal(t, Time.rfc2822("Fri, 31 Dec 1998 23:59:60 +0000"))
    assert_equal(t, Time.rfc2822("Fri, 31 Dec 1998 22:59:60 -0100"));t.utc
    t += 1 if t.sec == 60
    assert_equal(t, Time.rfc2822("Thu,  1 Jan 1999 00:00:00 UTC"))
    assert_equal(t, Time.rfc2822("Fri,  1 Jan 1999 00:00:00 -0000"));t.localtime
    assert_equal(t, Time.rfc2822("Fri,  1 Jan 1999 09:00:00 +0900"))
    assert_equal(t, Time.rfc2822("Fri,  1 Jan 1999 01:00:00 +0100"))
    assert_equal(t, Time.rfc2822("Fri,  1 Jan 1999 00:00:00 +0000"))
    assert_equal(t, Time.rfc2822("Fri, 31 Dec 1998 23:00:00 -0100"))
  end

  def test_xmlschema_leap_second
    t = Time.utc(1998,12,31,23,59,59)
    assert_equal(t, Time.xmlschema("1998-12-31T23:59:59Z"))
    assert_equal(t, Time.xmlschema("1998-12-31T23:59:59-00:00"));t.localtime
    assert_equal(t, Time.xmlschema("1999-01-01T08:59:59+09:00"))
    assert_equal(t, Time.xmlschema("1999-01-01T00:59:59+01:00"))
    assert_equal(t, Time.xmlschema("1998-12-31T23:59:59+00:00"))
    assert_equal(t, Time.xmlschema("1998-12-31T22:59:59-01:00"));t.utc
    t += 1
    assert_equal(t, Time.xmlschema("1998-12-31T23:59:60Z"))
    assert_equal(t, Time.xmlschema("1998-12-31T23:59:60-00:00"));t.localtime
    assert_equal(t, Time.xmlschema("1999-01-01T08:59:60+09:00"))
    assert_equal(t, Time.xmlschema("1999-01-01T00:59:60+01:00"))
    assert_equal(t, Time.xmlschema("1998-12-31T23:59:60+00:00"))
    assert_equal(t, Time.xmlschema("1998-12-31T22:59:60-01:00"));t.utc
    t += 1 if t.sec == 60
    assert_equal(t, Time.xmlschema("1999-01-01T00:00:00Z"))
    assert_equal(t, Time.xmlschema("1999-01-01T00:00:00-00:00"));t.localtime
    assert_equal(t, Time.xmlschema("1999-01-01T09:00:00+09:00"))
    assert_equal(t, Time.xmlschema("1999-01-01T01:00:00+01:00"))
    assert_equal(t, Time.xmlschema("1999-01-01T00:00:00+00:00"))
    assert_equal(t, Time.xmlschema("1998-12-31T23:00:00-01:00"))
  end

  def test_xmlschema_fraction
    assert_equal(500000, Time.xmlschema("2000-01-01T00:00:00.5+00:00").tv_usec)
  end

  def test_ruby_talk_152866
    t = Time::xmlschema('2005-08-30T22:48:00-07:00')
    assert_equal(31, t.day)
    assert_equal(8, t.mon)
  end

  def test_parse_fraction
    assert_equal(500000, Time.parse("2000-01-01T00:00:00.5+00:00").tv_usec)
  end

  def test_strptime
    assert_equal(Time.utc(2005, 8, 28, 06, 54, 20), Time.strptime("28/Aug/2005:06:54:20 +0000", "%d/%b/%Y:%T %z"))
    assert_equal(Time.at(1).localtime, Time.strptime("1", "%s"))
  end

  def test_nsec
    assert_equal(123456789, Time.xmlschema("2000-01-01T00:00:00.123456789+00:00").tv_nsec)
    assert_equal(123456789, Time.parse("2000-01-01T00:00:00.123456789+00:00").tv_nsec)
  end

  def test_huge_precision
    bug4456 = '[ruby-dev:43284]'
    assert_normal_exit %q{ Time.now.strftime("%1000000000F") }, bug4456
  end
end
