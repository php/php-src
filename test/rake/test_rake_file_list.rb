require File.expand_path('../helper', __FILE__)

class TestRakeFileList < Rake::TestCase
  FileList = Rake::FileList

  def setup
    super

    FileUtils.mkdir "CVS" rescue nil
    FileUtils.mkdir ".svn" rescue nil
    @cdir = "cfiles"
    FileUtils.mkdir @cdir rescue nil
    FileUtils.touch ".dummy"
    FileUtils.touch "x.bak"
    FileUtils.touch "x~"
    FileUtils.touch "core"
    FileUtils.touch "x.c"
    FileUtils.touch "xyz.c"
    FileUtils.touch "abc.c"
    FileUtils.touch "abc.h"
    FileUtils.touch "abc.x"
    FileUtils.touch "existing"

    open 'xyzzy.txt', 'w' do |io|
      io.puts 'x'
      io.puts 'XYZZY'
    end

  end

  def test_delegating_methods_do_not_include_to_a_or_to_ary
    assert ! FileList::DELEGATING_METHODS.include?("to_a"), "should not include to_a"
    assert ! FileList::DELEGATING_METHODS.include?(:to_a), "should not include to_a"
    assert ! FileList::DELEGATING_METHODS.include?("to_ary"), "should not include to_ary"
    assert ! FileList::DELEGATING_METHODS.include?(:to_ary), "should not include to_ary"
  end

  def test_create
    fl = FileList.new
    assert_equal 0, fl.size
  end

  def test_create_with_args
    fl = FileList.new("*.c", "x")
    assert_equal ["abc.c", "x.c", "xyz.c", "x"].sort,
      fl.sort
  end

  def test_create_with_block
    fl = FileList.new { |f| f.include("x") }
    assert_equal ["x"], fl.resolve
  end

  def test_create_with_brackets
    fl = FileList["*.c", "x"]
    assert_equal ["abc.c", "x.c", "xyz.c", "x"].sort,
      fl.sort
  end

  def test_create_with_brackets_and_filelist
    fl = FileList[FileList["*.c", "x"]]
    assert_equal ["abc.c", "x.c", "xyz.c", "x"].sort,
      fl.sort
  end

  def test_include_with_another_array
    fl = FileList.new.include(["x", "y", "z"])
    assert_equal ["x", "y", "z"].sort, fl.sort
  end

  def test_include_with_another_filelist
    fl = FileList.new.include(FileList["*.c", "x"])
    assert_equal ["abc.c", "x.c", "xyz.c", "x"].sort,
      fl.sort
  end

  def test_append
    fl = FileList.new
    fl << "a.rb" << "b.rb"
    assert_equal ['a.rb', 'b.rb'], fl
  end

  def test_add_many
    fl = FileList.new
    fl.include %w(a d c)
    fl.include('x', 'y')
    assert_equal ['a', 'd', 'c', 'x', 'y'], fl
    assert_equal ['a', 'd', 'c', 'x', 'y'], fl.resolve
  end

  def test_add_return
    f = FileList.new
    g = f << "x"
    assert_equal f.object_id, g.object_id
    h = f.include("y")
    assert_equal f.object_id, h.object_id
  end

  def test_match
    fl = FileList.new
    fl.include '*.c'

    assert_equal %w[abc.c x.c xyz.c], fl.sort
  end

  def test_add_matching
    fl = FileList.new
    fl << "a.java"
    fl.include '*.c'

    assert_equal %w[a.java abc.c x.c xyz.c], fl.sort
  end

  def test_multiple_patterns
    fl = FileList.new
    fl.include('*.z', '*foo*')

    assert_equal [], fl

    fl.include('*.c', '*xist*')
    assert_equal %w[x.c xyz.c abc.c existing].sort, fl.sort
  end

  def test_square_bracket_pattern
    fl = FileList.new
    fl.include("abc.[ch]")
    assert fl.size == 2
    assert fl.include?("abc.c")
    assert fl.include?("abc.h")
  end

  def test_curly_bracket_pattern
    fl = FileList.new
    fl.include("abc.{c,h}")
    assert fl.size == 2
    assert fl.include?("abc.c")
    assert fl.include?("abc.h")
  end

  def test_reject
    fl = FileList.new
    fl.include %w(x.c abc.c xyz.c existing)
    fl.reject! { |fn| fn =~ /^x/ }
    assert_equal %w[abc.c existing], fl
  end

  def test_exclude
    fl = FileList['x.c', 'abc.c', 'xyz.c', 'existing']
    fl.each { |fn| touch fn, :verbose => false }

    x = fl.exclude(%r{^x.+\.})

    assert_equal FileList, x.class
    assert_equal %w(x.c abc.c existing), fl
    assert_equal fl.object_id, x.object_id

    fl.exclude('*.c')

    assert_equal ['existing'], fl

    fl.exclude('existing')

    assert_equal [], fl
  end

  def test_excluding_via_block
    fl = FileList['a.c', 'b.c', 'xyz.c']
    fl.exclude { |fn| fn.pathmap('%n') == 'xyz' }
    assert fl.exclude?("xyz.c"), "Should exclude xyz.c"
    assert_equal ['a.c', 'b.c'], fl
  end

  def test_exclude_return_on_create
    fl = FileList['*'].exclude(/.*\.[hcx]$/)
    assert_equal %w[cfiles existing xyzzy.txt], fl.sort
    assert_equal FileList, fl.class
  end

  def test_exclude_with_string_return_on_create
    fl = FileList['*'].exclude('abc.c')
    assert_equal %w[abc.h abc.x cfiles existing x.c xyz.c xyzzy.txt], fl.sort
    assert_equal FileList, fl.class
  end

  def test_default_exclude
    fl = FileList.new
    fl.clear_exclude
    fl.include("**/*~", "**/*.bak", "**/core")
    assert fl.member?("core"), "Should include core"
    assert fl.member?("x.bak"), "Should include .bak files"
  end

  def test_unique
    fl = FileList.new
    fl << "x.c" << "a.c" << "b.rb" << "a.c"
    assert_equal ['x.c', 'a.c', 'b.rb', 'a.c'], fl
    fl.uniq!
    assert_equal ['x.c', 'a.c', 'b.rb'], fl
  end

  def test_to_string
    fl = FileList.new
    fl << "a.java" << "b.java"
    assert_equal  "a.java b.java", fl.to_s
    assert_equal  "a.java b.java", "#{fl}"
  end

  def test_to_array
    fl = FileList['a.java', 'b.java']
    assert_equal  ['a.java', 'b.java'], fl.to_a
    assert_equal  Array, fl.to_a.class
    assert_equal  ['a.java', 'b.java'], fl.to_ary
    assert_equal  Array, fl.to_ary.class
  end

  def test_to_s_pending
    fl = FileList['abc.*']
    result = fl.to_s
    assert_match(%r{abc\.c}, result)
    assert_match(%r{abc\.h}, result)
    assert_match(%r{abc\.x}, result)
    assert_match(%r{(abc\..\b ?){2}}, result)
  end

  def test_inspect_pending
    fl = FileList['abc.*']
    result = fl.inspect
    assert_match(%r{"abc\.c"}, result)
    assert_match(%r{"abc\.h"}, result)
    assert_match(%r{"abc\.x"}, result)
    assert_match(%r|^\[("abc\..", ){2}"abc\.."\]$|, result)
  end

  def test_sub
    fl = FileList["*.c"]
    f2 = fl.sub(/\.c$/, ".o")
    assert_equal FileList, f2.class
    assert_equal ["abc.o", "x.o", "xyz.o"].sort,
      f2.sort
    f3 = fl.gsub(/\.c$/, ".o")
    assert_equal FileList, f3.class
    assert_equal ["abc.o", "x.o", "xyz.o"].sort,
      f3.sort
  end

  def test_claim_to_be_a_kind_of_array
    fl = FileList['*.c']
    assert fl.is_a?(Array)
    assert fl.kind_of?(Array)
  end

  def test_claim_to_be_a_kind_of_filelist
    fl = FileList['*.c']
    assert fl.is_a?(FileList)
    assert fl.kind_of?(FileList)
  end

  def test_claim_to_be_a_filelist_instance
    fl = FileList['*.c']
    assert fl.instance_of?(FileList)
  end

  def test_dont_claim_to_be_an_array_instance
    fl = FileList['*.c']
    assert ! fl.instance_of?(Array)
  end

  def test_sub!
    f = "x/a.c"
    fl = FileList[f, "x/b.c"]
    res = fl.sub!(/\.c$/, ".o")
    assert_equal ["x/a.o", "x/b.o"].sort, fl.sort
    assert_equal "x/a.c", f
    assert_equal fl.object_id, res.object_id
  end

  def test_sub_with_block
    fl = FileList["src/org/onestepback/a.java", "src/org/onestepback/b.java"]
# The block version doesn't work the way I want it to ...
#    f2 = fl.sub(%r{^src/(.*)\.java$}) { |x|  "classes/" + $1 + ".class" }
    f2 = fl.sub(%r{^src/(.*)\.java$}, "classes/\\1.class")
    assert_equal [
      "classes/org/onestepback/a.class",
      "classes/org/onestepback/b.class"
    ].sort,
      f2.sort
  end

  def test_string_ext
    assert_equal "one.net", "one.two".ext("net")
    assert_equal "one.net", "one.two".ext(".net")
    assert_equal "one.net", "one".ext("net")
    assert_equal "one.net", "one".ext(".net")
    assert_equal "one.two.net", "one.two.c".ext(".net")
    assert_equal "one/two.net", "one/two.c".ext(".net")
    assert_equal "one.x/two.net", "one.x/two.c".ext(".net")
    assert_equal "one.x/two.net", "one.x/two".ext(".net")
    assert_equal ".onerc.net", ".onerc.dot".ext("net")
    assert_equal ".onerc.net", ".onerc".ext("net")
    assert_equal ".a/.onerc.net", ".a/.onerc".ext("net")
    assert_equal "one", "one.two".ext('')
    assert_equal "one", "one.two".ext
    assert_equal ".one", ".one.two".ext
    assert_equal ".one", ".one".ext
    assert_equal ".", ".".ext("c")
    assert_equal "..", "..".ext("c")
    # These only need to work in windows
    if Rake::Win32.windows?
      assert_equal "one.x\\two.net", "one.x\\two.c".ext(".net")
      assert_equal "one.x\\two.net", "one.x\\two".ext(".net")
    end
  end

  def test_filelist_ext
    assert_equal FileList['one.c', '.one.c'],
      FileList['one.net', '.one'].ext('c')
  end

  def test_gsub
    fl = FileList["*.c"]
    f2 = fl.gsub(/a/, "A")
    assert_equal ["Abc.c", "x.c", "xyz.c"].sort,
      f2.sort
  end

  def test_gsub!
    f = FileList["*.c"]
    f.gsub!(/a/, "A")
    assert_equal ["Abc.c", "x.c", "xyz.c"].sort,
      f.sort
  end

  def test_egrep_returns_0_if_no_matches
    files = FileList['test/lib/*_test.rb'].exclude("test/lib/filelist_test.rb")
    assert_equal 0, files.egrep(/XYZZY/) { }
  end

  def test_egrep_with_output
    files = FileList['*.txt']

    out, = capture_io do
      files.egrep(/XYZZY/)
    end

    assert_equal "xyzzy.txt:2:XYZZY\n", out
  end

  def test_egrep_with_block
    files = FileList['*.txt']
    found = nil

    files.egrep(/XYZZY/) do |fn, ln, line|
      found = [fn, ln, line]
    end

    assert_equal ["xyzzy.txt", 2, "XYZZY\n"], found
  end

  def test_egrep_with_error
    files = FileList['*.txt']

    _, err = capture_io do
      files.egrep(/XYZZY/) do |fn, ln, line |
        raise "_EGREP_FAILURE_"
      end
    end

    assert_equal "Error while processing 'xyzzy.txt': _EGREP_FAILURE_\n", err
  end

  def test_existing
    fl = FileList['abc.c', 'notthere.c']
    assert_equal ["abc.c"], fl.existing
    assert fl.existing.is_a?(FileList)
  end

  def test_existing!
    fl = FileList['abc.c', 'notthere.c']
    result = fl.existing!
    assert_equal ["abc.c"], fl
    assert_equal fl.object_id, result.object_id
  end

  def test_ignore_special
    f = FileList['*']
    assert ! f.include?("CVS"), "Should not contain CVS"
    assert ! f.include?(".svn"), "Should not contain .svn"
    assert ! f.include?(".dummy"), "Should not contain dot files"
    assert ! f.include?("x.bak"), "Should not contain .bak files"
    assert ! f.include?("x~"), "Should not contain ~ files"
    assert ! f.include?("core"), "Should not contain core files"
  end

  def test_clear_ignore_patterns
    f = FileList['*', '.svn']
    f.clear_exclude
    assert f.include?("abc.c")
    assert f.include?("xyz.c")
    assert f.include?("CVS")
    assert f.include?(".svn")
    assert f.include?("x.bak")
    assert f.include?("x~")
  end

  def test_exclude_with_alternate_file_seps
    fl = FileList.new
    assert fl.exclude?("x/CVS/y")
    assert fl.exclude?("x\\CVS\\y")
    assert fl.exclude?("x/.svn/y")
    assert fl.exclude?("x\\.svn\\y")
    assert fl.exclude?("x/core")
    assert fl.exclude?("x\\core")
  end

  def test_add_default_exclude_list
    fl = FileList.new
    fl.exclude(/~\d+$/)
    assert fl.exclude?("x/CVS/y")
    assert fl.exclude?("x\\CVS\\y")
    assert fl.exclude?("x/.svn/y")
    assert fl.exclude?("x\\.svn\\y")
    assert fl.exclude?("x/core")
    assert fl.exclude?("x\\core")
    assert fl.exclude?("x/abc~1")
  end

  def test_basic_array_functions
    f = FileList['b', 'c', 'a']
    assert_equal 'b', f.first
    assert_equal 'b', f[0]
    assert_equal 'a', f.last
    assert_equal 'a', f[2]
    assert_equal 'a', f[-1]
    assert_equal ['a', 'b', 'c'], f.sort
    f.sort!
    assert_equal ['a', 'b', 'c'], f
  end

  def test_flatten
    assert_equal ['a', 'x.c', 'xyz.c', 'abc.c'].sort,
      ['a', FileList['*.c']].flatten.sort
  end

  def test_clone_and_dup
    a = FileList['a', 'b', 'c']
    c = a.clone
    d = a.dup
    a << 'd'
    assert_equal ['a', 'b', 'c', 'd'], a
    assert_equal ['a', 'b', 'c'], c
    assert_equal ['a', 'b', 'c'], d
  end

  def test_dup_and_clone_replicate_taint
    a = FileList['a', 'b', 'c']
    a.taint
    c = a.clone
    d = a.dup
    assert c.tainted?, "Clone should be tainted"
    assert d.tainted?, "Dup should be tainted"
  end

  def test_duped_items_will_thaw
    a = FileList['a', 'b', 'c']
    a.freeze
    d = a.dup
    d << 'more'
    assert_equal ['a', 'b', 'c', 'more'], d
  end

  def test_cloned_items_stay_frozen
    a = FileList['a', 'b', 'c']
    a.freeze
    c = a.clone
    assert_raises(TypeError, RuntimeError) do
      c << 'more'
    end
  end

  def test_array_comparisons
    fl = FileList['b', 'b']
    a = ['b', 'a']
    b = ['b', 'b']
    c = ['b', 'c']
    assert_equal( 1,  fl <=> a )
    assert_equal( 0,  fl <=> b )
    assert_equal( -1, fl <=> c )
    assert_equal( -1, a <=> fl )
    assert_equal( 0,  b <=> fl )
    assert_equal( 1,  c <=> fl )
  end

  def test_array_equality
    a = FileList['a', 'b']
    b = ['a', 'b']
    assert a == b
    assert b == a
#   assert a.eql?(b)
#    assert b.eql?(a)
    assert ! a.equal?(b)
    assert ! b.equal?(a)
  end

  def test_enumeration_methods
    a = FileList['a', 'b']
    b = a.collect { |it| it.upcase }
    assert_equal ['A', 'B'], b
    assert_equal FileList,  b.class

    b = a.map { |it| it.upcase }
    assert_equal ['A', 'B'], b
    assert_equal FileList,  b.class

    b = a.sort
    assert_equal ['a', 'b'], b
    assert_equal FileList,  b.class

    b = a.sort_by { |it| it }
    assert_equal ['a', 'b'], b
    assert_equal FileList,  b.class

    b = a.find_all { |it| it == 'b'}
    assert_equal ['b'], b
    assert_equal FileList,  b.class

    b = a.select { |it| it.size == 1 }
    assert_equal ['a', 'b'], b
    assert_equal FileList,  b.class

    b = a.reject { |it| it == 'b' }
    assert_equal ['a'], b
    assert_equal FileList,  b.class

    b = a.grep(/./)
    assert_equal ['a', 'b'], b
    assert_equal FileList,  b.class

    b = a.partition { |it| it == 'b' }
    assert_equal [['b'], ['a']], b
    assert_equal Array, b.class
    assert_equal FileList,  b[0].class
    assert_equal FileList,  b[1].class

    b = a.zip(['x', 'y']).to_a
    assert_equal [['a', 'x'], ['b', 'y']], b
    assert_equal Array, b.class
    assert_equal Array, b[0].class
    assert_equal Array, b[1].class
  end

  def test_array_operators
    a = ['a', 'b']
    b = ['c', 'd']
    f = FileList['x', 'y']
    g = FileList['w', 'z']

    r = f + g
    assert_equal ['x', 'y', 'w', 'z'], r
    assert_equal FileList, r.class

    r = a + g
    assert_equal ['a', 'b', 'w', 'z'], r
    assert_equal Array, r.class

    r = f + b
    assert_equal ['x', 'y', 'c', 'd'], r
    assert_equal FileList, r.class

    r = FileList['w', 'x', 'y', 'z'] - f
    assert_equal ['w', 'z'], r
    assert_equal FileList, r.class

    r = FileList['w', 'x', 'y', 'z'] & f
    assert_equal ['x', 'y'], r
    assert_equal FileList, r.class

    r = f * 2
    assert_equal ['x', 'y', 'x', 'y'], r
    assert_equal FileList, r.class

    r = f * ','
    assert_equal 'x,y', r
    assert_equal String, r.class

    r = f | ['a', 'x']
    assert_equal ['a', 'x', 'y'].sort, r.sort
    assert_equal FileList, r.class
  end

  def test_other_array_returning_methods
    f = FileList['a', nil, 'b']
    r = f.compact
    assert_equal ['a', 'b'], r
    assert_equal FileList, r.class

    f = FileList['a', 'b']
    r = f.concat(['x', 'y'])
    assert_equal ['a', 'b', 'x', 'y'], r
    assert_equal FileList, r.class

    f = FileList['a', ['b', 'c'], FileList['d', 'e']]
    r = f.flatten
    assert_equal ['a', 'b', 'c', 'd', 'e'], r
    assert_equal FileList, r.class

    f = FileList['a', 'b', 'a']
    r = f.uniq
    assert_equal ['a', 'b'], r
    assert_equal FileList, r.class

    f = FileList['a', 'b', 'c', 'd']
    r = f.values_at(1,3)
    assert_equal ['b', 'd'], r
    assert_equal FileList, r.class
  end

  def test_file_utils_can_use_filelists
    cfiles = FileList['*.c']

    cp cfiles, @cdir, :verbose => false

    assert File.exist?(File.join(@cdir, 'abc.c'))
    assert File.exist?(File.join(@cdir, 'xyz.c'))
    assert File.exist?(File.join(@cdir, 'x.c'))
  end

end

