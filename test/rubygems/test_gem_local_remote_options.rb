require 'rubygems/test_case'
require 'rubygems/local_remote_options'
require 'rubygems/command'

class TestGemLocalRemoteOptions < Gem::TestCase

  def setup
    super

    @cmd = Gem::Command.new 'dummy', 'dummy'
    @cmd.extend Gem::LocalRemoteOptions
  end

  def test_add_local_remote_options
    @cmd.add_local_remote_options

    args = %w[-l -r -b -B 10 --source http://gems.example.com -p --update-sources]
    assert @cmd.handles?(args)
  end

  def test_both_eh
    assert_equal false, @cmd.both?

    @cmd.options[:domain] = :local

    assert_equal false, @cmd.both?

    @cmd.options[:domain] = :both

    assert_equal true, @cmd.both?
  end

  def test_clear_sources_option
    @cmd.add_local_remote_options

    s = URI.parse "http://only-gems.example.com/"

    @cmd.handle_options %W[--clear-sources --source #{s}]
    assert_equal [s.to_s], Gem.sources
  end

  def test_clear_sources_option_idiot_proof
    @cmd.add_local_remote_options
    @cmd.handle_options %W[--clear-sources]
    assert_equal Gem.default_sources, Gem.sources
  end

  def test_local_eh
    assert_equal false, @cmd.local?

    @cmd.options[:domain] = :local

    assert_equal true, @cmd.local?

    @cmd.options[:domain] = :both

    assert_equal true, @cmd.local?
  end

  def test_remote_eh
    assert_equal false, @cmd.remote?

    @cmd.options[:domain] = :remote

    assert_equal true, @cmd.remote?

    @cmd.options[:domain] = :both

    assert_equal true, @cmd.remote?
  end

  def test_source_option
    @cmd.add_source_option

    s1 = URI.parse 'http://more-gems.example.com/'
    s2 = URI.parse 'http://even-more-gems.example.com/'
    s3 = URI.parse 'http://other-gems.example.com/some_subdir'
    s4 = URI.parse 'http://more-gems.example.com/' # Intentional duplicate

    original_sources = Gem.sources.dup
    @cmd.handle_options %W[--source #{s1} --source #{s2} --source #{s3} --source #{s4}]

    assert_equal [original_sources, s1.to_s, s2.to_s, "#{s3}/"].flatten,
      Gem.sources
  end

  def test_update_sources_option
    @cmd.add_update_sources_option

    Gem.configuration.update_sources = false

    @cmd.handle_options %W[--update-sources]

    assert_equal true, Gem.configuration.update_sources

    @cmd.handle_options %W[--no-update-sources]

    assert_equal false, Gem.configuration.update_sources
  end

  def test_source_option_bad
    @cmd.add_source_option

    s1 = 'htp://more-gems.example.com'

    assert_raises OptionParser::InvalidArgument do
      @cmd.handle_options %W[--source #{s1}]
    end

    assert_equal [@gem_repo], Gem.sources
  end

end

