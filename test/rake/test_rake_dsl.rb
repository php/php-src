require File.expand_path('../helper', __FILE__)

class TestRakeDsl < Rake::TestCase

  def setup
    super
    Rake::Task.clear
  end

  def test_namespace_command
    namespace "n" do
      task "t"
    end
    refute_nil Rake::Task["n:t"]
  end

  def test_namespace_command_with_bad_name
    ex = assert_raises(ArgumentError) do
      namespace 1 do end
    end
    assert_match(/string/i, ex.message)
    assert_match(/symbol/i, ex.message)
  end

  def test_namespace_command_with_a_string_like_object
    name = Object.new
    def name.to_str
      "bob"
    end
    namespace name do
      task "t"
    end
    refute_nil Rake::Task["bob:t"]
  end

  class Foo
    def initialize
      task :foo_deprecated_a => "foo_deprecated_b" do
        print "a"
      end
      file "foo_deprecated_b" do
        print "b"
      end
    end
  end

  def test_deprecated_object_dsl
    out, err = capture_io do
      Foo.new
      Rake.application.invoke_task :foo_deprecated_a
    end
    assert_equal("ba", out)
    assert_match(/deprecated/, err)
    assert_match(/Foo\#task/, err)
    assert_match(/Foo\#file/, err)
    assert_match(/test_rake_dsl\.rb:\d+/, err)
  end

  def test_no_commands_constant
    assert ! defined?(Commands), "should not define Commands"
  end

  def test_deprecated_object_dsl_with_suppressed_warnings
    Rake.application.options.ignore_deprecate = true
    out, err = capture_io do
      Foo.new
      Rake.application.invoke_task :foo_deprecated_a
    end
    assert_equal("ba", out)
    refute_match(/deprecated/, err)
    refute_match(/Foo\#task/, err)
    refute_match(/Foo\#file/, err)
    refute_match(/test_rake_dsl\.rb:\d+/, err)
  ensure
    Rake.application.options.ignore_deprecate = false
  end
end
