require 'psych/helper'
require 'date'

module Psych
  class TestScalarScanner < TestCase
    attr_reader :ss

    def setup
      super
      @ss = Psych::ScalarScanner.new
    end

    def test_scan_time
      { '2001-12-15T02:59:43.1Z' => Time.utc(2001, 12, 15, 02, 59, 43, 100000),
        '2001-12-14t21:59:43.10-05:00' => Time.utc(2001, 12, 15, 02, 59, 43, 100000),
        '2001-12-14 21:59:43.10 -5' => Time.utc(2001, 12, 15, 02, 59, 43, 100000),
        '2001-12-15 2:59:43.10' => Time.utc(2001, 12, 15, 02, 59, 43, 100000),
        '2011-02-24 11:17:06 -0800' => Time.utc(2011, 02, 24, 19, 17, 06)
      }.each do |time_str, time|
        assert_equal time, @ss.tokenize(time_str)
      end
    end

    def test_scan_bad_dates
      x = '2000-15-01'
      assert_equal x, @ss.tokenize(x)

      x = '2000-10-51'
      assert_equal x, @ss.tokenize(x)

      x = '2000-10-32'
      assert_equal x, @ss.tokenize(x)
    end

    def test_scan_good_edge_date
      x = '2000-1-31'
      assert_equal Date.strptime(x, '%Y-%m-%d'), @ss.tokenize(x)
    end

    def test_scan_bad_edge_date
      x = '2000-11-31'
      assert_equal x, @ss.tokenize(x)
    end

    def test_scan_date
      date = '1980-12-16'
      token = @ss.tokenize date
      assert_equal 1980, token.year
      assert_equal 12, token.month
      assert_equal 16, token.day
    end

    def test_scan_inf
      assert_equal(1 / 0.0, ss.tokenize('.inf'))
    end

    def test_scan_minus_inf
      assert_equal(-1 / 0.0, ss.tokenize('-.inf'))
    end

    def test_scan_nan
      assert ss.tokenize('.nan').nan?
    end

    def test_scan_null
      assert_equal nil, ss.tokenize('null')
      assert_equal nil, ss.tokenize('~')
      assert_equal nil, ss.tokenize('')
    end

    def test_scan_symbol
      assert_equal :foo, ss.tokenize(':foo')
    end

    def test_scan_sexagesimal_float
      assert_equal 685230.15, ss.tokenize('190:20:30.15')
    end

    def test_scan_sexagesimal_int
      assert_equal 685230, ss.tokenize('190:20:30')
    end

    def test_scan_float
      assert_equal 1.2, ss.tokenize('1.2')
    end

    def test_scan_true
      assert_equal true, ss.tokenize('true')
    end
  end
end
