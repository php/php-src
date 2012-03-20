require 'test/unit'
require 'tempfile'
require 'thread'
require_relative 'envutil'

class TestAutoload < Test::Unit::TestCase
  def test_autoload_so
    # Continuation is always available, unless excluded intentionally.
    assert_in_out_err([], <<-INPUT, [], [])
    autoload :Continuation, "continuation"
    begin Continuation; rescue LoadError; end
    INPUT
  end

  def test_non_realpath_in_loadpath
    require 'tmpdir'
    tmpdir = Dir.mktmpdir('autoload')
    tmpdirs = [tmpdir]
    tmpdirs.unshift(tmpdir + '/foo')
    Dir.mkdir(tmpdirs[0])
    tmpfiles = [tmpdir + '/foo.rb', tmpdir + '/foo/bar.rb']
    open(tmpfiles[0] , 'w') do |f|
      f.puts <<-INPUT
$:.unshift(File.expand_path('..', __FILE__)+'/./foo')
module Foo
  autoload :Bar, 'bar'
end
p Foo::Bar
      INPUT
    end
    open(tmpfiles[1], 'w') do |f|
      f.puts 'class Foo::Bar; end'
    end
    assert_in_out_err([tmpfiles[0]], "", ["Foo::Bar"], [])
  ensure
    File.unlink(*tmpfiles) rescue nil if tmpfiles
    tmpdirs.each {|dir| Dir.rmdir(dir)}
  end

  def test_autoload_p
    bug4565 = '[ruby-core:35679]'

    require 'tmpdir'
    Dir.mktmpdir('autoload') {|tmpdir|
      tmpfile = tmpdir + '/foo.rb'
      a = Module.new do
        autoload :X, tmpfile
      end
      b = Module.new do
        include a
      end
      assert_equal(true, a.const_defined?(:X))
      assert_equal(true, b.const_defined?(:X))
      assert_equal(tmpfile, a.autoload?(:X), bug4565)
      assert_equal(tmpfile, b.autoload?(:X), bug4565)
    }
  end

  def test_require_explicit
    file = Tempfile.open(['autoload', '.rb'])
    file.puts 'class Object; AutoloadTest = 1; end'
    file.close
    add_autoload(file.path)
    begin
      assert_nothing_raised do
        assert(require file.path)
        assert_equal(1, ::AutoloadTest)
      end
    ensure
      remove_autoload_constant
    end
  end

  def test_threaded_accessing_constant
    file = Tempfile.open(['autoload', '.rb'])
    file.puts 'sleep 0.5; class AutoloadTest; X = 1; end'
    file.close
    add_autoload(file.path)
    begin
      assert_nothing_raised do
        t1 = Thread.new { ::AutoloadTest::X }
        t2 = Thread.new { ::AutoloadTest::X }
        [t1, t2].each(&:join)
      end
    ensure
      remove_autoload_constant
    end
  end

  def test_threaded_accessing_inner_constant
    file = Tempfile.open(['autoload', '.rb'])
    file.puts 'class AutoloadTest; sleep 0.5; X = 1; end'
    file.close
    add_autoload(file.path)
    begin
      assert_nothing_raised do
        t1 = Thread.new { ::AutoloadTest::X }
        t2 = Thread.new { ::AutoloadTest::X }
        [t1, t2].each(&:join)
      end
    ensure
      remove_autoload_constant
    end
  end

  def test_nameerror_when_autoload_did_not_define_the_constant
    file = Tempfile.open(['autoload', '.rb'])
    file.puts ''
    file.close
    add_autoload(file.path)
    begin
      assert_raise(NameError) do
        AutoloadTest
      end
    ensure
      remove_autoload_constant
    end
  end

  def test_override_autoload
    file = Tempfile.open(['autoload', '.rb'])
    file.puts ''
    file.close
    add_autoload(file.path)
    begin
      eval %q(class AutoloadTest; end)
      assert_equal(Class, AutoloadTest.class)
    ensure
      remove_autoload_constant
    end
  end

  def test_override_while_autoloading
    file = Tempfile.open(['autoload', '.rb'])
    file.puts 'class AutoloadTest; sleep 0.5; end'
    file.close
    add_autoload(file.path)
    begin
      # while autoloading...
      t = Thread.new { AutoloadTest }
      sleep 0.1
      # override it
      EnvUtil.suppress_warning {
        eval %q(AutoloadTest = 1)
      }
      t.join
      assert_equal(1, AutoloadTest)
    ensure
      remove_autoload_constant
    end
  end

  def add_autoload(path)
    eval <<-END
      class ::Object
        autoload :AutoloadTest, #{path.dump}
      end
    END
  end

  def remove_autoload_constant
    eval <<-END
      class ::Object
        remove_const(:AutoloadTest)
      end
    END
  end
end
