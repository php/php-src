require 'test/unit'
require 'tempfile'
require_relative 'ruby/envutil'

class TestTempfile < Test::Unit::TestCase
  def initialize(*)
    super
    @tempfile = nil
  end

  def tempfile(*args, &block)
    t = Tempfile.new(*args, &block)
    @tempfile = (t unless block)
  end

  def teardown
    if @tempfile
      @tempfile.close!
    end
  end

  def test_basic
    t = tempfile("foo")
    path = t.path
    t.write("hello world")
    t.close
    assert_equal "hello world", File.read(path)
  end

  def test_saves_in_dir_tmpdir_by_default
    t = tempfile("foo")
    assert_equal Dir.tmpdir, File.dirname(t.path)
    bug3733 = '[ruby-dev:42089]'
    assert_nothing_raised(SecurityError, bug3733) {
      proc {$SAFE = 1; File.expand_path(Dir.tmpdir)}.call
    }
  end

  def test_saves_in_given_directory
    subdir = File.join(Dir.tmpdir, "tempfile-test-#{rand}")
    Dir.mkdir(subdir)
    begin
      tempfile = Tempfile.new("foo", subdir)
      tempfile.close
      begin
        assert_equal subdir, File.dirname(tempfile.path)
      ensure
        tempfile.unlink
      end
    ensure
      Dir.rmdir(subdir)
    end
  end

  def test_basename
    t = tempfile("foo")
    assert_match(/^foo/, File.basename(t.path))
  end

  def test_basename_with_suffix
    t = tempfile(["foo", ".txt"])
    assert_match(/^foo/, File.basename(t.path))
    assert_match(/\.txt$/, File.basename(t.path))
  end

  def test_unlink
    t = tempfile("foo")
    path = t.path

    t.close
    assert File.exist?(path)

    t.unlink
    assert !File.exist?(path)

    assert_nil t.path
  end

  def test_unlink_silently_fails_on_windows
    tempfile = tempfile("foo")
    path = tempfile.path
    begin
      assert_nothing_raised do
        tempfile.unlink
      end
    ensure
      tempfile.close
      File.unlink(path) if File.exist?(path)
    end
  end

  def test_unlink_before_close_works_on_posix_systems
    tempfile = tempfile("foo")
    begin
      path = tempfile.path
      tempfile.unlink
      assert !File.exist?(path)
      tempfile.write("hello ")
      tempfile.write("world\n")
      tempfile.rewind
      assert_equal "hello world\n", tempfile.read
    ensure
      tempfile.close
      tempfile.unlink
    end
  end unless /mswin|mingw/ =~ RUBY_PLATFORM

  def test_close_and_close_p
    t = tempfile("foo")
    assert !t.closed?
    t.close
    assert t.closed?
  end

  def test_close_with_unlink_now_true_works
    t = tempfile("foo")
    path = t.path
    t.close(true)
    assert t.closed?
    assert_nil t.path
    assert !File.exist?(path)
  end

  def test_close_with_unlink_now_true_does_not_unlink_if_already_unlinked
    t = tempfile("foo")
    path = t.path
    t.unlink
    File.open(path, "w").close
    begin
      t.close(true)
      assert File.exist?(path)
    ensure
      File.unlink(path) rescue nil
    end
  end unless /mswin|mingw/ =~ RUBY_PLATFORM

  def test_close_bang_works
    t = tempfile("foo")
    path = t.path
    t.close!
    assert t.closed?
    assert_nil t.path
    assert !File.exist?(path)
  end

  def test_close_bang_does_not_unlink_if_already_unlinked
    t = tempfile("foo")
    path = t.path
    t.unlink
    File.open(path, "w").close
    begin
      t.close!
      assert File.exist?(path)
    ensure
      File.unlink(path) rescue nil
    end
  end unless /mswin|mingw/ =~ RUBY_PLATFORM

  def test_finalizer_does_not_unlink_if_already_unlinked
    assert_in_out_err('-rtempfile', <<-'EOS') do |(filename,*), (error,*)|
file = Tempfile.new('foo')
path = file.path
puts path
file.close!
File.open(path, "w").close
    EOS
      assert File.exist?(filename)
      File.unlink(filename)
      assert_nil error
    end

    assert_in_out_err('-rtempfile', <<-'EOS') do |(filename,*), (error,*)|
file = Tempfile.new('foo')
path = file.path
file.unlink
puts path
File.open(path, "w").close
    EOS
      if !filename.empty?
        # POSIX unlink semantics supported, continue with test
        assert File.exist?(filename)
        File.unlink(filename)
      end
      assert_nil error
    end
  end unless /mswin|mingw/ =~ RUBY_PLATFORM

  def test_close_does_not_make_path_nil
    t = tempfile("foo")
    t.close
    assert_not_nil t.path
  end

  def test_close_flushes_buffer
    t = tempfile("foo")
    t.write("hello")
    t.close
    assert_equal 5, File.size(t.path)
  end

  def test_tempfile_is_unlinked_when_ruby_exits
    assert_in_out_err('-rtempfile', <<-'EOS') do |(filename), (error)|
puts Tempfile.new('foo').path
    EOS
      assert !File.exist?(filename)
    end
  end

  def test_size_flushes_buffer_before_determining_file_size
    t = tempfile("foo")
    t.write("hello")
    assert_equal 0, File.size(t.path)
    assert_equal 5, t.size
    assert_equal 5, File.size(t.path)
  end

  def test_size_works_if_file_is_closed
    t = tempfile("foo")
    t.write("hello")
    t.close
    assert_equal 5, t.size
  end

  def test_concurrency
    threads = []
    tempfiles = []
    lock = Mutex.new
    cond = ConditionVariable.new
    start = false

    4.times do
      threads << Thread.new do
        lock.synchronize do
          while !start
            cond.wait(lock)
          end
        end
        result = []
        30.times do
          result << Tempfile.new('foo')
        end
        Thread.current[:result] = result
      end
    end

    lock.synchronize do
      start = true
      cond.broadcast
    end
    threads.each do |thread|
      thread.join
      tempfiles |= thread[:result]
    end
    filenames = tempfiles.map { |f| f.path }
    begin
      assert_equal filenames.size, filenames.uniq.size
    ensure
      tempfiles.each do |tempfile|
        tempfile.close!
      end
    end
  end

  module M
  end

  def test_extend
    o = tempfile("foo")
    o.extend M
    assert(M === o, "[ruby-dev:32932]")
  end

  def test_tempfile_encoding_nooption
    default_external=Encoding.default_external
    t = tempfile("TEST")
    t.write("\xE6\x9D\xBE\xE6\xB1\x9F")
    t.rewind
    assert_equal(default_external,t.read.encoding)
  end

  def test_tempfile_encoding_ascii8bit
    default_external=Encoding.default_external
    t = tempfile("TEST",:encoding=>"ascii-8bit")
    t.write("\xE6\x9D\xBE\xE6\xB1\x9F")
    t.rewind
    assert_equal(Encoding::ASCII_8BIT,t.read.encoding)
  end

  def test_tempfile_encoding_ascii8bit2
    default_external=Encoding.default_external
    t = tempfile("TEST",Dir::tmpdir,:encoding=>"ascii-8bit")
    t.write("\xE6\x9D\xBE\xE6\xB1\x9F")
    t.rewind
    assert_equal(Encoding::ASCII_8BIT,t.read.encoding)
  end

  def test_binmode
    t = tempfile("TEST", mode: IO::BINARY)
    if IO::BINARY.nonzero?
      assert(t.binmode?)
      t.open
      assert(t.binmode?, 'binmode after reopen')
    else
      assert_equal(0600, t.stat.mode & 0777)
    end
  end
end

