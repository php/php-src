require 'test/unit'
require 'tmpdir'

class TestNotImplement < Test::Unit::TestCase
  def test_respond_to_fork
    assert_include(Process.methods, :fork)
    if /linux/ =~ RUBY_PLATFORM
      assert_equal(true, Process.respond_to?(:fork))
    end
  end

  def test_respond_to_lchmod
    assert_include(File.methods, :lchmod)
    if /linux/ =~ RUBY_PLATFORM
      assert_equal(false, File.respond_to?(:lchmod))
    end
    if /freebsd/ =~ RUBY_PLATFORM
      assert_equal(true, File.respond_to?(:lchmod))
    end
  end

  def test_call_fork
    if Process.respond_to?(:fork)
      assert_nothing_raised {
        pid = fork {}
        Process.wait pid
      }
    end
  end

  def test_call_lchmod
    if File.respond_to?(:lchmod)
      Dir.mktmpdir {|d|
        f = "#{d}/f"
        g = "#{d}/g"
        File.open(f, "w") {}
        File.symlink f, g
        newmode = 0444
        File.lchmod newmode, "#{d}/g"
        snew = File.lstat(g)
        assert_equal(newmode, snew.mode & 0777)
      }
    end
  end

  def test_method_inspect_fork
    m = Process.method(:fork)
    if Process.respond_to?(:fork)
      assert_not_match(/not-implemented/, m.inspect)
    else
      assert_match(/not-implemented/, m.inspect)
    end
  end

  def test_method_inspect_lchmod
    m = File.method(:lchmod)
    if File.respond_to?(:lchmod)
      assert_not_match(/not-implemented/, m.inspect)
    else
      assert_match(/not-implemented/, m.inspect)
    end
  end

end
