require 'rubygems/test_case'
require 'rubygems/commands/outdated_command'

class TestGemCommandsOutdatedCommand < Gem::TestCase

  def setup
    super

    @cmd = Gem::Commands::OutdatedCommand.new
  end

  def test_initialize
    assert @cmd.handles?(%W[--platform #{Gem::Platform.local}])
  end

  def test_execute
    remote_10 = quick_spec 'foo', '1.0'
    remote_20 = quick_spec 'foo', '2.0'

    Gem::RemoteFetcher.fetcher = @fetcher = Gem::FakeFetcher.new

    util_clear_gems
    util_setup_spec_fetcher remote_10, remote_20

    quick_gem 'foo', '0.1'
    quick_gem 'foo', '0.2'

    Gem::Specification.reset

    use_ui @ui do
      @cmd.execute
    end

    assert_equal "foo (0.2 < 2.0)\n", @ui.output
    assert_equal "", @ui.error
  end
end

