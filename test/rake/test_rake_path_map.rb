require File.expand_path('../helper', __FILE__)

class TestRakePathMap < Rake::TestCase

  def test_returns_self_with_no_args
    assert_equal "abc.rb", "abc.rb".pathmap
  end

  def test_s_returns_file_separator
    sep = File::ALT_SEPARATOR || File::SEPARATOR
    assert_equal sep, "abc.rb".pathmap("%s")
    assert_equal sep, "".pathmap("%s")
    assert_equal "a#{sep}b", "a/b".pathmap("%d%s%f")
  end

  def test_f_returns_basename
    assert_equal "abc.rb", "abc.rb".pathmap("%f")
    assert_equal "abc.rb", "this/is/a/dir/abc.rb".pathmap("%f")
    assert_equal "abc.rb", "/this/is/a/dir/abc.rb".pathmap("%f")
  end

  def test_n_returns_basename_without_extension
    assert_equal "abc", "abc.rb".pathmap("%n")
    assert_equal "abc", "abc".pathmap("%n")
    assert_equal "abc", "this/is/a/dir/abc.rb".pathmap("%n")
    assert_equal "abc", "/this/is/a/dir/abc.rb".pathmap("%n")
    assert_equal "abc", "/this/is/a/dir/abc".pathmap("%n")
  end

  def test_d_returns_dirname
    assert_equal ".", "abc.rb".pathmap("%d")
    assert_equal "/", "/abc".pathmap("%d")
    assert_equal "this/is/a/dir", "this/is/a/dir/abc.rb".pathmap("%d")
    assert_equal "/this/is/a/dir", "/this/is/a/dir/abc.rb".pathmap("%d")
  end

  def test_9d_returns_partial_dirname
    assert_equal "this/is", "this/is/a/dir/abc.rb".pathmap("%2d")
    assert_equal "this", "this/is/a/dir/abc.rb".pathmap("%1d")
    assert_equal ".", "this/is/a/dir/abc.rb".pathmap("%0d")
    assert_equal "dir", "this/is/a/dir/abc.rb".pathmap("%-1d")
    assert_equal "a/dir", "this/is/a/dir/abc.rb".pathmap("%-2d")
    assert_equal "this/is/a/dir", "this/is/a/dir/abc.rb".pathmap("%100d")
    assert_equal "this/is/a/dir", "this/is/a/dir/abc.rb".pathmap("%-100d")
  end

  def test_x_returns_extension
    assert_equal "", "abc".pathmap("%x")
    assert_equal ".rb", "abc.rb".pathmap("%x")
    assert_equal ".rb", "abc.xyz.rb".pathmap("%x")
    assert_equal "", ".depends".pathmap("%x")
    assert_equal "", "dir/.depends".pathmap("%x")
  end

  def test_X_returns_everything_but_extension
    assert_equal "abc", "abc".pathmap("%X")
    assert_equal "abc", "abc.rb".pathmap("%X")
    assert_equal "abc.xyz", "abc.xyz.rb".pathmap("%X")
    assert_equal "ab.xyz", "ab.xyz.rb".pathmap("%X")
    assert_equal "a.xyz", "a.xyz.rb".pathmap("%X")
    assert_equal "abc", "abc.rb".pathmap("%X")
    assert_equal "ab", "ab.rb".pathmap("%X")
    assert_equal "a", "a.rb".pathmap("%X")
    assert_equal ".depends", ".depends".pathmap("%X")
    assert_equal "a/dir/.depends", "a/dir/.depends".pathmap("%X")
    assert_equal "/.depends", "/.depends".pathmap("%X")
  end

  def test_p_returns_entire_pathname
    assert_equal "abc.rb", "abc.rb".pathmap("%p")
    assert_equal "this/is/a/dir/abc.rb", "this/is/a/dir/abc.rb".pathmap("%p")
    assert_equal "/this/is/a/dir/abc.rb", "/this/is/a/dir/abc.rb".pathmap("%p")
  end

  def test_dash_returns_empty_string
    assert_equal "", "abc.rb".pathmap("%-")
    assert_equal "abc.rb", "abc.rb".pathmap("%X%-%x")
  end

  def test_percent_percent_returns_percent
    assert_equal "a%b", "".pathmap("a%%b")
  end

  def test_undefined_percent_causes_error
    assert_raises(ArgumentError) {
      "dir/abc.rb".pathmap("%z")
    }
  end

  def test_pattern_returns_substitutions
    assert_equal "bin/org/osb",
      "src/org/osb/Xyz.java".pathmap("%{src,bin}d")
  end

  def test_pattern_can_use_backreferences
    assert_equal "dir/hi/is", "dir/this/is".pathmap("%{t(hi)s,\\1}p")
  end

  def test_pattern_with_star_replacement_string_uses_block
    assert_equal "src/ORG/osb",
      "src/org/osb/Xyz.java".pathmap("%{/org,*}d") { |d| d.upcase }
    assert_equal "Xyz.java",
      "src/org/osb/Xyz.java".pathmap("%{.*,*}f") { |f| f.capitalize }
  end

  def test_pattern_with_no_replacement_nor_block_substitutes_empty_string
    assert_equal "bc.rb", "abc.rb".pathmap("%{a}f")
  end

  def test_pattern_works_with_certain_valid_operators
    assert_equal "dir/xbc.rb", "dir/abc.rb".pathmap("%{a,x}p")
    assert_equal "d1r", "dir/abc.rb".pathmap("%{i,1}d")
    assert_equal "xbc.rb", "dir/abc.rb".pathmap("%{a,x}f")
    assert_equal ".Rb", "dir/abc.rb".pathmap("%{r,R}x")
    assert_equal "xbc", "dir/abc.rb".pathmap("%{a,x}n")
  end

  def test_multiple_patterns
    assert_equal "this/is/b/directory/abc.rb",
      "this/is/a/dir/abc.rb".pathmap("%{a,b;dir,\\0ectory}p")
  end

  def test_partial_directory_selection_works_with_patterns
    assert_equal "this/is/a/long",
      "this/is/a/really/long/path/ok.rb".pathmap("%{/really/,/}5d")
  end

  def test_pattern_with_invalid_operator
    ex = assert_raises(ArgumentError) do
      "abc.xyz".pathmap("%{src,bin}z")
    end
    assert_match(/unknown.*pathmap.*spec.*z/i, ex.message)
  end

  def test_works_with_windows_separators
    if File::ALT_SEPARATOR
      assert_equal "abc", 'dir\abc.rb'.pathmap("%n")
      assert_equal 'this\is\a\dir',
        'this\is\a\dir\abc.rb'.pathmap("%d")
    end
  end

  def test_complex_patterns
    sep = "".pathmap("%s")
    assert_equal "dir/abc.rb", "dir/abc.rb".pathmap("%d/%n%x")
    assert_equal "./abc.rb", "abc.rb".pathmap("%d/%n%x")
    assert_equal "Your file extension is '.rb'",
      "dir/abc.rb".pathmap("Your file extension is '%x'")
    assert_equal "bin/org/onstepback/proj/A.class",
      "src/org/onstepback/proj/A.java".pathmap("%{src,bin}d/%n.class")
    assert_equal "src_work/bin/org/onstepback/proj/A.class",
      "src_work/src/org/onstepback/proj/A.java".pathmap('%{\bsrc\b,bin}X.class')
    assert_equal ".depends.bak", ".depends".pathmap("%X.bak")
    assert_equal "d#{sep}a/b/c#{sep}file.txt", "a/b/c/d/file.txt".pathmap("%-1d%s%3d%s%f")
  end
end

