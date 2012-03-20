require 'rubygems/test_case'
require 'rubygems'
require 'fileutils'

class TestGemPathSupport < Gem::TestCase
  def setup
    super

    ENV["GEM_HOME"] = @tempdir
    ENV["GEM_PATH"] = [@tempdir, "something"].join(File::PATH_SEPARATOR)
  end

  def test_initialize
    ps = Gem::PathSupport.new

    assert_equal ENV["GEM_HOME"], ps.home

    expected = util_path
    assert_equal expected, ps.path, "defaults to GEM_PATH"
  end

  def test_initialize_home
    ps = Gem::PathSupport.new "GEM_HOME" => "#{@tempdir}/foo"

    expected = File.join(@tempdir, "foo")
    assert_equal expected, ps.home

    assert_equal [expected, *util_path], ps.path
  end

  if defined?(File::ALT_SEPARATOR) and File::ALT_SEPARATOR
    def test_initialize_home_normalize
      alternate = @tempdir.gsub(File::SEPARATOR, File::ALT_SEPARATOR)
      ps = Gem::PathSupport.new "GEM_HOME" => alternate

      assert_equal @tempdir, ps.home, "normalize values"
    end
  end

  def test_initialize_path
    ps = Gem::PathSupport.new "GEM_PATH" => %W[#{@tempdir}/foo #{@tempdir}/bar]

    assert_equal ENV["GEM_HOME"], ps.home

    expected = [
                ENV["GEM_HOME"],
                File.join(@tempdir, 'foo'),
                File.join(@tempdir, 'bar'),
               ]

    assert_equal expected, ps.path
  end

  def test_initialize_home_path
    ps = Gem::PathSupport.new("GEM_HOME" => "#{@tempdir}/foo",
                              "GEM_PATH" => %W[#{@tempdir}/foo #{@tempdir}/bar])

    assert_equal File.join(@tempdir, "foo"), ps.home

    expected = [File.join(@tempdir, 'foo'), File.join(@tempdir, 'bar')]
    assert_equal expected, ps.path
  end

  def test_path_equals
    ps = Gem::PathSupport.new

    ps.send :path=, ['a', 'b']

    assert_equal [@tempdir, 'a', 'b'], ps.path
  end

  def test_path_equals_empty
    ps = Gem::PathSupport.new

    ps.send :path=, nil

    assert_equal [@tempdir, 'something'], ps.path
  end

  def test_path_equals_empty_no_GEM_PATH
    ENV.delete 'GEM_PATH'

    ps = Gem::PathSupport.new

    ps.send :path=, nil

    assert_equal [@tempdir, *Gem.default_path], ps.path
  end

  def util_path
    ENV["GEM_PATH"].split(File::PATH_SEPARATOR)
  end
end
