require 'test/unit'
require 'find'
require 'tmpdir'

class TestFind < Test::Unit::TestCase
  def test_empty
    Dir.mktmpdir {|d|
      a = []
      Find.find(d) {|f| a << f }
      assert_equal([d], a)
    }
  end

  def test_rec
    Dir.mktmpdir {|d|
      File.open("#{d}/a", "w"){}
      Dir.mkdir("#{d}/b")
      File.open("#{d}/b/a", "w"){}
      File.open("#{d}/b/b", "w"){}
      Dir.mkdir("#{d}/c")
      a = []
      Find.find(d) {|f| a << f }
      assert_equal([d, "#{d}/a", "#{d}/b", "#{d}/b/a", "#{d}/b/b", "#{d}/c"], a)
    }
  end

  def test_relative
    Dir.mktmpdir {|d|
      File.open("#{d}/a", "w"){}
      Dir.mkdir("#{d}/b")
      File.open("#{d}/b/a", "w"){}
      File.open("#{d}/b/b", "w"){}
      Dir.mkdir("#{d}/c")
      a = []
      Dir.chdir(d) {
        Find.find(".") {|f| a << f }
      }
      assert_equal([".", "./a", "./b", "./b/a", "./b/b", "./c"], a)
    }
  end

  def test_dont_follow_symlink
    Dir.mktmpdir {|d|
      File.open("#{d}/a", "w"){}
      Dir.mkdir("#{d}/b")
      File.open("#{d}/b/a", "w"){}
      File.open("#{d}/b/b", "w"){}
      begin
        File.symlink("#{d}/b", "#{d}/c")
      rescue NotImplementedError
        skip "symlink is not supported."
      end
      a = []
      Find.find(d) {|f| a << f }
      assert_equal([d, "#{d}/a", "#{d}/b", "#{d}/b/a", "#{d}/b/b", "#{d}/c"], a)
    }
  end

  def test_prune
    Dir.mktmpdir {|d|
      File.open("#{d}/a", "w"){}
      Dir.mkdir("#{d}/b")
      File.open("#{d}/b/a", "w"){}
      File.open("#{d}/b/b", "w"){}
      Dir.mkdir("#{d}/c")
      a = []
      Find.find(d) {|f|
        a << f
        Find.prune if f == "#{d}/b"
      }
      assert_equal([d, "#{d}/a", "#{d}/b", "#{d}/c"], a)
    }
  end

  def test_countup3
    Dir.mktmpdir {|d|
      1.upto(3) {|n| File.open("#{d}/#{n}", "w"){} }
      a = []
      Find.find(d) {|f| a << f }
      assert_equal([d, "#{d}/1", "#{d}/2", "#{d}/3"], a)
    }
  end

  def test_countdown3
    Dir.mktmpdir {|d|
      3.downto(1) {|n| File.open("#{d}/#{n}", "w"){} }
      a = []
      Find.find(d) {|f| a << f }
      assert_equal([d, "#{d}/1", "#{d}/2", "#{d}/3"], a)
    }
  end

  def test_unreadable_dir
    skip "no meaning test on Windows" if /mswin|mingw/ =~ RUBY_PLATFORM
    Dir.mktmpdir {|d|
      Dir.mkdir(dir = "#{d}/dir")
      File.open(file = "#{dir}/foo", "w"){}
      begin
        File.chmod(0300, dir)
        a = []
        Find.find(d) {|f| a << f }
        assert_equal([d, dir], a)
      ensure
        File.chmod(0700, dir)
      end
    }
  end

  def test_unsearchable_dir
    Dir.mktmpdir {|d|
      Dir.mkdir(dir = "#{d}/dir")
      File.open(file = "#{dir}/foo", "w"){}
      begin
        File.chmod(0600, dir)
        a = []
        Find.find(d) {|f| a << f }
        assert_equal([d, dir, file], a)
        skip "no meaning test on Windows" if /mswin|mingw/ =~ RUBY_PLATFORM
        assert_raise(Errno::EACCES) { File.lstat(file) }
      ensure
        File.chmod(0700, dir)
      end
    }
  end

  def test_dangling_symlink
    Dir.mktmpdir {|d|
      begin
        File.symlink("foo", "#{d}/bar")
      rescue NotImplementedError
        skip "symlink is not supported."
      end
      a = []
      Find.find(d) {|f| a << f }
      assert_equal([d, "#{d}/bar"], a)
      assert_raise(Errno::ENOENT) { File.stat("#{d}/bar") }
    }
  end

  def test_dangling_symlink_stat_error
    Dir.mktmpdir {|d|
      begin
        File.symlink("foo", "#{d}/bar")
      rescue NotImplementedError
        skip "symlink is not supported."
      end
      assert_raise(Errno::ENOENT) {
        Find.find(d) {|f| File.stat(f) }
      }
    }
  end

  def test_change_dir_to_file
    Dir.mktmpdir {|d|
      Dir.mkdir(dir_1 = "#{d}/d1")
      File.open(file_a = "#{dir_1}/a", "w"){}
      File.open(file_b = "#{dir_1}/b", "w"){}
      File.open(file_c = "#{dir_1}/c", "w"){}
      Dir.mkdir(dir_d = "#{dir_1}/d")
      File.open(file_de = "#{dir_d}/e", "w"){}
      dir_2 = "#{d}/d2"
      a = []
      Find.find(d) {|f|
        a << f
        if f == file_b
          File.rename(dir_1, dir_2)
          File.open(dir_1, "w") {}
        end
      }
      assert_equal([d, dir_1, file_a, file_b, file_c, dir_d], a)
    }
  end

  def test_change_dir_to_symlink_loop
    Dir.mktmpdir {|d|
      Dir.mkdir(dir_1 = "#{d}/d1")
      File.open(file_a = "#{dir_1}/a", "w"){}
      File.open(file_b = "#{dir_1}/b", "w"){}
      File.open(file_c = "#{dir_1}/c", "w"){}
      Dir.mkdir(dir_d = "#{dir_1}/d")
      File.open(file_de = "#{dir_d}/e", "w"){}
      dir_2 = "#{d}/d2"
      a = []
      Find.find(d) {|f|
        a << f
        if f == file_b
          File.rename(dir_1, dir_2)
          begin
            File.symlink("d1", dir_1)
          rescue NotImplementedError
            skip "symlink is not supported."
          end
        end
      }
      assert_equal([d, dir_1, file_a, file_b, file_c, dir_d], a)
    }
  end

  def test_enumerator
    Dir.mktmpdir {|d|
      File.open("#{d}/a", "w"){}
      Dir.mkdir("#{d}/b")
      File.open("#{d}/b/a", "w"){}
      File.open("#{d}/b/b", "w"){}
      Dir.mkdir("#{d}/c")
      e = Find.find(d)
      a = []
      e.each {|f| a << f }
      assert_equal([d, "#{d}/a", "#{d}/b", "#{d}/b/a", "#{d}/b/b", "#{d}/c"], a)
    }
  end

  class TestInclude < Test::Unit::TestCase
    include Find

    def test_functional_call
      Dir.mktmpdir {|d|
        File.open("#{d}/a", "w"){}
        a = []
        find(d) {|f| a << f }
        assert_equal([d, "#{d}/a"], a)
      }
    end
  end

end
