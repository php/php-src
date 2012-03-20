require 'test/unit'
require 'rational'
require 'delegate'
require 'timeout'
require 'delegate'
require_relative 'envutil'

class TestTime < Test::Unit::TestCase
  def setup
    @verbose = $VERBOSE
    $VERBOSE = nil
  end

  def teardown
    $VERBOSE = @verbose
  end

  def test_new
    assert_equal(Time.utc(2000,2,10), Time.new(2000,2,10, 11,0,0, 3600*11))
    assert_equal(Time.utc(2000,2,10), Time.new(2000,2,9, 13,0,0, -3600*11))
    assert_equal(Time.utc(2000,2,10), Time.new(2000,2,10, 11,0,0, "+11:00"))
    assert_equal(Rational(1,2), Time.new(2000,2,10, 11,0,5.5, "+11:00").subsec)
    bug4090 = '[ruby-dev:42631]'
    tm = [2001,2,28,23,59,30]
    t = Time.new(*tm, "-12:00")
    assert_equal([2001,2,28,23,59,30,-43200], [t.year, t.month, t.mday, t.hour, t.min, t.sec, t.gmt_offset], bug4090)
  end

  def test_time_add()
    assert_equal(Time.utc(2000, 3, 21, 3, 30) + 3 * 3600,
                 Time.utc(2000, 3, 21, 6, 30))
    assert_equal(Time.utc(2000, 3, 21, 3, 30) + (-3 * 3600),
                 Time.utc(2000, 3, 21, 0, 30))
    assert_equal(0, (Time.at(1.1) + 0.9).usec)

    assert((Time.utc(2000, 4, 1) + 24).utc?)
    assert(!(Time.local(2000, 4, 1) + 24).utc?)

    t = Time.new(2000, 4, 1, 0, 0, 0, "+01:00") + 24
    assert(!t.utc?)
    assert_equal(3600, t.utc_offset)
    t = Time.new(2000, 4, 1, 0, 0, 0, "+02:00") + 24
    assert(!t.utc?)
    assert_equal(7200, t.utc_offset)
  end

  def test_time_subt()
    assert_equal(Time.utc(2000, 3, 21, 3, 30) - 3 * 3600,
                 Time.utc(2000, 3, 21, 0, 30))
    assert_equal(Time.utc(2000, 3, 21, 3, 30) - (-3 * 3600),
                 Time.utc(2000, 3, 21, 6, 30))
    assert_equal(900000, (Time.at(1.1) - 0.2).usec)
  end

  def test_time_time()
    assert_equal(Time.utc(2000, 3, 21, 3, 30)  \
                -Time.utc(2000, 3, 21, 0, 30), 3*3600)
    assert_equal(Time.utc(2000, 3, 21, 0, 30)  \
                -Time.utc(2000, 3, 21, 3, 30), -3*3600)
  end

  def negative_time_t?
    begin
      Time.at(-1)
      true
    rescue ArgumentError
      false
    end
  end

  def test_timegm
    if negative_time_t?
      assert_equal(-0x80000000, Time.utc(1901, 12, 13, 20, 45, 52).tv_sec)
      assert_equal(-2, Time.utc(1969, 12, 31, 23, 59, 58).tv_sec)
      assert_equal(-1, Time.utc(1969, 12, 31, 23, 59, 59).tv_sec)
    end

    assert_equal(0, Time.utc(1970, 1, 1, 0, 0, 0).tv_sec) # the Epoch
    assert_equal(1, Time.utc(1970, 1, 1, 0, 0, 1).tv_sec)
    assert_equal(31535999, Time.utc(1970, 12, 31, 23, 59, 59).tv_sec)
    assert_equal(31536000, Time.utc(1971, 1, 1, 0, 0, 0).tv_sec)
    assert_equal(78796799, Time.utc(1972, 6, 30, 23, 59, 59).tv_sec)

    # 1972-06-30T23:59:60Z is the first leap second.
    if Time.utc(1972, 7, 1, 0, 0, 0) - Time.utc(1972, 6, 30, 23, 59, 59) == 1
      # no leap second.
      assert_equal(78796800, Time.utc(1972, 7, 1, 0, 0, 0).tv_sec)
      assert_equal(78796801, Time.utc(1972, 7, 1, 0, 0, 1).tv_sec)
      assert_equal(946684800, Time.utc(2000, 1, 1, 0, 0, 0).tv_sec)
      assert_equal(0x7fffffff, Time.utc(2038, 1, 19, 3, 14, 7).tv_sec)
    else
      # leap seconds supported.
      assert_equal(2, Time.utc(1972, 7, 1, 0, 0, 0) - Time.utc(1972, 6, 30, 23, 59, 59))
      assert_equal(78796800, Time.utc(1972, 6, 30, 23, 59, 60).tv_sec)
      assert_equal(78796801, Time.utc(1972, 7, 1, 0, 0, 0).tv_sec)
      assert_equal(78796802, Time.utc(1972, 7, 1, 0, 0, 1).tv_sec)
      assert_equal(946684822, Time.utc(2000, 1, 1, 0, 0, 0).tv_sec)
    end
  end

  def test_strtime
    t = nil
    assert_nothing_raised { t = Time.utc("2000", "1", "2" , "3", "4", "5") }
    assert_equal(Time.utc(2000,1,2,3,4,5), t)
  end

  def test_huge_difference
    if negative_time_t?
      assert_equal(Time.at(-0x80000000), Time.at(0x7fffffff) - 0xffffffff, "[ruby-dev:22619]")
      assert_equal(Time.at(-0x80000000), Time.at(0x7fffffff) + (-0xffffffff))
      assert_equal(Time.at(0x7fffffff), Time.at(-0x80000000) + 0xffffffff, "[ruby-dev:22619]")
      assert_equal(Time.at(0x7fffffff), Time.at(-0x80000000) - (-0xffffffff))
    end
  end

  def test_big_minus
    begin
      bigtime0 = Time.at(2**60)
      bigtime1 = Time.at(2**60+1)
    rescue RangeError
      return
    end
    assert_equal(1.0, bigtime1 - bigtime0)
  end

  def test_at
    assert_equal(100000, Time.at("0.1".to_r).usec)
    assert_equal(10000, Time.at("0.01".to_r).usec)
    assert_equal(1000, Time.at("0.001".to_r).usec)
    assert_equal(100, Time.at("0.0001".to_r).usec)
    assert_equal(10, Time.at("0.00001".to_r).usec)
    assert_equal(1, Time.at("0.000001".to_r).usec)
    assert_equal(100000000, Time.at("0.1".to_r).nsec)
    assert_equal(10000000, Time.at("0.01".to_r).nsec)
    assert_equal(1000000, Time.at("0.001".to_r).nsec)
    assert_equal(100000, Time.at("0.0001".to_r).nsec)
    assert_equal(10000, Time.at("0.00001".to_r).nsec)
    assert_equal(1000, Time.at("0.000001".to_r).nsec)
    assert_equal(100, Time.at("0.0000001".to_r).nsec)
    assert_equal(10, Time.at("0.00000001".to_r).nsec)
    assert_equal(1, Time.at("0.000000001".to_r).nsec)
    assert_equal(100000, Time.at(0.1).usec)
    assert_equal(10000, Time.at(0.01).usec)
    assert_equal(1000, Time.at(0.001).usec)
    assert_equal(100, Time.at(0.0001).usec)
    assert_equal(10, Time.at(0.00001).usec)
    assert_equal(3, Time.at(0.000003).usec)
    assert_equal(100000000, Time.at(0.1).nsec)
    assert_equal(10000000, Time.at(0.01).nsec)
    assert_equal(1000000, Time.at(0.001).nsec)
    assert_equal(100000, Time.at(0.0001).nsec)
    assert_equal(10000, Time.at(0.00001).nsec)
    assert_equal(3000, Time.at(0.000003).nsec)
    assert_equal(199, Time.at(0.0000002).nsec)
    assert_equal(10, Time.at(0.00000001).nsec)
    assert_equal(1, Time.at(0.000000001).nsec)

    assert_equal(0, Time.at(1e-10).nsec)
    assert_equal(0, Time.at(4e-10).nsec)
    assert_equal(0, Time.at(6e-10).nsec)
    assert_equal(1, Time.at(14e-10).nsec)
    assert_equal(1, Time.at(16e-10).nsec)
    if negative_time_t?
      assert_equal(999999999, Time.at(-1e-10).nsec)
      assert_equal(999999999, Time.at(-4e-10).nsec)
      assert_equal(999999999, Time.at(-6e-10).nsec)
      assert_equal(999999998, Time.at(-14e-10).nsec)
      assert_equal(999999998, Time.at(-16e-10).nsec)
    end

    t = Time.at(-4611686019).utc
    assert_equal(1823, t.year)

    t = Time.at(4611686018, 999999).utc
    assert_equal(2116, t.year)
    assert_equal("0.999999".to_r, t.subsec)

    t = Time.at(2**40 + "1/3".to_r, 9999999999999).utc
    assert_equal(36812, t.year)

    t = Time.at(-0x3fff_ffff_ffff_ffff)
    assert_equal(-146138510344, t.year)
    t = Time.at(-0x4000_0000_0000_0000)
    assert_equal(-146138510344, t.year)
    t = Time.at(-0x4000_0000_0000_0001)
    assert_equal(-146138510344, t.year)
    t = Time.at(-0x5000_0000_0000_0001)
    assert_equal(-182673138422, t.year)
    t = Time.at(-0x6000_0000_0000_0000)
    assert_equal(-219207766501, t.year)

    t = Time.at(0).utc
    assert_equal([1970,1,1, 0,0,0], [t.year, t.mon, t.day, t.hour, t.min, t.sec])
    t = Time.at(-86400).utc
    assert_equal([1969,12,31, 0,0,0], [t.year, t.mon, t.day, t.hour, t.min, t.sec])
    t = Time.at(-86400 * (400 * 365 + 97)).utc
    assert_equal([1970-400,1,1, 0,0,0], [t.year, t.mon, t.day, t.hour, t.min, t.sec])
    t = Time.at(-86400 * (400 * 365 + 97)*1000).utc
    assert_equal([1970-400*1000,1,1, 0,0,0], [t.year, t.mon, t.day, t.hour, t.min, t.sec])
    t = Time.at(-86400 * (400 * 365 + 97)*2421).utc
    assert_equal([1970-400*2421,1,1, 0,0,0], [t.year, t.mon, t.day, t.hour, t.min, t.sec])
    t = Time.at(-86400 * (400 * 365 + 97)*1000000).utc
    assert_equal([1970-400*1000000,1,1, 0,0,0], [t.year, t.mon, t.day, t.hour, t.min, t.sec])

    t = Time.at(-30613683110400).utc
    assert_equal([-968139,1,1, 0,0,0], [t.year, t.mon, t.day, t.hour, t.min, t.sec])
    t = Time.at(-30613683110401).utc
    assert_equal([-968140,12,31, 23,59,59], [t.year, t.mon, t.day, t.hour, t.min, t.sec])
  end

  def test_at2
    assert_equal(100, Time.at(0, 0.1).nsec)
    assert_equal(10, Time.at(0, 0.01).nsec)
    assert_equal(1, Time.at(0, 0.001).nsec)
  end

  def test_at_rational
    assert_equal(1, Time.at(Rational(1,1) / 1000000000).nsec)
    assert_equal(1, Time.at(1167609600 + Rational(1,1) / 1000000000).nsec)
  end

  def test_utc_subsecond
    assert_equal(500000, Time.utc(2007,1,1,0,0,1.5).usec)
    assert_equal(100000, Time.utc(2007,1,1,0,0,Rational(11,10)).usec)
  end

  def test_eq_nsec
    assert_equal(Time.at(0, 0.123), Time.at(0, 0.123))
    assert_not_equal(Time.at(0, 0.123), Time.at(0, 0.124))
  end

  def assert_marshal_roundtrip(t)
    iv_names = t.instance_variables
    iv_vals1 = iv_names.map {|n| t.instance_variable_get n }
    m = Marshal.dump(t)
    t2 = Marshal.load(m)
    iv_vals2 = iv_names.map {|n| t2.instance_variable_get n }
    assert_equal(t, t2)
    assert_equal(iv_vals1, iv_vals2)
    t2
  end

  def test_marshal_nsec
    assert_marshal_roundtrip(Time.at(0, 0.123))
    assert_marshal_roundtrip(Time.at(0, 0.120))
  end

  def test_marshal_nsec_191
    # generated by ruby 1.9.1p376
    m = "\x04\bIu:\tTime\r \x80\x11\x80@\xE2\x01\x00\x06:\rsubmicro\"\ax\x90"
    t = Marshal.load(m)
    assert_equal(Time.at(Rational(123456789, 1000000000)), t, "[ruby-dev:40133]")
  end

  def test_marshal_rational
    assert_marshal_roundtrip(Time.at(0, Rational(1,3)))
    assert_not_match(/Rational/, Marshal.dump(Time.at(0, Rational(1,3))))
  end

  def test_marshal_ivar
    t = Time.at(123456789, 987654.321)
    t.instance_eval { @var = 135 }
    assert_marshal_roundtrip(t)
    assert_marshal_roundtrip(Marshal.load(Marshal.dump(t)))
  end

  def test_marshal_timezone
    bug = '[ruby-dev:40063]'

    t1 = Time.gm(2000)
    m = Marshal.dump(t1.getlocal("-02:00"))
    t2 = Marshal.load(m)
    assert_equal(t1, t2)
    assert_equal(-7200, t2.utc_offset, bug)
    m = Marshal.dump(t1.getlocal("+08:15"))
    t2 = Marshal.load(m)
    assert_equal(t1, t2)
    assert_equal(29700, t2.utc_offset, bug)
  end

  def test_marshal_to_s
    t1 = Time.new(2011,11,8, 0,42,25, 9*3600)
    t2 = Time.at(Marshal.load(Marshal.dump(t1)))
    assert_equal("2011-11-08 00:42:25 +0900", t2.to_s,
      "[ruby-dev:44827] [Bug #5586]")
  end

  # Sat Jan 01 00:00:00 UTC 2000
  T2000 = Time.at(946684800).gmtime

  def test_security_error
    assert_raise(SecurityError) do
      Thread.new do
        t = Time.gm(2000)
        $SAFE = 4
        t.localtime
      end.join
    end
  end

  def test_at3
    assert_equal(T2000, Time.at(T2000))
#    assert_raise(RangeError) do
#      Time.at(2**31-1, 1_000_000)
#      Time.at(2**63-1, 1_000_000)
#    end
#    assert_raise(RangeError) do
#      Time.at(-2**31, -1_000_000)
#      Time.at(-2**63, -1_000_000)
#    end
  end

  def test_utc_or_local
    assert_equal(T2000, Time.gm(2000))
    assert_equal(T2000, Time.gm(0, 0, 0, 1, 1, 2000, :foo, :bar, false, :baz))
    assert_equal(T2000, Time.gm(2000, "jan"))
    assert_equal(T2000, Time.gm(2000, "1"))
    assert_equal(T2000, Time.gm(2000, 1, 1, 0, 0, 0, 0))
    assert_equal(T2000, Time.gm(2000, 1, 1, 0, 0, 0, "0"))
    assert_equal(T2000, Time.gm(2000, 1, 1, 0, 0, "0", :foo, :foo))
    assert_raise(ArgumentError) { Time.gm(2000, 1, 1, 0, 0, -1, :foo, :foo) }
    assert_raise(ArgumentError) { Time.gm(2000, 1, 1, 0, 0, -1.0, :foo, :foo) }
    assert_raise(RangeError) do
      Time.gm(2000, 1, 1, 0, 0, 10_000_000_000_000_000_001.0, :foo, :foo)
    end
    assert_raise(ArgumentError) { Time.gm(2000, 1, 1, 0, 0, -(2**31), :foo, :foo) }
    o = Object.new
    def o.to_r; nil; end
    assert_raise(TypeError) { Time.gm(2000, 1, 1, 0, 0, o, :foo, :foo) }
    def o.to_r; ""; end
    assert_raise(TypeError) { Time.gm(2000, 1, 1, 0, 0, o, :foo, :foo) }
    def o.to_r; Rational(11); end
    assert_equal(11, Time.gm(2000, 1, 1, 0, 0, o).sec)
    o = Object.new
    def o.to_int; 10; end
    assert_equal(10, Time.gm(2000, 1, 1, 0, 0, o).sec)
    assert_raise(ArgumentError) { Time.gm(2000, 13) }

    t = Time.local(2000)
    assert_equal(t.gmt_offset, T2000 - t)

    assert_equal(-4427700000, Time.utc(-4427700000,12,1).year)
    assert_equal(-2**30+10, Time.utc(-2**30+10,1,1).year)
  end

  def test_time_interval
    m = Mutex.new.lock
    assert_nothing_raised {
      Timeout.timeout(10) {
        m.sleep(0)
      }
    }
    assert_raise(ArgumentError) { m.sleep(-1) }
  end

  def test_to_f
    assert_equal(946684800.0, T2000.to_f)
  end

  def test_cmp
    assert_equal(-1, T2000 <=> Time.gm(2001))
    assert_equal(1, T2000 <=> Time.gm(1999))
    assert_nil(T2000 <=> 0)
  end

  def test_eql
    assert(T2000.eql?(T2000))
    assert(!T2000.eql?(Time.gm(2001)))
  end

  def test_utc_p
    assert(Time.gm(2000).gmt?)
    assert(!Time.local(2000).gmt?)
    assert(!Time.at(0).gmt?)
  end

  def test_hash
    assert_kind_of(Integer, T2000.hash)
  end

  def test_init_copy
    assert_equal(T2000, T2000.dup)
    assert_raise(TypeError) do
      T2000.instance_eval { initialize_copy(nil) }
    end
  end

  def test_localtime_gmtime
    assert_nothing_raised do
      t = Time.gm(2000)
      assert(t.gmt?)
      t.localtime
      assert(!t.gmt?)
      t.localtime
      assert(!t.gmt?)
      t.gmtime
      assert(t.gmt?)
      t.gmtime
      assert(t.gmt?)
    end

    t1 = Time.gm(2000)
    t2 = t1.getlocal
    assert_equal(t1, t2)
    t3 = t1.getlocal("-02:00")
    assert_equal(t1, t3)
    assert_equal(-7200, t3.utc_offset)
    t1.localtime
    assert_equal(t1, t2)
    assert_equal(t1.gmt?, t2.gmt?)
    assert_equal(t1, t3)

    t1 = Time.local(2000)
    t2 = t1.getgm
    assert_equal(t1, t2)
    t3 = t1.getlocal("-02:00")
    assert_equal(t1, t3)
    assert_equal(-7200, t3.utc_offset)
    t1.gmtime
    assert_equal(t1, t2)
    assert_equal(t1.gmt?, t2.gmt?)
    assert_equal(t1, t3)
  end

  def test_asctime
    assert_equal("Sat Jan  1 00:00:00 2000", T2000.asctime)
    assert_equal(Encoding::US_ASCII, T2000.asctime.encoding)
    assert_kind_of(String, Time.at(0).asctime)
  end

  def test_to_s
    assert_equal("2000-01-01 00:00:00 UTC", T2000.to_s)
    assert_equal(Encoding::US_ASCII, T2000.to_s.encoding)
    assert_kind_of(String, Time.at(946684800).getlocal.to_s)
    assert_equal(Time.at(946684800).getlocal.to_s, Time.at(946684800).to_s)
  end

  def test_zone
    assert_equal(Encoding.find('locale'), Time.now.zone.encoding)
  end

  def test_plus_minus_succ
    # assert_raise(RangeError) { T2000 + 10000000000 }
    # assert_raise(RangeError)  T2000 - 3094168449 }
    # assert_raise(RangeError) { T2000 + 1200798848 }
    assert_raise(TypeError) { T2000 + Time.now }
    assert_equal(T2000 + 1, T2000.succ)
  end

  def test_plus_type
    t0 = Time.utc(2000,1,1)
    n0 = t0.to_i
    n1 = n0+1
    t1 = Time.at(n1)
    assert_equal(t1, t0 + 1)
    assert_equal(t1, t0 + 1.0)
    assert_equal(t1, t0 + Rational(1,1))
    assert_equal(t1, t0 + SimpleDelegator.new(1))
    assert_equal(t1, t0 + SimpleDelegator.new(1.0))
    assert_equal(t1, t0 + SimpleDelegator.new(Rational(1,1)))
    assert_raise(TypeError) { t0 + nil }
    assert_raise(TypeError) { t0 + "1" }
    assert_raise(TypeError) { t0 + SimpleDelegator.new("1") }
    assert_equal(0.5, (t0 + 1.5).subsec)
    assert_equal(Rational(1,3), (t0 + Rational(4,3)).subsec)
    assert_equal(0.5, (t0 + SimpleDelegator.new(1.5)).subsec)
    assert_equal(Rational(1,3), (t0 + SimpleDelegator.new(Rational(4,3))).subsec)
  end

  def test_minus
    t = Time.at(-4611686018).utc - 100
    assert_equal(1823, t.year)
  end

  def test_readers
    assert_equal(0, T2000.sec)
    assert_equal(0, T2000.min)
    assert_equal(0, T2000.hour)
    assert_equal(1, T2000.mday)
    assert_equal(1, T2000.mon)
    assert_equal(2000, T2000.year)
    assert_equal(6, T2000.wday)
    assert_equal(1, T2000.yday)
    assert_equal(false, T2000.isdst)
    assert_equal("UTC", T2000.zone)
    assert_equal(Encoding.find("locale"), T2000.zone.encoding)
    assert_equal(0, T2000.gmt_offset)
    assert(!T2000.sunday?)
    assert(!T2000.monday?)
    assert(!T2000.tuesday?)
    assert(!T2000.wednesday?)
    assert(!T2000.thursday?)
    assert(!T2000.friday?)
    assert(T2000.saturday?)
    assert_equal([0, 0, 0, 1, 1, 2000, 6, 1, false, "UTC"], T2000.to_a)

    t = Time.at(946684800).getlocal
    assert_equal(t.sec, Time.at(946684800).sec)
    assert_equal(t.min, Time.at(946684800).min)
    assert_equal(t.hour, Time.at(946684800).hour)
    assert_equal(t.mday, Time.at(946684800).mday)
    assert_equal(t.mon, Time.at(946684800).mon)
    assert_equal(t.year, Time.at(946684800).year)
    assert_equal(t.wday, Time.at(946684800).wday)
    assert_equal(t.yday, Time.at(946684800).yday)
    assert_equal(t.isdst, Time.at(946684800).isdst)
    assert_equal(t.zone, Time.at(946684800).zone)
    assert_equal(Encoding.find("locale"), Time.at(946684800).zone.encoding)
    assert_equal(t.gmt_offset, Time.at(946684800).gmt_offset)
    assert_equal(t.sunday?, Time.at(946684800).sunday?)
    assert_equal(t.monday?, Time.at(946684800).monday?)
    assert_equal(t.tuesday?, Time.at(946684800).tuesday?)
    assert_equal(t.wednesday?, Time.at(946684800).wednesday?)
    assert_equal(t.thursday?, Time.at(946684800).thursday?)
    assert_equal(t.friday?, Time.at(946684800).friday?)
    assert_equal(t.saturday?, Time.at(946684800).saturday?)
    assert_equal(t.to_a, Time.at(946684800).to_a)
  end

  def test_strftime
    t = Time.at(946684800).getlocal
    assert_equal("Sat", T2000.strftime("%a"))
    assert_equal("Saturday", T2000.strftime("%A"))
    assert_equal("Jan", T2000.strftime("%b"))
    assert_equal("January", T2000.strftime("%B"))
    assert_kind_of(String, T2000.strftime("%c"))
    assert_equal("01", T2000.strftime("%d"))
    assert_equal("00", T2000.strftime("%H"))
    assert_equal("12", T2000.strftime("%I"))
    assert_equal("001", T2000.strftime("%j"))
    assert_equal("01", T2000.strftime("%m"))
    assert_equal("00", T2000.strftime("%M"))
    assert_equal("AM", T2000.strftime("%p"))
    assert_equal("00", T2000.strftime("%S"))
    assert_equal("00", T2000.strftime("%U"))
    assert_equal("00", T2000.strftime("%W"))
    assert_equal("6", T2000.strftime("%w"))
    assert_equal("01/01/00", T2000.strftime("%x"))
    assert_equal("00:00:00", T2000.strftime("%X"))
    assert_equal("00", T2000.strftime("%y"))
    assert_equal("2000", T2000.strftime("%Y"))
    assert_equal("UTC", T2000.strftime("%Z"))
    assert_equal("%", T2000.strftime("%%"))
    assert_equal("0", T2000.strftime("%-S"))

    assert_equal("", T2000.strftime(""))
    assert_equal("foo\0bar\x0000\x0000\x0000", T2000.strftime("foo\0bar\0%H\0%M\0%S"))
    assert_equal("foo" * 1000, T2000.strftime("foo" * 1000))

    t = Time.mktime(2000, 1, 1)
    assert_equal("Sat", t.strftime("%a"))

    t = Time.at(946684800, 123456.789)
    assert_equal("123", t.strftime("%3N"))
    assert_equal("123456", t.strftime("%6N"))
    assert_equal("123456789", t.strftime("%9N"))
    assert_equal("1234567890", t.strftime("%10N"))
    assert_equal("123456789", t.strftime("%0N"))
    assert_equal("000", t.strftime("%3S"))
    assert_equal("946684800", t.strftime("%s"))
    assert_equal("946684800", t.utc.strftime("%s"))

    t = Time.mktime(2001, 10, 1)
    assert_equal("2001-10-01", t.strftime("%F"))
    assert_equal(Encoding::UTF_8, t.strftime("\u3042%Z").encoding)
    assert_equal(true, t.strftime("\u3042%Z").valid_encoding?)

    t = Time.mktime(2001, 10, 1, 2, 0, 0)
    assert_equal("01", t.strftime("%d"))
    assert_equal("01", t.strftime("%0d"))
    assert_equal(" 1", t.strftime("%_d"))
    assert_equal(" 1", t.strftime("%e"))
    assert_equal("01", t.strftime("%0e"))
    assert_equal(" 1", t.strftime("%_e"))
    assert_equal("AM", t.strftime("%p"))
    assert_equal("am", t.strftime("%#p"))
    assert_equal("am", t.strftime("%P"))
    assert_equal("AM", t.strftime("%#P"))
    assert_equal("02", t.strftime("%H"))
    assert_equal("02", t.strftime("%0H"))
    assert_equal(" 2", t.strftime("%_H"))
    assert_equal("02", t.strftime("%I"))
    assert_equal("02", t.strftime("%0I"))
    assert_equal(" 2", t.strftime("%_I"))
    assert_equal(" 2", t.strftime("%k"))
    assert_equal("02", t.strftime("%0k"))
    assert_equal(" 2", t.strftime("%_k"))
    assert_equal(" 2", t.strftime("%l"))
    assert_equal("02", t.strftime("%0l"))
    assert_equal(" 2", t.strftime("%_l"))
    t = Time.mktime(2001, 10, 1, 14, 0, 0)
    assert_equal("PM", t.strftime("%p"))
    assert_equal("pm", t.strftime("%#p"))
    assert_equal("pm", t.strftime("%P"))
    assert_equal("PM", t.strftime("%#P"))
    assert_equal("14", t.strftime("%H"))
    assert_equal("14", t.strftime("%0H"))
    assert_equal("14", t.strftime("%_H"))
    assert_equal("02", t.strftime("%I"))
    assert_equal("02", t.strftime("%0I"))
    assert_equal(" 2", t.strftime("%_I"))
    assert_equal("14", t.strftime("%k"))
    assert_equal("14", t.strftime("%0k"))
    assert_equal("14", t.strftime("%_k"))
    assert_equal(" 2", t.strftime("%l"))
    assert_equal("02", t.strftime("%0l"))
    assert_equal(" 2", t.strftime("%_l"))

    t = Time.utc(1,1,4)
    assert_equal("0001", t.strftime("%Y"))
    assert_equal("0001", t.strftime("%G"))

    t = Time.utc(0,1,4)
    assert_equal("0000", t.strftime("%Y"))
    assert_equal("0000", t.strftime("%G"))

    t = Time.utc(-1,1,4)
    assert_equal("-0001", t.strftime("%Y"))
    assert_equal("-0001", t.strftime("%G"))

    # [ruby-dev:37155]
    t = Time.mktime(1970, 1, 18)
    assert_equal("0", t.strftime("%w"))
    assert_equal("7", t.strftime("%u"))

    # [ruby-dev:37160]
    assert_equal("\t", T2000.strftime("%t"))
    assert_equal("\t", T2000.strftime("%0t"))
    assert_equal("\t", T2000.strftime("%1t"))
    assert_equal("  \t", T2000.strftime("%3t"))
    assert_equal("00\t", T2000.strftime("%03t"))
    assert_equal("\n", T2000.strftime("%n"))
    assert_equal("\n", T2000.strftime("%0n"))
    assert_equal("\n", T2000.strftime("%1n"))
    assert_equal("  \n", T2000.strftime("%3n"))
    assert_equal("00\n", T2000.strftime("%03n"))

    # [ruby-dev:37162]
    assert_equal("SAT", T2000.strftime("%#a"))
    assert_equal("SATURDAY", T2000.strftime("%#A"))
    assert_equal("JAN", T2000.strftime("%#b"))
    assert_equal("JANUARY", T2000.strftime("%#B"))
    assert_equal("JAN", T2000.strftime("%#h"))
    assert_equal("FRIDAY", Time.local(2008,1,4).strftime("%#A"))

    t = Time.utc(2000,3,14, 6,53,"58.979323846".to_r) # Pi Day
    assert_equal("03/14/2000  6:53:58.97932384600000000000000000000",
                 t.strftime("%m/%d/%Y %l:%M:%S.%29N"))
    assert_equal("03/14/2000  6:53:58.9793238460",
                 t.strftime("%m/%d/%Y %l:%M:%S.%10N"))
    assert_equal("03/14/2000  6:53:58.979323846",
                 t.strftime("%m/%d/%Y %l:%M:%S.%9N"))
    assert_equal("03/14/2000  6:53:58.97932384",
                 t.strftime("%m/%d/%Y %l:%M:%S.%8N"))

    t = Time.utc(1592,3,14, 6,53,"58.97932384626433832795028841971".to_r) # Pi Day
    assert_equal("03/14/1592  6:53:58.97932384626433832795028841971",
                 t.strftime("%m/%d/%Y %l:%M:%S.%29N"))
    assert_equal("03/14/1592  6:53:58.9793238462",
                 t.strftime("%m/%d/%Y %l:%M:%S.%10N"))
    assert_equal("03/14/1592  6:53:58.979323846",
                 t.strftime("%m/%d/%Y %l:%M:%S.%9N"))
    assert_equal("03/14/1592  6:53:58.97932384",
                 t.strftime("%m/%d/%Y %l:%M:%S.%8N"))

    # [ruby-core:33985]
    assert_equal("3000000000", Time.at(3000000000).strftime('%s'))

    bug4457 = '[ruby-dev:43285]'
    assert_raise(Errno::ERANGE, bug4457) {Time.now.strftime('%8192z')}
  end

  def test_delegate
    d1 = SimpleDelegator.new(t1 = Time.utc(2000))
    d2 = SimpleDelegator.new(t2 = Time.utc(2001))
    assert_equal(-1, t1 <=> t2)
    assert_equal(1, t2 <=> t1)
    assert_equal(-1, d1 <=> d2)
    assert_equal(1, d2 <=> d1)
  end

  def test_to_r
    assert_kind_of(Rational, Time.new(2000,1,1,0,0,Rational(4,3)).to_r)
    assert_kind_of(Rational, Time.utc(1970).to_r)
  end

  def test_round
    t = Time.utc(1999,12,31, 23,59,59)
    t2 = (t+0.4).round
    assert_equal([59,59,23, 31,12,1999, 5,365,false,"UTC"], t2.to_a)
    assert_equal(0, t2.subsec)
    t2 = (t+0.49).round
    assert_equal([59,59,23, 31,12,1999, 5,365,false,"UTC"], t2.to_a)
    assert_equal(0, t2.subsec)
    t2 = (t+0.5).round
    assert_equal([0,0,0, 1,1,2000, 6,1,false,"UTC"], t2.to_a)
    assert_equal(0, t2.subsec)
    t2 = (t+1.4).round
    assert_equal([0,0,0, 1,1,2000, 6,1,false,"UTC"], t2.to_a)
    assert_equal(0, t2.subsec)
    t2 = (t+1.49).round
    assert_equal([0,0,0, 1,1,2000, 6,1,false,"UTC"], t2.to_a)
    assert_equal(0, t2.subsec)
    t2 = (t+1.5).round
    assert_equal([1,0,0, 1,1,2000, 6,1,false,"UTC"], t2.to_a)
    assert_equal(0, t2.subsec)

    t2 = (t+0.123456789).round(4)
    assert_equal([59,59,23, 31,12,1999, 5,365,false,"UTC"], t2.to_a)
    assert_equal(Rational(1235,10000), t2.subsec)

    off = 0.0
    100.times {|i|
      t2 = (t+off).round(1)
      assert_equal(Rational(i % 10, 10), t2.subsec)
      off += 0.1
    }
  end

  def test_getlocal_dont_share_eigenclass
    bug5012 = "[ruby-dev:44071]"

    t0 = Time.now
    class << t0; end
    t1 = t0.getlocal

    def t0.m
      0
    end

    assert_raise(NoMethodError, bug5012) { t1.m }
  end

  def test_time_subclass
    bug5036 = '[ruby-dev:44122]'
    tc = Class.new(Time)
    tc.inspect
    t = tc.now
    error = assert_raise(SecurityError) do
      proc do
        $SAFE = 4
        t.gmtime
      end.call
    end
    assert_equal("Insecure: can't modify #{tc}", error.message, bug5036)
  end
end
