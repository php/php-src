require 'test/unit'

class TestFnmatch < Test::Unit::TestCase

  def bracket_test(s, t) # `s' should start with neither '!' nor '^'
    0x21.upto(0x7E) do |i|
      assert_equal(t.include?(i.chr), File.fnmatch("[#{s}]", i.chr, File::FNM_DOTMATCH))
      assert_equal(t.include?(i.chr), !File.fnmatch("[^#{s}]", i.chr, File::FNM_DOTMATCH))
      assert_equal(t.include?(i.chr), !File.fnmatch("[!#{s}]", i.chr, File::FNM_DOTMATCH))
    end
  end
  def test_fnmatch
    assert(File.fnmatch('\[1\]' , '[1]'), "[ruby-dev:22819]")
    assert(File.fnmatch('*?', 'a'), "[ruby-dev:22815]")
    assert(File.fnmatch('*/', 'a/'))
    assert(File.fnmatch('\[1\]' , '[1]', File::FNM_PATHNAME))
    assert(File.fnmatch('*?', 'a', File::FNM_PATHNAME))
    assert(File.fnmatch('*/', 'a/', File::FNM_PATHNAME))
    # text
    assert(File.fnmatch('cat', 'cat'))
    assert(!File.fnmatch('cat', 'category'))
    assert(!File.fnmatch('cat', 'wildcat'))
    # '?' matches any one character
    assert(File.fnmatch('?at', 'cat'))
    assert(File.fnmatch('c?t', 'cat'))
    assert(File.fnmatch('ca?', 'cat'))
    assert(File.fnmatch('?a?', 'cat'))
    assert(!File.fnmatch('c??t', 'cat'))
    assert(!File.fnmatch('??at', 'cat'))
    assert(!File.fnmatch('ca??', 'cat'))
    # '*' matches any number (including 0) of any characters
    assert(File.fnmatch('c*', 'cats'))
    assert(File.fnmatch('c*ts', 'cats'))
    assert(File.fnmatch('*ts', 'cats'))
    assert(File.fnmatch('*c*a*t*s*', 'cats'))
    assert(!File.fnmatch('c*t', 'cats'))
    assert(!File.fnmatch('*abc', 'abcabz'))
    assert(File.fnmatch('*abz', 'abcabz'))
    assert(!File.fnmatch('a*abc', 'abc'))
    assert(File.fnmatch('a*bc', 'abc'))
    assert(!File.fnmatch('a*bc', 'abcd'))
    # [seq] : matches any character listed between bracket
    # [!seq] or [^seq] : matches any character except those listed between bracket
    bracket_test("bd-gikl-mosv-x", "bdefgiklmosvwx")
    # escaping character
    assert(File.fnmatch('\?', '?'))
    assert(!File.fnmatch('\?', '\?'))
    assert(!File.fnmatch('\?', 'a'))
    assert(!File.fnmatch('\?', '\a'))
    assert(File.fnmatch('\*', '*'))
    assert(!File.fnmatch('\*', '\*'))
    assert(!File.fnmatch('\*', 'cats'))
    assert(!File.fnmatch('\*', '\cats'))
    assert(File.fnmatch('\a', 'a'))
    assert(!File.fnmatch('\a', '\a'))
    assert(File.fnmatch('[a\-c]', 'a'))
    assert(File.fnmatch('[a\-c]', '-'))
    assert(File.fnmatch('[a\-c]', 'c'))
    assert(!File.fnmatch('[a\-c]', 'b'))
    assert(!File.fnmatch('[a\-c]', '\\'))
    # escaping character loses its meaning if FNM_NOESCAPE is set
    assert(!File.fnmatch('\?', '?', File::FNM_NOESCAPE))
    assert(File.fnmatch('\?', '\?', File::FNM_NOESCAPE))
    assert(!File.fnmatch('\?', 'a', File::FNM_NOESCAPE))
    assert(File.fnmatch('\?', '\a', File::FNM_NOESCAPE))
    assert(!File.fnmatch('\*', '*', File::FNM_NOESCAPE))
    assert(File.fnmatch('\*', '\*', File::FNM_NOESCAPE))
    assert(!File.fnmatch('\*', 'cats', File::FNM_NOESCAPE))
    assert(File.fnmatch('\*', '\cats', File::FNM_NOESCAPE))
    assert(!File.fnmatch('\a', 'a', File::FNM_NOESCAPE))
    assert(File.fnmatch('\a', '\a', File::FNM_NOESCAPE))
    assert(File.fnmatch('[a\-c]', 'a', File::FNM_NOESCAPE))
    assert(!File.fnmatch('[a\-c]', '-', File::FNM_NOESCAPE))
    assert(File.fnmatch('[a\-c]', 'c', File::FNM_NOESCAPE))
    assert(File.fnmatch('[a\-c]', 'b', File::FNM_NOESCAPE)) # '\\' < 'b' < 'c'
    assert(File.fnmatch('[a\-c]', '\\', File::FNM_NOESCAPE))
    # case is ignored if FNM_CASEFOLD is set
    assert(!File.fnmatch('cat', 'CAT'))
    assert(File.fnmatch('cat', 'CAT', File::FNM_CASEFOLD))
    assert(!File.fnmatch('[a-z]', 'D'))
    assert(File.fnmatch('[a-z]', 'D', File::FNM_CASEFOLD))
    assert(!File.fnmatch('[abc]', 'B'))
    assert(File.fnmatch('[abc]', 'B', File::FNM_CASEFOLD))
    # wildcard doesn't match '/' if FNM_PATHNAME is set
    assert(File.fnmatch('foo?boo', 'foo/boo'))
    assert(File.fnmatch('foo*', 'foo/boo'))
    assert(!File.fnmatch('foo?boo', 'foo/boo', File::FNM_PATHNAME))
    assert(!File.fnmatch('foo*', 'foo/boo', File::FNM_PATHNAME))
    # wildcard matches leading period if FNM_DOTMATCH is set
    assert(!File.fnmatch('*', '.profile'))
    assert(File.fnmatch('*', '.profile', File::FNM_DOTMATCH))
    assert(File.fnmatch('.*', '.profile'))
    assert(File.fnmatch('*', 'dave/.profile'))
    assert(File.fnmatch('*/*', 'dave/.profile'))
    assert(!File.fnmatch('*/*', 'dave/.profile', File::FNM_PATHNAME))
    assert(File.fnmatch('*/*', 'dave/.profile', File::FNM_PATHNAME | File::FNM_DOTMATCH))
    # recursive matching
    assert(File.fnmatch('**/foo', 'a/b/c/foo', File::FNM_PATHNAME))
    assert(File.fnmatch('**/foo', '/foo', File::FNM_PATHNAME))
    assert(!File.fnmatch('**/foo', 'a/.b/c/foo', File::FNM_PATHNAME))
    assert(File.fnmatch('**/foo', 'a/.b/c/foo', File::FNM_PATHNAME | File::FNM_DOTMATCH))
    assert(File.fnmatch('**/foo', '/root/foo', File::FNM_PATHNAME))
    assert(File.fnmatch('**/foo', 'c:/root/foo', File::FNM_PATHNAME))
  end

end
