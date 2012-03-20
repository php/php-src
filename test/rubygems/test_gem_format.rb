require 'rubygems/package/tar_test_case'
require 'rubygems/simple_gem'
require 'rubygems/format'

class TestGemFormat < Gem::Package::TarTestCase

  def setup
    super

    @simple_gem = SIMPLE_GEM
  end

  # HACK this test should do less
  def test_class_from_file_by_path
    util_make_gems

    gems = Dir[File.join(@gemhome, "cache", "*.gem")]

    names = [@a1, @a2, @a3a, @a_evil9, @b2, @c1_2, @pl1].map do |spec|
      spec.original_name
    end

    gems_n_names = gems.sort.zip names

    gems_n_names.each do |gemfile, name|
      spec = Gem::Format.from_file_by_path(gemfile).spec

      assert_equal name, spec.original_name
    end
  end

  def test_class_from_file_by_path_corrupt
    Tempfile.open 'corrupt' do |io|
      data = Gem.gzip 'a' * 10
      io.write tar_file_header('metadata.gz', "\000x", 0644, data.length)
      io.write data
      io.rewind

      e = assert_raises Gem::Package::FormatError do
        Gem::Format.from_file_by_path io.path
      end

      sub_message = 'Gem::Package::TarInvalidError: tar is corrupt, name contains null byte'
      assert_equal "corrupt gem (#{sub_message}) in #{io.path}", e.message
      assert_equal io.path, e.path
    end
  end

  def test_class_from_file_by_path_empty
    util_make_gems

    empty_gem = File.join @tempdir, 'empty.gem'
    FileUtils.touch empty_gem

    assert_nil Gem::Format.from_file_by_path(empty_gem)
  end

  def test_class_from_file_by_path_nonexistent
    assert_raises Gem::Exception do
      Gem::Format.from_file_by_path '/nonexistent'
    end
  end

  def test_class_from_io_garbled
    e = assert_raises Gem::Package::FormatError do
      # subtly bogus input
      Gem::Format.from_io(StringIO.new(@simple_gem.upcase))
    end

    assert_equal 'no metadata found', e.message

    e = assert_raises Gem::Package::FormatError do
      # Totally bogus input
      Gem::Format.from_io(StringIO.new(@simple_gem.reverse))
    end

    assert_equal 'no metadata found', e.message

    e = assert_raises Gem::Package::FormatError do
      # This was intentionally screws up YAML parsing.
      Gem::Format.from_io(StringIO.new(@simple_gem.gsub(/:/, "boom")))
    end

    assert_equal 'no metadata found', e.message
  end

end

