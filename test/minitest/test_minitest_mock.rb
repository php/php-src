######################################################################
# This file is imported from the minitest project.
# DO NOT make modifications in this repo. They _will_ be reverted!
# File a patch instead and assign it to Ryan Davis.
######################################################################

require 'minitest/mock'
require 'minitest/unit'

MiniTest::Unit.autorun

class TestMiniTestMock < MiniTest::Unit::TestCase
  def setup
    @mock = MiniTest::Mock.new.expect(:foo, nil)
    @mock.expect(:meaning_of_life, 42)
  end

  def test_create_stub_method
    assert_nil @mock.foo
  end

  def test_allow_return_value_specification
    assert_equal 42, @mock.meaning_of_life
  end

  def test_blow_up_if_not_called
    @mock.foo

    util_verify_bad
  end

  def test_not_blow_up_if_everything_called
    @mock.foo
    @mock.meaning_of_life

    assert @mock.verify
  end

  def test_allow_expectations_to_be_added_after_creation
    @mock.expect(:bar, true)
    assert @mock.bar
  end

  def test_not_verify_if_new_expected_method_is_not_called
    @mock.foo
    @mock.meaning_of_life
    @mock.expect(:bar, true)

    util_verify_bad
  end

  def test_blow_up_on_wrong_number_of_arguments
    @mock.foo
    @mock.meaning_of_life
    @mock.expect(:sum, 3, [1, 2])

    e = assert_raises ArgumentError do
      @mock.sum
    end

    assert_equal "mocked method :sum expects 2 arguments, got 0", e.message
  end

  def test_return_mock_does_not_raise
    retval = MiniTest::Mock.new
    mock = MiniTest::Mock.new
    mock.expect(:foo, retval)
    mock.foo

    assert mock.verify
  end

  def test_mock_args_does_not_raise
    arg = MiniTest::Mock.new
    mock = MiniTest::Mock.new
    mock.expect(:foo, nil, [arg])
    mock.foo(arg)

    assert mock.verify
  end

  def test_blow_up_on_wrong_arguments
    @mock.foo
    @mock.meaning_of_life
    @mock.expect(:sum, 3, [1, 2])

    @mock.sum(2, 4)

    util_verify_bad
  end

  def test_respond_appropriately
    assert @mock.respond_to?(:foo)
    assert @mock.respond_to?('foo')
    assert !@mock.respond_to?(:bar)
  end

  def test_no_method_error_on_unexpected_methods
    e = assert_raises NoMethodError do
      @mock.bar
    end

    expected = "unmocked method :bar, expected one of [:foo, :meaning_of_life]"

    assert_equal expected, e.message
  end

  def test_assign_per_mock_return_values
    a = MiniTest::Mock.new
    b = MiniTest::Mock.new

    a.expect(:foo, :a)
    b.expect(:foo, :b)

    assert_equal :a, a.foo
    assert_equal :b, b.foo
  end

  def test_do_not_create_stub_method_on_new_mocks
    a = MiniTest::Mock.new
    a.expect(:foo, :a)

    assert !MiniTest::Mock.new.respond_to?(:foo)
  end

  def test_mock_is_a_blank_slate
    @mock.expect :kind_of?, true, [Fixnum]
    @mock.expect :==, true, [1]

    assert @mock.kind_of?(Fixnum), "didn't mock :kind_of\?"
    assert @mock == 1, "didn't mock :=="
  end

  def test_verify_allows_called_args_to_be_loosely_specified
    mock = MiniTest::Mock.new
    mock.expect :loose_expectation, true, [Integer]
    mock.loose_expectation 1

    assert mock.verify
  end

  def test_verify_raises_with_strict_args
    mock = MiniTest::Mock.new
    mock.expect :strict_expectation, true, [2]
    mock.strict_expectation 1

    util_verify_bad
  end

  def test_verify_shows_the_actual_arguments_in_the_message
    mock = MiniTest::Mock.new
    mock.expect :capitalized, true, ["a"]
    mock.capitalized "b"
    e = assert_raises MockExpectationError do
      mock.verify
    end

    a = {:retval=>true, :args=>["a"]}
    b = {:retval=>true, :args=>["b"]}

    expected = "expected capitalized, #{a.inspect}, got [#{b.inspect}]"
    assert_equal expected, e.message
  end

  def util_verify_bad
    assert_raises MockExpectationError do
      @mock.verify
    end
  end
end
