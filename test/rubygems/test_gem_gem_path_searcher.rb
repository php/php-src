require 'rubygems/test_case'
require 'rubygems/gem_path_searcher'

class Gem::GemPathSearcher
  attr_accessor :gemspecs
  attr_accessor :lib_dirs
end

class TestGemGemPathSearcher < Gem::TestCase
  def setup
    super

    @foo1 = new_spec 'foo', '0.1', nil, "lib/foo.rb"
    @foo1.require_paths << 'lib2'
    path = File.join 'gems', @foo1.full_name, 'lib', 'foo.rb'
    write_file(path) { |fp| fp.puts "# #{path}" }

    @foo2 = new_spec 'foo', '0.2'
    @bar1 = new_spec 'bar', '0.1'
    @bar2 = new_spec 'bar', '0.2'
    @nrp  = new_spec 'nil_require_paths', '0.1' do |s|
      s.require_paths = nil
    end

    util_setup_fake_fetcher
    Gem::Specification.reset
    util_setup_spec_fetcher @foo1, @foo2, @bar1, @bar2

    @fetcher = Gem::FakeFetcher.new
    Gem::RemoteFetcher.fetcher = @fetcher

    @gps = Gem::Deprecate.skip_during { Gem::GemPathSearcher.new }
  end

  def test_find
    Gem::Deprecate.skip_during do
      assert_equal @foo1, @gps.find('foo')
    end
  end

  def test_find_all
    Gem::Deprecate.skip_during do
      assert_equal [@foo1], @gps.find_all('foo')
    end
  end

  def test_init_gemspecs
    Gem::Deprecate.skip_during do
      util_clear_gems
      util_setup_spec_fetcher @foo1, @foo2, @bar1, @bar2
      expected = [@bar2, @bar1, @foo2, @foo1].map(&:full_name)
      actual   = @gps.init_gemspecs.map(&:full_name)
      assert_equal expected, actual
    end
  end

  def test_lib_dirs_for
    Gem::Deprecate.skip_during do
      lib_dirs = @gps.lib_dirs_for(@foo1)
      expected = File.join @gemhome, 'gems', @foo1.full_name, '{lib,lib2}'

      assert_equal expected, lib_dirs
    end
  end

  def test_lib_dirs_for_nil_require_paths
    Gem::Deprecate.skip_during do
      assert_nil @gps.lib_dirs_for(@nrp)
    end
  end

  def test_matching_file_eh
    Gem::Deprecate.skip_during do
      refute @gps.matching_file?(@foo1, 'bar')
      assert @gps.matching_file?(@foo1, 'foo')
    end
  end

  def test_matching_files
    Gem::Deprecate.skip_during do
      assert_equal [], @gps.matching_files(@foo1, 'bar')

      expected = File.join @foo1.full_gem_path, 'lib', 'foo.rb'

      assert_equal [expected], @gps.matching_files(@foo1, 'foo')
    end
  end

  def test_matching_files_nil_require_paths
    Gem::Deprecate.skip_during do
      assert_empty @gps.matching_files(@nrp, 'foo')
    end
  end
end
