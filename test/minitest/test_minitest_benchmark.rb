######################################################################
# This file is imported from the minitest project.
# DO NOT make modifications in this repo. They _will_ be reverted!
# File a patch instead and assign it to Ryan Davis.
######################################################################

require 'minitest/autorun'
require 'minitest/benchmark'

##
# Used to verify data:
# http://www.wolframalpha.com/examples/RegressionAnalysis.html

class TestMiniTestBenchmark < MiniTest::Unit::TestCase
  def test_cls_bench_exp
    assert_equal [2, 4, 8, 16, 32], self.class.bench_exp(2, 32, 2)
  end

  def test_cls_bench_linear
    assert_equal [2, 4, 6, 8, 10], self.class.bench_linear(2, 10, 2)
  end

  def test_cls_benchmark_methods
    assert_equal [], self.class.benchmark_methods

    c = Class.new(MiniTest::Unit::TestCase) do
      def bench_blah
      end
    end

    assert_equal ["bench_blah"], c.benchmark_methods
  end

  def test_cls_bench_range
    assert_equal [1, 10, 100, 1_000, 10_000], self.class.bench_range
  end

  def test_fit_exponential_clean
    x = [1.0, 2.0, 3.0, 4.0, 5.0]
    y = x.map { |n| 1.1 * Math.exp(2.1 * n) }

    assert_fit :exponential, x, y, 1.0, 1.1, 2.1
  end

  def test_fit_exponential_noisy
    x = [1.0, 1.9, 2.6, 3.4, 5.0]
    y = [12, 10, 8.2, 6.9, 5.9]

    # verified with Numbers and R
    assert_fit :exponential, x, y, 0.95, 13.81148, -0.1820
  end

  def test_fit_constant_clean
    x = (1..5).to_a
    y = [5.0, 5.0, 5.0, 5.0, 5.0]

    assert_fit :linear, x, y, nil, 5.0, 0
  end

  def test_fit_constant_noisy
    x = (1..5).to_a
    y = [1.0, 1.2, 1.0, 0.8, 1.0]

    # verified in numbers and R
    assert_fit :linear, x, y, nil, 1.12, -0.04
  end

  def test_fit_linear_clean
    # y = m * x + b where m = 2.2, b = 3.1
    x = (1..5).to_a
    y = x.map { |n| 2.2 * n + 3.1 }

    assert_fit :linear, x, y, 1.0, 3.1, 2.2
  end

  def test_fit_linear_noisy
    x = [ 60,  61,  62,  63,  65]
    y = [3.1, 3.6, 3.8, 4.0, 4.1]

    # verified in numbers and R
    assert_fit :linear, x, y, 0.8315, -7.9635, 0.1878
  end

  def test_fit_power_clean
    # y = A x ** B, where B = b and A = e ** a
    # if, A = 1, B = 2, then

    x = [1.0, 2.0, 3.0, 4.0, 5.0]
    y = [1.0, 4.0, 9.0, 16.0, 25.0]

    assert_fit :power, x, y, 1.0, 1.0, 2.0
  end

  def test_fit_power_noisy
    # from www.engr.uidaho.edu/thompson/courses/ME330/lecture/least_squares.html
    x = [10, 12, 15, 17, 20, 22, 25, 27, 30, 32, 35]
    y = [95, 105, 125, 141, 173, 200, 253, 298, 385, 459, 602]

    # verified in numbers
    assert_fit :power, x, y, 0.90, 2.6217, 1.4556

    # income to % of households below income amount
    # http://library.wolfram.com/infocenter/Conferences/6461/PowerLaws.nb
    x = [15000, 25000, 35000, 50000, 75000, 100000]
    y = [0.154, 0.283, 0.402, 0.55, 0.733, 0.843]

    # verified in numbers
    assert_fit :power, x, y, 0.96, 3.119e-5, 0.8959
  end

  def assert_fit msg, x, y, fit, exp_a, exp_b
    a, b, rr = send "fit_#{msg}", x, y

    assert_operator rr, :>=, fit if fit
    assert_in_delta exp_a, a
    assert_in_delta exp_b, b
  end
end

