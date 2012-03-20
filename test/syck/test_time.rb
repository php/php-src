require 'test/unit'
require 'yaml'

module Syck
  class TestString < Test::Unit::TestCase
    def test_usec_long
      bug4571 = '[ruby-core:35713]'
      assert_equal(34, YAML.load("2011-03-22t23:32:11.0000342222+01:00").usec, bug4571)
    end

    def test_usec_very_long
      t = "2011-03-22t23:32:11.0000342"+"0"*1000+"1+01:00"
      assert_equal(34, YAML.load(t).usec)
    end

    def test_usec_full
      assert_equal(342222, YAML.load("2011-03-22t23:32:11.342222+01:00").usec)
    end

    def test_usec_short
      assert_equal(330000, YAML.load("2011-03-22t23:32:11.33+01:00").usec)
    end
  end
end
