require 'test/unit'
require 'tmpdir'
require_relative 'ruby/envutil'

class TestTracer < Test::Unit::TestCase
  include EnvUtil

  def test_tracer_with_option_r
    assert_in_out_err(%w[-rtracer -e 1]) do |(*lines),|
      case lines.size
      when 2
        assert_match(%r{rubygems/custom_require\.rb:\d+:Kernel:<:}, lines[0])
      when 1
        # do nothing
      else
        flunk "unexpected output from `ruby -rtracer -e 1`"
      end
      assert_equal "#0:-e:1::-: 1", lines.last
    end
  end

  def test_tracer_with_option_r_without_gems
    assert_in_out_err(%w[--disable-gems -rtracer -e 1]) do |(*lines),|
      case lines.size
      when 1
        # do nothing
      else
        flunk "unexpected output from `ruby --disable-gems -rtracer -e 1`"
      end
      assert_equal "#0:-e:1::-: 1", lines.last
    end
  end

  def test_tracer_with_require
    Dir.mktmpdir("test_ruby_tracer") do |dir|
      script = File.join(dir, "require_tracer.rb")
      open(script, "w") do |f|
        f.print <<-EOF
require 'tracer'
1
        EOF
      end
      assert_in_out_err([script]) do |(*lines),|
        assert_empty(lines)
      end
    end
  end

  def test_tracer_with_require_without_gems
    Dir.mktmpdir("test_ruby_tracer") do |dir|
      script = File.join(dir, "require_tracer.rb")
      open(script, "w") do |f|
        f.print <<-EOF
require 'tracer'
1
        EOF
      end
      assert_in_out_err(["--disable-gems", script]) do |(*lines),|
        assert_empty(lines)
      end
    end
  end
end
