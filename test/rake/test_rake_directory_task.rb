require File.expand_path('../helper', __FILE__)
require 'fileutils'

class TestRakeDirectoryTask < Rake::TestCase
  include Rake

  def test_directory
    desc "DESC"

    directory "a/b/c"

    assert_equal FileCreationTask, Task["a"].class
    assert_equal FileCreationTask, Task["a/b"].class
    assert_equal FileCreationTask, Task["a/b/c"].class

    assert_nil             Task["a"].comment
    assert_nil             Task["a/b"].comment
    assert_equal "DESC",   Task["a/b/c"].comment

    verbose(false) {
      Task['a/b'].invoke
    }

    assert File.exist?("a/b")
    refute File.exist?("a/b/c")
  end

  if Rake::Win32.windows?
    def test_directory_win32
      drive = Dir.pwd
      while drive != File.dirname(drive)
        drive = File.dirname(drive)
      end
      drive = drive[0...-1] if drive[-1] == ?/

      desc "WIN32 DESC"
      directory File.join(Dir.pwd, 'a/b/c')
      assert_equal FileTask, Task[drive].class if drive[-1] == ?:
      assert_equal FileCreationTask, Task[File.join(Dir.pwd, 'a')].class
      assert_equal FileCreationTask, Task[File.join(Dir.pwd, 'a/b')].class
      assert_equal FileCreationTask, Task[File.join(Dir.pwd, 'a/b/c')].class
      assert_nil             Task[drive].comment
      assert_equal "WIN32 DESC",   Task[File.join(Dir.pwd, 'a/b/c')].comment
      assert_nil             Task[File.join(Dir.pwd, 'a/b')].comment
      verbose(false) {
        Task[File.join(Dir.pwd, 'a/b')].invoke
      }
      assert File.exist?(File.join(Dir.pwd, 'a/b'))
      refute File.exist?(File.join(Dir.pwd, 'a/b/c'))
    end
  end
end
