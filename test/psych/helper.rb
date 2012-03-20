require 'minitest/autorun'
require 'stringio'
require 'tempfile'
require 'date'
require 'psych'

module Psych
  class TestCase < MiniTest::Unit::TestCase
    #
    # Convert between Psych and the object to verify correct parsing and
    # emitting
    #
    def assert_to_yaml( obj, yaml )
      assert_equal( obj, Psych::load( yaml ) )
      assert_equal( obj, Psych::parse( yaml ).transform )
      assert_equal( obj, Psych::load( obj.psych_to_yaml ) )
      assert_equal( obj, Psych::parse( obj.psych_to_yaml ).transform )
      assert_equal( obj, Psych::load(
        obj.psych_to_yaml(
          :UseVersion => true, :UseHeader => true, :SortKeys => true
        )
      ))
    end

    #
    # Test parser only
    #
    def assert_parse_only( obj, yaml )
      assert_equal( obj, Psych::load( yaml ) )
      assert_equal( obj, Psych::parse( yaml ).transform )
    end

    def assert_cycle( obj )
      v = Visitors::YAMLTree.new
      v << obj
      assert_equal(obj, Psych.load(v.tree.yaml))
      assert_equal( obj, Psych::load(Psych.dump(obj)))
      assert_equal( obj, Psych::load( obj.psych_to_yaml ) )
    end

    #
    # Make a time with the time zone
    #
    def mktime( year, mon, day, hour, min, sec, usec, zone = "Z" )
      usec = Rational(usec.to_s) * 1000000
      val = Time::utc( year.to_i, mon.to_i, day.to_i, hour.to_i, min.to_i, sec.to_i, usec )
      if zone != "Z"
        hour = zone[0,3].to_i * 3600
        min = zone[3,2].to_i * 60
        ofs = (hour + min)
        val = Time.at( val.tv_sec - ofs, val.tv_nsec / 1000.0 )
      end
      return val
    end
  end
end
