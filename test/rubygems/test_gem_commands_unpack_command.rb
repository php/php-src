require 'rubygems/test_case'
require 'rubygems/commands/unpack_command'

class TestGemCommandsUnpackCommand < Gem::TestCase

  def setup
    super

    Dir.chdir @tempdir do
      @cmd = Gem::Commands::UnpackCommand.new
    end
  end

  def test_find_in_cache
    util_make_gems

    assert_equal(
      @cmd.find_in_cache(File.basename @a1.cache_file),
      @a1.cache_file,
      'found a-1.gem in the cache'
    )
  end

  def test_get_path
    util_setup_fake_fetcher
    util_clear_gems
    util_setup_spec_fetcher @a1

    a1_data = nil

    open @a1.cache_file, 'rb' do |fp|
      a1_data = fp.read
    end

    Gem::RemoteFetcher.fetcher.data['http://gems.example.com/gems/a-1.gem'] =
      a1_data

    dep = Gem::Dependency.new(@a1.name, @a1.version)
    assert_equal(
      @cmd.get_path(dep),
      @a1.cache_file,
      'fetches a-1 and returns the cache path'
    )

    FileUtils.rm @a1.cache_file

    assert_equal(
      @cmd.get_path(dep),
      @a1.cache_file,
      'when removed from cache, refetches a-1'
    )
  end

  def test_execute
    util_make_gems

    @cmd.options[:args] = %w[a b]

    use_ui @ui do
      Dir.chdir @tempdir do
        @cmd.execute
      end
    end

    assert File.exist?(File.join(@tempdir, 'a-3.a')), 'a should be unpacked'
    assert File.exist?(File.join(@tempdir, 'b-2')),   'b should be unpacked'
  end

  def test_execute_gem_path
    util_setup_fake_fetcher
    util_setup_spec_fetcher

    Gem.clear_paths

    gemhome2 = File.join @tempdir, 'gemhome2'

    Gem.paths = { "GEM_PATH" => [gemhome2, @gemhome], "GEM_HOME" => gemhome2 }

    @cmd.options[:args] = %w[a]

    use_ui @ui do
      Dir.chdir @tempdir do
        @cmd.execute
      end
    end

    assert File.exist?(File.join(@tempdir, 'a-3.a'))
  end

  def test_execute_gem_path_missing
    util_setup_fake_fetcher
    util_setup_spec_fetcher

    Gem.clear_paths

    gemhome2 = File.join @tempdir, 'gemhome2'

    Gem.paths = { "GEM_PATH" => [gemhome2, @gemhome], "GEM_HOME" => gemhome2 }

    @cmd.options[:args] = %w[z]

    use_ui @ui do
      Dir.chdir @tempdir do
        @cmd.execute
      end
    end

    assert_equal '', @ui.output
  end

  def test_execute_remote
    util_setup_fake_fetcher
    util_setup_spec_fetcher @a1, @a2
    util_clear_gems

    a2_data = nil
    open @a2.cache_file, 'rb' do |fp|
      a2_data = fp.read
    end

    Gem::RemoteFetcher.fetcher.data['http://gems.example.com/gems/a-2.gem'] =
      a2_data

    Gem.configuration.verbose = :really
    @cmd.options[:args] = %w[a]

    use_ui @ui do
      Dir.chdir @tempdir do
        @cmd.execute
      end
    end

    assert File.exist?(File.join(@tempdir, 'a-2')), 'a should be unpacked'
  end

  def test_execute_spec
    util_make_gems

    @cmd.options[:args] = %w[a b]
    @cmd.options[:spec] = true

    use_ui @ui do
      Dir.chdir @tempdir do
        @cmd.execute
      end
    end

    assert File.exist?(File.join(@tempdir, 'a-3.a.gemspec'))
    assert File.exist?(File.join(@tempdir, 'b-2.gemspec'))
  end

  def test_execute_sudo
    skip 'Cannot perform this test on windows (chmod)' if win_platform?

    util_make_gems

    FileUtils.chmod 0555, @gemhome

    @cmd.options[:args] = %w[b]

    use_ui @ui do
      Dir.chdir @tempdir do
        @cmd.execute
      end
    end

    assert File.exist?(File.join(@tempdir, 'b-2')), 'b should be unpacked'
  ensure
    FileUtils.chmod 0755, @gemhome
  end

  def test_execute_with_target_option
    util_make_gems

    target = 'with_target'
    @cmd.options[:args] = %w[a]
    @cmd.options[:target] = target

    use_ui @ui do
      Dir.chdir @tempdir do
        @cmd.execute
      end
    end

    assert File.exist?(File.join(@tempdir, target, 'a-3.a'))
  end

  def test_execute_exact_match
    foo_spec = quick_spec 'foo'
    foo_bar_spec = quick_spec 'foo_bar'

    use_ui @ui do
      Dir.chdir @tempdir do
        Gem::Builder.new(foo_spec).build
        Gem::Builder.new(foo_bar_spec).build
      end
    end

    foo_path = File.join(@tempdir, "#{foo_spec.full_name}.gem")
    foo_bar_path = File.join(@tempdir, "#{foo_bar_spec.full_name}.gem")
    Gem::Installer.new(foo_path).install
    Gem::Installer.new(foo_bar_path).install

    @cmd.options[:args] = %w[foo]

    use_ui @ui do
      Dir.chdir @tempdir do
        @cmd.execute
      end
    end

    assert File.exist?(File.join(@tempdir, foo_spec.full_name))
  end

  def test_handle_options_metadata
    refute @cmd.options[:spec]

    @cmd.send :handle_options, %w[--spec a]

    assert @cmd.options[:spec]
  end

end

