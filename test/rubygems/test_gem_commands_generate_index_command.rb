require 'rubygems/test_case'
require 'rubygems/indexer'
require 'rubygems/commands/generate_index_command'

class TestGemCommandsGenerateIndexCommand < Gem::TestCase

  def setup
    super

    @cmd = Gem::Commands::GenerateIndexCommand.new
    @cmd.options[:directory] = @gemhome
  end

  def test_execute
    use_ui @ui do
      @cmd.execute
    end

    marshal = File.join @gemhome, 'Marshal.4.8'
    marshal_z = File.join @gemhome, 'Marshal.4.8.Z'

    assert File.exist?(marshal), marshal
    assert File.exist?(marshal_z), marshal_z
  end

  def test_execute_rss_update
    @cmd.options[:update] = true
    @cmd.options[:rss_host] = 'example.com'
    @cmd.options[:rss_gems_host] = 'gems.example.com'

    use_ui @ui do
      assert_raises Gem::MockGemUi::TermError do
        @cmd.execute
      end
    end

    assert_equal "ERROR:  --update not compatible with RSS generation\n",
                 @ui.error
    assert_empty @ui.output
  end

  def test_handle_options_directory
    return if win_platform?
    refute_equal '/nonexistent', @cmd.options[:directory]

    @cmd.handle_options %w[--directory /nonexistent]

    assert_equal '/nonexistent', @cmd.options[:directory]
  end

  def test_handle_options_directory_windows
    return unless win_platform?

    refute_equal '/nonexistent', @cmd.options[:directory]

    @cmd.handle_options %w[--directory C:/nonexistent]

    assert_equal 'C:/nonexistent', @cmd.options[:directory]
  end

  def test_handle_options_invalid
    e = assert_raises OptionParser::InvalidOption do
      @cmd.handle_options %w[--no-modern --no-legacy]
    end

    assert_equal 'invalid option: --no-legacy no indicies will be built',
                 e.message

    @cmd = Gem::Commands::GenerateIndexCommand.new
    e = assert_raises OptionParser::InvalidOption do
      @cmd.handle_options %w[--no-legacy --no-modern]
    end

    assert_equal 'invalid option: --no-modern no indicies will be built',
                 e.message
  end

  def test_handle_options_legacy
    @cmd.handle_options %w[--legacy]

    assert @cmd.options[:build_legacy]
    assert @cmd.options[:build_modern], ':build_modern not set'
  end

  def test_handle_options_modern
    @cmd.handle_options %w[--modern]

    assert @cmd.options[:build_legacy]
    assert @cmd.options[:build_modern], ':build_modern not set'
  end

  def test_handle_options_no_legacy
    @cmd.handle_options %w[--no-legacy]

    refute @cmd.options[:build_legacy]
    assert @cmd.options[:build_modern]
  end

  def test_handle_options_no_modern
    @cmd.handle_options %w[--no-modern]

    assert @cmd.options[:build_legacy]
    refute @cmd.options[:build_modern]
  end

  def test_handle_options_rss_gems_host
    @cmd.handle_options %w[--rss-gems-host gems.example.com]

    assert_equal 'gems.example.com', @cmd.options[:rss_gems_host]
  end

  def test_handle_options_rss_host
    @cmd.handle_options %w[--rss-host example.com]

    assert_equal 'example.com', @cmd.options[:rss_host]
  end

  def test_handle_options_rss_title
    @cmd.handle_options %w[--rss-title Example\ Gems]

    assert_equal 'Example Gems', @cmd.options[:rss_title]
  end

  def test_handle_options_update
    @cmd.handle_options %w[--update]

    assert @cmd.options[:update]
  end

end if ''.respond_to? :to_xs

