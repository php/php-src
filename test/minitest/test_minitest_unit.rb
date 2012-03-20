######################################################################
# This file is imported from the minitest project.
# DO NOT make modifications in this repo. They _will_ be reverted!
# File a patch instead and assign it to Ryan Davis.
######################################################################

require 'stringio'
require 'pathname'
require 'minitest/autorun'

module MyModule; end
class AnError < StandardError; include MyModule; end
class ImmutableString < String; def inspect; super.freeze; end; end

class TestMiniTestUnit < MiniTest::Unit::TestCase
  pwd = Pathname.new(File.expand_path(Dir.pwd))
  basedir = Pathname.new(File.expand_path("lib/minitest")) + 'mini'
  basedir = basedir.relative_path_from(pwd).to_s
  MINITEST_BASE_DIR = basedir[/\A\./] ? basedir : "./#{basedir}"
  BT_MIDDLE = ["#{MINITEST_BASE_DIR}/test.rb:161:in `each'",
               "#{MINITEST_BASE_DIR}/test.rb:158:in `each'",
               "#{MINITEST_BASE_DIR}/test.rb:139:in `run'",
               "#{MINITEST_BASE_DIR}/test.rb:106:in `run'"]

  def assert_report expected = nil
    expected ||= "Run options: --seed 42

# Running tests:

.

Finished tests in 0.00

1 tests, 1 assertions, 0 failures, 0 errors, 0 skips
"
    output = @output.string.sub(/Finished tests in .*/, "Finished tests in 0.00")
    output.sub!(/Loaded suite .*/, 'Loaded suite blah')
    output.sub!(/^(\s+)(?:#{Regexp.union(__FILE__, File.expand_path(__FILE__))}):\d+:/o, '\1FILE:LINE:')
    output.sub!(/\[(?:#{Regexp.union(__FILE__, File.expand_path(__FILE__))}):\d+\]/o, '[FILE:LINE]')
    assert_equal(expected, output)
  end

  def setup
    srand 42
    MiniTest::Unit::TestCase.reset
    @tu = MiniTest::Unit.new
    @output = StringIO.new("")
    MiniTest::Unit.runner = nil # protect the outer runner from the inner tests
    MiniTest::Unit.output = @output
  end

  def teardown
    MiniTest::Unit.output = $stdout
    Object.send :remove_const, :ATestCase if defined? ATestCase
  end

  def test_class_puke_with_assertion_failed
    exception = MiniTest::Assertion.new "Oh no!"
    exception.set_backtrace ["unhappy"]
    assert_equal 'F', @tu.puke('SomeClass', 'method_name', exception)
    assert_equal 1, @tu.failures
    assert_match(/^Failure.*Oh no!/m, @tu.report.first)
    assert_match("method_name(SomeClass) [unhappy]", @tu.report.first)
  end

  def test_class_puke_with_assertion_failed_and_long_backtrace
    bt = (["test/test_some_class.rb:615:in `method_name'",
           "#{MINITEST_BASE_DIR}/unit.rb:140:in `assert_raises'",
           "test/test_some_class.rb:615:in `each'",
           "test/test_some_class.rb:614:in `test_method_name'",
           "#{MINITEST_BASE_DIR}/test.rb:165:in `__send__'"] +
          BT_MIDDLE +
          ["#{MINITEST_BASE_DIR}/test.rb:29"])
    bt = util_expand_bt bt

    ex_location = util_expand_bt(["test/test_some_class.rb:615"]).first

    exception = MiniTest::Assertion.new "Oh no!"
    exception.set_backtrace bt
    assert_equal 'F', @tu.puke('TestSomeClass', 'test_method_name', exception)
    assert_equal 1, @tu.failures
    assert_match(/^Failure.*Oh no!/m, @tu.report.first)
    assert_match("test_method_name(TestSomeClass) [#{ex_location}]", @tu.report.first)
  end

  def test_class_puke_with_assertion_failed_and_user_defined_assertions
    bt = (["lib/test/my/util.rb:16:in `another_method_name'",
           "#{MINITEST_BASE_DIR}/unit.rb:140:in `assert_raises'",
           "lib/test/my/util.rb:15:in `block in assert_something'",
           "lib/test/my/util.rb:14:in `each'",
           "lib/test/my/util.rb:14:in `assert_something'",
           "test/test_some_class.rb:615:in `each'",
           "test/test_some_class.rb:614:in `test_method_name'",
           "#{MINITEST_BASE_DIR}/test.rb:165:in `__send__'"] +
          BT_MIDDLE +
          ["#{MINITEST_BASE_DIR}/test.rb:29"])
    bt = util_expand_bt bt

    ex_location = util_expand_bt(["test/test_some_class.rb:615"]).first

    exception = MiniTest::Assertion.new "Oh no!"
    exception.set_backtrace bt
    assert_equal 'F', @tu.puke('TestSomeClass', 'test_method_name', exception)
    assert_equal 1, @tu.failures
    assert_match(/^Failure.*Oh no!/m, @tu.report.first)
    assert_match("test_method_name(TestSomeClass) [#{ex_location}]", @tu.report.first)
  end

  def test_class_puke_with_failure_and_flunk_in_backtrace
    exception = begin
                  MiniTest::Unit::TestCase.new('fake tc').flunk
                rescue MiniTest::Assertion => failure
                  failure
                end
    assert_equal 'F', @tu.puke('SomeClass', 'method_name', exception)
    refute @tu.report.any?{|line| line =~ /in .flunk/}
  end

  def test_class_puke_with_flunk_and_user_defined_assertions
    bt = (["lib/test/my/util.rb:16:in `flunk'",
           "#{MINITEST_BASE_DIR}/unit.rb:140:in `assert_raises'",
           "lib/test/my/util.rb:15:in `block in assert_something'",
           "lib/test/my/util.rb:14:in `each'",
           "lib/test/my/util.rb:14:in `assert_something'",
           "test/test_some_class.rb:615:in `each'",
           "test/test_some_class.rb:614:in `test_method_name'",
           "#{MINITEST_BASE_DIR}/test.rb:165:in `__send__'"] +
          BT_MIDDLE +
          ["#{MINITEST_BASE_DIR}/test.rb:29"])
    bt = util_expand_bt bt

    ex_location = util_expand_bt(["test/test_some_class.rb:615"]).first

    exception = MiniTest::Assertion.new "Oh no!"
    exception.set_backtrace bt
    assert_equal 'F', @tu.puke('TestSomeClass', 'test_method_name', exception)
    assert_equal 1, @tu.failures
    assert_match(/^Failure.*Oh no!/m, @tu.report.first)
    assert_match("test_method_name(TestSomeClass) [#{ex_location}]", @tu.report.first)
  end

  def test_class_puke_with_non_failure_exception
    exception = Exception.new("Oh no again!")
    assert_equal 'E', @tu.puke('SomeClass', 'method_name', exception)
    assert_equal 1, @tu.errors
    assert_match(/^Exception.*Oh no again!/m, @tu.report.first)
  end

  def test_filter_backtrace
    # this is a semi-lame mix of relative paths.
    # I cheated by making the autotest parts not have ./
    bt = (["lib/autotest.rb:571:in `add_exception'",
           "test/test_autotest.rb:62:in `test_add_exception'",
           "#{MINITEST_BASE_DIR}/test.rb:165:in `__send__'"] +
          BT_MIDDLE +
          ["#{MINITEST_BASE_DIR}/test.rb:29",
           "test/test_autotest.rb:422"])
    bt = util_expand_bt bt

    ex = ["lib/autotest.rb:571:in `add_exception'",
          "test/test_autotest.rb:62:in `test_add_exception'"]
    ex = util_expand_bt ex

    fu = MiniTest::filter_backtrace(bt)

    assert_equal ex, fu
  end

  def test_filter_backtrace_all_unit
    bt = (["#{MINITEST_BASE_DIR}/test.rb:165:in `__send__'"] +
          BT_MIDDLE +
          ["#{MINITEST_BASE_DIR}/test.rb:29"])
    ex = bt.clone
    fu = MiniTest::filter_backtrace(bt)
    assert_equal ex, fu
  end

  def test_filter_backtrace_unit_starts
    bt = (["#{MINITEST_BASE_DIR}/test.rb:165:in `__send__'"] +
          BT_MIDDLE +
          ["#{MINITEST_BASE_DIR}/mini/test.rb:29",
           "-e:1"])

    bt = util_expand_bt bt

    ex = ["-e:1"]
    fu = MiniTest::filter_backtrace(bt)
    assert_equal ex, fu
  end

  def test_run_test
    tc = Class.new(MiniTest::Unit::TestCase) do
      attr_reader :foo

      def run_test name
        @foo = "hi mom!"
        super
        @foo = "okay"
      end

      def test_something
        assert_equal "hi mom!", foo
      end
    end

    Object.const_set(:ATestCase, tc)

    @tu.run %w[--seed 42]

    expected = "Run options: --seed 42

# Running tests:

.

Finished tests in 0.00

1 tests, 1 assertions, 0 failures, 0 errors, 0 skips
"
    assert_report expected
  end

  def test_run_error
    tc = Class.new(MiniTest::Unit::TestCase) do
      def test_something
        assert true
      end

      def test_error
        raise "unhandled exception"
      end
    end

    Object.const_set(:ATestCase, tc)

    @tu.run %w[--seed 42]

    expected = "Run options: --seed 42

# Running tests:

E.

Finished tests in 0.00

  1) Error:
test_error(ATestCase):
RuntimeError: unhandled exception
    FILE:LINE:in `test_error'

2 tests, 1 assertions, 0 failures, 1 errors, 0 skips
"
    assert_report expected
  end

  def test_run_error_teardown
    tc = Class.new(MiniTest::Unit::TestCase) do
      def test_something
        assert true
      end

      def teardown
        raise "unhandled exception"
      end
    end

    Object.const_set(:ATestCase, tc)

    @tu.run %w[--seed 42]

    expected = "Run options: --seed 42

# Running tests:

E

Finished tests in 0.00

  1) Error:
test_something(ATestCase):
RuntimeError: unhandled exception
    FILE:LINE:in `teardown'

1 tests, 1 assertions, 0 failures, 1 errors, 0 skips
"
    assert_report expected
  end

  def test_run_failing
    tc = Class.new(MiniTest::Unit::TestCase) do
      def test_something
        assert true
      end

      def test_failure
        assert false
      end
    end

    Object.const_set(:ATestCase, tc)

    @tu.run %w[--seed 42]

    expected = "Run options: --seed 42

# Running tests:

F.

Finished tests in 0.00

  1) Failure:
test_failure(ATestCase) [FILE:LINE]:
Failed assertion, no message given.

2 tests, 2 assertions, 1 failures, 0 errors, 0 skips
"
    assert_report expected
  end

  def test_run_failing_filtered
    tc = Class.new(MiniTest::Unit::TestCase) do
      def test_something
        assert true
      end

      def test_failure
        assert false
      end
    end

    Object.const_set(:ATestCase, tc)

    @tu.run %w[--name /some|thing/ --seed 42]

    expected = "Run options: --name \"/some|thing/\" --seed 42

# Running tests:

.

Finished tests in 0.00

1 tests, 1 assertions, 0 failures, 0 errors, 0 skips
"
    assert_report expected
  end

  def test_run_passing
    tc = Class.new(MiniTest::Unit::TestCase) do
      def test_something
        assert true
      end
    end

    Object.const_set(:ATestCase, tc)

    @tu.run %w[--seed 42]

    assert_report
  end

  def test_run_skip
    tc = Class.new(MiniTest::Unit::TestCase) do
      def test_something
        assert true
      end

      def test_skip
        skip "not yet"
      end
    end

    Object.const_set(:ATestCase, tc)

    @tu.run %w[--seed 42]

    expected = "Run options: --seed 42

# Running tests:

S.

Finished tests in 0.00

2 tests, 1 assertions, 0 failures, 0 errors, 1 skips
"
    assert_report expected
  end

  def test_run_skip_verbose
    tc = Class.new(MiniTest::Unit::TestCase) do
      def test_something
        assert true
      end

      def test_skip
        skip "not yet"
      end
    end

    Object.const_set(:ATestCase, tc)

    @tu.run %w[--seed 42 --verbose]

    expected = "Run options: --seed 42 --verbose

# Running tests:

ATestCase#test_skip = 0.00 s = S
ATestCase#test_something = 0.00 s = .


Finished tests in 0.00

  1) Skipped:
test_skip(ATestCase) [FILE:LINE]:
not yet

2 tests, 1 assertions, 0 failures, 0 errors, 1 skips
"
    assert_report expected
  end

  def test_default_runner_is_minitest_unit
    assert_instance_of MiniTest::Unit, MiniTest::Unit.runner
  end

  def test_run_with_other_runner

    runner = Class.new(MiniTest::Unit) do
      # Run once before each suite
      def _run_suite(suite, type)
        begin
          suite.before_suite
          super(suite, type)
        end
      end
    end

    tc = Class.new(MiniTest::Unit::TestCase) do

      def self.before_suite
        MiniTest::Unit.output.puts "Running #{self.name} tests"
        @@foo = 1
      end

      def test_something
        assert_equal 1, @@foo
      end

      def test_something_else
        assert_equal 1, @@foo
      end
    end

    Object.const_set(:ATestCase, tc)
    MiniTest::Unit.runner = runner.new
    @tu.run %w[--seed 42]

    # We should only see 'running ATestCase tests' once
    expected = "Run options: --seed 42

# Running tests:

Running ATestCase tests
..

Finished tests in 0.00

2 tests, 2 assertions, 0 failures, 0 errors, 0 skips
"
    assert_report expected
  end

  def with_overridden_include
    Class.class_eval do
      def inherited_with_hacks klass
        throw :inherited_hook
      end

      alias inherited_without_hacks inherited
      alias inherited               inherited_with_hacks
      alias IGNORE_ME!              inherited # 1.8 bug. god I love venture bros
    end

    yield

  ensure
    Class.class_eval do
      alias inherited inherited_without_hacks

      undef_method :inherited_with_hacks
      undef_method :inherited_without_hacks
    end

    refute_respond_to Class, :inherited_with_hacks
    refute_respond_to Class, :inherited_without_hacks
  end

  def test_inherited_hook_plays_nice_with_others
    with_overridden_include do
      assert_throws :inherited_hook do
        Class.new MiniTest::Unit::TestCase
      end
    end
  end

  def test_setup_hooks
    call_order = []

    tc = Class.new(MiniTest::Unit::TestCase) do
      define_method :setup do
        super()
        call_order << :method
      end

      define_method :test2 do
        call_order << :test2
      end

      define_method :test1 do
        call_order << :test1
      end
    end

    tc.add_setup_hook lambda { call_order << :proc }

    argument = nil

    tc.add_setup_hook do |arg|
      argument = arg
      call_order << :block
    end

    @tu.run %w[--seed 42]

    assert_kind_of tc, argument

    expected = [:method, :proc, :block, :test1,
                :method, :proc, :block, :test2]

    assert_equal expected, call_order
  end

  def test_teardown_hooks
    call_order = []

    tc = Class.new(MiniTest::Unit::TestCase) do
      define_method :teardown do
        super()
        call_order << :method
      end

      define_method :test2 do
        call_order << :test2
      end

      define_method :test1 do
        call_order << :test1
      end
    end

    tc.add_teardown_hook lambda { call_order << :proc }

    argument = nil

    tc.add_teardown_hook do |arg|
      argument = arg
      call_order << :block
    end

    @tu.run %w[--seed 42]

    assert_kind_of tc, argument

    expected = [:test1, :block, :proc, :method,
                :test2, :block, :proc, :method]

    assert_equal expected, call_order
  end

  def test_setup_and_teardown_hooks_survive_inheritance
    call_order = []

    parent = Class.new(MiniTest::Unit::TestCase) do
      define_method :setup do
        super()
        call_order << :setup_method
      end

      define_method :teardown do
        super()
        call_order << :teardown_method
      end

      define_method :test_something do
        call_order << :test
      end
    end

    parent.add_setup_hook     { call_order << :setup_hook }
    parent.add_teardown_hook  { call_order << :teardown_hook }

    _ = Class.new parent

    parent.add_setup_hook     { call_order << :setup_after }
    parent.add_teardown_hook  { call_order << :teardown_after }

    @tu.run %w[--seed 42]

    # Once for the parent class, once for the child
    expected = [:setup_method, :setup_hook, :setup_after, :test,
                :teardown_after, :teardown_hook, :teardown_method] * 2

    assert_equal expected, call_order
  end

  def util_expand_bt bt
    if RUBY_VERSION >= '1.9.0' then
      bt.map { |f| (f =~ /^\./) ? File.expand_path(f) : f }
    else
      bt
    end
  end
end

class TestMiniTestUnitTestCase < MiniTest::Unit::TestCase
  def setup
    MiniTest::Unit::TestCase.reset

    @tc = MiniTest::Unit::TestCase.new 'fake tc'
    @zomg = "zomg ponies!"
    @assertion_count = 1
  end

  def teardown
    assert_equal(@assertion_count, @tc._assertions,
                 "expected #{@assertion_count} assertions to be fired during the test, not #{@tc._assertions}") if @tc._assertions
    Object.send :remove_const, :ATestCase if defined? ATestCase
  end

  def test_assert
    @assertion_count = 2

    @tc.assert_equal true, @tc.assert(true), "returns true on success"
  end

  def test_assert__triggered
    util_assert_triggered "Failed assertion, no message given." do
      @tc.assert false
    end
  end

  def test_assert__triggered_message
    util_assert_triggered @zomg do
      @tc.assert false, @zomg
    end
  end

  def test_assert_block
    @tc.assert_block do
      true
    end
  end

  def test_assert_block_triggered
    util_assert_triggered "blah.\nExpected block to return true value." do
      @tc.assert_block "blah" do
        false
      end
    end
  end

  def test_assert_empty
    @assertion_count = 2

    @tc.assert_empty []
  end

  def test_assert_empty_triggered
    @assertion_count = 2

    util_assert_triggered "Expected [1] to be empty." do
      @tc.assert_empty [1]
    end
  end

  def test_assert_equal
    @tc.assert_equal 1, 1
  end

  def test_assert_equal_different_diff_deactivated
    without_diff do
      util_assert_triggered util_msg("haha" * 10, "blah" * 10) do
        o1 = "haha" * 10
        o2 = "blah" * 10

        @tc.assert_equal o1, o2
      end
    end
  end

  def test_assert_equal_different_hex
    c = Class.new do
      def initialize s; @name = s; end
    end

    o1 = c.new "a"
    o2 = c.new "b"
    msg = "--- expected
           +++ actual
           @@ -1 +1 @@
           -#<#<Class:0xXXXXXX>:0xXXXXXX @name=\"a\">
           +#<#<Class:0xXXXXXX>:0xXXXXXX @name=\"b\">
           ".gsub(/^ +/, "")

    util_assert_triggered msg do
      @tc.assert_equal o1, o2
    end
  end

  def test_assert_equal_different_hex_invisible
    o1 = Object.new
    o2 = Object.new

    msg = "No visible difference.
           You should look at your implementation of Object#==.
           #<Object:0xXXXXXX>".gsub(/^ +/, "")

    util_assert_triggered msg do
      @tc.assert_equal o1, o2
    end
  end

  def test_assert_equal_different_long
    msg = "--- expected
           +++ actual
           @@ -1 +1 @@
           -\"hahahahahahahahahahahahahahahahahahahaha\"
           +\"blahblahblahblahblahblahblahblahblahblah\"
           ".gsub(/^ +/, "")

    util_assert_triggered msg do
      o1 = "haha" * 10
      o2 = "blah" * 10

      @tc.assert_equal o1, o2
    end
  end

  def test_assert_equal_different_long_invisible
    msg = "No visible difference.
           You should look at your implementation of String#==.
           \"blahblahblahblahblahblahblahblahblahblah\"".gsub(/^ +/, "")

    util_assert_triggered msg do
      o1 = "blah" * 10
      o2 = "blah" * 10
      def o1.== o
        false
      end
      @tc.assert_equal o1, o2
    end
  end

  def test_assert_equal_different_long_msg
    msg = "message.
           --- expected
           +++ actual
           @@ -1 +1 @@
           -\"hahahahahahahahahahahahahahahahahahahaha\"
           +\"blahblahblahblahblahblahblahblahblahblah\"
           ".gsub(/^ +/, "")

    util_assert_triggered msg do
      o1 = "haha" * 10
      o2 = "blah" * 10
      @tc.assert_equal o1, o2, "message"
    end
  end

  def test_assert_equal_different_short
    util_assert_triggered util_msg(1, 2) do
      @tc.assert_equal 1, 2
    end
  end

  def test_assert_equal_different_short_msg
    util_assert_triggered util_msg(1, 2, "message") do
      @tc.assert_equal 1, 2, "message"
    end
  end

  def test_assert_equal_different_short_multiline
    msg = "--- expected\n+++ actual\n@@ -1,2 +1,2 @@\n \"a\n-b\"\n+c\"\n"
    util_assert_triggered msg do
      @tc.assert_equal "a\nb", "a\nc"
    end
  end

  def test_assert_in_delta
    @tc.assert_in_delta 0.0, 1.0 / 1000, 0.1
  end

  def test_assert_in_delta_triggered
    util_assert_triggered 'Expected 0.0 - 0.001 (0.001) to be < 1.0e-06.' do
      @tc.assert_in_delta 0.0, 1.0 / 1000, 0.000001
    end
  end

  def test_assert_in_epsilon
    @assertion_count = 8

    @tc.assert_in_epsilon 10000, 9991
    @tc.assert_in_epsilon 9991, 10000
    @tc.assert_in_epsilon 1.0, 1.001
    @tc.assert_in_epsilon 1.001, 1.0

    @tc.assert_in_epsilon 10000, 9999.1, 0.0001
    @tc.assert_in_epsilon 9999.1, 10000, 0.0001
    @tc.assert_in_epsilon 1.0, 1.0001, 0.0001
    @tc.assert_in_epsilon 1.0001, 1.0, 0.0001
  end

  def test_assert_in_epsilon_triggered
    util_assert_triggered 'Expected 10000 - 9990 (10) to be < 9.99.' do
      @tc.assert_in_epsilon 10000, 9990
    end
  end

  def test_assert_includes
    @assertion_count = 2

    @tc.assert_includes [true], true
  end

  def test_assert_includes_triggered
    @assertion_count = 3

    e = @tc.assert_raises MiniTest::Assertion do
      @tc.assert_includes [true], false
    end

    expected = "Expected [true] to include false."
    assert_equal expected, e.message
  end

  def test_assert_instance_of
    @tc.assert_instance_of String, "blah"
  end

  def test_assert_instance_of_triggered
    util_assert_triggered 'Expected "blah" to be an instance of Array, not String.' do
      @tc.assert_instance_of Array, "blah"
    end
  end

  def test_assert_kind_of
    @tc.assert_kind_of String, "blah"
  end

  def test_assert_kind_of_triggered
    util_assert_triggered 'Expected "blah" to be a kind of Array, not String.' do
      @tc.assert_kind_of Array, "blah"
    end
  end

  def test_assert_match
    @assertion_count = 2
    @tc.assert_match(/\w+/, "blah blah blah")
  end

  def test_assert_match_object
    @assertion_count = 2

    pattern = Object.new
    def pattern.=~(other) true end

    @tc.assert_match pattern, 5
  end

  def test_assert_match_object_triggered
    @assertion_count = 2

    pattern = Object.new
    def pattern.=~(other) false end
    def pattern.inspect; "[Object]" end

    util_assert_triggered 'Expected [Object] to match 5.' do
      @tc.assert_match pattern, 5
    end
  end

  def test_assert_match_triggered
    @assertion_count = 2
    util_assert_triggered 'Expected /\d+/ to match "blah blah blah".' do
      @tc.assert_match(/\d+/, "blah blah blah")
    end
  end

  def test_assert_nil
    @tc.assert_nil nil
  end

  def test_assert_nil_triggered
    util_assert_triggered 'Expected 42 to be nil.' do
      @tc.assert_nil 42
    end
  end

  def test_assert_operator
    @tc.assert_operator 2, :>, 1
  end

  def test_assert_operator_bad_object
    bad = Object.new
    def bad.==(other) true end

    @tc.assert_operator bad, :equal?, bad
  end

  def test_assert_operator_triggered
    util_assert_triggered "Expected 2 to be < 1." do
      @tc.assert_operator 2, :<, 1
    end
  end

  def test_assert_output_both
    @assertion_count = 2

    @tc.assert_output "yay", "blah" do
      print "yay"
      $stderr.print "blah"
    end
  end

  def test_assert_output_err
    @tc.assert_output nil, "blah" do
      $stderr.print "blah"
    end
  end

  def test_assert_output_neither
    @assertion_count = 0

    @tc.assert_output do
      # do nothing
    end
  end

  def test_assert_output_out
    @tc.assert_output "blah" do
      print "blah"
    end
  end

  def test_assert_output_triggered_both
    util_assert_triggered util_msg("yay", "boo", "In stdout") do
      @tc.assert_output "yay", "blah" do
        print "boo"
        $stderr.print "blah blah"
      end
    end
  end

  def test_assert_output_triggered_err
    util_assert_triggered util_msg("blah", "blah blah", "In stderr") do
      @tc.assert_output nil, "blah" do
        $stderr.print "blah blah"
      end
    end
  end

  def test_assert_output_triggered_out
    util_assert_triggered util_msg("blah", "blah blah", "In stdout") do
      @tc.assert_output "blah" do
        print "blah blah"
      end
    end
  end

  def test_assert_raises
    @tc.assert_raises RuntimeError do
      raise "blah"
    end
  end

  ##
  # *sigh* This is quite an odd scenario, but it is from real (albeit
  # ugly) test code in ruby-core:
  #
  # http://svn.ruby-lang.org/cgi-bin/viewvc.cgi?view=rev&revision=29259

  def test_assert_raises_skip
    @assertion_count = 0

    util_assert_triggered "skipped", MiniTest::Skip do
      @tc.assert_raises ArgumentError do
        begin
          raise "blah"
        rescue
          skip "skipped"
        end
      end
    end
  end

  def test_assert_raises_module
    @tc.assert_raises MyModule do
      raise AnError
    end
  end

  def test_assert_raises_triggered_different
    e = assert_raises MiniTest::Assertion do
      @tc.assert_raises RuntimeError do
        raise SyntaxError, "icky"
      end
    end

    expected = "[RuntimeError] exception expected, not
Class: <SyntaxError>
Message: <\"icky\">
---Backtrace---
FILE:LINE:in `test_assert_raises_triggered_different'
---------------"

    actual = e.message.gsub(/^.+:\d+/, 'FILE:LINE')
    actual.gsub!(/block \(\d+ levels\) in /, '') if RUBY_VERSION >= '1.9.0'

    assert_equal expected, actual
  end

  def test_assert_raises_triggered_different_msg
    e = assert_raises MiniTest::Assertion do
      @tc.assert_raises RuntimeError, "XXX" do
        raise SyntaxError, "icky"
      end
    end

    expected = "XXX
[RuntimeError] exception expected, not
Class: <SyntaxError>
Message: <\"icky\">
---Backtrace---
FILE:LINE:in `test_assert_raises_triggered_different_msg'
---------------"

    actual = e.message.gsub(/^.+:\d+/, 'FILE:LINE')
    actual.gsub!(/block \(\d+ levels\) in /, '') if RUBY_VERSION >= '1.9.0'

    assert_equal expected, actual
  end

  def test_assert_raises_triggered_none
    e = assert_raises MiniTest::Assertion do
      @tc.assert_raises MiniTest::Assertion do
        # do nothing
      end
    end

    expected = "MiniTest::Assertion expected but nothing was raised."

    assert_equal expected, e.message
  end

  def test_assert_raises_triggered_none_msg
    e = assert_raises MiniTest::Assertion do
      @tc.assert_raises MiniTest::Assertion, "XXX" do
        # do nothing
      end
    end

    expected = "XXX\nMiniTest::Assertion expected but nothing was raised."

    assert_equal expected, e.message
  end

  def test_assert_raises_triggered_subclass
    e = assert_raises MiniTest::Assertion do
      @tc.assert_raises StandardError do
        raise AnError
      end
    end

    expected = "[StandardError] exception expected, not
Class: <AnError>
Message: <\"AnError\">
---Backtrace---
FILE:LINE:in `test_assert_raises_triggered_subclass'
---------------"

    actual = e.message.gsub(/^.+:\d+/, 'FILE:LINE')
    actual.gsub!(/block \(\d+ levels\) in /, '') if RUBY_VERSION >= '1.9.0'

    assert_equal expected, actual
  end

  def test_assert_respond_to
    @tc.assert_respond_to "blah", :empty?
  end

  def test_assert_respond_to_triggered
    util_assert_triggered 'Expected "blah" (String) to respond to #rawr!.' do
      @tc.assert_respond_to "blah", :rawr!
    end
  end

  def test_assert_same
    @assertion_count = 3

    o = "blah"
    @tc.assert_same 1, 1
    @tc.assert_same :blah, :blah
    @tc.assert_same o, o
  end

  def test_assert_same_triggered
    @assertion_count = 2

    util_assert_triggered 'Expected 2 (oid=N) to be the same as 1 (oid=N).' do
      @tc.assert_same 1, 2
    end

    s1 = "blah"
    s2 = "blah"

    util_assert_triggered 'Expected "blah" (oid=N) to be the same as "blah" (oid=N).' do
      @tc.assert_same s1, s2
    end
  end

  def test_assert_send
    @tc.assert_send [1, :<, 2]
  end

  def test_assert_send_bad
    util_assert_triggered "Expected 1.>(*[2]) to return true." do
      @tc.assert_send [1, :>, 2]
    end
  end

  def test_assert_silent
    @assertion_count = 2

    @tc.assert_silent do
      # do nothing
    end
  end

  def test_assert_silent_triggered_err
    @assertion_count = 2

    util_assert_triggered util_msg("", "blah blah", "In stderr") do
      @tc.assert_silent do
        $stderr.print "blah blah"
      end
    end
  end

  def test_assert_silent_triggered_out
    util_assert_triggered util_msg("", "blah blah", "In stdout") do
      @tc.assert_silent do
        print "blah blah"
      end
    end
  end

  def test_assert_throws
    @tc.assert_throws(:blah) do
      throw :blah
    end
  end

  def test_assert_throws_different
    util_assert_triggered 'Expected :blah to have been thrown, not :not_blah.' do
      @tc.assert_throws(:blah) do
        throw :not_blah
      end
    end
  end

  def test_assert_throws_unthrown
    util_assert_triggered 'Expected :blah to have been thrown.' do
      @tc.assert_throws(:blah) do
        # do nothing
      end
    end
  end

  def test_capture_io
    @assertion_count = 0

    orig_verbose = $VERBOSE
    $VERBOSE = false
    out, err = capture_io do
      puts 'hi'
      warn 'bye!'
    end

    assert_equal "hi\n", out
    assert_equal "bye!\n", err
  ensure
    $VERBOSE = orig_verbose
  end

  def test_class_asserts_match_refutes
    @assertion_count = 0

    methods = MiniTest::Assertions.public_instance_methods
    methods.map! { |m| m.to_s } if Symbol === methods.first

    ignores = %w(assert_block assert_no_match assert_not_equal
                 assert_not_nil assert_not_same assert_nothing_raised
                 assert_nothing_thrown assert_output assert_raise
                 assert_raises assert_send assert_silent assert_throws)

    asserts = methods.grep(/^assert/).sort - ignores
    refutes = methods.grep(/^refute/).sort - ignores

    assert_empty refutes.map { |n| n.sub(/^refute/, 'assert') } - asserts
    assert_empty asserts.map { |n| n.sub(/^assert/, 'refute') } - refutes
  end

  def test_class_inherited
    @assertion_count = 0

    Object.const_set(:ATestCase, Class.new(MiniTest::Unit::TestCase))

    assert_equal [ATestCase], MiniTest::Unit::TestCase.test_suites
  end

  def test_class_test_suites
    @assertion_count = 0

    Object.const_set(:ATestCase, Class.new(MiniTest::Unit::TestCase))

    assert_equal 1, MiniTest::Unit::TestCase.test_suites.size
    assert_equal [ATestCase], MiniTest::Unit::TestCase.test_suites
  end

  def test_expectation
    @assertion_count = 2

    @tc.assert_equal true, 1.must_equal(1)
  end

  def test_expectation_triggered
    util_assert_triggered "Expected: 2\n  Actual: 1" do
      1.must_equal 2
    end
  end

  def test_flunk
    util_assert_triggered 'Epic Fail!' do
      @tc.flunk
    end
  end

  def test_flunk_message
    util_assert_triggered @zomg do
      @tc.flunk @zomg
    end
  end

  def test_message
    @assertion_count = 0

    assert_equal "blah2.",         @tc.message          { "blah2" }.call
    assert_equal "blah2.",         @tc.message("")      { "blah2" }.call
    assert_equal "blah1.\nblah2.", @tc.message(:blah1)  { "blah2" }.call
    assert_equal "blah1.\nblah2.", @tc.message("blah1") { "blah2" }.call
  end

  def test_pass
    @tc.pass
  end

  def test_prints
    printer = Class.new { extend MiniTest::Assertions }
    @tc.assert_equal '"test"', printer.mu_pp(ImmutableString.new 'test')
  end

  def test_refute
    @assertion_count = 2

    @tc.assert_equal false, @tc.refute(false), "returns false on success"
  end

  def test_refute_empty
    @assertion_count = 2

    @tc.refute_empty [1]
  end

  def test_refute_empty_triggered
    @assertion_count = 2

    util_assert_triggered "Expected [] to not be empty." do
      @tc.refute_empty []
    end
  end

  def test_refute_equal
    @tc.refute_equal "blah", "yay"
  end

  def test_refute_equal_triggered
    util_assert_triggered 'Expected "blah" to not be equal to "blah".' do
      @tc.refute_equal "blah", "blah"
    end
  end

  def test_refute_in_delta
    @tc.refute_in_delta 0.0, 1.0 / 1000, 0.000001
  end

  def test_refute_in_delta_triggered
    util_assert_triggered 'Expected 0.0 - 0.001 (0.001) to not be < 0.1.' do
      @tc.refute_in_delta 0.0, 1.0 / 1000, 0.1
    end
  end

  def test_refute_in_epsilon
    @tc.refute_in_epsilon 10000, 9990
  end

  def test_refute_in_epsilon_triggered
    util_assert_triggered 'Expected 10000 - 9991 (9) to not be < 10.0.' do
      @tc.refute_in_epsilon 10000, 9991
      fail
    end
  end

  def test_refute_includes
    @assertion_count = 2

    @tc.refute_includes [true], false
  end

  def test_refute_includes_triggered
    @assertion_count = 3

    e = @tc.assert_raises MiniTest::Assertion do
      @tc.refute_includes [true], true
    end

    expected = "Expected [true] to not include true."
    assert_equal expected, e.message
  end

  def test_refute_instance_of
    @tc.refute_instance_of Array, "blah"
  end

  def test_refute_instance_of_triggered
    util_assert_triggered 'Expected "blah" to not be an instance of String.' do
      @tc.refute_instance_of String, "blah"
    end
  end

  def test_refute_kind_of
    @tc.refute_kind_of Array, "blah"
  end

  def test_refute_kind_of_triggered
    util_assert_triggered 'Expected "blah" to not be a kind of String.' do
      @tc.refute_kind_of String, "blah"
    end
  end

  def test_refute_match
    @assertion_count = 2
    @tc.refute_match(/\d+/, "blah blah blah")
  end

  def test_refute_match_object
    @assertion_count = 2
    @tc.refute_match Object.new, 5 # default #=~ returns false
  end

  def test_refute_match_object_triggered
    @assertion_count = 2

    pattern = Object.new
    def pattern.=~(other) true end
    def pattern.inspect; "[Object]" end

    util_assert_triggered 'Expected [Object] to not match 5.' do
      @tc.refute_match pattern, 5
    end
  end

  def test_refute_match_triggered
    @assertion_count = 2
    util_assert_triggered 'Expected /\w+/ to not match "blah blah blah".' do
      @tc.refute_match(/\w+/, "blah blah blah")
    end
  end

  def test_refute_nil
    @tc.refute_nil 42
  end

  def test_refute_nil_triggered
    util_assert_triggered 'Expected nil to not be nil.' do
      @tc.refute_nil nil
    end
  end

  def test_refute_operator
    @tc.refute_operator 2, :<, 1
  end

  def test_refute_operator_bad_object
    bad = Object.new
    def bad.==(other) true end

    @tc.refute_operator true, :equal?, bad
  end

  def test_refute_operator_triggered
    util_assert_triggered "Expected 2 to not be > 1." do
      @tc.refute_operator 2, :>, 1
    end
  end

  def test_refute_respond_to
    @tc.refute_respond_to "blah", :rawr!
  end

  def test_refute_respond_to_triggered
    util_assert_triggered 'Expected "blah" to not respond to empty?.' do
      @tc.refute_respond_to "blah", :empty?
    end
  end

  def test_refute_same
    @tc.refute_same 1, 2
  end

  def test_refute_same_triggered
    util_assert_triggered 'Expected 1 (oid=N) to not be the same as 1 (oid=N).' do
      @tc.refute_same 1, 1
    end
  end

  def test_skip
    @assertion_count = 0

    util_assert_triggered "haha!", MiniTest::Skip do
      @tc.skip "haha!"
    end
  end

  def test_test_methods_random
    @assertion_count = 0

    sample_test_case = Class.new(MiniTest::Unit::TestCase) do
      def test_test1; assert "does not matter" end
      def test_test2; assert "does not matter" end
      def test_test3; assert "does not matter" end
    end

    srand 42
    expected = %w(test_test2 test_test1 test_test3)
    assert_equal expected, sample_test_case.test_methods
  end

  def test_test_methods_sorted
    @assertion_count = 0

    sample_test_case = Class.new(MiniTest::Unit::TestCase) do
      def self.test_order; :sorted end
      def test_test3; assert "does not matter" end
      def test_test2; assert "does not matter" end
      def test_test1; assert "does not matter" end
    end

    expected = %w(test_test1 test_test2 test_test3)
    assert_equal expected, sample_test_case.test_methods
  end

  def util_assert_triggered expected, klass = MiniTest::Assertion
    e = assert_raises(klass) do
      yield
    end

    msg = e.message.sub(/(---Backtrace---).*/m, '\1')
    msg.gsub!(/\(oid=[-0-9]+\)/, '(oid=N)')

    assert_equal expected, msg
  end

  def util_msg exp, act, msg = nil
    s = "Expected: #{exp.inspect}\n  Actual: #{act.inspect}"
    s = "#{msg}.\n#{s}" if msg
    s
  end

  def without_diff
    old_diff = MiniTest::Assertions.diff
    MiniTest::Assertions.diff = nil

    yield
  ensure
    MiniTest::Assertions.diff = old_diff
  end
end
