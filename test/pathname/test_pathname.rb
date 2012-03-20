require 'test/unit'
require 'pathname'

require 'fileutils'
require 'tmpdir'
require 'enumerator'

require_relative '../ruby/envutil'

class TestPathname < Test::Unit::TestCase
  def self.define_assertion(name, linenum, &block)
    name = "test_#{name}_#{linenum}"
    define_method(name, &block)
  end

  def self.get_linenum
    if /:(\d+):/ =~ caller[1]
      $1.to_i
    else
      nil
    end
  end

  def self.defassert(name, result, *args)
    define_assertion(name, get_linenum) {
      mesg = "#{name}(#{args.map {|a| a.inspect }.join(', ')})"
      assert_nothing_raised(mesg) {
        assert_equal(result, self.send(name, *args), mesg)
      }
    }
  end

  def self.defassert_raise(name, exc, *args)
    define_assertion(name, get_linenum) {
      message = "#{name}(#{args.map {|a| a.inspect }.join(', ')})"
      assert_raise(exc, message) { self.send(name, *args) }
    }
  end

  DOSISH = File::ALT_SEPARATOR != nil
  DOSISH_DRIVE_LETTER = File.dirname("A:") == "A:."
  DOSISH_UNC = File.dirname("//") == "//"

  def cleanpath_aggressive(path)
    Pathname.new(path).cleanpath.to_s
  end

  defassert(:cleanpath_aggressive, '/',       '/')
  defassert(:cleanpath_aggressive, '.',       '')
  defassert(:cleanpath_aggressive, '.',       '.')
  defassert(:cleanpath_aggressive, '..',      '..')
  defassert(:cleanpath_aggressive, 'a',       'a')
  defassert(:cleanpath_aggressive, '/',       '/.')
  defassert(:cleanpath_aggressive, '/',       '/..')
  defassert(:cleanpath_aggressive, '/a',      '/a')
  defassert(:cleanpath_aggressive, '.',       './')
  defassert(:cleanpath_aggressive, '..',      '../')
  defassert(:cleanpath_aggressive, 'a',       'a/')
  defassert(:cleanpath_aggressive, 'a/b',     'a//b')
  defassert(:cleanpath_aggressive, 'a',       'a/.')
  defassert(:cleanpath_aggressive, 'a',       'a/./')
  defassert(:cleanpath_aggressive, '.',       'a/..')
  defassert(:cleanpath_aggressive, '.',       'a/../')
  defassert(:cleanpath_aggressive, '/a',      '/a/.')
  defassert(:cleanpath_aggressive, '..',      './..')
  defassert(:cleanpath_aggressive, '..',      '../.')
  defassert(:cleanpath_aggressive, '..',      './../')
  defassert(:cleanpath_aggressive, '..',      '.././')
  defassert(:cleanpath_aggressive, '/',       '/./..')
  defassert(:cleanpath_aggressive, '/',       '/../.')
  defassert(:cleanpath_aggressive, '/',       '/./../')
  defassert(:cleanpath_aggressive, '/',       '/.././')
  defassert(:cleanpath_aggressive, 'a/b/c',   'a/b/c')
  defassert(:cleanpath_aggressive, 'b/c',     './b/c')
  defassert(:cleanpath_aggressive, 'a/c',     'a/./c')
  defassert(:cleanpath_aggressive, 'a/b',     'a/b/.')
  defassert(:cleanpath_aggressive, '.',       'a/../.')
  defassert(:cleanpath_aggressive, '/a',      '/../.././../a')
  defassert(:cleanpath_aggressive, '../../d', 'a/b/../../../../c/../d')

  if DOSISH_UNC
    defassert(:cleanpath_aggressive, '//a/b/c', '//a/b/c/')
  else
    defassert(:cleanpath_aggressive, '/',       '///')
    defassert(:cleanpath_aggressive, '/a',      '///a')
    defassert(:cleanpath_aggressive, '/',       '///..')
    defassert(:cleanpath_aggressive, '/',       '///.')
    defassert(:cleanpath_aggressive, '/',       '///a/../..')
  end

  def cleanpath_conservative(path)
    Pathname.new(path).cleanpath(true).to_s
  end

  defassert(:cleanpath_conservative, '/',      '/')
  defassert(:cleanpath_conservative, '.',      '')
  defassert(:cleanpath_conservative, '.',      '.')
  defassert(:cleanpath_conservative, '..',     '..')
  defassert(:cleanpath_conservative, 'a',      'a')
  defassert(:cleanpath_conservative, '/',      '/.')
  defassert(:cleanpath_conservative, '/',      '/..')
  defassert(:cleanpath_conservative, '/a',     '/a')
  defassert(:cleanpath_conservative, '.',      './')
  defassert(:cleanpath_conservative, '..',     '../')
  defassert(:cleanpath_conservative, 'a/',     'a/')
  defassert(:cleanpath_conservative, 'a/b',    'a//b')
  defassert(:cleanpath_conservative, 'a/.',    'a/.')
  defassert(:cleanpath_conservative, 'a/.',    'a/./')
  defassert(:cleanpath_conservative, 'a/..',   'a/../')
  defassert(:cleanpath_conservative, '/a/.',   '/a/.')
  defassert(:cleanpath_conservative, '..',     './..')
  defassert(:cleanpath_conservative, '..',     '../.')
  defassert(:cleanpath_conservative, '..',     './../')
  defassert(:cleanpath_conservative, '..',     '.././')
  defassert(:cleanpath_conservative, '/',      '/./..')
  defassert(:cleanpath_conservative, '/',      '/../.')
  defassert(:cleanpath_conservative, '/',      '/./../')
  defassert(:cleanpath_conservative, '/',      '/.././')
  defassert(:cleanpath_conservative, 'a/b/c',  'a/b/c')
  defassert(:cleanpath_conservative, 'b/c',    './b/c')
  defassert(:cleanpath_conservative, 'a/c',    'a/./c')
  defassert(:cleanpath_conservative, 'a/b/.',  'a/b/.')
  defassert(:cleanpath_conservative, 'a/..',   'a/../.')
  defassert(:cleanpath_conservative, '/a',     '/../.././../a')
  defassert(:cleanpath_conservative, 'a/b/../../../../c/../d', 'a/b/../../../../c/../d')

  if DOSISH_UNC
    defassert(:cleanpath_conservative, '//',     '//')
  else
    defassert(:cleanpath_conservative, '/',      '//')
  end

  # has_trailing_separator?(path) -> bool
  def has_trailing_separator?(path)
    Pathname.allocate.__send__(:has_trailing_separator?, path)
  end

  defassert(:has_trailing_separator?, false, "/")
  defassert(:has_trailing_separator?, false, "///")
  defassert(:has_trailing_separator?, false, "a")
  defassert(:has_trailing_separator?, true, "a/")

  def add_trailing_separator(path)
    Pathname.allocate.__send__(:add_trailing_separator, path)
  end

  def del_trailing_separator(path)
    Pathname.allocate.__send__(:del_trailing_separator, path)
  end

  defassert(:del_trailing_separator, "/", "/")
  defassert(:del_trailing_separator, "/a", "/a")
  defassert(:del_trailing_separator, "/a", "/a/")
  defassert(:del_trailing_separator, "/a", "/a//")
  defassert(:del_trailing_separator, ".", ".")
  defassert(:del_trailing_separator, ".", "./")
  defassert(:del_trailing_separator, ".", ".//")

  if DOSISH_DRIVE_LETTER
    defassert(:del_trailing_separator, "A:", "A:")
    defassert(:del_trailing_separator, "A:/", "A:/")
    defassert(:del_trailing_separator, "A:/", "A://")
    defassert(:del_trailing_separator, "A:.", "A:.")
    defassert(:del_trailing_separator, "A:.", "A:./")
    defassert(:del_trailing_separator, "A:.", "A:.//")
  end

  if DOSISH_UNC
    defassert(:del_trailing_separator, "//", "//")
    defassert(:del_trailing_separator, "//a", "//a")
    defassert(:del_trailing_separator, "//a", "//a/")
    defassert(:del_trailing_separator, "//a", "//a//")
    defassert(:del_trailing_separator, "//a/b", "//a/b")
    defassert(:del_trailing_separator, "//a/b", "//a/b/")
    defassert(:del_trailing_separator, "//a/b", "//a/b//")
    defassert(:del_trailing_separator, "//a/b/c", "//a/b/c")
    defassert(:del_trailing_separator, "//a/b/c", "//a/b/c/")
    defassert(:del_trailing_separator, "//a/b/c", "//a/b/c//")
  else
    defassert(:del_trailing_separator, "/", "///")
    defassert(:del_trailing_separator, "///a", "///a/")
  end

  if DOSISH
    defassert(:del_trailing_separator, "a", "a\\")
    defassert(:del_trailing_separator, "\225\\".force_encoding("cp932"), "\225\\\\".force_encoding("cp932"))
    defassert(:del_trailing_separator, "\225".force_encoding("cp437"), "\225\\\\".force_encoding("cp437"))
  end

  def test_plus
    assert_kind_of(Pathname, Pathname("a") + Pathname("b"))
  end

  def plus(path1, path2) # -> path
    (Pathname.new(path1) + Pathname.new(path2)).to_s
  end

  defassert(:plus, '/', '/', '/')
  defassert(:plus, 'a/b', 'a', 'b')
  defassert(:plus, 'a', 'a', '.')
  defassert(:plus, 'b', '.', 'b')
  defassert(:plus, '.', '.', '.')
  defassert(:plus, '/b', 'a', '/b')

  defassert(:plus, '/', '/', '..')
  defassert(:plus, '.', 'a', '..')
  defassert(:plus, 'a', 'a/b', '..')
  defassert(:plus, '../..', '..', '..')
  defassert(:plus, '/c', '/', '../c')
  defassert(:plus, 'c', 'a', '../c')
  defassert(:plus, 'a/c', 'a/b', '../c')
  defassert(:plus, '../../c', '..', '../c')

  defassert(:plus, 'a//b/d//e', 'a//b/c', '../d//e')

  def test_parent
    assert_equal(Pathname("."), Pathname("a").parent)
  end

  def parent(path) # -> path
    Pathname.new(path).parent.to_s
  end

  defassert(:parent, '/', '/')
  defassert(:parent, '/', '/a')
  defassert(:parent, '/a', '/a/b')
  defassert(:parent, '/a/b', '/a/b/c')
  defassert(:parent, '.', 'a')
  defassert(:parent, 'a', 'a/b')
  defassert(:parent, 'a/b', 'a/b/c')
  defassert(:parent, '..', '.')
  defassert(:parent, '../..', '..')

  def test_join
    r = Pathname("a").join(Pathname("b"), Pathname("c"))
    assert_equal(Pathname("a/b/c"), r)
  end

  def test_absolute
    assert_equal(true, Pathname("/").absolute?)
    assert_equal(false, Pathname("a").absolute?)
  end

  def relative?(path)
    Pathname.new(path).relative?
  end

  defassert(:relative?, false, '/')
  defassert(:relative?, false, '/a')
  defassert(:relative?, false, '/..')
  defassert(:relative?, true, 'a')
  defassert(:relative?, true, 'a/b')

  if DOSISH_DRIVE_LETTER
    defassert(:relative?, false, 'A:')
    defassert(:relative?, false, 'A:/')
    defassert(:relative?, false, 'A:/a')
  end

  if File.dirname('//') == '//'
    defassert(:relative?, false, '//')
    defassert(:relative?, false, '//a')
    defassert(:relative?, false, '//a/')
    defassert(:relative?, false, '//a/b')
    defassert(:relative?, false, '//a/b/')
    defassert(:relative?, false, '//a/b/c')
  end

  def relative_path_from(dest_directory, base_directory)
    Pathname.new(dest_directory).relative_path_from(Pathname.new(base_directory)).to_s
  end

  defassert(:relative_path_from, "../a", "a", "b")
  defassert(:relative_path_from, "../a", "a", "b/")
  defassert(:relative_path_from, "../a", "a/", "b")
  defassert(:relative_path_from, "../a", "a/", "b/")
  defassert(:relative_path_from, "../a", "/a", "/b")
  defassert(:relative_path_from, "../a", "/a", "/b/")
  defassert(:relative_path_from, "../a", "/a/", "/b")
  defassert(:relative_path_from, "../a", "/a/", "/b/")

  defassert(:relative_path_from, "../b", "a/b", "a/c")
  defassert(:relative_path_from, "../a", "../a", "../b")

  defassert(:relative_path_from, "a", "a", ".")
  defassert(:relative_path_from, "..", ".", "a")

  defassert(:relative_path_from, ".", ".", ".")
  defassert(:relative_path_from, ".", "..", "..")
  defassert(:relative_path_from, "..", "..", ".")

  defassert(:relative_path_from, "c/d", "/a/b/c/d", "/a/b")
  defassert(:relative_path_from, "../..", "/a/b", "/a/b/c/d")
  defassert(:relative_path_from, "../../../../e", "/e", "/a/b/c/d")
  defassert(:relative_path_from, "../b/c", "a/b/c", "a/d")

  defassert(:relative_path_from, "../a", "/../a", "/b")
  defassert(:relative_path_from, "../../a", "../a", "b")
  defassert(:relative_path_from, ".", "/a/../../b", "/b")
  defassert(:relative_path_from, "..", "a/..", "a")
  defassert(:relative_path_from, ".", "a/../b", "b")

  defassert(:relative_path_from, "a", "a", "b/..")
  defassert(:relative_path_from, "b/c", "b/c", "b/..")

  defassert_raise(:relative_path_from, ArgumentError, "/", ".")
  defassert_raise(:relative_path_from, ArgumentError, ".", "/")
  defassert_raise(:relative_path_from, ArgumentError, "a", "..")
  defassert_raise(:relative_path_from, ArgumentError, ".", "..")

  def with_tmpchdir(base=nil)
    Dir.mktmpdir(base) {|d|
      d = Pathname.new(d).realpath.to_s
      Dir.chdir(d) {
        yield d
      }
    }
  end

  def has_symlink?
    begin
      File.symlink(nil, nil)
    rescue NotImplementedError
      return false
    rescue TypeError
    end
    return true
  end

  def realpath(path, basedir=nil)
    Pathname.new(path).realpath(basedir).to_s
  end

  def test_realpath
    return if !has_symlink?
    with_tmpchdir('rubytest-pathname') {|dir|
      assert_raise(Errno::ENOENT) { realpath("#{dir}/not-exist") }
      File.symlink("not-exist-target", "#{dir}/not-exist")
      assert_raise(Errno::ENOENT) { realpath("#{dir}/not-exist") }

      File.symlink("loop", "#{dir}/loop")
      assert_raise(Errno::ELOOP) { realpath("#{dir}/loop") }
      assert_raise(Errno::ELOOP) { realpath("#{dir}/loop", dir) }

      File.symlink("../#{File.basename(dir)}/./not-exist-target", "#{dir}/not-exist2")
      assert_raise(Errno::ENOENT) { realpath("#{dir}/not-exist2") }

      File.open("#{dir}/exist-target", "w") {}
      File.symlink("../#{File.basename(dir)}/./exist-target", "#{dir}/exist2")
      assert_nothing_raised { realpath("#{dir}/exist2") }

      File.symlink("loop-relative", "loop-relative")
      assert_raise(Errno::ELOOP) { realpath("#{dir}/loop-relative") }

      Dir.mkdir("exist")
      assert_equal("#{dir}/exist", realpath("exist"))
      assert_raise(Errno::ELOOP) { realpath("../loop", "#{dir}/exist") }

      File.symlink("loop1/loop1", "loop1")
      assert_raise(Errno::ELOOP) { realpath("#{dir}/loop1") }

      File.symlink("loop2", "loop3")
      File.symlink("loop3", "loop2")
      assert_raise(Errno::ELOOP) { realpath("#{dir}/loop2") }

      Dir.mkdir("b")

      File.symlink("b", "c")
      assert_equal("#{dir}/b", realpath("c"))
      assert_equal("#{dir}/b", realpath("c/../c"))
      assert_equal("#{dir}/b", realpath("c/../c/../c/."))

      File.symlink("..", "b/d")
      assert_equal("#{dir}/b", realpath("c/d/c/d/c"))

      File.symlink("#{dir}/b", "e")
      assert_equal("#{dir}/b", realpath("e"))

      Dir.mkdir("f")
      Dir.mkdir("f/g")
      File.symlink("f/g", "h")
      assert_equal("#{dir}/f/g", realpath("h"))
      File.chmod(0000, "f")
      assert_raise(Errno::EACCES) { realpath("h") }
      File.chmod(0755, "f")
    }
  end

  def realdirpath(path)
    Pathname.new(path).realdirpath.to_s
  end

  def test_realdirpath
    return if !has_symlink?
    Dir.mktmpdir('rubytest-pathname') {|dir|
      rdir = realpath(dir)
      assert_equal("#{rdir}/not-exist", realdirpath("#{dir}/not-exist"))
      assert_raise(Errno::ENOENT) { realdirpath("#{dir}/not-exist/not-exist-child") }
      File.symlink("not-exist-target", "#{dir}/not-exist")
      assert_equal("#{rdir}/not-exist-target", realdirpath("#{dir}/not-exist"))
      File.symlink("../#{File.basename(dir)}/./not-exist-target", "#{dir}/not-exist2")
      assert_equal("#{rdir}/not-exist-target", realdirpath("#{dir}/not-exist2"))
      File.open("#{dir}/exist-target", "w") {}
      File.symlink("../#{File.basename(dir)}/./exist-target", "#{dir}/exist")
      assert_equal("#{rdir}/exist-target", realdirpath("#{dir}/exist"))
      File.symlink("loop", "#{dir}/loop")
      assert_raise(Errno::ELOOP) { realdirpath("#{dir}/loop") }
    }
  end

  def descend(path)
    Pathname.new(path).enum_for(:descend).map {|v| v.to_s }
  end

  defassert(:descend, %w[/ /a /a/b /a/b/c], "/a/b/c")
  defassert(:descend, %w[a a/b a/b/c], "a/b/c")
  defassert(:descend, %w[. ./a ./a/b ./a/b/c], "./a/b/c")
  defassert(:descend, %w[a/], "a/")

  def ascend(path)
    Pathname.new(path).enum_for(:ascend).map {|v| v.to_s }
  end

  defassert(:ascend, %w[/a/b/c /a/b /a /], "/a/b/c")
  defassert(:ascend, %w[a/b/c a/b a], "a/b/c")
  defassert(:ascend, %w[./a/b/c ./a/b ./a .], "./a/b/c")
  defassert(:ascend, %w[a/], "a/")

  def test_initialize
    p1 = Pathname.new('a')
    assert_equal('a', p1.to_s)
    p2 = Pathname.new(p1)
    assert_equal(p1, p2)
  end

  def test_initialize_nul
    assert_raise(ArgumentError) { Pathname.new("a\0") }
  end

  class AnotherStringLike # :nodoc:
    def initialize(s) @s = s end
    def to_str() @s end
    def ==(other) @s == other end
  end

  def test_equality
    obj = Pathname.new("a")
    str = "a"
    sym = :a
    ano = AnotherStringLike.new("a")
    assert_equal(false, obj == str)
    assert_equal(false, str == obj)
    assert_equal(false, obj == ano)
    assert_equal(false, ano == obj)
    assert_equal(false, obj == sym)
    assert_equal(false, sym == obj)

    obj2 = Pathname.new("a")
    assert_equal(true, obj == obj2)
    assert_equal(true, obj === obj2)
    assert_equal(true, obj.eql?(obj2))
  end

  def test_hashkey
    h = {}
    h[Pathname.new("a")] = 1
    h[Pathname.new("a")] = 2
    assert_equal(1, h.size)
  end

  def assert_pathname_cmp(e, s1, s2)
    p1 = Pathname.new(s1)
    p2 = Pathname.new(s2)
    r = p1 <=> p2
    assert(e == r,
      "#{p1.inspect} <=> #{p2.inspect}: <#{e}> expected but was <#{r}>")
  end
  def test_comparison
    assert_pathname_cmp( 0, "a", "a")
    assert_pathname_cmp( 1, "b", "a")
    assert_pathname_cmp(-1, "a", "b")
    ss = %w(
      a
      a/
      a/b
      a.
      a0
    )
    s1 = ss.shift
    ss.each {|s2|
      assert_pathname_cmp(-1, s1, s2)
      s1 = s2
    }
  end

  def test_comparison_string
    assert_equal(nil, Pathname.new("a") <=> "a")
    assert_equal(nil, "a" <=> Pathname.new("a"))
  end

  def pathsub(path, pat, repl) Pathname.new(path).sub(pat, repl).to_s end
  defassert(:pathsub, "a.o", "a.c", /\.c\z/, ".o")

  def pathsubext(path, repl) Pathname.new(path).sub_ext(repl).to_s end
  defassert(:pathsubext, 'a.o', 'a.c', '.o')
  defassert(:pathsubext, 'a.o', 'a.c++', '.o')
  defassert(:pathsubext, 'a.png', 'a.gif', '.png')
  defassert(:pathsubext, 'ruby.tar.bz2', 'ruby.tar.gz', '.bz2')
  defassert(:pathsubext, 'd/a.o', 'd/a.c', '.o')
  defassert(:pathsubext, 'foo', 'foo.exe', '')
  defassert(:pathsubext, 'lex.yy.o', 'lex.yy.c', '.o')
  defassert(:pathsubext, 'fooaa.o', 'fooaa', '.o')
  defassert(:pathsubext, 'd.e/aa.o', 'd.e/aa', '.o')
  defassert(:pathsubext, 'long_enough.bug-3664', 'long_enough.not_to_be_embeded[ruby-core:31640]', '.bug-3664')

  def test_sub_matchdata
    result = Pathname("abc.gif").sub(/\..*/) {
      assert_not_nil($~)
      assert_equal(".gif", $~[0])
      ".png"
    }
    assert_equal("abc.png", result.to_s)
  end

  def root?(path)
    Pathname.new(path).root?
  end

  defassert(:root?, true, "/")
  defassert(:root?, true, "//")
  defassert(:root?, true, "///")
  defassert(:root?, false, "")
  defassert(:root?, false, "a")

  def test_mountpoint?
    r = Pathname("/").mountpoint?
    assert_include([true, false], r)
  end

  def test_destructive_update
    path = Pathname.new("a")
    path.to_s.replace "b"
    assert_equal(Pathname.new("a"), path)
  end

  def test_null_character
    assert_raise(ArgumentError) { Pathname.new("\0") }
  end

  def test_taint
    obj = Pathname.new("a"); assert_same(obj, obj.taint)
    obj = Pathname.new("a"); assert_same(obj, obj.untaint)

    assert_equal(false, Pathname.new("a"      )           .tainted?)
    assert_equal(false, Pathname.new("a"      )      .to_s.tainted?)
    assert_equal(true,  Pathname.new("a"      ).taint     .tainted?)
    assert_equal(true,  Pathname.new("a"      ).taint.to_s.tainted?)
    assert_equal(true,  Pathname.new("a".taint)           .tainted?)
    assert_equal(true,  Pathname.new("a".taint)      .to_s.tainted?)
    assert_equal(true,  Pathname.new("a".taint).taint     .tainted?)
    assert_equal(true,  Pathname.new("a".taint).taint.to_s.tainted?)

    str = "a"
    path = Pathname.new(str)
    str.taint
    assert_equal(false, path     .tainted?)
    assert_equal(false, path.to_s.tainted?)
  end

  def test_untaint
    obj = Pathname.new("a"); assert_same(obj, obj.untaint)

    assert_equal(false, Pathname.new("a").taint.untaint     .tainted?)
    assert_equal(false, Pathname.new("a").taint.untaint.to_s.tainted?)

    str = "a".taint
    path = Pathname.new(str)
    str.untaint
    assert_equal(true, path     .tainted?)
    assert_equal(true, path.to_s.tainted?)
  end

  def test_freeze
    obj = Pathname.new("a"); assert_same(obj, obj.freeze)

    assert_equal(false, Pathname.new("a"       )            .frozen?)
    assert_equal(false, Pathname.new("a".freeze)            .frozen?)
    assert_equal(true,  Pathname.new("a"       ).freeze     .frozen?)
    assert_equal(true,  Pathname.new("a".freeze).freeze     .frozen?)
    assert_equal(false, Pathname.new("a"       )       .to_s.frozen?)
    assert_equal(false, Pathname.new("a".freeze)       .to_s.frozen?)
    assert_equal(false, Pathname.new("a"       ).freeze.to_s.frozen?)
    assert_equal(false, Pathname.new("a".freeze).freeze.to_s.frozen?)
  end

  def test_freeze_and_taint
    obj = Pathname.new("a")
    obj.freeze
    assert_equal(false, obj.tainted?)
    assert_raise(RuntimeError) { obj.taint }

    obj = Pathname.new("a")
    obj.taint
    assert_equal(true, obj.tainted?)
    obj.freeze
    assert_equal(true, obj.tainted?)
    assert_nothing_raised { obj.taint }
  end

  def test_to_s
    str = "a"
    obj = Pathname.new(str)
    assert_equal(str, obj.to_s)
    assert_not_same(str, obj.to_s)
    assert_not_same(obj.to_s, obj.to_s)
  end

  def test_kernel_open
    count = 0
    result = Kernel.open(Pathname.new(__FILE__)) {|f|
      assert(File.identical?(__FILE__, f))
      count += 1
      2
    }
    assert_equal(1, count)
    assert_equal(2, result)
  end

  def test_each_filename
    result = []
    Pathname.new("/usr/bin/ruby").each_filename {|f| result << f }
    assert_equal(%w[usr bin ruby], result)
    assert_equal(%w[usr bin ruby], Pathname.new("/usr/bin/ruby").each_filename.to_a)
  end

  def test_kernel_pathname
    assert_equal(Pathname.new("a"), Pathname("a"))
  end

  def test_children
    with_tmpchdir('rubytest-pathname') {|dir|
      open("a", "w") {}
      open("b", "w") {}
      Dir.mkdir("d")
      open("d/x", "w") {}
      open("d/y", "w") {}
      assert_equal([Pathname("a"), Pathname("b"), Pathname("d")], Pathname(".").children.sort)
      assert_equal([Pathname("d/x"), Pathname("d/y")], Pathname("d").children.sort)
      assert_equal([Pathname("x"), Pathname("y")], Pathname("d").children(false).sort)
    }
  end

  def test_each_child
    with_tmpchdir('rubytest-pathname') {|dir|
      open("a", "w") {}
      open("b", "w") {}
      Dir.mkdir("d")
      open("d/x", "w") {}
      open("d/y", "w") {}
      a = []; Pathname(".").each_child {|v| a << v }; a.sort!
      assert_equal([Pathname("a"), Pathname("b"), Pathname("d")], a)
      a = []; Pathname("d").each_child {|v| a << v }; a.sort!
      assert_equal([Pathname("d/x"), Pathname("d/y")], a)
      a = []; Pathname("d").each_child(false) {|v| a << v }; a.sort!
      assert_equal([Pathname("x"), Pathname("y")], a)
    }
  end

  def test_each_line
    with_tmpchdir('rubytest-pathname') {|dir|
      open("a", "w") {|f| f.puts 1, 2 }
      a = []
      Pathname("a").each_line {|line| a << line }
      assert_equal(["1\n", "2\n"], a)

      a = []
      Pathname("a").each_line("2") {|line| a << line }
      assert_equal(["1\n2", "\n"], a)

      a = []
      Pathname("a").each_line(1) {|line| a << line }
      assert_equal(["1", "\n", "2", "\n"], a)

      a = []
      Pathname("a").each_line("2", 1) {|line| a << line }
      assert_equal(["1", "\n", "2", "\n"], a)

      a = []
      enum = Pathname("a").each_line
      enum.each {|line| a << line }
      assert_equal(["1\n", "2\n"], a)
    }
  end

  def test_readlines
    with_tmpchdir('rubytest-pathname') {|dir|
      open("a", "w") {|f| f.puts 1, 2 }
      a = Pathname("a").readlines
      assert_equal(["1\n", "2\n"], a)
    }
  end

  def test_read
    with_tmpchdir('rubytest-pathname') {|dir|
      open("a", "w") {|f| f.puts 1, 2 }
      assert_equal("1\n2\n", Pathname("a").read)
    }
  end

  def test_binread
    with_tmpchdir('rubytest-pathname') {|dir|
      open("a", "w") {|f| f.write "abc" }
      str = Pathname("a").binread
      assert_equal("abc", str)
      assert_equal(Encoding::ASCII_8BIT, str.encoding)
    }
  end

  def test_sysopen
    with_tmpchdir('rubytest-pathname') {|dir|
      open("a", "w") {|f| f.write "abc" }
      fd = Pathname("a").sysopen
      io = IO.new(fd)
      begin
        assert_equal("abc", io.read)
      ensure
        io.close
      end
    }
  end

  def test_atime
    assert_kind_of(Time, Pathname(__FILE__).atime)
  end

  def test_ctime
    assert_kind_of(Time, Pathname(__FILE__).ctime)
  end

  def test_mtime
    assert_kind_of(Time, Pathname(__FILE__).mtime)
  end

  def test_chmod
    with_tmpchdir('rubytest-pathname') {|dir|
      open("a", "w") {|f| f.write "abc" }
      path = Pathname("a")
      old = path.stat.mode
      path.chmod(0444)
      assert_equal(0444, path.stat.mode & 0777)
      path.chmod(old)
    }
  end

  def test_lchmod
    return if !has_symlink?
    with_tmpchdir('rubytest-pathname') {|dir|
      open("a", "w") {|f| f.write "abc" }
      File.symlink("a", "l")
      path = Pathname("l")
      old = path.lstat.mode
      begin
        path.lchmod(0444)
      rescue NotImplementedError
        next
      end
      assert_equal(0444, path.lstat.mode & 0777)
      path.chmod(old)
    }
  end

  def test_chown
    with_tmpchdir('rubytest-pathname') {|dir|
      open("a", "w") {|f| f.write "abc" }
      path = Pathname("a")
      old_uid = path.stat.uid
      old_gid = path.stat.gid
      begin
        path.chown(0, 0)
      rescue Errno::EPERM
        next
      end
      assert_equal(0, path.stat.uid)
      assert_equal(0, path.stat.gid)
      path.chown(old_uid, old_gid)
    }
  end

  def test_lchown
    return if !has_symlink?
    with_tmpchdir('rubytest-pathname') {|dir|
      open("a", "w") {|f| f.write "abc" }
      File.symlink("a", "l")
      path = Pathname("l")
      old_uid = path.stat.uid
      old_gid = path.stat.gid
      begin
        path.lchown(0, 0)
      rescue Errno::EPERM
        next
      end
      assert_equal(0, path.stat.uid)
      assert_equal(0, path.stat.gid)
      path.lchown(old_uid, old_gid)
    }
  end

  def test_fnmatch
    path = Pathname("a")
    assert_equal(true, path.fnmatch("*"))
    assert_equal(false, path.fnmatch("*.*"))
    assert_equal(false, Pathname(".foo").fnmatch("*"))
    assert_equal(true, Pathname(".foo").fnmatch("*", File::FNM_DOTMATCH))
  end

  def test_fnmatch?
    path = Pathname("a")
    assert_equal(true, path.fnmatch?("*"))
    assert_equal(false, path.fnmatch?("*.*"))
  end

  def test_ftype
    with_tmpchdir('rubytest-pathname') {|dir|
      open("f", "w") {|f| f.write "abc" }
      assert_equal("file", Pathname("f").ftype)
      Dir.mkdir("d")
      assert_equal("directory", Pathname("d").ftype)
    }
  end

  def test_make_link
    with_tmpchdir('rubytest-pathname') {|dir|
      open("a", "w") {|f| f.write "abc" }
      Pathname("l").make_link(Pathname("a"))
      assert_equal("abc", Pathname("l").read)
    }
  end

  def test_open
    with_tmpchdir('rubytest-pathname') {|dir|
      open("a", "w") {|f| f.write "abc" }
      path = Pathname("a")

      path.open {|f|
        assert_equal("abc", f.read)
      }

      path.open("r") {|f|
        assert_equal("abc", f.read)
      }

      Pathname("b").open("w", 0444) {|f| f.write "def" }
      assert_equal(0444, File.stat("b").mode & 0777)
      assert_equal("def", File.read("b"))

      Pathname("c").open("w", 0444, {}) {|f| f.write "ghi" }
      assert_equal(0444, File.stat("c").mode & 0777)
      assert_equal("ghi", File.read("c"))

      g = path.open
      assert_equal("abc", g.read)
      g.close
    }
  end

  def test_readlink
    return if !has_symlink?
    with_tmpchdir('rubytest-pathname') {|dir|
      open("a", "w") {|f| f.write "abc" }
      File.symlink("a", "l")
      assert_equal(Pathname("a"), Pathname("l").readlink)
    }
  end

  def test_rename
    with_tmpchdir('rubytest-pathname') {|dir|
      open("a", "w") {|f| f.write "abc" }
      Pathname("a").rename(Pathname("b"))
      assert_equal("abc", File.read("b"))
    }
  end

  def test_stat
    with_tmpchdir('rubytest-pathname') {|dir|
      open("a", "w") {|f| f.write "abc" }
      s = Pathname("a").stat
      assert_equal(3, s.size)
    }
  end

  def test_lstat
    return if !has_symlink?
    with_tmpchdir('rubytest-pathname') {|dir|
      open("a", "w") {|f| f.write "abc" }
      File.symlink("a", "l")
      s = Pathname("l").lstat
      assert_equal(true, s.symlink?)
      s = Pathname("l").stat
      assert_equal(false, s.symlink?)
      assert_equal(3, s.size)
      s = Pathname("a").lstat
      assert_equal(false, s.symlink?)
      assert_equal(3, s.size)
    }
  end

  def test_make_symlink
    return if !has_symlink?
    with_tmpchdir('rubytest-pathname') {|dir|
      open("a", "w") {|f| f.write "abc" }
      Pathname("l").make_symlink(Pathname("a"))
      s = Pathname("l").lstat
      assert_equal(true, s.symlink?)
    }
  end

  def test_truncate
    with_tmpchdir('rubytest-pathname') {|dir|
      open("a", "w") {|f| f.write "abc" }
      Pathname("a").truncate(2)
      assert_equal("ab", File.read("a"))
    }
  end

  def test_utime
    with_tmpchdir('rubytest-pathname') {|dir|
      open("a", "w") {|f| f.write "abc" }
      atime = Time.utc(2000)
      mtime = Time.utc(1999)
      Pathname("a").utime(atime, mtime)
      s = File.stat("a")
      assert_equal(atime, s.atime)
      assert_equal(mtime, s.mtime)
    }
  end

  def test_basename
    assert_equal(Pathname("basename"), Pathname("dirname/basename").basename)
    assert_equal(Pathname("bar"), Pathname("foo/bar.x").basename(".x"))
  end

  def test_dirname
    assert_equal(Pathname("dirname"), Pathname("dirname/basename").dirname)
  end

  def test_extname
    assert_equal(".ext", Pathname("basename.ext").extname)
  end

  def test_expand_path
    drv = DOSISH_DRIVE_LETTER ? Dir.pwd.sub(%r(/.*), '') : ""
    assert_equal(Pathname(drv + "/a"), Pathname("/a").expand_path)
    assert_equal(Pathname(drv + "/a"), Pathname("a").expand_path("/"))
    assert_equal(Pathname(drv + "/a"), Pathname("a").expand_path(Pathname("/")))
    assert_equal(Pathname(drv + "/b"), Pathname("/b").expand_path(Pathname("/a")))
    assert_equal(Pathname(drv + "/a/b"), Pathname("b").expand_path(Pathname("/a")))
  end

  def test_split
    assert_equal([Pathname("dirname"), Pathname("basename")], Pathname("dirname/basename").split)
  end

  def test_blockdev?
    with_tmpchdir('rubytest-pathname') {|dir|
      open("f", "w") {|f| f.write "abc" }
      assert_equal(false, Pathname("f").blockdev?)
    }
  end

  def test_chardev?
    with_tmpchdir('rubytest-pathname') {|dir|
      open("f", "w") {|f| f.write "abc" }
      assert_equal(false, Pathname("f").chardev?)
    }
  end

  def test_executable?
    with_tmpchdir('rubytest-pathname') {|dir|
      open("f", "w") {|f| f.write "abc" }
      assert_equal(false, Pathname("f").executable?)
    }
  end

  def test_executable_real?
    with_tmpchdir('rubytest-pathname') {|dir|
      open("f", "w") {|f| f.write "abc" }
      assert_equal(false, Pathname("f").executable_real?)
    }
  end

  def test_exist?
    with_tmpchdir('rubytest-pathname') {|dir|
      open("f", "w") {|f| f.write "abc" }
      assert_equal(true, Pathname("f").exist?)
    }
  end

  def test_grpowned?
    skip "Unix file owner test" if DOSISH
    with_tmpchdir('rubytest-pathname') {|dir|
      open("f", "w") {|f| f.write "abc" }
      File.chown(-1, Process.gid, "f")
      assert_equal(true, Pathname("f").grpowned?)
    }
  end

  def test_directory?
    with_tmpchdir('rubytest-pathname') {|dir|
      open("f", "w") {|f| f.write "abc" }
      assert_equal(false, Pathname("f").directory?)
      Dir.mkdir("d")
      assert_equal(true, Pathname("d").directory?)
    }
  end

  def test_file?
    with_tmpchdir('rubytest-pathname') {|dir|
      open("f", "w") {|f| f.write "abc" }
      assert_equal(true, Pathname("f").file?)
      Dir.mkdir("d")
      assert_equal(false, Pathname("d").file?)
    }
  end

  def test_pipe?
    with_tmpchdir('rubytest-pathname') {|dir|
      open("f", "w") {|f| f.write "abc" }
      assert_equal(false, Pathname("f").pipe?)
    }
  end

  def test_socket?
    with_tmpchdir('rubytest-pathname') {|dir|
      open("f", "w") {|f| f.write "abc" }
      assert_equal(false, Pathname("f").socket?)
    }
  end

  def test_owned?
    with_tmpchdir('rubytest-pathname') {|dir|
      open("f", "w") {|f| f.write "abc" }
      assert_equal(true, Pathname("f").owned?)
    }
  end

  def test_readable?
    with_tmpchdir('rubytest-pathname') {|dir|
      open("f", "w") {|f| f.write "abc" }
      assert_equal(true, Pathname("f").readable?)
    }
  end

  def test_world_readable?
    skip "Unix file mode bit test" if DOSISH
    with_tmpchdir('rubytest-pathname') {|dir|
      open("f", "w") {|f| f.write "abc" }
      File.chmod(0400, "f")
      assert_equal(nil, Pathname("f").world_readable?)
      File.chmod(0444, "f")
      assert_equal(0444, Pathname("f").world_readable?)
    }
  end

  def test_readable_real?
    with_tmpchdir('rubytest-pathname') {|dir|
      open("f", "w") {|f| f.write "abc" }
      assert_equal(true, Pathname("f").readable_real?)
    }
  end

  def test_setuid?
    with_tmpchdir('rubytest-pathname') {|dir|
      open("f", "w") {|f| f.write "abc" }
      assert_equal(false, Pathname("f").setuid?)
    }
  end

  def test_setgid?
    with_tmpchdir('rubytest-pathname') {|dir|
      open("f", "w") {|f| f.write "abc" }
      assert_equal(false, Pathname("f").setgid?)
    }
  end

  def test_size
    with_tmpchdir('rubytest-pathname') {|dir|
      open("f", "w") {|f| f.write "abc" }
      assert_equal(3, Pathname("f").size)
      open("z", "w") {|f| }
      assert_equal(0, Pathname("z").size)
      assert_raise(Errno::ENOENT) { Pathname("not-exist").size }
    }
  end

  def test_size?
    with_tmpchdir('rubytest-pathname') {|dir|
      open("f", "w") {|f| f.write "abc" }
      assert_equal(3, Pathname("f").size?)
      open("z", "w") {|f| }
      assert_equal(nil, Pathname("z").size?)
      assert_equal(nil, Pathname("not-exist").size?)
    }
  end

  def test_sticky?
    skip "Unix file mode bit test" if DOSISH
    with_tmpchdir('rubytest-pathname') {|dir|
      open("f", "w") {|f| f.write "abc" }
      assert_equal(false, Pathname("f").sticky?)
    }
  end

  def test_symlink?
    with_tmpchdir('rubytest-pathname') {|dir|
      open("f", "w") {|f| f.write "abc" }
      assert_equal(false, Pathname("f").symlink?)
    }
  end

  def test_writable?
    with_tmpchdir('rubytest-pathname') {|dir|
      open("f", "w") {|f| f.write "abc" }
      assert_equal(true, Pathname("f").writable?)
    }
  end

  def test_world_writable?
    skip "Unix file mode bit test" if DOSISH
    with_tmpchdir('rubytest-pathname') {|dir|
      open("f", "w") {|f| f.write "abc" }
      File.chmod(0600, "f")
      assert_equal(nil, Pathname("f").world_writable?)
      File.chmod(0666, "f")
      assert_equal(0666, Pathname("f").world_writable?)
    }
  end

  def test_writable_real?
    with_tmpchdir('rubytest-pathname') {|dir|
      open("f", "w") {|f| f.write "abc" }
      assert_equal(true, Pathname("f").writable?)
    }
  end

  def test_zero?
    with_tmpchdir('rubytest-pathname') {|dir|
      open("f", "w") {|f| f.write "abc" }
      assert_equal(false, Pathname("f").zero?)
      open("z", "w") {|f| }
      assert_equal(true, Pathname("z").zero?)
      assert_equal(false, Pathname("not-exist").zero?)
    }
  end

  def test_s_glob
    with_tmpchdir('rubytest-pathname') {|dir|
      open("f", "w") {|f| f.write "abc" }
      Dir.mkdir("d")
      assert_equal([Pathname("d"), Pathname("f")], Pathname.glob("*").sort)
      a = []
      Pathname.glob("*") {|path| a << path }
      a.sort!
      assert_equal([Pathname("d"), Pathname("f")], a)
    }
  end

  def test_s_getwd
    wd = Pathname.getwd
    assert_kind_of(Pathname, wd)
  end

  def test_s_pwd
    wd = Pathname.pwd
    assert_kind_of(Pathname, wd)
  end

  def test_entries
    with_tmpchdir('rubytest-pathname') {|dir|
      open("a", "w") {}
      open("b", "w") {}
      assert_equal([Pathname("."), Pathname(".."), Pathname("a"), Pathname("b")], Pathname(".").entries.sort)
    }
  end

  def test_each_entry
    with_tmpchdir('rubytest-pathname') {|dir|
      open("a", "w") {}
      open("b", "w") {}
      a = []
      Pathname(".").each_entry {|v| a << v }
      assert_equal([Pathname("."), Pathname(".."), Pathname("a"), Pathname("b")], a.sort)
    }
  end

  def test_mkdir
    with_tmpchdir('rubytest-pathname') {|dir|
      Pathname("d").mkdir
      assert(File.directory?("d"))
      Pathname("e").mkdir(0770)
      assert(File.directory?("e"))
    }
  end

  def test_rmdir
    with_tmpchdir('rubytest-pathname') {|dir|
      Pathname("d").mkdir
      assert(File.directory?("d"))
      Pathname("d").rmdir
      assert(!File.exists?("d"))
    }
  end

  def test_opendir
    with_tmpchdir('rubytest-pathname') {|dir|
      open("a", "w") {}
      open("b", "w") {}
      a = []
      Pathname(".").opendir {|d|
        d.each {|e| a << e }
      }
      assert_equal([".", "..", "a", "b"], a.sort)
    }
  end

  def test_find
    with_tmpchdir('rubytest-pathname') {|dir|
      open("a", "w") {}
      open("b", "w") {}
      Dir.mkdir("d")
      open("d/x", "w") {}
      open("d/y", "w") {}
      a = []; Pathname(".").find {|v| a << v }; a.sort!
      assert_equal([Pathname("."), Pathname("a"), Pathname("b"), Pathname("d"), Pathname("d/x"), Pathname("d/y")], a)
      a = []; Pathname("d").find {|v| a << v }; a.sort!
      assert_equal([Pathname("d"), Pathname("d/x"), Pathname("d/y")], a)
      a = Pathname(".").find.sort
      assert_equal([Pathname("."), Pathname("a"), Pathname("b"), Pathname("d"), Pathname("d/x"), Pathname("d/y")], a)
      a = Pathname("d").find.sort
      assert_equal([Pathname("d"), Pathname("d/x"), Pathname("d/y")], a)
    }
  end

  def test_mkpath
    with_tmpchdir('rubytest-pathname') {|dir|
      Pathname("a/b/c/d").mkpath
      assert(File.directory?("a/b/c/d"))
    }
  end

  def test_rmtree
    with_tmpchdir('rubytest-pathname') {|dir|
      Pathname("a/b/c/d").mkpath
      assert(File.exist?("a/b/c/d"))
      Pathname("a").rmtree
      assert(!File.exist?("a"))
    }
  end

  def test_unlink
    with_tmpchdir('rubytest-pathname') {|dir|
      open("f", "w") {|f| f.write "abc" }
      Pathname("f").unlink
      assert(!File.exist?("f"))
      Dir.mkdir("d")
      Pathname("d").unlink
      assert(!File.exist?("d"))
    }
  end

  def test_matchop
    assert_raise(NoMethodError) { Pathname("a") =~ /a/ }
  end

  def test_file_basename
    assert_equal("bar", File.basename(Pathname.new("foo/bar")))
  end

  def test_file_dirname
    assert_equal("foo", File.dirname(Pathname.new("foo/bar")))
  end

  def test_file_split
    assert_equal(["foo", "bar"], File.split(Pathname.new("foo/bar")))
  end

  def test_file_extname
    assert_equal(".baz", File.extname(Pathname.new("bar.baz")))
  end

  def test_file_fnmatch
    assert(File.fnmatch("*.*", Pathname.new("bar.baz")))
  end

  def test_file_join
    assert_equal("foo/bar", File.join(Pathname.new("foo"), Pathname.new("bar")))
    lambda {
      $SAFE = 1
      assert_equal("foo/bar", File.join(Pathname.new("foo"), Pathname.new("bar").taint))
    }.call
  end
end
