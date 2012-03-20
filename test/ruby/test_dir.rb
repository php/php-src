require 'test/unit'

require 'tmpdir'
require 'fileutils'
require 'pathname'

class TestDir < Test::Unit::TestCase

  def setup
    @verbose = $VERBOSE
    $VERBOSE = nil
    @root = Pathname.new(Dir.mktmpdir('__test_dir__')).realpath.to_s
    @nodir = File.join(@root, "dummy")
    for i in ?a..?z
      if i.ord % 2 == 0
        FileUtils.touch(File.join(@root, i))
      else
        FileUtils.mkdir(File.join(@root, i))
      end
    end
  end

  def teardown
    $VERBOSE = @verbose
    FileUtils.remove_entry_secure @root if File.directory?(@root)
  end

  def test_seek
    dir = Dir.open(@root)
    begin
      cache = []
      loop do
        pos = dir.tell
        break unless name = dir.read
        cache << [pos, name]
      end
      for x,y in cache.sort_by {|z| z[0] % 3 } # shuffle
        dir.seek(x)
        assert_equal(y, dir.read)
      end
    ensure
      dir.close
    end
  end

  def test_JVN_13947696
    b = lambda {
      d = Dir.open('.')
      $SAFE = 4
      d.close
    }
    assert_raise(SecurityError) { b.call }
  end

  def test_nodir
    assert_raise(Errno::ENOENT) { Dir.open(@nodir) }
  end

  def test_inspect
    d = Dir.open(@root)
    assert_match(/^#<Dir:#{ Regexp.quote(@root) }>$/, d.inspect)
    assert_match(/^#<Dir:.*>$/, Dir.allocate.inspect)
  ensure
    d.close
  end

  def test_path
    d = Dir.open(@root)
    assert_equal(@root, d.path)
    assert_nil(Dir.allocate.path)
  ensure
    d.close
  end

  def test_set_pos
    d = Dir.open(@root)
    loop do
      i = d.pos
      break unless x = d.read
      d.pos = i
      assert_equal(x, d.read)
    end
  ensure
    d.close
  end

  def test_rewind
    d = Dir.open(@root)
    a = (0..5).map { d.read }
    d.rewind
    b = (0..5).map { d.read }
    assert_equal(a, b)
    assert_raise(SecurityError) do
      Thread.new do
        $SAFE = 4
        d.rewind
      end.join
    end
  ensure
    d.close
  end

  def test_chdir
    @pwd = Dir.pwd
    @env_home = ENV["HOME"]
    @env_logdir = ENV["LOGDIR"]
    ENV.delete("HOME")
    ENV.delete("LOGDIR")

    assert_raise(Errno::ENOENT) { Dir.chdir(@nodir) }
    assert_raise(ArgumentError) { Dir.chdir }
    ENV["HOME"] = @pwd
    Dir.chdir do
      assert_equal(@pwd, Dir.pwd)
      Dir.chdir(@root)
      assert_equal(@root, Dir.pwd)
    end

  ensure
    begin
      Dir.chdir(@pwd)
    rescue
      abort("cannot return the original directory: #{ @pwd }")
    end
    if @env_home
      ENV["HOME"] = @env_home
    else
      ENV.delete("HOME")
    end
    if @env_logdir
      ENV["LOGDIR"] = @env_logdir
    else
      ENV.delete("LOGDIR")
    end
  end

  def test_chroot_nodir
    assert_raise(NotImplementedError, Errno::ENOENT, Errno::EPERM
		) { Dir.chroot(File.join(@nodir, "")) }
  end

  def test_close
    d = Dir.open(@root)
    d.close
    assert_raise(IOError) { d.read }
  end

  def test_glob
    assert_equal((%w(. ..) + (?a..?z).to_a).map{|f| File.join(@root, f) },
                 Dir.glob(File.join(@root, "*"), File::FNM_DOTMATCH).sort)
    assert_equal([@root] + (?a..?z).map {|f| File.join(@root, f) }.sort,
                 Dir.glob([@root, File.join(@root, "*")]).sort)
    assert_equal([@root] + (?a..?z).map {|f| File.join(@root, f) }.sort,
                 Dir.glob(@root + "\0\0\0" + File.join(@root, "*")).sort)

    assert_equal((?a..?z).step(2).map {|f| File.join(File.join(@root, f), "") }.sort,
                 Dir.glob(File.join(@root, "*/")).sort)

    FileUtils.touch(File.join(@root, "{}"))
    assert_equal(%w({} a).map{|f| File.join(@root, f) },
                 Dir.glob(File.join(@root, '{\{\},a}')))
    assert_equal([], Dir.glob(File.join(@root, '[')))
    assert_equal([], Dir.glob(File.join(@root, '[a-\\')))

    assert_equal([File.join(@root, "a")], Dir.glob(File.join(@root, 'a\\')))
    assert_equal((?a..?f).map {|f| File.join(@root, f) }.sort, Dir.glob(File.join(@root, '[abc/def]')).sort)

    d = "\u{3042}\u{3044}".encode("utf-16le")
    assert_raise(Encoding::CompatibilityError) {Dir.glob(d)}
    m = Class.new {define_method(:to_path) {d}}
    assert_raise(Encoding::CompatibilityError) {Dir.glob(m.new)}
  end

  def test_foreach
    assert_equal(Dir.foreach(@root).to_a.sort, %w(. ..) + (?a..?z).to_a)
  end

  def test_dir_enc
    dir = Dir.open(@root, encoding: "UTF-8")
    begin
      while name = dir.read
	assert_equal(Encoding.find("UTF-8"), name.encoding)
      end
    ensure
      dir.close
    end

    dir = Dir.open(@root, encoding: "ASCII-8BIT")
    begin
      while name = dir.read
	assert_equal(Encoding.find("ASCII-8BIT"), name.encoding)
      end
    ensure
      dir.close
    end
  end

  def test_symlink
    begin
      ["dummy", *?a..?z].each do |f|
	File.symlink(File.join(@root, f),
		     File.join(@root, "symlink-#{ f }"))
      end
    rescue NotImplementedError
      return
    end

    assert_equal([*?a..?z, *"symlink-a".."symlink-z"].each_slice(2).map {|f, _| File.join(@root, f + "/") }.sort,
		 Dir.glob(File.join(@root, "*/")).sort)

    Dir.glob(File.join(@root, "**/"))
  end

end
