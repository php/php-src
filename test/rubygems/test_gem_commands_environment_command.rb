require 'rubygems/test_case'
require 'rubygems/commands/environment_command'

class TestGemCommandsEnvironmentCommand < Gem::TestCase

  def setup
    super

    @cmd = Gem::Commands::EnvironmentCommand.new
  end

  def test_execute
    orig_sources = Gem.sources.dup
    Gem.sources.replace %w[http://gems.example.com]
    Gem.configuration['gemcutter_key'] = 'blah'

    @cmd.send :handle_options, %w[]

    use_ui @ui do
      @cmd.execute
    end

    assert_match %r|RUBYGEMS VERSION: (\d\.)+\d|, @ui.output
    assert_match %r|RUBY VERSION: \d\.\d\.\d \(.*\) \[.*\]|, @ui.output
    assert_match %r|INSTALLATION DIRECTORY: #{Regexp.escape @gemhome}|,
                 @ui.output
    assert_match %r|RUBYGEMS PREFIX: |, @ui.output
    assert_match %r|RUBY EXECUTABLE:.*#{Gem::ConfigMap[:ruby_install_name]}|,
                 @ui.output
    assert_match %r|EXECUTABLE DIRECTORY:|, @ui.output
    assert_match %r|RUBYGEMS PLATFORMS:|, @ui.output
    assert_match %r|- #{Gem::Platform.local}|, @ui.output
    assert_match %r|GEM PATHS:|, @ui.output
    assert_match %r|- #{Regexp.escape @gemhome}|, @ui.output
    assert_match %r|GEM CONFIGURATION:|, @ui.output
    assert_match %r|"gemcutter_key" => "\*\*\*\*"|, @ui.output
    assert_match %r|:verbose => |, @ui.output
    assert_match %r|REMOTE SOURCES:|, @ui.output
    assert_equal '', @ui.error

  ensure
    Gem.sources.replace orig_sources
  end

  def test_execute_gemdir
    @cmd.send :handle_options, %w[gemdir]

    use_ui @ui do
      @cmd.execute
    end

    assert_equal "#{@gemhome}\n", @ui.output
    assert_equal '', @ui.error
  end

  def test_execute_gempath
    @cmd.send :handle_options, %w[gempath]

    use_ui @ui do
      @cmd.execute
    end

    assert_equal "#{@gemhome}\n", @ui.output
    assert_equal '', @ui.error
  end

  def test_execute_gempath_multiple
    Gem.clear_paths
    path = [@gemhome, "#{@gemhome}2"].join File::PATH_SEPARATOR
    ENV['GEM_PATH'] = path

    @cmd.send :handle_options, %w[gempath]

    use_ui @ui do
      @cmd.execute
    end

    assert_equal "#{Gem.path.join File::PATH_SEPARATOR}\n", @ui.output
    assert_equal '', @ui.error
  end

  def test_execute_packageversion
    @cmd.send :handle_options, %w[packageversion]

    use_ui @ui do
      @cmd.execute
    end

    assert_equal "#{Gem::RubyGemsPackageVersion}\n", @ui.output
    assert_equal '', @ui.error
  end

  def test_execute_remotesources
    orig_sources = Gem.sources.dup
    Gem.sources.replace %w[http://gems.example.com]

    @cmd.send :handle_options, %w[remotesources]

    use_ui @ui do
      @cmd.execute
    end

    assert_equal "http://gems.example.com\n", @ui.output
    assert_equal '', @ui.error

  ensure
    Gem.sources.replace orig_sources
  end

  def test_execute_unknown
    @cmd.send :handle_options, %w[unknown]

    assert_raises Gem::CommandLineError do
      use_ui @ui do
        @cmd.execute
      end
    end

    assert_equal '', @ui.output
    assert_equal '', @ui.error
  end

  def test_execute_version
    @cmd.send :handle_options, %w[version]

    use_ui @ui do
      @cmd.execute
    end

    assert_equal "#{Gem::VERSION}\n", @ui.output
    assert_equal '', @ui.error
  end

  def test_execute_platform
    @cmd.send :handle_options, %w[platform]

    use_ui @ui do
      @cmd.execute
    end

    assert_equal "#{Gem.platforms.join File::PATH_SEPARATOR}\n", @ui.output
    assert_equal '', @ui.error
  end
end
