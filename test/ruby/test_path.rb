require 'test/unit'

class TestPath < Test::Unit::TestCase
  def test_path
    assert_equal("a", File.basename("a"))
    assert_equal("b", File.basename("a/b"))
    assert_equal("b", File.basename("a/b/"))
    assert_equal("/", File.basename("/"))
    assert_equal("/", File.basename("//"))
    assert_equal("/", File.basename("///"))
    assert_equal("b", File.basename("a/b////"))
    assert_equal("a", File.basename("a.rb", ".rb"))
    assert_equal("a", File.basename("a.rb///", ".rb"))
    assert_equal("a", File.basename("a.rb///", ".*"))
    assert_equal("a.rb", File.basename("a.rb///", ".c"))
    assert_equal(".", File.dirname("a"))
    assert_equal("/", File.dirname("/"))
    assert_equal("/", File.dirname("/a"))
    assert_equal("a", File.dirname("a/b"))
    assert_equal("a/b", File.dirname("a/b/c"))
    assert_equal("/a/b", File.dirname("/a/b/c"))
    assert_equal("/a", File.dirname("/a/b/"))
    assert_equal("/a", File.dirname("/a/b///"))
    case Dir.pwd
    when %r'\A\w:'
      assert_match(/\A\w:\/\z/, File.expand_path(".", "/"))
      assert_match(/\A\w:\/a\z/, File.expand_path("a", "/"))
      dosish = true
    when %r'\A//'
      assert_match(%r'\A//[^/]+/[^/]+\z', File.expand_path(".", "/"))
      assert_match(%r'\A//[^/]+/[^/]+/a\z', File.expand_path(".", "/"))
      dosish = true
    else
      assert_equal("/", File.expand_path(".", "/"))
      assert_equal("/sub", File.expand_path("sub", "/"))
    end
    if dosish
      assert_equal("//127.0.0.1/share", File.expand_path("/", "//127.0.0.1/share/sub"))
      assert_equal("//127.0.0.1/share/dir", File.expand_path("/dir", "//127.0.0.1/share/sub"))
      assert_equal("z:/", File.expand_path("/", "z:/sub"))
      assert_equal("z:/dir", File.expand_path("/dir", "z:/sub"))
    end
    assert_equal("//", File.expand_path(".", "//"))
    assert_equal("//sub", File.expand_path("sub", "//"))

    assert_equal("//127.0.0.1/\u3042", File.expand_path("\u3042", "//127.0.0.1"))
  end

  def test_dirname
    if /(bcc|ms)win\d|mingw|cygwin|emx/ =~ RUBY_PLATFORM
      # DOSISH_DRIVE_LETTER
      assert_equal('C:.', File.dirname('C:'))
      assert_equal('C:.', File.dirname('C:a'))
      assert_equal('C:.', File.dirname('C:a/'))
      assert_equal('C:a', File.dirname('C:a/b'), "[ruby-dev:27738]")

      assert_equal('C:/', File.dirname('C:/'))
      assert_equal('C:/', File.dirname('C:/a'))
      assert_equal('C:/', File.dirname('C:/a/'))
      assert_equal('C:/a', File.dirname('C:/a/b'))

      assert_equal('C:/', File.dirname('C://'))
      assert_equal('C:/', File.dirname('C://a'))
      assert_equal('C:/', File.dirname('C://a/'))
      assert_equal('C:/a', File.dirname('C://a/b'))

      assert_equal('C:/', File.dirname('C:///'), "[ruby-dev:27738]")
      assert_equal('C:/', File.dirname('C:///a'))
      assert_equal('C:/', File.dirname('C:///a/'))
      assert_equal('C:/a', File.dirname('C:///a/b'))
    else
      # others
      assert_equal('.', File.dirname('C:'))
      assert_equal('.', File.dirname('C:a'))
      assert_equal('.', File.dirname('C:a/'))
      assert_equal('C:a', File.dirname('C:a/b'))

      assert_equal('.', File.dirname('C:/'))
      assert_equal('C:', File.dirname('C:/a'))
      assert_equal('C:', File.dirname('C:/a/'))
      assert_equal('C:/a', File.dirname('C:/a/b'))

      assert_equal('.', File.dirname('C://'))
      assert_equal('C:', File.dirname('C://a'))
      assert_equal('C:', File.dirname('C://a/'))
      # not spec.
      #assert_equal('C://a', File.dirname('C://a/b'))

      assert_equal('.', File.dirname('C:///'))
      assert_equal('C:', File.dirname('C:///a'))
      assert_equal('C:', File.dirname('C:///a/'))
      # not spec.
      #assert_equal('C:///a', File.dirname('C:///a/b'))
    end

    assert_equal('.', File.dirname(''))
    assert_equal('.', File.dirname('a'))
    assert_equal('.', File.dirname('a/'))
    assert_equal('a', File.dirname('a/b'))

    assert_equal('/', File.dirname('/'))
    assert_equal('/', File.dirname('/a'))
    assert_equal('/', File.dirname('/a/'))
    assert_equal('/a', File.dirname('/a/b'))

    if /(bcc|ms|cyg)win|mingw|emx/ =~ RUBY_PLATFORM
      # DOSISH_UNC
      assert_equal('//', File.dirname('//'))
      assert_equal('//a', File.dirname('//a'))
      assert_equal('//a', File.dirname('//a/'))
      assert_equal('//a/b', File.dirname('//a/b'))
      assert_equal('//a/b', File.dirname('//a/b/'))
      assert_equal('//a/b', File.dirname('//a/b/c'))

      assert_equal('//', File.dirname('///'))
      assert_equal('//a', File.dirname('///a'))
      assert_equal('//a', File.dirname('///a/'))
      assert_equal('//a/b', File.dirname('///a/b'))
      assert_equal('//a/b', File.dirname('///a/b/'))
      assert_equal('//a/b', File.dirname('///a/b/c'))
    else
      # others
      assert_equal('/', File.dirname('//'))
      assert_equal('/', File.dirname('//a'))
      assert_equal('/', File.dirname('//a/'))
      assert_equal('/a', File.dirname('//a/b'))
      assert_equal('/a', File.dirname('//a/b/'))
      assert_equal('/a/b', File.dirname('//a/b/c'))

      assert_equal('/', File.dirname('///'))
      assert_equal('/', File.dirname('///a'))
      assert_equal('/', File.dirname('///a/'))
      assert_equal('/a', File.dirname('///a/b'))
      assert_equal('/a', File.dirname('///a/b/'))
      assert_equal('/a/b', File.dirname('///a/b/c'))
    end
  end

  def test_basename
    if /(bcc|ms)win\d|mingw|cygwin|emx/ =~ RUBY_PLATFORM
      # DOSISH_DRIVE_LETTER
      assert_equal('', File.basename('C:'))
      assert_equal('a', File.basename('C:a'))
      assert_equal('a', File.basename('C:a/'))
      assert_equal('b', File.basename('C:a/b'))

      assert_equal('/', File.basename('C:/'))
      assert_equal('a', File.basename('C:/a'))
      assert_equal('a', File.basename('C:/a/'))
      assert_equal('b', File.basename('C:/a/b'))

      assert_equal('/', File.basename('C://'))
      assert_equal('a', File.basename('C://a'))
      assert_equal('a', File.basename('C://a/'))
      assert_equal('b', File.basename('C://a/b'))

      assert_equal('/', File.basename('C:///'))
      assert_equal('a', File.basename('C:///a'))
      assert_equal('a', File.basename('C:///a/'))
      assert_equal('b', File.basename('C:///a/b'))
    else
      # others
      assert_equal('C:', File.basename('C:'))
      assert_equal('C:a', File.basename('C:a'))
      assert_equal('C:a', File.basename('C:a/'))
      assert_equal('b', File.basename('C:a/b'))

      assert_equal('C:', File.basename('C:/'))
      assert_equal('a', File.basename('C:/a'))
      assert_equal('a', File.basename('C:/a/'))
      assert_equal('b', File.basename('C:/a/b'))

      assert_equal('C:', File.basename('C://'))
      assert_equal('a', File.basename('C://a'))
      assert_equal('a', File.basename('C://a/'))
      assert_equal('b', File.basename('C://a/b'))

      assert_equal('C:', File.basename('C:///'))
      assert_equal('a', File.basename('C:///a'))
      assert_equal('a', File.basename('C:///a/'))
      assert_equal('b', File.basename('C:///a/b'))
    end

    assert_equal('', File.basename(''))
    assert_equal('a', File.basename('a'))
    assert_equal('a', File.basename('a/'))
    assert_equal('b', File.basename('a/b'))

    assert_equal('/', File.basename('/'))
    assert_equal('a', File.basename('/a'))
    assert_equal('a', File.basename('/a/'))
    assert_equal('b', File.basename('/a/b'))

    assert_equal("..", File.basename("..", ".*"))

    if /(bcc|ms|cyg)win|mingw|emx/ =~ RUBY_PLATFORM
      # DOSISH_UNC
      assert_equal('/', File.basename('//'))
      assert_equal('/', File.basename('//a'))
      assert_equal('/', File.basename('//a/'))
      assert_equal('/', File.basename('//a/b'), "[ruby-dev:27776]")
      assert_equal('/', File.basename('//a/b/'))
      assert_equal('c', File.basename('//a/b/c'))

      assert_equal('/', File.basename('///'))
      assert_equal('/', File.basename('///a'))
      assert_equal('/', File.basename('///a/'))
      assert_equal('/', File.basename('///a/b'))
      assert_equal('/', File.basename('///a/b/'))
      assert_equal('c', File.basename('///a/b/c'))
    else
      # others
      assert_equal('/', File.basename('//'))
      assert_equal('a', File.basename('//a'))
      assert_equal('a', File.basename('//a/'))
      assert_equal('b', File.basename('//a/b'))
      assert_equal('b', File.basename('//a/b/'))
      assert_equal('c', File.basename('//a/b/c'))

      assert_equal('/', File.basename('///'))
      assert_equal('a', File.basename('///a'))
      assert_equal('a', File.basename('///a/'))
      assert_equal('b', File.basename('///a/b'))
      assert_equal('b', File.basename('///a/b/'))
      assert_equal('c', File.basename('///a/b/c'))
    end
  end

  def test_extname
    assert_equal('', File.extname('a'))
    ext = '.rb'
    assert_equal(ext, File.extname('a.rb'))
    unless /mswin|bccwin|mingw/ =~ RUBY_PLATFORM
      # trailing spaces and dots are ignored on NTFS.
      ext = ''
    end
    assert_equal(ext, File.extname('a.rb.'))
    assert_equal('', File.extname('a.'))
    assert_equal('', File.extname('.x'))
    assert_equal('', File.extname('..x'))
  end

  def test_ascii_incompatible_path
    s = "\u{221e}\u{2603}"
    assert_raise(Encoding::CompatibilityError) {open(s.encode("utf-16be"))}
    assert_raise(Encoding::CompatibilityError) {open(s.encode("utf-16le"))}
    assert_raise(Encoding::CompatibilityError) {open(s.encode("utf-32be"))}
    assert_raise(Encoding::CompatibilityError) {open(s.encode("utf-32le"))}
  end

  def test_join
    bug5483 = '[ruby-core:40338]'
    path = %w[a b]
    Encoding.list.each do |e|
      next unless e.ascii_compatible?
      assert_equal(e, File.join(*path.map {|s| s.force_encoding(e)}).encoding, bug5483)
    end
  end
end
