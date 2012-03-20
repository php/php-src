require 'rubygems/test_case'
require 'rubygems/package'
require 'rubygems/security'
require 'rubygems/commands/fetch_command'

class TestGemCommandsFetchCommand < Gem::TestCase

  def setup
    super

    @cmd = Gem::Commands::FetchCommand.new
  end

  def test_execute
    util_setup_fake_fetcher
    util_setup_spec_fetcher @a2

    @fetcher.data["#{@gem_repo}gems/#{@a2.file_name}"] =
      File.read(@a2.cache_file)

    @cmd.options[:args] = [@a2.name]

    use_ui @ui do
      Dir.chdir @tempdir do
        @cmd.execute
      end
    end

    assert File.exist?(File.join(@tempdir, @a2.file_name)),
           "#{@a2.full_name} not fetched"
  end

  def test_execute_prerelease
    util_setup_fake_fetcher true
    util_clear_gems
    util_setup_spec_fetcher @a2, @a2_pre

    @fetcher.data["#{@gem_repo}gems/#{@a2.file_name}"] =
      File.read(@a2.cache_file)
    @fetcher.data["#{@gem_repo}gems/#{@a2_pre.file_name}"] =
      File.read(@a2_pre.cache_file)

    @cmd.options[:args] = [@a2.name]
    @cmd.options[:prerelease] = true

    use_ui @ui do
      Dir.chdir @tempdir do
        @cmd.execute
      end
    end

    assert File.exist?(File.join(@tempdir, @a2_pre.file_name)),
           "#{@a2_pre.full_name} not fetched"
  end

  def test_execute_version
    util_setup_fake_fetcher
    util_setup_spec_fetcher @a1, @a2

    @fetcher.data["#{@gem_repo}gems/#{@a1.file_name}"] =
      File.read(@a1.cache_file)

    @cmd.options[:args] = [@a2.name]
    @cmd.options[:version] = Gem::Requirement.new '1'

    use_ui @ui do
      Dir.chdir @tempdir do
        @cmd.execute
      end
    end

    assert File.exist?(File.join(@tempdir, @a1.file_name)),
           "#{@a1.full_name} not fetched"
  end

end

